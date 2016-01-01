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
#include "WorldPacket.h"
#include "Opcodes.h"
#include "Log.h"
#include "Player.h"
#include "ObjectMgr.h"
#include "WorldSession.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "Object.h"
#include "Chat.h"
#include "BattlegroundMgr.h"
#include "BattlegroundWS.h"
#include "Battleground.h"
#include "ArenaTeam.h"
#include "Language.h"
#include "World.h"
#include "DisableMgr.h"

void WorldSession::HandleBattlegroundHelloOpcode(WorldPacket& recv_data)
{
    uint64 guid;
    recv_data >> guid;
    sLog.outDebug("WORLD: Recvd CMSG_BATTLEMASTER_HELLO Message from (GUID: %u TypeId:%u)", GUID_LOPART(guid), GuidHigh2TypeId(GUID_HIPART(guid)));

    Creature* unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->isBattleMaster())                             // it's not battlemaster
        return;

    // Stop the npc if moving
    if (!unit->IsStopped())
        unit->StopMoving();

    uint32 bgTypeId = sObjectMgr.GetBattleMasterBG(unit->GetEntry());

    if (!_player->GetBGAccessByLevel(bgTypeId))
    {
        // temp, must be gossip message...
        SendNotification(LANG_YOUR_BG_LEVEL_REQ_ERROR);
        return;
    }

    SendBattlegGroundList(guid, bgTypeId);
}

void WorldSession::SendBattlegGroundList(uint64 guid, uint32 bgTypeId)
{
    WorldPacket data;
    sBattlegroundMgr.BuildBattlegroundListPacket(&data, guid, _player, bgTypeId);
    SendPacket(&data);
}

