/*
 * This file is part of the OregonCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "DatabaseEnv.h"
#include "Config/Config.h"

#include "Common.h"
#include "../../game/UpdateFields.h"

#include "Utilities/Util.h"
#include "Platform/Define.h"
#include "Platform/CompilerDefs.h"
#include "Threading.h"
#include "Database/SqlDelayThread.h"
#include "Database/SqlOperations.h"
#include "Timer.h"

#include <ctime>
#include <iostream>
#include <fstream>
#if PLATFORM == PLATFORM_UNIX
#include <sys/file.h>
#endif

size_t Database::db_count = 0;

Database::Database() : mMysql(NULL), m_connected(false)
{
    // before first connection
    if (db_count++ == 0)
    {
        // Mysql Library Init
        mysql_library_init(-1, NULL, NULL);

        if (!mysql_thread_safe())
            sLog.outFatal("FATAL ERROR: Used MySQL library isn't thread-safe.");
    }
}

Database::~Database()
{
    if (m_delayThread)
        HaltDelayThread();

    if (mMysql)
        mysql_close(mMysql);

    // Free Mysql library pointers for last ~DB
    if (--db_count == 0)
        mysql_library_end();
}

bool Database::Initialize(const char* infoString)
{
    // Enable logging of SQL commands (usally only GM commands)
    // (See method: PExecuteLog)
    m_logSQL = sConfig.GetBoolDefault("LogSQL", false);
    m_logsDir = sConfig.GetStringDefault("LogsDir", "");
    if (!m_logsDir.empty())
    {
        if ((m_logsDir.at(m_logsDir.length() - 1) != '/') && (m_logsDir.at(m_logsDir.length() - 1) != '\\'))
            m_logsDir.append("/");
    }

    tranThread = NULL;
    MYSQL* mysqlInit;
    mysqlInit = mysql_init(NULL);
    if (!mysqlInit)
    {
        sLog.outError("Could not initialize Mysql connection");
        return false;
    }

    InitDelayThread();

    Tokens tokens = StrSplit(infoString, ";");

    Tokens::iterator iter;

    std::string host, port_or_socket, user, password, database;
    int port;
    char const* unix_socket;

    iter = tokens.begin();

    if (iter != tokens.end())
        host = *iter++;
    if (iter != tokens.end())
        port_or_socket = *iter++;
    if (iter != tokens.end())
        user = *iter++;
    if (iter != tokens.end())
        password = *iter++;
    if (iter != tokens.end())
        database = *iter++;

    mysql_options(mysqlInit, MYSQL_SET_CHARSET_NAME, "utf8");
    #ifdef _WIN32
    if (host == ".")                                         // named pipe use option (Windows)
    {
        unsigned int opt = MYSQL_PROTOCOL_PIPE;
        mysql_options(mysqlInit, MYSQL_OPT_PROTOCOL, (char const*)&opt);
        port = 0;
        unix_socket = 0;
    }
    else                                                    // generic case
    {
        port = atoi(port_or_socket.c_str());
        unix_socket = 0;
    }
    #else
    if (host == ".")                                         // socket use option (Unix/Linux)
    {
        unsigned int opt = MYSQL_PROTOCOL_SOCKET;
        mysql_options(mysqlInit, MYSQL_OPT_PROTOCOL, (char const*)&opt);
        host = "localhost";
        port = 0;
        unix_socket = port_or_socket.c_str();
    }
    else                                                    // generic case
    {
        port = atoi(port_or_socket.c_str());
        unix_socket = 0;
    }
    #endif

    mMysql = mysql_real_connect(mysqlInit, host.c_str(), user.c_str(),
                                password.c_str(), database.c_str(), port, unix_socket, 0);

    if (mMysql)
    {
        sLog.outDetail("Connected to MySQL database at %s", host.c_str());
        sLog.outDebug("MySQL client library: %s", mysql_get_client_info());
        sLog.outDebug("MySQL server ver: %s ", mysql_get_server_info( mMysql));

        if (!mysql_autocommit(mMysql, 1))
            sLog.outDebug("AUTOCOMMIT SUCCESSFULLY SET TO 1");
        else
            sLog.outDebug("AUTOCOMMIT NOT SET TO 1");

        // set connection properties to UTF8 to properly handle locales for different
        // server configs - core sends data in UTF8, so MySQL must expect UTF8 too
        // mysql_set_character_set is just like SET NAMES, but also sets encoding in client library
        // which enforces mysql_real_escape_string to be safe
        mysql_set_character_set(mMysql, "utf8");
        Execute("SET CHARACTER SET `utf8`");

        #if MYSQL_VERSION_ID >= 50003
        my_bool my_true = (my_bool)1;
        if (mysql_options(mMysql, MYSQL_OPT_RECONNECT, &my_true))
            sLog.outDebug("Failed to turn on MYSQL_OPT_RECONNECT.");
        else
            sLog.outDebug("Successfully turned on MYSQL_OPT_RECONNECT.");
        #else
#warning "Your mySQL client lib version does not support reconnecting after a timeout.\nIf this causes you any trouble we advice you to upgrade your mySQL client libs to at least mySQL 5.0.13 to resolve this problem."
        #endif
        
        m_connected = true;
        return true;
    }
    else
    {
        sLog.outError("Could not connect to MySQL database at %s: %s", host.c_str(), mysql_error(mysqlInit));
        mysql_close(mysqlInit);
        return false;
    }
}

void Database::ThreadStart()
{
    mysql_thread_init();
}

void Database::ThreadEnd()
{
    mysql_thread_end();
}

void Database::escape_string(std::string& str)
{
    if (str.empty())
        return;

    char* buf = new char[str.size() * 2 + 1];
    escape_string(buf, str.c_str(), str.size());
    str = buf;
    delete[] buf;
}

unsigned long Database::escape_string(char* to, const char* from, unsigned long length)
{
    if (!mMysql || !to || !from || !length)
        return 0;

    return (mysql_real_escape_string(mMysql, to, from, length));
}


bool Database::PExecuteLog(const char* format, ...)
{
    if (!format)
        return false;

    va_list ap;
    char szQuery [MAX_QUERY_LEN];
    va_start(ap, format);
    int res = vsnprintf(szQuery, MAX_QUERY_LEN, format, ap);
    va_end(ap);

    if (res == -1)
    {
        sLog.outError("SQL Query truncated (and not execute) for format: %s", format);
        return false;
    }

    sLog.outSQL("%s", szQuery);
    return Execute(szQuery);
}

void Database::SetResultQueue(SqlResultQueue* queue)
{
    m_queryQueues[ACE_Based::Thread::current()] = queue;
}

bool Database::_Query(const char* sql, MYSQL_RES** pResult, MYSQL_FIELD** pFields, uint64* pRowCount, uint32* pFieldCount)
{
    if (!mMysql)
        return 0;

    {
        // guarded block for thread-safe mySQL request
        ACE_Guard<ACE_Thread_Mutex> query_connection_guard(mMutex);
        #ifdef OREGON_DEBUG
        uint32 _s = getMSTime();
        #endif
        if (mysql_query(mMysql, sql))
        {
            sLog.outErrorDb("SQL: %s", sql);
            sLog.outErrorDb("query ERROR: %s", mysql_error(mMysql));
            return false;
        }
        else
        {
            #ifdef OREGON_DEBUG
            // prevent recursive death
            unsigned long oldMask = sLog.GetDBLogMask();
            sLog.SetDBLogMask(oldMask & ~(1 << LOG_TYPE_DEBUG));
            sLog.outDebug("[%u ms] SQL: %s", getMSTimeDiff(_s, getMSTime()), sql);
            sLog.SetDBLogMask(oldMask);
            #endif
        }

        *pResult = mysql_store_result(mMysql);
        *pRowCount = mysql_affected_rows(mMysql);
        *pFieldCount = mysql_field_count(mMysql);
    }

    if (!*pResult )
        return false;

    if (!*pRowCount)
    {
        mysql_free_result(*pResult);
        return false;
    }

    *pFields = mysql_fetch_fields(*pResult);
    return true;
}

QueryResult_AutoPtr Database::Query(const char* sql)
{
    MYSQL_RES* result = NULL;
    MYSQL_FIELD* fields = NULL;
    uint64 rowCount = 0;
    uint32 fieldCount = 0;

    if (!_Query(sql, &result, &fields, &rowCount, &fieldCount))
        return QueryResult_AutoPtr(NULL);

    QueryResult* queryResult = new QueryResult(result, fields, rowCount, fieldCount);

    queryResult->NextRow();

    return QueryResult_AutoPtr(queryResult);
}

QueryResult_AutoPtr Database::PQuery(const char* format, ...)
{
    if (!format)
        return QueryResult_AutoPtr(NULL);

    va_list ap;
    char szQuery [MAX_QUERY_LEN];
    va_start(ap, format);
    int res = vsnprintf(szQuery, MAX_QUERY_LEN, format, ap);
    va_end(ap);

    if (res == -1)
    {
        sLog.outError("SQL Query truncated (and not execute) for format: %s", format);
        return QueryResult_AutoPtr(NULL);
    }

    return Query(szQuery);
}

bool Database::Execute(const char* sql)
{
    if (!mMysql)
        return false;

    // don't use queued execution if it has not been initialized
    if (!m_threadBody)
        return DirectExecute(sql);

    nMutex.acquire();
    tranThread = ACE_Based::Thread::current();              // owner of this transaction
    TransactionQueues::iterator i = m_tranQueues.find(tranThread);
    if (i != m_tranQueues.end() && i->second != NULL)
        i->second->DelayExecute(sql);                       // Statement for transaction
    else
        m_threadBody->Delay(new SqlStatement(sql));         // Simple sql statement

    nMutex.release();
    return true;
}

bool Database::PExecute(const char* format, ...)
{
    if (!format)
        return false;

    va_list ap;
    char szQuery [MAX_QUERY_LEN];
    va_start(ap, format);
    int res = vsnprintf(szQuery, MAX_QUERY_LEN, format, ap);
    va_end(ap);

    if (res == -1)
    {
        sLog.outError("SQL Query truncated (and not execute) for format: %s", format);
        return false;
    }

    return Execute(szQuery);
}

bool Database::DirectExecute(bool lock, const char* sql)
{
    if (!mMysql)
        return false;

    if (lock)
        mMutex.acquire();

    #ifdef OREGON_DEBUG
    uint32 _s = getMSTime();
    #endif
    if (mysql_query(mMysql, sql))
    {
        sLog.outErrorDb("SQL: %s", sql);
        sLog.outErrorDb("SQL ERROR: %s", mysql_error(mMysql));
        if (lock)
            mMutex.release();
        return false;
    }
    else
    {
        #ifdef OREGON_DEBUG
        // prevent recursive death
        unsigned long oldMask = sLog.GetDBLogMask();
        sLog.SetDBLogMask(oldMask & ~(1 << LOG_TYPE_DEBUG));
        sLog.outDebug("[%u ms] SQL: %s", getMSTimeDiff(_s, getMSTime()), sql);
        sLog.SetDBLogMask(oldMask);
        #endif
    }

    if (lock)
        mMutex.release();

    return true;
}

bool Database::DirectPExecute(const char* format, ...)
{
    if (!format)
        return false;

    va_list ap;
    char szQuery [MAX_QUERY_LEN];
    va_start(ap, format);
    int res = vsnprintf(szQuery, MAX_QUERY_LEN, format, ap);
    va_end(ap);

    if (res == -1)
    {
        sLog.outError("SQL Query truncated (and not execute) for format: %s", format);
        return false;
    }

    return DirectExecute(szQuery);
}

bool Database::_TransactionCmd(const char* sql)
{
    if (mysql_query(mMysql, sql))
    {
        sLog.outError("SQL: %s", sql);
        sLog.outError("SQL ERROR: %s", mysql_error(mMysql));
        return false;
    }
    #if OREGON_DEBUG
    else
        DEBUG_LOG("SQL: %s", sql);
    #endif

    return true;
}

bool Database::BeginTransaction()
{
    if (!mMysql)
        return false;

    nMutex.acquire();
    tranThread = ACE_Based::Thread::current();              // owner of this transaction
    TransactionQueues::iterator i = m_tranQueues.find(tranThread);
    if (i != m_tranQueues.end() && i->second != NULL)
        // If for thread exists queue and also contains transaction
        // delete that transaction (not allow trans in trans)
        delete i->second;

    m_tranQueues[tranThread] = new SqlTransaction();
    nMutex.release();
    return true;
}

bool Database::CommitTransaction()
{
    if (!mMysql)
        return false;

    bool _res = false;

    nMutex.acquire();
    tranThread = ACE_Based::Thread::current();
    TransactionQueues::iterator i = m_tranQueues.find(tranThread);
    if (i != m_tranQueues.end() && i->second != NULL)
    {
        m_threadBody->Delay(i->second);
        m_tranQueues.erase(i);
        _res = true;
    }
    nMutex.release();
    return _res;
}

bool Database::RollbackTransaction()
{
    if (!mMysql)
        return false;

    nMutex.acquire();
    tranThread = ACE_Based::Thread::current();
    TransactionQueues::iterator i = m_tranQueues.find(tranThread);
    if (i != m_tranQueues.end() && i->second != NULL)
    {
        delete i->second;
        i->second = NULL;
        m_tranQueues.erase(i);
    }
    nMutex.release();
    return true;
}

/**
  * @brief Atomically executed SqlTransaction.
  * Don't call this directly, use \ref BeginTransaction and \ref CommitTransaction instead.
  */
