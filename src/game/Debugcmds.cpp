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
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "Player.h"
#include "Opcodes.h"
#include "Chat.h"
#include "Log.h"
#include "Unit.h"
#include "ObjectAccessor.h"
#include "GossipDef.h"
#include "Language.h"
#include "MapManager.h"
#include "BattlegroundMgr.h"
#include <fstream>
#include "ObjectMgr.h"
#include "InstanceData.h"
#include "M2Stores.h"

bool ChatHandler::HandleDebugInArcCommand(const char* /*args*/)
{
    Object* obj = getSelectedUnit();

    if (!obj)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        return true;
    }

    SendSysMessage(LANG_NOT_IMPLEMENTED);

    return true;
}

bool ChatHandler::HandleDebugSpellFailCommand(const char* args)
{
    if (!args)
        return false;

    char* px = strtok((char*)args, " ");
    if (!px)
        return false;

    uint8 failnum = (uint8)atoi(px);

    WorldPacket data(SMSG_CAST_FAILED, 5);
    data << uint32(133);
    data << uint8(failnum);
    m_session->SendPacket(&data);

    return true;
}

bool ChatHandler::HandleSetPoiCommand(const char* args)
{
    Player* pPlayer = m_session->GetPlayer();
    Unit* target = getSelectedUnit();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        return true;
    }

    if (!args)
        return false;

    char* icon_text = strtok((char*)args, " ");
    char* flags_text = strtok(NULL, " ");
    if (!icon_text || !flags_text)
        return false;

    uint32 icon = atol(icon_text);
    uint32 flags = atol(flags_text);

    sLog.outDetail("Command : POI, NPC = %u, icon = %u flags = %u", target->GetGUIDLow(), icon, flags);
    pPlayer->PlayerTalkClass->SendPointOfInterest(target->GetPositionX(), target->GetPositionY(), Poi_Icon(icon), flags, 30, "Test POI");
    return true;
}

bool ChatHandler::HandleEquipErrorCommand(const char* args)
{
    if (!args)
        return false;

    uint8 msg = atoi(args);
    m_session->GetPlayer()->SendEquipError(msg, 0, 0);
    return true;
}

bool ChatHandler::HandleSellErrorCommand(const char* args)
{
    if (!args)
        return false;

    uint8 msg = atoi(args);
    m_session->GetPlayer()->SendSellError(msg, 0, 0, 0);
    return true;
}

bool ChatHandler::HandleBuyErrorCommand(const char* args)
{
    if (!args)
        return false;

    uint8 msg = atoi(args);
    m_session->GetPlayer()->SendBuyError(msg, 0, 0, 0);
    return true;
}

bool ChatHandler::HandleSendOpcodeCommand(const char* /*args*/)
{
    Unit* unit = getSelectedUnit();
    Player* player = NULL;
    if (!unit || (unit->GetTypeId() != TYPEID_PLAYER))
        player = m_session->GetPlayer();
    else
        player = unit->ToPlayer();
    if (!unit) unit = player;

    std::ifstream ifs("opcode.txt");
    if (ifs.bad())
        return false;

    uint32 opcode;
    ifs >> opcode;

    WorldPacket data(opcode, 0);

    while (!ifs.eof())
    {
        std::string type;
        ifs >> type;

        if (type == "")
            break;

        if (type == "uint8")
        {
            uint16 val1;
            ifs >> val1;
            data << uint8(val1);
        }
        else if (type == "uint16")
        {
            uint16 val2;
            ifs >> val2;
            data << val2;
        }
        else if (type == "uint32")
        {
            uint32 val3;
            ifs >> val3;
            data << val3;
        }
        else if (type == "uint64")
        {
            uint64 val4;
            ifs >> val4;
            data << val4;
        }
        else if (type == "float")
        {
            float val5;
            ifs >> val5;
            data << val5;
        }
        else if (type == "string")
        {
            std::string val6;
            ifs >> val6;
            data << val6;
        }
        else if (type == "pguid")
            data << unit->GetPackGUID();
        else if (type == "myguid")
            data << player->GetPackGUID();
        else if (type == "pos")
        {
            data << unit->GetPositionX();
            data << unit->GetPositionY();
            data << unit->GetPositionZ();
        }
        else if (type == "mypos")
        {
            data << player->GetPositionX();
            data << player->GetPositionY();
            data << player->GetPositionZ();
        }
        else
        {
            sLog.outDebug("Sending opcode: unknown type '%s'", type.c_str());
            break;
        }
    }
    ifs.close();
    sLog.outDebug("Sending opcode %u", data.GetOpcode());
    if (sLog.IsOutDebug())
        data.hexlike();
    unit->ToPlayer()->GetSession()->SendPacket(&data);
    PSendSysMessage(LANG_COMMAND_OPCODESENT, data.GetOpcode(), unit->GetName());
    return true;
}

