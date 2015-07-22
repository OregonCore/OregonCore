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

#if !defined(QUERYRESULT_H)
#define QUERYRESULT_H

#include <ace/Refcounted_Auto_Ptr.h>
#include <ace/Null_Mutex.h>
#include <stdexcept>

#include "Field.h"
#include "Utilities/UnorderedMap.h"

#ifdef WIN32
#define FD_SETSIZE 1024
#include <winsock2.h>
#endif
#include <mysql.h>

class QueryResult
{
    public:
        QueryResult(MYSQL_RES* result, MYSQL_FIELD* fields, uint64 rowCount, uint32 fieldCount);
        ~QueryResult();

        bool NextRow();

        Field* Fetch() const { return mCurrentRow; }
        Field const& operator [] (int index) const { return mCurrentRow[index]; }
        Field const& operator [] (const char* name) const { return mCurrentRow[GetField_idx(name)]; }

        uint32 GetFieldCount() const { return mFieldCount; }
        uint64 GetRowCount() const { return mRowCount; }

        size_t GetField_idx(const char* name) const
        {
            for (size_t i = 0; i < mFieldCount; ++i)
                if (!strcmp(name, mFields[i].name))
                    return i;

            std::string err = "No column named ";
            err += name;
            err += "in field list!";
            throw std::invalid_argument(err);
        }

    protected:
        Field* mCurrentRow;
        uint32 mFieldCount;
        uint64 mRowCount;
        MYSQL_FIELD* mFields;
    
    private:
        void EndQuery();
        
        MYSQL_RES* mResult;
};

class PreparedQueryResult
{
    public:
        PreparedQueryResult(MYSQL_STMT* stmt);
        ~PreparedQueryResult();

        bool NextRow();

        Field* Fetch() const { return mCurrentRow; }
        Field const& operator [] (int index) const { return mCurrentRow[index]; }
        Field const& operator [] (const char* name) const { return mCurrentRow[GetField_idx(name)]; }

        uint32 GetFieldCount() const { return mFieldCount; }
        uint64 GetRowCount() const { return mRowCount; }

        size_t GetField_idx(const char* name) const
        {
            for (size_t i = 0; i < mFieldCount; ++i)
                if (!strcmp(name, mFields[i].name))
                    return i;

            std::string err = "No column named ";
            err += name;
            err += "in field list!";
            throw std::invalid_argument(err);
        }

    protected:
        Field* mCurrentRow;
        uint32 mFieldCount;
        uint64 mRowCount;
        MYSQL_FIELD* mFields;
    
    private:
        void EndQuery();

        std::vector<Field*> mRows;
        size_t mCursor;
        MYSQL_RES* mMetaData;
};

typedef ACE_Refcounted_Auto_Ptr<QueryResult, ACE_Null_Mutex> QueryResult_AutoPtr;
typedef ACE_Refcounted_Auto_Ptr<PreparedQueryResult, ACE_Null_Mutex> PreparedQueryResult_AutoPtr;

#endif
