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
SDName: Old_Hillsbrad
SD%Complete: 95
SDComment:
SDTodo: We need to move all texts and sounds to DB. Possibly can clean up a whole lot.
SDCategory: Caverns of Time, Old Hillsbrad Foothills
EndScriptData */

/* ContentData
npc_erozion
npc_thrall_old_hillsbrad
npc_taretha
erozion_image
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "old_hillsbrad.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"

enum Quests
{
    QUEST_ENTRY_HILLSBRAD  = 10282,
    QUEST_ENTRY_DIVERSION  = 10283,
    QUEST_ENTRY_ESCAPE     = 10284,
    QUEST_ENTRY_RETURN     = 10285
};

enum Items
{
    ITEM_ENTRY_BOMBS       = 25853
};

/*######
## npc_erozion
######*/

bool GossipHello_npc_erozion(Player *player, Creature *creature)
{
    if (creature->isQuestGiver())
        player->PrepareQuestMenu(creature->GetGUID());

    ScriptedInstance* pInstance = (ScriptedInstance*)creature->GetInstanceData();
    if ( pInstance && pInstance->GetData(TYPE_BARREL_DIVERSION) != DONE && !player->HasItemCount(ITEM_ENTRY_BOMBS, 1))
        player->ADD_GOSSIP_ITEM( 0, "I need a pack of Incendiary Bombs.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    player->SEND_GOSSIP_MENU(9778, creature->GetGUID());

    return true;
}

bool GossipSelect_npc_erozion(Player *player, Creature *creature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF+1)
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_ENTRY_BOMBS, 1);
        if (msg == EQUIP_ERR_OK)
             player->StoreNewItem(dest, ITEM_ENTRY_BOMBS, true);

        player->SEND_GOSSIP_MENU(9515, creature->GetGUID());
    }

    return true;
}

/*######
## npc_thrall_old_hillsbrad
######*/

enum Thrall
{
    //Thrall event texts
    SAY_TH_START_EVENT_PART1     = -1560023,
    SAY_ARMORER_CALL_GUARDS      = -1560059,
    SAY_TH_KILL_ARMORER          = -1560050,
    SAY_TH_ARMORY                = -1560024,
    SAY_TH_ARMORY_2              = -1560060,
    SAY_AMBUSH_P1                = -1560073,
    SAY_AMBUSH2_P1               = -1560074,
    SAY_THRALL_END_P1            = -1560072,
    SAY_TH_SKARLOC_MEET          = -1560025,
    SAY_TH_SKARLOC_TAUNT         = -1560026,
    SAY_TH_START_EVENT_PART2     = -1560027,
    SAY_TH_MOUNTS_UP             = -1560028,
    EMOTE_TH_STARTLE_HORSE       = -1560061,
    EMOTE_TH_CALM_HORSE          = -1560062,
    SAY_TH_HEAD_TOWN             = -1560063,
    SAY_TH_CHURCH_ENTER          = -1560064,
    SAY_TH_CHURCH_END            = -1560029,
    SAY_TH_MEET_TARETHA          = -1560030,
    SAY_TH_EPOCH_WONDER          = -1560031,
    SAY_TH_EPOCH_KILL_TARETHA    = -1560032,
    SAY_TH_EVENT_COMPLETE        = -1560033,

    SAY_TA_FREE                  = -1560048,
    SAY_TA_ESCAPED               = -1560049,
    SAY_TR_GLAD_SAFE             = -1560054,
    SAY_TA_NEVER_MET             = -1560055,
    SAY_TR_THEN_WHO              = -1560056,
    SAY_WIPE_MEMORY              = -1560051,
    SAY_ABOUT_TARETHA            = -1560052,
    SAY_TA_FAREWELL              = -1560053,
    SAY_LOOKOUT_BARN_1           = -1560065,
    SAY_PROTECTOR_BARN_2         = -1560066,
    SAY_LOOKOUT_CHURCH           = -1560067,
    SAY_LOOKOUT_INN              = -1560068,

    SAY_EPOCH_ENTER1             = -1560013,
    SAY_EPOCH_ENTER2             = -1560014,
    SAY_EPOCH_ENTER3             = -1560015,

    SAY_IMAGE_1                  = -1560075,
    SAY_IMAGE_2                  = -1560076,

    SAY_TH_RANDOM_LOW_HP1        = -1560034,
    SAY_TH_RANDOM_LOW_HP2        = -1560035,

    SAY_TH_RANDOM_DIE1           = -1560036,
    SAY_TH_RANDOM_DIE2           = -1560037,

    SAY_TH_RANDOM_AGGRO1         = -1560038,
    SAY_TH_RANDOM_AGGRO2         = -1560039,
    SAY_TH_RANDOM_AGGRO3         = -1560040,
    SAY_TH_RANDOM_AGGRO4         = -1560041,

    SAY_TH_RANDOM_KILL1          = -1560042,
    SAY_TH_RANDOM_KILL2          = -1560043,
    SAY_TH_RANDOM_KILL3          = -1560044,

    SAY_TH_LEAVE_COMBAT1         = -1560045,
    SAY_TH_LEAVE_COMBAT2         = -1560046,
    SAY_TH_LEAVE_COMBAT3         = -1560047,

    NPC_IMAGE_OF_ERONZION        = 19438,
    NPC_SKARLOC_MOUNT            = 18798,
    NPC_THRALL_QUEST_TRIGGER     = 20156,

    NPC_WARDEN                   = 17833,
    NPC_VETERAN                  = 17860,
    NPC_MAGE                     = 18934,
    NPC_SENTRY                   = 17819,

    NPC_CHURCH_GUARDSMAN         = 23175,
    NPC_CHURCH_PROTECTOR         = 23179,
    NPC_CHURCH_LOOKOUT           = 23177,

    NPC_INN_GUARDSMAN            = 23176,
    NPC_INN_PROTECTOR            = 23180,
    NPC_INN_LOOKOUT              = 23178,

    NPC_INFINITE_DEFILER         = 18171,
    NPC_INFINITE_SABOTEOR        = 18172,
    NPC_INFINITE_SLAYER          = 18170,

    NPC_TARREN_MILL_GUARDSMAN    = 18092,
    NPC_TARREN_MILL_PROTECTOR    = 18093,
    NPC_TARREN_MILL_LOOKOUT      = 18094,

    NPC_ARMORER                  = 18764,
    NPC_SKARLOC                  = 17862,
    NPC_EPOCH                    = 18096,
    NPC_THRALL                   = 17876,
    NPC_EROZION                  = 18723,

    SPELL_KNOCKOUT_ARMORER       = 32890,
    SPELL_STRIKE                 = 14516,
    SPELL_SHIELD_BLOCK           = 12169,
    SPELL_SHADOW_SPIKE           = 33125,
    SPELL_TELEPORT               = 7791,
    SPELL_MEMORY_WIPE            = 33336,
    SPELL_MEMORY_WP_RESUME       = 33337,
    SPELL_SHADOW_PRISON          = 33071, 

    GO_PRISON_DOOR               = 184393
};

