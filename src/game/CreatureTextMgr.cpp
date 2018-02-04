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
#include "DatabaseEnv.h"
#include "ObjectMgr.h"
#include "Cell.h"
#include "CellImpl.h"
#include "Chat.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CreatureTextMgr.h"
#include "World.h"

class CreatureTextBuilder
{
public:
    CreatureTextBuilder(WorldObject* obj, uint8 gender, ChatMsg msgtype, uint8 textGroup, uint32 id, uint32 language, WorldObject* target)
        : _source(obj), _gender(gender), _msgType(msgtype), _textGroup(textGroup), _textId(id), _language(language), _target(target) { }

    size_t operator()(WorldPacket* data, int loc_idx) const
    {
        std::string text = sCreatureTextMgr->GetLocalizedChatString(_source->GetEntry(), _gender, _textGroup, _textId, loc_idx);

        return CreatureTextMgr::BuildMonsterChat(data, _source, _msgType, text, Language(_language), _target, loc_idx);
    }

private:
    WorldObject* _source;
    uint8 _gender;
    ChatMsg _msgType;
    uint8 _textGroup;
    uint32 _textId;
    uint32 _language;
    WorldObject* _target;
};

class PlayerTextBuilder
{
public:
    PlayerTextBuilder(WorldObject* obj, WorldObject* speaker, uint8 gender, ChatMsg msgtype, uint8 textGroup, uint32 id, uint32 language, WorldObject* target)
        : _source(obj), _talker(speaker), _gender(gender), _msgType(msgtype), _textGroup(textGroup), _textId(id), _language(language), _target(target) { }

    size_t operator()(WorldPacket* data, int loc_idx) const
    {
        std::string const& text = sCreatureTextMgr->GetLocalizedChatString(_source->GetEntry(), _gender, _textGroup, _textId, loc_idx);

        return CreatureTextMgr::BuildMonsterChat(data, _talker, _msgType, text, Language(_language), _target, loc_idx);
    }

private:
    WorldObject* _source;
    WorldObject* _talker;
    uint8 _gender;
    ChatMsg _msgType;
    uint8 _textGroup;
    uint32 _textId;
    uint32 _language;
    WorldObject* _target;
};

void CreatureTextMgr::LoadCreatureTexts()
{
    uint32 oldMSTime = getMSTime();

    mTextMap.clear(); // for reload case
    //all currently used temp texts are NOT reset
                                                      
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT CreatureID, GroupID, ID, Text, Type, Language, Probability, Emote, Duration, Sound, TextRange FROM creature_text");

    if (!result)
    {
        sLog.outString(">> Loaded 0 ceature texts. DB table `creature_text` is empty.");

        return;
    }

    uint32 textCount = 0;

    do
    {
        Field* fields = result->Fetch();
        CreatureTextEntry temp;

        temp.creatureId      = fields[0].GetUInt32();
        temp.groupId         = fields[1].GetUInt8();
        temp.id              = fields[2].GetUInt8();
        temp.text            = fields[3].GetString();
        temp.type            = ChatMsg(fields[4].GetUInt8());
        temp.lang            = Language(fields[5].GetUInt8());
        temp.probability     = fields[6].GetFloat();
        temp.emote           = Emote(fields[7].GetUInt32());
        temp.duration        = fields[8].GetUInt32();
        temp.sound           = fields[9].GetUInt32();
        temp.TextRange       = CreatureTextRange(fields[10].GetUInt8());

        if (temp.sound)
        {
            if (!sSoundEntriesStore.LookupEntry(temp.sound))
            {
                sLog.outError("CreatureTextMgr: Entry %u, Group %u in table `creature_text` has Sound %u but sound does not exist.", temp.creatureId, temp.groupId, temp.sound);
                temp.sound = 0;
            }
        }

        if (!GetLanguageDescByID(temp.lang))
        {
            sLog.outError("CreatureTextMgr: Entry %u, Group %u in table `creature_text` using Language %u but Language does not exist.", temp.creatureId, temp.groupId, uint32(temp.lang));
            temp.lang = LANG_UNIVERSAL;
        }

        if (temp.type >= MAX_CHAT_MSG_TYPE)
        {
            sLog.outError("CreatureTextMgr: Entry %u, Group %u in table `creature_text` has Type %u but this Chat Type does not exist.", temp.creatureId, temp.groupId, uint32(temp.type));
            temp.type = CHAT_MSG_SAY;
        }

        if (temp.emote)
        {
            if (!sEmotesStore.LookupEntry(temp.emote))
            {
                sLog.outError("CreatureTextMgr: Entry %u, Group %u in table `creature_text` has Emote %u but emote does not exist.", temp.creatureId, temp.groupId, uint32(temp.emote));
                temp.emote = EMOTE_ONESHOT_NONE;
            }
        }

        if (temp.TextRange > TEXT_RANGE_WORLD)
        {
            sLog.outError("CreatureTextMgr: Entry %u, Group %u, Id %u in table `creature_text` has incorrect TextRange %u.", temp.creatureId, temp.groupId, temp.id, temp.TextRange);
            temp.TextRange = TEXT_RANGE_NORMAL;
        }

        // add the text into our entry's group
        mTextMap[temp.creatureId][temp.groupId].push_back(temp);

        ++textCount;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u creature texts for %lu creatures in %u ms", textCount, mTextMap.size(), GetMSTimeDiffToNow(oldMSTime));
}

void CreatureTextMgr::LoadCreatureTextLocales()
{
    uint32 oldMSTime = getMSTime();

    mLocaleTextMap.clear(); // for reload case

    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT CreatureID, GroupID, ID, Locale, Text FROM creature_text_locale");

    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();
        uint32 creatureId = fields[0].GetUInt32();
        uint32 groupId = fields[1].GetUInt8();
        uint32 id = fields[2].GetUInt8();
        std::string localeName = fields[3].GetString();
        std::string text = fields[4].GetString();

        CreatureTextLocale& data = mLocaleTextMap[CreatureTextId(creatureId, groupId, id)];
        LocaleConstant locale = GetLocaleByName(localeName);
        if (locale == LOCALE_enUS)
            continue;

        ObjectMgr::AddLocaleString(text, locale, data.Text);
    } while (result->NextRow());

    sLog.outString(">> Loaded %u creature localized texts in %u ms", int32(mLocaleTextMap.size()), GetMSTimeDiffToNow(oldMSTime));

}

