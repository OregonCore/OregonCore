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
SDName: Npc_Taxi
SD%Complete: 0%
SDComment: To be used for taxi NPCs that are located globally.
SDCategory: NPCs
EndScriptData
*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#define GOSSIP_SUSURRUS         "I am ready."
#define GOSSIP_NETHER_DRAKE     "I'm ready to fly! Take me up, dragon!"
#define GOSSIP_BRAZEN           "I am ready to go to Durnholde Keep."
#define GOSSIP_DABIREE1         "Fly me to Murketh and Shaadraz Gateways"
#define GOSSIP_DABIREE2         "Fly me to Shatter Point"
#define GOSSIP_WINDBELLOW1      "Fly me to The Abyssal Shelf"
#define GOSSIP_WINDBELLOW2      "Fly me to Honor Point"
#define GOSSIP_BRACK1           "Fly me to Murketh and Shaadraz Gateways"
#define GOSSIP_BRACK2           "Fly me to The Abyssal Shelf"
#define GOSSIP_BRACK3           "Fly me to Spinebreaker Post"
#define GOSSIP_IRENA            "Fly me to Skettis please"
#define GOSSIP_CLOUDBREAKER1    "Speaking of action, I've been ordered to undertake an air strike."
#define GOSSIP_CLOUDBREAKER2    "I need to intercept the Dawnblade reinforcements."
#define GOSSIP_DRAGONHAWK       "<Ride the dragonhawk to Sun's Reach>"
#define GOSSIP_VERONIA          "Fly me to Manaforge Coruu please"
#define GOSSIP_DEESAK           "Fly me to Ogri'la please"
#define GOSSIP_CRIMSONWING      "<Ride the gryphons to Survey Alcaz Island>"
#define GOSSIP_WILLIAMKEILAR1   "Take me to Northpass Tower."
#define GOSSIP_WILLIAMKEILAR2   "Take me to Eastwall Tower."
#define GOSSIP_WILLIAMKEILAR3   "Take me to Crown Guard Tower."

