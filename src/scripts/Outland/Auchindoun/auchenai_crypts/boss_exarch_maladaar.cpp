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
SDName: Boss_Exarch_Maladaar
SD%Complete: 95
SDComment: Most of event implemented, some adjustments to timers remain and possibly make some better code for switching his dark side in to better "images" of player.
SDCategory: Auchindoun, Auchenai Crypts
EndScriptData */

/* ContentData
mob_stolen_soul
boss_exarch_maladaar
mob_avatar_of_martyred
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum ExarchMaladaar
{
    SAY_INTRO                   = -1558000,
    SAY_SUMMON                  = -1558001,
    SAY_AGGRO_1                 = -1558002,
    SAY_AGGRO_2                 = -1558003,
    SAY_AGGRO_3                 = -1558004,
    SAY_ROAR                    = -1558005,
    SAY_SOUL_CLEAVE             = -1558006,
    SAY_SLAY_1                  = -1558007,
    SAY_SLAY_2                  = -1558008,
    SAY_DEATH                   = -1558009,

    SPELL_RIBBON_OF_SOULS       = 32422,
    SPELL_SOUL_SCREAM           = 32421,
    SPELL_STOLEN_SOUL           = 32346,
    SPELL_STOLEN_SOUL_VISUAL    = 32395,
    SPELL_SUMMON_AVATAR         = 32424,

    ENTRY_STOLEN_SOUL           = 18441,

    EVENT_SPELL_FEAR = 1,
    EVENT_SPELL_RIBBON = 2,
    EVENT_SPELL_SOUL = 3,
    EVENT_CHECK_HEALTH = 4,
    EVENT_MORTALSTRIKE = 5
};

struct boss_exarch_maladaarAI : public ScriptedAI
{
    boss_exarch_maladaarAI(Creature* c) : ScriptedAI(c)
    {
        HasTaunted = false;
    }

    bool HasTaunted;
    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (!HasTaunted && me->IsWithinDistInMap(who, 150.0f) && who->GetTypeId() == TYPEID_PLAYER)
        {
            DoScriptText(SAY_INTRO, me);
            HasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }


    void EnterCombat(Unit*)
    {

        DoScriptText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);

        events.ScheduleEvent(EVENT_SPELL_FEAR, 15000);
        events.ScheduleEvent(EVENT_SPELL_RIBBON, 5000);
        events.ScheduleEvent(EVENT_SPELL_SOUL, 25000);
        events.ScheduleEvent(EVENT_CHECK_HEALTH, 5000);

    }

    void KilledUnit(Unit*)
    {

        DoScriptText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        if (rand() % 2)
            return;
    }

    void JustDied(Unit*)
    {
        DoScriptText(SAY_DEATH, me);
        //When Exarch Maladar is defeated D'ore appear.
        DoSpawnCreature(19412, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 600000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        switch (events.ExecuteEvent())
        {
        case EVENT_CHECK_HEALTH:
            if (HealthBelowPct(25))
            {
                DoScriptText(SAY_SUMMON, me);
                me->CastSpell(me, SPELL_SUMMON_AVATAR, false);
                return;
            }
            events.Repeat(2000);
            break;
        case EVENT_SPELL_SOUL:
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100.0f, true))
            {
                DoScriptText(SAY_ROAR, me);
                me->CastSpell(target, SPELL_STOLEN_SOUL, false);
                if (Creature* summon = me->SummonCreature(ENTRY_STOLEN_SOUL, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
                {
                    summon->CastSpell(summon, SPELL_STOLEN_SOUL_VISUAL, false);
                    summon->SetDisplayId(target->GetDisplayId());
                    summon->AI()->DoAction(target->getClass());
                    summon->AI()->AttackStart(target);
                }
            }
            events.Repeat(urand(25000, 30000));
            break;
        case EVENT_SPELL_RIBBON:
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                me->CastSpell(target, SPELL_RIBBON_OF_SOULS, false);
            events.Repeat(urand(10000, 20000));
            break;
        case EVENT_SPELL_FEAR:
            me->CastSpell(me, SPELL_SOUL_SCREAM, false);
            events.Repeat(urand(15000, 25000));
            break;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_exarch_maladaar(Creature* pCreature)
{
    return new boss_exarch_maladaarAI (pCreature);
}

#define SPELL_AV_MORTAL_STRIKE          16856
#define SPELL_AV_MORTAL_STRIKE          16856

struct mob_avatar_of_martyredAI : public ScriptedAI
{
    mob_avatar_of_martyredAI(Creature* c) : ScriptedAI(c) {}

    uint32 Mortal_Strike_timer;
    EventMap events;

    void Reset() {}

    void EnterCombat(Unit*)
    {
        Mortal_Strike_timer = 10000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
        case EVENT_MORTALSTRIKE:
            DoCastVictim(SPELL_AV_MORTAL_STRIKE);
            events.Repeat(10000 + rand() % 20 * 1000);
            break;
        }
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_avatar_of_martyred(Creature* pCreature)
{
    return new mob_avatar_of_martyredAI (pCreature);
}

enum stolenSoul
{
    SPELL_MOONFIRE = 37328,
    SPELL_FIREBALL = 37329,
    SPELL_MIND_FLAY = 37330,
    SPELL_HEMORRHAGE = 37331,
    SPELL_FROSTSHOCK = 37332,
    SPELL_CURSE_OF_AGONY = 37334,
    SPELL_MORTAL_STRIKE = 37335,
    SPELL_FREEZING_TRAP = 37368,
    SPELL_HAMMER_OF_JUSTICE = 37369,

    EVENT_STOLEN_SOUL_SPELL = 1,
};

struct mob_stolen_soulAI : public ScriptedAI
{
    mob_stolen_soulAI(Creature* c) : ScriptedAI(c) {}

    uint8 myClass;
    EventMap events;

    void Reset()
    {
        myClass = CLASS_WARRIOR;
        events.ScheduleEvent(EVENT_STOLEN_SOUL_SPELL, 1000);
    }

    void EnterCombat(Unit*) { }

    void SetMyClass(uint8 myclass)
    {
        myClass = myclass;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (events.ExecuteEvent() == EVENT_STOLEN_SOUL_SPELL)
        {
            switch (myClass)
            {
            case CLASS_WARRIOR:
                me->CastSpell(me->GetVictim(), SPELL_MORTAL_STRIKE, false);
                events.Repeat(6000);
                break;
            case CLASS_PALADIN:
                me->CastSpell(me->GetVictim(), SPELL_HAMMER_OF_JUSTICE, false);
                events.Repeat(6000);
                break;
            case CLASS_HUNTER:
                me->CastSpell(me->GetVictim(), SPELL_FREEZING_TRAP, false);
                events.Repeat(20000);
                break;
            case CLASS_ROGUE:
                me->CastSpell(me->GetVictim(), SPELL_HEMORRHAGE, false);
                events.Repeat(10000);
                break;
            case CLASS_PRIEST:
                me->CastSpell(me->GetVictim(), SPELL_MIND_FLAY, false);
                events.Repeat(5000);
                break;
            case CLASS_SHAMAN:
                me->CastSpell(me->GetVictim(), SPELL_FROSTSHOCK, false);
                events.Repeat(8000);
                break;
            case CLASS_MAGE:
                me->CastSpell(me->GetVictim(), SPELL_FIREBALL, false);
                events.Repeat(5000);
                break;
            case CLASS_WARLOCK:
                me->CastSpell(me->GetVictim(), SPELL_CURSE_OF_AGONY, false);
                events.Repeat(20000);
                break;
            case CLASS_DRUID:
                me->CastSpell(me->GetVictim(), SPELL_MOONFIRE, false);
                events.Repeat(10000);
                break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_stolen_soul(Creature* creature)
{
    return new mob_stolen_soulAI (creature);
}

void AddSC_boss_exarch_maladaar()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_exarch_maladaar";
    newscript->GetAI = &GetAI_boss_exarch_maladaar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_avatar_of_martyred";
    newscript->GetAI = &GetAI_mob_avatar_of_martyred;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_stolen_soul";
    newscript->GetAI = &GetAI_mob_stolen_soul;
    newscript->RegisterSelf();
}

