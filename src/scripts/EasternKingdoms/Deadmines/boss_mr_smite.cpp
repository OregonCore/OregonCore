/*
* Copyright (C) 2008-2010 Trinity <http://www.trinitycore.org/>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

/* ScriptData
SDName: Boss Mr.Smite
SD%Complete:
SDComment: Timers and say taken from acid script
EndScriptData */

#include "ScriptPCH.h"
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

    NPC_DEFIAS_BLACKGUARD   = 636,

    SAY_PHASE_1             = -1036002,
    SAY_PHASE_2             = -1036003
};

struct boss_mr_smiteAI : public ScriptedAI
{
    boss_mr_smiteAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = pCreature->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 uiTrashTimer;
    uint32 uiSlamTimer;
    uint32 uiNimbleReflexesTimer;

    uint8 uiHealth;

    uint32 uiPhase;
    uint32 uiTimer;

    void Reset()
    {
        uiTrashTimer = urand(5000,9000);
        uiSlamTimer = 9000;
        uiNimbleReflexesTimer = urand(15500,31600);

        uiHealth = 0;

        uiPhase = 0;
        uiTimer = 0;

        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, EQUIP_SWORD);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, SWORD_EQUIP_INFO);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, 0);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+2, 0);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho && pWho->GetEntry() == NPC_DEFIAS_BLACKGUARD)
        {
            Unit* pTarget = pWho->getVictim();
            if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER)
            {
                if (pWho->isInCombat())
                    me->AI()->AttackStart(pTarget);
            }
        }
    }

    void EnterCombat(Unit* /*pWho*/) {}

    bool bCheckChances()
    {
        uint32 uiChances = urand(0,99);
        if (uiChances <= 15)
            return false;
        else
            return true;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
            return;

        if (uiPhase)
        {
            if (uiTimer <= uiDiff)
            {
                switch (uiPhase)
                {
                    case 1:
                        me->GetMotionMaster()->MovePoint(1,1.37994,-780.29,9.81929);
                        uiPhase = 2;
                        break;
                    case 3:
                        me->SetStandState(UNIT_STAND_STATE_KNEEL);
                        uiTimer = 2000;
                        uiPhase = 4;
                        break;
                    case 4:
                        if (uiHealth == 1)
                        {
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, EQUIP_AXE);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, AXE_EQUIP_INFO);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, EQUIP_AXE);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+2, AXE_EQUIP_INFO);
                        }
                        else
                        {
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, EQUIP_MACE);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, MACE_EQUIP_INFO);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, 0);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+2, 0);
                        }
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        uiTimer = 1000;
                        uiPhase = 5;
                        break;
                    case 5:
                        SetCombatMovement(true);
                        me->AI()->AttackStart(me->getVictim());
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        me->GetMotionMaster()->MoveChase(me->getVictim(), me->m_CombatDistance);
                        uiPhase = 0;
                        break;
                }
            } else uiTimer -= uiDiff;
        }

        if (uiPhase)
            return;

        /*START ACID-AI*/
        if (uiTrashTimer <= uiDiff)
        {
            if (bCheckChances())
                DoCast(me, SPELL_TRASH);
            uiTrashTimer = urand(6000,15500);
        } else uiTrashTimer -= uiDiff;

        if (uiSlamTimer <= uiDiff)
        {
            if (bCheckChances())
                DoCast(me->getVictim(), SPELL_SMITE_SLAM);
            uiSlamTimer = 11000;
        } else uiSlamTimer -= uiDiff;

        if (uiNimbleReflexesTimer <= uiDiff)
        {
            if (bCheckChances())
                DoCast(me, SPELL_NIMBLE_REFLEXES);
            uiNimbleReflexesTimer = urand(27300,60100);
        } else uiNimbleReflexesTimer -= uiDiff;
        /*END ACID-AI*/

        if ((uiHealth == 0 && me->GetHealth()*100 / me->GetMaxHealth() <= 66) || (uiHealth == 1 && me->GetHealth()*100 / me->GetMaxHealth() <= 33))
        {
            DoCastAOE(SPELL_SMITE_STOMP, false);
            if (uiHealth == 0)
                DoScriptText(SAY_PHASE_1, me);
            else
                DoScriptText(SAY_PHASE_2, me);

            ++uiHealth;
            if (pInstance)
                if (GameObject* pGo = GameObject::GetGameObject((*me),pInstance->GetData64(DATA_SMITE_CHEST)))
                {
                    SetCombatMovement(false);
                    me->GetMotionMaster()->Clear();
                    me->AttackStop();
                    me->SetReactState(REACT_PASSIVE);
                    uiPhase = 1;
                    uiTimer = 2500;
                }
        }
        else
            DoMeleeAttackIfReady();
    }

    void MovementInform(uint32 uiType, uint32 /*uiId*/)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        uiTimer = 500;
        uiPhase = 3;

        if (GameObject* pGo = GameObject::GetGameObject((*me),pInstance->GetData64(DATA_SMITE_CHEST)))
            me->SetFacingToObject(pGo);
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
