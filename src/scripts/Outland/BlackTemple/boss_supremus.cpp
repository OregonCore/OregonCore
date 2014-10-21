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
SDName: Boss_Supremus
SD%Complete: 95
SDComment: Need to implement molten punch
SDCategory: Black Temple
EndScriptData */

#include "ScriptPCH.h"
#include "black_temple.h"

#define EMOTE_NEW_TARGET            -1564010
#define EMOTE_PUNCH_GROUND          -1564011                //DoScriptText(EMOTE_PUNCH_GROUND, me);
#define EMOTE_GROUND_CRACK          -1564012

//Spells
#define SPELL_MOLTEN_PUNCH          40126
#define SPELL_HATEFUL_STRIKE        41926
#define SPELL_MOLTEN_FLAME          40980
#define SPELL_VOLCANIC_ERUPTION     40117
#define SPELL_VOLCANIC_SUMMON       40276
#define SPELL_BERSERK               45078
#define SPELL_CHARGE                41581

#define CREATURE_VOLCANO            23085
#define CREATURE_STALKER            23095

struct molten_flameAI : public NullCreatureAI
{
    molten_flameAI(Creature* c) : NullCreatureAI(c)
    {
        float x, y, z;
        me->GetNearPoint(me, x, y, z, 1, 50, M_PI*2*rand_norm());
        me->GetMotionMaster()->MovePoint(0, x, y, z);
    }
};

struct boss_supremusAI : public ScriptedAI
{
    boss_supremusAI(Creature* c) : ScriptedAI(c), summons(me)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 SummonFlameTimer;
    uint32 SwitchTargetTimer;
    uint32 PhaseSwitchTimer;
    uint32 SummonVolcanoTimer;
    uint32 HatefulStrikeTimer;
    uint32 BerserkTimer;

    bool Phase1;

    SummonList summons;

    void Reset()
    {
        if (pInstance)
        {
            if (me->isAlive())
            {
                pInstance->SetData(DATA_SUPREMUSEVENT, NOT_STARTED);
            }
        }

        HatefulStrikeTimer = 5000;
        SummonFlameTimer = 20000;
        SwitchTargetTimer = 90000;
        PhaseSwitchTimer = 60000;
        SummonVolcanoTimer = 5000;
        BerserkTimer = 900000;                              // 15 minute enrage

        Phase1 = true;
        summons.DespawnAll();

        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
        me->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, false);
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoZoneInCombat();

