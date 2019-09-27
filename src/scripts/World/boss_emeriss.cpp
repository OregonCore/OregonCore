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
SDName: Emeriss
SD%Complete: 90
SDComment: Teleport function & Mark of Nature missing
SDCategory: Bosses
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum eEnums
{
    SAY_AGGRO               = -1000401,
    SAY_CASTCORRUPTION      = -1000402,

    SPELL_SLEEP             = 24777,
    SPELL_NOXIOUSBREATH     = 24818,
    SPELL_TAILSWEEP         = 15847,
    //SPELL_MARKOFNATURE    = 25040,                        // Not working
    SPELL_VOLATILEINFECTION = 24928,
    SPELL_CORRUPTIONOFEARTH = 24910
};

struct boss_emerissAI : public ScriptedAI
{
    boss_emerissAI(Creature* c) : ScriptedAI(c) {}

    uint32 m_uiSleep_Timer;
    uint32 m_uiNoxiousBreath_Timer;
    uint32 m_uiTailSweep_Timer;
    //uint32 m_uiMarkOfNature_Timer;
    uint32 m_uiVolatileInfection_Timer;
    uint32 m_uiCorruptionsCasted;

    void Reset()
    {
        m_uiSleep_Timer = 15000 + rand() % 5000;
        m_uiNoxiousBreath_Timer = 8000;
        m_uiTailSweep_Timer = 4000;
        //m_uiMarkOfNature_Timer = 45000;
        m_uiVolatileInfection_Timer = 12000;
        m_uiCorruptionsCasted = 0;
    }

    void Aggro(Unit* /*pWho*/)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!UpdateVictim())
            return;

        //Sleep_Timer
        if (m_uiSleep_Timer <= uiDiff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_SLEEP);

            m_uiSleep_Timer = 8000 + rand() % 8000;
        }
        else
            m_uiSleep_Timer -= uiDiff;

        //NoxiousBreath_Timer
        if (m_uiNoxiousBreath_Timer <= uiDiff)
        {
            DoCastVictim( SPELL_NOXIOUSBREATH);
            m_uiNoxiousBreath_Timer = 14000 + rand() % 6000;
        }
        else
            m_uiNoxiousBreath_Timer -= uiDiff;

        //Tailsweep every 2 seconds
        if (m_uiTailSweep_Timer <= uiDiff)
        {
            DoCast(me, SPELL_TAILSWEEP);
            m_uiTailSweep_Timer = 2000;
        }
        else
            m_uiTailSweep_Timer -= uiDiff;

        //MarkOfNature_Timer
        //if (m_uiMarkOfNature_Timer <= uiDiff)
        //{
        //    DoCastVictim( SPELL_MARKOFNATURE);
        //    m_uiMarkOfNature_Timer = 45000;
        //}
        //else
        //    m_uiMarkOfNature_Timer -= uiDiff;

        //VolatileInfection_Timer
        if (m_uiVolatileInfection_Timer <= uiDiff)
        {
            DoCastVictim( SPELL_VOLATILEINFECTION);
            m_uiVolatileInfection_Timer = 7000 + rand() % 5000;
        }
        else
            m_uiVolatileInfection_Timer -= uiDiff;

        //CorruptionofEarth_Timer
        //CorruptionofEarth at 75%, 50% and 25%
        if ((me->GetHealth() * 100 / me->GetMaxHealth()) <= (100 - (25 * m_uiCorruptionsCasted)))
        {
            ++m_uiCorruptionsCasted;                        // prevent casting twice on same hp
            DoScriptText(SAY_CASTCORRUPTION, me);
            DoCastVictim( SPELL_CORRUPTIONOFEARTH);
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_emeriss(Creature* pCreature)
{
    return new boss_emerissAI (pCreature);
}

void AddSC_boss_emeriss()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_emeriss";
    newscript->GetAI = &GetAI_boss_emeriss;
    newscript->RegisterSelf();
}

