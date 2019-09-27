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
#include "Log.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "World.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "GossipDef.h"
#include "QuestDef.h"
#include "ObjectAccessor.h"
#include "Group.h"
#include "Battleground.h"
#include "BattlegroundAV.h"
#include "ScriptMgr.h"
#include "CreatureAI.h"
#include "GameObjectAI.h"
#include "ConditionMgr.h"

void WorldSession::HandleQuestgiverStatusQueryOpcode(WorldPacket& recv_data)
{
    uint64 guid;
    recv_data >> guid;
    uint8 questStatus = DIALOG_STATUS_NONE;
    uint8 defstatus = DIALOG_STATUS_NONE;

    Object* questgiver = ObjectAccessor::GetObjectByTypeMask(*_player, guid, TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT);
    if (!questgiver)
    {
        sLog.outDetail("Error in CMSG_QUESTGIVER_STATUS_QUERY, called for not found questgiver (Typeid: %u GUID: %u)", GuidHigh2TypeId(GUID_HIPART(guid)), GUID_LOPART(guid));
        return;
    }

    switch (questgiver->GetTypeId())
    {
    case TYPEID_UNIT:
        {
            DEBUG_LOG("WORLD: Received CMSG_QUESTGIVER_STATUS_QUERY for npc, guid = %u", uint32(GUID_LOPART(guid)));
            Creature* cr_questgiver = questgiver->ToCreature();
            if (!cr_questgiver->IsHostileTo(_player))       // not show quest status to enemies
            {
                questStatus = sScriptMgr.NPCDialogStatus(_player, cr_questgiver);
                if (questStatus > 6)
                    questStatus = getDialogStatus(_player, cr_questgiver, defstatus);
            }
            break;
        }
    case TYPEID_GAMEOBJECT:
        {
            DEBUG_LOG("WORLD: Received CMSG_QUESTGIVER_STATUS_QUERY for GameObject guid = %u", uint32(GUID_LOPART(guid)));
            GameObject* go_questgiver = (GameObject*)questgiver;
            questStatus = sScriptMgr.GODialogStatus(_player, go_questgiver);
            if (questStatus > 6)
                questStatus = getDialogStatus(_player, go_questgiver, defstatus);
            break;
        }
    default:
        sLog.outError("QuestGiver called for unexpected type %u", questgiver->GetTypeId());
        break;
    }

    //inform client about status of quest
    _player->PlayerTalkClass->SendQuestGiverStatus(questStatus, guid);
}

void WorldSession::HandleQuestgiverHelloOpcode(WorldPacket& recv_data)
{
    uint64 guid;
    recv_data >> guid;

    sLog.outDebug ("WORLD: Received CMSG_QUESTGIVER_HELLO npc = %u", GUID_LOPART(guid));

    Creature* pCreature = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_NONE);
    if (!pCreature)
    {
        sLog.outDebug ("WORLD: HandleQuestgiverHelloOpcode - Unit (GUID: %u) not found or you can't interact with him.", GUID_LOPART(guid));
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveSpellsCausingAura(SPELL_AURA_FEIGN_DEATH);
    // Stop the npc if moving
    if (!pCreature->IsStopped())
        pCreature->StopMoving();

    if (sScriptMgr.GossipHello(_player, pCreature))
        return;

    _player->PrepareGossipMenu(pCreature, pCreature->GetCreatureTemplate()->GossipMenuId);
    _player->SendPreparedGossip(pCreature);

    pCreature->AI()->sGossipHello(_player);
}