        if (pInstance)
            pInstance->SetData(DATA_SUPREMUSEVENT, IN_PROGRESS);
    }

    void ToggleDoors(bool close)
    {
        if (GameObject* Doors = GameObject::GetGameObject(*me, pInstance->GetData64(DATA_GAMEOBJECT_SUPREMUS_DOORS)))
        {
            if (close) Doors->SetGoState(GO_STATE_READY);                 // Closed
            else Doors->SetGoState(GO_STATE_ACTIVE);                      // Open
        }
    }

    void JustDied(Unit* /*killer*/)
    {
        if (pInstance)
        {
            pInstance->SetData(DATA_SUPREMUSEVENT, DONE);
            ToggleDoors(false);
        }
        summons.DespawnAll();
    }

    void JustSummoned(Creature* summon) {summons.Summon(summon);}
    void SummonedCreatureDespawn(Creature* summon) {summons.Despawn(summon);}

    Unit* CalculateHatefulStrikeTarget()
    {
        uint32 health = 0;
        Unit* pTarget = NULL;

        std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();
        std::list<HostileReference*>::iterator i = m_threatlist.begin();
        for (i = m_threatlist.begin(); i != m_threatlist.end();++i)
        {
            Unit* pUnit = Unit::GetUnit((*me), (*i)->getUnitGuid());
            if (pUnit && me->IsWithinMeleeRange(pUnit))
            {
                if (pUnit->GetHealth() > health)
                {
                    health = pUnit->GetHealth();
                    pTarget = pUnit;
                }
            }
        }

        return pTarget;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (!me->HasAura(SPELL_BERSERK, 0))
        {
            if (BerserkTimer <= diff)
                DoCast(me, SPELL_BERSERK);
            else BerserkTimer -= diff;
        }

        if (SummonFlameTimer <= diff)
        {
            DoCast(me, SPELL_MOLTEN_PUNCH);
            SummonFlameTimer = 10000;
        } else SummonFlameTimer -= diff;

        if (Phase1)
        {
            if (HatefulStrikeTimer <= diff)
            {
                if (Unit* pTarget = CalculateHatefulStrikeTarget())
                {
                    DoCast(pTarget, SPELL_HATEFUL_STRIKE);
                    HatefulStrikeTimer = 5000;
                }
            } else HatefulStrikeTimer -= diff;
        }

        if (!Phase1)
        {
            if (SwitchTargetTimer <= diff)
            {
                if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 1, 100, true))
                {
                    if (me->GetDistance2d(me->getVictim()) < 40)
                        me->CastSpell(me->getVictim(),SPELL_CHARGE,false);

                    DoResetThreat();
                    me->AddThreat(pTarget, 5000000.0f);
                    DoScriptText(EMOTE_NEW_TARGET, me);
                    SwitchTargetTimer = 10000;
                }
            } else SwitchTargetTimer -= diff;

            if (SummonVolcanoTimer <= diff)
            {
                if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 999, true))
                {
                    DoCast(pTarget, SPELL_VOLCANIC_SUMMON);
                    DoScriptText(EMOTE_GROUND_CRACK, me);
                    SummonVolcanoTimer = 10000;
                }
            } else SummonVolcanoTimer -= diff;
        }

        if (PhaseSwitchTimer <= diff)
        {
            if (!Phase1)
            {
                Phase1 = true;
                DoResetThreat();
                PhaseSwitchTimer = 60000;
                me->SetSpeed(MOVE_RUN, 1.2f);
                DoZoneInCombat();
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, false);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, false);
            }
            else
            {
                Phase1 = false;
                DoResetThreat();
                SwitchTargetTimer = 10000;
                SummonVolcanoTimer = 2000;
                PhaseSwitchTimer = 60000;
                me->SetSpeed(MOVE_RUN, 0.9f);
                DoZoneInCombat();
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT,SPELL_EFFECT_ATTACK_ME, true);
            }
        } else PhaseSwitchTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct npc_volcanoAI : public ScriptedAI
{
    npc_volcanoAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance *pInstance;

    uint32 CheckTimer;
    bool Eruption;

    void Reset()
    {
        CheckTimer = 3000;
        Eruption = false;

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void EnterCombat(Unit* /*who*/) {}

    void MoveInLineOfSight(Unit* /*who*/)
    {
        return; // paralyze the npc
    }

    void UpdateAI(const uint32 diff)
    {
        if (CheckTimer <= diff)
        {
            uint64 SupremusGUID = pInstance->GetData64(DATA_SUPREMUS);
            Creature* Supremus = (Unit::GetCreature((*me), SupremusGUID));
            if (!Eruption && Supremus && !((boss_supremusAI*)Supremus->AI())->Phase1)
            {
                Eruption = true;
                DoCast(me, SPELL_VOLCANIC_ERUPTION);
            }
            else if ((Eruption && Supremus && ((boss_supremusAI*)Supremus->AI())->Phase1) || !Supremus)
            {
                if (me->HasAura(SPELL_VOLCANIC_ERUPTION, 0))
                    me->RemoveAura(SPELL_VOLCANIC_ERUPTION, 0);
            }
            CheckTimer = 1500;
        } else CheckTimer -= diff;
    }
};

CreatureAI* GetAI_boss_supremus(Creature* pCreature)
{
    return new boss_supremusAI (pCreature);
}

CreatureAI* GetAI_molten_flame(Creature* pCreature)
{
    return new molten_flameAI (pCreature);
}

CreatureAI* GetAI_npc_volcano(Creature* pCreature)
{
    return new npc_volcanoAI (pCreature);
}

void AddSC_boss_supremus()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_supremus";
    newscript->GetAI = &GetAI_boss_supremus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "molten_flame";
    newscript->GetAI = &GetAI_molten_flame;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_volcano";
    newscript->GetAI = &GetAI_npc_volcano;
    newscript->RegisterSelf();
}

