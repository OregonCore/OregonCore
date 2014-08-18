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
SDName: Hinterlands
SD%Complete: 100
SDComment: Quest support: 863, 2742
SDCategory: The Hinterlands
EndScriptData */

/* ContentData
npc_00x09hl
npc_rinji
EndContentData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"

/*######
## npc_00x09hl
######*/

enum eOOX
{
    SAY_OOX_START           = -1000287,
    SAY_OOX_AGGRO1          = -1000288,
    SAY_OOX_AGGRO2          = -1000289,
    SAY_OOX_AMBUSH          = -1000290,
    SAY_OOX_END             = -1000292,

    QUEST_RESQUE_OOX_09     = 836,

    NPC_MARAUDING_OWL       = 7808,
    NPC_VILE_AMBUSHER       = 7809,

};

struct npc_00x09hlAI : public npc_escortAI
{
    npc_00x09hlAI(Creature* pCreature) : npc_escortAI(pCreature) { }

    void Reset() { }

    void WaypointReached(uint32 uiPointId)
    {
        Player* pPlayer = GetPlayerForEscort();
        if (!pPlayer)
            return;

        switch(uiPointId)
        {
            case 26:
                DoScriptText(SAY_OOX_AMBUSH, me);
                for (uint8 i = 0; i < 3; ++i)
                    me->SummonCreature(NPC_MARAUDING_OWL, 178.111f, -3801.58f, 128.37f, 0.0f,    TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 30000);
                break;
            case 43:
                DoScriptText(SAY_OOX_AMBUSH, me);
                for (uint8 i = 0; i < 3; ++i)
                    me->SummonCreature(NPC_VILE_AMBUSHER, -116.258f, -4211.96f, 121.878f, 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 30000);
                break;
            case 64:
                DoScriptText(SAY_OOX_END, me);
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_RESQUE_OOX_09, me);
                break;
        }
    }

    void EnterCombat(Unit* pWho)
    {
        if (pWho->GetEntry() == NPC_MARAUDING_OWL || pWho->GetEntry() == NPC_VILE_AMBUSHER)
            return;

        DoScriptText(RAND(SAY_OOX_AGGRO1,SAY_OOX_AGGRO2), me);
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(me);
    }
};

bool QuestAccept_npc_00x09hl(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_RESQUE_OOX_09)
    {
        pCreature->SetStandState(UNIT_STAND_STATE_STAND);
        pCreature->setFaction(113);
        pCreature->SetHealth(pCreature->GetMaxHealth());
        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);

        DoScriptText(SAY_OOX_START, pCreature);

        if (npc_00x09hlAI* pEscortAI = CAST_AI(npc_00x09hlAI, pCreature->AI()))
            pEscortAI->Start(false, true, pPlayer->GetGUID(), pQuest);
    }
    return true;
}

CreatureAI* GetAI_npc_00x09hl(Creature* pCreature)
{
    return new npc_00x09hlAI(pCreature);
}

/*######
## npc_rinji
######*/

enum eRinji
{
    SAY_RIN_FREE            = -1000403,
    SAY_RIN_BY_OUTRUNNER    = -1000404,
    SAY_RIN_HELP_1          = -1000405,
    SAY_RIN_HELP_2          = -1000406,
    SAY_RIN_COMPLETE        = -1000407,
    SAY_RIN_PROGRESS_1      = -1000408,
    SAY_RIN_PROGRESS_2      = -1000409,

    QUEST_RINJI_TRAPPED     = 2742,
    NPC_RANGER              = 2694,
    NPC_OUTRUNNER           = 2691,
    GO_RINJI_CAGE           = 142036
};

struct Location
{
    float m_fX, m_fY, m_fZ;
};

Location m_afAmbushSpawn[] =
{
    {191.296204f, -2839.329346f, 107.388f},
    {70.972466f,  -2848.674805f, 109.459f}
};

Location m_afAmbushMoveTo[] =
{
    {166.630386f, -2824.780273f, 108.153f},
    {70.886589f,  -2874.335449f, 116.675f}
};

