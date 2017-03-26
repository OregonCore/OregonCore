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

struct boss_nexusprince_shaffarAI : public ScriptedAI
{
    boss_nexusprince_shaffarAI(Creature* c) : ScriptedAI(c) {}

    uint32 Blink_Timer;
    uint32 Beacon_Timer;
    uint32 FireBall_Timer;
    uint32 Frostbolt_Timer;
    uint32 FrostNova_Timer;

    Creature* Beacon[NR_INITIAL_BEACONS];

    bool HasTaunted;
    bool CanBlink;

    void RemoveBeaconFromList(Creature* targetBeacon)
    {
        for (uint8 i = 0; i < NR_INITIAL_BEACONS; i++)
            if (Beacon[i] && Beacon[i]->GetGUID() == targetBeacon->GetGUID())
                Beacon[i] = NULL;
    }

    void Reset()
    {
        Blink_Timer = 1500;
        Beacon_Timer = 10000;
        FireBall_Timer = 8000;
        Frostbolt_Timer = 4000;
        FrostNova_Timer = 15000;

        HasTaunted = false;
        CanBlink = false;

        float dist = 8.0f;
        float posX, posY, posZ, angle;
        me->GetHomePosition(posX, posY, posZ, angle);

        Beacon[0] = me->SummonCreature(ENTRY_BEACON, posX - dist, posY - dist, posZ, angle, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7200000);
        Beacon[1] = me->SummonCreature(ENTRY_BEACON, posX - dist, posY + dist, posZ, angle, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7200000);
        Beacon[2] = me->SummonCreature(ENTRY_BEACON, posX + dist, posY, posZ, angle, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7200000);

        for (uint8 i = 0; i < NR_INITIAL_BEACONS; i++)
        {
            if (Beacon[i])
                Beacon[i]->CastSpell(Beacon[i], SPELL_ETHEREAL_BEACON_VISUAL, false);
        }
    }

    void EnterEvadeMode()
    {
        //Despawn still living initial beacons.
        for (uint8 i = 0; i < NR_INITIAL_BEACONS; i++)
        {
            if (Beacon[i] && Beacon[i]->IsAlive())
            {
                Beacon[i]->RemoveAllAuras();
                Beacon[i]->CombatStop();
                Beacon[i]->StopMoving();
                Beacon[i]->Kill(Beacon[i]);
            }
        }
        ScriptedAI::EnterEvadeMode();
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (!me->GetVictim() && who->isTargetableForAttack() && (me->IsHostileTo(who)) && who->isInAccessiblePlaceFor (me))
        {
            if (!HasTaunted && me->IsWithinDistInMap(who, 100.0f))
            {
                DoScriptText(SAY_INTRO, me);
                HasTaunted = true;
            }

            if (!me->canFly() && me->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
                return;

            float attackRadius = me->GetAttackDistance(who);
            if (me->IsWithinDistInMap(who, attackRadius) && me->IsWithinLOSInMap(who))
            {
                //who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                AttackStart(who);
            }
        }
    }

    void EnterCombat(Unit* who)
    {
        switch (rand() % 3)
        {
        case 0:
            DoScriptText(SAY_AGGRO_1, me);
            break;
        case 1:
            DoScriptText(SAY_AGGRO_2, me);
            break;
        case 2:
            DoScriptText(SAY_AGGRO_3, me);
            break;
        }

        // Send initial beacons to join the fight if not already
        for (uint8 i = 0; i < NR_INITIAL_BEACONS; i++)
            if (Beacon[i] && Beacon[i]->IsAlive() && !Beacon[i]->IsInCombat())
                Beacon[i]->AI()->AttackStart(who);
    }

    void JustSummoned(Creature* summoned)
    {
        if (summoned->GetEntry() == ENTRY_BEACON)
        {
            summoned->CastSpell(summoned, SPELL_ETHEREAL_BEACON_VISUAL, false);

            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                summoned->AI()->AttackStart(pTarget);
        }
    }

    void KilledUnit(Unit*)
    {
        switch (rand() % 2)
        {
        case 0:
            DoScriptText(SAY_SLAY_1, me);
            break;
        case 1:
            DoScriptText(SAY_SLAY_2, me);
            break;
        }
    }

    void JustDied(Unit*)
    {
        DoScriptText(SAY_DEAD, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (FrostNova_Timer <= diff)
        {
            if (me->IsNonMeleeSpellCast(false))
                me->InterruptNonMeleeSpells(true);

            DoCast(me, SPELL_FROSTNOVA);
            FrostNova_Timer  = 17500 + rand() % 7500;
            CanBlink = true;
        }
        else FrostNova_Timer -= diff;

        if (Frostbolt_Timer <= diff)
        {
            DoCastVictim(SPELL_FROSTBOLT);
            Frostbolt_Timer = 4500 + rand() % 1500;
        }
        else Frostbolt_Timer -= diff;

        if (FireBall_Timer <= diff)
        {
            DoCastVictim(SPELL_FIREBALL);
            FireBall_Timer = 4500 + rand() % 1500;
        }
        else FireBall_Timer -= diff;

        if (CanBlink)
        {
            if (Blink_Timer <= diff)
            {
                if (me->IsNonMeleeSpellCast(false))
                    me->InterruptNonMeleeSpells(true);

                DoCast(me, SPELL_BLINK);
                Blink_Timer = 1000 + rand() % 1500;
                CanBlink = false;
            }
            else Blink_Timer -= diff;
        }

        if (Beacon_Timer <= diff)
        {
            if (me->IsNonMeleeSpellCast(false))
                me->InterruptNonMeleeSpells(true);

            if (!urand(0, 3))
                DoScriptText(SAY_SUMMON, me);

            DoCast(me, SPELL_ETHEREAL_BEACON);

            Beacon_Timer = 10000;
        }
        else Beacon_Timer -= diff;

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

    void JustDied(Unit*)
    {
        if (Creature* Shaffar = me->FindNearestCreature(ENTRY_SHAFFAR, 100))
            ((boss_nexusprince_shaffarAI*)(CAST_CRE(Shaffar)->AI()))->RemoveBeaconFromList(me);
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

