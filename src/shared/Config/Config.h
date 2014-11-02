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

#ifndef CONFIG_H
#define CONFIG_H

#include "Common.h"
#include <Policies/Singleton.h>
#include "Platform/Define.h"

class ACE_Configuration_Heap;

class Config
{
    public:
        Config();
        ~Config();

        bool SetSource(const char* file);
        bool Reload();

        std::string GetStringDefault(const char* name, const char* def);
        bool GetBoolDefault(const char* name, const bool def = false);
        int32 GetIntDefault(const char* name, const int32 def);
        float GetFloatDefault(const char* name, const float def);

        std::string GetFilename() const
        {
            return mFilename;
        }

    private:

        std::string mFilename;
        ACE_Configuration_Heap* mConf;
};

#define sConfig Oregon::Singleton<Config>::Instance()

#endif

