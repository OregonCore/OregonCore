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
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "DatabaseEnv.h"
#include "Config/Config.h"

#include "Common.h"
#include "../../game/UpdateFields.h"

#include "Util.h"
#include "Platform/Define.h"
#include "Threading.h"
#include "Database/SqlDelayThread.h"
#include "Database/SqlOperations.h"
#include "Timer.h"

#include <ctime>
#include <iostream>
#include <fstream>
#ifdef PLATFORM == PLATFORM_UNIX
#include <sys/file.h>
#endif

static const my_bool my_true = 1;

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

    for (PreparedStatementsMap::iterator it = m_preparedStatements.begin(); it != m_preparedStatements.end(); ++it)
    {
        mysql_stmt_close(it->second->stmt);
        delete it->second;
    }

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

bool Database::PreparedExecuteLog(const char* sql, const char* format, ...)
{
    if (format)
    {
        PreparedValues values(strlen(format));

        va_list ap;
        va_start(ap, format);
        _ConvertValistToPreparedValues(ap, values, format);
        va_end(ap);

        return PreparedExecuteLog(sql, values);
    }
 
    PreparedValues values(0);
    return PreparedExecuteLog(sql, values);
}

bool Database::PreparedExecuteLog(const char* sql, PreparedValues& values)
{
    std::string query(sql);
    std::stringstream ss;
    size_t pos = 0, i = 0;

    while ((pos = query.find("?", pos)) != std::string::npos)
    {
        switch (values[i].type)
        {
            case ARG_TYPE_STRING:
            case ARG_TYPE_STRING_ALT:
                {
                    std::string safeStr(values[i].data.string);
                    escape_string(safeStr);
                    safeStr += '\'';
                    safeStr.insert(0, "'");
                    query.replace(pos, 1, safeStr);
                }
                break;
            case ARG_TYPE_BINARY:
            case ARG_TYPE_BINARY_ALT:
                ss << "0x" << std::hex;
                for (size_t j = 0; j < values[i].data.length; ++j)
                    ss << *reinterpret_cast<const char*>(values[i].data.binary);
                ss.clear();
                break;
            case ARG_TYPE_FLOAT:
                ss << values[i].data.float_;
                query.replace(pos, 1, ss.str());
                ss.clear();
                break;
            case ARG_TYPE_DOUBLE:
                ss << values[i].data.double_;
                query.replace(pos, 1, ss.str());
                ss.clear();
                break;
            case ARG_TYPE_NUMBER:
            case ARG_TYPE_NUMBER_ALT:
                ss << values[i].data.number;
                query.replace(pos, 1, ss.str());
                ss.clear();
                break;
            case ARG_TYPE_UNSIGNED_NUMBER:
                ss << values[i].data.unsignedNumber;
                query.replace(pos, 1, ss.str());
                ss.clear();
                break;
            case ARG_TYPE_LARGE_NUMBER:
            case ARG_TYPE_LARGE_NUMBER_ALT:
                ss << values[i].data.largeNumber;
                query.replace(pos, 1, ss.str());
                ss.clear();
                break;
            case ARG_TYPE_LARGE_UNSIGNED_NUMBER:
                ss << values[i].data.unsignedLargeNumber;
                query.replace(pos, 1, ss.str());
                ss.clear();
                break;
        }

        ++i;
    }

    sLog.outSQL("%s", query.c_str());

    if (values.size())
        return PreparedExecute(sql, values);
    else
        return PreparedExecute(sql);
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

        bool ok = (item.isStmt) ? _ExecutePreparedStatement(item.stmt, item.values, NULL, false) : DirectExecute(false, item.sql);
        if (!ok)
        {
            transaction->queue.pop();
            mysql_rollback(mMysql);
            mysql_autocommit(mMysql, 1);
            return false;
        }

        if (!item.isStmt)
            free (item.sql);
        else
            delete item.values;
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

    if (FILE* fp = fopen(file, "rb"))
    {
        #if PLATFORM == PLATFORM_WINDOWS
        HANDLE fileHandle = (HANDLE) _get_osfhandle(fileno(fp));
        LockFile(fileHandle, 0, 0, -1, -1);
        #else
        flock(fileno(fp), LOCK_SH);
        #endif
        //------
        
        struct stat info;
        fstat(fileno(fp), &info);

        // if less than 1MB allocate on stack, else on heap
        char* contents = (info.st_size > 1024*1024) ? new char[info.st_size] : (char*) alloca(info.st_size);

        if (fread(contents, info.st_size, 1, fp) == 1)
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
        #if PLATFORM == PLATFORM_WINDOWS
        UnlockFile(fileHandle, 0, 0, -1, -1);
        close((int)fileHandle);
        #else
        flock(fileno(fp), LOCK_UN);
        #endif
        fclose(fp);
    }

    mysql_set_server_option(mMysql, MYSQL_OPTION_MULTI_STATEMENTS_OFF);
    mysql_autocommit(mMysql, 1);
    if (in_transaction)
        mysql_rollback(mMysql);
    return success;
}