bool ChatHandler::HandleUpdateWorldStateCommand(const char* args)
{
    char* w = strtok((char*)args, " ");
    char* s = strtok(NULL, " ");

    if (!w || !s)
        return false;

    uint32 world = (uint32)atoi(w);
    uint32 state = (uint32)atoi(s);
    m_session->GetPlayer()->SendUpdateWorldState(world, state);
    return true;
}

bool ChatHandler::HandleDebugPlayCinematicCommand(const char* args)
{
    // USAGE: .debug play cinematic #cinematicid
    // #cinematicid - ID decimal number from CinemaicSequences.dbc (1st column)
    if (!*args)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 dwId = atoi((char*)args);

    if (!sCinematicSequencesStore.LookupEntry(dwId))
    {
        PSendSysMessage(LANG_CINEMATIC_NOT_EXIST, dwId);
        SetSentErrorMessage(true);
        return false;
    }

    // Dump camera locations
    if (CinematicSequencesEntry const* cineSeq = sCinematicSequencesStore.LookupEntry(dwId))
    {
        std::unordered_map<uint32, FlyByCameraCollection>::const_iterator itr = sFlyByCameraStore.find(cineSeq->cinematicCamera);
        if (itr != sFlyByCameraStore.end())
        {
            PSendSysMessage("Waypoints for sequence %u, camera %u", dwId, cineSeq->cinematicCamera);
            uint32 count = 1;
            for (FlyByCamera cam : itr->second)
            {
                PSendSysMessage("%02u - %7ums [%f, %f, %f] Facing %f (%f degrees)", count, cam.timeStamp, cam.locations.x, cam.locations.y, cam.locations.z, cam.locations.w, cam.locations.w * (180 / M_PI));
                count++;
            }
            PSendSysMessage("%u waypoints dumped", itr->second.size());
        }
    }

    m_session->GetPlayer()->SendCinematicStart(dwId);
    return true;
}

//Play sound
bool ChatHandler::HandleDebugPlaySoundCommand(const char* args)
{
    // USAGE: .debug playsound #soundid
    // #soundid - ID decimal number from SoundEntries.dbc (1st column)
    if (!*args)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 dwSoundId = atoi((char*)args);

    if (!sSoundEntriesStore.LookupEntry(dwSoundId))
    {
        PSendSysMessage(LANG_SOUND_NOT_EXIST, dwSoundId);
        SetSentErrorMessage(true);
        return false;
    }

    Unit* unit = getSelectedUnit();
    if (!unit)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    if (m_session->GetPlayer()->GetSelection())
        unit->PlayDistanceSound(dwSoundId, m_session->GetPlayer());
    else
        unit->PlayDirectSound(dwSoundId, m_session->GetPlayer());

    PSendSysMessage(LANG_YOU_HEAR_SOUND, dwSoundId);
    return true;
}

//Send notification in channel
bool ChatHandler::HandleSendChannelNotifyCommand(const char* args)
{
    if (!args)
        return false;

    const char* name = "test";
    uint8 code = atoi(args);

    WorldPacket data(SMSG_CHANNEL_NOTIFY, (1 + 10));
    data << code;                                           // notify type
    data << name;                                           // channel name
    data << uint32(0);
    data << uint32(0);
    m_session->SendPacket(&data);
    return true;
}

