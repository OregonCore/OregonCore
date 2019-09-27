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
SDName: Custom_Example
SD%Complete: 100
SDComment: Short custom scripting example
SDCategory: Script Examples
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"

// **** This script is designed as an example for others to build on ****
// **** Please modify whatever you'd like to as this script is only for developement ****

// **** Script Info ****
// This script is written in a way that it can be used for both friendly and hostile monsters
// Its primary purpose is to show just how much you can really do with scripts
// I recommend trying it out on both an agressive NPC and on friendly npc

// **** Quick Info ****
// Functions with Handled Function marked above them are functions that are called automatically by the core
// Functions that are marked Custom Function are functions I've created to simplify code

#define SPELL_BUFF      25661
#define SPELL_ONE       12555
#define SPELL_ONE_ALT   24099
#define SPELL_TWO       10017
#define SPELL_THREE     26027
#define SPELL_ENRAGE    23537
#define SPELL_BESERK    32309

#define SAY_AGGRO       "Let the games begin."
#define SAY_RANDOM_0    "I see endless suffering. I see torment. I see rage. I see everything."
#define SAY_RANDOM_1    "Muahahahaha"
#define SAY_RANDOM_2    "These mortal infedels my lord, they have invaded your sanctum and seek to steal your secrets."
#define SAY_RANDOM_3    "You are already dead."
#define SAY_RANDOM_4    "Where to go? What to do? So many choices that all end in pain, end in death."
#define SAY_BESERK      "$N, I sentance you to death!"
#define SAY_PHASE       "The suffering has just begun!"

#define GOSSIP_ITEM     "I'm looking for a fight"
#define SAY_DANCE       "I always thought I was a good dancer"
#define SAY_SALUTE      "Move out Soldier!"

struct custom_exampleAI : public ScriptedAI
{
    //*** HANDLED FUNCTION ***
    //This is the constructor, called only once when the creature is first created
    custom_exampleAI(Creature* c) : ScriptedAI(c) {}

    //*** CUSTOM VARIABLES ****
    //These variables are for use only by this individual script.
    //Nothing else will ever call them but us.

    uint32 Say_Timer;                                       //Timer for random chat
    uint32 Rebuff_Timer;                                    //Timer for rebuffing
    uint32 Spell_1_Timer;                                   //Timer for spell 1 when in combat
    uint32 Spell_2_Timer;                                   //Timer for spell 1 when in combat
    uint32 Spell_3_Timer;                                   //Timer for spell 1 when in combat
    uint32 Beserk_Timer;                                    //Timer until we go into Beserk (enraged) mode
    uint32 Phase;                                           //The current battle phase we are in
    uint32 Phase_Timer;                                     //Timer until phase transition

    //*** HANDLED FUNCTION ***
    //This is called whenever the core decides we need to evade
    void Reset()
    {
        Phase = 1;                                          //Start in phase 1
        Phase_Timer = 60000;                                //60 seconds
        Spell_1_Timer = 5000;                               //5 seconds
        Spell_2_Timer = 37000;                              //37 seconds
        Spell_3_Timer = 19000;                              //19 seconds
        Beserk_Timer = 120000;                              //2 minutes
    }

    //*** HANDLED FUNCTION ***
    //Attack Start is called whenever someone hits us.
    void EnterCombat(Unit* /*who*/)
    {
        //Say some stuff
        me->MonsterSay(SAY_AGGRO, LANG_UNIVERSAL, 0);
        DoPlaySoundToSet(me, 8280);
    }

