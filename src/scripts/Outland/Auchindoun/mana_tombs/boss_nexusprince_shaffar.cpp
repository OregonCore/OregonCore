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
SDName: Boss_NexusPrince_Shaffar
SD%Complete: 80
SDComment: Need more tuning of spell timers, it should not be as linear fight as current. Also should possibly find a better way to deal with his three initial beacons to make sure all aggro.
SDCategory: Auchindoun, Mana Tombs
EndScriptData */

/* ContentData
boss_nexusprince_shaffar
mob_ethereal_beacon
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#define SAY_INTRO                       -1557000
#define SAY_AGGRO_1                     -1557001
#define SAY_AGGRO_2                     -1557002
#define SAY_AGGRO_3                     -1557003
#define SAY_SLAY_1                      -1557004
#define SAY_SLAY_2                      -1557005
#define SAY_SUMMON                      -1557006
#define SAY_DEAD                        -1557007

#define SPELL_BLINK                     34605
#define SPELL_FROSTBOLT                 32370
#define SPELL_FIREBALL                  20420
#define SPELL_FROSTNOVA                 32365

#define SPELL_ETHEREAL_BEACON           32371               // Summon 18431
#define SPELL_ETHEREAL_BEACON_VISUAL    32368

#define ENTRY_BEACON                    18431
#define ENTRY_SHAFFAR                   18344

#define NR_INITIAL_BEACONS              3

enum events
{
    EVENT_SPELL_BEACON = 1,
    EVENT_SPELL_FR_FI = 2,
    EVENT_SPELL_FROST_NOVA = 3,
    EVENT_SPELL_BLINK = 4,
};

struct boss_nexusprince_shaffarAI : public ScriptedAI
{
    boss_nexusprince_shaffarAI(Creature* c) : ScriptedAI(c), summons(me) {}

    SummonList summons;
    EventMap events;
    bool HasTaunted;

    void Reset()
    {
        HasTaunted = false;

        float dist = 8.0f;
        float posX, posY, posZ, angle;
        me->GetHomePosition(posX, posY, posZ, angle);
        summons.DespawnAll();
        events.Reset();
        me->SummonCreature(ENTRY_BEACON, posX - dist, posY - dist, posZ, angle, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7200000);
        me->SummonCreature(ENTRY_BEACON, posX - dist, posY + dist, posZ, angle, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7200000);
        me->SummonCreature(ENTRY_BEACON, posX + dist, posY, posZ, angle, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7200000);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (!HasTaunted && who->GetTypeId() == TYPEID_PLAYER && me->IsWithinDistInMap(who, 100.0f))
        {
            DoScriptText(SAY_INTRO, me);
            HasTaunted = true;
        }
    }

    void EnterCombat(Unit* who)
    {

        DoScriptText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);
        me->SetInCombatWithZone();
        summons.DoZoneInCombat();

        events.ScheduleEvent(EVENT_SPELL_BEACON, 10000);
        events.ScheduleEvent(EVENT_SPELL_FR_FI, 4000);
        events.ScheduleEvent(EVENT_SPELL_FROST_NOVA, 15000);
    }

    void JustSummoned(Creature* summon)
    {
        if (summon->GetEntry() == ENTRY_BEACON)
        {
            summon->CastSpell(summon, SPELL_ETHEREAL_BEACON_VISUAL, false);

            if (me->IsInCombat())
                if (Unit* target = SelectTargetFromPlayerList(50.0f, NULL, true))
                    summon->AI()->AttackStart(target);
        }

        summons.Summon(summon);
    }

    void SummonedCreatureDespawn(Creature* summon)
    {
        summons.Despawn(summon);
    }

    void KilledUnit(Unit* victim)
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
        {
            DoScriptText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }
    }

    void JustDied(Unit*)
    {
        DoScriptText(SAY_DEAD, me);
        summons.DespawnAll();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;
        
        events.Update(diff);
        switch (events.ExecuteEvent())
        {
        case EVENT_SPELL_FROST_NOVA:
            me->CastSpell(me, SPELL_FROSTNOVA, false);
            events.Repeat(urand(16000, 23000));
            events.DelayEvents(1500);
            events.ScheduleEvent(EVENT_SPELL_BLINK, 1500);
            break;
        case EVENT_SPELL_FR_FI:
            me->CastSpell(me->GetVictim(), RAND(SPELL_FROSTBOLT, SPELL_FIREBALL), false);
            events.Repeat(urand(3000, 4000));
            break;
        case EVENT_SPELL_BLINK:
            me->CastSpell(me, SPELL_BLINK, false);
            events.RescheduleEvent(EVENT_SPELL_FR_FI, 0);
            break;
        case EVENT_SPELL_BEACON:
            if (!urand(0, 3))
                DoScriptText(SAY_SUMMON, me);

            me->CastSpell(me, SPELL_ETHEREAL_BEACON, true);
            events.Repeat(10000);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_nexusprince_shaffar(Creature* pCreature)
{
    return new boss_nexusprince_shaffarAI (pCreature);
}

#define SPELL_ARCANE_BOLT               15254
#define SPELL_ETHEREAL_APPRENTICE       32372               // Summon 18430

struct mob_ethereal_beaconAI : public ScriptedAI
{
    mob_ethereal_beaconAI(Creature* c) : ScriptedAI(c)
    {
        HeroicMode = me->GetMap()->IsHeroic();
    }

    bool HeroicMode;
    uint32 Apprentice_Timer;
    uint32 ArcaneBolt_Timer;
    uint32 Check_Timer;

    void KillSelf()
    {
        me->Kill(me);
    }

    void Reset()
    {
        Apprentice_Timer = (HeroicMode ? 10000 : 20000);
        ArcaneBolt_Timer = 1000;
        Check_Timer = 1000;
    }

    void EnterCombat(Unit* who)
    {
        // Send Shaffar to fight
        Creature* Shaffar = me->FindNearestCreature(ENTRY_SHAFFAR, 100);
        if (!Shaffar || Shaffar->isDead())
        {
            KillSelf();
            return;
        }
        if (!Shaffar->IsInCombat())
            Shaffar->AI()->AttackStart(who);
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->AI()->AttackStart(me->GetVictim());
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Check_Timer <= diff)
        {
            Creature* Shaffar = me->FindNearestCreature(ENTRY_SHAFFAR, 100);
            if (!Shaffar || Shaffar->isDead() || !Shaffar->IsInCombat())
            {
                KillSelf();
                return;
            }
            Check_Timer = 1000;
        }
        else Check_Timer -= diff;

        if (ArcaneBolt_Timer <= diff)
        {
            DoCastVictim(SPELL_ARCANE_BOLT);
            ArcaneBolt_Timer = 2000 + rand() % 2500;
        }
        else ArcaneBolt_Timer -= diff;

        if (Apprentice_Timer <= diff)
        {
            if (me->IsNonMeleeSpellCast(false))
                me->InterruptNonMeleeSpells(true);

            me->CastSpell(me, SPELL_ETHEREAL_APPRENTICE, true);
            if (me->IsPet())
                ((Pet*)me)->SetDuration(0);
            KillSelf();
            return;
        }
        else Apprentice_Timer -= diff;
    }
};

CreatureAI* GetAI_mob_ethereal_beacon(Creature* pCreature)
{
    return new mob_ethereal_beaconAI (pCreature);
}

#define SPELL_ETHEREAL_APPRENTICE_FIREBOLT          32369
#define SPELL_ETHEREAL_APPRENTICE_FROSTBOLT         32370

struct mob_ethereal_apprenticeAI : public ScriptedAI
{
    mob_ethereal_apprenticeAI(Creature* c) : ScriptedAI(c) {}

    uint32 Cast_Timer;

    bool isFireboltTurn;

    void Reset()
    {
        Cast_Timer = 3000;
        isFireboltTurn = true;
    }

    void EnterCombat(Unit*) {}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Cast_Timer <= diff)
        {
            if (isFireboltTurn)
            {
                me->CastSpell(me->GetVictim(), SPELL_ETHEREAL_APPRENTICE_FIREBOLT, true);
                isFireboltTurn = false;
            }
            else
            {
                me->CastSpell(me->GetVictim(), SPELL_ETHEREAL_APPRENTICE_FROSTBOLT, true);
                isFireboltTurn = true;
            }
            Cast_Timer = 3000;
        }
        else Cast_Timer -= diff;
    }
};

CreatureAI* GetAI_mob_ethereal_apprentice(Creature* pCreature)
{
    return new mob_ethereal_apprenticeAI (pCreature);
}

void AddSC_boss_nexusprince_shaffar()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_nexusprince_shaffar";
    newscript->GetAI = &GetAI_boss_nexusprince_shaffar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ethereal_beacon";
    newscript->GetAI = &GetAI_mob_ethereal_beacon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ethereal_apprentice";
    newscript->GetAI = &GetAI_mob_ethereal_apprentice;
    newscript->RegisterSelf();
}

