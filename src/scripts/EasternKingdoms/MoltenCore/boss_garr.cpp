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
SDName: Boss_Garr
SD%Complete: 50
SDComment: Adds NYI
SDCategory: Molten Core
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "molten_core.h"

enum Spells
{
    // Garr spells
    SPELL_ANTIMAGICPULSE       = 19492,
    SPELL_MAGMASHACKLES        = 19496,
    SPELL_ENRAGE               = 19516,     //Stacking enrage (stacks to 10 times)

    //Add spells
    SPELL_ERUPTION             = 19497,
    SPELL_MASSIVE_ERUPTION     = 20483,     // @todo possible on death
    SPELL_IMMOLATE             = 20294,
    SPELL_SEPARATION_ANXIETY   = 23492      // Used if separated too far from Garr
};

struct boss_garrAI : public ScriptedAI
{
    boss_garrAI(Creature* c) : ScriptedAI(c) { }

    uint32 antiMagicPulseTimer;
    uint32 magmaShacklesTimer;

    void Reset()
    {
        antiMagicPulseTimer = 25000;                       //These times are probably wrong
        magmaShacklesTimer = 15000;
    }

    void EnterCombat(Unit* /*who*/) { }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //antiMagicPulseTimer
        if (antiMagicPulseTimer <= diff)
        {
            DoCast(me, SPELL_ANTIMAGICPULSE);
            antiMagicPulseTimer = urand(10000, 15000);
        }
        else antiMagicPulseTimer -= diff;

        //magmaShacklesTimer
        if (magmaShacklesTimer <= diff)
        {
            DoCast(me, SPELL_MAGMASHACKLES);
            magmaShacklesTimer = urand(8000, 12000);
        }
        else magmaShacklesTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct mob_fireswornAI : public ScriptedAI
{
    mob_fireswornAI(Creature* c) : ScriptedAI(c)
    {
        instance = (ScriptedInstance*)c->GetInstanceData();
        Reset();
    }

    ScriptedInstance* instance;

    uint32 immolateTimer;
    uint32 seperationCheckTimer;

    void Reset()
    {
        immolateTimer = urand(4000, 8000);
        seperationCheckTimer = 5000;
    }

    void EnterCombat(Unit* /*who*/) { }

    void JustDied(Unit* /*pKiller*/)
    {
        if (instance)
            if (Creature* Garr = Creature::GetCreature(*me, instance->GetData64(DATA_GARR)))
                Garr->CastSpell(Garr, SPELL_ENRAGE, true, NULL, NULL, me->GetGUID());
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //immolateTimer
        if (immolateTimer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_IMMOLATE);

            immolateTimer = urand(5000, 10000);
        }
        else immolateTimer -= diff;

        //seperationCheckTimer
        if (seperationCheckTimer <= diff)
        {
            if (instance)
                return;

            Creature* Garr = Creature::GetCreature(*me, instance->GetData64(DATA_GARR));

             if (Garr && Garr->IsAlive() && !me->IsWithinDist2d(Garr->GetPositionX(), Garr->GetPositionY(), 50.0f))
                 DoCast(me, SPELL_SEPARATION_ANXIETY, true);

            seperationCheckTimer = 5000;
        }
        else seperationCheckTimer -= diff;

        //Cast Erruption and let them die
        if (me->GetHealthPct() <= 10.0f)
        {
            DoCastVictim(SPELL_ERUPTION);
            me->setDeathState(JUST_DIED);
            me->RemoveCorpse();
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_garr(Creature* pCreature)
{
    return new boss_garrAI (pCreature);
}

CreatureAI* GetAI_mob_firesworn(Creature* pCreature)
{
    return new mob_fireswornAI (pCreature);
}

void AddSC_boss_garr()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_garr";
    newscript->GetAI = &GetAI_boss_garr;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_firesworn";
    newscript->GetAI = &GetAI_mob_firesworn;
    newscript->RegisterSelf();
}

