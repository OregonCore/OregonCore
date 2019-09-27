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

#ifndef _WARDEN_MAC_H
#define _WARDEN_MAC_H

#include "Auth/SARC4.h"
#include <map>
#include "Auth/BigNumber.h"
#include "ByteBuffer.h"

class WorldSession;
class WardenBase;

class WardenMac : WardenBase
{
    public:
        WardenMac();
        ~WardenMac();

        void Init(WorldSession* pClient, BigNumber* K);
        ClientWardenModule* GetModuleForClient(WorldSession* session);
        void InitializeModule();
        void RequestHash();
        void HandleHashResult(ByteBuffer& buff);
        void RequestData();
        void HandleData(ByteBuffer& buff);
};

#endif
