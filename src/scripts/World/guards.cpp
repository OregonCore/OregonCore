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
SDName: Guards
SD%Complete: 100
SDComment: All Guard gossip data, quite some npc_text-id's still missing, adding constantly as new id's are known. CombatAI should be organized better for future.
SDCategory: Guards
EndScriptData */

/* ContentData
guard_azuremyst
guard_bluffwatcher
guard_contested
guard_darnassus
guard_dunmorogh
guard_durotar
guard_elwynnforest
guard_eversong
guard_exodar
guard_ironforge
guard_mulgore
guard_orgrimmar
guard_shattrath
guard_shattrath_aldor
guard_shattrath_scryer
guard_silvermoon
guard_stormwind
guard_teldrassil
guard_tirisfal
guard_undercity
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGuardAI.h"
#include "ScriptedGossip.h"

//script spesific action
#define GOSSIP_ACTION_TAVERN                  101
#define GOSSIP_ACTION_GEMMERCHANT             102
#define GOSSIP_ACTION_MANALOOM                103

//script spesific sender
#define GOSSIP_SENDER_SEC_GEMMERCHANT         101
#define GOSSIP_SENDER_SEC_AUCTIONHOUSE        102

//script spesific gossip text
#define GOSSIP_TEXT_TAVERN                    "Worlds End Tavern"
#define GOSSIP_TEXT_BANKSCYERS                "Scyers bank"
#define GOSSIP_TEXT_BANKALDOR                 "Aldor Bank"
#define GOSSIP_TEXT_INNSCYERS                 "Scyers Inn"
#define GOSSIP_TEXT_INNALDOR                  "Aldor Inn"
#define GOSSIP_TEXT_STABLESCYERS              "Scyers Stable"
#define GOSSIP_TEXT_STABLEALDOR               "Aldor Stable"
#define GOSSIP_TEXT_BATTLEMASTERALLIANCE      "Alliance Battlemasters"
#define GOSSIP_TEXT_BATTLEMASTERHORDE         "Horde Battlemasters"
#define GOSSIP_TEXT_BATTLEMASTERARENA         "Arena Battlemasters"
#define GOSSIP_TEXT_MANALOOM                  "Mana Loom"
#define GOSSIP_TEXT_ALCHEMYLAB                "Alchemy Lab"
#define GOSSIP_TEXT_GEMMERCHANT               "Gem Merchant"
#define GOSSIP_TEXT_GEMSCYERS                 "Scyers Gem Merchant"
#define GOSSIP_TEXT_GEMALDOR                  "Aldor Gem Merchant"

#define GOSSIP_TEXT_AH_SILVERMOON_1           "Western Auction House"
#define GOSSIP_TEXT_AH_SILVERMOON_2           "Royal Exchange Auction House"

#define GOSSIP_TEXT_INN_SILVERMOON_1          "Silvermoon City Inn"
#define GOSSIP_TEXT_INN_SILVERMOON_2          "Wayfarer's Rest tavern"

/*******************************************************
 * guard_azuremyst start
 *******************************************************/

bool GossipHello_guard_azuremyst(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANK            , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HIPPOGRYPH      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GUILDMASTER     , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN             , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_CLASSTRAINER    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER     , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
    player->SEND_GOSSIP_MENU(10066, pCreature->GetGUID());
    return true;
}

void SendDefaultMenu_guard_azuremyst(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Bank
        player->SEND_POI(-3918.95f, -11544.7f, 6, 6, 0, "Bank");
        player->SEND_GOSSIP_MENU(10067, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Hippogryph Master
        player->SEND_POI(-4057.15f, -11788.6f, 6, 6, 0, "Stephanos");
        player->SEND_GOSSIP_MENU(10071, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Guild master
        player->SEND_POI(-4092.43f, -11626.6f, 6, 6, 0, "Funaam");
        player->SEND_GOSSIP_MENU(10073, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Inn
        player->SEND_POI(-4129.43f, -12469, 6, 6, 0, "Caregiver Chellan");
        player->SEND_GOSSIP_MENU(10074, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Stable Master
        player->SEND_POI(-4146.42f, -12492.7f, 6, 6, 0, "Esbina");
        player->SEND_GOSSIP_MENU(10075, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Class trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_DRUID               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HUNTER              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAGE                , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PALADIN             , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PRIEST              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SHAMAN              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARRIOR             , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->SEND_GOSSIP_MENU(10076, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Profession trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BLACKSMITHING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENCHANTING          , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENGINEERING         , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FISHING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HERBALISM           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_JEWELCRAFTING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 9);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING      , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 10);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MINING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 11);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 12);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAILORING           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 13);
        player->SEND_GOSSIP_MENU(10087, pCreature->GetGUID());
        break;
    }
}

