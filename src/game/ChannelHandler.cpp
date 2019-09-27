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

#include "WorldSession.h"
#include "Channel.h"
#include "ChannelMgr.h"
#include "DBCStores.h"
#include "Log.h"
#include "ObjectMgr.h"                                      // for normalizePlayerName
#include "Player.h"

INSTANTIATE_SINGLETON_1(AllianceChannelMgr);
INSTANTIATE_SINGLETON_1(HordeChannelMgr);

void WorldSession::HandleJoinChannel(WorldPacket& recvPacket)
{
    uint32 channelId;
    uint8 unknown1, unknown2;
    std::string channelName, password;

    recvPacket >> channelId >> unknown1 >> unknown2 >> channelName >> password;

    sLog.outDebug("CMSG_JOIN_CHANNEL %s Channel: %u, unk1: %u, unk2: %u, channel: %s, password: %s",
        GetPlayerName(), channelId, unknown1, unknown2, channelName.c_str(), password.c_str());

    if (channelName.empty() || isdigit(channelName[0]))
    {
        WorldPacket data(SMSG_CHANNEL_NOTIFY, 1 + channelName.size());
        data << uint8(CHAT_INVALID_NAME_NOTICE) << channelName;
        SendPacket(&data);
        return;
    }

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* channel = cMgr->GetJoinChannel(channelName, channelId))
            channel->Join(_player->GetGUID(), password.c_str());
}

void WorldSession::HandleLeaveChannel(WorldPacket& recvPacket)
{
    uint32 channelId;
    std::string channelName;
    recvPacket >> channelId >> channelName;

    sLog.outDebug("CMSG_LEAVE_CHANNEL %s Channel: %s, channelId: %u",
        GetPlayerName(), channelName.c_str(), channelId);

    if (channelName.empty() && !channelId)
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
    {
        if (Channel* channel = cMgr->GetChannel(channelName, _player))
            channel->Leave(_player->GetGUID(), true);

        cMgr->LeftChannel(channelName);
    }
}

void WorldSession::HandleChannelList(WorldPacket& recvPacket)
{
    std::string channelName;
    recvPacket >> channelName;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, _player))
            channel->List(_player);
}

void WorldSession::HandleChannelPassword(WorldPacket& recvPacket)
{
    std::string channelName, password;
    recvPacket >> channelName >> password;

    sLog.outDebug("CMSG_CHANNEL_PASSWORD %s Channel: %s, Password: %s",
        GetPlayerName(), channelName.c_str(), password.c_str());

    if (password.length() > MAX_CHANNEL_PASS_STR)
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, _player))
            channel->Password(_player->GetGUID(), password.c_str());
}

void WorldSession::HandleChannelSetOwner(WorldPacket& recvPacket)
{
    std::string channelName, targetName;
    recvPacket >> channelName >> targetName;

    sLog.outDebug("CMSG_CHANNEL_SET_OWNER %s Channel: %s, Target: %s",
        GetPlayerName(), channelName.c_str(), targetName.c_str());

    if (!normalizePlayerName(targetName))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, _player))
            channel->SetOwner(_player->GetGUID(), targetName.c_str());
}

void WorldSession::HandleChannelOwner(WorldPacket& recvPacket)
{
    std::string channelName;
    recvPacket >> channelName;

    sLog.outDebug("CMSG_CHANNEL_OWNER %s Channel: %s",
        GetPlayerName(), channelName.c_str());

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, _player))
            channel->SendWhoOwner(_player->GetGUID());
}

void WorldSession::HandleChannelModerator(WorldPacket& recvPacket)
{
    std::string channelName, targetName;
    recvPacket >> channelName >> targetName;

    sLog.outDebug("CMSG_CHANNEL_MODERATOR %s Channel: %s, Target: %s",
        GetPlayerName(), channelName.c_str(), targetName.c_str());

    if (!normalizePlayerName(targetName))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, _player))
            channel->SetModerator(_player->GetGUID(), targetName.c_str());
}

void WorldSession::HandleChannelUnmoderator(WorldPacket& recvPacket)
{
    std::string channelName, targetName;
    recvPacket >> channelName >> targetName;

    sLog.outDebug("CMSG_CHANNEL_UNMODERATOR %s Channel: %s, Target: %s",
        GetPlayerName(), channelName.c_str(), targetName.c_str());

    if (!normalizePlayerName(targetName))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, _player))
            channel->UnsetModerator(_player->GetGUID(), targetName.c_str());
}

void WorldSession::HandleChannelMute(WorldPacket& recvPacket)
{
    std::string channelName, targetName;
    recvPacket >> channelName >> targetName;

    sLog.outDebug("CMSG_CHANNEL_MUTE %s Channel: %s, Target: %s",
        GetPlayerName(), channelName.c_str(), targetName.c_str());

    if (!normalizePlayerName(targetName))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, _player))
            channel->SetMute(_player->GetGUID(), targetName.c_str());
}

