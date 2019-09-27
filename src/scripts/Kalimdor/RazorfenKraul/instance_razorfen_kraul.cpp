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

/* ScriptData
SDName: Instance_Razorfen_Kraul
SD%Complete:
SDComment:
SDCategory: Razorfen Kraul
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "razorfen_kraul.h"

#define WARD_KEEPERS_NR 2

struct instance_razorfen_kraul : public ScriptedInstance
{
    instance_razorfen_kraul(Map* pMap) : ScriptedInstance(pMap)
    {
        Initialize();
    };

    uint64 DoorWardGUID;
    uint32 WardCheck_Timer;
    int WardKeeperAlive;

    void Initialize()
    {
        WardKeeperAlive = 2;
        WardCheck_Timer = 4000;
        DoorWardGUID = 0;
    }

    Player* GetPlayerInMap()
    {
        Map::PlayerList const& players = instance->GetPlayers();

        if (!players.isEmpty())
        {
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                if (Player* plr = itr->GetSource())
                    return plr;
            }
        }
        debug_log("OSCR: Instance Razorfen Kraul: GetPlayerInMap, but PlayerList is empty!");
        return NULL;
    }

    void OnGameObjectCreate(GameObject* pGo, bool /*apply*/)
    {
        switch (pGo->GetEntry())
        {
        case 21099:
            DoorWardGUID = pGo->GetGUID();
            break;
        }
    }

    void OnCreatureDeath(Creature* pCreature)
    {
        switch (pCreature->GetEntry())
        {
        case 4625:
            --WardKeeperAlive;
            break;
        }
    }

    void Update(uint32 diff)
    {
        if (WardKeeperAlive == 0)
        {
            if (WardCheck_Timer <= diff)
            {
                HandleGameObject(DoorWardGUID, true);
                WardKeeperAlive = 0;
                WardCheck_Timer = 4000;
            }
            else
                WardCheck_Timer -= diff;
        }
    }

    void SetData(uint32 type, uint32 data)
    {
        switch (type)
        {
        case TYPE_WARD_KEEPERS:
            if (data == NOT_STARTED)
                WardKeeperAlive = 1;
            break;
        }
    }

};

InstanceData* GetInstanceData_instance_razorfen_kraul(Map* pMap)
{
    return new instance_razorfen_kraul(pMap);
}

void AddSC_instance_razorfen_kraul()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "instance_razorfen_kraul";
    newscript->GetInstanceData = &GetInstanceData_instance_razorfen_kraul;
    newscript->RegisterSelf();
}
