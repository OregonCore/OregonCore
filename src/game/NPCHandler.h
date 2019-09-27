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

#ifndef __NPCHANDLER_H
#define __NPCHANDLER_H

// GCC have alternative #pragma pack(N) syntax and old gcc version not support pack(push,N), also any gcc version not support it at some platform
#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

struct PageText
{
    uint32 Page_ID;
    char* Text;

    uint32 Next_Page;
};

// GCC have alternative #pragma pack() syntax and old gcc version not support pack(pop), also any gcc version not support it at some platform
#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif

struct QEmote
{
    uint32 _Emote;
    uint32 _Delay;
};

struct GossipTextOption
{
    std::string Text_0;
    std::string Text_1;
    uint32 Language;
    float Probability;
    QEmote Emotes[3];
};

struct GossipText
{
    uint32 Text_ID;
    GossipTextOption Options[8];
};

struct PageTextLocale
{
    std::vector<std::string> Text;
};

struct NpcTextLocale
{
    NpcTextLocale()
    {
        Text_0.resize(8);
        Text_1.resize(8);
    }

    std::vector<std::vector<std::string> > Text_0;
    std::vector<std::vector<std::string> > Text_1;
};
#endif