void WorldSession::HandleChannelUnmute(WorldPacket& recvPacket)
{
    std::string channelName, targetName;
    recvPacket >> channelName >> targetName;

    sLog.outDebug("CMSG_CHANNEL_UNMUTE %s Channel: %s, Target: %s",
        GetPlayerName(), channelName.c_str(), targetName.c_str());

    if (!normalizePlayerName(targetName))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, _player))
            channel->UnsetMute(_player->GetGUID(), targetName.c_str());
}

void WorldSession::HandleChannelInvite(WorldPacket& recvPacket)
{
    std::string channelName, targetName;
    recvPacket >> channelName >> targetName;

    sLog.outDebug("CMSG_CHANNEL_INVITE %s Channel: %s, Target: %s",
        GetPlayerName(), channelName.c_str(), targetName.c_str());

    if (!normalizePlayerName(targetName))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, _player))
            channel->Invite(_player->GetGUID(), targetName.c_str());
}

void WorldSession::HandleChannelKick(WorldPacket& recvPacket)
{
    std::string channelName, targetName;
    recvPacket >> channelName >> targetName;

    sLog.outDebug("CMSG_CHANNEL_KICK %s Channel: %s, Target: %s",
        GetPlayerName(), channelName.c_str(), targetName.c_str());

    if (!normalizePlayerName(targetName))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, _player))
            channel->Kick(_player->GetGUID(), targetName.c_str());
}

void WorldSession::HandleChannelBan(WorldPacket& recvPacket)
{
    std::string channelName, targetName;
    recvPacket >> channelName >> targetName;

    sLog.outDebug("CMSG_CHANNEL_BAN %s Channel: %s, Target: %s",
        GetPlayerName(), channelName.c_str(), targetName.c_str());

    if (!normalizePlayerName(targetName))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, _player))
            channel->Ban(_player->GetGUID(), targetName.c_str());
}

void WorldSession::HandleChannelUnban(WorldPacket& recvPacket)
{
    std::string channelName, targetName;
    recvPacket >> channelName >> targetName;

    sLog.outDebug("CMSG_CHANNEL_UNBAN %s Channel: %s, Target: %s",
        GetPlayerName(), channelName.c_str(), targetName.c_str());

    if (!normalizePlayerName(targetName))
        return;

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, _player))
            channel->UnBan(_player->GetGUID(), targetName.c_str());
}

void WorldSession::HandleChannelAnnounce(WorldPacket& recvPacket)
{
    std::string channelName;
    recvPacket >> channelName;

    sLog.outDebug("CMSG_CHANNEL_ANNOUNCEMENTS %s Channel: %s",
        GetPlayerName(), channelName.c_str());

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, _player))
            channel->Announce(_player->GetGUID());
}

void WorldSession::HandleChannelModerate(WorldPacket& recvPacket)
{
    std::string channelName;
    recvPacket >> channelName;

    sLog.outDebug("CMSG_CHANNEL_MODERATE %s Channel: %s",
        GetPlayerName(), channelName.c_str());

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, _player))
            channel->Moderate(_player->GetGUID());
}

void WorldSession::HandleChannelRosterQuery(WorldPacket& recvPacket)
{
    std::string channelName;
    recvPacket >> channelName;

    sLog.outDebug("CMSG_CHANNEL_ROSTER %s Channel: %s",
        GetPlayerName(), channelName.c_str());

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel* channel = cMgr->GetChannel(channelName, _player))
            channel->List(_player);
}

void WorldSession::HandleGetChannelMemberCount(WorldPacket& recvPacket)
{
    std::string channelName;
    recvPacket >> channelName;

    sLog.outDebug("CMSG_GET_CHANNEL_MEMBER_COUNT %s Channel: %s",
        GetPlayerName(), channelName.c_str());

    if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
    {
        if (Channel* channel = cMgr->GetChannel(channelName, _player))
        {
            WorldPacket data(SMSG_CHANNEL_MEMBER_COUNT, channel->GetName().size() + 1 + 1 + 4);
            data << channel->GetName();
            data << uint8(channel->GetFlags());
            data << uint32(channel->GetNumPlayers());
            SendPacket(&data);
        }
    }
}

void WorldSession::HandleSetChannelWatch(WorldPacket& recvPacket)
{
    std::string channelName;
    recvPacket >> channelName;

    sLog.outDebug("CMSG_SET_CHANNEL_WATCH %s Channel: %s",
        GetPlayerName(), channelName.c_str());

    /*if (ChannelMgr* cMgr = channelMgr(_player->GetTeam()))
        if (Channel *chn = cMgr->GetChannel(channelname, _player))
            chn->JoinNotify(_player->GetGUID());*/
}

