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
SDName: Terokkar_Forest
SD%Complete: 80
SDComment: Quest support: 9889, 10009, 10873, 10896, 11096, 10052, 10051, 10898, 10446/10447, 10852, 10887, 10922, 11085, Skettis->Ogri'la Flight
SDCategory: Terokkar Forest
EndScriptData */

/* ContentData
mob_unkor_the_ruthless
mob_infested_root_walker
mob_rotting_forest_rager
mob_netherweb_victim
npc_floon
npc_isla_starmane
npc_skyguard_prisoner
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"

/*######
## mob_unkor_the_ruthless
######*/

enum eUnkor
{
    SAY_SUBMIT                  = -1000194,

    FACTION_HOSTILE             = 45,
    FACTION_FRIENDLY            = 35,
    QUEST_DONTKILLTHEFATONE     = 9889,

    SPELL_PULVERIZE             = 2676
                                  //SPELL_QUID9889            = 32174
};

struct mob_unkor_the_ruthlessAI : public ScriptedAI
{
    mob_unkor_the_ruthlessAI(Creature* c) : ScriptedAI(c) {}

    bool CanDoQuest;
    uint32 UnkorUnfriendly_Timer;
    uint32 Pulverize_Timer;

    void Reset()
    {
        CanDoQuest = false;
        UnkorUnfriendly_Timer = 0;
        Pulverize_Timer = 3000;
        me->SetStandState(UNIT_STAND_STATE_STAND);
        me->SetFaction(FACTION_HOSTILE);
    }

    void EnterCombat(Unit* /*who*/) {}

    void DoNice()
    {
        DoScriptText(SAY_SUBMIT, me);
        me->SetFaction(FACTION_FRIENDLY);
        me->SetStandState(UNIT_STAND_STATE_SIT);
        me->RemoveAllAuras();
        me->DeleteThreatList();
        me->CombatStop();
        UnkorUnfriendly_Timer = 60000;
    }

    void DamageTaken(Unit* done_by, uint32& damage)
    {
        if (done_by->GetTypeId() == TYPEID_PLAYER)
            if ((me->GetHealth() - damage) * 100 / me->GetMaxHealth() < 30)
            {
                if (Group* pGroup = CAST_PLR(done_by)->GetGroup())
                {
                    for (GroupReference* itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
                    {
                        Player* groupie = itr->GetSource();
                        if (groupie && groupie->IsInMap(done_by) &&
                            groupie->GetQuestStatus(QUEST_DONTKILLTHEFATONE) == QUEST_STATUS_INCOMPLETE &&
                            groupie->GetReqKillOrCastCurrentCount(QUEST_DONTKILLTHEFATONE, 18260) == 10)
                        {
                            groupie->AreaExploredOrEventHappens(QUEST_DONTKILLTHEFATONE);
                            if (!CanDoQuest)
                                CanDoQuest = true;
                        }
                    }
                }
                else if (CAST_PLR(done_by)->GetQuestStatus(QUEST_DONTKILLTHEFATONE) == QUEST_STATUS_INCOMPLETE &&
                         CAST_PLR(done_by)->GetReqKillOrCastCurrentCount(QUEST_DONTKILLTHEFATONE, 18260) == 10)
                {
                    CAST_PLR(done_by)->AreaExploredOrEventHappens(QUEST_DONTKILLTHEFATONE);
                    CanDoQuest = true;
                }
            }
    }

    void UpdateAI(const uint32 diff)
    {
        if (CanDoQuest)
        {
            if (!UnkorUnfriendly_Timer)
            {
                //DoCast(me,SPELL_QUID9889);        //not using spell for now
                DoNice();
            }
            else
            {
                if (UnkorUnfriendly_Timer <= diff)
                {
                    EnterEvadeMode();
                    return;
                }
                else UnkorUnfriendly_Timer -= diff;
            }
        }

        if (!UpdateVictim())
            return;

        if (Pulverize_Timer <= diff)
        {
            DoCast(me, SPELL_PULVERIZE);
            Pulverize_Timer = 9000;
        }
        else Pulverize_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_unkor_the_ruthless(Creature* pCreature)
{
    return new mob_unkor_the_ruthlessAI (pCreature);
}

/*######
## mob_infested_root_walker
######*/

struct mob_infested_root_walkerAI : public ScriptedAI
{
    mob_infested_root_walkerAI(Creature* c) : ScriptedAI(c) {}

    void Reset() 
	{
		regrowth_timer = 12000;
	}
	
	uint32 regrowth_timer;

    void EnterCombat(Unit* /*who*/) { }

    void DamageTaken(Unit* done_by, uint32& damage)
    {
        if (done_by && done_by->GetTypeId() == TYPEID_PLAYER)
            if (me->GetHealth() <= damage)
                if (rand() % 100 < 75)
                    //Summon Wood Mites
                    me->CastSpell(me, 39130, true);
					me->CastSpell(me, 39130, true);
					me->CastSpell(me, 39130, true);
					me->CastSpell(me, 39130, true);
					me->CastSpell(me, 39130, true);
    }

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (regrowth_timer <= diff)
		{
			DoCast(me, 39000);
			regrowth_timer = 15000;
		}
		else regrowth_timer -= diff;

		DoMeleeAttackIfReady();
	}
};
CreatureAI* GetAI_mob_infested_root_walker(Creature* pCreature)
{
    return new mob_infested_root_walkerAI (pCreature);
}

/*######
## mob_rotting_forest_rager
######*/

struct mob_rotting_forest_ragerAI : public ScriptedAI
{
    mob_rotting_forest_ragerAI(Creature* c) : ScriptedAI(c) {}

    void Reset() 
	{
		thunderclap_timer = 5000;
	}

	uint32 thunderclap_timer;

    void EnterCombat(Unit* /*who*/) { }

    void DamageTaken(Unit* done_by, uint32& damage)
    {
        if (done_by->GetTypeId() == TYPEID_PLAYER)
            if (me->GetHealth() <= damage)
                if (rand() % 100 < 75)
                    //Summon Lots of Wood Mights
                    me->CastSpell(me, 39134, true);
					me->CastSpell(me, 39130, true);
					me->CastSpell(me, 39130, true);
					me->CastSpell(me, 39130, true);
					me->CastSpell(me, 39130, true);
					me->CastSpell(me, 39130, true);
					me->CastSpell(me, 39130, true);
					me->CastSpell(me, 39130, true);
					me->CastSpell(me, 39130, true);
					me->CastSpell(me, 39130, true);
					me->CastSpell(me, 39130, true);
    }

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (thunderclap_timer <= diff)
		{
			DoCast(15548);
			thunderclap_timer = 12000;
		}
		else thunderclap_timer -= diff;

		DoMeleeAttackIfReady();
	}
};
CreatureAI* GetAI_mob_rotting_forest_rager(Creature* pCreature)
{
    return new mob_rotting_forest_ragerAI (pCreature);
}

/*######
## mob_netherweb_victim
######*/

enum TakenintheNight
{
    NPC_QUEST_TARGET        = 22459,
    QUEST_TAKEN_INTHE_NIGHT = 10873
};

const uint32 netherwebVictims[6] =
{
    18470, 16805, 21242, 18452, 22482, 21285
};
struct mob_netherweb_victimAI : public ScriptedAI
{
    mob_netherweb_victimAI(Creature* c) : ScriptedAI(c) {}

    void Reset() { }
    void EnterCombat(Unit* /*who*/) { }
    void MoveInLineOfSight(Unit* /*who*/) { }

