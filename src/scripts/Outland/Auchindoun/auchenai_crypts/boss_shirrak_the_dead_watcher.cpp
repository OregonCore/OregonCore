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
Name: Boss_Shirrak_the_dead_watcher
%Complete: 80
Comment: InhibitMagic should stack slower far from the boss, proper Visual for Focus Fire, heroic implemented
Category: Auchindoun, Auchenai Crypts
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum eShirrak
{
    SPELL_INHIBIT_MAGIC             = 32264,
    SPELL_ATTRACT_MAGIC             = 32265,
    SPELL_CARNIVOROUS_BITE_N        = 36383,
    SPELL_CARNIVOROUS_BITE_H        = 39382,

    SPELL_FIERY_BLAST_N             = 32302,
    SPELL_FIERY_BLAST_H             = 38382,
    SPELL_FOCUS_FIRE_VISUAL         = 32286,
    SPELL_FOCUS_CAST                = 32300,

    EVENT_SPELL_INHIBIT_MAGIC       = 1,
    EVENT_SPELL_ATTRACT_MAGIC       = 2,
    EVENT_SPELL_CARNIVOROUS         = 3,
    EVENT_SPELL_FOCUS_FIRE          = 4,
    EVENT_SPELL_FOCUS_FIRE_2        = 5,
    EVENT_SPELL_FOCUS_FIRE_3        = 6,
    EVENT_SPELL_FIRE_BLAST          = 7,

    ENTRY_FOCUS_FIRE                = 18374,

};

struct boss_shirrak_the_dead_watcherAI : public ScriptedAI
{
    boss_shirrak_the_dead_watcherAI(Creature* c) : ScriptedAI(c)
    {
        HeroicMode = me->GetMap()->IsHeroic();
    }

    EventMap events;
    uint64 focusGUID;
    bool HeroicMode;
    std::ostringstream emote;

    void Reset()
    {
        events.Reset();
        focusGUID = 0;
        me->SetControlled(false, UNIT_STATE_ROOT);
    }

    void EnterEvadeMode()
    {
        me->SetControlled(false, UNIT_STATE_ROOT);
        ScriptedAI::EnterEvadeMode();
    }

    void EnterCombat(Unit*)
    { 
        events.ScheduleEvent(EVENT_SPELL_INHIBIT_MAGIC, 0);
        events.ScheduleEvent(EVENT_SPELL_ATTRACT_MAGIC, 28000);
        events.ScheduleEvent(EVENT_SPELL_CARNIVOROUS, 10000);
        events.ScheduleEvent(EVENT_SPELL_FOCUS_FIRE, 17000);
    }

    void JustSummoned(Creature* summon)
    {
        summon->CastSpell(summon, SPELL_FOCUS_FIRE_VISUAL, true);
    }

    void SpellHitTarget(Unit* target, const SpellEntry* spellInfo)
    {
        if (spellInfo->Id == SPELL_FOCUS_CAST)
            target->CastSpell(target, HeroicMode ? SPELL_FIERY_BLAST_H :  SPELL_FIERY_BLAST_N, false);
    }

    uint8 getStackCount(float dist)
    {
        if (dist < 15)
            return 4;
        if (dist < 25)
            return 3;
        if (dist < 35)
            return 2;
        return 1;
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!UpdateVictim())
            return;

        events.Update(diff);
        
        switch (events.ExecuteEvent())
        {
        case EVENT_SPELL_INHIBIT_MAGIC:
        {
            Map::PlayerList const& PlayerList = me->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                if (Player* i_pl = i->GetSource())
                {
                    float dist = me->GetDistance(i_pl);
                    if (i_pl->IsAlive() && dist < 45.0f)
                    {
                        Aura* aura = i_pl->GetAura(SPELL_INHIBIT_MAGIC, NULL);
                        if (!aura)
                            me->AddAura(SPELL_INHIBIT_MAGIC, i_pl);

                        if (aura)
                            aura->SetStackAmount(getStackCount(dist));
                    }
                    else
                        i_pl->RemoveAurasDueToSpell(SPELL_INHIBIT_MAGIC);
                }
            events.Repeat(3000);
            break;
        }
        case EVENT_SPELL_ATTRACT_MAGIC:
            me->CastSpell(me, SPELL_ATTRACT_MAGIC, false);
            events.Repeat(30000);
            events.RescheduleEvent(EVENT_SPELL_CARNIVOROUS, 1500);
            break;

        case EVENT_SPELL_CARNIVOROUS:
            me->CastSpell(me, HeroicMode ? SPELL_CARNIVOROUS_BITE_H : SPELL_CARNIVOROUS_BITE_N, false);
            events.Repeat(10000);
            break;
        case EVENT_SPELL_FOCUS_FIRE:
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 60.0f, true))
            {
                if (Creature* cr = me->SummonCreature(ENTRY_FOCUS_FIRE, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 7000))
                    focusGUID = cr->GetGUID();
            }
            emote << me->GetName() << " Is Focusing on " << me->GetVictim()->GetName();
            me->MonsterTextEmote(emote.str().c_str(), 0, true);
            events.Repeat(urand(15000, 20000));
            events.ScheduleEvent(EVENT_SPELL_FOCUS_FIRE_2, 3000);
            events.ScheduleEvent(EVENT_SPELL_FOCUS_FIRE_2, 3500);
            events.ScheduleEvent(EVENT_SPELL_FOCUS_FIRE_2, 4000);
            events.ScheduleEvent(EVENT_SPELL_FOCUS_FIRE_3, 5000);
            me->SetControlled(true, UNIT_STATE_ROOT);
            break;
        case EVENT_SPELL_FOCUS_FIRE_2:
            if (Unit* flare = ObjectAccessor::GetCreature(*me, focusGUID))
                me->CastSpell(flare, SPELL_FOCUS_CAST, true);
            break;
        case EVENT_SPELL_FOCUS_FIRE_3:
            me->SetControlled(false, UNIT_STATE_ROOT);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_shirrak_the_dead_watcher(Creature* pCreature)
{
    return new boss_shirrak_the_dead_watcherAI (pCreature);
}

struct mob_focus_fireAI : public ScriptedAI
{
    mob_focus_fireAI(Creature* c) : ScriptedAI(c)
    {
        HeroicMode = me->GetMap()->IsHeroic();
    }

    bool HeroicMode;
    EventMap events;

    void EnterCombat(Unit*)
    { 
        events.ScheduleEvent(EVENT_SPELL_FIRE_BLAST, 3000 + (rand() % 1000));
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
        case EVENT_SPELL_FIRE_BLAST:
            DoCast(me, HeroicMode ? SPELL_FIERY_BLAST_H : SPELL_FIERY_BLAST_N, false);
            events.Repeat(1000);
            break;
        }
    }
};

CreatureAI* GetAI_mob_focus_fire(Creature* pCreature)
{
    return new mob_focus_fireAI (pCreature);
}

void AddSC_boss_shirrak_the_dead_watcher()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_shirrak_the_dead_watcher";
    newscript->GetAI = &GetAI_boss_shirrak_the_dead_watcher;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_focus_fire";
    newscript->GetAI = &GetAI_mob_focus_fire;
    newscript->RegisterSelf();
}

