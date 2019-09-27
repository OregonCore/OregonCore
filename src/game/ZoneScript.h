/**
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
 *
 * @file ZoneScript.h
 */

#ifndef ZONE_SCRIPT_H_
#define ZONE_SCRIPT_H_

#include "Common.h"
#include "Creature.h"

//struct CreatureData;
class Creature;
class GameObject;

class ZoneScript
{
    public:
        explicit ZoneScript() {}

        virtual uint32 GetCreatureEntry(uint32 /*guidlow*/, const CreatureData* data)
        {
            return data->id;
        }
        virtual uint32 GetGameObjectEntry(uint32 /*guidlow*/, uint32 entry)
        {
            return entry;
        }

        virtual void OnCreatureCreate(Creature*, bool /*add*/) {}
        virtual void OnGameObjectCreate(GameObject* /*go*/, bool /*add*/) {}
        virtual void OnCreatureDeath(Creature* /*creature*/) {}
        virtual void OnPlayerDeath(Player *) {}

        /// All-purpose data storage 64 bit
        virtual uint64 GetData64(uint32 /*DataId*/)
        {
            return 0;
        }
        virtual void SetData64(uint32 /*DataId*/, uint64 /*Value*/) {}

        /// All-purpose data storage 32 bit
        virtual uint32 GetData(uint32 /*DataId*/)
        {
            return 0;
        }
        virtual void SetData(uint32 /*DataId*/, uint32 /*Value*/) {}

        // These functions are called explicitly by scripting stuff, no regular core-link for them, yet.
        virtual void ProcessEvent(GameObject* /*obj*/, uint32 /*eventId = 0*/) {}
        virtual void ProcessEvent(Unit* /*obj*/, uint32 /*eventId = 0*/) {}
};

#endif

