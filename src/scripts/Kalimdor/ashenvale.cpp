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
SDName: Ashenvale
SD%Complete: 70
SDComment: Quest support: 6544, 6482, 6641
SDCategory: Ashenvale Forest
EndScriptData */

/* ContentData
npc_muglash
npc_torek
npc_ruul_snowhoof
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"

/*####
# npc_muglash
####*/

enum
{
    SAY_MUG_START1          = -1000501,
    SAY_MUG_START2          = -1000502,
    SAY_MUG_BRAZIER         = -1000503,
    SAY_MUG_BRAZIER_WAIT    = -1000504,
    SAY_MUG_ON_GUARD        = -1000505,
    SAY_MUG_REST            = -1000506,
    SAY_MUG_DONE            = -1000507,
    SAY_MUG_GRATITUDE       = -1000508,
    SAY_MUG_PATROL          = -1000509,
    SAY_MUG_RETURN          = -1000510,

    QUEST_VORSHA            = 6641,

    GO_NAGA_BRAZIER         = 178247,
    NPC_MUGLASH             = 12717,

    NPC_WRATH_RIDER         = 3713,
    NPC_WRATH_SORCERESS     = 3717,
    NPC_WRATH_RAZORTAIL     = 3712,

    NPC_WRATH_PRIESTESS     = 3944,
    NPC_WRATH_MYRMIDON      = 3711,
    NPC_WRATH_SEAWITCH      = 3715,

    NPC_VORSHA              = 12940
};

static float m_afFirstNagaCoord[3][3] =
{
    {3603.504150f, 1122.631104f, 1.635f},                   // rider
    {3589.293945f, 1148.664063f, 5.565f},                   // sorceress
    {3609.925537f, 1168.759521f, -1.168f}                   // razortail
};

static float m_afSecondNagaCoord[3][3] =
{
    {3609.925537f, 1168.759521f, -1.168f},                  // witch
    {3645.652100f, 1139.425415f, 1.322f},                   // priest
    {3583.602051f, 1128.405762f, 2.347f}                    // myrmidon
};

static float m_fVorshaCoord[] = {3633.056885f, 1172.924072f, -5.388f};

