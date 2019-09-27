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
SDName: Boss_Loatheb
SD%Complete: 100
SDComment:
SDCategory: Naxxramas
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#define SAY_AGGRO1          "You are mine now!"
#define SAY_AGGRO2          "I see you!"
#define SAY_AGGRO3          "You...are next!"
#define SAY_SLAY1           "Close your eyes... sleep!"
#define SAY_SLAY2           "The races of the world will perish. It is only a matter of time."
#define SAY_SLAY3           "I see endless suffering, I see torment, I see rage. I see... everything!"
#define SAY_SLAY4           "Soon... the world will tremble!"
#define SAY_SLAY5           "The end is upon you."
#define SAY_SLAY6           "Hungry worms will feast on your rotten flesh!"
#define SAY_DEATH           ""

#define SOUND_AGGRO1        8825
#define SOUND_AGGRO2        8826
#define SOUND_AGGRO3        8827
#define SOUND_SLAY1         8829
#define SOUND_SLAY2         8830
#define SOUND_SLAY3         8831
#define SOUND_SLAY4         8832
#define SOUND_SLAY5         8833
#define SOUND_SLAY6         8834
#define SOUND_DEATH         8828

#define SPELL_CORRUPTED_MIND        29198
#define SPELL_POISON_AURA           29865
#define SPELL_INEVITABLE_DOOM       29204
#define SPELL_REMOVE_CURSE          30281

#define ADD_1X 2957.040f
#define ADD_1Y -3997.590f
#define ADD_1Z 274.280f

#define ADD_2X 2909.130f
#define ADD_2Y -4042.970f
#define ADD_2Z 274.280f

#define ADD_3X 2861.102f
#define ADD_3Y -3997.901f
#define ADD_3Z 274.280f

struct boss_loathebAI : public ScriptedAI
{
    boss_loathebAI(Creature* c) : ScriptedAI(c) {}

    uint32 CorruptedMind_Timer;
    uint32 PoisonAura_Timer;
    uint32 InevitableDoom_Timer;
    uint32 InevitableDoom5mins_Timer;
    uint32 RemoveCurse_Timer;
    uint32 Summon_Timer;

    void Reset()
    {
        CorruptedMind_Timer = 4000;
        PoisonAura_Timer = 2500;
        InevitableDoom_Timer = 120000;
        InevitableDoom5mins_Timer = 300000;
        RemoveCurse_Timer = 30000;
        Summon_Timer = 8000;
    }

    void EnterCombat(Unit* /*who*/)
    {
        switch (rand() % 3)
        {
        case 0:
            me->MonsterYell(SAY_AGGRO1, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, SOUND_AGGRO1);
            break;
        case 1:
            me->MonsterYell(SAY_AGGRO2, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, SOUND_AGGRO2);
            break;
        case 2:
            me->MonsterYell(SAY_AGGRO3, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, SOUND_AGGRO3);
            break;
        }
    }

    void KilledUnit(Unit* /*victim*/)
    {
        switch (rand() % 6)
        {
        case 0:
            me->MonsterYell(SAY_SLAY1, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, SOUND_SLAY1);
            break;
        case 1:
            me->MonsterYell(SAY_SLAY2, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, SOUND_SLAY2);
            break;
        case 2:
            me->MonsterYell(SAY_SLAY3, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, SOUND_SLAY3);
            break;
        case 3:
            me->MonsterYell(SAY_SLAY4, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, SOUND_SLAY4);
            break;
        case 4:
            me->MonsterYell(SAY_SLAY5, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, SOUND_SLAY5);
            break;
        case 5:
            me->MonsterYell(SAY_SLAY6, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, SOUND_SLAY6);
            break;
        }
    }

    void JustDied(Unit* /*Killer*/)
    {
        me->MonsterYell(SAY_DEATH, LANG_UNIVERSAL, 0);
        DoPlaySoundToSet(me, SOUND_DEATH);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //CorruptedMind_Timer
        if (CorruptedMind_Timer <= diff)
        {
            DoCastVictim(SPELL_CORRUPTED_MIND);
            CorruptedMind_Timer = 62000;
        }
        else CorruptedMind_Timer -= diff;

        //PoisonAura_Timer
        if (PoisonAura_Timer <= diff)
        {
            DoCastVictim(SPELL_POISON_AURA);
            PoisonAura_Timer = 60000;
        }
        else PoisonAura_Timer -= diff;

        //InevitableDoom_Timer
        if (InevitableDoom_Timer <= diff)
        {
            DoCastVictim(SPELL_INEVITABLE_DOOM);
            InevitableDoom_Timer = 120000;
        }
        else InevitableDoom_Timer -= diff;

        //InevitableDoom5mins_Timer
        if (InevitableDoom5mins_Timer <= diff)
        {
            DoCastVictim(SPELL_INEVITABLE_DOOM);
            InevitableDoom5mins_Timer = 15000;
        }
        else InevitableDoom5mins_Timer -= diff;

        //RemoveCurse_Timer
        if (RemoveCurse_Timer <= diff)
        {
            DoCast(me, SPELL_REMOVE_CURSE);
            RemoveCurse_Timer = 30000;
        }
        else RemoveCurse_Timer -= diff;

        //Summon_Timer
        if (Summon_Timer <= diff)
        {
            Unit* pTarget = NULL;
            Unit* SummonedSpores = NULL;

            SummonedSpores = me->SummonCreature(16286, ADD_1X, ADD_1Y, ADD_1Z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 80000);
            SummonedSpores = me->SummonCreature(16286, ADD_2X, ADD_2Y, ADD_2Z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 80000);
            SummonedSpores = me->SummonCreature(16286, ADD_3X, ADD_3Y, ADD_3Z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 80000);
            if (SummonedSpores)
            {
                pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if (pTarget)
                    SummonedSpores->AddThreat(pTarget, 1.0f);
            }

            Summon_Timer = 28000;
        }
        else Summon_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_loatheb(Creature* pCreature)
{
    return new boss_loathebAI (pCreature);
}

void AddSC_boss_loatheb()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_loatheb";
    newscript->GetAI = &GetAI_boss_loatheb;
    newscript->RegisterSelf();
}

