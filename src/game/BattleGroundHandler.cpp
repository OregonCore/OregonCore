/*
 * Copyright (C) 2010-2014 OregonCore <http://www.oregoncore.com/>
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
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
#include "BattleGroundMgr.h"
#include "BattleGroundWS.h"
#include "BattleGround.h"
#include "ArenaTeam.h"
#include "Language.h"
#include "World.h"

void WorldSession::HandleBattleGroundHelloOpcode(WorldPacket & recv_data)
{
    uint64 guid;
    recv_data >> guid;
    sLog.outDebug("WORLD: Recvd CMSG_BATTLEMASTER_HELLO Message from (GUID: %u TypeId:%u)", GUID_LOPART(guid),GuidHigh2TypeId(GUID_HIPART(guid)));

    Creature *unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->isBattleMaster())                             // it's not battlemaster
        return;

    // Stop the npc if moving
    if(!unit->IsStopped())
        unit->StopMoving();

    uint32 bgTypeId = objmgr.GetBattleMasterBG(unit->GetEntry());

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
    sBattleGroundMgr.BuildBattleGroundListPacket(&data, guid, _player, bgTypeId);
    SendPacket(&data);
}

void WorldSession::HandleBattleGroundJoinOpcode(WorldPacket & recv_data)
{
    uint64 guid;
    uint32 bgTypeId;
    uint32 instanceId;
    uint8 joinAsGroup;
    Group * grp;

    recv_data >> guid;                                      // battlemaster guid
    recv_data >> bgTypeId;                                  // battleground type id (DBC id)
    recv_data >> instanceId;                                // instance id, 0 if First Available selected
    recv_data >> joinAsGroup;                               // join as group

    if (bgTypeId >= MAX_BATTLEGROUND_TYPES)
    {
        sLog.outError("Battleground: invalid bgtype received. possible cheater? player guid %u",_player->GetGUIDLow());
        return;
    }

    sLog.outDebug("WORLD: Recvd CMSG_BATTLEMASTER_JOIN Message from (GUID: %u TypeId:%u)", GUID_LOPART(guid),GuidHigh2TypeId(GUID_HIPART(guid)));

    // can do this, since it's battleground, not arena
    uint32 bgQueueTypeId = sBattleGroundMgr.BGQueueTypeId(bgTypeId, 0);

    // ignore if player is already in BG
    if (_player->InBattleGround())
        return;

    Creature *unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->isBattleMaster())                             // it's not battlemaster
        return;

    // get bg instance or bg template if instance not found
    BattleGround *bg = NULL;
    if (instanceId)
        bg = sBattleGroundMgr.GetBattleGround(instanceId);

    if (!bg && !(bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId)))
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
        if (_player->GetBattleGroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES)
            //player is already in this queue
            return;
        // check if has free queue slots
        if (!_player->HasFreeBattleGroundQueueId())
            return;
    }
    else
    {
        grp = _player->GetGroup();
        // no group found, error
        if (!grp)
            return;
        uint32 err = grp->CanJoinBattleGroundQueue(bgTypeId, bgQueueTypeId, 0, bg->GetMaxPlayersPerTeam(), false, 0);
        if (err != BG_JOIN_ERR_OK)
        {
            SendBattleGroundOrArenaJoinError(err);
            return;
        }
    }
    // if we're here, then the conditions to join a bg are met. We can proceed in joining.

    // _player->GetGroup() was already checked, grp is already initialized
    if (joinAsGroup /* && _player->GetGroup()*/)
    {
        sLog.outDebug("Battleground: the following players are joining as group:");
        GroupQueueInfo * ginfo = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].AddGroup(_player, bgTypeId, 0, false, 0);
        for (GroupReference *itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player *member = itr->getSource();
            if (!member) continue;   // this should never happen

            uint32 queueSlot = member->AddBattleGroundQueueId(bgQueueTypeId);           // add to queue

            // store entry point coords (same as leader entry point)
            if (!sWorld.getConfig(CONFIG_BATTLEGROUND_WRATH_LEAVE_MODE))
                member->SetBattleGroundEntryPoint(_player->GetMapId(),_player->GetPositionX(),_player->GetPositionY(),_player->GetPositionZ(),_player->GetOrientation());

            WorldPacket data;
                                                            // send status packet (in queue)
            sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, member->GetTeam(), queueSlot, STATUS_WAIT_QUEUE, 0, 0);
            member->GetSession()->SendPacket(&data);
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(&data, bgTypeId);
            member->GetSession()->SendPacket(&data);
            sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].AddPlayer(member, ginfo);
            sLog.outDebug("Battleground: player joined queue for bg queue type %u bg type %u: GUID %u, NAME %s",bgQueueTypeId,bgTypeId,member->GetGUIDLow(), member->GetName());
        }
        sLog.outDebug("Battleground: group end");
        sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].Update(bgTypeId, _player->GetBattleGroundQueueIdFromLevel());
    }
    else
    {
        // already checked if queueSlot is valid, now just get it
        uint32 queueSlot = _player->AddBattleGroundQueueId(bgQueueTypeId);
        // store entry point coords
        if (!sWorld.getConfig(CONFIG_BATTLEGROUND_WRATH_LEAVE_MODE))
            _player->SetBattleGroundEntryPoint(_player->GetMapId(),_player->GetPositionX(),_player->GetPositionY(),_player->GetPositionZ(),_player->GetOrientation());

        WorldPacket data;
                                                            // send status packet (in queue)
        sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, _player->GetTeam(), queueSlot, STATUS_WAIT_QUEUE, 0, 0);
        SendPacket(&data);

        GroupQueueInfo * ginfo = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].AddGroup(_player, bgTypeId, 0, false, 0);
        sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].AddPlayer(_player, ginfo);
        sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].Update(bgTypeId, _player->GetBattleGroundQueueIdFromLevel());
        sLog.outDebug("Battleground: player joined queue for bg queue type %u bg type %u: GUID %u, NAME %s",bgQueueTypeId,bgTypeId,_player->GetGUIDLow(), _player->GetName());
    }
}