bool Database::ExecuteTransaction(SqlTransaction* transaction)
{
    SqlTransaction::QueuedItem item;

    ACE_Guard<ACE_Thread_Mutex> connection_guard(mMutex);
    ACE_Guard<ACE_Thread_Mutex> transaction_guard(transaction->mutex);

    if (transaction->queue.empty())
        return true;

    if (mysql_autocommit(mMysql, 0))
        return false;

    if (mysql_real_query(mMysql, "START TRANSACTION", sizeof("START TRANSACTION")-1))
        return false;
    
    while (!transaction->queue.empty())
    {
        item = transaction->queue.front();

        bool ok = DirectExecute(false, item.sql);
        if (!ok)
        {
            transaction->queue.pop();
            mysql_rollback(mMysql);
            mysql_autocommit(mMysql, 1);
            return false;
        }

        free (item.sql);
        transaction->queue.pop();
    }

    if (mysql_commit(mMysql))
        return false;

    mysql_autocommit(mMysql, 1);
    return true;
}

void Database::InitDelayThread()
{
    assert(!m_delayThread);

    //New delay thread for delay execute
    m_threadBody = new SqlDelayThread(this);              // will deleted at m_delayThread delete
    m_delayThread = new ACE_Based::Thread(m_threadBody);
}

void Database::HaltDelayThread()
{
    if (!m_threadBody || !m_delayThread)
        return;

    m_threadBody->Stop();                                   //Stop event
    m_delayThread->wait();                                  //Wait for flush to DB
    delete m_delayThread;                                   //This also deletes m_threadBody
    m_delayThread = NULL;
    m_threadBody = NULL;
}

