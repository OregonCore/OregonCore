/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify

 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Blackrock_Depths
SD%Complete: 95
SDComment: Quest support: 4001, 4342, 7604, 4322. Vendor Lokhtos Darkbargainer. Need to rewrite the Jail Break support
SDCategory: Blackrock Depths
EndScriptData */

/* ContentData
at_ring_of_law
npc_grimstone
mob_phalanx
npc_kharan_mighthammer
npc_lokhtos_darkbargainer
npc_dughal_stormwing
npc_marshal_windsor
npc_marshal_reginald_windsor
npc_tobias_seecher
npc_rocknot
EndContentData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"
#include "blackrock_depths.h"

#define C_GRIMSTONE         10096
#define C_THELDREN          16059

//4 or 6 in total? 1+2+1 / 2+2+2 / 3+3. Depending on this, code should be changed.
#define MOB_AMOUNT          4

uint32 RingMob[]=
{
    8925,                                                   // Dredge Worm
    8926,                                                   // Deep Stinger
    8927,                                                   // Dark Screecher
    8928,                                                   // Burrowing Thundersnout
    8933,                                                   // Cave Creeper
    8932,                                                   // Borer Beetle
};

uint32 RingBoss[]=
{
    9027,                                                   // Gorosh
    9028,                                                   // Grizzle
    9029,                                                   // Eviscerator
    9030,                                                   // Ok'thor
    9031,                                                   // Anub'shiah
    9032,                                                   // Hedrum
};

float RingLocations[6][3]=
{
    {604.802673f, -191.081985f, -54.058590f},                  // ring
    {604.072998f, -222.106918f, -52.743759f},                  // first gate
    {621.400391f, -214.499054f, -52.814453f},                  // hiding in corner
    {601.300781f, -198.556992f, -53.950256f},                  // ring
    {631.818359f, -180.548126f, -52.654770f},                  // second gate
    {627.390381f, -201.075974f, -52.692917f}                   // hiding in corner
};

bool AreaTrigger_at_ring_of_law(Player *player, const AreaTriggerEntry *at)
{
    ScriptedInstance* pInstance = (player->GetInstanceData());

    if (pInstance)
    {
        if (pInstance->GetData(TYPE_RING_OF_LAW) == IN_PROGRESS || pInstance->GetData(TYPE_RING_OF_LAW) == DONE)
            return false;

        pInstance->SetData(TYPE_RING_OF_LAW,IN_PROGRESS);
        player->SummonCreature(C_GRIMSTONE,625.559f,-205.618f,-52.735f,2.609f,TEMPSUMMON_DEAD_DESPAWN,0);

        return false;
    }
    return false;
}

/*######
## npc_grimstone
######*/

//TODO: implement quest part of event (different end boss)
struct npc_grimstoneAI : public npc_escortAI
{
    npc_grimstoneAI(Creature *c) : npc_escortAI(c)
    {
        pInstance = c->GetInstanceData();
        MobSpawnId = rand()%6;
    }

    ScriptedInstance* pInstance;

    uint8 EventPhase;
    uint32 Event_Timer;

    uint8 MobSpawnId;
    uint8 MobCount;
    uint32 MobDeath_Timer;

    uint64 RingMobGUID[4];
    uint64 RingBossGUID;

    bool CanWalk;

    void Reset()
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        EventPhase = 0;
        Event_Timer = 1000;

        MobCount = 0;
        MobDeath_Timer = 0;

        for (uint8 i = 0; i < MOB_AMOUNT; i++)
            RingMobGUID[i] = 0;

        RingBossGUID = 0;

        CanWalk = false;
    }

    void EnterCombat(Unit *who) { }

    //TODO: move them to center
    void SummonRingMob()
    {
        if (Creature* tmp = me->SummonCreature(RingMob[MobSpawnId],608.960f,-235.322f,-53.907f,1.857f,TEMPSUMMON_DEAD_DESPAWN,0))
            RingMobGUID[MobCount] = tmp->GetGUID();

        ++MobCount;

        if (MobCount == MOB_AMOUNT)
            MobDeath_Timer = 2500;
    }

    //TODO: move them to center
    void SummonRingBoss()
    {
        if (Creature* tmp = me->SummonCreature(RingBoss[rand()%6],644.300f,-175.989f,-53.739f,3.418f,TEMPSUMMON_DEAD_DESPAWN,0))
            RingBossGUID = tmp->GetGUID();

        MobDeath_Timer = 2500;
    }

    void WaypointReached(uint32 i)
    {
        switch(i)
        {
        case 0:
            DoScriptText(-1000000, me);//2
            CanWalk = false;
            Event_Timer = 5000;
            break;
        case 1:
            DoScriptText(-1000000, me);//4
            CanWalk = false;
            Event_Timer = 5000;
            break;
        case 2:
            CanWalk = false;
            break;
        case 3:
            DoScriptText(-1000000, me);//5
            break;
        case 4:
            DoScriptText(-1000000, me);//6
            CanWalk = false;
            Event_Timer = 5000;
            break;
        case 5:
            if (pInstance)
            {
                pInstance->SetData(TYPE_RING_OF_LAW,DONE);
                debug_log("OSCR: npc_grimstone: event reached end and set complete.");
            }
            break;
        }
    }

    void HandleGameObject(uint32 id, bool open)
    {
        pInstance->HandleGameObject(pInstance->GetData64(id), open);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!pInstance)
            return;

        if (MobDeath_Timer)
        {
            if (MobDeath_Timer <= diff)
            {
                MobDeath_Timer = 2500;

                if (RingBossGUID)
                {
                    Creature *boss = Unit::GetCreature(*me,RingBossGUID);
                    if (boss && !boss->isAlive() && boss->isDead())
                    {
                        RingBossGUID = 0;
                        Event_Timer = 5000;
                        MobDeath_Timer = 0;
                        return;
                    }
                    return;
                }

                for (uint8 i = 0; i < MOB_AMOUNT; i++)
                {
                    Creature *mob = Unit::GetCreature(*me,RingMobGUID[i]);
                    if (mob && !mob->isAlive() && mob->isDead())
                    {
                        RingMobGUID[i] = 0;
                        --MobCount;

                        //seems all are gone, so set timer to continue and discontinue this
                        if (!MobCount)
                        {
                            Event_Timer = 5000;
                            MobDeath_Timer = 0;
                        }
                    }
                }
            } else MobDeath_Timer -= diff;
        }

        if (Event_Timer)
        {
            if (Event_Timer <= diff)
            {
                switch(EventPhase)
                {
                case 0:
                    DoScriptText(-1000000, me);//1
                    HandleGameObject(DATA_ARENA4, false);
                    Start(false, false, false);
                    CanWalk = true;
                    Event_Timer = 0;
                    break;
                case 1:
                    CanWalk = true;
                    Event_Timer = 0;
                    break;
                case 2:
                    Event_Timer = 2000;
                    break;
                case 3:
                    HandleGameObject(DATA_ARENA1, true);
                    Event_Timer = 3000;
                    break;
                case 4:
                    CanWalk = true;
                    me->SetVisibility(VISIBILITY_OFF);
                    SummonRingMob();
                    Event_Timer = 8000;
                    break;
                case 5:
                    SummonRingMob();
                    SummonRingMob();
                    Event_Timer = 8000;
                    break;
                case 6:
                    SummonRingMob();
                    Event_Timer = 0;
                    break;
                case 7:
                    me->SetVisibility(VISIBILITY_ON);
                    HandleGameObject(DATA_ARENA1, false);
                    DoScriptText(-1000000, me);//4
                    CanWalk = true;
                    Event_Timer = 0;
                    break;
                case 8:
                    HandleGameObject(DATA_ARENA2, true);
                    Event_Timer = 5000;
                    break;
                case 9:
                    me->SetVisibility(VISIBILITY_OFF);
                    SummonRingBoss();
                    Event_Timer = 0;
                    break;
                case 10:
                    //if quest, complete
                    HandleGameObject(DATA_ARENA2, false);
                    HandleGameObject(DATA_ARENA3, true);
                    HandleGameObject(DATA_ARENA4, true);
                    CanWalk = true;
                    Event_Timer = 0;
                    break;
                }
                ++EventPhase;
            } else Event_Timer -= diff;
        }

        if (CanWalk)
            npc_escortAI::UpdateAI(diff);
       }
};

