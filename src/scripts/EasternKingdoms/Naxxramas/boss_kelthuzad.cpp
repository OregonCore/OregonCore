/*
 * Copyright (C) 2010-2014 OregonCore <http://www.oregoncore.com/>
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
SDName: Boss_KelThuzud
SD%Complete: 0
SDComment: VERIFY SCRIPT
SDCategory: Naxxramas
EndScriptData */

#include "ScriptPCH.h"

//when shappiron dies. dialog between kel and lich king (in this order)
#define SAY_SAPP_DIALOG1            -1533084
#define SAY_SAPP_DIALOG2_LICH       -1533085
#define SAY_SAPP_DIALOG3            -1533086
#define SAY_SAPP_DIALOG4_LICH       -1533087
#define SAY_SAPP_DIALOG5            -1533088

//when cat dies
#define SAY_CAT_DIED                -1533089

//when each of the 4 wing bosses dies
#define SAY_TAUNT1                  -1533090
#define SAY_TAUNT2                  -1533091
#define SAY_TAUNT3                  -1533092
#define SAY_TAUNT4                  -1533093

#define SAY_SUMMON_MINIONS          -1533105                //start of phase 1

#define SAY_AGGRO1                  -1533094                //start of phase 2
#define SAY_AGGRO2                  -1533095
#define SAY_AGGRO3                  -1533096

#define SAY_SLAY1                   -1533097
#define SAY_SLAY2                   -1533098

#define SAY_DEATH                   -1533099

#define SAY_CHAIN1                  -1533100
#define SAY_CHAIN2                  -1533101
#define SAY_FROST_BLAST             -1533102

#define SAY_REQUEST_AID             -1533103                //start of phase 3
#define SAY_ANSWER_REQUEST          -1533104                //lich king answer

#define SAY_SPECIAL1_MANA_DET       -1533106
#define SAY_SPECIAL3_MANA_DET       -1533107
#define SAY_SPECIAL2_DISPELL        -1533108

//***THIS SCRIPTS IS UNDER DEVELOPMENT***
/*
DATA.
This script has been made with info taken from wowwikki... so there are things wrong...
like spell timers and Says. Also there's another major problem as there is no aggroed list
I cannot make Kel'thuzad to pTarget specific party members, that is needed for spells like
Mana Detonation... so what I'm doing untill now is just to cast everything on my main aggroed
pTarget. Sorry for him.
Another bug is that there are spells that are actually NOT working... I have to implement
them first.
Need DISPELL efect
I also don't know the emotes
*/

//common needed defines
#define NAXXRAMAS_MAP                533
//Positional defines
#define ADDX_LEFT_FAR               3783.272705f
#define ADDY_LEFT_FAR               -5062.697266f
#define ADDZ_LEFT_FAR               143.711203f
#define ADDO_LEFT_FAR               3.617599f

#define ADDX_LEFT_MIDDLE            3730.291260f
#define ADDY_LEFT_MIDDLE            -5027.239258f
#define ADDZ_LEFT_MIDDLE            143.956909f
#define ADDO_LEFT_MIDDLE            4.461900f

#define ADDX_LEFT_NEAR              3683.868652f
#define ADDY_LEFT_NEAR              -5057.281250f
#define ADDZ_LEFT_NEAR              143.183884f
#define ADDO_LEFT_NEAR              5.237086f

#define ADDX_RIGHT_FAR              3759.355225f
#define ADDY_RIGHT_FAR              -5174.128418f
#define ADDZ_RIGHT_FAR              143.802383f
#define ADDO_RIGHT_FAR              2.170104f

#define ADDX_RIGHT_MIDDLE           370.724365f
#define ADDY_RIGHT_MIDDLE           -5185.123047f
#define ADDZ_RIGHT_MIDDLE           143.928024f
#define ADDO_RIGHT_MIDDLE           1.309310f

#define ADDX_RIGHT_NEAR             3665.121094f
#define ADDY_RIGHT_NEAR             -5138.679199f
#define ADDZ_RIGHT_NEAR             143.183212f
#define ADDO_RIGHT_NEAR             0.604023f

#define WALKX_LEFT_FAR              3754.431396f
#define WALKY_LEFT_FAR              -5080.727734f
#define WALKZ_LEFT_FAR              142.036316f
#define WALKO_LEFT_FAR              3.736189f

#define WALKX_LEFT_MIDDLE           3724.396484f
#define WALKY_LEFT_MIDDLE           -5061.330566f
#define WALKZ_LEFT_MIDDLE           142.032700f
#define WALKO_LEFT_MIDDLE           4.564785f