bool Database::ExecuteFile(const char* file)
{
    if (!mMysql)
        return false;

    if (mysql_set_server_option(mMysql, MYSQL_OPTION_MULTI_STATEMENTS_ON))
    {
        sLog.outErrorDb("Cannot turn multi-statements on: %s", mysql_error(mMysql));
        return false;
    }

    mysql_autocommit(mMysql, 0);
    if (mysql_real_query(mMysql, "START TRANSACTION", sizeof("START TRANSACTION")-1))
    {
        sLog.outErrorDb("Couldn't start transaction for db update file: %s", file);
        return false;
    }

    bool in_transaction = true;
    bool success = false;

    if (FILE* fp = ACE_OS::fopen(file, "rb"))
    {
        #if PLATFORM == PLATFORM_UNIX
        flock(fileno(fp), LOCK_SH);
        #endif
        //------
        
        struct stat info;
        fstat(fileno(fp), &info);

        // if less than 1MB allocate on stack, else on heap
        char* contents = (info.st_size > 1024*1024) ? new char[info.st_size] : (char*) alloca(info.st_size);

        if (ACE_OS::fread(contents, info.st_size, 1, fp) == 1)
        {
            if (mysql_real_query(mMysql, contents, info.st_size))
            {
                sLog.outErrorDb("Cannot execute file %s, size: %lu: %s", file, info.st_size, mysql_error(mMysql));
            }
            else
            {
                do
                {
                    if (mysql_field_count(mMysql))
                        if (MYSQL_RES* result = mysql_use_result(mMysql))
                            mysql_free_result(result);
                }
                while (0 == mysql_next_result(mMysql));

                // check whether the last mysql_next_result ended with an error
                if (*mysql_error(mMysql))
                {
                    success = false;
                    sLog.outErrorDb("Cannot execute file %s, size: %lu: %s", file, info.st_size, mysql_error(mMysql));
                    if (mysql_rollback(mMysql))
                        sLog.outErrorDb("ExecuteFile(): Rollback ended with an error!");
                    else
                        in_transaction = false;
                }
                else
                {
                    if (mysql_commit(mMysql))
                        sLog.outErrorDb("mysql_commit() failed. Update %s will not be applied!", file);
                    else
                        in_transaction = false;
                success = true;
            }
        }
        }
        else
        {
           sLog.outErrorDb("Couldn't read file %s, size: %lu", file, info.st_size);
           return false;
        }

        // if allocated on heap, free memory
        if (info.st_size > 1024*1024)
            delete [] contents;

        //------
        #if PLATFORM == PLATFORM_UNIX
        flock(fileno(fp), LOCK_UN);
        #endif
        ACE_OS::fclose(fp);
    }

    mysql_set_server_option(mMysql, MYSQL_OPTION_MULTI_STATEMENTS_OFF);
    mysql_autocommit(mMysql, 1);
    if (in_transaction)
        mysql_rollback(mMysql);
    return success;
}