//Send notification in chat
bool ChatHandler::HandleSendChatMsgCommand(const char* args)
{
    if (!args)
        return false;

    const char* msg = "testtest";
    uint8 type = atoi(args);
    WorldPacket data;
    ChatHandler::FillMessageData(&data, m_session, type, 0, "chan", m_session->GetPlayer()->GetGUID(), msg, m_session->GetPlayer());
    m_session->SendPacket(&data);
    return true;
}

bool ChatHandler::HandleSendQuestPartyMsgCommand(const char* args)
{
    uint32 msg = atol((char*)args);
    m_session->GetPlayer()->SendPushToPartyResponse(m_session->GetPlayer(), msg);
    return true;
}

bool ChatHandler::HandleGetLootRecipient(const char* /*args*/)
{
    Creature* target = getSelectedCreature();
    if (!target)
        return false;

    PSendSysMessage("loot recipient: %s", target->hasLootRecipient() ? (target->GetLootRecipient() ? target->GetLootRecipient()->GetName() : "offline") : "no loot recipient");
    return true;
}

bool ChatHandler::HandleSendQuestInvalidMsgCommand(const char* args)
{
    uint32 msg = atol((char*)args);
    m_session->GetPlayer()->SendCanTakeQuestResponse(msg);
    return true;
}

