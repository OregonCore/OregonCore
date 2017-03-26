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

#include "SpellMgr.h"
#include "ObjectMgr.h"
#include "DisableMgr.h"

DisableMgr::DisableMgr()
{
}

DisableMgr::~DisableMgr()
{
    for (DisableMap::iterator itr = m_DisableMap.begin(); itr != m_DisableMap.end(); ++itr)
        itr->second.clear();
    m_DisableMap.clear();
}

void DisableMgr::LoadDisables()
{
    // reload case
    for (DisableMap::iterator itr = m_DisableMap.begin(); itr != m_DisableMap.end(); ++itr)
        itr->second.clear();
    m_DisableMap.clear();

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT sourceType,entry,flags,params_0,params_1 FROM disables");

    uint32 total_count = 0;

    if (!result)
    {
        sLog.outString(">> Loaded %u disables", total_count);
        return;
    }

    Field* fields;
    do
    {
        fields = result->Fetch();
        DisableType type = DisableType(fields[0].GetUInt32());
        if (uint32(type) >= MAX_DISABLE_TYPES)
        {
            sLog.outErrorDb("Invalid type %u specified in `disables` table, skipped.", type);
            continue;
        }
        uint32 entry = fields[1].GetUInt32();
        uint8 flags = fields[2].GetUInt8();
        std::string params_0 = fields[3].GetString();
        std::string params_1 = fields[4].GetString();

        DisableData data;
        data.flags = flags;

        switch (type)
        {
        case DISABLE_TYPE_SPELL:
            {
                if (!(sSpellStore.LookupEntry(entry) || flags & SPELL_DISABLE_DEPRECATED_SPELL))
                {
                    sLog.outErrorDb("Spell entry %u from `disables` doesn't exist in dbc, skipped.", entry);
                    continue;
                }

                if (!flags || flags > MAX_SPELL_DISABLE_TYPE)
                {
                    sLog.outErrorDb("Disable flags for spell %u are invalid, skipped.", entry);
                    continue;
                }

                if (flags & SPELL_DISABLE_MAP)
                {

                    Tokens tokens = StrSplit(params_0, ",");
                    for (uint8 i = 0; i < tokens.size(); )
                        data.params[0].insert(atoi(tokens[i++].c_str()));
                }

                if (flags & SPELL_DISABLE_AREA)
                {
                    Tokens tokens = StrSplit(params_1, ",");
                    for (uint8 i = 0; i < tokens.size(); )
                        data.params[1].insert(atoi(tokens[i++].c_str()));
                }

            }
            break;
        // checked later
        case DISABLE_TYPE_QUEST:
            break;
        case DISABLE_TYPE_MAP:
            {
                MapEntry const* mapEntry = sMapStore.LookupEntry(entry);
                if (!mapEntry)
                {
                    sLog.outErrorDb("Map entry %u from `disables` doesn't exist in dbc, skipped.", entry);
                    continue;
                }
                bool isFlagInvalid = false;
                switch (mapEntry->map_type)
                {
                case MAP_COMMON:
                    if (flags)
                        isFlagInvalid = true;
                    break;
                case MAP_INSTANCE:
                case MAP_RAID:
                    if (flags & DUNGEON_STATUSFLAG_NORMAL && !mapEntry->IsDungeon())
                        isFlagInvalid = true;
                    if (flags & DUNGEON_STATUSFLAG_HEROIC && !mapEntry->SupportsHeroicMode())
                        isFlagInvalid = true;
                    else if (flags & RAID_STATUSFLAG_10MAN && !mapEntry->IsRaid())
                        isFlagInvalid = true;
                    else if (flags & RAID_STATUSFLAG_25MAN && !mapEntry->IsRaid())
                        isFlagInvalid = true;
                    break;
                case MAP_BATTLEGROUND:
                case MAP_ARENA:
                    sLog.outErrorDb("Battleground map %u specified to be disabled in map case, skipped.", entry);
                    continue;
                }
                if (isFlagInvalid)
                {
                    sLog.outErrorDb("Disable flags for map %u are invalid, skipped.", entry);
                    continue;
                }
                break;
            }
        case DISABLE_TYPE_BATTLEGROUND:
            if (!sBattlemasterListStore.LookupEntry(entry))
            {
                sLog.outErrorDb("Battleground entry %u from `disables` doesn't exist in dbc, skipped.", entry);
                continue;
            }
            if (flags)
                sLog.outErrorDb("Disable flags specified for battleground %u, useless data.", entry);
            break;
        }

        m_DisableMap[type].insert(DisableTypeMap::value_type(entry, data));
        ++total_count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u disables.", total_count);
}

void DisableMgr::CheckQuestDisables()
{
    uint32 count = m_DisableMap[DISABLE_TYPE_QUEST].size();
    if (!count)
        return;

    // check only quests, rest already done at startup
    for (DisableTypeMap::iterator itr = m_DisableMap[DISABLE_TYPE_QUEST].begin(); itr != m_DisableMap[DISABLE_TYPE_QUEST].end();)
    {
        const uint32 entry = itr->first;
        if (!sObjectMgr.GetQuestTemplate(entry))
        {
            sLog.outErrorDb("Quest entry %u from `disables` doesn't exist, skipped.", entry);
            m_DisableMap[DISABLE_TYPE_QUEST].erase(itr++);
            continue;
        }
        if (itr->second.flags)
            sLog.outErrorDb("Disable flags specified for quest %u, useless data.", entry);
        ++itr;
    }
}

bool DisableMgr::IsDisabledFor(DisableType type, uint32 entry, Unit* pUnit, uint8 flags)
{
    assert(uint32(type) < MAX_DISABLE_TYPES);
    if (m_DisableMap[type].empty())
        return false;

    DisableTypeMap::iterator itr = m_DisableMap[type].find(entry);
    if (itr == m_DisableMap[type].end())    // not disabled
        return false;

    switch (type)
    {
    case DISABLE_TYPE_SPELL:
        {
            uint8 spellFlags = itr->second.flags;
            if (pUnit)
            {
                if ((spellFlags & SPELL_DISABLE_PLAYER && pUnit->GetTypeId() == TYPEID_PLAYER) ||
                    (pUnit->GetTypeId() == TYPEID_UNIT && (pUnit->IsPet() && spellFlags & (SPELL_DISABLE_PET | SPELL_DISABLE_CREATURE))))
                {
                    if (spellFlags & SPELL_DISABLE_MAP)
                    {
                        std::set<uint32> const& mapIds = itr->second.params[0];
                        if (mapIds.find(pUnit->GetMapId()) != mapIds.end())
                            return true;                                        // Spell is disabled on current map

                        if (!(flags & SPELL_DISABLE_AREA))
                            return false;                                       // Spell is disabled on another map, but not this one, return false

                        // Spell is disabled in an area, but not explicitly our current mapId. Continue processing.
                    }

                    if (spellFlags & SPELL_DISABLE_AREA)
                    {
                        std::set<uint32> const& areaIds = itr->second.params[1];
                        if (areaIds.find(pUnit->GetAreaId()) != areaIds.end())
                            return true;                                        // Spell is disabled in this area
                        return false;                                           // Spell is disabled in another area, but not this one, return false
                    }
                    else
                        return true;                                            // Spell disabled for all maps
                }
                return false;
            }
            else if (spellFlags & SPELL_DISABLE_DEPRECATED_SPELL)    // call not from spellcast
                return true;
            else if (flags & SPELL_DISABLE_LOS)
                return spellFlags & SPELL_DISABLE_LOS;
        }
    case DISABLE_TYPE_MAP:
        if (Player* pPlayer = pUnit->ToPlayer())
        {
            MapEntry const* mapEntry = sMapStore.LookupEntry(entry);
            if (mapEntry->IsDungeon())
            {
                uint8 disabledModes = itr->second.flags;
                uint32 targetDifficulty = pPlayer->GetDifficulty();
                switch (targetDifficulty)
                {
                case DIFFICULTY_NORMAL:
                    return disabledModes & DUNGEON_STATUSFLAG_NORMAL;
                case DIFFICULTY_HEROIC:
                    return disabledModes & DUNGEON_STATUSFLAG_HEROIC;
                }
            }
            else if (mapEntry->map_type == MAP_COMMON)
                return true;
        }
        return false;
    case DISABLE_TYPE_QUEST:
        if (!pUnit)
            return true;
        if (Player const* pPlayer = pUnit->ToPlayer())
            if (pPlayer->IsGameMaster())
                return false;
        return true;
    case DISABLE_TYPE_BATTLEGROUND:
        return true;
    }

    return false;
}
