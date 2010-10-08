/*
 * Copyright (C) 2008-2010 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Ungoro Crater
SD%Complete: 100
SDComment: Support for Quest: 4245, 4491
SDCategory: Ungoro Crater
EndScriptData */

/* ContentData
npc_a-me
npc_ringo
EndContentData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"
#include "ScriptedFollowerAI.h"

#define SAY_READY -1000200
#define SAY_AGGRO1 -1000201
#define SAY_SEARCH -1000202
#define SAY_AGGRO2 -1000203
#define SAY_AGGRO3 -1000204
#define SAY_FINISH -1000205

#define SPELL_DEMORALIZINGSHOUT  13730

#define QUEST_CHASING_AME 4245
#define ENTRY_TARLORD 6519
#define ENTRY_TARLORD1 6519
#define ENTRY_STOMPER 6513

struct npc_ameAI : public npc_escortAI
{
    npc_ameAI(Creature *c) : npc_escortAI(c) {}

    uint32 DEMORALIZINGSHOUT_Timer;

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {

         case 19:
            me->SummonCreature(ENTRY_STOMPER, -6391.69, -1730.49, -272.83, 4.96, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            DoScriptText(SAY_AGGRO1, me, pPlayer);
            break;
            case 28:
            DoScriptText(SAY_SEARCH, me, pPlayer);
            break;
            case 38:
            me->SummonCreature(ENTRY_TARLORD, -6370.75, -1382.84, -270.51, 6.06, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            DoScriptText(SAY_AGGRO2, me, pPlayer);
            break;
            case 49:
            me->SummonCreature(ENTRY_TARLORD1, -6324.44, -1181.05, -270.17, 4.34, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            DoScriptText(SAY_AGGRO3, me, pPlayer);
            break;
         case 55:
            DoScriptText(SAY_FINISH, me, pPlayer);
            if (pPlayer)
                pPlayer->GroupEventHappens(QUEST_CHASING_AME,me);
            break;

        }
    }

    void Reset()
    {
      DEMORALIZINGSHOUT_Timer = 5000;
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->AI()->AttackStart(me);
    }

    void JustDied(Unit* /*killer*/)
    {
        if (Player* pPlayer = GetPlayerForEscort())
            pPlayer->FailQuest(QUEST_CHASING_AME);
    }

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);
        if (!UpdateVictim())
            return;

        if (DEMORALIZINGSHOUT_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_DEMORALIZINGSHOUT);
            DEMORALIZINGSHOUT_Timer = 70000;
        } else DEMORALIZINGSHOUT_Timer -= diff;

    }
};

bool QuestAccept_npc_ame(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_CHASING_AME)
    {
        CAST_AI(npc_escortAI, (pCreature->AI()))->Start(false, false, pPlayer->GetGUID());
        DoScriptText(SAY_READY, pCreature, pPlayer);
        pCreature->SetUInt32Value(UNIT_FIELD_BYTES_1,0);
        // Change faction so mobs attack
        pCreature->setFaction(113);
    }
    return true;
}