void WorldSession::HandleBattlegroundJoinOpcode(WorldPacket& recv_data)
{
    uint64 guid;
    uint32 bgTypeId;
    uint32 instanceId;
    uint8 joinAsGroup;
    Group* grp;

    recv_data >> guid;                                      // battlemaster guid
    recv_data >> bgTypeId;                                  // battleground type id (DBC id)
    recv_data >> instanceId;                                // instance id, 0 if First Available selected
    recv_data >> joinAsGroup;                               // join as group

    if (bgTypeId >= MAX_BATTLEGROUND_TYPES)
    {
        sLog.outError("Battleground: invalid bgtype received. possible cheater? player guid %u", _player->GetGUIDLow());
        return;
    }

    if (sDisableMgr.IsDisabledFor(DISABLE_TYPE_BATTLEGROUND, bgTypeId, NULL))
    {
        ChatHandler(this).PSendSysMessage(LANG_BG_DISABLED);
        return;
    }

    sLog.outDebug("WORLD: Recvd CMSG_BATTLEMASTER_JOIN Message from (GUID: %u TypeId:%u)", GUID_LOPART(guid), GuidHigh2TypeId(GUID_HIPART(guid)));

    // can do this, since it's battleground, not arena
    uint32 bgQueueTypeId = sBattlegroundMgr.BGQueueTypeId(bgTypeId, 0);

    // ignore if player is already in BG
    if (_player->InBattleground())
        return;

    Creature* unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->isBattleMaster())                             // it's not battlemaster
        return;

    // get bg instance or bg template if instance not found
    Battleground* bg = NULL;
    if (instanceId)
        bg = sBattlegroundMgr.GetBattleground(instanceId);

    if (!bg && !(bg = sBattlegroundMgr.GetBattlegroundTemplate(bgTypeId)))
    {
        sLog.outError("Battleground: no available bg / template found");
        return;
    }

    // check queueing conditions
    if (!joinAsGroup)
    {
        // check Deserter debuff
        if (!_player->CanJoinToBattleground())
        {
            WorldPacket data(SMSG_GROUP_JOINED_BATTLEGROUND, 4);
            data << (uint32) 0xFFFFFFFE;
            _player->GetSession()->SendPacket(&data);
            return;
        }
        // check if already in queue
        if (_player->GetBattlegroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES)
            //player is already in this queue
            return;
        // check if has free queue slots
        if (!_player->HasFreeBattlegroundQueueId())
            return;
    }
    else
    {
        grp = _player->GetGroup();
        // no group found, error
        if (!grp)
            return;
        uint32 err = grp->CanJoinBattlegroundQueue(bgTypeId, bgQueueTypeId, 0, bg->GetMaxPlayersPerTeam(), false, 0);
        if (err != BG_JOIN_ERR_OK)
        {
            SendBattlegroundOrArenaJoinError(err);
            return;
        }
    }
    // if we're here, then the conditions to join a bg are met. We can proceed in joining.

    // _player->GetGroup() was already checked, grp is already initialized
    if (joinAsGroup /* && _player->GetGroup()*/)
    {
        sLog.outDebug("Battleground: the following players are joining as group:");
        GroupQueueInfo* ginfo = sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].AddGroup(_player, bgTypeId, 0, false, 0);
        for (GroupReference* itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* member = itr->getSource();
            if (!member) continue;   // this should never happen

            uint32 queueSlot = member->AddBattlegroundQueueId(bgQueueTypeId);           // add to queue

            // store entry point coords (same as leader entry point)
            if (!sWorld.getConfig(CONFIG_BATTLEGROUND_WRATH_LEAVE_MODE))
                member->SetBattlegroundEntryPoint(_player->GetMapId(), _player->GetPositionX(), _player->GetPositionY(), _player->GetPositionZ(), _player->GetOrientation());

            WorldPacket data;
            // send status packet (in queue)
            sBattlegroundMgr.BuildBattlegroundStatusPacket(&data, bg, member->GetTeam(), queueSlot, STATUS_WAIT_QUEUE, 0, 0);
            member->GetSession()->SendPacket(&data);
            sBattlegroundMgr.BuildGroupJoinedBattlegroundPacket(&data, bgTypeId);
            member->GetSession()->SendPacket(&data);
            sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].AddPlayer(member, ginfo);
            sLog.outDebug("Battleground: player joined queue for bg queue type %u bg type %u: GUID %u, NAME %s", bgQueueTypeId, bgTypeId, member->GetGUIDLow(), member->GetName());
        }
        sLog.outDebug("Battleground: group end");
        sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].Update(bgTypeId, _player->GetBattlegroundQueueIdFromLevel());
    }
    else
    {
        // already checked if queueSlot is valid, now just get it
        uint32 queueSlot = _player->AddBattlegroundQueueId(bgQueueTypeId);
        // store entry point coords
        if (!sWorld.getConfig(CONFIG_BATTLEGROUND_WRATH_LEAVE_MODE))
            _player->SetBattlegroundEntryPoint(_player->GetMapId(), _player->GetPositionX(), _player->GetPositionY(), _player->GetPositionZ(), _player->GetOrientation());

        WorldPacket data;
        // send status packet (in queue)
        sBattlegroundMgr.BuildBattlegroundStatusPacket(&data, bg, _player->GetTeam(), queueSlot, STATUS_WAIT_QUEUE, 0, 0);
        SendPacket(&data);

        GroupQueueInfo* ginfo = sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].AddGroup(_player, bgTypeId, 0, false, 0);
        sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].AddPlayer(_player, ginfo);
        sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].Update(bgTypeId, _player->GetBattlegroundQueueIdFromLevel());
        sLog.outDebug("Battleground: player joined queue for bg queue type %u bg type %u: GUID %u, NAME %s", bgQueueTypeId, bgTypeId, _player->GetGUIDLow(), _player->GetName());
    }
}

void WorldSession::HandleBattlegroundPlayerPositionsOpcode(WorldPacket& /*recv_data*/)
{
    // empty opcode
    sLog.outDebug("WORLD: Recvd MSG_BATTLEGROUND_PLAYER_POSITIONS Message");

    Battleground* bg = _player->GetBattleground();
    if (!bg)                                                 // can't be received if player not in battleground
        return;

    switch (bg->GetTypeID())
    {
    case BATTLEGROUND_WS:
        {
            uint32 count1 = 0;                                  //always constant zero?
            uint32 count2 = 0;                                  //count of next fields

            Player* ali_plr = sObjectMgr.GetPlayer(((BattlegroundWS*)bg)->GetAllianceFlagPickerGUID());
            if (ali_plr)
                ++count2;

            Player* horde_plr = sObjectMgr.GetPlayer(((BattlegroundWS*)bg)->GetHordeFlagPickerGUID());
            if (horde_plr)
                ++count2;

            WorldPacket data(MSG_BATTLEGROUND_PLAYER_POSITIONS, (4 + 4 + 16 * count1 + 16 * count2));
            data << count1;                                     // alliance flag holders count - obsolete, now always 0
            /*for (uint8 i = 0; i < count1; ++i)
            {
                data << uint64(0);                              // guid
                data << (float)0;                               // x
                data << (float)0;                               // y
            }*/
            data << count2;                                     // horde flag holders count - obsolete, now count of next fields
            if (ali_plr)
            {
                data << (uint64)ali_plr->GetGUID();
                data << (float)ali_plr->GetPositionX();
                data << (float)ali_plr->GetPositionY();
            }
            if (horde_plr)
            {
                data << (uint64)horde_plr->GetGUID();
                data << (float)horde_plr->GetPositionX();
                data << (float)horde_plr->GetPositionY();
            }

            SendPacket(&data);
        }
        break;
    case BATTLEGROUND_EY:
        //TODO : fix me!
        break;
    case BATTLEGROUND_AB:
    case BATTLEGROUND_AV:
        {
            //for other BG types - send default
            WorldPacket data(MSG_BATTLEGROUND_PLAYER_POSITIONS, (4 + 4));
            data << uint32(0);
            data << uint32(0);
            SendPacket(&data);
        }
        break;
    default:
        //maybe it is sent also in arena - do nothing
        break;
    }
}

