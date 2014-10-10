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
SDName: Orgrimmar
SD%Complete: 100
SDComment: Quest support: 2460, 5727, 6566
SDCategory: Orgrimmar
EndScriptData */

/* ContentData
npc_neeru_fireblade     npc_text + gossip options text missing
npc_shenthul
npc_thrall_warchief
npc_eitrigg
EndContentData */

#include "ScriptPCH.h"

/*######
## npc_neeru_fireblade
######*/

#define QUEST_5727  5727

#define GOSSIP_HNF "You may speak frankly, Neeru..."
#define GOSSIP_SNF "[PH] ..."
bool GossipHello_npc_neeru_fireblade(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(QUEST_5727) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HNF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(4513, pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_neeru_fireblade(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF+1:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SNF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(4513, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->AreaExploredOrEventHappens(QUEST_5727);
        break;
    }
    return true;
}

/*######
## npc_shenthul
######*/

enum eShenthul
{
    QUEST_SHATTERED_SALUTE  = 2460
};

struct npc_shenthulAI : public ScriptedAI
{
    npc_shenthulAI(Creature* c) : ScriptedAI(c) {}

    bool CanTalk;
    bool CanEmote;
    uint32 Salute_Timer;
    uint32 Reset_Timer;
    uint64 PlayerGUID;

    void Reset()
    {
        CanTalk = false;
        CanEmote = false;
        Salute_Timer = 6000;
        Reset_Timer = 0;
        PlayerGUID = 0;
    }

    void EnterCombat(Unit* /*who*/) {}

    void UpdateAI(const uint32 diff)
    {
        if (CanEmote)
        {
            if (Reset_Timer <= diff)
            {
                if (Player* pPlayer = Unit::GetPlayer(*me, PlayerGUID))
                {
                    if (pPlayer->GetTypeId() == TYPEID_PLAYER && pPlayer->GetQuestStatus(QUEST_SHATTERED_SALUTE) == QUEST_STATUS_INCOMPLETE)
                        pPlayer->FailQuest(QUEST_SHATTERED_SALUTE);
                }
                Reset();
            }
            else Reset_Timer -= diff;
        }

        if (CanTalk && !CanEmote)
        {
            if (Salute_Timer <= diff)
            {
                me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                CanEmote = true;
                Reset_Timer = 60000;
            }
            else Salute_Timer -= diff;
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

    void ReceiveEmote(Player* pPlayer, uint32 emote)
    {
        if (emote == TEXTEMOTE_SALUTE && pPlayer->GetQuestStatus(QUEST_SHATTERED_SALUTE) == QUEST_STATUS_INCOMPLETE)
        {
            if (CanEmote)
            {
                pPlayer->AreaExploredOrEventHappens(QUEST_SHATTERED_SALUTE);
                Reset();
            }
        }
    }
};

CreatureAI* GetAI_npc_shenthul(Creature* pCreature)
{
    return new npc_shenthulAI (pCreature);
}

bool QuestAccept_npc_shenthul(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_SHATTERED_SALUTE)
    {
        CAST_AI(npc_shenthulAI, pCreature->AI())->CanTalk = true;
        CAST_AI(npc_shenthulAI, pCreature->AI())->PlayerGUID = pPlayer->GetGUID();
    }
    return true;
}

/*######
## npc_thrall_warchief
######*/

#define QUEST_6566              6566

#define SPELL_CHAIN_LIGHTNING   16033
#define SPELL_SHOCK             16034

#define GOSSIP_HTW "Please share your wisdom with me, Warchief."
#define GOSSIP_STW1 "What discoveries?"
#define GOSSIP_STW2 "Usurper?"
#define GOSSIP_STW3 "With all due respect, Warchief - why not allow them to be destroyed? Does this not strengthen our position?"
#define GOSSIP_STW4 "I... I did not think of it that way, Warchief."
#define GOSSIP_STW5 "I live only to serve, Warchief! My life is empty and meaningless without your guidance."
#define GOSSIP_STW6 "Of course, Warchief!"

//@todo verify abilities/timers
struct npc_thrall_warchiefAI : public ScriptedAI
{
    npc_thrall_warchiefAI(Creature* c) : ScriptedAI(c) {}

    uint32 ChainLightning_Timer;
    uint32 Shock_Timer;

    void Reset()
    {
        ChainLightning_Timer = 2000;
        Shock_Timer = 8000;
    }

    void EnterCombat(Unit* /*who*/) {}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (ChainLightning_Timer <= diff)
        {
            DoCastVictim( SPELL_CHAIN_LIGHTNING);
            ChainLightning_Timer = 9000;
        }
        else ChainLightning_Timer -= diff;

        if (Shock_Timer <= diff)
        {
            DoCastVictim( SPELL_SHOCK);
            Shock_Timer = 15000;
        }
        else Shock_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_npc_thrall_warchief(Creature* pCreature)
{
    return new npc_thrall_warchiefAI (pCreature);
}

bool GossipHello_npc_thrall_warchief(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(QUEST_6566) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HTW, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_thrall_warchief(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF+1:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_STW1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(5733, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_STW2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        pPlayer->SEND_GOSSIP_MENU(5734, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+3:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_STW3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
        pPlayer->SEND_GOSSIP_MENU(5735, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+4:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_STW4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
        pPlayer->SEND_GOSSIP_MENU(5736, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+5:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_STW5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
        pPlayer->SEND_GOSSIP_MENU(5737, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+6:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_STW6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
        pPlayer->SEND_GOSSIP_MENU(5738, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+7:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->AreaExploredOrEventHappens(QUEST_6566);
        break;
    }
    return true;
}

/*######
## npc_eitrigg
######*/

#define GOSSIP_HELLO    "Hello, Eitrigg. I bring news from Blackrock Spire."
#define GOSSIP_SELECT1  "There is only one Warchief, Eitrigg!"
#define GOSSIP_SELECT2  "What do you mean?"
#define GOSSIP_SELECT3  "Hearthglen?? But..."
#define GOSSIP_SELECT4  "I will take you up on that offer, Eitrigg."
#define GOSSIP_SELECT5  "Ah, so that is how they pushed the Dark Iron to the lower parts of the Spire."
#define GOSSIP_SELECT6  "Perhaps there exists a way?"
#define GOSSIP_SELECT7  "As you wish, Eitrigg."

bool GossipHello_npc_eitrigg(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(4941) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(3573, pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_eitrigg(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(3574, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+1:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(3575, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        pPlayer->SEND_GOSSIP_MENU(3576, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+3:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
        pPlayer->SEND_GOSSIP_MENU(3577, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+4:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
        pPlayer->SEND_GOSSIP_MENU(3578, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+5:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
        pPlayer->SEND_GOSSIP_MENU(3579, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+6:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
        pPlayer->SEND_GOSSIP_MENU(3580, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+7:
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->AreaExploredOrEventHappens(4941);
        break;
    }
    return true;
}

void AddSC_orgrimmar()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_neeru_fireblade";
    newscript->pGossipHello =  &GossipHello_npc_neeru_fireblade;
    newscript->pGossipSelect = &GossipSelect_npc_neeru_fireblade;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_shenthul";
    newscript->GetAI = &GetAI_npc_shenthul;
    newscript->pQuestAccept =  &QuestAccept_npc_shenthul;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_thrall_warchief";
    newscript->GetAI = &GetAI_npc_thrall_warchief;
    newscript->pGossipHello =  &GossipHello_npc_thrall_warchief;
    newscript->pGossipSelect = &GossipSelect_npc_thrall_warchief;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_eitrigg";
    newscript->pGossipHello =  &GossipHello_npc_eitrigg;
    newscript->pGossipSelect = &GossipSelect_npc_eitrigg;
    newscript->RegisterSelf();
}

