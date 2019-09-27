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

#include "ScriptSystem.h"
#include "ObjectMgr.h"
#include "Database/DatabaseEnv.h"
#include "ScriptMgr.h"

SystemMgr::SystemMgr()
{
}

SystemMgr& SystemMgr::Instance()
{
    static SystemMgr pSysMgr;
    return pSysMgr;
}

void SystemMgr::LoadVersion()
{
    //Get Version information
    QueryResult_AutoPtr Result = WorldDatabase.Query("SELECT script_version FROM version LIMIT 1");

    if (Result)
    {
        Field* pFields = Result->Fetch();

        outstring_log("OSCR: Database version is: %s\n", pFields[0].GetString());
    }
    else
        error_log("OSCR: Missing version.script_version information.\n");
}

void SystemMgr::LoadScriptTexts()
{
    LoadOregonStrings(WorldDatabase, "script_texts", TEXT_SOURCE_RANGE, 1 + (TEXT_SOURCE_RANGE * 2));

    QueryResult_AutoPtr Result = WorldDatabase.Query("SELECT entry, sound, type, language, emote FROM script_texts");

    if (Result)
    {
        uint32 uiCount = 0;

        do
        {
            Field* pFields = Result->Fetch();
            StringTextData pTemp;

            int32 iId           = pFields[0].GetInt32();
            pTemp.uiSoundId     = pFields[1].GetUInt32();
            pTemp.uiType        = pFields[2].GetUInt32();
            pTemp.uiLanguage    = pFields[3].GetUInt32();
            pTemp.uiEmote       = pFields[4].GetUInt32();

            if (iId >= 0)
            {
                error_db_log("OSCR: Entry %i in table script_texts is not a negative value.", iId);
                continue;
            }

            if (iId > TEXT_SOURCE_RANGE || iId <= TEXT_SOURCE_RANGE * 2)
            {
                error_db_log("OSCR: Entry %i in table script_texts is out of accepted entry range for table.", iId);
                continue;
            }

            if (pTemp.uiSoundId)
            {
                if (!GetSoundEntriesStore()->LookupEntry(pTemp.uiSoundId))
                    error_db_log("OSCR: Entry %i in table script_texts has soundId %u but sound does not exist.", iId, pTemp.uiSoundId);
            }

            if (!GetLanguageDescByID(pTemp.uiLanguage))
                error_db_log("OSCR: Entry %i in table script_texts using Language %u but Language does not exist.", iId, pTemp.uiLanguage);

            if (pTemp.uiType > CHAT_TYPE_ZONE_YELL)
                error_db_log("OSCR: Entry %i in table script_texts has Type %u but this Chat Type does not exist.", iId, pTemp.uiType);

            m_mTextDataMap[iId] = pTemp;
            ++uiCount;
        }
        while (Result->NextRow());

        outstring_log(">> Loaded %u additional Script Texts data.", uiCount);
    }
    else
        outstring_log(">> Loaded 0 additional Script Texts data. DB table script_texts is empty.");
}

void SystemMgr::LoadScriptTextsCustom()
{
    LoadOregonStrings(WorldDatabase, "custom_texts", TEXT_SOURCE_RANGE * 2, 1 + (TEXT_SOURCE_RANGE * 3));

    QueryResult_AutoPtr Result = WorldDatabase.Query("SELECT entry, sound, type, language, emote FROM custom_texts");

    if (Result)
    {
        uint32 uiCount = 0;

        do
        {
            Field* pFields = Result->Fetch();
            StringTextData pTemp;

            int32 iId              = pFields[0].GetInt32();
            pTemp.uiSoundId        = pFields[1].GetUInt32();
            pTemp.uiType           = pFields[2].GetUInt32();
            pTemp.uiLanguage       = pFields[3].GetUInt32();
            pTemp.uiEmote          = pFields[4].GetUInt32();

            if (iId >= 0)
            {
                error_db_log("OSCR: Entry %i in table custom_texts is not a negative value.", iId);
                continue;
            }

            if (iId > TEXT_SOURCE_RANGE * 2 || iId <= TEXT_SOURCE_RANGE * 3)
            {
                error_db_log("OSCR: Entry %i in table custom_texts is out of accepted entry range for table.", iId);
                continue;
            }

            if (pTemp.uiSoundId)
            {
                if (!GetSoundEntriesStore()->LookupEntry(pTemp.uiSoundId))
                    error_db_log("OSCR: Entry %i in table custom_texts has soundId %u but sound does not exist.", iId, pTemp.uiSoundId);
            }

            if (!GetLanguageDescByID(pTemp.uiLanguage))
                error_db_log("OSCR: Entry %i in table custom_texts using Language %u but Language does not exist.", iId, pTemp.uiLanguage);

            if (pTemp.uiType > CHAT_TYPE_ZONE_YELL)
                error_db_log("OSCR: Entry %i in table custom_texts has Type %u but this Chat Type does not exist.", iId, pTemp.uiType);

            m_mTextDataMap[iId] = pTemp;
            ++uiCount;
        }
        while (Result->NextRow());

        outstring_log(">> Loaded %u additional Custom Texts data.", uiCount);
    }
    else
        outstring_log(">> Loaded 0 additional Custom Texts data. DB table custom_texts is empty.");
}

void SystemMgr::LoadScriptWaypoints()
{
    // Drop Existing Waypoint list
    m_mPointMoveMap.clear();

    uint64 uiCreatureCount = 0;

    // Load Waypoints
    QueryResult_AutoPtr Result = WorldDatabase.Query("SELECT COUNT(entry) FROM script_waypoint GROUP BY entry");
    if (Result)
        uiCreatureCount = Result->GetRowCount();

    Result = WorldDatabase.Query("SELECT entry, pointid, location_x, location_y, location_z, waittime FROM script_waypoint ORDER BY pointid");

    if (Result)
    {
        uint32 uiNodeCount = 0;

        do
        {
            Field* pFields = Result->Fetch();
            ScriptPointMove pTemp;

            pTemp.uiCreatureEntry   = pFields[0].GetUInt32();
            uint32 uiEntry          = pTemp.uiCreatureEntry;
            pTemp.uiPointId         = pFields[1].GetUInt32();
            pTemp.fX                = pFields[2].GetFloat();
            pTemp.fY                = pFields[3].GetFloat();
            pTemp.fZ                = pFields[4].GetFloat();
            pTemp.uiWaitTime        = pFields[5].GetUInt32();

            CreatureInfo const* pCInfo = GetCreatureTemplateStore(pTemp.uiCreatureEntry);

            if (!pCInfo)
            {
                error_db_log("OSCR: DB table script_waypoint has waypoint for non-existant creature entry %u", pTemp.uiCreatureEntry);
                continue;
            }

            if (!pCInfo->ScriptID)
                error_db_log("OSCR: DB table script_waypoint has waypoint for creature entry %u, but creature does not have ScriptName defined and then useless.", pTemp.uiCreatureEntry);

            m_mPointMoveMap[uiEntry].push_back(pTemp);
            ++uiNodeCount;
        }
        while (Result->NextRow());

        outstring_log(">> Loaded %u Script Waypoint nodes.", uiNodeCount);
    }
    else
        outstring_log(">> Loaded 0 Script Waypoints. DB table script_waypoint is empty.");
}