uint32 CreatureTextMgr::SendChat(Creature* source, uint8 textGroup, WorldObject* whisperTarget /*= nullptr*/, ChatMsg msgType /*= CHAT_MSG_ADDON*/, Language language /*= LANG_ADDON*/, CreatureTextRange range /*= TEXT_RANGE_NORMAL*/, uint32 sound /*= 0*/, Team team /*= TEAM_OTHER*/, bool gmOnly /*= false*/, Player* srcPlr /*= nullptr*/)
{
    if (!source)
        return 0;

    CreatureTextMap::const_iterator sList = mTextMap.find(source->GetEntry());
    if (sList == mTextMap.end())
    {
        sLog.outError("CreatureTextMgr: Could not find Text for Creature %s (Entry %u, GUID %u) in 'creature_text' table. Ignoring.", source->GetName(), source->GetEntry(), source->GetGUIDLow());
        return 0;
    }

    CreatureTextHolder const& textHolder = sList->second;
    CreatureTextHolder::const_iterator itr = textHolder.find(textGroup);
    if (itr == textHolder.end())
    {
        sLog.outError("CreatureTextMgr: Could not find TextGroup %u for Creature %s (Entry %u, GUID %u) in 'creature_text' table. Ignoring.", uint32(textGroup), source->GetName(), source->GetEntry(), source->GetGUIDLow());
        return 0;
    }

    CreatureTextGroup const& textGroupContainer = itr->second;  //has all texts in the group
    CreatureTextRepeatIds repeatGroup = GetRepeatGroup(source, textGroup);//has all textIDs from the group that were already said
    CreatureTextGroup tempGroup;//will use this to talk after sorting repeatGroup

    for (CreatureTextGroup::const_iterator giter = textGroupContainer.begin(); giter != textGroupContainer.end(); ++giter)
        if (std::find(repeatGroup.begin(), repeatGroup.end(), giter->id) == repeatGroup.end())
            tempGroup.push_back(*giter);

    if (tempGroup.empty())
    {
        source->ClearTextRepeatGroup(textGroup);
        tempGroup = textGroupContainer;
    }

    uint8 count = 0;
    float lastChance = -1;
    bool isEqualChanced = true;

    float totalChance = 0;

    for (CreatureTextGroup::const_iterator iter = tempGroup.begin(); iter != tempGroup.end(); ++iter)
    {
        if (lastChance >= 0 && lastChance != iter->probability)
            isEqualChanced = false;

        lastChance = iter->probability;
        totalChance += iter->probability;
        ++count;
    }

    int32 offset = -1;
    if (!isEqualChanced)
    {
        for (CreatureTextGroup::const_iterator iter = tempGroup.begin(); iter != tempGroup.end(); ++iter)
        {
            uint32 chance = uint32(iter->probability);
            uint32 r = urand(0, 100);
            ++offset;
            if (r <= chance)
                break;
        }
    }

    uint32 pos = 0;
    if (isEqualChanced || offset < 0)
        pos = urand(0, count - 1);
    else if (offset >= 0)
        pos = offset;

    CreatureTextGroup::const_iterator iter = tempGroup.begin() + pos;

    ChatMsg finalType = (msgType == CHAT_MSG_ADDON) ? iter->type : msgType;
    Language finalLang = (language == LANG_ADDON) ? iter->lang : language;
    uint32 finalSound = sound ? sound : iter->sound;

    if (range == TEXT_RANGE_NORMAL)
        range = iter->TextRange;

    if (finalSound)
        SendSound(source, finalSound, finalType, whisperTarget, range, team, gmOnly);

    Unit* finalSource = source;
    if (srcPlr)
        finalSource = srcPlr;

    if (iter->emote)
        SendEmote(finalSource, iter->emote);

    if (srcPlr)
    {
        PlayerTextBuilder builder(source, finalSource, finalSource->getGender(), finalType, iter->groupId, iter->id, finalLang, whisperTarget);
        SendChatPacket(finalSource, builder, finalType, whisperTarget, range, team, gmOnly);
    }
    else
    {
        CreatureTextBuilder builder(finalSource, finalSource->getGender(), finalType, iter->groupId, iter->id, finalLang, whisperTarget);
        SendChatPacket(finalSource, builder, finalType, whisperTarget, range, team, gmOnly);
    }

    if (isEqualChanced || totalChance == 100.0f)
        SetRepeatId(source, textGroup, iter->id);

    return iter->duration;
}