void WorldSession::HandleBattlegroundPVPlogdataOpcode(WorldPacket& /*recv_data*/)
{
    sLog.outDebug("WORLD: Recvd MSG_PVP_LOG_DATA Message");

    Battleground* bg = _player->GetBattleground();
    if (!bg)
        return;

    WorldPacket data;
    sBattlegroundMgr.BuildPvpLogDataPacket(&data, bg);
    SendPacket(&data);

    DEBUG_LOG("WORLD: Sent MSG_PVP_LOG_DATA Message");
}

void WorldSession::HandleBattlegroundListOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("WORLD: Recvd CMSG_BATTLEFIELD_LIST Message");

    uint32 bgTypeId;
    recv_data >> bgTypeId;                                  // id from DBC

    if (bgTypeId >= MAX_BATTLEGROUND_TYPES)
    {
        sLog.outError("Battleground: invalid bgtype received.");
        return;
    }

    BattlemasterListEntry const* bl = sBattlemasterListStore.LookupEntry(bgTypeId);

    if (!bl)
        return;

    WorldPacket data;
    sBattlegroundMgr.BuildBattlegroundListPacket(&data, _player->GetGUID(), _player, bgTypeId);
    SendPacket(&data);
}

void WorldSession::HandleBattlegroundPlayerPortOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("WORLD: Recvd CMSG_BATTLEFIELD_PORT Message");

    uint8 type;                                             // arenatype if arena
    uint8 unk2;                                             // unk, can be 0x0 (may be if was invited?) and 0x1
    uint32 instanceId;
    uint32 bgTypeId;                                        // type id from dbc
    uint16 unk;                                             // 0x1F90 constant?
    uint8 action;                                           // enter battle 0x1, leave queue 0x0

    recv_data >> type >> unk2 >> bgTypeId >> unk >> action;

    if (bgTypeId >= MAX_BATTLEGROUND_TYPES)
    {
        sLog.outError("Battleground: invalid bgtype received.");
        // update battleground slots for the player to fix his UI and sent data.
        // this is a HACK, I don't know why the client starts sending invalid packets in the first place.
        // it usually happens with extremely high latency (if debugging / stepping in the code for example)
        if (_player->InBattlegroundQueue())
        {
            // update all queues, send invitation info if player is invited, queue info if queued
            for (uint32 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; i++)
            {
                uint32 queue_id = _player->GetBattlegroundQueueId(i);
                if (!queue_id)
                    continue;
                BattlegroundQueue::QueuedPlayersMap::iterator itrPlayerStatus = sBattlegroundMgr.m_BattlegroundQueues[queue_id].m_QueuedPlayers[_player->GetBattlegroundQueueIdFromLevel()].find(_player->GetGUID());
                // if the player is not in queue, contine
                if (itrPlayerStatus == sBattlegroundMgr.m_BattlegroundQueues[queue_id].m_QueuedPlayers[_player->GetBattlegroundQueueIdFromLevel()].end())
                    continue;

                // no group information, this should never happen
                if (!itrPlayerStatus->second.GroupInfo)
                    continue;

                Battleground* bg = NULL;

                // get possibly needed data from groupinfo
                bgTypeId = itrPlayerStatus->second.GroupInfo->BgTypeId;
                uint8 arenatype = itrPlayerStatus->second.GroupInfo->ArenaType;
                uint8 israted = itrPlayerStatus->second.GroupInfo->IsRated;
                uint8 status = 0;


                if (!itrPlayerStatus->second.GroupInfo->IsInvitedToBGInstanceGUID)
                {
                    // not invited to bg, get template
                    bg = sBattlegroundMgr.GetBattlegroundTemplate(bgTypeId);
                    status = STATUS_WAIT_QUEUE;
                }
                else
                {
                    // get the bg we're invited to
                    bg = sBattlegroundMgr.GetBattleground(itrPlayerStatus->second.GroupInfo->IsInvitedToBGInstanceGUID);
                    status = STATUS_WAIT_JOIN;
                }

                // if bg not found, then continue
                if (!bg)
                    continue;

                // don't invite if already in the instance
                if (_player->InBattleground() && _player->GetBattleground() && _player->GetBattleground()->GetInstanceID() == bg->GetInstanceID())
                    continue;

                // re - invite player with proper data
                WorldPacket data;
                sBattlegroundMgr.BuildBattlegroundStatusPacket(&data, bg, itrPlayerStatus->second.GroupInfo->Team ? itrPlayerStatus->second.GroupInfo->Team : _player->GetTeam(), i, status, INVITE_ACCEPT_WAIT_TIME, 0, arenatype, israted);
                SendPacket(&data);
            }
        }
        return;
    }

    uint32 bgQueueTypeId = 0;
    // get the bg what we were invited to
    BattlegroundQueue::QueuedPlayersMap::iterator itrPlayerStatus;
    bgQueueTypeId = sBattlegroundMgr.BGQueueTypeId(bgTypeId, type);
    itrPlayerStatus = sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].m_QueuedPlayers[_player->GetBattlegroundQueueIdFromLevel()].find(_player->GetGUID());

    if (itrPlayerStatus == sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].m_QueuedPlayers[_player->GetBattlegroundQueueIdFromLevel()].end())
    {
        sLog.outError("Battleground: itrplayerstatus not found.");
        return;
    }
    instanceId = itrPlayerStatus->second.GroupInfo->IsInvitedToBGInstanceGUID;

    // if action == 1, then instanceId is _required_
    if (!instanceId && action == 1)
    {
        sLog.outError("Battleground: instance not found.");
        return;
    }

    Battleground* bg = sBattlegroundMgr.GetBattleground(instanceId);

    // bg template might and must be used in case of leaving queue, when instance is not created yet
    if (!bg && action == 0)
        bg = sBattlegroundMgr.GetBattlegroundTemplate(bgTypeId);

    if (!bg)
    {
        sLog.outError("Battleground: bg not found.");
        return;
    }

    bgTypeId = bg->GetTypeID();

    if (_player->InBattlegroundQueue())
    {
        uint32 queueSlot = 0;
        uint32 team = 0;
        uint32 arenatype = 0;
        uint32 israted = 0;
        uint32 rating = 0;
        uint32 opponentsRating = 0;
        // get the team info from the queue
        BattlegroundQueue::QueuedPlayersMap::iterator pitr = sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].m_QueuedPlayers[_player->GetBattlegroundQueueIdFromLevel()].find(_player->GetGUID());
        if (pitr != sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].m_QueuedPlayers[_player->GetBattlegroundQueueIdFromLevel()].end()
            && pitr->second.GroupInfo)
        {
            team = pitr->second.GroupInfo->Team;
            arenatype = pitr->second.GroupInfo->ArenaType;
            israted = pitr->second.GroupInfo->IsRated;
            rating = pitr->second.GroupInfo->ArenaTeamRating;
            opponentsRating = pitr->second.GroupInfo->OpponentsTeamRating;
        }
        else
        {
            sLog.outError("Battleground: Invalid player queue info!");
            return;
        }
        // if player is trying to enter battleground (not arena) and he has deserter debuff, we must just remove him from queue
        if (arenatype == 0 && !_player->CanJoinToBattleground())
        {
            sLog.outDebug("Battleground: player %s (%u) has a deserter debuff, do not port him to battleground!", _player->GetName(), _player->GetGUIDLow());
            action = 0;
        }
        WorldPacket data;
        switch (action)
        {
        case 1:                                     // port to battleground
            if (!_player->IsInvitedForBattlegroundQueueType(bgQueueTypeId))
                return;                                     // cheating?

            if (sWorld.getConfig(CONFIG_BATTLEGROUND_WRATH_LEAVE_MODE))
                _player->SetBattlegroundEntryPoint();

            // resurrect the player
            if (!_player->IsAlive())
            {
                _player->ResurrectPlayer(1.0f);
                _player->SpawnCorpseBones();
            }
            // stop taxi flight at port
            if (_player->isInFlight())
            {
                _player->GetMotionMaster()->MovementExpired();
                _player->CleanupAfterTaxiFlight();
            }
            queueSlot = _player->GetBattlegroundQueueIndex(bgQueueTypeId);
            sBattlegroundMgr.BuildBattlegroundStatusPacket(&data, bg, _player->GetTeam(), queueSlot, STATUS_IN_PROGRESS, 0, bg->GetStartTime());
            _player->GetSession()->SendPacket(&data);
            // remove battleground queue status from BGmgr
            sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].RemovePlayer(_player->GetGUID(), false);
            // this is still needed here if battleground "jumping" shouldn't add deserter debuff
            // also this required to prevent stuck at old battleground after SetBattlegroundId set to new
            if (Battleground* currentBg = _player->GetBattleground())
                currentBg->RemovePlayerAtLeave(_player->GetGUID(), false, true);

            // set the destination instance id
            _player->SetBattlegroundId(bg->GetInstanceID());
            // set the destination team
            _player->SetBGTeam(team);
            // bg->HandleBeforeTeleportToBattleground(_player);
            sBattlegroundMgr.SendToBattleground(_player, instanceId);
            // add only in HandleMoveWorldPortAck()
            // bg->AddPlayer(_player,team);
            sLog.outDebug("Battleground: player %s (%u) joined battle for bg %u, bgtype %u, queue type %u.", _player->GetName(), _player->GetGUIDLow(), bg->GetInstanceID(), bg->GetTypeID(), bgQueueTypeId);
            break;
        case 0:                                         // leave queue
            queueSlot = _player->GetBattlegroundQueueIndex(bgQueueTypeId);
            /*
            if player leaves rated arena match before match start, it is counted as he played but he lost
            */
            if (israted)
            {
                ArenaTeam* at = sObjectMgr.GetArenaTeamById(team);
                if (at)
                {
                    sLog.outDebug("UPDATING memberLost's personal arena rating for %u by opponents rating: %u, because he has left queue!", GUID_LOPART(_player->GetGUID()), opponentsRating);
                    at->MemberLost(_player, opponentsRating);
                    at->SaveToDB();
                }
            }
            _player->RemoveBattlegroundQueueId(bgQueueTypeId); // must be called this way, because if you move this call to queue->removeplayer, it causes bugs
            sBattlegroundMgr.BuildBattlegroundStatusPacket(&data, bg, _player->GetTeam(), queueSlot, STATUS_NONE, 0, 0);
            sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].RemovePlayer(_player->GetGUID(), true);
            // player left queue, we should update it, maybe now his group fits in
            sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].Update(bgTypeId, _player->GetBattlegroundQueueIdFromLevel(), arenatype, israted, rating);
            SendPacket(&data);
            sLog.outDebug("Battleground: player %s (%u) left queue for bgtype %u, queue type %u.", _player->GetName(), _player->GetGUIDLow(), bg->GetTypeID(), bgQueueTypeId);
            break;
        default:
            sLog.outError("Battleground port: unknown action %u", action);
            break;
        }
    }
}

void WorldSession::HandleBattlegroundLeaveOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("WORLD: Recvd CMSG_LEAVE_BATTLEFIELD Message");

    recv_data.read_skip<uint8>();                           // unk1
    recv_data.read_skip<uint8>();                           // unk2
    recv_data.read_skip<uint32>();                          // BattlegroundTypeId
    recv_data.read_skip<uint16>();                          // unk3

    //if (bgTypeId >= MAX_BATTLEGROUND_TYPES)                  // cheating? but not important in this case
    //    return;

    // not allow leave battleground in combat
    if (_player->IsInCombat())
        if (Battleground* bg = _player->GetBattleground())
            if (bg->GetStatus() != STATUS_WAIT_LEAVE)
                return;

    _player->LeaveBattleground();
}

void WorldSession::HandleBattlefieldStatusOpcode(WorldPacket& /*recv_data*/)
{
    // empty opcode
    sLog.outDebug("WORLD: Battleground status");

    WorldPacket data;

    // @todo we must put player back to battleground in case disconnect (< 5 minutes offline time) or teleport player on login(!) from battleground map to entry point
    if (_player->InBattleground())
    {
        Battleground* bg = _player->GetBattleground();
        if (bg)
        {
            uint32 bgQueueTypeId = sBattlegroundMgr.BGQueueTypeId(bg->GetTypeID(), bg->GetArenaType());
            uint32 queueSlot = _player->GetBattlegroundQueueIndex(bgQueueTypeId);
            if ((bg->GetStatus() <= STATUS_IN_PROGRESS))
            {
                sBattlegroundMgr.BuildBattlegroundStatusPacket(&data, bg, _player->GetTeam(), queueSlot, STATUS_IN_PROGRESS, 0, bg->GetStartTime());
                SendPacket(&data);
            }
            for (uint32 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; i++)
            {
                uint32 queue_id = _player->GetBattlegroundQueueId(i);       // battlegroundqueueid stores the type id, not the instance id, so this is definitely wrong
                uint8 arenatype = sBattlegroundMgr.BGArenaType(queue_id);
                uint8 isRated = 0;
                if (i == queueSlot || !queue_id)                            // we need to get the instance ids
                    continue;
                BattlegroundQueue::QueuedPlayersMap::iterator itrPlayerStatus = sBattlegroundMgr.m_BattlegroundQueues[queue_id].m_QueuedPlayers[_player->GetBattlegroundQueueIdFromLevel()].find(_player->GetGUID());
                if (itrPlayerStatus == sBattlegroundMgr.m_BattlegroundQueues[queue_id].m_QueuedPlayers[_player->GetBattlegroundQueueIdFromLevel()].end())
                    continue;
                if (itrPlayerStatus->second.GroupInfo)
                {
                    arenatype = itrPlayerStatus->second.GroupInfo->ArenaType;
                    isRated = itrPlayerStatus->second.GroupInfo->IsRated;
                }
                Battleground* bg2 = sBattlegroundMgr.GetBattlegroundTemplate(sBattlegroundMgr.BGTemplateId(queue_id)); //  try this
                if (bg2)
                {
                    //in this call is small bug, this call should be filled by player's waiting time in queue
                    //this call nulls all timers for client :
                    sBattlegroundMgr.BuildBattlegroundStatusPacket(&data, bg2, _player->GetTeam(), i, STATUS_WAIT_QUEUE, 0, 0, arenatype, isRated);
                    SendPacket(&data);
                }
            }
        }
    }
    else
    {
        // we should update all queues? .. i'm not sure if this code is correct
        for (uint32 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; i++)
        {
            uint32 queue_id = _player->GetBattlegroundQueueId(i);
            if (!queue_id)
                continue;

            uint32 bgTypeId = sBattlegroundMgr.BGTemplateId(queue_id);
            uint8 arenatype = sBattlegroundMgr.BGArenaType(queue_id);
            uint8 isRated = 0;
            Battleground* bg = sBattlegroundMgr.GetBattlegroundTemplate(bgTypeId);
            BattlegroundQueue::QueuedPlayersMap::iterator itrPlayerStatus = sBattlegroundMgr.m_BattlegroundQueues[queue_id].m_QueuedPlayers[_player->GetBattlegroundQueueIdFromLevel()].find(_player->GetGUID());
            if (itrPlayerStatus == sBattlegroundMgr.m_BattlegroundQueues[queue_id].m_QueuedPlayers[_player->GetBattlegroundQueueIdFromLevel()].end())
                continue;
            if (itrPlayerStatus->second.GroupInfo)
            {
                arenatype = itrPlayerStatus->second.GroupInfo->ArenaType;
                isRated = itrPlayerStatus->second.GroupInfo->IsRated;
            }
            if (bg && queue_id)
            {
                sBattlegroundMgr.BuildBattlegroundStatusPacket(&data, bg, _player->GetTeam(), i, STATUS_WAIT_QUEUE, 0, 0, arenatype, isRated);
                SendPacket(&data);
            }
        }
    }
    /*   else              // not sure if it needed...
        {
            for (uint32 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; i++)
            {
                sBattlegroundMgr.BuildBattlegroundStatusPacket(&data, NULL, _player->GetTeam(),i , STATUS_NONE, 0, 0);
                SendPacket(&data);
            }
        }*/
}

