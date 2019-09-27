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
SDName: Black_Temple
SD%Complete: 95
SDComment: Spirit of Olum: Player Teleporter to Seer Kanai Teleport after defeating Naj'entus and Supremus. @todo Find proper gossip.
SDCategory: Black Temple
EndScriptData */

/* ContentData
npc_spirit_of_olum
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "black_temple.h"
#include "ScriptedGossip.h"

/*###
# npc_spirit_of_olum
####*/

#define SPELL_TELEPORT      41566                           // s41566 - Teleport to Ashtongue NPC's
#define GOSSIP_OLUM1        "Teleport me to the other Ashtongue Deathsworn"

bool GossipHello_npc_spirit_of_olum(Player* pPlayer, Creature* pCreature)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData();

    if (pInstance && (pInstance->GetData(DATA_SUPREMUSEVENT) >= DONE) && (pInstance->GetData(DATA_HIGHWARLORDNAJENTUSEVENT) >= DONE))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_OLUM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_spirit_of_olum(Player* pPlayer, Creature* /*pCreature*/, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
        pPlayer->CLOSE_GOSSIP_MENU();

    pPlayer->InterruptNonMeleeSpells(false);
    pPlayer->CastSpell(pPlayer, SPELL_TELEPORT, false);
    return true;
}

void AddSC_black_temple()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_spirit_of_olum";
    newscript->pGossipHello = &GossipHello_npc_spirit_of_olum;
    newscript->pGossipSelect = &GossipSelect_npc_spirit_of_olum;
    newscript->RegisterSelf();
}

