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
SDName: Boss_Felblood_Kaelthas
SD%Complete: 80
SDComment: Normal and Heroic Support. Issues: Arcane Spheres do not initially follow targets.
SDCategory: Magisters' Terrace
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "magisters_terrace.h"
#include "WorldPacket.h"
#include "Unit.h"

#define SAY_AGGRO                   -1585023                //This yell should be done when the room is cleared. For now, set it as a movelineofsight yell.
#define SAY_PHOENIX                 -1585024
#define SAY_FLAMESTRIKE             -1585025
#define SAY_GRAVITY_LAPSE           -1585026
#define SAY_TIRED                   -1585027
#define SAY_RECAST_GRAVITY          -1585028
#define SAY_DEATH                   -1585029

/*** Spells ***/

// Phase 1 spells
#define SPELL_FIREBALL_NORMAL         44189                 // Deals 2700-3300 damage at current target
#define SPELL_FIREBALL_HEROIC         46164                 //       4950-6050

#define SPELL_PHOENIX                 44194                 // Summons a phoenix (Doesn't work?)
#define SPELL_PHOENIX_BURN            44197                 // A spell Phoenix uses to damage everything around
#define SPELL_REBIRTH_DMG             44196                 // DMG if a Phoenix rebirth happen

#define SPELL_FLAMESTRIKE1_NORMAL     44190                 // Damage part
#define SPELL_FLAMESTRIKE1_HEROIC     46163                 // Heroic damage part
#define SPELL_FLAMESTRIKE2            44191                 // Flamestrike indicator before the damage
#define SPELL_FLAMESTRIKE3            44192                 // Summons the trigger + animation (projectile)

#define SPELL_SHOCK_BARRIER           46165                 // Heroic only; 10k damage shield, followed by Pyroblast
#define SPELL_PYROBLAST               36819                 // Heroic only; 45-55k fire damage

// Phase 2 spells
#define SPELL_GRAVITY_LAPSE_INITIAL   44224                 // Cast at the beginning of every Gravity Lapse
#define SPELL_GRAVITY_LAPSE_CHANNEL   44251                 // Channeled; blue beam animation to every enemy in range
#define SPELL_TELEPORT_CENTER         44218                 // Should teleport people to the center. Requires DB entry in spell_target_position.
#define SPELL_GRAVITY_LAPSE_FLY       44227                 // Hastens flyspeed and allows flying for 1 minute. For some reason removes 44226.
#define SPELL_GRAVITY_LAPSE_DOT       44226                 // Knocks up in the air and applies a 300 DPS DoT.
#define SPELL_ARCANE_SPHERE_PASSIVE   44263                 // Passive auras on Arcane Spheres
#define SPELL_POWER_FEEDBACK          44233                 // Stuns him, making him take 50% more damage for 10 seconds. Cast after Gravity Lapse
#define SPELL_GRAVITY_LAPSE_PLAYER    44219
#define SPELL_SUMMON_ARCANE_SPHERE    44265

/*** Creatures ***/
#define CREATURE_PHOENIX              24674
#define CREATURE_PHOENIX_EGG          24675
#define CREATURE_ARCANE_SPHERE        24708

enum Misc
{
    EVENT_INIT_COMBAT = 1,
    EVENT_SPELL_FIREBALL = 2,
    EVENT_SPELL_PHOENIX = 3,
    EVENT_SPELL_FLAMESTRIKE = 4,
    EVENT_SPELL_SHOCK_BARRIER = 5,
    EVENT_CHECK_HEALTH = 6,
    EVENT_GRAVITY_LAPSE_1_1 = 7,
    EVENT_GRAVITY_LAPSE_1_2 = 8,
    EVENT_GRAVITY_LAPSE_2 = 9,
    EVENT_GRAVITY_LAPSE_3 = 10,
    EVENT_GRAVITY_LAPSE_4 = 11,
    EVENT_GRAVITY_LAPSE_5 = 12,
    EVENT_FINISH_TALK = 13,

    ACTION_TELEPORT_PLAYERS = 1,
    ACTION_KNOCKUP = 2,
    ACTION_ALLOW_FLY = 3,
    ACTION_REMOVE_FLY = 4,
};