#define SPEED_WALK              (0.5f)
#define SPEED_RUN               (1.0f)
#define SPEED_MOUNT             (1.6f)

#define THRALL_WEAPON_MODEL     22106
#define THRALL_WEAPON_INFO      218169346
#define THRALL_SHIELD_MODEL     18662
#define THRALL_SHIELD_INFO      234948100
#define THRALL_MODEL_UNEQUIPPED 17292
#define THRALL_MODEL_EQUIPPED   18165
#define SKARLOC_MOUNT_MODEL     18223

//gossip items
#define GOSSIP_ITEM_START       "We are ready to get you out of here, Thrall. Let's go!"
#define GOSSIP_ID_START         9568
#define GOSSIP_ID_SKARLOC1      9614                        //I'm glad Taretha is alive. We now must find a way to free her...
#define GOSSIP_ITEM_SKARLOC1    "Taretha cannot see you, Thrall."
#define GOSSIP_ID_SKARLOC2      9579                        //What do you mean by this? Is Taretha in danger?
#define GOSSIP_ITEM_SKARLOC2    "The situation is rather complicated, Thrall. It would be best for you to head into the mountains now, before more of Blackmoore's men show up. We'll make sure Taretha is safe."
#define GOSSIP_ID_SKARLOC3      9580
#define GOSSIP_ITEM_SKARLOC3    "Tarren Mill."
#define GOSSIP_ID_TARREN        9597                        //tarren mill is beyond these trees
#define GOSSIP_ITEM_TARREN      "We're ready, Thrall."
#define GOOSIP_ID_TARETHA       9614
#define GOSSIP_ID_COMPLETE      9578                        //Thank you friends, I owe my freedom to you. Where is Taretha? I hoped to see her

struct npc_thrall_old_hillsbradAI : public npc_escortAI
{
    npc_thrall_old_hillsbradAI(Creature *creature) : npc_escortAI(creature)
    {
        pInstance = (ScriptedInstance*)creature->GetInstanceData();
        HadMount = false;
        me->setActive(true);
        me->AddUnitState(UNIT_STATE_IGNORE_PATHFINDING);
    }

    ScriptedInstance *pInstance;

    uint64 TarethaGUID;
    uint64 MountGUID;
    uint64 ArmorerGUID;
    uint64 ImageGUID;
    uint64 EpochGUID;
    uint8 Part;
    uint32 Steps;
    uint32 StepsTimer;
    uint32 StrikeTimer;
    uint32 ShieldBlockTimer;
    uint32 EmoteTimer;

    bool LowHp;
    bool HadMount;
    bool Event;

    void Reset()
    {
        LowHp = false;
        Event = false;
        
        MountGUID = 0;
        ArmorerGUID = 0;
        ImageGUID = 0;
        EpochGUID = 0;
        TarethaGUID = pInstance->GetData64(DATA_TARETHA);

        Steps = 0;
        StepsTimer = 0;
        EmoteTimer = 0;
        Part = 0;
        StrikeTimer = urand(3000, 7000);
        ShieldBlockTimer = urand(6000, 11000);

        if (HadMount)
            DoMount();

        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            DoUnmount();
            HadMount = false;
        }
        else
            DoScriptText(RAND(SAY_TH_LEAVE_COMBAT1, SAY_TH_LEAVE_COMBAT2, SAY_TH_LEAVE_COMBAT3), me);

