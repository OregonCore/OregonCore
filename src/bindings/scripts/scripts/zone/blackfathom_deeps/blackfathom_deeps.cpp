/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify

 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Blackfathom_Deeps
SD%Complete: 75%
SDComment: Support for Akumai Event/Doors.
SDCategory: Blackfathom Deeps
EndScriptData */

/* ContentData
go_fire_akumai
EndContentData */

#include "precompiled.h"
#include "def_blackfathom_deeps.h"

static float SpawnPoints[5][4] =
{
    {-868.0, -174.0, -25.87, 0}, //enc 1
    {-873.0, -164.0, -25.87, 0},

    {-769.3, -174.0, -25.87, 0}, //enc 2

    {-868.0, -154.0, -25.87, 32}, //enc 3

    {-769.3, -154.0, -25.87, 32}  //enc 4
};

void SummonCreatureWithRandomTarget(uint32 creatureId, int position, float mod, Unit* pTarget, GameObject* pGO)
{
    Creature *pSummoned = pGO->SummonCreature(creatureId, SpawnPoints[position][0], SpawnPoints[position][1] + mod, SpawnPoints[position][2], SpawnPoints[position][3], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 250000);
    if(pSummoned)
    {

        if(pTarget)
        {
            pSummoned->AddThreat(pTarget, 32.0f);
            ((Creature*)pSummoned)->AI()->AttackStart(pTarget);
        }

        //TODO: instance group random player...

    }
}

bool GOHello_go_fire_akumai(Player *player, GameObject* pGO)
{
    uint32 candlesNumber;
    
    ScriptedInstance *pInstance = (player->GetInstanceData()) ? ((ScriptedInstance*)player->GetInstanceData()) : NULL;

    if(!pInstance)
    {
        player->GetSession()->SendNotification("Instance script not initialized.");
        return true;
    }    
    candlesNumber = pInstance->GetData(DATA_CANDLES);
    candlesNumber++;

    switch(candlesNumber)
    {
        case 1:
            SummonCreatureWithRandomTarget(4977, 0, 0, player, pGO);
            SummonCreatureWithRandomTarget(4977, 0, 1, player, pGO);
            SummonCreatureWithRandomTarget(4977, 0, 2, player, pGO);
            SummonCreatureWithRandomTarget(4977, 0, 3, player, pGO);
            SummonCreatureWithRandomTarget(4977, 0, 4, player, pGO);

            SummonCreatureWithRandomTarget(4977, 1, 0, player, pGO);
            SummonCreatureWithRandomTarget(4977, 1, 1, player, pGO);
            SummonCreatureWithRandomTarget(4977, 1, 2, player, pGO);
            SummonCreatureWithRandomTarget(4977, 1, 3, player, pGO);
            SummonCreatureWithRandomTarget(4977, 1, 4, player, pGO);

            break;
        case 2:
            SummonCreatureWithRandomTarget(4823, 2, 0, player, pGO);
            SummonCreatureWithRandomTarget(4823, 2, 1, player, pGO);
            SummonCreatureWithRandomTarget(4823, 2, 2, player, pGO);
            SummonCreatureWithRandomTarget(4823, 2, 3, player, pGO);

            break;
        case 3:
            SummonCreatureWithRandomTarget(4825, 3, 0, player, pGO);
            SummonCreatureWithRandomTarget(4825, 3, 2, player, pGO);
            SummonCreatureWithRandomTarget(4825, 3, 4, player, pGO);
            break;
        case 4:
            SummonCreatureWithRandomTarget(4978, 4, 2, player, pGO);
            SummonCreatureWithRandomTarget(4978, 4, 4, player, pGO);
            break;
    }
    pInstance->SetData(DATA_CANDLES, candlesNumber);
    debug_log("Blackfathom Deeps - Instance Data: %i", (int) candlesNumber);
    return false;
}

void AddSC_blackfathom_deeps()
{
    Script *newscript;
    
    newscript = new Script;
    newscript->Name="go_fire_akumai";
    newscript->pGOHello = &GOHello_go_fire_akumai;
    newscript->RegisterSelf();

}

