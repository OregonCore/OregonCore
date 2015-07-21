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

#ifndef DATABASE_PREPARED_STATEMENT_H
#define DATABASE_PREPARED_STATEMENT_H

#include "Common.h"
#include <mysql.h>

struct PreparedStatement
{
    MYSQL_STMT* stmt;
    std::string types;
};

enum PreparedArgType
{
    ARG_TYPE_STRING                = 's', //!< char const*, can be NULL
    ARG_TYPE_BINARY                = 'b', //!< void const*, can be NULL
    ARG_TYPE_NUMBER                = 'i', //!< signed int
    ARG_TYPE_UNSIGNED_NUMBER       = 'u', //!< unsigned int
    ARG_TYPE_LARGE_NUMBER          = 'I', //!< 64bit signed int
    ARG_TYPE_LARGE_UNSIGNED_NUMBER = 'U', //!< 64bit unsigned int
    ARG_TYPE_FLOAT                 = 'f', //!< floating point
    ARG_TYPE_DOUBLE                = 'F', //!< double

    ARG_TYPE_STRING_ALT            = 'S', //!< alias
    ARG_TYPE_BINARY_ALT            = 'B', //!< alias
    ARG_TYPE_NUMBER_ALT            = 'd', //!< alias
    ARG_TYPE_LARGE_NUMBER_ALT      = 'D'  //!< alias
};

union PreparedArgDataUnion
{
    struct
    {
        union
        {
            const char* string;
            const void* binary;
        };

        size_t length;
    };

    int32 number;
    uint32 unsignedNumber;
    
    int64 largeNumber;
    uint64 unsignedLargeNumber;

    float float_;
    double double_;
};

class PreparedValues
{
    protected:
        friend class Database;
        struct Value
        {
            PreparedArgType type;
            PreparedArgDataUnion data;

            #define ConstructorImpl(datatype, argtype, section) \
                Value(datatype param)         \
                {                             \
                    type = argtype;           \
                    data.section = param;     \
                }
            ConstructorImpl(int32,       ARG_TYPE_NUMBER,                number);
            ConstructorImpl(uint32,      ARG_TYPE_UNSIGNED_NUMBER,       unsignedNumber);
            ConstructorImpl(int64,       ARG_TYPE_LARGE_NUMBER,          largeNumber);
            ConstructorImpl(uint64,      ARG_TYPE_LARGE_UNSIGNED_NUMBER, unsignedLargeNumber);
            ConstructorImpl(float,       ARG_TYPE_FLOAT,                 float_);
            ConstructorImpl(double,      ARG_TYPE_DOUBLE,                double_);

            Value(const char* str)
            {
                type = ARG_TYPE_STRING;
                data.string = str;
                data.length = strlen(str);
            }

            Value(std::string const& str)
            {
                type = ARG_TYPE_STRING;
                data.string = str.c_str();
                data.length = str.length();
            }

            Value(std::pair<const void*, size_t> binaryData)
            {
                type = ARG_TYPE_BINARY;
                data.binary = binaryData.first;
                data.length = binaryData.second;
            }
            #undef ConstructorImpl
        };

        std::vector<Value> m_values;
        #ifdef OREGON_DEBUG
        size_t m_size;
        #endif
    public:
        explicit PreparedValues(size_t size)
            #ifdef OREGON_DEBUG
            : m_size(size)
            #endif
        {
            m_values.reserve(size);
        }

        template<class T>
        PreparedValues& operator<<(T data)
        {
            #ifdef OREGON_DEBUG
            ASSERT(m_values.size() + 1 <= m_size);
            #endif
            m_values.push_back(data);
            return *this;
        }

        size_t size() const
        {
            return m_values.size();
        }

        Value& operator[] (size_t index)
        {
            return m_values[index];
        }
};

#endif
