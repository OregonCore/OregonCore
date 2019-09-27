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
SDName: Boss Mr.Smite
SD%Complete:
SDComment: Timers and say taken from acid script
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "deadmines.h"

enum eSpels
{
    SPELL_TRASH             = 3391,
    SPELL_SMITE_STOMP       = 6432,
    SPELL_SMITE_SLAM        = 6435,
    SPELL_NIMBLE_REFLEXES   = 6264,

    EQUIP_SWORD             = 5144,
    EQUIP_AXE               = 13913,
    EQUIP_MACE              = 19610,
    SWORD_EQUIP_INFO        = 218169346,
    AXE_EQUIP_INFO          = 218169346,
    MACE_EQUIP_INFO         = 50267394,

    SAY_PHASE_1             = -1036002,
    SAY_PHASE_2             = -1036003
};

struct boss_mr_smiteAI : public ScriptedAI
{
    boss_mr_smiteAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Initialize();
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }


    void Initialize()
    {
        uiTrashTimer = urand(5000, 9000);
        uiSlamTimer = 9000;
        uiNimbleReflexesTimer = urand(15500, 31600);

        uiHealth = 0;

        uiPhase = 0;
        uiTimer = 0;

        uiIsMoving = false;
    }


    ScriptedInstance* pInstance;

    uint32 uiTrashTimer;
    uint32 uiSlamTimer;
    uint32 uiNimbleReflexesTimer;

    uint8 uiHealth;

    uint32 uiPhase;
    uint32 uiTimer;

    bool uiIsMoving;

    void Reset()
    {
        Initialize();

        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, EQUIP_SWORD);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, SWORD_EQUIP_INFO);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + 1, 0);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO + 2, 0);
        me->SetStandState(UNIT_STAND_STATE_STAND);
        me->SetReactState(REACT_AGGRESSIVE);
    }

    void EnterCombat(Unit* /*pWho*/) {}

    bool bCheckChances()
    {
        uint32 uiChances = urand(0, 99);
        if (uiChances <= 15)
            return false;
        else
            return true;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
            return;

        if (!uiIsMoving) // halt abilities in between phases
        {
            if (uiTrashTimer <= uiDiff)
            {
                if (bCheckChances())
                    DoCast(me, SPELL_TRASH);
                uiTrashTimer = urand(6000, 15500);
            }
            else uiTrashTimer -= uiDiff;

            if (uiSlamTimer <= uiDiff)
            {
                if (bCheckChances())
                    DoCastVictim(SPELL_SMITE_SLAM);
                uiSlamTimer = 11000;
            }
            else uiSlamTimer -= uiDiff;

            if (uiNimbleReflexesTimer <= uiDiff)
            {
                if (bCheckChances())
                    DoCast(me, SPELL_NIMBLE_REFLEXES);
                uiNimbleReflexesTimer = urand(27300, 60100);
            }
            else uiNimbleReflexesTimer -= uiDiff;
        }

        if ((uiHealth == 0 && HealthBelowPct(66)) || (uiHealth == 1 && HealthBelowPct(33)))
        {
            ++uiHealth;
            DoCastAOE(SPELL_SMITE_STOMP, false);
            SetCombatMovement(false);
            me->AttackStop();
            me->InterruptNonMeleeSpells(false);
            me->SetReactState(REACT_PASSIVE);
            uiTimer = 2500;
            uiPhase = 1;
            switch (uiHealth)
            {
                case 1:
                    DoScriptText(SAY_PHASE_1, me);
                    break;
                case 2:
                    DoScriptText(SAY_PHASE_2, me);
                    break;
            }
        }

        if (uiPhase)
        {
            if (uiTimer <= uiDiff)
            {
                switch (uiPhase)
                {
                    case 1:
                    {
                        if (uiIsMoving)
                            break;

                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(1, 1.37994f, -780.29f, 9.81929f);
                        uiIsMoving = true;
                        break;
                    }
                    case 2:
                        if (uiHealth == 1)
                        {
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, EQUIP_AXE);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, AXE_EQUIP_INFO);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + 1, EQUIP_AXE);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO + 2, AXE_EQUIP_INFO);
                        }
                        else
                        {
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, EQUIP_MACE);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, MACE_EQUIP_INFO);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + 1, 0);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO + 2, 0);
                        }
                        uiTimer = 500;
                        uiPhase = 3;
                        break;
                    case 3:
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        uiTimer = 750;
                        uiPhase = 4;
                        break;
                    case 4:
                        me->SetReactState(REACT_AGGRESSIVE);
                        SetCombatMovement(true);
                        me->GetMotionMaster()->MoveChase(me->GetVictim(), me->m_CombatDistance);
                        uiIsMoving = false;
                        uiPhase = 0;
                        break;
                }
            } else uiTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }


    void MovementInform(uint32 uiType, uint32 /*uiId*/)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        me->SetFacingTo(5.47f);
        me->SetStandState(UNIT_STAND_STATE_KNEEL);

        uiTimer = 2000;
        uiPhase = 2;
    }

};

CreatureAI* GetAI_boss_mr_smite(Creature* pCreature)
{
    return new boss_mr_smiteAI (pCreature);
}

void AddSC_boss_mr_smite()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_mr_smite";
    newscript->GetAI = &GetAI_boss_mr_smite;
    newscript->RegisterSelf();
}