PreparedStatement* Database::_GetOrMakePreparedStatement(const char* query, const char* format, PreparedValues* values)
{
    ACE_Guard<ACE_Thread_Mutex> guardian(pMutex);
    PreparedStatementsMap::iterator it = m_preparedStatements.find(query);

    if (it != m_preparedStatements.end())
        return it->second; // found, ok
 
    MYSQL_STMT* stmt = mysql_stmt_init(mMysql);

    if (!stmt)
    {
        sLog.outError("mysql_stmt_init() failed: %s", mysql_error(mMysql));
        return 0;
    }

    {
        // set prefetch rows to maximum, thus making results buffered
        unsigned long rows = (unsigned long) -1;
        if (mysql_stmt_attr_set(stmt, STMT_ATTR_PREFETCH_ROWS, &rows))
            sLog.outError("mysql_stmt_attr_set() failed.");

        if (mysql_stmt_attr_set(stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &my_true))
            sLog.outError("mysql_stmt_attr_set() failed.");
    }

    PreparedStatement* prepStmt = new PreparedStatement;
    prepStmt->stmt = stmt;
    
    if (format)
    {
        prepStmt->types = format;
        
        for (const char* it = format; *it != '\0'; ++it)
        {
            switch (*it)
            {
                case ARG_TYPE_STRING:
                case ARG_TYPE_STRING_ALT:
                case ARG_TYPE_NUMBER:
                case ARG_TYPE_NUMBER_ALT:
                case ARG_TYPE_UNSIGNED_NUMBER:
                case ARG_TYPE_LARGE_NUMBER:
                case ARG_TYPE_LARGE_NUMBER_ALT:
                case ARG_TYPE_LARGE_UNSIGNED_NUMBER:
                case ARG_TYPE_FLOAT:
                case ARG_TYPE_DOUBLE:
                case ARG_TYPE_BINARY:
                case ARG_TYPE_BINARY_ALT:
                    break;
                default:
                    sLog.outError("Unknown format type '%c' for prepared statement (%s)", *it, query);
                    delete prepStmt;
                    return 0;
            }
        }
    }
    else if (values)
    {
        for (size_t i = 0; i < values->m_values.size(); ++i)
            prepStmt->types.append(1, static_cast<char>(values->m_values[i].type));
    }

    {
        if (mysql_stmt_prepare(stmt, query, strlen(query)))
        {
            sLog.outErrorDb("mysql_stmt_prepare() failed: %s, sql: %s", mysql_stmt_error(stmt), query);
            delete prepStmt;
            return 0;
        }
    }

    return m_preparedStatements.insert(std::pair<std::string, PreparedStatement*>(query, prepStmt)).first->second;
}

