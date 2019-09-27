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
#include "QueryResult.h"

QueryResult::QueryResult(MYSQL_RES* result, MYSQL_FIELD* fields, uint64 rowCount, uint32 fieldCount)
    : mFieldCount(fieldCount)
    , mRowCount(rowCount)
    , mFields(fields)
    , mResult(result)
{
    mCurrentRow = new Field[mFieldCount];
    ASSERT(mCurrentRow);

    for (uint32 i = 0; i < mFieldCount; i++)
        mCurrentRow[i].SetType(fields[i].type);
}

PreparedQueryResult::PreparedQueryResult(MYSQL_STMT* stmt) : mCursor(0)
{
    // as mysql prepared statements c api is not thread safe,
    // we need to copy all result data to a different location
    // so another thread can execute this statement again

    mFieldCount = mysql_stmt_field_count(stmt);
    mRowCount = mysql_stmt_num_rows(stmt);

    MYSQL_BIND* binding = new MYSQL_BIND[mFieldCount];
    my_bool* isNull = new my_bool[mFieldCount];
    unsigned long* length = new unsigned long[mFieldCount];

    memset(binding, 0, sizeof(MYSQL_BIND) * mFieldCount);
    memset(isNull,  0, sizeof(my_bool) * mFieldCount);
    memset(length,  0, sizeof(unsigned long) * mFieldCount);

    mMetaData = mysql_stmt_result_metadata(stmt);
    mFields = mysql_fetch_fields(mMetaData);
    for (size_t i = 0; i < mFieldCount; ++i)
    {
        // if its a string, reserve one byte for ascii nul
        size_t size = mFields[i].max_length + 1;
        binding[i].buffer_type = mFields[i].type;
        binding[i].buffer = malloc(size);
        binding[i].buffer_length = size;
        binding[i].length = &length[i];
        binding[i].is_null = &isNull[i];
        binding[i].error = NULL;
        binding[i].is_unsigned = mFields[i].flags & UNSIGNED_FLAG;

        memset(binding[i].buffer, 0, size);
    }

    if (mysql_stmt_bind_result(stmt, binding))
    {
        // should never happen
        sLog.outError("mysql_stmt_bind_result() failed!");
        goto PreparedQuery_Cleanup; // after this point, destiny is bond...
    }

    mRows.resize(mRowCount, NULL);
    for (size_t i = 0; i < mRowCount; ++i)
    {
        mRows[i] = new Field[mFieldCount];
        
        switch (mysql_stmt_fetch(stmt))
        {
            case 0: // success
                break;
            case 1: // error
                sLog.outError("mysql_stmt_fetch() failed: %s", mysql_stmt_error(stmt));
                /* FALLTHROUGH */
            case MYSQL_NO_DATA: // no more data exists
                mRowCount = i;
                goto PreparedQuery_Break_2;
            case MYSQL_DATA_TRUNCATED:
                sLog.outDetail("mysql_stmt_fetch() - data truncated");
                break;
        }

        for (size_t j = 0; j < mFieldCount; ++j)
            mRows[i][j].SetBinaryValue(binding[j].buffer, length[i], mFields[j].type);
    }

PreparedQuery_Break_2:

    mCurrentRow = mRows[0];
    ASSERT(mCurrentRow);

PreparedQuery_Cleanup:

    delete[] binding;
    delete[] isNull;
    delete[] length;

    mysql_stmt_free_result(stmt);
    mysql_stmt_reset(stmt);
}

PreparedQueryResult::~PreparedQueryResult()
{
    EndQuery();
}

bool PreparedQueryResult::NextRow()
{
    if (++mCursor >= mRows.size())
    {
        EndQuery();
        return false;
    }

    mCurrentRow = mRows[mCursor];
    return true;
}

void PreparedQueryResult::EndQuery()
{
    for (size_t i = 0; i < mRows.size(); ++i)
        delete[] mRows[i];
    mRows.clear();

    if (mMetaData)
        mysql_free_result(mMetaData);
}

QueryResult::~QueryResult()
{
    EndQuery();
}

bool QueryResult::NextRow()
{
    MYSQL_ROW row;

    if (!mResult)
        return false;

    row = mysql_fetch_row(mResult);
    if (!row)
    {
        EndQuery();
        return false;
    }

    for (uint32 i = 0; i < mFieldCount; i++)
        mCurrentRow[i].SetValue(row[i]);

    return true;
}

void QueryResult::EndQuery()
{
    if (mCurrentRow)
    {
        delete [] mCurrentRow;
        mCurrentRow = 0;
    }

    if (mResult)
    {
        mysql_free_result(mResult);
        mResult = 0;
    }
}
#if 0
enum Field::DataTypes QueryResult::ConvertNativeType(enum_field_types mysqlType) const
{
    switch (mysqlType)
    {
    case FIELD_TYPE_TIMESTAMP:
        case FIELD_TYPE_DATE:
            case FIELD_TYPE_TIME:
                case FIELD_TYPE_DATETIME:
                    case FIELD_TYPE_YEAR:
                        case FIELD_TYPE_STRING:
                            case FIELD_TYPE_VAR_STRING:
                                case FIELD_TYPE_BLOB:
                                    case FIELD_TYPE_SET:
                                        case FIELD_TYPE_NULL:
                                                return Field::DB_TYPE_STRING;
    case FIELD_TYPE_TINY:

    case FIELD_TYPE_SHORT:
    case FIELD_TYPE_LONG:
    case FIELD_TYPE_INT24:
    case FIELD_TYPE_LONGLONG:
    case FIELD_TYPE_ENUM:
        return Field::DB_TYPE_INTEGER;
    case FIELD_TYPE_DECIMAL:
    case FIELD_TYPE_FLOAT:
    case FIELD_TYPE_DOUBLE:
        return Field::DB_TYPE_FLOAT;
    default:
        return Field::DB_TYPE_UNKNOWN;
    }
}
#endif
