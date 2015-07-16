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

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "Database/SQLStorage.h"
#include "Database/SQLStorageImpl.h"
#include "PoolMgr.h"
#include "Log.h"
#include "MapManager.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "UpdateMask.h"
#include "World.h"
#include "WorldSession.h"
#include "Group.h"
#include "Guild.h"
#include "ArenaTeam.h"
#include "Transports.h"
#include "Language.h"
#include "GameEventMgr.h"
#include "Spell.h"
#include "Chat.h"
#include "AccountMgr.h"
#include "InstanceSaveMgr.h"
#include "SpellAuras.h"
#include "Util.h"
#include "WaypointManager.h"
#include "GossipDef.h"
#include "DisableMgr.h"

INSTANTIATE_SINGLETON_1(ObjectMgr);

ScriptMapMap sQuestEndScripts;
ScriptMapMap sQuestStartScripts;
ScriptMapMap sSpellScripts;
ScriptMapMap sGameObjectScripts;
ScriptMapMap sEventScripts;
ScriptMapMap sGossipScripts;
ScriptMapMap sWaypointScripts;

std::string GetScriptsTableNameByType(ScriptsType type)
{
    std::string res = "";
    switch (type)
    {
    case SCRIPTS_QUEST_END:
        res = "quest_end_scripts";
        break;
    case SCRIPTS_QUEST_START:
        res = "quest_start_scripts";
        break;
    case SCRIPTS_SPELL:
        res = "spell_scripts";
        break;
    case SCRIPTS_GAMEOBJECT:
        res = "gameobject_scripts";
        break;
    case SCRIPTS_EVENT:
        res = "event_scripts";
        break;
    case SCRIPTS_WAYPOINT:
        res = "waypoint_scripts";
        break;
    case SCRIPTS_GOSSIP:
        res = "gossip_scripts";
        break;
    default:
        break;
    }
    return res;
}

ScriptMapMap* GetScriptsMapByType(ScriptsType type)
{
    ScriptMapMap* res = NULL;
    switch (type)
    {
    case SCRIPTS_QUEST_END:
        res = &sQuestEndScripts;
        break;
    case SCRIPTS_QUEST_START:
        res = &sQuestStartScripts;
        break;
    case SCRIPTS_SPELL:
        res = &sSpellScripts;
        break;
    case SCRIPTS_GAMEOBJECT:
        res = &sGameObjectScripts;
        break;
    case SCRIPTS_EVENT:
        res = &sEventScripts;
        break;
    case SCRIPTS_WAYPOINT:
        res = &sWaypointScripts;
        break;
    case SCRIPTS_GOSSIP:
        res = &sGossipScripts;
        break;
    default:
        break;
    }
    return res;
}

std::string GetScriptCommandName(ScriptCommands command)
{
    std::string res = "";
    switch (command)
    {
    case SCRIPT_COMMAND_TALK:
        res = "SCRIPT_COMMAND_TALK";
        break;
    case SCRIPT_COMMAND_EMOTE:
        res = "SCRIPT_COMMAND_EMOTE";
        break;
    case SCRIPT_COMMAND_FIELD_SET:
        res = "SCRIPT_COMMAND_FIELD_SET";
        break;
    case SCRIPT_COMMAND_MOVE_TO:
        res = "SCRIPT_COMMAND_MOVE_TO";
        break;
    case SCRIPT_COMMAND_FLAG_SET:
        res = "SCRIPT_COMMAND_FLAG_SET";
        break;
    case SCRIPT_COMMAND_FLAG_REMOVE:
        res = "SCRIPT_COMMAND_FLAG_REMOVE";
        break;
    case SCRIPT_COMMAND_TELEPORT_TO:
        res = "SCRIPT_COMMAND_TELEPORT_TO";
        break;
    case SCRIPT_COMMAND_QUEST_EXPLORED:
        res = "SCRIPT_COMMAND_QUEST_EXPLORED";
        break;
    case SCRIPT_COMMAND_KILL_CREDIT:
        res = "SCRIPT_COMMAND_KILL_CREDIT";
        break;
    case SCRIPT_COMMAND_RESPAWN_GAMEOBJECT:
        res = "SCRIPT_COMMAND_RESPAWN_GAMEOBJECT";
        break;
    case SCRIPT_COMMAND_TEMP_SUMMON_CREATURE:
        res = "SCRIPT_COMMAND_TEMP_SUMMON_CREATURE";
        break;
    case SCRIPT_COMMAND_OPEN_DOOR:
        res = "SCRIPT_COMMAND_OPEN_DOOR";
        break;
    case SCRIPT_COMMAND_CLOSE_DOOR:
        res = "SCRIPT_COMMAND_CLOSE_DOOR";
        break;
    case SCRIPT_COMMAND_ACTIVATE_OBJECT:
        res = "SCRIPT_COMMAND_ACTIVATE_OBJECT";
        break;
    case SCRIPT_COMMAND_REMOVE_AURA:
        res = "SCRIPT_COMMAND_REMOVE_AURA";
        break;
    case SCRIPT_COMMAND_CAST_SPELL:
        res = "SCRIPT_COMMAND_CAST_SPELL";
        break;
    case SCRIPT_COMMAND_PLAY_SOUND:
        res = "SCRIPT_COMMAND_PLAY_SOUND";
        break;
    case SCRIPT_COMMAND_CREATE_ITEM:
        res = "SCRIPT_COMMAND_CREATE_ITEM";
        break;
    case SCRIPT_COMMAND_DESPAWN_SELF:
        res = "SCRIPT_COMMAND_DESPAWN_SELF";
        break;
    case SCRIPT_COMMAND_DO_NOTHING:
        res = "SCRIPT_COMMAND_DO_NOTHING";
        break;
    case SCRIPT_COMMAND_LOAD_PATH:
        res = "SCRIPT_COMMAND_LOAD_PATH";
        break;
    case SCRIPT_COMMAND_CALLSCRIPT_TO_UNIT:
        res = "SCRIPT_COMMAND_CALLSCRIPT_TO_UNIT";
        break;
    case SCRIPT_COMMAND_KILL:
        res = "SCRIPT_COMMAND_KILL";
        break;
    // Oregon only
    case SCRIPT_COMMAND_ORIENTATION:
        res = "SCRIPT_COMMAND_ORIENTATION";
        break;
    case SCRIPT_COMMAND_EQUIP:
        res = "SCRIPT_COMMAND_EQUIP";
        break;
    case SCRIPT_COMMAND_MODEL:
        res = "SCRIPT_COMMAND_MODEL";
        break;
    case SCRIPT_COMMAND_CLOSE_GOSSIP:
        res = "SCRIPT_COMMAND_CLOSE_GOSSIP";
        break;
    default:
        {
            char sz[32];
            sprintf(sz, "Unknown command: %u", command);
            res = sz;
            break;
        }
    }
    return res;
}

std::string ScriptInfo::GetDebugInfo() const
{
    char sz[256];
    sprintf(sz, "%s ('%s' script id: %u)", GetScriptCommandName(command).c_str(), GetScriptsTableNameByType(type).c_str(), id);
    return std::string(sz);
}

bool normalizePlayerName(std::string& name)
{
    if (name.empty())
        return false;

    wchar_t wstr_buf[MAX_INTERNAL_PLAYER_NAME + 1];
    size_t wstr_len = MAX_INTERNAL_PLAYER_NAME;

    if (!Utf8toWStr(name, &wstr_buf[0], wstr_len))
        return false;

    wstr_buf[0] = wcharToUpper(wstr_buf[0]);
    for (size_t i = 1; i < wstr_len; ++i)
        wstr_buf[i] = wcharToLower(wstr_buf[i]);

    if (!WStrToUtf8(wstr_buf, wstr_len, name))
        return false;

    return true;
}

LanguageDesc lang_description[LANGUAGES_COUNT] =
{
    { LANG_ADDON,           0, 0                       },
    { LANG_UNIVERSAL,       0, 0                       },
    { LANG_ORCISH,        669, SKILL_LANG_ORCISH       },
    { LANG_DARNASSIAN,    671, SKILL_LANG_DARNASSIAN   },
    { LANG_TAURAHE,       670, SKILL_LANG_TAURAHE      },
    { LANG_DWARVISH,      672, SKILL_LANG_DWARVEN      },
    { LANG_COMMON,        668, SKILL_LANG_COMMON       },
    { LANG_DEMONIC,       815, SKILL_LANG_DEMON_TONGUE },
    { LANG_TITAN,         816, SKILL_LANG_TITAN        },
    { LANG_THALASSIAN,    813, SKILL_LANG_THALASSIAN   },
    { LANG_DRACONIC,      814, SKILL_LANG_DRACONIC     },
    { LANG_KALIMAG,       817, SKILL_LANG_OLD_TONGUE   },
    { LANG_GNOMISH,      7340, SKILL_LANG_GNOMISH      },
    { LANG_TROLL,        7341, SKILL_LANG_TROLL        },
    { LANG_GUTTERSPEAK, 17737, SKILL_LANG_GUTTERSPEAK  },
    { LANG_DRAENEI,     29932, SKILL_LANG_DRAENEI      },
    { LANG_ZOMBIE,          0, 0                       },
    { LANG_GNOMISH_BINARY,  0, 0                       },
    { LANG_GOBLIN_BINARY,   0, 0                       }
};

LanguageDesc const* GetLanguageDescByID(uint32 lang)
{
    for (uint8 i = 0; i < LANGUAGES_COUNT; ++i)
    {
        if (uint32(lang_description[i].lang_id) == lang)
            return &lang_description[i];
    }

    return NULL;
}

ObjectMgr::ObjectMgr()
{
    m_hiCharGuid        = 1;
    m_hiCreatureGuid    = 1;
    m_hiPetGuid         = 1;
    m_hiItemGuid        = 1;
    m_hiGoGuid          = 1;
    m_hiDoGuid          = 1;
    m_hiCorpseGuid      = 1;
    m_hiPetNumber       = 1;
    m_ItemTextId        = 1;
    m_mailid            = 1;
    m_guildId           = 1;
    m_arenaTeamId       = 1;
    m_auctionid         = 1;

    mGuildBankTabPrice.resize(GUILD_BANK_MAX_TABS);
    mGuildBankTabPrice[0] = 100;
    mGuildBankTabPrice[1] = 250;
    mGuildBankTabPrice[2] = 500;
    mGuildBankTabPrice[3] = 1000;
    mGuildBankTabPrice[4] = 2500;
    mGuildBankTabPrice[5] = 5000;
}

ObjectMgr::~ObjectMgr()
{
    for (QuestMap::iterator i = mQuestTemplates.begin(); i != mQuestTemplates.end(); ++i)
        delete i->second;
    mQuestTemplates.clear();

    for (GossipTextMap::iterator i = mGossipText.begin(); i != mGossipText.end(); ++i)
        delete i->second;
    mGossipText.clear();

    mAreaTriggers.clear();

    for (PetLevelInfoMap::iterator i = petInfo.begin(); i != petInfo.end(); ++i)
        delete[] i->second;
    petInfo.clear();

    // free only if loaded
    for (int class_ = 0; class_ < MAX_CLASSES; ++class_)
        delete[] playerClassInfo[class_].levelInfo;

    for (int race = 0; race < MAX_RACES; ++race)
        for (int class_ = 0; class_ < MAX_CLASSES; ++class_)
            delete[] playerInfo[race][class_].levelInfo;

    // free group and guild objects
    for (GroupSet::iterator itr = mGroupSet.begin(); itr != mGroupSet.end(); ++itr)
        delete (*itr);

    for (GuildMap::iterator itr = mGuildMap.begin(); itr != mGuildMap.end(); ++itr)
        delete itr->second;
    mGuildMap.clear();

    for (ArenaTeamMap::iterator itr = mArenaTeamMap.begin(); itr != mArenaTeamMap.end(); ++itr)
        delete itr->second;

    for (CacheVendorItemMap::iterator itr = m_mCacheVendorItemMap.begin(); itr != m_mCacheVendorItemMap.end(); ++itr)
        itr->second.Clear();

    for (CacheTrainerSpellMap::iterator itr = m_mCacheTrainerSpellMap.begin(); itr != m_mCacheTrainerSpellMap.end(); ++itr)
        itr->second.Clear();
}

Group* ObjectMgr::GetGroupByLeader(const uint64& guid) const
{
    for (GroupSet::const_iterator itr = mGroupSet.begin(); itr != mGroupSet.end(); ++itr)
        if ((*itr)->GetLeaderGUID() == guid)
            return *itr;

    return NULL;
}

Guild*  ObjectMgr::GetGuildById(const uint32 GuildId) const
{
    GuildMap::const_iterator itr = mGuildMap.find(GuildId);
    if (itr != mGuildMap.end())
        return itr->second;

    return NULL;
}

Guild*  ObjectMgr::GetGuildByName(const std::string& guildname) const
{
    for (GuildMap::const_iterator itr = mGuildMap.begin(); itr != mGuildMap.end(); ++itr)
        if (itr->second->GetName() == guildname)
            return itr->second;

    return NULL;
}

std::string ObjectMgr::GetGuildNameById(const uint32 GuildId) const
{
    GuildMap::const_iterator itr = mGuildMap.find(GuildId);
    if (itr != mGuildMap.end())
        return itr->second->GetName();

    return "";
}

Guild* ObjectMgr::GetGuildByLeader(const uint64& guid) const
{
    for (GuildMap::const_iterator itr = mGuildMap.begin(); itr != mGuildMap.end(); ++itr)
        if (itr->second->GetLeader() == guid)
            return itr->second;

    return NULL;
}

void ObjectMgr::AddGuild(Guild* guild)
{
    mGuildMap[guild->GetId()] = guild;
}

void ObjectMgr::RemoveGuild(uint32 Id)
{
    mGuildMap.erase(Id);
}

ArenaTeam* ObjectMgr::GetArenaTeamById(const uint32 arenateamid) const
{
    ArenaTeamMap::const_iterator itr = mArenaTeamMap.find(arenateamid);
    if (itr != mArenaTeamMap.end())
        return itr->second;

    return NULL;
}

ArenaTeam* ObjectMgr::GetArenaTeamByName(const std::string& arenateamname) const
{
    for (ArenaTeamMap::const_iterator itr = mArenaTeamMap.begin(); itr != mArenaTeamMap.end(); ++itr)
        if (itr->second->GetName() == arenateamname)
            return itr->second;

    return NULL;
}

ArenaTeam* ObjectMgr::GetArenaTeamByCaptain(uint64 const& guid) const
{
    for (ArenaTeamMap::const_iterator itr = mArenaTeamMap.begin(); itr != mArenaTeamMap.end(); ++itr)
        if (itr->second->GetCaptain() == guid)
            return itr->second;

    return NULL;
}

void ObjectMgr::AddArenaTeam(ArenaTeam* arenaTeam)
{
    mArenaTeamMap[arenaTeam->GetId()] = arenaTeam;
}

void ObjectMgr::RemoveArenaTeam(uint32 Id)
{
    mArenaTeamMap.erase(Id);
}

CreatureInfo const* ObjectMgr::GetCreatureTemplate(uint32 id)
{
    return sCreatureStorage.LookupEntry<CreatureInfo>(id);
}

void ObjectMgr::LoadCreatureLocales()
{
    mCreatureLocaleMap.clear();                              // need for reload case

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry,name_loc1,subname_loc1,name_loc2,subname_loc2,name_loc3,subname_loc3,name_loc4,subname_loc4,name_loc5,subname_loc5,name_loc6,subname_loc6,name_loc7,subname_loc7,name_loc8,subname_loc8 FROM locales_creature");

    if (!result)
    {


        sLog.outString(">> Loaded 0 creature locale strings. DB table locales_creature is empty.");
        return;
    }


    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        CreatureLocale& data = mCreatureLocaleMap[entry];

        for (uint8 i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[1 + 2 * (i - 1)].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if (data.Name.size() <= idx)
                        data.Name.resize(idx + 1);

                    data.Name[idx] = str;
                }
            }
            str = fields[1 + 2 * (i - 1) + 1].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if (data.SubName.size() <= idx)
                        data.SubName.resize(idx + 1);

                    data.SubName[idx] = str;
                }
            }
        }
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %lu creature locale strings", mCreatureLocaleMap.size());
}

void ObjectMgr::LoadGossipMenuItemsLocales()
{
    mGossipMenuItemsLocaleMap.clear();                      // need for reload case

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT menu_id,id,"
                                 "option_text_loc1,box_text_loc1,option_text_loc2,box_text_loc2,"
                                 "option_text_loc3,box_text_loc3,option_text_loc4,box_text_loc4,"
                                 "option_text_loc5,box_text_loc5,option_text_loc6,box_text_loc6,"
                                 "option_text_loc7,box_text_loc7,option_text_loc8,box_text_loc8 "
                                 "FROM locales_gossip_menu_option");

    if (!result)
    {


        sLog.outString(">> Loaded 0 gossip_menu_option locale strings. DB table `locales_gossip_menu_option` is empty.");
        return;
    }


    do
    {
        Field* fields = result->Fetch();

        uint16 menuId   = fields[0].GetUInt16();
        uint16 id       = fields[1].GetUInt16();

        GossipMenuItemsLocale& data = mGossipMenuItemsLocaleMap[MAKE_PAIR32(menuId, id)];

        for (uint8 i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[1 + 2 * (i - 1)].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if (data.OptionText.size() <= idx)
                        data.OptionText.resize(idx + 1);

                    data.OptionText[idx] = str;
                }
            }
            str = fields[1 + 2 * (i - 1) + 1].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if (data.BoxText.size() <= idx)
                        data.BoxText.resize(idx + 1);

                    data.BoxText[idx] = str;
                }
            }
        }
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u gossip_menu_option locale strings", mGossipMenuItemsLocaleMap.size());
}

struct SQLCreatureLoader : public SQLStorageLoaderBase<SQLCreatureLoader>
{
    template<class D>
    void convert_from_str(uint32 field_pos, char* src, D& dst)
    {
        dst = D(sObjectMgr.GetScriptId(src));
    }
};

void ObjectMgr::LoadCreatureTemplates()
{
    SQLCreatureLoader loader;
    loader.Load(sCreatureStorage);

    sLog.outString(">> Loaded %u creature definitions", sCreatureStorage.RecordCount);

    std::set<uint32> heroicEntries;                         // already loaded heroic value in creatures
    std::set<uint32> hasHeroicEntries;                      // already loaded creatures with heroic entry values

    // check data correctness
    for (uint32 i = 1; i < sCreatureStorage.MaxEntry; ++i)
    {
        CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(i);
        if (!cInfo)
            continue;

        if (cInfo->HeroicEntry)
        {
            CreatureInfo const* heroicInfo = GetCreatureTemplate(cInfo->HeroicEntry);
            if (!heroicInfo)
            {
                sLog.outErrorDb("Creature (Entry: %u) has heroic_entry=%u but creature entry %u does not exist.", cInfo->Entry, cInfo->HeroicEntry, cInfo->HeroicEntry);
                continue;
            }

            if (heroicEntries.find(i) != heroicEntries.end())
            {
                sLog.outErrorDb("Creature (Entry: %u) listed as heroic but has value in heroic_entry.", i);
                continue;
            }

            if (heroicEntries.find(cInfo->HeroicEntry) != heroicEntries.end())
            {
                sLog.outErrorDb("Creature (Entry: %u) already listed as heroic for another entry.", cInfo->HeroicEntry);
                continue;
            }

            if (hasHeroicEntries.find(cInfo->HeroicEntry) != hasHeroicEntries.end())
            {
                sLog.outErrorDb("Creature (Entry: %u) has heroic_entry=%u but creature entry %u also has heroic entry.", i, cInfo->HeroicEntry, cInfo->HeroicEntry);
                continue;
            }

            if (cInfo->npcflag != heroicInfo->npcflag)
            {
                sLog.outErrorDb("Creature (Entry: %u) listed in creature_template_substitution has different npcflag in heroic mode.", i);
                continue;
            }

            if (cInfo->classNum != heroicInfo->classNum)
            {
                sLog.outErrorDb("Creature (Entry: %u) listed in creature_template_substitution has different classNum in heroic mode.", i);
                continue;
            }

            if (cInfo->race != heroicInfo->race)
            {
                sLog.outErrorDb("Creature (Entry: %u) listed in creature_template_substitution has different race in heroic mode.", i);
                continue;
            }

            if (cInfo->trainer_type != heroicInfo->trainer_type)
            {
                sLog.outErrorDb("Creature (Entry: %u) listed in creature_template_substitution has different trainer_type in heroic mode.", i);
                continue;
            }

            if (cInfo->trainer_spell != heroicInfo->trainer_spell)
            {
                sLog.outErrorDb("Creature (Entry: %u) listed in creature_template_substitution has different trainer_spell in heroic mode.", i);
                continue;
            }

            hasHeroicEntries.insert(i);
            heroicEntries.insert(cInfo->HeroicEntry);
        }

        FactionTemplateEntry const* factionTemplate = sFactionTemplateStore.LookupEntry(cInfo->faction_A);
        if (!factionTemplate)
            sLog.outErrorDb("Creature (Entry: %u) has invalid faction_A template (%u)", cInfo->Entry, cInfo->faction_A);

        factionTemplate = sFactionTemplateStore.LookupEntry(cInfo->faction_H);
        if (!factionTemplate)
            sLog.outErrorDb("Creature (Entry: %u) has invalid faction_H template (%u)", cInfo->Entry, cInfo->faction_H);

        // check model ids, supplying and sending non-existent ids to the client might crash them
        if (cInfo->Modelid_A1 && !sCreatureModelStorage.LookupEntry<CreatureModelInfo>(cInfo->Modelid_A1))
        {
            sLog.outErrorDb("Creature (Entry: %u) has invalid Modelid_A1 (%u), setting it to 0", cInfo->Entry, cInfo->Modelid_A1);
            const_cast<CreatureInfo*>(cInfo)->Modelid_A1 = 0;
        }
        if (cInfo->Modelid_A2 && !sCreatureModelStorage.LookupEntry<CreatureModelInfo>(cInfo->Modelid_A2))
        {
            sLog.outErrorDb("Creature (Entry: %u) has invalid Modelid_A2 (%u), setting it to 0", cInfo->Entry, cInfo->Modelid_A2);
            const_cast<CreatureInfo*>(cInfo)->Modelid_A2 = 0;
        }
        if (cInfo->Modelid_H1 && !sCreatureModelStorage.LookupEntry<CreatureModelInfo>(cInfo->Modelid_H1))
        {
            sLog.outErrorDb("Creature (Entry: %u) has invalid Modelid_H1 (%u), setting it to 0", cInfo->Entry, cInfo->Modelid_H1);
            const_cast<CreatureInfo*>(cInfo)->Modelid_H1 = 0;
        }
        if (cInfo->Modelid_H2 && !sCreatureModelStorage.LookupEntry<CreatureModelInfo>(cInfo->Modelid_H2))
        {
            sLog.outErrorDb("Creature (Entry: %u) has invalid Modelid_H2 (%u), setting it to 0", cInfo->Entry, cInfo->Modelid_H2);
            const_cast<CreatureInfo*>(cInfo)->Modelid_H2 = 0;
        }

        for (int k = 0; k < MAX_KILL_CREDIT; ++k)
        {
            if (cInfo->KillCredit[k])
            {
                if (!GetCreatureTemplate(cInfo->KillCredit[k]))
                {
                    sLog.outErrorDb("Creature (Entry: %u) has not existed creature entry in `KillCredit%d` (%u)", cInfo->Entry, k + 1, cInfo->KillCredit[k]);
                    const_cast<CreatureInfo*>(cInfo)->KillCredit[k] = 0;
                }
            }
        }

        if (cInfo->dmgschool >= MAX_SPELL_SCHOOL)
        {
            sLog.outErrorDb("Creature (Entry: %u) has invalid spell school value (%u) in dmgschool", cInfo->Entry, cInfo->dmgschool);
            const_cast<CreatureInfo*>(cInfo)->dmgschool = SPELL_SCHOOL_NORMAL;
        }

        if (cInfo->baseattacktime == 0)
            const_cast<CreatureInfo*>(cInfo)->baseattacktime  = BASE_ATTACK_TIME;

        if (cInfo->rangeattacktime == 0)
            const_cast<CreatureInfo*>(cInfo)->rangeattacktime = BASE_ATTACK_TIME;

        if ((cInfo->npcflag & UNIT_NPC_FLAG_TRAINER) && cInfo->trainer_type >= MAX_TRAINER_TYPE)
            sLog.outErrorDb("Creature (Entry: %u) has wrong trainer type %u", cInfo->Entry, cInfo->trainer_type);

        if (cInfo->InhabitType <= 0 || cInfo->InhabitType > INHABIT_ANYWHERE)
        {
            sLog.outErrorDb("Creature (Entry: %u) has wrong value (%u) in InhabitType, creature will not correctly walk/swim/fly", cInfo->Entry, cInfo->InhabitType);
            const_cast<CreatureInfo*>(cInfo)->InhabitType = INHABIT_ANYWHERE;
        }

        if (cInfo->PetSpellDataId)
        {
            CreatureSpellDataEntry const* spellDataId = sCreatureSpellDataStore.LookupEntry(cInfo->PetSpellDataId);
            if (!spellDataId)
                sLog.outErrorDb("Creature (Entry: %u) has invalid PetSpellDataId (%u)", cInfo->Entry, cInfo->PetSpellDataId);
        }

        for (int i = 0; i < CREATURE_MAX_SPELLS; ++i)
        {
            if (cInfo->spells[i] && !sSpellStore.LookupEntry(cInfo->spells[i]))
            {
                sLog.outErrorDb("Creature (Entry: %u) has non-existing Spell%d (%u), set to 0", cInfo->Entry, i + 1, cInfo->spells[i]);
                const_cast<CreatureInfo*>(cInfo)->spells[i] = 0;
            }
        }

        if (cInfo->MovementType >= MAX_DB_MOTION_TYPE)
        {
            sLog.outErrorDb("Creature (Entry: %u) has wrong movement generator type (%u), ignore and set to IDLE.", cInfo->Entry, cInfo->MovementType);
            const_cast<CreatureInfo*>(cInfo)->MovementType = IDLE_MOTION_TYPE;
        }

        if (cInfo->equipmentId > 0)                          // 0 no equipment
        {
            if (!GetEquipmentInfo(cInfo->equipmentId) && !GetEquipmentInfoRaw(cInfo->equipmentId))
            {
                sLog.outErrorDb("Table creature_template has creature (Entry: %u) with equipment_id %u not found in table creature_equip_template, set to no equipment.", cInfo->Entry, cInfo->equipmentId);
                const_cast<CreatureInfo*>(cInfo)->equipmentId = 0;
            }
        }

        // if not set custom creature scale then load scale from CreatureDisplayInfo.dbc
        if (cInfo->scale <= 0.0f)
        {
            uint32 modelid = cInfo->GetFirstValidModelId();
            CreatureDisplayInfoEntry const* ScaleEntry = sCreatureDisplayInfoStore.LookupEntry(modelid);
            const_cast<CreatureInfo*>(cInfo)->scale = ScaleEntry ? ScaleEntry->scale : 1.0f;
        }
    }
}

void ObjectMgr::ConvertCreatureAddonAuras(CreatureDataAddon* addon, char const* table, char const* guidEntryStr)
{
    // Now add the auras, format "spellid effectindex spellid effectindex..."
    char* p, *s;
    std::vector<int> val;
    s = p = (char*)reinterpret_cast<char const*>(addon->auras);
    if (p)
    {
        while (p[0] != 0)
        {
            ++p;
            if (p[0] == ' ')
            {
                val.push_back(atoi(s));
                s = ++p;
            }
        }
        if (p != s)
            val.push_back(atoi(s));

        // free char* loaded memory
        delete[] (char*)reinterpret_cast<char const*>(addon->auras);

        // wrong list
        if (val.size() % 2)
        {
            addon->auras = NULL;
            sLog.outErrorDb("Creature (%s: %u) has wrong auras data in %s.", guidEntryStr, addon->guidOrEntry, table);
            return;
        }
    }

    // empty list
    if (val.empty())
    {
        addon->auras = NULL;
        return;
    }

    // replace by new structures array
    const_cast<CreatureDataAddonAura*&>(addon->auras) = new CreatureDataAddonAura[val.size() / 2 + 1];

    uint32 i = 0;
    for (size_t j = 0; j < val.size() / 2; ++j)
    {
        CreatureDataAddonAura& cAura = const_cast<CreatureDataAddonAura&>(addon->auras[i]);
        cAura.spell_id = (uint32)val[2 * j + 0];
        cAura.effect_idx  = (uint32)val[2 * j + 1];
        if (cAura.effect_idx > 2)
        {
            sLog.outErrorDb("Creature (%s: %u) has wrong effect %u for spell %u in auras field in %s.", guidEntryStr, addon->guidOrEntry, cAura.effect_idx, cAura.spell_id, table);
            continue;
        }
        SpellEntry const* AdditionalSpellInfo = sSpellStore.LookupEntry(cAura.spell_id);
        if (!AdditionalSpellInfo)
        {
            sLog.outErrorDb("Creature (%s: %u) has wrong spell %u defined in auras field in %s.", guidEntryStr, addon->guidOrEntry, cAura.spell_id, table);
            continue;
        }

        if (!AdditionalSpellInfo->Effect[cAura.effect_idx] || !AdditionalSpellInfo->EffectApplyAuraName[cAura.effect_idx])
        {
            sLog.outErrorDb("Creature (%s: %u) has not aura effect %u of spell %u defined in auras field in %s.", guidEntryStr, addon->guidOrEntry, cAura.effect_idx, cAura.spell_id, table);
            continue;
        }

        ++i;
    }

    // fill terminator element (after last added)
    CreatureDataAddonAura& endAura = const_cast<CreatureDataAddonAura&>(addon->auras[i]);
    endAura.spell_id   = 0;
    endAura.effect_idx = 0;
}

void ObjectMgr::LoadCreatureAddons()
{
    sCreatureInfoAddonStorage.Load();

    sLog.outString(">> Loaded %u creature template addons", sCreatureInfoAddonStorage.RecordCount);

    // check data correctness and convert 'auras'
    for (uint32 i = 1; i < sCreatureInfoAddonStorage.MaxEntry; ++i)
    {
        CreatureDataAddon const* addon = sCreatureInfoAddonStorage.LookupEntry<CreatureDataAddon>(i);
        if (!addon)
            continue;

        if (!sEmotesStore.LookupEntry(addon->emote))
            sLog.outErrorDb("Creature (GUID: %u) has invalid emote (%u) defined in creature_template_addon.", addon->guidOrEntry, addon->emote);

        ConvertCreatureAddonAuras(const_cast<CreatureDataAddon*>(addon), "creature_template_addon", "Entry");

        if (!sCreatureStorage.LookupEntry<CreatureInfo>(addon->guidOrEntry))
            sLog.outErrorDb("Creature (Entry: %u) does not exist but has a record in creature_template_addon", addon->guidOrEntry);
    }

    sCreatureDataAddonStorage.Load();

    sLog.outString(">> Loaded %u creature addons", sCreatureDataAddonStorage.RecordCount);

    // check data correctness and convert 'auras'
    for (uint32 i = 1; i < sCreatureDataAddonStorage.MaxEntry; ++i)
    {
        CreatureDataAddon const* addon = sCreatureDataAddonStorage.LookupEntry<CreatureDataAddon>(i);
        if (!addon)
            continue;

        if (!sEmotesStore.LookupEntry(addon->emote))
            sLog.outErrorDb("Creature (GUID: %u) has invalid emote (%u) defined in creature_addon.", addon->guidOrEntry, addon->emote);

        ConvertCreatureAddonAuras(const_cast<CreatureDataAddon*>(addon), "creature_addon", "GUIDLow");

        if (mCreatureDataMap.find(addon->guidOrEntry) == mCreatureDataMap.end())
            sLog.outErrorDb("Creature (GUID: %u) does not exist but has a record in creature_addon", addon->guidOrEntry);
    }
}

EquipmentInfo const* ObjectMgr::GetEquipmentInfo(uint32 entry)
{
    return sEquipmentStorage.LookupEntry<EquipmentInfo>(entry);
}

EquipmentInfoRaw const* ObjectMgr::GetEquipmentInfoRaw(uint32 entry)
{
    return sEquipmentStorageRaw.LookupEntry<EquipmentInfoRaw>(entry);
}

void ObjectMgr::LoadEquipmentTemplates()
{
    sEquipmentStorage.Load();

    for (uint32 i = 0; i < sEquipmentStorage.MaxEntry; ++i)
    {
        EquipmentInfo const* eqInfo = sEquipmentStorage.LookupEntry<EquipmentInfo>(i);

        if (!eqInfo)
            continue;

        for (uint8 j = 0; j < 3; ++j)
        {
            if (!eqInfo->equipentry[j])
                continue;

            ItemPrototype const* itemProto = GetItemPrototype(eqInfo->equipentry[j]);
            if (!itemProto)
            {
                sLog.outErrorDb("Unknown item (entry=%u) in creature_equip_template.equipentry%u for entry = %u, forced to 0.", eqInfo->equipentry[j], j + 1, i);
                const_cast<EquipmentInfo*>(eqInfo)->equipentry[j] = 0;
                continue;
            }

            if (itemProto->InventoryType != INVTYPE_WEAPON &&
                itemProto->InventoryType != INVTYPE_SHIELD &&
                itemProto->InventoryType != INVTYPE_RANGED &&
                itemProto->InventoryType != INVTYPE_2HWEAPON &&
                itemProto->InventoryType != INVTYPE_WEAPONMAINHAND &&
                itemProto->InventoryType != INVTYPE_WEAPONOFFHAND &&
                itemProto->InventoryType != INVTYPE_HOLDABLE &&
                itemProto->InventoryType != INVTYPE_THROWN &&
                itemProto->InventoryType != INVTYPE_RANGEDRIGHT &&
                itemProto->InventoryType != INVTYPE_RELIC)
            {
                sLog.outErrorDb("Item (entry=%u) in creature_equip_template.equipentry%u for entry = %u is not equipable in a hand, forced to 0.", eqInfo->equipentry[j], j + 1, i);
                const_cast<EquipmentInfo*>(eqInfo)->equipentry[j] = 0;
            }
        }
    }

    sLog.outString(">> Loaded %u equipment template", sEquipmentStorage.RecordCount);

    sEquipmentStorageRaw.Load();
    for (uint32 i = 1; i < sEquipmentStorageRaw.MaxEntry; ++i)
        if (sEquipmentStorageRaw.LookupEntry<EquipmentInfoRaw>(i))
            if (sEquipmentStorage.LookupEntry<EquipmentInfo>(i))
                sLog.outErrorDb("Table 'creature_equip_template_raw` have redundant data for ID %u ('creature_equip_template` already have data)", i);

    sLog.outString( ">> Loaded %u equipment template (deprecated format)", sEquipmentStorageRaw.RecordCount );
}