bool ChatHandler::HandleGetItemState(const char* args)
{
    if (!args)
        return false;

    std::string state_str = args;

    ItemUpdateState state = ITEM_UNCHANGED;
    bool list_queue = false, check_all = false;
    if (state_str == "unchanged") state = ITEM_UNCHANGED;
    else if (state_str == "changed") state = ITEM_CHANGED;
    else if (state_str == "new") state = ITEM_NEW;
    else if (state_str == "removed") state = ITEM_REMOVED;
    else if (state_str == "queue") list_queue = true;
    else if (state_str == "check_all") check_all = true;
    else return false;

    Player* player = getSelectedPlayer();
    if (!player) player = m_session->GetPlayer();

    if (!list_queue && !check_all)
    {
        state_str = "The player has the following " + state_str + " items: ";
        SendSysMessage(state_str.c_str());
        for (uint8 i = PLAYER_SLOT_START; i < PLAYER_SLOT_END; i++)
        {
            if (i >= BUYBACK_SLOT_START && i < BUYBACK_SLOT_END)
                continue;

            Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
            if (!item) continue;
            if (!item->IsBag())
            {
                if (item->GetState() == state)
                    PSendSysMessage("bag: 255 slot: %d guid: %d owner: %d", item->GetSlot(), item->GetGUIDLow(), GUID_LOPART(item->GetOwnerGUID()));
            }
            else
            {
                Bag* bag = (Bag*)item;
                for (uint8 j = 0; j < bag->GetBagSize(); ++j)
                {
                    Item* item = bag->GetItemByPos(j);
                    if (item && item->GetState() == state)
                        PSendSysMessage("bag: 255 slot: %d guid: %d owner: %d", item->GetSlot(), item->GetGUIDLow(), GUID_LOPART(item->GetOwnerGUID()));
                }
            }
        }
    }

    if (list_queue)
    {
        std::vector<Item* >& updateQueue = player->GetItemUpdateQueue();
        for (size_t i = 0; i < updateQueue.size(); i++)
        {
            Item* item = updateQueue[i];
            if (!item) continue;

            Bag* container = item->GetContainer();
            uint8 bag_slot = container ? container->GetSlot() : uint8(INVENTORY_SLOT_BAG_0);

            std::string st;
            switch (item->GetState())
            {
            case ITEM_UNCHANGED:
                st = "unchanged";
                break;
            case ITEM_CHANGED:
                st = "changed";
                break;
            case ITEM_NEW:
                st = "new";
                break;
            case ITEM_REMOVED:
                st = "removed";
                break;
            }

            PSendSysMessage("bag: %d slot: %d guid: %d - state: %s", bag_slot, item->GetSlot(), item->GetGUIDLow(), st.c_str());
        }
        if (updateQueue.empty())
            PSendSysMessage("updatequeue empty");
    }

    if (check_all)
    {
        bool error = false;
        std::vector<Item* >& updateQueue = player->GetItemUpdateQueue();
        for (uint8 i = PLAYER_SLOT_START; i < PLAYER_SLOT_END; i++)
        {
            if (i >= BUYBACK_SLOT_START && i < BUYBACK_SLOT_END)
                continue;

            Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
            if (!item) continue;

            if (item->GetSlot() != i)
            {
                PSendSysMessage("item at slot %d, guid %d has an incorrect slot value: %d", i, item->GetGUIDLow(), item->GetSlot());
                error = true;
                continue;
            }

            if (item->GetOwnerGUID() != player->GetGUID())
            {
                PSendSysMessage("for the item at slot %d and itemguid %d, owner's guid (%d) and player's guid (%d) don't match!", item->GetSlot(), item->GetGUIDLow(), GUID_LOPART(item->GetOwnerGUID()), player->GetGUIDLow());
                error = true;
                continue;
            }

            if (Bag* container = item->GetContainer())
            {
                PSendSysMessage("item at slot: %d guid: %d has a container (slot: %d, guid: %d) but shouldnt!", item->GetSlot(), item->GetGUIDLow(), container->GetSlot(), container->GetGUIDLow());
                error = true;
                continue;
            }

            if (item->IsInUpdateQueue())
            {
                uint16 qp = item->GetQueuePos();
                if (qp > updateQueue.size())
                {
                    PSendSysMessage("item at slot: %d guid: %d has a queuepos (%d) larger than the update queue size! ", item->GetSlot(), item->GetGUIDLow(), qp);
                    error = true;
                    continue;
                }

                if (updateQueue[qp] == NULL)
                {
                    PSendSysMessage("item at slot: %d guid: %d has a queuepos (%d) that points to NULL in the queue!", item->GetSlot(), item->GetGUIDLow(), qp);
                    error = true;
                    continue;
                }

                if (updateQueue[qp] != item)
                {
                    PSendSysMessage("item at slot: %d guid: %d has has a queuepos (%d) that points to another item in the queue (bag: %d, slot: %d, guid: %d)", item->GetSlot(), item->GetGUIDLow(), qp, updateQueue[qp]->GetBagSlot(), updateQueue[qp]->GetSlot(), updateQueue[qp]->GetGUIDLow());
                    error = true;
                    continue;
                }
            }
            else if (item->GetState() != ITEM_UNCHANGED)
            {
                PSendSysMessage("item at slot: %d guid: %d is not in queue but should be (state: %d)!", item->GetSlot(), item->GetGUIDLow(), item->GetState());
                error = true;
                continue;
            }

            if (item->IsBag())
            {
                Bag* bag = (Bag*)item;
                for (uint8 j = 0; j < bag->GetBagSize(); ++j)
                {
                    Item* item = bag->GetItemByPos(j);
                    if (!item) continue;

                    if (item->GetSlot() != j)
                    {
                        PSendSysMessage("the item in bag %d slot %d, guid %d has an incorrect slot value: %d", bag->GetSlot(), j, item->GetGUIDLow(), item->GetSlot());
                        error = true;
                        continue;
                    }

                    if (item->GetOwnerGUID() != player->GetGUID())
                    {
                        PSendSysMessage("for the item in bag %d at slot %d and itemguid %d, owner's guid (%d) and player's guid (%d) don't match!", bag->GetSlot(), item->GetSlot(), item->GetGUIDLow(), GUID_LOPART(item->GetOwnerGUID()), player->GetGUIDLow());
                        error = true;
                        continue;
                    }

                    Bag* container = item->GetContainer();
                    if (!container)
                    {
                        PSendSysMessage("the item in bag %d at slot %d with guid %d has no container!", bag->GetSlot(), item->GetSlot(), item->GetGUIDLow());
                        error = true;
                        continue;
                    }

                    if (container != bag)
                    {
                        PSendSysMessage("the item in bag %d at slot %d with guid %d has a different container(slot %d guid %d)!", bag->GetSlot(), item->GetSlot(), item->GetGUIDLow(), container->GetSlot(), container->GetGUIDLow());
                        error = true;
                        continue;
                    }

                    if (item->IsInUpdateQueue())
                    {
                        uint16 qp = item->GetQueuePos();
                        if (qp > updateQueue.size())
                        {
                            PSendSysMessage("item in bag: %d at slot: %d guid: %d has a queuepos (%d) larger than the update queue size! ", bag->GetSlot(), item->GetSlot(), item->GetGUIDLow(), qp);
                            error = true;
                            continue;
                        }

                        if (updateQueue[qp] == NULL)
                        {
                            PSendSysMessage("item in bag: %d at slot: %d guid: %d has a queuepos (%d) that points to NULL in the queue!", bag->GetSlot(), item->GetSlot(), item->GetGUIDLow(), qp);
                            error = true;
                            continue;
                        }

                        if (updateQueue[qp] != item)
                        {
                            PSendSysMessage("item in bag: %d at slot: %d guid: %d has has a queuepos (%d) that points to another item in the queue (bag: %d, slot: %d, guid: %d)", bag->GetSlot(), item->GetSlot(), item->GetGUIDLow(), qp, updateQueue[qp]->GetBagSlot(), updateQueue[qp]->GetSlot(), updateQueue[qp]->GetGUIDLow());
                            error = true;
                            continue;
                        }
                    }
                    else if (item->GetState() != ITEM_UNCHANGED)
                    {
                        PSendSysMessage("item in bag: %d at slot: %d guid: %d is not in queue but should be (state: %d)!", bag->GetSlot(), item->GetSlot(), item->GetGUIDLow(), item->GetState());
                        error = true;
                        continue;
                    }
                }
            }
        }

        for (size_t i = 0; i < updateQueue.size(); i++)
        {
            Item* item = updateQueue[i];
            if (!item) continue;

            if (item->GetOwnerGUID() != player->GetGUID())
            {
                PSendSysMessage("queue(%lu): for the an item (guid %d), the owner's guid (%d) and player's guid (%d) don't match!", i, item->GetGUIDLow(), GUID_LOPART(item->GetOwnerGUID()), player->GetGUIDLow());
                error = true;
                continue;
            }

            if (item->GetQueuePos() != i)
            {
                PSendSysMessage("queue(%lu): for the an item (guid %d), the queuepos doesn't match it's position in the queue!", i, item->GetGUIDLow());
                error = true;
                continue;
            }

            if (item->GetState() == ITEM_REMOVED) continue;
            Item* test = player->GetItemByPos(item->GetBagSlot(), item->GetSlot());

            if (test == NULL)
            {
                PSendSysMessage("queue(%lu): the bag(%d) and slot(%d) values for the item with guid %d are incorrect, the player doesn't have an item at that position!", i, item->GetBagSlot(), item->GetSlot(), item->GetGUIDLow());
                error = true;
                continue;
            }

            if (test != item)
            {
                PSendSysMessage("queue(%lu): the bag(%d) and slot(%d) values for the item with guid %d are incorrect, the item with guid %d is there instead!", i, item->GetBagSlot(), item->GetSlot(), item->GetGUIDLow(), test->GetGUIDLow());
                error = true;
                continue;
            }
        }
        if (!error)
            SendSysMessage("All OK!");
    }

    return true;
}

