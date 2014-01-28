/*
 * Copyright (C) 2010-2012 OregonCore <http://www.oregoncore.com/>
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2012 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Boss_Four_Horsemen
SD%Complete: 75
SDComment: Lady Blaumeux, Thane Korthazz, Sir Zeliek, Baron Rivendare (this script maybe is not correct for TC1)
SDCategory: Naxxramas
EndScriptData */

#include "ScriptPCH.h"

//all horsemen
#define SPELL_SHIELDWALL                29061
#define SPELL_BESERK                     26662

//lady blaumeux
#define SAY_BLAU_AGGRO                  -1533044
#define SAY_BLAU_TAUNT1                 -1533045
#define SAY_BLAU_TAUNT2                 -1533046
#define SAY_BLAU_TAUNT3                 -1533047
#define SAY_BLAU_SPECIAL                -1533048
#define SAY_BLAU_SLAY                    -1533049
#define SAY_BLAU_DEATH                  -1533050

#define SPELL_MARK_OF_BLAUMEUX        28833
#define SPELL_UNYILDING_PAIN          57381
#define SPELL_VOIDZONE                  28863
#define H_SPELL_VOIDZONE                57463
#define SPELL_SHADOW_BOLT              57374
#define H_SPELL_SHADOW_BOLT            57464

#define C_SPIRIT_OF_BLAUMEUX          16776

struct boss_lady_blaumeuxAI : public ScriptedAI
{
    boss_lady_blaumeuxAI(Creature *c) : ScriptedAI(c) {}

    uint32 Mark_Timer;
    uint32 VoidZone_Timer;
    bool ShieldWall1;
    bool ShieldWall2;

    void Reset()
    {
        Mark_Timer = 20000;                                            // First Horsemen Mark is applied at 20 sec.
        VoidZone_Timer = 12000;                                      // right
        ShieldWall1 = true;
        ShieldWall2 = true;
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(SAY_BLAU_AGGRO, me);
    }

    void KilledUnit(Unit* /*Victim*/)
    {
        DoScriptText(SAY_BLAU_SLAY, me);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_BLAU_DEATH, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        // Mark of Blaumeux
        if (Mark_Timer <= diff)
        {
            DoCast(me->getVictim(),SPELL_MARK_OF_BLAUMEUX);
            Mark_Timer = 12000;
        } else Mark_Timer -= diff;

        // Shield Wall - All 4 horsemen will shield wall at 50% hp and 20% hp for 20 seconds
        if (ShieldWall1 && (me->GetHealth()*100 / me->GetMaxHealth()) < 50)
        {
            if (ShieldWall1)
            {
                DoCast(me,SPELL_SHIELDWALL);
                ShieldWall1 = false;
            }
        }
        if (ShieldWall2 && (me->GetHealth()*100 / me->GetMaxHealth()) < 20)
        {
            if (ShieldWall2)
            {
                DoCast(me,SPELL_SHIELDWALL);
                ShieldWall2 = false;
            }
        }

        // Void Zone
        if (VoidZone_Timer <= diff)
        {
            DoCast(me->getVictim(),SPELL_VOIDZONE);
            VoidZone_Timer = 12000;
        } else VoidZone_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lady_blaumeux(Creature* pCreature)
{
    return new boss_lady_blaumeuxAI (pCreature);
}

//baron rivendare
#define SAY_RIVE_AGGRO1                 -1533065
#define SAY_RIVE_AGGRO2                 -1533066
#define SAY_RIVE_AGGRO3                 -1533067
#define SAY_RIVE_SLAY1                  -1533068
#define SAY_RIVE_SLAY2                  -1533069
#define SAY_RIVE_SPECIAL                -1533070
#define SAY_RIVE_TAUNT1                 -1533071
#define SAY_RIVE_TAUNT2                 -1533072
#define SAY_RIVE_TAUNT3                 -1533073
#define SAY_RIVE_DEATH                  -1533074

#define SPELL_MARK_OF_RIVENDARE      28834
#define SPELL_UNHOLY_SHADOW            28882
#define H_SPELL_UNHOLY_SHADOW         57369

#define C_SPIRIT_OF_RIVENDARE         0                              //creature entry not known yet

struct boss_rivendare_naxxAI : public ScriptedAI
{
    boss_rivendare_naxxAI(Creature *c) : ScriptedAI(c) {}

    void Reset()
    {
    }

    void EnterCombat(Unit* /*who*/)
    {
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_RIVE_AGGRO1, me); break;
            case 1: DoScriptText(SAY_RIVE_AGGRO2, me); break;
            case 2: DoScriptText(SAY_RIVE_AGGRO3, me); break;
        }
    }

    void KilledUnit(Unit* /*Victim*/)
    {
        switch(rand()%2)
        {
            case 0: DoScriptText(SAY_RIVE_SLAY1, me); break;
            case 1: DoScriptText(SAY_RIVE_SLAY2, me); break;
        }
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_RIVE_DEATH, me);
    }

    void UpdateAI(const uint32 /*diff*/)
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_rivendare_naxx(Creature* pCreature)
{
    return new boss_rivendare_naxxAI (pCreature);
}

//thane korthazz
#define SAY_KORT_AGGRO                  -1533051
#define SAY_KORT_TAUNT1                 -1533052
#define SAY_KORT_TAUNT2                 -1533053
#define SAY_KORT_TAUNT3                 -1533054
#define SAY_KORT_SPECIAL                -1533055
#define SAY_KORT_SLAY                    -1533056
#define SAY_KORT_DEATH                  -1533057