struct boss_felblood_kaelthasAI : public ScriptedAI
{
    boss_felblood_kaelthasAI(Creature* c) : ScriptedAI(c), summons(me)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        introSpeak = false;
    }

    ScriptedInstance* pInstance;
    EventMap events;
    EventMap events2;
    SummonList summons;
    bool introSpeak;


    void Reset()
    {
        events.Reset();
        summons.DespawnAll();
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, false);
        pInstance->SetData(DATA_KAELTHAS_EVENT, NOT_STARTED);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
    }

    void JustSummoned(Creature* summon)
    {
        for (SummonList::const_iterator itr = summons.begin(); itr != summons.end(); ++itr)
            if (*itr == summon->GetGUID())
                return;
        summons.Summon(summon);

        summon->SetInCombatWithZone();
    }

    void InitializeAI()
    {
        ScriptedAI::InitializeAI();
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
    }

    void JustDied(Unit* /*killer*/)
    {
        pInstance->SetData(DATA_KAELTHAS_EVENT, DONE);
    }

    void EnterCombat(Unit* /*who*/)
    {
        pInstance->SetData(DATA_KAELTHAS_EVENT, IN_PROGRESS);
        me->SetInCombatWithZone();

        events.ScheduleEvent(EVENT_SPELL_FIREBALL, 0);
        events.ScheduleEvent(EVENT_SPELL_PHOENIX, 15000);
        events.ScheduleEvent(EVENT_SPELL_FLAMESTRIKE, 22000);
        events.ScheduleEvent(EVENT_CHECK_HEALTH, 1000);

        if (me->GetMap()->IsHeroic())
            events.ScheduleEvent(EVENT_SPELL_SHOCK_BARRIER, 50000);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (!introSpeak && me->IsWithinDistInMap(who, 40.0f) && who->GetTypeId() == TYPEID_PLAYER)
        {
            DoScriptText(SAY_AGGRO, me);
            introSpeak = true;
            events2.ScheduleEvent(EVENT_INIT_COMBAT, 35000);
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void DamageTaken(Unit*, uint32& damage, DamageEffectType, SpellSchoolMask)
    {
        if (damage >= me->GetHealth())
        {
            damage = me->GetHealth() - 1;
            if (me->isRegeneratingHealth())
            {
                me->setRegeneratingHealth(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
                me->SetRooted(true);
                me->CombatStop();
                me->SetReactState(REACT_PASSIVE);
                LapseAction(ACTION_REMOVE_FLY);
                events.Reset();
                events2.ScheduleEvent(EVENT_FINISH_TALK, 6000);
            }
        }
    }

    void LapseAction(uint8 action)
    {
        uint8 counter = 0;
        Map::PlayerList const& playerList = me->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr, ++counter)
            if (Player* player = itr->GetSource())
            {
                if (action == ACTION_TELEPORT_PLAYERS)
                    me->CastSpell(player, SPELL_GRAVITY_LAPSE_PLAYER + counter, true);
                else if (action == ACTION_KNOCKUP)
                    player->CastSpell(player, SPELL_GRAVITY_LAPSE_DOT, true, NULL, NULL, me->GetGUID());
                else if (action == ACTION_ALLOW_FLY)
                {
                    player->CastSpell(player, SPELL_GRAVITY_LAPSE_FLY, true, NULL, NULL, me->GetGUID());
                }
                else if (action == ACTION_REMOVE_FLY)
                {
                    player->RemoveAurasDueToSpell(SPELL_GRAVITY_LAPSE_FLY);
                    player->RemoveAurasDueToSpell(SPELL_GRAVITY_LAPSE_DOT);
                }
            }
    }

    void UpdateAI(const uint32 diff)
    {
        events2.Update(diff);

        switch (events2.ExecuteEvent())
        {
        case EVENT_INIT_COMBAT:
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_IMMUNE_TO_NPC);
            if (Unit* target = SelectTargetFromPlayerList(50.0f, NULL, true))
                AttackStart(target);
            return;
        case EVENT_FINISH_TALK:
            me->Kill(me, false);
            return;
        }

        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        switch (uint32 eventId = events.ExecuteEvent())
        {
        case EVENT_SPELL_FIREBALL:
            me->CastSpell(me->GetVictim(), me->GetMap()->IsHeroic() ? SPELL_FIREBALL_NORMAL : SPELL_FIREBALL_HEROIC, false);
            events.ScheduleEvent(EVENT_SPELL_FIREBALL, urand(3000, 4500));
            break;
        case EVENT_SPELL_FLAMESTRIKE:
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
            {
                me->CastSpell(target, SPELL_FLAMESTRIKE3, true);
                DoScriptText(SAY_FLAMESTRIKE, me);
            }
            events.ScheduleEvent(EVENT_SPELL_FLAMESTRIKE, 25000);
            break;
        case EVENT_SPELL_SHOCK_BARRIER:
            me->CastSpell(me, SPELL_SHOCK_BARRIER, true);
            me->CastCustomSpell(SPELL_PYROBLAST, SPELLVALUE_MAX_TARGETS, 1, (Unit*)NULL, false);
            events.ScheduleEvent(EVENT_SPELL_SHOCK_BARRIER, 50000);
            break;
        case EVENT_SPELL_PHOENIX:
            DoScriptText(SAY_PHOENIX, me);
            me->CastSpell(me, SPELL_PHOENIX, false);
            events.ScheduleEvent(EVENT_SPELL_PHOENIX, 60000);
            break;
        case EVENT_CHECK_HEALTH:
            if (HealthBelowPct(50))
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
                me->CastSpell(me, SPELL_TELEPORT_CENTER, true);
                events.Reset();

                me->StopMoving();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();

                events.SetPhase(1);
                events.ScheduleEvent(EVENT_GRAVITY_LAPSE_1_1, 0);
                break;
            }
            events.ScheduleEvent(EVENT_CHECK_HEALTH, 500);
            break;
        case EVENT_GRAVITY_LAPSE_1_1:
        case EVENT_GRAVITY_LAPSE_1_2:
            DoScriptText(eventId == EVENT_GRAVITY_LAPSE_1_1 ? SAY_GRAVITY_LAPSE : SAY_RECAST_GRAVITY, me);
            me->CastSpell(me, SPELL_GRAVITY_LAPSE_INITIAL, false);
            events.ScheduleEvent(EVENT_GRAVITY_LAPSE_2, 2000);
            break;
        case EVENT_GRAVITY_LAPSE_2:
            LapseAction(ACTION_TELEPORT_PLAYERS);
            events.ScheduleEvent(EVENT_GRAVITY_LAPSE_3, 1000);
            break;
        case EVENT_GRAVITY_LAPSE_3:
            LapseAction(ACTION_KNOCKUP);
            events.ScheduleEvent(EVENT_GRAVITY_LAPSE_4, 1000);
            break;
        case EVENT_GRAVITY_LAPSE_4:
            LapseAction(ACTION_ALLOW_FLY);
            for (uint8 i = 0; i < 3; ++i)
                me->CastSpell(me, SPELL_SUMMON_ARCANE_SPHERE, true);

            me->CastSpell(me, SPELL_GRAVITY_LAPSE_CHANNEL, false);
            events.ScheduleEvent(EVENT_GRAVITY_LAPSE_5, 30000);
            break;
        case EVENT_GRAVITY_LAPSE_5:
            LapseAction(ACTION_REMOVE_FLY);
            me->InterruptNonMeleeSpells(false);
            DoScriptText(SAY_TIRED, me);
            me->CastSpell(me, SPELL_POWER_FEEDBACK, false);
            events.ScheduleEvent(EVENT_GRAVITY_LAPSE_1_2, 10000);
            break;
        }

        if (events.GetPhaseMask() == 0)
            DoMeleeAttackIfReady();

    }
};