CreatureModelInfo const* ObjectMgr::GetCreatureModelInfo(uint32 modelid)
{
    return sCreatureModelStorage.LookupEntry<CreatureModelInfo>(modelid);
}

uint32 ObjectMgr::ChooseDisplayId(uint32 /*team*/, const CreatureInfo* cinfo, const CreatureData* data)
{
    // Load creature model (display id)
    uint32 display_id = 0;

    if (!data || data->displayid == 0)
        display_id = cinfo->GetRandomValidModelId();
    else
        display_id = data->displayid;

    return display_id;
}

CreatureModelInfo const* ObjectMgr::GetCreatureModelRandomGender(uint32 display_id)
{
    CreatureModelInfo const* minfo = GetCreatureModelInfo(display_id);
    if (!minfo)
        return NULL;

    // If a model for another gender exists, 50% chance to use it
    if (minfo->modelid_other_gender != 0 && urand(0, 1) == 0)
    {
        CreatureModelInfo const* minfo_tmp = GetCreatureModelInfo(minfo->modelid_other_gender);
        if (!minfo_tmp)
        {
            sLog.outErrorDb("Model (Entry: %u) has modelid_other_gender %u not found in table creature_model_info. ", minfo->modelid, minfo->modelid_other_gender);
            return minfo;                                   // not fatal, just use the previous one
        }
        else
            return minfo_tmp;
    }
    else
        return minfo;
}

void ObjectMgr::LoadCreatureModelInfo()
{
    sCreatureModelStorage.Load();

    sLog.outString(">> Loaded %u creature model based info", sCreatureModelStorage.RecordCount);

    // check if combat_reach is valid
    for (uint32 i = 1; i < sCreatureModelStorage.MaxEntry; ++i)
    {
        CreatureModelInfo const* mInfo = sCreatureModelStorage.LookupEntry<CreatureModelInfo>(i);
        if (!mInfo)
            continue;

        if (mInfo->combat_reach < 0.1f)
        {
            //sLog.outErrorDb("Creature model (Entry: %u) has invalid combat reach (%f), setting it to 0.5", mInfo->modelid, mInfo->combat_reach);
            const_cast<CreatureModelInfo*>(mInfo)->combat_reach = DEFAULT_COMBAT_REACH;
        }
    }
}

bool ObjectMgr::CheckCreatureLinkedRespawn(uint32 guid, uint32 linkedGuid) const
{
    const CreatureData* const slave = GetCreatureData(guid);
    const CreatureData* const master = GetCreatureData(linkedGuid);

    if (!slave || !master) // they must have a corresponding entry in db
    {
        sLog.outError("LinkedRespawn: Creature '%u' linking to '%u' which doesn't exist", guid, linkedGuid);
        return false;
    }

    const MapEntry* const map = sMapStore.LookupEntry(master->mapid);

    if (master->mapid != slave->mapid        // link only to same map
        && (!map || map->Instanceable()))   // or to unistanced world
    {
        sLog.outError("LinkedRespawn: Creature '%u' linking to '%u' on an unpermitted map", guid, linkedGuid);
        return false;
    }

    if (!(master->spawnMask & slave->spawnMask)  // they must have a possibility to meet (normal/heroic difficulty)
        && (!map || map->Instanceable()))
    {
        sLog.outError("LinkedRespawn: Creature '%u' linking to '%u' with not corresponding spawnMask", guid, linkedGuid);
        return false;
    }

    return true;
}

void ObjectMgr::LoadCreatureLinkedRespawn()
{
    mCreatureLinkedRespawnMap.clear();
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT guid, linkedGuid FROM creature_linked_respawn ORDER BY guid ASC");

    if (!result)
    {


        sLog.outErrorDb(">> Loaded 0 linked respawns. DB table creature_linked_respawn is empty.");
        return;
    }


    do
    {
        Field* fields = result->Fetch();

        uint32 guid = fields[0].GetUInt32();
        uint32 linkedGuid = fields[1].GetUInt32();

        if (CheckCreatureLinkedRespawn(guid, linkedGuid))
            mCreatureLinkedRespawnMap[guid] = linkedGuid;

    }
    while (result->NextRow());

    sLog.outString(">> Loaded %lu linked respawns", mCreatureLinkedRespawnMap.size());
}

bool ObjectMgr::SetCreatureLinkedRespawn(uint32 guid, uint32 linkedGuid)
{
    if (!guid)
        return false;

    if (!linkedGuid) // we're removing the linking
    {
        mCreatureLinkedRespawnMap.erase(guid);
        WorldDatabase.DirectPExecute("DELETE FROM creature_linked_respawn WHERE guid = '%u'", guid);
        return true;
    }

    if (CheckCreatureLinkedRespawn(guid, linkedGuid)) // we add/change linking
    {
        mCreatureLinkedRespawnMap[guid] = linkedGuid;
        WorldDatabase.DirectPExecute("REPLACE INTO creature_linked_respawn(guid,linkedGuid) VALUES ('%u','%u')", guid, linkedGuid);
        return true;
    }
    return false;
}

void ObjectMgr::LoadCreatures()
{
    uint32 count = 0;
    //                                                       0              1   2    3
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT creature.guid, id, map, modelid,"
                                 //4             5           6           7           8            9              10         11
                                 "equipment_id, position_x, position_y, position_z, orientation, spawntimesecs, spawndist, currentwaypoint,"
                                 //12        13       14            15         16     17
                                 "curhealth, curmana, MovementType, spawnMask, event, pool_entry "
                                 "FROM creature LEFT OUTER JOIN game_event_creature ON creature.guid = game_event_creature.guid "
                                 "LEFT OUTER JOIN pool_creature ON creature.guid = pool_creature.guid");

    if (!result)
    {


        sLog.outErrorDb(">> Loaded 0 creature. DB table creature is empty.");
        return;
    }

    // build single time for check creature data
    std::set<uint32> heroicCreatures;
    for (uint32 i = 0; i < sCreatureStorage.MaxEntry; ++i)
        if (CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(i))
            if (cInfo->HeroicEntry)
                heroicCreatures.insert(cInfo->HeroicEntry);


    do
    {
        Field* fields = result->Fetch();

        uint32 guid         = fields[ 0].GetUInt32();
        uint32 entry        = fields[ 1].GetUInt32();

        CreatureInfo const* cInfo = GetCreatureTemplate(entry);
        if (!cInfo)
        {
            sLog.outErrorDb("Table `creature` has creature (GUID: %u) with invalid creature entry %u, skipped.", guid, entry);
            continue;
        }

        CreatureData& data = mCreatureDataMap[guid];

        data.id             = entry;
        data.mapid          = fields[ 2].GetUInt32();
        data.displayid      = fields[ 3].GetUInt32();
        data.equipmentId    = fields[ 4].GetUInt32();
        data.posX           = fields[ 5].GetFloat();
        data.posY           = fields[ 6].GetFloat();
        data.posZ           = fields[ 7].GetFloat();
        data.orientation    = fields[ 8].GetFloat();
        data.spawntimesecs  = fields[ 9].GetUInt32();
        data.spawndist      = fields[10].GetFloat();
        data.currentwaypoint = fields[11].GetUInt32();
        data.curhealth      = fields[12].GetUInt32();
        data.curmana        = fields[13].GetUInt32();
        data.movementType   = fields[14].GetUInt8();
        data.spawnMask      = fields[15].GetUInt8();
        int16 gameEvent     = fields[16].GetInt16();
        int32 PoolId        = fields[17].GetInt32();

        if (heroicCreatures.find(data.id) != heroicCreatures.end())
        {
            sLog.outErrorDb("Table creature has creature (GUID: %u, Entry %u) that is listed as heroic template in creature_template_substitution, skipped.", guid, data.id);
            continue;
        }

        if (data.equipmentId > 0)                            // -1 no equipment, 0 use default
        {
            if (!GetEquipmentInfo(data.equipmentId) && !GetEquipmentInfoRaw(data.equipmentId))
            {
                sLog.outErrorDb("Table creature has creature (Entry: %u) with equipment_id %u not found in table creature_equip_template or creature_equip_template, set to no equipment.", data.id, data.equipmentId);
                data.equipmentId = -1;
            }
        }

        if (cInfo->RegenHealth && data.curhealth < cInfo->minhealth)
        {
            sLog.outErrorDb("Table creature has creature (GUID: %u Entry: %u) with creature_template.RegenHealth=1 and low current health (%u), creature_template.minhealth=%u.", guid, data.id, data.curhealth, cInfo->minhealth);
            data.curhealth = cInfo->minhealth;
        }

        if (data.curmana < cInfo->minmana)
        {
            sLog.outErrorDb("Table creature has creature (GUID: %u Entry: %u) with low current mana (%u), creature_template.minmana=%u.", guid, data.id, data.curmana, cInfo->minmana);
            data.curmana = cInfo->minmana;
        }

        if (data.spawndist < 0.0f)
        {
            sLog.outErrorDb("Table creature has creature (GUID: %u Entry: %u) with spawndist< 0, set to 0.", guid, data.id);
            data.spawndist = 0.0f;
        }
        else if (data.movementType == RANDOM_MOTION_TYPE)
        {
            if (data.spawndist == 0.0f)
            {
                sLog.outErrorDb("Table creature has creature (GUID: %u Entry: %u) with MovementType=1 (random movement) but with spawndist=0, replaced by idle movement type (0).", guid, data.id);
                data.movementType = IDLE_MOTION_TYPE;
            }
        }
        else if (data.movementType == IDLE_MOTION_TYPE)
        {
            if (data.spawndist != 0.0f)
            {
                sLog.outErrorDb("Table creature has creature (GUID: %u Entry: %u) with MovementType=0 (idle) has spawndist<>0, set to 0.", guid, data.id);
                data.spawndist = 0.0f;
            }
        }

        if (gameEvent == 0 && PoolId == 0)                      // if not this is to be managed by GameEvent System or Pool system
            AddCreatureToGrid(guid, &data);

        ++count;

    }
    while (result->NextRow());

    sLog.outString(">> Loaded %lu creatures", mCreatureDataMap.size());
}

void ObjectMgr::AddCreatureToGrid(uint32 guid, CreatureData const* data)
{
    uint8 mask = data->spawnMask;
    for (uint8 i = 0; mask != 0; i++, mask >>= 1)
    {
        if (mask & 1)
        {
            CellPair cell_pair = Oregon::ComputeCellPair(data->posX, data->posY);
            uint32 cell_id = (cell_pair.y_coord * TOTAL_NUMBER_OF_CELLS_PER_MAP) + cell_pair.x_coord;

            CellObjectGuids& cell_guids = mMapObjectGuids[MAKE_PAIR32(data->mapid, i)][cell_id];
            cell_guids.creatures.insert(guid);
        }
    }
}

void ObjectMgr::RemoveCreatureFromGrid(uint32 guid, CreatureData const* data)
{
    uint8 mask = data->spawnMask;
    for (uint8 i = 0; mask != 0; i++, mask >>= 1)
    {
        if (mask & 1)
        {
            CellPair cell_pair = Oregon::ComputeCellPair(data->posX, data->posY);
            uint32 cell_id = (cell_pair.y_coord * TOTAL_NUMBER_OF_CELLS_PER_MAP) + cell_pair.x_coord;

            CellObjectGuids& cell_guids = mMapObjectGuids[MAKE_PAIR32(data->mapid, i)][cell_id];
            cell_guids.creatures.erase(guid);
        }
    }
}

uint32 ObjectMgr::AddGOData(uint32 entry, uint32 artKit, uint32 mapId, float x, float y, float z, float o, uint32 spawntimedelay, float rotation0, float rotation1, float rotation2, float rotation3)
{
    GameObjectInfo const* goinfo = GetGameObjectInfo(entry);
    if (!goinfo)
        return 0;

    Map* map = const_cast<Map*>(MapManager::Instance().CreateBaseMap(mapId));
    if (!map)
        return 0;

    uint32 guid = GenerateLowGuid(HIGHGUID_GAMEOBJECT);
    GameObjectData& data = NewGOData(guid);
    data.id             = entry;
    data.mapid          = mapId;
    data.posX           = x;
    data.posY           = y;
    data.posZ           = z;
    data.orientation    = o;
    data.rotation0      = rotation0;
    data.rotation1      = rotation1;
    data.rotation2      = rotation2;
    data.rotation3      = rotation3;
    data.spawntimesecs  = spawntimedelay;
    data.animprogress   = 100;
    data.spawnMask      = 1;
    data.go_state       = GO_STATE_READY;
    data.artKit         = artKit;
    data.dbData = false;

    AddGameobjectToGrid(guid, &data);

    // Spawn if necessary (loaded grids only)
    // We use spawn coords to spawn
    if (!map->Instanceable() && map->IsLoaded(x, y))
    {
        GameObject* go = new GameObject;
        if (!go->LoadFromDB(guid, map))
        {
            sLog.outError("AddGOData: cannot add gameobject entry %u to map", entry);
            delete go;
            return 0;
        }
        map->AddToMap(go);
    }

    return guid;
}

uint32 ObjectMgr::AddCreData(uint32 entry, uint32 /*team*/, uint32 mapId, float x, float y, float z, float o, uint32 spawntimedelay)
{
    CreatureInfo const* cInfo = GetCreatureTemplate(entry);
    if (!cInfo)
        return 0;

    uint32 guid = GenerateLowGuid(HIGHGUID_UNIT);
    CreatureData& data = NewOrExistCreatureData(guid);
    data.id = entry;
    data.mapid = mapId;
    data.displayid = 0;
    data.equipmentId = cInfo->equipmentId;
    data.posX = x;
    data.posY = y;
    data.posZ = z;
    data.orientation = o;
    data.spawntimesecs = spawntimedelay;
    data.spawndist = 0;
    data.currentwaypoint = 0;
    data.curhealth = cInfo->maxhealth;
    data.curmana = cInfo->maxmana;
    data.movementType = cInfo->MovementType;
    data.spawnMask = 1;
    data.dbData = false;

    AddCreatureToGrid(guid, &data);

    // Spawn if necessary (loaded grids only)
    if (Map* map = const_cast<Map*>(MapManager::Instance().CreateBaseMap(mapId)))
    {
        // We use spawn coords to spawn
        if (!map->Instanceable() && !map->IsRemovalGrid(x, y))
        {
            Creature* creature = new Creature;
            if (!creature->LoadFromDB(guid, map))
            {
                sLog.outError("AddCreature: cannot add creature entry %u to map", entry);
                delete creature;
                return 0;
            }
            map->AddToMap(creature);
        }
    }

    return guid;
}

void ObjectMgr::LoadGameobjects()
{
    uint32 count = 0;

    //                                                       0                1   2    3           4           5           6
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT gameobject.guid, id, map, position_x, position_y, position_z, orientation,"
                                 //   7          8          9          10         11             12            13     14         15     16
                                 "rotation0, rotation1, rotation2, rotation3, spawntimesecs, animprogress, state, spawnMask, event, pool_entry "
                                 "FROM gameobject LEFT OUTER JOIN game_event_gameobject ON gameobject.guid = game_event_gameobject.guid "
                                 "LEFT OUTER JOIN pool_gameobject ON gameobject.guid = pool_gameobject.guid");

    if (!result)
    {


        sLog.outErrorDb(">> Loaded 0 gameobjects. DB table gameobject is empty.");
        return;
    }


    do
    {
        Field* fields = result->Fetch();

        uint32 guid         = fields[ 0].GetUInt32();
        uint32 entry        = fields[ 1].GetUInt32();

        GameObjectInfo const* gInfo = GetGameObjectInfo(entry);
        if (!gInfo)
        {
            sLog.outErrorDb("Table `gameobject` has gameobject (GUID: %u) with invalid gameobject entry %u, skipped.", guid, entry);
            continue;
        }

        GameObjectData& data = mGameObjectDataMap[guid];

        data.id             = entry;
        data.mapid          = fields[ 2].GetUInt32();
        data.posX           = fields[ 3].GetFloat();
        data.posY           = fields[ 4].GetFloat();
        data.posZ           = fields[ 5].GetFloat();
        data.orientation    = fields[ 6].GetFloat();
        data.rotation0      = fields[ 7].GetFloat();
        data.rotation1      = fields[ 8].GetFloat();
        data.rotation2      = fields[ 9].GetFloat();
        data.rotation3      = fields[10].GetFloat();
        data.spawntimesecs  = fields[11].GetInt32();
        data.animprogress   = fields[12].GetUInt32();
        data.artKit         = 0;

        uint32 go_state     = fields[13].GetUInt32();
        if (go_state >= MAX_GO_STATE)
        {
            sLog.outErrorDb("Table `gameobject` has gameobject (GUID: %u Entry: %u) with invalid `state` (%u) value, skipped.", guid, data.id, go_state);
            continue;
        }
        data.go_state       = GOState(go_state);

        data.spawnMask      = fields[14].GetUInt8();
        int16 gameEvent     = fields[15].GetInt16();
        int32 PoolId        = fields[16].GetInt32();


        if (data.rotation2 < -1.0f || data.rotation2 > 1.0f)
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid rotation2 (%f) value, skip", guid, data.id, data.rotation2);
            continue;
        }

        if (data.rotation3 < -1.0f || data.rotation3 > 1.0f)
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid rotation3 (%f) value, skip", guid, data.id, data.rotation3);
            continue;
        }

        if (!MapManager::IsValidMapCoord(data.mapid, data.posX, data.posY, data.posZ, data.orientation))
        {
            sLog.outErrorDb("Table `gameobject` have gameobject (GUID: %u Entry: %u) with invalid coordinates, skip", guid, data.id);
            continue;
        }

        if (gameEvent == 0 && PoolId == 0)                      // if not this is to be managed by GameEvent System or Pool system
            AddGameobjectToGrid(guid, &data);
        ++count;

    }
    while (result->NextRow());

    sLog.outString(">> Loaded %lu gameobjects", mGameObjectDataMap.size());
}

void ObjectMgr::AddGameobjectToGrid(uint32 guid, GameObjectData const* data)
{
    uint8 mask = data->spawnMask;
    for (uint8 i = 0; mask != 0; i++, mask >>= 1)
    {
        if (mask & 1)
        {
            CellPair cell_pair = Oregon::ComputeCellPair(data->posX, data->posY);
            uint32 cell_id = (cell_pair.y_coord * TOTAL_NUMBER_OF_CELLS_PER_MAP) + cell_pair.x_coord;

            CellObjectGuids& cell_guids = mMapObjectGuids[MAKE_PAIR32(data->mapid, i)][cell_id];
            cell_guids.gameobjects.insert(guid);
        }
    }
}

void ObjectMgr::RemoveGameobjectFromGrid(uint32 guid, GameObjectData const* data)
{
    uint8 mask = data->spawnMask;
    for (uint8 i = 0; mask != 0; i++, mask >>= 1)
    {
        if (mask & 1)
        {
            CellPair cell_pair = Oregon::ComputeCellPair(data->posX, data->posY);
            uint32 cell_id = (cell_pair.y_coord * TOTAL_NUMBER_OF_CELLS_PER_MAP) + cell_pair.x_coord;

            CellObjectGuids& cell_guids = mMapObjectGuids[MAKE_PAIR32(data->mapid, i)][cell_id];
            cell_guids.gameobjects.erase(guid);
        }
    }
}

void ObjectMgr::LoadCreatureRespawnTimes()
{
    uint32 count = 0;

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT guid,respawntime,instance FROM creature_respawn");

    if (!result)
    {


        sLog.outString(">> Loaded 0 creature respawn time.");
        return;
    }


    do
    {
        Field* fields = result->Fetch();

        uint32 loguid       = fields[0].GetUInt32();
        uint64 respawn_time = fields[1].GetUInt64();
        uint32 instance     = fields[2].GetUInt32();

        mCreatureRespawnTimes[MAKE_PAIR64(loguid, instance)] = time_t(respawn_time);

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %lu creature respawn times", mCreatureRespawnTimes.size());
}

void ObjectMgr::LoadGameobjectRespawnTimes()
{
    // remove outdated data
    WorldDatabase.DirectExecute("DELETE FROM gameobject_respawn WHERE respawntime <= UNIX_TIMESTAMP(NOW())");

    uint32 count = 0;

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT guid,respawntime,instance FROM gameobject_respawn");

    if (!result)
    {


        sLog.outString(">> Loaded 0 gameobject respawn time.");
        return;
    }


    do
    {
        Field* fields = result->Fetch();

        uint32 loguid       = fields[0].GetUInt32();
        uint64 respawn_time = fields[1].GetUInt64();
        uint32 instance     = fields[2].GetUInt32();

        mGORespawnTimes[MAKE_PAIR64(loguid, instance)] = time_t(respawn_time);

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %lu gameobject respawn times", mGORespawnTimes.size());
}

// name must be checked to correctness (if received) before call this function
uint64 ObjectMgr::GetPlayerGUIDByName(std::string name) const
{
    uint64 guid = 0;

    CharacterDatabase.escape_string(name);

    // Player name safe to sending to DB (checked at login) and this function using
    QueryResult_AutoPtr result = CharacterDatabase.PQuery("SELECT guid FROM characters WHERE name = '%s'", name.c_str());
    if (result)
        guid = MAKE_NEW_GUID((*result)[0].GetUInt32(), 0, HIGHGUID_PLAYER);

    return guid;
}

bool ObjectMgr::GetPlayerNameByGUID(const uint64& guid, std::string& name) const
{
    // prevent DB access for online player
    if (Player* player = GetPlayer(guid))
    {
        name = player->GetName();
        return true;
    }

    QueryResult_AutoPtr result = CharacterDatabase.PQuery("SELECT name FROM characters WHERE guid = '%u'", GUID_LOPART(guid));

    if (result)
    {
        name = (*result)[0].GetCppString();
        return true;
    }

    return false;
}

uint32 ObjectMgr::GetPlayerTeamByGUID(const uint64& guid) const
{
    QueryResult_AutoPtr result = CharacterDatabase.PQuery("SELECT race FROM characters WHERE guid = '%u'", GUID_LOPART(guid));

    if (result)
    {
        uint8 race = (*result)[0].GetUInt8();
        return Player::TeamForRace(race);
    }

    return 0;
}

uint32 ObjectMgr::GetPlayerAccountIdByGUID(const uint64& guid) const
{
    QueryResult_AutoPtr result = CharacterDatabase.PQuery("SELECT account FROM characters WHERE guid = '%u'", GUID_LOPART(guid));
    if (result)
    {
        uint32 acc = (*result)[0].GetUInt32();
        return acc;
    }

    return 0;
}

uint32 ObjectMgr::GetPlayerAccountIdByPlayerName(const std::string& name) const
{
    QueryResult_AutoPtr result = CharacterDatabase.PQuery("SELECT account FROM characters WHERE name = '%s'", name.c_str());
    if (result)
    {
        uint32 acc = (*result)[0].GetUInt32();
        return acc;
    }

    return 0;
}

void ObjectMgr::LoadItemLocales()
{
    mItemLocaleMap.clear();                                 // need for reload case

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry,name_loc1,description_loc1,name_loc2,description_loc2,name_loc3,description_loc3,name_loc4,description_loc4,name_loc5,description_loc5,name_loc6,description_loc6,name_loc7,description_loc7,name_loc8,description_loc8 FROM locales_item");

    if (!result)
    {


        sLog.outString(">> Loaded 0 Item locale strings. DB table locales_item is empty.");
        return;
    }


    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        ItemLocale& data = mItemLocaleMap[entry];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[1 + 2 * (i - 1)].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if (data.Name.size() <= idx)
                        data.Name.resize(idx + 1);

                    data.Name[idx] = str;
                }
            }

            str = fields[1 + 2 * (i - 1) + 1].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if (data.Description.size() <= idx)
                        data.Description.resize(idx + 1);

                    data.Description[idx] = str;
                }
            }
        }
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %lu Item locale strings", mItemLocaleMap.size());
}

struct SQLItemLoader : public SQLStorageLoaderBase<SQLItemLoader>
{
    template<class D>
    void convert_from_str(uint32 field_pos, char* src, D& dst)
    {
        dst = D(sObjectMgr.GetScriptId(src));
    }
};

void ObjectMgr::LoadItemPrototypes()
{
    SQLItemLoader loader;
    loader.Load(sItemStorage);
    sLog.outString(">> Loaded %u item prototypes", sItemStorage.RecordCount);

    // check data correctness
    for (uint32 i = 1; i < sItemStorage.MaxEntry; ++i)
    {
        ItemPrototype const* proto = sItemStorage.LookupEntry<ItemPrototype >(i);
        ItemEntry const* dbcitem = sItemStore.LookupEntry(i);
        if (!proto)
        {
            /* to many errors, and possible not all items really used in game
            if (dbcitem)
                sLog.outErrorDb("Item (Entry: %u) doesn't exists in DB, but must exist.",i);
            */
            continue;
        }

        if (dbcitem)
        {
            if (proto->InventoryType != dbcitem->InventoryType)
            {
                sLog.outErrorDb("Item (Entry: %u) not correct %u inventory type, must be %u (still using DB value).", i, proto->InventoryType, dbcitem->InventoryType);
                // It safe let use InventoryType from DB
            }

            if (proto->DisplayInfoID != dbcitem->DisplayId)
            {
                sLog.outErrorDb("Item (Entry: %u) not correct %u display id, must be %u (using it).", i, proto->DisplayInfoID, dbcitem->DisplayId);
                const_cast<ItemPrototype*>(proto)->DisplayInfoID = dbcitem->DisplayId;
            }
            if (proto->Sheath != dbcitem->Sheath)
            {
                sLog.outErrorDb("Item (Entry: %u) not correct %u sheath, must be %u  (using it).", i, proto->Sheath, dbcitem->Sheath);
                const_cast<ItemPrototype*>(proto)->Sheath = dbcitem->Sheath;
            }
        }
        else
            sLog.outErrorDb("Item (Entry: %u) not correct (not found in list of existing items).", i);

        if (proto->Class >= MAX_ITEM_CLASS)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong Class value (%u)", i, proto->Class);
            const_cast<ItemPrototype*>(proto)->Class = ITEM_CLASS_JUNK;
        }

        if (proto->SubClass >= MaxItemSubclassValues[proto->Class])
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong Subclass value (%u) for class %u", i, proto->SubClass, proto->Class);
            const_cast<ItemPrototype*>(proto)->SubClass = 0;// exist for all item classes
        }

        if (proto->Quality >= MAX_ITEM_QUALITY)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong Quality value (%u)", i, proto->Quality);
            const_cast<ItemPrototype*>(proto)->Quality = ITEM_QUALITY_NORMAL;
        }

        if (proto->BuyCount <= 0)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong BuyCount value (%u), set to default(1).", i, proto->BuyCount);
            const_cast<ItemPrototype*>(proto)->BuyCount = 1;
        }

        if (proto->InventoryType >= MAX_INVTYPE)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong InventoryType value (%u)", i, proto->InventoryType);
            const_cast<ItemPrototype*>(proto)->InventoryType = INVTYPE_NON_EQUIP;
        }

        if (proto->RequiredSkill >= MAX_SKILL_TYPE)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong RequiredSkill value (%u)", i, proto->RequiredSkill);
            const_cast<ItemPrototype*>(proto)->RequiredSkill = 0;
        }

        if (!(proto->AllowableClass & CLASSMASK_ALL_PLAYABLE))
            sLog.outErrorDb("Item (Entry: %u) has no playable classes (%u) in AllowableClass and can't be equipped.", i, proto->AllowableClass);

        if (!(proto->AllowableRace & RACEMASK_ALL_PLAYABLE))
            sLog.outErrorDb("Item (Entry: %u) has no playable races (%u) in AllowableRace and can't be equipped.", i, proto->AllowableRace);

        if (proto->RequiredSpell && !sSpellStore.LookupEntry(proto->RequiredSpell))
        {
            sLog.outErrorDb("Item (Entry: %u) has invalid spell in RequiredSpell (%u)", i, proto->RequiredSpell);
            const_cast<ItemPrototype*>(proto)->RequiredSpell = 0;
        }

        if (proto->RequiredReputationRank >= MAX_REPUTATION_RANK)
            sLog.outErrorDb("Item (Entry: %u) has wrong reputation rank in RequiredReputationRank (%u), item can't be used.", i, proto->RequiredReputationRank);

        if (proto->RequiredReputationFaction)
        {
            if (!sFactionStore.LookupEntry(proto->RequiredReputationFaction))
            {
                sLog.outErrorDb("Item (Entry: %u) has invalid faction in RequiredReputationFaction (%u)", i, proto->RequiredReputationFaction);
                const_cast<ItemPrototype*>(proto)->RequiredReputationFaction = 0;
            }

            if (proto->RequiredReputationRank == MIN_REPUTATION_RANK)
                sLog.outErrorDb("Item (Entry: %u) has min. reputation rank in RequiredReputationRank (0) but RequiredReputationFaction > 0, faction setting is useless.", i);
        }
        else if (proto->RequiredReputationRank > MIN_REPUTATION_RANK)
            sLog.outErrorDb("Item (Entry: %u) has RequiredReputationFaction == 0 but RequiredReputationRank > 0, rank setting is useless.", i);

        if (proto->Stackable == 0)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong value in stackable (%u), replace by default 1.", i, proto->Stackable);
            const_cast<ItemPrototype*>(proto)->Stackable = 1;
        }
        else if (proto->Stackable > 255)
        {
            sLog.outErrorDb("Item (Entry: %u) has too large value in stackable (%u), replace by hardcoded upper limit (255).", i, proto->Stackable);
            const_cast<ItemPrototype*>(proto)->Stackable = 255;
        }

        if (proto->ContainerSlots > MAX_BAG_SIZE)
        {
            sLog.outErrorDb("Item (Entry: %u) has too large value in ContainerSlots (%u), replace by hardcoded limit (%u).", i, proto->ContainerSlots, MAX_BAG_SIZE);
            const_cast<ItemPrototype*>(proto)->ContainerSlots = MAX_BAG_SIZE;
        }

        for (int j = 0; j < 10; j++)
        {
            // for ItemStatValue != 0
            if (proto->ItemStat[j].ItemStatValue && proto->ItemStat[j].ItemStatType >= MAX_ITEM_MOD)
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong stat_type%d (%u)", i, j + 1, proto->ItemStat[j].ItemStatType);
                const_cast<ItemPrototype*>(proto)->ItemStat[j].ItemStatType = 0;
            }
        }

        for (int j = 0; j < 5; j++)
        {
            if (proto->Damage[j].DamageType >= MAX_SPELL_SCHOOL)
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong dmg_type%d (%u)", i, j + 1, proto->Damage[j].DamageType);
                const_cast<ItemPrototype*>(proto)->Damage[j].DamageType = 0;
            }
        }

        // special format
        if (proto->Spells[0].SpellId == SPELL_GENERIC_LEARN)
        {
            // spell_1
            if (proto->Spells[0].SpellTrigger != ITEM_SPELLTRIGGER_ON_USE)
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong item spell trigger value in spelltrigger_%d (%u) for special learning format", i, 0 + 1, proto->Spells[0].SpellTrigger);
                const_cast<ItemPrototype*>(proto)->Spells[0].SpellId = 0;
                const_cast<ItemPrototype*>(proto)->Spells[0].SpellTrigger = ITEM_SPELLTRIGGER_ON_USE;
                const_cast<ItemPrototype*>(proto)->Spells[1].SpellId = 0;
                const_cast<ItemPrototype*>(proto)->Spells[1].SpellTrigger = ITEM_SPELLTRIGGER_ON_USE;
            }

            // spell_2 have learning spell
            if (proto->Spells[1].SpellTrigger != ITEM_SPELLTRIGGER_LEARN_SPELL_ID)
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong item spell trigger value in spelltrigger_%d (%u) for special learning format.", i, 1 + 1, proto->Spells[1].SpellTrigger);
                const_cast<ItemPrototype*>(proto)->Spells[0].SpellId = 0;
                const_cast<ItemPrototype*>(proto)->Spells[1].SpellId = 0;
                const_cast<ItemPrototype*>(proto)->Spells[1].SpellTrigger = ITEM_SPELLTRIGGER_ON_USE;
            }
            else if (!proto->Spells[1].SpellId)
            {
                sLog.outErrorDb("Item (Entry: %u) not has expected spell in spellid_%d in special learning format.", i, 1 + 1);
                const_cast<ItemPrototype*>(proto)->Spells[0].SpellId = 0;
                const_cast<ItemPrototype*>(proto)->Spells[1].SpellTrigger = ITEM_SPELLTRIGGER_ON_USE;
            }
            else
            {
                SpellEntry const* spellInfo = sSpellStore.LookupEntry(proto->Spells[1].SpellId);
                if (!spellInfo && !sDisableMgr.IsDisabledFor(DISABLE_TYPE_SPELL, spellInfo->Id, NULL))
                {
                    sLog.outErrorDb("Item (Entry: %u) has invalid spell in spellid_%d (%u)", i, 1 + 1, proto->Spells[1].SpellId);
                    const_cast<ItemPrototype*>(proto)->Spells[0].SpellId = 0;
                    const_cast<ItemPrototype*>(proto)->Spells[1].SpellId = 0;
                    const_cast<ItemPrototype*>(proto)->Spells[1].SpellTrigger = ITEM_SPELLTRIGGER_ON_USE;
                }
                // allowed only in special format
                else if (proto->Spells[1].SpellId == SPELL_GENERIC_LEARN)
                {
                    sLog.outErrorDb("Item (Entry: %u) has broken spell in spellid_%d (%u)", i, 1 + 1, proto->Spells[1].SpellId);
                    const_cast<ItemPrototype*>(proto)->Spells[0].SpellId = 0;
                    const_cast<ItemPrototype*>(proto)->Spells[1].SpellId = 0;
                    const_cast<ItemPrototype*>(proto)->Spells[1].SpellTrigger = ITEM_SPELLTRIGGER_ON_USE;
                }
            }

            // spell_3*,spell_4*,spell_5* is empty
            for (int j = 2; j < 5; j++)
            {
                if (proto->Spells[j].SpellTrigger != ITEM_SPELLTRIGGER_ON_USE)
                {
                    sLog.outErrorDb("Item (Entry: %u) has wrong item spell trigger value in spelltrigger_%d (%u)", i, j + 1, proto->Spells[j].SpellTrigger);
                    const_cast<ItemPrototype*>(proto)->Spells[j].SpellId = 0;
                    const_cast<ItemPrototype*>(proto)->Spells[j].SpellTrigger = ITEM_SPELLTRIGGER_ON_USE;
                }
                else if (proto->Spells[j].SpellId != 0)
                {
                    sLog.outErrorDb("Item (Entry: %u) has wrong spell in spellid_%d (%u) for learning special format", i, j + 1, proto->Spells[j].SpellId);
                    const_cast<ItemPrototype*>(proto)->Spells[j].SpellId = 0;
                }
            }
        }
        // normal spell list
        else
        {
            for (int j = 0; j < 5; j++)
            {
                if (proto->Spells[j].SpellTrigger >= MAX_ITEM_SPELLTRIGGER || proto->Spells[j].SpellTrigger == ITEM_SPELLTRIGGER_LEARN_SPELL_ID)
                {
                    sLog.outErrorDb("Item (Entry: %u) has wrong item spell trigger value in spelltrigger_%d (%u)", i, j + 1, proto->Spells[j].SpellTrigger);
                    const_cast<ItemPrototype*>(proto)->Spells[j].SpellId = 0;
                    const_cast<ItemPrototype*>(proto)->Spells[j].SpellTrigger = ITEM_SPELLTRIGGER_ON_USE;
                }

                if (proto->Spells[j].SpellId)
                {
                    SpellEntry const* spellInfo = sSpellStore.LookupEntry(proto->Spells[j].SpellId);
                    if (!spellInfo && !sDisableMgr.IsDisabledFor(DISABLE_TYPE_SPELL, spellInfo->Id, NULL))
                    {
                        sLog.outErrorDb("Item (Entry: %u) has invalid spell in spellid_%d (%u)", i, j + 1, proto->Spells[j].SpellId);
                        const_cast<ItemPrototype*>(proto)->Spells[j].SpellId = 0;
                    }
                    // allowed only in special format
                    else if (proto->Spells[j].SpellId == SPELL_GENERIC_LEARN)
                    {
                        sLog.outErrorDb("Item (Entry: %u) has broken spell in spellid_%d (%u)", i, j + 1, proto->Spells[j].SpellId);
                        const_cast<ItemPrototype*>(proto)->Spells[j].SpellId = 0;
                    }
                }
            }
        }

        if (proto->Bonding >= MAX_BIND_TYPE)
            sLog.outErrorDb("Item (Entry: %u) has wrong Bonding value (%u)", i, proto->Bonding);

        if (proto->PageText && !sPageTextStore.LookupEntry<PageText>(proto->PageText))
            sLog.outErrorDb("Item (Entry: %u) has invalid first page (Id:%u)", i, proto->PageText);

        if (proto->LockID && !sLockStore.LookupEntry(proto->LockID))
            sLog.outErrorDb("Item (Entry: %u) has wrong LockID (%u)", i, proto->LockID);

        if (proto->Sheath >= MAX_SHEATHETYPE)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong Sheath (%u)", i, proto->Sheath);
            const_cast<ItemPrototype*>(proto)->Sheath = SHEATHETYPE_NONE;
        }

        if (proto->RandomProperty && !sItemRandomPropertiesStore.LookupEntry(GetItemEnchantMod(proto->RandomProperty)))
        {
            sLog.outErrorDb("Item (Entry: %u) has unknown (wrong or not listed in item_enchantment_template) RandomProperty (%u)", i, proto->RandomProperty);
            const_cast<ItemPrototype*>(proto)->RandomProperty = 0;
        }

        if (proto->RandomSuffix && !sItemRandomSuffixStore.LookupEntry(GetItemEnchantMod(proto->RandomSuffix)))
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong RandomSuffix (%u)", i, proto->RandomSuffix);
            const_cast<ItemPrototype*>(proto)->RandomSuffix = 0;
        }

        if (proto->ItemSet && !sItemSetStore.LookupEntry(proto->ItemSet))
        {
            sLog.outErrorDb("Item (Entry: %u) has invalid ItemSet (%u)", i, proto->ItemSet);
            const_cast<ItemPrototype*>(proto)->ItemSet = 0;
        }

        if (proto->Area && !GetAreaEntryByAreaID(proto->Area))
            sLog.outErrorDb("Item (Entry: %u) has wrong Area (%u)", i, proto->Area);

        if (proto->Map && !sMapStore.LookupEntry(proto->Map))
            sLog.outErrorDb("Item (Entry: %u) has wrong Map (%u)", i, proto->Map);

        if (proto->TotemCategory && !sTotemCategoryStore.LookupEntry(proto->TotemCategory))
            sLog.outErrorDb("Item (Entry: %u) has wrong TotemCategory (%u)", i, proto->TotemCategory);

        for (int j = 0; j < 3; j++)
        {
            if (proto->Socket[j].Color && (proto->Socket[j].Color & SOCKET_COLOR_ALL) != proto->Socket[j].Color)
            {
                sLog.outErrorDb("Item (Entry: %u) has wrong socketColor_%d (%u)", i, j + 1, proto->Socket[j].Color);
                const_cast<ItemPrototype*>(proto)->Socket[j].Color = 0;
            }
        }

        if (proto->GemProperties && !sGemPropertiesStore.LookupEntry(proto->GemProperties))
            sLog.outErrorDb("Item (Entry: %u) has wrong GemProperties (%u)", i, proto->GemProperties);

        if (proto->FoodType >= MAX_PET_DIET)
        {
            sLog.outErrorDb("Item (Entry: %u) has wrong FoodType value (%u)", i, proto->FoodType);
            const_cast<ItemPrototype*>(proto)->FoodType = 0;
        }
    }

    // this DBC used currently only for check item templates in DB.
    sItemStore.Clear();
}