void WorldSession::HandleAreaSpiritHealerQueryOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("WORLD: CMSG_AREA_SPIRIT_HEALER_QUERY");

    Battleground* bg = _player->GetBattleground();
    if (!bg)
        return;

    uint64 guid;
    recv_data >> guid;

    Creature* unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->isSpiritService())                            // it's not spirit service
        return;

    sBattlegroundMgr.SendAreaSpiritHealerQueryOpcode(_player, bg, guid);
}

void WorldSession::HandleAreaSpiritHealerQueueOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("WORLD: CMSG_AREA_SPIRIT_HEALER_QUEUE");

    Battleground* bg = _player->GetBattleground();
    if (!bg)
        return;

    uint64 guid;
    recv_data >> guid;

    Creature* unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->isSpiritService())                            // it's not spirit service
        return;

    bg->AddPlayerToResurrectQueue(guid, _player->GetGUID());
}

void WorldSession::HandleBattlegroundArenaJoin(WorldPacket& recv_data)
{
    sLog.outDebug("WORLD: CMSG_BATTLEMASTER_JOIN_ARENA");
    //recv_data.hexlike();

    uint64 guid;                                            // arena Battlemaster guid
    uint8 arenaslot;                                        // 2v2, 3v3 or 5v5
    uint8 asGroup;                                          // asGroup
    uint8 isRated;                                          // isRated
    Group* grp = NULL;

    recv_data >> guid >> arenaslot >> asGroup >> isRated;

    // ignore if we already in BG or BG queue
    if (_player->InBattleground())
        return;

    Creature* unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->isBattleMaster())                             // it's not battle master
        return;

    uint8 arenatype = 0;
    uint32 arenaRating = 0;

    switch (arenaslot)
    {
    case 0:
        arenatype = ARENA_TYPE_2v2;
        break;
    case 1:
        arenatype = ARENA_TYPE_3v3;
        break;
    case 2:
        arenatype = ARENA_TYPE_5v5;
        break;
    default:
        sLog.outError("Unknown arena slot %u at HandleBattlegroundArenaJoin()", arenaslot);
        return;
    }

    //check existance
    Battleground* bg = NULL;
    if (!(bg = sBattlegroundMgr.GetBattlegroundTemplate(BATTLEGROUND_AA)))
    {
        sLog.outError("Battleground: template bg (all arenas) not found");
        return;
    }

    if (sDisableMgr.IsDisabledFor(DISABLE_TYPE_BATTLEGROUND, BATTLEGROUND_AA, NULL))
    {
        ChatHandler(this).PSendSysMessage(LANG_ARENA_DISABLED);
        return;
    }

    uint8 bgTypeId = bg->GetTypeID();
    uint32 bgQueueTypeId = sBattlegroundMgr.BGQueueTypeId(bgTypeId, arenatype);

    // check queueing conditions
    if (!asGroup)
    {
        // check if already in queue
        if (_player->GetBattlegroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES)
            //player is already in this queue
            return;
        // check if has free queue slots
        if (!_player->HasFreeBattlegroundQueueId())
            return;
    }
    else
    {
        grp = _player->GetGroup();
        // no group found, error
        if (!grp)
            return;
        uint32 err = grp->CanJoinBattlegroundQueue(bgTypeId, bgQueueTypeId, arenatype, arenatype, (bool)isRated, arenaslot);
        if (err != BG_JOIN_ERR_OK)
        {
            SendBattlegroundOrArenaJoinError(err);
            return;
        }
    }

    uint32 ateamId = 0;

    if (isRated)
    {
        ateamId = _player->GetArenaTeamId(arenaslot);
        // check real arenateam existence only here (if it was moved to group->CanJoin .. () then we would ahve to get it twice)
        ArenaTeam* at = sObjectMgr.GetArenaTeamById(ateamId);
        if (!at)
        {
            _player->GetSession()->SendNotInArenaTeamPacket(arenatype);
            return;
        }
        // get the team rating for queueing
        arenaRating = at->GetRating();
        // the arenateam id must match for everyone in the group
        // get the personal ratings for queueing
        uint32 avg_pers_rating = 0;
        for (GroupReference* itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* member = itr->getSource();

            // calc avg personal rating
            avg_pers_rating += member->GetArenaPersonalRating(arenaslot);
        }

        if (arenatype)
            avg_pers_rating /= arenatype;

        // if avg personal rating is more than 150 points below the teams rating, the team will be queued against an opponent matching or similar to the average personal rating
        if (avg_pers_rating + 150 < arenaRating)
            arenaRating = avg_pers_rating;
    }

    if (asGroup)
    {
        GroupQueueInfo* ginfo = sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].AddGroup(_player, bgTypeId, arenatype, isRated, arenaRating, ateamId);
        sLog.outDebug("Battleground: arena join as group start");
        if (isRated)
            sLog.outDebug("Battleground: arena team id %u, leader %s queued with rating %u for type %u", _player->GetArenaTeamId(arenaslot), _player->GetName(), arenaRating, arenatype);
        for (GroupReference* itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* member = itr->getSource();
            if (!member) continue;

            uint32 queueSlot = member->AddBattlegroundQueueId(bgQueueTypeId);// add to queue

            // store entry point coords (same as leader entry point)
            if (!sWorld.getConfig(CONFIG_BATTLEGROUND_WRATH_LEAVE_MODE))
                member->SetBattlegroundEntryPoint(_player->GetMapId(), _player->GetPositionX(), _player->GetPositionY(), _player->GetPositionZ(), _player->GetOrientation());

            WorldPacket data;
            // send status packet (in queue)
            sBattlegroundMgr.BuildBattlegroundStatusPacket(&data, bg, member->GetTeam(), queueSlot, STATUS_WAIT_QUEUE, 0, 0, arenatype, isRated);
            member->GetSession()->SendPacket(&data);
            sBattlegroundMgr.BuildGroupJoinedBattlegroundPacket(&data, bgTypeId);
            member->GetSession()->SendPacket(&data);
            sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].AddPlayer(member, ginfo);
            sLog.outDebug("Battleground: player joined queue for arena as group bg queue type %u bg type %u: GUID %u, NAME %s", bgQueueTypeId, bgTypeId, member->GetGUIDLow(), member->GetName());
        }
        sLog.outDebug("Battleground: arena join as group end");
        sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].Update(bgTypeId, _player->GetBattlegroundQueueIdFromLevel(), arenatype, isRated, arenaRating);
    }
    else
    {
        uint32 queueSlot = _player->AddBattlegroundQueueId(bgQueueTypeId);

        // store entry point coords
        if (!sWorld.getConfig(CONFIG_BATTLEGROUND_WRATH_LEAVE_MODE))
            _player->SetBattlegroundEntryPoint(_player->GetMapId(), _player->GetPositionX(), _player->GetPositionY(), _player->GetPositionZ(), _player->GetOrientation());

        WorldPacket data;
        // send status packet (in queue)
        sBattlegroundMgr.BuildBattlegroundStatusPacket(&data, bg, _player->GetTeam(), queueSlot, STATUS_WAIT_QUEUE, 0, 0, arenatype, isRated);
        SendPacket(&data);
        GroupQueueInfo* ginfo = sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].AddGroup(_player, bgTypeId, arenatype, isRated, arenaRating);
        sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].AddPlayer(_player, ginfo);
        sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].Update(bgTypeId, _player->GetBattlegroundQueueIdFromLevel(), arenatype, isRated, arenaRating);
        sLog.outDebug("Battleground: player joined queue for arena, skirmish, bg queue type %u bg type %u: GUID %u, NAME %s", bgQueueTypeId, bgTypeId, _player->GetGUIDLow(), _player->GetName());
    }
}