struct npc_muglashAI : public npc_escortAI
{
    npc_muglashAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_uiWaveId = 0;
        m_bIsBrazierExtinguished = false;
        Reset();
    }

    bool m_bIsBrazierExtinguished;

    uint32 m_uiWaveId;
    uint32 m_uiEventTimer;

    void Reset()
    {
        m_uiEventTimer = 10000;

        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_uiWaveId = 0;
            m_bIsBrazierExtinguished = false;
        }
    }

    void Aggro(Unit* /*pWho*/)
    {
        if (HasEscortState(STATE_ESCORT_PAUSED))
        {
            if (urand(0, 1))
                return;

            if (Player* pPlayer = GetPlayerForEscort())
                DoScriptText(SAY_MUG_ON_GUARD, me, pPlayer);
        }
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
        case 0:
            if (Player* pPlayer = GetPlayerForEscort())
                DoScriptText(SAY_MUG_START2, me, pPlayer);
            break;
        case 24:
            if (Player* pPlayer = GetPlayerForEscort())
                DoScriptText(SAY_MUG_BRAZIER, me, pPlayer);

            if (GameObject* pGo = me->FindNearestGameObject(GO_NAGA_BRAZIER, INTERACTION_DISTANCE * 2))
            {
                //some kind of event flag? Update to player/group only?
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                SetEscortPaused(true);
            }
            break;
        case 25:
            DoScriptText(SAY_MUG_GRATITUDE, me);

            if (Player* pPlayer = GetPlayerForEscort())
                pPlayer->GroupEventHappens(QUEST_VORSHA, me);
            break;
        case 26:
            DoScriptText(SAY_MUG_PATROL, me);
            break;
        case 27:
            DoScriptText(SAY_MUG_RETURN, me);
            break;
        }
    }

    void DoWaveSummon()
    {
        switch (m_uiWaveId)
        {
        case 1:
            me->SummonCreature(NPC_WRATH_RIDER,     m_afFirstNagaCoord[0][0], m_afFirstNagaCoord[0][1], m_afFirstNagaCoord[0][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
            me->SummonCreature(NPC_WRATH_SORCERESS, m_afFirstNagaCoord[1][0], m_afFirstNagaCoord[1][1], m_afFirstNagaCoord[1][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
            me->SummonCreature(NPC_WRATH_RAZORTAIL, m_afFirstNagaCoord[2][0], m_afFirstNagaCoord[2][1], m_afFirstNagaCoord[2][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
            break;
        case 2:
            me->SummonCreature(NPC_WRATH_PRIESTESS, m_afSecondNagaCoord[0][0], m_afSecondNagaCoord[0][1], m_afSecondNagaCoord[0][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
            me->SummonCreature(NPC_WRATH_MYRMIDON,  m_afSecondNagaCoord[1][0], m_afSecondNagaCoord[1][1], m_afSecondNagaCoord[1][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
            me->SummonCreature(NPC_WRATH_SEAWITCH,  m_afSecondNagaCoord[2][0], m_afSecondNagaCoord[2][1], m_afSecondNagaCoord[2][2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
            break;
        case 3:
            me->SummonCreature(NPC_VORSHA, m_fVorshaCoord[0], m_fVorshaCoord[1], m_fVorshaCoord[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
            break;
        case 4:
            SetEscortPaused(false);
            DoScriptText(SAY_MUG_DONE, me);
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
        {
            if (HasEscortState(STATE_ESCORT_PAUSED) && m_bIsBrazierExtinguished)
            {
                if (m_uiEventTimer < uiDiff)
                {
                    ++m_uiWaveId;
                    DoWaveSummon();
                    m_uiEventTimer = 10000;
                }
                else
                    m_uiEventTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_muglash(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_VORSHA)
    {
        if (npc_muglashAI* pEscortAI = dynamic_cast<npc_muglashAI*>(pCreature->AI()))
        {
            DoScriptText(SAY_MUG_START1, pCreature);
            pCreature->setFaction(FACTION_ESCORT_H_PASSIVE);

            pEscortAI->Start(false, false, pPlayer->GetGUID(), pQuest);
        }
    }

    return true;
}

CreatureAI* GetAI_npc_muglash(Creature* pCreature)
{
    return new npc_muglashAI(pCreature);
}

bool GOHello_go_naga_brazier(Player* /*pPlayer*/, GameObject* pGo)
{
    if (Creature* pCreature = GetClosestCreatureWithEntry(pGo, NPC_MUGLASH, INTERACTION_DISTANCE * 2))
    {
        if (npc_muglashAI* pEscortAI = dynamic_cast<npc_muglashAI*>(pCreature->AI()))
        {
            DoScriptText(SAY_MUG_BRAZIER_WAIT, pCreature);

            pEscortAI->m_bIsBrazierExtinguished = true;
            return false;
        }
    }

    return true;
}

/*####
# npc_torek
####*/

enum
{
    SAY_READY                   = -1000106,
    SAY_MOVE                    = -1000107,
    SAY_PREPARE                 = -1000108,
    SAY_WIN                     = -1000109,
    SAY_END                     = -1000110,

    SPELL_REND                  = 11977,
    SPELL_THUNDERCLAP           = 8078,

    QUEST_TOREK_ASSULT          = 6544,

    NPC_SPLINTERTREE_RAIDER     = 12859,
    ENTRY_DURIEL                = 12860,
    ENTRY_SILVERWING_SENTINEL   = 12896,
    ENTRY_SILVERWING_WARRIOR    = 12897
};

struct npc_torekAI : public npc_escortAI
{
    npc_torekAI(Creature* c) : npc_escortAI(c) {}

    uint32 Rend_Timer;
    uint32 Thunderclap_Timer;
    bool Completed;

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {
        case 1:
            DoScriptText(SAY_MOVE, me, pPlayer);
            break;
        case 8:
            DoScriptText(SAY_PREPARE, me, pPlayer);
            break;
        case 19:
            //@todo verify location and creatures amount.
            me->SummonCreature(ENTRY_DURIEL, 1776.73f, -2049.06f, 109.83f, 1.54f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            me->SummonCreature(ENTRY_SILVERWING_SENTINEL, 1774.64f, -2049.41f, 109.83f, 1.40f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            me->SummonCreature(ENTRY_SILVERWING_WARRIOR, 1778.73f, -2049.50f, 109.83f, 1.67f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            break;
        case 20:
            DoScriptText(SAY_WIN, me, pPlayer);
            Completed = true;
            if (pPlayer)
                pPlayer->GroupEventHappens(QUEST_TOREK_ASSULT, me);
            break;
        case 21:
            DoScriptText(SAY_END, me, pPlayer);
            break;
        }
    }

    void Reset()
    {
        Rend_Timer = 5000;
        Thunderclap_Timer = 8000;
        Completed = false;
    }

    void EnterCombat(Unit* /*who*/) {}

    void JustSummoned(Creature* summoned)
    {
        summoned->AI()->AttackStart(me);
    }

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);

        if (!UpdateVictim())
            return;

        if (Rend_Timer <= diff)
        {
            DoCastVictim(SPELL_REND);
            Rend_Timer = 20000;
        }
        else Rend_Timer -= diff;

        if (Thunderclap_Timer <= diff)
        {
            DoCast(me, SPELL_THUNDERCLAP);
            Thunderclap_Timer = 30000;
        }
        else Thunderclap_Timer -= diff;
    }
};

bool QuestAccept_npc_torek(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_TOREK_ASSULT)
    {
        //@todo find companions, make them follow Torek, at any time (possibly done by mangos/database in future?)
        DoScriptText(SAY_READY, pCreature, pPlayer);
        pCreature->setFaction(113);

        if (npc_escortAI* pEscortAI = CAST_AI(npc_torekAI, pCreature->AI()))
            pEscortAI->Start(true, true, pPlayer->GetGUID());
    }

    return true;
}

CreatureAI* GetAI_npc_torek(Creature* pCreature)
{
    return new npc_torekAI(pCreature);
}

/*####
# npc_ruul_snowhoof
####*/

#define QUEST_FREEDOM_TO_RUUL    6482
#define GO_CAGE                  178147

struct npc_ruul_snowhoofAI : public npc_escortAI
{
    npc_ruul_snowhoofAI(Creature* c) : npc_escortAI(c) {}

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {
        case 0:
            {
                me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                GameObject* Cage = me->FindNearestGameObject(GO_CAGE, 20);
                if (Cage)
                    Cage->SetGoState(GO_STATE_ACTIVE);
                break;
            }
        case 13:
            me->SummonCreature(3922, 3449.218018f, -587.825073f, 174.978867f, 4.714445f, TEMPSUMMON_DEAD_DESPAWN, 60000);
            me->SummonCreature(3921, 3446.384521f, -587.830872f, 175.186279f, 4.714445f, TEMPSUMMON_DEAD_DESPAWN, 60000);
            me->SummonCreature(3926, 3444.218994f, -587.835327f, 175.380600f, 4.714445f, TEMPSUMMON_DEAD_DESPAWN, 60000);
            break;
        case 19:
            me->SummonCreature(3922, 3508.344482f, -492.024261f, 186.929031f, 4.145029f, TEMPSUMMON_DEAD_DESPAWN, 60000);
            me->SummonCreature(3921, 3506.265625f, -490.531006f, 186.740128f, 4.239277f, TEMPSUMMON_DEAD_DESPAWN, 60000);
            me->SummonCreature(3926, 3503.682373f, -489.393799f, 186.629684f, 4.349232f, TEMPSUMMON_DEAD_DESPAWN, 60000);
            break;

        case 21:
            {
                if (pPlayer)
                    pPlayer->GroupEventHappens(QUEST_FREEDOM_TO_RUUL, me);

                break;
            }
        }
    }

    void EnterCombat(Unit* /*who*/) {}

    void Reset()
    {
        GameObject* Cage = me->FindNearestGameObject(GO_CAGE, 20);
        if (Cage)
            Cage->SetGoState(GO_STATE_READY);
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->AI()->AttackStart(me);
    }

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);
    }
};

bool QuestAccept_npc_ruul_snowhoof(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_FREEDOM_TO_RUUL)
    {
        pCreature->setFaction(113);

        if (npc_escortAI* pEscortAI = CAST_AI(npc_ruul_snowhoofAI, (pCreature->AI())))
            pEscortAI->Start(true, false, pPlayer->GetGUID());
    }
    return true;
}

CreatureAI* GetAI_npc_ruul_snowhoofAI(Creature* pCreature)
{
    return new npc_ruul_snowhoofAI(pCreature);
}

/*######
## npc_feero
######*/

enum Ferro
{
	FACTION_ESCORTEE = 774,

	NPC_DARK_STRAND_ASSASSINS = 3879,
	NPC_FORSAKEN_SCOUT = 3893,
	NPC_CAEDAKAR_LEFT = 3900,
	NPC_ALIGAR_MIDDLE = 3898,
	NPC_BALIZAR_RIGHT = 3899,

	QUEST_SUPPLIES_TO_AUBERDINE = 976
};

#define SAY_TEXT_1 "It looks like we're in trouble. Look lively, here they come!"
#define SAY_DEATH_1 "Assassins from that cult you found... Let's get moving before someone else finds us out here."
#define SAY_TEXT_2 "Hold! I sense an evil presence... Undead!"
#define SAY_DEATH_2 "They're coming out of the woodwork today. Let's keep moving or we may find more things that want me dead."
#define SAY_TEXT_3_A "These three again?"
#define SAY_TEXT_3_B "I'll finish you off for good this time!"
#define SAY_END_1 "Well done! I should be fine on my own from here. Remember to talk to Delgren when you return to Maestra's Post in Ashenvale."

#define SAY_BALIZAR "Not quite so sure of yourself without the Purifier, hm?"
#define SAY_SCOUT "A paladin! Slaying him would please the master. Attack!"

struct npc_feeroAI : public npc_escortAI
{
	npc_feeroAI(Creature* creature) : npc_escortAI(creature) {}

	void Reset() { }

	void JustSummoned(Creature* summoned)
	{
		summoned->AI()->AttackStart(me);
	}

	void WaypointReached(uint32 waypointId)
	{
		Player* player = GetPlayerForEscort();
		if (!player)
			return;

		switch (waypointId)
		{
		case 18:
			me->MonsterSay(SAY_TEXT_1, LANG_UNIVERSAL, 0);
			me->SummonCreature(NPC_DARK_STRAND_ASSASSINS, 3491.09f, 214.76f, 11.36f, 3.30f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
			me->SummonCreature(NPC_DARK_STRAND_ASSASSINS, 3491.18f, 212.28f, 11.25f, 3.03f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
			me->SummonCreature(NPC_DARK_STRAND_ASSASSINS, 3490.74f, 210.59f, 11.32f, 3.03f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
			me->SummonCreature(NPC_DARK_STRAND_ASSASSINS, 3490.46f, 208.78f, 11.39f, 2.93f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
			break;
		case 19:
			me->MonsterSay(SAY_DEATH_1, LANG_UNIVERSAL, 0);
			break;
		case 27:
			me->MonsterSay(SAY_TEXT_2, LANG_UNIVERSAL, 0);
			me->SummonCreature(NPC_FORSAKEN_SCOUT, 3782.51f, 145.57f, 8.54f, 2.93f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
			me->SummonCreature(NPC_FORSAKEN_SCOUT, 3778.84f, 143.46f, 8.41f, 2.93f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
			me->SummonCreature(NPC_FORSAKEN_SCOUT, 3782.26f, 149.61f, 8.34f, 2.93f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);

			if (Creature* scout = me->FindNearestCreature(NPC_FORSAKEN_SCOUT, 30.0f, true))
			{
				scout->MonsterSay(SAY_SCOUT, LANG_UNIVERSAL, 0);
			}

			break;
		case 28:
			me->MonsterSay(SAY_DEATH_2, LANG_UNIVERSAL, 0);
			break;
		case 41:
			me->MonsterSay(SAY_TEXT_3_A, LANG_UNIVERSAL, 0);
			me->SummonCreature(NPC_CAEDAKAR_LEFT, 4108.34f, 53.69f, 26.18f, 2.93f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
			me->SummonCreature(NPC_ALIGAR_MIDDLE, 4114.55f, 54.48f, 27.21f, 2.93f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
			me->SummonCreature(NPC_BALIZAR_RIGHT, 4116.81f, 50.14f, 26.15f, 2.93f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
			
			if (Creature* balizar = me->FindNearestCreature(NPC_BALIZAR_RIGHT, 30.0f, true))
			{
				balizar->MonsterSay(SAY_BALIZAR, LANG_UNIVERSAL, 0);
			}
			break;
		case 42:
			me->MonsterSay(SAY_TEXT_3_B, LANG_UNIVERSAL, 0);
			break;
		case 43:
			me->MonsterSay(SAY_END_1, LANG_UNIVERSAL, 0);
			SetRun();
			player->GroupEventHappens(QUEST_SUPPLIES_TO_AUBERDINE, me);
			break;
		}
	}
};

CreatureAI* GetAI_npc_feero(Creature* pCreature)
{
	npc_feeroAI* feeroAI = new npc_feeroAI(pCreature);

	feeroAI->AddWaypoint(1, 3175.930908, 193.541306, 3.483540, 0);
	feeroAI->AddWaypoint(2, 3187.917969, 197.117691, 4.699296, 0);
	feeroAI->AddWaypoint(3, 3203.483643, 192.349060, 5.908475, 0);
	feeroAI->AddWaypoint(4, 3219.118408, 182.236420, 6.588406, 0);
	feeroAI->AddWaypoint(5, 3229.847412, 191.230438, 7.494555, 0);
	feeroAI->AddWaypoint(6, 3225.035156, 199.438843, 7.096720, 0);
	feeroAI->AddWaypoint(7, 3227.651855, 210.760071, 8.629334, 0);
	feeroAI->AddWaypoint(8, 3232.935303, 223.724869, 10.052238, 0);
	feeroAI->AddWaypoint(9, 3263.071777, 225.984848, 10.645896, 0);
	feeroAI->AddWaypoint(10, 3284.759521, 220.414124, 10.950543, 0);
	feeroAI->AddWaypoint(11, 3315.646973, 210.198044, 11.967686, 0);
	feeroAI->AddWaypoint(12, 3341.024414, 214.290497, 13.320419, 0);
	feeroAI->AddWaypoint(13, 3367.133789, 224.587524, 11.867117, 0);
	feeroAI->AddWaypoint(14, 3409.073242, 226.385315, 9.215232, 0);
	feeroAI->AddWaypoint(15, 3432.292236, 225.396271, 10.028325, 0);
	feeroAI->AddWaypoint(16, 3454.865723, 219.339172, 12.593150, 0);
	feeroAI->AddWaypoint(17, 3470.463867, 214.818161, 13.264424, 0);
	feeroAI->AddWaypoint(18, 3481.416992, 212.556610, 12.354552, 2000);
	feeroAI->AddWaypoint(19, 3500.315674, 210.936295, 10.226085, 0);
	feeroAI->AddWaypoint(20, 3532.806641, 215.041473, 8.372272, 0);
	feeroAI->AddWaypoint(21, 3565.314209, 217.748749, 5.300299, 0);
	feeroAI->AddWaypoint(22, 3601.654297, 217.771378, 1.299005, 0);
	feeroAI->AddWaypoint(23, 3638.605713, 212.525879, 1.433142, 0);
	feeroAI->AddWaypoint(24, 3680.757324, 200.308197, 3.385010, 0);
	feeroAI->AddWaypoint(25, 3725.670410, 180.395966, 6.314014, 0);
	feeroAI->AddWaypoint(26, 3762.346924, 159.685959, 7.388617, 0);
	feeroAI->AddWaypoint(27, 3774.541260, 151.170029, 7.799640, 2000);
	feeroAI->AddWaypoint(28, 3789.697754, 140.396774, 9.062237, 0);
	feeroAI->AddWaypoint(29, 3821.424072, 111.609528, .258650, 0);
	feeroAI->AddWaypoint(30, 3850.376465, 84.710922, 13.941991, 0);
	feeroAI->AddWaypoint(31, 3875.349121, 60.388409, 14.988914, 0);
	feeroAI->AddWaypoint(32, 3908.238525, 35.209225, 15.332011, 0);
	feeroAI->AddWaypoint(33, 3942.200928, 14.888245, 16.969385, 0);
	feeroAI->AddWaypoint(34, 3976.427246, -0.073566, 16.968657, 0);
	feeroAI->AddWaypoint(35, 4008.343750, -6.628914, 16.464090, 0);
	feeroAI->AddWaypoint(36, 4029.483643, -6.640755, 16.549721, 0);
	feeroAI->AddWaypoint(37, 4050.055908, 1.488156, 15.746178, 0);
	feeroAI->AddWaypoint(38, 4083.412109, 14.085828, 15.851171, 0);
	feeroAI->AddWaypoint(39, 4098.462891, 20.032930, 17.252523, 0);
	feeroAI->AddWaypoint(40, 4105.861816, 34.792000, 20.284599, 0);
	feeroAI->AddWaypoint(41, 4110.536133, 45.538300, 23.154394, 2000);
	feeroAI->AddWaypoint(42, 4112.863281, 51.454445, 26.165501, 0);
	feeroAI->AddWaypoint(43, 4126.177246, 53.689651, 26.399027, 3000);
	feeroAI->AddWaypoint(44, 4149.127441, 46.833157, 24.660984, 0);
	feeroAI->AddWaypoint(45, 4164.439941, 55.935448, 26.793362, 0);
	
	return feeroAI;
}

bool QuestAccept_npc_feero(Player* player, Creature* creature, Quest const* quest)
{
		if (quest->GetQuestId() == QUEST_SUPPLIES_TO_AUBERDINE)
		{
			if (npc_escortAI* pEscortAI = CAST_AI(npc_feeroAI, creature->AI()))
				pEscortAI->Start(true, false, player->GetGUID());

			creature->setFaction(774);
			creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		}

	return true;
}

void AddSC_ashenvale()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_muglash";
    newscript->GetAI = &GetAI_npc_muglash;
    newscript->pQuestAccept = &QuestAccept_npc_muglash;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_naga_brazier";
    newscript->pGOHello = &GOHello_go_naga_brazier;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_torek";
    newscript->GetAI = &GetAI_npc_torek;
    newscript->pQuestAccept = &QuestAccept_npc_torek;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_ruul_snowhoof";
    newscript->GetAI = &GetAI_npc_ruul_snowhoofAI;
    newscript->pQuestAccept = &QuestAccept_npc_ruul_snowhoof;
    newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_feero";
	newscript->GetAI = &GetAI_npc_feero;
	newscript->pQuestAccept = &QuestAccept_npc_feero;
	newscript->RegisterSelf();
}
