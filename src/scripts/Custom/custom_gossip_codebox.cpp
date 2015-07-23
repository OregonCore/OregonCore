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
SDName: Custom_Gossip_Codebox
SD%Complete: 100
SDComment: Show a codebox in gossip option
SDCategory: Script Examples
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include <cstring>

//This function is called when the player opens the gossip menubool
bool GossipHello_custom_gossip_codebox(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM_EXTENDED(0, "A quiz: what's your name?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "", 0, true);
    player->ADD_GOSSIP_ITEM(0, "I'm not interested", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    player->PlayerTalkClass->SendGossipMenu(907, pCreature->GetGUID());
    return true;
}

//This function is called when the player clicks an option on the gossip menubool
bool GossipSelect_custom_gossip_codebox(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 2)
    {
        pCreature->Say("Normal select, guess you're not interested.", LANG_UNIVERSAL, 0);
        player->CLOSE_GOSSIP_MENU();
    }
    return true;
}

bool GossipSelectWithCode_custom_gossip_codebox(Player* player, Creature* pCreature, uint32 sender, uint32 action, const char* sCode)
{
    if (sender == GOSSIP_SENDER_MAIN)
    {
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            if (std::strcmp(sCode, player->GetName()) != 0)
            {
                pCreature->Say("Wrong!", LANG_UNIVERSAL, 0);
                pCreature->CastSpell(player, 12826, true);
            }
            else
            {
                pCreature->Say("You're right, you are allowed to see my inner secrets.", LANG_UNIVERSAL, 0);
                pCreature->CastSpell(player, 26990, true);
            }
            player->CLOSE_GOSSIP_MENU();
            return true;
        }
    }
    return false;
}

void AddSC_custom_gossip_codebox()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "custom_gossip_codebox";
    newscript->pGossipHello =           &GossipHello_custom_gossip_codebox;
    newscript->pGossipSelect =          &GossipSelect_custom_gossip_codebox;
    newscript->pGossipSelectWithCode =  &GossipSelectWithCode_custom_gossip_codebox;
    newscript->RegisterSelf();
}

