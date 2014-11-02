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
SDName: Western_Plaguelands
SD%Complete: 90
SDComment: Quest support: 5216, 5219, 5222, 5225, 5229, 5231, 5233, 5235, 9446. To obtain Vitreous Focuser (could use more spesifics about gossip items)
SDCategory: Western Plaguelands
EndScriptData */

/* ContentData
npcs_dithers_and_arbington
npc_myranda_the_hag
npc_the_scourge_cauldron
npc_andorhal_tower
npc_anchorite_truuen
EndContentData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"

/*######
## npcs_dithers_and_arbington
######*/

#define GOSSIP_HDA1 "What does the Felstone Field Cauldron need?"
#define GOSSIP_HDA2 "What does the Dalson's Tears Cauldron need?"
#define GOSSIP_HDA3 "What does the Writhing Haunt Cauldron need?"
#define GOSSIP_HDA4 "What does the Gahrron's Withering Cauldron need?"

#define GOSSIP_SDA1 "Thanks, i need a Vitreous Focuser"

bool GossipHello_npcs_dithers_and_arbington(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());
    if (pCreature->isVendor())
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    if (pPlayer->GetQuestRewardStatus(5237) || pPlayer->GetQuestRewardStatus(5238))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HDA1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HDA2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HDA3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HDA4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
        pPlayer->SEND_GOSSIP_MENU(3985, pCreature->GetGUID());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npcs_dithers_and_arbington(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
    case GOSSIP_ACTION_TRADE:
        pPlayer->SEND_VENDORLIST(pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+1:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SDA1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
        pPlayer->SEND_GOSSIP_MENU(3980, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SDA1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
        pPlayer->SEND_GOSSIP_MENU(3981, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+3:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SDA1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
        pPlayer->SEND_GOSSIP_MENU(3982, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+4:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SDA1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
        pPlayer->SEND_GOSSIP_MENU(3983, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+5:
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, 17529, false);
        break;
    }
    return true;
}

/*######
## npc_myranda_the_hag
######*/

enum eMyranda
{
    QUEST_SUBTERFUGE        = 5862,
    QUEST_IN_DREAMS         = 5944,
    SPELL_SCARLET_ILLUSION  = 17961
};

#define GOSSIP_ITEM_ILLUSION    "I am ready for the illusion, Myranda."

bool GossipHello_npc_myranda_the_hag(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(QUEST_SUBTERFUGE) == QUEST_STATUS_COMPLETE &&
        pPlayer->GetQuestStatus(QUEST_IN_DREAMS) != QUEST_STATUS_COMPLETE &&
        !pPlayer->HasAura(SPELL_SCARLET_ILLUSION, 0))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ILLUSION, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(4773, pCreature->GetGUID());
        return true;
    }
    else
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_myranda_the_hag(Player* pPlayer, Creature* /*pCreature*/, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, SPELL_SCARLET_ILLUSION, false);
    }
    return true;
}

/*######
## npc_the_scourge_cauldron
######*/

struct npc_the_scourge_cauldronAI : public ScriptedAI
{
    npc_the_scourge_cauldronAI(Creature* c) : ScriptedAI(c) {}

    void Reset() {}

    void EnterCombat(Unit* /*who*/) {}

