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

#ifndef DATABASE_H
#define DATABASE_H

#include "Threading.h"
#include "Utilities/UnorderedMap.h"
#include "Database/SqlDelayThread.h"
#include "Policies/Singleton.h"
#include "ace/Thread_Mutex.h"
#include "ace/Guard_T.h"
#include "ace/Atomic_Op.h"
#include "PreparedStatement.h"
#include "QueryResult.h"

#ifdef WIN32
#define FD_SETSIZE 1024
#include <winsock2.h>
#endif
#include <mysql.h>

class SqlTransaction;
class SqlResultQueue;
class SqlQueryHolder;

typedef UNORDERED_MAP<ACE_Based::Thread*, SqlTransaction*> TransactionQueues;
typedef UNORDERED_MAP<ACE_Based::Thread*, SqlResultQueue*> QueryQueues;

#define MAX_QUERY_LEN   1024

class Database
{
    protected:
        TransactionQueues m_tranQueues;                            // Transaction queues from diff. threads
        QueryQueues m_queryQueues;                                 // Query queues from diff threads
        SqlDelayThread* m_threadBody;                              // Pointer to delay sql executer (owned by m_delayThread)
        ACE_Based::Thread* m_delayThread;                          // Pointer to executer thread

    public:

        Database();
        ~Database();

        /// @param infoString should be formated like hostname;username;password;database.
        bool Initialize(const char* infoString);

        bool IsConnected() const { return m_connected; }

        void InitDelayThread();
        void HaltDelayThread();

        QueryResult_AutoPtr Query(const char* sql);
        QueryResult_AutoPtr PQuery(const char* format, ...) ATTR_PRINTF(2, 3);

        bool ExecuteFile(const char* file);

        // Async queries and query holders, implemented in DatabaseImpl.h

        // Query / member
        template<class Class>
        bool AsyncQuery(Class* object, void (Class::*method)(QueryResult_AutoPtr), const char* sql);
        template<class Class, typename ParamType1>
        bool AsyncQuery(Class* object, void (Class::*method)(QueryResult_AutoPtr, ParamType1), ParamType1 param1, const char* sql);
        template<class Class, typename ParamType1, typename ParamType2>
        bool AsyncQuery(Class* object, void (Class::*method)(QueryResult_AutoPtr, ParamType1, ParamType2), ParamType1 param1, ParamType2 param2, const char* sql);
        template<class Class, typename ParamType1, typename ParamType2, typename ParamType3>
        bool AsyncQuery(Class* object, void (Class::*method)(QueryResult_AutoPtr, ParamType1, ParamType2, ParamType3), ParamType1 param1, ParamType2 param2, ParamType3 param3, const char* sql);
        // Query / static
        template<typename ParamType1>
        bool AsyncQuery(void (*method)(QueryResult_AutoPtr, ParamType1), ParamType1 param1, const char* sql);
        template<typename ParamType1, typename ParamType2>
        bool AsyncQuery(void (*method)(QueryResult_AutoPtr, ParamType1, ParamType2), ParamType1 param1, ParamType2 param2, const char* sql);
        template<typename ParamType1, typename ParamType2, typename ParamType3>
        bool AsyncQuery(void (*method)(QueryResult_AutoPtr, ParamType1, ParamType2, ParamType3), ParamType1 param1, ParamType2 param2, ParamType3 param3, const char* sql);
        // PQuery / member
        template<class Class>
        bool AsyncPQuery(Class* object, void (Class::*method)(QueryResult_AutoPtr), const char* format, ...) ATTR_PRINTF(4, 5);
        template<class Class, typename ParamType1>
        bool AsyncPQuery(Class* object, void (Class::*method)(QueryResult_AutoPtr, ParamType1), ParamType1 param1, const char* format, ...) ATTR_PRINTF(5, 6);
        template<class Class, typename ParamType1, typename ParamType2>
        bool AsyncPQuery(Class* object, void (Class::*method)(QueryResult_AutoPtr, ParamType1, ParamType2), ParamType1 param1, ParamType2 param2, const char* format, ...) ATTR_PRINTF(6, 7);
        template<class Class, typename ParamType1, typename ParamType2, typename ParamType3>
        bool AsyncPQuery(Class* object, void (Class::*method)(QueryResult_AutoPtr, ParamType1, ParamType2, ParamType3), ParamType1 param1, ParamType2 param2, ParamType3 param3, const char* format, ...) ATTR_PRINTF(7, 8);
        // PQuery / static
        template<typename ParamType1>
        bool AsyncPQuery(void (*method)(QueryResult_AutoPtr, ParamType1), ParamType1 param1, const char* format, ...) ATTR_PRINTF(4, 5);
        template<typename ParamType1, typename ParamType2>
        bool AsyncPQuery(void (*method)(QueryResult_AutoPtr, ParamType1, ParamType2), ParamType1 param1, ParamType2 param2, const char* format, ...) ATTR_PRINTF(5, 6);
        template<typename ParamType1, typename ParamType2, typename ParamType3>
        bool AsyncPQuery(void (*method)(QueryResult_AutoPtr, ParamType1, ParamType2, ParamType3), ParamType1 param1, ParamType2 param2, ParamType3 param3, const char* format, ...) ATTR_PRINTF(6, 7);
        template<class Class>
        // QueryHolder
        bool DelayQueryHolder(Class* object, void (Class::*method)(QueryResult_AutoPtr, SqlQueryHolder*), SqlQueryHolder* holder);
        template<class Class, typename ParamType1>
        bool DelayQueryHolder(Class* object, void (Class::*method)(QueryResult_AutoPtr, SqlQueryHolder*, ParamType1), SqlQueryHolder* holder, ParamType1 param1);