void ObjectMgr::LoadPetLevelInfo()
{
    // Loading levels data
    {
        //                                                        0               1      2   3     4    5    6    7     8    9
        QueryResult_AutoPtr result  = WorldDatabase.Query("SELECT creature_entry, level, hp, mana, str, agi, sta, inte, spi, armor FROM pet_levelstats");

        uint32 count = 0;

        if (!result)
        {

            sLog.outString(">> Loaded %u level pet stats definitions", count);
            sLog.outErrorDb("Error loading pet_levelstats table or empty table.");
            return;
        }


        do
        {
            Field* fields = result->Fetch();

            uint32 creature_id = fields[0].GetUInt32();
            if (!sCreatureStorage.LookupEntry<CreatureInfo>(creature_id))
            {
                sLog.outErrorDb("Wrong creature id %u in pet_levelstats table, ignoring.", creature_id);
                continue;
            }

            uint32 current_level = fields[1].GetUInt32();
            if (current_level > sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL))
            {
                if (current_level > STRONG_MAX_LEVEL)        // hardcoded level maximum
                    sLog.outErrorDb("Wrong (> %u) level %u in pet_levelstats table, ignoring.", STRONG_MAX_LEVEL, current_level);
                else
                    sLog.outDetail("Unused (> MaxPlayerLevel in Oregond.conf) level %u in pet_levelstats table, ignoring.", current_level);
                continue;
            }
            else if (current_level < 1)
            {
                sLog.outErrorDb("Wrong (<1) level %u in pet_levelstats table, ignoring.", current_level);
                continue;
            }

            PetLevelInfo*& pInfoMapEntry = petInfo[creature_id];

            if (pInfoMapEntry == NULL)
                pInfoMapEntry =  new PetLevelInfo[sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL)];

            // data for level 1 stored in [0] array element, ...
            PetLevelInfo* pLevelInfo = &pInfoMapEntry[current_level - 1];

            pLevelInfo->health = fields[2].GetUInt16();
            pLevelInfo->mana   = fields[3].GetUInt16();
            pLevelInfo->armor  = fields[9].GetUInt16();

            for (int i = 0; i < MAX_STATS; i++)
                pLevelInfo->stats[i] = fields[i + 4].GetUInt16();

            ++count;
        }
        while (result->NextRow());

        sLog.outString(">> Loaded %u level pet stats definitions", count);
    }

    // Fill gaps and check integrity
    for (PetLevelInfoMap::iterator itr = petInfo.begin(); itr != petInfo.end(); ++itr)
    {
        PetLevelInfo* pInfo = itr->second;

        // fatal error if no level 1 data
        if (!pInfo || pInfo[0].health == 0)
            sLog.outFatal("Creature %u does not have pet stats data for Level 1!", itr->first);

        // fill level gaps
        for (uint32 level = 1; level < sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL); ++level)
        {
            if (pInfo[level].health == 0)
            {
                sLog.outErrorDb("Creature %u has no data for Level %i pet stats data, using data of Level %i.", itr->first, level + 1, level);
                pInfo[level] = pInfo[level - 1];
            }
        }
    }
}

PetLevelInfo const* ObjectMgr::GetPetLevelInfo(uint32 creature_id, uint32 level) const
{
    if (level > sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL))
        level = sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL);

    PetLevelInfoMap::const_iterator itr = petInfo.find(creature_id);
    if (itr == petInfo.end())
        return NULL;

    return &itr->second[level - 1];                         // data for level 1 stored in [0] array element, ...
}

void ObjectMgr::LoadPlayerInfo()
{
    // Load playercreate
    {
        //                                                       0     1      2    3     4           5           6           7
        QueryResult_AutoPtr result = WorldDatabase.Query("SELECT race, class, map, zone, position_x, position_y, position_z, orientation FROM playercreateinfo");

        uint32 count = 0;

        if (!result)
        {
            sLog.outString(">> Loaded %u player create definitions", count);
            sLog.outFatal("Error loading playercreateinfo table or empty table.");
        }


        do
        {
            Field* fields = result->Fetch();

            uint32 current_race  = fields[0].GetUInt32();
            uint32 current_class = fields[1].GetUInt32();
            uint32 mapId         = fields[2].GetUInt32();
            uint32 areaId        = fields[3].GetUInt32();
            float  positionX     = fields[4].GetFloat();
            float  positionY     = fields[5].GetFloat();
            float  positionZ     = fields[6].GetFloat();
            float  orientation   = fields[7].GetFloat();

            if (current_race >= MAX_RACES)
            {
                sLog.outErrorDb("Wrong race %u in playercreateinfo table, ignoring.", current_race);
                continue;
            }

            ChrRacesEntry const* rEntry = sChrRacesStore.LookupEntry(current_race);
            if (!rEntry)
            {
                sLog.outErrorDb("Wrong race %u in playercreateinfo table, ignoring.", current_race);
                continue;
            }

            if (current_class >= MAX_CLASSES)
            {
                sLog.outErrorDb("Wrong class %u in playercreateinfo table, ignoring.", current_class);
                continue;
            }

            if (!sChrClassesStore.LookupEntry(current_class))
            {
                sLog.outErrorDb("Wrong class %u in playercreateinfo table, ignoring.", current_class);
                continue;
            }

            // accept DB data only for valid position (and non instanceable)
            if (!MapManager::IsValidMapCoord(mapId, positionX, positionY, positionZ, orientation))
            {
                sLog.outErrorDb("Wrong home position for class %u race %u pair in playercreateinfo table, ignoring.", current_class, current_race);
                continue;
            }

            if (sMapStore.LookupEntry(mapId)->Instanceable())
            {
                sLog.outErrorDb("Home position in instanceable map for class %u race %u pair in playercreateinfo table, ignoring.", current_class, current_race);
                continue;
            }

            PlayerInfo* pInfo = &playerInfo[current_race][current_class];

            pInfo->mapId       = mapId;
            pInfo->areaId      = areaId;
            pInfo->positionX   = positionX;
            pInfo->positionY   = positionY;
            pInfo->positionZ   = positionZ;
            pInfo->orientation = orientation;

            pInfo->displayId_m = rEntry->model_m;
            pInfo->displayId_f = rEntry->model_f;

            ++count;
        }
        while (result->NextRow());

        sLog.outString(">> Loaded %u player create definitions", count);
    }

    // Load playercreate items
    {
        //                                                       0     1      2       3
        QueryResult_AutoPtr result = WorldDatabase.Query("SELECT race, class, itemid, amount FROM playercreateinfo_item");

        uint32 count = 0;

        if (!result)


            sLog.outString(">> Loaded %u custom player create items", count);
        else
        {

            do
            {
                Field* fields = result->Fetch();

                uint32 current_race = fields[0].GetUInt32();
                if (current_race >= MAX_RACES)
                {
                    sLog.outErrorDb("Wrong race %u in playercreateinfo_item table, ignoring.", current_race);
                    continue;
                }

                uint32 current_class = fields[1].GetUInt32();
                if (current_class >= MAX_CLASSES)
                {
                    sLog.outErrorDb("Wrong class %u in playercreateinfo_item table, ignoring.", current_class);
                    continue;
                }

                PlayerInfo* pInfo = &playerInfo[current_race][current_class];

                uint32 item_id = fields[2].GetUInt32();

                if (!GetItemPrototype(item_id))
                {
                    sLog.outErrorDb("Item id %u (race %u class %u) in playercreateinfo_item table but not listed in item_template, ignoring.", item_id, current_race, current_class);
                    continue;
                }

                uint32 amount  = fields[3].GetUInt32();

                if (!amount)
                {
                    sLog.outErrorDb("Item id %u (class %u race %u) has amount == 0 in playercreateinfo_item table, ignoring.", item_id, current_race, current_class);
                    continue;
                }

                pInfo->item.push_back(PlayerCreateInfoItem(item_id, amount));

                ++count;
            }
            while (result->NextRow());

            sLog.outString(">> Loaded %u custom player create items", count);
        }
    }

    // Load playercreate spells
    {

        QueryResult_AutoPtr result = QueryResult_AutoPtr(NULL);
        if (sWorld.getConfig(CONFIG_START_ALL_SPELLS))
            result = WorldDatabase.Query("SELECT race, class, Spell, Active FROM playercreateinfo_spell_custom");
        else
            result = WorldDatabase.Query("SELECT race, class, Spell, Active FROM playercreateinfo_spell");

        uint32 count = 0;

        if (!result)
        {

            sLog.outString(">> Loaded %u player create spells", count);
            sLog.outErrorDb("Error loading player starting spells or empty table.");
        }
        else
        {

            do
            {
                Field* fields = result->Fetch();

                uint32 current_race = fields[0].GetUInt32();
                if (current_race >= MAX_RACES)
                {
                    sLog.outErrorDb("Wrong race %u in playercreateinfo_spell table, ignoring.", current_race);
                    continue;
                }

                uint32 current_class = fields[1].GetUInt32();
                if (current_class >= MAX_CLASSES)
                {
                    sLog.outErrorDb("Wrong class %u in playercreateinfo_spell table, ignoring.", current_class);
                    continue;
                }

                PlayerInfo* pInfo = &playerInfo[current_race][current_class];
                pInfo->spell.push_back(CreateSpellPair(fields[2].GetUInt16(), fields[3].GetUInt8()));

                ++count;
            }
            while (result->NextRow());

            sLog.outString(">> Loaded %u player create spells", count);
        }
    }

    // Load playercreate actions
    {
        //                                                0     1      2       3       4     5
        QueryResult_AutoPtr result = WorldDatabase.Query("SELECT race, class, button, action, type, misc FROM playercreateinfo_action");

        uint32 count = 0;

        if (!result)
        {

            sLog.outString(">> Loaded %u player create actions", count);
            sLog.outErrorDb("Error loading playercreateinfo_action table or empty table.");
        }
        else
        {

            do
            {
                Field* fields = result->Fetch();

                uint32 current_race = fields[0].GetUInt32();
                if (current_race >= MAX_RACES)
                {
                    sLog.outErrorDb("Wrong race %u in playercreateinfo_action table, ignoring.", current_race);
                    continue;
                }

                uint32 current_class = fields[1].GetUInt32();
                if (current_class >= MAX_CLASSES)
                {
                    sLog.outErrorDb("Wrong class %u in playercreateinfo_action table, ignoring.", current_class);
                    continue;
                }

                PlayerInfo* pInfo = &playerInfo[current_race][current_class];
                pInfo->action[0].push_back(fields[2].GetUInt16());
                pInfo->action[1].push_back(fields[3].GetUInt16());
                pInfo->action[2].push_back(fields[4].GetUInt16());
                pInfo->action[3].push_back(fields[5].GetUInt16());

                ++count;
            }
            while (result->NextRow());

            sLog.outString(">> Loaded %u player create actions", count);
        }
    }

    // Loading levels data (class only dependent)
    {
        //                                                 0      1      2       3
        QueryResult_AutoPtr result  = WorldDatabase.Query("SELECT class, level, basehp, basemana FROM player_classlevelstats");

        uint32 count = 0;

        if (!result)
        {

            sLog.outString(">> Loaded %u level health/mana definitions", count);
            sLog.outFatal("Error loading player_classlevelstats table or empty table.");
        }


        do
        {
            Field* fields = result->Fetch();

            uint32 current_class = fields[0].GetUInt32();
            if (current_class >= MAX_CLASSES)
            {
                sLog.outErrorDb("Wrong class %u in player_classlevelstats table, ignoring.", current_class);
                continue;
            }

            uint32 current_level = fields[1].GetUInt32();
            if (current_level > sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL))
            {
                if (current_level > STRONG_MAX_LEVEL)        // hardcoded level maximum
                    sLog.outErrorDb("Wrong (> %u) level %u in player_classlevelstats table, ignoring.", STRONG_MAX_LEVEL, current_level);
                else
                    sLog.outDetail("Unused (> MaxPlayerLevel in Oregond.conf) level %u in player_classlevelstats table, ignoring.", current_level);
                continue;
            }

            PlayerClassInfo* pClassInfo = &playerClassInfo[current_class];

            if (!pClassInfo->levelInfo)
                pClassInfo->levelInfo = new PlayerClassLevelInfo[sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL)];

            PlayerClassLevelInfo* pClassLevelInfo = &pClassInfo->levelInfo[current_level - 1];

            pClassLevelInfo->basehealth = fields[2].GetUInt16();
            pClassLevelInfo->basemana   = fields[3].GetUInt16();

            ++count;
        }
        while (result->NextRow());

        sLog.outString(">> Loaded %u level health/mana definitions", count);
    }

    // Fill gaps and check integrity
    for (int class_ = 0; class_ < MAX_CLASSES; ++class_)
    {
        // skip non existed classes
        if (!sChrClassesStore.LookupEntry(class_))
            continue;

        PlayerClassInfo* pClassInfo = &playerClassInfo[class_];

        // fatal error if no level 1 data
        if (!pClassInfo->levelInfo || pClassInfo->levelInfo[0].basehealth == 0)
            sLog.outFatal("Class %i Level 1 does not have health/mana data!", class_);

        // fill level gaps
        for (uint32 level = 1; level < sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL); ++level)
        {
            if (pClassInfo->levelInfo[level].basehealth == 0)
            {
                sLog.outErrorDb("Class %i Level %i does not have health/mana data. Using stats data of level %i.", class_, level + 1, level);
                pClassInfo->levelInfo[level] = pClassInfo->levelInfo[level - 1];
            }
        }
    }

    // Loading levels data (class/race dependent)
    {
        //                                                        0     1      2      3    4    5    6    7
        QueryResult_AutoPtr result  = WorldDatabase.Query("SELECT race, class, level, str, agi, sta, inte, spi FROM player_levelstats");

        uint32 count = 0;

        if (!result)
        {
            sLog.outString(">> Loaded %u level stats definitions", count);
            sLog.outFatal("Error loading player_levelstats table or empty table.");
        }


        do
        {
            Field* fields = result->Fetch();

            uint32 current_race = fields[0].GetUInt32();
            if (current_race >= MAX_RACES)
            {
                sLog.outErrorDb("Wrong race %u in player_levelstats table, ignoring.", current_race);
                continue;
            }

            uint32 current_class = fields[1].GetUInt32();
            if (current_class >= MAX_CLASSES)
            {
                sLog.outErrorDb("Wrong class %u in player_levelstats table, ignoring.", current_class);
                continue;
            }

            uint32 current_level = fields[2].GetUInt32();
            if (current_level > sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL))
            {
                if (current_level > STRONG_MAX_LEVEL)        // hardcoded level maximum
                    sLog.outErrorDb("Wrong (> %u) level %u in player_levelstats table, ignoring.", STRONG_MAX_LEVEL, current_level);
                else
                    sLog.outDetail("Unused (> MaxPlayerLevel in Oregond.conf) level %u in player_levelstats table, ignoring.", current_level);
                continue;
            }

            PlayerInfo* pInfo = &playerInfo[current_race][current_class];

            if (!pInfo->levelInfo)
                pInfo->levelInfo = new PlayerLevelInfo[sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL)];

            PlayerLevelInfo* pLevelInfo = &pInfo->levelInfo[current_level - 1];

            for (int i = 0; i < MAX_STATS; i++)
                pLevelInfo->stats[i] = fields[i + 3].GetUInt8();

            ++count;
        }
        while (result->NextRow());

        sLog.outString(">> Loaded %u level stats definitions", count);
    }

    // Fill gaps and check integrity
    for (int race = 0; race < MAX_RACES; ++race)
    {
        // skip non existed races
        if (!sChrRacesStore.LookupEntry(race))
            continue;

        for (int class_ = 0; class_ < MAX_CLASSES; ++class_)
        {
            // skip non existed classes
            if (!sChrClassesStore.LookupEntry(class_))
                continue;

            PlayerInfo* pInfo = &playerInfo[race][class_];

            // skip non loaded combinations
            if (!pInfo->displayId_m || !pInfo->displayId_f)
                continue;

            // skip expansion races if not playing with expansion
            if (sWorld.getConfig(CONFIG_EXPANSION) < 1 && (race == RACE_BLOODELF || race == RACE_DRAENEI))
                continue;

            // skip expansion classes if not playing with expansion
            if (sWorld.getConfig(CONFIG_EXPANSION) < 2 && class_ == CLASS_DEATH_KNIGHT)
                continue;

            // fatal error if no level 1 data
            if (!pInfo->levelInfo || pInfo->levelInfo[0].stats[0] == 0)
                sLog.outFatal("Race %i Class %i Level 1 does not have stats data!", race, class_);

            // fill level gaps
            for (uint32 level = 1; level < sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL); ++level)
            {
                if (pInfo->levelInfo[level].stats[0] == 0)
                {
                    sLog.outErrorDb("Race %i Class %i Level %i does not have stats data. Using stats data of level %i.", race, class_, level + 1, level);
                    pInfo->levelInfo[level] = pInfo->levelInfo[level - 1];
                }
            }
        }
    }
}

void ObjectMgr::GetPlayerClassLevelInfo(uint32 class_, uint32 level, PlayerClassLevelInfo* info) const
{
    if (level < 1 || class_ >= MAX_CLASSES)
        return;

    PlayerClassInfo const* pInfo = &playerClassInfo[class_];

    if (level > sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL))
        level = sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL);

    *info = pInfo->levelInfo[level - 1];
}

void ObjectMgr::GetPlayerLevelInfo(uint32 race, uint32 class_, uint32 level, PlayerLevelInfo* info) const
{
    if (level < 1 || race   >= MAX_RACES || class_ >= MAX_CLASSES)
        return;

    PlayerInfo const* pInfo = &playerInfo[race][class_];
    if (pInfo->displayId_m == 0 || pInfo->displayId_f == 0)
        return;

    if (level <= sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL))
        *info = pInfo->levelInfo[level - 1];
    else
        BuildPlayerLevelInfo(race, class_, level, info);
}

void ObjectMgr::BuildPlayerLevelInfo(uint8 race, uint8 _class, uint8 level, PlayerLevelInfo* info) const
{
    // base data (last known level)
    *info = playerInfo[race][_class].levelInfo[sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL) - 1];

    for (int lvl = sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL) - 1; lvl < level; ++lvl)
    {
        switch (_class)
        {
        case CLASS_WARRIOR:
            info->stats[STAT_STRENGTH]  += (lvl > 23 ? 2 : (lvl > 1  ? 1 : 0));
            info->stats[STAT_STAMINA]   += (lvl > 23 ? 2 : (lvl > 1  ? 1 : 0));
            info->stats[STAT_AGILITY]   += (lvl > 36 ? 1 : (lvl > 6 && (lvl % 2) ? 1 : 0));
            info->stats[STAT_INTELLECT] += (lvl > 9 && !(lvl % 2) ? 1 : 0);
            info->stats[STAT_SPIRIT]    += (lvl > 9 && !(lvl % 2) ? 1 : 0);
            break;
        case CLASS_PALADIN:
            info->stats[STAT_STRENGTH]  += (lvl > 3  ? 1 : 0);
            info->stats[STAT_STAMINA]   += (lvl > 33 ? 2 : (lvl > 1 ? 1 : 0));
            info->stats[STAT_AGILITY]   += (lvl > 38 ? 1 : (lvl > 7 && !(lvl % 2) ? 1 : 0));
            info->stats[STAT_INTELLECT] += (lvl > 6 && (lvl % 2) ? 1 : 0);
            info->stats[STAT_SPIRIT]    += (lvl > 7 ? 1 : 0);
            break;
        case CLASS_HUNTER:
            info->stats[STAT_STRENGTH]  += (lvl > 4  ? 1 : 0);
            info->stats[STAT_STAMINA]   += (lvl > 4  ? 1 : 0);
            info->stats[STAT_AGILITY]   += (lvl > 33 ? 2 : (lvl > 1 ? 1 : 0));
            info->stats[STAT_INTELLECT] += (lvl > 8 && (lvl % 2) ? 1 : 0);
            info->stats[STAT_SPIRIT]    += (lvl > 38 ? 1 : (lvl > 9 && !(lvl % 2) ? 1 : 0));
            break;
        case CLASS_ROGUE:
            info->stats[STAT_STRENGTH]  += (lvl > 5  ? 1 : 0);
            info->stats[STAT_STAMINA]   += (lvl > 4  ? 1 : 0);
            info->stats[STAT_AGILITY]   += (lvl > 16 ? 2 : (lvl > 1 ? 1 : 0));
            info->stats[STAT_INTELLECT] += (lvl > 8 && !(lvl % 2) ? 1 : 0);
            info->stats[STAT_SPIRIT]    += (lvl > 38 ? 1 : (lvl > 9 && !(lvl % 2) ? 1 : 0));
            break;
        case CLASS_PRIEST:
            info->stats[STAT_STRENGTH]  += (lvl > 9 && !(lvl % 2) ? 1 : 0);
            info->stats[STAT_STAMINA]   += (lvl > 5  ? 1 : 0);
            info->stats[STAT_AGILITY]   += (lvl > 38 ? 1 : (lvl > 8 && (lvl % 2) ? 1 : 0));
            info->stats[STAT_INTELLECT] += (lvl > 22 ? 2 : (lvl > 1 ? 1 : 0));
            info->stats[STAT_SPIRIT]    += (lvl > 3  ? 1 : 0);
            break;
        case CLASS_SHAMAN:
            info->stats[STAT_STRENGTH]  += (lvl > 34 ? 1 : (lvl > 6 && (lvl % 2) ? 1 : 0));
            info->stats[STAT_STAMINA]   += (lvl > 4 ? 1 : 0);
            info->stats[STAT_AGILITY]   += (lvl > 7 && !(lvl % 2) ? 1 : 0);
            info->stats[STAT_INTELLECT] += (lvl > 5 ? 1 : 0);
            info->stats[STAT_SPIRIT]    += (lvl > 4 ? 1 : 0);
            break;
        case CLASS_MAGE:
            info->stats[STAT_STRENGTH]  += (lvl > 9 && !(lvl % 2) ? 1 : 0);
            info->stats[STAT_STAMINA]   += (lvl > 5  ? 1 : 0);
            info->stats[STAT_AGILITY]   += (lvl > 9 && !(lvl % 2) ? 1 : 0);
            info->stats[STAT_INTELLECT] += (lvl > 24 ? 2 : (lvl > 1 ? 1 : 0));
            info->stats[STAT_SPIRIT]    += (lvl > 33 ? 2 : (lvl > 2 ? 1 : 0));
            break;
        case CLASS_WARLOCK:
            info->stats[STAT_STRENGTH]  += (lvl > 9 && !(lvl % 2) ? 1 : 0);
            info->stats[STAT_STAMINA]   += (lvl > 38 ? 2 : (lvl > 3 ? 1 : 0));
            info->stats[STAT_AGILITY]   += (lvl > 9 && !(lvl % 2) ? 1 : 0);
            info->stats[STAT_INTELLECT] += (lvl > 33 ? 2 : (lvl > 2 ? 1 : 0));
            info->stats[STAT_SPIRIT]    += (lvl > 38 ? 2 : (lvl > 3 ? 1 : 0));
            break;
        case CLASS_DRUID:
            info->stats[STAT_STRENGTH]  += (lvl > 38 ? 2 : (lvl > 6 && (lvl % 2) ? 1 : 0));
            info->stats[STAT_STAMINA]   += (lvl > 32 ? 2 : (lvl > 4 ? 1 : 0));
            info->stats[STAT_AGILITY]   += (lvl > 38 ? 2 : (lvl > 8 && (lvl % 2) ? 1 : 0));
            info->stats[STAT_INTELLECT] += (lvl > 38 ? 3 : (lvl > 4 ? 1 : 0));
            info->stats[STAT_SPIRIT]    += (lvl > 38 ? 3 : (lvl > 5 ? 1 : 0));
        }
    }
}