void WorldSession::HandleBattlegroundReportAFK(WorldPacket& recv_data)
{
    uint64 playerGuid;
    recv_data >> playerGuid;
    Player* reportedPlayer = sObjectMgr.GetPlayer(playerGuid);

    if (!reportedPlayer)
    {
        sLog.outDebug("WorldSession::HandleBattlegroundReportAFK: player not found");
        return;
    }

    sLog.outDebug("WorldSession::HandleBattlegroundReportAFK: %s reported %s", _player->GetName(), reportedPlayer->GetName());

    reportedPlayer->ReportedAfkBy(_player);
}

void WorldSession::SendBattlegroundOrArenaJoinError(uint8 err)
{
    WorldPacket data;
    int32 msg;
    switch (err)
    {
    case BG_JOIN_ERR_OFFLINE_MEMBER:
        msg = LANG_BG_GROUP_OFFLINE_MEMBER;
        break;
    case BG_JOIN_ERR_GROUP_TOO_MANY:
        msg = LANG_BG_GROUP_TOO_LARGE;
        break;
    case BG_JOIN_ERR_MIXED_FACTION:
        msg = LANG_BG_GROUP_MIXED_FACTION;
        break;
    case BG_JOIN_ERR_MIXED_LEVELS:
        msg = LANG_BG_GROUP_MIXED_LEVELS;
        break;
    case BG_JOIN_ERR_GROUP_MEMBER_ALREADY_IN_QUEUE:
        msg = LANG_BG_GROUP_MEMBER_ALREADY_IN_QUEUE;
        break;
    case BG_JOIN_ERR_GROUP_DESERTER:
        msg = LANG_BG_GROUP_MEMBER_DESERTER;
        break;
    case BG_JOIN_ERR_ALL_QUEUES_USED:
        msg = LANG_BG_GROUP_MEMBER_NO_FREE_QUEUE_SLOTS;
        break;
    case BG_JOIN_ERR_GROUP_NOT_ENOUGH:
    case BG_JOIN_ERR_MIXED_ARENATEAM:
    default:
        return;
        break;
    }
    ChatHandler::FillMessageData(&data, NULL, CHAT_MSG_BG_SYSTEM_NEUTRAL, LANG_UNIVERSAL, NULL, 0, GetOregonString(msg), NULL);
    SendPacket(&data);
    return;
}