        bool Execute(const char* sql);
        bool PExecute(const char* format, ...) ATTR_PRINTF(2, 3);

        bool DirectExecute(const char* sql)
        {
            return DirectExecute(true, sql);
        }
        bool DirectPExecute(const char* format, ...) ATTR_PRINTF(2, 3);
        bool DirectExecute(PreparedStatement* stmt, PreparedValues& values, va_list* args);

        // Writes SQL commands to a LOG file (see Oregond.conf "LogSQL")
        bool PExecuteLog(const char* format, ...) ATTR_PRINTF(2, 3);

        // Writes SQL commands to a LOG file (see Oregond.conf "LogSQL")
        // but runs via PreparedStatements
        bool PreparedExecuteLog(const char* sql, const char* format = NULL, ...);
        bool PreparedExecuteLog(const char* sql, PreparedValues& values);

        bool BeginTransaction();
        bool CommitTransaction();
        bool RollbackTransaction();

        bool ExecuteTransaction(SqlTransaction* transaction);

        PreparedQueryResult_AutoPtr PreparedQuery(const char* sql, const char* format = NULL, ...);
        PreparedQueryResult_AutoPtr PreparedQuery(const char* sql, PreparedValues& values);
        bool PreparedExecute(const char* sql, const char* format = NULL, ...);
        bool PreparedExecute(const char* sql, PreparedValues& values);

        operator bool () const
        {
            return mMysql != NULL;
        }
        unsigned long escape_string(char* to, const char* from, unsigned long length);
        void escape_string(std::string& str);

        void ThreadStart();
        void ThreadEnd();

        // sets the result queue of the current thread, be careful what thread you call this from
        void SetResultQueue(SqlResultQueue* queue);

    protected:
        bool DirectExecute(bool lock, const char* sql);
    private:
        bool m_logSQL;
        std::string m_logsDir;
        ACE_Thread_Mutex mMutex;        // For thread safe operations between core and mySQL server
        ACE_Thread_Mutex nMutex;        // For thread safe operations on m_transQueues
        ACE_Thread_Mutex pMutex;        // For thread safe operations on m_preparedStatements

        ACE_Based::Thread* tranThread;

        MYSQL* mMysql;
        bool m_connected;

        static size_t db_count;

        bool _TransactionCmd(const char* sql);
        bool _Query(const char* sql, MYSQL_RES** pResult, MYSQL_FIELD** pFields, uint64* pRowCount, uint32* pFieldCount);

        PreparedStatement* _GetOrMakePreparedStatement(const char* query, const char* format, PreparedValues* values);
        bool _ExecutePreparedStatement(PreparedStatement* ps, PreparedValues* values, va_list* args, bool resultset);
        void _ConvertValistToPreparedValues(va_list ap, PreparedValues& values, const char* fmt);

        typedef UNORDERED_MAP<std::string, PreparedStatement*> PreparedStatementsMap;
        PreparedStatementsMap m_preparedStatements;
};
#endif

