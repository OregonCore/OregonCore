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

#ifndef MANGOS_CREATURE_EAI_MGR_H
#define MANGOS_CREATURE_EAI_MGR_H

#include "Common.h"
#include "CreatureEventAI.h"

class CreatureEventAIMgr
{
    public:
        CreatureEventAIMgr() {};
        ~CreatureEventAIMgr() {};

        void LoadCreatureEventAI_Texts(bool check_entry_use);
        void LoadCreatureEventAI_Summons(bool check_entry_use);
        void LoadCreatureEventAI_Scripts();

        CreatureEventAI_Event_Map  const& GetCreatureEventAIMap()       const
        {
            return m_CreatureEventAI_Event_Map;
        }
        CreatureEventAI_Summon_Map const& GetCreatureEventAISummonMap() const
        {
            return m_CreatureEventAI_Summon_Map;
        }
        CreatureEventAI_TextMap    const& GetCreatureEventAITextMap()   const
        {
            return m_CreatureEventAI_TextMap;
        }

    private:
        void CheckUnusedAITexts();
        void CheckUnusedAISummons();

        CreatureEventAI_Event_Map  m_CreatureEventAI_Event_Map;
        CreatureEventAI_Summon_Map m_CreatureEventAI_Summon_Map;
        CreatureEventAI_TextMap    m_CreatureEventAI_TextMap;
};

#define CreatureEAI_Mgr Oregon::Singleton<CreatureEventAIMgr>::Instance()
#endif

