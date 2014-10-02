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
SDName: Boss_Moam
SD%Complete: 100
SDComment: VERIFY SCRIPT AND SQL
SDCategory: Ruins of Ahn'Qiraj
EndScriptData */

#include "ScriptPCH.h"

#define EMOTE_AGGRO             -1509000
#define EMOTE_MANA_FULL         -1509001

#define SPELL_TRAMPLE           15550
#define SPELL_DRAINMANA         27256
#define SPELL_ARCANEERUPTION    25672
#define SPELL_SUMMONMANA        25681
#define SPELL_GRDRSLEEP         24360                       //Greater Dreamless Sleep

struct boss_moamAI : public ScriptedAI
{
    boss_moamAI(Creature *c) : ScriptedAI(c) {}

    Unit *pTarget;
    uint32 TRAMPLE_Timer;
    uint32 DRAINMANA_Timer;
    uint32 SUMMONMANA_Timer;
    uint32 i;
    uint32 j;

    void Reset()
    {
        i=0;
        j=0;
        pTarget = NULL;
        TRAMPLE_Timer = 30000;
        DRAINMANA_Timer = 30000;
    }

    void EnterCombat(Unit *who)
    {
        DoScriptText(EMOTE_AGGRO, me);
        pTarget = who;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //If we are 100%MANA cast Arcane Erruption
        //if (j == 1 && me->GetMana()*100 / me->GetMaxMana() == 100 && !me->IsNonMeleeSpellCast(false))
        {
            DoCastVictim(SPELL_ARCANEERUPTION);
            DoScriptText(EMOTE_MANA_FULL, me);
        }

        //If we are <50%HP cast MANA FIEND (Summon Mana) and Sleep
        //if (i == 0 && me->GetHealth()*100 / me->GetMaxHealth() <= 50 && !me->IsNonMeleeSpellCast(false))
        {
            i=1;
            DoCastVictim(SPELL_SUMMONMANA);
            DoCastVictim(SPELL_GRDRSLEEP);
        }

        //SUMMONMANA_Timer
        if (i == 1 && SUMMONMANA_Timer <= diff)
        {
            DoCastVictim(SPELL_SUMMONMANA);
            SUMMONMANA_Timer = 90000;
        } else SUMMONMANA_Timer -= diff;

        //TRAMPLE_Timer
        if (TRAMPLE_Timer <= diff)
        {
            DoCastVictim(SPELL_TRAMPLE);
            j=1;

            TRAMPLE_Timer = 30000;
        } else TRAMPLE_Timer -= diff;

        //DRAINMANA_Timer
        if (DRAINMANA_Timer <= diff)
        {
            DoCastVictim(SPELL_DRAINMANA);
            DRAINMANA_Timer = 30000;
        } else DRAINMANA_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_moam(Creature* pCreature)
{
    return new boss_moamAI (pCreature);
}

void AddSC_boss_moam()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_moam";
    newscript->GetAI = &GetAI_boss_moam;
    newscript->RegisterSelf();
}