CreatureAI* GetAI_npc_ame(Creature* pCreature)
{
    npc_ameAI* thisAI = new npc_ameAI(pCreature);

   thisAI->AddWaypoint(1, -6380.38, -1965.14, -258.292, 5000);
   thisAI->AddWaypoint(2, -6383.06, -1962.9, -258.936);
   thisAI->AddWaypoint(3, -6391.09, -1956.13, -260.291);
   thisAI->AddWaypoint(4, -6395.29, -1933.58, -262.949);
   thisAI->AddWaypoint(5, -6396.58, -1919.93, -263.838);
   thisAI->AddWaypoint(6, -6389.01, -1912.64, -260.689);
   thisAI->AddWaypoint(7, -6369.19, -1892.87, -255.924);
   thisAI->AddWaypoint(8, -6373.77, -1879.36, -259.268);
   thisAI->AddWaypoint(9, -6377.55, -1869.56, -260.503);
   thisAI->AddWaypoint(10, -6376.58, -1860.79, -260.026);
   thisAI->AddWaypoint(11, -6373.13, -1847.22, -259.249);
   thisAI->AddWaypoint(12, -6370.54, -1837.04, -260.007);
   thisAI->AddWaypoint(13, -6372.52, -1829.16, -260.071);
   thisAI->AddWaypoint(14, -6377.13, -1815.94, -262.632);
   thisAI->AddWaypoint(15, -6380.27, -1806.95, -265.53);
   thisAI->AddWaypoint(16, -6386.04, -1790.43, -268.546);
   thisAI->AddWaypoint(17, -6386.72, -1776.29, -269.851);
   thisAI->AddWaypoint(18, -6385.92, -1762.31, -271.494);
   thisAI->AddWaypoint(19, -6384.69, -1744.86, -272.196);
   thisAI->AddWaypoint(20, -6383.8, -1732.66, -272.222);
   thisAI->AddWaypoint(21, -6382.66, -1716.96, -272.235);
   thisAI->AddWaypoint(22, -6381.5, -1703.01, -272.964);
   thisAI->AddWaypoint(23, -6379.96, -1685.58, -272.842);
   thisAI->AddWaypoint(24, -6379.34, -1678.61, -272.34);
   thisAI->AddWaypoint(25, -6364.45, -1636.27, -271.065);
   thisAI->AddWaypoint(26, -6371.85, -1626.36, -272.188);
   thisAI->AddWaypoint(27, -6383.5, -1629.01, -272.206);
   thisAI->AddWaypoint(28, -6388.09, -1635.37, -272.105, 5000);
   thisAI->AddWaypoint(29, -6375.42, -1637.33, -272.193);
   thisAI->AddWaypoint(30, -6365.46, -1617.25, -272.141);
   thisAI->AddWaypoint(31, -6353.79, -1603.48, -271.932);
   thisAI->AddWaypoint(32, -6340.24, -1592.41, -269.435);
   thisAI->AddWaypoint(33, -6329.45, -1566.89, -269.895);
   thisAI->AddWaypoint(34, -6312.2, -1499.06, -269.507);
   thisAI->AddWaypoint(35, -6304.55, -1468.5, -269.431);
   thisAI->AddWaypoint(36, -6310.36, -1440.94, -268.427);
   thisAI->AddWaypoint(37, -6321, -1418.91, -266.525);
   thisAI->AddWaypoint(38, -6358.76, -1389.97, -267.522);
   thisAI->AddWaypoint(39, -6378.65, -1375.67, -271.749);
   thisAI->AddWaypoint(40, -6387.22, -1360.95, -272.109);
   thisAI->AddWaypoint(41, -6406.95, -1323.87, -271.586);
   thisAI->AddWaypoint(42, -6405, -1311.92, -271.906);
   thisAI->AddWaypoint(43, -6395.56, -1303.62, -271.902);
   thisAI->AddWaypoint(44, -6375.97, -1296.08, -271.865);
   thisAI->AddWaypoint(45, -6364.39, -1281.23, -269.012);
   thisAI->AddWaypoint(46, -6353.71, -1263.19, -267.95);
   thisAI->AddWaypoint(47, -6340.09, -1248.65, -267.441);
   thisAI->AddWaypoint(48, -6338.21, -1237.11, -267.844);
   thisAI->AddWaypoint(49, -6336.6, -1219.69, -269.196);
   thisAI->AddWaypoint(50, -6334.44, -1202.33, -271.527);
   thisAI->AddWaypoint(51, -6329.56, -1189.82, -270.947);
   thisAI->AddWaypoint(52, -6324.66, -1179.46, -270.103);
   thisAI->AddWaypoint(53, -6315.08, -1176.74, -269.735);
   thisAI->AddWaypoint(54, -6308.49, -1179.12, -269.57);
   thisAI->AddWaypoint(55, -6302.43, -1181.32, -269.328, 5000);
   thisAI->AddWaypoint(56, -6298.87, -1185.79, -269.278);

    return (CreatureAI*)thisAI;
}

/*####
# npc_ringo
####*/

enum eRingo
{
    SAY_RIN_START_1             = -1000416,
    SAY_RIN_START_2             = -1000417,

    SAY_FAINT_1                 = -1000418,
    SAY_FAINT_2                 = -1000419,
    SAY_FAINT_3                 = -1000420,
    SAY_FAINT_4                 = -1000421,

    SAY_WAKE_1                  = -1000422,
    SAY_WAKE_2                  = -1000423,
    SAY_WAKE_3                  = -1000424,
    SAY_WAKE_4                  = -1000425,

    SAY_RIN_END_1               = -1000426,
    SAY_SPR_END_2               = -1000427,
    SAY_RIN_END_3               = -1000428,
    EMOTE_RIN_END_4             = -1000429,
    EMOTE_RIN_END_5             = -1000430,
    SAY_RIN_END_6               = -1000431, // signed for 6784
    SAY_SPR_END_7               = -1000432,
    EMOTE_RIN_END_8             = -1000433,

    SPELL_REVIVE_RINGO          = 15591,
    QUEST_A_LITTLE_HELP         = 4491,
    NPC_SPRAGGLE                = 9997,
    FACTION_ESCORTEE            = 113
};

struct npc_ringoAI : public FollowerAI
{
    npc_ringoAI(Creature* pCreature) : FollowerAI(pCreature) { }

    uint32 m_uiFaintTimer;
    uint32 m_uiEndEventProgress;
    uint32 m_uiEndEventTimer;

    uint64 SpraggleGUID;

