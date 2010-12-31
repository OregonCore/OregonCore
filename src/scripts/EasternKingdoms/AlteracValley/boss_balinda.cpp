/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Boss_Balinda
SD%Complete:
SDComment:
EndScriptData */

#include "ScriptPCH.h"

enum Spells
{
    SPELL_ARCANE_EXPLOSION                        = 46608,
    SPELL_CONE_OF_COLD                            = 38384,
    SPELL_FIREBALL                                = 46988,
    SPELL_FROSTBOLT                               = 46987
};

enum Yells
{
    YELL_AGGRO                                    = -1810019,
    YELL_EVADE                                    = -1810020
};

struct boss_balindaAI : public ScriptedAI
{
    boss_balindaAI(Creature *c) : ScriptedAI(c) {}

    uint32 uiArcaneExplosionTimer;
    uint32 uiConeOfColdTimer;
    uint32 uiFireBoltTimer;
    uint32 uiFrostboltTimer;
    uint32 uiResetTimer;

    void Reset()
    {
        uiArcaneExplosionTimer      = urand(5*IN_MILLISECONDS,15*IN_MILLISECONDS);
        uiConeOfColdTimer           = 8*IN_MILLISECONDS;
        uiFireBoltTimer             = 1*IN_MILLISECONDS;
        uiFrostboltTimer            = 4*IN_MILLISECONDS;
        uiResetTimer                = 5*IN_MILLISECONDS;
    }

    void EnterCombat(Unit * /*who*/)
    {
        DoScriptText(YELL_AGGRO, me);
    }

    void JustRespawned()
    {
        Reset();
    }

    void KilledUnit(Unit* victim){}

    void JustDied(Unit* Killer){}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (uiArcaneExplosionTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_ARCANE_EXPLOSION);
            uiArcaneExplosionTimer =  urand(5*IN_MILLISECONDS,15*IN_MILLISECONDS);
        } else uiArcaneExplosionTimer -= diff;

        if (uiConeOfColdTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_CONE_OF_COLD);
            uiConeOfColdTimer = urand(10*IN_MILLISECONDS,20*IN_MILLISECONDS);
        } else uiConeOfColdTimer -= diff;

        if (uiFireBoltTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_FIREBALL);
            uiFireBoltTimer = urand(5*IN_MILLISECONDS,9*IN_MILLISECONDS);
        } else uiFireBoltTimer -= diff;

        if (uiFrostboltTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_FROSTBOLT);
            uiFrostboltTimer = urand(4*IN_MILLISECONDS,12*IN_MILLISECONDS);
        } else uiFrostboltTimer -= diff;


        // check if creature is not outside of building
        if (uiResetTimer <= diff)
        {
            if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 50)
            {
                EnterEvadeMode();
                DoScriptText(YELL_EVADE, me);
            }
            uiResetTimer = 5*IN_MILLISECONDS;
        } else uiResetTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_balinda(Creature* pCreature)
{
    return new boss_balindaAI (pCreature);
}

void AddSC_boss_balinda()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_balinda";
    newscript->GetAI = &GetAI_boss_balinda;
    newscript->RegisterSelf();
}
