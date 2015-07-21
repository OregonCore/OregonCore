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

#include "Database/SqlDelayThread.h"
#include "Database/SqlOperations.h"
#include "DatabaseEnv.h"

SqlDelayThread::SqlDelayThread(Database* db) : m_dbEngine(db), m_running(true)
{
}

void SqlDelayThread::run()
{
    mysql_thread_init();

    SqlAsyncTask* s = NULL;

    ACE_Time_Value _time(2);
    while (m_running)
    {
        // if the running state gets turned off while sleeping
        // empty the queue before exiting
        s = dynamic_cast<SqlAsyncTask*> (m_sqlQueue.dequeue());
        if (s)
        {
            s->call();
            delete s;
        }
    }

    mysql_thread_end();
}

void SqlDelayThread::Stop()
{
    // wait for queue to become empty, so no query
    // will be missed and also no memory leak will be created
    while (m_sqlQueue.method_count())
        ACE_Based::Thread::Sleep(10);

    m_running = false;
    m_sqlQueue.queue()->deactivate();
}

bool SqlDelayThread::Delay(SqlOperation* sql)
{
    int res = m_sqlQueue.enqueue(new SqlAsyncTask(m_dbEngine, sql));
    return (res != -1);
}