size_t CreatureTextMgr::BuildMonsterChat(WorldPacket* data, WorldObject* source, ChatMsg msgtype, std::string text, Language language, WorldObject* whisperTarget, int loc_idx)
{
    if (!source)
        return 0;

    size_t receiverGUIDPos = 0;
    data->Initialize(SMSG_MESSAGECHAT);

    std::string senderName = "";
    uint64 senderGuid = 0;
    if (source)
    {
        senderGuid = source->GetGUID();
        senderName = source->GetNameForLocaleIdx(loc_idx);
    }

    std::string receiverName = "";
    uint64 receiverGuid = 0;
    if (whisperTarget)
    {
        receiverGuid = whisperTarget->GetGUID();
        receiverName = whisperTarget->GetNameForLocaleIdx(loc_idx);
    }

    *data << (uint8)msgtype;
    *data << (uint32)language;
    *data << (uint64)senderGuid;
    *data << (uint32)0; // 2.1.0
    *data << (uint32)(senderName.length() + 1);
    *data << senderName.c_str();
    receiverGUIDPos = data->wpos();
    *data << (uint64)receiverGuid; // Unit Target
    if (whisperTarget && !IS_PLAYER_GUID(receiverGuid)) //can only whisper players
    {
        *data << uint32(receiverName.length() + 1);                         // string listener_name_length
        *data << receiverName.c_str();                                      // string listener_name
    }
    *data << (uint32)(text.length() + 1);
    *data << text;
    *data << (uint8)0; // ChatTag

    return receiverGUIDPos;
}

