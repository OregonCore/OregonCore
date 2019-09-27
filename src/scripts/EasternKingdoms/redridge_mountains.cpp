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

/* Script Data Start
SDName: Redridge Mountains
SD%Complete: 100%
SDComment: Support for quest 219.
Script Data End */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"

enum eCorporalKeeshan
{
    QUEST_MISSING_IN_ACTION = 219,

    SAY_CORPORAL_1  = -1000561,
    SAY_CORPORAL_2  = -1000562,
    SAY_CORPORAL_3  = -1000563,
    SAY_CORPORAL_4  = -1000564,
    SAY_CORPORAL_5  = -1000565,

    SPELL_MOCKING_BLOW  = 21008,
    SPELL_SHIELD_BASH   = 11972,
};

struct npc_corporal_keeshanAI : public npc_escortAI
{
    npc_corporal_keeshanAI(Creature* pCreature) : npc_escortAI(pCreature) {}

    uint32 uiPhase;
    uint32 uiTimer;
    uint32 uiMockingBlowTimer;
    uint32 uiShieldBashTimer;

    void Reset()
    {
        uiTimer = 0;
        uiPhase = 0;
        uiMockingBlowTimer = 5000;
        uiShieldBashTimer  = 8000;
    }

    void WaypointReached(uint32 uiI)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        if (uiI >= 65 && me->GetUnitMovementFlags() == MOVEMENTFLAG_WALK_MODE)
            me->SetWalk(false);

        switch (uiI)
        {
        case 39:
            SetEscortPaused(true);
            uiTimer = 2000;
            uiPhase = 1;
            break;
        case 65:
            me->SetWalk(false);
            break;
        case 115:
            pPlayer->AreaExploredOrEventHappens(QUEST_MISSING_IN_ACTION);
            uiTimer = 2000;
            uiPhase = 4;
            break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (HasEscortState(STATE_ESCORT_NONE))
            return;

        npc_escortAI::UpdateAI(uiDiff);

        if (uiPhase)
        {
            if (uiTimer <= uiDiff)
            {
                switch (uiPhase)
                {
                case 1:
                    me->SetStandState(UNIT_STAND_STATE_SIT);
                    uiTimer = 1000;
                    uiPhase = 2;
                    break;
                case 2:
                    DoScriptText(SAY_CORPORAL_2, me);
                    uiTimer = 15000;
                    uiPhase = 3;
                    break;
                case 3:
                    DoScriptText(SAY_CORPORAL_3, me);
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    SetEscortPaused(false);
                    uiTimer = 0;
                    uiPhase = 0;
                    break;
                case 4:
                    DoScriptText(SAY_CORPORAL_4, me);
                    uiTimer = 2500;
                    uiPhase = 5;
                case 5:
                    DoScriptText(SAY_CORPORAL_5, me);
                    uiTimer = 0;
                    uiPhase = 0;
                }
            }
            else uiTimer -= uiDiff;
        }

        if (!UpdateVictim())
            return;

        if (uiMockingBlowTimer <= uiDiff)
        {
            DoCastVictim(SPELL_MOCKING_BLOW);
            uiMockingBlowTimer = 5000;
        }
        else uiMockingBlowTimer -= uiDiff;

        if (uiShieldBashTimer <= uiDiff)
        {
            DoCastVictim(SPELL_MOCKING_BLOW);
            uiShieldBashTimer = 8000;
        }
        else uiShieldBashTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_corporal_keeshan(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_MISSING_IN_ACTION)
    {
        CAST_AI(npc_corporal_keeshanAI, pCreature->AI())->Start(true, false, pPlayer->GetGUID(), pQuest);
        DoScriptText(SAY_CORPORAL_1, pCreature);
    }

    return false;
}

CreatureAI* GetAI_npc_corporal_keeshan(Creature* pCreature)
{
    return new npc_corporal_keeshanAI(pCreature);
}

void AddSC_redridge_mountains()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_corporal_keeshan";
    newscript->GetAI = &GetAI_npc_corporal_keeshan;
    newscript->pQuestAccept = &QuestAccept_npc_corporal_keeshan;
    newscript->RegisterSelf();
}