bool ChatHandler::HandleDebugBattlegroundCommand(const char* /*args*/)
{
    sBattlegroundMgr.ToggleTesting();
    return true;
}

bool ChatHandler::HandleDebugArenaCommand(const char* /*args*/)
{
    sBattlegroundMgr.ToggleArenaTesting();
    return true;
}

bool ChatHandler::HandleDebugThreatList(const char* /*args*/)
{
    Creature* target = getSelectedCreature();
    if (!target || target->IsTotem() || target->IsPet())
        return false;

    ThreatContainer::StorageType tlist = target->getThreatManager().getThreatList();
    ThreatContainer::StorageType::iterator itr;
    uint32 cnt = 0;
    PSendSysMessage("Threat list of %s (guid %u)", target->GetName(), target->GetGUIDLow());
    for (itr = tlist.begin(); itr != tlist.end(); ++itr)
    {
        Unit* unit = (*itr)->getTarget();
        if (!unit)
            continue;
        ++cnt;
        PSendSysMessage("   %u.   %s   (guid %u)  - threat %f", cnt, unit->GetName(), unit->GetGUIDLow(), (*itr)->getThreat());
    }
    SendSysMessage("End of threat list.");
    return true;
}

bool ChatHandler::HandleDebugHostilRefList(const char* /*args*/)
{
    Unit* target = getSelectedUnit();
    if (!target)
        target = m_session->GetPlayer();
    HostileReference* ref = target->getHostileRefManager().getFirst();
    uint32 cnt = 0;
    PSendSysMessage("Hostil reference list of %s (guid %u)", target->GetName(), target->GetGUIDLow());
    while (ref)
    {
        if (Unit* unit = ref->GetSource()->getOwner())
        {
            ++cnt;
            PSendSysMessage("   %u.   %s   (guid %u)  - threat %f", cnt, unit->GetName(), unit->GetGUIDLow(), ref->getThreat());
        }
        ref = ref->next();
    }
    SendSysMessage("End of hostil reference list.");
    return true;
}

