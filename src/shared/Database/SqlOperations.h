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

#ifndef __SQLOPERATIONS_H
#define __SQLOPERATIONS_H

#include "Common.h"

#include "ace/Thread_Mutex.h"
#include "ace/Method_Request.h"
#include "LockedQueue.h"
#include <queue>
#include "Utilities/Callback.h"
#include "QueryResult.h"
#include "Database.h"

// BASE

class Database;
class SqlDelayThread;
struct PreparedStatement;

class SqlOperation
{
    public:
        virtual void OnRemove()
        {
            delete this;
        }
        virtual void Execute(Database* db) = 0;
        virtual ~SqlOperation() {}
};

// ASYNC STATEMENTS / TRANSACTIONS

class SqlStatement : public SqlOperation
{
    private:
        const char* m_sql;
    public:
        SqlStatement(const char* sql) : m_sql(strdup(sql)) {}
        ~SqlStatement()
        {
            void* tofree = const_cast<char*>(m_sql);
            free(tofree);
        }
        void Execute(Database* db);
};

class SqlPreparedStatement : public SqlOperation
{
    private:
        PreparedStatement* m_stmt;
        PreparedValues m_values;
    public:
        SqlPreparedStatement(PreparedStatement* stmt, PreparedValues& values) : m_stmt(stmt), m_values(values) {}

        void Execute(Database* db)
        {
            db->DirectExecute(m_stmt, m_values, NULL);
        }
};

class SqlTransaction : public SqlOperation
{
    protected:
        friend class Database;
        struct QueuedItem
        {
            union
            {
                char* sql;
                struct
                {
                    PreparedStatement* stmt;
                    PreparedValues* values;
                };
            };

            bool isStmt;
        };

        std::queue<QueuedItem> queue;
        ACE_Thread_Mutex mutex;
    public:
        ~SqlTransaction()
        {
            while (!queue.empty())
            {
                QueuedItem item = queue.front();
                if (!item.isStmt)
                    free (item.sql);
                else
                    delete item.values;
                queue.pop();
            }
        }

        void DelayExecute(const char* sql)
        {
            QueuedItem item;
            item.sql = strdup(sql);
            item.isStmt = false;

            mutex.acquire();
            queue.push(item);
            mutex.release();
        }
        void DelayExecute(PreparedStatement* stmt, PreparedValues& values)
        {
            QueuedItem item;
            item.stmt = stmt;
            item.values = new PreparedValues(values.size());
            *item.values = values;
            item.isStmt = true;

            mutex.acquire();
            queue.push(item);
            mutex.release();
        }
        void Execute(Database* db)
        {
            db->ExecuteTransaction(this);
        }
};

// ASYNC QUERIES

class SqlQuery;                                             // contains a single async query
class QueryResult;                                          // the result of one
class SqlResultQueue;                                       // queue for thread sync
class SqlQueryHolder;                                       // groups several async quries
class SqlQueryHolderEx;                                     // points to a holder, added to the delay thread

class SqlResultQueue : public ACE_Based::LockedQueue<Oregon::IQueryCallback*, ACE_Thread_Mutex>
{
    public:
        SqlResultQueue() {}
        void Update();
};

class SqlQuery : public SqlOperation
{
    private:
        const char* m_sql;
        Oregon::IQueryCallback* m_callback;
        SqlResultQueue* m_queue;
    public:
        SqlQuery(const char* sql, Oregon::IQueryCallback* callback, SqlResultQueue* queue)
            : m_sql(strdup(sql)), m_callback(callback), m_queue(queue) {}
        ~SqlQuery()
        {
            void* tofree = const_cast<char*>(m_sql);
            free(tofree);
        }
        void Execute(Database* db);
};

class SqlQueryHolder
{
        friend class SqlQueryHolderEx;
    private:
        typedef std::pair<const char*, QueryResult_AutoPtr> SqlResultPair;
        std::vector<SqlResultPair> m_queries;
    public:
        SqlQueryHolder() {}
        ~SqlQueryHolder();
        bool SetQuery(size_t index, const char* sql);
        bool SetPQuery(size_t index, const char* format, ...) ATTR_PRINTF(3, 4);
        void SetSize(size_t size);
        QueryResult_AutoPtr GetResult(size_t index);
        void SetResult(size_t index, QueryResult_AutoPtr result);
        bool Execute(Oregon::IQueryCallback* callback, SqlDelayThread* thread, SqlResultQueue* queue);
};

class SqlQueryHolderEx : public SqlOperation
{
    private:
        SqlQueryHolder* m_holder;
        Oregon::IQueryCallback* m_callback;
        SqlResultQueue* m_queue;
    public:
        SqlQueryHolderEx(SqlQueryHolder* holder, Oregon::IQueryCallback* callback, SqlResultQueue* queue)
            : m_holder(holder), m_callback(callback), m_queue(queue) {}
        void Execute(Database* db);
};

class SqlAsyncTask : public ACE_Method_Request
{
    public:
        SqlAsyncTask(Database* db, SqlOperation* op) : m_db(db), m_op(op) {}
        ~SqlAsyncTask()
        {
            if (!m_op)
                return;

            delete m_op;
            m_op = NULL;
        }

        int call()
        {
            if (m_db == NULL || m_op == NULL)
                return -1;

            try
            {
                m_op->Execute(m_db);
            }
            catch (...)
            {
                return -1;
            }

            return 0;
        }

    private:
        Database* m_db;
        SqlOperation* m_op;
};
#endif                                                      //__SQLOPERATIONS_H