void WorldSession::HandleBattleGroundPlayerPositionsOpcode(WorldPacket & /*recv_data*/)
{
                                                            // empty opcode
    sLog.outDebug("WORLD: Recvd MSG_BATTLEGROUND_PLAYER_POSITIONS Message");

    BattleGround *bg = _player->GetBattleGround();
    if (!bg)                                                 // can't be received if player not in battleground
        return;

    switch(bg->GetTypeID())
    {
        case BATTLEGROUND_WS:
            {
                uint32 count1 = 0;                                  //always constant zero?
                uint32 count2 = 0;                                  //count of next fields

                Player *ali_plr = objmgr.GetPlayer(((BattleGroundWS*)bg)->GetAllianceFlagPickerGUID());
                if (ali_plr)
                    ++count2;

                Player *horde_plr = objmgr.GetPlayer(((BattleGroundWS*)bg)->GetHordeFlagPickerGUID());
                if (horde_plr)
                    ++count2;

                WorldPacket data(MSG_BATTLEGROUND_PLAYER_POSITIONS, (4+4+16*count1+16*count2));
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
                WorldPacket data(MSG_BATTLEGROUND_PLAYER_POSITIONS, (4+4));
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

void WorldSession::HandleBattleGroundPVPlogdataOpcode(WorldPacket & /*recv_data*/)
{
    sLog.outDebug("WORLD: Recvd MSG_PVP_LOG_DATA Message");

    BattleGround *bg = _player->GetBattleGround();
    if (!bg)
        return;

    WorldPacket data;
    sBattleGroundMgr.BuildPvpLogDataPacket(&data, bg);
    SendPacket(&data);

    DEBUG_LOG("WORLD: Sent MSG_PVP_LOG_DATA Message");
}

void WorldSession::HandleBattleGroundListOpcode(WorldPacket &recv_data)
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
    sBattleGroundMgr.BuildBattleGroundListPacket(&data, _player->GetGUID(), _player, bgTypeId);
    SendPacket(&data);
}

void WorldSession::HandleBattleGroundPlayerPortOpcode(WorldPacket &recv_data)
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
        if (_player->InBattleGroundQueue())
        {
            // update all queues, send invitation info if player is invited, queue info if queued
            for (uint32 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; i++)
            {
                uint32 queue_id = _player->GetBattleGroundQueueId(i);
                if (!queue_id)
                    continue;
                BattleGroundQueue::QueuedPlayersMap::iterator itrPlayerStatus = sBattleGroundMgr.m_BattleGroundQueues[queue_id].m_QueuedPlayers[_player->GetBattleGroundQueueIdFromLevel()].find(_player->GetGUID());
                // if the player is not in queue, contine
                if (itrPlayerStatus == sBattleGroundMgr.m_BattleGroundQueues[queue_id].m_QueuedPlayers[_player->GetBattleGroundQueueIdFromLevel()].end())
                    continue;

                // no group information, this should never happen
                if (!itrPlayerStatus->second.GroupInfo)
                    continue;

                BattleGround * bg = NULL;

                // get possibly needed data from groupinfo
                bgTypeId = itrPlayerStatus->second.GroupInfo->BgTypeId;
                uint8 arenatype = itrPlayerStatus->second.GroupInfo->ArenaType;
                uint8 israted = itrPlayerStatus->second.GroupInfo->IsRated;
                uint8 status = 0;


                if (!itrPlayerStatus->second.GroupInfo->IsInvitedToBGInstanceGUID)
                {
                    // not invited to bg, get template
                    bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
                    status = STATUS_WAIT_QUEUE;
                }
                else
                {
                    // get the bg we're invited to
                    bg = sBattleGroundMgr.GetBattleGround(itrPlayerStatus->second.GroupInfo->IsInvitedToBGInstanceGUID);
                    status = STATUS_WAIT_JOIN;
                }

                // if bg not found, then continue
                if (!bg)
                    continue;

                // don't invite if already in the instance
                if (_player->InBattleGround() && _player->GetBattleGround() && _player->GetBattleGround()->GetInstanceID() == bg->GetInstanceID())
                    continue;

                // re - invite player with proper data
                WorldPacket data;
                sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, itrPlayerStatus->second.GroupInfo->Team?itrPlayerStatus->second.GroupInfo->Team:_player->GetTeam(), i, status, INVITE_ACCEPT_WAIT_TIME, 0, arenatype, israted);
                SendPacket(&data);
            }
        }
        return;
    }

    uint32 bgQueueTypeId = 0;
    // get the bg what we were invited to
    BattleGroundQueue::QueuedPlayersMap::iterator itrPlayerStatus;
    bgQueueTypeId = sBattleGroundMgr.BGQueueTypeId(bgTypeId,type);
    itrPlayerStatus = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].m_QueuedPlayers[_player->GetBattleGroundQueueIdFromLevel()].find(_player->GetGUID());

    if (itrPlayerStatus == sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].m_QueuedPlayers[_player->GetBattleGroundQueueIdFromLevel()].end())
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

    BattleGround *bg = sBattleGroundMgr.GetBattleGround(instanceId);

    // bg template might and must be used in case of leaving queue, when instance is not created yet
    if (!bg && action == 0)
        bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);

    if (!bg)
    {
        sLog.outError("Battleground: bg not found.");
        return;
    }

    bgTypeId = bg->GetTypeID();

    if (_player->InBattleGroundQueue())
    {
        uint32 queueSlot = 0;
        uint32 team = 0;
        uint32 arenatype = 0;
        uint32 israted = 0;
        uint32 rating = 0;
        uint32 opponentsRating = 0;
        // get the team info from the queue
        BattleGroundQueue::QueuedPlayersMap::iterator pitr = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].m_QueuedPlayers[_player->GetBattleGroundQueueIdFromLevel()].find(_player->GetGUID());
        if (pitr != sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].m_QueuedPlayers[_player->GetBattleGroundQueueIdFromLevel()].end()
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
        switch(action)
        {
            case 1:                                     // port to battleground
                if (!_player->IsInvitedForBattleGroundQueueType(bgQueueTypeId))
                    return;                                     // cheating?

                if (sWorld.getConfig(CONFIG_BATTLEGROUND_WRATH_LEAVE_MODE))
                    _player->SetBattleGroundEntryPoint();

                // resurrect the player
                if (!_player->isAlive())
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
                queueSlot = _player->GetBattleGroundQueueIndex(bgQueueTypeId);
                sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, _player->GetTeam(), queueSlot, STATUS_IN_PROGRESS, 0, bg->GetStartTime());
                _player->GetSession()->SendPacket(&data);
                // remove battleground queue status from BGmgr
                sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].RemovePlayer(_player->GetGUID(), false);
                // this is still needed here if battleground "jumping" shouldn't add deserter debuff
                // also this required to prevent stuck at old battleground after SetBattleGroundId set to new
                if (BattleGround *currentBg = _player->GetBattleGround())
                    currentBg->RemovePlayerAtLeave(_player->GetGUID(), false, true);

                // set the destination instance id
                _player->SetBattleGroundId(bg->GetInstanceID());
                // set the destination team
                _player->SetBGTeam(team);
                // bg->HandleBeforeTeleportToBattleGround(_player);
                sBattleGroundMgr.SendToBattleGround(_player, instanceId);
                // add only in HandleMoveWorldPortAck()
                // bg->AddPlayer(_player,team);
                sLog.outDebug("Battleground: player %s (%u) joined battle for bg %u, bgtype %u, queue type %u.",_player->GetName(),_player->GetGUIDLow(),bg->GetInstanceID(),bg->GetTypeID(),bgQueueTypeId);
                break;
            case 0:                                         // leave queue
                queueSlot = _player->GetBattleGroundQueueIndex(bgQueueTypeId);
                /*
                if player leaves rated arena match before match start, it is counted as he played but he lost
                */
                if (israted)
                {
                    ArenaTeam * at = objmgr.GetArenaTeamById(team);
                    if (at)
                    {
                        sLog.outDebug("UPDATING memberLost's personal arena rating for %u by opponents rating: %u, because he has left queue!", GUID_LOPART(_player->GetGUID()), opponentsRating);
                        at->MemberLost(_player, opponentsRating);
                        at->SaveToDB();
                    }
                }
                _player->RemoveBattleGroundQueueId(bgQueueTypeId); // must be called this way, because if you move this call to queue->removeplayer, it causes bugs
                sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, _player->GetTeam(), queueSlot, STATUS_NONE, 0, 0);
                sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].RemovePlayer(_player->GetGUID(), true);
                // player left queue, we should update it, maybe now his group fits in
                sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].Update(bgTypeId,_player->GetBattleGroundQueueIdFromLevel(),arenatype,israted,rating);
                SendPacket(&data);
                sLog.outDebug("Battleground: player %s (%u) left queue for bgtype %u, queue type %u.",_player->GetName(),_player->GetGUIDLow(),bg->GetTypeID(),bgQueueTypeId);
                break;
            default:
                sLog.outError("Battleground port: unknown action %u", action);
                break;
        }
    }
}