    //*** HANDLED FUNCTION ***
    //Update AI is called Every single map update (roughly once every 100ms if a player is within the grid)
    void UpdateAI(const uint32 diff)
    {
        //Out of combat timers
        if (!me->GetVictim())
        {
            //Random Say timer
            if (Say_Timer <= diff)
            {
                //Random switch between 5 outcomes
                switch (rand() % 5)
                {
                case 0:
                    me->MonsterYell(SAY_RANDOM_0, LANG_UNIVERSAL, 0);
                    DoPlaySoundToSet(me, 8831); //8831 is the index of the sound we are playing. You find these numbers in SoundEntries.dbc
                    break;

                case 1:
                    me->MonsterYell(SAY_RANDOM_1, LANG_UNIVERSAL, 0);
                    DoPlaySoundToSet(me, 8818);
                    break;

                case 2:
                    me->MonsterYell(SAY_RANDOM_2, LANG_UNIVERSAL, 0);
                    DoPlaySoundToSet(me, 8041);
                    break;

                case 3:
                    me->MonsterYell(SAY_RANDOM_3, LANG_UNIVERSAL, 0);
                    DoPlaySoundToSet(me, 8581);
                    break;

                case 4:
                    me->MonsterYell(SAY_RANDOM_4, LANG_UNIVERSAL, 0);
                    DoPlaySoundToSet(me, 8791);
                    break;
                }

                Say_Timer = 45000;                          //Say something agian in 45 seconds
            }
            else Say_Timer -= diff;

            //Rebuff timer
            if (Rebuff_Timer <= diff)
            {
                DoCast(me, SPELL_BUFF);
                Rebuff_Timer = 900000;                      //Rebuff agian in 15 minutes
            }
            else Rebuff_Timer -= diff;
        }

        //Return since we have no target
        if (!UpdateVictim())
            return;

        //Spell 1 timer
        if (Spell_1_Timer <= diff)
        {
            //Cast spell one on our current target.
            if (rand() % 50 > 10)
                DoCastVictim(SPELL_ONE_ALT);
            else if (me->GetDistance(me->GetVictim()) < 25)
                DoCastVictim(SPELL_ONE);

            Spell_1_Timer = 5000;
        }
        else Spell_1_Timer -= diff;

        //Spell 2 timer
        if (Spell_2_Timer <= diff)
        {
            //Cast spell one on our current target.
            DoCastVictim(SPELL_TWO);

            Spell_2_Timer = 37000;
        }
        else Spell_2_Timer -= diff;

        //Spell 3 timer
        if (Phase > 1)
        {
            if (Spell_3_Timer <= diff)
            {
                //Cast spell one on our current target.
                DoCastVictim(SPELL_THREE);

                Spell_3_Timer = 19000;
            }
            else Spell_3_Timer -= diff;
        }

        //Beserk timer
        if (Phase > 1)
        {
            if (Beserk_Timer <= diff)
            {
                //Say our line then cast uber death spell
                DoPlaySoundToSet(me, 8588);
                me->MonsterYell(SAY_BESERK, LANG_UNIVERSAL, me->GetVictim()->GetGUID());
                DoCastVictim(SPELL_BESERK);

                //Cast our beserk spell agian in 12 seconds if we didn't kill everyone
                Beserk_Timer = 12000;
            }
            else Beserk_Timer -= diff;
        }

        //Phase timer
        if (Phase == 1)
        {
            if (Phase_Timer <= diff)
            {
                //Go to next phase
                Phase++;
                me->MonsterYell(SAY_PHASE, LANG_UNIVERSAL, 0);
                DoCast(me, SPELL_ENRAGE);
            }
            else Phase_Timer -= diff;
        }

        DoMeleeAttackIfReady();
    }

    //Our Recive emote function
    void ReceiveEmote(Player* /*pPlayer*/, uint32 emote)
    {
        me->HandleEmoteCommand(emote);

        if (emote == TEXT_EMOTE_DANCE)
            me->MonsterSay(SAY_DANCE, LANG_UNIVERSAL, 0);

        if (emote == TEXT_EMOTE_SALUTE)
            me->MonsterSay(SAY_SALUTE, LANG_UNIVERSAL, 0);
    }
};

//This is the GetAI method used by all scripts that involve AI
//It is called every time a new creature using this script is created
CreatureAI* GetAI_custom_example(Creature* pCreature)
{
    return new custom_exampleAI (pCreature);
}

//This function is called when the player clicks an option on the gossip menu
void SendDefaultMenu_custom_example(Player* pPlayer, Creature* pCreature, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)               //Fight time
    {
        //Set our faction to hostile twoards all
        pCreature->SetFaction(24);
        pCreature->Attack(pPlayer, true);
        pPlayer->PlayerTalkClass->CloseGossip();
    }
}

//This function is called when the player clicks an option on the gossip menu
bool GossipSelect_custom_example(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiSender == GOSSIP_SENDER_MAIN)
        SendDefaultMenu_custom_example(pPlayer, pCreature, uiAction);

    return true;
}

//This function is called when the player opens the gossip menu
bool GossipHello_custom_example(Player* pPlayer, Creature* pCreature)
{
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM        , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->PlayerTalkClass->SendGossipMenu(907, pCreature->GetGUID());

    return true;
}

//This is the actual function called only once durring InitScripts()
//It must define all handled functions that are to be run in this script
//For example if you want this Script to handle Emotes you must include
//newscript->ReciveEmote = My_Emote_Function;
void AddSC_custom_example()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "custom_example";
    newscript->GetAI = &GetAI_custom_example;
    newscript->pGossipHello = &GossipHello_custom_example;
    newscript->pGossipSelect = &GossipSelect_custom_example;
    newscript->RegisterSelf();
}