#define WALKX_LEFT_NEAR             3687.158424f
#define WALKY_LEFT_NEAR             -5076.834473f
#define WALKZ_LEFT_NEAR             142.017319f
#define WALKO_LEFT_NEAR             5.237086f

#define WALKX_RIGHT_FAR             3687.571777f
#define WALKY_RIGHT_FAR             -5126.831055f
#define WALKZ_RIGHT_FAR             142.017807f
#define WALKO_RIGHT_FAR             0.604023f

#define WALKX_RIGHT_MIDDLE          3707.990733f
#define WALKY_RIGHT_MIDDLE          -5151.450195f
#define WALKZ_RIGHT_MIDDLE          142.032562f
#define WALKO_RIGHT_MIDDLE          1.376855f

#define WALKX_RIGHT_NEAR            3739.500000f
#define WALKY_RIGHT_NEAR            -5141.883989f
#define WALKZ_RIGHT_NEAR            142.0141130f
#define WALKO_RIGHT_NEAR            2.121412f

//spells to be casted
#define SPELL_FROST_BOLT            28478
#define H_SPELL_FROST_BOLT          55802
#define SPELL_FROST_BOLT_NOVA       28479
#define H_SPELL_FROST_BOLT_NOVA     55807

#define SPELL_CHAINS_OF_KELTHUZAD   28410
#define SPELL_MANA_DETONATION       27819
#define SPELL_SHADOW_FISURE         27810
#define SPELL_FROST_BLAST           27808

struct boss_kelthuzadAI : public ScriptedAI
{
    boss_kelthuzadAI(Creature* c) : ScriptedAI(c)
    {
        GuardiansOfIcecrown[0] = 0;
        GuardiansOfIcecrown[1] = 0;
        GuardiansOfIcecrown[2] = 0;
        GuardiansOfIcecrown[3] = 0;
        GuardiansOfIcecrown[4] = 0;
        GuardiansOfIcecrown_Count = 0;
    }

    uint64 GuardiansOfIcecrown[5];
    uint32 GuardiansOfIcecrown_Count;
    uint32 GuardiansOfIcecrown_Timer;
    uint32 FrostBolt_Timer;
    uint32 FrostBoltNova_Timer;
    uint32 ChainsOfKelthuzad_Timer;
    uint32 ManaDetonation_Timer;
    uint32 ShadowFisure_Timer;
    uint32 FrostBlast_Timer;
    uint32 ChainsOfKelthuzad_Targets;
    uint32 Phase1_Timer;
    bool Phase2;
    bool Phase3;

