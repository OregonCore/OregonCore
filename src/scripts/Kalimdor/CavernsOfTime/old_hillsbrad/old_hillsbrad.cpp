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
npc_brazen
npc_erozion
npc_thrall_old_hillsbrad
npc_taretha
erozion_image
EndContentData */

#include "ScriptPCH.h"
#include "old_hillsbrad.h"
#include "ScriptedEscortAI.h"

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
## npc_brazen
######*/

bool GossipHello_npc_brazen(Player *player, Creature *creature)
{
    player->ADD_GOSSIP_ITEM(0, "I am ready to go to Durnholde Keep.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    player->SEND_GOSSIP_MENU(9779, creature->GetGUID());
    return true;
}

bool GossipSelect_npc_brazen(Player *player, Creature *creature, uint32 sender, uint32 action )
{
    if (action == GOSSIP_ACTION_INFO_DEF+1)
    {
        if (!player->HasItemCount(ITEM_ENTRY_BOMBS, 1))
            player->SEND_GOSSIP_MENU(9780, creature->GetGUID());
        else
        {
            player->CLOSE_GOSSIP_MENU();

            std::vector<uint32> nodes;

            nodes.resize(2);
            nodes[0] = 115;                                 //from brazen
            nodes[1] = 116;                                 //end outside durnholde
            player->ActivateTaxiPathTo(nodes);              //TaxiPath 534
        }
    }
    return true;
}

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
                        me->SummonCreature(NPC_SKARLOC_MOUNT, 2488.64f, 625.77f, 58.26, 4.71f,TEMPSUMMON_TIMED_DESPAWN,10000);
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
                       if (Creature* Epoch = me->SummonCreature(NPC_EPOCH,2639.13,698.55,65.43,4.59,TEMPSUMMON_DEAD_DESPAWN,120000))
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
                if (Player* player = itr->getSource())
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
                AddWaypoint(0, 2230.91, 118.765, 82.2947, 2000),
                AddWaypoint(1, 2230.33, 114.980, 82.2946, 0);
                AddWaypoint(2, 2233.36, 111.057, 82.2996, 0);
                AddWaypoint(3, 2231.17, 108.486, 82.6624, 0);
                AddWaypoint(4, 2220.22, 114.605, 89.4264, 0);
                AddWaypoint(5, 2215.23, 115.990, 89.4549, 0);
                AddWaypoint(6, 2207.97, 105.351, 89.4549, 0);
                AddWaypoint(7, 2202.53, 106.922, 89.4549, 0);
                AddWaypoint(8, 2192.26, 112.618, 89.4549, 2000);
                AddWaypoint(9, 2182.96, 117.850, 89.4548, 2000);
                AddWaypoint(10, 2182.11, 120.328, 89.4548, 5000);
                AddWaypoint(11, 2182.11, 120.329, 89.4548, 5000);
                AddWaypoint(12, 2182.11, 120.330, 89.4548, 3000);
                AddWaypoint(13, 2189.44, 113.922, 89.4549, 0); 
                AddWaypoint(14, 2195.63, 110.584, 89.4549, 0);
                AddWaypoint(15, 2201.09, 115.115, 89.4549, 0);
                AddWaypoint(16, 2204.34, 121.036, 89.4355, 0);
                AddWaypoint(17, 2208.66, 129.127, 87.9560, 0);
                AddWaypoint(18, 2193.09, 137.940, 88.2164, 0);
                AddWaypoint(19, 2173.39, 149.064, 87.9227, 0);
                AddWaypoint(20, 2164.25, 137.965, 85.0595, 0);
                AddWaypoint(21, 2149.31, 125.645, 77.0858, 0);
                AddWaypoint(22, 2142.78, 127.173, 75.5954, 0);
                AddWaypoint(23, 2139.28, 133.952, 73.6386, 0);
                AddWaypoint(24, 2139.54, 155.235, 67.1269, 0);
                AddWaypoint(25, 2145.38, 167.551, 64.8974, 0);
                AddWaypoint(26, 2134.28, 175.304, 67.9446, 0);
                AddWaypoint(27, 2118.08, 187.387, 68.8141, 0);
                AddWaypoint(28, 2105.88, 195.461, 65.1854, 0);
                AddWaypoint(29, 2096.77, 196.939, 65.2117, 0);
                AddWaypoint(30, 2083.90, 209.395, 64.8736, 0);
                AddWaypoint(31, 2063.40, 229.509, 64.4883, 25000);
                AddWaypoint(32, 2063.40, 229.510, 64.4883, 0);
                AddWaypoint(33, 2063.40, 229.512, 64.4883, 0);
                ((npc_escortAI*)(me->AI()))->SetClearWaypoints(true);
                ((npc_escortAI*)(me->AI()))->SetDespawnAtEnd(false);
                ((npc_escortAI*)(me->AI()))->SetDespawnAtFar(false);
                Start(true, true, player->GetGUID());
                break;
            case 2:
                AddWaypoint(0, 2046.70, 251.941, 62.7851, 4000);
                AddWaypoint(1, 2046.70, 251.942, 62.7851, 3000);
                AddWaypoint(2, 2011.77, 278.478, 65.3388, 0);
                AddWaypoint(3, 2005.08, 289.676, 66.1179, 0);
                AddWaypoint(4, 2033.11, 337.450, 66.0948, 0);
                AddWaypoint(5, 2070.30, 416.208, 66.0893, 0);
                AddWaypoint(6, 2086.76, 469.768, 65.9182, 0);
                AddWaypoint(7, 2101.70, 497.955, 61.7881, 0);
                AddWaypoint(8, 2133.39, 530.933, 55.3700, 0);
                AddWaypoint(9, 2157.91, 559.635, 48.5157, 0);
                AddWaypoint(10, 2167.34, 586.191, 42.4394, 0);
                AddWaypoint(11, 2174.17, 637.643, 33.9002, 0);
                AddWaypoint(12, 2179.31, 656.053, 34.723, 0);
                AddWaypoint(13, 2183.65, 670.941, 34.0318, 0);
                AddWaypoint(14, 2201.50, 668.616, 36.1236, 0);
                AddWaypoint(15, 2221.56, 652.747, 36.6153, 0);
                AddWaypoint(16, 2238.97, 640.125, 37.2214, 0);
                AddWaypoint(17, 2251.17, 620.574, 40.1473, 0);
                AddWaypoint(18, 2261.98, 595.303, 41.4117, 0);
                AddWaypoint(19, 2278.67, 560.172, 38.9090, 0);
                AddWaypoint(20, 2336.72, 528.327, 40.9369, 0);
                AddWaypoint(21, 2381.04, 519.612, 37.7312, 0);
                AddWaypoint(22, 2412.20, 515.425, 39.2068, 0);
                AddWaypoint(23, 2452.39, 516.174, 42.9387, 0);
                AddWaypoint(24, 2467.38, 539.389, 47.4992, 0);
                AddWaypoint(25, 2470.70, 554.333, 46.6668, 0);
                AddWaypoint(26, 2478.07, 575.321, 55.4549, 0);
                AddWaypoint(27, 2480.00, 585.408, 56.6921, 0);
                AddWaypoint(28, 2482.67, 608.817, 55.6643, 0);
                AddWaypoint(29, 2485.62, 626.061, 58.0132, 2000);
                AddWaypoint(30, 2486.91, 626.356, 58.0761, 2000);
                AddWaypoint(31, 2486.91, 626.357, 58.0761, 0);
                ((npc_escortAI*)(me->AI()))->SetClearWaypoints(true);
                ((npc_escortAI*)(me->AI()))->SetDespawnAtEnd(false);
                ((npc_escortAI*)(me->AI()))->SetDespawnAtFar(false);
                Start(true, false, player->GetGUID());
                break;
            case 3:
                AddWaypoint(0, 2488.58, 660.940, 57.3913, 0);
                AddWaypoint(1, 2502.56, 686.059, 55.6252, 0);
                AddWaypoint(2, 2502.08, 694.360, 55.5083, 0);
                AddWaypoint(3, 2491.46, 694.321, 55.7163, 0);
                AddWaypoint(4, 2491.10, 703.300, 55.7630, 0);
                AddWaypoint(5, 2485.64, 702.992, 55.7917, 0);
                AddWaypoint(6, 2479.63, 696.521, 55.7901, 0);
                AddWaypoint(7, 2476.24, 696.204, 55.8093, 0);
                AddWaypoint(8, 2475.39, 695.983, 55.8146, 0);
                AddWaypoint(9, 2477.75, 694.473, 55.7945, 0);
                AddWaypoint(10, 2481.27, 697.747, 55.7910, 0);
                AddWaypoint(11, 2486.31, 703.131, 55.7861, 0);
                AddWaypoint(12, 2490.76, 703.511, 55.7662, 0);
                AddWaypoint(13, 2491.30, 694.792, 55.7195, 0);
                AddWaypoint(14, 2502.08, 694.360, 55.5083, 0);
                AddWaypoint(15, 2507.99, 679.298, 56.3760, 0);
                AddWaypoint(16, 2524.79, 669.919, 54.9258, 0);
                AddWaypoint(17, 2543.19, 665.289, 56.2957, 0);
                AddWaypoint(18, 2566.49, 664.354, 54.5034, 0);
                AddWaypoint(19, 2592.00, 664.611, 56.4394, 0);
                AddWaypoint(20, 2616.14, 665.499, 55.1610, 0);
                AddWaypoint(21, 2623.56, 666.965, 54.3983, 0);
                AddWaypoint(22, 2629.99, 661.059, 54.2738, 0);
                AddWaypoint(23, 2629.00, 656.982, 56.0651, 0);
                AddWaypoint(24, 2620.84, 633.007, 56.0300, 3000);
                AddWaypoint(25, 2621.62, 635.352, 56.0300, 0);
                AddWaypoint(26, 2622.99, 639.178, 56.0300, 0);
                AddWaypoint(27, 2628.73, 656.693, 56.0610, 0);
                AddWaypoint(28, 2630.34, 661.135, 54.2738, 0);
                AddWaypoint(29, 2635.38, 672.243, 54.4508, 0);
                AddWaypoint(30, 2644.13, 668.158, 55.3797, 0);
                AddWaypoint(31, 2646.82, 666.740, 56.9898, 0);
                AddWaypoint(32, 2658.22, 665.432, 57.1725, 0);
                AddWaypoint(33, 2661.88, 674.849, 57.1725, 0);
                AddWaypoint(34, 2656.23, 677.208, 57.1725, 0);
                AddWaypoint(35, 2652.28, 670.270, 61.9353, 0);
                AddWaypoint(36, 2650.79, 664.290, 61.9302, 0);
                AddWaypoint(37, 2660.48, 659.409, 61.9370, 5000);
                AddWaypoint(38, 2660.48, 659.410, 61.9370, 0);
                ((npc_escortAI*)(me->AI()))->SetClearWaypoints(true);
                ((npc_escortAI*)(me->AI()))->SetDespawnAtEnd(false);
                ((npc_escortAI*)(me->AI()))->SetDespawnAtFar(false);
                Start(true, true, player->GetGUID());
                break;
            case 4:
                AddWaypoint(0, 2660.48, 659.411, 61.9370, 5000);
                AddWaypoint(1, 2660.48, 659.412, 61.9370, 2000);
                AddWaypoint(2, 2652.32, 664.994, 61.9305, 0);
                AddWaypoint(3, 2652.37, 670.561, 61.9368, 0);
                AddWaypoint(4, 2656.05, 676.761, 57.1727, 0);
                AddWaypoint(5, 2658.49, 677.166, 57.1727, 0);
                AddWaypoint(6, 2659.28, 667.117, 57.1727, 0);
                AddWaypoint(7, 2649.71, 665.387, 57.1727, 0);
                AddWaypoint(8, 2634.79, 672.964, 54.4577, 0);
                AddWaypoint(9, 2635.06, 673.892, 54.4713, 6000);
                AddWaypoint(10, 2635.06, 673.893, 54.4713, 6000);
                AddWaypoint(11, 2635.06, 673.890, 54.4713, 0);
                AddWaypoint(12, 2634.30, 661.698, 54.4147, 0);
                AddWaypoint(13, 2652.21, 644.396, 56.1906, 0);
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
            DoCast(me->getVictim(), SPELL_STRIKE);
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
    newscript->Name="npc_brazen";
    newscript->pGossipHello =   &GossipHello_npc_brazen;
    newscript->pGossipSelect =  &GossipSelect_npc_brazen;
    newscript->RegisterSelf();

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