struct npc_rinjiAI : public npc_escortAI
{
    npc_rinjiAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_bIsByOutrunner = false;
        m_iSpawnId = 0;
    }

    bool m_bIsByOutrunner;
    uint32 m_uiPostEventCount;
    uint32 m_uiPostEventTimer;
    int m_iSpawnId;

    void Reset()
    {
        m_uiPostEventCount = 0;
        m_uiPostEventTimer = 3000;
    }

    void JustRespawned()
    {
        m_bIsByOutrunner = false;
        m_iSpawnId = 0;

        npc_escortAI::JustRespawned();
    }

    void EnterCombat(Unit* pWho)
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
        {
            if (pWho->GetEntry() == NPC_OUTRUNNER && !m_bIsByOutrunner)
            {
                DoScriptText(SAY_RIN_BY_OUTRUNNER, pWho);
                m_bIsByOutrunner = true;
            }

            if (rand()%4)
                return;

            //only if attacked and escorter is not in combat?
            DoScriptText(RAND(SAY_RIN_HELP_1,SAY_RIN_HELP_2), me);
        }
    }

    void DoSpawnAmbush(bool bFirst)
    {
        if (!bFirst)
            m_iSpawnId = 1;

        me->SummonCreature(NPC_RANGER,
            m_afAmbushSpawn[m_iSpawnId].m_fX, m_afAmbushSpawn[m_iSpawnId].m_fY, m_afAmbushSpawn[m_iSpawnId].m_fZ, 0.0f,
            TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000);

        for (int i = 0; i < 2; ++i)
        {
            me->SummonCreature(NPC_OUTRUNNER,
                m_afAmbushSpawn[m_iSpawnId].m_fX, m_afAmbushSpawn[m_iSpawnId].m_fY, m_afAmbushSpawn[m_iSpawnId].m_fZ, 0.0f,
                TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000);
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->RemoveUnitMovementFlag(MOVEFLAG_WALK_MODE);
        pSummoned->GetMotionMaster()->MovePoint(0, m_afAmbushMoveTo[m_iSpawnId].m_fX, m_afAmbushMoveTo[m_iSpawnId].m_fY, m_afAmbushMoveTo[m_iSpawnId].m_fZ);
    }

    void WaypointReached(uint32 uiPointId)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch(uiPointId)
        {
            case 1:
                DoScriptText(SAY_RIN_FREE, me, pPlayer);
                break;
            case 7:
                DoSpawnAmbush(true);
                break;
            case 13:
                DoSpawnAmbush(false);
                break;
            case 17:
                DoScriptText(SAY_RIN_COMPLETE, me, pPlayer);
                pPlayer->GroupEventHappens(QUEST_RINJI_TRAPPED, me);
                SetRun();
                m_uiPostEventCount = 1;
                break;
        }
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        //Check if we have a current target
        if (!UpdateVictim())
        {
            if (HasEscortState(STATE_ESCORT_ESCORTING) && m_uiPostEventCount)
            {
                if (m_uiPostEventTimer <= uiDiff)
                {
                    m_uiPostEventTimer = 3000;

                    if (Unit* pPlayer = GetPlayerForEscort())
                    {
                        switch(m_uiPostEventCount)
                        {
                            case 1:
                                DoScriptText(SAY_RIN_PROGRESS_1, me, pPlayer);
                                ++m_uiPostEventCount;
                                break;
                            case 2:
                                DoScriptText(SAY_RIN_PROGRESS_2, me, pPlayer);
                                m_uiPostEventCount = 0;
                                break;
                        }
                    }
                    else
                    {
                        me->ForcedDespawn();
                        return;
                    }
                }
                else
                    m_uiPostEventTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_rinji(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_RINJI_TRAPPED)
    {
        if (GameObject* pGo = pCreature->FindNearestGameObject(GO_RINJI_CAGE, INTERACTION_DISTANCE))
            pGo->UseDoorOrButton();

        if (npc_rinjiAI* pEscortAI = CAST_AI(npc_rinjiAI, pCreature->AI()))
            pEscortAI->Start(false, false, pPlayer->GetGUID(), pQuest);
    }
    return true;
}

CreatureAI* GetAI_npc_rinji(Creature* pCreature)
{
    return new npc_rinjiAI(pCreature);
}

void AddSC_hinterlands()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_00x09hl";
    newscript->GetAI = &GetAI_npc_00x09hl;
    newscript->pQuestAccept = &QuestAccept_npc_00x09hl;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_rinji";
    newscript->GetAI = &GetAI_npc_rinji;
    newscript->pQuestAccept = &QuestAccept_npc_rinji;
    newscript->RegisterSelf();
}