void ObjectMgr::LoadGuilds()
{
    Guild* newGuild;
    uint32 count = 0;

    //                                                    0             1          2          3           4           5           6
    QueryResult_AutoPtr result = CharacterDatabase.Query("SELECT guild.guildid,guild.name,leaderguid,EmblemStyle,EmblemColor,BorderStyle,BorderColor,"
                                 //   7               8    9    10         11        12
                                 "BackgroundColor,info,motd,createdate,BankMoney,COUNT(guild_bank_tab.guildid) "
                                 "FROM guild LEFT JOIN guild_bank_tab ON guild.guildid = guild_bank_tab.guildid GROUP BY guild.guildid ORDER BY guildid ASC");

    if (!result)
    {



        sLog.outString(">> Loaded %u guild definitions", count);
        return;
    }

    // load guild ranks
    //                                                                0       1   2     3      4
    QueryResult_AutoPtr guildRanksResult   = CharacterDatabase.Query("SELECT guildid,rid,rname,rights,BankMoneyPerDay FROM guild_rank ORDER BY guildid ASC, rid ASC");

    // load guild members
    //                                                                0       1                 2    3     4       5                  6
    QueryResult_AutoPtr guildMembersResult = CharacterDatabase.Query("SELECT guildid,guild_member.guid,rank,pnote,offnote,BankResetTimeMoney,BankRemMoney,"
            //   7                 8                9                 10               11                12
            "BankResetTimeTab0,BankRemSlotsTab0,BankResetTimeTab1,BankRemSlotsTab1,BankResetTimeTab2,BankRemSlotsTab2,"
            //   13                14               15                16               17                18
            "BankResetTimeTab3,BankRemSlotsTab3,BankResetTimeTab4,BankRemSlotsTab4,BankResetTimeTab5,BankRemSlotsTab5,"
            //   19               20                21                22               23                       24
            "characters.name, characters.level, characters.class, characters.zone, characters.logout_time, characters.account "
            "FROM guild_member LEFT JOIN characters ON characters.guid = guild_member.guid ORDER BY guildid ASC");

    // load guild bank tab rights
    //                                                                      0       1     2   3       4
    QueryResult_AutoPtr guildBankTabRightsResult = CharacterDatabase.Query("SELECT guildid,TabId,rid,gbright,SlotPerDay FROM guild_bank_right ORDER BY guildid ASC, TabId ASC");


    do
    {
        //Field *fields = result->Fetch();

        ++count;

        newGuild = new Guild;
        if (!newGuild->LoadGuildFromDB(result) ||
            !newGuild->LoadRanksFromDB(guildRanksResult) ||
            !newGuild->LoadMembersFromDB(guildMembersResult) ||
            !newGuild->LoadBankRightsFromDB(guildBankTabRightsResult) ||
            !newGuild->CheckGuildStructure()
           )
        {
            newGuild->Disband();
            delete newGuild;
            continue;
        }
        newGuild->LoadGuildEventLogFromDB();
        newGuild->LoadGuildBankEventLogFromDB();
        newGuild->LoadGuildBankFromDB();
        AddGuild(newGuild);

    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u guild definitions", count);
}

void ObjectMgr::LoadArenaTeams()
{
    uint32 count = 0;

    //                                                     0                      1    2           3    4               5
    QueryResult_AutoPtr result = CharacterDatabase.Query( "SELECT arena_team.arenateamid,name,captainguid,type,BackgroundColor,EmblemStyle,"
                                 //   6           7           8            9      10    11   12     13    14
                                 "EmblemColor,BorderStyle,BorderColor, rating,games,wins,played,wins2,rank "
                                 "FROM arena_team LEFT JOIN arena_team_stats ON arena_team.arenateamid = arena_team_stats.arenateamid ORDER BY arena_team.arenateamid ASC" );

    if (!result)
    {



        sLog.outString(">> Loaded %u arenateam definitions", count);
        return;
    }

    // load arena_team members
    QueryResult_AutoPtr arenaTeamMembersResult = CharacterDatabase.Query(
                //          0           1           2           3         4             5           6               7    8
                "SELECT arenateamid,member.guid,played_week,wons_week,played_season,wons_season,personal_rating,name,class "
                "FROM arena_team_member member LEFT JOIN characters chars on member.guid = chars.guid ORDER BY member.arenateamid ASC");


    do
    {
        ++count;

        ArenaTeam* newArenaTeam = new ArenaTeam;
        if (!newArenaTeam->LoadArenaTeamFromDB(result) ||
            !newArenaTeam->LoadMembersFromDB(arenaTeamMembersResult))
        {
            newArenaTeam->Disband(NULL);
            delete newArenaTeam;
            continue;
        }
        AddArenaTeam(newArenaTeam);
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u arenateam definitions", count);
}

void ObjectMgr::LoadReferredFriends()
{
    QueryResult_AutoPtr result = LoginDatabase.Query("SELECT id1, id2 FROM account_referred");
    if (!result)
    {

        sLog.outString(">> Loaded 0 Referred Friends");
        return;
    }

    uint64 Id1;
    uint64 Id2;
    ReferFriendMap::const_iterator it;

    // clear these in case of reload
    m_referrerFriends.clear();
    m_referredFriends.clear();

    do
    {
        Field* field = result->Fetch();

        Id1 = field[0].GetUInt64(); // referrer
        Id2 = field[1].GetUInt64(); // referred

        if ((it = m_referredFriends.find(Id2)) != m_referredFriends.end())
        {
            sLog.outErrorDb("RAF Conflict! Account " UI64FMTD " is already linked with " UI64FMTD " (attempt to double link with account " UI64FMTD ")", Id2, it->second, Id1);
            continue;
        }

        // We use double link for fast checking
        m_referrerFriends[Id1] = Id2;
        m_referredFriends[Id2] = Id1;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded " UI64FMTD " Referred Friends", result->GetRowCount());
}

RAFLinkStatus ObjectMgr::GetRAFLinkStatus (uint64 account, uint64* linked) const
{
    RAFLinkStatus status = RAF_LINK_NONE;
    ReferFriendMap::const_iterator i = m_referrerFriends.find(account);

    if (i != m_referrerFriends.end())
        status = RAF_LINK_REFERRER;
    else
    {
        i = m_referredFriends.find(account);
        if (i != m_referredFriends.end())
            status = RAF_LINK_REFERRED;
    }

    if (linked)
        *linked = (status == RAF_LINK_NONE) ? RAF_LINK_NONE : i->second;

    return status;
}

RAFLinkStatus ObjectMgr::GetRAFLinkStatus (uint64 AccOne, uint64 AccTwo) const
{
    ReferFriendMap::const_iterator i = m_referrerFriends.find(AccOne), j;
    if (i != m_referrerFriends.end())
    {
        j = m_referredFriends.find(AccTwo);
        if (j == m_referredFriends.end())
            return RAF_LINK_NONE;

        if (i->first  == j->second &&
            i->second == j->first)
            return RAF_LINK_REFERRER;

        return RAF_LINK_NONE;
    }

    i = m_referredFriends.find(AccOne);
    if (i != m_referredFriends.end())
    {
        j = m_referrerFriends.find(AccTwo);
        if (j == m_referrerFriends.end())
            return RAF_LINK_NONE;

        if (i->first  == j->second &&
            i->second == j->first)
            return RAF_LINK_REFERRED;

        // fall down
    }

    return RAF_LINK_NONE;
}


#define UPDATE_PLAYER(plr)                         \
    if (Player* player = plr)                      \
    {                                              \
        player->SetRAFStatus(RAF_LINK_NONE);       \
        player->SetRestBonus(player->GetRestBonus()); \
    }

void ObjectMgr::LinkIntoRAF(uint64 AccOne, uint64 AccTwo)
{
    m_referrerFriends[AccOne] = AccTwo;
    m_referredFriends[AccTwo] = AccOne;
    LoginDatabase.PExecute("REPLACE INTO account_referred (id1, id2) VALUES (" UI64FMTD ", " UI64FMTD ")", AccOne, AccTwo);
    UPDATE_PLAYER(ObjectAccessor::Instance().FindPlayerByAccountId(AccOne))
    UPDATE_PLAYER(ObjectAccessor::Instance().FindPlayerByAccountId(AccTwo))
}

void ObjectMgr::UnlinkFromRAF(uint64 AccOne)
{
    uint64 AccTwo;
    switch (GetRAFLinkStatus(AccOne, &AccTwo))
    {
    case RAF_LINK_NONE:
        return;
    case RAF_LINK_REFERRER:
        m_referrerFriends.erase(m_referrerFriends.find(AccOne));
        m_referredFriends.erase(m_referredFriends.find(AccTwo));
        LoginDatabase.PExecute("DELETE FROM account_referred WHERE id1 = " UI64FMTD " AND id2 = " UI64FMTD "", AccOne, AccTwo);
        UPDATE_PLAYER(ObjectAccessor::Instance().FindPlayerByAccountId(AccOne))
        UPDATE_PLAYER(ObjectAccessor::Instance().FindPlayerByAccountId(AccTwo))
        break;
    case RAF_LINK_REFERRED:
        m_referrerFriends.erase(m_referrerFriends.find(AccTwo));
        m_referredFriends.erase(m_referredFriends.find(AccOne));
        LoginDatabase.PExecute("DELETE FROM account_referred WHERE id1 = " UI64FMTD " AND id2 = " UI64FMTD "", AccTwo, AccOne);
        UPDATE_PLAYER(ObjectAccessor::Instance().FindPlayerByAccountId(AccOne))
        UPDATE_PLAYER(ObjectAccessor::Instance().FindPlayerByAccountId(AccTwo))
        break;
    }
}

#undef UPDATE_PLAYER

Player* ObjectMgr::GetRAFLinkedBuddyForPlayer(const Player* plr1) const
{
    ReferFriendMap::const_iterator it = m_referrerFriends.find(plr1->GetSession()->GetAccountId());
    if (it == m_referrerFriends.end())
    {
        it = m_referredFriends.find(plr1->GetSession()->GetAccountId());
        if (it == m_referredFriends.end())
            return NULL;
    }

    return ObjectAccessor::Instance().FindPlayerByAccountId(it->second);
}

void ObjectMgr::LoadGroups()
{
    // -- loading groups --
    Group* group = NULL;
    uint64 leaderGuid = 0;
    uint32 count = 0;
    //                                                           0         1              2           3           4              5      6      7      8      9      10     11     12     13      14          15
    QueryResult_AutoPtr result = CharacterDatabase.Query("SELECT mainTank, mainAssistant, lootMethod, looterGuid, lootThreshold, icon1, icon2, icon3, icon4, icon5, icon6, icon7, icon8, isRaid, difficulty, leaderGuid FROM groups");

    if (!result)
    {


        sLog.outString(">> Loaded %u group definitions", count);
        return;
    }


    do
    {
        Field* fields = result->Fetch();
        ++count;
        leaderGuid = MAKE_NEW_GUID(fields[15].GetUInt32(), 0, HIGHGUID_PLAYER);

        group = new Group;
        if (!group->LoadGroupFromDB(leaderGuid, result, false))
        {
            group->Disband();
            delete group;
            continue;
        }
        AddGroup(group);
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u group definitions", count);

    // -- loading members --
    count = 0;
    group = NULL;
    leaderGuid = 0;
    //                                        0           1          2         3
    result = CharacterDatabase.Query("SELECT memberGuid, assistant, subgroup, leaderGuid FROM group_member ORDER BY leaderGuid");
    if (!result)
    {
    }
    else
    {
        do
        {
            Field* fields = result->Fetch();
            count++;
            leaderGuid = MAKE_NEW_GUID(fields[3].GetUInt32(), 0, HIGHGUID_PLAYER);
            if (!group || group->GetLeaderGUID() != leaderGuid)
            {
                group = GetGroupByLeader(leaderGuid);
                if (!group)
                {
                    sLog.outErrorDb("Incorrect entry in group_member table : no group with leader %d for member %d!", fields[3].GetUInt32(), fields[0].GetUInt32());
                    CharacterDatabase.PExecute("DELETE FROM group_member WHERE memberGuid = '%d'", fields[0].GetUInt32());
                    continue;
                }
            }

            if (!group->LoadMemberFromDB(fields[0].GetUInt32(), fields[2].GetUInt8(), fields[1].GetBool()))
            {
                sLog.outErrorDb("Incorrect entry in group_member table : member %d cannot be added to player %d's group!", fields[0].GetUInt32(), fields[3].GetUInt32());
                CharacterDatabase.PExecute("DELETE FROM group_member WHERE memberGuid = '%d'", fields[0].GetUInt32());
            }
        }
        while (result->NextRow());
    }

    // clean groups
    // @todo maybe delete from the DB before loading in this case
    for (GroupSet::iterator itr = mGroupSet.begin(); itr != mGroupSet.end();)
    {
        if ((*itr)->GetMembersCount() < 2)
        {
            (*itr)->Disband();
            delete *itr;
            mGroupSet.erase(itr++);
        }
        else
            ++itr;
    }

    // -- loading instances --
    count = 0;
    group = NULL;
    leaderGuid = 0;
    result = CharacterDatabase.Query(
                 //      0           1    2         3          4           5
                 "SELECT leaderGuid, map, instance, permanent, difficulty, resettime, "
                 // 6
                 "(SELECT COUNT(*) FROM character_instance WHERE guid = leaderGuid AND instance = group_instance.instance AND permanent = 1 LIMIT 1) "
                 "FROM group_instance LEFT JOIN instance ON instance = id ORDER BY leaderGuid"
             );

    if (!result)
    {
    }
    else
    {
        do
        {
            Field* fields = result->Fetch();
            count++;
            leaderGuid = MAKE_NEW_GUID(fields[0].GetUInt32(), 0, HIGHGUID_PLAYER);
            if (!group || group->GetLeaderGUID() != leaderGuid)
            {
                group = GetGroupByLeader(leaderGuid);
                if (!group)
                {
                    sLog.outErrorDb("Incorrect entry in group_instance table : no group with leader %d", fields[0].GetUInt32());
                    continue;
                }
            }

            MapEntry const* mapEntry = sMapStore.LookupEntry(fields[1].GetUInt32());
            if (!mapEntry || !mapEntry->IsDungeon())
            {
                sLog.outErrorDb("Incorrect entry in group_instance table : no dungeon map %d", fields[1].GetUInt32());
                continue;
            }

            InstanceSave* save = sInstanceSaveMgr.AddInstanceSave(mapEntry->MapID, fields[2].GetUInt32(), (DungeonDifficulties)fields[4].GetUInt8(), (time_t)fields[5].GetUInt64(), (fields[6].GetUInt32() == 0), true);
            group->BindToInstance(save, fields[3].GetBool(), true);
        }
        while (result->NextRow());
    }

    sLog.outString(">> Loaded %u group-instance binds total", count);

    sLog.outString(">> Loaded %u group members total", count);
}

void ObjectMgr::LoadQuests()
{
    // For reload case
    for (QuestMap::const_iterator itr = mQuestTemplates.begin(); itr != mQuestTemplates.end(); ++itr)
        delete itr->second;
    mQuestTemplates.clear();

    mExclusiveQuestGroups.clear();

    //                                                       0      1       2           3         4           5     6                7              8              9
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry, Method, ZoneOrSort, MinLevel, QuestLevel, Type, RequiredClasses, RequiredRaces, RequiredSkill, RequiredSkillValue,"
                                 //   10                    11                 12                     13                   14                     15                   16                17
                                 "RepObjectiveFaction, RepObjectiveValue, RequiredMinRepFaction, RequiredMinRepValue, RequiredMaxRepFaction, RequiredMaxRepValue, SuggestedPlayers, LimitTime,"
                                 //   18          19            20           21           22           23              24                25         26            27
                                 "QuestFlags, SpecialFlags, CharTitleId, PrevQuestId, NextQuestId, ExclusiveGroup, NextQuestInChain, SrcItemId, SrcItemCount, SrcSpell,"
                                 //   28     29       30          31               32                33       34              35              36              37
                                 "Title, Details, Objectives, OfferRewardText, RequestItemsText, EndText, ObjectiveText1, ObjectiveText2, ObjectiveText3, ObjectiveText4,"
                                 //   38          39          40          41          42             43             44             45
                                 "ReqItemId1, ReqItemId2, ReqItemId3, ReqItemId4, ReqItemCount1, ReqItemCount2, ReqItemCount3, ReqItemCount4,"
                                 //   46            47            48            49            50               51               52               53
                                 "ReqSourceId1, ReqSourceId2, ReqSourceId3, ReqSourceId4, ReqSourceCount1, ReqSourceCount2, ReqSourceCount3, ReqSourceCount4,"
                                 //   54                  55                  56                  57                  58                     59                     60                     61
                                 "ReqCreatureOrGOId1, ReqCreatureOrGOId2, ReqCreatureOrGOId3, ReqCreatureOrGOId4, ReqCreatureOrGOCount1, ReqCreatureOrGOCount2, ReqCreatureOrGOCount3, ReqCreatureOrGOCount4,"
                                 //   62                63                64                65 
                                 "ReqSpellCast1, ReqSpellCast2, ReqSpellCast3, ReqSpellCast4,"                                 
                                 //   66             67             68             69   70                   71                   72
                                 "RewChoiceItemId1, RewChoiceItemId2, RewChoiceItemId3, RewChoiceItemId4, RewChoiceItemId5, RewChoiceItemId6,"
                                 //   73                74                  75                   76                   78                   79
                                 "RewChoiceItemCount1, RewChoiceItemCount2, RewChoiceItemCount3, RewChoiceItemCount4, RewChoiceItemCount5, RewChoiceItemCount6,"
                                 //   80      81          82          83          84             85             86             87
                                 "RewItemId1, RewItemId2, RewItemId3, RewItemId4, RewItemCount1, RewItemCount2, RewItemCount3, RewItemCount4,"
                                 //   88          89              90              91              92
                                 "RewRepFaction1, RewRepFaction2, RewRepFaction3, RewRepFaction4, RewRepFaction5, RewRepValue1, RewRepValue2, RewRepValue3, RewRepValue4, RewRepValue5,"
                                 //   93                94           95                96      97            98               99               100         101     102    103
                                 "RewHonorableKills, RewOrReqMoney, RewMoneyMaxLevel, RewSpell, RewSpellCast, RewMailTemplateId, RewMailDelaySecs, PointMapId, PointX, PointY, PointOpt,"
                                 //   104            105            106            107            108              109            110                111                112                113
                                 "DetailsEmote1, DetailsEmote2, DetailsEmote3, DetailsEmote4, IncompleteEmote, CompleteEmote, OfferRewardEmote1, OfferRewardEmote2, OfferRewardEmote3, OfferRewardEmote4,"
                                 //   114          115
                                 "StartScript, CompleteScript"
                                 " FROM quest_template");

    if (!result)
    {

        sLog.outString(">> Loaded 0 quests definitions");
        sLog.outErrorDb("quest_template table is empty!");
        return;
    }

    // create multimap previous quest for each existed quest
    // some quests can have many previous maps set by NextQuestId in previous quest
    // for example set of race quests can lead to single not race specific quest
    do
    {
        Field* fields = result->Fetch();

        Quest* newQuest = new Quest(fields);
        mQuestTemplates[newQuest->GetQuestId()] = newQuest;
    }
    while (result->NextRow());

    // Post processing
    for (QuestMap::iterator iter = mQuestTemplates.begin(); iter != mQuestTemplates.end(); ++iter)
    {
        // skip post-loading checks for disabled quests
        if (sDisableMgr.IsDisabledFor(DISABLE_TYPE_QUEST, iter->first, NULL))
            continue;

        Quest* qinfo = iter->second;

        // additional quest integrity checks (GO, creature_template and item_template must be loaded already)

        if (qinfo->GetQuestMethod() >= 3)
            sLog.outErrorDb("Quest %u has Method = %u, expected values are 0, 1 or 2.", qinfo->GetQuestId(), qinfo->GetQuestMethod());

        if (qinfo->SpecialFlags & ~QUEST_SPECIAL_FLAGS_DB_ALLOWED)
        {
            sLog.outErrorDb("Quest %u has SpecialFlags = %u > max allowed value. Correct SpecialFlags to value <= %u",
                            qinfo->GetQuestId(), qinfo->SpecialFlags, QUEST_SPECIAL_FLAGS_DB_ALLOWED);
            qinfo->SpecialFlags &= QUEST_SPECIAL_FLAGS_DB_ALLOWED;
        }

        if (qinfo->QuestFlags & QUEST_FLAGS_DAILY)
        {
            if (!(qinfo->SpecialFlags & QUEST_SPECIAL_FLAGS_REPEATABLE))
            {
                sLog.outErrorDb("Daily Quest %u not marked as repeatable in SpecialFlags, added.", qinfo->GetQuestId());
                qinfo->SpecialFlags |= QUEST_SPECIAL_FLAGS_REPEATABLE;
            }
        }

        if (qinfo->QuestFlags & QUEST_FLAGS_AUTO_REWARDED)
        {
            // at auto-reward can be rewarded only RewChoiceItemId[0]
            for (int j = 1; j < QUEST_REWARD_CHOICES_COUNT; ++j)
            {
                if (uint32 id = qinfo->RewChoiceItemId[j])
                {
                    sLog.outErrorDb("Quest %u has RewChoiceItemId%d = %u but item from RewChoiceItemId%d can't be rewarded with quest flag QUEST_FLAGS_AUTO_REWARDED.",
                                    qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes, quest ignore this data
                }
            }
        }

        // client quest log visual (area case)
        if (qinfo->ZoneOrSort > 0)
        {
            if (!GetAreaEntryByAreaID(qinfo->ZoneOrSort))
            {
                sLog.outErrorDb("Quest %u has ZoneOrSort = %u (zone case) but zone with this id does not exist.",
                                qinfo->GetQuestId(), qinfo->ZoneOrSort);
                // no changes, quest not dependent from this value but can have problems at client
            }
        }
        // client quest log visual (sort case)
        if (qinfo->ZoneOrSort < 0)
        {
            QuestSortEntry const* qSort = sQuestSortStore.LookupEntry(-int32(qinfo->ZoneOrSort));
            if (!qSort)
            {
                sLog.outErrorDb("Quest %u has ZoneOrSort = %i (sort case) but quest sort with this id does not exist.",
                                qinfo->GetQuestId(), qinfo->ZoneOrSort);
                // no changes, quest not dependent from this value but can have problems at client (note some may be 0, we must allow this so no check)
            }
            //check for proper RequiredSkill value (skill case)
            if (int32 skill_id =  SkillByQuestSort(-int32(qinfo->ZoneOrSort)))
            {
                // skill is positive value in SkillOrClass
                if (qinfo->RequiredSkill != skill_id)
                {
                    sLog.outErrorDb("Quest %u has ZoneOrSort = %i but `RequiredSkill` does not have a corresponding value (%i).",
                                    qinfo->GetQuestId(), qinfo->ZoneOrSort, skill_id);
                    //override, and force proper value here?
                }
            }
        }

        // RequiredClasses, can be 0/CLASSMASK_ALL_PLAYABLE to allow any class
        if (qinfo->RequiredClasses)
        {
            if (!(qinfo->RequiredClasses & CLASSMASK_ALL_PLAYABLE))
            {
                sLog.outErrorDb("Quest %u does not contain any playable classes in `RequiredClasses` (%u), value set to 0 (all classes).", qinfo->GetQuestId(), qinfo->RequiredClasses);
                qinfo->RequiredClasses = 0;
            }
        }

        // RequiredRaces, can be 0/RACEMASK_ALL_PLAYABLE to allow any race
        if (qinfo->RequiredRaces)
        {
            if (!(qinfo->RequiredRaces & RACEMASK_ALL_PLAYABLE))
            {
                sLog.outErrorDb("Quest %u does not contain any playable races in `RequiredRaces` (%u), value set to 0 (all races).", qinfo->GetQuestId(), qinfo->RequiredRaces);
                qinfo->RequiredRaces = 0;
            }
        }

        // RequiredSkill, can be 0
        if (qinfo->RequiredSkill)
        {
            if (!sSkillLineStore.LookupEntry(qinfo->RequiredSkill))
            {
                sLog.outErrorDb("Quest %u has `RequiredSkill` = %u but this skill does not exist",
                                qinfo->GetQuestId(), qinfo->RequiredSkill);
            }
        }

        if (qinfo->RequiredSkillValue)
        {
            if (qinfo->RequiredSkillValue > sWorld.GetConfigMaxSkillValue())
            {
                sLog.outErrorDb("Quest %u has RequiredSkillValue = %u but max possible skill is %u, quest cannot be completed.",
                                qinfo->GetQuestId(), qinfo->RequiredSkillValue, sWorld.GetConfigMaxSkillValue());
                // no changes, quest can't be done for this requirement
            }
        }
        // else Skill quests can have 0 skill level, this is ok

        if (qinfo->RepObjectiveFaction && !sFactionStore.LookupEntry(qinfo->RepObjectiveFaction))
        {
            sLog.outErrorDb("Quest %u has RepObjectiveFaction = %u but faction template %u does not exist, quest cannot be completed.",
                            qinfo->GetQuestId(), qinfo->RepObjectiveFaction, qinfo->RepObjectiveFaction);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->RequiredMinRepFaction && !sFactionStore.LookupEntry(qinfo->RequiredMinRepFaction))
        {
            sLog.outErrorDb("Quest %u has RequiredMinRepFaction = %u but faction template %u does not exist, quest cannot be completed.",
                            qinfo->GetQuestId(), qinfo->RequiredMinRepFaction, qinfo->RequiredMinRepFaction);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->RequiredMaxRepFaction && !sFactionStore.LookupEntry(qinfo->RequiredMaxRepFaction))
        {
            sLog.outErrorDb("Quest %u has RequiredMaxRepFaction = %u but faction template %u does not exist, quest cannot be completed.",
                            qinfo->GetQuestId(), qinfo->RequiredMaxRepFaction, qinfo->RequiredMaxRepFaction);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->RequiredMinRepValue && qinfo->RequiredMinRepValue > Player::Reputation_Cap)
        {
            sLog.outErrorDb("Quest %u has RequiredMinRepValue = %d but max reputation is %u, quest cannot be completed.",
                            qinfo->GetQuestId(), qinfo->RequiredMinRepValue, Player::Reputation_Cap);
            // no changes, quest can't be done for this requirement
        }

        if (qinfo->RequiredMinRepValue && qinfo->RequiredMaxRepValue && qinfo->RequiredMaxRepValue <= qinfo->RequiredMinRepValue)
        {
            sLog.outErrorDb("Quest %u has RequiredMaxRepValue = %d and RequiredMinRepValue = %d, quest cannot be completed.",
                            qinfo->GetQuestId(), qinfo->RequiredMaxRepValue, qinfo->RequiredMinRepValue);
            // no changes, quest can't be done for this requirement
        }

        if (!qinfo->RepObjectiveFaction && qinfo->RepObjectiveValue > 0)
        {
            sLog.outErrorDb("Quest %u has RepObjectiveValue = %d but RepObjectiveFaction is 0, value has no effect",
                            qinfo->GetQuestId(), qinfo->RepObjectiveValue);
            // warning
        }

        if (!qinfo->RequiredMinRepFaction && qinfo->RequiredMinRepValue > 0)
        {
            sLog.outErrorDb("Quest %u has RequiredMinRepValue = %d but RequiredMinRepFaction is 0, value has no effect",
                            qinfo->GetQuestId(), qinfo->RequiredMinRepValue);
            // warning
        }

        if (!qinfo->RequiredMaxRepFaction && qinfo->RequiredMaxRepValue > 0)
        {
            sLog.outErrorDb("Quest %u has RequiredMaxRepValue = %d but RequiredMaxRepFaction is 0, value has no effect",
                            qinfo->GetQuestId(), qinfo->RequiredMaxRepValue);
            // warning
        }

        if (qinfo->CharTitleId && !sCharTitlesStore.LookupEntry(qinfo->CharTitleId))
        {
            sLog.outErrorDb("Quest %u has CharTitleId = %u but CharTitle Id %u does not exist, quest will not reward title.",
                            qinfo->GetQuestId(), qinfo->GetCharTitleId(), qinfo->GetCharTitleId());
            qinfo->CharTitleId = 0;
            // quest can't reward this title
        }

        if (qinfo->SrcItemId)
        {
            if (!sItemStorage.LookupEntry<ItemPrototype>(qinfo->SrcItemId))
            {
                sLog.outErrorDb("Quest %u has SrcItemId = %u but item with entry %u does not exist, quest cannot be completed.",
                                qinfo->GetQuestId(), qinfo->SrcItemId, qinfo->SrcItemId);
                qinfo->SrcItemId = 0;                       // quest can't be done for this requirement
            }
            else if (qinfo->SrcItemCount == 0)
            {
                sLog.outErrorDb("Quest %u has SrcItemId = %u but SrcItemCount = 0, set to 1 but needs fixed in DB.",
                                qinfo->GetQuestId(), qinfo->SrcItemId);
                qinfo->SrcItemCount = 1;                    // update to 1 for allow quest work for backward compatibility with DB
            }
        }
        else if (qinfo->SrcItemCount > 0)
        {
            sLog.outErrorDb("Quest %u has SrcItemId = 0 but SrcItemCount = %u, useless value.",
                            qinfo->GetQuestId(), qinfo->SrcItemCount);
            qinfo->SrcItemCount = 0;                        // no quest work changes in fact
        }

        if (qinfo->SrcSpell)
        {
            SpellEntry const* spellInfo = sSpellStore.LookupEntry(qinfo->SrcSpell);
            if (!spellInfo)
            {
                sLog.outErrorDb("Quest %u has SrcSpell = %u but spell %u doesn't exist, quest cannot be completed.",
                                qinfo->GetQuestId(), qinfo->SrcSpell, qinfo->SrcSpell);
                qinfo->SrcSpell = 0;                        // quest can't be done for this requirement
            }
            else if (!SpellMgr::IsSpellValid(spellInfo))
            {
                sLog.outErrorDb("Quest %u has SrcSpell = %u but spell %u is broken, quest cannot be completed.",
                                qinfo->GetQuestId(), qinfo->SrcSpell, qinfo->SrcSpell);
                qinfo->SrcSpell = 0;                        // quest can't be done for this requirement
            }
        }

        for (int j = 0; j < QUEST_OBJECTIVES_COUNT; ++j)
        {
            uint32 id = qinfo->ReqItemId[j];
            if (id)
            {
                if (qinfo->ReqItemCount[j] == 0)
                {
                    sLog.outErrorDb("Quest %u has ReqItemId%d = %u but ReqItemCount%d = 0, quest cannot be completed.",
                                    qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes, quest can't be done for this requirement
                }

                qinfo->SetSpecialFlag(QUEST_SPECIAL_FLAGS_DELIVER);

                if (!sItemStorage.LookupEntry<ItemPrototype>(id))
                {
                    sLog.outErrorDb("Quest %u has ReqItemId%d = %u but item with entry %u does not exist, quest cannot be completed.",
                                    qinfo->GetQuestId(), j + 1, id, id);
                    qinfo->ReqItemCount[j] = 0;             // prevent incorrect work of quest
                }
            }
            else if (qinfo->ReqItemCount[j] > 0)
            {
                sLog.outErrorDb("Quest %u has ReqItemId%d = 0 but ReqItemCount%d = %u, quest cannot be completed.",
                                qinfo->GetQuestId(), j + 1, j + 1, qinfo->ReqItemCount[j]);
                qinfo->ReqItemCount[j] = 0;                 // prevent incorrect work of quest
            }
        }

        for (int j = 0; j < QUEST_SOURCE_ITEM_IDS_COUNT; ++j)
        {
            uint32 id = qinfo->ReqSourceId[j];
            if (id)
            {
                if (!sItemStorage.LookupEntry<ItemPrototype>(id))
                {
                    sLog.outErrorDb("Quest %u has ReqSourceId%d = %u but item with entry %u does not exist, quest cannot be completed.",
                                    qinfo->GetQuestId(), j + 1, id, id);
                    // no changes, quest can't be done for this requirement
                }
            }
            else
            {
                if (qinfo->ReqSourceCount[j] > 0)
                {
                    sLog.outErrorDb("Quest %u has ReqSourceId%d = 0 but ReqSourceCount%d = %u.",
                                    qinfo->GetQuestId(), j + 1, j + 1, qinfo->ReqSourceCount[j]);
                    // no changes, quest ignore this data
                }
            }
        }

        for (int j = 0; j < QUEST_OBJECTIVES_COUNT; ++j)
        {
            uint32 id = qinfo->ReqSpell[j];
            if (id)
            {
                SpellEntry const* spellInfo = sSpellStore.LookupEntry(id);
                if (!spellInfo)
                {
                    sLog.outErrorDb("Quest %u has ReqSpellCast%d = %u but spell %u does not exist, quest cannot be completed.",
                                    qinfo->GetQuestId(), j + 1, id, id);
                    // no changes, quest can't be done for this requirement
                }

                if (!qinfo->ReqCreatureOrGOId[j])
                {
                    bool found = false;
                    for (int k = 0; k < 3; ++k)
                    {
                        if ((spellInfo->Effect[k] == SPELL_EFFECT_QUEST_COMPLETE && uint32(spellInfo->EffectMiscValue[k]) == qinfo->QuestId) ||
                            spellInfo->Effect[k] == SPELL_EFFECT_SEND_EVENT)
                        {
                            found = true;
                            break;
                        }
                    }

                    if (found)
                    {
                        if (!qinfo->HasSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT))
                        {
                            sLog.outErrorDb("Spell (id: %u) has SPELL_EFFECT_QUEST_COMPLETE or SPELL_EFFECT_SEND_EVENT for quest %u and ReqCreatureOrGOId%d = 0, but quest does not have flag QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT. Quest flags or ReqCreatureOrGOId%d must be fixed, quest modified to enable objective.", spellInfo->Id, qinfo->QuestId, j + 1, j + 1);

                            // this will prevent quest completing without objective
                            //const_cast<Quest*>(qinfo)->SetFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT);
                        }
                    }
                    else
                    {
                        sLog.outErrorDb("Quest %u has ReqSpellCast%d = %u and ReqCreatureOrGOId%d = 0 but spell %u does not have SPELL_EFFECT_QUEST_COMPLETE or SPELL_EFFECT_SEND_EVENT effect for this quest, quest cannot be completed.",
                                        qinfo->GetQuestId(), j + 1, id, j + 1, id);
                        // no changes, quest can't be done for this requirement
                    }
                }
            }
        }

        for (int j = 0; j < QUEST_OBJECTIVES_COUNT; ++j)
        {
            int32 id = qinfo->ReqCreatureOrGOId[j];
            if (id < 0 && !sGOStorage.LookupEntry<GameObjectInfo>(-id))
            {
                sLog.outErrorDb("Quest %u has ReqCreatureOrGOId%d = %i but gameobject %u does not exist, quest cannot be completed.",
                                qinfo->GetQuestId(), j + 1, id, uint32(-id));
                qinfo->ReqCreatureOrGOId[j] = 0;            // quest can't be done for this requirement
            }

            if (id > 0 && !sCreatureStorage.LookupEntry<CreatureInfo>(id))
            {
                sLog.outErrorDb("Quest %u has ReqCreatureOrGOId%d = %i but creature with entry %u does not exist, quest cannot be completed.",
                                qinfo->GetQuestId(), j + 1, id, uint32(id));
                qinfo->ReqCreatureOrGOId[j] = 0;            // quest can't be done for this requirement
            }

            if (id)
            {
                // In fact SpeakTo and Kill are quite same: either you can speak to mob:SpeakTo or you can't:Kill/Cast

                qinfo->SetSpecialFlag(QUEST_SPECIAL_FLAGS_KILL | QUEST_SPECIAL_FLAGS_CAST | QUEST_SPECIAL_FLAGS_SPEAKTO);

                if (!qinfo->ReqCreatureOrGOCount[j])
                {
                    sLog.outErrorDb("Quest %u has ReqCreatureOrGOId%d = %u but ReqCreatureOrGOCount%d = 0, quest cannot be completed.",
                                    qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes, quest can be incorrectly done, but we already report this
                }
            }
            else if (qinfo->ReqCreatureOrGOCount[j] > 0)
            {
                sLog.outErrorDb("Quest %u has ReqCreatureOrGOId%d = 0 but ReqCreatureOrGOCount%d = %u.",
                                qinfo->GetQuestId(), j + 1, j + 1, qinfo->ReqCreatureOrGOCount[j]);
                // no changes, quest ignore this data
            }
        }

        for (int j = 0; j < QUEST_REWARD_CHOICES_COUNT; ++j)
        {
            uint32 id = qinfo->RewChoiceItemId[j];
            if (id)
            {
                if (!sItemStorage.LookupEntry<ItemPrototype>(id))
                {
                    sLog.outErrorDb("Quest %u has RewChoiceItemId%d = %u but item with entry %u does not exist, quest will not reward this item.",
                                    qinfo->GetQuestId(), j + 1, id, id);
                    qinfo->RewChoiceItemId[j] = 0;          // no changes, quest will not reward this
                }

                if (!qinfo->RewChoiceItemCount[j])
                {
                    sLog.outErrorDb("Quest %u has RewChoiceItemId%d = %u but RewChoiceItemCount%d = 0, quest cannot be completed.",
                                    qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes, quest can't be done
                }
            }
            else if (qinfo->RewChoiceItemCount[j] > 0)
            {
                sLog.outErrorDb("Quest %u has RewChoiceItemId%d = 0 but RewChoiceItemCount%d = %u.",
                                qinfo->GetQuestId(), j + 1, j + 1, qinfo->RewChoiceItemCount[j]);
                // no changes, quest ignore this data
            }
        }

        for (int j = 0; j < QUEST_REWARDS_COUNT; ++j)
        {
            uint32 id = qinfo->RewItemId[j];
            if (id)
            {
                if (!sItemStorage.LookupEntry<ItemPrototype>(id))
                {
                    sLog.outErrorDb("Quest %u has RewItemId%d = %u but item with entry %u does not exist, quest will not reward this item.",
                                    qinfo->GetQuestId(), j + 1, id, id);
                    qinfo->RewItemId[j] = 0;                // no changes, quest will not reward this item
                }

                if (!qinfo->RewItemCount[j])
                {
                    sLog.outErrorDb("Quest %u has RewItemId%d = %u but RewItemCount%d = 0, quest will not reward this item.",
                                    qinfo->GetQuestId(), j + 1, id, j + 1);
                    // no changes
                }
            }
            else if (qinfo->RewItemCount[j] > 0)
            {
                sLog.outErrorDb("Quest %u has RewItemId%d = 0 but RewItemCount%d = %u.",
                                qinfo->GetQuestId(), j + 1, j + 1, qinfo->RewItemCount[j]);
                // no changes, quest ignore this data
            }
        }

        for (int j = 0; j < QUEST_REPUTATIONS_COUNT; ++j)
        {
            if (qinfo->RewRepFaction[j])
            {
                if (!qinfo->RewRepValue[j])
                {
                    sLog.outErrorDb("Quest %u has RewRepFaction%d = %u but RewRepValue%d = 0, quest will not reward this reputation.",
                                    qinfo->GetQuestId(), j + 1, qinfo->RewRepValue[j], j + 1);
                    // no changes
                }

                if (!sFactionStore.LookupEntry(qinfo->RewRepFaction[j]))
                {
                    sLog.outErrorDb("Quest %u has RewRepFaction%d = %u but raw faction (faction.dbc) %u does not exist, quest will not reward reputation for this faction.",
                                    qinfo->GetQuestId(), j + 1, qinfo->RewRepFaction[j] , qinfo->RewRepFaction[j]);
                    qinfo->RewRepFaction[j] = 0;            // quest will not reward this
                }
            }
            else if (qinfo->RewRepValue[j] != 0)
            {
                sLog.outErrorDb("Quest %u has RewRepFaction%d = 0 but RewRepValue%d = %u.",
                                qinfo->GetQuestId(), j + 1, j + 1, qinfo->RewRepValue[j]);
                // no changes, quest ignore this data
            }
        }

        if (qinfo->RewSpell)
        {
            SpellEntry const* spellInfo = sSpellStore.LookupEntry(qinfo->RewSpell);

            if (!spellInfo)
            {
                sLog.outErrorDb("Quest %u has RewSpell = %u but spell %u does not exist, spell removed as display reward.",
                                qinfo->GetQuestId(), qinfo->RewSpell, qinfo->RewSpell);
                qinfo->RewSpell = 0;                        // no spell reward will display for this quest
            }

            else if (!SpellMgr::IsSpellValid(spellInfo))
            {
                sLog.outErrorDb("Quest %u has RewSpell = %u but spell %u is broken, quest cannot be completed.",
                                qinfo->GetQuestId(), qinfo->RewSpell, qinfo->RewSpell);
                qinfo->RewSpell = 0;                        // no spell reward will display for this quest
            }

        }

        if (qinfo->RewSpellCast)
        {
            SpellEntry const* spellInfo = sSpellStore.LookupEntry(qinfo->RewSpellCast);

            if (!spellInfo)
            {
                sLog.outErrorDb("Quest %u has RewSpellCast = %u but spell %u does not exist, quest will not have a spell reward.",
                                qinfo->GetQuestId(), qinfo->RewSpellCast, qinfo->RewSpellCast);
                qinfo->RewSpellCast = 0;                    // no spell will be casted on player
            }

            else if (!SpellMgr::IsSpellValid(spellInfo))
            {
                sLog.outErrorDb("Quest %u has RewSpellCast = %u but spell %u is broken, quest cannot be completed.",
                                qinfo->GetQuestId(), qinfo->RewSpellCast, qinfo->RewSpellCast);
                qinfo->RewSpellCast = 0;                    // no spell will be casted on player
            }

        }

        if (qinfo->RewMailTemplateId)
        {
            if (!sMailTemplateStore.LookupEntry(qinfo->RewMailTemplateId))
            {
                sLog.outErrorDb("Quest %u has RewMailTemplateId = %u but mail template  %u does not exist, quest will not have a mail reward.",
                                qinfo->GetQuestId(), qinfo->RewMailTemplateId, qinfo->RewMailTemplateId);
                qinfo->RewMailTemplateId = 0;               // no mail will send to player
                qinfo->RewMailDelaySecs = 0;                // no mail will send to player
            }
        }

        if (qinfo->NextQuestInChain)
        {
            if (mQuestTemplates.find(qinfo->NextQuestInChain) == mQuestTemplates.end())
            {
                sLog.outErrorDb("Quest %u has NextQuestInChain = %u but quest %u does not exist, quest chain will not work.",
                                qinfo->GetQuestId(), qinfo->NextQuestInChain , qinfo->NextQuestInChain);
                qinfo->NextQuestInChain = 0;
            }
            else
                mQuestTemplates[qinfo->NextQuestInChain]->prevChainQuests.push_back(qinfo->GetQuestId());
        }

        // fill additional data stores
        if (qinfo->PrevQuestId)
        {
            if (mQuestTemplates.find(abs(qinfo->GetPrevQuestId())) == mQuestTemplates.end())
                sLog.outErrorDb("Quest %d has PrevQuestId %i, but no such quest", qinfo->GetQuestId(), qinfo->GetPrevQuestId());
            else
                qinfo->prevQuests.push_back(qinfo->PrevQuestId);
        }

        if (qinfo->NextQuestId)
        {
            if (mQuestTemplates.find(abs(qinfo->GetNextQuestId())) == mQuestTemplates.end())
                sLog.outErrorDb("Quest %d has NextQuestId %i, but no such quest", qinfo->GetQuestId(), qinfo->GetNextQuestId());
            else
            {
                int32 signedQuestId = qinfo->NextQuestId < 0 ? -int32(qinfo->GetQuestId()) : int32(qinfo->GetQuestId());
                mQuestTemplates[abs(qinfo->GetNextQuestId())]->prevQuests.push_back(signedQuestId);
            }
        }

        if (qinfo->ExclusiveGroup)
            mExclusiveQuestGroups.insert(std::pair<int32, uint32>(qinfo->ExclusiveGroup, qinfo->GetQuestId()));
        if (qinfo->LimitTime)
            qinfo->SetSpecialFlag(QUEST_SPECIAL_FLAGS_TIMED);
    }

    // check QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT for spell with SPELL_EFFECT_QUEST_COMPLETE
    for (uint32 i = 0; i < sSpellStore.GetNumRows(); ++i)
    {
        SpellEntry const* spellInfo = sSpellStore.LookupEntry(i);
        if (!spellInfo)
            continue;

        for (int j = 0; j < 3; ++j)
        {
            if (spellInfo->Effect[j] != SPELL_EFFECT_QUEST_COMPLETE)
                continue;

            uint32 quest_id = spellInfo->EffectMiscValue[j];

            Quest const* quest = GetQuestTemplate(quest_id);

            // some quest referenced in spells not exist (outdated spells)
            if (!quest)
                continue;

            if (!quest->HasSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT))
            {
                sLog.outErrorDb("Spell (id: %u) has SPELL_EFFECT_QUEST_COMPLETE for quest %u , but quest does not have flag QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT. Quest flags must be fixed, quest modified to enable objective.", spellInfo->Id, quest_id);

                // this will prevent quest completing without objective
                const_cast<Quest*>(quest)->SetSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT);
            }
        }
    }

    sLog.outString(">> Loaded %u quest definitions", mQuestTemplates.size());
}

void ObjectMgr::LoadQuestLocales()
{
    mQuestLocaleMap.clear();                                // need for reload case

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry,"
                                 "Title_loc1,Details_loc1,Objectives_loc1,OfferRewardText_loc1,RequestItemsText_loc1,EndText_loc1,ObjectiveText1_loc1,ObjectiveText2_loc1,ObjectiveText3_loc1,ObjectiveText4_loc1,"
                                 "Title_loc2,Details_loc2,Objectives_loc2,OfferRewardText_loc2,RequestItemsText_loc2,EndText_loc2,ObjectiveText1_loc2,ObjectiveText2_loc2,ObjectiveText3_loc2,ObjectiveText4_loc2,"
                                 "Title_loc3,Details_loc3,Objectives_loc3,OfferRewardText_loc3,RequestItemsText_loc3,EndText_loc3,ObjectiveText1_loc3,ObjectiveText2_loc3,ObjectiveText3_loc3,ObjectiveText4_loc3,"
                                 "Title_loc4,Details_loc4,Objectives_loc4,OfferRewardText_loc4,RequestItemsText_loc4,EndText_loc4,ObjectiveText1_loc4,ObjectiveText2_loc4,ObjectiveText3_loc4,ObjectiveText4_loc4,"
                                 "Title_loc5,Details_loc5,Objectives_loc5,OfferRewardText_loc5,RequestItemsText_loc5,EndText_loc5,ObjectiveText1_loc5,ObjectiveText2_loc5,ObjectiveText3_loc5,ObjectiveText4_loc5,"
                                 "Title_loc6,Details_loc6,Objectives_loc6,OfferRewardText_loc6,RequestItemsText_loc6,EndText_loc6,ObjectiveText1_loc6,ObjectiveText2_loc6,ObjectiveText3_loc6,ObjectiveText4_loc6,"
                                 "Title_loc7,Details_loc7,Objectives_loc7,OfferRewardText_loc7,RequestItemsText_loc7,EndText_loc7,ObjectiveText1_loc7,ObjectiveText2_loc7,ObjectiveText3_loc7,ObjectiveText4_loc7,"
                                 "Title_loc8,Details_loc8,Objectives_loc8,OfferRewardText_loc8,RequestItemsText_loc8,EndText_loc8,ObjectiveText1_loc8,ObjectiveText2_loc8,ObjectiveText3_loc8,ObjectiveText4_loc8"
                                 " FROM locales_quest"
                                                    );

    if (!result)
    {


        sLog.outString(">> Loaded 0 Quest locale strings. DB table locales_quest is empty.");
        return;
    }


    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        QuestLocale& data = mQuestLocaleMap[entry];

        for (int i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[1 + 10 * (i - 1)].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if (data.Title.size() <= idx)
                        data.Title.resize(idx + 1);

                    data.Title[idx] = str;
                }
            }
            str = fields[1 + 10 * (i - 1) + 1].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if (data.Details.size() <= idx)
                        data.Details.resize(idx + 1);

                    data.Details[idx] = str;
                }
            }
            str = fields[1 + 10 * (i - 1) + 2].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if (data.Objectives.size() <= idx)
                        data.Objectives.resize(idx + 1);

                    data.Objectives[idx] = str;
                }
            }
            str = fields[1 + 10 * (i - 1) + 3].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if (data.OfferRewardText.size() <= idx)
                        data.OfferRewardText.resize(idx + 1);

                    data.OfferRewardText[idx] = str;
                }
            }
            str = fields[1 + 10 * (i - 1) + 4].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if (data.RequestItemsText.size() <= idx)
                        data.RequestItemsText.resize(idx + 1);

                    data.RequestItemsText[idx] = str;
                }
            }
            str = fields[1 + 10 * (i - 1) + 5].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if (data.EndText.size() <= idx)
                        data.EndText.resize(idx + 1);

                    data.EndText[idx] = str;
                }
            }
            for (int k = 0; k < 4; ++k)
            {
                str = fields[1 + 10 * (i - 1) + 6 + k].GetCppString();
                if (!str.empty())
                {
                    int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                    if (idx >= 0)
                    {
                        if (data.ObjectiveText[k].size() <= idx)
                            data.ObjectiveText[k].resize(idx + 1);

                        data.ObjectiveText[k][idx] = str;
                    }
                }
            }
        }
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u Quest locale strings", mQuestLocaleMap.size());
}