    void DoDie()
    {
        //summoner dies here
        me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        //override any database `spawntimesecs` to prevent duplicated summons
        uint32 rTime = me->GetRespawnDelay();
        if (rTime < 600)
            me->SetRespawnDelay(600);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (!who || who->GetTypeId() != TYPEID_PLAYER)
            return;

        if (who->GetTypeId() == TYPEID_PLAYER)
        {
            switch (me->GetAreaId())
            {
            case 199:                                   //felstone
                if (CAST_PLR(who)->GetQuestStatus(5216) == QUEST_STATUS_INCOMPLETE ||
                    CAST_PLR(who)->GetQuestStatus(5229) == QUEST_STATUS_INCOMPLETE)
                {
                    me->SummonCreature(11075, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
                    DoDie();
                }
                break;
            case 200:                                   //dalson
                if (CAST_PLR(who)->GetQuestStatus(5219) == QUEST_STATUS_INCOMPLETE ||
                    CAST_PLR(who)->GetQuestStatus(5231) == QUEST_STATUS_INCOMPLETE)
                {
                    me->SummonCreature(11077, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
                    DoDie();
                }
                break;
            case 201:                                   //gahrron
                if (CAST_PLR(who)->GetQuestStatus(5225) == QUEST_STATUS_INCOMPLETE ||
                    CAST_PLR(who)->GetQuestStatus(5235) == QUEST_STATUS_INCOMPLETE)
                {
                    me->SummonCreature(11078, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
                    DoDie();
                }
                break;
            case 202:                                   //writhing
                if (CAST_PLR(who)->GetQuestStatus(5222) == QUEST_STATUS_INCOMPLETE ||
                    CAST_PLR(who)->GetQuestStatus(5233) == QUEST_STATUS_INCOMPLETE)
                {
                    me->SummonCreature(11076, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
                    DoDie();
                }
                break;
            }
        }
    }
};
CreatureAI* GetAI_npc_the_scourge_cauldron(Creature* pCreature)
{
    return new npc_the_scourge_cauldronAI (pCreature);
}

/*######
##    npcs_andorhal_tower
######*/

enum eAndorhalTower
{
    GO_BEACON_TORCH                             = 176093
};

struct npc_andorhal_towerAI : public Scripted_NoMovementAI
{
    npc_andorhal_towerAI(Creature* c) : Scripted_NoMovementAI(c) {}

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho || pWho->GetTypeId() != TYPEID_PLAYER)
            return;

        if (me->FindNearestGameObject(GO_BEACON_TORCH, 10.0f))
            CAST_PLR(pWho)->KilledMonsterCredit(me->GetEntry(), me->GetGUID());
    }
};

CreatureAI* GetAI_npc_andorhal_tower(Creature* pCreature)
{
    return new npc_andorhal_towerAI (pCreature);
}

/*######
##  npc_anchorite_truuen
######*/

enum eTruuen
{
    SAY_WP_0                    = -1000709,
    SAY_WP_1                    = -1000710,
    SAY_WP_2                    = -1000711,
    SAY_WP_3                    = -1000712,
    SAY_WP_4                    = -1000713,
    SAY_WP_5                    = -1000714,
    SAY_WP_6                    = -1000715,

    NPC_GHOST_UTHER             = 17233,
    NPC_THEL_DANIS              = 1854,
    NPC_GHOUL                   = 1791,

    QUEST_TOMB_LIGHTBRINGER     = 9446
};

struct npc_anchorite_truuenAI : public npc_escortAI
{
    npc_anchorite_truuenAI(Creature* pCreature) : npc_escortAI(pCreature) { }

    uint32 EventTimer;
    uint64 UterGUID;
    uint32 uiPhase;

    void Reset()
    {
        EventTimer = 5000;
        UterGUID = 0;
        uiPhase = 0;
    }

    void WaypointReached(uint32 uiPointId)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (uiPointId)
        {
        case 8:
            DoScriptText(SAY_WP_0, me);
            for (int i = 0; i < 2; i++)
                me->SummonCreature(NPC_GHOUL, 1035.43, -1572.97, 61.5412, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90000);
            break;
        case 9:
            DoScriptText(SAY_WP_1, me);
            break;
        case 14:
            for (int i = 0; i < 4; i++)
                me->SummonCreature(NPC_GHOUL, 1159.77, -1762.64, 60.5699, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            break;
        case 15:
            DoScriptText(SAY_WP_2, me);
            SetRun(false);
            break;
        case 22:
            if (Creature* pTheldanis = me->FindNearestCreature(NPC_THEL_DANIS, 50))
                DoScriptText(SAY_WP_3, pTheldanis);
            break;
        case 23:
            if (Creature* pUter = me->SummonCreature(NPC_GHOST_UTHER, 971.86, -1825.42 , 81.99 , 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 10000))
            {
                pUter->AddUnitMovementFlag(MOVEFLAG_LEVITATING);
                DoScriptText(SAY_WP_4, pUter, me);
                UterGUID = pUter->GetGUID();
                uiPhase = 1;
                me->SetStandState(UNIT_STAND_STATE_KNEEL);
                SetEscortPaused(true);
            }
            break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        npc_escortAI::UpdateAI(uiDiff);

        if (!UpdateVictim())
        {
            if (HasEscortState(STATE_ESCORT_PAUSED) && uiPhase)
            {
                if (EventTimer <= uiDiff)
                {
                    EventTimer = 5000;

                    if (Creature* pUter = Unit::GetCreature(*me, UterGUID))
                    {
                        switch (uiPhase)
                        {
                        case 1:
                            DoScriptText(SAY_WP_5, pUter, me);
                            me->SetStandState(UNIT_STAND_STATE_STAND);
                            ++uiPhase;
                            break;
                        case 2:
                            DoScriptText(SAY_WP_6, pUter, me);
                            me->ForcedDespawn(30000);
                            pUter->ForcedDespawn(30000);
                            uiPhase = 0;
                            break;
                        }
                    }
                }
                else
                    EventTimer -= uiDiff;
            }
            return;
        }

        DoMeleeAttackIfReady();
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_GHOUL)
            pSummoned->AI()->AttackStart(me);
    }
};

CreatureAI* GetAI_npc_anchorite_truuen(Creature* pCreature)
{
    return new npc_anchorite_truuenAI(pCreature);
}

bool QuestAccept_npc_anchorite_truuen(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_TOMB_LIGHTBRINGER)
    {
        if (npc_anchorite_truuenAI* pEscortAI = CAST_AI(npc_anchorite_truuenAI, pCreature->AI()))
        {
            pEscortAI->Start(true, true, pPlayer->GetGUID(), pQuest);
            pEscortAI->SetDespawnAtEnd(false);
            return true;
        }
    }
    return false;
}

void AddSC_western_plaguelands()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npcs_dithers_and_arbington";
    newscript->pGossipHello = &GossipHello_npcs_dithers_and_arbington;
    newscript->pGossipSelect = &GossipSelect_npcs_dithers_and_arbington;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_myranda_the_hag";
    newscript->pGossipHello = &GossipHello_npc_myranda_the_hag;
    newscript->pGossipSelect = &GossipSelect_npc_myranda_the_hag;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_the_scourge_cauldron";
    newscript->GetAI = &GetAI_npc_the_scourge_cauldron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_andorhal_tower";
    newscript->GetAI = &GetAI_npc_andorhal_tower;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_anchorite_truuen";
    newscript->GetAI = &GetAI_npc_anchorite_truuen;
    newscript->pQuestAccept =  &QuestAccept_npc_anchorite_truuen;
    newscript->RegisterSelf();
}
