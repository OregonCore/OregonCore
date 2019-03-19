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
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/* ScriptData
Name: Boss_Shirrak_the_dead_watcher
%Complete: 80
Comment: InhibitMagic should stack slower far from the boss, proper Visual for Focus Fire, heroic implemented
Category: Auchindoun, Auchenai Crypts
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#define SPELL_INHIBITMAGIC          32264
#define SPELL_ATTRACTMAGIC          32265
#define N_SPELL_CARNIVOROUSBITE     36383
#define H_SPELL_CARNIVOROUSBITE     39382
#define SPELL_CARNIVOROUSBITE       (HeroicMode?H_SPELL_CARNIVOROUSBITE:N_SPELL_CARNIVOROUSBITE)

#define ENTRY_FOCUS_FIRE            18374

#define N_SPELL_FIERY_BLAST         32302
#define H_SPELL_FIERY_BLAST         38382
#define SPELL_FIERY_BLAST           (HeroicMode?H_SPELL_FIERY_BLAST:N_SPELL_FIERY_BLAST)
#define SPELL_FOCUS_FIRE_VISUAL     42075 //need to find better visual

#define EMOTE_FOCUSES_ON            "focuses on "

struct boss_shirrak_the_dead_watcherAI : public ScriptedAI
{
    boss_shirrak_the_dead_watcherAI(Creature* c) : ScriptedAI(c)
    {
        HeroicMode = me->GetMap()->IsHeroic();
    }

    uint32 Inhibitmagic_Timer;
    uint32 Attractmagic_Timer;
    uint32 Carnivorousbite_Timer;
    uint32 FocusFire_Timer;
    bool HeroicMode;
    Unit* focusedTarget;

    void Reset()
    {
        Inhibitmagic_Timer = 0;
        Attractmagic_Timer = 28000;
        Carnivorousbite_Timer = 10000;
        FocusFire_Timer = 17000;
        focusedTarget = NULL;
    }

    void EnterCombat(Unit*)
    { }

    void JustSummoned(Creature* summoned)
    {
        if (summoned && summoned->GetEntry() == ENTRY_FOCUS_FIRE)
        {
            summoned->CastSpell(summoned, SPELL_FOCUS_FIRE_VISUAL, false);
            summoned->SetFaction(me->GetFaction());
            summoned->SetLevel(me->getLevel());
            summoned->AddUnitState(UNIT_STATE_ROOT);

            if (focusedTarget)
                summoned->AI()->AttackStart(focusedTarget);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Inhibitmagic_Timer
        if (Inhibitmagic_Timer <= diff)
        {
            float dist;
            Map* map = me->GetMap();
            Map::PlayerList const& PlayerList = map->GetPlayers();
            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                if (Player* i_pl = i->GetSource())
                    if (i_pl->IsAlive() && (dist = i_pl->GetDistance(me)) < 45)
                    {
                        i_pl->RemoveAurasDueToSpell(SPELL_INHIBITMAGIC);
                        me->AddAura(SPELL_INHIBITMAGIC, i_pl);
                        if (dist < 35)
                            me->AddAura(SPELL_INHIBITMAGIC, i_pl);
                        if (dist < 25)
                            me->AddAura(SPELL_INHIBITMAGIC, i_pl);
                        if (dist < 15)
                            me->AddAura(SPELL_INHIBITMAGIC, i_pl);
                    }
            Inhibitmagic_Timer = 3000 + (rand() % 1000);
        }
        else Inhibitmagic_Timer -= diff;

        //Return since we have no target
        if (!UpdateVictim())
            return;

        //Attractmagic_Timer
        if (Attractmagic_Timer <= diff)
        {
            DoCast(me, SPELL_ATTRACTMAGIC);
            Attractmagic_Timer = 30000;
            Carnivorousbite_Timer = 1500;
        }
        else Attractmagic_Timer -= diff;

        //Carnivorousbite_Timer
        if (Carnivorousbite_Timer <= diff)
        {
            DoCast(me, SPELL_CARNIVOROUSBITE);
            Carnivorousbite_Timer = 10000;
        }
        else Carnivorousbite_Timer -= diff;

        //FocusFire_Timer
        if (FocusFire_Timer <= diff)
        {
            // Summon Focus Fire & Emote
            Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 1);
            if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER && pTarget->IsAlive())
            {
                focusedTarget = pTarget;
                me->SummonCreature(ENTRY_FOCUS_FIRE, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 5500);

                // Emote
                std::string* emote = new std::string(EMOTE_FOCUSES_ON);
                emote->append(pTarget->GetName());
                emote->append("!");
                const char* text = emote->c_str();
                me->MonsterTextEmote(text, 0, true);
                delete emote;
            }
            FocusFire_Timer = 15000 + (rand() % 5000);
        }
        else FocusFire_Timer -= diff;

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
    uint32 FieryBlast_Timer;
    bool fiery1, fiery2;

    void Reset()
    {
        FieryBlast_Timer = 3000 + (rand() % 1000);
        fiery1 = fiery2 = true;
    }

    void EnterCombat(Unit*)
    { }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!UpdateVictim())
            return;

        //FieryBlast_Timer
        if (fiery2 && FieryBlast_Timer <= diff)
        {
            DoCast(me, SPELL_FIERY_BLAST);

            if (fiery1) fiery1 = false;
            else if (fiery2) fiery2 = false;

            FieryBlast_Timer = 1000;
        }
        else FieryBlast_Timer -= diff;

        //DoMeleeAttackIfReady();
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

