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

#ifndef OREGON_PACKET_BUILDER_H
#define OREGON_PACKET_BUILDER_H

#include "WorldPacket.h"

namespace Movement
{
    class MoveSpline;
    class PacketBuilder
    {
            static void WriteCommonMonsterMovePart(const MoveSpline& mov, WorldPacket& data);
        public:

            static void WriteMonsterMove(const MoveSpline& mov, WorldPacket& data);
            static void WriteCreate(const MoveSpline& mov, ByteBuffer& data);
    };
}
#endif // OREGON_PACKET_BUILDER_H