    void Reset()
    {
        FrostBolt_Timer = (rand()%60)*1000;                 //It won't be more than a minute without cast it
        FrostBoltNova_Timer = 15000;                        //Cast every 15 seconds
        ChainsOfKelthuzad_Timer = (rand()%30+30)*1000;      //Cast no sooner than once every 30 seconds
        ManaDetonation_Timer = 20000;                       //Seems to cast about every 20 seconds
        ShadowFisure_Timer = 25000;                         //25 seconds
        FrostBlast_Timer = (rand()%30+30)*1000;             //Random time between 30-60 seconds
        GuardiansOfIcecrown_Timer = 5000;                   //5 seconds for summoning each Guardian of Icecrown in phase 3

        for (int i=0; i<5; i++)
        {
            if (GuardiansOfIcecrown[i])
        {
            //delete creature
            Unit* pUnit = Unit::GetUnit((*me), GuardiansOfIcecrown[i]);
            if (pUnit && pUnit->isAlive())
                pUnit->DealDamage(pUnit, pUnit->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            GuardiansOfIcecrown[i] = 0;
        }
        }

        Phase1_Timer = 310000;                              //Phase 1 lasts 5 minutes and 10 seconds
        Phase2 = false;
        Phase3 = false;
    }

    void KilledUnit()
    {
        if (rand()%2)
            DoScriptText(SAY_SLAY1, me);
        else
            DoScriptText(SAY_SLAY2, me);
    }

    void JustDied(Unit*)
    {
        DoScriptText(SAY_DEATH, me);
        for (int i=0; i<5; i++)
            if (GuardiansOfIcecrown[i])
        {
            Unit* pUnit = Unit::GetUnit((*me), GuardiansOfIcecrown[i]);
            if (!pUnit || !pUnit->isAlive())
                continue;

            pUnit->CombatStop();
            float Walk_Pos_X = 0;
            float Walk_Pos_Y = 0;
            float Walk_Pos_Z = 0;
            switch(rand()%6)
            {
                case 0:
                    Walk_Pos_X = ADDX_LEFT_FAR;
                    Walk_Pos_Y = ADDY_LEFT_FAR;
                    Walk_Pos_Z = ADDZ_LEFT_FAR;
                    break;
                case 1:
                    Walk_Pos_X = ADDX_LEFT_MIDDLE;
                    Walk_Pos_Y = ADDY_LEFT_MIDDLE;
                    Walk_Pos_Z = ADDZ_LEFT_MIDDLE;
                    break;
                case 2:
                    Walk_Pos_X = ADDX_LEFT_NEAR;
                    Walk_Pos_Y = ADDY_LEFT_NEAR;
                    Walk_Pos_Z = ADDZ_LEFT_NEAR;
                    break;
                case 3:
                    Walk_Pos_X = ADDX_RIGHT_FAR;
                    Walk_Pos_Y = ADDY_RIGHT_FAR;
                    Walk_Pos_Z = ADDZ_RIGHT_FAR;
                    break;
                case 4:
                    Walk_Pos_X = ADDX_RIGHT_MIDDLE;
                    Walk_Pos_Y = ADDY_RIGHT_MIDDLE;
                    Walk_Pos_Z = ADDZ_RIGHT_MIDDLE;
                    break;
                case 5:
                    Walk_Pos_X = ADDX_RIGHT_NEAR;
                    Walk_Pos_Y = ADDY_RIGHT_NEAR;
                    Walk_Pos_Z = ADDZ_RIGHT_NEAR;
                    break;
            }
            pUnit->SendMonsterMoveWithSpeed(Walk_Pos_X, Walk_Pos_Y, Walk_Pos_Z,MOVEFLAG_WALK_MODE);
        }
    }

    void EnterCombat(Unit*)
    {
        switch(rand()%3)
        {
        case 0: DoScriptText(SAY_AGGRO1, me); break;
        case 1: DoScriptText(SAY_AGGRO2, me); break;
        case 2: DoScriptText(SAY_AGGRO3, me); break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
         if (!UpdateVictim())
            return;

        if (me->getVictim() && me->isAlive())
        {
            //Check for Frost Bolt
            if (FrostBolt_Timer <= diff)
            {
                DoCast(me->getVictim(),SPELL_FROST_BOLT);
                //Cast again on time
                FrostBolt_Timer = (rand()%60)*1000;
            } else FrostBolt_Timer -= diff;

            //Check for Frost Bolt Nova
            if (FrostBoltNova_Timer <= diff)
            {
                DoCast(me->getVictim(),SPELL_FROST_BOLT_NOVA);
                FrostBoltNova_Timer = 15000;
            } else FrostBoltNova_Timer -= diff;

            //Check for Chains Of Kelthuzad
            if (ChainsOfKelthuzad_Timer <= diff)
            {
                //DoCast(me->getVictim(),SPELL_CHAINS_OF_KELTHUZAD);

                //if (rand()%2 == 0)
                   //DoScriptText(SAY_CHAIN1, me);
                //else
                    //DoScriptText(SAY_CHAIN2, me);
                ChainsOfKelthuzad_Timer = (rand()%30+30)*1000;
            } else ChainsOfKelthuzad_Timer -= diff;

            //Check for Mana Detonation
            if (ManaDetonation_Timer <= diff)
            {
                //time to cast
                DoCast(me->getVictim(),SPELL_MANA_DETONATION);

                 if (rand()%2)
                     DoScriptText(SAY_SPECIAL1_MANA_DET, me);
                ManaDetonation_Timer = 20000;
            } else ManaDetonation_Timer -= diff;

            //Check for Shadow Fissure
            if (ShadowFisure_Timer <= diff)
            {
                DoCast(me->getVictim(),SPELL_SHADOW_FISURE);

               if (rand()%2)
                   DoScriptText(SAY_SPECIAL3_MANA_DET, me);
                ShadowFisure_Timer = 25000;
            } else ShadowFisure_Timer -= diff;

            //Check for Frost Blast
            if (FrostBlast_Timer <= diff)
            {
                //time to cast
                DoCast(me->getVictim(),SPELL_FROST_BLAST);

                if (rand()%2 == 0)
                    DoScriptText(SAY_FROST_BLAST, me);
                FrostBlast_Timer = (rand()%30+30)*1000;
            } else FrostBlast_Timer -= diff;

            //start phase 3 when we are 40% health
            if (!Phase3 && (me->GetHealth()*100 / me->GetMaxHealth()) < 40)
            {
                Phase3 = true;
                    DoScriptText(SAY_REQUEST_AID, me);
                //here Lich King should respond to KelThuzad but I don't know which creature to make talk
                //so for now just make Kelthuzad says it.
                DoScriptText(SAY_ANSWER_REQUEST, me);
            }

            if (Phase3 && (GuardiansOfIcecrown_Count < 5))
            {
                if (GuardiansOfIcecrown_Timer <= diff)
                {
                    //Summon a Guardian of Icecrown in a random alcove (Creature # 16441)
                    //uint32 TimeToWalk;
                    Unit* pUnit = NULL;
                    float Walk_Pos_X;
                    float Walk_Pos_Y;
                    float Walk_Pos_Z;
                    switch(rand()%6)
                    {
                        case 0:
                            pUnit = me->SummonCreature(16441,ADDX_LEFT_FAR,ADDY_LEFT_FAR,ADDZ_LEFT_FAR,ADDO_LEFT_FAR,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,1000);
                            //Setting walk position
                            Walk_Pos_X = WALKX_LEFT_FAR;
                            Walk_Pos_Y = WALKY_LEFT_FAR;
                            Walk_Pos_Z = WALKZ_LEFT_FAR;
                            break;
                        case 1:
                            pUnit = me->SummonCreature(16441,ADDX_LEFT_MIDDLE,ADDY_LEFT_MIDDLE,ADDZ_LEFT_MIDDLE,ADDO_LEFT_MIDDLE,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,1000);
                            //Start moving guardian towards the center of the room
                            Walk_Pos_X = WALKX_LEFT_MIDDLE;
                            Walk_Pos_Y = WALKY_LEFT_MIDDLE;
                            Walk_Pos_Z = WALKZ_LEFT_MIDDLE;
                            break;
                        case 2:
                            pUnit = me->SummonCreature(16441,ADDX_LEFT_NEAR,ADDY_LEFT_NEAR,ADDZ_LEFT_NEAR,ADDO_LEFT_NEAR,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,1000);
                            //Start moving guardian towards the center of the room
                            Walk_Pos_X = WALKX_LEFT_NEAR;
                            Walk_Pos_Y = WALKY_LEFT_NEAR;
                            Walk_Pos_Z = WALKZ_LEFT_NEAR;
                            break;
                        case 3:

                            pUnit = me->SummonCreature(16441,ADDX_RIGHT_FAR,ADDY_RIGHT_FAR,ADDZ_RIGHT_FAR,ADDO_RIGHT_FAR,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,1000);
                            //Start moving guardian towards the center of the room
                            Walk_Pos_X = WALKX_RIGHT_FAR;
                            Walk_Pos_Y = WALKY_RIGHT_FAR;
                            Walk_Pos_Z = WALKZ_RIGHT_FAR;
                            break;
                        case 4:
                            pUnit = me->SummonCreature(16441,ADDX_RIGHT_MIDDLE,ADDY_RIGHT_MIDDLE,ADDZ_RIGHT_MIDDLE,ADDO_RIGHT_MIDDLE,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,1000);
                            //Start moving guardian towards the center of the room
                            Walk_Pos_X = WALKX_RIGHT_MIDDLE;
                            Walk_Pos_Y = WALKY_RIGHT_MIDDLE;
                            Walk_Pos_Z = WALKZ_RIGHT_MIDDLE;
                            break;
                        case 5:
                            pUnit = me->SummonCreature(16441,ADDX_RIGHT_NEAR,ADDY_RIGHT_NEAR,ADDZ_RIGHT_NEAR,ADDO_RIGHT_NEAR,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,1000);
                            //Start moving guardian towards the center of the room
                            Walk_Pos_X = WALKX_RIGHT_NEAR;
                            Walk_Pos_Y = WALKY_RIGHT_NEAR;
                            Walk_Pos_Z = WALKZ_RIGHT_NEAR;
                            break;
                    }

                    if (pUnit)
                    {
                        //if we find no one to figth walk to the center
                        if (!pUnit->isInCombat())
                            pUnit->SendMonsterMoveWithSpeed(Walk_Pos_X,Walk_Pos_Y,Walk_Pos_Z,MOVEFLAG_WALK_MODE);

                        //Safe storing of creatures
                        GuardiansOfIcecrown[GuardiansOfIcecrown_Count] = pUnit->GetGUID();

                        //Update guardian count
                        GuardiansOfIcecrown_Count++;

                    }
                    //5 seconds until summoning next guardian
                    GuardiansOfIcecrown_Timer = 5000;
                }
                else GuardiansOfIcecrown_Timer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    }
};

CreatureAI* GetAI_boss_kelthuzadAI(Creature* pCreature)
{
    return new boss_kelthuzadAI (pCreature);
}

void AddSC_boss_kelthuzad()
{

    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_kelthuzad";
    newscript->GetAI = &GetAI_boss_kelthuzadAI;
    newscript->RegisterSelf();
}

