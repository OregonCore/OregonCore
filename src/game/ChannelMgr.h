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

#ifndef OREGONCORE_CHANNELMGR_H
#define OREGONCORE_CHANNELMGR_H

#include "Channel.h"
#include "Policies/Singleton.h"
#include "World.h"

#include <map>
#include <string>

#define MAX_CHANNEL_PASS_STR 31

class ChannelMgr
{
    public:
        typedef std::map<std::string, Channel*> ChannelMap;
        ChannelMgr() {}
        ~ChannelMgr()
        {
            for (ChannelMap::iterator itr = channels.begin(); itr != channels.end(); ++itr)
                delete itr->second;

            channels.clear();
        }
        Channel* GetJoinChannel(const std::string& name, uint32 channel_id)
        {
            if (channels.count(name) == 0)
            {
                Channel* nchan = new Channel(name, channel_id);
                channels[name] = nchan;
            }
            return channels[name];
        }
        Channel* GetChannel(const std::string& name, Player* p)
        {
            ChannelMap::const_iterator i = channels.find(name);

            if (i == channels.end())
            {
                WorldPacket data;
                MakeNotOnPacket(&data, name);
                p->GetSession()->SendPacket(&data);
                return NULL;
            }
            else
                return i->second;
        }
        void LeftChannel(const std::string& name)
        {
            ChannelMap::const_iterator i = channels.find(name);

            if (i == channels.end())
                return;

            Channel* channel = i->second;

            if (channel->GetNumPlayers() == 0 && !channel->IsConstant())
            {
                channels.erase(name);
                delete channel;
            }
        }
    private:
        ChannelMap channels;
        void MakeNotOnPacket(WorldPacket* data, const std::string& name)
        {
            data->Initialize(SMSG_CHANNEL_NOTIFY, 1 + name.size());
            (*data) << uint8(CHAT_NOT_MEMBER_NOTICE) << name;
        }
};

class AllianceChannelMgr : public ChannelMgr {};
class HordeChannelMgr    : public ChannelMgr {};

inline ChannelMgr* channelMgr(uint32 team)
{
    if (sWorld.getConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHANNEL))
        //For Test,No Seprate Faction
        return &Oregon::Singleton<AllianceChannelMgr>::Instance();

    if (team == ALLIANCE)
        return &Oregon::Singleton<AllianceChannelMgr>::Instance();
    if (team == HORDE)
        return &Oregon::Singleton<HordeChannelMgr>::Instance();
    return NULL;
}
#endif

