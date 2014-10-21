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
SDName: Boss_Ayamiss
SD%Complete: 50
SDComment: VERIFY SCRIPT
SDCategory: Ruins of Ahn'Qiraj
EndScriptData */

#include "ScriptPCH.h"

/*
To do:
make him fly from 70-100%
*/

#define SPELL_STINGERSPRAY 25749
#define SPELL_POISONSTINGER 25748                           //only used in phase1
#define SPELL_SUMMONSWARMER 25844                           //might be 25708
// #define SPELL_PARALYZE 23414 doesnt work correct (core)

struct boss_ayamissAI : public ScriptedAI
{
    boss_ayamissAI(Creature* c) : ScriptedAI(c) {}

    Unit *pTarget;
    uint32 STINGERSPRAY_Timer;
    uint32 POISONSTINGER_Timer;
    uint32 SUMMONSWARMER_Timer;
    uint32 phase;

    void Reset()
    {
        pTarget = NULL;
        STINGERSPRAY_Timer = 30000;
        POISONSTINGER_Timer = 30000;
        SUMMONSWARMER_Timer = 60000;
        phase=1;
    }

    void EnterCombat(Unit *who)
    {
        pTarget = who;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //If he is 70% start phase 2
        if (phase == 1 && me->GetHealth()*100 / me->GetMaxHealth() <= 70 && !me->IsNonMeleeSpellCast(false))
        {
            phase=2;
        }

        //STINGERSPRAY_Timer (only in phase2)
        if (phase == 2 && STINGERSPRAY_Timer <= diff)
        {
            DoCastVictim(SPELL_STINGERSPRAY);
            STINGERSPRAY_Timer = 30000;
        } else STINGERSPRAY_Timer -= diff;

        //POISONSTINGER_Timer (only in phase1)
        if (phase == 1 && POISONSTINGER_Timer <= diff)
        {
            DoCastVictim(SPELL_POISONSTINGER);
            POISONSTINGER_Timer = 30000;
        } else POISONSTINGER_Timer -= diff;

        //SUMMONSWARMER_Timer (only in phase1)
        if (SUMMONSWARMER_Timer <= diff)
        {
            DoCastVictim(SPELL_SUMMONSWARMER);
            SUMMONSWARMER_Timer = 60000;
        } else SUMMONSWARMER_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_ayamiss(Creature* pCreature)
{
    return new boss_ayamissAI (pCreature);
}

void AddSC_boss_ayamiss()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_ayamiss";
    newscript->GetAI = &GetAI_boss_ayamiss;
    newscript->RegisterSelf();
}

