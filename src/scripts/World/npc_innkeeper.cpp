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
SDName: Npc_Innkeeper
SD%Complete: 50
SDComment:
SDCategory: NPCs
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "GameEventMgr.h"
#include "ScriptedGossip.h"

#define HALLOWEEN_EVENTID       12
#define SPELL_TRICK_OR_TREATED  24755
#define SPELL_TREAT             24715

#define LOCALE_TRICK_OR_TREAT_0 "Trick or Treat!"
#define LOCALE_TRICK_OR_TREAT_2 "Des bonbons ou des blagues!"
#define LOCALE_TRICK_OR_TREAT_3 "S��es oder Saures!"
#define LOCALE_TRICK_OR_TREAT_6 "�Truco o trato!"

bool isEventActive()
{
    return isGameEventActive(HALLOWEEN_EVENTID);
}

bool GossipHello_npc_innkeeper(Player* pPlayer, Creature* pCreature)
{

    pPlayer->TalkedToCreature(pCreature->GetEntry(), pCreature->GetGUID());

    pPlayer->PrepareGossipMenu(pCreature, 0); //send innkeeper menu too

    if (isEventActive() && !pPlayer->HasAura(SPELL_TRICK_OR_TREATED, 0))
    {
        const char* localizedEntry;
        switch (pPlayer->GetSession()->GetSessionDbLocaleIndex())
        {
        case 0:
            localizedEntry = LOCALE_TRICK_OR_TREAT_0;
            break;
        case 2:
            localizedEntry = LOCALE_TRICK_OR_TREAT_2;
            break;
        case 3:
            localizedEntry = LOCALE_TRICK_OR_TREAT_3;
            break;
        case 6:
            localizedEntry = LOCALE_TRICK_OR_TREAT_6;
            break;
        default:
            localizedEntry = LOCALE_TRICK_OR_TREAT_0;
        }

        pPlayer->ADD_GOSSIP_ITEM(0, localizedEntry, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + HALLOWEEN_EVENTID);
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_innkeeper(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + HALLOWEEN_EVENTID && isEventActive() && !pPlayer->HasAura(SPELL_TRICK_OR_TREATED, 0))
    {
        pPlayer->CastSpell(pPlayer, SPELL_TRICK_OR_TREATED, true);

        if (urand(0, 1))
            pPlayer->CastSpell(pPlayer, SPELL_TREAT, true);
        else
        {
            int32 trickspell = 0;
            switch (rand() % 9)                             // note that female characters can get male costumes and vice versa
            {
            case 0:
                trickspell = 24753;                     // cannot cast, random 30sec
                break;
            case 1:
                trickspell = 24713;                     // lepper gnome costume
                break;
            case 2:
                trickspell = 24735;                     // male ghost costume
                break;
            case 3:
                trickspell = 24736;                     // female ghostcostume
                break;
            case 4:
                trickspell = 24710;                     // male ninja costume
                break;
            case 5:
                trickspell = 24711;                     // female ninja costume
                break;
            case 6:
                trickspell = 24708;                     // male pirate costume
                break;
            case 7:
                trickspell = 24709;                     // female pirate costume
                break;
            case 8:
                trickspell = 24723;                     // skeleton costume
                break;
            }
            pPlayer->CastSpell(pPlayer, trickspell, true);
        }
        pPlayer->CLOSE_GOSSIP_MENU();
        return true;
    }
    //Trininty Gossip core handling dont work...
    else if (uiAction == GOSSIP_OPTION_VENDOR)
        pPlayer->SEND_VENDORLIST(pCreature->GetGUID());
    else if (uiAction == GOSSIP_OPTION_INNKEEPER)
    {
        pPlayer->PlayerTalkClass->CloseGossip();
        pPlayer->SetBindPoint(pCreature->GetGUID());
    }

    return true;
}

void AddSC_npc_innkeeper()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "npc_innkeeper";
    newscript->pGossipHello = &GossipHello_npc_innkeeper;
    newscript->pGossipSelect = &GossipSelect_npc_innkeeper;
    newscript->RegisterSelf();
}

