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
SDName: Boss_Azuregos
SD%Complete: 90
SDComment: Teleport not included, spell reflect not effecting dots (Core problem)
SDCategory: Azshara
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#define SAY_TELEPORT            -1000100

#define SPELL_MARKOFFROST        23182
#define SPELL_MANASTORM          21097
#define SPELL_CHILL              21098
#define SPELL_FROSTBREATH        21099
#define SPELL_REFLECT            22067
#define SPELL_CLEAVE              8255                      //Perhaps not right ID
#define SPELL_ENRAGE             23537

struct boss_azuregosAI : public ScriptedAI
{
    boss_azuregosAI(Creature* c) : ScriptedAI(c) {}

    uint32 MarkOfFrost_Timer;
    uint32 ManaStorm_Timer;
    uint32 Chill_Timer;
    uint32 Breath_Timer;
    uint32 Teleport_Timer;
    uint32 Reflect_Timer;
    uint32 Cleave_Timer;
    uint32 Enrage_Timer;
    bool Enraged;

    void Reset()
    {
        MarkOfFrost_Timer = 35000;
        ManaStorm_Timer = 5000 + rand() % 12000;
        Chill_Timer = 10000 + rand() % 20000;
        Breath_Timer = 2000 + rand() % 6000;
        Teleport_Timer = 30000;
        Reflect_Timer = 15000 + rand() % 15000;
        Cleave_Timer = 7000;
        Enrage_Timer = 0;
        Enraged = false;
    }

    void EnterCombat(Unit* /*who*/) {}

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!UpdateVictim())
            return;

        if (Teleport_Timer <= diff)
        {
            DoScriptText(SAY_TELEPORT, me);
            ThreatContainer::StorageType threatlist = me->getThreatManager().getThreatList();
            ThreatContainer::StorageType::const_iterator i = threatlist.begin();
            for (i = threatlist.begin(); i != threatlist.end(); ++i)
            {
                Unit* pUnit = Unit::GetUnit((*me), (*i)->getUnitGuid());
                if (pUnit && (pUnit->GetTypeId() == TYPEID_PLAYER))
                    DoTeleportPlayer(pUnit, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 3, pUnit->GetOrientation());
            }

            DoResetThreat();
            Teleport_Timer = 30000;
        }
        else Teleport_Timer -= diff;

        //        //MarkOfFrost_Timer
        //        if (MarkOfFrost_Timer <= diff)
        //        {
        //            DoCastVictim(SPELL_MARKOFFROST);
        //            MarkOfFrost_Timer = 25000;
        //        } else MarkOfFrost_Timer -= diff;

        //Chill_Timer
        if (Chill_Timer <= diff)
        {
            DoCastVictim(SPELL_CHILL);
            Chill_Timer = 13000 + rand() % 12000;
        }
        else Chill_Timer -= diff;

        //Breath_Timer
        if (Breath_Timer <= diff)
        {
            DoCastVictim(SPELL_FROSTBREATH);
            Breath_Timer = 10000 + rand() % 5000;
        }
        else Breath_Timer -= diff;

        //ManaStorm_Timer
        if (ManaStorm_Timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_MANASTORM);
            ManaStorm_Timer = 7500 + rand() % 5000;
        }
        else ManaStorm_Timer -= diff;

        //Reflect_Timer
        if (Reflect_Timer <= diff)
        {
            DoCast(me, SPELL_REFLECT);
            Reflect_Timer = 20000 + rand() % 15000;
        }
        else Reflect_Timer -= diff;

        //Cleave_Timer
        if (Cleave_Timer <= diff)
        {
            DoCastVictim(SPELL_CLEAVE);
            Cleave_Timer = 7000;
        }
        else Cleave_Timer -= diff;

        //Enrage_Timer
        if (HealthBelowPct(25) && !Enraged)
        {
            DoCast(me, SPELL_ENRAGE);
            Enraged = true;
        }

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_azuregos(Creature* pCreature)
{
    return new boss_azuregosAI (pCreature);
}

void AddSC_boss_azuregos()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_azuregos";
    newscript->GetAI = &GetAI_boss_azuregos;
    newscript->RegisterSelf();
}

