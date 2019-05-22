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

#if !defined(FIELD_H)
#define FIELD_H

#include "Common.h"
#include <mysql.h>

class Field
{
    public:

        Field() = default;
        Field(Field const&) = delete;
        Field& operator=(Field const&) = delete;
        ~Field() = default;

        enum_field_types GetType() const
        {
            return mType;
        }

        const char* GetString() const
        {
            return mValue;
        }

        std::string GetCppString() const
        {
            return mValue ? mValue : "";
        }

        float GetFloat() const
        {
            return mValue ? static_cast<float>(atof(mValue)) : 0.0f;
        }

        bool GetBool() const
        {
            return mValue ? atoi(mValue) > 0 : false;
        }

        int32 GetInt32() const
        {
            return mValue ? static_cast<int32>(atol(mValue)) : int32(0);
        }

        uint8 GetUInt8() const
        {
            return mValue ? static_cast<uint8>(atol(mValue)) : uint8(0);
        }

        uint16 GetUInt16() const
        {
            return mValue ? static_cast<uint16>(atol(mValue)) : uint16(0);
        }

        int16 GetInt16() const
        {
            return mValue ? static_cast<int16>(atol(mValue)) : int16(0);
        }

        uint32 GetUInt32() const
        {
            return mValue ? static_cast<uint32>(atol(mValue)) : uint32(0);
        }

        uint64 GetUInt64() const
        {
            return mValue ? strtoull(mValue, NULL, 10) : uint64(0);
        }

        int64 GetInt64() const
        {
            return mValue ? strtoll(mValue, NULL, 10) : int64(0);
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
        };

    private:

        const char* mValue = nullptr;
        enum_field_types mType = MYSQL_TYPE_NULL;  //!< The mysql type of this field
};
#endif