#define SPELL_MARK_OF_KORTHAZZ        28832
#define SPELL_METEOR                     26558                         // me->getVictim() auto-area spell but with a core problem

#define C_SPIRIT_OF_KORTHAZZ          16778

struct boss_thane_korthazzAI : public ScriptedAI
{
    boss_thane_korthazzAI(Creature *c) : ScriptedAI(c) {}

    uint32 Mark_Timer;
    uint32 Meteor_Timer;
    bool ShieldWall1;
    bool ShieldWall2;

    void Reset()
    {
        Mark_Timer = 20000;                                            // First Horsemen Mark is applied at 20 sec.
        Meteor_Timer = 30000;                                         // wrong
        ShieldWall1 = true;
        ShieldWall2 = true;
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(SAY_KORT_AGGRO, me);
    }

    void KilledUnit(Unit* /*Victim*/)
    {
        DoScriptText(SAY_KORT_SLAY, me);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_KORT_DEATH, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        // Mark of Korthazz
        if (Mark_Timer <= diff)
        {
            DoCast(me->getVictim(),SPELL_MARK_OF_KORTHAZZ);
            Mark_Timer = 12000;
        } else Mark_Timer -= diff;

        // Shield Wall - All 4 horsemen will shield wall at 50% hp and 20% hp for 20 seconds
        if (ShieldWall1 && (me->GetHealth()*100 / me->GetMaxHealth()) < 50)
        {
            if (ShieldWall1)
            {
                DoCast(me,SPELL_SHIELDWALL);
                ShieldWall1 = false;
            }
        }
        if (ShieldWall2 && (me->GetHealth()*100 / me->GetMaxHealth()) < 20)
        {
            if (ShieldWall2)
            {
                DoCast(me,SPELL_SHIELDWALL);
              ShieldWall2 = false;
            }
        }

        // Meteor
        if (Meteor_Timer <= diff)
        {
            DoCast(me->getVictim(),SPELL_METEOR);
            Meteor_Timer = 20000;                                    // wrong
      } else Meteor_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_thane_korthazz(Creature* pCreature)
{
    return new boss_thane_korthazzAI (pCreature);
}

//sir zeliek
#define SAY_ZELI_AGGRO                  -1533058
#define SAY_ZELI_TAUNT1                 -1533059
#define SAY_ZELI_TAUNT2                 -1533060
#define SAY_ZELI_TAUNT3                 -1533061
#define SAY_ZELI_SPECIAL                -1533062
#define SAY_ZELI_SLAY                    -1533063
#define SAY_ZELI_DEATH                  -1533064

#define SPELL_MARK_OF_ZELIEK          28835
#define SPELL_HOLY_WRATH                28883
#define H_SPELL_HOLY_WRATH             57466
#define SPELL_HOLY_BOLT                 57376
#define H_SPELL_HOLY_BOLT              57465

#define C_SPIRIT_OF_ZELIREK            16777

struct boss_sir_zeliekAI : public ScriptedAI
{
    boss_sir_zeliekAI(Creature *c) : ScriptedAI(c) {}

    uint32 Mark_Timer;
    uint32 HolyWrath_Timer;
    bool ShieldWall1;
    bool ShieldWall2;

    void Reset()
    {
        Mark_Timer = 20000;                                            // First Horsemen Mark is applied at 20 sec.
        HolyWrath_Timer = 12000;                                     // right
        ShieldWall1 = true;
        ShieldWall2 = true;
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(SAY_ZELI_AGGRO, me);
    }

    void KilledUnit(Unit* /*Victim*/)
    {
        DoScriptText(SAY_ZELI_SLAY, me);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_ZELI_DEATH, me);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!UpdateVictim())
            return;

        // Mark of Zeliek
        if (Mark_Timer <= diff)
        {
            DoCast(me->getVictim(),SPELL_MARK_OF_ZELIEK);
            Mark_Timer = 12000;
        } else Mark_Timer -= diff;

        // Shield Wall - All 4 horsemen will shield wall at 50% hp and 20% hp for 20 seconds
        if (ShieldWall1 && (me->GetHealth()*100 / me->GetMaxHealth()) < 50)
        {
            if (ShieldWall1)
            {
                DoCast(me,SPELL_SHIELDWALL);
                ShieldWall1 = false;
            }
     }
        if (ShieldWall2 && (me->GetHealth()*100 / me->GetMaxHealth()) < 20)
        {
            if (ShieldWall2)
            {
                DoCast(me,SPELL_SHIELDWALL);
                ShieldWall2 = false;
            }
        }

        // Holy Wrath
        if (HolyWrath_Timer <= diff)
        {
            DoCast(me->getVictim(),SPELL_HOLY_WRATH);
            HolyWrath_Timer = 12000;
        } else HolyWrath_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sir_zeliek(Creature* pCreature)
{
    return new boss_sir_zeliekAI (pCreature);
}

void AddSC_boss_four_horsemen()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_lady_blaumeux";
    newscript->GetAI = &GetAI_boss_lady_blaumeux;
    newscript->RegisterSelf();

    /*
    //Disabled to prevent "CRASH ALERT!"
    //this boss is wrong here, only for wotlk is need it.
    newscript = new Script;
    newscript->Name = "boss_rivendare_naxx";
    newscript->GetAI = &GetAI_boss_rivendare_naxx;
    newscript->RegisterSelf();
    */
    newscript = new Script;
    newscript->Name = "boss_thane_korthazz";
    newscript->GetAI = &GetAI_boss_thane_korthazz;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_sir_zeliek";
    newscript->GetAI = &GetAI_boss_sir_zeliek;
  newscript->RegisterSelf();
}