void SendClassTrainerMenu_guard_azuremyst(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Druid
        player->SEND_POI(-4274.81f, -11495.3f, 6, 6, 0, "Shalannius");
        player->SEND_GOSSIP_MENU(10077, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Hunter
        player->SEND_POI(-4203.65f, -12526.5f, 6, 6, 0, "Acteon");
        player->SEND_GOSSIP_MENU(10078, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Mage
        player->SEND_POI(-4149.62f, -12530.1f, 6, 6, 0, "Semid");
        player->SEND_GOSSIP_MENU(10081, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Paladin
        player->SEND_POI(-4138.98f, -12468.5f, 6, 6, 0, "Tullas");
        player->SEND_GOSSIP_MENU(10083, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Priest
        player->SEND_POI(-4131.66f, -12478.6f, 6, 6, 0, "Guvan");
        player->SEND_GOSSIP_MENU(10084, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Shaman
        player->SEND_POI(-4162.33f, -12456.1f, 6, 6, 0, "Tuluun");
        player->SEND_GOSSIP_MENU(10085, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Warrior
        player->SEND_POI(-4165.05f, -12536.4f, 6, 6, 0, "Ruada");
        player->SEND_GOSSIP_MENU(10086, pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_azuremyst(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_POI(-4191.15f, -12470, 6, 6, 0, "Daedal");
        player->SEND_GOSSIP_MENU(10088, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Blacksmithing
        player->SEND_POI(-4726.29f, -12387, 6, 6, 0, "Blacksmith Calypso");
        player->SEND_GOSSIP_MENU(10089, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Cooking
        player->SEND_POI(-4710.87f, -12400.6f, 6, 6, 0, "'Cookie' McWeaksauce");
        player->SEND_GOSSIP_MENU(10090, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Enchanting
        player->SEND_POI(-3882.85f, -11496.7f, 6, 6, 0, "Nahogg");
        player->SEND_GOSSIP_MENU(10091, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Engineering
        player->SEND_POI(-4157.57f, -12470.2f, 6, 6, 0, "Artificer Daelo");
        player->SEND_GOSSIP_MENU(10092, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //First Aid
        player->SEND_POI(-4199.11f, -12469.9f, 6, 6, 0, "Anchorite Fateema");
        player->SEND_GOSSIP_MENU(10093, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Fishing
        player->SEND_POI(-4266.38f, -12985.1f, 6, 6, 0, "Diktynna");
        player->SEND_GOSSIP_MENU(10094, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Herbalism
        player->SEND_GOSSIP_MENU(10095, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Jewelcrafting
        player->SEND_POI(-3781.55f, -11541.8f, 6, 6, 0, "Farii");
        player->SEND_GOSSIP_MENU(10096, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Leatherworking
        player->SEND_POI(-3442.68f, -12322.2f, 6, 6, 0, "Moordo");
        player->SEND_GOSSIP_MENU(10098, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Mining
        player->SEND_POI(-4179.89f, -12493.1f, 6, 6, 0, "Dulvi");
        player->SEND_GOSSIP_MENU(10097, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 12:                   //Skinning
        player->SEND_POI(-3431.17f, -12316.5f, 6, 6, 0, "Gurf");
        player->SEND_GOSSIP_MENU(10098, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 13:                   //Tailoring
        player->SEND_POI(-4711.54f, -12386.7f, 6, 6, 0, "Erin Kelly");
        player->SEND_GOSSIP_MENU(10099, pCreature->GetGUID());
        break;
    }
}

bool GossipSelect_guard_azuremyst(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_azuremyst(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_CLASSTRAIN:
        SendClassTrainerMenu_guard_azuremyst(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_azuremyst(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_azuremyst end
 *******************************************************/

CreatureAI* GetAI_guard_azuremyst(Creature* pCreature)
{
    return new guardAI (pCreature);
}

/*******************************************************
 * guard_bluffwatcher start
 *******************************************************/

bool GossipHello_guard_bluffwatcher(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANK         , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WINDRIDER    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GUILDMASTER  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN          , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAILBOX      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_AUCTIONHOUSE , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WEAPONMASTER , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTER , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_CLASSTRAINER , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
    player->SEND_GOSSIP_MENU(3543, pCreature->GetGUID());
    return true;
}

void SendDefaultMenu_guard_bluffwatcher(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Bank
        player->SEND_POI(-1257.8f, 24.14f, 6, 6, 0, "Thunder Bluff Bank");
        player->SEND_GOSSIP_MENU(1292, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Wind master
        player->SEND_POI(-1196.43f, 28.26f, 6, 6, 0, "Wind Rider Roost");
        player->SEND_GOSSIP_MENU(1293, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Guild master
        player->SEND_POI(-1296.5f, 127.57f, 6, 6, 0, "Thunder Bluff Civic Information");
        player->SEND_GOSSIP_MENU(1291, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Inn
        player->SEND_POI(-1296, 39.7f, 6, 6, 0, "Thunder Bluff Inn");
        player->SEND_GOSSIP_MENU(3153, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Mailbox
        player->SEND_POI(-1263.59f, 44.36f, 6, 6, 0, "Thunder Bluff Mailbox");
        player->SEND_GOSSIP_MENU(3154, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Auction House
        player->SEND_POI(1381.77f, -4371.16f, 6, 6, 0, GOSSIP_TEXT_AUCTIONHOUSE);
        player->SEND_GOSSIP_MENU(3155, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Weapon master
        player->SEND_POI(-1282.31f, 89.56f, 6, 6, 0, "Ansekhwa");
        player->SEND_GOSSIP_MENU(4520, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Stable master
        player->SEND_POI(-1270.19f, 48.84f, 6, 6, 0, "Bulrug");
        player->SEND_GOSSIP_MENU(5977, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //battlemaster
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALTERACVALLEY       , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ARATHIBASIN         , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARSONGULCH         , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 3);
        player->SEND_GOSSIP_MENU(7527, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Class trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_DRUID               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HUNTER              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAGE                , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PRIEST              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SHAMAN              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARRIOR             , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->SEND_GOSSIP_MENU(3542, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Profession trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BLACKSMITHING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENCHANTING          , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FISHING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HERBALISM           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING      , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MINING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 9);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 10);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAILORING           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 11);
        player->SEND_GOSSIP_MENU(3541, pCreature->GetGUID());
        break;
    }
}

void SendBattleMasterMenu_guard_bluffwatcher(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //AV
        player->SEND_POI(-1387.82f, -97.55f, 6, 6, 0, "Taim Ragetotem");
        player->SEND_GOSSIP_MENU(7522, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //AB
        player->SEND_POI(-997, 214.12f, 6, 6, 0, "Martin Lindsey");
        player->SEND_GOSSIP_MENU(7648, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //WSG
        player->SEND_POI(-1384.94f, -75.91f, 6, 6, 0, "Kergul Bloodaxe");
        player->SEND_GOSSIP_MENU(7523, pCreature->GetGUID());
        break;
    }
}

void SendClassTrainerMenu_guard_bluffwatcher(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Druid
        player->SEND_POI(-1054.47f, -285, 6, 6, 0, "Hall of Elders");
        player->SEND_GOSSIP_MENU(1294, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Hunter
        player->SEND_POI(-1416.32f, -114.28f, 6, 6, 0, "Hunter's Hall");
        player->SEND_GOSSIP_MENU(1295, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Mage
        player->SEND_POI(-1061.2f, 195.5f, 6, 6, 0, "Pools of Vision");
        player->SEND_GOSSIP_MENU(1296, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Priest
        player->SEND_POI(-1061.2f, 195.5f, 6, 6, 0, "Pools of Vision");
        player->SEND_GOSSIP_MENU(1297, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Shaman
        player->SEND_POI(-989.54f, 278.25f, 6, 6, 0, "Hall of Spirits");
        player->SEND_GOSSIP_MENU(1298, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Warrior
        player->SEND_POI(-1416.32f, -114.28f, 6, 6, 0, "Hunter's Hall");
        player->SEND_GOSSIP_MENU(1299, pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_bluffwatcher(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_POI(-1085.56f, 27.29f, 6, 6, 0, "Bena's Alchemy");
        player->SEND_GOSSIP_MENU(1332, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Blacksmithing
        player->SEND_POI(-1239.75f, 104.88f, 6, 6, 0, "Karn's Smithy");
        player->SEND_GOSSIP_MENU(1333, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Cooking
        player->SEND_POI(-1214.5f, -21.23f, 6, 6, 0, "Aska's Kitchen");
        player->SEND_GOSSIP_MENU(1334, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Enchanting
        player->SEND_POI(-1112.65f, 48.26f, 6, 6, 0, "Dawnstrider Enchanters");
        player->SEND_GOSSIP_MENU(1335, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //First Aid
        player->SEND_POI(-996.58f, 200.5f, 6, 6, 0, "Spiritual Healing");
        player->SEND_GOSSIP_MENU(1336, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Fishing
        player->SEND_POI(-1169.35f, -68.87f, 6, 6, 0, "Mountaintop Bait & Tackle");
        player->SEND_GOSSIP_MENU(1337, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Herbalism
        player->SEND_POI(-1137.7f, -1.51f, 6, 6, 0, "Holistic Herbalism");
        player->SEND_GOSSIP_MENU(1338, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Leatherworking
        player->SEND_POI(-1156.22f, 66.86f, 6, 6, 0, "Thunder Bluff Armorers");
        player->SEND_GOSSIP_MENU(1339, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Mining
        player->SEND_POI(-1249.17f, 155, 6, 6, 0, "Stonehoof Geology");
        player->SEND_GOSSIP_MENU(1340, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Skinning
        player->SEND_POI(-1148.56f, 51.18f, 6, 6, 0, "Mooranta");
        player->SEND_GOSSIP_MENU(1343, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Tailoring
        player->SEND_POI(-1156.22f, 66.86f, 6, 6, 0, "Thunder Bluff Armorers");
        player->SEND_GOSSIP_MENU(1341, pCreature->GetGUID());
        break;
    }
}

bool GossipSelect_guard_bluffwatcher(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_bluffwatcher(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_CLASSTRAIN:
        SendClassTrainerMenu_guard_bluffwatcher(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_bluffwatcher(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_BATTLEINFO:
        SendBattleMasterMenu_guard_bluffwatcher(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_bluffwatcher end
 *******************************************************/

CreatureAI* GetAI_guard_bluffwatcher(Creature* pCreature)
{
    return new guardAI (pCreature);
}

/*******************************************************
 * guard_contested start
 *******************************************************/

CreatureAI* GetAI_guard_contested(Creature* pCreature)
{
    return new guardAI (pCreature);
}
/*******************************************************
 * guard_contested end
 *******************************************************/

/*******************************************************
 * guard_darnassus start
 *******************************************************/

bool GossipHello_guard_darnassus(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_AUCTIONHOUSE   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANK           , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HIPPOGRYPH     , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GUILDMASTER    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN            , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAILBOX        , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WEAPONMASTER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_CLASSTRAINER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
    player->SEND_GOSSIP_MENU(3016, pCreature->GetGUID());
    return true;
}

void SendDefaultMenu_guard_darnassus(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Auction house
        player->SEND_POI(9861.23f, 2334.55f, 6, 6, 0, "Darnassus Auction House");
        player->SEND_GOSSIP_MENU(3833, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Bank
        player->SEND_POI(9938.45f, 2512.35f, 6, 6, 0, "Darnassus Bank");
        player->SEND_GOSSIP_MENU(3017, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Wind master
        player->SEND_POI(9945.65f, 2618.94f, 6, 6, 0, "Rut'theran Village");
        player->SEND_GOSSIP_MENU(3018, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Guild master
        player->SEND_POI(10076.40f, 2199.59f, 6, 6, 0, "Darnassus Guild Master");
        player->SEND_GOSSIP_MENU(3019, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Inn
        player->SEND_POI(10133.29f, 2222.52f, 6, 6, 0, "Darnassus Inn");
        player->SEND_GOSSIP_MENU(3020, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Mailbox
        player->SEND_POI(9942.17f, 2495.48f, 6, 6, 0, "Darnassus Mailbox");
        player->SEND_GOSSIP_MENU(3021, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Stable master
        player->SEND_POI(10167.20f, 2522.66f, 6, 6, 0, "Alassin");
        player->SEND_GOSSIP_MENU(5980, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Weapon trainer
        player->SEND_POI(9907.11f, 2329.70f, 6, 6, 0, "Ilyenia Moonfire");
        player->SEND_GOSSIP_MENU(4517, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Battlemaster
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALTERACVALLEY   , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ARATHIBASIN     , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARSONGULCH     , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 3);
        player->SEND_GOSSIP_MENU(7519, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Class trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_DRUID           , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HUNTER          , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PRIEST          , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ROGUE           , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARRIOR         , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->SEND_GOSSIP_MENU(4264, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Profession trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY         , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING         , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENCHANTING      , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FISHING         , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HERBALISM       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING  , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAILORING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 9);
        player->SEND_GOSSIP_MENU(4273, pCreature->GetGUID());
        break;
    }
}

void SendBattleMasterMenu_guard_darnassus(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //AV
        player->SEND_POI(9923.61f, 2327.43f, 6, 6, 0, "Brogun Stoneshield");
        player->SEND_GOSSIP_MENU(7518, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //AB
        player->SEND_POI(9977.37f, 2324.39f, 6, 6, 0, "Keras Wolfheart");
        player->SEND_GOSSIP_MENU(7651,  pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //WSG
        player->SEND_POI(9979.84f, 2315.79f, 6, 6, 0, "Aethalas");
        player->SEND_GOSSIP_MENU(7482, pCreature->GetGUID());
        break;
    }
}

void SendClassTrainerMenu_guard_darnassus(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Druid
        player->SEND_POI(10186, 2570.46f, 6, 6, 0, "Darnassus Druid Trainer");
        player->SEND_GOSSIP_MENU(3024, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Hunter
        player->SEND_POI(10177.29f, 2511.10f, 6, 6, 0, "Darnassus Hunter Trainer");
        player->SEND_GOSSIP_MENU(3023, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Priest
        player->SEND_POI(9659.12f, 2524.88f, 6, 6, 0, "Temple of the Moon");
        player->SEND_GOSSIP_MENU(3025, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Rogue
        player->SEND_POI(10122, 2599.12f, 6, 6, 0, "Darnassus Rogue Trainer");
        player->SEND_GOSSIP_MENU(3026, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Warrior
        player->SEND_POI(9951.91f, 2280.38f, 6, 6, 0, "Warrior's Terrace");
        player->SEND_GOSSIP_MENU(3033, pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_darnassus(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_POI(10075.90f, 2356.76f, 6, 6, 0, "Darnassus Alchemy Trainer");
        player->SEND_GOSSIP_MENU(3035, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Cooking
        player->SEND_POI(10088.59f, 2419.21f, 6, 6, 0, "Darnassus Cooking Trainer");
        player->SEND_GOSSIP_MENU(3036, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Enchanting
        player->SEND_POI(10146.09f, 2313.42f, 6, 6, 0, "Darnassus Enchanting Trainer");
        player->SEND_GOSSIP_MENU(3337, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //First Aid
        player->SEND_POI(10150.09f, 2390.43f, 6, 6, 0, "Darnassus First Aid Trainer");
        player->SEND_GOSSIP_MENU(3037, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Fishing
        player->SEND_POI(9836.20f, 2432.17f, 6, 6, 0, "Darnassus Fishing Trainer");
        player->SEND_GOSSIP_MENU(3038, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Herbalism
        player->SEND_POI(9757.17f, 2430.16f, 6, 6, 0, "Darnassus Herbalism Trainer");
        player->SEND_GOSSIP_MENU(3039, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Leatherworking
        player->SEND_POI(10086.59f, 2255.77f, 6, 6, 0, "Darnassus Leatherworking Trainer");
        player->SEND_GOSSIP_MENU(3040, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Skinning
        player->SEND_POI(10081.40f, 2257.18f, 6, 6, 0, "Darnassus Skinning Trainer");
        player->SEND_GOSSIP_MENU(3042, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Tailoring
        player->SEND_POI(10079.70f, 2268.19f, 6, 6, 0, "Darnassus Tailor");
        player->SEND_GOSSIP_MENU(3044, pCreature->GetGUID());
        break;
    }
}

bool GossipSelect_guard_darnassus(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_darnassus(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_CLASSTRAIN:
        SendClassTrainerMenu_guard_darnassus(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_darnassus(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_BATTLEINFO:
        SendBattleMasterMenu_guard_darnassus(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_darnassus end
 *******************************************************/

CreatureAI* GetAI_guard_darnassus(Creature* pCreature)
{
    return new guardAI (pCreature);
}

/*******************************************************
 * guard_dunmorogh start
 *******************************************************/

bool GossipHello_guard_dunmorogh(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANK             , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HIPPOGRYPH       , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GUILDMASTER      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN              , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER     , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_CLASSTRAINER     , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
    player->SEND_GOSSIP_MENU(4287, pCreature->GetGUID());

    return true;
}

void SendDefaultMenu_guard_dunmorogh(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Bank
        player->SEND_GOSSIP_MENU(4288, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Gryphon master
        player->SEND_GOSSIP_MENU(4289, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Guild master
        player->SEND_GOSSIP_MENU(4290, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Inn
        player->SEND_POI(-5582.66f, -525.89f, 6, 6, 0, "Thunderbrew Distillery");
        player->SEND_GOSSIP_MENU(4291, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Stable Master
        player->SEND_POI(-5604, -509.58f, 6, 6, 0, "Shelby Stoneflint");
        player->SEND_GOSSIP_MENU(5985, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Class trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HUNTER               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAGE                 , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PALADIN              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PRIEST               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ROGUE                , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARLOCK              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARRIOR              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->SEND_GOSSIP_MENU(4292, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Profession trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BLACKSMITHING        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENCHANTING           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENGINEERING          , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FISHING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HERBALISM            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 9);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MINING               , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 10);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 11);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAILORING            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 12);
        player->SEND_GOSSIP_MENU(4300, pCreature->GetGUID());
        break;
    }
}

void SendClassTrainerMenu_guard_dunmorogh(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Hunter
        player->SEND_POI(-5618.29f, -454.25f, 6, 6, 0, "Grif Wildheart");
        player->SEND_GOSSIP_MENU(4293, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Mage
        player->SEND_POI(-5585.6f, -539.99f, 6, 6, 0, "Magis Sparkmantle");
        player->SEND_GOSSIP_MENU(4294, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Paladin
        player->SEND_POI(-5585.6f, -539.99f, 6, 6, 0, "Azar Stronghammer");
        player->SEND_GOSSIP_MENU(4295, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Priest
        player->SEND_POI(-5591.74f, -525.61f, 6, 6, 0, "Maxan Anvol");
        player->SEND_GOSSIP_MENU(4296, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Rogue
        player->SEND_POI(-5602.75f, -542.4f, 6, 6, 0, "Hogral Bakkan");
        player->SEND_GOSSIP_MENU(4297, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Warlock
        player->SEND_POI(-5641.97f, -523.76f, 6, 6, 0, "Gimrizz Shadowcog");
        player->SEND_GOSSIP_MENU(4298, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Warrior
        player->SEND_POI(-5604.79f, -529.38f, 6, 6, 0, "Granis Swiftaxe");
        player->SEND_GOSSIP_MENU(4299, pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_dunmorogh(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_GOSSIP_MENU(4301, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Blacksmithing
        player->SEND_POI(-5584.72f, -428.41f, 6, 6, 0, "Tognus Flintfire");
        player->SEND_GOSSIP_MENU(4302, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Cooking
        player->SEND_POI(-5596.85f, -541.43f, 6, 6, 0, "Gremlock Pilsnor");
        player->SEND_GOSSIP_MENU(4303, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Enchanting
        player->SEND_GOSSIP_MENU(4304, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Engineering
        player->SEND_POI(-5531, -666.53f, 6, 6, 0, "Bronk Guzzlegear");
        player->SEND_GOSSIP_MENU(4305, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //First Aid
        player->SEND_POI(-5603.67f, -523.57f, 6, 6, 0, "Thamner Pol");
        player->SEND_GOSSIP_MENU(4306, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Fishing
        player->SEND_POI(-5199.9f, 58.58f, 6, 6, 0, "Paxton Ganter");
        player->SEND_GOSSIP_MENU(4307, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Herbalism
        player->SEND_GOSSIP_MENU(4308, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Leatherworking
        player->SEND_GOSSIP_MENU(4310, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Mining
        player->SEND_POI(-5531, -666.53f, 6, 6, 0, "Yarr Hamerstone");
        player->SEND_GOSSIP_MENU(4311, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Skinning
        player->SEND_GOSSIP_MENU(4312, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 12:                   //Tailoring
        player->SEND_GOSSIP_MENU(4313, pCreature->GetGUID());
        break;
    }
}

bool GossipSelect_guard_dunmorogh(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_dunmorogh(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_CLASSTRAIN:
        SendClassTrainerMenu_guard_dunmorogh(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_dunmorogh(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_dunmorogh end
 *******************************************************/

CreatureAI* GetAI_guard_dunmorogh(Creature* pCreature)
{
    return new guardAI (pCreature);
}

/*******************************************************
 * guard_durotar start
 *******************************************************/

bool GossipHello_guard_durotar(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANK         , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WINDRIDER    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN          , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_CLASSTRAINER , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->SEND_GOSSIP_MENU(4037, pCreature->GetGUID());
    return true;
}

void SendDefaultMenu_guard_durotar(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Bank
        player->SEND_GOSSIP_MENU(4032, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Wind rider
        player->SEND_GOSSIP_MENU(4033, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Inn
        player->SEND_POI(338.7f, -4688.87f, 6, 6, 0, "Razor Hill Inn");
        player->SEND_GOSSIP_MENU(4034, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Stable master
        player->SEND_POI(330.31f, -4710.66f, 6, 6, 0, "Shoja'my");
        player->SEND_GOSSIP_MENU(5973, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Class trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HUNTER              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAGE                , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PRIEST              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ROGUE               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SHAMAN              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARLOCK             , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARRIOR             , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->SEND_GOSSIP_MENU(4035, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Profession trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BLACKSMITHING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENCHANTING          , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENGINEERING         , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FISHING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HERBALISM           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING      , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 9);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MINING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 10);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 11);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAILORING           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 12);
        player->SEND_GOSSIP_MENU(4036, pCreature->GetGUID());
        break;
    }
}

void SendClassTrainerMenu_guard_durotar(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Hunter
        player->SEND_POI(276, -4706.72f, 6, 6, 0, "Thotar");
        player->SEND_GOSSIP_MENU(4013, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Mage
        player->SEND_POI(-839.33f, -4935.6f, 6, 6, 0, "Un'Thuwa");
        player->SEND_GOSSIP_MENU(4014, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Priest
        player->SEND_POI(296.22f, -4828.1f, 6, 6, 0, "Tai'jin");
        player->SEND_GOSSIP_MENU(4015, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Rogue
        player->SEND_POI(265.76f, -4709, 6, 6, 0, "Kaplak");
        player->SEND_GOSSIP_MENU(4016, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Shaman
        player->SEND_POI(307.79f, -4836.97f, 6, 6, 0, "Swart");
        player->SEND_GOSSIP_MENU(4017, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Warlock
        player->SEND_POI(355.88f, -4836.45f, 6, 6, 0, "Dhugru Gorelust");
        player->SEND_GOSSIP_MENU(4018, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Warrior
        player->SEND_POI(312.3f, -4824.66f, 6, 6, 0, "Tarshaw Jaggedscar");
        player->SEND_GOSSIP_MENU(4019, pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_durotar(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_POI(-800.25f, -4894.33f, 6, 6, 0, "Miao'zan");
        player->SEND_GOSSIP_MENU(4020, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Blacksmithing
        player->SEND_POI(373.24f, -4716.45f, 6, 6, 0, "Dwukk");
        player->SEND_GOSSIP_MENU(4021, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Cooking
        player->SEND_GOSSIP_MENU(4022, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Enchanting
        player->SEND_GOSSIP_MENU(4023, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Engineering
        player->SEND_POI(368.95f, -4723.95f, 6, 6, 0, "Mukdrak");
        player->SEND_GOSSIP_MENU(4024, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //First Aid
        player->SEND_POI(327.17f, -4825.62f, 6, 6, 0, "Rawrk");
        player->SEND_GOSSIP_MENU(4025, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Fishing
        player->SEND_POI(-1065.48f, -4777.43f, 6, 6, 0, "Lau'Tiki");
        player->SEND_GOSSIP_MENU(4026, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Herbalism
        player->SEND_POI(-836.25f, -4896.89f, 6, 6, 0, "Mishiki");
        player->SEND_GOSSIP_MENU(4027, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Leatherworking
        player->SEND_GOSSIP_MENU(4028, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Mining
        player->SEND_POI(366.94f, -4705, 6, 6, 0, "Krunn");
        player->SEND_GOSSIP_MENU(4029, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Skinning
        player->SEND_GOSSIP_MENU(4030, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 12:                   //Tailoring
        player->SEND_GOSSIP_MENU(4031, pCreature->GetGUID());
        break;
    }
}

bool GossipSelect_guard_durotar(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_durotar(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_CLASSTRAIN:
        SendClassTrainerMenu_guard_durotar(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_durotar(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_durotar end
 *******************************************************/

CreatureAI* GetAI_guard_durotar(Creature* pCreature)
{
    return new guardAI (pCreature);
}

/*******************************************************
 * guard_elwynnforest start
 *******************************************************/

bool GossipHello_guard_elwynnforest(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANK          , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GRYPHON       , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GUILDMASTER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN           , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_CLASSTRAINER  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
    player->SEND_GOSSIP_MENU(933, pCreature->GetGUID());
    return true;
}

void SendDefaultMenu_guard_elwynnforest(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Bank
        player->SEND_GOSSIP_MENU(4260, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Gryphon master
        player->SEND_GOSSIP_MENU(4261, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Guild master
        player->SEND_GOSSIP_MENU(4262, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Inn
        player->SEND_POI(-9459.34f, 42.08f, 6, 6, 0, "Lion's Pride Inn");
        player->SEND_GOSSIP_MENU(4263, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Stable Master
        player->SEND_POI(-9466.62f, 45.87f, 6, 6, 0, "Erma");
        player->SEND_GOSSIP_MENU(5983, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Class trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_DRUID               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HUNTER              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAGE                , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PALADIN             , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PRIEST              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ROGUE               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARLOCK             , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARRIOR             , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->SEND_GOSSIP_MENU(4264, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Profession trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BLACKSMITHING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENCHANTING          , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENGINEERING         , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FISHING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HERBALISM           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING      , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 9);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MINING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 10);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 11);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAILORING           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 12);
        player->SEND_GOSSIP_MENU(4273, pCreature->GetGUID());
        break;
    }
}

void SendClassTrainerMenu_guard_elwynnforest(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Druid
        player->SEND_GOSSIP_MENU(4265, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Hunter
        player->SEND_GOSSIP_MENU(4266, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Mage
        player->SEND_POI(-9471.12f, 33.44f, 6, 6, 0, "Zaldimar Wefhellt");
        player->SEND_GOSSIP_MENU(4268, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Paladin
        player->SEND_POI(-9469, 108.05f, 6, 6, 0, "Brother Wilhelm");
        player->SEND_GOSSIP_MENU(4269, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Priest
        player->SEND_POI(-9461.07f, 32.6f, 6, 6, 0, "Priestess Josetta");
        player->SEND_GOSSIP_MENU(4267, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Rogue
        player->SEND_POI(-9465.13f, 13.29f, 6, 6, 0, "Keryn Sylvius");
        player->SEND_GOSSIP_MENU(4270, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Warlock
        player->SEND_POI(-9473.21f, -4.08f, 6, 6, 0, "Maximillian Crowe");
        player->SEND_GOSSIP_MENU(4272, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Warrior
        player->SEND_POI(-9461.82f, 109.50f, 6, 6, 0, "Lyria Du Lac");
        player->SEND_GOSSIP_MENU(4271, pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_elwynnforest(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_POI(-9057.04f, 153.63f, 6, 6, 0, "Alchemist Mallory");
        player->SEND_GOSSIP_MENU(4274, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Blacksmithing
        player->SEND_POI(-9456.58f, 87.90f, 6, 6, 0, "Smith Argus");
        player->SEND_GOSSIP_MENU(4275, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Cooking
        player->SEND_POI(-9467.54f, -3.16f, 6, 6, 0, "Tomas");
        player->SEND_GOSSIP_MENU(4276, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Enchanting
        player->SEND_GOSSIP_MENU(4277, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Engineering
        player->SEND_GOSSIP_MENU(4278, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //First Aid
        player->SEND_POI(-9456.82f, 30.49f, 6, 6, 0, "Michelle Belle");
        player->SEND_GOSSIP_MENU(4279, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Fishing
        player->SEND_POI(-9386.54f, -118.73f, 6, 6, 0, "Lee Brown");
        player->SEND_GOSSIP_MENU(4280, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Herbalism
        player->SEND_POI(-9060.70f, 149.23f, 6, 6, 0, "Herbalist Pomeroy");
        player->SEND_GOSSIP_MENU(4281, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Leatherworking
        player->SEND_POI(-9376.12f, -75.23f, 6, 6, 0, "Adele Fielder");
        player->SEND_GOSSIP_MENU(4282, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Mining
        player->SEND_GOSSIP_MENU(4283, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Skinning
        player->SEND_POI(-9536.91f, -1212.76f, 6, 6, 0, "Helene Peltskinner");
        player->SEND_GOSSIP_MENU(4284, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 12:                   //Tailoring
        player->SEND_POI(-9376.12f, -75.23f, 6, 6, 0, "Eldrin");
        player->SEND_GOSSIP_MENU(4285, pCreature->GetGUID());
        break;
    }
}

bool GossipSelect_guard_elwynnforest(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_elwynnforest(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_CLASSTRAIN:
        SendClassTrainerMenu_guard_elwynnforest(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_elwynnforest(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_elwynnforest end
 *******************************************************/

CreatureAI* GetAI_guard_elwynnforest(Creature* pCreature)
{
    return new guardAI (pCreature);
}

/*******************************************************
 * guard_eversong start
 *******************************************************/

bool GossipHello_guard_eversong(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATHANDLER    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GUILDMASTER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN           , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_CLASSTRAINER  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->SEND_GOSSIP_MENU(10180, pCreature->GetGUID());
    return true;
}

void SendDefaultMenu_guard_eversong(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Bat Handler
        player->SEND_POI(9371.93f, -7164.80f, 6, 6, 0, "Skymistress Gloaming");
        player->SEND_GOSSIP_MENU(10181, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Guild master
        player->SEND_GOSSIP_MENU(10182, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Inn
        player->SEND_POI(9483.74f, -6844.58f, 6, 6, 0, "Delaniel's inn");
        player->SEND_GOSSIP_MENU(10183, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Stable Master
        player->SEND_POI(9489.62f, -6829.93f, 6, 6, 0, "Anathos");
        player->SEND_GOSSIP_MENU(10184, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Class trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_DRUID               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HUNTER              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAGE                , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PALADIN             , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PRIEST              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ROGUE               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARLOCK             , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->SEND_GOSSIP_MENU(10180, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Profession trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BLACKSMITHING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENGINEERING         , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FISHING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HERBALISM           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_JEWELCRAFTING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING      , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 9);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MINING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 10);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 11);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAILORING           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 12);
        player->SEND_GOSSIP_MENU(10180, pCreature->GetGUID());
        break;
    }
}

void SendClassTrainerMenu_guard_eversong(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Druid
        player->SEND_GOSSIP_MENU(10185, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Hunter
        player->SEND_POI(9527.44f, -6865.25f, 6, 6, 0, "Hannovia");
        player->SEND_GOSSIP_MENU(10186, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Mage
        player->SEND_POI(9464.24f, -6855.52f, 6, 6, 0, "Garridel");
        player->SEND_GOSSIP_MENU(10187, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Paladin
        player->SEND_POI(9517.61f, -6871.04f, 6, 6, 0, "Noellene");
        player->SEND_GOSSIP_MENU(10189, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Priest
        player->SEND_POI(9467.39f, -6845.72f, 6, 6, 0, "Ponaris");
        player->SEND_GOSSIP_MENU(10190, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Rogue
        player->SEND_POI(9533.67f, -6877.39f, 6, 6, 0, "Tannaria");
        player->SEND_GOSSIP_MENU(10191, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Warlock
        player->SEND_POI(9468.99f, -6865.60f, 6, 6, 0, "Celoenus");
        player->SEND_GOSSIP_MENU(10192, pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_eversong(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_POI(8659.90f, -6368.12f, 6, 6, 0, "Arcanist Sheynathren");
        player->SEND_GOSSIP_MENU(10193, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Blacksmithing
        player->SEND_POI(8984.21f, -7419.21f, 6, 6, 0, "Arathel Sunforge");
        player->SEND_GOSSIP_MENU(10194, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Cooking
        player->SEND_POI(9494.04f, -6881.51f, 6, 6, 0, "Quarelestra");
        player->SEND_GOSSIP_MENU(10195, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Engineering
        player->SEND_GOSSIP_MENU(10197, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //First Aid
        player->SEND_POI(9479.46f, -6879.16f, 6, 6, 0, "Kanaria");
        player->SEND_GOSSIP_MENU(10198, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Fishing
        player->SEND_GOSSIP_MENU(10199, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Herbalism
        player->SEND_POI(8678.92f, -6329.09f, 6, 6, 0, "Botanist Tyniarrel");
        player->SEND_GOSSIP_MENU(10200, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Jewelcrafting
        player->SEND_POI(9484.32f, -6874.98f, 6, 6, 0, "Aleinia");
        player->SEND_GOSSIP_MENU(10203, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Leatherworking
        player->SEND_POI(9362.04f, -7130.33f, 6, 6, 0, "Sathein");
        player->SEND_GOSSIP_MENU(10204, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Mining
        player->SEND_GOSSIP_MENU(10205, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Skinning
        player->SEND_POI(9362.04f, -7130.33f, 6, 6, 0, "Mathreyn");
        player->SEND_GOSSIP_MENU(10206, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 12:                   //Tailoring
        player->SEND_POI(8680.36f, -6327.51f, 6, 6, 0, "Sempstress Ambershine");
        player->SEND_GOSSIP_MENU(10207, pCreature->GetGUID());
        break;
    }
}

bool GossipSelect_guard_eversong(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_eversong(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_CLASSTRAIN:
        SendClassTrainerMenu_guard_eversong(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_eversong(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_eversong end
 *******************************************************/

CreatureAI* GetAI_guard_eversong(Creature* pCreature)
{
    return new guardAI (pCreature);
}

/*******************************************************
 * guard_exodar start
 *******************************************************/

bool GossipHello_guard_exodar(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_AUCTIONHOUSE   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANK           , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GUILDMASTER    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HIPPOGRYPH     , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN            , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAILBOX        , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WEAPONMASTER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_CLASSTRAINER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
    player->SEND_GOSSIP_MENU(9551, pCreature->GetGUID());
    return true;
}

void SendDefaultMenu_guard_exodar(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Auction house
        player->SEND_POI(-4023.6f, -11739.3f, 6, 6, 0, "Exodar Auction House");
        player->SEND_GOSSIP_MENU(9528, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Bank
        player->SEND_POI(-3923.89f, -11544.5f, 6, 6, 0, "Exodar Bank");
        player->SEND_GOSSIP_MENU(9529, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Guild master
        player->SEND_POI(-4092.57f, -11626.5f, 6, 6, 0, "Exodar Guild Master");
        player->SEND_GOSSIP_MENU(9539, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Hippogryph master
        player->SEND_POI(-4060.46f, -11787.1f, 6, 6, 0, "Exodar Hippogryph Master");
        player->SEND_GOSSIP_MENU(9530, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Inn
        player->SEND_POI(-3741.87f, -11695.1f, 6, 6, 0, "Exodar Inn");
        player->SEND_GOSSIP_MENU(9545, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Mailbox
        player->SEND_POI(-3972.5f, -11696.0, 6, 6, 0, "Mailbox");
        player->SEND_GOSSIP_MENU(10254, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Stable master
        player->SEND_POI(-3786.5f, -11702.5f, 6, 6, 0, "Stable Master Arthaid");
        player->SEND_GOSSIP_MENU(9558, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Weapon trainer
        player->SEND_POI(-4215.68f, -11628.9f, 6, 6, 0, "Weapon Master Handiir");
        player->SEND_GOSSIP_MENU(9565, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Battlemaster
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALTERACVALLEY       , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ARATHIBASIN         , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ARENA               , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_EYEOFTHESTORM       , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARSONGULCH         , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 5);
        player->SEND_GOSSIP_MENU(9531, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Class trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_DRUID               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HUNTER              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAGE                , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PALADIN             , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PRIEST              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SHAMAN              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARRIOR             , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->SEND_GOSSIP_MENU(9533, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Profession trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BLACKSMITHING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENCHANTING          , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENGINEERING         , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FISHING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_JEWELCRAFTING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HERBALISM           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 9);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING      , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 10);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MINING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 11);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 12);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAILORING           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 13);
        player->SEND_GOSSIP_MENU(9555, pCreature->GetGUID());
        break;
    }
}

void SendBattleMasterMenu_guard_exodar(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //AV
        player->SEND_POI(-3978.1f, -11357, 6, 6, 0, "Alterac Valley Battlemaster");
        player->SEND_GOSSIP_MENU(9531, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //AB
        player->SEND_POI(-3998.9f, -11345.2f, 6, 6, 0, "Arathi Basin Battlemaster");
        player->SEND_GOSSIP_MENU(9531,  pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //A
        player->SEND_POI(-3759.27f, -11695.63f, 6, 6, 0, "Miglik Blotstrom");
        player->SEND_GOSSIP_MENU(10223, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //EOS
        player->SEND_POI(-3978.1f, -11357, 6, 6, 0, "Eye Of The Storm Battlemaster");
        player->SEND_GOSSIP_MENU(9531, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //WSG
        player->SEND_POI(-3977.5f, -11381.2f, 6, 6, 0, "Warsong Gulch Battlemaster");
        player->SEND_GOSSIP_MENU(9531, pCreature->GetGUID());
        break;
    }
}

void SendClassTrainerMenu_guard_exodar(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Druid
        player->SEND_POI(-4276.0f, -11495, 6, 6, 0, "Exodar Druid Trainer");
        player->SEND_GOSSIP_MENU(9534, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Hunter
        player->SEND_POI(-4210.6f, -11575.2f, 6, 6, 0, "Exodar Hunter Trainer");
        player->SEND_GOSSIP_MENU(9544, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Mage
        player->SEND_POI(-4057.32f, -11556.5f, 6, 6, 0, "Exodar Mage Trainer");
        player->SEND_GOSSIP_MENU(9550, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Paladin
        player->SEND_POI(-4191.2f, -11470.4f, 6, 6, 0, "Exodar Paladin Trainer");
        player->SEND_GOSSIP_MENU(9553, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Priest
        player->SEND_POI(-3969.63f, -11482.8f, 6, 6, 0, "Exodar Priest Trainer");
        player->SEND_GOSSIP_MENU(9554, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Shaman
        player->SEND_POI(-3805.5f, -11380.7f, 6, 6, 0, "Exodar Shaman Trainer");
        player->SEND_GOSSIP_MENU(9556, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Warrior
        player->SEND_POI(-4189.43f, -11653.7f, 6, 6, 0, "Exodar Warrior Trainer");
        player->SEND_GOSSIP_MENU(9562, pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_exodar(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_POI(-4040.6f, -11364.5f, 6, 6, 0, "Exodar Alchemy Trainer");
        player->SEND_GOSSIP_MENU(9527, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Blacksmithing
        player->SEND_POI(-4229.5f, -11706, 6, 6, 0, "Exodar Blacksmithing Trainer");
        player->SEND_GOSSIP_MENU(9532, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Cooking
        player->SEND_POI(-3798.3f, -11651.7f, 6, 6, 0, "Exodar Cooking Trainer");
        player->SEND_GOSSIP_MENU(9551, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Enchanting
        player->SEND_POI(-3889.3f, -11495, 6, 6, 0, "Exodar Enchanting Trainer");
        player->SEND_GOSSIP_MENU(9535, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Engineering
        player->SEND_POI(-4257.68f, -11640.3f, 6, 6, 0, "Exodar Engineering Trainer");
        player->SEND_GOSSIP_MENU(9536, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //First Aid
        player->SEND_POI(-3769.5f, -11479.6f, 6, 6, 0, "Exodar First Aid Trainer");
        player->SEND_GOSSIP_MENU(9537, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Fishing
        player->SEND_POI(-3725.5f, -11385.2f, 6, 6, 0, "Exodar Fishing Trainer");
        player->SEND_GOSSIP_MENU(9538, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Jewelcrafting
        player->SEND_POI(-3783, -11546, 6, 6, 0, "Exodar Jewelcrafting Trainer");
        player->SEND_GOSSIP_MENU(9547, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Herbalism
        player->SEND_POI(-4040.6f, -11364.5f, 6, 6, 0, "Exodar Herbalist Trainer");
        player->SEND_GOSSIP_MENU(9543, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Leatherworking
        player->SEND_POI(-4140.6f, -11776.7f, 6, 6, 0, "Exodar Leatherworking Trainer");
        player->SEND_GOSSIP_MENU(9549, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Mining
        player->SEND_POI(-4228, -11697, 6, 6, 0, "Exodar Mining Trainer");
        player->SEND_GOSSIP_MENU(9552, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 12:                   //Skinning
        player->SEND_POI(-4134.97f, -11760.5f, 6, 6, 0, "Exodar Skinning Trainer");
        player->SEND_GOSSIP_MENU(9557, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 13:                   //Tailoring
        player->SEND_POI(-4092.5f, -11744.5f, 6, 6, 0, "Exodar Tailor Trainer");
        player->SEND_GOSSIP_MENU(9559, pCreature->GetGUID());
        break;
    }
}

bool GossipSelect_guard_exodar(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_exodar(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_CLASSTRAIN:
        SendClassTrainerMenu_guard_exodar(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_exodar(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_BATTLEINFO:
        SendBattleMasterMenu_guard_exodar(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_exodar end
 *******************************************************/

CreatureAI* GetAI_guard_exodar(Creature* pCreature)
{
    return new guardAI (pCreature);
}

/*******************************************************
 * guard_ironforge start
 *******************************************************/

bool GossipHello_guard_ironforge(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_AUCTIONHOUSE      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_IRONFORGE_BANK    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_DEEPRUNTRAM       , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GRYPHON           , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GUILDMASTER       , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN               , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAILBOX           , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WEAPONMASTER      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTER      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_CLASSTRAINER      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER       , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
    player->SEND_GOSSIP_MENU(2760, pCreature->GetGUID());
    return true;
}

void SendDefaultMenu_guard_ironforge(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Auction House
        player->SEND_POI(-4957.39f, -911.6f, 6, 6, 0, "Ironforge Auction House");
        player->SEND_GOSSIP_MENU(3014, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Bank
        player->SEND_POI(-4891.91f, -991.47f, 6, 6, 0, "The Vault");
        player->SEND_GOSSIP_MENU(2761, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Tram
        player->SEND_POI(-4835.27f, -1294.69f, 6, 6, 0, "Deeprun Tram");
        player->SEND_GOSSIP_MENU(3814, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Gryphon Master
        player->SEND_POI(-4821.52f, -1152.3f, 6, 6, 0, "Ironforge Gryphon Master");
        player->SEND_GOSSIP_MENU(2762, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Guild Master
        player->SEND_POI(-5021, -996.45f, 6, 6, 0, "Ironforge Visitor's Center");
        player->SEND_GOSSIP_MENU(2764, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Inn
        player->SEND_POI(-4850.47f, -872.57f, 6, 6, 0, "Stonefire Tavern");
        player->SEND_GOSSIP_MENU(2768, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Mailbox
        player->SEND_POI(-4845.7f, -880.55f, 6, 6, 0, "Ironforge Mailbox");
        player->SEND_GOSSIP_MENU(2769, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Stable Master
        player->SEND_POI(-5010.2f, -1262, 6, 6, 0, "Ulbrek Firehand");
        player->SEND_GOSSIP_MENU(5986, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Weapons Trainer
        player->SEND_POI(-5040, -1201.88f, 6, 6, 0, "Bixi and Buliwyf");
        player->SEND_GOSSIP_MENU(4518, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Battlemaster
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALTERACVALLEY   , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ARATHIBASIN     , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARSONGULCH     , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 3);
        player->SEND_GOSSIP_MENU(7529, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Class Trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HUNTER          , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAGE            , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PALADIN         , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PRIEST          , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ROGUE           , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARLOCK         , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARRIOR         , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SHAMAN          , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->SEND_GOSSIP_MENU(2766, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 12:                   //Profession Trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY         , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BLACKSMITHING   , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING         , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENCHANTING      , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENGINEERING     , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FISHING         , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HERBALISM       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING  , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 9);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MINING          , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 10);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 11);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAILORING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 12);
        player->SEND_GOSSIP_MENU(2793, pCreature->GetGUID());
        break;
    }
}

void SendBattleMasterMenu_guard_ironforge(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //AV
        player->SEND_POI(-5047.87f, -1263.77f, 6, 6, 0, "Glordrum Steelbeard");
        player->SEND_GOSSIP_MENU(7483, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //AB
        player->SEND_POI(-5038.37f, -1266.39f, 6, 6, 0, "Donal Osgood");
        player->SEND_GOSSIP_MENU(7649, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //WSG
        player->SEND_POI(-5037.24f, -1274.82f, 6, 6, 0, "Lylandris");
        player->SEND_GOSSIP_MENU(7528, pCreature->GetGUID());
        break;
    }
}

void SendClassTrainerMenu_guard_ironforge(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Hunter
        player->SEND_POI(-5023, -1253.68f, 6, 6, 0, "Hall of Arms");
        player->SEND_GOSSIP_MENU(2770, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Mage
        player->SEND_POI(-4627, -926.45f, 6, 6, 0, "Hall of Mysteries");
        player->SEND_GOSSIP_MENU(2771, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Paladin
        player->SEND_POI(-4627.02f, -926.45f, 6, 6, 0, "Hall of Mysteries");
        player->SEND_GOSSIP_MENU(2773, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Priest
        player->SEND_POI(-4627, -926.45f, 6, 6, 0, "Hall of Mysteries");
        player->SEND_GOSSIP_MENU(2772, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Rogue
        player->SEND_POI(-4647.83f, -1124, 6, 6, 0, "Ironforge Rogue Trainer");
        player->SEND_GOSSIP_MENU(2774, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Warlock
        player->SEND_POI(-4605, -1110.45f, 6, 6, 0, "Ironforge Warlock Trainer");
        player->SEND_GOSSIP_MENU(2775, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Warrior
        player->SEND_POI(-5023.08f, -1253.68f, 6, 6, 0, "Hall of Arms");
        player->SEND_GOSSIP_MENU(2776, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Shaman
        player->SEND_POI(-4732, -1147, 6, 6, 0, "Ironforge Shaman Trainer");
        //incorrect id
        player->SEND_GOSSIP_MENU(2766, pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_ironforge(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_POI(-4858.5f, -1241.83f, 6, 6, 0, "Berryfizz's Potions and Mixed Drinks");
        player->SEND_GOSSIP_MENU(2794, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Blacksmithing
        player->SEND_POI(-4796.97f, -1110.17f, 6, 6, 0, "The Great Forge");
        player->SEND_GOSSIP_MENU(2795, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Cooking
        player->SEND_POI(-4767.83f, -1184.59f, 6, 6, 0, "The Bronze Kettle");
        player->SEND_GOSSIP_MENU(2796, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Enchanting
        player->SEND_POI(-4803.72f, -1196.53f, 6, 6, 0, "Thistlefuzz Arcanery");
        player->SEND_GOSSIP_MENU(2797, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Engineering
        player->SEND_POI(-4799.56f, -1250.23f, 6, 6, 0, "Springspindle's Gadgets");
        player->SEND_GOSSIP_MENU(2798, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //First Aid
        player->SEND_POI(-4881.6f, -1153.13f, 6, 6, 0, "Ironforge Physician");
        player->SEND_GOSSIP_MENU(2799, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Fishing
        player->SEND_POI(-4597.91f, -1091.93f, 6, 6, 0, "Traveling Fisherman");
        player->SEND_GOSSIP_MENU(2800, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Herbalism
        player->SEND_POI(-4876.9f, -1151.92f, 6, 6, 0, "Ironforge Physician");
        player->SEND_GOSSIP_MENU(2801, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Leatherworking
        player->SEND_POI(-4745, -1027.57f, 6, 6, 0, "Finespindle's Leather Goods");
        player->SEND_GOSSIP_MENU(2802, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Minning
        player->SEND_POI(-4705.06f, -1116.43f, 6, 6, 0, "Deepmountain Mining Guild");
        player->SEND_GOSSIP_MENU(2804, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Skinning
        player->SEND_POI(-4745, -1027.57f, 6, 6, 0, "Finespindle's Leather Goods");
        player->SEND_GOSSIP_MENU(2805, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 12:                   //Tailoring
        player->SEND_POI(-4719.60f, -1056.96f, 6, 6, 0, "Stonebrow's Clothier");
        player->SEND_GOSSIP_MENU(2807, pCreature->GetGUID());
        break;
    }
}

bool GossipSelect_guard_ironforge(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_ironforge(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_CLASSTRAIN:
        SendClassTrainerMenu_guard_ironforge(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_ironforge(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_BATTLEINFO:
        SendBattleMasterMenu_guard_ironforge(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_ironforge end
 *******************************************************/

CreatureAI* GetAI_guard_ironforge(Creature* pCreature)
{
    return new guardAI (pCreature);
}

/*******************************************************
 * guard_mulgore start
 *******************************************************/

bool GossipHello_guard_mulgore(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANK         , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WINDRIDER    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN          , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_CLASSTRAINER , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->SEND_GOSSIP_MENU(3543, pCreature->GetGUID());
    return true;
}

void SendDefaultMenu_guard_mulgore(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Bank
        player->SEND_GOSSIP_MENU(4051, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Wind rider
        player->SEND_GOSSIP_MENU(4052, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Inn
        player->SEND_POI(-2361.38f, -349.19f, 6, 6, 0, "Bloodhoof Village Inn");
        player->SEND_GOSSIP_MENU(4053, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Stable master
        player->SEND_POI(-2338.86f, -357.56f, 6, 6, 0, "Seikwa");
        player->SEND_GOSSIP_MENU(5976, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Class trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_DRUID           , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HUNTER          , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SHAMAN          , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARRIOR         , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->SEND_GOSSIP_MENU(4069, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Profession trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY         , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BLACKSMITHING   , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING         , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENCHANTING      , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FISHING         , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HERBALISM       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING  , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MINING          , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 9);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 10);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAILORING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 11);
        player->SEND_GOSSIP_MENU(4070, pCreature->GetGUID());
        break;
    }
}

void SendClassTrainerMenu_guard_mulgore(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Druid
        player->SEND_POI(-2312.15f, -443.69f, 6, 6, 0, "Gennia Runetotem");
        player->SEND_GOSSIP_MENU(4054, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Hunter
        player->SEND_POI(-2178.14f, -406.14f, 6, 6, 0, "Yaw Sharpmane");
        player->SEND_GOSSIP_MENU(4055, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Shaman
        player->SEND_POI(-2301.5f, -439.87f, 6, 6, 0, "Narm Skychaser");
        player->SEND_GOSSIP_MENU(4056, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Warrior
        player->SEND_POI(-2345.43f, -494.11f, 6, 6, 0, "Krang Stonehoof");
        player->SEND_GOSSIP_MENU(4057, pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_mulgore(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_GOSSIP_MENU(4058, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Blacksmithing
        player->SEND_GOSSIP_MENU(4059, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Cooking
        player->SEND_POI(-2263.34f, -287.91f, 6, 6, 0, "Pyall Silentstride");
        player->SEND_GOSSIP_MENU(4060, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Enchanting
        player->SEND_GOSSIP_MENU(4061, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //First Aid
        player->SEND_POI(-2353.52f, -355.82f, 6, 6, 0, "Vira Younghoof");
        player->SEND_GOSSIP_MENU(4062, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Fishing
        player->SEND_POI(-2349.21f, -241.37f, 6, 6, 0, "Uthan Stillwater");
        player->SEND_GOSSIP_MENU(4063, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Herbalism
        player->SEND_GOSSIP_MENU(4064, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Leatherworking
        player->SEND_POI(-2257.12f, -288.63f, 6, 6, 0, "Chaw Stronghide");
        player->SEND_GOSSIP_MENU(4065, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Mining
        player->SEND_GOSSIP_MENU(4066, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Skinning
        player->SEND_POI(-2252.94f, -291.32f, 6, 6, 0, "Yonn Deepcut");
        player->SEND_GOSSIP_MENU(4067, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Tailoring
        player->SEND_GOSSIP_MENU(4068, pCreature->GetGUID());
        break;
    }
}

bool GossipSelect_guard_mulgore(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_mulgore(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_CLASSTRAIN:
        SendClassTrainerMenu_guard_mulgore(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_mulgore(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_mulgore end
 *******************************************************/

CreatureAI* GetAI_guard_mulgore(Creature* pCreature)
{
    return new guardAI (pCreature);
}

/*******************************************************
 * guard_orgrimmar start
 *******************************************************/

bool GossipHello_guard_orgrimmar(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANK          , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WINDRIDER     , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GUILDMASTER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN           , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAILBOX       , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_AUCTIONHOUSE  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ZEPPLINMASTER , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WEAPONMASTER  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_OFFICERS      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTER  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_CLASSTRAINER  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
    player->SEND_GOSSIP_MENU(2593, pCreature->GetGUID());

    return true;
}

void SendDefaultMenu_guard_orgrimmar(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Bank
        player->SEND_POI(1631.51f, -4375.33f, 6, 6, 0, "Bank of Orgrimmar");
        player->SEND_GOSSIP_MENU(2554, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //wind rider
        player->SEND_POI(1676.6f, -4332.72f, 6, 6, 0, "The Sky Tower");
        player->SEND_GOSSIP_MENU(2555, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //guild master
        player->SEND_POI(1576.93f, -4294.75f, 6, 6, 0, "Horde Embassy");
        player->SEND_GOSSIP_MENU(2556, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Inn
        player->SEND_POI(1644.51f, -4447.27f, 6, 6, 0, "Orgrimmar Inn");
        player->SEND_GOSSIP_MENU(2557, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //mailbox
        player->SEND_POI(1622.53f, -4388.79f, 6, 6, 0, "Orgrimmar Mailbox");
        player->SEND_GOSSIP_MENU(2558, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //auction house
        player->SEND_POI(1679.21f, -4450.1f, 6, 6, 0, "Orgrimmar Auction House");
        player->SEND_GOSSIP_MENU(3075, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //zeppelin
        player->SEND_POI(1337.36f, -4632.7f, 6, 6, 0, "Orgrimmar Zeppelin Tower");
        player->SEND_GOSSIP_MENU(3173, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //weapon master
        player->SEND_POI(2092.56f, -4823.95f, 6, 6, 0, "Sayoc & Hanashi");
        player->SEND_GOSSIP_MENU(4519, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //stable master
        player->SEND_POI(2133.12f, -4663.93f, 6, 6, 0, "Xon'cha");
        player->SEND_GOSSIP_MENU(5974, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //officers lounge
        player->SEND_POI(1633.56f, -4249.37f, 6, 6, 0, "Hall of Legends");
        player->SEND_GOSSIP_MENU(7046, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //battlemaster
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALTERACVALLEY       , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ARATHIBASIN         , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARSONGULCH         , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 3);
        player->SEND_GOSSIP_MENU(7521, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 12:                   //class trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HUNTER              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAGE                , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PRIEST              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SHAMAN              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ROGUE               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARLOCK             , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARRIOR             , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PALADIN             , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->SEND_GOSSIP_MENU(2599, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 13:                   //profession trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BLACKSMITHING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENCHANTING          , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENGINEERING         , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FISHING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HERBALISM           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING      , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 9);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MINING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 10);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 11);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAILORING           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 12);
        player->SEND_GOSSIP_MENU(2594, pCreature->GetGUID());
        break;
    }
}

void SendBattleMasterMenu_guard_orgrimmar(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //AV
        player->SEND_POI(1983.92f, -4794.2f, 6, 6, 0, "Hall of the Brave");
        player->SEND_GOSSIP_MENU(7484, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //AB
        player->SEND_POI(1983.92f, -4794.2f, 6, 6, 0, "Hall of the Brave");
        player->SEND_GOSSIP_MENU(7644, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //WSG
        player->SEND_POI(1983.92f, -4794.2f, 6, 6, 0, "Hall of the Brave");
        player->SEND_GOSSIP_MENU(7520, pCreature->GetGUID());
        break;
    }
}

void SendClassTrainerMenu_guard_orgrimmar(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Hunter
        player->SEND_POI(2114.84f, -4625.31f, 6, 6, 0, "Orgrimmar Hunter's Hall");
        player->SEND_GOSSIP_MENU(2559, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Mage
        player->SEND_POI(1451.26f, -4223.33f, 6, 6, 0, "Darkbriar Lodge");
        player->SEND_GOSSIP_MENU(2560, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Priest
        player->SEND_POI(1442.21f, -4183.24f, 6, 6, 0, "Spirit Lodge");
        player->SEND_GOSSIP_MENU(2561, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Shaman
        player->SEND_POI(1925.34f, -4181.89f, 6, 6, 0, "Thrall's Fortress");
        player->SEND_GOSSIP_MENU(2562, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Rogue
        player->SEND_POI(1773.39f, -4278.97f, 6, 6, 0, "Shadowswift Brotherhood");
        player->SEND_GOSSIP_MENU(2563, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Warlock
        player->SEND_POI(1849.57f, -4359.68f, 6, 6, 0, "Darkfire Enclave");
        player->SEND_GOSSIP_MENU(2564, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Warrior
        player->SEND_POI(1983.92f, -4794.2f, 6, 6, 0, "Hall of the Brave");
        player->SEND_GOSSIP_MENU(2565, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Paladin
        player->SEND_POI(1906.65f, -4134.26f, 6, 6, 0, "Valley of Wisdom");
        player->SEND_GOSSIP_MENU(10843, pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_orgrimmar(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_POI(1955.17f, -4475.79f, 6, 6, 0, "Yelmak's Alchemy and Potions");
        player->SEND_GOSSIP_MENU(2497, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Blacksmithing
        player->SEND_POI(2054.34f, -4831.85f, 6, 6, 0, "The Burning Anvil");
        player->SEND_GOSSIP_MENU(2499, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Cooking
        player->SEND_POI(1780.96f, -4481.31f, 6, 6, 0, "Borstan's Firepit");
        player->SEND_GOSSIP_MENU(2500, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Enchanting
        player->SEND_POI(1917.5f, -4434.95f, 6, 6, 0, "Godan's Runeworks");
        player->SEND_GOSSIP_MENU(2501, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Engineering
        player->SEND_POI(2038.45f, -4744.75f, 6, 6, 0, "Nogg's Machine Shop");
        player->SEND_GOSSIP_MENU(2653, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //First Aid
        player->SEND_POI(1485.21f, -4160.91f, 6, 6, 0, "Survival of the Fittest");
        player->SEND_GOSSIP_MENU(2502, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Fishing
        player->SEND_POI(1994.15f, -4655.7f, 6, 6, 0, "Lumak's Fishing");
        player->SEND_GOSSIP_MENU(2503, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Herbalism
        player->SEND_POI(1898.61f, -4454.93f, 6, 6, 0, "Jandi's Arboretum");
        player->SEND_GOSSIP_MENU(2504, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Leatherworking
        player->SEND_POI(1852.82f, -4562.31f, 6, 6, 0, "Kodohide Leatherworkers");
        player->SEND_GOSSIP_MENU(2513, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Mining
        player->SEND_POI(2029.79f, -4704, 6, 6, 0, "Red Canyon Mining");
        player->SEND_GOSSIP_MENU(2515, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Skinning
        player->SEND_POI(1852.82f, -4562.31f, 6, 6, 0, "Kodohide Leatherworkers");
        player->SEND_GOSSIP_MENU(2516, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 12:                   //Tailoring
        player->SEND_POI(1802.66f, -4560.66f, 6, 6, 0, "Magar's Cloth Goods");
        player->SEND_GOSSIP_MENU(2518, pCreature->GetGUID());
        break;
    }
}

bool GossipSelect_guard_orgrimmar(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_orgrimmar(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_CLASSTRAIN:
        SendClassTrainerMenu_guard_orgrimmar(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_orgrimmar(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_BATTLEINFO:
        SendBattleMasterMenu_guard_orgrimmar(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_orgrimmar end
 *******************************************************/

CreatureAI* GetAI_guard_orgrimmar(Creature* pCreature)
{
    return new guardAI_orgrimmar (pCreature);
}

/*******************************************************
 * guard_shattrath start
 *******************************************************/

bool GossipHello_guard_shattrath(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAVERN                       , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANK                         , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN                          , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FLIGHTMASTER                 , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAILBOX                      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER                 , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTER                 , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER                  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MANALOOM                     , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMYLAB                   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GEMMERCHANT                  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
    player->SEND_GOSSIP_MENU(10321, pCreature->GetGUID());

    return true;
}

void SendDefaultMenu_guard_shattrath(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Tavern
        player->SEND_POI(-1759.5f, 5165, 6, 6, 0, "Worlds End Tavern");
        player->SEND_GOSSIP_MENU(10394, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Bank
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANKALDOR            , GOSSIP_SENDER_SEC_BANK, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANKSCYERS           , GOSSIP_SENDER_SEC_BANK, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(10379, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Inn
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INNALDOR             , GOSSIP_SENDER_SEC_INN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INNSCYERS            , GOSSIP_SENDER_SEC_INN, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(10382, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Flight master
        player->SEND_POI(-1832, 5299, 6, 6, 0, "Flight Master");
        player->SEND_GOSSIP_MENU(10385, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Mailbox
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANKALDOR           , GOSSIP_SENDER_SEC_MAILBOX, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INNALDOR            , GOSSIP_SENDER_SEC_MAILBOX, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANKSCYERS          , GOSSIP_SENDER_SEC_MAILBOX, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INNSCYERS           , GOSSIP_SENDER_SEC_MAILBOX, GOSSIP_ACTION_INFO_DEF + 4);
        player->SEND_GOSSIP_MENU(10386, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Stable master
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEALDOR          , GOSSIP_SENDER_SEC_STABLEMASTER, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLESCYERS         , GOSSIP_SENDER_SEC_STABLEMASTER, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(10387, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Battlemaster
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTERALLIANCE , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTERHORDE    , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTERARENA    , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 3);
        player->SEND_GOSSIP_MENU(10388, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Profession master
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BLACKSMITHING        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENCHANTING           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_JEWELCRAFTING        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->SEND_GOSSIP_MENU(10391, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Mana Loom
        player->SEND_POI(-2070, 5265.5f, 6, 6, 0, "Mana Loom");
        player->SEND_GOSSIP_MENU(10503, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Alchemy Lab
        player->SEND_POI(-1648.5f, 5540, 6, 6, 0, "Alchemy Lab");
        player->SEND_GOSSIP_MENU(10321, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Gem Merchant
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GEMALDOR             , GOSSIP_SENDER_SEC_GEMMERCHANT, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GEMSCYERS            , GOSSIP_SENDER_SEC_GEMMERCHANT, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(10697, pCreature->GetGUID());
        break;
    }
}

void SendBankMenu_guard_shattrath(Player* player, Creature* pCreature, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        player->SEND_POI(-1730.5f, 5496, 6, 6, 0, "Aldor Bank");
        player->SEND_GOSSIP_MENU(10380,  pCreature->GetGUID());
    }
    if (action == GOSSIP_ACTION_INFO_DEF + 2)
    {
        player->SEND_POI(-1997.7f, 5363, 6, 6, 0, "Scyers Bank");
        player->SEND_GOSSIP_MENU(10381, pCreature->GetGUID());
    }
}

void SendInnMenu_guard_shattrath(Player* player, Creature* pCreature, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        player->SEND_POI(-1895, 5767, 6, 6, 0, "Aldor Inn");
        player->SEND_GOSSIP_MENU(10383,  pCreature->GetGUID());
    }
    if (action == GOSSIP_ACTION_INFO_DEF + 2)
    {
        player->SEND_POI(-2178, 5405, 6, 6, 0, "Scyers Inn");
        player->SEND_GOSSIP_MENU(10384, pCreature->GetGUID());
    }
}

void SendMailboxMenu_guard_shattrath(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:
        player->SEND_POI(-1730.5f, 5496, 6, 6, 0, "Aldor Bank");
        player->SEND_GOSSIP_MENU(10380,  pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:
        player->SEND_POI(-1895, 5767, 6, 6, 0, "Aldor Inn");
        player->SEND_GOSSIP_MENU(10383,  pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:
        player->SEND_POI(-1997.7f, 5363, 6, 6, 0, "Scyers Bank");
        player->SEND_GOSSIP_MENU(10381, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:
        player->SEND_POI(-2178, 5405, 6, 6, 0, "Scyers Inn");
        player->SEND_GOSSIP_MENU(10384, pCreature->GetGUID());
        break;
    }
}

void SendStableMasterMenu_guard_shattrath(Player* player, Creature* pCreature, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        player->SEND_POI(-1888.5f, 5761, 6, 6, 0, "Aldor Stable");
        player->SEND_GOSSIP_MENU(10321,  pCreature->GetGUID());
    }
    if (action == GOSSIP_ACTION_INFO_DEF + 2)
    {
        player->SEND_POI(-2170, 5404, 6, 6, 0, "Scyers Stable");
        player->SEND_GOSSIP_MENU(10321, pCreature->GetGUID());
    }
}

void SendBattleMasterMenu_guard_shattrath(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:
        player->SEND_POI(-1774, 5251, 6, 6, 0, "Alliance Battlemasters");
        player->SEND_GOSSIP_MENU(10389, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:
        player->SEND_POI(-1963, 5263, 6, 6, 0, "Horde Battlemasters");
        player->SEND_GOSSIP_MENU(10390,  pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:
        player->SEND_POI(-1960, 5175, 6, 6, 0, "Arena Battlemasters");
        player->SEND_GOSSIP_MENU(12510,  pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_shattrath(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_POI(-1648.5f, 5534, 6, 6, 0, "Lorokeem");
        player->SEND_GOSSIP_MENU(10392, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Blacksmithing
        player->SEND_POI(-1847, 5222, 6, 6, 0, "Kradu Grimblade and Zula Slagfury");
        player->SEND_GOSSIP_MENU(10400, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Cooking
        player->SEND_POI(-2067.4f, 5316.5f, 6, 6, 0, "Jack Trapper");
        player->SEND_GOSSIP_MENU(10393, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Enchanting
        player->SEND_POI(-2263.5f, 5563.5f, 6, 6, 0, "High Enchanter Bardolan");
        player->SEND_GOSSIP_MENU(10395, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //First Aid
        player->SEND_POI(-1591, 5265.5f, 6, 6, 0, "Mildred Fletcher");
        player->SEND_GOSSIP_MENU(10396, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Jewelcrafting
        player->SEND_POI(-1654, 5667.5f, 6, 6, 0, "Hamanar");
        player->SEND_GOSSIP_MENU(10397, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Leatherworking
        player->SEND_POI(-2060.5f, 5256.5f, 6, 6, 0, "Darmari");
        player->SEND_GOSSIP_MENU(10399, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Skinning
        player->SEND_POI(-2048, 5300, 6, 6, 0, "Seymour");
        player->SEND_GOSSIP_MENU(10398, pCreature->GetGUID());
        break;
    }
}

void SendGemMerchantMenu_guard_shattrath(Player* player, Creature* pCreature, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        player->SEND_POI(-1645, 5669.5f, 6, 6, 0, "Aldor Gem Merchant");
        player->SEND_GOSSIP_MENU(10698,  pCreature->GetGUID());
    }
    if (action == GOSSIP_ACTION_INFO_DEF + 2)
    {
        player->SEND_POI(-2193, 5424.5f, 6, 6, 0, "Scyers Gem Merchant");
        player->SEND_GOSSIP_MENU(10699, pCreature->GetGUID());
    }
}

bool GossipSelect_guard_shattrath(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_shattrath(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_BANK:
        SendBankMenu_guard_shattrath(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_INN:
        SendInnMenu_guard_shattrath(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_STABLEMASTER:
        SendStableMasterMenu_guard_shattrath(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_GEMMERCHANT:
        SendGemMerchantMenu_guard_shattrath(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_MAILBOX:
        SendMailboxMenu_guard_shattrath(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_shattrath(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_BATTLEINFO:
        SendBattleMasterMenu_guard_shattrath(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_shattrath end
 *******************************************************/

CreatureAI* GetAI_guard_shattrath(Creature* pCreature)
{
    return new guardAI (pCreature);
}

/*******************************************************
 * guard_shattrath_aldor
 *******************************************************/

#define SPELL_BANISHED_SHATTRATH_A  36642
#define SPELL_BANISHED_SHATTRATH_S  36671
#define SPELL_BANISH_TELEPORT       36643
#define SPELL_EXILE                 39533

struct guard_shattrath_aldorAI : public guardAI
{
    guard_shattrath_aldorAI(Creature* c) : guardAI(c) {}

    uint32 Exile_Timer;
    uint32 Banish_Timer;
    uint64 PlayerGUID;
    bool CanTeleport;

    void Reset()
    {
        Banish_Timer = 5000;
        Exile_Timer = 8500;
        PlayerGUID = 0;
        CanTeleport = false;
    }

    void EnterCombat(Unit* /*who*/) {}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (CanTeleport)
        {
            if (Exile_Timer <= diff)
            {
                if (Unit* temp = Unit::GetUnit(*me, PlayerGUID))
                {
                    temp->CastSpell(temp, SPELL_EXILE, true);
                    temp->CastSpell(temp, SPELL_BANISH_TELEPORT, true);
                }
                PlayerGUID = 0;
                Exile_Timer = 8500;
                CanTeleport = false;
            }
            else Exile_Timer -= diff;
        }
        else if (Banish_Timer <= diff)
        {
            Unit* temp = me->GetVictim();
            if (temp && temp->GetTypeId() == TYPEID_PLAYER)
            {
                DoCast(temp, SPELL_BANISHED_SHATTRATH_A);
                Banish_Timer = 9000;
                PlayerGUID = temp->GetGUID();
                if (PlayerGUID)
                    CanTeleport = true;
            }
        }
        else Banish_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

bool GossipHello_guard_shattrath_aldor(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAVERN                       , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANK                         , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN                          , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FLIGHTMASTER                 , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAILBOX                      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER                 , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTER                 , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER                  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MANALOOM                     , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMYLAB                   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GEMMERCHANT                  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
    player->SEND_GOSSIP_MENU(10524, pCreature->GetGUID());
    return true;
}

void SendDefaultMenu_guard_shattrath_aldor(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Tavern
        player->SEND_POI(-1759.5, 5165, 6, 6, 0, "Worlds End Tavern");
        player->SEND_GOSSIP_MENU(10394, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Bank
        player->SEND_POI(-1730.5, 5496, 6, 6, 0, "Aldor Bank");
        player->SEND_GOSSIP_MENU(10380, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Inn
        player->SEND_POI(-1895, 5767, 6, 6, 0, "Aldor Inn");
        player->SEND_GOSSIP_MENU(10525,  pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Flight master
        player->SEND_POI(-1832, 5299, 6, 6, 0, "Shattrath Flight Master");
        player->SEND_GOSSIP_MENU(10402, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Mailbox
        player->SEND_POI(0, 0, 6, 6, 0, "Aldor Mailbox");
        //unknown
        player->SEND_GOSSIP_MENU(10524, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Stable master
        player->SEND_POI(-1888.5, 5761, 6, 6, 0, "Aldor Stable Master");
        player->SEND_GOSSIP_MENU(10527, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Battlemaster
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTERALLIANCE , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTERHORDE    , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTERARENA    , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 3);
        player->SEND_GOSSIP_MENU(10388, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Profession master
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BLACKSMITHING        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENCHANTING           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_JEWELCRAFTING        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->SEND_GOSSIP_MENU(10391, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Mana Loom
        player->SEND_POI(-2070, 5265.5, 6, 6, 0, "Mana Loom");
        player->SEND_GOSSIP_MENU(10522, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Alchemy Lab
        player->SEND_POI(-1648.5, 5540, 6, 6, 0, "Alchemy Lab");
        player->SEND_GOSSIP_MENU(10696, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Gem Merchant
        player->SEND_POI(-1645, 5669.5, 6, 6, 0, "Aldor Gem Merchant");
        player->SEND_GOSSIP_MENU(10411, pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_shattrath_aldor(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_POI(-1648.5f, 5534, 6, 6, 0, "Lorokeem");
        player->SEND_GOSSIP_MENU(10392, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Blacksmithing
        player->SEND_POI(-1847, 5222, 6, 6, 0, "Kradu Grimblade and Zula Slagfury");
        player->SEND_GOSSIP_MENU(10400, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Cooking
        player->SEND_POI(-2067.4f, 5316.5f, 6, 6, 0, "Jack Trapper");
        player->SEND_GOSSIP_MENU(10393, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Enchanting
        player->SEND_POI(-2263.5f, 5563.5f, 6, 6, 0, "High Enchanter Bardolan");
        player->SEND_GOSSIP_MENU(10528, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //First Aid
        player->SEND_POI(-1591, 5265.5f, 6, 6, 0, "Mildred Fletcher");
        player->SEND_GOSSIP_MENU(10396, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Jewelcrafting
        player->SEND_POI(-1654, 5667.5f, 6, 6, 0, "Hamanar");
        player->SEND_GOSSIP_MENU(10529, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Leatherworking
        player->SEND_POI(-2060.5f, 5256.5f, 6, 6, 0, "Darmari");
        player->SEND_GOSSIP_MENU(10399, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Skinning
        player->SEND_POI(-2048, 5300, 6, 6, 0, "Seymour");
        player->SEND_GOSSIP_MENU(10419, pCreature->GetGUID());
        break;
    }
}

bool GossipSelect_guard_shattrath_aldor(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_shattrath_aldor(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_shattrath_aldor(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_BATTLEINFO:
        SendBattleMasterMenu_guard_shattrath(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_shattrath_aldor end
 *******************************************************/

CreatureAI* GetAI_guard_shattrath_aldor(Creature* pCreature)
{
    return new guard_shattrath_aldorAI (pCreature);
}

/*******************************************************
 * guard_shattrath_scryer
 *******************************************************/

struct guard_shattrath_scryerAI : public guardAI
{
    guard_shattrath_scryerAI(Creature* c) : guardAI(c) {}

    uint32 Exile_Timer;
    uint32 Banish_Timer;
    uint64 PlayerGUID;
    bool CanTeleport;

    void Reset()
    {
        Banish_Timer = 5000;
        Exile_Timer = 8500;
        PlayerGUID = 0;
        CanTeleport = false;
    }

    void EnterCombat(Unit* /*who*/) {}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (CanTeleport)
        {
            if (Exile_Timer <= diff)
            {
                if (Unit* temp = Unit::GetUnit(*me, PlayerGUID))
                {
                    temp->CastSpell(temp, SPELL_EXILE, true);
                    temp->CastSpell(temp, SPELL_BANISH_TELEPORT, true);
                }
                PlayerGUID = 0;
                Exile_Timer = 8500;
                CanTeleport = false;
            }
            else Exile_Timer -= diff;
        }
        else if (Banish_Timer <= diff)
        {
            Unit* temp = me->GetVictim();
            if (temp && temp->GetTypeId() == TYPEID_PLAYER)
            {
                DoCast(temp, SPELL_BANISHED_SHATTRATH_S);
                Banish_Timer = 9000;
                PlayerGUID = temp->GetGUID();
                if (PlayerGUID)
                    CanTeleport = true;
            }
        }
        else Banish_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

bool GossipHello_guard_shattrath_scryer(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAVERN                       , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANK                         , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN                          , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FLIGHTMASTER                 , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAILBOX                      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER                 , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTER                 , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER                  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MANALOOM                     , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMYLAB                   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GEMMERCHANT                  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
    player->SEND_GOSSIP_MENU(10430, pCreature->GetGUID());
    return true;
}

void SendDefaultMenu_guard_shattrath_scryer(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Tavern
        player->SEND_POI(-1759.5f, 5165, 6, 6, 0, "Worlds End Tavern");
        player->SEND_GOSSIP_MENU(10431, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Bank
        player->SEND_POI(-1996.6f, 5363.7f, 6, 6, 0, "Scryer Bank");
        player->SEND_GOSSIP_MENU(10432, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Inn
        player->SEND_POI(-2176.6f, 5405.8f, 6, 6, 0, "Scryer Inn");
        player->SEND_GOSSIP_MENU(10433,  pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Flight master
        player->SEND_POI(-1832, 5299, 6, 6, 0, "Shattrath Flight Master");
        player->SEND_GOSSIP_MENU(10435, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Mailbox
        player->SEND_POI(-2174.3f, 5411.4f, 6, 6, 0, "Scryer Mailbox");
        player->SEND_GOSSIP_MENU(10436, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Stable master
        player->SEND_POI(-2169.9f, 5405.1f, 6, 6, 0, "Scryer Stable Master");
        player->SEND_GOSSIP_MENU(10437, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Battlemaster
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTERALLIANCE , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTERHORDE    , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTERARENA    , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 3);
        player->SEND_GOSSIP_MENU(10438, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Profession master
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BLACKSMITHING        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENCHANTING           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_JEWELCRAFTING        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->SEND_GOSSIP_MENU(10504, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Mana Loom
        player->SEND_POI(-2070, 5265.5f, 6, 6, 0, "Mana Loom");
        player->SEND_GOSSIP_MENU(10522, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Alchemy Lab
        player->SEND_POI(-1648.5f, 5540, 6, 6, 0, "Alchemy Lab");
        player->SEND_GOSSIP_MENU(10701, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Gem Merchant
        player->SEND_POI(-1645, 5669.5f, 6, 6, 0, "Scryer Gem Merchant");
        player->SEND_GOSSIP_MENU(10702, pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_shattrath_scryer(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_POI(-1648.5f, 5534, 6, 6, 0, "Lorokeem");
        player->SEND_GOSSIP_MENU(10516, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Blacksmithing
        player->SEND_POI(-1847, 5222, 6, 6, 0, "Kradu Grimblade and Zula Slagfury");
        player->SEND_GOSSIP_MENU(10517, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Cooking
        player->SEND_POI(-2067.4f, 5316.5f, 6, 6, 0, "Jack Trapper");
        player->SEND_GOSSIP_MENU(10518, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Enchanting
        player->SEND_POI(-2263.5f, 5563.5f, 6, 6, 0, "High Enchanter Bardolan");
        player->SEND_GOSSIP_MENU(10519, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //First Aid
        player->SEND_POI(-1591, 5265.5f, 6, 6, 0, "Mildred Fletcher");
        player->SEND_GOSSIP_MENU(10520, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Jewelcrafting
        player->SEND_POI(-1654, 5667.5f, 6, 6, 0, "Hamanar");
        player->SEND_GOSSIP_MENU(10521, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Leatherworking
        player->SEND_POI(-2060.5f, 5256.5f, 6, 6, 0, "Darmari");
        player->SEND_GOSSIP_MENU(10523, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Skinning
        player->SEND_POI(-2048, 5300, 6, 6, 0, "Seymour");
        player->SEND_GOSSIP_MENU(10523, pCreature->GetGUID());
        break;
    }
}

bool GossipSelect_guard_shattrath_scryer(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_shattrath_scryer(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_shattrath_scryer(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_BATTLEINFO:
        SendBattleMasterMenu_guard_shattrath(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_shattrath_scryer end
 *******************************************************/

CreatureAI* GetAI_guard_shattrath_scryer(Creature* pCreature)
{
    return new guard_shattrath_scryerAI (pCreature);
}

/*******************************************************
 * guard_silvermoon start
 *******************************************************/

bool GossipHello_guard_silvermoon(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_AUCTIONHOUSE   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANK           , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GUILDMASTER    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN            , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAILBOX        , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WEAPONMASTER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WINDRIDER      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_CLASSTRAINER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
    player->SEND_GOSSIP_MENU(9316, pCreature->GetGUID());
    return true;
}

void SendDefaultMenu_guard_silvermoon(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Auction house
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_AH_SILVERMOON_1 , GOSSIP_SENDER_SEC_AUCTIONHOUSE, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_AH_SILVERMOON_2 , GOSSIP_SENDER_SEC_AUCTIONHOUSE, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(9317, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Bank
        player->SEND_POI(9808.4f, -7488.16f, 6, 6, 0, "Silvermoon Bank");
        player->SEND_GOSSIP_MENU(9322, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Guild master
        player->SEND_POI(9474.97f, -7345.21f, 6, 6, 0, "Tandrine");
        player->SEND_GOSSIP_MENU(9324, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Inn
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN_SILVERMOON_1 , GOSSIP_SENDER_SEC_INN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN_SILVERMOON_2 , GOSSIP_SENDER_SEC_INN, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(9602, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Mailbox
        player->SEND_POI(9658.33f, -7492.17f, 6, 6, 0, "Silvermoon Mailbox");
        player->SEND_GOSSIP_MENU(9326, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Stable master
        player->SEND_POI(9904.95f, -7404.31f, 6, 6, 0, "Shalenn");
        player->SEND_GOSSIP_MENU(9327, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Weapon trainer
        player->SEND_POI(9841.17f, -7505.13f, 6, 6, 0, "Ileda");
        player->SEND_GOSSIP_MENU(9328, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Wind master
        player->SEND_POI(9378.45f, -7163.94f, 6, 6, 0, "Silvermoon Wind Master");
        player->SEND_GOSSIP_MENU(10181, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Battlemaster
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALTERACVALLEY   , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ARATHIBASIN     , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ARENA           , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_EYEOFTHESTORM   , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARSONGULCH     , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 5);
        player->SEND_GOSSIP_MENU(9329, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Class trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_DRUID                , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HUNTER               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAGE                 , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PALADIN              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PRIEST               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ROGUE                , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARLOCK              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->SEND_GOSSIP_MENU(9331, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Profession trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BLACKSMITHING        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENCHANTING           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENGINEERING          , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FISHING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_JEWELCRAFTING        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HERBALISM            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 9);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 10);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MINING               , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 11);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 12);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAILORING            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 13);
        player->SEND_GOSSIP_MENU(9338, pCreature->GetGUID());
        break;
    }
}

void SendAuctionhouseMenu_guard_silvermoon(Player* player, Creature* pCreature, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        player->SEND_POI(9644.47f, -7140.22f, 6, 6, 0, "Western Auction House");
        player->SEND_GOSSIP_MENU(9318, pCreature->GetGUID());
    }
    if (action == GOSSIP_ACTION_INFO_DEF + 2)
    {
        player->SEND_POI(9683.27f, -7521.22f, 6, 6, 0, "Royal Exchange Auction House");
        player->SEND_GOSSIP_MENU(9319, pCreature->GetGUID());
    }
}

void SendInnMenu_guard_silvermoon(Player* player, Creature* pCreature, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        player->SEND_POI(9677.7f, -7368, 6, 6, 0, "Silvermoon City Inn");
        player->SEND_GOSSIP_MENU(9325, pCreature->GetGUID());
    }
    if (action == GOSSIP_ACTION_INFO_DEF + 2)
    {
        player->SEND_POI(9561.1f, -7517.5f, 6, 6, 0, "Wayfarer's Rest tavern");
        player->SEND_GOSSIP_MENU(9603, pCreature->GetGUID());
    }
}

void SendBattleMasterMenu_guard_silvermoon(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //AV
        player->SEND_POI(9850.49f, -7572.26f, 6, 6, 0, "Gurak");
        player->SEND_GOSSIP_MENU(9329, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //AB
        player->SEND_POI(9857.18f, -7564.36f, 6, 6, 0, "Karen Wentworth");
        player->SEND_GOSSIP_MENU(9329,  pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //A
        player->SEND_POI(9850.6f, -7559.25f, 6, 6, 0, "Bipp Glizzitor");
        player->SEND_GOSSIP_MENU(9329, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //EOS
        player->SEND_POI(9857.18f, -7564.36f, 6, 6, 0, "Karen Wentworth");
        player->SEND_GOSSIP_MENU(9329, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //WSG
        player->SEND_POI(9845.45f, -7562.58f, 6, 6, 0, "Krukk");
        player->SEND_GOSSIP_MENU(9329, pCreature->GetGUID());
        break;
    }
}

void SendClassTrainerMenu_guard_silvermoon(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Druid
        player->SEND_POI(9700.55f, -7262.57f, 6, 6, 0, "Harene Plainwalker");
        player->SEND_GOSSIP_MENU(9330, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Hunter
        player->SEND_POI(9927.48f, -7426.14f, 6, 6, 0, "Zandine");
        player->SEND_GOSSIP_MENU(9332, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Mage
        player->SEND_POI(9995.07f, -7118.17f, 6, 6, 0, "Quithas");
        player->SEND_GOSSIP_MENU(9333, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Paladin
        player->SEND_POI(9850.22f, -7516.93f, 6, 6, 0, "Champion Bachi");
        player->SEND_GOSSIP_MENU(9334, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Priest
        player->SEND_POI(9926.79f, -7066.66f, 6, 6, 0, "Belestra");
        player->SEND_GOSSIP_MENU(9335, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Rogue
        player->SEND_POI(9739.88f, -7374.33f, 6, 6, 0, "Zelanis");
        player->SEND_GOSSIP_MENU(9336, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Warlock
        player->SEND_POI(9787.57f, -7284.63f, 6, 6, 0, "Alamma");
        player->SEND_GOSSIP_MENU(9337, pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_silvermoon(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_POI(9998.09f, -7214.36f, 6, 6, 0, "Silvermoon Alchemy Trainer");
        player->SEND_GOSSIP_MENU(9316, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Blacksmithing
        player->SEND_POI(9841.43f, -7361.53f, 6, 6, 0, "Silvermoon Blacksmithing Trainer");
        player->SEND_GOSSIP_MENU(9340, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Cooking
        player->SEND_POI(9577.26f, -7243.6f, 6, 6, 0, "Silvermoon Cooking Trainer");
        player->SEND_GOSSIP_MENU(9316, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Enchanting
        player->SEND_POI(9962.57f, -7246.18f, 6, 6, 0, "Silvermoon Enchanting Trainer");
        player->SEND_GOSSIP_MENU(9341, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Engineering
        player->SEND_POI(9820.18f, -7329.56f, 6, 6, 0, "Silvermoon Engineering Trainer");
        player->SEND_GOSSIP_MENU(9316, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //First Aid
        player->SEND_POI(9579.8f, -7343.71f, 6, 6, 0, "Silvermoon First Aid Trainer");
        player->SEND_GOSSIP_MENU(9316, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Fishing
        player->SEND_POI(9602.73f, -7328.3f, 6, 6, 0, "Silvermoon Fishing Trainer");
        player->SEND_GOSSIP_MENU(9316, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Jewelcrafting
        player->SEND_POI(9553.54f, -7506.43f, 6, 6, 0, "Silvermoon Jewelcrafting Trainer");
        player->SEND_GOSSIP_MENU(9346, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Herbalism
        player->SEND_POI(10004.4f, -7216.86f, 6, 6, 0, "Silvermoon Herbalism Trainer");
        player->SEND_GOSSIP_MENU(9316, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Leatherworking
        player->SEND_POI(9503.72f, -7430.16f, 6, 6, 0, "Silvermoon Leatherworking Trainer");
        player->SEND_GOSSIP_MENU(9347, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Mining
        player->SEND_POI(9805.1f, -7355.56f, 6, 6, 0, "Silvermoon Mining Trainer");
        player->SEND_GOSSIP_MENU(9348, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 12:                   //Skinning
        player->SEND_POI(9513.37f, -7429.4f, 6, 6, 0, "Silvermoon Skinning Trainer");
        player->SEND_GOSSIP_MENU(9316, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 13:                   //Tailoring
        player->SEND_POI(9750.55f, -7095.28f, 6, 6, 0, "Silvermoon Tailor");
        player->SEND_GOSSIP_MENU(9350, pCreature->GetGUID());
        break;
    }
}

bool GossipSelect_guard_silvermoon(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_silvermoon(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_AUCTIONHOUSE:
        SendAuctionhouseMenu_guard_silvermoon(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_INN:
        SendInnMenu_guard_silvermoon(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_CLASSTRAIN:
        SendClassTrainerMenu_guard_silvermoon(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_silvermoon(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_BATTLEINFO:
        SendBattleMasterMenu_guard_silvermoon(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_silvermoon end
 *******************************************************/

CreatureAI* GetAI_guard_silvermoon(Creature* pCreature)
{
    return new guardAI (pCreature);
}

/*******************************************************
 * guard_stormwind start
 *******************************************************/

bool GossipHello_guard_stormwind(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_AUCTIONHOUSE      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STORMWIND_BANK    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_DEEPRUNTRAM       , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN               , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GRYPHON           , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GUILDMASTER       , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAILBOX           , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WEAPONMASTER      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_OFFICERS          , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTER      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_CLASSTRAINER      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER       , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
    player->SEND_GOSSIP_MENU(933, pCreature->GetGUID());
    return true;
}

void SendDefaultMenu_guard_stormwind(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Auction House
        player->SEND_POI(-8811.46f, 667.46f, 6, 6, 0, "Stormwind Auction House");
        player->SEND_GOSSIP_MENU(3834, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Bank
        player->SEND_POI(-8916.87f, 622.87f, 6, 6, 0, "Stormwind Bank");
        player->SEND_GOSSIP_MENU(764, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Deeprun tram
        player->SEND_POI(-8378.88f, 554.23f, 6, 6, 0, "The Deeprun Tram");
        player->SEND_GOSSIP_MENU(3813, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Inn
        player->SEND_POI(-8869.0f, 675.4f, 6, 6, 0, "The Gilded Rose");
        player->SEND_GOSSIP_MENU(3860, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Gryphon Master
        player->SEND_POI(-8837.0f, 493.5f, 6, 6, 0, "Stormwind Gryphon Master");
        player->SEND_GOSSIP_MENU(879, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Guild Master
        player->SEND_POI(-8894.0f, 611.2f, 6, 6, 0, "Stormwind Vistor`s Center");
        player->SEND_GOSSIP_MENU(882, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Mailbox
        player->SEND_POI(-8876.48f, 649.18f, 6, 6, 0, "Stormwind Mailbox");
        player->SEND_GOSSIP_MENU(3861, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Stable Master
        player->SEND_POI(-8433.0f, 554.7f, 6, 6, 0, "Jenova Stoneshield");
        player->SEND_GOSSIP_MENU(5984, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Weapon Trainer
        player->SEND_POI(-8797.0f, 612.8f, 6, 6, 0, "Woo Ping");
        player->SEND_GOSSIP_MENU(4516, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Officers Lounge
        player->SEND_POI(-8759.92f, 399.69f, 6, 6, 0, "Champions` Hall");
        player->SEND_GOSSIP_MENU(7047, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Battlemasters
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALTERACVALLEY        , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ARATHIBASIN          , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARSONGULCH          , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 3);
        player->SEND_GOSSIP_MENU(7499, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 12:                   //Class trainers
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAGE                 , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ROGUE                , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARRIOR              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_DRUID                , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PRIEST               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PALADIN              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HUNTER               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARLOCK              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SHAMAN               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 9);
        player->SEND_GOSSIP_MENU(898, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 13:                   //Profession trainers
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BLACKSMITHING        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENCHANTING           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENGINEERING          , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FISHING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HERBALISM            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 9);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MINING               , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 10);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 11);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAILORING            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 12);
        player->SEND_GOSSIP_MENU(918, pCreature->GetGUID());
        break;
    }
}

void SendBattleMasterMenu_guard_stormwind(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //AV
        player->SEND_POI(-8443.88f, 335.99f, 6, 6, 0, "Thelman Slatefist");
        player->SEND_GOSSIP_MENU(7500, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //AB
        player->SEND_POI(-8443.88f, 335.99f, 6, 6, 0, "Lady Hoteshem");
        player->SEND_GOSSIP_MENU(7650, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //WSG
        player->SEND_POI(-8443.88f, 335.99f, 6, 6, 0, "Elfarran");
        player->SEND_GOSSIP_MENU(7501, pCreature->GetGUID());
        break;
    }
}

void SendClassTrainerMenu_guard_stormwind(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Mage
        player->SEND_POI(-9012.0f, 867.6f, 6, 6, 0, "Wizard`s Sanctum");
        player->SEND_GOSSIP_MENU(899, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Rogue
        player->SEND_POI(-8753.0f, 367.8f, 6, 6, 0, "Stormwind - Rogue House");
        player->SEND_GOSSIP_MENU(900, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Warrior
        player->SEND_POI(-8690.11f, 324.85f, 6, 6, 0, "Command Center");
        player->SEND_GOSSIP_MENU(901, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Druid
        player->SEND_POI(-8751.0f, 1124.5f, 6, 6, 0, "The Park");
        player->SEND_GOSSIP_MENU(902, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Priest
        player->SEND_POI(-8512.0f, 862.4f, 6, 6, 0, "Catedral Of Light");
        player->SEND_GOSSIP_MENU(903, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Paladin
        player->SEND_POI(-8577.0f, 881.7f, 6, 6, 0, "Catedral Of Light");
        player->SEND_GOSSIP_MENU(904, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Hunter
        player->SEND_POI(-8413.0f, 541.5f, 6, 6, 0, "Hunter Lodge");
        player->SEND_GOSSIP_MENU(905, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Warlock
        player->SEND_POI(-8948.91f, 998.35f, 6, 6, 0, "The Slaughtered Lamb");
        player->SEND_GOSSIP_MENU(906, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Shaman
        player->SEND_POI(-9033, 550, 6, 6, 0, "Valley Of Heroes");
        //incorrect id
        player->SEND_GOSSIP_MENU(2593, pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_stormwind(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_POI(-8988.0f, 759.60f, 6, 6, 0, "Alchemy Needs");
        player->SEND_GOSSIP_MENU(919, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Blacksmithing
        player->SEND_POI(-8424.0f, 616.9f, 6, 6, 0, "Therum Deepforge");
        player->SEND_GOSSIP_MENU(920, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Cooking
        player->SEND_POI(-8611.0f, 364.6f, 6, 6, 0, "Pig and Whistle Tavern");
        player->SEND_GOSSIP_MENU(921, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Enchanting
        player->SEND_POI(-8858.0f, 803.7f, 6, 6, 0, "Lucan Cordell");
        player->SEND_GOSSIP_MENU(941, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Engineering
        player->SEND_POI(-8347.0f, 644.1f, 6, 6, 0, "Lilliam Sparkspindle");
        player->SEND_GOSSIP_MENU(922, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //First Aid
        player->SEND_POI(-8513.0f, 801.8f, 6, 6, 0, "Shaina Fuller");
        player->SEND_GOSSIP_MENU(923, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Fishing
        player->SEND_POI(-8803.0f, 767.5f, 6, 6, 0, "Arnold Leland");
        player->SEND_GOSSIP_MENU(940, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Herbalism
        player->SEND_POI(-8967.0f, 779.5f, 6, 6, 0, "Alchemy Needs");
        player->SEND_GOSSIP_MENU(924, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Leatherworking
        player->SEND_POI(-8726.0f, 477.4f, 6, 6, 0, "The Protective Hide");
        player->SEND_GOSSIP_MENU(925, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Mining
        player->SEND_POI(-8434.0f, 692.8f, 6, 6, 0, "Gelman Stonehand");
        player->SEND_GOSSIP_MENU(927, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Skinning
        player->SEND_POI(-8716.0f, 469.4f, 6, 6, 0, "The Protective Hide");
        player->SEND_GOSSIP_MENU(928, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 12:                   //Tailoring
        player->SEND_POI(-8938.0f, 800.7f, 6, 6, 0, "Duncan`s Textiles");
        player->SEND_GOSSIP_MENU(929, pCreature->GetGUID());
        break;
    }
}

bool GossipSelect_guard_stormwind(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_stormwind(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_CLASSTRAIN:
        SendClassTrainerMenu_guard_stormwind(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_stormwind(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_BATTLEINFO:
        SendBattleMasterMenu_guard_stormwind(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_stormwind end
 *******************************************************/

CreatureAI* GetAI_guard_stormwind(Creature* pCreature)
{
    return new guardAI_stormwind (pCreature);
}

/*******************************************************
 * guard_teldrassil start
 *******************************************************/

bool GossipHello_guard_teldrassil(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANK         , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FERRY        , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GUILDMASTER  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN          , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_CLASSTRAINER , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
    player->SEND_GOSSIP_MENU(4316, pCreature->GetGUID());
    return true;
}

void SendDefaultMenu_guard_teldrassil(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Bank
        player->SEND_GOSSIP_MENU(4317, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Rut`theran
        player->SEND_GOSSIP_MENU(4318, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Guild master
        player->SEND_GOSSIP_MENU(4319, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Inn
        player->SEND_POI(9821.49f, 960.13f, 6, 6, 0, "Dolanaar Inn");
        player->SEND_GOSSIP_MENU(4320, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //stable master
        player->SEND_POI(9808.37f, 931.1f, 6, 6, 0, "Seriadne");
        player->SEND_GOSSIP_MENU(5982, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //class trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_DRUID                , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HUNTER               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PRIEST               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ROGUE                , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARRIOR              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->SEND_GOSSIP_MENU(4264, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //profession trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENCHANTING           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FISHING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HERBALISM            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAILORING            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 9);
        player->SEND_GOSSIP_MENU(4273, pCreature->GetGUID());
        break;
    }
}

void SendClassTrainerMenu_guard_teldrassil(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Druid
        player->SEND_POI(9741.58f, 963.7f, 6, 6, 0, "Kal");
        player->SEND_GOSSIP_MENU(4323, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Hunter
        player->SEND_POI(9815.12f, 926.28f, 6, 6, 0, "Dazalar");
        player->SEND_GOSSIP_MENU(4324, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Priest
        player->SEND_POI(9906.16f, 986.63f, 6, 6, 0, "Laurna Morninglight");
        player->SEND_GOSSIP_MENU(4325, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Rogue
        player->SEND_POI(9789.00f, 942.86f, 6, 6, 0, "Jannok Breezesong");
        player->SEND_GOSSIP_MENU(4326, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Warrior
        player->SEND_POI(9821.96f, 950.61f, 6, 6, 0, "Kyra Windblade");
        player->SEND_GOSSIP_MENU(4327, pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_teldrassil(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_POI(9767.59f, 878.81f, 6, 6, 0, "Cyndra Kindwhisper");
        player->SEND_GOSSIP_MENU(4329, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Cooking
        player->SEND_POI(9751.19f, 906.13f, 6, 6, 0, "Zarrin");
        player->SEND_GOSSIP_MENU(4330, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Enchanting
        player->SEND_POI(10677.59f, 1946.56f, 6, 6, 0, "Alanna Raveneye");
        player->SEND_GOSSIP_MENU(4331, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //First Aid
        player->SEND_POI(9903.12f, 999.00f, 6, 6, 0, "Byancie");
        player->SEND_GOSSIP_MENU(4332, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Fishing
        player->SEND_GOSSIP_MENU(4333, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Herbalism
        player->SEND_POI(9773.78f, 875.88f, 6, 6, 0, "Malorne Bladeleaf");
        player->SEND_GOSSIP_MENU(4334, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Leatherworking
        player->SEND_POI(10152.59f, 1681.46f, 6, 6, 0, "Nadyia Maneweaver");
        player->SEND_GOSSIP_MENU(4335, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Skinning
        player->SEND_POI(10135.59f, 1673.18f, 6, 6, 0, "Radnaal Maneweaver");
        player->SEND_GOSSIP_MENU(4336, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Tailoring
        player->SEND_GOSSIP_MENU(4337, pCreature->GetGUID());
        break;
    }
}

bool GossipSelect_guard_teldrassil(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_teldrassil(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_CLASSTRAIN:
        SendClassTrainerMenu_guard_teldrassil(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_teldrassil(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_teldrassil end
 *******************************************************/

CreatureAI* GetAI_guard_teldrassil(Creature* pCreature)
{
    return new guardAI (pCreature);
}

/*******************************************************
 * guard_tirisfal start
 *******************************************************/

bool GossipHello_guard_tirisfal(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANK            , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATHANDLER      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN             , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_CLASSTRAINER    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER     , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->SEND_GOSSIP_MENU(4097, pCreature->GetGUID());
    return true;
}

void SendDefaultMenu_guard_tirisfal(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Bank
        player->SEND_GOSSIP_MENU(4074, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //bat handler
        player->SEND_GOSSIP_MENU(4075, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Inn
        player->SEND_POI(2246.68f, 241.89f, 6, 6, 0, "Gallows` End Tavern");
        player->SEND_GOSSIP_MENU(4076, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Stable Master
        player->SEND_POI(2267.66f, 319.32f, 6, 6, 0, "Morganus");
        player->SEND_GOSSIP_MENU(5978, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Class trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAGE                 , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PRIEST               , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ROGUE                , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARLOCK              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARRIOR              , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->SEND_GOSSIP_MENU(4292, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Profession trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BLACKSMITHING        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENCHANTING           , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENGINEERING          , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FISHING              , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HERBALISM            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 9);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MINING               , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 10);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING             , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 11);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAILORING            , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 12);
        player->SEND_GOSSIP_MENU(4096, pCreature->GetGUID());
        break;
    }
}

void SendClassTrainerMenu_guard_tirisfal(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Mage
        player->SEND_POI(2259.18f, 240.93f, 6, 6, 0, "Cain Firesong");
        player->SEND_GOSSIP_MENU(4077, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Priest
        player->SEND_POI(2259.18f, 240.93f, 6, 6, 0, "Dark Cleric Beryl");
        player->SEND_GOSSIP_MENU(4078, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Rogue
        player->SEND_POI(2259.18f, 240.93f, 6, 6, 0, "Marion Call");
        player->SEND_GOSSIP_MENU(4079, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Warlock
        player->SEND_POI(2259.18f, 240.93f, 6, 6, 0, "Rupert Boch");
        player->SEND_GOSSIP_MENU(4080, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Warrior
        player->SEND_POI(2256.48f, 240.32f, 6, 6, 0, "Austil de Mon");
        player->SEND_GOSSIP_MENU(4081, pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_tirisfal(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_POI(2263.25f, 344.23f, 6, 6, 0, "Carolai Anise");
        player->SEND_GOSSIP_MENU(4082, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Blacksmithing
        player->SEND_GOSSIP_MENU(4083, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Cooking
        player->SEND_GOSSIP_MENU(4084, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Enchanting
        player->SEND_POI(2250.35f, 249.12f, 6, 6, 0, "Vance Undergloom");
        player->SEND_GOSSIP_MENU(4085, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Engineering
        player->SEND_GOSSIP_MENU(4086, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //First Aid
        player->SEND_POI(2246.68f, 241.89f, 6, 6, 0, "Nurse Neela");
        player->SEND_GOSSIP_MENU(4087, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Fishing
        player->SEND_POI(2292.37f, -10.72f, 6, 6, 0, "Clyde Kellen");
        player->SEND_GOSSIP_MENU(4088, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Herbalism
        player->SEND_POI(2268.21f, 331.69f, 6, 6, 0, "Faruza");
        player->SEND_GOSSIP_MENU(4089, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Leatherworking
        player->SEND_POI(2027.00f, 78.72f, 6, 6, 0, "Shelene Rhobart");
        player->SEND_GOSSIP_MENU(4090, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Mining
        player->SEND_GOSSIP_MENU(4091, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Skinning
        player->SEND_POI(2027.00f, 78.72f, 6, 6, 0, "Rand Rhobart");
        player->SEND_GOSSIP_MENU(4092, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 12:                   //Tailoring
        player->SEND_POI(2160.45f, 659.93f, 6, 6, 0, "Bowen Brisboise");
        player->SEND_GOSSIP_MENU(4093, pCreature->GetGUID());
        break;
    }
}

bool GossipSelect_guard_tirisfal(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_tirisfal(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_CLASSTRAIN:
        SendClassTrainerMenu_guard_tirisfal(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_tirisfal(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_tirisfal end
 *******************************************************/

CreatureAI* GetAI_guard_tirisfal(Creature* pCreature)
{
    return new guardAI (pCreature);
}

/*******************************************************
 * guard_undercity start
 *******************************************************/

bool GossipHello_guard_undercity(Player* player, Creature* pCreature)
{
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BANK          , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATHANDLER    , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_GUILDMASTER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_INN           , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAILBOX       , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_AUCTIONHOUSE  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ZEPPLINMASTER , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WEAPONMASTER  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_STABLEMASTER  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BATTLEMASTER  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_CLASSTRAINER  , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
    player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PROFTRAINER   , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
    player->SEND_GOSSIP_MENU(3543, pCreature->GetGUID());
    return true;
}

void SendDefaultMenu_guard_undercity(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Bank
        player->SEND_POI(1595.64f, 232.45f, 6, 6, 0, "Undercity Bank");
        player->SEND_GOSSIP_MENU(3514, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Bat handler
        player->SEND_POI(1565.9f, 271.43f, 6, 6, 0, "Undercity Bat Handler");
        player->SEND_GOSSIP_MENU(3515, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Guild master
        player->SEND_POI(1594.17f, 205.57f, 6, 6, 0, "Undercity Guild Master");
        player->SEND_GOSSIP_MENU(3516, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Inn
        player->SEND_POI(1639.43f, 220.99f, 6, 6, 0, "Undercity Inn");
        player->SEND_GOSSIP_MENU(3517, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Mailbox
        player->SEND_POI(1632.68f, 219.4f, 6, 6, 0, "Undercity Mailbox");
        player->SEND_GOSSIP_MENU(3518, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //Auction House
        player->SEND_POI(1647.9f, 258.49f, 6, 6, 0, "Undercity Auction House");
        player->SEND_GOSSIP_MENU(3519, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Zeppelin
        player->SEND_POI(2059.00f, 274.86f, 6, 6, 0, "Undercity Zeppelin");
        player->SEND_GOSSIP_MENU(3520, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Weapon Master
        player->SEND_POI(1670.31f, 324.66f, 6, 6, 0, "Archibald");
        player->SEND_GOSSIP_MENU(4521, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Stable master
        player->SEND_POI(1634.18f, 226.76f, 6, 6, 0, "Anya Maulray");
        player->SEND_GOSSIP_MENU(5979, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Battlemaster
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALTERACVALLEY   , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ARATHIBASIN     , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARSONGULCH     , GOSSIP_SENDER_SEC_BATTLEINFO, GOSSIP_ACTION_INFO_DEF + 3);
        player->SEND_GOSSIP_MENU(7527, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Class trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MAGE            , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_PRIEST          , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ROGUE           , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARLOCK         , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_WARRIOR         , GOSSIP_SENDER_SEC_CLASSTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->SEND_GOSSIP_MENU(3542, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 12:                   //Profession trainer
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ALCHEMY         , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_BLACKSMITHING   , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_COOKING         , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENCHANTING      , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_ENGINEERING     , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FIRSTAID        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_FISHING         , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 7);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_HERBALISM       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 8);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_LEATHERWORKING  , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 9);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_MINING          , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 10);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_SKINNING        , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 11);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT_TAILORING       , GOSSIP_SENDER_SEC_PROFTRAIN, GOSSIP_ACTION_INFO_DEF + 12);
        player->SEND_GOSSIP_MENU(3541, pCreature->GetGUID());
        break;
    }
}

void SendBattleMasterMenu_guard_undercity(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //AV
        player->SEND_POI(1329.00f, 333.92f, 6, 6, 0, "Grizzle Halfmane");
        player->SEND_GOSSIP_MENU(7525, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //AB
        player->SEND_POI(1283.3f, 287.16f, 6, 6, 0, "Sir Malory Wheeler");
        player->SEND_GOSSIP_MENU(7646, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //WSG
        player->SEND_POI(1265.00f, 351.18f, 6, 6, 0, "Kurden Bloodclaw");
        player->SEND_GOSSIP_MENU(7526, pCreature->GetGUID());
        break;
    }
}

void SendClassTrainerMenu_guard_undercity(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Mage
        player->SEND_POI(1781.00f, 53.00f, 6, 6, 0, "Undercity Mage Trainers");
        player->SEND_GOSSIP_MENU(3513, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Priest
        player->SEND_POI(1758.33f, 401.5f, 6, 6, 0, "Undercity Priest Trainers");
        player->SEND_GOSSIP_MENU(3521, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Rogue
        player->SEND_POI(1418.56f, 65.00f, 6, 6, 0, "Undercity Rogue Trainers");
        player->SEND_GOSSIP_MENU(3524, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Warlock
        player->SEND_POI(1780.92f, 53.16f, 6, 6, 0, "Undercity Warlock Trainers");
        player->SEND_GOSSIP_MENU(3526, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Warrior
        player->SEND_POI(1775.59f, 418.19f, 6, 6, 0, "Undercity Warrior Trainers");
        player->SEND_GOSSIP_MENU(3527, pCreature->GetGUID());
        break;
    }
}

void SendProfTrainerMenu_guard_undercity(Player* player, Creature* pCreature, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:                    //Alchemy
        player->SEND_POI(1419.82f, 417.19f, 6, 6, 0, "The Apothecarium");
        player->SEND_GOSSIP_MENU(3528, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:                    //Blacksmithing
        player->SEND_POI(1696.0f, 285.0f, 6, 6, 0, "Undercity Blacksmithing Trainer");
        player->SEND_GOSSIP_MENU(3529, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:                    //Cooking
        player->SEND_POI(1596.34f, 274.68f, 6, 6, 0, "Undercity Cooking Trainer");
        player->SEND_GOSSIP_MENU(3530, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 4:                    //Enchanting
        player->SEND_POI(1488.54f, 280.19f, 6, 6, 0, "Undercity Enchanting Trainer");
        player->SEND_GOSSIP_MENU(3531, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 5:                    //Engineering
        player->SEND_POI(1408.58f, 143.43f, 6, 6, 0, "Undercity Engineering Trainer");
        player->SEND_GOSSIP_MENU(3532, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 6:                    //First Aid
        player->SEND_POI(1519.65f, 167.19f, 6, 6, 0, "Undercity First Aid Trainer");
        player->SEND_GOSSIP_MENU(3533, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 7:                    //Fishing
        player->SEND_POI(1679.9f, 89.0f, 6, 6, 0, "Undercity Fishing Trainer");
        player->SEND_GOSSIP_MENU(3534, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 8:                    //Herbalism
        player->SEND_POI(1558.0f, 349.36f, 6, 6, 0, "Undercity Herbalism Trainer");
        player->SEND_GOSSIP_MENU(3535, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 9:                    //Leatherworking
        player->SEND_POI(1498.76f, 196.43f, 6, 6, 0, "Undercity Leatherworking Trainer");
        player->SEND_GOSSIP_MENU(3536, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 10:                   //Mining
        player->SEND_POI(1642.88f, 335.58f, 6, 6, 0, "Undercity Mining Trainer");
        player->SEND_GOSSIP_MENU(3537, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 11:                   //Skinning
        player->SEND_POI(1498.6f, 196.46f, 6, 6, 0, "Undercity Skinning Trainer");
        player->SEND_GOSSIP_MENU(3538, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 12:                   //Tailoring
        player->SEND_POI(1689.55f, 193.0f, 6, 6, 0, "Undercity Tailoring Trainer");
        player->SEND_GOSSIP_MENU(3539, pCreature->GetGUID());
        break;
    }
}

bool GossipSelect_guard_undercity(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (sender)
    {
    case GOSSIP_SENDER_MAIN:
        SendDefaultMenu_guard_undercity(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_CLASSTRAIN:
        SendClassTrainerMenu_guard_undercity(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_PROFTRAIN:
        SendProfTrainerMenu_guard_undercity(player, pCreature, action);
        break;
    case GOSSIP_SENDER_SEC_BATTLEINFO:
        SendBattleMasterMenu_guard_undercity(player, pCreature, action);
        break;
    }
    return true;
}

/*******************************************************
 * guard_undercity end
 *******************************************************/

CreatureAI* GetAI_guard_undercity(Creature* pCreature)
{
    return new guardAI (pCreature);
}

/*******************************************************
 * AddSC
 *******************************************************/

void AddSC_guards()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "guard_azuremyst";
    newscript->pGossipHello          = &GossipHello_guard_azuremyst;
    newscript->pGossipSelect         = &GossipSelect_guard_azuremyst;
    newscript->GetAI = &GetAI_guard_azuremyst;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_bluffwatcher";
    newscript->pGossipHello          = &GossipHello_guard_bluffwatcher;
    newscript->pGossipSelect         = &GossipSelect_guard_bluffwatcher;
    newscript->GetAI = &GetAI_guard_bluffwatcher;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_contested";
    newscript->GetAI = &GetAI_guard_contested;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_darnassus";
    newscript->pGossipHello          = &GossipHello_guard_darnassus;
    newscript->pGossipSelect         = &GossipSelect_guard_darnassus;
    newscript->GetAI = &GetAI_guard_darnassus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_dunmorogh";
    newscript->pGossipHello          = &GossipHello_guard_dunmorogh;
    newscript->pGossipSelect         = &GossipSelect_guard_dunmorogh;
    newscript->GetAI = &GetAI_guard_dunmorogh;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_durotar";
    newscript->pGossipHello          = &GossipHello_guard_durotar;
    newscript->pGossipSelect         = &GossipSelect_guard_durotar;
    newscript->GetAI = &GetAI_guard_durotar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_elwynnforest";
    newscript->pGossipHello          = &GossipHello_guard_elwynnforest;
    newscript->pGossipSelect         = &GossipSelect_guard_elwynnforest;
    newscript->GetAI = &GetAI_guard_elwynnforest;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_eversong";
    newscript->pGossipHello          = &GossipHello_guard_eversong;
    newscript->pGossipSelect         = &GossipSelect_guard_eversong;
    newscript->GetAI = &GetAI_guard_eversong;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_exodar";
    newscript->pGossipHello          = &GossipHello_guard_exodar;
    newscript->pGossipSelect         = &GossipSelect_guard_exodar;
    newscript->GetAI = &GetAI_guard_exodar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_ironforge";
    newscript->pGossipHello          = &GossipHello_guard_ironforge;
    newscript->pGossipSelect         = &GossipSelect_guard_ironforge;
    newscript->GetAI = &GetAI_guard_ironforge;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_mulgore";
    newscript->pGossipHello          = &GossipHello_guard_mulgore;
    newscript->pGossipSelect         = &GossipSelect_guard_mulgore;
    newscript->GetAI = &GetAI_guard_mulgore;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_orgrimmar";
    newscript->pGossipHello          = &GossipHello_guard_orgrimmar;
    newscript->pGossipSelect         = &GossipSelect_guard_orgrimmar;
    newscript->GetAI = &GetAI_guard_orgrimmar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_shattrath";
    newscript->pGossipHello          = &GossipHello_guard_shattrath;
    newscript->pGossipSelect         = &GossipSelect_guard_shattrath;
    newscript->GetAI = &GetAI_guard_shattrath;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_shattrath_aldor";
    newscript->GetAI = &GetAI_guard_shattrath_aldor;
    newscript->pGossipHello          = &GossipHello_guard_shattrath_aldor;
    newscript->pGossipSelect         = &GossipSelect_guard_shattrath_aldor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_shattrath_scryer";
    newscript->GetAI = &GetAI_guard_shattrath_scryer;
    newscript->pGossipHello          = &GossipHello_guard_shattrath_scryer;
    newscript->pGossipSelect         = &GossipSelect_guard_shattrath_scryer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_silvermoon";
    newscript->pGossipHello          = &GossipHello_guard_silvermoon;
    newscript->pGossipSelect         = &GossipSelect_guard_silvermoon;
    newscript->GetAI = &GetAI_guard_silvermoon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_stormwind";
    newscript->pGossipHello          = &GossipHello_guard_stormwind;
    newscript->pGossipSelect         = &GossipSelect_guard_stormwind;
    newscript->GetAI = &GetAI_guard_stormwind;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_teldrassil";
    newscript->pGossipHello          = &GossipHello_guard_teldrassil;
    newscript->pGossipSelect         = &GossipSelect_guard_teldrassil;
    newscript->GetAI = &GetAI_guard_teldrassil;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_tirisfal";
    newscript->pGossipHello          = &GossipHello_guard_tirisfal;
    newscript->pGossipSelect         = &GossipSelect_guard_tirisfal;
    newscript->GetAI = &GetAI_guard_tirisfal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "guard_undercity";
    newscript->pGossipHello          = &GossipHello_guard_undercity;
    newscript->pGossipSelect         = &GossipSelect_guard_undercity;
    newscript->GetAI = &GetAI_guard_undercity;
    newscript->RegisterSelf();
}