struct mob_felkael_flamestrikeAI : public ScriptedAI
{
    mob_felkael_flamestrikeAI(Creature* c) : ScriptedAI(c)
    {
        Heroic = c->GetMap()->IsHeroic();
    }

    uint32 FlameStrikeTimer;
    bool Heroic;

    void Reset()
    {
        FlameStrikeTimer = 5000;

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetFaction(14);

        DoCast(me, SPELL_FLAMESTRIKE2, true);
    }

    void EnterCombat(Unit* /*who*/) {}
    void MoveInLineOfSight(Unit* /*who*/) {}
    void UpdateAI(const uint32 diff)
    {
        if (FlameStrikeTimer <= diff)
        {
            DoCast(me, Heroic ? SPELL_FLAMESTRIKE1_HEROIC : SPELL_FLAMESTRIKE1_NORMAL, true);
            me->Kill(me);
        }
        else FlameStrikeTimer -= diff;
    }
};

struct mob_felkael_phoenixAI : public ScriptedAI
{
    mob_felkael_phoenixAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    uint32 BurnTimer;
    uint32 Death_Timer;
    bool Rebirth;
    bool FakeDeath;

    void Reset()
    {
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE + UNIT_FLAG_NON_ATTACKABLE);
        me->SetLevitate(true);
        DoCast(me, SPELL_PHOENIX_BURN, true);
        BurnTimer = 2000;
        Death_Timer = 3000;
        Rebirth = false;
        FakeDeath = false;
    }

    void EnterCombat(Unit* /*who*/) {}

    void DamageTaken(Unit* /*pKiller*/, uint32& damage)
    {
        if (damage < me->GetHealth())
            return;

        //Prevent glitch if in fake death
        if (FakeDeath)
        {
            damage = 0;
            return;

        }
        //Don't really die in all phases of Kael'Thas
        if (pInstance && pInstance->GetData(DATA_KAELTHAS_EVENT) == 0)
        {
            //prevent death
            damage = 0;
            FakeDeath = true;

            me->InterruptNonMeleeSpells(false);
            me->SetHealth(0);
            me->StopMoving();
            me->ClearComboPointHolders();
            me->RemoveAllAurasOnDeath();
            me->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
            me->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->ClearAllReactives();
            me->SetUInt64Value(UNIT_FIELD_TARGET, 0);
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveIdle();
            me->SetStandState(UNIT_STAND_STATE_DEAD);
        }
    }

    void JustDied(Unit* /*slayer*/)
    {
        Position pos;
        me->GetPosition(&pos);

        me->SummonCreature(CREATURE_PHOENIX_EGG, pos, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 45000);
        me->DisappearAndDie(true);
    }

    void UpdateAI(const uint32 diff)
    {
        //If we are fake death, we cast revbirth and after that we kill the phoenix to spawn the egg.
        if (FakeDeath)
        {
            if (!Rebirth)
            {
                DoCast(me, SPELL_REBIRTH_DMG);
                Rebirth = true;
            }

            if (Rebirth)
            {

                if (Death_Timer <= diff)
                {
                    me->SummonCreature(CREATURE_PHOENIX_EGG, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 45000);
                    me->DisappearAndDie(true);
                    Rebirth = false;
                }
                else Death_Timer -= diff;
            }
        }

        if (!UpdateVictim())
            return;

        if (BurnTimer <= diff)
        {
            //spell Burn should possible do this, but it doesn't, so do this for now.
            uint16 dmg = urand(1650, 2050);
            me->DealDamage(me, dmg, 0, DOT, SPELL_SCHOOL_MASK_FIRE, NULL, false);
            BurnTimer += 2000;
        }
        BurnTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct mob_felkael_phoenix_eggAI : public ScriptedAI
{
    mob_felkael_phoenix_eggAI(Creature* c) : ScriptedAI(c) {}

    uint32 HatchTimer;

    void Reset()
    {
        HatchTimer = 10000;
    }

    void EnterCombat(Unit* /*who*/) {}
    void MoveInLineOfSight(Unit* /*who*/) {}

    void UpdateAI(const uint32 diff)
    {
        if (HatchTimer <= diff)
        {
            me->SummonCreature(CREATURE_PHOENIX, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000);
            me->Kill(me);
        }
        else HatchTimer -= diff;
    }
};

struct mob_arcane_sphereAI : public ScriptedAI
{
    mob_arcane_sphereAI(Creature* c) : ScriptedAI(c)
    {
        Reset();
    }

    uint32 DespawnTimer;
    uint32 ChangeTargetTimer;

    void Reset()
    {
        DespawnTimer = 30000;
        ChangeTargetTimer = urand(6000, 12000);

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetCanFly(true);
        me->SetFaction(14);
        DoCast(me, SPELL_ARCANE_SPHERE_PASSIVE, true);
        me->SetInCombatWithZone();
    }

    void EnterCombat(Unit* /*who*/) {}

    void UpdateAI(const uint32 diff)
    {
        if (DespawnTimer <= diff)
            me->Kill(me);
        else
            DespawnTimer -= diff;

        //Return since we have no target
        if (!UpdateVictim())
            return;

        if (ChangeTargetTimer <= diff)
        {
            if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
            {
                me->AddThreat(pTarget, 1.0f);
                me->TauntApply(pTarget);
                AttackStart(pTarget);
            }

            ChangeTargetTimer = urand(5000, 15000);
        }
        else ChangeTargetTimer -= diff;
    }
};

CreatureAI* GetAI_boss_felblood_kaelthas(Creature* c)
{
    return GetInstanceAI<boss_felblood_kaelthasAI>(c);
}

CreatureAI* GetAI_mob_arcane_sphere(Creature* c)
{
    return new mob_arcane_sphereAI(c);
}

CreatureAI* GetAI_mob_felkael_phoenix(Creature* c)
{
    return GetInstanceAI<mob_felkael_phoenixAI>(c);
}

CreatureAI* GetAI_mob_felkael_phoenix_egg(Creature* c)
{
    return new mob_felkael_phoenix_eggAI(c);
}

CreatureAI* GetAI_mob_felkael_flamestrike(Creature* c)
{
    return new mob_felkael_flamestrikeAI(c);
}

void AddSC_boss_felblood_kaelthas()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_felblood_kaelthas";
    newscript->GetAI = &GetAI_boss_felblood_kaelthas;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_arcane_sphere";
    newscript->GetAI = &GetAI_mob_arcane_sphere;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_felkael_phoenix";
    newscript->GetAI = &GetAI_mob_felkael_phoenix;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_felkael_phoenix_egg";
    newscript->GetAI = &GetAI_mob_felkael_phoenix_egg;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_felkael_flamestrike";
    newscript->GetAI = &GetAI_mob_felkael_flamestrike;
    newscript->RegisterSelf();
}
