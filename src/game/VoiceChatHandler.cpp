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
#include "WorldSession.h"
#include "World.h"
#include "Log.h"

void WorldSession::HandleVoiceSettingsOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("WORLD: CMSG_VOICE_SETTINGS");
    // uint8 isVoiceEnabled, uint8 isMicrophoneEnabled
    recv_data.read_skip<uint8>();
    recv_data.read_skip<uint8>();
    if (sLog.IsOutDebug())
        recv_data.hexlike();
}

void WorldSession::HandleChannelEnableVoiceOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("WORLD: CMSG_CHANNEL_ENABLE_VOICE");
    // Enable Voice button in channel context menu
    if (sLog.IsOutDebug())
        recv_data.hexlike();
}

void WorldSession::HandleChannelVoiceChatQuery(WorldPacket& recv_data)
{
    sLog.outDebug("WORLD: CMSG_CHANNEL_VOICE_CHAT_QUERY");
    recv_data.read_skip<uint32>();
    recv_data.read_skip<char*>();
    if (sLog.IsOutDebug())
        recv_data.hexlike();
}