void CreatureTextMgr::SendNonChatPacket(WorldPacket* data, WorldObject* source, ChatMsg msgtype, WorldObject* whisperTarget, CreatureTextRange range, Team team, bool gmOnly) const
{
    if (!source)
        return;

    switch (msgtype)
    {
    case CHAT_MSG_MONSTER_PARTY:
    {
        if (!whisperTarget)
            return;

        if (Player* whisperPlayer = whisperTarget->ToPlayer())
        {
            if (Group* group = whisperPlayer->GetGroup())
                group->BroadcastPacket(data, false);
        }
        return;
    }
    case CHAT_MSG_MONSTER_WHISPER:
    case CHAT_MSG_RAID_BOSS_WHISPER:
    {
        if (range == TEXT_RANGE_NORMAL) // ignores team and gmOnly
        {
            if (!whisperTarget || whisperTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            whisperTarget->ToPlayer()->GetSession()->SendPacket(data);
            return;
        }
    }
    break;
    case CHAT_MSG_SAY://default dist
    default:
        break;
    }

    switch (range)
    {
        case TEXT_RANGE_AREA:
            {
                uint32 areaId = source->GetAreaId();
                Map::PlayerList const& pList = source->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
                    if (itr->GetSource()->GetAreaId() == areaId && (!team || (team && itr->GetSource()->GetTeam() == team)) && (!gmOnly || itr->GetSource()->IsGameMaster()))
                        itr->GetSource()->GetSession()->SendPacket(data);
            }
            return;
        case TEXT_RANGE_ZONE:
            {
                uint32 zoneId = source->GetZoneId();
                Map::PlayerList const& pList = source->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
                    if (itr->GetSource()->GetZoneId() == zoneId && (!team || (team && itr->GetSource()->GetTeam() == team)) && (!gmOnly || itr->GetSource()->IsGameMaster()))
                        itr->GetSource()->GetSession()->SendPacket(data);
            }
            return;
        case TEXT_RANGE_MAP:
            {
                Map::PlayerList const& pList = source->GetMap()->GetPlayers();
                for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
                    if (!team || ((team && itr->GetSource()->GetTeam() == team) && (!gmOnly || itr->GetSource()->IsGameMaster())))
                        itr->GetSource()->GetSession()->SendPacket(data);
            }
            return;
        case TEXT_RANGE_WORLD:
            {
                const SessionMap smap = sWorld.GetAllSessions();
                for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
                    if (Player* plr = (*iter).second->GetPlayer())
                        if (plr->GetSession() && (!team || (team && plr->GetTeam() == team)) && (!gmOnly || plr->IsGameMaster()))
                            plr->GetSession()->SendPacket(data);
            }
            return;
        case TEXT_RANGE_NORMAL:
        default:
            break;
    }

    float dist = GetRangeForChatType(msgtype);
    source->SendMessageToSetInRange(data, dist, true);
}

float CreatureTextMgr::GetRangeForChatType(ChatMsg msgType) const
{
    float dist = sWorld.getConfig(CONFIG_LISTEN_RANGE_SAY);
    switch (msgType)
    {
        case CHAT_MSG_MONSTER_YELL:
            dist = sWorld.getConfig(CONFIG_LISTEN_RANGE_YELL);
            break;
        case CHAT_MSG_MONSTER_EMOTE:
        case CHAT_MSG_RAID_BOSS_EMOTE:
            dist = sWorld.getConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE);
            break;
        default:
            break;
    }

    return dist;
}

void CreatureTextMgr::SendSound(Creature* source, uint32 sound, ChatMsg msgType, WorldObject* whisperTarget, CreatureTextRange range, Team team, bool gmOnly)
{
    if (!sound || !source)
        return;

    WorldPacket data(SMSG_PLAY_SOUND, 4);
    data << uint32(sound);
    SendNonChatPacket(&data, source, msgType, whisperTarget, range, team, gmOnly);
}

void CreatureTextMgr::SendEmote(Unit* source, uint32 emote)
{
    if (!source)
        return;

    source->HandleEmoteCommand(emote);
}

void CreatureTextMgr::SetRepeatId(Creature* source, uint8 textGroup, uint8 id)
{
    if (!source)
        return;

    source->SetTextRepeatId(textGroup, id);
}

CreatureTextRepeatIds CreatureTextMgr::GetRepeatGroup(Creature* source, uint8 textGroup)
{
    ASSERT(source);//should never happen

    return source->GetTextRepeatGroup(textGroup);
}

bool CreatureTextMgr::TextExist(uint32 sourceEntry, uint8 textGroup)
{
    if (!sourceEntry)
        return false;

    CreatureTextMap::const_iterator sList = mTextMap.find(sourceEntry);
    if (sList == mTextMap.end())
    {
        sLog.outDebug("CreatureTextMgr::TextExist: Could not find Text for Creature (entry %u) in 'creature_text' table.", sourceEntry);
        return false;
    }

    CreatureTextHolder const& textHolder = sList->second;
    CreatureTextHolder::const_iterator itr = textHolder.find(textGroup);
    if (itr == textHolder.end())
    {
        sLog.outDebug("CreatureTextMgr::TextExist: Could not find TextGroup %u for Creature (entry %u).", uint32(textGroup), sourceEntry);
        return false;
    }

    return true;
}

std::string CreatureTextMgr::GetLocalizedChatString(uint32 entry, uint8 gender, uint8 textGroup, uint32 id, int loc_idx) const
{
    CreatureTextMap::const_iterator mapitr = mTextMap.find(entry);
    if (mapitr == mTextMap.end())
        return "";

    CreatureTextHolder::const_iterator holderItr = mapitr->second.find(textGroup);
    if (holderItr == mapitr->second.end())
        return "";

    CreatureTextGroup::const_iterator groupItr = holderItr->second.begin();
    for (; groupItr != holderItr->second.end(); ++groupItr)
        if (groupItr->id == id)
            break;

    if (groupItr == holderItr->second.end())
        return "";

    std::string baseText = groupItr->text;

    LocaleCreatureTextMap::const_iterator locItr = mLocaleTextMap.find(CreatureTextId(entry, uint32(textGroup), id));
    if (locItr != mLocaleTextMap.end())
        ObjectMgr::GetLocaleString(locItr->second.Text, loc_idx, baseText);

    return baseText;
}