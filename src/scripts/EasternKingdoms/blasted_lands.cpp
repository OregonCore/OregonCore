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
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/* ScriptData
SDName: Blasted_Lands
SD%Complete: 90
SDComment: Quest support: 2784, 2801, 3628. Missing some texts for Fallen Hero. Teleporter to Rise of the Defiler missing group support.
SDCategory: Blasted Lands
EndScriptData */

/* ContentData
npc_deathly_usher
npc_fallen_hero_of_horde
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"

/*######
## npc_deathly_usher
######*/

#define GOSSIP_ITEM_USHER "I wish to to visit the Rise of the Defiler."

#define SPELL_TELEPORT_SINGLE           12885
#define SPELL_TELEPORT_SINGLE_IN_GROUP  13142
#define SPELL_TELEPORT_GROUP            27686

bool GossipHello_npc_deathly_usher(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(3628) == QUEST_STATUS_INCOMPLETE && pPlayer->HasItemCount(10757, 1))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_USHER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_deathly_usher(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, SPELL_TELEPORT_SINGLE, true);
    }

    return true;
}

/*######
## npc_fallen_hero_of_horde
######*/
#define GOSSIP_H_F1 "Why are you here?"
#define GOSSIP_H_F2 "Continue story..."

#define GOSSIP_ITEM_FALLEN "Continue..."

#define GOSSIP_ITEM_FALLEN1 "What could be worse than death?"
#define GOSSIP_ITEM_FALLEN2 "Subordinates?"
#define GOSSIP_ITEM_FALLEN3 "What are the stones of binding?"
#define GOSSIP_ITEM_FALLEN4 "You can count on me, Hero"
#define GOSSIP_ITEM_FALLEN5 "I shall"

bool GossipHello_npc_fallen_hero_of_horde(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->IsQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(2784) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_H_F1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    if (pPlayer->GetQuestStatus(2801) == QUEST_STATUS_INCOMPLETE && pPlayer->GetTeam() == HORDE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_H_F2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    if (pPlayer->GetQuestStatus(2801) == QUEST_STATUS_INCOMPLETE && pPlayer->GetTeam() == ALLIANCE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_H_F1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_fallen_hero_of_horde(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF+1:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
        pPlayer->SEND_GOSSIP_MENU(1392, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+11:
        pPlayer->SEND_GOSSIP_MENU(1411, pCreature->GetGUID());
        if (pPlayer->GetQuestStatus(2784) == QUEST_STATUS_INCOMPLETE)
            pPlayer->AreaExploredOrEventHappens(2784);
        if (pPlayer->GetTeam() == ALLIANCE)
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            pPlayer->SEND_GOSSIP_MENU(1411, pCreature->GetGUID());
        }
        break;

    case GOSSIP_ACTION_INFO_DEF+2:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 21);
        pPlayer->SEND_GOSSIP_MENU(1451, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+21:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 22);
        pPlayer->SEND_GOSSIP_MENU(1452, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+22:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 23);
        pPlayer->SEND_GOSSIP_MENU(1453, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+23:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 24);
        pPlayer->SEND_GOSSIP_MENU(1454, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+24:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 25);
        pPlayer->SEND_GOSSIP_MENU(1455, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+25:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 26);
        pPlayer->SEND_GOSSIP_MENU(1456, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+26:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->AreaExploredOrEventHappens(2801);
        break;
    }
    return true;
}

void AddSC_blasted_lands()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_deathly_usher";
    newscript->pGossipHello =  &GossipHello_npc_deathly_usher;
    newscript->pGossipSelect = &GossipSelect_npc_deathly_usher;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_fallen_hero_of_horde";
    newscript->pGossipHello =  &GossipHello_npc_fallen_hero_of_horde;
    newscript->pGossipSelect = &GossipSelect_npc_fallen_hero_of_horde;
    newscript->RegisterSelf();
}