CreatureAI* GetAI_npc_grimstone(Creature* pCreature)
{
    npc_grimstoneAI* Grimstone_AI = new npc_grimstoneAI(pCreature);

    for (uint8 i = 0; i < 6; ++i)
        Grimstone_AI->AddWaypoint(i, RingLocations[i][0], RingLocations[i][1], RingLocations[i][2]);

    return (CreatureAI*)Grimstone_AI;

}

/*######
## mob_phalanx
######*/

#define SPELL_THUNDERCLAP       8732
#define SPELL_FIREBALLVOLLEY    22425
#define SPELL_MIGHTYBLOW        14099

struct mob_phalanxAI : public ScriptedAI
{
    mob_phalanxAI(Creature *c) : ScriptedAI(c) {}

    uint32 ThunderClap_Timer;
    uint32 FireballVolley_Timer;
    uint32 MightyBlow_Timer;

    void Reset()
    {
        ThunderClap_Timer = 12000;
        FireballVolley_Timer =0;
        MightyBlow_Timer = 15000;
    }

    void EnterCombat(Unit *who)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!UpdateVictim())
            return;

        //ThunderClap_Timer
        if (ThunderClap_Timer <= diff)
        {
            DoCast(me->getVictim(),SPELL_THUNDERCLAP);
            ThunderClap_Timer = 10000;
        } else ThunderClap_Timer -= diff;

        //FireballVolley_Timer
        if (me->GetHealth()*100 / me->GetMaxHealth() < 51)
        {
            if (FireballVolley_Timer <= diff)
            {
                DoCast(me->getVictim(),SPELL_FIREBALLVOLLEY);
                FireballVolley_Timer = 15000;
            } else FireballVolley_Timer -= diff;
        }

        //MightyBlow_Timer
        if (MightyBlow_Timer <= diff)
        {
            DoCast(me->getVictim(),SPELL_MIGHTYBLOW);
            MightyBlow_Timer = 10000;
        } else MightyBlow_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_mob_phalanx(Creature* pCreature)
{
    return new mob_phalanxAI (pCreature);
}

/*######
## npc_kharan_mighthammer
######*/

#define QUEST_4001  4001
#define QUEST_4342  4342

#define GOSSIP_ITEM_KHARAN_1    "I need to know where the princess are, Kharan!"
#define GOSSIP_ITEM_KHARAN_2    "All is not lost, Kharan!"

#define GOSSIP_ITEM_KHARAN_3    "Gor'shak is my friend, you can trust me."
#define GOSSIP_ITEM_KHARAN_4    "Not enough, you need to tell me more."
#define GOSSIP_ITEM_KHARAN_5    "So what happened?"
#define GOSSIP_ITEM_KHARAN_6    "Continue..."
#define GOSSIP_ITEM_KHARAN_7    "So you suspect that someone on the inside was involved? That they were tipped off?"
#define GOSSIP_ITEM_KHARAN_8    "Continue with your story please."
#define GOSSIP_ITEM_KHARAN_9    "Indeed."
#define GOSSIP_ITEM_KHARAN_10   "The door is open, Kharan. You are a free man."