bool Database::_ExecutePreparedStatement(PreparedStatement* ps, PreparedValues* values, va_list* args, bool resultset)
{
    size_t paramCount = mysql_stmt_param_count(ps->stmt);
    MYSQL_BIND* binding = NULL;
    bool myValues = false;

    if (paramCount)
    {
        if (args)
        {
            if (paramCount != ps->types.size())
            {
                sLog.outErrorDb("Count of parameters passed doesn't equal to count of parameters in prepared statement!");
                delete[] binding;
                return false;
            }

            if (!values)
            {
                values = new PreparedValues(paramCount);
                myValues = true;
            }

            _ConvertValistToPreparedValues(*args, *values, ps->types.c_str());
        }
        else
        {
            ASSERT (values);

            if (paramCount != values->m_values.size())
            {
                sLog.outErrorDb("Count of parameters passed doesn't equal to count of parameters in prepared statement!");
                delete[] binding;
                return false;
            }
        }

        binding = new MYSQL_BIND[paramCount];
        memset(binding, 0, sizeof(MYSQL_BIND)*paramCount);

        for (size_t i = 0; i < paramCount; ++i)
        {
            switch ((*values)[i].type)
            {
                case ARG_TYPE_STRING:
                case ARG_TYPE_STRING_ALT:
                    binding[i].buffer_type = MYSQL_TYPE_STRING;
                    binding[i].buffer = const_cast<char*> ((*values)[i].data.string);
                    binding[i].buffer_length = (*values)[i].data.length;
                    break;
                case ARG_TYPE_BINARY:
                case ARG_TYPE_BINARY_ALT:
                    binding[i].buffer_type = MYSQL_TYPE_BLOB;
                    binding[i].buffer = const_cast<void*> ((*values)[i].data.binary);
                    binding[i].buffer_length = (*values)[i].data.length;
                    break;
                case ARG_TYPE_UNSIGNED_NUMBER:
                    binding[i].is_unsigned = my_true; 
                    /* FALLTHROUGH */
                case ARG_TYPE_NUMBER:
                case ARG_TYPE_NUMBER_ALT:
                    binding[i].buffer_type = MYSQL_TYPE_LONG;
                    binding[i].buffer = &(*values)[i].data.number;
                    binding[i].buffer_length = sizeof((*values)[i].data.number);
                    break;
                case ARG_TYPE_LARGE_UNSIGNED_NUMBER:
                    binding[i].is_unsigned = my_true; 
                    /* FALLTHROUGH */
                case ARG_TYPE_LARGE_NUMBER:
                case ARG_TYPE_LARGE_NUMBER_ALT:
                    binding[i].buffer_type = MYSQL_TYPE_LONGLONG;
                    binding[i].buffer = &(*values)[i].data.largeNumber;
                    binding[i].buffer_length = sizeof(&(*values)[i].data.largeNumber);
                    break;
                case ARG_TYPE_FLOAT:
                    binding[i].buffer_type = MYSQL_TYPE_FLOAT;
                    binding[i].buffer = &(*values)[i].data.float_;
                    binding[i].buffer_length = sizeof(&(*values)[i].data.float_);
                    break;
                case ARG_TYPE_DOUBLE:
                    binding[i].buffer_type = MYSQL_TYPE_DOUBLE;
                    binding[i].buffer = &(*values)[i].data.double_;
                    binding[i].buffer_length = sizeof((*values)[i].data.double_);
                    break;
            }
        }

        if (mysql_stmt_bind_param(ps->stmt, binding))
        {
            sLog.outError("mysql_stmt_bind_param() failed: %s", mysql_stmt_error(ps->stmt));
            delete[] binding;
            if (myValues)
                delete values;
            return false;
        }
    }

    if (mysql_stmt_execute(ps->stmt))
    {
        sLog.outError("mysql_stmt_execute() failed: %s", mysql_stmt_error(ps->stmt));
        delete[] binding;
        if (myValues)
            delete values;
        return false;
    }

    if (myValues)
        delete values;

    // this is safe, even if there's no result
    mysql_stmt_store_result(ps->stmt);

    if (resultset)
    {
        if (!mysql_stmt_field_count(ps->stmt))
        {
            delete[] binding;
            return false;
        }

        if (!mysql_stmt_num_rows(ps->stmt))
        {
            mysql_stmt_free_result(ps->stmt);
            delete[] binding;
            return false;
        }
    }
    else
    {
        if (mysql_stmt_field_count(ps->stmt))
            mysql_stmt_free_result(ps->stmt);
        mysql_stmt_reset(ps->stmt);
    }

    delete[] binding;
    return true;
}

void Database::_ConvertValistToPreparedValues(va_list args, PreparedValues& values, const char* fmt)
{
    for (const char* i = fmt; *i != '\0'; ++i)
    {
        switch (*i)
        {
            case ARG_TYPE_STRING:
            case ARG_TYPE_STRING_ALT:
                values << va_arg(args, const char*);
                break;
            case ARG_TYPE_BINARY:
            case ARG_TYPE_BINARY_ALT:
                {
                    size_t size = va_arg(args, size_t);
                    const void* data = va_arg(args, const void*);
                    values << std::pair<const void*, size_t>(data, size);
                }
                break;
            case ARG_TYPE_UNSIGNED_NUMBER:
                values << va_arg(args, uint32);
                break;
            case ARG_TYPE_NUMBER:
            case ARG_TYPE_NUMBER_ALT:
                values << va_arg(args, int32);
                break;
            case ARG_TYPE_LARGE_UNSIGNED_NUMBER:
                values << va_arg(args, uint64);
                break;
            case ARG_TYPE_LARGE_NUMBER:
            case ARG_TYPE_LARGE_NUMBER_ALT:
                values << va_arg(args, int64);
                break;
            case ARG_TYPE_FLOAT:
                // passed floats are promoted to doubles
                values << (float) va_arg(args, double);
                break;
            case ARG_TYPE_DOUBLE:
                values << va_arg(args, double);
                break;
        }
    }
}

