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

#define SAY_INTRO                   -1545016
#define SAY_REGEN                   -1545017
#define SAY_AGGRO1                  -1545018
#define SAY_AGGRO2                  -1545019
#define SAY_AGGRO3                  -1545020
#define SAY_SLAY1                   -1545021
#define SAY_SLAY2                   -1545022
#define SAY_DEATH                   -1545023

#define SPELL_SPELL_REFLECTION      31534
#define SPELL_IMPALE                39061
#define SPELL_WARLORDS_RAGE         37081
#define SPELL_WARLORDS_RAGE_NAGA    31543

#define SPELL_WARLORDS_RAGE_PROC    36453

struct mob_naga_distillerAI : public ScriptedAI
{
    mob_naga_distillerAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    void Reset()
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		me->SetRooted(true);
		me->SetStunned(true);

        //hack, due to really weird spell behaviour :(
        if (pInstance)
        {
            if (pInstance->GetData(TYPE_DISTILLER) == IN_PROGRESS)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }
        }
    }

    void EnterCombat(Unit* /*who*/) { }

    void StartRageGen(Unit* /*caster*/)
    {
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        DoCast(me, SPELL_WARLORDS_RAGE_NAGA, true);

        if (pInstance)
            pInstance->SetData(TYPE_DISTILLER, IN_PROGRESS);
    }

    void DamageTaken(Unit* /*done_by*/, uint32& damage)
    {
        if (me->GetHealth() <= damage)
            if (pInstance)
                pInstance->SetData(TYPE_DISTILLER, DONE);
    }
};

struct boss_warlord_kalithreshAI : public ScriptedAI
{
    boss_warlord_kalithreshAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 Reflection_Timer;
    uint32 Impale_Timer;
    uint32 Rage_Timer;
    bool CanRage;

    void Reset()
    {
        Reflection_Timer = 10000;
        Impale_Timer = 7000 + rand() % 7000;
        Rage_Timer = 45000;
        CanRage = false;

        if (pInstance)
            pInstance->SetData(TYPE_WARLORD_KALITHRESH, NOT_STARTED);
    }

    void EnterCombat(Unit* /*who*/)
    {
        switch (rand() % 3)
        {
        case 0:
            DoScriptText(SAY_AGGRO1, me);
            break;
        case 1:
            DoScriptText(SAY_AGGRO2, me);
            break;
        case 2:
            DoScriptText(SAY_AGGRO3, me);
            break;
        }

        if (pInstance)
            pInstance->SetData(TYPE_WARLORD_KALITHRESH, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        switch (rand() % 2)
        {
        case 0:
            DoScriptText(SAY_SLAY1, me);
            break;
        case 1:
            DoScriptText(SAY_SLAY2, me);
            break;
        }
    }

    Creature* SelectCreatureInGrid(uint32 entry, float range)
    {
        Creature* pCreature = NULL;

        CellCoord pair(Oregon::ComputeCellCoord(me->GetPositionX(), me->GetPositionY()));
        Cell cell(pair);
        cell.SetNoCreate();

        Oregon::NearestCreatureEntryWithLiveStateInObjectRangeCheck creature_check(*me, entry, true, range);
        Oregon::CreatureLastSearcher<Oregon::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pCreature, creature_check);
        TypeContainerVisitor<Oregon::CreatureLastSearcher<Oregon::NearestCreatureEntryWithLiveStateInObjectRangeCheck>, GridTypeMapContainer> creature_searcher(searcher);
        cell.Visit(pair, creature_searcher, *(me->GetMap()), *me, me->GetGridActivationRange());

        return pCreature;
    }

    void SpellHit(Unit* /*caster*/, const SpellEntry* spell)
    {
        //FIXME: hack :(
        if (spell->Id == SPELL_WARLORDS_RAGE_PROC)
            if (pInstance)
                if (pInstance->GetData(TYPE_DISTILLER) == DONE)
                    me->RemoveAurasDueToSpell(SPELL_WARLORDS_RAGE_PROC);
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

        if (Rage_Timer <= diff)
        {
            Creature* distiller = SelectCreatureInGrid(17954, 100);
            if (distiller)
            {
                DoScriptText(SAY_REGEN, me);
                DoCast(me, SPELL_WARLORDS_RAGE);
                ((mob_naga_distillerAI*)distiller->AI())->StartRageGen(me);
            }
            Rage_Timer = 3000 + rand() % 15000;
        }
        else Rage_Timer -= diff;

        //Reflection_Timer
        if (Reflection_Timer <= diff)
        {
            DoCast(me, SPELL_SPELL_REFLECTION);
            Reflection_Timer = 15000 + rand() % 10000;
        }
        else Reflection_Timer -= diff;

        //Impale_Timer
        if (Impale_Timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_IMPALE);

            Impale_Timer = 7500 + rand() % 5000;
        }
        else Impale_Timer -= diff;

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