    void Reset()
    {
        m_uiFaintTimer = urand(30000, 60000);
        m_uiEndEventProgress = 0;
        m_uiEndEventTimer = 1000;
        SpraggleGUID = 0;
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        FollowerAI::MoveInLineOfSight(pWho);

        if (!me->getVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE) && pWho->GetEntry() == NPC_SPRAGGLE)
        {
            if (me->IsWithinDistInMap(pWho, INTERACTION_DISTANCE))
            {
                if (Player* pPlayer = GetLeaderForFollower())
                {
                    if (pPlayer->GetQuestStatus(QUEST_A_LITTLE_HELP) == QUEST_STATUS_INCOMPLETE)
                        pPlayer->GroupEventHappens(QUEST_A_LITTLE_HELP, me);
                }

                SpraggleGUID = pWho->GetGUID();
                SetFollowComplete(true);
            }
        }
    }

    void SpellHit(Unit* /*pCaster*/, const SpellEntry* pSpell)
    {
        if (HasFollowState(STATE_FOLLOW_INPROGRESS | STATE_FOLLOW_PAUSED) && pSpell->Id == SPELL_REVIVE_RINGO)
            ClearFaint();
    }

    void SetFaint()
    {
        if (!HasFollowState(STATE_FOLLOW_POSTEVENT))
        {
            SetFollowPaused(true);

            DoScriptText(RAND(SAY_FAINT_1,SAY_FAINT_2,SAY_FAINT_3,SAY_FAINT_4), me);
        }

        //what does actually happen here? Emote? Aura?
        me->SetStandState(UNIT_STAND_STATE_SLEEP);
    }

    void ClearFaint()
    {
        me->SetStandState(UNIT_STAND_STATE_STAND);

        if (HasFollowState(STATE_FOLLOW_POSTEVENT))
            return;

        DoScriptText(RAND(SAY_WAKE_1,SAY_WAKE_2,SAY_WAKE_3,SAY_WAKE_4), me);

        SetFollowPaused(false);
    }

    void UpdateFollowerAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
        {
            if (HasFollowState(STATE_FOLLOW_POSTEVENT))
            {
                if (m_uiEndEventTimer <= uiDiff)
                {
                    Unit *pSpraggle = Unit::GetUnit(*me, SpraggleGUID);
                    if (!pSpraggle || !pSpraggle->isAlive())
                    {
                        SetFollowComplete();
                        return;
                    }

                    switch(m_uiEndEventProgress)
                    {
                        case 1:
                            DoScriptText(SAY_RIN_END_1, me);
                            m_uiEndEventTimer = 3000;
                            break;
                        case 2:
                            DoScriptText(SAY_SPR_END_2, pSpraggle);
                            m_uiEndEventTimer = 5000;
                            break;
                        case 3:
                            DoScriptText(SAY_RIN_END_3, me);
                            m_uiEndEventTimer = 1000;
                            break;
                        case 4:
                            DoScriptText(EMOTE_RIN_END_4, me);
                            SetFaint();
                            m_uiEndEventTimer = 9000;
                            break;
                        case 5:
                            DoScriptText(EMOTE_RIN_END_5, me);
                            ClearFaint();
                            m_uiEndEventTimer = 1000;
                            break;
                        case 6:
                            DoScriptText(SAY_RIN_END_6, me);
                            m_uiEndEventTimer = 3000;
                            break;
                        case 7:
                            DoScriptText(SAY_SPR_END_7, pSpraggle);
                            m_uiEndEventTimer = 10000;
                            break;
                        case 8:
                            DoScriptText(EMOTE_RIN_END_8, me);
                            m_uiEndEventTimer = 5000;
                            break;
                        case 9:
                            SetFollowComplete();
                            break;
                    }

                    ++m_uiEndEventProgress;
                }
                else
                    m_uiEndEventTimer -= uiDiff;
            }
            else if (HasFollowState(STATE_FOLLOW_INPROGRESS))
            {
                if (!HasFollowState(STATE_FOLLOW_PAUSED))
                {
                    if (m_uiFaintTimer <= uiDiff)
                    {
                        SetFaint();
                        m_uiFaintTimer = urand(60000, 120000);
                    }
                    else
                        m_uiFaintTimer -= uiDiff;
                }
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_ringo(Creature* pCreature)
{
    return new npc_ringoAI(pCreature);
}

bool QuestAccept_npc_ringo(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_A_LITTLE_HELP)
    {
        if (npc_ringoAI* pRingoAI = CAST_AI(npc_ringoAI, pCreature->AI()))
        {
            pCreature->SetStandState(UNIT_STAND_STATE_STAND);
            pRingoAI->StartFollow(pPlayer, FACTION_ESCORTEE, pQuest);
        }
    }

    return true;
}

void AddSC_ungoro_crater()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_ame";
    newscript->GetAI = &GetAI_npc_ame;
    newscript->pQuestAccept = &QuestAccept_npc_ame;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_ringo";
    newscript->GetAI = &GetAI_npc_ringo;
    newscript->pQuestAccept = &QuestAccept_npc_ringo;
    newscript->RegisterSelf();
}

