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
SDName: Boss_Warlord_Kalithres
SD%Complete: 65
SDComment: Contains workarounds regarding warlord's rage spells not acting as expected. Both scripts here require review and fine tuning.
SDCategory: Coilfang Resevoir, The Steamvault
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "steam_vault.h"
#include "GridNotifiers.h"
#include "Cell.h"
#include "CellImpl.h"
                               
enum NagaDistiller
{
    SAY_INTRO                       = -1545016,
    SAY_REGEN                       = -1545017,
    SAY_AGGRO1                      = -1545018,
    SAY_AGGRO2                      = -1545019,
    SAY_AGGRO3                      = -1545020,
    SAY_SLAY1                       = -1545021,
    SAY_SLAY2                       = -1545022,
    SAY_DEATH                       = -1545023,

    SPELL_SPELL_REFLECTION          = 31534,
    SPELL_IMPALE                    = 39061,
    SPELL_WARLORDS_RAGE             = 37081,
    SPELL_WARLORDS_RAGE_NAGA        = 31543,
    SPELL_WARLORDS_RAGE_PROC        = 36453,

    NPC_NAGA_DISTILLER              = 17954,

    EVENT_SPELL_REFLECTION          = 1,
    EVENT_SPELL_IMPALE              = 2,
    EVENT_SPELL_RAGE                = 3
};

struct mob_naga_distillerAI : public NullCreatureAI
{
    mob_naga_distillerAI(Creature* c) : NullCreatureAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    uint32 spellTimer;

    void Reset()
    {
        spellTimer = 0;
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void DoAction(int32 param)
    {
        if (param != 1)
            return;

        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->CastSpell(me, SPELL_WARLORDS_RAGE_NAGA, true);
        spellTimer = 1;
    }

    void UpdateAI(uint32 diff)
    {
        if (spellTimer)
        {
            spellTimer += diff;
            if (spellTimer >= 12000)
            {
                if (Creature* kali = me->FindNearestCreature(NPC_WARLORD_KALITHRESH, 100.0f))
                    kali->CastSpell(kali, SPELL_WARLORDS_RAGE_PROC, true);
                me->Kill(me, false);
            }
        }
    }
};

struct boss_warlord_kalithreshAI : public ScriptedAI
{
    boss_warlord_kalithreshAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    EventMap events;

    void Reset()
    {
        events.Reset();

        if (pInstance)
            pInstance->SetData(TYPE_WARLORD_KALITHRESH, NOT_STARTED);
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(RAND(SAY_AGGRO1, SAY_AGGRO2, SAY_AGGRO3), me);

        events.ScheduleEvent(EVENT_SPELL_REFLECTION, 10000);
        events.ScheduleEvent(EVENT_SPELL_IMPALE, urand(7000, 14000));
        events.ScheduleEvent(EVENT_SPELL_RAGE, 20000);

        if (pInstance)
            pInstance->SetData(TYPE_WARLORD_KALITHRESH, IN_PROGRESS);
    }

    void KilledUnit(Unit* victim)
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2), me);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance)
            pInstance->SetData(TYPE_WARLORD_KALITHRESH, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
        case EVENT_SPELL_REFLECTION:
            me->CastSpell(me, SPELL_SPELL_REFLECTION, false);
            events.Repeat(urand(15000, 20000));
            break;
        case EVENT_SPELL_IMPALE:
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 10.0f, true))
                me->CastSpell(target, SPELL_IMPALE, false);
            events.Repeat(urand(7500, 12500));
            break;
        case EVENT_SPELL_RAGE:
            if (Creature* distiller = me->FindNearestCreature(NPC_NAGA_DISTILLER, 100.0f))
            {
                me->GetMotionMaster()->MoveToTarget(NPC_NAGA_DISTILLER, 100.0f, true);
                DoScriptText(SAY_REGEN, me);
                distiller->AI()->DoAction(1);
            }
            events.Repeat(45000);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_naga_distiller(Creature* pCreature)
{
    return GetInstanceAI<mob_naga_distillerAI>(pCreature);
}

CreatureAI* GetAI_boss_warlord_kalithresh(Creature* pCreature)
{
    return GetInstanceAI<boss_warlord_kalithreshAI>(pCreature);
}

void AddSC_boss_warlord_kalithresh()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "mob_naga_distiller";
    newscript->GetAI = &GetAI_mob_naga_distiller;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_warlord_kalithresh";
    newscript->GetAI = &GetAI_boss_warlord_kalithresh;
    newscript->RegisterSelf();
}