void WorldSession::HandleQuestgiverAcceptQuestOpcode(WorldPacket& recv_data)
{
    uint64 guid;
    uint32 quest;
    recv_data >> guid >> quest;

    if (!GetPlayer()->IsAlive())
        return;

    DEBUG_LOG("WORLD: Received CMSG_QUESTGIVER_ACCEPT_QUEST npc = %u, quest = %u", uint32(GUID_LOPART(guid)), quest);

    Object* pObject = ObjectAccessor::GetObjectByTypeMask(*_player, guid, TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT | TYPEMASK_ITEM | TYPEMASK_PLAYER);

    // no or incorrect quest giver
    if (!pObject || pObject == _player || (pObject->GetTypeId() != TYPEID_PLAYER && !pObject->hasQuest(quest)) || 
        (pObject->GetTypeId() == TYPEID_PLAYER && !pObject->ToPlayer()->CanShareQuest(quest)))
    {
        _player->PlayerTalkClass->CloseGossip();
        _player->SetDivider(0);
        return;
    }

    // some kind of WPE protection
    if (!_player->CanInteractWithQuestGiver(pObject))
        return;

    Quest const* qInfo = sObjectMgr.GetQuestTemplate(quest);
    if (qInfo)
    {
        // prevent cheating
        if (!GetPlayer()->CanTakeQuest(qInfo, true))
        {
            _player->PlayerTalkClass->CloseGossip();
            _player->SetDivider(0);
            return;
        }

        if (_player->GetDivider() != 0)
        {
            Player* pPlayer = ObjectAccessor::FindPlayer(_player->GetDivider());
            if (pPlayer)
            {
                pPlayer->SendPushToPartyResponse(_player, QUEST_PARTY_MSG_ACCEPT_QUEST);
                _player->SetDivider(0);
            }
        }

        if (_player->CanAddQuest(qInfo, true))
        {
            _player->AddQuest(qInfo, pObject);

            if (qInfo->HasFlag(QUEST_FLAGS_PARTY_ACCEPT))
            {
                if (Group* pGroup = _player->GetGroup())
                {
                    for (GroupReference* itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
                    {
                        Player* pPlayer = itr->GetSource();

                        if (!pPlayer || pPlayer == _player || pPlayer->IsAtGroupRewardDistance(_player))     // not self
                            continue;

                        if (pPlayer->CanTakeQuest(qInfo, false))
                        {
                            pPlayer->SetDivider(_player->GetGUID());

                            //need confirmation that any gossip window will close
                            pPlayer->PlayerTalkClass->CloseGossip();

                            _player->SendQuestConfirmAccept(qInfo, pPlayer);
                        }
                    }
                }
            }

            _player->PlayerTalkClass->CloseGossip();

            if (_player->CanCompleteQuest(quest))
                _player->CompleteQuest(quest);

            switch (pObject->GetTypeId())
            {
            case TYPEID_UNIT:
                sScriptMgr.QuestAccept(_player, pObject->ToCreature(), qInfo);
                pObject->ToCreature()->AI()->sQuestAccept(_player, qInfo);
                break;
            case TYPEID_ITEM:
            case TYPEID_CONTAINER:
                {
                    sScriptMgr.ItemQuestAccept(_player, ((Item*)pObject), qInfo);

                    // destroy not required for quest finish quest starting item
                    bool destroyItem = true;
                    for (int i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
                    {
                        if ((qInfo->ReqItemId[i] == ((Item*)pObject)->GetEntry()) && (((Item*)pObject)->GetProto()->MaxCount > 0))
                        {
                            destroyItem = false;
                            break;
                        }
                    }

                    if (destroyItem)
                        _player->DestroyItem(((Item*)pObject)->GetBagSlot(), ((Item*)pObject)->GetSlot(), true);

                    break;
                }
            case TYPEID_GAMEOBJECT:
                sScriptMgr.GOQuestAccept(_player, ((GameObject*)pObject), qInfo);
                pObject->ToGameObject()->AI()->QuestAccept(_player, qInfo);
                break;
            }
            _player->PlayerTalkClass->CloseGossip();

            if (qInfo->GetSrcSpell() > 0)
                _player->CastSpell(_player, qInfo->GetSrcSpell(), true);

            return;
        }
    }

    _player->PlayerTalkClass->CloseGossip();
}

void WorldSession::HandleQuestgiverQuestQueryOpcode(WorldPacket& recv_data)
{
    uint64 guid;
    uint32 quest;
    recv_data >> guid >> quest;
    DEBUG_LOG("WORLD: Received CMSG_QUESTGIVER_QUERY_QUEST npc = %u, quest = %u", uint32(GUID_LOPART(guid)), quest);

    // Verify that the guid is valid and is a questgiver or involved in the requested quest
    Object* pObject = ObjectAccessor::GetObjectByTypeMask(*_player, guid, TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT | TYPEMASK_ITEM);
    if (!pObject || (!pObject->hasQuest(quest) && !pObject->hasInvolvedQuest(quest)))
    {
        _player->PlayerTalkClass->CloseGossip();
        return;
    }

    Quest const* pQuest = sObjectMgr.GetQuestTemplate(quest);
    if (pQuest)
        _player->PlayerTalkClass->SendQuestGiverQuestDetails(pQuest, pObject->GetGUID(), true);
}

void WorldSession::HandleQuestQueryOpcode(WorldPacket& recv_data)
{
    if (!_player)
        return;

    uint32 quest;
    recv_data >> quest;
    DEBUG_LOG("WORLD: Received CMSG_QUEST_QUERY quest = %u", quest);

    Quest const* pQuest = sObjectMgr.GetQuestTemplate(quest);
    if (pQuest)
        _player->PlayerTalkClass->SendQuestQueryResponse(pQuest);
}

void WorldSession::HandleQuestgiverChooseRewardOpcode(WorldPacket& recv_data)
{
    uint32 quest, reward;
    uint64 guid;
    recv_data >> guid >> quest >> reward;

    if (reward >= QUEST_REWARD_CHOICES_COUNT)
    {
        sLog.outError("Error in CMSG_QUESTGIVER_CHOOSE_REWARD: player %s (guid %d) tried to get invalid reward (%u) (probably packet hacking)", _player->GetName(), _player->GetGUIDLow(), reward);
        return;
    }

    if (!GetPlayer()->IsAlive())
        return;

    sLog.outDebug("WORLD: Received CMSG_QUESTGIVER_CHOOSE_REWARD npc = %u, quest = %u, reward = %u", uint32(GUID_LOPART(guid)), quest, reward);

    Object* pObject = ObjectAccessor::GetObjectByTypeMask(*_player, guid, TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT);
    if (!pObject)
        return;

    if (!pObject->hasInvolvedQuest(quest))
        return;

    if (Quest const *pQuest = sObjectMgr.GetQuestTemplate(quest))
    {
        if ((!_player->CanSeeStartQuest(pQuest) &&  _player->GetQuestStatus(quest) == QUEST_STATUS_NONE) ||
            (_player->GetQuestStatus(quest) != QUEST_STATUS_COMPLETE && !pQuest->IsAutoComplete()))
        {
            sLog.outError("HACK ALERT: Player %s (guid: %u) is trying to complete quest (id: %u) but he has no right to do it!",
                           _player->GetName(), _player->GetGUIDLow(), quest);
            return;
        }
        if (_player->CanRewardQuest(pQuest, reward, true))
        {
            _player->RewardQuest(pQuest, reward, pObject);

            switch (pObject->GetTypeId())
            {
                case TYPEID_UNIT:
                {
                    Creature* questgiver = pObject->ToCreature();
                    if (!(sScriptMgr.ChooseReward(_player, pObject->ToCreature(), pQuest, reward)))
                    {
                        // Send next quest
                        if (Quest const* nextquest = _player->GetNextQuest(guid, pQuest))
                            _player->PlayerTalkClass->SendQuestGiverQuestDetails(nextquest, guid, true);

                        questgiver->AI()->sQuestReward(_player, pQuest, reward);
                    }
                    break;
                }
                case TYPEID_GAMEOBJECT:
                {
                    GameObject* questGiver = pObject->ToGameObject();
                    if (!sScriptMgr.GOChooseReward(_player, ((GameObject*)pObject), pQuest, reward))
                    {
                        // Send next quest
                        if (Quest const* nextquest = _player->GetNextQuest(guid, pQuest))
                            _player->PlayerTalkClass->SendQuestGiverQuestDetails(nextquest, guid, true);

                        questGiver->AI()->QuestReward(_player, pQuest, reward);
                    }
                    break;
                }
                default:
                    break;
            }
        }
        else
            _player->PlayerTalkClass->SendQuestGiverOfferReward(pQuest, guid, true);
    }
}

void WorldSession::HandleQuestgiverRequestRewardOpcode(WorldPacket& recv_data)
{
    uint32 quest;
    uint64 guid;
    recv_data >> guid >> quest;

    if (!GetPlayer()->IsAlive())
        return;

    sLog.outDebug("WORLD: Received CMSG_QUESTGIVER_REQUEST_REWARD npc = %u, quest = %u", uint32(GUID_LOPART(guid)), quest);

    Object* pObject = ObjectAccessor::GetObjectByTypeMask(*_player, guid, TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT);
    if (!pObject || !pObject->hasInvolvedQuest(quest))
        return;

    if (_player->CanCompleteQuest(quest))
        _player->CompleteQuest(quest);

    if (_player->GetQuestStatus(quest) != QUEST_STATUS_COMPLETE)
        return;

    if (Quest const* pQuest = sObjectMgr.GetQuestTemplate(quest))
        _player->PlayerTalkClass->SendQuestGiverOfferReward(pQuest, guid, true);
}

void WorldSession::HandleQuestgiverCancel(WorldPacket& /*recv_data*/)
{
    sLog.outDebug("WORLD: Received CMSG_QUESTGIVER_CANCEL");

    _player->PlayerTalkClass->CloseGossip();
}

void WorldSession::HandleQuestLogSwapQuest(WorldPacket& recv_data)
{
    uint8 slot1, slot2;
    recv_data >> slot1 >> slot2;

    if (slot1 == slot2 || slot1 >= MAX_QUEST_LOG_SIZE || slot2 >= MAX_QUEST_LOG_SIZE)
        return;

    sLog.outDebug("WORLD: Received CMSG_QUESTLOG_SWAP_QUEST slot 1 = %u, slot 2 = %u", slot1, slot2);

    GetPlayer()->SwapQuestSlot(slot1, slot2);
}

void WorldSession::HandleQuestLogRemoveQuest(WorldPacket& recv_data)
{
    uint8 slot;
    recv_data >> slot;

    sLog.outDebug("WORLD: Received CMSG_QUESTLOG_REMOVE_QUEST slot = %u", slot);

    if (slot < MAX_QUEST_LOG_SIZE)
    {
        if (uint32 quest = _player->GetQuestSlotQuestId(slot))
        {
            if (!_player->TakeQuestSourceItem(quest, true))
                return;                                     // can't un-equip some items, reject quest cancel

            if (const Quest* pQuest = sObjectMgr.GetQuestTemplate(quest))
            {
                if (pQuest->HasSpecialFlag(QUEST_SPECIAL_FLAGS_TIMED))
                    _player->RemoveTimedQuest(quest);
            }

            _player->AbandonQuest(quest);
            _player->SetQuestStatus(quest, QUEST_STATUS_NONE);
        }

        _player->SetQuestSlot(slot, 0);
    }
}

void WorldSession::HandleQuestConfirmAccept(WorldPacket& recv_data)
{
    uint32 questId;
    recv_data >> questId;

    sLog.outDebug("WORLD: Received CMSG_QUEST_CONFIRM_ACCEPT quest = %u", questId);

    if (Quest const* quest = sObjectMgr.GetQuestTemplate(questId))
    {
        if (!quest->HasFlag(QUEST_FLAGS_PARTY_ACCEPT))
            return;

        Player* originalPlayer = ObjectAccessor::FindPlayer(_player->GetDivider());
        if (!originalPlayer)
            return;

        if (!_player->IsInSameRaidWith(originalPlayer))
            return;

        if (!_player->CanTakeQuest(quest, true))
            return;

        if (_player->CanAddQuest(quest, true))
            _player->AddQuest(quest, NULL);                // NULL, this prevent DB script from duplicate running

        _player->SetDivider(0);
    }
}

void WorldSession::HandleQuestComplete(WorldPacket& recv_data)
{
    uint32 quest;
    uint64 guid;
    recv_data >> guid >> quest;

    if (!_player->IsAlive())
        return;

    sLog.outDebug("WORLD: Received CMSG_QUESTGIVER_COMPLETE_QUEST npc = %u, quest = %u", uint32(GUID_LOPART(guid)), quest);

    if (Quest const* pQuest = sObjectMgr.GetQuestTemplate(quest))
    {
        if (!_player->CanSeeStartQuest(pQuest) && _player->GetQuestStatus(quest)==QUEST_STATUS_NONE)
        {
            sLog.outError("Possible hacking attempt: Player %s [guid: %u] tried to complete quest [entry: %u] without being in possession of the quest!",
                          _player->GetName(), _player->GetGUIDLow(), quest);
            return;
        }
        // @todo need a virtual function
        if (_player->InBattleground())
            if (Battleground* bg = _player->GetBattleground())
                if (bg->GetTypeID() == BATTLEGROUND_AV)
                    ((BattlegroundAV*)bg)->HandleQuestComplete(quest, _player);

        if (_player->GetQuestStatus(quest) != QUEST_STATUS_COMPLETE)
        {
            if (pQuest->IsRepeatable())
                _player->PlayerTalkClass->SendQuestGiverRequestItems(pQuest, guid, _player->CanCompleteRepeatableQuest(pQuest), false);
            else
                _player->PlayerTalkClass->SendQuestGiverRequestItems(pQuest, guid, _player->CanRewardQuest(pQuest, false), false);
        }
        else
            _player->PlayerTalkClass->SendQuestGiverRequestItems(pQuest, guid, _player->CanRewardQuest(pQuest, false), false);
    }
}

void WorldSession::HandleQuestAutoLaunch(WorldPacket& /*recvPacket*/)
{
    sLog.outDebug("WORLD: Received CMSG_QUESTGIVER_QUEST_AUTOLAUNCH");
}

void WorldSession::HandleQuestPushToParty(WorldPacket& recvPacket)
{
    uint32 questId;
    recvPacket >> questId;

    if (!_player->CanShareQuest(questId))
        return;

    sLog.outDebug("WORLD: Received CMSG_PUSHQUESTTOPARTY quest = %u", questId);

    if (Quest const* pQuest = sObjectMgr.GetQuestTemplate(questId))
    {
        if (Group* pGroup = _player->GetGroup())
        {
            for (GroupReference* itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* pPlayer = itr->GetSource();

                if (!pPlayer || pPlayer == _player || !pPlayer->IsInMap(_player))         // skip self
                    continue;

                //player already has quest
                if (!pPlayer->SatisfyQuestStatus(pQuest, false))
                {
                    _player->SendPushToPartyResponse(pPlayer, QUEST_PARTY_MSG_HAVE_QUEST);
                    continue;
                }

                //player already completed quest
                if (pPlayer->GetQuestStatus(questId) == QUEST_STATUS_COMPLETE)
                {
                    _player->SendPushToPartyResponse(pPlayer, QUEST_PARTY_MSG_FINISH_QUEST);
                    continue;
                }

                //player is ineligible for the quest
                if (!pPlayer->CanTakeQuest(pQuest, false))
                {
                    _player->SendPushToPartyResponse(pPlayer, QUEST_PARTY_MSG_CANT_TAKE_QUEST);
                    continue;
                }

                //player has full quest log
                if (!pPlayer->SatisfyQuestLog(false))
                {
                    _player->SendPushToPartyResponse(pPlayer, QUEST_PARTY_MSG_LOG_FULL);
                    continue;
                }

                if (pPlayer->GetDivider() != 0)
                {
                    _player->SendPushToPartyResponse(pPlayer, QUEST_PARTY_MSG_BUSY);
                    continue;
                }

                _player->SendPushToPartyResponse(pPlayer, QUEST_PARTY_MSG_SHARING_QUEST);

                // 2.4.3: can only share quests within 10 yards
               if (_player->GetDistance2d(pPlayer) > 10)
                {
                    _player->SendPushToPartyResponse(pPlayer, QUEST_PARTY_MSG_TOO_FAR);
                    continue;
                } 

                if ((pQuest->IsAutoComplete() && pQuest->IsRepeatable() && !pQuest->IsDaily()) || pQuest->IsAutoComplete())
                    pPlayer->PlayerTalkClass->SendQuestGiverRequestItems(pQuest, _player->GetGUID(), pPlayer->CanCompleteRepeatableQuest(pQuest), true);
                else
                {
                    pPlayer->PlayerTalkClass->SendQuestGiverQuestDetails(pQuest, _player->GetGUID(), true);
                    pPlayer->SetDivider(_player->GetGUID());
                }
            }
        }
    }
}

void WorldSession::HandleQuestPushResult(WorldPacket& recvPacket)
{
    uint64 guid;
    uint8 msg;
    recvPacket >> guid >> msg;

    sLog.outDebug("WORLD: Received MSG_QUEST_PUSH_RESULT");

    if (_player->GetDivider() != 0)
    {
        Player* pPlayer = ObjectAccessor::FindPlayer(_player->GetDivider());
        if (pPlayer)
        {
            WorldPacket data(MSG_QUEST_PUSH_RESULT, (8 + 1));
            data << uint64(guid);
            data << uint8(msg);                             // valid values: 0-8
            pPlayer->GetSession()->SendPacket(&data);
            _player->SetDivider(0);
        }
    }
}

uint32 WorldSession::getDialogStatus(Player* pPlayer, Object* questgiver, uint32 defstatus)
{
    uint32 result = defstatus;

    QuestRelations const* qir;
    QuestRelations const* qr;

    switch (questgiver->GetTypeId())
    {
    case TYPEID_GAMEOBJECT:
        {
            qir = &sObjectMgr.mGOQuestInvolvedRelations;
            qr  = &sObjectMgr.mGOQuestRelations;
            break;
        }
    case TYPEID_UNIT:
        {
            qir = &sObjectMgr.mCreatureQuestInvolvedRelations;
            qr  = &sObjectMgr.mCreatureQuestRelations;
            break;
        }
    default:
        //its imposible, but check ^)
        sLog.outError("Warning: GetDialogStatus called for unexpected type %u", questgiver->GetTypeId());
        return DIALOG_STATUS_NONE;
    }

    for (QuestRelations::const_iterator i = qir->lower_bound(questgiver->GetEntry()); i != qir->upper_bound(questgiver->GetEntry()); ++i)
    {
        uint32 result2 = 0;
        uint32 quest_id = i->second;
        Quest const* pQuest = sObjectMgr.GetQuestTemplate(quest_id);
        if (!pQuest) continue;

        ConditionList conditions = sConditionMgr.GetConditionsForNotGroupedEntry(CONDITION_SOURCE_TYPE_QUEST_SHOW_MARK, pQuest->GetQuestId());
        if (!sConditionMgr.IsObjectMeetToConditions(pPlayer, conditions))
            continue;

        QuestStatus status = pPlayer->GetQuestStatus(quest_id);
        if ((status == QUEST_STATUS_COMPLETE && !pPlayer->GetQuestRewardStatus(quest_id)) ||
            (pQuest->IsAutoComplete() && pPlayer->CanTakeQuest(pQuest, false)))
        {
            if (pQuest->IsAutoComplete() && pQuest->IsRepeatable())
                result2 = DIALOG_STATUS_REWARD_REP;
            else
                result2 = DIALOG_STATUS_REWARD;
        }
        else if (status == QUEST_STATUS_INCOMPLETE)
            result2 = DIALOG_STATUS_INCOMPLETE;

        if (result2 > result)
            result = result2;
    }

    for (QuestRelations::const_iterator i = qr->lower_bound(questgiver->GetEntry()); i != qr->upper_bound(questgiver->GetEntry()); ++i)
    {
        uint32 result2 = 0;
        uint32 quest_id = i->second;
        Quest const* pQuest = sObjectMgr.GetQuestTemplate(quest_id);
        if (!pQuest)
            continue;

        ConditionList conditions = sConditionMgr.GetConditionsForNotGroupedEntry(CONDITION_SOURCE_TYPE_QUEST_SHOW_MARK, pQuest->GetQuestId());
        if (!sConditionMgr.IsObjectMeetToConditions(pPlayer, conditions))
            continue;

        QuestStatus status = pPlayer->GetQuestStatus(quest_id);
        if (status == QUEST_STATUS_NONE)
        {
            if (pPlayer->CanSeeStartQuest(pQuest))
            {
                if (pPlayer->SatisfyQuestLevel(pQuest, false))
                {
                    auto questStatusItr = pPlayer->getQuestStatusMap().find(quest_id);
                    bool rewarded = (questStatusItr != pPlayer->getQuestStatusMap().end()) ? questStatusItr->second.m_rewarded : false;

                    if (pQuest->IsAutoComplete() || (pQuest->IsRepeatable() && rewarded))
                    {
                        result2 = DIALOG_STATUS_REWARD_REP;
                    }
                    else if (pPlayer->getLevel() <= pPlayer->GetQuestLevelForPlayer(pQuest) + sWorld.getConfig(CONFIG_QUEST_LOW_LEVEL_HIDE_DIFF))
                    {
                        if (pQuest->HasFlag(QUEST_FLAGS_DAILY))
                            result2 = DIALOG_STATUS_AVAILABLE_REP;
                        else
                            result2 = DIALOG_STATUS_AVAILABLE;
                    }
                    else
                        result2 = DIALOG_STATUS_CHAT;
                }
                else
                    result2 = DIALOG_STATUS_UNAVAILABLE;
            }
        }

        if (result2 > result)
            result = result2;
    }

    return result;
}

void WorldSession::HandleQuestgiverStatusQueryMultipleOpcode(WorldPacket& /*recvPacket*/)
{
    DEBUG_LOG("WORLD: Received CMSG_QUESTGIVER_STATUS_MULTIPLE_QUERY");

    uint32 count = 0;

    WorldPacket data(SMSG_QUESTGIVER_STATUS_MULTIPLE, 4);
    data << uint32(count);                                  // placeholder

    for (Player::ClientGUIDs::iterator itr = _player->m_clientGUIDs.begin(); itr != _player->m_clientGUIDs.end(); ++itr)
    {
        uint8 questStatus = DIALOG_STATUS_NONE;
        uint8 defstatus = DIALOG_STATUS_NONE;

        if (IS_CREATURE_GUID(*itr))
        {
            Creature* questgiver = GetPlayer()->GetMap()->GetCreature(*itr);
            if (!questgiver || questgiver->IsHostileTo(_player))
                continue;
            if (!questgiver->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
                continue;
            questStatus = sScriptMgr.NPCDialogStatus(_player, questgiver);
            if (questStatus > 6)
                questStatus = getDialogStatus(_player, questgiver, defstatus);

            data << uint64(questgiver->GetGUID());
            data << uint8(questStatus);
            ++count;
        }
        else if (IS_GAMEOBJECT_GUID(*itr))
        {
            GameObject* questgiver = GetPlayer()->GetMap()->GetGameObject(*itr);
            if (!questgiver)
                continue;
            if (questgiver->GetGoType() != GAMEOBJECT_TYPE_QUESTGIVER)
                continue;
            questStatus = sScriptMgr.GODialogStatus(_player, questgiver);
            if (questStatus > 6)
                questStatus = getDialogStatus(_player, questgiver, defstatus);

            data << uint64(questgiver->GetGUID());
            data << uint8(questStatus);
            ++count;
        }
    }

    data.put<uint32>(0, count);                             // write real count
    SendPacket(&data);
}