void WorldSession::HandleBattleGroundLeaveOpcode(WorldPacket & recv_data)
{
    sLog.outDebug("WORLD: Recvd CMSG_LEAVE_BATTLEFIELD Message");

    recv_data.read_skip<uint8>();                           // unk1
    recv_data.read_skip<uint8>();                           // unk2
    recv_data.read_skip<uint32>();                          // BattleGroundTypeId
    recv_data.read_skip<uint16>();                          // unk3

    //if (bgTypeId >= MAX_BATTLEGROUND_TYPES)                  // cheating? but not important in this case
    //    return;

    // not allow leave battleground in combat
    if (_player->isInCombat())
        if (BattleGround* bg = _player->GetBattleGround())
            if (bg->GetStatus() != STATUS_WAIT_LEAVE)
                return;

    _player->LeaveBattleground();
}

void WorldSession::HandleBattlefieldStatusOpcode(WorldPacket & /*recv_data*/)
{
    // empty opcode
    sLog.outDebug("WORLD: Battleground status");

    WorldPacket data;

    // TODO: we must put player back to battleground in case disconnect (< 5 minutes offline time) or teleport player on login(!) from battleground map to entry point
    if (_player->InBattleGround())
    {
        BattleGround *bg = _player->GetBattleGround();
        if (bg)
        {
            uint32 bgQueueTypeId = sBattleGroundMgr.BGQueueTypeId(bg->GetTypeID(), bg->GetArenaType());
            uint32 queueSlot = _player->GetBattleGroundQueueIndex(bgQueueTypeId);
            if ((bg->GetStatus() <= STATUS_IN_PROGRESS))
            {
                sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, _player->GetTeam(), queueSlot, STATUS_IN_PROGRESS, 0, bg->GetStartTime());
                SendPacket(&data);
            }
            for (uint32 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; i++)
            {
                uint32 queue_id = _player->GetBattleGroundQueueId(i);       // battlegroundqueueid stores the type id, not the instance id, so this is definitely wrong
                uint8 arenatype = sBattleGroundMgr.BGArenaType(queue_id);
                uint8 isRated = 0;
                if (i == queueSlot || !queue_id)                            // we need to get the instance ids
                    continue;
                BattleGroundQueue::QueuedPlayersMap::iterator itrPlayerStatus = sBattleGroundMgr.m_BattleGroundQueues[queue_id].m_QueuedPlayers[_player->GetBattleGroundQueueIdFromLevel()].find(_player->GetGUID());
                if (itrPlayerStatus == sBattleGroundMgr.m_BattleGroundQueues[queue_id].m_QueuedPlayers[_player->GetBattleGroundQueueIdFromLevel()].end())
                    continue;
                if (itrPlayerStatus->second.GroupInfo)
                {
                    arenatype = itrPlayerStatus->second.GroupInfo->ArenaType;
                    isRated = itrPlayerStatus->second.GroupInfo->IsRated;
                }
                BattleGround *bg2 = sBattleGroundMgr.GetBattleGroundTemplate(sBattleGroundMgr.BGTemplateId(queue_id)); //  try this
                if (bg2)
                {
                    //in this call is small bug, this call should be filled by player's waiting time in queue
                    //this call nulls all timers for client :
                    sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg2, _player->GetTeam(), i, STATUS_WAIT_QUEUE, 0, 0,arenatype,isRated);
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
            uint32 queue_id = _player->GetBattleGroundQueueId(i);
            if (!queue_id)
                continue;

            uint32 bgTypeId = sBattleGroundMgr.BGTemplateId(queue_id);
            uint8 arenatype = sBattleGroundMgr.BGArenaType(queue_id);
            uint8 isRated = 0;
            BattleGround *bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
            BattleGroundQueue::QueuedPlayersMap::iterator itrPlayerStatus = sBattleGroundMgr.m_BattleGroundQueues[queue_id].m_QueuedPlayers[_player->GetBattleGroundQueueIdFromLevel()].find(_player->GetGUID());
            if (itrPlayerStatus == sBattleGroundMgr.m_BattleGroundQueues[queue_id].m_QueuedPlayers[_player->GetBattleGroundQueueIdFromLevel()].end())
                continue;
            if (itrPlayerStatus->second.GroupInfo)
            {
                arenatype = itrPlayerStatus->second.GroupInfo->ArenaType;
                isRated = itrPlayerStatus->second.GroupInfo->IsRated;
            }
            if (bg && queue_id)
            {
                sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, _player->GetTeam(), i, STATUS_WAIT_QUEUE, 0, 0, arenatype, isRated);
                SendPacket(&data);
            }
        }
    }