/**
  * @brief Runs Query via Prepared Statements.
  * If statement doesn't exist, it shall be created.
  * This function blocks calling thread until query is done,
  * if your query is result-less use \ref PreparedExecute instead.
  * @param sql The sql to execute in template form
  * @param format Types of bound variables/values - see \ref PreparedArgType
  * @returns Result with compatible interface to \ref QueryResult.
  *
  * @note For compatiblity reasons, if query result failed or contains no rows,
  * the result will be NULL and may cause a crash if you don't count with it.
  *
  * @see Database::Query, Database::PQuery, Database::PreparedExecute
  * Example:
  *
  *     ...PreparedQuery("SELECT 1, 2, 3 FROM table WHERE id = ? AND name = ?", "is", 20, "john")
  */
PreparedQueryResult_AutoPtr Database::PreparedQuery(const char* sql, const char* format, ...)
{
    ACE_Guard<ACE_Thread_Mutex> guardian(mMutex);
    PreparedStatement* stmt = _GetOrMakePreparedStatement(sql, format, NULL);

    if (!stmt)
        return PreparedQueryResult_AutoPtr(NULL);

    va_list ap;
    va_start(ap, format);

    if (!_ExecutePreparedStatement(stmt, NULL, &ap, true))
    {
        va_end(ap);
        return PreparedQueryResult_AutoPtr(NULL);
    }
    
    va_end(ap);

    return PreparedQueryResult_AutoPtr(new PreparedQueryResult(stmt->stmt));
}

PreparedQueryResult_AutoPtr Database::PreparedQuery(const char* sql, PreparedValues& values)
{
    ACE_Guard<ACE_Thread_Mutex> guardian(mMutex);
    PreparedStatement* stmt = _GetOrMakePreparedStatement(sql, NULL, &values);

    if (!stmt)
        return PreparedQueryResult_AutoPtr(NULL);

    if (!_ExecutePreparedStatement(stmt, &values, NULL, true))
        return PreparedQueryResult_AutoPtr(NULL);

    return PreparedQueryResult_AutoPtr(new PreparedQueryResult(stmt->stmt));
}

bool Database::DirectExecute(PreparedStatement* stmt, PreparedValues& values, va_list* args)
{
    ACE_Guard<ACE_Thread_Mutex> guardian(mMutex);

    return _ExecutePreparedStatement(stmt, &values, args, false);
}


/**
  * @brief Executes Query via Prepared Statements.
  */
bool Database::PreparedExecute(const char* sql, const char* format, ...)
{
    if (!mMysql)
        return false;

    PreparedValues values(strlen(format));

    va_list args;
    va_start(args, format);
    _ConvertValistToPreparedValues(args, values, format);
    va_end(args);
    
    PreparedStatement* stmt = _GetOrMakePreparedStatement(sql, NULL, &values);

    // don't use queued execution if it has not been initialized
    if (!m_threadBody)
        return DirectExecute(stmt, values, NULL);

    nMutex.acquire();
    tranThread = ACE_Based::Thread::current();              // owner of this transaction
    TransactionQueues::iterator i = m_tranQueues.find(tranThread);
    if (i != m_tranQueues.end() && i->second != NULL)
        i->second->DelayExecute(stmt, values);                        // Statement for transaction
    else
        m_threadBody->Delay(new SqlPreparedStatement(stmt, values));         // Simple sql statement

    nMutex.release();
    return true;
}

/**
  * @brief Executes Query via Prepared Statements.
  */
bool Database::PreparedExecute(const char* sql, PreparedValues& values)
{
    if (!mMysql)
        return false;

    PreparedStatement* stmt = _GetOrMakePreparedStatement(sql, NULL, &values);

    // don't use queued execution if it has not been initialized
    if (!m_threadBody)
        return DirectExecute(stmt, values, NULL);

    nMutex.acquire();
    tranThread = ACE_Based::Thread::current();              // owner of this transaction
    TransactionQueues::iterator i = m_tranQueues.find(tranThread);
    if (i != m_tranQueues.end() && i->second != NULL)
        i->second->DelayExecute(stmt, values);                        // Statement for transaction
    else
        m_threadBody->Delay(new SqlPreparedStatement(stmt, values));  // Simple sql statement

    nMutex.release();
    return true;
}