void ObjectMgr::LoadPetCreateSpells()
{
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry, Spell1, Spell2, Spell3, Spell4 FROM petcreateinfo_spell");
    if (!result)
    {

        sLog.outString(">> Loaded 0 pet create spells");
        sLog.outErrorDb("petcreateinfo_spell table is empty!");
        return;
    }

    uint32 count = 0;


    mPetCreateSpell.clear();

    do
    {
        Field* fields = result->Fetch();

        uint32 creature_id = fields[0].GetUInt32();

        if (!creature_id || !sCreatureStorage.LookupEntry<CreatureInfo>(creature_id))
            continue;

        PetCreateSpellEntry PetCreateSpell;
        for (int i = 0; i < 4; i++)
        {
            PetCreateSpell.spellid[i] = fields[i + 1].GetUInt32();

            if (PetCreateSpell.spellid[i] && !sSpellStore.LookupEntry(PetCreateSpell.spellid[i]))
                sLog.outErrorDb("Spell %u listed in petcreateinfo_spell does not exist", PetCreateSpell.spellid[i]);
        }

        mPetCreateSpell[creature_id] = PetCreateSpell;

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u pet create spells", count);
}

void ObjectMgr::LoadScripts(ScriptsType type)
{
    ScriptMapMap* scripts = GetScriptsMapByType(type);
    if (!scripts)
        return;

    std::string tableName = GetScriptsTableNameByType(type);
    if (tableName.empty())
        return;

    if (sWorld.IsScriptScheduled())                          // function don't must be called in time scripts use.
        return;

    scripts->clear();                                        // need for reload support

    QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT id,delay,command,datalong,datalong2,dataint, x, y, z, o FROM %s", tableName.c_str());

    uint32 count = 0;

    if (!result)
    {

        sLog.outString(">> Loaded %u script definitions", count);
        return;
    }


    do
    {

        Field* fields = result->Fetch();
        ScriptInfo tmp;
        tmp.type      = type;
        tmp.id           = fields[0].GetUInt32();
        tmp.delay        = fields[1].GetUInt32();
        tmp.command      = ScriptCommands(fields[2].GetUInt32());
        tmp.Raw.nData[0] = fields[3].GetUInt32();
        tmp.Raw.nData[1] = fields[4].GetUInt32();
        tmp.Raw.nData[2] = fields[5].GetInt32();
        tmp.Raw.fData[0] = fields[6].GetFloat();
        tmp.Raw.fData[1] = fields[7].GetFloat();
        tmp.Raw.fData[2] = fields[8].GetFloat();
        tmp.Raw.fData[3] = fields[9].GetFloat();

        // generic command args check
        switch (tmp.command)
        {
        case SCRIPT_COMMAND_TALK:
            {
                if (tmp.Talk.ChatType > CHAT_TYPE_WHISPER && tmp.Talk.ChatType != CHAT_MSG_RAID_BOSS_WHISPER)
                {
                    sLog.outErrorDb("Table `%s` has invalid talk type (datalong = %u) in SCRIPT_COMMAND_TALK for script id %u",
                                    tableName.c_str(), tmp.Talk.ChatType, tmp.id);
                    continue;
                }
                if (!tmp.Talk.TextID)
                {
                    sLog.outErrorDb("Table `%s` has invalid talk text id (dataint = %i) in SCRIPT_COMMAND_TALK for script id %u",
                                    tableName.c_str(), tmp.Talk.TextID, tmp.id);
                    continue;
                }
                if (tmp.Talk.TextID < MIN_DB_SCRIPT_STRING_ID || tmp.Talk.TextID >= MAX_DB_SCRIPT_STRING_ID)
                {
                    sLog.outErrorDb("Table `%s` has out of range text id (dataint = %i expected %u-%u) in SCRIPT_COMMAND_TALK for script id %u",
                                    tableName.c_str(), tmp.Talk.TextID, MIN_DB_SCRIPT_STRING_ID, MAX_DB_SCRIPT_STRING_ID, tmp.id);
                    continue;
                }

                break;
            }

        case SCRIPT_COMMAND_EMOTE:
            {
                if (!sEmotesStore.LookupEntry(tmp.Emote.EmoteID))
                {
                    sLog.outErrorDb("Table `%s` has invalid emote id (datalong = %u) in SCRIPT_COMMAND_EMOTE for script id %u",
                                    tableName.c_str(), tmp.Emote.EmoteID, tmp.id);
                    continue;
                }
                break;
            }

        case SCRIPT_COMMAND_TELEPORT_TO:
            {
                if (!sMapStore.LookupEntry(tmp.TeleportTo.MapID))
                {
                    sLog.outErrorDb("Table `%s` has invalid map (Id: %u) in SCRIPT_COMMAND_TELEPORT_TO for script id %u",
                                    tableName.c_str(), tmp.TeleportTo.MapID, tmp.id);
                    continue;
                }

                if (!Oregon::IsValidMapCoord(tmp.TeleportTo.DestX, tmp.TeleportTo.DestY, tmp.TeleportTo.DestZ, tmp.TeleportTo.Orientation))
                {
                    sLog.outErrorDb("Table `%s` has invalid coordinates (X: %f Y: %f Z: %f O: %f) in SCRIPT_COMMAND_TELEPORT_TO for script id %u",
                                    tableName.c_str(), tmp.TeleportTo.DestX, tmp.TeleportTo.DestY, tmp.TeleportTo.DestZ, tmp.TeleportTo.Orientation, tmp.id);
                    continue;
                }
                break;
            }

        case SCRIPT_COMMAND_QUEST_EXPLORED:
            {
                Quest const* quest = GetQuestTemplate(tmp.QuestExplored.QuestID);
                if (!quest)
                {
                    sLog.outErrorDb("Table `%s` has invalid quest (ID: %u) in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u",
                                    tableName.c_str(), tmp.QuestExplored.QuestID, tmp.id);
                    continue;
                }

                if (!quest->HasSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT))
                {
                    sLog.outErrorDb("Table `%s` has quest (ID: %u) in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u, but quest not have flag QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT in quest flags. Script command or quest flags wrong. Quest modified to require objective.",
                                    tableName.c_str(), tmp.QuestExplored.QuestID, tmp.id);

                    // this will prevent quest completing without objective
                    const_cast<Quest*>(quest)->SetSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT);

                    // continue; - quest objective requirement set and command can be allowed
                }

                if (float(tmp.QuestExplored.Distance) > DEFAULT_VISIBILITY_DISTANCE)
                {
                    sLog.outErrorDb("Table `%s` has too large distance (%u) for exploring objective complete in `datalong2` in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u",
                                    tableName.c_str(), tmp.QuestExplored.Distance, tmp.id);
                    continue;
                }

                if (tmp.QuestExplored.Distance && float(tmp.QuestExplored.Distance) > DEFAULT_VISIBILITY_DISTANCE)
                {
                    sLog.outErrorDb("Table `%s` has too large distance (%u) for exploring objective complete in `datalong2` in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u, max distance is %f or 0 for disable distance check",
                                    tableName.c_str(), tmp.QuestExplored.Distance, tmp.id, DEFAULT_VISIBILITY_DISTANCE);
                    continue;
                }

                if (tmp.QuestExplored.Distance && float(tmp.QuestExplored.Distance) < INTERACTION_DISTANCE)
                {
                    sLog.outErrorDb("Table `%s` has too small distance (%u) for exploring objective complete in `datalong2` in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u, min distance is %f or 0 for disable distance check",
                                    tableName.c_str(), tmp.QuestExplored.Distance, tmp.id, INTERACTION_DISTANCE);
                    continue;
                }

                break;
            }

        case SCRIPT_COMMAND_KILL_CREDIT:
            {
                if (!GetCreatureTemplate(tmp.KillCredit.CreatureEntry))
                {
                    sLog.outErrorDb("Table `%s` has invalid creature (Entry: %u) in SCRIPT_COMMAND_KILL_CREDIT for script id %u",
                                    tableName.c_str(), tmp.KillCredit.CreatureEntry, tmp.id);
                    continue;
                }
                break;
            }

        case SCRIPT_COMMAND_RESPAWN_GAMEOBJECT:
            {
                GameObjectData const* data = GetGOData(tmp.RespawnGameobject.GOGuid);
                if (!data)
                {
                    sLog.outErrorDb("Table `%s` has invalid gameobject (GUID: %u) in SCRIPT_COMMAND_RESPAWN_GAMEOBJECT for script id %u",
                                    tableName.c_str(), tmp.RespawnGameobject.GOGuid, tmp.id);
                    continue;
                }

                GameObjectInfo const* info = GetGameObjectInfo(data->id);
                if (!info)
                {
                    sLog.outErrorDb("Table `%s` has gameobject with invalid entry (GUID: %u Entry: %u) in SCRIPT_COMMAND_RESPAWN_GAMEOBJECT for script id %u",
                                    tableName.c_str(), tmp.RespawnGameobject.GOGuid, data->id, tmp.id);
                    continue;
                }

                if (info->type == GAMEOBJECT_TYPE_FISHINGNODE ||
                    info->type == GAMEOBJECT_TYPE_FISHINGHOLE ||
                    info->type == GAMEOBJECT_TYPE_DOOR        ||
                    info->type == GAMEOBJECT_TYPE_BUTTON      ||
                    info->type == GAMEOBJECT_TYPE_TRAP)
                {
                    sLog.outErrorDb("Table `%s` has gameobject type (%u) unsupported by command SCRIPT_COMMAND_RESPAWN_GAMEOBJECT for script id %u",
                                    tableName.c_str(), info->id, tmp.id);
                    continue;
                }
                break;
            }

        case SCRIPT_COMMAND_TEMP_SUMMON_CREATURE:
            {
                if (!Oregon::IsValidMapCoord(tmp.TempSummonCreature.PosX, tmp.TempSummonCreature.PosY, tmp.TempSummonCreature.PosZ, tmp.TempSummonCreature.Orientation))
                {
                    sLog.outErrorDb("Table `%s` has invalid coordinates (X: %f Y: %f Z: %f O: %f) in SCRIPT_COMMAND_TEMP_SUMMON_CREATURE for script id %u",
                                    tableName.c_str(), tmp.TempSummonCreature.PosX, tmp.TempSummonCreature.PosY, tmp.TempSummonCreature.PosZ, tmp.TempSummonCreature.Orientation, tmp.id);
                    continue;
                }

                if (!GetCreatureTemplate(tmp.TempSummonCreature.CreatureEntry))
                {
                    sLog.outErrorDb("Table `%s` has invalid creature (Entry: %u) in SCRIPT_COMMAND_TEMP_SUMMON_CREATURE for script id %u",
                                    tableName.c_str(), tmp.TempSummonCreature.CreatureEntry, tmp.id);
                    continue;
                }
                break;
            }

        case SCRIPT_COMMAND_OPEN_DOOR:
        case SCRIPT_COMMAND_CLOSE_DOOR:
            {
                GameObjectData const* data = GetGOData(tmp.ToggleDoor.GOGuid);
                if (!data)
                {
                    sLog.outErrorDb("Table `%s` has invalid gameobject (GUID: %u) in %s for script id %u",
                                    tableName.c_str(), tmp.ToggleDoor.GOGuid, GetScriptCommandName(tmp.command).c_str(), tmp.id);
                    continue;
                }

                GameObjectInfo const* info = GetGameObjectInfo(data->id);
                if (!info)
                {
                    sLog.outErrorDb("Table `%s` has gameobject with invalid entry (GUID: %u Entry: %u) in %s for script id %u",
                                    tableName.c_str(), tmp.ToggleDoor.GOGuid, data->id, GetScriptCommandName(tmp.command).c_str(), tmp.id);
                    continue;
                }

                if (info->type != GAMEOBJECT_TYPE_DOOR)
                {
                    sLog.outErrorDb("Table `%s` has gameobject type (%u) non supported by command %s for script id %u",
                                    tableName.c_str(), info->id, GetScriptCommandName(tmp.command).c_str(), tmp.id);
                    continue;
                }

                break;
            }

        case SCRIPT_COMMAND_REMOVE_AURA:
            {
                if (!sSpellStore.LookupEntry(tmp.RemoveAura.SpellID))
                {
                    sLog.outErrorDb("Table `%s` using non-existent spell (id: %u) in SCRIPT_COMMAND_REMOVE_AURA for script id %u",
                                    tableName.c_str(), tmp.RemoveAura.SpellID, tmp.id);
                    continue;
                }
                if (tmp.RemoveAura.Flags & ~0x1)                    // 1 bits (0,1)
                {
                    sLog.outErrorDb("Table `%s` using unknown flags in datalong2 (%u) in SCRIPT_COMMAND_REMOVE_AURA for script id %u",
                                    tableName.c_str(), tmp.RemoveAura.Flags, tmp.id);
                    continue;
                }
                break;
            }

        case SCRIPT_COMMAND_CAST_SPELL:
            {
                if (!sSpellStore.LookupEntry(tmp.CastSpell.SpellID))
                {
                    sLog.outErrorDb("Table `%s` using non-existent spell (id: %u) in SCRIPT_COMMAND_CAST_SPELL for script id %u",
                                    tableName.c_str(), tmp.CastSpell.SpellID, tmp.id);
                    continue;
                }
                if (tmp.CastSpell.Flags > 4)                      // targeting type
                {
                    sLog.outErrorDb("Table `%s` using unknown target in datalong2 (%u) in SCRIPT_COMMAND_CAST_SPELL for script id %u",
                                    tableName.c_str(), tmp.CastSpell.Flags, tmp.id);
                    continue;
                }
                if (tmp.CastSpell.Flags != 4 && tmp.CastSpell.CreatureEntry & ~0x1)                      // 1 bit (0,1)
                {
                    sLog.outErrorDb("Table `%s` using unknown flags in dataint (%u) in SCRIPT_COMMAND_CAST_SPELL for script id %u",
                                    tableName.c_str(), tmp.CastSpell.CreatureEntry, tmp.id);
                    continue;
                }
                else if (tmp.CastSpell.Flags == 4 && !GetCreatureTemplate(tmp.CastSpell.CreatureEntry))
                {
                    sLog.outErrorDb("Table `%s` using invalid creature entry in dataint (%u) in SCRIPT_COMMAND_CAST_SPELL for script id %u",
                                    tableName.c_str(), tmp.CastSpell.CreatureEntry, tmp.id);
                    continue;
                }
                break;
            }

        case SCRIPT_COMMAND_CREATE_ITEM:
            {
                if (!GetItemPrototype(tmp.CreateItem.ItemEntry))
                {
                    sLog.outErrorDb("Table `%s` has nonexistent item (entry: %u) in SCRIPT_COMMAND_CREATE_ITEM for script id %u",
                                    tableName.c_str(), tmp.CreateItem.ItemEntry, tmp.id);
                    continue;
                }
                if (!tmp.CreateItem.Amount)
                {
                    sLog.outErrorDb("Table `%s` SCRIPT_COMMAND_CREATE_ITEM but amount is %u for script id %u",
                                    tableName.c_str(), tmp.CreateItem.Amount, tmp.id);
                    continue;
                }
                break;
            }
        default:
            break;
        }

        if (scripts->find(tmp.id) == scripts->end())
        {
            ScriptMap emptyMap;
            (*scripts)[tmp.id] = emptyMap;
        }
        (*scripts)[tmp.id].insert(std::pair<uint32, ScriptInfo>(tmp.delay, tmp));

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u script definitions", count);
}

void ObjectMgr::LoadGameObjectScripts()
{
    LoadScripts(SCRIPTS_GAMEOBJECT);

    // check ids
    for (ScriptMapMap::const_iterator itr = sGameObjectScripts.begin(); itr != sGameObjectScripts.end(); ++itr)
    {
        if (!GetGOData(itr->first))
            sLog.outErrorDb("Table `gameobject_scripts` has not existing gameobject (GUID: %u) as script id", itr->first);
    }
}

void ObjectMgr::LoadQuestEndScripts()
{
    LoadScripts(SCRIPTS_QUEST_END);

    // check ids
    for (ScriptMapMap::const_iterator itr = sQuestEndScripts.begin(); itr != sQuestEndScripts.end(); ++itr)
    {
        if (!GetQuestTemplate(itr->first))
            sLog.outErrorDb("Table `quest_end_scripts` has not existing quest (Id: %u) as script id", itr->first);
    }
}

void ObjectMgr::LoadQuestStartScripts()
{
    LoadScripts(SCRIPTS_QUEST_START);

    // check ids
    for (ScriptMapMap::const_iterator itr = sQuestStartScripts.begin(); itr != sQuestStartScripts.end(); ++itr)
    {
        if (!GetQuestTemplate(itr->first))
            sLog.outErrorDb("Table `quest_start_scripts` has not existing quest (Id: %u) as script id", itr->first);
    }
}

void ObjectMgr::LoadSpellScripts()
{
    LoadScripts(SCRIPTS_SPELL);

    // check ids
    for (ScriptMapMap::const_iterator itr = sSpellScripts.begin(); itr != sSpellScripts.end(); ++itr)
    {
        SpellEntry const* spellInfo = sSpellStore.LookupEntry(itr->first);

        if (!spellInfo)
        {
            sLog.outErrorDb("Table `spell_scripts` has not existing spell (Id: %u) as script id", itr->first);
            continue;
        }

        //check for correct spellEffect
        bool found = false;
        for (int i = 0; i < 3; ++i)
        {
            // skip empty effects
            if (!spellInfo->Effect[i])
                continue;

            if (spellInfo->Effect[i] == SPELL_EFFECT_SCRIPT_EFFECT)
            {
                found =  true;
                break;
            }
        }

        if (!found)
            sLog.outErrorDb("Table spell_scripts has unsupported spell (Id: %u) without SPELL_EFFECT_SCRIPT_EFFECT (%u) spell effect", itr->first, SPELL_EFFECT_SCRIPT_EFFECT);
    }
}

void ObjectMgr::LoadEventScripts()
{
    LoadScripts(SCRIPTS_EVENT);

    std::set<uint32> evt_scripts;
    // Load all possible script entries from gameobjects
    for (uint32 i = 1; i < sGOStorage.MaxEntry; ++i)
    {
        GameObjectInfo const* goInfo = sGOStorage.LookupEntry<GameObjectInfo>(i);
        if (goInfo)
        {
            switch (goInfo->type)
            {
            case GAMEOBJECT_TYPE_GOOBER:
                if (goInfo->goober.eventId)
                    evt_scripts.insert(goInfo->goober.eventId);
                break;
            case GAMEOBJECT_TYPE_CHEST:
                if (goInfo->chest.eventId)
                    evt_scripts.insert(goInfo->chest.eventId);
                break;
            case GAMEOBJECT_TYPE_CAMERA:
                if (goInfo->camera.eventID)
                    evt_scripts.insert(goInfo->camera.eventID);
            default:
                break;
            }
        }
    }
    // Load all possible script entries from spells
    for (uint32 i = 1; i < sSpellStore.GetNumRows(); ++i)
    {
        SpellEntry const* spell = sSpellStore.LookupEntry(i);
        if (spell)
        {
            for (uint8 j = 0; j < 3; ++j)
            {
                if (spell->Effect[j] == SPELL_EFFECT_SEND_EVENT)
                {
                    if (spell->EffectMiscValue[j])
                        evt_scripts.insert(spell->EffectMiscValue[j]);
                }
            }
        }
    }

    for(size_t path_idx = 0; path_idx < sTaxiPathNodesByPath.size(); ++path_idx)
    {
        for(size_t node_idx = 0; node_idx < sTaxiPathNodesByPath[path_idx].size(); ++node_idx)
        {
            TaxiPathNodeEntry const& node = sTaxiPathNodesByPath[path_idx][node_idx];

            if (node.arrivalEventID)
                evt_scripts.insert(node.arrivalEventID);

            if (node.departureEventID)
                evt_scripts.insert(node.departureEventID);
        }
    }

    // Then check if all scripts are in above list of possible script entries
    for (ScriptMapMap::const_iterator itr = sEventScripts.begin(); itr != sEventScripts.end(); ++itr)
    {
        std::set<uint32>::const_iterator itr2 = evt_scripts.find(itr->first);
        if (itr2 == evt_scripts.end())
            sLog.outErrorDb("Table `event_scripts` has script (Id: %u) not referring to any gameobject_template type 10 data2 field, type 3 data6 field, type 13 data 2 field or any spell effect %u or path taxi node data",
                            itr->first, SPELL_EFFECT_SEND_EVENT);
    }
}

//Load WP Scripts
void ObjectMgr::LoadWaypointScripts()
{
    LoadScripts(SCRIPTS_WAYPOINT);

    std::set<uint32> actionSet;

    for (ScriptMapMap::const_iterator itr = sWaypointScripts.begin(); itr != sWaypointScripts.end(); ++itr)
        actionSet.insert(itr->first);

    QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT DISTINCT(`action`) FROM waypoint_data");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 action = fields[0].GetUInt32();

            actionSet.erase(action);

        }
        while (result->NextRow());
    }

    for (std::set<uint32>::iterator itr = actionSet.begin(); itr != actionSet.end(); ++itr)
        sLog.outErrorDb("There is no waypoint which links to the waypoint script %u", *itr);
}

void ObjectMgr::LoadGossipScripts()
{
    LoadScripts(SCRIPTS_GOSSIP);

    // checks are done in LoadGossipMenuItems
}

void ObjectMgr::LoadItemTexts()
{
    QueryResult_AutoPtr result = CharacterDatabase.Query("SELECT id, text FROM item_text");

    uint32 count = 0;

    if (!result)
    {

        sLog.outString(">> Loaded %u item pages", count);
        return;
    }


    Field* fields;
    do
    {

        fields = result->Fetch();

        mItemTexts[ fields[0].GetUInt32() ] = fields[1].GetCppString();

        ++count;

    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u item texts", count);
}

void ObjectMgr::LoadPageTexts()
{
    sPageTextStore.Load();
    sLog.outString(">> Loaded %u page texts", sPageTextStore.RecordCount);

    for (uint32 i = 1; i < sPageTextStore.MaxEntry; ++i)
    {
        // check data correctness
        PageText const* page = sPageTextStore.LookupEntry<PageText>(i);
        if (!page)
            continue;

        if (page->Next_Page && !sPageTextStore.LookupEntry<PageText>(page->Next_Page))
        {
            sLog.outErrorDb("Page text (Id: %u) has invalid next page (Id:%u)", i, page->Next_Page);
            continue;
        }

        // detect circular reference
        std::set<uint32> checkedPages;
        for (PageText const* pageItr = page; pageItr; pageItr = sPageTextStore.LookupEntry<PageText>(pageItr->Next_Page))
        {
            if (!pageItr->Next_Page)
                break;
            checkedPages.insert(pageItr->Page_ID);
            if (checkedPages.find(pageItr->Next_Page) != checkedPages.end())
            {
                std::ostringstream ss;
                ss << "The text page(s) ";
                for (std::set<uint32>::iterator itr = checkedPages.begin(); itr != checkedPages.end(); ++itr)
                    ss << *itr << " ";
                ss << "create(s) a circular reference, which can cause the server to freeze. Changing Next_Page of page "
                   << pageItr->Page_ID << " to 0";
                sLog.outErrorDb("%s", ss.str().c_str());
                const_cast<PageText*>(pageItr)->Next_Page = 0;
                break;
            }
        }
    }
}

void ObjectMgr::LoadPageTextLocales()
{
    mPageTextLocaleMap.clear();                             // need for reload case

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry,text_loc1,text_loc2,text_loc3,text_loc4,text_loc5,text_loc6,text_loc7,text_loc8 FROM locales_page_text");

    if (!result)
    {


        sLog.outString(">> Loaded 0 PageText locale strings. DB table locales_page_text is empty.");
        return;
    }


    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        PageTextLocale& data = mPageTextLocaleMap[entry];

        for (uint8 i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[i].GetCppString();
            if (str.empty())
                continue;

            int idx = GetOrNewIndexForLocale(LocaleConstant(i));
            if (idx >= 0)
            {
                if (data.Text.size() <= idx)
                    data.Text.resize(idx + 1);

                data.Text[idx] = str;
            }
        }

    }
    while (result->NextRow());

    sLog.outString(">> Loaded %lu PageText locale strings", mPageTextLocaleMap.size());
}

