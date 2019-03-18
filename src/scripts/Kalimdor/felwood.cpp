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
SDName: Felwood
SD%Complete: 95
SDComment: Quest support: 4101, 4102
SDCategory: Felwood
EndScriptData */

/* ContentData
npcs_riverbreeze_and_silversky
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"

/*######
## npcs_riverbreeze_and_silversky
######*/

#define GOSSIP_ITEM_BEACON  "Please make me a Cenarion Beacon"

bool GossipHello_npcs_riverbreeze_and_silversky(Player* pPlayer, Creature* pCreature)
{
    uint32 eCreature = pCreature->GetEntry();

    if (pCreature->IsQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (eCreature == 9528)
    {
        if (pPlayer->GetQuestRewardStatus(4101))
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BEACON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(2848, pCreature->GetGUID());
        }
        else if (pPlayer->GetTeam() == HORDE)
            pPlayer->SEND_GOSSIP_MENU(2845, pCreature->GetGUID());
        else
            pPlayer->SEND_GOSSIP_MENU(2844, pCreature->GetGUID());
    }

    if (eCreature == 9529)
    {
        if (pPlayer->GetQuestRewardStatus(4102))
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BEACON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            pPlayer->SEND_GOSSIP_MENU(2849, pCreature->GetGUID());
        }
        else if (pPlayer->GetTeam() == ALLIANCE)
            pPlayer->SEND_GOSSIP_MENU(2843, pCreature->GetGUID());
        else
            pPlayer->SEND_GOSSIP_MENU(2842, pCreature->GetGUID());
    }

    return true;
}

bool GossipSelect_npcs_riverbreeze_and_silversky(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, 15120, false);
    }
    return true;
}

void AddSC_felwood()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npcs_riverbreeze_and_silversky";
    newscript->pGossipHello = &GossipHello_npcs_riverbreeze_and_silversky;
    newscript->pGossipSelect = &GossipSelect_npcs_riverbreeze_and_silversky;
    newscript->RegisterSelf();
}

