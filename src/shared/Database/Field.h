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

#if !defined(FIELD_H)
#define FIELD_H

#include "Common.h"
#include <mysql.h>

class Field
{
    public:

        Field() : mValue(NULL), mType(MYSQL_TYPE_NULL) {}

        ~Field()
        {
            if (mIsRawValue)
                free (mRawValue);
        }

        enum_field_types GetType() const
        {
            return mType;
        }

        const char* GetString() const
        {
            if (!mIsRawValue)
                return mValue;

            if (mType != MYSQL_TYPE_STRING)
                return NULL;

            return reinterpret_cast<char*> (mRawValue);
        }

        std::string GetCppString() const
        {
            if (!mIsRawValue)
                return mValue ? mValue : "";                    // std::string s = 0 have undefine result in C++

            if (mType != MYSQL_TYPE_STRING || !mRawValue)
                return "";

            return reinterpret_cast<char*> (mRawValue);
        }

        std::vector<uint8> GetBinary() const
        {
            std::vector<uint8> data;

            if (!mIsRawValue)
                return data;

            data.resize(mRawValueSize, 0);
            std::copy((unsigned char*)mRawValue, ((unsigned char*) mRawValue) + mRawValueSize, data.begin());
            
            return data;
        }

        #ifdef OREGON_DEBUG
        #  define dbg_help_var 1
        #else
        #  define dbg_help_var 0
        #endif
        #define GetIntegerDataImpl(name, expected_type, ctype, regular_cast)   \
        ctype name() const                                                     \
        {                                                                      \
            if (!mValue)                                                       \
                return 0;                                                      \
                                                                               \
            if (!mIsRawValue)                                                  \
                return static_cast<ctype>(regular_cast);                       \
                                                                               \
            if (mType != expected_type)                                        \
            {                                                                  \
                /*if (dbg_help_var)*/                                              \
                /*    error_db_log(__FUNCTION__ " on incorrent column data type.");*/ \
                                                                               \
                switch (mType)                                                 \
                {                                                                                                   \
                    case MYSQL_TYPE_TINY:     return static_cast<ctype>(*reinterpret_cast<int8*>(mRawValue));       \
                    case MYSQL_TYPE_SHORT:    return static_cast<ctype>(*reinterpret_cast<int16*>(mRawValue));      \
                    case MYSQL_TYPE_INT24:                                                                          \
                    case MYSQL_TYPE_LONG:     return static_cast<ctype>(*reinterpret_cast<int32*>(mRawValue));      \
                    case MYSQL_TYPE_LONGLONG: return static_cast<ctype>(*reinterpret_cast<int64*>(mRawValue));      \
                    case MYSQL_TYPE_FLOAT:    return static_cast<ctype>(*reinterpret_cast<float*>(mRawValue));      \
                    case MYSQL_TYPE_DOUBLE:   return static_cast<ctype>(*reinterpret_cast<double*>(mRawValue));     \
                    default:                  return 0;                                                             \
                }                                                                                                   \
            }                                                                                                       \
                                                                                                                    \
            return *reinterpret_cast<ctype*> (mRawValue);                                                           \
        }

        GetIntegerDataImpl(GetFloat,  MYSQL_TYPE_FLOAT,      float,  strtof(mValue,   NULL))
        GetIntegerDataImpl(GetDouble, MYSQL_TYPE_DOUBLE,     double, strtod(mValue,   NULL))
        GetIntegerDataImpl(GetInt8,   MYSQL_TYPE_TINY,       int8,   strtol(mValue,   NULL, 10))
        GetIntegerDataImpl(GetInt16,  MYSQL_TYPE_SHORT,      int16,  strtol(mValue,   NULL, 10))
        GetIntegerDataImpl(GetInt32,  MYSQL_TYPE_LONG,       int32,  strtol(mValue,   NULL, 10))
        GetIntegerDataImpl(GetInt64,  MYSQL_TYPE_LONGLONG,   int64,  strtoll(mValue,  NULL, 10))
        GetIntegerDataImpl(GetUInt8,  MYSQL_TYPE_TINY,       uint8,  strtoul(mValue,  NULL, 10))
        GetIntegerDataImpl(GetUInt16, MYSQL_TYPE_SHORT,      uint16, strtoul(mValue,  NULL, 10))
        GetIntegerDataImpl(GetUInt32, MYSQL_TYPE_LONG,       uint32, strtoul(mValue,  NULL, 10))
        GetIntegerDataImpl(GetUInt64, MYSQL_TYPE_LONGLONG,   uint64, strtoull(mValue, NULL, 10))

        #undef GetIntegerDataImpl 
        #undef dbg_help_var

        bool GetBool() const
        {
            return GetUInt8() == 1;
        }

        /// Used by PreparedQueryResult, we need to copy the original data and free it later
        void SetBinaryValue(void* value, size_t size, enum_field_types type)
        {
            mRawValue = value;
            mRawValueSize = size;
            mType = type;
            mIsRawValue = true;
        }
        
        /// Called by QueryResult
        void SetType(enum_field_types type)
        {
            mType = type;
        }
        
        /// Called by QueryResult, no need free this
        void SetValue(const char* value)
        {
            mValue = value;
            mIsRawValue = false;
        };

    private:
        Field(Field const&);
        Field& operator=(Field const&);

        union
        {
            /// data and size (prepared statements)
            /// freed in destructor
            struct
            {
                void* mRawValue;
                size_t mRawValueSize;
            };

            /// data from regular query, we don't own this data,
            /// mysql_free_result will free these
            const char* mValue;
        };

        enum_field_types mType;  //!< The mysql type of this field
        bool mIsRawValue;        //!< flag whether we have native data (from prepared statements) or not
};
#endif