struct SQLInstanceLoader : public SQLStorageLoaderBase<SQLInstanceLoader>
{
    template<class D>
    void convert_from_str(uint32 field_pos, char* src, D& dst)
    {
        dst = D(sObjectMgr.GetScriptId(src));
    }
};

void ObjectMgr::LoadInstanceTemplate()
{
    SQLInstanceLoader loader;
    loader.Load(sInstanceTemplate);

    for (uint32 i = 0; i < sInstanceTemplate.MaxEntry; i++)
    {
        InstanceTemplate* temp = (InstanceTemplate*)GetInstanceTemplate(i);
        if (!temp) continue;
        const MapEntry* entry = sMapStore.LookupEntry(temp->map);
        if (!entry)
        {
            sLog.outErrorDb("ObjectMgr::LoadInstanceTemplate: bad mapid %d for template!", temp->map);
            continue;
        }
        else if (!entry->HasResetTime())
            continue;

        if (temp->reset_delay == 0)
        {
            // use defaults from the DBC
            if (entry->SupportsHeroicMode())
                temp->reset_delay = entry->resetTimeHeroic / DAY;
            else if (entry->resetTimeRaid && entry->IsRaid())
                temp->reset_delay = entry->resetTimeRaid / DAY;
        }

        // the reset_delay must be at least one day
        temp->reset_delay = std::max((uint32)1, (uint32)(temp->reset_delay * sWorld.getRate(RATE_INSTANCE_RESET_TIME)));
    }

    sLog.outString(">> Loaded %u Instance Template definitions", sInstanceTemplate.RecordCount);
}

void ObjectMgr::AddGossipText(GossipText* pGText)
{
    ASSERT(pGText->Text_ID);
    ASSERT(mGossipText.find(pGText->Text_ID) == mGossipText.end());
    mGossipText[pGText->Text_ID] = pGText;
}

GossipText* ObjectMgr::GetGossipText(uint32 Text_ID)
{
    GossipTextMap::const_iterator itr;
    for (itr = mGossipText.begin(); itr != mGossipText.end(); ++itr)
    {
        if (itr->second->Text_ID == Text_ID)
            return itr->second;
    }
    return NULL;
}

void ObjectMgr::LoadGossipText()
{
    GossipText* pGText;
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT * FROM npc_text");

    int count = 0;
    if (!result)
    {

        sLog.outString(">> Loaded %u npc texts", count);
        return;
    }

    int cic;


    do
    {
        ++count;
        cic = 0;

        Field* fields = result->Fetch();


        pGText = new GossipText;
        pGText->Text_ID    = fields[cic++].GetUInt32();

        for (int i = 0; i < 8; i++)
        {
            pGText->Options[i].Text_0           = fields[cic++].GetCppString();
            pGText->Options[i].Text_1           = fields[cic++].GetCppString();

            pGText->Options[i].Language         = fields[cic++].GetUInt32();
            pGText->Options[i].Probability      = fields[cic++].GetFloat();

            pGText->Options[i].Emotes[0]._Delay  = fields[cic++].GetUInt32();
            pGText->Options[i].Emotes[0]._Emote  = fields[cic++].GetUInt32();

            pGText->Options[i].Emotes[1]._Delay  = fields[cic++].GetUInt32();
            pGText->Options[i].Emotes[1]._Emote  = fields[cic++].GetUInt32();

            pGText->Options[i].Emotes[2]._Delay  = fields[cic++].GetUInt32();
            pGText->Options[i].Emotes[2]._Emote  = fields[cic++].GetUInt32();
        }

        if (!pGText->Text_ID)
        {
            delete pGText;
            continue;
        }

        AddGossipText(pGText);

    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u npc texts", count);
}

void ObjectMgr::LoadNpcTextLocales()
{
    mNpcTextLocaleMap.clear();                              // need for reload case

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry,"
                                 "Text0_0_loc1,Text0_1_loc1,Text1_0_loc1,Text1_1_loc1,Text2_0_loc1,Text2_1_loc1,Text3_0_loc1,Text3_1_loc1,Text4_0_loc1,Text4_1_loc1,Text5_0_loc1,Text5_1_loc1,Text6_0_loc1,Text6_1_loc1,Text7_0_loc1,Text7_1_loc1,"
                                 "Text0_0_loc2,Text0_1_loc2,Text1_0_loc2,Text1_1_loc2,Text2_0_loc2,Text2_1_loc2,Text3_0_loc2,Text3_1_loc1,Text4_0_loc2,Text4_1_loc2,Text5_0_loc2,Text5_1_loc2,Text6_0_loc2,Text6_1_loc2,Text7_0_loc2,Text7_1_loc2,"
                                 "Text0_0_loc3,Text0_1_loc3,Text1_0_loc3,Text1_1_loc3,Text2_0_loc3,Text2_1_loc3,Text3_0_loc3,Text3_1_loc1,Text4_0_loc3,Text4_1_loc3,Text5_0_loc3,Text5_1_loc3,Text6_0_loc3,Text6_1_loc3,Text7_0_loc3,Text7_1_loc3,"
                                 "Text0_0_loc4,Text0_1_loc4,Text1_0_loc4,Text1_1_loc4,Text2_0_loc4,Text2_1_loc4,Text3_0_loc4,Text3_1_loc1,Text4_0_loc4,Text4_1_loc4,Text5_0_loc4,Text5_1_loc4,Text6_0_loc4,Text6_1_loc4,Text7_0_loc4,Text7_1_loc4,"
                                 "Text0_0_loc5,Text0_1_loc5,Text1_0_loc5,Text1_1_loc5,Text2_0_loc5,Text2_1_loc5,Text3_0_loc5,Text3_1_loc1,Text4_0_loc5,Text4_1_loc5,Text5_0_loc5,Text5_1_loc5,Text6_0_loc5,Text6_1_loc5,Text7_0_loc5,Text7_1_loc5,"
                                 "Text0_0_loc6,Text0_1_loc6,Text1_0_loc6,Text1_1_loc6,Text2_0_loc6,Text2_1_loc6,Text3_0_loc6,Text3_1_loc1,Text4_0_loc6,Text4_1_loc6,Text5_0_loc6,Text5_1_loc6,Text6_0_loc6,Text6_1_loc6,Text7_0_loc6,Text7_1_loc6,"
                                 "Text0_0_loc7,Text0_1_loc7,Text1_0_loc7,Text1_1_loc7,Text2_0_loc7,Text2_1_loc7,Text3_0_loc7,Text3_1_loc1,Text4_0_loc7,Text4_1_loc7,Text5_0_loc7,Text5_1_loc7,Text6_0_loc7,Text6_1_loc7,Text7_0_loc7,Text7_1_loc7, "
                                 "Text0_0_loc8,Text0_1_loc8,Text1_0_loc8,Text1_1_loc8,Text2_0_loc8,Text2_1_loc8,Text3_0_loc8,Text3_1_loc1,Text4_0_loc8,Text4_1_loc8,Text5_0_loc8,Text5_1_loc8,Text6_0_loc8,Text6_1_loc8,Text7_0_loc8,Text7_1_loc8 "
                                 " FROM locales_npc_text");

    if (!result)
    {


        sLog.outString(">> Loaded 0 Quest locale strings. DB table locales_npc_text is empty.");
        return;
    }


    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        NpcTextLocale& data = mNpcTextLocaleMap[entry];

        for (uint8 i = 1; i < MAX_LOCALE; ++i)
        {
            for (uint8 j = 0; j < 8; ++j)
            {
                std::string str0 = fields[1 + 8 * 2 * (i - 1) + 2 * j].GetCppString();
                if (!str0.empty())
                {
                    int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                    if (idx >= 0)
                    {
                        if (data.Text_0[j].size() <= idx)
                            data.Text_0[j].resize(idx + 1);

                        data.Text_0[j][idx] = str0;
                    }
                }
                std::string str1 = fields[1 + 8 * 2 * (i - 1) + 2 * j + 1].GetCppString();
                if (!str1.empty())
                {
                    int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                    if (idx >= 0)
                    {
                        if (data.Text_1[j].size() <= idx)
                            data.Text_1[j].resize(idx + 1);

                        data.Text_1[j][idx] = str1;
                    }
                }
            }
        }
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %lu NpcText locale strings", mNpcTextLocaleMap.size());
}

//not very fast function but it is called only once a day, or on starting-up
void ObjectMgr::ReturnOrDeleteOldMails(bool serverUp)
{
    time_t basetime = time(NULL);
    sLog.outDebug("Returning mails current time: hour: %d, minute: %d, second: %d ", localtime(&basetime)->tm_hour, localtime(&basetime)->tm_min, localtime(&basetime)->tm_sec);
    //delete all old mails without item and without body immediately, if starting server
    if (!serverUp)
        CharacterDatabase.PExecute("DELETE FROM mail WHERE expire_time < '" UI64FMTD "' AND has_items = '0' AND itemTextId = 0", (uint64)basetime);
    //                                                            0  1           2      3        4          5         6           7   8       9
    QueryResult_AutoPtr result = CharacterDatabase.PQuery("SELECT id,messageType,sender,receiver,itemTextId,has_items,expire_time,cod,checked,mailTemplateId FROM mail WHERE expire_time < '" UI64FMTD "'", (uint64)basetime);
    if (!result)
        return;                                             // any mails need to be returned or deleted
    Field* fields;
    //std::ostringstream delitems, delmails; //will be here for optimization
    //bool deletemail = false, deleteitem = false;
    //delitems << "DELETE FROM item_instance WHERE guid IN (";
    //delmails << "DELETE FROM mail WHERE id IN ("
    do
    {
        fields = result->Fetch();
        Mail* m = new Mail;
        m->messageID = fields[0].GetUInt32();
        m->messageType = fields[1].GetUInt8();
        m->sender = fields[2].GetUInt32();
        m->receiver = fields[3].GetUInt32();
        m->itemTextId = fields[4].GetUInt32();
        bool has_items = fields[5].GetBool();
        m->expire_time = (time_t)fields[6].GetUInt64();
        m->deliver_time = 0;
        m->COD = fields[7].GetUInt32();
        m->checked = fields[8].GetUInt32();
        m->mailTemplateId = fields[9].GetInt16();

        Player* pl = 0;
        if (serverUp)
            pl = GetPlayer((uint64)m->receiver);
        if (pl && pl->m_mailsLoaded)
        {
            //this code will run very improbably (the time is between 4 and 5 am, in game is online a player, who has old mail
            //his in mailbox and he has already listed his mails)
            delete m;
            continue;
        }
        //delete or return mail:
        if (has_items)
        {
            QueryResult_AutoPtr resultItems = CharacterDatabase.PQuery("SELECT item_guid,item_template FROM mail_items WHERE mail_id='%u'", m->messageID);
            if (resultItems)
            {
                do
                {
                    Field* fields2 = resultItems->Fetch();

                    uint32 item_guid_low = fields2[0].GetUInt32();
                    uint32 item_template = fields2[1].GetUInt32();

                    m->AddItem(item_guid_low, item_template);
                }
                while (resultItems->NextRow());
            }
            // if it is mail from AH, it shouldn't be returned, but deleted
            if (m->messageType != MAIL_NORMAL || m->messageType == MAIL_AUCTION || (m->checked & (MAIL_CHECK_MASK_COD_PAYMENT | MAIL_CHECK_MASK_RETURNED)))
            {
                // mail open and then not returned
                for (std::vector<MailItemInfo>::iterator itr2 = m->items.begin(); itr2 != m->items.end(); ++itr2)
                    CharacterDatabase.PExecute("DELETE FROM item_instance WHERE guid = '%u'", itr2->item_guid);
            }
            else
            {
                //mail will be returned:
                CharacterDatabase.PExecute("UPDATE mail SET sender = '%u', receiver = '%u', expire_time = '" UI64FMTD "', deliver_time = '" UI64FMTD "',cod = '0', checked = '%u' WHERE id = '%u'", m->receiver, m->sender, (uint64)(basetime + 30 * DAY), (uint64)basetime, MAIL_CHECK_MASK_RETURNED, m->messageID);
                delete m;
                continue;
            }
        }

        if (m->itemTextId)
            CharacterDatabase.PExecute("DELETE FROM item_text WHERE id = '%u'", m->itemTextId);

        //deletemail = true;
        //delmails << m->messageID << ", ";
        CharacterDatabase.PExecute("DELETE FROM mail WHERE id = '%u'", m->messageID);
        delete m;
    }
    while (result->NextRow());
}

void ObjectMgr::LoadQuestAreaTriggers()
{
    mQuestAreaTriggerMap.clear();                           // need for reload case

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT id,quest FROM areatrigger_involvedrelation");

    uint32 count = 0;

    if (!result)
    {

        sLog.outString(">> Loaded %u quest trigger points", count);
        return;
    }


    do
    {
        ++count;

        Field* fields = result->Fetch();

        uint32 trigger_ID = fields[0].GetUInt32();
        uint32 quest_ID   = fields[1].GetUInt32();

        AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(trigger_ID);
        if (!atEntry)
        {
            sLog.outErrorDb("Area trigger (ID:%u) does not exist in AreaTrigger.dbc.", trigger_ID);
            continue;
        }

        Quest const* quest = GetQuestTemplate(quest_ID);

        if (!quest)
        {
            sLog.outErrorDb("Table areatrigger_involvedrelation has record (id: %u) for invalid quest %u", trigger_ID, quest_ID);
            continue;
        }

        if (!quest->HasSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT))
        {
            sLog.outErrorDb("Table areatrigger_involvedrelation has record (id: %u) for not quest %u, but quest does not have flag QUEST_OREGON_FLAGS_EXPLORATION_OR_EVENT. Trigger or quest flags must be fixed, quest modified to require objective.", trigger_ID, quest_ID);

            // this will prevent quest completing without objective
            const_cast<Quest*>(quest)->SetSpecialFlag(QUEST_SPECIAL_FLAGS_EXPLORATION_OR_EVENT);

            // continue; - quest modified to required objective and trigger can be allowed.
        }

        mQuestAreaTriggerMap[trigger_ID] = quest_ID;

    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u quest trigger points", count);
}

void ObjectMgr::LoadTavernAreaTriggers()
{
    mTavernAreaTriggerSet.clear();                          // need for reload case

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT id FROM areatrigger_tavern");

    uint32 count = 0;

    if (!result)
    {

        sLog.outString(">> Loaded %u tavern triggers", count);
        return;
    }


    do
    {
        ++count;

        Field* fields = result->Fetch();

        uint32 Trigger_ID      = fields[0].GetUInt32();

        AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(Trigger_ID);
        if (!atEntry)
        {
            sLog.outErrorDb("Area trigger (ID:%u) does not exist in AreaTrigger.dbc.", Trigger_ID);
            continue;
        }

        mTavernAreaTriggerSet.insert(Trigger_ID);
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u tavern triggers", count);
}

void ObjectMgr::LoadAreaTriggerScripts()
{
    mAreaTriggerScripts.clear();                            // need for reload case
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry, ScriptName FROM areatrigger_scripts");

    uint32 count = 0;

    if (!result)
    {

        sLog.outString(">> Loaded %u areatrigger scripts", count);
        return;
    }


    do
    {
        ++count;

        Field* fields = result->Fetch();

        uint32 Trigger_ID      = fields[0].GetUInt32();
        const char* scriptName = fields[1].GetString();

        AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(Trigger_ID);
        if (!atEntry)
        {
            sLog.outErrorDb("Area trigger (ID:%u) does not exist in AreaTrigger.dbc.", Trigger_ID);
            continue;
        }
        mAreaTriggerScripts[Trigger_ID] = GetScriptId(scriptName);
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u areatrigger scripts", count);
}

uint32 ObjectMgr::GetNearestTaxiNode(float x, float y, float z, uint32 mapid)
{
    bool found = false;
    float dist;
    uint32 id = 0;

    for (uint32 i = 1; i < sTaxiNodesStore.GetNumRows(); ++i)
    {
        TaxiNodesEntry const* node = sTaxiNodesStore.LookupEntry(i);
        if (node && node->map_id == mapid)
        {
            float dist2 = (node->x - x) * (node->x - x) + (node->y - y) * (node->y - y) + (node->z - z) * (node->z - z);
            if (found)
            {
                if (dist2 < dist)
                {
                    dist = dist2;
                    id = i;
                }
            }
            else
            {
                found = true;
                dist = dist2;
                id = i;
            }
        }
    }

    return id;
}

void ObjectMgr::GetTaxiPath(uint32 source, uint32 destination, uint32& path, uint32& cost)
{
    TaxiPathSetBySource::iterator src_i = sTaxiPathSetBySource.find(source);
    if (src_i == sTaxiPathSetBySource.end())
    {
        path = 0;
        cost = 0;
        return;
    }

    TaxiPathSetForSource& pathSet = src_i->second;

    TaxiPathSetForSource::iterator dest_i = pathSet.find(destination);
    if (dest_i == pathSet.end())
    {
        path = 0;
        cost = 0;
        return;
    }

    cost = dest_i->second.price;
    path = dest_i->second.ID;
}

uint16 ObjectMgr::GetTaxiMount(uint32 id, uint32 team)
{
    uint16 mount_entry = 0;
    uint16 mount_id = 0;

    TaxiNodesEntry const* node = sTaxiNodesStore.LookupEntry(id);
    if (node)
    {
        if (team == ALLIANCE) mount_entry = node->alliance_mount_type;
        else mount_entry = node->horde_mount_type;

        CreatureInfo const* cinfo = GetCreatureTemplate(mount_entry);
        if (cinfo)
        {
            if (! (mount_id = cinfo->GetRandomValidModelId()))
            {
                sLog.outErrorDb("No displayid found for the taxi mount with the entry %u! Can't load it!", mount_entry);
                return false;
            }
        }
    }

    CreatureModelInfo const* minfo = GetCreatureModelInfo(mount_id);
    if (!minfo)
    {
        sLog.outErrorDb("Taxi mount (Entry: %u) for taxi node (Id: %u) for team %u has model %u not found in table creature_model_info, can't load. ",
                        mount_entry, id, team, mount_id);

        return false;
    }
    if (minfo->modelid_other_gender != 0)
        mount_id = urand(0, 1) ? mount_id : minfo->modelid_other_gender;

    return mount_id;
}

/* -- didnt compile with mmaps
void ObjectMgr::GetTaxiPathNodes(uint32 path, Path &pathnodes, std::vector<uint32>& mapIds)
{
    if (path >= sTaxiPathNodesByPath.size())
        return;

    TaxiPathNodeList& nodeList = sTaxiPathNodesByPath[path];

    pathnodes.Resize(nodeList.size());
    mapIds.resize(nodeList.size());

    for (size_t i = 0; i < nodeList.size(); ++i)
    {
        pathnodes[ i ].x = nodeList[i].x;
        pathnodes[ i ].y = nodeList[i].y;
        pathnodes[ i ].z = nodeList[i].z;

        mapIds[i] = nodeList[i].mapid;
    }
}

void ObjectMgr::GetTransportPathNodes(uint32 path, TransportPath &pathnodes)
{
    if (path >= sTaxiPathNodesByPath.size())
        return;

    TaxiPathNodeList& nodeList = sTaxiPathNodesByPath[path];

    pathnodes.Resize(nodeList.size());

    for (size_t i = 0; i < nodeList.size(); ++i)
    {
        pathnodes[ i ].mapid = nodeList[i].mapid;
        pathnodes[ i ].x = nodeList[i].x;
        pathnodes[ i ].y = nodeList[i].y;
        pathnodes[ i ].z = nodeList[i].z;
        pathnodes[ i ].actionFlag = nodeList[i].actionFlag;
        pathnodes[ i ].delay = nodeList[i].delay;
    }
}
*/
void ObjectMgr::LoadGraveyardZones()
{
    mGraveYardMap.clear();                                  // need for reload case

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT id,ghost_zone,faction FROM game_graveyard_zone");

    uint32 count = 0;

    if (!result)
    {

        sLog.outString(">> Loaded %u graveyard-zone links", count);
        return;
    }


    do
    {
        ++count;

        Field* fields = result->Fetch();

        uint32 safeLocId = fields[0].GetUInt32();
        uint32 zoneId = fields[1].GetUInt32();
        uint32 team   = fields[2].GetUInt32();

        WorldSafeLocsEntry const* entry = sWorldSafeLocsStore.LookupEntry(safeLocId);
        if (!entry)
        {
            sLog.outErrorDb("Table game_graveyard_zone has record for invalid graveyard (WorldSafeLocs.dbc id) %u, skipped.", safeLocId);
            continue;
        }

        AreaTableEntry const* areaEntry = GetAreaEntryByAreaID(zoneId);
        if (!areaEntry)
        {
            sLog.outErrorDb("Table game_graveyard_zone has record for invalid zone id (%u), skipped.", zoneId);
            continue;
        }

        if (areaEntry->zone != 0)
        {
            sLog.outErrorDb("Table game_graveyard_zone has record subzone id (%u) instead of zone, skipped.", zoneId);
            continue;
        }

        if (team != 0 && team != HORDE && team != ALLIANCE)
        {
            sLog.outErrorDb("Table game_graveyard_zone has record for non player faction (%u), skipped.", team);
            continue;
        }

        if (!AddGraveYardLink(safeLocId, zoneId, team, false))
            sLog.outErrorDb("Table game_graveyard_zone has a duplicate record for Graveyard (ID: %u) and Zone (ID: %u), skipped.", safeLocId, zoneId);
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u graveyard-zone links", count);
}

WorldSafeLocsEntry const* ObjectMgr::GetClosestGraveYard(float x, float y, float z, uint32 MapId, uint32 team)
{
    // search for zone associated closest graveyard
    uint32 zoneId = MapManager::Instance().GetZoneId(MapId, x, y, z);

    // Simulate std. algorithm:
    //   found some graveyard associated to (ghost_zone,ghost_map)
    //
    //   if mapId == graveyard.mapId (ghost in plain zone or city or battleground) and search graveyard at same map
    //     then check faction
    //   if mapId != graveyard.mapId (ghost in instance) and search any graveyard associated
    //     then check faction
    GraveYardMap::const_iterator graveLow  = mGraveYardMap.lower_bound(zoneId);
    GraveYardMap::const_iterator graveUp   = mGraveYardMap.upper_bound(zoneId);
    if (graveLow == graveUp)
    {
        sLog.outErrorDb("Table game_graveyard_zone incomplete: Zone %u Team %u does not have a linked graveyard.", zoneId, team);
        return NULL;
    }

    // at corpse map
    bool foundNear = false;
    float distNear;
    WorldSafeLocsEntry const* entryNear = NULL;

    // at entrance map for corpse map
    bool foundEntr = false;
    float distEntr;
    WorldSafeLocsEntry const* entryEntr = NULL;

    // some where other
    WorldSafeLocsEntry const* entryFar = NULL;

    MapEntry const* mapEntry = sMapStore.LookupEntry(MapId);

    for (GraveYardMap::const_iterator itr = graveLow; itr != graveUp; ++itr)
    {
        GraveYardData const& data = itr->second;

        WorldSafeLocsEntry const* entry = sWorldSafeLocsStore.LookupEntry(data.safeLocId);
        if (!entry)
        {
            sLog.outErrorDb("Table game_graveyard_zone has record for invalid graveyard (WorldSafeLocs.dbc id) %u, skipped.", data.safeLocId);
            continue;
        }

        // skip enemy faction graveyard
        // team == 0 case can be at call from .neargrave
        if (data.team != 0 && team != 0 && data.team != team)
            continue;

        // find now nearest graveyard at other map
        if (MapId != entry->map_id)
        {
            // if find graveyard at different map from where entrance placed (or no entrance data), use any first
            if (!mapEntry || mapEntry->entrance_map < 0 || mapEntry->entrance_map != entry->map_id ||
                mapEntry->entrance_x == 0 && mapEntry->entrance_y == 0)
            {
                // not have any corrdinates for check distance anyway
                entryFar = entry;
                continue;
            }

            // at entrance map calculate distance (2D);
            float dist2 = (entry->x - mapEntry->entrance_x) * (entry->x - mapEntry->entrance_x)
                          + (entry->y - mapEntry->entrance_y) * (entry->y - mapEntry->entrance_y);
            if (foundEntr)
            {
                if (dist2 < distEntr)
                {
                    distEntr = dist2;
                    entryEntr = entry;
                }
            }
            else
            {
                foundEntr = true;
                distEntr = dist2;
                entryEntr = entry;
            }
        }
        // find now nearest graveyard at same map
        else
        {
            float dist2 = (entry->x - x) * (entry->x - x) + (entry->y - y) * (entry->y - y) + (entry->z - z) * (entry->z - z);
            if (foundNear)
            {
                if (dist2 < distNear)
                {
                    distNear = dist2;
                    entryNear = entry;
                }
            }
            else
            {
                foundNear = true;
                distNear = dist2;
                entryNear = entry;
            }
        }
    }

    if (entryNear)
        return entryNear;

    if (entryEntr)
        return entryEntr;

    return entryFar;
}

GraveYardData const* ObjectMgr::FindGraveYardData(uint32 id, uint32 zoneId)
{
    GraveYardMap::const_iterator graveLow  = mGraveYardMap.lower_bound(zoneId);
    GraveYardMap::const_iterator graveUp   = mGraveYardMap.upper_bound(zoneId);

    for (GraveYardMap::const_iterator itr = graveLow; itr != graveUp; ++itr)
    {
        if (itr->second.safeLocId == id)
            return &itr->second;
    }

    return NULL;
}

bool ObjectMgr::AddGraveYardLink(uint32 id, uint32 zoneId, uint32 team, bool inDB)
{
    if (FindGraveYardData(id, zoneId))
        return false;

    // add link to loaded data
    GraveYardData data;
    data.safeLocId = id;
    data.team = team;

    mGraveYardMap.insert(GraveYardMap::value_type(zoneId, data));

    // add link to DB
    if (inDB)
    {
        WorldDatabase.PExecuteLog("INSERT INTO game_graveyard_zone (id,ghost_zone,faction) "
                                  "VALUES ('%u', '%u','%u')", id, zoneId, team);
    }

    return true;
}

void ObjectMgr::RemoveGraveYardLink(uint32 id, uint32 zoneId, uint32 team, bool inDB)
{
    GraveYardMap::iterator graveLow  = mGraveYardMap.lower_bound(zoneId);
    GraveYardMap::iterator graveUp   = mGraveYardMap.upper_bound(zoneId);
    if (graveLow == graveUp)
    {
        //sLog.outErrorDb("Table game_graveyard_zone incomplete: Zone %u Team %u does not have a linked graveyard.",zoneId,team);
        return;
    }

    bool found = false;

    GraveYardMap::iterator itr;

    for (itr = graveLow; itr != graveUp; ++itr)
    {
        GraveYardData& data = itr->second;

        // skip not matching safezone id
        if (data.safeLocId != id)
            continue;

        // skip enemy faction graveyard at same map (normal area, city, or battleground)
        // team == 0 case can be at call from .neargrave
        if (data.team != 0 && team != 0 && data.team != team)
            continue;

        found = true;
        break;
    }

    // no match, return
    if (!found)
        return;

    // remove from links
    mGraveYardMap.erase(itr);

    // remove link from DB
    if (inDB)
        WorldDatabase.PExecute("DELETE FROM game_graveyard_zone WHERE id = '%u' AND ghost_zone = '%u' AND faction = '%u'", id, zoneId, team);

    return;
}

void ObjectMgr::LoadAreaTriggerTeleports()
{
    mAreaTriggers.clear();                                  // need for reload case

    uint32 count = 0;

    //                                                       0   1          2           3                  4                  5                  6
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT id, access_id, target_map, target_position_x, target_position_y, target_position_z, target_orientation FROM areatrigger_teleport");
    if (!result)
    {



        sLog.outString(">> Loaded %u area trigger teleport definitions", count);
        return;
    }


    do
    {
        Field* fields = result->Fetch();


        ++count;

        uint32 Trigger_ID = fields[0].GetUInt32();

        AreaTrigger at;

        at.access_id                = fields[1].GetUInt32();
        at.target_mapId             = fields[2].GetUInt32();
        at.target_X                 = fields[3].GetFloat();
        at.target_Y                 = fields[4].GetFloat();
        at.target_Z                 = fields[5].GetFloat();
        at.target_Orientation       = fields[6].GetFloat();

        AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(Trigger_ID);
        if (!atEntry)
        {
            sLog.outErrorDb("Area trigger (ID:%u) does not exist in AreaTrigger.dbc.", Trigger_ID);
            continue;
        }

        MapEntry const* mapEntry = sMapStore.LookupEntry(at.target_mapId);
        if (!mapEntry)
        {
            sLog.outErrorDb("Area trigger (ID:%u) target map (ID: %u) does not exist in Map.dbc.", Trigger_ID, at.target_mapId);
            continue;
        }

        if (at.target_X == 0 && at.target_Y == 0 && at.target_Z == 0)
        {
            sLog.outErrorDb("Area trigger (ID:%u) target coordinates not provided.", Trigger_ID);
            continue;
        }

        mAreaTriggers[Trigger_ID] = at;

    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u area trigger teleport definitions", count);
}

void ObjectMgr::LoadAccessRequirements()
{
    mAccessRequirements.clear();                                  // need for reload case

    uint32 count = 0;

    //                                                       0   1          2          3     4      5           6            7           8                  9                  10
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT id, level_min, level_max, item, item2, heroic_key, heroic_key2, quest_done, quest_failed_text, heroic_quest_done, heroic_quest_failed_text FROM access_requirement");
    if (!result)
    {



        sLog.outString(">> Loaded %u access requirement definitions", count);
        return;
    }


    do
    {
        Field* fields = result->Fetch();


        ++count;

        uint32 requiremt_ID = fields[0].GetUInt32();

        AccessRequirement ar;

        ar.levelMin                 = fields[1].GetUInt8();
        ar.levelMax                 = fields[2].GetUInt32();
        ar.item                     = fields[3].GetUInt32();
        ar.item2                    = fields[4].GetUInt32();
        ar.heroicKey                = fields[5].GetUInt32();
        ar.heroicKey2               = fields[6].GetUInt32();
        ar.quest                    = fields[7].GetUInt32();
        ar.questFailedText          = fields[8].GetCppString();
        ar.heroicQuest              = fields[9].GetUInt32();
        ar.heroicQuestFailedText    = fields[10].GetCppString();

        if (ar.item)
        {
            ItemPrototype const* pProto = GetItemPrototype(ar.item);
            if (!pProto)
            {
                sLog.outError("Key item %u does not exist for requirement %u, removing key requirement.", ar.item, requiremt_ID);
                ar.item = 0;
            }
        }

        if (ar.item2)
        {
            ItemPrototype const* pProto = GetItemPrototype(ar.item2);
            if (!pProto)
            {
                sLog.outError("Second item %u does not exist for requirement %u, removing key requirement.", ar.item2, requiremt_ID);
                ar.item2 = 0;
            }
        }

        if (ar.heroicKey)
        {
            ItemPrototype const* pProto = GetItemPrototype(ar.heroicKey);
            if (!pProto)
            {
                sLog.outError("Heroic key %u not exist for trigger %u, remove key requirement.", ar.heroicKey, requiremt_ID);
                ar.heroicKey = 0;
            }
        }

        if (ar.heroicKey2)
        {
            ItemPrototype const* pProto = GetItemPrototype(ar.heroicKey2);
            if (!pProto)
            {
                sLog.outError("Second heroic key %u not exist for trigger %u, remove key requirement.", ar.heroicKey2, requiremt_ID);
                ar.heroicKey2 = 0;
            }
        }

        if (ar.heroicQuest)
        {
            if (!GetQuestTemplate(ar.heroicQuest))
            {
                sLog.outErrorDb("Required Heroic Quest %u does not exist for trigger %u, removed heroic quest requirement.", ar.heroicQuest, requiremt_ID);
                ar.heroicQuest = 0;
            }
        }

        if (ar.quest)
        {
            if (!GetQuestTemplate(ar.quest))
            {
                sLog.outErrorDb("Required Quest %u does not exist for trigger %u, removed quest requirement.", ar.quest, requiremt_ID);
                ar.quest = 0;
            }
        }

        mAccessRequirements[requiremt_ID] = ar;

    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u access requirement definitions", count);
}

AreaTrigger const* ObjectMgr::GetGoBackTrigger(uint32 Map) const
{
    const MapEntry* mapEntry = sMapStore.LookupEntry(Map);
    if (!mapEntry) return NULL;
    for (AreaTriggerMap::const_iterator itr = mAreaTriggers.begin(); itr != mAreaTriggers.end(); ++itr)
    {
        if (itr->second.target_mapId == mapEntry->entrance_map)
        {
            AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(itr->first);
            if (atEntry && atEntry->mapid == Map)
                return &itr->second;
        }
    }
    return NULL;
}

/**
 * Searches for the areatrigger which teleports players to the given map
 */
AreaTrigger const* ObjectMgr::GetMapEntranceTrigger(uint32 Map) const
{
    for (AreaTriggerMap::const_iterator itr = mAreaTriggers.begin(); itr != mAreaTriggers.end(); ++itr)
    {
        if (itr->second.target_mapId == Map)
        {
            AreaTriggerEntry const* atEntry = sAreaTriggerStore.LookupEntry(itr->first);
            if (atEntry)
                return &itr->second;
        }
    }
    return NULL;
}

void ObjectMgr::SetHighestGuids()
{
    QueryResult_AutoPtr result = CharacterDatabase.Query("SELECT MAX(guid) FROM characters");
    if (result)
        m_hiCharGuid = (*result)[0].GetUInt32() + 1;

    result = WorldDatabase.Query("SELECT MAX(guid) FROM creature");
    if (result)
        m_hiCreatureGuid = (*result)[0].GetUInt32() + 1;

    // pet guids are not saved to DB, set to 0 (pet guid != pet id)
    m_hiPetGuid = 0;

    result = CharacterDatabase.Query("SELECT MAX(guid) FROM item_instance");
    if (result)
        m_hiItemGuid = (*result)[0].GetUInt32() + 1;

    // Cleanup other tables from not existed guids ( >= m_hiItemGuid)
    CharacterDatabase.PExecute("DELETE FROM character_inventory WHERE item >= '%u'", m_hiItemGuid);
    CharacterDatabase.PExecute("DELETE FROM mail_items WHERE item_guid >= '%u'", m_hiItemGuid);
    CharacterDatabase.PExecute("DELETE FROM auctionhouse WHERE itemguid >= '%u'", m_hiItemGuid);
    CharacterDatabase.PExecute("DELETE FROM guild_bank_item WHERE item_guid >= '%u'", m_hiItemGuid);

    result = WorldDatabase.Query("SELECT MAX(guid) FROM gameobject");
    if (result)
        m_hiGoGuid = (*result)[0].GetUInt32() + 1;

    result = CharacterDatabase.Query("SELECT MAX(id) FROM auctionhouse");
    if (result)
        m_auctionid = (*result)[0].GetUInt32() + 1;

    result = CharacterDatabase.Query("SELECT MAX(id) FROM mail");
    if (result)
        m_mailid = (*result)[0].GetUInt32() + 1;

    result = CharacterDatabase.Query("SELECT MAX(id) FROM item_text");
    if (result)
        m_ItemTextId = (*result)[0].GetUInt32() + 1;

    result = CharacterDatabase.Query("SELECT MAX(guid) FROM corpse");
    if (result)
        m_hiCorpseGuid = (*result)[0].GetUInt32() + 1;

    result = CharacterDatabase.Query("SELECT MAX(arenateamid) FROM arena_team");
    if (result)
        m_arenaTeamId = (*result)[0].GetUInt32() + 1;

    result = CharacterDatabase.Query("SELECT MAX(guildid) FROM guild");
    if (result)
        m_guildId = (*result)[0].GetUInt32() + 1;
}

uint32 ObjectMgr::GenerateArenaTeamId()
{
    if (m_arenaTeamId >= 0xFFFFFFFE)
    {
        sLog.outError("Arena team ids overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return m_arenaTeamId++;
}

uint32 ObjectMgr::GenerateGuildId()
{
    if (m_guildId >= 0xFFFFFFFE)
    {
        sLog.outError("Guild ids overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return m_guildId++;
}

uint32 ObjectMgr::GenerateAuctionID()
{
    if (m_auctionid >= 0xFFFFFFFE)
    {
        sLog.outError("Auctions ids overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return m_auctionid++;
}

uint32 ObjectMgr::GenerateMailID()
{
    if (m_mailid >= 0xFFFFFFFE)
    {
        sLog.outError("Mail ids overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return m_mailid++;
}

uint32 ObjectMgr::GenerateItemTextID()
{
    if (m_ItemTextId >= 0xFFFFFFFE)
    {
        sLog.outError("Item text ids overflow!! Can't continue, shutting down server. ");
        World::StopNow(ERROR_EXIT_CODE);
    }
    return m_ItemTextId++;
}

uint32 ObjectMgr::CreateItemText(std::string text)
{
    uint32 newItemTextId = GenerateItemTextID();
    //insert new itempage to container
    mItemTexts[ newItemTextId ] = text;
    //save new itempage
    CharacterDatabase.escape_string(text);
    //any Delete query needed, itemTextId is maximum of all ids
    std::ostringstream query;
    query << "INSERT INTO item_text (id,text) VALUES ('" << newItemTextId << "', '" << text << "')";
    CharacterDatabase.Execute(query.str().c_str());         //needs to be run this way, because mail body may be more than 1024 characters
    return newItemTextId;
}

uint32 ObjectMgr::GenerateLowGuid(HighGuid guidhigh)
{
    switch (guidhigh)
    {
    case HIGHGUID_ITEM:
        if (m_hiItemGuid >= 0xFFFFFFFE)
        {
            sLog.outError("Item guid overflow!! Can't continue, shutting down server. ");
            World::StopNow(ERROR_EXIT_CODE);
        }
        return m_hiItemGuid++;
    case HIGHGUID_UNIT:
        if (m_hiCreatureGuid >= 0x00FFFFFE)
        {
            sLog.outError("Creature guid overflow!! Can't continue, shutting down server. ");
            World::StopNow(ERROR_EXIT_CODE);
        }
        return m_hiCreatureGuid++;
    case HIGHGUID_PET:
        if (m_hiPetGuid >= 0x00FFFFFE)
        {
            sLog.outError("Pet guid overflow!! Can't continue, shutting down server. ");
            World::StopNow(ERROR_EXIT_CODE);
        }
        return m_hiPetGuid++;
    case HIGHGUID_PLAYER:
        if (m_hiCharGuid >= 0xFFFFFFFE)
        {
            sLog.outError("Players guid overflow!! Can't continue, shutting down server. ");
            World::StopNow(ERROR_EXIT_CODE);
        }
        return m_hiCharGuid++;
    case HIGHGUID_GAMEOBJECT:
        if (m_hiGoGuid >= 0x00FFFFFE)
        {
            sLog.outError("Gameobject guid overflow!! Can't continue, shutting down server. ");
            World::StopNow(ERROR_EXIT_CODE);
        }
        return m_hiGoGuid++;
    case HIGHGUID_CORPSE:
        if (m_hiCorpseGuid >= 0xFFFFFFFE)
        {
            sLog.outError("Corpse guid overflow!! Can't continue, shutting down server. ");
            World::StopNow(ERROR_EXIT_CODE);
        }
        return m_hiCorpseGuid++;
    case HIGHGUID_DYNAMICOBJECT:
        if (m_hiDoGuid >= 0xFFFFFFFE)
        {
            sLog.outError("DynamicObject guid overflow!! Can't continue, shutting down server. ");
            World::StopNow(ERROR_EXIT_CODE);
        }
        return m_hiDoGuid++;
    default:
        ASSERT(0);
    }

    ASSERT(0);
    return 0;
}

void ObjectMgr::LoadGameObjectLocales()
{
    mGameObjectLocaleMap.clear();                           // need for reload case

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry,"
                                 "name_loc1,name_loc2,name_loc3,name_loc4,name_loc5,name_loc6,name_loc7,name_loc8,"
                                 "castbarcaption_loc1,castbarcaption_loc2,castbarcaption_loc3,castbarcaption_loc4,"
                                 "castbarcaption_loc5,castbarcaption_loc6,castbarcaption_loc7,castbarcaption_loc8 FROM locales_gameobject");

    if (!result)
    {


        sLog.outString(">> Loaded 0 gameobject locale strings. DB table locales_gameobject is empty.");
        return;
    }


    do
    {
        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();

        GameObjectLocale& data = mGameObjectLocaleMap[entry];

        for (uint8 i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[i].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if (data.Name.size() <= idx)
                        data.Name.resize(idx + 1);

                    data.Name[idx] = str;
                }
            }
        }

        for (uint8 i = MAX_LOCALE; i < MAX_LOCALE * 2 - 1; ++i)
        {
            std::string str = fields[i].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    if (data.CastBarCaption.size() <= idx)
                        data.CastBarCaption.resize(idx + 1);

                    data.CastBarCaption[idx] = str;
                }
            }
        }

    }
    while (result->NextRow());

    sLog.outString(">> Loaded %lu gameobject locale strings", mGameObjectLocaleMap.size());
}

struct SQLGameObjectLoader : public SQLStorageLoaderBase<SQLGameObjectLoader>
{
    template<class D>
    void convert_from_str(uint32 /*field_pos*/, char* src, D& dst)
    {
        dst = D(sObjectMgr.GetScriptId(src));
    }
};

void ObjectMgr::LoadGameobjectInfo()
{
    SQLGameObjectLoader loader;
    loader.Load(sGOStorage);

    // some checks
    for (uint32 id = 1; id < sGOStorage.MaxEntry; id++)
    {
        GameObjectInfo const* goInfo = sGOStorage.LookupEntry<GameObjectInfo>(id);
        if (!goInfo)
            continue;

        switch (goInfo->type)
        {
        case GAMEOBJECT_TYPE_DOOR:                      //0
            {
                if (goInfo->door.lockId)
                {
                    if (!sLockStore.LookupEntry(goInfo->door.lockId))
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) has data1=%u but lock (Id: %u) not found.",
                                        id, goInfo->type, goInfo->door.lockId, goInfo->door.lockId);
                }
                break;
            }
        case GAMEOBJECT_TYPE_BUTTON:                    //1
            {
                if (goInfo->button.lockId)
                {
                    if (!sLockStore.LookupEntry(goInfo->button.lockId))
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) has data1=%u but lock (Id: %u) not found.",
                                        id, goInfo->type, goInfo->button.lockId, goInfo->button.lockId);
                }
                break;
            }
        case GAMEOBJECT_TYPE_CHEST:                     //3
            {
                if (goInfo->chest.lockId)
                {
                    if (!sLockStore.LookupEntry(goInfo->chest.lockId))
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) has data0=%u but lock (Id: %u) not found.",
                                        id, goInfo->type, goInfo->chest.lockId, goInfo->chest.lockId);
                }
                if (goInfo->chest.linkedTrapId)              // linked trap
                {
                    if (GameObjectInfo const* trapInfo = sGOStorage.LookupEntry<GameObjectInfo>(goInfo->chest.linkedTrapId))
                    {
                        if (trapInfo->type != GAMEOBJECT_TYPE_TRAP)
                            sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) has data7=%u but GO (Entry %u) does not have GAMEOBJECT_TYPE_TRAP (%u) type.",
                                            id, goInfo->type, goInfo->chest.linkedTrapId, goInfo->chest.linkedTrapId, GAMEOBJECT_TYPE_TRAP);
                    }
                    /* disable check for while
                    else
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) has data2=%u but trap GO (Entry %u) does not exist in gameobject_template.",
                            id,goInfo->type,goInfo->chest.linkedTrapId,goInfo->chest.linkedTrapId);
                    */
                }
                break;
            }
        case GAMEOBJECT_TYPE_TRAP:                      //6
            {
                /* disable check for while
                if (goInfo->trap.spellId)                    // spell
                {
                    if (!sSpellStore.LookupEntry(goInfo->trap.spellId))
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) has data3=%u but Spell (Entry %u) does not exist.",
                            id,goInfo->type,goInfo->trap.spellId,goInfo->trap.spellId);
                }
                */
                break;
            }
        case GAMEOBJECT_TYPE_CHAIR:                     //7
            if (goInfo->chair.height > 2)
            {
                sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) has data1=%u but valid chair height range is 0..2.",
                                id, goInfo->type, goInfo->chair.height);

                // prevent client and server unexpected work
                const_cast<GameObjectInfo*>(goInfo)->chair.height = 0;
            }
            break;
        case GAMEOBJECT_TYPE_SPELL_FOCUS:               //8
            {
                if (goInfo->spellFocus.focusId)
                {
                    if (!sSpellFocusObjectStore.LookupEntry(goInfo->spellFocus.focusId))
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) has data0=%u but SpellFocus (Id: %u) does not exist.",
                                        id, goInfo->type, goInfo->spellFocus.focusId, goInfo->spellFocus.focusId);
                }

                if (goInfo->spellFocus.linkedTrapId)         // linked trap
                {
                    if (GameObjectInfo const* trapInfo = sGOStorage.LookupEntry<GameObjectInfo>(goInfo->spellFocus.linkedTrapId))
                    {
                        if (trapInfo->type != GAMEOBJECT_TYPE_TRAP)
                            sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) has data2=%u but GO (Entry %u) does not have GAMEOBJECT_TYPE_TRAP (%u) type.",
                                            id, goInfo->type, goInfo->spellFocus.linkedTrapId, goInfo->spellFocus.linkedTrapId, GAMEOBJECT_TYPE_TRAP);
                    }
                    /* disable check for while
                    else
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) has data2=%u but trap GO (Entry %u) does not exist in gameobject_template.",
                            id,goInfo->type,goInfo->spellFocus.linkedTrapId,goInfo->spellFocus.linkedTrapId);
                    */
                }
                break;
            }
        case GAMEOBJECT_TYPE_GOOBER:                    //10
            {
                if (goInfo->goober.pageId)                   // pageId
                {
                    if (!sPageTextStore.LookupEntry<PageText>(goInfo->goober.pageId))
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) has data7=%u but PageText (Entry %u) does not exist.",
                                        id, goInfo->type, goInfo->goober.pageId, goInfo->goober.pageId);
                }
                /* disable check for while
                if (goInfo->goober.spellId)                  // spell
                {
                    if (!sSpellStore.LookupEntry(goInfo->goober.spellId))
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) has data2=%u but Spell (Entry %u) does not exist.",
                            id,goInfo->type,goInfo->goober.spellId,goInfo->goober.spellId);
                }
                */
                if (goInfo->goober.linkedTrapId)             // linked trap
                {
                    if (GameObjectInfo const* trapInfo = sGOStorage.LookupEntry<GameObjectInfo>(goInfo->goober.linkedTrapId))
                    {
                        if (trapInfo->type != GAMEOBJECT_TYPE_TRAP)
                            sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) has data12=%u but GO (Entry %u) does not have GAMEOBJECT_TYPE_TRAP (%u) type.",
                                            id, goInfo->type, goInfo->goober.linkedTrapId, goInfo->goober.linkedTrapId, GAMEOBJECT_TYPE_TRAP);
                    }
                    /* disable check for while
                    else
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) has data12=%u but trap GO (Entry %u) does not exist in gameobject_template.",
                            id,goInfo->type,goInfo->goober.linkedTrapId,goInfo->goober.linkedTrapId);
                    */
                }
                break;
            }
        case GAMEOBJECT_TYPE_MO_TRANSPORT:              //15
            {
                if (goInfo->moTransport.taxiPathId)
                {
                    if (goInfo->moTransport.taxiPathId >= sTaxiPathNodesByPath.size() || sTaxiPathNodesByPath[goInfo->moTransport.taxiPathId].empty())
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) has data0=%u but TaxiPath (Id: %u) does not exist.",
                                        id, goInfo->type, goInfo->moTransport.taxiPathId, goInfo->moTransport.taxiPathId);
                }
                break;
            }
        case GAMEOBJECT_TYPE_SUMMONING_RITUAL:          //18
            {
                /* disabled
                if (goInfo->summoningRitual.spellId)
                {
                    if (!sSpellStore.LookupEntry(goInfo->summoningRitual.spellId))
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) has data1=%u but Spell (Entry %u) does not exist.",
                            id,goInfo->type,goInfo->summoningRitual.spellId,goInfo->summoningRitual.spellId);
                }
                */
                break;
            }
        case GAMEOBJECT_TYPE_SPELLCASTER:               //22
            {
                if (goInfo->spellcaster.spellId)             // spell
                {
                    if (!sSpellStore.LookupEntry(goInfo->spellcaster.spellId))
                        sLog.outErrorDb("Gameobject (Entry: %u GoType: %u) has data3=%u but Spell (Entry %u) does not exist.",
                                        id, goInfo->type, goInfo->spellcaster.spellId, goInfo->spellcaster.spellId);
                }
                break;
            }
        }
    }

    sLog.outString(">> Loaded %u game object templates", sGOStorage.RecordCount);
}