bool ChatHandler::HandleSetInstanceDataCommand(const char* args)
{
    if (!args || !m_session->GetPlayer())
        return false;

    InstanceData* pInstance = m_session->GetPlayer()->GetInstanceData();
    if (!pInstance)
    {
        PSendSysMessage("You are not in scripted instance.");
        SetSentErrorMessage(true);
        return false;
    }

    char* id = strtok((char*)args, " ");
    char* data = strtok(NULL, " ");

    if (!id || !data)
        return false;

    uint32 _id = uint32(atoi(id));
    uint32 _data = uint32(atoi(data));

    pInstance->SetData(_id, _data);
    return true;
}

bool ChatHandler::HandleGetInstanceDataCommand(const char* args)
{
    if (!args || !m_session->GetPlayer())
        return false;

    InstanceData* pInstance = m_session->GetPlayer()->GetInstanceData();
    if (!pInstance)
    {
        PSendSysMessage("You are not in scripted instance.");
        SetSentErrorMessage(true);
        return false;
    }

    char* id = strtok((char*)args, " ");

    if (!id)
        return false;

    uint32 _id = uint32(atoi(id));

    PSendSysMessage("Result: %u", pInstance->GetData(_id));
    return true;
}

bool ChatHandler::HandleSpellCrashTestCommand(const char* /*args*/)
{
    /* Casts ALL spells. Useful for testing/founding out if/what spell causes server to crash */

    Unit* player = m_session->GetPlayer();

    for (uint32 i = 31000; i <= 53085; ++i)
    {
        if (SpellEntry const* spellInfo = sSpellStore.LookupEntry(i))
        {
            sLog.outDebugInLine("Testing spell %u ... ", i);
            Creature* trigger = player->SummonTrigger(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), 600, NULL);
            trigger->CastSpell(trigger, spellInfo, true);
            trigger->DisappearAndDie(false);
            sLog.outDebug("OK!");
        }
    }

    PSendSysMessage("ALL OK!");
    return true;
}

bool ChatHandler::HandlePartyResultCommand(const char* args)
{
    if (!m_session)
        return false;

    int operation = atoi(args);
    int result = atoi(std::max<const char*>(strchr(args, ' ')+1, ""));

    m_session->SendPartyResult(PartyOperation(operation), "", PartyResult(result));
    return true;
}

bool ChatHandler::HandleDebugAnimationCommand(const char* args)
{
    if (!m_session || !m_session->GetPlayer())
        return false;

    WorldPacket data(SMSG_GAMEOBJECT_CUSTOM_ANIM, 12);
    data << (m_session->GetPlayer()->GetSelection() ? m_session->GetPlayer()->GetSelection() : m_session->GetPlayer()->GetGUID());
    data << uint32(atoi(args));
    m_session->SendPacket(&data);

    return true;
}