/*    else              // not sure if it needed...
    {
        for (uint32 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; i++)
        {
            sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, NULL, _player->GetTeam(),i , STATUS_NONE, 0, 0);
            SendPacket(&data);
        }
    }*/
}

void WorldSession::HandleAreaSpiritHealerQueryOpcode(WorldPacket & recv_data)
{
    sLog.outDebug("WORLD: CMSG_AREA_SPIRIT_HEALER_QUERY");

    BattleGround *bg = _player->GetBattleGround();
    if (!bg)
        return;

    uint64 guid;
    recv_data >> guid;

    Creature *unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->isSpiritService())                            // it's not spirit service
        return;

    sBattleGroundMgr.SendAreaSpiritHealerQueryOpcode(_player, bg, guid);
}

void WorldSession::HandleAreaSpiritHealerQueueOpcode(WorldPacket & recv_data)
{
    sLog.outDebug("WORLD: CMSG_AREA_SPIRIT_HEALER_QUEUE");

    BattleGround *bg = _player->GetBattleGround();
    if (!bg)
        return;

    uint64 guid;
    recv_data >> guid;

    Creature *unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->isSpiritService())                            // it's not spirit service
        return;

    bg->AddPlayerToResurrectQueue(guid, _player->GetGUID());
}

void WorldSession::HandleBattleGroundArenaJoin(WorldPacket & recv_data)
{
    sLog.outDebug("WORLD: CMSG_BATTLEMASTER_JOIN_ARENA");
    //recv_data.hexlike();

    uint64 guid;                                            // arena Battlemaster guid
    uint8 arenaslot;                                        // 2v2, 3v3 or 5v5
    uint8 asGroup;                                          // asGroup
    uint8 isRated;                                          // isRated
    Group * grp = NULL;

    recv_data >> guid >> arenaslot >> asGroup >> isRated;

    // ignore if we already in BG or BG queue
    if (_player->InBattleGround())
        return;

    Creature *unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->isBattleMaster())                             // it's not battle master
        return;

    uint8 arenatype = 0;
    uint32 arenaRating = 0;

    switch(arenaslot)
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
            sLog.outError("Unknown arena slot %u at HandleBattleGroundArenaJoin()", arenaslot);
            return;
    }

    //check existance
    BattleGround* bg = NULL;
    if (!(bg = sBattleGroundMgr.GetBattleGroundTemplate(BATTLEGROUND_AA)))
    {
        sLog.outError("Battleground: template bg (all arenas) not found");
        return;
    }

    uint8 bgTypeId = bg->GetTypeID();
    uint32 bgQueueTypeId = sBattleGroundMgr.BGQueueTypeId(bgTypeId, arenatype);

    // check queueing conditions
    if (!asGroup)
    {
        // check if already in queue
        if (_player->GetBattleGroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES)
            //player is already in this queue
            return;
        // check if has free queue slots
        if (!_player->HasFreeBattleGroundQueueId())
            return;
    }
    else
    {
        grp = _player->GetGroup();
        // no group found, error
        if (!grp)
            return;
        uint32 err = grp->CanJoinBattleGroundQueue(bgTypeId, bgQueueTypeId, arenatype, arenatype, (bool)isRated, arenaslot);
        if (err != BG_JOIN_ERR_OK)
        {
            SendBattleGroundOrArenaJoinError(err);
            return;
        }
    }

    uint32 ateamId = 0;

    if (isRated)
    {
        ateamId = _player->GetArenaTeamId(arenaslot);
        // check real arenateam existence only here (if it was moved to group->CanJoin .. () then we would ahve to get it twice)
        ArenaTeam * at = objmgr.GetArenaTeamById(ateamId);
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
        for (GroupReference *itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player *member = itr->getSource();

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
        GroupQueueInfo * ginfo = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].AddGroup(_player, bgTypeId, arenatype, isRated, arenaRating, ateamId);
        sLog.outDebug("Battleground: arena join as group start");
        if (isRated)
            sLog.outDebug("Battleground: arena team id %u, leader %s queued with rating %u for type %u",_player->GetArenaTeamId(arenaslot),_player->GetName(),arenaRating,arenatype);
        for (GroupReference *itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player *member = itr->getSource();
            if (!member) continue;

            uint32 queueSlot = member->AddBattleGroundQueueId(bgQueueTypeId);// add to queue

            // store entry point coords (same as leader entry point)
            if (!sWorld.getConfig(CONFIG_BATTLEGROUND_WRATH_LEAVE_MODE))
                member->SetBattleGroundEntryPoint(_player->GetMapId(),_player->GetPositionX(),_player->GetPositionY(),_player->GetPositionZ(),_player->GetOrientation());

            WorldPacket data;
            // send status packet (in queue)
            sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, member->GetTeam(), queueSlot, STATUS_WAIT_QUEUE, 0, 0, arenatype, isRated);
            member->GetSession()->SendPacket(&data);
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(&data, bgTypeId);
            member->GetSession()->SendPacket(&data);
            sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].AddPlayer(member, ginfo);
            sLog.outDebug("Battleground: player joined queue for arena as group bg queue type %u bg type %u: GUID %u, NAME %s",bgQueueTypeId,bgTypeId,member->GetGUIDLow(), member->GetName());
        }
        sLog.outDebug("Battleground: arena join as group end");
        sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].Update(bgTypeId, _player->GetBattleGroundQueueIdFromLevel(), arenatype, isRated, arenaRating);
    }
    else
    {
        uint32 queueSlot = _player->AddBattleGroundQueueId(bgQueueTypeId);

        // store entry point coords
        if (!sWorld.getConfig(CONFIG_BATTLEGROUND_WRATH_LEAVE_MODE))
            _player->SetBattleGroundEntryPoint(_player->GetMapId(),_player->GetPositionX(),_player->GetPositionY(),_player->GetPositionZ(),_player->GetOrientation());

        WorldPacket data;
        // send status packet (in queue)
        sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, _player->GetTeam(), queueSlot, STATUS_WAIT_QUEUE, 0, 0, arenatype, isRated);
        SendPacket(&data);
        GroupQueueInfo * ginfo = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].AddGroup(_player, bgTypeId, arenatype, isRated, arenaRating);
        sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].AddPlayer(_player, ginfo);
        sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId].Update(bgTypeId, _player->GetBattleGroundQueueIdFromLevel(), arenatype, isRated, arenaRating);
        sLog.outDebug("Battleground: player joined queue for arena, skirmish, bg queue type %u bg type %u: GUID %u, NAME %s",bgQueueTypeId,bgTypeId,_player->GetGUIDLow(), _player->GetName());
    }
}

void WorldSession::HandleBattleGroundReportAFK(WorldPacket & recv_data)
{
    uint64 playerGuid;
    recv_data >> playerGuid;
    Player *reportedPlayer = objmgr.GetPlayer(playerGuid);

    if (!reportedPlayer)
    {
        sLog.outDebug("WorldSession::HandleBattleGroundReportAFK: player not found");
        return;
    }

    sLog.outDebug("WorldSession::HandleBattleGroundReportAFK: %s reported %s", _player->GetName(), reportedPlayer->GetName());

    reportedPlayer->ReportedAfkBy(_player);
}

void WorldSession::SendBattleGroundOrArenaJoinError(uint8 err)
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

