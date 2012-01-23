/*
 * Copyright (C) 2010-2012 OregonCore <http://www.oregoncore.com/>
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2012 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Boss_Maexxna
SD%Complete: 80
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "ScriptPCH.h"

#define SPELL_WEBTRAP           28622                       //Spell is normally used by the webtrap on the wall NOT by Maexxna
#define SPELL_WEBSPRAY          29484
#define H_SPELL_WEBSPRAY        54125
#define SPELL_POISONSHOCK       28741
#define H_SPELL_POISONSHOCK     54122
#define SPELL_NECROTICPOISON    28776
#define H_SPELL_NECROTICPOISON  54121
#define SPELL_FRENZY            54123
#define H_SPELL_FRENZY          54124

#define SPELL_SUMMON_SPIDERLING 29434

#define LOC_X1    3546.796f
#define LOC_Y1    -3869.082f
#define LOC_Z1    296.450f

#define LOC_X2    3531.271f
#define LOC_Y2    -3847.424f
#define LOC_Z2    299.450f

#define LOC_X3    3497.067f
#define LOC_Y3    -3843.384f
#define LOC_Z3    302.384f

struct mob_webwrapAI : public ScriptedAI
{
    mob_webwrapAI(Creature *c) : ScriptedAI(c) {}

    uint64 victimGUID;

    void Reset()
    {
        victimGUID = 0;
    }

    void SetVictim(Unit* victim)
    {
        if (victim)
        {
            victimGUID = victim->GetGUID();
            victim->CastSpell(victim, SPELL_WEBTRAP, true);
        }
    }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (damage > me->GetHealth())
        {
            if (victimGUID)
            {
                Unit* victim = NULL;
                victim = Unit::GetUnit((*me), victimGUID);
                if (victim)
                    victim->RemoveAurasDueToSpell(SPELL_WEBTRAP);
            }
        }
    }

    void EnterCombat(Unit *who)
    {
    }

    void MoveInLineOfSight(Unit *who)
    {
    }

    void UpdateAI(const uint32 diff)
    {
    }
};

struct boss_maexxnaAI : public ScriptedAI
{
    boss_maexxnaAI(Creature *c) : ScriptedAI(c) {}

    uint32 WebTrap_Timer;
    uint32 WebSpray_Timer;
    uint32 PoisonShock_Timer;
    uint32 NecroticPoison_Timer;
    uint32 SummonSpiderling_Timer;
    bool Enraged;

    void Reset()
    {
        WebTrap_Timer = 20000;                              //20 sec init, 40 sec normal
        WebSpray_Timer = 40000;                             //40 seconds
        PoisonShock_Timer = 20000;                          //20 seconds
        NecroticPoison_Timer = 30000;                       //30 seconds
        SummonSpiderling_Timer = 30000;                     //30 sec init, 40 sec normal
        Enraged = false;
    }

    void EnterCombat(Unit *who)
    {
    }

    void DoCastWebWrap()
    {
        std::list<HostileReference *> t_list = me->getThreatManager().getThreatList();
        std::vector<Unit *> targets;

        //This spell doesn't work if we only have 1 player on threat list
        if (t_list.size() < 2)
            return;

        //begin + 1 , so we don't target the one with the highest threat
        std::list<HostileReference *>::iterator itr = t_list.begin();
        std::advance(itr, 1);
        for (; itr != t_list.end(); ++itr)                   //store the threat list in a different container
        {
            Unit *pTarget = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                                                            //only on alive players
            if (pTarget && pTarget->isAlive() && pTarget->GetTypeId() == TYPEID_PLAYER)
                targets.push_back(pTarget);
        }

        while (targets.size() > 3)
                                                            //cut down to size if we have more than 3 targets
            targets.erase(targets.begin()+rand()%targets.size());

        int i = 0;
        for (std::vector<Unit *>::iterator itr = targets.begin(); itr != targets.end(); ++itr, ++i)
        {
            // Teleport the 3 targets to a location on the wall and summon a Web Wrap on them
            Unit *pTarget = *itr;
            Creature* Wrap = NULL;
            if (pTarget)
            {
                switch(i)
                {
                    case 0:
                        DoTeleportPlayer(pTarget, LOC_X1, LOC_Y1, LOC_Z1, pTarget->GetOrientation());
                        Wrap = me->SummonCreature(16486, LOC_X1, LOC_Y1, LOC_Z1, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);
                        break;
                    case 1:
                        DoTeleportPlayer(pTarget, LOC_X2, LOC_Y2, LOC_Z2, pTarget->GetOrientation());
                        Wrap = me->SummonCreature(16486, LOC_X2, LOC_Y2, LOC_Z2, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);
                        break;
                    case 2:
                        DoTeleportPlayer(pTarget, LOC_X3, LOC_Y3, LOC_Z3, pTarget->GetOrientation());
                        Wrap = me->SummonCreature(16486, LOC_X3, LOC_Y3, LOC_Z3, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);
                        break;
                }
                if (Wrap)
                {
                    Wrap->setFaction(me->getFaction());
                    ((mob_webwrapAI*)Wrap->AI())->SetVictim(pTarget);
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //WebTrap_Timer
        if (WebTrap_Timer <= diff)
        {
            DoCastWebWrap();
            WebTrap_Timer = 40000;
        } else WebTrap_Timer -= diff;

        //WebSpray_Timer
        if (WebSpray_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_WEBSPRAY);
            WebSpray_Timer = 40000;
        } else WebSpray_Timer -= diff;

        //PoisonShock_Timer
        if (PoisonShock_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_POISONSHOCK);
            PoisonShock_Timer = 20000;
        } else PoisonShock_Timer -= diff;

        //NecroticPoison_Timer
        if (NecroticPoison_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_NECROTICPOISON);
            NecroticPoison_Timer = 30000;
        } else NecroticPoison_Timer -= diff;

        //SummonSpiderling_Timer
        if (SummonSpiderling_Timer <= diff)
        {
            DoCast(me, SPELL_SUMMON_SPIDERLING);
            SummonSpiderling_Timer = 40000;
        } else SummonSpiderling_Timer -= diff;

        //Enrage if not already enraged and below 30%
        if (!Enraged && (me->GetHealth()*100 / me->GetMaxHealth()) < 30)
        {
            DoCast(me,SPELL_FRENZY);
            Enraged = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_webwrap(Creature* pCreature)
{
    return new mob_webwrapAI (pCreature);
}

CreatureAI* GetAI_boss_maexxna(Creature* pCreature)
{
    return new boss_maexxnaAI (pCreature);
}

void AddSC_boss_maexxna()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_maexxna";
    newscript->GetAI = &GetAI_boss_maexxna;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_webwrap";
    newscript->GetAI = &GetAI_mob_webwrap;
    newscript->RegisterSelf();
}

