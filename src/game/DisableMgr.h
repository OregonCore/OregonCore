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

#include <ace/Singleton.h>
#include "Unit.h"

#ifndef _DISABLEMGR_H
#define _DISABLEMGR_H

enum DisableType
{
    DISABLE_TYPE_SPELL                  = 0,
    DISABLE_TYPE_QUEST                  = 1,
    DISABLE_TYPE_MAP                    = 2,
    DISABLE_TYPE_BATTLEGROUND           = 3
};

enum SpellDisableTypes
{
    SPELL_DISABLE_PLAYER            = 0x1,
    SPELL_DISABLE_CREATURE          = 0x2,
    SPELL_DISABLE_PET               = 0x4,
    SPELL_DISABLE_DEPRECATED_SPELL  = 0x8,
    SPELL_DISABLE_MAP               = 0x10,
    SPELL_DISABLE_AREA              = 0x20,
    SPELL_DISABLE_LOS               = 0x40,
    MAX_SPELL_DISABLE_TYPE = (  SPELL_DISABLE_PLAYER | SPELL_DISABLE_CREATURE | SPELL_DISABLE_PET |
                                SPELL_DISABLE_DEPRECATED_SPELL | SPELL_DISABLE_MAP | SPELL_DISABLE_AREA |
                                SPELL_DISABLE_LOS),
};

#define MAX_DISABLE_TYPES 4

struct DisableData
{
    uint8 flags;
    std::set<uint32> params[2];                             // params0, params1
};

typedef std::map<uint32, DisableData> DisableTypeMap;       // single disables here with optional data
typedef std::map<DisableType, DisableTypeMap> DisableMap;   // global disable map by source

class DisableMgr
{
        friend class ACE_Singleton<DisableMgr, ACE_Null_Mutex>;
        DisableMgr();

    public:
        ~DisableMgr();

        void LoadDisables();
        bool IsDisabledFor(DisableType type, uint32 entry, Unit* pUnit, uint8 flags = 0);
        void CheckQuestDisables();

    protected:
        DisableMap m_DisableMap;
};

#define sDisableMgr (*ACE_Singleton<DisableMgr, ACE_Null_Mutex>::instance())

#endif //_DISABLEMGR_H
