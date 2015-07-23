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
SDName: Duskwood
SD%Complete: 100
SDComment: Quest Support:8735
SDCategory: Duskwood
EndScriptData */

/* ContentData
boss_twilight_corrupter
at_twilight_grove
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

/*#####
## Quest: The Nightmare's Corruption
#####*/

enum
{
    CORRUPTER_YELL_COME         = -1000654,
    CORRUPTER_YELL_CANT_STOP    = -1000655,
    CORRUPTER_EMOTE_SQUEEZER    = -1000656,

    NPC_TWILINGHT_CORRUPTER     = 15625,
    CORRUPTER_DESPAWN_TIMER     = 1800000, //30 min timer

    SPELL_SOUL_CORRUPTION       = 25805,
    SPELL_CREATURE_OF_NIGHTMARE = 25806,
    SPELL_LEVEL_UP              = 24312
};

/*######
# at_twilight_grove
######*/

bool AreaTrigger_at_twilight_grove(Player* pPlayer, const AreaTriggerEntry* /*at*/)
{
    if (pPlayer->HasQuestForItem(21149))
    {
        if (Creature* pCorrupter = pPlayer->FindNearestCreature(NPC_TWILINGHT_CORRUPTER, 10.0f))
        {
            DoScriptText(CORRUPTER_YELL_COME, pCorrupter, pPlayer);
            return false;
        }

        if (Creature* pCorrupter = pPlayer->SummonCreature(NPC_TWILINGHT_CORRUPTER, -10636.9, -389.254, 102.626, 0, TEMPSUMMON_TIMED_DESPAWN, CORRUPTER_DESPAWN_TIMER))
        {
            pCorrupter->SetVisibility(VISIBILITY_OFF);
            DoScriptText(CORRUPTER_YELL_COME, pCorrupter, pPlayer);
        }

        if (Creature* TCorrupter = pPlayer->SummonCreature(NPC_TWILINGHT_CORRUPTER, -10328.16, -489.57, 49.95, 0, TEMPSUMMON_MANUAL_DESPAWN, 60000))
        {
            TCorrupter->setFaction(14);
            TCorrupter->SetMaxHealth(832750);
        }
    }
    return false;
};

/*######
# boss_twilight_corrupter
######*/

struct boss_twilight_corrupterAI : public ScriptedAI
{
    boss_twilight_corrupterAI(Creature* c) : ScriptedAI(c) {}

    uint32 SoulCorruption_Timer;
    uint32 CreatureOfNightmare_Timer;
    uint8 KillCount;

    void Reset()
    {
        SoulCorruption_Timer = 15000;
        CreatureOfNightmare_Timer = 30000;
        KillCount = 0;
    }
    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(CORRUPTER_YELL_CANT_STOP, me);
    }

    void KilledUnit(Unit* victim)
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
        {
            ++KillCount;
            DoScriptText(CORRUPTER_EMOTE_SQUEEZER, me, victim);

            if (KillCount == 3)
            {
                DoCast(me, SPELL_LEVEL_UP, true);
                KillCount = 0;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;
        if (SoulCorruption_Timer <= diff)
        {
            DoCastVictim( SPELL_SOUL_CORRUPTION);
            SoulCorruption_Timer = rand() % 4000 + 15000; //gotta confirm Timers
        }
        else SoulCorruption_Timer -= diff;
        if (CreatureOfNightmare_Timer <= diff)
        {
            DoCastVictim( SPELL_CREATURE_OF_NIGHTMARE);
            CreatureOfNightmare_Timer = 45000; //gotta confirm Timers
        }
        else CreatureOfNightmare_Timer -= diff;
        DoMeleeAttackIfReady();
    };
};

CreatureAI* GetAI_boss_twilight_corrupter(Creature* pCreature)
{
    return new boss_twilight_corrupterAI (pCreature);
}

void AddSC_duskwood()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_twilight_corrupter";
    newscript->GetAI = &GetAI_boss_twilight_corrupter;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "at_twilight_grove";
    newscript->pAreaTrigger = &AreaTrigger_at_twilight_grove;
    newscript->RegisterSelf();
}