bool GossipHello_npc_kharan_mighthammer(Player *player, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        player->PrepareQuestMenu(pCreature->GetGUID());

    if (player->GetQuestStatus(QUEST_4001) == QUEST_STATUS_INCOMPLETE)
         player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KHARAN_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    if (player->GetQuestStatus(4342) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KHARAN_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);

    if (player->GetTeam() == HORDE)
        player->SEND_GOSSIP_MENU(2473, pCreature->GetGUID());
    else
        player->SEND_GOSSIP_MENU(2474, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_kharan_mighthammer(Player *player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
             player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KHARAN_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(2475, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KHARAN_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            player->SEND_GOSSIP_MENU(2476, pCreature->GetGUID());
            break;

        case GOSSIP_ACTION_INFO_DEF+3:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KHARAN_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            player->SEND_GOSSIP_MENU(2477, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KHARAN_6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            player->SEND_GOSSIP_MENU(2478, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
             player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KHARAN_7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
            player->SEND_GOSSIP_MENU(2479, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KHARAN_8, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+7);
            player->SEND_GOSSIP_MENU(2480, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+7:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KHARAN_9, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+8);
            player->SEND_GOSSIP_MENU(2481, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+8:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KHARAN_10, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+9);
            player->SEND_GOSSIP_MENU(2482, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+9:
            player->CLOSE_GOSSIP_MENU();
            if (player->GetTeam() == HORDE)
                player->AreaExploredOrEventHappens(QUEST_4001);
            else
                player->AreaExploredOrEventHappens(QUEST_4342);
            break;
    }
    return true;
}

/*######
## npc_lokhtos_darkbargainer
######*/

#define ITEM_THRORIUM_BROTHERHOOD_CONTRACT               18628
#define ITEM_SULFURON_INGOT                              17203
#define QUEST_A_BINDING_CONTRACT                         7604
#define SPELL_CREATE_THORIUM_BROTHERHOOD_CONTRACT_DND    23059

#define GOSSIP_ITEM_SHOW_ACCESS     "Show me what I have access to, Lothos."
#define GOSSIP_ITEM_GET_CONTRACT    "Get Thorium Brotherhood Contract"

bool GossipHello_npc_lokhtos_darkbargainer(Player *player, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        player->PrepareQuestMenu(pCreature->GetGUID());

    if (pCreature->isVendor() && player->GetReputationRank(59) >= REP_FRIENDLY)
          player->ADD_GOSSIP_ITEM(1, GOSSIP_ITEM_SHOW_ACCESS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    if (player->GetQuestRewardStatus(QUEST_A_BINDING_CONTRACT) != 1 &&
        !player->HasItemCount(ITEM_THRORIUM_BROTHERHOOD_CONTRACT, 1, true) &&
        player->HasItemCount(ITEM_SULFURON_INGOT, 1))
    {
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_GET_CONTRACT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }

    if (player->GetReputationRank(59) < REP_FRIENDLY)
        player->SEND_GOSSIP_MENU(3673, pCreature->GetGUID());
    else
        player->SEND_GOSSIP_MENU(3677, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_lokhtos_darkbargainer(Player *player, Creature* pCreature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        player->CLOSE_GOSSIP_MENU();
        player->CastSpell(player, SPELL_CREATE_THORIUM_BROTHERHOOD_CONTRACT_DND, false);
    }
    if (action == GOSSIP_ACTION_TRADE)
        player->SEND_VENDORLIST(pCreature->GetGUID());

    return true;
}

/*######
## npc_dughal_stormwing
######*/

#define QUEST_JAIL_BREAK        4322
#define SAY_DUGHAL_FREE         "Thank you, $N! I'm free!!!"
#define GOSSIP_DUGHAL           "You're free, Dughal! Get out of here!"

ScriptedInstance *pInstance;
/*
struct npc_dughal_stormwingAI : public npc_escortAI
{
    npc_dughal_stormwingAI(Creature *c) : npc_escortAI(c) {}

    void WaypointReached(uint32 i)
    {
    switch(i)
        {
        case 0:me->Say(SAY_DUGHAL_FREE, LANG_UNIVERSAL, PlayerGUID); break;
        case 1:pInstance->SetData(DATA_DUGHAL,ENCOUNTER_STATE_OBJECTIVE_COMPLETED);break;
        case 2:
            me->SetVisibility(VISIBILITY_OFF);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pInstance->SetData(DATA_DUGHAL,ENCOUNTER_STATE_ENDED);
            break;
        }
    }

    void EnterCombat(Unit* who) { }
    void Reset() {}

    void JustDied(Unit* killer)
    {
        if (IsBeingEscorted && killer == me)
        {
            me->SetVisibility(VISIBILITY_OFF);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pInstance->SetData(DATA_DUGHAL,ENCOUNTER_STATE_ENDED);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (pInstance->GetData(DATA_QUEST_JAIL_BREAK) == ENCOUNTER_STATE_NOT_STARTED) return;
        if ((pInstance->GetData(DATA_QUEST_JAIL_BREAK) == ENCOUNTER_STATE_IN_PROGRESS || pInstance->GetData(DATA_QUEST_JAIL_BREAK) == ENCOUNTER_STATE_FAILED || pInstance->GetData(DATA_QUEST_JAIL_BREAK) == ENCOUNTER_STATE_ENDED)&& pInstance->GetData(DATA_DUGHAL) == ENCOUNTER_STATE_ENDED)
        {
            me->SetVisibility(VISIBILITY_OFF);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }
        else
        {
            me->SetVisibility(VISIBILITY_ON);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }
        npc_escortAI::UpdateAI(diff);
    }
};
CreatureAI* GetAI_npc_dughal_stormwing(Creature* pCreature)
{
    npc_dughal_stormwingAI* dughal_stormwingAI = new npc_dughal_stormwingAI(pCreature);

    dughal_stormwingAI->AddWaypoint(0, 280.42f,-82.86f, -77.12f,0);
    dughal_stormwingAI->AddWaypoint(1, 287.64f,-87.01f, -76.79f,0);
    dughal_stormwingAI->AddWaypoint(2, 354.63f,-64.95f, -67.53f,0);

    return (CreatureAI*)dughal_stormwingAI;
}
bool GossipHello_npc_dughal_stormwing(Player *player, Creature* pCreature)
{
    if (player->GetQuestStatus(QUEST_JAIL_BREAK) == QUEST_STATUS_INCOMPLETE && pInstance->GetData(DATA_QUEST_JAIL_BREAK) == ENCOUNTER_STATE_IN_PROGRESS)
    {
        player->ADD_GOSSIP_ITEM(0, GOSSIP_DUGHAL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(2846, pCreature->GetGUID());
    }
    return true;
}

bool GossipSelect_npc_dughal_stormwing(Player *player, Creature* pCreature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        player->CLOSE_GOSSIP_MENU();
        ((npc_escortAI*)(pCreature->AI()))->Start(false, false, true, player->GetGUID());
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        pInstance->SetData(DATA_QUEST_JAIL_BREAK,ENCOUNTER_STATE_IN_PROGRESS);
    }
    return true;
}
 */
/*######
## npc_marshal_windsor
######*/

#define SAY_WINDSOR_AGGRO1          "You locked up the wrong Marshal. Prepare to be destroyed!"
#define SAY_WINDSOR_AGGRO2          "I bet you're sorry now, aren't you !?!!"
#define SAY_WINDSOR_AGGRO3          "You better hold me back $N or they are going to feel some prison house beatings."
#define SAY_WINDSOR_1               "Let's get a move on. My gear should be in the storage area up this way..."
#define SAY_WINDSOR_4_1             "Check that cell, $N. If someone is alive in there, we need to get them out."
#define SAY_WINDSOR_4_2             "Get him out of there!"
#define SAY_WINDSOR_4_3             "Good work! We're almost there, $N. This way."
#define SAY_WINDSOR_6               "This is it, $N. My stuff should be in that room. Cover me, I'm going in!"
#define SAY_WINDSOR_9               "Ah, there it is!"
#define MOB_ENTRY_REGINALD_WINDSOR  9682

Player* PlayerStart;
/*
struct npc_marshal_windsorAI : public npc_escortAI
{
    npc_marshal_windsorAI(Creature *c) : npc_escortAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    void WaypointReached(uint32 i)
    {
    switch(i)
        {
        case 1:
            me->Say(SAY_WINDSOR_1, LANG_UNIVERSAL, PlayerGUID);
            break;
        case 7:
            me->HandleEmoteCommand(EMOTE_STATE_POINT);
            me->Say(SAY_WINDSOR_4_1, LANG_UNIVERSAL, PlayerGUID);
            IsOnHold=true;
            break;
        case 10:
            me->setFaction(534);
            break;
        case 12:
            me->Say(SAY_WINDSOR_6, LANG_UNIVERSAL, PlayerGUID);
            pInstance->SetData(DATA_SUPPLY_ROOM, ENCOUNTER_STATE_IN_PROGRESS);
            break;
        case 13:
            me->HandleEmoteCommand(EMOTE_STATE_USESTANDING);//EMOTE_STATE_WORK
            break;
        case 14:
            pInstance->SetData(DATA_GATE_SR,0);
            me->setFaction(11);
            break;
        case 16:
            me->Say(SAY_WINDSOR_9, LANG_UNIVERSAL, PlayerGUID);
            break;
        case 17:
            me->HandleEmoteCommand(EMOTE_STATE_USESTANDING);//EMOTE_STATE_WORK
            break;
        case 18:
            pInstance->SetData(DATA_GATE_SC,0);
            break;
        case 19:
            me->SetVisibility(VISIBILITY_OFF);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SummonCreature(MOB_ENTRY_REGINALD_WINDSOR,403.61f,-51.71f,-63.92f,3.600434f,TEMPSUMMON_DEAD_DESPAWN ,0);
            pInstance->SetData(DATA_SUPPLY_ROOM, ENCOUNTER_STATE_ENDED);
            break;
        }
    }

    void EnterCombat(Unit* who)
        {
        switch(rand()%3)
            {
            case 0:me->Say(SAY_WINDSOR_AGGRO1, LANG_UNIVERSAL, PlayerGUID);break;
            case 1:me->Say(SAY_WINDSOR_AGGRO2, LANG_UNIVERSAL, PlayerGUID);break;
            case 2:me->Say(SAY_WINDSOR_AGGRO3, LANG_UNIVERSAL, PlayerGUID);break;
            }
        }

    void Reset() {}

    void JustDied(Unit *slayer)
    {
        pInstance->SetData(DATA_QUEST_JAIL_BREAK,ENCOUNTER_STATE_FAILED);
    }

    void UpdateAI(const uint32 diff)
    {
        if (pInstance->GetData(DATA_QUEST_JAIL_BREAK) == ENCOUNTER_STATE_NOT_STARTED) return;
        if (pInstance->GetData(DATA_DUGHAL) == ENCOUNTER_STATE_OBJECTIVE_COMPLETED)
            IsOnHold = false;
        if (!pInstance->GetData(DATA_GATE_D) && pInstance->GetData(DATA_DUGHAL) == ENCOUNTER_STATE_NOT_STARTED)
            {
            me->Say(SAY_WINDSOR_4_2, LANG_UNIVERSAL, PlayerGUID);
            pInstance->SetData(DATA_DUGHAL, ENCOUNTER_STATE_BEFORE_START);
            }
        if (pInstance->GetData(DATA_DUGHAL) == ENCOUNTER_STATE_OBJECTIVE_COMPLETED)
            {
            me->Say(SAY_WINDSOR_4_3, LANG_UNIVERSAL, PlayerGUID);
            pInstance->SetData(DATA_DUGHAL, ENCOUNTER_STATE_ENDED);
            }
        if ((pInstance->GetData(DATA_QUEST_JAIL_BREAK) == ENCOUNTER_STATE_IN_PROGRESS || pInstance->GetData(DATA_QUEST_JAIL_BREAK) == ENCOUNTER_STATE_FAILED || pInstance->GetData(DATA_QUEST_JAIL_BREAK) == ENCOUNTER_STATE_ENDED)&& pInstance->GetData(DATA_SUPPLY_ROOM) == ENCOUNTER_STATE_ENDED)
        {
            me->SetVisibility(VISIBILITY_OFF);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }
        else
        {
            me->SetVisibility(VISIBILITY_ON);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }
        npc_escortAI::UpdateAI(diff);
    }
};
CreatureAI* GetAI_npc_marshal_windsor(Creature* pCreature)
{
    npc_marshal_windsorAI* marshal_windsorAI = new npc_marshal_windsorAI(pCreature);

    marshal_windsorAI->AddWaypoint(0, 316.336f,-225.528f, -77.7258f,7000);
    marshal_windsorAI->AddWaypoint(1, 316.336f,-225.528f, -77.7258f,2000);
    marshal_windsorAI->AddWaypoint(2, 322.96f,-207.13f, -77.87f,0);
    marshal_windsorAI->AddWaypoint(3, 281.05f,-172.16f, -75.12f,0);
    marshal_windsorAI->AddWaypoint(4, 272.19f,-139.14f, -70.61f,0);
    marshal_windsorAI->AddWaypoint(5, 283.62f,-116.09f, -70.21f,0);
    marshal_windsorAI->AddWaypoint(6, 296.18f,-94.30f, -74.08f,0);
    marshal_windsorAI->AddWaypoint(7, 294.57f,-93.11f, -74.08f,0);
    marshal_windsorAI->AddWaypoint(8, 314.31f,-74.31f, -76.09f,0);
    marshal_windsorAI->AddWaypoint(9, 360.22f,-62.93f, -66.77f,0);
    marshal_windsorAI->AddWaypoint(10, 383.38f,-69.40f, -63.25f,0);
    marshal_windsorAI->AddWaypoint(11, 389.99f,-67.86f, -62.57f,0);
    marshal_windsorAI->AddWaypoint(12, 400.98f,-72.01f, -62.31f,0);
    marshal_windsorAI->AddWaypoint(13, 404.22f,-62.30f, -63.50f,2300);
    marshal_windsorAI->AddWaypoint(14, 404.22f,-62.30f, -63.50f,1500);
    marshal_windsorAI->AddWaypoint(154, 407.65f,-51.86f, -63.96f,0);
    marshal_windsorAI->AddWaypoint(16, 403.61f,-51.71f, -63.92f,1000);
    marshal_windsorAI->AddWaypoint(17, 403.61f,-51.71f, -63.92f,2000);
    marshal_windsorAI->AddWaypoint(18, 403.61f,-51.71f, -63.92f,1000);
    marshal_windsorAI->AddWaypoint(19, 403.61f,-51.71f, -63.92f,0);

    return (CreatureAI*)marshal_windsorAI;
}

bool QuestAccept_npc_marshal_windsor(Player *player, Creature *creature, Quest const *quest)
{
    if (quest->GetQuestId() == 4322)
        {PlayerStart = player;
        if (pInstance->GetData(DATA_QUEST_JAIL_BREAK) == ENCOUNTER_STATE_NOT_STARTED)
        {
                ((npc_escortAI*)(creature->AI()))->Start(true, true, false, player->GetGUID());
                pInstance->SetData(DATA_QUEST_JAIL_BREAK,ENCOUNTER_STATE_IN_PROGRESS);
                creature->setFaction(11);
        }

        }
    return false;
}
  */
/*######
## npc_marshal_reginald_windsor
######*/

#define SAY_REGINALD_WINDSOR_0_1    "Can you feel the power, $N??? It's time to ROCK!"
#define SAY_REGINALD_WINDSOR_0_2    "Now we just have to free Tobias and we can get out of here. This way!"
#define SAY_REGINALD_WINDSOR_5_1    "Open it."
#define SAY_REGINALD_WINDSOR_5_2    "I never did like those two. Let's get moving."
#define SAY_REGINALD_WINDSOR_7_1    "Open it and be careful this time!"
#define SAY_REGINALD_WINDSOR_7_2    "That intolerant dirtbag finally got what was coming to him. Good riddance!"
#define SAY_REGINALD_WINDSOR_7_3    "Alright, let's go."
#define SAY_REGINALD_WINDSOR_13_1   "Open it. We need to hurry up. I can smell those Dark Irons coming a mile away and I can tell you one thing, they're COMING!"
#define SAY_REGINALD_WINDSOR_13_2   "Administering fists of fury on Crest Killer!"
#define SAY_REGINALD_WINDSOR_13_3   "He has to be in the last cell. Unless... they killed him."
#define SAY_REGINALD_WINDSOR_14_1   "Get him out of there!"
#define SAY_REGINALD_WINDSOR_14_2   "Excellent work, $N. Let's find the exit. I think I know the way. Follow me!"
#define SAY_REGINALD_WINDSOR_20_1   "We made it!"
#define SAY_REGINALD_WINDSOR_20_2   "Meet me at Maxwell's encampment. We'll go over the next stages of the plan there and figure out a way to decode my tablets without the decryption ring."
#define MOB_ENTRY_SHILL_DINGER      9678
#define MOB_ENTRY_CREST_KILLER      9680

int wp = 0;
/*
struct npc_marshal_reginald_windsorAI : public npc_escortAI
{
    npc_marshal_reginald_windsorAI(Creature *c) : npc_escortAI(c)
    {
    }

    void WaypointReached(uint32 i)
    {
    wp=i;
    switch(i)
        {
        case 0:
            me->setFaction(11);
            me->Say(SAY_REGINALD_WINDSOR_0_1, LANG_UNIVERSAL, PlayerGUID);
            break;
        case 1:
            me->Say(SAY_REGINALD_WINDSOR_0_2, LANG_UNIVERSAL, PlayerGUID);
            break;
        case 7:
            me->HandleEmoteCommand(EMOTE_STATE_POINT);
            me->Say(SAY_REGINALD_WINDSOR_5_1, LANG_UNIVERSAL, PlayerGUID);
            IsOnHold=true;
            break;
        case 8:
            me->Say(SAY_REGINALD_WINDSOR_5_2, LANG_UNIVERSAL, PlayerGUID);
            break;
        case 11:
            me->HandleEmoteCommand(EMOTE_STATE_POINT);
            me->Say(SAY_REGINALD_WINDSOR_7_1, LANG_UNIVERSAL, PlayerGUID);
            IsOnHold=true;
            break;
        case 12:
            me->Say(SAY_REGINALD_WINDSOR_7_2, LANG_UNIVERSAL, PlayerGUID);
            break;
        case 13:
            me->Say(SAY_REGINALD_WINDSOR_7_3, LANG_UNIVERSAL, PlayerGUID);
            break;
        case 20:
            me->HandleEmoteCommand(EMOTE_STATE_POINT);
            me->Say(SAY_REGINALD_WINDSOR_13_1, LANG_UNIVERSAL, PlayerGUID);
            IsOnHold=true;
            break;
        case 21:
            me->Say(SAY_REGINALD_WINDSOR_13_3, LANG_UNIVERSAL, PlayerGUID);
            break;
        case 23:
            me->HandleEmoteCommand(EMOTE_STATE_POINT);
            me->Say(SAY_REGINALD_WINDSOR_14_1, LANG_UNIVERSAL, PlayerGUID);
            IsOnHold=true;
            break;
        case 24:
            me->Say(SAY_REGINALD_WINDSOR_14_2, LANG_UNIVERSAL, PlayerGUID);
            break;
        case 31:
            me->Say(SAY_REGINALD_WINDSOR_20_1, LANG_UNIVERSAL, PlayerGUID);
            break;
        case 32:
            me->Say(SAY_REGINALD_WINDSOR_20_2, LANG_UNIVERSAL, PlayerGUID);
            PlayerStart->GroupEventHappens(QUEST_JAIL_BREAK, me);
            pInstance->SetData(DATA_SHILL, ENCOUNTER_STATE_ENDED);
            break;
        }
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (IsBeingEscorted)
            return;

        if (who->GetTypeId() == TYPEID_PLAYER)
        {
            if (CAST_PLR(who)->GetQuestStatus(4322) == QUEST_STATUS_INCOMPLETE)
            {
                float Radius = 10.0f;
                if (me->IsWithinDistInMap(who, Radius))
                {
                    IsOnHold = false;
                    ((npc_escortAI*)(me->AI()))->Start(true, true, false, who->GetGUID());
                }
            }
        }
    }

    void EnterCombat(Unit* who)
        {
        switch(rand()%3)
            {
            case 0:me->Say(SAY_WINDSOR_AGGRO1, LANG_UNIVERSAL, PlayerGUID);break;
            case 1:me->Say(SAY_WINDSOR_AGGRO2, LANG_UNIVERSAL, PlayerGUID);break;
            case 2:me->Say(SAY_WINDSOR_AGGRO3, LANG_UNIVERSAL, PlayerGUID);break;
            }
        }
    void Reset() {}

    void JustDied(Unit *slayer)
    {
        pInstance->SetData(DATA_QUEST_JAIL_BREAK,ENCOUNTER_STATE_FAILED);
    }

    void UpdateAI(const uint32 diff)
    {
        if (pInstance->GetData(DATA_QUEST_JAIL_BREAK) == ENCOUNTER_STATE_NOT_STARTED) return;
        if (wp == 7)
            {
            if (!pInstance->GetData(DATA_GATE_J) && pInstance->GetData(DATA_JAZ) == ENCOUNTER_STATE_NOT_STARTED)
                {
                    pInstance->SetData(DATA_CREATURE_JAZ,1);
                    pInstance->SetData(DATA_JAZ,ENCOUNTER_STATE_IN_PROGRESS);
                }
            if (pInstance->GetData(DATA_CREATURE_JAZ) && pInstance->GetData(DATA_CREATURE_OGRABISI) && pInstance->GetData(DATA_JAZ) == ENCOUNTER_STATE_IN_PROGRESS)
                {
                    IsOnHold = false;
                    pInstance->SetData(DATA_JAZ,ENCOUNTER_STATE_ENDED);
                }
            }
        else if (wp == 11)
            {
            if (!pInstance->GetData(DATA_GATE_S) && pInstance->GetData(DATA_SHILL) == ENCOUNTER_STATE_NOT_STARTED)
                {
                    pInstance->SetData(DATA_CREATURE_SHILL,1);
                    pInstance->SetData(DATA_SHILL,ENCOUNTER_STATE_IN_PROGRESS);
                }
            if (pInstance->GetData(DATA_CREATURE_SHILL) && pInstance->GetData(DATA_SHILL) == ENCOUNTER_STATE_IN_PROGRESS)
                {
                    pInstance->SetData(DATA_SHILL,ENCOUNTER_STATE_ENDED);
                    IsOnHold = false;
                }
            }
        else if (wp == 20)
            {
            if (!pInstance->GetData(DATA_GATE_C) && pInstance->GetData(DATA_CREST) == ENCOUNTER_STATE_NOT_STARTED)
                {
                    pInstance->SetData(DATA_CREATURE_CREST,1);
                    me->Say(SAY_REGINALD_WINDSOR_13_2, LANG_UNIVERSAL, PlayerGUID);
                    pInstance->SetData(DATA_CREST,ENCOUNTER_STATE_IN_PROGRESS);
                }
            if (pInstance->GetData(DATA_CREATURE_CREST) && pInstance->GetData(DATA_CREST) == ENCOUNTER_STATE_IN_PROGRESS)
                {
                    IsOnHold = false;
                    pInstance->SetData(DATA_CREST,ENCOUNTER_STATE_ENDED);
                }
            }
        if (pInstance->GetData(DATA_TOBIAS) == ENCOUNTER_STATE_OBJECTIVE_COMPLETED) IsOnHold = false;
        npc_escortAI::UpdateAI(diff);
    }
};
CreatureAI* GetAI_npc_marshal_reginald_windsor(Creature* pCreature)
{
    npc_marshal_reginald_windsorAI* marshal_reginald_windsorAI = new npc_marshal_reginald_windsorAI(pCreature);

    marshal_reginald_windsorAI->AddWaypoint(0, 403.61f,-52.71f, -63.92f,4000);
    marshal_reginald_windsorAI->AddWaypoint(1, 403.61f,-52.71f, -63.92f,4000);
    marshal_reginald_windsorAI->AddWaypoint(2, 406.33f,-54.87f, -63.95f,0);
    marshal_reginald_windsorAI->AddWaypoint(3, 407.99f,-73.91f, -62.26f,0);
    marshal_reginald_windsorAI->AddWaypoint(4, 557.03f,-119.71f, -61.83f,0);
    marshal_reginald_windsorAI->AddWaypoint(5, 573.40f,-124.39f, -65.07f,0);
    marshal_reginald_windsorAI->AddWaypoint(6, 593.91f,-130.29f, -69.25f,0);
    marshal_reginald_windsorAI->AddWaypoint(7, 593.21f,-132.16f, -69.25f,0);
    marshal_reginald_windsorAI->AddWaypoint(8, 593.21f,-132.16f, -69.25f,3000);
    marshal_reginald_windsorAI->AddWaypoint(9, 622.81f,-135.55f, -71.92f,0);
    marshal_reginald_windsorAI->AddWaypoint(10, 634.68f,-151.29f, -70.32f,0);
    marshal_reginald_windsorAI->AddWaypoint(11, 635.06f,-153.25f, -70.32f,0);
    marshal_reginald_windsorAI->AddWaypoint(12, 635.06f,-153.25f, -70.32f,3000);
    marshal_reginald_windsorAI->AddWaypoint(13, 635.06f,-153.25f, -70.32f,1500);
    marshal_reginald_windsorAI->AddWaypoint(14, 655.25f,-172.39f, -73.72f,0);
    marshal_reginald_windsorAI->AddWaypoint(15, 654.79f,-226.30f, -83.06f,0);
    marshal_reginald_windsorAI->AddWaypoint(16, 622.85f,-268.85f, -83.96f,0);
    marshal_reginald_windsorAI->AddWaypoint(17, 579.45f,-275.56f, -80.44f,0);
    marshal_reginald_windsorAI->AddWaypoint(18, 561.19f,-266.85f, -75.59f,0);
    marshal_reginald_windsorAI->AddWaypoint(19, 547.91f,-253.92f, -70.34f,0);
    marshal_reginald_windsorAI->AddWaypoint(20, 549.20f,-252.40f, -70.34f,0);
    marshal_reginald_windsorAI->AddWaypoint(21, 549.20f,-252.40f, -70.34f,4000);
    marshal_reginald_windsorAI->AddWaypoint(22, 555.33f,-269.16f, -74.40f,0);
    marshal_reginald_windsorAI->AddWaypoint(23, 554.31f,-270.88f, -74.40f,0);
    marshal_reginald_windsorAI->AddWaypoint(24, 554.31f,-270.88f, -74.40f,4000);
    marshal_reginald_windsorAI->AddWaypoint(25, 536.10f,-249.60f, -67.47f,0);
    marshal_reginald_windsorAI->AddWaypoint(26, 520.94f,-216.65f, -59.28f,0);
    marshal_reginald_windsorAI->AddWaypoint(27, 505.99f,-148.74f, -62.17f,0);
    marshal_reginald_windsorAI->AddWaypoint(28, 484.21f,-56.24f, -62.43f,0);
    marshal_reginald_windsorAI->AddWaypoint(29, 470.39f,-6.01f, -70.10f,0);
    marshal_reginald_windsorAI->AddWaypoint(30, 451.27f,30.85f, -70.07f,0);
    marshal_reginald_windsorAI->AddWaypoint(31, 452.45f,29.85f, -70.37f,1500);
    marshal_reginald_windsorAI->AddWaypoint(32, 452.45f,29.85f, -70.37f,7000);
    marshal_reginald_windsorAI->AddWaypoint(33, 452.45f,29.85f, -70.37f,10000);
    marshal_reginald_windsorAI->AddWaypoint(34, 451.27f,31.85f, -70.07f,0);

    return (CreatureAI*)marshal_reginald_windsorAI;
}
*/
/*######
## npc_tobias_seecher
######*/

#define SAY_TOBIAS_FREE         "Thank you! I will run for safety immediately!"
/*
struct npc_tobias_seecherAI : public npc_escortAI
{
    npc_tobias_seecherAI(Creature *c) :npc_escortAI(c) {}

    void EnterCombat(Unit* who) { }
    void Reset() {}

    void JustDied(Unit* killer)
    {
        if (IsBeingEscorted && killer == me)
        {
            me->SetVisibility(VISIBILITY_OFF);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pInstance->SetData(DATA_TOBIAS,ENCOUNTER_STATE_ENDED);
        }
    }

    void WaypointReached(uint32 i)
    {
    switch(i)
        {
        case 0:me->Say(SAY_TOBIAS_FREE, LANG_UNIVERSAL, PlayerGUID); break;
        case 2:
            pInstance->SetData(DATA_TOBIAS,ENCOUNTER_STATE_OBJECTIVE_COMPLETED);break;
        case 4:
            me->SetVisibility(VISIBILITY_OFF);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pInstance->SetData(DATA_TOBIAS,ENCOUNTER_STATE_ENDED);
            break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (pInstance->GetData(DATA_QUEST_JAIL_BREAK) == ENCOUNTER_STATE_NOT_STARTED) return;
        if ((pInstance->GetData(DATA_QUEST_JAIL_BREAK) == ENCOUNTER_STATE_IN_PROGRESS || pInstance->GetData(DATA_QUEST_JAIL_BREAK) == ENCOUNTER_STATE_FAILED || pInstance->GetData(DATA_QUEST_JAIL_BREAK) == ENCOUNTER_STATE_ENDED)&& pInstance->GetData(DATA_TOBIAS) == ENCOUNTER_STATE_ENDED)
        {
            me->SetVisibility(VISIBILITY_OFF);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }
        else
        {
            me->SetVisibility(VISIBILITY_ON);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }
        npc_escortAI::UpdateAI(diff);
    }
};

CreatureAI* GetAI_npc_tobias_seecher(Creature* pCreature)
{
    npc_tobias_seecherAI* tobias_seecherAI = new npc_tobias_seecherAI(pCreature);

    tobias_seecherAI->AddWaypoint(0, 549.21f, -281.07f, -75.27f);
    tobias_seecherAI->AddWaypoint(1, 554.39f, -267.39f, -73.68f);
    tobias_seecherAI->AddWaypoint(2, 533.59f, -249.38f, -67.04f);
    tobias_seecherAI->AddWaypoint(3, 519.44f, -217.02f, -59.34f);
    tobias_seecherAI->AddWaypoint(4, 506.55f, -153.49f, -62.34f);

    return (CreatureAI*)tobias_seecherAI;
}

bool GossipHello_npc_tobias_seecher(Player *player, Creature* pCreature)
{
    if (player->GetQuestStatus(QUEST_JAIL_BREAK) == QUEST_STATUS_INCOMPLETE && pInstance->GetData(DATA_QUEST_JAIL_BREAK) == ENCOUNTER_STATE_IN_PROGRESS)
    {
        player->ADD_GOSSIP_ITEM(0, "Get out of here, Tobias, you're free!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(2847, pCreature->GetGUID());
    }
    return true;
}

bool GossipSelect_npc_tobias_seecher(Player *player, Creature* pCreature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        player->CLOSE_GOSSIP_MENU();
        ((npc_escortAI*)(pCreature->AI()))->Start(false, false, true, player->GetGUID());
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        pInstance->SetData(DATA_TOBIAS,ENCOUNTER_STATE_IN_PROGRESS);
    }
    return true;
}
*/

/*######
## npc_rocknot
######*/

#define SAY_GOT_BEER        -1230000
#define SPELL_DRUNKEN_RAGE  14872
#define QUEST_ALE           4295

float BarWpLocations[8][3]=
{
    {883.294861f, -188.926300f, -43.703655f},
    {872.763550f, -185.605621f, -43.703655f},                  //b1
    {867.923401f, -188.006393f, -43.703655f},                  //b2
    {863.295898f, -190.795212f, -43.703655f},                  //b3
    {856.139587f, -194.652756f, -43.703655f},                  //b4
    {851.878906f, -196.928131f, -43.703655f},                  //b5
    {877.035217f, -187.048080f, -43.703655f},
    {891.198000f, -197.924000f, -43.620400f}                   //home
};

uint32 BarWpWait[8]=
{
    0,
    5000,
    5000,
    5000,
    5000,
    15000,
    0,
    0
};

struct npc_rocknotAI : public npc_escortAI
{
    npc_rocknotAI(Creature *c) : npc_escortAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 BreakKeg_Timer;
    uint32 BreakDoor_Timer;

    void Reset()
    {
        if (HasEscortState(STATE_ESCORT_ESCORTING))
            return;

        BreakKeg_Timer = 0;
        BreakDoor_Timer = 0;
    }

    void EnterCombat(Unit *who) { }

    void DoGo(uint32 id, uint32 state)
    {
        if (GameObject *go = GameObject::GetGameObject(*me,pInstance->GetData64(id)))
            go->SetGoState((GOState)state);
    }

    void WaypointReached(uint32 i)
    {
        if (!pInstance)
            return;

        switch(i)
        {
        case 1:
            me->HandleEmoteCommand(EMOTE_ONESHOT_KICK);
            break;
        case 2:
            me->HandleEmoteCommand(EMOTE_ONESHOT_ATTACKUNARMED);
            break;
        case 3:
            me->HandleEmoteCommand(EMOTE_ONESHOT_ATTACKUNARMED);
            break;
        case 4:
            me->HandleEmoteCommand(EMOTE_ONESHOT_KICK);
            break;
        case 5:
            me->HandleEmoteCommand(EMOTE_ONESHOT_KICK);
            BreakKeg_Timer = 2000;
            break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!pInstance)
            return;

        if (BreakKeg_Timer)
        {
            if (BreakKeg_Timer <= diff)
            {
                DoGo(DATA_GO_BAR_KEG,0);
                BreakKeg_Timer = 0;
                BreakDoor_Timer = 1000;
            } else BreakKeg_Timer -= diff;
        }

        if (BreakDoor_Timer)
        {
            if (BreakDoor_Timer <= diff)
            {
                DoGo(DATA_GO_BAR_DOOR,2);
                DoGo(DATA_GO_BAR_KEG_TRAP,0);               //doesn't work very well, leaving code here for future
                //spell by trap has effect61, this indicate the bar go hostile

                if (Unit *tmp = Unit::GetUnit(*me,pInstance->GetData64(DATA_PHALANX)))
                    tmp->setFaction(14);

                //for later, this event(s) has alot more to it.
                //optionally, DONE can trigger bar to go hostile.
                pInstance->SetData(TYPE_BAR,DONE);

                BreakDoor_Timer = 0;
            } else BreakDoor_Timer -= diff;
        }

        npc_escortAI::UpdateAI(diff);
    }
};

CreatureAI* GetAI_npc_rocknot(Creature* pCreature)
{
    npc_rocknotAI* Rocknot_AI = new npc_rocknotAI(pCreature);

    for (uint8 i = 0; i < 8; ++i)
        Rocknot_AI->AddWaypoint(i, BarWpLocations[i][0], BarWpLocations[i][1], BarWpLocations[i][2], BarWpWait[i]);

    return (CreatureAI*)Rocknot_AI;
}

bool ChooseReward_npc_rocknot(Player *player, Creature* pCreature, const Quest *_Quest, uint32 item)
{
    ScriptedInstance* pInstance = (pCreature->GetInstanceData());

    if (!pInstance)
        return true;

    if (pInstance->GetData(TYPE_BAR) == DONE || pInstance->GetData(TYPE_BAR) == SPECIAL)
        return true;

    if (_Quest->GetQuestId() == QUEST_ALE)
    {
        if (pInstance->GetData(TYPE_BAR) != IN_PROGRESS)
            pInstance->SetData(TYPE_BAR,IN_PROGRESS);

        pInstance->SetData(TYPE_BAR,SPECIAL);

        //keep track of amount in instance script, returns SPECIAL if amount ok and event in progress
        if (pInstance->GetData(TYPE_BAR) == SPECIAL)
        {
            DoScriptText(SAY_GOT_BEER, pCreature);
            pCreature->CastSpell(pCreature,SPELL_DRUNKEN_RAGE,false);
            if (npc_escortAI* pEscortAI = CAST_AI(npc_rocknotAI, pCreature->AI()))
                pEscortAI->Start(false, false);
        }
    }

    return true;
}

/*######
##
######*/

void AddSC_blackrock_depths()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "at_ring_of_law";
    newscript->pAreaTrigger = &AreaTrigger_at_ring_of_law;
    newscript->RegisterSelf();

     newscript = new Script;
     newscript->Name = "npc_grimstone";
     newscript->GetAI = &GetAI_npc_grimstone;
     newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_phalanx";
    newscript->GetAI = &GetAI_mob_phalanx;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_kharan_mighthammer";
    newscript->pGossipHello =  &GossipHello_npc_kharan_mighthammer;
    newscript->pGossipSelect = &GossipSelect_npc_kharan_mighthammer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_lokhtos_darkbargainer";
    newscript->pGossipHello =  &GossipHello_npc_lokhtos_darkbargainer;
    newscript->pGossipSelect = &GossipSelect_npc_lokhtos_darkbargainer;
    newscript->RegisterSelf();
/*
    newscript = new Script;
    newscript->Name = "npc_dughal_stormwing";
    newscript->pGossipHello =  &GossipHello_npc_dughal_stormwing;
    newscript->pGossipSelect = &GossipSelect_npc_dughal_stormwing;
    newscript->GetAI = &GetAI_npc_dughal_stormwing;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tobias_seecher";
    newscript->pGossipHello =  &GossipHello_npc_tobias_seecher;
    newscript->pGossipSelect = &GossipSelect_npc_tobias_seecher;
    newscript->GetAI = &GetAI_npc_tobias_seecher;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_marshal_windsor";
    newscript->pQuestAccept = &QuestAccept_npc_marshal_windsor;
    newscript->GetAI = &GetAI_npc_marshal_windsor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_marshal_reginald_windsor";
    newscript->GetAI = &GetAI_npc_marshal_reginald_windsor;
    newscript->RegisterSelf();*/

     newscript = new Script;
     newscript->Name = "npc_rocknot";
     newscript->GetAI = &GetAI_npc_rocknot;
     newscript->pChooseReward = &ChooseReward_npc_rocknot;
     newscript->RegisterSelf();
}