void ObjectMgr::LoadExplorationBaseXP()
{
    uint32 count = 0;
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT level,basexp FROM exploration_basexp");

    if (!result)
    {


        sLog.outString(">> Loaded %u BaseXP definitions", count);
        return;
    }


    do
    {

        Field* fields = result->Fetch();
        uint32 level  = fields[0].GetUInt32();
        uint32 basexp = fields[1].GetUInt32();
        mBaseXPTable[level] = basexp;
        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u BaseXP definitions", count);
}

uint32 ObjectMgr::GetBaseXP(uint32 level)
{
    return mBaseXPTable[level] ? mBaseXPTable[level] : 0;
}

void ObjectMgr::LoadPetNames()
{
    uint32 count = 0;
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT word,entry,half FROM pet_name_generation");

    if (!result)
    {


        sLog.outString(">> Loaded %u pet name parts", count);
        return;
    }


    do
    {

        Field* fields = result->Fetch();
        std::string word = fields[0].GetString();
        uint32 entry     = fields[1].GetUInt32();
        bool   half      = fields[2].GetBool();
        if (half)
            PetHalfName1[entry].push_back(word);
        else
            PetHalfName0[entry].push_back(word);
        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u pet name parts", count);
}

void ObjectMgr::LoadPetNumber()
{
    QueryResult_AutoPtr result = CharacterDatabase.Query("SELECT MAX(id) FROM character_pet");
    if (result)
    {
        Field* fields = result->Fetch();
        m_hiPetNumber = fields[0].GetUInt32() + 1;
    }


    sLog.outString(">> Loaded the max pet number: %d", m_hiPetNumber - 1);
}

std::string ObjectMgr::GeneratePetName(uint32 entry)
{
    std::vector<std::string>& list0 = PetHalfName0[entry];
    std::vector<std::string>& list1 = PetHalfName1[entry];

    if (list0.empty() || list1.empty())
    {
        CreatureInfo const* cinfo = GetCreatureTemplate(entry);
        char* petname = GetPetName(cinfo->family, sWorld.GetDefaultDbcLocale());
        if (!petname)
            petname = cinfo->Name;
        return std::string(petname);
    }

    return *(list0.begin() + urand(0, list0.size() - 1)) + *(list1.begin() + urand(0, list1.size() - 1));
}

uint32 ObjectMgr::GeneratePetNumber()
{
    return ++m_hiPetNumber;
}

void ObjectMgr::LoadCorpses()
{
    uint32 count = 0;
    //                                                           0           1           2           3            4    5          6          7       8       9      10     11        12    13           14        15    16
    QueryResult_AutoPtr result = CharacterDatabase.Query("SELECT position_x, position_y, position_z, orientation, map, displayId, itemCache, bytes1, bytes2, guild, flags, dynFlags, time, corpse_type, instance, guid, player FROM corpse WHERE corpse_type <> 0");

    if (!result)
    {


        sLog.outString(">> Loaded %u corpses", count);
        return;
    }


    do
    {

        Field* fields = result->Fetch();

        uint32 guid = fields[15].GetUInt32();

        Corpse* corpse = new Corpse;
        if (!corpse->LoadFromDB(guid, fields))
        {
            delete corpse;
            continue;
        }

        ObjectAccessor::Instance().AddCorpse(corpse);

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u corpses", count);
}

void ObjectMgr::LoadReputationOnKill()
{
    uint32 count = 0;

    //                                                       0            1                     2
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT creature_id, RewOnKillRepFaction1, RewOnKillRepFaction2,"
                                 //   3             4             5                   6             7             8                   9
                                 "IsTeamAward1, MaxStanding1, RewOnKillRepValue1, IsTeamAward2, MaxStanding2, RewOnKillRepValue2, TeamDependent "
                                 "FROM creature_onkill_reputation");

    if (!result)
    {


        sLog.outErrorDb(">> Loaded 0 creature award reputation definitions. DB table creature_onkill_reputation is empty.");
        return;
    }


    do
    {
        Field* fields = result->Fetch();

        uint32 creature_id = fields[0].GetUInt32();

        ReputationOnKillEntry repOnKill;
        repOnKill.repfaction1          = fields[1].GetUInt32();
        repOnKill.repfaction2          = fields[2].GetUInt32();
        repOnKill.is_teamaward1        = fields[3].GetBool();
        repOnKill.reputation_max_cap1  = fields[4].GetUInt32();
        repOnKill.repvalue1            = fields[5].GetInt32();
        repOnKill.is_teamaward2        = fields[6].GetBool();
        repOnKill.reputation_max_cap2  = fields[7].GetUInt32();
        repOnKill.repvalue2            = fields[8].GetInt32();
        repOnKill.team_dependent       = fields[9].GetUInt8();

        if (!GetCreatureTemplate(creature_id))
        {
            sLog.outErrorDb("Table creature_onkill_reputation has data for invalid creature entry (%u), skipped", creature_id);
            continue;
        }

        if (repOnKill.repfaction1)
        {
            FactionEntry const* factionEntry1 = sFactionStore.LookupEntry(repOnKill.repfaction1);
            if (!factionEntry1)
            {
                sLog.outErrorDb("Faction (faction.dbc) %u does not exist but is used in creature_onkill_reputation", repOnKill.repfaction1);
                continue;
            }
        }

        if (repOnKill.repfaction2)
        {
            FactionEntry const* factionEntry2 = sFactionStore.LookupEntry(repOnKill.repfaction2);
            if (!factionEntry2)
            {
                sLog.outErrorDb("Faction (faction.dbc) %u does not exist but is used in creature_onkill_reputation", repOnKill.repfaction2);
                continue;
            }
        }

        mRepOnKill[creature_id] = repOnKill;

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u creature award reputation definitions", count);
}

void ObjectMgr::LoadWeatherZoneChances()
{
    uint32 count = 0;

    //                                                       0     1                   2                   3                    4                   5                   6                    7                 8                 9                  10                  11                  12
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT zone, spring_rain_chance, spring_snow_chance, spring_storm_chance, summer_rain_chance, summer_snow_chance, summer_storm_chance, fall_rain_chance, fall_snow_chance, fall_storm_chance, winter_rain_chance, winter_snow_chance, winter_storm_chance FROM game_weather");

    if (!result)
    {


        sLog.outErrorDb(">> Loaded 0 weather definitions. DB table game_weather is empty.");
        return;
    }


    do
    {
        Field* fields = result->Fetch();

        uint32 zone_id = fields[0].GetUInt32();

        WeatherZoneChances& wzc = mWeatherZoneMap[zone_id];

        for (int season = 0; season < WEATHER_SEASONS; ++season)
        {
            wzc.data[season].rainChance  = fields[season * (MAX_WEATHER_TYPE - 1) + 1].GetUInt32();
            wzc.data[season].snowChance  = fields[season * (MAX_WEATHER_TYPE - 1) + 2].GetUInt32();
            wzc.data[season].stormChance = fields[season * (MAX_WEATHER_TYPE - 1) + 3].GetUInt32();

            if (wzc.data[season].rainChance > 100)
            {
                wzc.data[season].rainChance = 25;
                sLog.outErrorDb("Weather for zone %u season %u has wrong rain chance > 100%%", zone_id, season);
            }

            if (wzc.data[season].snowChance > 100)
            {
                wzc.data[season].snowChance = 25;
                sLog.outErrorDb("Weather for zone %u season %u has wrong snow chance > 100%%", zone_id, season);
            }

            if (wzc.data[season].stormChance > 100)
            {
                wzc.data[season].stormChance = 25;
                sLog.outErrorDb("Weather for zone %u season %u has wrong storm chance > 100%%", zone_id, season);
            }
        }

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u weather definitions", count);
}

void ObjectMgr::SaveCreatureRespawnTime(uint32 loguid, uint32 instance, time_t t)
{
    mCreatureRespawnTimes[MAKE_PAIR64(loguid, instance)] = t;
    WorldDatabase.PExecute("DELETE FROM creature_respawn WHERE guid = '%u' AND instance = '%u'", loguid, instance);
    if (t)
        WorldDatabase.PExecute("INSERT INTO creature_respawn VALUES ('%u', '" UI64FMTD "', '%u')", loguid, uint64(t), instance);
}

void ObjectMgr::DeleteCreatureData(uint32 guid)
{
    // remove mapid*cellid -> guid_set map
    CreatureData const* data = GetCreatureData(guid);
    if (data)
        RemoveCreatureFromGrid(guid, data);

    mCreatureDataMap.erase(guid);
}

void ObjectMgr::SaveGORespawnTime(uint32 loguid, uint32 instance, time_t t)
{
    mGORespawnTimes[MAKE_PAIR64(loguid, instance)] = t;
    WorldDatabase.PExecute("DELETE FROM gameobject_respawn WHERE guid = '%u' AND instance = '%u'", loguid, instance);
    if (t)
        WorldDatabase.PExecute("INSERT INTO gameobject_respawn VALUES ('%u', '" UI64FMTD "', '%u')", loguid, uint64(t), instance);
}

void ObjectMgr::DeleteRespawnTimeForInstance(uint32 instance)
{
    RespawnTimes::iterator next;

    for (RespawnTimes::iterator itr = mGORespawnTimes.begin(); itr != mGORespawnTimes.end(); itr = next)
    {
        next = itr;
        ++next;

        if (GUID_HIPART(itr->first) == instance)
            mGORespawnTimes.erase(itr);
    }

    for (RespawnTimes::iterator itr = mCreatureRespawnTimes.begin(); itr != mCreatureRespawnTimes.end(); itr = next)
    {
        next = itr;
        ++next;

        if (GUID_HIPART(itr->first) == instance)
            mCreatureRespawnTimes.erase(itr);
    }

    WorldDatabase.PExecute("DELETE FROM creature_respawn WHERE instance = '%u'", instance);
    WorldDatabase.PExecute("DELETE FROM gameobject_respawn WHERE instance = '%u'", instance);
}

void ObjectMgr::DeleteGOData(uint32 guid)
{
    // remove mapid*cellid -> guid_set map
    GameObjectData const* data = GetGOData(guid);
    if (data)
        RemoveGameobjectFromGrid(guid, data);

    mGameObjectDataMap.erase(guid);
}

void ObjectMgr::AddCorpseCellData(uint32 mapid, uint32 cellid, uint32 player_guid, uint32 instance)
{
    // corpses are always added to spawn mode 0 and they are spawned by their instance id
    CellObjectGuids& cell_guids = mMapObjectGuids[MAKE_PAIR32(mapid, 0)][cellid];
    cell_guids.corpses[player_guid] = instance;
}

void ObjectMgr::DeleteCorpseCellData(uint32 mapid, uint32 cellid, uint32 player_guid)
{
    // corpses are always added to spawn mode 0 and they are spawned by their instance id
    CellObjectGuids& cell_guids = mMapObjectGuids[MAKE_PAIR32(mapid, 0)][cellid];
    cell_guids.corpses.erase(player_guid);
}

void ObjectMgr::LoadQuestRelationsHelper(QuestRelations& map, std::string table, bool starter, bool go)
{
    map.clear();                                            // need for reload case

    uint32 count = 0;

    QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT id, quest, pool_entry FROM %s qr LEFT JOIN pool_quest pq ON qr.quest = pq.entry", table.c_str());

    if (!result)
    {


        sLog.outErrorDb(">> Loaded 0 quest relations from %s, table is empty.", table.c_str());
        return;
    }


    PooledQuestRelation* poolRelationMap = go ? &sPoolMgr.mQuestGORelation : &sPoolMgr.mQuestCreatureRelation;
    if (starter)
        poolRelationMap->clear();

    do
    {

        uint32 id     = result->Fetch()[0].GetUInt32();
        uint32 quest  = result->Fetch()[1].GetUInt32();
        uint32 poolId = result->Fetch()[2].GetUInt32();

        if (mQuestTemplates.find(quest) == mQuestTemplates.end())
        {
            sLog.outErrorDb("Table `%s: Quest %u listed for entry %u does not exist.", table.c_str(), quest, id);
            continue;
        }

        if (!poolId || !starter)
            map.insert(QuestRelations::value_type(id, quest));
        else if (starter)
            poolRelationMap->insert(PooledQuestRelation::value_type(quest, id));

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u quest relations from %s", count, table.c_str());
}

void ObjectMgr::LoadGameobjectQuestRelations()
{
    LoadQuestRelationsHelper(mGOQuestRelations, "gameobject_questrelation", true, true);

    for (QuestRelations::iterator itr = mGOQuestRelations.begin(); itr != mGOQuestRelations.end(); ++itr)
    {
        GameObjectInfo const* goInfo = GetGameObjectInfo(itr->first);
        if (!goInfo)
            sLog.outErrorDb("Table gameobject_questrelation has data for invalid gameobject entry (%u) and valid quest %u", itr->first, itr->second);
        else if (goInfo->type != GAMEOBJECT_TYPE_QUESTGIVER)
            sLog.outErrorDb("Table gameobject_questrelation has data gameobject entry (%u) for quest %u, but GO is not GAMEOBJECT_TYPE_QUESTGIVER", itr->first, itr->second);
    }
}

void ObjectMgr::LoadGameobjectInvolvedRelations()
{
    LoadQuestRelationsHelper(mGOQuestInvolvedRelations, "gameobject_involvedrelation", false, true);

    for (QuestRelations::iterator itr = mGOQuestInvolvedRelations.begin(); itr != mGOQuestInvolvedRelations.end(); ++itr)
    {
        GameObjectInfo const* goInfo = GetGameObjectInfo(itr->first);
        if (!goInfo)
            sLog.outErrorDb("Table gameobject_involvedrelation has data for invalid gameobject entry (%u) and valid quest %u", itr->first, itr->second);
        else if (goInfo->type != GAMEOBJECT_TYPE_QUESTGIVER)
            sLog.outErrorDb("Table gameobject_involvedrelation has data gameobject entry (%u) for quest %u, but GO is not GAMEOBJECT_TYPE_QUESTGIVER", itr->first, itr->second);
    }
}

void ObjectMgr::LoadCreatureQuestRelations()
{
    LoadQuestRelationsHelper(mCreatureQuestRelations, "creature_questrelation", true, false);

    for (QuestRelations::iterator itr = mCreatureQuestRelations.begin(); itr != mCreatureQuestRelations.end(); ++itr)
    {
        CreatureInfo const* cInfo = GetCreatureTemplate(itr->first);
        if (!cInfo)
            sLog.outErrorDb("Table creature_questrelation has data for invalid creature entry (%u) and valid quest %u", itr->first, itr->second);
        else if (!(cInfo->npcflag & UNIT_NPC_FLAG_QUESTGIVER))
            sLog.outErrorDb("Table creature_questrelation has creature entry (%u) for quest %u, but npcflag does not include UNIT_NPC_FLAG_QUESTGIVER", itr->first, itr->second);
    }
}

void ObjectMgr::LoadCreatureInvolvedRelations()
{
    LoadQuestRelationsHelper(mCreatureQuestInvolvedRelations, "creature_involvedrelation", false, false);

    for (QuestRelations::iterator itr = mCreatureQuestInvolvedRelations.begin(); itr != mCreatureQuestInvolvedRelations.end(); ++itr)
    {
        CreatureInfo const* cInfo = GetCreatureTemplate(itr->first);
        if (!cInfo)
            sLog.outErrorDb("Table creature_involvedrelation has data for invalid creature entry (%u) and valid quest %u", itr->first, itr->second);
        else if (!(cInfo->npcflag & UNIT_NPC_FLAG_QUESTGIVER))
            sLog.outErrorDb("Table creature_involvedrelation has creature entry (%u) for quest %u, but npcflag does not include UNIT_NPC_FLAG_QUESTGIVER", itr->first, itr->second);
    }
}

void ObjectMgr::LoadReservedPlayersNames()
{
    m_ReservedNames.clear();                                // need for reload case

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT name FROM reserved_name");

    uint32 count = 0;

    if (!result)
    {

        sLog.outString(">> Loaded %u reserved player names", count);
        return;
    }


    Field* fields;
    do
    {
        fields = result->Fetch();
        std::string name = fields[0].GetCppString();
        if (normalizePlayerName(name))
        {
            m_ReservedNames.insert(name);
            ++count;
        }
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u reserved player names", count);
}

enum LanguageType
{
    LT_BASIC_LATIN    = 0x0000,
    LT_EXTENDEN_LATIN = 0x0001,
    LT_CYRILLIC       = 0x0002,
    LT_EAST_ASIA      = 0x0004,
    LT_ANY            = 0xFFFF
};

static LanguageType GetRealmLanguageType(bool create)
{
    switch (sWorld.getConfig(CONFIG_REALM_ZONE))
    {
    case REALM_ZONE_UNKNOWN:                            // any language
    case REALM_ZONE_DEVELOPMENT:
    case REALM_ZONE_TEST_SERVER:
    case REALM_ZONE_QA_SERVER:
        return LT_ANY;
    case REALM_ZONE_UNITED_STATES:                      // extended-Latin
    case REALM_ZONE_OCEANIC:
    case REALM_ZONE_LATIN_AMERICA:
    case REALM_ZONE_ENGLISH:
    case REALM_ZONE_GERMAN:
    case REALM_ZONE_FRENCH:
    case REALM_ZONE_SPANISH:
        return LT_EXTENDEN_LATIN;
    case REALM_ZONE_KOREA:                              // East-Asian
    case REALM_ZONE_TAIWAN:
    case REALM_ZONE_CHINA:
        return LT_EAST_ASIA;
    case REALM_ZONE_RUSSIAN:                            // Cyrillic
        return LT_CYRILLIC;
    default:
        return create ? LT_BASIC_LATIN : LT_ANY;        // basic-Latin at create, any at login
    }
}

bool isValidString(std::wstring wstr, uint32 strictMask, bool numericOrSpace, bool create = false)
{
    if (strictMask == 0)                                       // any language, ignore realm
    {
        if (isExtendedLatinString(wstr, numericOrSpace))
            return true;
        if (isCyrillicString(wstr, numericOrSpace))
            return true;
        if (isEastAsianString(wstr, numericOrSpace))
            return true;
        return false;
    }

    if (strictMask & 0x2)                                    // realm zone specific
    {
        LanguageType lt = GetRealmLanguageType(create);
        if (lt & LT_EXTENDEN_LATIN)
            if (isExtendedLatinString(wstr, numericOrSpace))
                return true;
        if (lt & LT_CYRILLIC)
            if (isCyrillicString(wstr, numericOrSpace))
                return true;
        if (lt & LT_EAST_ASIA)
            if (isEastAsianString(wstr, numericOrSpace))
                return true;
    }

    if (strictMask & 0x1)                                    // basic Latin
    {
        if (isBasicLatinString(wstr, numericOrSpace))
            return true;
    }

    return false;
}

bool ObjectMgr::IsValidName(const std::string& name, bool create)
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return false;

    if (wname.size() < 1 || wname.size() > MAX_PLAYER_NAME)
        return false;

    uint32 strictMask = sWorld.getConfig(CONFIG_STRICT_PLAYER_NAMES);

    return isValidString(wname, strictMask, false, create);
}

bool ObjectMgr::IsValidCharterName(const std::string& name)
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return false;

    if (wname.size() < 1)
        return false;

    uint32 strictMask = sWorld.getConfig(CONFIG_STRICT_CHARTER_NAMES);

    return isValidString(wname, strictMask, true);
}

bool ObjectMgr::IsValidPetName(const std::string& name)
{
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return false;

    if (wname.size() < 1)
        return false;

    uint32 strictMask = sWorld.getConfig(CONFIG_STRICT_PET_NAMES);

    return isValidString(wname, strictMask, false);
}

int ObjectMgr::GetIndexForLocale(LocaleConstant loc)
{
    if (loc == LOCALE_enUS)
        return -1;

    for (size_t i = 0; i < m_LocalForIndex.size(); ++i)
        if (m_LocalForIndex[i] == loc)
            return i;

    return -1;
}

LocaleConstant ObjectMgr::GetLocaleForIndex(int i)
{
    if (i < 0 || i >= m_LocalForIndex.size())
        return LOCALE_enUS;

    return m_LocalForIndex[i];
}

int ObjectMgr::GetOrNewIndexForLocale(LocaleConstant loc)
{
    if (loc == LOCALE_enUS)
        return -1;

    for (size_t i = 0; i < m_LocalForIndex.size(); ++i)
        if (m_LocalForIndex[i] == loc)
            return i;

    m_LocalForIndex.push_back(loc);
    return m_LocalForIndex.size() - 1;
}

void ObjectMgr::LoadBattleMastersEntry()
{
    mBattleMastersMap.clear();                              // need for reload case

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry,bg_template FROM battlemaster_entry");

    uint32 count = 0;

    if (!result)
    {

        sLog.outString(">> Loaded 0 battlemaster entries - table is empty!");
        return;
    }


    do
    {
        ++count;

        Field* fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();
        uint32 bgTypeId  = fields[1].GetUInt32();

        mBattleMastersMap[entry] = bgTypeId;

    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u battlemaster entries", count);
}

void ObjectMgr::LoadGameObjectForQuests()
{
    mGameObjectForQuestSet.clear();                         // need for reload case

    uint32 count = 0;

    // collect GO entries for GO that must activated
    for (uint32 go_entry = 1; go_entry < sGOStorage.MaxEntry; ++go_entry)
    {
        GameObjectInfo const* goInfo = sGOStorage.LookupEntry<GameObjectInfo>(go_entry);
        if (!goInfo)
            continue;

        switch (goInfo->type)
        {
        // scan GO chest with loot including quest items
        case GAMEOBJECT_TYPE_CHEST:
            {
                uint32 loot_id = GameObject::GetLootId(goInfo);

                // find quest loot for GO
                if (goInfo->chest.questId || LootTemplates_Gameobject.HaveQuestLootFor(loot_id))
                {
                    mGameObjectForQuestSet.insert(go_entry);
                    ++count;
                }
                break;
            }
        case GAMEOBJECT_TYPE_GOOBER:
            {
                if (goInfo->goober.questId)                  //quests objects
                {
                    mGameObjectForQuestSet.insert(go_entry);
                    count++;
                }
                break;
            }
        default:
            break;
        }
    }

    sLog.outString(">> Loaded %u GameObject for quests", count);
}

bool ObjectMgr::LoadOregonStrings(DatabaseType& db, char const* table, int32 min_value, int32 max_value)
{
    int32 start_value = min_value;
    int32 end_value   = max_value;
    // some string can have negative indexes range
    if (start_value < 0)
    {
        if (end_value >= start_value)
        {
            sLog.outErrorDb("Table '%s' attempt loaded with invalid range (%d - %d), strings not loaded.", table, min_value, max_value);
            return false;
        }

        // real range (max+1,min+1) exaple: (-10,-1000) -> -999...-10+1
        std::swap(start_value, end_value);
        ++start_value;
        ++end_value;
    }
    else
    {
        if (start_value >= end_value)
        {
            sLog.outErrorDb("Table '%s' attempt loaded with invalid range (%d - %d), strings not loaded.", table, min_value, max_value);
            return false;
        }
    }

    // cleanup affected map part for reloading case
    for (OregonStringLocaleMap::iterator itr = mOregonStringLocaleMap.begin(); itr != mOregonStringLocaleMap.end();)
    {
        if (itr->first >= start_value && itr->first < end_value)
            mOregonStringLocaleMap.erase(itr++);
        else
            ++itr;
    }

    QueryResult_AutoPtr result = db.PQuery("SELECT entry,content_default,content_loc1,content_loc2,content_loc3,content_loc4,content_loc5,content_loc6,content_loc7,content_loc8 FROM %s", table);

    if (!result)
    {


        if (min_value == MIN_OREGON_STRING_ID)              // error only in case internal strings
            sLog.outErrorDb(">> Loaded 0 Oregon strings. DB table %s is empty. Cannot continue.", table);
        else
            sLog.outString(">> Loaded 0 string templates. DB table %s is empty.", table);
        return false;
    }

    uint32 count = 0;


    do
    {
        Field* fields = result->Fetch();

        int32 entry = fields[0].GetInt32();

        if (entry == 0)
        {
            sLog.outErrorDb("Table %s contain reserved entry 0, ignored.", table);
            continue;
        }
        else if (entry < start_value || entry >= end_value)
        {
            sLog.outErrorDb("Table %s contain entry %i out of allowed range (%d - %d), ignored.", table, entry, min_value, max_value);
            continue;
        }

        OregonStringLocale& data = mOregonStringLocaleMap[entry];

        if (data.Content.size() > 0)
        {
            sLog.outErrorDb("Table %s contain data for already loaded entry  %i (from another table?), ignored.", table, entry);
            continue;
        }

        data.Content.resize(1);
        ++count;

        // 0 -> default, idx in to idx+1
        data.Content[0] = fields[1].GetCppString();

        for (uint8 i = 1; i < MAX_LOCALE; ++i)
        {
            std::string str = fields[i + 1].GetCppString();
            if (!str.empty())
            {
                int idx = GetOrNewIndexForLocale(LocaleConstant(i));
                if (idx >= 0)
                {
                    // 0 -> default, idx in to idx+1
                    if (data.Content.size() <= idx + 1)
                        data.Content.resize(idx + 2);

                    data.Content[idx + 1] = str;
                }
            }
        }
    }
    while (result->NextRow());

    if (min_value == MIN_OREGON_STRING_ID)
        sLog.outString(">> Loaded %u Oregon strings from table %s", count, table);
    else
        sLog.outString(">> Loaded %u string templates from %s", count, table);

    return true;
}

const char* ObjectMgr::GetOregonString(int32 entry, int locale_idx) const
{
    // locale_idx == -1 -> default, locale_idx >= 0 in to idx+1
    // Content[0] always exist if exist OregonStringLocale
    if (OregonStringLocale const* msl = GetOregonStringLocale(entry))
    {
        if (msl->Content.size() > locale_idx + 1 && !msl->Content[locale_idx + 1].empty())
            return msl->Content[locale_idx + 1].c_str();
        else
            return msl->Content[0].c_str();
    }

    if (entry > 0)
        sLog.outErrorDb("Entry %i not found in oregon_string table.", entry);
    else
        sLog.outErrorDb("Oregon string entry %i not found in DB.", entry);
    return "<error>";
}

void ObjectMgr::LoadFishingBaseSkillLevel()
{
    mFishingBaseForArea.clear();                            // for reload case

    uint32 count = 0;
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry,skill FROM skill_fishing_base_level");

    if (!result)
    {


        sLog.outErrorDb(">> Loaded skill_fishing_base_level, table is empty!");
        return;
    }


    do
    {

        Field* fields = result->Fetch();
        uint32 entry  = fields[0].GetUInt32();
        int32 skill   = fields[1].GetInt32();

        AreaTableEntry const* fArea = GetAreaEntryByAreaID(entry);
        if (!fArea)
        {
            sLog.outErrorDb("AreaId %u defined in skill_fishing_base_level does not exist", entry);
            continue;
        }

        mFishingBaseForArea[entry] = skill;
        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u areas for fishing base skill level", count);
}

bool ObjectMgr::CheckDeclinedNames(std::wstring mainpart, DeclinedName const& names)
{
    for (int i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
    {
        std::wstring wname;
        if (!Utf8toWStr(names.name[i], wname))
            return false;

        if (mainpart != GetMainPartOfName(wname, i + 1))
            return false;
    }
    return true;
}

uint32 ObjectMgr::GetAreaTriggerScriptId(uint32 trigger_id)
{
    AreaTriggerScriptMap::const_iterator i = mAreaTriggerScripts.find(trigger_id);
    if (i != mAreaTriggerScripts.end())
        return i->second;
    return 0;
}

SkillRangeType GetSkillRangeType(SkillLineEntry const* pSkill, bool racial)
{
    switch (pSkill->categoryId)
    {
    case SKILL_CATEGORY_LANGUAGES:
        return SKILL_RANGE_LANGUAGE;
    case SKILL_CATEGORY_WEAPON:
        if (pSkill->id != SKILL_FIST_WEAPONS)
            return SKILL_RANGE_LEVEL;
        else
            return SKILL_RANGE_MONO;
    case SKILL_CATEGORY_ARMOR:
    case SKILL_CATEGORY_CLASS:
        if (pSkill->id != SKILL_POISONS && pSkill->id != SKILL_LOCKPICKING)
            return SKILL_RANGE_MONO;
        else
            return SKILL_RANGE_LEVEL;
    case SKILL_CATEGORY_SECONDARY:
    case SKILL_CATEGORY_PROFESSION:
        // not set skills for professions and racial abilities
        if (IsProfessionSkill(pSkill->id))
            return SKILL_RANGE_RANK;
        else if (racial)
            return SKILL_RANGE_NONE;
        else
            return SKILL_RANGE_MONO;
    default:
    case SKILL_CATEGORY_ATTRIBUTES:                     //not found in dbc
    case SKILL_CATEGORY_NOT_DISPLAYED:                  //only GENEREC(DND)
        return SKILL_RANGE_NONE;
    }
}

void ObjectMgr::LoadGameTele()
{
    m_GameTeleMap.clear();                                  // for reload case

    uint32 count = 0;
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT id, position_x, position_y, position_z, orientation, map, name FROM game_tele");

    if (!result)
    {


        sLog.outErrorDb(">> Loaded game_tele, table is empty!");
        return;
    }


    do
    {

        Field* fields = result->Fetch();

        uint32 id         = fields[0].GetUInt32();

        GameTele gt;

        gt.position_x     = fields[1].GetFloat();
        gt.position_y     = fields[2].GetFloat();
        gt.position_z     = fields[3].GetFloat();
        gt.orientation    = fields[4].GetFloat();
        gt.mapId          = fields[5].GetUInt32();
        gt.name           = fields[6].GetCppString();

        if (!MapManager::IsValidMapCoord(gt.mapId, gt.position_x, gt.position_y, gt.position_z, gt.orientation))
        {
            sLog.outErrorDb("Wrong position for id %u (name: %s) in game_tele table, ignoring.", id, gt.name.c_str());
            continue;
        }

        if (!Utf8toWStr(gt.name, gt.wnameLow))
        {
            sLog.outErrorDb("Wrong UTF8 name for id %u in game_tele table, ignoring.", id);
            continue;
        }

        wstrToLower(gt.wnameLow);

        m_GameTeleMap[id] = gt;

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u game tele's", count);
}

GameTele const* ObjectMgr::GetGameTele(const std::string& name) const
{
    // explicit name case
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return NULL;

    // converting string that we try to find to lower case
    wstrToLower(wname);

    // Alternative first GameTele what contains wnameLow as substring in case no GameTele location found
    const GameTele* alt = NULL;
    for (GameTeleMap::const_iterator itr = m_GameTeleMap.begin(); itr != m_GameTeleMap.end(); ++itr)
    {
        if (itr->second.wnameLow == wname)
            return &itr->second;
        else if (alt == NULL && itr->second.wnameLow.find(wname) != std::wstring::npos)
            alt = &itr->second;
    }

    return alt;
}

bool ObjectMgr::AddGameTele(GameTele& tele)
{
    // find max id
    uint32 new_id = 0;
    for (GameTeleMap::const_iterator itr = m_GameTeleMap.begin(); itr != m_GameTeleMap.end(); ++itr)
        if (itr->first > new_id)
            new_id = itr->first;

    // use next
    ++new_id;

    if (!Utf8toWStr(tele.name, tele.wnameLow))
        return false;

    wstrToLower(tele.wnameLow);

    m_GameTeleMap[new_id] = tele;

    return WorldDatabase.PExecuteLog("INSERT INTO game_tele (id,position_x,position_y,position_z,orientation,map,name) VALUES (%u,%f,%f,%f,%f,%d,'%s')",
                                     new_id, tele.position_x, tele.position_y, tele.position_z, tele.orientation, tele.mapId, tele.name.c_str());
}

bool ObjectMgr::DeleteGameTele(const std::string& name)
{
    // explicit name case
    std::wstring wname;
    if (!Utf8toWStr(name, wname))
        return false;

    // converting string that we try to find to lower case
    wstrToLower(wname);

    for (GameTeleMap::iterator itr = m_GameTeleMap.begin(); itr != m_GameTeleMap.end(); ++itr)
    {
        if (itr->second.wnameLow == wname)
        {
            WorldDatabase.PExecuteLog("DELETE FROM game_tele WHERE name = '%s'", itr->second.name.c_str());
            m_GameTeleMap.erase(itr);
            return true;
        }
    }

    return false;
}

void ObjectMgr::LoadTrainerSpell()
{
    // For reload case
    for (CacheTrainerSpellMap::iterator itr = m_mCacheTrainerSpellMap.begin(); itr != m_mCacheTrainerSpellMap.end(); ++itr)
        itr->second.Clear();
    m_mCacheTrainerSpellMap.clear();

    std::set<uint32> skip_trainers;

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry, spell,spellcost,reqskill,reqskillvalue,reqlevel FROM npc_trainer");

    if (!result)
    {
        sLog.outErrorDb(">> Loaded npc_trainer, table is empty!");
        return;
    }

    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 entry  = fields[0].GetUInt32();
        uint32 spell  = fields[1].GetUInt32();

        CreatureInfo const* cInfo = GetCreatureTemplate(entry);

        if (!cInfo)
        {
            sLog.outErrorDb("Table npc_trainer has entry for invalid creature template (Entry: %u), ignore", entry);
            continue;
        }

        if (!(cInfo->npcflag & UNIT_NPC_FLAG_TRAINER))
        {
            if (skip_trainers.find(entry) == skip_trainers.end())
            {
                sLog.outErrorDb("Table npc_trainer has data for creature template (Entry: %u) without trainer flag, ignore", entry);
                skip_trainers.insert(entry);
            }
            continue;
        }

        SpellEntry const* spellinfo = sSpellStore.LookupEntry(spell);
        if (!spellinfo)
        {
            sLog.outErrorDb("Table npc_trainer for Trainer (Entry: %u) has invalid spell %u, ignore", entry, spell);
            continue;
        }

        if (!SpellMgr::IsSpellValid(spellinfo))
        {
            sLog.outErrorDb("Table npc_trainer for Trainer (Entry: %u) has broken learning spell %u, ignore", entry, spell);
            continue;
        }

        TrainerSpell* pTrainerSpell = new TrainerSpell();
        pTrainerSpell->spell         = spell;
        pTrainerSpell->spellcost     = fields[2].GetUInt32();
        pTrainerSpell->reqskill      = fields[3].GetUInt32();
        pTrainerSpell->reqskillvalue = fields[4].GetUInt32();
        pTrainerSpell->reqlevel      = fields[5].GetUInt32();

        if (!pTrainerSpell->reqlevel)
            pTrainerSpell->reqlevel = spellinfo->spellLevel;

        TrainerSpellData& data = m_mCacheTrainerSpellMap[entry];

        if (SpellMgr::IsProfessionSpell(spell))
            data.trainerType = 2;

        data.spellList.push_back(pTrainerSpell);
        ++count;

    }
    while (result->NextRow());

    sLog.outString(">> Loaded Trainers %d", count);
}

void ObjectMgr::LoadVendors()
{
    // For reload case
    for (CacheVendorItemMap::iterator itr = m_mCacheVendorItemMap.begin(); itr != m_mCacheVendorItemMap.end(); ++itr)
        itr->second.Clear();
    m_mCacheVendorItemMap.clear();

    std::set<uint32> skip_vendors;

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry, item, maxcount, incrtime, ExtendedCost FROM npc_vendor");
    if (!result)
    {


        sLog.outErrorDb(">> Loaded npc_vendor, table is empty!");
        return;
    }


    uint32 count = 0;
    do
    {
        Field* fields = result->Fetch();

        uint32 entry        = fields[0].GetUInt32();
        uint32 item_id      = fields[1].GetUInt32();
        uint32 maxcount     = fields[2].GetUInt32();
        uint32 incrtime     = fields[3].GetUInt32();
        uint32 ExtendedCost = fields[4].GetUInt32();

        if (!IsVendorItemValid(entry, item_id, maxcount, incrtime, ExtendedCost, NULL, &skip_vendors))
            continue;

        VendorItemData& vList = m_mCacheVendorItemMap[entry];

        vList.AddItem(item_id, maxcount, incrtime, ExtendedCost);
        ++count;

    }
    while (result->NextRow());

    sLog.outString(">> Loaded %d Vendors ", count);
}

void ObjectMgr::LoadNpcTextId()
{

    m_mCacheNpcTextIdMap.clear();

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT npc_guid, textid FROM npc_gossip");
    if (!result)
    {


        sLog.outErrorDb(">> Loaded npc_gossip, table is empty!");
        return;
    }


    uint32 count = 0;
    uint32 guid, textid;
    do
    {

        Field* fields = result->Fetch();

        guid   = fields[0].GetUInt32();
        textid = fields[1].GetUInt32();

        if (!GetCreatureData(guid))
        {
            sLog.outErrorDb("Table npc_gossip has invalid creature (GUID: %u) entry, ignore. ", guid);
            continue;
        }
        if (!GetGossipText(textid))
        {
            sLog.outErrorDb("Table npc_gossip for creature (GUID: %u) has invalid Textid (%u), ignore. ", guid, textid);
            continue;
        }

        m_mCacheNpcTextIdMap[guid] = textid ;
        ++count;

    }
    while (result->NextRow());

    sLog.outString(">> Loaded %d NpcTextId ", count);
}

void ObjectMgr::LoadGossipMenu()
{
    m_mGossipMenusMap.clear();

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry, text_id FROM gossip_menu");

    if (!result)
    {
        sLog.outDebug(">> Loaded gossip_menu, table is empty!");
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        GossipMenus gMenu;

        gMenu.entry             = fields[0].GetUInt32();
        gMenu.text_id           = fields[1].GetUInt32();

        if (!GetGossipText(gMenu.text_id))
        {
            sLog.outErrorDb("Table gossip_menu entry %u is using invalid text_id %u", gMenu.entry, gMenu.text_id);
            continue;
        }

        m_mGossipMenusMap.insert(GossipMenusMap::value_type(gMenu.entry, gMenu));

        ++count;
    }
    while (result->NextRow());

    sLog.outString( ">> Loaded %u gossip_menu entries", count);
}

void ObjectMgr::LoadGossipMenuItems()
{
    m_mGossipMenuItemsMap.clear();

    QueryResult_AutoPtr result = WorldDatabase.Query(
                                     "SELECT menu_id, id, option_icon, option_text, option_id, npc_option_npcflag, "
                                     "action_menu_id, action_poi_id, action_script_id, box_coded, box_money, box_text "
                                     "FROM gossip_menu_option");

    if (!result)
    {
        sLog.outErrorDb(">> Loaded gossip_menu_option, table is empty!");
        return;
    }

    uint32 count = 0;

    std::set<uint32> gossipScriptSet;

    for (ScriptMapMap::const_iterator itr = sGossipScripts.begin(); itr != sGossipScripts.end(); ++itr)
        gossipScriptSet.insert(itr->first);

    do
    {
        Field* fields = result->Fetch();

        GossipMenuItems gMenuItem;

        gMenuItem.menu_id               = fields[0].GetUInt32();
        gMenuItem.id                    = fields[1].GetUInt32();
        gMenuItem.option_icon           = fields[2].GetUInt8();
        gMenuItem.option_text           = fields[3].GetCppString();
        gMenuItem.option_id             = fields[4].GetUInt32();
        gMenuItem.npc_option_npcflag    = fields[5].GetUInt32();
        gMenuItem.action_menu_id        = fields[6].GetUInt32();
        gMenuItem.action_poi_id         = fields[7].GetUInt32();
        gMenuItem.action_script_id      = fields[8].GetUInt32();
        gMenuItem.box_coded             = fields[9].GetUInt8() != 0;
        gMenuItem.box_money             = fields[10].GetUInt32();
        gMenuItem.box_text              = fields[11].GetCppString();

        if (gMenuItem.option_icon >= GOSSIP_ICON_MAX)
        {
            sLog.outErrorDb("Table gossip_menu_option for menu %u, id %u has unknown icon id %u. Replacing with GOSSIP_ICON_CHAT", gMenuItem.menu_id, gMenuItem.id, gMenuItem.option_icon);
            gMenuItem.option_icon = GOSSIP_ICON_CHAT;
        }

        if (gMenuItem.option_id == GOSSIP_OPTION_NONE)
            sLog.outErrorDb("Table gossip_menu_option for menu %u, id %u use option id GOSSIP_OPTION_NONE. Option will never be used", gMenuItem.menu_id, gMenuItem.id);

        if (gMenuItem.option_id >= GOSSIP_OPTION_MAX)
            sLog.outErrorDb("Table gossip_menu_option for menu %u, id %u has unknown option id %u. Option will not be used", gMenuItem.menu_id, gMenuItem.id, gMenuItem.option_id);

        if (gMenuItem.action_script_id)
        {
            if (gMenuItem.option_id != GOSSIP_OPTION_GOSSIP)
            {
                sLog.outErrorDb("Table gossip_menu_option for menu %u, id %u has action_script_id %u but option_id is not GOSSIP_OPTION_GOSSIP, ignoring", gMenuItem.menu_id, gMenuItem.id, gMenuItem.action_script_id);
                continue;
            }

            if (sGossipScripts.find(gMenuItem.action_script_id) == sGossipScripts.end())
            {
                sLog.outErrorDb("Table gossip_menu_option for menu %u, id %u has action_script_id %u that does not exist in `gossip_scripts`, ignoring", gMenuItem.menu_id, gMenuItem.id, gMenuItem.action_script_id);
                continue;
            }

            gossipScriptSet.erase(gMenuItem.action_script_id);
        }

        m_mGossipMenuItemsMap.insert(GossipMenuItemsMap::value_type(gMenuItem.menu_id, gMenuItem));

        ++count;
    }
    while (result->NextRow());

    if (!gossipScriptSet.empty())
    {
        for (std::set<uint32>::const_iterator itr = gossipScriptSet.begin(); itr != gossipScriptSet.end(); ++itr)
            sLog.outErrorDb("Table `gossip_scripts` contain unused script, id %u.", *itr);
    }

    sLog.outString(">> Loaded %u gossip_menu_option entries", count);
}

void ObjectMgr::AddVendorItem(uint32 entry, uint32 item, uint32 maxcount, uint32 incrtime, uint32 extendedcost, bool savetodb)
{
    VendorItemData& vList = m_mCacheVendorItemMap[entry];
    vList.AddItem(item, maxcount, incrtime, extendedcost);

    if (savetodb) WorldDatabase.PExecuteLog("INSERT INTO npc_vendor (entry,item,maxcount,incrtime,extendedcost) VALUES('%u','%u','%u','%u','%u')", entry, item, maxcount, incrtime, extendedcost);
}

bool ObjectMgr::RemoveVendorItem(uint32 entry, uint32 item, bool savetodb)
{
    CacheVendorItemMap::iterator  iter = m_mCacheVendorItemMap.find(entry);
    if (iter == m_mCacheVendorItemMap.end())
        return false;

    if (!iter->second.FindItem(item))
        return false;

    iter->second.RemoveItem(item);
    if (savetodb) WorldDatabase.PExecuteLog("DELETE FROM npc_vendor WHERE entry='%u' AND item='%u'", entry, item);
    return true;
}

bool ObjectMgr::IsVendorItemValid(uint32 vendor_entry, uint32 item_id, uint32 maxcount, uint32 incrtime, uint32 ExtendedCost, Player* pl, std::set<uint32>* skip_vendors, uint32 ORnpcflag) const
{
    CreatureInfo const* cInfo = GetCreatureTemplate(vendor_entry);
    if (!cInfo)
    {
        if (pl)
            ChatHandler(pl).SendSysMessage(LANG_COMMAND_VENDORSELECTION);
        else
            sLog.outErrorDb("Table (game_event_)npc_vendor has data for invalid creature template (Entry: %u), ignore", vendor_entry);
        return false;
    }

    if (!((cInfo->npcflag | ORnpcflag) & UNIT_NPC_FLAG_VENDOR))
    {
        if (!skip_vendors || skip_vendors->count(vendor_entry) == 0)
        {
            if (pl)
                ChatHandler(pl).SendSysMessage(LANG_COMMAND_VENDORSELECTION);
            else
                sLog.outErrorDb("Table (game_event_)npc_vendor has data for creature template (Entry: %u) without vendor flag, ignore", vendor_entry);

            if (skip_vendors)
                skip_vendors->insert(vendor_entry);
        }
        return false;
    }

    if (!GetItemPrototype(item_id))
    {
        if (pl)
            ChatHandler(pl).PSendSysMessage(LANG_ITEM_NOT_FOUND, item_id);
        else
            sLog.outErrorDb("Table (game_event_)npc_vendor for Vendor (Entry: %u) has invalid item (%u) in item list, ignore", vendor_entry, item_id);
        return false;
    }

    if (ExtendedCost && !sItemExtendedCostStore.LookupEntry(ExtendedCost))
    {
        if (pl)
            ChatHandler(pl).PSendSysMessage(LANG_EXTENDED_COST_NOT_EXIST, ExtendedCost);
        else
            sLog.outErrorDb("Table (game_event_)npc_vendor has Item (Entry: %u) with invalid ExtendedCost (%u) for vendor (%u), ignore", item_id, ExtendedCost, vendor_entry);
        return false;
    }

    if (maxcount > 0 && incrtime == 0)
    {
        if (pl)
            ChatHandler(pl).PSendSysMessage("MaxCount != 0 (%u) but IncrTime == 0", maxcount);
        else
            sLog.outErrorDb("Table (game_event_)npc_vendor has maxcount (%u) for item %u of vendor (Entry: %u) but incrtime=0, ignore", maxcount, item_id, vendor_entry);
        return false;
    }
    else if (maxcount == 0 && incrtime > 0)
    {
        if (pl)
            ChatHandler(pl).PSendSysMessage("MaxCount == 0 but IncrTime<>=0");
        else
            sLog.outErrorDb("Table (game_event_)npc_vendor has maxcount=0 for item %u of vendor (Entry: %u) but incrtime<>0, ignore", item_id, vendor_entry);
        return false;
    }

    VendorItemData const* vItems = GetNpcVendorItemList(vendor_entry);
    if (!vItems)
        return true;                                        // later checks for non-empty lists

    if (vItems->FindItem(item_id))
    {
        if (pl)
            ChatHandler(pl).PSendSysMessage(LANG_ITEM_ALREADY_IN_LIST, item_id);
        else
            sLog.outErrorDb("Table (game_event_)npc_vendor has duplicate items %u for vendor (Entry: %u), ignore", item_id, vendor_entry);
        return false;
    }

    if (vItems->GetItemCount() >= MAX_VENDOR_ITEMS)
    {
        if (pl)
            ChatHandler(pl).SendSysMessage(LANG_COMMAND_ADDVENDORITEMITEMS);
        else
            sLog.outErrorDb("Table npc_vendor has too many items (%u >= %i) for vendor (Entry: %u), ignore", vItems->GetItemCount(), MAX_VENDOR_ITEMS, vendor_entry);
        return false;
    }

    return true;
}

void ObjectMgr::LoadScriptNames()
{
    m_scriptNames.push_back("");
    QueryResult_AutoPtr result = WorldDatabase.Query(
                                     "SELECT DISTINCT(ScriptName) FROM creature_template WHERE ScriptName <> '' "
                                     "UNION "
                                     "SELECT DISTINCT(ScriptName) FROM gameobject_template WHERE ScriptName <> '' "
                                     "UNION "
                                     "SELECT DISTINCT(ScriptName) FROM item_template WHERE ScriptName <> '' "
                                     "UNION "
                                     "SELECT DISTINCT(ScriptName) FROM areatrigger_scripts WHERE ScriptName <> '' "
                                     "UNION "
                                      "SELECT DISTINCT(ScriptName) FROM conditions WHERE ScriptName <> '' "
                                      "UNION "
                                     "SELECT DISTINCT(script) FROM instance_template WHERE script <> ''");

    if (result)
    {
        do
        {
            m_scriptNames.push_back((*result)[0].GetString());
        }
        while (result->NextRow());
    }

    //OnEvent Changes
    m_scriptNames.push_back("scripted_on_events");

    std::sort(m_scriptNames.begin(), m_scriptNames.end());
}

uint32 ObjectMgr::GetScriptId(const char* name)
{
    // use binary search to find the script name in the sorted vector
    // assume "" is the first element
    if (!name) return 0;
    ScriptNameMap::const_iterator itr =
        std::lower_bound(m_scriptNames.begin(), m_scriptNames.end(), name);
    if (itr == m_scriptNames.end() || *itr != name) return 0;
    return itr - m_scriptNames.begin();
}

void ObjectMgr::CheckScripts(ScriptsType type, std::set<int32>& ids)
{
    ScriptMapMap* scripts = GetScriptsMapByType(type);
    if (!scripts)
        return;
    for (ScriptMapMap::const_iterator itrMM = scripts->begin(); itrMM != scripts->end(); ++itrMM)
    {
        for (ScriptMap::const_iterator itrM = itrMM->second.begin(); itrM != itrMM->second.end(); ++itrM)
        {
            switch (itrM->second.command)
            {
            case SCRIPT_COMMAND_TALK:
                {
                    if (!GetOregonStringLocale (itrM->second.Talk.TextID))
                        sLog.outErrorDb("Table `db_script_string` not has string id  %u used db script (ID: %u)", itrM->second.Talk.TextID, itrMM->first);

                    if (ids.find(itrM->second.Talk.TextID) != ids.end())
                        ids.erase(itrM->second.Talk.TextID);
                }
            default:
                break;
            }
        }
    }
}

void ObjectMgr::LoadDbScriptStrings()
{
    LoadOregonStrings(WorldDatabase, "db_script_string", MIN_DB_SCRIPT_STRING_ID, MAX_DB_SCRIPT_STRING_ID);

    std::set<int32> ids;

    for (int32 i = MIN_DB_SCRIPT_STRING_ID; i < MAX_DB_SCRIPT_STRING_ID; ++i)
        if (GetOregonStringLocale(i))
            ids.insert(i);

    for (int type = SCRIPTS_FIRST; type < SCRIPTS_LAST; ++type)
        CheckScripts(ScriptsType(type), ids);

    for (std::set<int32>::const_iterator itr = ids.begin(); itr != ids.end(); ++itr)
        sLog.outErrorDb("Table `db_script_string` has unused string id  %u", *itr);
}

// Functions for scripting access
uint32 GetAreaTriggerScriptId(uint32 trigger_id)
{
    return sObjectMgr.GetAreaTriggerScriptId(trigger_id);
}

bool LoadOregonStrings(DatabaseType& db, char const* table, int32 start_value, int32 end_value)
{
    // MAX_DB_SCRIPT_STRING_ID is max allowed negative value for scripts (scrpts can use only more deep negative values
    // start/end reversed for negative values
    if (start_value > MAX_DB_SCRIPT_STRING_ID || end_value >= start_value)
    {
        sLog.outErrorDb("Table '%s' attempt loaded with reserved by core range (%d - %d), strings not loaded.", table, start_value, end_value + 1);
        return false;
    }

    return sObjectMgr.LoadOregonStrings(db, table, start_value, end_value);
}

uint32 GetScriptId(const char* name)
{
    return sObjectMgr.GetScriptId(name);
}

ObjectMgr::ScriptNameMap& GetScriptNames()
{
    return sObjectMgr.GetScriptNames();
}

GameObjectInfo const* GetGameObjectInfo(uint32 id)
{
    return sObjectMgr.GetGameObjectInfo(id);
}

CreatureInfo const* GetCreatureTemplate(uint32 id)
{
    return sObjectMgr.GetCreatureTemplate(id);
}

CreatureInfo const* GetCreatureTemplateStore(uint32 entry)
{
    return sCreatureStorage.LookupEntry<CreatureInfo>(entry);
}

Quest const* GetQuestTemplateStore(uint32 entry)
{
    return sObjectMgr.GetQuestTemplate(entry);
}

void ObjectMgr::LoadTransportEvents()
{

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT entry, waypoint_id, event_id FROM transport_events");

    if (!result)
    {
        sLog.outString(">> Transport events table is empty");
        return;
    }


    do
    {

        Field* fields = result->Fetch();

        //Load event values
        uint32 entry = fields[0].GetUInt32();
        uint32 waypoint_id = fields[1].GetUInt32();
        uint32 event_id = fields[2].GetUInt32();

        uint32 event_count = (entry * 100) + waypoint_id;
        TransportEventMap[event_count] = event_id;
    }
    while (result->NextRow());

    sLog.outString("\n>> Loaded " UI64FMTD " transport events \n", result->GetRowCount());
}