    void JustDied(Unit* Killer)
    {
        if (Killer->GetTypeId() == TYPEID_PLAYER)
        {
            if (CAST_PLR(Killer)->GetQuestStatus(10873) == QUEST_STATUS_INCOMPLETE)
            {
                if (rand() % 100 < 25)
                {
                    DoSpawnCreature(NPC_QUEST_TARGET, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    CAST_PLR(Killer)->KilledMonsterCredit(NPC_QUEST_TARGET, 0);
                }
                else
                    DoSpawnCreature(netherwebVictims[rand() % 6], 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);

                if (rand() % 100 < 75)
                    DoSpawnCreature(netherwebVictims[rand() % 6], 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                DoSpawnCreature(netherwebVictims[rand() % 6], 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
            }
        }
    }
};
CreatureAI* GetAI_mob_netherweb_victim(Creature* pCreature)
{
    return new mob_netherweb_victimAI (pCreature);
}

/*######
## npc_floon
######*/

#define GOSSIP_FLOON1           "I have been sent by Sal'salabim to collect a debt that you owe. Pay up or I'm going to hurt you."
#define GOSSIP_FLOON2           "Hand over the money or die...again!"

enum eFloon
{
    SAY_FLOON_ATTACK        = -1000195,

    FACTION_HOSTILE_FL      = 1738,
    FACTION_FRIENDLY_FL     = 35,

    SPELL_SILENCE           = 6726,
    SPELL_FROSTBOLT         = 9672,
    SPELL_FROST_NOVA        = 11831
};

struct npc_floonAI : public ScriptedAI
{
    npc_floonAI(Creature* c) : ScriptedAI(c) {}

    uint32 Silence_Timer;
    uint32 Frostbolt_Timer;
    uint32 FrostNova_Timer;

    void Reset()
    {
        Silence_Timer = 2000;
        Frostbolt_Timer = 4000;
        FrostNova_Timer = 9000;
        me->SetFaction(FACTION_FRIENDLY_FL);
    }

    void EnterCombat(Unit* /*who*/) {}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Silence_Timer <= diff)
        {
            DoCastVictim(SPELL_SILENCE);
            Silence_Timer = 30000;
        }
        else Silence_Timer -= diff;

        if (FrostNova_Timer <= diff)
        {
            DoCast(me, SPELL_FROST_NOVA);
            FrostNova_Timer = 20000;
        }
        else FrostNova_Timer -= diff;

        if (Frostbolt_Timer <= diff)
        {
            DoCastVictim(SPELL_FROSTBOLT);
            Frostbolt_Timer = 5000;
        }
        else Frostbolt_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_npc_floon(Creature* pCreature)
{
    return new npc_floonAI (pCreature);
}

bool GossipHello_npc_floon(Player* player, Creature* pCreature)
{
    if (player->GetQuestStatus(10009) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_FLOON1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SEND_GOSSIP_MENU(9442, pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_floon(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
	switch (action)
    {
	case GOSSIP_ACTION_INFO_DEF + 1:
		player->ADD_GOSSIP_ITEM(0, GOSSIP_FLOON2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(9443, pCreature->GetGUID());
		break;
	case GOSSIP_ACTION_INFO_DEF + 2:
        player->CLOSE_GOSSIP_MENU();
		pCreature->SetFaction(1738);
        DoScriptText(SAY_FLOON_ATTACK, pCreature, player);
        ((npc_floonAI*)pCreature->AI())->AttackStart(player);
		break;
    }

    return true;
}

/*######
## npc_isla_starmane
######*/

enum eIsla
{
    SAY_PROGRESS_1  = -1910149,
	SAY_PROGRESS_2  = -1910150,
	SAY_PROGRESS_3  = -1910151,

    QUEST_EFTW_H    = 10052,
    QUEST_EFTW_A    = 10051,
    GO_CAGE         = 182794,
    SPELL_CAT       = 32447
};

struct npc_isla_starmaneAI : public npc_escortAI
{
    npc_isla_starmaneAI(Creature* c) : npc_escortAI(c) {}

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {
        case 0:
            {
                GameObject* Cage = me->FindNearestGameObject(GO_CAGE, 10);
                if (Cage)
                    Cage->SetGoState(GO_STATE_ACTIVE);
            }
            break;
        case 5:
            DoScriptText(SAY_PROGRESS_1, me, pPlayer);
            break;
        case 6:
            DoScriptText(SAY_PROGRESS_2, me, pPlayer);
            break;
        case 29:
            DoScriptText(SAY_PROGRESS_3, me, pPlayer);
            if (pPlayer)
            {
                if (pPlayer->GetTeam() == ALLIANCE)
                    pPlayer->GroupEventHappens(QUEST_EFTW_A, me);
                else if (pPlayer->GetTeam() == HORDE)
                    pPlayer->GroupEventHappens(QUEST_EFTW_H, me);
            }
            me->SetInFront(pPlayer);
            break;
        case 30:
            me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
            break;
        case 31:
            DoCast(me, SPELL_CAT);
            me->SetWalk(false);
            break;
        }
    }

    void Reset()
    {
        me->RestoreFaction();
    }

    void JustDied(Unit* /*killer*/)
    {
        if (Player* pPlayer = GetPlayerForEscort())
        {
            if (pPlayer->GetTeam() == ALLIANCE)
                pPlayer->FailQuest(QUEST_EFTW_A);
            else if (pPlayer->GetTeam() == HORDE)
                pPlayer->FailQuest(QUEST_EFTW_H);
        }
    }
};

bool QuestAccept_npc_isla_starmane(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_EFTW_H || quest->GetQuestId() == QUEST_EFTW_A)
    {
        CAST_AI(npc_escortAI, (pCreature->AI()))->Start(true, false, pPlayer->GetGUID());
        pCreature->SetFaction(113);
    }
    return true;
}

CreatureAI* GetAI_npc_isla_starmaneAI(Creature* pCreature)
{
    return  new npc_isla_starmaneAI(pCreature);
}

/*######
## go_skull_pile
######*/

#define GOSSIP_S_DARKSCREECHER_AKKARAI   "Summon Darkscreecher Akkarai"
#define GOSSIP_S_KARROG                  "Summon Karrog"
#define GOSSIP_S_GEZZARAK_THE_HUNTRESS   "Summon Gezzarak the Huntress"
#define GOSSIP_S_VAKKIZ_THE_WINDRAGER    "Summon Vakkiz the Windrager"
#define ITEM_SKULL_PILE_OFFERING		  32620

bool GossipHello_go_skull_pile(Player* player, GameObject* _GO)
{
    if ((player->GetQuestStatus(11885) == QUEST_STATUS_INCOMPLETE) || player->GetQuestRewardStatus(11885))
    {
        player->ADD_GOSSIP_ITEM(0, GOSSIP_S_DARKSCREECHER_AKKARAI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_S_KARROG, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_S_GEZZARAK_THE_HUNTRESS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_S_VAKKIZ_THE_WINDRAGER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    }

    player->SEND_GOSSIP_MENU(_GO->GetGOInfo()->questgiver.gossipID, _GO->GetGUID());
    return true;
}

void SendActionMenu_go_skull_pile(Player* player, GameObject* _GO, uint32 action)
{
    switch (action)
    {
	case GOSSIP_ACTION_INFO_DEF + 1:
		if (player->HasItemCount(ITEM_SKULL_PILE_OFFERING, 10))
		{
			player->DestroyItemCount(ITEM_SKULL_PILE_OFFERING, 10, true);
			player->SummonCreature(23161, _GO->GetPositionX(), _GO->GetPositionY(), _GO->GetPositionZ(), 0.0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
		}
		break;
	case GOSSIP_ACTION_INFO_DEF + 2:
		if (player->HasItemCount(ITEM_SKULL_PILE_OFFERING, 10))
		{
			player->DestroyItemCount(ITEM_SKULL_PILE_OFFERING, 10, true);
			player->SummonCreature(23165, _GO->GetPositionX(), _GO->GetPositionY(), _GO->GetPositionZ(), 0.0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
		}
		break;
	case GOSSIP_ACTION_INFO_DEF + 3:
		if (player->HasItemCount(ITEM_SKULL_PILE_OFFERING, 10))
		{
			player->DestroyItemCount(ITEM_SKULL_PILE_OFFERING, 10, true);
			player->SummonCreature(23163, _GO->GetPositionX(), _GO->GetPositionY(), _GO->GetPositionZ(), 0.0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
		}
		break;
	case GOSSIP_ACTION_INFO_DEF + 4:
		if (player->HasItemCount(ITEM_SKULL_PILE_OFFERING, 10))
		{
			player->DestroyItemCount(ITEM_SKULL_PILE_OFFERING, 10, true);
			player->SummonCreature(23162, _GO->GetPositionX(), _GO->GetPositionY(), _GO->GetPositionZ(), 0.0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
		}
		break;
    }
}

bool GossipSelect_go_skull_pile(Player* player, GameObject* _GO, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendActionMenu_go_skull_pile(player, _GO, action);
        break;
    }
    return true;
}

/*######
## npc_skywing
######*/

enum
{
    QUEST_SKYWING         = 10898,

    NPC_LUANGA_IMPRISONER = 18533
};

struct npc_skywingAI : public npc_escortAI
{
    npc_skywingAI(Creature* c) : npc_escortAI(c) {}

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();
        if (!pPlayer)
            return;

        switch (i)
        {
        case 7:
            me->SummonCreature(NPC_LUANGA_IMPRISONER, -3399.274658f, 4055.948975f, 18.603474f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
            break;
        case 8:
            pPlayer->AreaExploredOrEventHappens(10898);
            break;
        }
    }

    void EnterCombat(Unit* /*who*/) {}

    void Reset() {}

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(me);
    }

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);
    }
};

CreatureAI* GetAI_npc_skywingAI(Creature* pCreature)
{
    return new npc_skywingAI(pCreature);
}

bool QuestAccept_npc_skywing(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_SKYWING)
    {
        if (npc_skywingAI* pEscortAI = CAST_AI(npc_skywingAI, pCreature->AI()))
            pEscortAI->Start(false, true, pPlayer->GetGUID(), quest);
    }

    return true;
}

/*######
## npc_slim
######*/

enum eSlim
{
    FACTION_CONSORTIUM  = 933
};

bool GossipHello_npc_slim(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->IsVendor() && pPlayer->GetReputationRank(FACTION_CONSORTIUM) >= REP_FRIENDLY)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
        pPlayer->SEND_GOSSIP_MENU(9896, pCreature->GetGUID());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(9895, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_slim(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetGUID());

    return true;
}

/*########
####npc_akuno
#####*/

enum
{
    SAY_AKU_START           = -1000477,
    SAY_AKU_AMBUSH_A        = -1000478,
    SAY_AKU_AMBUSH_B        = -1000479,
    SAY_AKU_AMBUSH_B_REPLY  = -1000480,
    SAY_AKU_COMPLETE        = -1000481,

    SPELL_CHAIN_LIGHTNING   = 39945,

    QUEST_ESCAPING_TOMB     = 10887,
    NPC_CABAL_SKIRMISHER    = 21661
};

static float m_afAmbushB1[] = { -2895.525879f, 5336.431641f, -11.800f};
static float m_afAmbushB2[] = { -2890.604980f, 5331.938965f, -11.282f};

struct npc_akunoAI : public npc_escortAI
{
    npc_akunoAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiChainLightningTimer;

    void Reset()
    {
        m_uiChainLightningTimer = 1000;
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
        case 5:
            DoScriptText(SAY_AKU_AMBUSH_A, me);
            me->SummonCreature(NPC_CABAL_SKIRMISHER, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            break;
        case 14:
            DoScriptText(SAY_AKU_AMBUSH_B, me);

            if (Creature* pTemp = me->SummonCreature(NPC_CABAL_SKIRMISHER, m_afAmbushB1[0], m_afAmbushB1[1], m_afAmbushB1[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000))
                DoScriptText(SAY_AKU_AMBUSH_B_REPLY, pTemp);

            me->SummonCreature(NPC_CABAL_SKIRMISHER, m_afAmbushB2[0], m_afAmbushB2[1], m_afAmbushB2[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            break;
        case 15:
            SetRun();
            break;
        case 18:
            DoScriptText(SAY_AKU_COMPLETE, me);

            if (Player* pPlayer = GetPlayerForEscort())
                pPlayer->GroupEventHappens(QUEST_ESCAPING_TOMB, me);

            break;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(me);
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
            return;

        if (m_uiChainLightningTimer < uiDiff)
        {
            DoCastVictim( SPELL_CHAIN_LIGHTNING);
            m_uiChainLightningTimer = urand(7000, 14000);
        }
        else
            m_uiChainLightningTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_akuno(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPING_TOMB)
    {
        if (npc_akunoAI* pEscortAI = CAST_AI(npc_akunoAI, pCreature->AI()))
        {
            pCreature->SetStandState(UNIT_STAND_STATE_STAND);
            pCreature->SetFaction(FACTION_ESCORT_N_NEUTRAL_ACTIVE);

            DoScriptText(SAY_AKU_START, pCreature);
            pEscortAI->Start(false, false, pPlayer->GetGUID(), pQuest);
        }
    }
    return true;
}

CreatureAI* GetAI_npc_akuno(Creature* pCreature)
{
    return new npc_akunoAI(pCreature);
}

/*######
## npc_skyguard_handler_deesak
######*/

#define GOSSIP_SKYGUARD "Fly me to Ogri'la please"

bool GossipHello_npc_skyguard_handler_deesak(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->IsQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetReputationRank(1031) >= REP_HONORED)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SKYGUARD, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_skyguard_handler_deesak(Player* pPlayer, Creature* /*pCreature*/, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer, 41279, true);             //TaxiPath 705 (Taxi - Skettis to Skyguard Outpost)
    }
    return true;
}

/*######
## npc_letoll
######*/

enum
{
    SAY_LE_START                    = -1000511,
    SAY_LE_KEEP_SAFE                = -1000512,
    SAY_LE_NORTH                    = -1000513,
    SAY_LE_ARRIVE                   = -1000514,
    SAY_LE_BURIED                   = -1000515,
    SAY_LE_ALMOST                   = -1000516,
    SAY_LE_DRUM                     = -1000517,
    SAY_LE_DRUM_REPLY               = -1000518,
    SAY_LE_DISCOVERY                = -1000519,
    SAY_LE_DISCOVERY_REPLY          = -1000520,
    SAY_LE_NO_LEAVE                 = -1000521,
    SAY_LE_NO_LEAVE_REPLY1          = -1000522,
    SAY_LE_NO_LEAVE_REPLY2          = -1000523,
    SAY_LE_NO_LEAVE_REPLY3          = -1000524,
    SAY_LE_NO_LEAVE_REPLY4          = -1000525,
    SAY_LE_SHUT                     = -1000526,
    SAY_LE_REPLY_HEAR               = -1000527,
    SAY_LE_IN_YOUR_FACE             = -1000528,
    SAY_LE_HELP_HIM                 = -1000529,
    EMOTE_LE_PICK_UP                = -1000530,
    SAY_LE_THANKS                   = -1000531,

    QUEST_DIGGING_BONES             = 10922,

    NPC_RESEARCHER                  = 22464,
    NPC_BONE_SIFTER                 = 22466,

    MAX_RESEARCHER                  = 4
};

//Some details still missing from here, and will also have issues if followers evade for any reason.
struct npc_letollAI : public npc_escortAI
{
    npc_letollAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_uiEventTimer = 5000;
        m_uiEventCount = 0;
        Reset();
    }

    std::list<Creature*> m_lResearchersList;

    uint32 m_uiEventTimer;
    uint32 m_uiEventCount;

    void Reset() {}

    //will make them follow, but will only work until they enter combat with any unit
    void SetFormation()
    {
        uint32 uiCount = 0;

        for (std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
        {
            float fAngle = uiCount < MAX_RESEARCHER ? M_PI / MAX_RESEARCHER - (uiCount * 2 * M_PI / MAX_RESEARCHER) : 0.0f;

            if ((*itr)->IsAlive() && !(*itr)->IsInCombat())
                (*itr)->GetMotionMaster()->MoveFollow(me, 2.5f, fAngle);

            ++uiCount;
        }
    }

    Creature* GetAvailableResearcher(uint8 uiListNum)
    {
        if (!m_lResearchersList.empty())
        {
            uint8 uiNum = 1;

            for (std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
            {
                if (uiListNum && uiListNum != uiNum)
                {
                    ++uiNum;
                    continue;
                }

                if ((*itr)->IsAlive() && (*itr)->IsWithinDistInMap(me, 20.0f))
                    return (*itr);
            }
        }

        return NULL;
    }

    void JustStartedEscort()
    {
        m_uiEventTimer = 5000;
        m_uiEventCount = 0;

        m_lResearchersList.clear();

        me->GetCreatureListWithEntryInGrid(m_lResearchersList, NPC_RESEARCHER, 25.0f);

        if (!m_lResearchersList.empty())
            SetFormation();
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
        case 0:
            JustStartedEscort();
            for (std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
                (*itr)->SetWalk(true);
            if (Player* pPlayer = GetPlayerForEscort())
                DoScriptText(SAY_LE_KEEP_SAFE, me, pPlayer);
            break;
        case 1:
            DoScriptText(SAY_LE_NORTH, me);
            break;
        case 10:
            DoScriptText(SAY_LE_ARRIVE, me);
            break;
        case 12:
            DoScriptText(SAY_LE_BURIED, me);
            SetEscortPaused(true);
            break;
        case 13:
            SetRun();
            for (std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
                (*itr)->SetUnitMovementFlags(MOVEMENTFLAG_NONE);
            break;
        }
    }

    void Aggro(Unit* pWho)
    {
        if (pWho->IsInCombat() && pWho->GetTypeId() == TYPEID_UNIT && pWho->GetEntry() == NPC_BONE_SIFTER)
            DoScriptText(SAY_LE_HELP_HIM, me);
    }

    void JustSummoned(Creature* pSummoned)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (pPlayer && pPlayer->IsAlive())
            pSummoned->AI()->AttackStart(pPlayer);
        else
            pSummoned->AI()->AttackStart(me);
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
        {
            if (HasEscortState(STATE_ESCORT_PAUSED))
            {
                if (m_uiEventTimer < uiDiff)
                {
                    m_uiEventTimer = 7000;

                    switch (m_uiEventCount)
                    {
                    case 0:
                        DoScriptText(SAY_LE_ALMOST, me);
                        break;
                    case 1:
                        DoScriptText(SAY_LE_DRUM, me);
                        break;
                    case 2:
                        if (Creature* pResearcher = GetAvailableResearcher(0))
                            DoScriptText(SAY_LE_DRUM_REPLY, pResearcher);
                        break;
                    case 3:
                        DoScriptText(SAY_LE_DISCOVERY, me);
                        break;
                    case 4:
                        if (Creature* pResearcher = GetAvailableResearcher(0))
                            DoScriptText(SAY_LE_DISCOVERY_REPLY, pResearcher);
                        break;
                    case 5:
                        DoScriptText(SAY_LE_NO_LEAVE, me);
                        break;
                    case 6:
                        if (Creature* pResearcher = GetAvailableResearcher(1))
                            DoScriptText(SAY_LE_NO_LEAVE_REPLY1, pResearcher);
                        break;
                    case 7:
                        if (Creature* pResearcher = GetAvailableResearcher(2))
                            DoScriptText(SAY_LE_NO_LEAVE_REPLY2, pResearcher);
                        break;
                    case 8:
                        if (Creature* pResearcher = GetAvailableResearcher(3))
                            DoScriptText(SAY_LE_NO_LEAVE_REPLY3, pResearcher);
                        break;
                    case 9:
                        if (Creature* pResearcher = GetAvailableResearcher(4))
                            DoScriptText(SAY_LE_NO_LEAVE_REPLY4, pResearcher);
                        break;
                    case 10:
                        DoScriptText(SAY_LE_SHUT, me);
                        break;
                    case 11:
                        if (Creature* pResearcher = GetAvailableResearcher(0))
                            DoScriptText(SAY_LE_REPLY_HEAR, pResearcher);
                        break;
                    case 12:
                        DoScriptText(SAY_LE_IN_YOUR_FACE, me);
                        me->SummonCreature(NPC_BONE_SIFTER, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                        break;
                    case 13:
                        DoScriptText(EMOTE_LE_PICK_UP, me);

                        if (Player* pPlayer = GetPlayerForEscort())
                        {
                            DoScriptText(SAY_LE_THANKS, me, pPlayer);
                            pPlayer->GroupEventHappens(QUEST_DIGGING_BONES, me);
                        }

                        SetEscortPaused(false);
                        break;
                    }

                    ++m_uiEventCount;
                }
                else
                    m_uiEventTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_letoll(Creature* pCreature)
{
    return new npc_letollAI(pCreature);
}

bool QuestAccept_npc_letoll(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_DIGGING_BONES)
    {
        if (npc_letollAI* pEscortAI = dynamic_cast<npc_letollAI*>(pCreature->AI()))
        {
            DoScriptText(SAY_LE_START, pCreature);
            pCreature->SetFaction(FACTION_ESCORT_N_NEUTRAL_PASSIVE);

            pEscortAI->Start(false, false, pPlayer->GetGUID(), pQuest, true);
        }
    }

    return true;
}

/*######
## npc_mana_bomb_exp_trigger
######*/

enum
{
    SAY_COUNT_1                 = -1000472,
    SAY_COUNT_2                 = -1000473,
    SAY_COUNT_3                 = -1000474,
    SAY_COUNT_4                 = -1000475,
    SAY_COUNT_5                 = -1000476,

    SPELL_MANA_BOMB_LIGHTNING   = 37843,
    SPELL_MANA_BOMB_EXPL        = 35513,

    NPC_MANA_BOMB_EXPL_TRIGGER  = 20767,
    NPC_MANA_BOMB_KILL_TRIGGER  = 21039
};

struct npc_mana_bomb_exp_triggerAI : public ScriptedAI
{
    npc_mana_bomb_exp_triggerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    GameObject* pManaBomb;

    bool m_bIsActivated;
    uint32 m_uiEventTimer;
    uint32 m_uiEventCounter;

    void Reset()
    {
        pManaBomb = NULL;
        m_bIsActivated = false;
        m_uiEventTimer = 1000;
        m_uiEventCounter = 0;
    }

    void DoTrigger(Player* pPlayer, GameObject* pGo)
    {
        if (m_bIsActivated)
            return;

        m_bIsActivated = true;

        pPlayer->KilledMonsterCredit(NPC_MANA_BOMB_KILL_TRIGGER, 0);

        pManaBomb = pGo;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_bIsActivated)
            return;

        if (m_uiEventTimer < uiDiff)
        {
            m_uiEventTimer = 1000;

            if (m_uiEventCounter < 10)
                me->CastSpell(me, SPELL_MANA_BOMB_LIGHTNING, false);

            switch (m_uiEventCounter)
            {
            case 5:
                if (pManaBomb)
                    pManaBomb->SetGoState(GO_STATE_ACTIVE);

                DoScriptText(SAY_COUNT_1, me);
                break;
            case 6:
                DoScriptText(SAY_COUNT_2, me);
                break;
            case 7:
                DoScriptText(SAY_COUNT_3, me);
                break;
            case 8:
                DoScriptText(SAY_COUNT_4, me);
                break;
            case 9:
                DoScriptText(SAY_COUNT_5, me);
                break;
            case 10:
                me->CastSpell(me, SPELL_MANA_BOMB_EXPL, false);
                break;
            case 30:
                if (pManaBomb)
                    pManaBomb->SetGoState(GO_STATE_READY);

                Reset();
                break;
            }

            ++m_uiEventCounter;
        }
        else
            m_uiEventTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_mana_bomb_exp_trigger(Creature* pCreature)
{
    return new npc_mana_bomb_exp_triggerAI(pCreature);
}

/*######
## go_mana_bomb
######*/

bool GOHello_go_mana_bomb(Player* pPlayer, GameObject* pGo)
{
    if (Creature* pCreature = GetClosestCreatureWithEntry(pGo, NPC_MANA_BOMB_EXPL_TRIGGER, INTERACTION_DISTANCE))
    {
        if (npc_mana_bomb_exp_triggerAI* pBombAI = dynamic_cast<npc_mana_bomb_exp_triggerAI*>(pCreature->AI()))
            pBombAI->DoTrigger(pPlayer, pGo);
    }

    return true;
}


/*#####
## go_veil_skith_cage & npc_captive_child
#####*/

enum
{
    QUEST_MISSING_FRIENDS     = 10852,
    NPC_CAPTIVE_CHILD         = 22314,
    SAY_THANKS_1              = -1000590,
    SAY_THANKS_2              = -1000591,
    SAY_THANKS_3              = -1000592,
};

bool GOHello_veil_skith_cage(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_MISSING_FRIENDS) == QUEST_STATUS_INCOMPLETE)
    {
        if (Creature* pChild = pGo->FindNearestCreature( NPC_CAPTIVE_CHILD, 5, true))
        {
            pPlayer->KilledMonsterCredit(NPC_CAPTIVE_CHILD, 0);

            switch (urand(0, 2))
            {
            case 0:
                DoScriptText(SAY_THANKS_1, pChild);
                break;
            case 1:
                DoScriptText(SAY_THANKS_2, pChild);
                break;
            case 2:
                DoScriptText(SAY_THANKS_3, pChild);
                break;
            }

            pChild->GetMotionMaster()->Clear();
            pChild->GetMotionMaster()->MovePoint(0, -2648.049f, 5274.573f, 1.691529f);
        }
    }
    return false;
};

struct npc_captive_child : public ScriptedAI
{
    npc_captive_child(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset() {}

    void WaypointReached(uint32 /*uiPointId*/)
    {
        // we only have one waypoint
        me->ForcedDespawn();
    }
};

CreatureAI* GetAI_npc_captive_child(Creature* pCreature)
{
    return new npc_captive_child(pCreature);
}

/*######
## npc_skyguard_prisoner
######*/

enum SPrisoner
{
    SAY_START                     = -1000716,
    SAY_DONT_LIKE                 = -1000717,
    SAY_COMPLETE                  = -1000718,

    GO_PRISONER_CAGE              = 185952,

    QUEST_ESCAPE_FROM_SKETTIS     = 11085,
};

struct npc_skyguard_prisonerAI : public npc_escortAI
{
    npc_skyguard_prisonerAI(Creature* pCreature) : npc_escortAI(pCreature) {}

    void Reset() {}

    uint32 CalculateWaypointID()
    {
        //@todo we have 2 ways to calculate wich waypoint we need to use

        /*if (abs(me->GetPositionX() + 4108.25) < abs(me->GetPositionX() + 3718.81) &&
            abs(me->GetPositionX() + 4108.25) < abs(me->GetPositionX() + 3671.51))
            return 1;

        if (abs(me->GetPositionX() + 3718.81) < abs(me->GetPositionX() + 4108.25) &&
            abs(me->GetPositionX() + 3718.81) < abs(me->GetPositionX() + 3671.51))
            return 2;

        return 3;*/

        switch (me->GetGUIDLow())
        {
        case 1189307:
            return 1;
            break;
        case 1189309:
            return 2;
            break;
        default:
            return 3;
            break;
        }
    }

    void StartEvent(Player* pPlayer, const Quest* pQuest)
    {
        switch (CalculateWaypointID())
        {
        case 1:
            AddWaypoint(0, -4108.25f, 3032.18f, 344.799f, 3000);
            AddWaypoint(1, -4114.41f, 3036.73f, 344.039f);
            AddWaypoint(2, -4126.41f, 3026.07f, 344.156f);
            AddWaypoint(3, -4145.17f, 3029.69f, 337.423f);
            AddWaypoint(4, -4173.69f, 3035.72f, 343.346f);
            AddWaypoint(5, -4173.70f, 3047.37f, 343.888f);
            AddWaypoint(6, -4183.47f, 3060.62f, 344.157f, 3000);
            AddWaypoint(7, -4179.13f, 3090.20f, 323.971f, 30000);
            Start(false, false, pPlayer->GetGUID(), pQuest);
            break;
        case 2:
            AddWaypoint(0, -3718.81f, 3787.24f, 302.890f, 3000);
            AddWaypoint(1, -3714.44f, 3780.35f, 302.075f);
            AddWaypoint(2, -3698.33f, 3788.04f, 302.171f);
            AddWaypoint(3, -3679.36f, 3780.25f, 295.077f);
            AddWaypoint(4, -3654.82f, 3770.43f, 301.291f);
            AddWaypoint(5, -3656.07f, 3757.31f, 301.985f);
            AddWaypoint(6, -3648.83f, 3743.07f, 302.173f, 3000);
            AddWaypoint(7, -3659.16f, 3714.94f, 281.576f, 30000);
            Start(false, false, pPlayer->GetGUID(), pQuest);
            break;
        case 3:
            AddWaypoint(0, -3671.51f, 3385.36f, 312.956f, 3000);
            AddWaypoint(1, -3677.74f, 3379.05f, 312.136f);
            AddWaypoint(2, -3667.52f, 3366.45f, 312.233f);
            AddWaypoint(3, -3672.87f, 3343.52f, 304.994f);
            AddWaypoint(4, -3679.35f, 3319.01f, 311.419f);
            AddWaypoint(5, -3692.93f, 3318.69f, 312.081f);
            AddWaypoint(6, -3704.08f, 3309.56f, 312.233f, 3000);
            AddWaypoint(7, -3733.99f, 3315.77f, 292.093f, 30000);
            Start(false, false, pPlayer->GetGUID(), pQuest);
            break;
        }
        return;
    }

    void WaypointReached(uint32 uiPointId)
    {
        if (/*Player* pPlayer = */GetPlayerForEscort())
        {
            switch (uiPointId)
            {
            case 0:
                DoScriptText(SAY_START, me);
                break;
            case 6:
                DoScriptText(SAY_DONT_LIKE, me);
                break;
            case 7:
                DoScriptText(SAY_COMPLETE, me);
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_ESCAPE_FROM_SKETTIS, me);
                break;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        npc_escortAI::UpdateAI(uiDiff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_skyguard_prisoner(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPE_FROM_SKETTIS)
    {
        if (GameObject* pGo = pCreature->FindNearestGameObject(GO_PRISONER_CAGE, 10.0f))
            pGo->UseDoorOrButton();

        if (npc_skyguard_prisonerAI* pEscortAI = CAST_AI(npc_skyguard_prisonerAI, pCreature->AI()))
            pEscortAI->StartEvent(pPlayer, pQuest);
    }
    return true;
}

CreatureAI* GetAI_npc_skyguard_prisoner(Creature* pCreature)
{
    return new npc_skyguard_prisonerAI(pCreature);
}

/*######
## npc_darkscreecher_akarai
######*/

enum Akkarai
{
	NPC_AKKARAI_HATCHLING = 23206,

	SPELL_AKKARAI_FROSTBOLT = 40429,
	SPELL_FLOCK = 40427,
	SPELL_CURSE = 15730,
	SPELL_BLINK = 38981,

	SAY_SPAWN = -1910085,
	SAY_SUMMON = -1910086
};

struct npc_darkscreecher_akaraiAI : public ScriptedAI
{
	npc_darkscreecher_akaraiAI(Creature* pCreature) : ScriptedAI(pCreature) { }

	void Reset()
	{
		frostbolt_timer = 4500;
		flock_timer = 10000;
		curse_timer = 1000;
		blink_timer = 30000;
	}

	uint32 frostbolt_timer;
	uint32 flock_timer;
	uint32 curse_timer;
	uint32 blink_timer;

	void EnterCombat(Unit* /*who*/)
	{
		DoScriptText(SAY_SPAWN, me);
	}

	void UpdateAI(const uint32 diff) override
	{
		if (!UpdateVictim())
			return;

		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (frostbolt_timer <= diff)
		{
			DoCastVictim(SPELL_AKKARAI_FROSTBOLT);
			frostbolt_timer = urand(5000, 8500);
		}
		else frostbolt_timer -= diff;

		if (curse_timer <= diff)
		{
			DoCastVictim(SPELL_CURSE);
			curse_timer = 25000;
		}
		else curse_timer -= diff;

		if (flock_timer <= diff)
		{
			DoCast(SPELL_FLOCK);
			flock_timer = 12000;

			DoScriptText(SAY_SUMMON, me);

			me->SummonCreature(NPC_AKKARAI_HATCHLING, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0, TEMPSUMMON_TIMED_DESPAWN, 30000);
			me->SummonCreature(NPC_AKKARAI_HATCHLING, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0, TEMPSUMMON_TIMED_DESPAWN, 30000);
			me->SummonCreature(NPC_AKKARAI_HATCHLING, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0, TEMPSUMMON_TIMED_DESPAWN, 30000);
			me->SummonCreature(NPC_AKKARAI_HATCHLING, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0, TEMPSUMMON_TIMED_DESPAWN, 30000);
			me->SummonCreature(NPC_AKKARAI_HATCHLING, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0, TEMPSUMMON_TIMED_DESPAWN, 30000);
			me->SummonCreature(NPC_AKKARAI_HATCHLING, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0.0, TEMPSUMMON_TIMED_DESPAWN, 30000);
		}
		else flock_timer -= diff;

		if (blink_timer <= diff && HealthBelowPct(80))
		{
			DoCast(SPELL_BLINK);
			blink_timer = 15000;
		}
		else blink_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_darkscreecher_akarai(Creature* pCreature)
{
	return new npc_darkscreecher_akaraiAI(pCreature);
}

/*######
## npc_private_weeks
######*/

#define GOSSIP_ITEM1_DISGUISE "Private Weeks, I need another disguise."

bool QuestAccept_npc_private_weeks(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
	if (quest->GetQuestId() == 10040)
	{
		pPlayer->CastSpell(pPlayer, 32756, true);            // Give Disguise buff
		pPlayer->CastSpell(pPlayer, pPlayer->getGender() == GENDER_FEMALE ? 38081 : 38080, true); // Give Disguise
	}

	return true;
}

bool ChooseReward_npc_private_weeks(Player* pPlayer, Creature* pCreature, const Quest* _Quest, uint32 /*item*/)
{
	if (_Quest->GetQuestId() == 10040)
	{
		if (pPlayer->HasAura(32756, 0))
			pPlayer->RemoveAurasDueToSpell(32756);

		if (pPlayer->HasAura(38080, 0))
			pPlayer->RemoveAurasDueToSpell(38080);

		if (pPlayer->HasAura(38081, 0))
			pPlayer->RemoveAurasDueToSpell(38081);
	}

	return false;
}

bool GossipHello_npc_private_weeks(Player* player, Creature* pCreature)
{
	if (pCreature->IsQuestGiver())
		player->PrepareQuestMenu(pCreature->GetGUID());

	// Give Another Disguise
	if (player->GetQuestStatus(10040) == QUEST_STATUS_INCOMPLETE)
		player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM1_DISGUISE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

	player->SEND_GOSSIP_MENU(9517, pCreature->GetGUID());

	return true;
}

bool GossipSelect_npc_private_weeks(Player* player, Creature* /*pCreature*/, uint32 /*sender*/, uint32 action)
{
	if (action == GOSSIP_ACTION_INFO_DEF + 1)
	{
		player->CLOSE_GOSSIP_MENU();
		player->CastSpell(player, 32756, true);            // Give Another Disguise
		player->CastSpell(player, player->getGender() == GENDER_FEMALE ? 38081 : 38080, true); // Give Disguise
	}

	return true;
}

/*######
## npc_scout_nefris
######*/

#define GOSSIP_ITEM2_DISGUISE "Scout Nefris, I need another disguise."

bool QuestAccept_npc_scout_nefris(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
	if (quest->GetQuestId() == 10041)
	{
		pPlayer->CastSpell(pPlayer, 32756, true);            // Give Disguise buff
		pPlayer->CastSpell(pPlayer, pPlayer->getGender() == GENDER_FEMALE ? 38081 : 38080, true); // Give Disguise
	}

	return true;
}

bool ChooseReward_npc_scout_nefris(Player* pPlayer, Creature* pCreature, const Quest* _Quest, uint32 /*item*/)
{
	if (_Quest->GetQuestId() == 10041)
	{
		if (pPlayer->HasAura(32756, 0))
			pPlayer->RemoveAurasDueToSpell(32756);

		if (pPlayer->HasAura(38080, 0))
			pPlayer->RemoveAurasDueToSpell(38080);

		if (pPlayer->HasAura(38081, 0))
			pPlayer->RemoveAurasDueToSpell(38081);
	}

	return false;
}

bool GossipHello_npc_scout_nefris(Player* player, Creature* pCreature)
{
	if (pCreature->IsQuestGiver())
		player->PrepareQuestMenu(pCreature->GetGUID());

	// Give Another Disguise
	if (player->GetQuestStatus(10041) == QUEST_STATUS_INCOMPLETE)
		player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM2_DISGUISE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

	player->SEND_GOSSIP_MENU(9520, pCreature->GetGUID());

	return true;
}

bool GossipSelect_npc_scout_nefris(Player* player, Creature* /*pCreature*/, uint32 /*sender*/, uint32 action)
{
	if (action == GOSSIP_ACTION_INFO_DEF + 1)
	{
		player->CLOSE_GOSSIP_MENU();
		player->CastSpell(player, 32756, true);            // Give Another Disguise
		player->CastSpell(player, player->getGender() == GENDER_FEMALE ? 38081 : 38080, true); // Give Disguise
	}

	return true;
}

// Executioner Gossip
bool GossipHello_npc_shadowy_executioner(Player* player, Creature* pCreature)
{
	if (player->GetQuestStatus(10040) == QUEST_STATUS_INCOMPLETE)
		player->SEND_GOSSIP_MENU(23019, pCreature->GetGUID());

	if (player->GetQuestStatus(10041) == QUEST_STATUS_INCOMPLETE)
		player->SEND_GOSSIP_MENU(23019, pCreature->GetGUID());

	return true;
}

//Laborer Gossip

#define GOSSIP_LABORER "Why are you fixing all of this up?"

bool GossipHello_npc_shadowy_laborer(Player* player, Creature* pCreature)
{
	if (player->GetQuestStatus(10040) == QUEST_STATUS_INCOMPLETE)
		player->ADD_GOSSIP_ITEM(0, GOSSIP_LABORER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->SEND_GOSSIP_MENU(9497, pCreature->GetGUID());

	if (player->GetQuestStatus(10041) == QUEST_STATUS_INCOMPLETE)
		player->ADD_GOSSIP_ITEM(0, GOSSIP_LABORER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->SEND_GOSSIP_MENU(9497, pCreature->GetGUID());

	return true;
}

bool GossipSelect_npc_shadowy_laborer(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
	if (action == GOSSIP_ACTION_INFO_DEF + 1)
	{
		if (Creature* laborer = pCreature->FindNearestCreature(18717, 5, true))
		{
			player->KilledMonsterCredit(18717, laborer->GetGUID());
		}

		player->SEND_GOSSIP_MENU(23020, pCreature->GetGUID());
	}

	return true;
}

//Shadowy Summoner

#define SPELL_SUMMON_SUCUBBUS  8722
#define SPELL_SCORCH		   13878

struct npc_shadowy_summonerAI : public ScriptedAI
{
	npc_shadowy_summonerAI(Creature* pCreature) : ScriptedAI(pCreature) { }

	void Reset()
	{
		scorch_timer = 4500;
		
		DoCast(SPELL_SUMMON_SUCUBBUS);
	}

	uint32 scorch_timer;

	void EnterCombat(Unit* /*who*/) { }

	void UpdateAI(const uint32 diff) override
	{
		if (!UpdateVictim())
			return;

		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (scorch_timer <= diff)
		{
			DoCastVictim(SPELL_SCORCH);
			scorch_timer = urand(5000, 8500);
		}
		else scorch_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_shadowy_summoner(Creature* pCreature)
{
	return new npc_shadowy_summonerAI(pCreature);
}

bool GossipHello_npc_shadowy_summoner(Player* player, Creature* pCreature)
{
	if (player->GetQuestStatus(10040) == QUEST_STATUS_INCOMPLETE)
		player->SEND_GOSSIP_MENU(23021, pCreature->GetGUID());

	if (player->GetQuestStatus(10041) == QUEST_STATUS_INCOMPLETE)
		player->SEND_GOSSIP_MENU(23021, pCreature->GetGUID());

	return true;
}

//Shadowy Advisor

#define GOSSIP_ADVISOR "Advisor, what's the lastest news?"

#define SPELL_ADVSHADOWBOLT 9613

struct npc_shadowy_advisorAI : public ScriptedAI
{
	npc_shadowy_advisorAI(Creature* pCreature) : ScriptedAI(pCreature) { }

	void Reset()
	{
		bolt_timer = 4500;
	}

	uint32 bolt_timer;

	void EnterCombat(Unit* /*who*/) { }

	void UpdateAI(const uint32 diff) override
	{
		if (!UpdateVictim())
			return;

		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (bolt_timer <= diff)
		{
			DoCastVictim(SPELL_ADVSHADOWBOLT);
			bolt_timer = urand(5000, 8500);
		}
		else bolt_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_shadowy_advisor(Creature* pCreature)
{
	return new npc_shadowy_advisorAI(pCreature);
}

bool GossipHello_npc_shadowy_advisor(Player* player, Creature* pCreature)
{
	if (player->GetQuestStatus(10040) == QUEST_STATUS_INCOMPLETE)
		player->ADD_GOSSIP_ITEM(0, GOSSIP_ADVISOR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->SEND_GOSSIP_MENU(9505, pCreature->GetGUID());

	if (player->GetQuestStatus(10041) == QUEST_STATUS_INCOMPLETE)
		player->ADD_GOSSIP_ITEM(0, GOSSIP_ADVISOR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->SEND_GOSSIP_MENU(9505, pCreature->GetGUID());

	return true;
}

bool GossipSelect_npc_shadowy_advisor(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
	if (action == GOSSIP_ACTION_INFO_DEF + 1)
	{
		if (Creature* advisor = pCreature->FindNearestCreature(18719, 5, true))
		{
			player->KilledMonsterCredit(18719, advisor->GetGUID());
		}

		player->SEND_GOSSIP_MENU(9506, pCreature->GetGUID());
	}

	return true;
}

//Shadowy Initiate

#define GOSSIP_INITIATE "What are you doing here?"

#define SPELL_ARCANECHANNEL 23017

struct npc_shadowy_initiateAI : public ScriptedAI
{
	npc_shadowy_initiateAI(Creature* pCreature) : ScriptedAI(pCreature) { }

	void Reset()
	{
		bolt_timer = 4500;

		DoCast(SPELL_ARCANECHANNEL);
	}

	uint32 bolt_timer;

	void EnterCombat(Unit* /*who*/) { }

	void UpdateAI(const uint32 diff) override
	{
		if (!UpdateVictim())
			return;

		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (bolt_timer <= diff)
		{
			DoCastVictim(SPELL_ADVSHADOWBOLT);
			bolt_timer = urand(5000, 8500);
		}
		else bolt_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_shadowy_initiate(Creature* pCreature)
{
	return new npc_shadowy_initiateAI(pCreature);
}

bool GossipHello_npc_shadowy_initiate(Player* player, Creature* pCreature)
{
	if (player->GetQuestStatus(10040) == QUEST_STATUS_INCOMPLETE)
		player->ADD_GOSSIP_ITEM(0, GOSSIP_INITIATE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
	player->SEND_GOSSIP_MENU(9502, pCreature->GetGUID());

	if (player->GetQuestStatus(10041) == QUEST_STATUS_INCOMPLETE)
		player->ADD_GOSSIP_ITEM(0, GOSSIP_INITIATE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
	player->SEND_GOSSIP_MENU(9502, pCreature->GetGUID());

	return true;
}

bool GossipSelect_npc_shadowy_initiate(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
	if (action == GOSSIP_ACTION_INFO_DEF + 1)
	{
		if (Creature* initiate = pCreature->FindNearestCreature(18716, 5, true))
		{
			player->KilledMonsterCredit(18716, initiate->GetGUID());
		}

		player->SEND_GOSSIP_MENU(9503, pCreature->GetGUID());
	}

	return true;
}

//Empoor 

#define SPELL_LIGHTNING_SHIELD  12550
#define SPELL_FROST_SHOCK		12548

#define SAY_MEANING			-1910152

#define GOSSIP_EMPOOR "Empoor, you are going  to tell me what I want to know, or else!"

struct npc_empoorAI : public ScriptedAI
{
	npc_empoorAI(Creature* pCreature) : ScriptedAI(pCreature) { }

	void Reset()
	{
		me->SetFaction(35);
		me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

		if (Creature* guard = me->FindNearestCreature(18483, 15, true))
		{
			guard->SetFaction(35);
		}

		reset_timer = 0;
		fschock_timer = 4000;
		lightning_shield_timer = 1000;	
	}

	uint32 reset_timer;
	uint32 fschock_timer;
	uint32 lightning_shield_timer;

	void EnterCombat(Unit* /*who*/) { }

	void DamageTaken(Unit* done_by, uint32& damage)
	{
		if (done_by->GetTypeId() == TYPEID_PLAYER)
			if ((me->GetHealth() - damage) * 100 / me->GetMaxHealth() < 20)
			{
				me->SetFaction(35);
				me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);

				if (Creature* guard = me->FindNearestCreature(18483, 15, true))
				{
					guard->SetFaction(35);
				}

				damage = 0;
				EnterEvadeMode();
			}
	}

	void UpdateAI(const uint32 diff) override
	{
		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (!UpdateVictim())
			return;

		if (fschock_timer <= diff)
		{
			DoCastVictim(SPELL_FROST_SHOCK);
			fschock_timer = urand(8000, 12000);
		}
		else fschock_timer -= diff;

		if (lightning_shield_timer <= diff)
		{
			DoCast(me, SPELL_LIGHTNING_SHIELD);
			lightning_shield_timer = 25000;
		}
		else lightning_shield_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_empoor(Creature* pCreature)
{
	return new npc_empoorAI(pCreature);
}

bool GossipHello_npc_empoor(Player* player, Creature* pCreature)
{
	if (pCreature->IsQuestGiver())
		player->PrepareQuestMenu(pCreature->GetGUID());

	if (player->GetQuestStatus(9978) == QUEST_STATUS_COMPLETE)
		player->ADD_GOSSIP_ITEM(0, GOSSIP_EMPOOR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
	player->SEND_GOSSIP_MENU(9405, pCreature->GetGUID());

	return true;
}

bool GossipSelect_npc_empoor(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
	if (action == GOSSIP_ACTION_INFO_DEF + 1)
	{
		player->CLOSE_GOSSIP_MENU();
		DoScriptText(SAY_MEANING, pCreature, player);
		pCreature->SetFaction(14);

		((npc_empoorAI*)pCreature->AI())->AttackStart(player);

		if (Creature* guard = pCreature->FindNearestCreature(18483, 15, true))
		{
			guard->SetFaction(14);
		}
	}

	return true;
}

/*######
## npc_pathaleon_image
######*/

enum panthimage
{
	SAY_PATHALEON1 = -1910154,
	SAY_PATHALEON2 = -1910157,
	SAY_PATHALEON3 = -1910158,
	SAY_PATHALEON4 = -1910159,
	SAY_PATHALEON5 = -1910161,

	SAY_PLAYER_ENTER = -1910163,
	
	SPELL_TELE = 7741,
	
	NPC_VOLDOUN = 18554,
};

struct npc_pathaleon_image2AI : public ScriptedAI
{
	npc_pathaleon_image2AI(Creature* pCreature) : ScriptedAI(pCreature) {}

	bool Event;

	uint32 uiSumTimer;
	uint32 uiStepsTimer;
	uint32 uiSteps;

	void Reset()
	{
		uiSumTimer = 5000;
		uiStepsTimer = 0;
		uiSteps = 0;
		Event = false;
	}

	int32 NextStep(uint32 uiSteps)
	{
		switch (uiSteps)
		{
		case 1:
			me->SetVisible(true);
			return 5000;
		case 2:
			DoScriptText(SAY_PATHALEON1, me, 0);
			return 12000;
		case 3:
			DoScriptText(SAY_PATHALEON2, me, 0);
			return 8500;
		case 4:
			DoScriptText(SAY_PATHALEON3, me, 0);
			return 8500;
		case 5:
			DoScriptText(SAY_PATHALEON4, me, 0);
			return 12000;
		case 6:
			DoScriptText(SAY_PATHALEON5, me, 0);
			return 300000;
		case 7:
			DoScriptText(SAY_PLAYER_ENTER, me, 0);
			return 4000;
		case 8:
			DoCast(SPELL_TELE);
			return 1000;
		case 9:		
			me->DisappearAndDie(false);
		default:
			return 0;
		}
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (Creature* vold = me->FindNearestCreature(NPC_VOLDOUN, 10.0f, true))
		{
			Event = true;
		}
		else
			Event = false;

		if (uiStepsTimer <= uiDiff)
		{
			if (Event)
				uiStepsTimer = NextStep(++uiSteps);
		}
		else uiStepsTimer -= uiDiff;
		
	}
};

CreatureAI* GetAI_npc_pathaleon_image2(Creature* pCreature)
{
	return new npc_pathaleon_image2AI(pCreature);
}

enum voldconv
{
	SAY_VOLD_INTRO = -1910153,
	SAY_VOLD1 = -1910155,
	SAY_VOLD2 = -1910156,
	SAY_VOLD3 = -1910160,

	SAY_AGGRO = -1910162,

	SAY_PLAYER_ENTER2 = -1910163,

	NPC_IMAGE = 31000,

	SPELL_HL = 13952,
	SPELL_SEALHEAL = 15277,
	SPELL_JUDGMENT = 32917
};

struct npc_voldounAI : public ScriptedAI
{
	npc_voldounAI(Creature* pCreature) : ScriptedAI(pCreature) { }

	void Reset()
	{
		me->SetStandState(UNIT_STAND_STATE_STAND);

		intro = false;
		SayLine1 = false;
		SayLine2 = false;
		SayLine3 = false;
		conversation_timer = 0;
		conv_timer = 0;
		conv_timer2 = 0;

		holylight_timer = 15000;
		sealheal_timer = 2000;
		judgement_timer = 11000;
	}

	bool intro;
	bool SayLine1;
	bool SayLine2;
	bool SayLine3;
	uint32 conversation_timer;
	uint32 conv_timer;
	uint32 conv_timer2;
	uint32 holylight_timer;
	uint32 sealheal_timer;
	uint32 judgement_timer;

	void EnterCombat(Unit* /*who*/) 
	{
		DoScriptText(SAY_AGGRO, me);

		if (Creature* image = me->FindNearestCreature(NPC_IMAGE, 20.0f, true))
		{
			DoScriptText(SAY_PLAYER_ENTER, image, me);
			image->DisappearAndDie(false);
		}
	}

	void UpdateAI(const uint32 diff) override
	{
		if (!UpdateVictim())
		{
			if (!intro)
			{
				if (Creature* image = me->FindNearestCreature(NPC_IMAGE, 10.0f, true))
				{
					DoScriptText(SAY_VOLD_INTRO, me, image);					
					me->SetStandState(UNIT_STAND_STATE_KNEEL);
					intro = true;
					conversation_timer = 10000;
				}
			}

			if (conversation_timer <= diff && SayLine1 == false && intro == true)
			{
				DoScriptText(SAY_VOLD1, me);
				SayLine1 = true;
				me->SetStandState(UNIT_STAND_STATE_STAND);
				conv_timer = 8000;
			}
			else conversation_timer -= diff;

			if (conv_timer <= diff && SayLine2 == false && intro == true)
			{
				DoScriptText(SAY_VOLD2, me);
				SayLine2 = true;
				conv_timer2 = 20000;
			}
			else conv_timer -= diff;

			if (conv_timer2 <= diff && SayLine3 == false && intro == true)
			{
				DoScriptText(SAY_VOLD3, me);
				SayLine3 = true;
			}
			else conv_timer2 -= diff;
		}

		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (UpdateVictim())
		{
			if (holylight_timer <= diff)
			{
				DoCast(me, SPELL_HL);
				holylight_timer = 11000;
			}
			else holylight_timer -= diff;

			if (sealheal_timer <= diff)
			{
				DoCast(me, SPELL_SEALHEAL);
				sealheal_timer = 25000;;
			}
			else sealheal_timer -= diff;

			if (judgement_timer <= diff)
			{
				DoCastVictim(SPELL_JUDGMENT);
				judgement_timer = 15000;;
			}
			else judgement_timer -= diff;
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_voldoun(Creature* pCreature)
{
	return new npc_voldounAI(pCreature);
}

struct mob_mountain_colossusAI : public ScriptedAI
{
	mob_mountain_colossusAI(Creature* pCreature) : ScriptedAI(pCreature) { }

	void Reset()
	{
		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
	}

	void EnterCombat(Unit* /*who*/) { }

	void UpdateAI(const uint32 diff) override
	{
		if (!UpdateVictim())
			return;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_mountain_colossus(Creature* pCreature)
{
	return new mob_mountain_colossusAI(pCreature);
}

/*######
## npc_hungry_nether_ray
######*/

enum
{
	NPC_BLACKWING_WARP_CHASER = 23219,
	QUEST_HUNGRY_NETHER_RAYS = 11093
};

struct npc_hungry_nether_rayAI : public ScriptedAI
{
	npc_hungry_nether_rayAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	uint32 uiCheckTimer;
	uint64 uiWarpGUID;

	void Reset()
	{
		uiCheckTimer = 4000;
		uiWarpGUID = 0;
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (uiCheckTimer <= uiDiff)
		{
			if (Creature* pWarp = me->FindNearestCreature(NPC_BLACKWING_WARP_CHASER, 9.0f, false))
			{
				if (pWarp->GetGUID() != uiWarpGUID && CAST_PLR(me->GetOwner())->GetQuestStatus(QUEST_HUNGRY_NETHER_RAYS) == QUEST_STATUS_INCOMPLETE)
				{
					uiWarpGUID = pWarp->GetGUID();
					me->HandleEmoteCommand(ANIM_EMOTE_EAT);
				}
			}
			uiCheckTimer = 4000;
		}
		else uiCheckTimer -= uiDiff;
	}
};

CreatureAI* GetAI_npc_hungry_nether_ray(Creature* pCreature)
{
	return new npc_hungry_nether_rayAI(pCreature);
}


/*######
## go_ancient_skull_pile
######*/
#define GOSSIP_S_TEROKK   "<Call Forth Terokk.>"
#define ITEM_OFFERING	  32720
#define NPC_TEROKK        21838
#define QUEST_TEROKK	  11073

bool GossipHello_go_ancient_skull_pile(Player *player, GameObject* _GO)
{
	if (player->GetQuestStatus(QUEST_TEROKK) == QUEST_STATUS_INCOMPLETE)
	{
		player->ADD_GOSSIP_ITEM(0, GOSSIP_S_TEROKK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
	}

	player->SEND_GOSSIP_MENU(921062, _GO->GetGUID());
	return true;
}

void SendActionMenu_go_ancient_skull_pile(Player *player, GameObject* _GO, uint32 action)
{
	switch (action)
	{
	case GOSSIP_ACTION_INFO_DEF + 1:
		if (player->HasItemCount(ITEM_OFFERING, 1))
		{
			player->DestroyItemCount(ITEM_OFFERING, 1, true);
			player->SummonCreature(NPC_TEROKK, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
		}
		break;
	}
}

bool GossipSelect_go_ancient_skull_pile(Player *player, GameObject* _GO, uint32 sender, uint32 action)
{
	switch (sender)
	{
	case GOSSIP_SENDER_MAIN:    SendActionMenu_go_ancient_skull_pile(player, _GO, action); break;
	}
	return true;
}

enum Terokk
{
	SPELL_WILL_OF_THE_ARRAKOA_GOD = 40722,
	SPELL_ENRAGE = 20747,
	SPELL_DIVINE_SHIELD = 40733,
	SPELL_CLEAVE = 15284,
	SPELL_SHADOWBOLT_VOLLEY = 40721,
	SPELL_CHOSEN_ONE = 40726,

	EMOTE_ENRAGE = -1910164,
	SAY_SHIELDED = -1910165,
	SAY_CHOSEN_ONE = -1910166,
	SAY_SUMMONED = -1910167,

	SAY_VICTORY = -1910168,
	YELL_FORMATION = -1910169,
	SAY_BOMBING = -1910170,
	
	ACE_PATH = 599599507,

	NPC_ACE = 23377,
	NPC_TEROKK_TRIGGER = 61001,

	SPELL_BOMB = 40657,
	SPELL_FLARE = 40655
};

struct npc_skyguard_aceAI : public ScriptedAI
{
	npc_skyguard_aceAI(Creature* pCreature) : ScriptedAI(pCreature) { }

	void Reset()
	{
		me->SetCanFly(true);
		me->SetLevitate(true);
		me->SetHover(true);
		me->GetMotionMaster()->MovePath(ACE_PATH, true);
	}

	void Bombing()
	{
		if (Creature* BombTrigger = me->FindNearestCreature(NPC_TEROKK_TRIGGER, 150.0f, true))
			DoCast(BombTrigger, SPELL_BOMB);
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;
	}
};

CreatureAI* GetAI_npc_skyguard_ace(Creature* pCreature)
{
	return new npc_skyguard_aceAI(pCreature);
}

struct npc_terokkAI : public ScriptedAI
{
	npc_terokkAI(Creature* pCreature) : ScriptedAI(pCreature) { }

	uint32 cleave_timer;
	uint32 shadowbolt_volley_timer;
	uint32 ChosenOne_timer;
	uint32 summon_timer;
	uint32 bomb_timer;

	//bool enrage;
	bool spellHit;
	bool SayShield;

	bool shielded;
	bool shielded2;
	bool shielded3;

	bool bombed1;
	bool bombed2;
	bool bombed3;

	bool YellFormation;

	void Reset()
	{
		cleave_timer = 6000;
		shadowbolt_volley_timer = 10000;
		ChosenOne_timer = 20000;
		summon_timer = 500;
		bomb_timer = 0;
		
		//enrage = false;
		SayShield = false;
		spellHit = false;

		shielded = false;
		shielded2 = false;
		shielded3 = false;

		bombed1 = false;
		bombed2 = false;
		bombed3 = false;

		YellFormation = false;
	}

	void EnterCombat(Unit* /*who*/) 
	{
		DoScriptText(SAY_SUMMONED, me);
	}

	void SpellHit(Unit* Hitter, const SpellEntry* Spellkind)
	{
		if (Spellkind->Id == 40657 && !spellHit)
		{
			if (me->HasAura(SPELL_BOMB, 0))
				me->RemoveAurasDueToSpell(SPELL_DIVINE_SHIELD);
			DoCast(me, SPELL_ENRAGE);
			DoScriptText(EMOTE_ENRAGE, me);
			spellHit = true;

		}
	}

	void JustDied(Unit* /*victim*/)
	{
		DoCast(me, SPELL_WILL_OF_THE_ARRAKOA_GOD);

		if (Creature* ace = me->FindNearestCreature(NPC_ACE, 100.0f, true))
		{
			DoScriptText(SAY_VICTORY, ace);
			ace->DisappearAndDie(false);
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (ChosenOne_timer <= diff)
		{
			if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 40, true))
			{
				DoCast(pTarget, SPELL_CHOSEN_ONE);
				DoScriptText(SAY_CHOSEN_ONE, me, pTarget);
				ChosenOne_timer = 45000;
			}
		}
		else ChosenOne_timer -= diff;

		if (cleave_timer <= diff)
		{
			DoCastVictim(SPELL_CLEAVE);
			cleave_timer = 6000;
		}
		else cleave_timer -= diff;

		if (shadowbolt_volley_timer <= diff)
		{
			DoCastVictim(SPELL_SHADOWBOLT_VOLLEY);
			shadowbolt_volley_timer = 10000;
		}
		else shadowbolt_volley_timer -= diff;

		if (HealthBelowPct(50) && YellFormation == false)
		{
			me->SummonCreature(NPC_ACE, -3787.21f, 3503.76f, 324.43f, 2.24f, TEMPSUMMON_MANUAL_DESPAWN, 0);

			if (Creature* ace = me->FindNearestCreature(NPC_ACE, 150.0f, true))
			{
				DoScriptText(YELL_FORMATION, ace);
				YellFormation = true;
			}
		}

		if (HealthBelowPct(30) && shielded == false && SayShield == false)
		{
			DoCast(me, SPELL_DIVINE_SHIELD);
			DoScriptText(SAY_SHIELDED, me);
			bomb_timer = 25000;
			shielded = true;
			SayShield = true;

			if (Creature* ace = me->FindNearestCreature(NPC_ACE, 150.0f, true))
			{
				DoScriptText(SAY_BOMBING, ace);
			}
		}

		if (HealthBelowPct(20) && shielded2 == false)
		{
			DoCast(me, SPELL_DIVINE_SHIELD);
			shielded2 = true;
			spellHit = false;
			bomb_timer = 25000;
		}

		if (HealthBelowPct(10) && shielded3 == false)
		{
			DoCast(me, SPELL_DIVINE_SHIELD);
			shielded3 = true;
			spellHit = false;
			bomb_timer = 25000;
		}

		if (shielded == true && bombed1 == false && bomb_timer <= diff)
		{
			if (Creature* ace = me->FindNearestCreature(NPC_ACE, 150.0f, true))
			{
				CAST_AI(npc_skyguard_aceAI, me->AI())->Bombing();
				bombed1 = true;
			}
		}
		else bomb_timer -= diff;

		if (shielded2 == true && bombed2 == false && bomb_timer <= diff)
		{
			if (Creature* ace = me->FindNearestCreature(NPC_ACE, 150.0f, true))
			{
				CAST_AI(npc_skyguard_aceAI, me->AI())->Bombing();
				bombed2 = true;
			}
		}
		else bomb_timer -= diff;

		if (shielded3 == true && bombed3 == false && bomb_timer <= diff)
		{
			if (Creature* ace = me->FindNearestCreature(NPC_ACE, 150.0f, true))
			{
				CAST_AI(npc_skyguard_aceAI, me->AI())->Bombing();
				bombed3 = true;
			}
		}
		else bomb_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_terokk(Creature* pCreature)
{
	return new npc_terokkAI(pCreature);
}

/*######
## QUEST_FIRES_OVER_SKETTIS (11007)
######*/

struct npc_kalliri_triggerAI : public ScriptedAI
{
    npc_kalliri_triggerAI(Creature* c) : ScriptedAI(c) { }

    bool spellHit;

    void Reset()
    {
        spellHit = false;
    }

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (caster->GetTypeId() == TYPEID_PLAYER && spell->Id == 39844 && !spellHit)
        {
            if (Player* plWho = caster->GetCharmerOrOwnerPlayerOrPlayerItself())
            {
                plWho->KilledMonsterCredit(22991);
                me->DisappearAndDie();

                if (GameObject* egg = me->FindNearestGameObject(185549, 2.0f))
                {
                    egg->SetGoState(GO_STATE_ACTIVE);
                }

                spellHit = true;
            }
        }
    }
};

CreatureAI* GetAI_npc_kalliri_trigger(Creature* pCreature)
{
    return new npc_kalliri_triggerAI(pCreature);
}

void AddSC_terokkar_forest()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "mob_unkor_the_ruthless";
    newscript->GetAI = &GetAI_mob_unkor_the_ruthless;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_infested_root_walker";
    newscript->GetAI = &GetAI_mob_infested_root_walker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_rotting_forest_rager";
    newscript->GetAI = &GetAI_mob_rotting_forest_rager;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_netherweb_victim";
    newscript->GetAI = &GetAI_mob_netherweb_victim;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_floon";
    newscript->pGossipHello =  &GossipHello_npc_floon;
    newscript->pGossipSelect = &GossipSelect_npc_floon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_isla_starmane";
    newscript->GetAI = &GetAI_npc_isla_starmaneAI;
    newscript->pQuestAccept = &QuestAccept_npc_isla_starmane;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_skull_pile";
    newscript->pGOHello  = &GossipHello_go_skull_pile;
    newscript->pGOSelect = &GossipSelect_go_skull_pile;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_skywing";
    newscript->GetAI = &GetAI_npc_skywingAI;
    newscript->pQuestAccept = &QuestAccept_npc_skywing;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_slim";
    newscript->pGossipHello =  &GossipHello_npc_slim;
    newscript->pGossipSelect = &GossipSelect_npc_slim;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_akuno";
    newscript->GetAI = &GetAI_npc_akuno;
    newscript->pQuestAccept = &QuestAccept_npc_akuno;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_letoll";
    newscript->GetAI = &GetAI_npc_letoll;
    newscript->pQuestAccept = &QuestAccept_npc_letoll;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_mana_bomb_exp_trigger";
    newscript->GetAI = &GetAI_npc_mana_bomb_exp_trigger;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_mana_bomb";
    newscript->pGOHello = &GOHello_go_mana_bomb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_skyguard_handler_deesak";
    newscript->pGossipHello =  &GossipHello_npc_skyguard_handler_deesak;
    newscript->pGossipSelect = &GossipSelect_npc_skyguard_handler_deesak;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_veil_skith_cage";
    newscript->pGOHello =  &GOHello_veil_skith_cage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_captive_child";
    newscript->GetAI = &GetAI_npc_captive_child;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_skyguard_prisoner";
    newscript->GetAI = &GetAI_npc_skyguard_prisoner;
    newscript->pQuestAccept = &QuestAccept_npc_skyguard_prisoner;
    newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_darkscreecher_akarai";
	newscript->GetAI = &GetAI_npc_darkscreecher_akarai;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_private_weeks";
	newscript->pGossipHello = &GossipHello_npc_private_weeks;
	newscript->pGossipSelect = &GossipSelect_npc_private_weeks;
	newscript->pQuestAccept = &QuestAccept_npc_private_weeks;
	newscript->pChooseReward = &ChooseReward_npc_private_weeks;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_scout_nefris";
	newscript->pGossipHello = &GossipHello_npc_scout_nefris;
	newscript->pGossipSelect = &GossipSelect_npc_scout_nefris;
	newscript->pQuestAccept = &QuestAccept_npc_scout_nefris;
	newscript->pChooseReward = &ChooseReward_npc_scout_nefris;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_shadowy_executioner";
	newscript->pGossipHello = &GossipHello_npc_shadowy_executioner;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_shadowy_laborer";
	newscript->pGossipHello = &GossipHello_npc_shadowy_laborer;
	newscript->pGossipSelect = &GossipSelect_npc_shadowy_laborer;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_shadowy_summoner";
	newscript->GetAI = &GetAI_npc_shadowy_summoner;
	newscript->pGossipHello = &GossipHello_npc_shadowy_summoner;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_shadowy_advisor";
	newscript->GetAI = &GetAI_npc_shadowy_advisor;
	newscript->pGossipHello = &GossipHello_npc_shadowy_advisor;
	newscript->pGossipSelect = &GossipSelect_npc_shadowy_advisor;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_shadowy_initiate";
	newscript->GetAI = &GetAI_npc_shadowy_initiate;
	newscript->pGossipHello = &GossipHello_npc_shadowy_initiate;
	newscript->pGossipSelect = &GossipSelect_npc_shadowy_initiate;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_empoor";
	newscript->GetAI = &GetAI_npc_empoor;
	newscript->pGossipHello = &GossipHello_npc_empoor;
	newscript->pGossipSelect = &GossipSelect_npc_empoor;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_pathaleon_image2";
	newscript->GetAI = &GetAI_npc_pathaleon_image2;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_voldoun";
	newscript->GetAI = &GetAI_npc_voldoun;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "mob_mountain_colossus";
	newscript->GetAI = &GetAI_mob_mountain_colossus;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "go_ancient_skull_pile";
	newscript->pGOHello = &GossipHello_go_ancient_skull_pile;
	newscript->pGOSelect = &GossipSelect_go_ancient_skull_pile;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_terokk";
	newscript->GetAI = &GetAI_npc_terokk;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_skyguard_ace";
	newscript->GetAI = &GetAI_npc_skyguard_ace;
	newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_kalliri_trigger";
    newscript->GetAI = &GetAI_npc_kalliri_trigger;
    newscript->RegisterSelf();
}