        if (pInstance && pInstance->GetData(TYPE_THRALL_PART1) == NOT_STARTED)
        {
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, 0);
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, 0);
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+1, 0);
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, 0);
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+2, 0);
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+3, 0);
            me->SetUInt32Value(UNIT_FIELD_DISPLAYID, THRALL_MODEL_UNEQUIPPED);
        }
        else
        {
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, THRALL_WEAPON_MODEL);
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, THRALL_WEAPON_INFO);
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+1, 781);
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, THRALL_SHIELD_MODEL);
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+2, THRALL_SHIELD_INFO);
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+3, 1038);
            me->SetUInt32Value(UNIT_FIELD_DISPLAYID, THRALL_MODEL_EQUIPPED);
        }
    }

    void WaypointReached(uint32 i)
    {
        switch (Part)
        {
            case 1:
                switch (i)
                {
                    case 0:
                        if (GameObject* door = me->FindNearestGameObject(GO_PRISON_DOOR, INTERACTION_DISTANCE))
                            door->UseDoorOrButton(6);
                        break;
                    case 7:
                        if (Creature* Armorer = me->SummonCreature(NPC_ARMORER, 2180.95f, 119.19f, 89.456f, 5.692f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 35000))
                            DoScriptText(SAY_ARMORER_CALL_GUARDS, Armorer);
                        break;
                    case 8:
                        SetRun(false);
                        break;
                    case 9:
                        DoScriptText(SAY_TH_KILL_ARMORER, me);
                        if (Creature* Armorer = me->GetMap()->GetCreature(ArmorerGUID))
                        {
                            DoCast(Armorer, SPELL_KNOCKOUT_ARMORER);
                            Armorer->setDeathState(JUST_DIED);
                        }
                        break;
                    case 10:
                        DoScriptText(SAY_TH_ARMORY, me);
                        me->SetStandState(UNIT_STAND_STATE_KNEEL);
                        break;
                    case 11:
                        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, THRALL_WEAPON_MODEL);
                        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, THRALL_WEAPON_INFO);
                        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+1, 781);
                        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY+1, THRALL_SHIELD_MODEL);
                        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+2, THRALL_SHIELD_INFO);
                        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO+3, 1038);
                        me->SetUInt32Value(UNIT_FIELD_DISPLAYID, THRALL_MODEL_EQUIPPED);
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        SetRun();
                        break;
                    case 12:
                        if (Player* player = GetPlayerForEscort())
                            me->SetFacingToObject(player);
                        DoScriptText(SAY_TH_ARMORY_2, me);
                        break;
                    case 17:
                        me->SummonCreature(NPC_MAGE, 2186.909f, 139.8108f, 88.21628f, 5.75f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        me->SummonCreature(NPC_WARDEN, 2187.943f, 141.6124f, 88.21628f, 5.73f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        me->SummonCreature(NPC_VETERAN, 2190.508f, 140.4597f, 88.21628f, 6.04f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        me->SummonCreature(NPC_VETERAN, 2189.543f, 139.0996f, 88.23965f, 0.21f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        break;
                    case 20:
                        me->SummonCreature(NPC_MAGE, 2149.463f, 104.9756f, 73.63239f, 1.71f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        me->SummonCreature(NPC_SENTRY, 2147.642f, 105.0251f, 73.99422f, 1.52f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        me->SummonCreature(NPC_VETERAN, 2149.212f, 107.2005f, 74.15676f, 1.71f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        me->SummonCreature(NPC_WARDEN, 2147.328f, 106.7235f, 74.34447f, 1.69f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        break;
                    case 23:
                        me->SummonCreature(NPC_MAGE, 2142.363f, 172.4260f, 66.30494f, 2.54f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        me->SummonCreature(NPC_SENTRY, 2138.177f, 168.6046f, 66.30494f, 2.47f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        me->SummonCreature(NPC_SENTRY, 2142.372f, 174.2907f, 66.30494f, 2.56f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        me->SummonCreature(NPC_VETERAN, 2140.146f, 169.2364f, 66.30494f, 2.49f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        break;
                    case 25:
                        me->SummonCreature(NPC_MAGE, 2107.938f, 192.0753f, 66.30494f, 2.54f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        me->SummonCreature(NPC_MAGE, 2109.852f, 195.1403f, 66.30493f, 2.42f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        me->SummonCreature(NPC_VETERAN, 2108.486f, 189.9346f, 66.30494f, 2.68f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        me->SummonCreature(NPC_VETERAN, 2112.387f, 195.4947f, 66.30494f, 2.39f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        break;
                    case 31:
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_POINT);
                        EmoteTimer = 2000;
                        me->SetOrientation(2.18f);
                        me->SummonCreature(NPC_SKARLOC, 2000.201f, 277.9190f, 66.4911f, 6.11f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                        DoScriptText(SAY_TH_SKARLOC_MEET, me);
                        break;
                    case 33:
                        me->SetOrientation(2.18f);
                        DoScriptText(SAY_THRALL_END_P1, me);
                        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        break;
                }
                break;
            case 2:
                switch (i)
                {
                    case 1:
                        DoScriptText(SAY_TH_MOUNTS_UP, me);
                        DoMount();
                        if (Creature *mount = (Creature*)(Unit::GetUnit((*me), pInstance->GetData64(DATA_SKARLOC_MOUNT))))
                            mount->ForcedDespawn();
                        else if (Creature* mount = me->FindNearestCreature(18798, 50.0f, true))
                            mount->ForcedDespawn();
                        SetRun();
                        break;
                    case 29:
                        me->SummonCreature(NPC_SKARLOC_MOUNT, 2488.64f, 625.77f, 58.26f, 4.71f,TEMPSUMMON_TIMED_DESPAWN,10000);
                        DoUnmount();
                        HadMount = false;
                        SetRun(false);
                        break;
                    case 30:
                        if (Creature* mount = me->GetMap()->GetCreature(MountGUID))
                            me->SetFacingToObject(mount);
                        DoScriptText(EMOTE_TH_STARTLE_HORSE, me);
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_EXCLAMATION);
                        EmoteTimer = 2000;
                        break;
                    case 31:
                        if (Creature* mount = me->GetMap()->GetCreature(MountGUID))
                        {
                            mount->SetWalk(false);
                            mount->GetMotionMaster()->MovePoint(0, 2517.504f, 506.253f, 42.329f);
                        }
                        me->SetOrientation(4.66f);
                        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        pInstance->SetData(TYPE_THRALL_PART2, DONE);
                        break;
                }
                break;
            case 3:
                switch (i)
                {
                    case 3:
                        SetRun(false);
                        break;
                    case 6:
                        DoScriptText(EMOTE_TH_CALM_HORSE, me);
                        break;
                    case 9:
                        me->SummonCreature(NPC_TARREN_MILL_PROTECTOR, 2510.16f, 693.98f, 55.50f, 2.84f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                        me->SummonCreature(NPC_TARREN_MILL_LOOKOUT, 2510.02f, 697.72f, 55.51f, 3.38f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                        me->SummonCreature(NPC_TARREN_MILL_GUARDSMAN, 2507.83f, 693.76f, 55.50f, 3.14f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                        me->SummonCreature(NPC_TARREN_MILL_GUARDSMAN, 2508.22f, 698.00f, 55.50f, 3.14f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                        break;
                    case 13:
                        DoScriptText(SAY_TH_HEAD_TOWN, me);
                        SetRun();
                        break;
                    case 24:
                        if (Player* player = GetPlayerForEscort())
                            me->SetFacingToObject(player);
                        DoScriptText(SAY_TH_CHURCH_ENTER, me);
                        break;
                    case 25:
                        me->SummonCreature(NPC_CHURCH_PROTECTOR, 2627.88f, 657.63f, 55.98f, 4.28f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        me->SummonCreature(NPC_CHURCH_LOOKOUT, 2627.27f, 655.17f, 56.03f, 4.50f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        me->SummonCreature(NPC_CHURCH_LOOKOUT, 2629.21f, 654.81f, 56.04f, 4.38f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        me->SummonCreature(NPC_CHURCH_GUARDSMAN, 2629.98f, 656.96f, 55.96f, 4.34f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        break;
                    case 27:
                        DoScriptText(SAY_TH_CHURCH_END, me);
                        break;
                    case 35:
                        me->SummonCreature(NPC_INN_PROTECTOR, 2652.71f, 660.31f, 61.93f, 1.67f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        me->SummonCreature(NPC_INN_LOOKOUT, 2648.96f, 662.59f, 61.93f, 0.79f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        me->SummonCreature(NPC_INN_LOOKOUT, 2657.36f, 662.34f, 61.93f, 2.68f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        me->SummonCreature(NPC_INN_GUARDSMAN, 2656.39f, 659.77f, 61.93f, 2.61f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                        break;
                    case 36:
                        SetRun(false);
                        break;
                    case 37:
                        if (Creature* Taretha = me->GetMap()->GetCreature(TarethaGUID))
                            DoScriptText(SAY_TA_ESCAPED, Taretha, me);
                        break;
                    case 38:
                        pInstance->SetData(TYPE_THRALL_PART3,DONE);
                        DoScriptText(SAY_TH_MEET_TARETHA, me);
                        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        break;
                }
                break;
            case 4:
                switch (i)
                {
                    case 0:
                       if (Creature* Epoch = me->SummonCreature(NPC_EPOCH, 2639.13f, 698.55f, 65.43f, 4.59f, TEMPSUMMON_DEAD_DESPAWN, 120000))
                           DoScriptText(SAY_EPOCH_ENTER1, Epoch);
                        me->SetOrientation(2.63f);
                        DoScriptText(SAY_TH_EPOCH_WONDER, me);
                        break;
                    case 1:
                        me->SetOrientation(5.79f);
                        DoScriptText(SAY_TH_EPOCH_KILL_TARETHA, me);
                        if (Creature* Epoch = me->GetMap()->GetCreature(EpochGUID))
                            DoScriptText(SAY_EPOCH_ENTER2, Epoch);
                        if (uint64 TarethaGUID = pInstance->GetData64(DATA_TARETHA))
                        {
                            Creature* Taretha = (Unit::GetCreature(*me, TarethaGUID));
                            Taretha->CastSpell(Taretha, SPELL_SHADOW_SPIKE, true);
                            Taretha->SetStandState(UNIT_STAND_STATE_DEAD);
                        } else if (Creature* Taretha = me->FindNearestCreature(18887, 300.0f, true))
                        {
                            Taretha->CastSpell(Taretha, SPELL_SHADOW_SPIKE, true);
                            Taretha->SetStandState(UNIT_STAND_STATE_DEAD);
                        }
                        break;
                    case 9:
                        if (Creature* Epoch = me->GetMap()->GetCreature(EpochGUID))
                        {
                            DoScriptText(SAY_EPOCH_ENTER3, Epoch);
                            me->SetFacingToObject(Epoch);
                        }
                        SetEscortPaused(true);
                        break;
                    case 11:
                        if (uint64 TarethaGUID = pInstance->GetData64(DATA_TARETHA))
                        {
                            Creature* Taretha = (Unit::GetCreature(*me, TarethaGUID));
                            if (Player* pPlayer = GetPlayerForEscort())
                                CAST_AI(npc_escortAI, (Taretha->AI()))->Start(false, true, pPlayer->GetGUID());
                        } else if (Creature* Taretha = me->FindNearestCreature(18887, 300.0f, true))
                        {
                            if (Player* pPlayer = GetPlayerForEscort())
                                CAST_AI(npc_escortAI, (Taretha->AI()))->Start(false, true, pPlayer->GetGUID());
                        }
                        Event = true;
                        me->SetOrientation(5.79f);
                        SetEscortPaused(true);
                        break;
                    case 13:
                        Event = false;
                        pInstance->SetData(TYPE_THRALL_EVENT, DONE);
                        break;
                }
                break;
        }
    }

    void DoMount()
    {
        me->Mount(SKARLOC_MOUNT_MODEL);
        me->SetSpeed(MOVE_RUN,SPEED_MOUNT);
    }

    void DoUnmount()
    {
        me->Dismount();
        me->SetSpeed(MOVE_RUN,SPEED_RUN);
    }

    void DoAction(const int32 action)
    {
        SetEscortPaused(false);
    }

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (spell->Id ==  SPELL_MEMORY_WP_RESUME)
        me->RemoveAllAuras();
    }

    void EnterCombat(Unit* who)
    {
        DoScriptText(RAND(SAY_TH_RANDOM_AGGRO1, SAY_TH_RANDOM_AGGRO2, SAY_TH_RANDOM_AGGRO3, SAY_TH_RANDOM_AGGRO4), me);

        if (me->IsMounted())
        {
            DoUnmount();
            HadMount = true;
        }
    }

    void QuestCredit()
    {
        Map* pMap = me->GetMap();
        Map::PlayerList const& players = pMap->GetPlayers();
        if (!players.isEmpty() && pMap->IsDungeon())
        {
            for(Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                if (Player* player = itr->GetSource())
                    player->KilledMonsterCredit(NPC_THRALL_QUEST_TRIGGER);
            }
        }
    }

    void JustSummoned(Creature* summoned)
    {
        switch (summoned->GetEntry())
        {
            case NPC_ARMORER:
                ArmorerGUID = summoned->GetGUID();
                summoned->SetReactState(REACT_PASSIVE);
                summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                break;
            case NPC_WARDEN:
                DoScriptText(RAND(SAY_AMBUSH_P1, SAY_AMBUSH2_P1), summoned);
                summoned->AI()->AttackStart(me);
                break;
            case NPC_SKARLOC_MOUNT:
                MountGUID = summoned->GetGUID();
                break;
            case NPC_EROZION:
                ImageGUID = summoned->GetGUID();
                summoned->CastSpell(summoned, SPELL_TELEPORT, false);
                break;
            case NPC_CHURCH_PROTECTOR:
                DoScriptText(SAY_LOOKOUT_CHURCH, summoned);
                summoned->AI()->AttackStart(me);
                break;
            case NPC_INN_PROTECTOR:
                DoScriptText(SAY_LOOKOUT_INN, summoned);
                summoned->AI()->AttackStart(me);
                break;
            case NPC_TARREN_MILL_PROTECTOR:
                DoScriptText(SAY_PROTECTOR_BARN_2, summoned);
                break;
            case NPC_TARREN_MILL_LOOKOUT:
                DoScriptText(SAY_LOOKOUT_BARN_1, summoned);
                break;
            case NPC_EPOCH:
                EpochGUID = summoned->GetGUID();
                summoned->SetLevitate(true);
                break;
            case NPC_TARREN_MILL_GUARDSMAN:
            case NPC_SKARLOC:
                break;
            default:
                summoned->AI()->AttackStart(me);
                break;
        }
    }

    void KilledUnit(Unit *victim)
    {
        DoScriptText(RAND(SAY_TH_RANDOM_KILL1, SAY_TH_RANDOM_KILL2, SAY_TH_RANDOM_KILL3), me);
    }

    void JustDied(Unit *slayer)
    {
        if (pInstance)
        {
            pInstance->SetData(TYPE_THRALL_EVENT,FAIL);
            slayer->SummonCreature(NPC_IMAGE_OF_ERONZION, me->GetPositionX()+2, me->GetPositionY()+2, me->GetPositionZ(),me->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 10000);
        }

        // Don't do a yell if he kills self (if player goes too far or at the end).
        if (slayer == me)
            return;

        DoScriptText(RAND(SAY_TH_RANDOM_DIE1, SAY_TH_RANDOM_DIE2), me);
    }

    int32 NextStep(uint32 Steps)
    {       
        if (uint64 TarethaGUID = pInstance->GetData64(DATA_TARETHA))
        {
            Creature* Taretha = (Unit::GetCreature(*me, TarethaGUID));
            Creature* Image = me->GetMap()->GetCreature(ImageGUID);

            switch (Steps)
            {
                case 1:
                    return 15000;
                case 2:
                    DoScriptText(SAY_TR_GLAD_SAFE, me);
                    return 10000;
                case 3:
                    DoScriptText(SAY_TA_NEVER_MET, Taretha);
                    return 10000;
                case 4:
                    DoScriptText(SAY_TR_THEN_WHO, me);
                    return 5000;
                case 5:
                    me->SummonCreature(NPC_EROZION, 2648.47f, 684.43f, 55.713f, 3.86f, TEMPSUMMON_TIMED_DESPAWN, 300000);
                    return 5000;
                case 6:
                    Image->CastSpell(Image, SPELL_MEMORY_WIPE, false);
                    return 5000;
                case 7:
                    DoScriptText(SAY_WIPE_MEMORY, Image);
                    return 10000;
                case 8:
                    DoScriptText(SAY_ABOUT_TARETHA, Image);
                    return 5000;
                case 9:
                    Image->CastSpell(Image, SPELL_MEMORY_WP_RESUME, false);
                    DoScriptText(SAY_TH_EVENT_COMPLETE, me);
                    return 6000;
                case 10:
                    Taretha->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                    DoScriptText(SAY_TA_FAREWELL, Taretha);
                    SetEscortPaused(false);
                    QuestCredit();
                    return 3000;
                case 11:
                    Taretha->SetWalk(true);
                    Taretha->GetMotionMaster()->MovePoint(0, 2639.96f, 703.66f, 56.056f);
                    Taretha->ForcedDespawn(11000);
                default:
                    return 0;
            }
        } else if (Creature* Taretha = me->FindNearestCreature(18887, 300.0f, true))
            {
                Creature* Image = me->GetMap()->GetCreature(ImageGUID);         

                switch (Steps)
                {
                    case 1:
                        return 15000;
                    case 2:
                        DoScriptText(SAY_TR_GLAD_SAFE, me);
                        return 10000;
                    case 3:
                        DoScriptText(SAY_TA_NEVER_MET, Taretha);
                        return 10000;
                    case 4:
                        DoScriptText(SAY_TR_THEN_WHO, me);
                        return 5000;
                    case 5:
                        me->SummonCreature(NPC_EROZION, 2648.47f, 684.43f, 55.713f, 3.86f, TEMPSUMMON_TIMED_DESPAWN, 300000);
                        return 5000;
                    case 6:
                        Image->CastSpell(Image, SPELL_MEMORY_WIPE, false);
                        return 5000;
                    case 7:
                        DoScriptText(SAY_WIPE_MEMORY, Image);
                        return 10000;
                    case 8:
                        DoScriptText(SAY_ABOUT_TARETHA, Image);
                        return 5000;
                    case 9:
                        Image->CastSpell(Image, SPELL_MEMORY_WP_RESUME, false);
                        DoScriptText(SAY_TH_EVENT_COMPLETE, me);
                        return 6000;
                    case 10:
                        Taretha->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                        DoScriptText(SAY_TA_FAREWELL, Taretha);
                        SetEscortPaused(false);
                        QuestCredit();
                        return 3000;
                    case 11:
                        Taretha->SetWalk(true);
                        Taretha->GetMotionMaster()->MovePoint(0, 2639.96f, 703.66f, 56.056f);
                        Taretha->ForcedDespawn(11000);
                    default:
                        return 0;
                }
            }
        return true;
    }

    void StartEscort (Player* player, uint8 Parts)
    {
        Part = Parts;

        switch (Parts)
        {
            case 1:
                AddWaypoint(0, 2230.91f, 118.765f, 82.2947f, 2000),
                AddWaypoint(1, 2230.33f, 114.980f, 82.2946f, 0);
                AddWaypoint(2, 2233.36f, 111.057f, 82.2996f, 0);
                AddWaypoint(3, 2231.17f, 108.486f, 82.6624f, 0);
                AddWaypoint(4, 2220.22f, 114.605f, 89.4264f, 0);
                AddWaypoint(5, 2215.23f, 115.990f, 89.4549f, 0);
                AddWaypoint(6, 2207.97f, 105.351f, 89.4549f, 0);
                AddWaypoint(7, 2202.53f, 106.922f, 89.4549f, 0);
                AddWaypoint(8, 2192.26f, 112.618f, 89.4549f, 2000);
                AddWaypoint(9, 2182.96f, 117.850f, 89.4548f, 2000);
                AddWaypoint(10, 2182.11f, 120.328f, 89.4548f, 5000);
                AddWaypoint(11, 2182.11f, 120.329f, 89.4548f, 5000);
                AddWaypoint(12, 2182.11f, 120.330f, 89.4548f, 3000);
                AddWaypoint(13, 2189.44f, 113.922f, 89.4549f, 0); 
                AddWaypoint(14, 2195.63f, 110.584f, 89.4549f, 0);
                AddWaypoint(15, 2201.09f, 115.115f, 89.4549f, 0);
                AddWaypoint(16, 2204.34f, 121.036f, 89.4355f, 0);
                AddWaypoint(17, 2208.66f, 129.127f, 87.9560f, 0);
                AddWaypoint(18, 2193.09f, 137.940f, 88.2164f, 0);
                AddWaypoint(19, 2173.39f, 149.064f, 87.9227f, 0);
                AddWaypoint(20, 2164.25f, 137.965f, 85.0595f, 0);
                AddWaypoint(21, 2149.31f, 125.645f, 77.0858f, 0);
                AddWaypoint(22, 2142.78f, 127.173f, 75.5954f, 0);
                AddWaypoint(23, 2139.28f, 133.952f, 73.6386f, 0);
                AddWaypoint(24, 2139.54f, 155.235f, 67.1269f, 0);
                AddWaypoint(25, 2145.38f, 167.551f, 64.8974f, 0);
                AddWaypoint(26, 2134.28f, 175.304f, 67.9446f, 0);
                AddWaypoint(27, 2118.08f, 187.387f, 68.8141f, 0);
                AddWaypoint(28, 2105.88f, 195.461f, 65.1854f, 0);
                AddWaypoint(29, 2096.77f, 196.939f, 65.2117f, 0);
                AddWaypoint(30, 2083.90f, 209.395f, 64.8736f, 0);
                AddWaypoint(31, 2063.40f, 229.509f, 64.4883f, 25000);
                AddWaypoint(32, 2063.40f, 229.510f, 64.4883f, 0);
                AddWaypoint(33, 2063.40f, 229.512f, 64.4883f, 0);
                ((npc_escortAI*)(me->AI()))->SetClearWaypoints(true);
                ((npc_escortAI*)(me->AI()))->SetDespawnAtEnd(false);
                ((npc_escortAI*)(me->AI()))->SetDespawnAtFar(false);
                Start(true, true, player->GetGUID());
                break;
            case 2:
                AddWaypoint(0, 2046.70f, 251.941f, 62.7851f, 4000);
                AddWaypoint(1, 2046.70f, 251.942f, 62.7851f, 3000);
                AddWaypoint(2, 2011.77f, 278.478f, 65.3388f, 0);
                AddWaypoint(3, 2005.08f, 289.676f, 66.1179f, 0);
                AddWaypoint(4, 2033.11f, 337.450f, 66.0948f, 0);
                AddWaypoint(5, 2070.30f, 416.208f, 66.0893f, 0);
                AddWaypoint(6, 2086.76f, 469.768f, 65.9182f, 0);
                AddWaypoint(7, 2101.70f, 497.955f, 61.7881f, 0);
                AddWaypoint(8, 2133.39f, 530.933f, 55.3700f, 0);
                AddWaypoint(9, 2157.91f, 559.635f, 48.5157f, 0);
                AddWaypoint(10, 2167.34f, 586.191f, 42.4394f, 0);
                AddWaypoint(11, 2174.17f, 637.643f, 33.9002f, 0);
                AddWaypoint(12, 2179.31f, 656.053f, 34.723f, 0);
                AddWaypoint(13, 2183.65f, 670.941f, 34.0318f, 0);
                AddWaypoint(14, 2201.50f, 668.616f, 36.1236f, 0);
                AddWaypoint(15, 2221.56f, 652.747f, 36.6153f, 0);
                AddWaypoint(16, 2238.97f, 640.125f, 37.2214f, 0);
                AddWaypoint(17, 2251.17f, 620.574f, 40.1473f, 0);
                AddWaypoint(18, 2261.98f, 595.303f, 41.4117f, 0);
                AddWaypoint(19, 2278.67f, 560.172f, 38.9090f, 0);
                AddWaypoint(20, 2336.72f, 528.327f, 40.9369f, 0);
                AddWaypoint(21, 2381.04f, 519.612f, 37.7312f, 0);
                AddWaypoint(22, 2412.20f, 515.425f, 39.2068f, 0);
                AddWaypoint(23, 2452.39f, 516.174f, 42.9387f, 0);
                AddWaypoint(24, 2467.38f, 539.389f, 47.4992f, 0);
                AddWaypoint(25, 2470.70f, 554.333f, 46.6668f, 0);
                AddWaypoint(26, 2478.07f, 575.321f, 55.4549f, 0);
                AddWaypoint(27, 2480.00f, 585.408f, 56.6921f, 0);
                AddWaypoint(28, 2482.67f, 608.817f, 55.6643f, 0);
                AddWaypoint(29, 2485.62f, 626.061f, 58.0132f, 2000);
                AddWaypoint(30, 2486.91f, 626.356f, 58.0761f, 2000);
                AddWaypoint(31, 2486.91f, 626.357f, 58.0761f, 0);
                ((npc_escortAI*)(me->AI()))->SetClearWaypoints(true);
                ((npc_escortAI*)(me->AI()))->SetDespawnAtEnd(false);
                ((npc_escortAI*)(me->AI()))->SetDespawnAtFar(false);
                Start(true, false, player->GetGUID());
                break;
            case 3:
                AddWaypoint(0, 2488.58f, 660.940f, 57.3913f, 0);
                AddWaypoint(1, 2502.56f, 686.059f, 55.6252f, 0);
                AddWaypoint(2, 2502.08f, 694.360f, 55.5083f, 0);
                AddWaypoint(3, 2491.46f, 694.321f, 55.7163f, 0);
                AddWaypoint(4, 2491.10f, 703.300f, 55.7630f, 0);
                AddWaypoint(5, 2485.64f, 702.992f, 55.7917f, 0);
                AddWaypoint(6, 2479.63f, 696.521f, 55.7901f, 0);
                AddWaypoint(7, 2476.24f, 696.204f, 55.8093f, 0);
                AddWaypoint(8, 2475.39f, 695.983f, 55.8146f, 0);
                AddWaypoint(9, 2477.75f, 694.473f, 55.7945f, 0);
                AddWaypoint(10, 2481.27f, 697.747f, 55.7910f, 0);
                AddWaypoint(11, 2486.31f, 703.131f, 55.7861f, 0);
                AddWaypoint(12, 2490.76f, 703.511f, 55.7662f, 0);
                AddWaypoint(13, 2491.30f, 694.792f, 55.7195f, 0);
                AddWaypoint(14, 2502.08f, 694.360f, 55.5083f, 0);
                AddWaypoint(15, 2507.99f, 679.298f, 56.3760f, 0);
                AddWaypoint(16, 2524.79f, 669.919f, 54.9258f, 0);
                AddWaypoint(17, 2543.19f, 665.289f, 56.2957f, 0);
                AddWaypoint(18, 2566.49f, 664.354f, 54.5034f, 0);
                AddWaypoint(19, 2592.00f, 664.611f, 56.4394f, 0);
                AddWaypoint(20, 2616.14f, 665.499f, 55.1610f, 0);
                AddWaypoint(21, 2623.56f, 666.965f, 54.3983f, 0);
                AddWaypoint(22, 2629.99f, 661.059f, 54.2738f, 0);
                AddWaypoint(23, 2629.00f, 656.982f, 56.0651f, 0);
                AddWaypoint(24, 2620.84f, 633.007f, 56.0300f, 3000);
                AddWaypoint(25, 2621.62f, 635.352f, 56.0300f, 0);
                AddWaypoint(26, 2622.99f, 639.178f, 56.0300f, 0);
                AddWaypoint(27, 2628.73f, 656.693f, 56.0610f, 0);
                AddWaypoint(28, 2630.34f, 661.135f, 54.2738f, 0);
                AddWaypoint(29, 2635.38f, 672.243f, 54.4508f, 0);
                AddWaypoint(30, 2644.13f, 668.158f, 55.3797f, 0);
                AddWaypoint(31, 2646.82f, 666.740f, 56.9898f, 0);
                AddWaypoint(32, 2658.22f, 665.432f, 57.1725f, 0);
                AddWaypoint(33, 2661.88f, 674.849f, 57.1725f, 0);
                AddWaypoint(34, 2656.23f, 677.208f, 57.1725f, 0);
                AddWaypoint(35, 2652.28f, 670.270f, 61.9353f, 0);
                AddWaypoint(36, 2650.79f, 664.290f, 61.9302f, 0);
                AddWaypoint(37, 2660.48f, 659.409f, 61.9370f, 5000);
                AddWaypoint(38, 2660.48f, 659.410f, 61.9370f, 0);
                ((npc_escortAI*)(me->AI()))->SetClearWaypoints(true);
                ((npc_escortAI*)(me->AI()))->SetDespawnAtEnd(false);
                ((npc_escortAI*)(me->AI()))->SetDespawnAtFar(false);
                Start(true, true, player->GetGUID());
                break;
            case 4:
                AddWaypoint(0, 2660.48f, 659.411f, 61.9370f, 5000);
                AddWaypoint(1, 2660.48f, 659.412f, 61.9370f, 2000);
                AddWaypoint(2, 2652.32f, 664.994f, 61.9305f, 0);
                AddWaypoint(3, 2652.37f, 670.561f, 61.9368f, 0);
                AddWaypoint(4, 2656.05f, 676.761f, 57.1727f, 0);
                AddWaypoint(5, 2658.49f, 677.166f, 57.1727f, 0);
                AddWaypoint(6, 2659.28f, 667.117f, 57.1727f, 0);
                AddWaypoint(7, 2649.71f, 665.387f, 57.1727f, 0);
                AddWaypoint(8, 2634.79f, 672.964f, 54.4577f, 0);
                AddWaypoint(9, 2635.06f, 673.892f, 54.4713f, 6000);
                AddWaypoint(10, 2635.06f, 673.893f, 54.4713f, 6000);
                AddWaypoint(11, 2635.06f, 673.890f, 54.4713f, 0);
                AddWaypoint(12, 2634.30f, 661.698f, 54.4147f, 0);
                AddWaypoint(13, 2652.21f, 644.396f, 56.1906f, 0);
                ((npc_escortAI*)(me->AI()))->SetClearWaypoints(true);
                ((npc_escortAI*)(me->AI()))->SetDespawnAtEnd(true);
                ((npc_escortAI*)(me->AI()))->SetDespawnAtFar(false);
                Start(true, true, player->GetGUID());
                break;
        }
        return;
    }

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);

        if (StepsTimer <= diff)
        {
            if (Event)
                StepsTimer = NextStep(++Steps);
        }
        else StepsTimer -= diff;

        if (HadMount && !me->IsInCombat())
            DoMount();

        if (EmoteTimer)
        {
            if (EmoteTimer <= diff)
            {
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                EmoteTimer = 0;
            }
            else EmoteTimer -= diff;
        }

        if (!UpdateVictim())
            return;

        if (StrikeTimer < diff)
        {
            DoCast(me->GetVictim(), SPELL_STRIKE);
            StrikeTimer = urand(4000, 7000);
        }
        else StrikeTimer -= diff;

        if (ShieldBlockTimer < diff)
        {
            DoCast(me, SPELL_SHIELD_BLOCK);
            ShieldBlockTimer = urand(8000, 15000);
        }
        else ShieldBlockTimer -= diff;

        if (!LowHp && ((me->GetHealth()*100 / me->GetMaxHealth()) < 20))
        {
            DoScriptText(RAND(SAY_TH_RANDOM_LOW_HP1, SAY_TH_RANDOM_LOW_HP2), me);
            LowHp = true;
        }
    }
};

CreatureAI* GetAI_npc_thrall_old_hillsbrad(Creature* creature)
{
    return new npc_thrall_old_hillsbradAI(creature);
}

bool GossipHello_npc_thrall_old_hillsbrad(Player *player, Creature *creature)
{
    if (creature->isQuestGiver())
    {
        player->PrepareQuestMenu( creature->GetGUID() );
        player->SendPreparedQuest(creature->GetGUID());
    }

    ScriptedInstance* pInstance = (ScriptedInstance*)creature->GetInstanceData();
    if (pInstance)
    {
        if (pInstance->GetData(DATA_DRAKE_DEATH) == DONE && pInstance->GetData(TYPE_THRALL_PART1) == NOT_STARTED)
        {
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(GOSSIP_ID_START, creature->GetGUID());
        }

        if (pInstance->GetData(TYPE_THRALL_PART1) == DONE && pInstance->GetData(TYPE_THRALL_PART2) == NOT_STARTED)
        {
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_SKARLOC1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(GOSSIP_ID_SKARLOC1, creature->GetGUID());
        }

        if (pInstance->GetData(TYPE_THRALL_PART2) == DONE && pInstance->GetData(TYPE_THRALL_PART3) == NOT_STARTED)
        {
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_TARREN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            player->SEND_GOSSIP_MENU(GOSSIP_ID_TARREN, creature->GetGUID());
        }

        if (pInstance->GetData(TYPE_THRALL_PART3) == DONE && pInstance->GetData(TYPE_THRALL_PART4) == NOT_STARTED)
        {
            player->SEND_GOSSIP_MENU(GOOSIP_ID_TARETHA, creature->GetGUID());
        }
    }

    return true;
}

bool GossipSelect_npc_thrall_old_hillsbrad(Player *player, Creature *creature, uint32 sender, uint32 action)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)creature->GetInstanceData();
    uint8 Parts = 0;

    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            player->CLOSE_GOSSIP_MENU();
            if (pInstance)
            {
                pInstance->SetData(TYPE_THRALL_EVENT, IN_PROGRESS);
                pInstance->SetData(TYPE_THRALL_PART1, IN_PROGRESS);
            }

            DoScriptText(SAY_TH_START_EVENT_PART1, creature);
            Parts = 1;
            CAST_AI(npc_thrall_old_hillsbradAI, creature->AI())->StartEscort(player, Parts);
            break;

        case GOSSIP_ACTION_INFO_DEF+2:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_SKARLOC2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+20);
            player->SEND_GOSSIP_MENU(GOSSIP_ID_SKARLOC2, creature->GetGUID());
            break;

        case GOSSIP_ACTION_INFO_DEF+20:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_SKARLOC3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+22);
            player->SEND_GOSSIP_MENU(GOSSIP_ID_SKARLOC3, creature->GetGUID());
            break;

        case GOSSIP_ACTION_INFO_DEF+22:
            player->CLOSE_GOSSIP_MENU();
            if (pInstance)
            {
                pInstance->SetData(TYPE_THRALL_EVENT, IN_PROGRESS);
                pInstance->SetData(TYPE_THRALL_PART2,IN_PROGRESS);
            }

            DoScriptText(SAY_TH_START_EVENT_PART2, creature);
            Parts = 2;
            CAST_AI(npc_thrall_old_hillsbradAI, creature->AI())->StartEscort(player, Parts);
            break;

        case GOSSIP_ACTION_INFO_DEF+3:
            player->CLOSE_GOSSIP_MENU();
            if (pInstance)
            {
                pInstance->SetData(TYPE_THRALL_EVENT, IN_PROGRESS);
                pInstance->SetData(TYPE_THRALL_PART3,IN_PROGRESS);
            }
            Parts = 3;
            CAST_AI(npc_thrall_old_hillsbradAI, creature->AI())->StartEscort(player, Parts);
            break;
    }
    return true;
}

/*######
## npc_taretha
######*/

#define GOSSIP_ID_EPOCH1        9610                        //Thank you for helping Thrall escape, friends. Now I only hope
#define GOSSIP_ITEM_EPOCH1      "Strange wizard?"
#define GOSSIP_ID_EPOCH2        9613                        //Yes, friends. This man was no wizard of
#define GOSSIP_ITEM_EPOCH2      "We'll get you out. Taretha. Don't worry. I doubt the wizard would wander too far away."

struct npc_tarethaAI : public npc_escortAI
{
    npc_tarethaAI(Creature *creature) : npc_escortAI(creature)
    {
        pInstance = (ScriptedInstance*)creature->GetInstanceData();
    }

    ScriptedInstance *pInstance;

    void WaypointReached(uint32 i)
    {
        switch (i)
        {
            case 0:
                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->RemoveAllAuras();
                break;
            case 6:
                DoScriptText(SAY_TA_FREE, me);
                break;
            case 7:
                me->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                break;
        }
    }

    void Reset() 
    {
        DoCast(me, SPELL_SHADOW_PRISON, true);
    }

    void EnterCombat(Unit* who) {}

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (spell->Id ==  SPELL_MEMORY_WP_RESUME)
            me->RemoveAllAuras();
    }

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);
    }
};

CreatureAI* GetAI_npc_taretha(Creature* creature)
{
    return new npc_tarethaAI(creature);
}

bool GossipHello_npc_taretha(Player *player, Creature *creature)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)creature->GetInstanceData();
    if (pInstance && pInstance->GetData(TYPE_THRALL_PART3) == DONE && pInstance->GetData(TYPE_THRALL_PART4) == NOT_STARTED)
    {
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_EPOCH1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        player->SEND_GOSSIP_MENU(GOSSIP_ID_EPOCH1, creature->GetGUID());
    }

    return true;
}

bool GossipSelect_npc_taretha(Player *player, Creature *creature, uint32 sender, uint32 action)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)creature->GetInstanceData();
    uint8 Parts = 4;

    if (action == GOSSIP_ACTION_INFO_DEF+1)
    {
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_EPOCH2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        player->SEND_GOSSIP_MENU(GOSSIP_ID_EPOCH2, creature->GetGUID());
    }

    if (action == GOSSIP_ACTION_INFO_DEF+2)
    {
        player->CLOSE_GOSSIP_MENU();

        if (pInstance)
        {
            pInstance->SetData(TYPE_THRALL_EVENT, IN_PROGRESS);
            pInstance->SetData(TYPE_THRALL_PART4,IN_PROGRESS);
        }

         if (uint64 ThrallGUID = pInstance->GetData64(DATA_THRALL))
         {
             Creature* Thrall = (Unit::GetCreature((*creature), ThrallGUID));
             if (Thrall)
             {
                 CAST_AI(npc_thrall_old_hillsbradAI, Thrall->AI())->StartEscort(player, Parts);
             }
         } else {
             Creature* Thrall = creature->FindNearestCreature(17876, 10.0f, true);
             if (Thrall)
                CAST_AI(npc_thrall_old_hillsbradAI, Thrall->AI())->StartEscort(player, Parts);
         }
    }

    return true;
}

struct erozion_imageAI : public ScriptedAI
{
    erozion_imageAI(Creature *creature) : ScriptedAI(creature)
    {
        pInstance = (ScriptedInstance*)creature->GetInstanceData();
    }

    ScriptedInstance *pInstance;

    bool Intro;

    uint8 Next;
    uint32 StepsTimer;
    uint32 Steps;

    void Reset()
    {
        me->CastSpell(me, SPELL_TELEPORT, false);
        Intro = true;
        Steps = 0;
        StepsTimer = 0;
    }

    int32 NextStep(uint32 Steps)
    {       
       if (Creature *Thrall = (Creature*)(Unit::GetUnit((*me), pInstance->GetData64(DATA_THRALL))))
       {
           switch (Steps)
           {
               case 1:
                   me->SetFacingToObject(Thrall);
                   return 2000;
               case 2:
                   DoScriptText(SAY_IMAGE_1, me);
                   return 15000;
               case 3:
                   if (pInstance)
                   {
                       if (pInstance->GetData(TYPE_THRALL_PART1) == NOT_STARTED)
                           me->SummonCreature(NPC_THRALL, 2231.51f, 119.84f, 82.297f, 4.15f,TEMPSUMMON_DEAD_DESPAWN,15000);

                       if (pInstance->GetData(TYPE_THRALL_PART1) == DONE && pInstance->GetData(TYPE_THRALL_PART2) == NOT_STARTED)
                       {
                           me->SummonCreature(NPC_THRALL, 2063.40f, 229.512f, 64.488f, 2.18f,TEMPSUMMON_DEAD_DESPAWN,15000);
                           me->SummonCreature(NPC_SKARLOC_MOUNT,2047.90f, 254.85f, 62.822f, 5.94f, TEMPSUMMON_DEAD_DESPAWN, 15000);
                       }

                       if (pInstance->GetData(TYPE_THRALL_PART2) == DONE && pInstance->GetData(TYPE_THRALL_PART3) == NOT_STARTED)
                           me->SummonCreature(NPC_THRALL, 2486.91f, 626.357f, 58.076f, 4.66f,TEMPSUMMON_DEAD_DESPAWN,15000);

                       if (pInstance->GetData(TYPE_THRALL_PART3) == DONE && pInstance->GetData(TYPE_THRALL_PART4) == NOT_STARTED)
                       {
                           me->SummonCreature(NPC_THRALL, 2660.48f, 659.409f, 61.937f, 5.83f,TEMPSUMMON_DEAD_DESPAWN,15000);
                           if (uint64 TarethaGUID = pInstance->GetData64(DATA_TARETHA))
                           {
                               if (Unit *Taretha = me->GetMap()->GetCreature(TarethaGUID))
                               Taretha->SetStandState(UNIT_STAND_STATE_STAND);
                           }
                        }
                    }
                    return 5000;
                case 4:
                    DoScriptText(SAY_IMAGE_2, me);
                    return 15000;
                 case 5:
                     me->CastSpell(me, SPELL_TELEPORT, false);
                     me->ForcedDespawn(1500);
                 default:
                     return 0;
            }
       } else if (Creature *Thrall = (Creature*)(me->FindNearestCreature(17876, 300.0f, false)))
       {
          switch (Steps)
           {
               case 1:
                   me->SetFacingToObject(Thrall);
                   return 2000;
               case 2:
                   DoScriptText(SAY_IMAGE_1, me);
                   return 15000;
               case 3:
                   if (pInstance)
                   {
                       if (pInstance->GetData(TYPE_THRALL_PART1) == NOT_STARTED)
                           me->SummonCreature(NPC_THRALL, 2231.51f, 119.84f, 82.297f, 4.15f,TEMPSUMMON_DEAD_DESPAWN,15000);

                       if (pInstance->GetData(TYPE_THRALL_PART1) == DONE && pInstance->GetData(TYPE_THRALL_PART2) == NOT_STARTED)
                       {
                           me->SummonCreature(NPC_THRALL, 2063.40f, 229.512f, 64.488f, 2.18f,TEMPSUMMON_DEAD_DESPAWN,15000);
                           me->SummonCreature(NPC_SKARLOC_MOUNT,2047.90f, 254.85f, 62.822f, 5.94f, TEMPSUMMON_DEAD_DESPAWN, 15000);
                       }

                       if (pInstance->GetData(TYPE_THRALL_PART2) == DONE && pInstance->GetData(TYPE_THRALL_PART3) == NOT_STARTED)
                           me->SummonCreature(NPC_THRALL, 2486.91f, 626.357f, 58.076f, 4.66f,TEMPSUMMON_DEAD_DESPAWN,15000);

                       if (pInstance->GetData(TYPE_THRALL_PART3) == DONE && pInstance->GetData(TYPE_THRALL_PART4) == NOT_STARTED)
                       {
                           me->SummonCreature(NPC_THRALL, 2660.48f, 659.409f, 61.937f, 5.83f,TEMPSUMMON_DEAD_DESPAWN,15000);
                           if (uint64 TarethaGUID = pInstance->GetData64(DATA_TARETHA))
                           {
                               if (Unit *Taretha = me->GetMap()->GetCreature(TarethaGUID))
                               Taretha->SetStandState(UNIT_STAND_STATE_STAND);
                           }
                        }
                    }
                    return 5000;
                case 4:
                    DoScriptText(SAY_IMAGE_2, me);
                    return 15000;
                 case 5:
                     me->CastSpell(me, SPELL_TELEPORT, false);
                     me->ForcedDespawn(1500);
                 default:
                     return 0;
            }
       }
        return true;
    }

    void UpdateAI(const uint32 diff)
    {
        if (StepsTimer <= diff)
        {
            if (Intro)
                StepsTimer = NextStep(++Steps);
        }
        else StepsTimer -= diff;
    }
};

CreatureAI* GetAI_erozion_image(Creature *creature)
{
    return new erozion_imageAI (creature);
}

/*######
## AddSC
######*/

void AddSC_old_hillsbrad()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="npc_erozion";
    newscript->pGossipHello =   &GossipHello_npc_erozion;
    newscript->pGossipSelect =  &GossipSelect_npc_erozion;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_thrall_old_hillsbrad";
    newscript->pGossipHello =  &GossipHello_npc_thrall_old_hillsbrad;
    newscript->pGossipSelect = &GossipSelect_npc_thrall_old_hillsbrad;
    newscript->GetAI = &GetAI_npc_thrall_old_hillsbrad;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_taretha";
    newscript->pGossipHello =   &GossipHello_npc_taretha;
    newscript->pGossipSelect =  &GossipSelect_npc_taretha;
    newscript->GetAI = &GetAI_npc_taretha;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="erozion_image";
    newscript->GetAI = &GetAI_erozion_image;
    newscript->RegisterSelf();
}
