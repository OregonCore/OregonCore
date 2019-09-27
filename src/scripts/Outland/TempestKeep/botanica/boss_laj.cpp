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
SDName: Boss_Laj
SD%Complete: 90
SDComment: Immunities are wrong, must be adjusted to use resistance from creature_templates. Most spells require database support.
SDCategory: Tempest Keep, The Botanica
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"


enum spells
{
    SPELL_ALLERGIC_REACTION     = 34697,
    SPELL_TELEPORT_SELF         = 34673,

    SPELL_SUMMON_LASHER_1       = 34681,
    SPELL_SUMMON_FLAYER_1       = 34682,
    SPELL_SUMMON_LASHER_2       = 34684,
    SPELL_SUMMON_FLAYER_2       = 34685,
    SPELL_SUMMON_LASHER_3       = 34686,
    SPELL_SUMMON_FLAYER_4       = 34687,
    SPELL_SUMMON_LASHER_4       = 34688,
    SPELL_SUMMON_FLAYER_3       = 34690,

    SPELL_DAMAGE_IMMUNE_ARCANE  = 34304,
    SPELL_DAMAGE_IMMUNE_FIRE    = 34305,
    SPELL_DAMAGE_IMMUNE_FROST   = 34306,
    SPELL_DAMAGE_IMMUNE_NATURE  = 34308,
    SPELL_DAMAGE_IMMUNE_SHADOW  = 34309
};

enum Misc
{
    EMOTE_SUMMON                    = -1553006,

    MODEL_DEFAULT                   = 13109,
    MODEL_ARCANE                    = 14213,
    MODEL_FIRE                      = 13110,
    MODEL_FROST                     = 14112,
    MODEL_NATURE                    = 14214,


    EVENT_ALERGIC_REACTION          = 1,
    EVENT_TRANSFORM                 = 2,
    EVENT_TELEPORT                  = 3,
    EVENT_SUMMON                    = 4
};

struct boss_lajAI : public ScriptedAI
{
    boss_lajAI(Creature* c) : ScriptedAI(c) {}

    EventMap events;

    void Reset()
    {
        events.Reset();
        me->SetDisplayId(MODEL_DEFAULT);
        _lastTransform = SPELL_SCHOOL_MASK_SHADOW;
        me->CastSpell(me, SPELL_DAMAGE_IMMUNE_SHADOW, true);
    }

    void DoTransform()
    {
        me->RemoveAurasDueToSpell(_lastTransform);

        switch (_lastTransform = RAND(SPELL_DAMAGE_IMMUNE_SHADOW, SPELL_DAMAGE_IMMUNE_FIRE, SPELL_DAMAGE_IMMUNE_FROST, SPELL_DAMAGE_IMMUNE_NATURE, SPELL_DAMAGE_IMMUNE_ARCANE))
        {
            case SPELL_DAMAGE_IMMUNE_SHADOW: me->SetDisplayId(MODEL_DEFAULT); break;
            case SPELL_DAMAGE_IMMUNE_ARCANE: me->SetDisplayId(MODEL_ARCANE); break;
            case SPELL_DAMAGE_IMMUNE_FIRE: me->SetDisplayId(MODEL_FIRE); break;
            case SPELL_DAMAGE_IMMUNE_FROST: me->SetDisplayId(MODEL_FROST); break;
            case SPELL_DAMAGE_IMMUNE_NATURE: me->SetDisplayId(MODEL_NATURE); break;
        }
        me->CastSpell(me, _lastTransform, true);
    }

    void EnterCombat(Unit* /*who*/)
    {
        events.ScheduleEvent(EVENT_ALERGIC_REACTION, 5000);
        events.ScheduleEvent(EVENT_TRANSFORM, 30000);
        events.ScheduleEvent(EVENT_TELEPORT, 20000);
    }

    void JustSummoned(Creature* summon)
    {
        if (summon && me->GetVictim())
            summon->AI()->AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
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
        case EVENT_ALERGIC_REACTION:
            me->CastSpell(me->GetVictim(), SPELL_ALLERGIC_REACTION, false);
            events.ScheduleEvent(EVENT_ALERGIC_REACTION, 25000);
            break;
        case EVENT_TELEPORT:
            me->CastSpell(me, SPELL_TELEPORT_SELF, false);
            events.ScheduleEvent(EVENT_SUMMON, 2500);
            events.ScheduleEvent(EVENT_TELEPORT, 30000);
            break;
        case EVENT_SUMMON:
            DoScriptText(EMOTE_SUMMON, me);
            me->CastSpell(me, SPELL_SUMMON_LASHER_1, true);
            me->CastSpell(me, SPELL_SUMMON_FLAYER_1, true);
            break;
        case EVENT_TRANSFORM:
            DoTransform();
            events.ScheduleEvent(EVENT_TRANSFORM, 35000);
            break;
        }

        DoMeleeAttackIfReady();
    }
    private: 
        uint32 _lastTransform;
};

CreatureAI* GetAI_boss_laj(Creature* pCreature)
{
    return new boss_lajAI (pCreature);
}

void AddSC_boss_laj()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_laj";
    newscript->GetAI = &GetAI_boss_laj;
    newscript->RegisterSelf();
}