bool GossipHello_npc_taxi(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    switch (pCreature->GetEntry())
    {
    case 17435: // Azuremyst Isle - Susurrus
        if (pPlayer->HasItemCount(23843, 1, true))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SUSURRUS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        break;
    case 20903: // Netherstorm - Protectorate Nether Drake
        if (pPlayer->GetQuestStatus(10438) == QUEST_STATUS_INCOMPLETE && pPlayer->HasItemCount(29778, 1))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_NETHER_DRAKE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        break;
    case 18725: // Old Hillsbrad Foothills - Brazen
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BRAZEN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        break;
    case 19409: // Hellfire Peninsula - Wing Commander Dabir'ee
        //Mission: The Murketh and Shaadraz Gateways
        if (pPlayer->GetQuestStatus(10146) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_DABIREE1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

        //Shatter Point
        if (!pPlayer->GetQuestRewardStatus(10340))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_DABIREE2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
        break;
    case 20235: // Hellfire Peninsula - Gryphoneer Windbellow
        //Mission: The Abyssal Shelf || Return to the Abyssal Shelf
        if (pPlayer->GetQuestStatus(10163) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(10346) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_WINDBELLOW1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);

        //Go to the Front
        if (pPlayer->GetQuestStatus(10382) != QUEST_STATUS_NONE && !pPlayer->GetQuestRewardStatus(10382))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_WINDBELLOW2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
        break;
    case 19401: // Hellfire Peninsula - Wing Commander Brack
        //Mission: The Murketh and Shaadraz Gateways
        if (pPlayer->GetQuestStatus(10129) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BRACK1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);

        //Mission: The Abyssal Shelf || Return to the Abyssal Shelf
        if (pPlayer->GetQuestStatus(10162) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(10347) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BRACK2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);

        //Spinebreaker Post
        if (pPlayer->GetQuestStatus(10242) == QUEST_STATUS_COMPLETE && !pPlayer->GetQuestRewardStatus(10242))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BRACK3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
        break;
    case 23413: // Blade's Edge Mountains - Skyguard Handler Irena
        if (pPlayer->GetReputationRank(1031) >= REP_HONORED)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_IRENA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
        break;
    case 25059: // Isle of Quel'Danas - Ayren Cloudbreaker
        if (pPlayer->GetQuestStatus(11532) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(11533) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_CLOUDBREAKER1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);

        if (pPlayer->GetQuestStatus(11542) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(11543) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_CLOUDBREAKER2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
        break;
    case 25236: // Isle of Quel'Danas - Unrestrained Dragonhawk
        if (pPlayer->GetQuestStatus(11542) == QUEST_STATUS_COMPLETE || pPlayer->GetQuestStatus(11543) == QUEST_STATUS_COMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_DRAGONHAWK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
        break;
    case 20162: // Netherstorm - Veronia
        //Behind Enemy Lines
        if (pPlayer->GetQuestStatus(10652) && !pPlayer->GetQuestRewardStatus(10652))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_VERONIA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
        break;
    case 23415: // Terokkar Forest - Skyguard Handler Deesak
        if (pPlayer->GetReputationRank(1031) >= REP_HONORED)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_DEESAK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);
        break;
    case 23704: // Dustwallow Marsh - Cassa Crimsonwing
        if (pPlayer->GetQuestStatus(11142) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_CRIMSONWING, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 16);
        break;
    case 17209:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_WILLIAMKEILAR1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 17);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_WILLIAMKEILAR2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 18);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_WILLIAMKEILAR3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 19);
        break;
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_taxi(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    std::vector<uint32> nodes;

    switch (uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF:
        //spellId is correct, however it gives flight a somewhat funny effect //TaxiPath 506.
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, 32474, true);
        break;
    case GOSSIP_ACTION_INFO_DEF + 1:
        pPlayer->CLOSE_GOSSIP_MENU();

        nodes.resize(2);
        nodes[0] = 152;                                      //from drake
        nodes[1] = 153;                                      //end at drake
        pPlayer->ActivateTaxiPathTo(nodes);                  //TaxiPath 627 (possibly 627+628(152->153->154->155) )
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:
        if (!pPlayer->HasItemCount(25853, 1))
            pPlayer->SEND_GOSSIP_MENU(9780, pCreature->GetGUID());
        else
        {
            pPlayer->CLOSE_GOSSIP_MENU();

            nodes.resize(2);
            nodes[0] = 115;                                  //from brazen
            nodes[1] = 116;                                  //end outside durnholde
            pPlayer->ActivateTaxiPathTo(nodes);              //TaxiPath 534
        }
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, 33768, true);             //TaxiPath 585 (Gateways Murket and Shaadraz)
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, 35069, true);             //TaxiPath 612 (Taxi - Hellfire Peninsula - Expedition Point to Shatter Point)
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, 33899, true);             //TaxiPath 589 (Aerial Assault Flight (Alliance))
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, 35065, true);             //TaxiPath 607 (Taxi - Hellfire Peninsula - Shatter Point to Beach Head)
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, 33659, true);             //TaxiPath 584 (Gateways Murket and Shaadraz)
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, 33825, true);             //TaxiPath 587 (Aerial Assault Flight (Horde))
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, 34578, true);             //TaxiPath 604 (Taxi - Reaver's Fall to Spinebreaker Ridge)
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, 41278, true);             //TaxiPath 706
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, 45071, true);             //TaxiPath 779
        break;
    case GOSSIP_ACTION_INFO_DEF + 12:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, 45113, true);             //TaxiPath 784
        break;
    case GOSSIP_ACTION_INFO_DEF + 13:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, 45353, true);             //TaxiPath 788
        break;
    case GOSSIP_ACTION_INFO_DEF + 14:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, 34905, true);             //TaxiPath 606
        break;
    case GOSSIP_ACTION_INFO_DEF + 15:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, 41279, true);             //TaxiPath 705 (Taxi - Skettis to Skyguard Outpost)
        break;
    case GOSSIP_ACTION_INFO_DEF + 16:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, 42295, true);
        break;
    case GOSSIP_ACTION_INFO_DEF + 17:
        pPlayer->CLOSE_GOSSIP_MENU();

        nodes.resize(2);
        nodes[0] = 84;
        nodes[1] = 85;
        pPlayer->ActivateTaxiPathTo(nodes);
        break;
    case GOSSIP_ACTION_INFO_DEF + 18:
        pPlayer->CLOSE_GOSSIP_MENU();

        nodes.resize(2);
        nodes[0] = 84;
        nodes[1] = 86;
        pPlayer->ActivateTaxiPathTo(nodes);
        break;
    case GOSSIP_ACTION_INFO_DEF + 19:
        pPlayer->CLOSE_GOSSIP_MENU();

        nodes.resize(2);
        nodes[0] = 84;
        nodes[1] = 87;
        pPlayer->ActivateTaxiPathTo(nodes);
        break;
    }

    return true;
}

void AddSC_npc_taxi()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_taxi";
    newscript->pGossipHello = &GossipHello_npc_taxi;
    newscript->pGossipSelect = &GossipSelect_npc_taxi;
    newscript->RegisterSelf();
}
