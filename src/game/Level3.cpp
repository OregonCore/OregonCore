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

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "ObjectMgr.h"
#include "AuctionHouseMgr.h"
#include "AccountMgr.h"
#include "PlayerDump.h"
#include "SpellMgr.h"
#include "Player.h"
#include "Opcodes.h"
#include "GameObject.h"
#include "Chat.h"
#include "Log.h"
#include "Guild.h"
#include "ObjectAccessor.h"
#include "MapManager.h"
#include "SpellAuras.h"
#include "Language.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "Weather.h"
#include "PointMovementGenerator.h"
#include "TargetedMovementGenerator.h"
#include "SkillDiscovery.h"
#include "SkillExtraItems.h"
#include "SystemConfig.h"
#include "Config/Config.h"
#include "Utilities/Util.h"
#include "ItemEnchantmentMgr.h"
#include "BattlegroundMgr.h"
#include "InstanceSaveMgr.h"
#include "InstanceData.h"
#include "AuctionHouseBot.h"
#include "CreatureEventAIMgr.h"
#include "DisableMgr.h"
#include "ConditionMgr.h"
#include "ScriptMgr.h"

bool ChatHandler::HandleAHBotOptionsCommand(const char* args)
{
    uint32 ahMapID = 0;
    char* opt = strtok((char*)args, " ");
    char* ahMapIdStr = strtok(NULL, " ");
    if (ahMapIdStr)
        ahMapID = (uint32) strtoul(ahMapIdStr, NULL, 0);
    if (!opt)
    {
        PSendSysMessage("Syntax is: ahbotoptions $option $ahMapID (2, 6 or 7) $parameter");
        PSendSysMessage("Try ahbotoptions help to see a list of options.");
        return false;
    }
    int l = strlen(opt);

    if (strncmp(opt, "help", l) == 0)
    {
        PSendSysMessage("AHBot commands:");
        PSendSysMessage("ahexpire");
        PSendSysMessage("minitems");
        PSendSysMessage("maxitems");
        //PSendSysMessage("");
        //PSendSysMessage("");
        PSendSysMessage("percentages");
        PSendSysMessage("minprice");
        PSendSysMessage("maxprice");
        PSendSysMessage("minbidprice");
        PSendSysMessage("maxbidprice");
        PSendSysMessage("maxstack");
        PSendSysMessage("buyerprice");
        PSendSysMessage("bidinterval");
        PSendSysMessage("bidsperinterval");
        return true;
    }
    else if (strncmp(opt, "ahexpire", l) == 0)
    {
        if (!ahMapIdStr)
        {
            PSendSysMessage("Syntax is: ahbotoptions ahexpire $ahMapID (2, 6 or 7)");
            return false;
        }
        auctionbot.Commands(0, ahMapID, 0, NULL);
    }
    else if (strncmp(opt, "minitems", l) == 0)
    {
        char* param1 = strtok(NULL, " ");
        if ((!ahMapIdStr) || (!param1))
        {
            PSendSysMessage("Syntax is: ahbotoptions minitems $ahMapID (2, 6 or 7) $minItems");
            return false;
        }
        auctionbot.Commands(1, ahMapID, 0, param1);
    }
    else if (strncmp(opt, "maxitems", l) == 0)
    {
        char* param1 = strtok(NULL, " ");
        if ((!ahMapIdStr) || (!param1))
        {
            PSendSysMessage("Syntax is: ahbotoptions maxitems $ahMapID (2, 6 or 7) $maxItems");
            return false;
        }
        auctionbot.Commands(2, ahMapID, 0, param1);
    }
    else if (strncmp(opt, "mintime", l) == 0)
    {
        PSendSysMessage("ahbotoptions mintime has been deprecated");
        return false;
        /*
        char * param1 = strtok(NULL, " ");
        if ((!ahMapIdStr) || (!param1))
        {
            PSendSysMessage("Syntax is: ahbotoptions mintime $ahMapID (2, 6 or 7) $mintime");
            return false;
        }
        auctionbot.Commands(3, ahMapID, 0, param1);
        */
    }
    else if (strncmp(opt, "maxtime", l) == 0)
    {
        PSendSysMessage("ahbotoptions maxtime has been deprecated");
        return false;
        /*
        char * param1 = strtok(NULL, " ");
        if ((!ahMapIdStr) || (!param1))
        {
            PSendSysMessage("Syntax is: ahbotoptions maxtime $ahMapID (2, 6 or 7) $maxtime");
            return false;
        }
        auctionbot.Commands(4, ahMapID, 0, param1);
        */
    }
    else if (strncmp(opt, "percentages", l) == 0)
    {
        char* param1 = strtok(NULL, " ");
        char* param2 = strtok(NULL, " ");
        char* param3 = strtok(NULL, " ");
        char* param4 = strtok(NULL, " ");
        char* param5 = strtok(NULL, " ");
        char* param6 = strtok(NULL, " ");
        char* param7 = strtok(NULL, " ");
        char* param8 = strtok(NULL, " ");
        char* param9 = strtok(NULL, " ");
        char* param10 = strtok(NULL, " ");
        char* param11 = strtok(NULL, " ");
        char* param12 = strtok(NULL, " ");
        char* param13 = strtok(NULL, " ");
        char* param14 = strtok(NULL, " ");
        if ((!ahMapIdStr) || (!param14))
        {
            PSendSysMessage("Syntax is: ahbotoptions percentages $ahMapID (2, 6 or 7) $1 $2 $3 $4 $5 $6 $7 $8 $9 $10 $11 $12 $13 $14");
            PSendSysMessage("1 GreyTradeGoods 2 WhiteTradeGoods 3 GreenTradeGoods 4 BlueTradeGoods 5 PurpleTradeGoods");
            PSendSysMessage("6 OrangeTradeGoods 7 YellowTradeGoods 8 GreyItems 9 WhiteItems 10 GreenItems 11 BlueItems");
            PSendSysMessage("12 PurpleItems 13 OrangeItems 14 YellowItems");
            PSendSysMessage("The total must add up to 100%%");
            return false;
        }
        uint32 greytg = (uint32) strtoul(param1, NULL, 0);
        uint32 whitetg = (uint32) strtoul(param2, NULL, 0);
        uint32 greentg = (uint32) strtoul(param3, NULL, 0);
        uint32 bluetg = (uint32) strtoul(param3, NULL, 0);
        uint32 purpletg = (uint32) strtoul(param5, NULL, 0);
        uint32 orangetg = (uint32) strtoul(param6, NULL, 0);
        uint32 yellowtg = (uint32) strtoul(param7, NULL, 0);
        uint32 greyi = (uint32) strtoul(param8, NULL, 0);
        uint32 whitei = (uint32) strtoul(param9, NULL, 0);
        uint32 greeni = (uint32) strtoul(param10, NULL, 0);
        uint32 bluei = (uint32) strtoul(param11, NULL, 0);
        uint32 purplei = (uint32) strtoul(param12, NULL, 0);
        uint32 orangei = (uint32) strtoul(param13, NULL, 0);
        uint32 yellowi = (uint32) strtoul(param14, NULL, 0);
        uint32 totalPercent = greytg + whitetg + greentg + bluetg + purpletg + orangetg + yellowtg + greyi + whitei + greeni + bluei + purplei + orangei + yellowi;
        if ((totalPercent == 0) || (totalPercent != 100))
        {
            PSendSysMessage("Syntax is: ahbotoptions percentages $ahMapID (2, 6 or 7) $1 $2 $3 $4 $5 $6 $7 $8 $9 $10 $11 $12 $13 $14");
            PSendSysMessage("1 GreyTradeGoods 2 WhiteTradeGoods 3 GreenTradeGoods 4 BlueTradeGoods 5 PurpleTradeGoods");
            PSendSysMessage("6 OrangeTradeGoods 7 YellowTradeGoods 8 GreyItems 9 WhiteItems 10 GreenItems 11 BlueItems");
            PSendSysMessage("12 PurpleItems 13 OrangeItems 14 YellowItems");
            PSendSysMessage("The total must add up to 100%%");
            return false;
        }
        char param[100];
        param[0] = '\0';
        strcat(param, param1);
        strcat(param, " ");
        strcat(param, param2);
        strcat(param, " ");
        strcat(param, param3);
        strcat(param, " ");
        strcat(param, param4);
        strcat(param, " ");
        strcat(param, param5);
        strcat(param, " ");
        strcat(param, param6);
        strcat(param, " ");
        strcat(param, param7);
        strcat(param, " ");
        strcat(param, param8);
        strcat(param, " ");
        strcat(param, param9);
        strcat(param, " ");
        strcat(param, param10);
        strcat(param, " ");
        strcat(param, param11);
        strcat(param, " ");
        strcat(param, param12);
        strcat(param, " ");
        strcat(param, param13);
        strcat(param, " ");
        strcat(param, param14);
        auctionbot.Commands(5, ahMapID, 0, param);
    }
    else if (strncmp(opt, "minprice", l) == 0)
    {
        char* param1 = strtok(NULL, " ");
        char* param2 = strtok(NULL, " ");
        if ((!ahMapIdStr) || (!param1) || (!param2))
        {
            PSendSysMessage("Syntax is: ahbotoptions minprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $price");
            return false;
        }
        if (strncmp(param1, "grey", l) == 0)
            auctionbot.Commands(6, ahMapID, AHB_GREY, param2);
        else if (strncmp(param1, "white", l) == 0)
            auctionbot.Commands(6, ahMapID, AHB_WHITE, param2);
        else if (strncmp(param1, "green", l) == 0)
            auctionbot.Commands(6, ahMapID, AHB_GREEN, param2);
        else if (strncmp(param1, "blue", l) == 0)
            auctionbot.Commands(6, ahMapID, AHB_BLUE, param2);
        else if (strncmp(param1, "purple", l) == 0)
            auctionbot.Commands(6, ahMapID, AHB_PURPLE, param2);
        else if (strncmp(param1, "orange", l) == 0)
            auctionbot.Commands(6, ahMapID, AHB_ORANGE, param2);
        else if (strncmp(param1, "yellow", l) == 0)
            auctionbot.Commands(6, ahMapID, AHB_YELLOW, param2);
        else
        {
            PSendSysMessage("Syntax is: ahbotoptions minprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $price");
            return false;
        }
    }
    else if (strncmp(opt, "maxprice", l) == 0)
    {
        char* param1 = strtok(NULL, " ");
        char* param2 = strtok(NULL, " ");
        if ((!ahMapIdStr) || (!param1) || (!param2))
        {
            PSendSysMessage("Syntax is: ahbotoptions maxprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $price");
            return false;
        }
        if (strncmp(param1, "grey", l) == 0)
            auctionbot.Commands(7, ahMapID, AHB_GREY, param2);
        else if (strncmp(param1, "white", l) == 0)
            auctionbot.Commands(7, ahMapID, AHB_WHITE, param2);
        else if (strncmp(param1, "green", l) == 0)
            auctionbot.Commands(7, ahMapID, AHB_GREEN, param2);
        else if (strncmp(param1, "blue", l) == 0)
            auctionbot.Commands(7, ahMapID, AHB_BLUE, param2);
        else if (strncmp(param1, "purple", l) == 0)
            auctionbot.Commands(7, ahMapID, AHB_PURPLE, param2);
        else if (strncmp(param1, "orange", l) == 0)
            auctionbot.Commands(7, ahMapID, AHB_ORANGE, param2);
        else if (strncmp(param1, "yellow", l) == 0)
            auctionbot.Commands(7, ahMapID, AHB_YELLOW, param2);
        else
        {
            PSendSysMessage("Syntax is: ahbotoptions maxprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $price");
            return false;
        }
    }
    else if (strncmp(opt, "minbidprice", l) == 0)
    {
        char* param1 = strtok(NULL, " ");
        char* param2 = strtok(NULL, " ");
        if ((!ahMapIdStr) || (!param1) || (!param2))
        {
            PSendSysMessage("Syntax is: ahbotoptions minbidprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $price");
            return false;
        }
        uint32 minBidPrice = (uint32) strtoul(param2, NULL, 0);
        if ((minBidPrice < 1) || (minBidPrice > 100))
        {
            PSendSysMessage("The min bid price multiplier must be between 1 and 100");
            return false;
        }
        if (strncmp(param1, "grey", l) == 0)
            auctionbot.Commands(8, ahMapID, AHB_GREY, param2);
        else if (strncmp(param1, "white", l) == 0)
            auctionbot.Commands(8, ahMapID, AHB_WHITE, param2);
        else if (strncmp(param1, "green", l) == 0)
            auctionbot.Commands(8, ahMapID, AHB_GREEN, param2);
        else if (strncmp(param1, "blue", l) == 0)
            auctionbot.Commands(8, ahMapID, AHB_BLUE, param2);
        else if (strncmp(param1, "purple", l) == 0)
            auctionbot.Commands(8, ahMapID, AHB_PURPLE, param2);
        else if (strncmp(param1, "orange", l) == 0)
            auctionbot.Commands(8, ahMapID, AHB_ORANGE, param2);
        else if (strncmp(param1, "yellow", l) == 0)
            auctionbot.Commands(8, ahMapID, AHB_YELLOW, param2);
        else
        {
            PSendSysMessage("Syntax is: ahbotoptions minbidprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $price");
            return false;
        }
    }
    else if (strncmp(opt, "maxbidprice", l) == 0)
    {
        char* param1 = strtok(NULL, " ");
        char* param2 = strtok(NULL, " ");
        if ((!ahMapIdStr) || (!param1) || (!param2))
        {
            PSendSysMessage("Syntax is: ahbotoptions maxbidprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $price");
            return false;
        }
        uint32 maxBidPrice = (uint32) strtoul(param2, NULL, 0);
        if ((maxBidPrice < 1) || (maxBidPrice > 100))
        {
            PSendSysMessage("The max bid price multiplier must be between 1 and 100");
            return false;
        }
        if (strncmp(param1, "grey", l) == 0)
            auctionbot.Commands(9, ahMapID, AHB_GREY, param2);
        else if (strncmp(param1, "white", l) == 0)
            auctionbot.Commands(9, ahMapID, AHB_WHITE, param2);
        else if (strncmp(param1, "green", l) == 0)
            auctionbot.Commands(9, ahMapID, AHB_GREEN, param2);
        else if (strncmp(param1, "blue", l) == 0)
            auctionbot.Commands(9, ahMapID, AHB_BLUE, param2);
        else if (strncmp(param1, "purple", l) == 0)
            auctionbot.Commands(9, ahMapID, AHB_PURPLE, param2);
        else if (strncmp(param1, "orange", l) == 0)
            auctionbot.Commands(9, ahMapID, AHB_ORANGE, param2);
        else if (strncmp(param1, "yellow", l) == 0)
            auctionbot.Commands(9, ahMapID, AHB_YELLOW, param2);
        else
        {
            PSendSysMessage("Syntax is: ahbotoptions max bidprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $price");
            return false;
        }
    }
    else if (strncmp(opt, "maxstack", l) == 0)
    {
        char* param1 = strtok(NULL, " ");
        char* param2 = strtok(NULL, " ");
        if ((!ahMapIdStr) || (!param1) || (!param2))
        {
            PSendSysMessage("Syntax is: ahbotoptions maxstack $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $value");
            return false;
        }
        int32 maxStack = (uint32) strtol(param2, NULL, 0);
        if (maxStack < 0)
        {
            PSendSysMessage("maxstack can't be a negative number.");
            return false;
        }
        if (strncmp(param1, "grey", l) == 0)
            auctionbot.Commands(10, ahMapID, AHB_GREY, param2);
        else if (strncmp(param1, "white", l) == 0)
            auctionbot.Commands(10, ahMapID, AHB_WHITE, param2);
        else if (strncmp(param1, "green", l) == 0)
            auctionbot.Commands(10, ahMapID, AHB_GREEN, param2);
        else if (strncmp(param1, "blue", l) == 0)
            auctionbot.Commands(10, ahMapID, AHB_BLUE, param2);
        else if (strncmp(param1, "purple", l) == 0)
            auctionbot.Commands(10, ahMapID, AHB_PURPLE, param2);
        else if (strncmp(param1, "orange", l) == 0)
            auctionbot.Commands(10, ahMapID, AHB_ORANGE, param2);
        else if (strncmp(param1, "yellow", l) == 0)
            auctionbot.Commands(10, ahMapID, AHB_YELLOW, param2);
        else
        {
            PSendSysMessage("Syntax is: ahbotoptions maxstack $ahMapID (2, 6 or 7) $color (grey, white, green, blue, purple, orange or yellow) $value");
            return false;
        }
    }
    else if (strncmp(opt, "buyerprice", l) == 0)
    {
        char* param1 = strtok(NULL, " ");
        char* param2 = strtok(NULL, " ");
        if ((!ahMapIdStr) || (!param1) || (!param2))
        {
            PSendSysMessage("Syntax is: ahbotoptions buyerprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue or purple) $price");
            return false;
        }
        if (strncmp(param1, "grey", l) == 0)
            auctionbot.Commands(11, ahMapID, AHB_GREY, param2);
        else if (strncmp(param1, "white", l) == 0)
            auctionbot.Commands(11, ahMapID, AHB_WHITE, param2);
        else if (strncmp(param1, "green", l) == 0)
            auctionbot.Commands(11, ahMapID, AHB_GREEN, param2);
        else if (strncmp(param1, "blue", l) == 0)
            auctionbot.Commands(11, ahMapID, AHB_BLUE, param2);
        else if (strncmp(param1, "purple", l) == 0)
            auctionbot.Commands(11, ahMapID, AHB_PURPLE, param2);
        else if (strncmp(param1, "orange", l) == 0)
            auctionbot.Commands(11, ahMapID, AHB_ORANGE, param2);
        else if (strncmp(param1, "yellow", l) == 0)
            auctionbot.Commands(11, ahMapID, AHB_YELLOW, param2);
        else
        {
            PSendSysMessage("Syntax is: ahbotoptions buyerprice $ahMapID (2, 6 or 7) $color (grey, white, green, blue or purple) $price");
            return false;
        }
    }
    else if (strncmp(opt, "bidinterval", l) == 0)
    {
        char* param1 = strtok(NULL, " ");
        if ((!ahMapIdStr) || (!param1))
        {
            PSendSysMessage("Syntax is: ahbotoptions bidinterval $ahMapID (2, 6 or 7) $interval(in minutes)");
            return false;
        }
        auctionbot.Commands(12, ahMapID, 0, param1);
    }
    else if (strncmp(opt, "bidsperinterval", l) == 0)
    {
        char* param1 = strtok(NULL, " ");
        if ((!ahMapIdStr) || (!param1))
        {
            PSendSysMessage("Syntax is: ahbotoptions bidsperinterval $ahMapID (2, 6 or 7) $bids");
            return false;
        }
        auctionbot.Commands(13, ahMapID, 0, param1);
    }
    else
    {
        PSendSysMessage("Syntax is: ahbotoptions $option $ahMapID (2, 6 or 7) $parameter");
        PSendSysMessage("Try ahbotoptions help to see a list of options.");
        return false;
    }
    return true;
}

//reload commands
bool ChatHandler::HandleRAFReloadCommand(const char*)
{
    sObjectMgr.LoadReferredFriends();
    PSendSysMessage("RAF reloaded.");
    return true;
}

bool ChatHandler::HandleReloadCommand(const char* arg)
{
    // this is error catcher for wrong table name in .reload commands
    PSendSysMessage("Db table with name starting from '%s' not found and can't be reloaded.", arg);
    SetSentErrorMessage(true);
    return false;
}

bool ChatHandler::HandleReloadAllCommand(const char*)
{
    HandleReloadAreaTriggerTeleportCommand("");
    HandleReloadSkillFishingBaseLevelCommand("");

    HandleReloadAllAreaCommand("");
    HandleReloadAllLootCommand("");
    HandleReloadAllNpcCommand("");
    HandleReloadAllQuestCommand("");
    HandleReloadAllSpellCommand("");
    HandleReloadAllItemCommand("");
    HandleReloadAllLocalesCommand("");

    HandleReloadCommandCommand("");
    HandleReloadReservedNameCommand("");
    HandleReloadOregonStringCommand("");
    HandleReloadGameTeleCommand("");
    HandleReloadAutobroadcastCommand("");
    return true;
}

bool ChatHandler::HandleReloadAllAreaCommand(const char*)
{
    //HandleReloadQuestAreaTriggersCommand(""); -- reloaded in HandleReloadAllQuestCommand
    HandleReloadAreaTriggerTeleportCommand("");
    HandleReloadAreaTriggerTavernCommand("");
    HandleReloadGameGraveyardZoneCommand("");
    return true;
}

bool ChatHandler::HandleReloadAllLootCommand(const char*)
{
    sLog.outString("Re-Loading Loot Tables...");
    LoadLootTables();
    SendGlobalGMSysMessage("DB tables *_loot_template reloaded.");
    sConditionMgr.LoadConditions(true);
    return true;
}

bool ChatHandler::HandleReloadAllNpcCommand(const char* /*args*/)
{
    HandleReloadNpcGossipCommand("a");
    HandleReloadNpcTrainerCommand("a");
    HandleReloadNpcVendorCommand("a");
    return true;
}

bool ChatHandler::HandleReloadAllQuestCommand(const char* /*args*/)
{
    HandleReloadQuestAreaTriggersCommand("a");
    HandleReloadQuestTemplateCommand("a");

    sLog.outString("Re-Loading Quests Relations...");
    sObjectMgr.LoadQuestStartersAndEnders();
    SendGlobalGMSysMessage("DB tables *_queststarter and *_questender reloaded.");
    return true;
}

bool ChatHandler::HandleReloadAllScriptsCommand(const char*)
{
    if (sWorld.IsScriptScheduled())
    {
        PSendSysMessage("DB scripts used currently, please attempt reload later.");
        SetSentErrorMessage(true);
        return false;
    }

    sLog.outString("Re-Loading Scripts...");
    HandleReloadGameObjectScriptsCommand("a");
    HandleReloadEventScriptsCommand("a");
    HandleReloadQuestEndScriptsCommand("a");
    HandleReloadQuestStartScriptsCommand("a");
    HandleReloadSpellScriptsCommand("a");
    SendGlobalGMSysMessage("DB tables *_scripts reloaded.");
    HandleReloadDbScriptStringCommand("a");
    HandleReloadWpScriptsCommand("a");
    return true;
}

bool ChatHandler::HandleReloadAllSpellCommand(const char*)
{
    HandleReloadSkillDiscoveryTemplateCommand("a");
    HandleReloadSkillExtraItemTemplateCommand("a");
    HandleReloadSpellAffectCommand("a");
    HandleReloadSpellRequiredCommand("a");
    HandleReloadSpellLearnSpellCommand("a");
    HandleReloadSpellGroupsCommand("a");
    HandleReloadSpellGroupStackRulesCommand("a");
    HandleReloadSpellLinkedSpellCommand("a");
    HandleReloadSpellProcEventCommand("a");
    HandleReloadSpellTargetPositionCommand("a");
    HandleReloadSpellThreatsCommand("a");
    HandleReloadSpellPetAurasCommand("a");
    return true;
}

bool ChatHandler::HandleReloadAllItemCommand(const char*)
{
    HandleReloadPageTextsCommand("a");
    HandleReloadItemEnchantementsCommand("a");
    return true;
}

bool ChatHandler::HandleReloadAllLocalesCommand(const char* /*args*/)
{
    HandleReloadLocalesCreatureCommand("a");
    HandleReloadLocalesGameobjectCommand("a");
    HandleReloadLocalesItemCommand("a");
    HandleReloadLocalesNpcTextCommand("a");
    HandleReloadLocalesPageTextCommand("a");
    HandleReloadLocalesQuestCommand("a");
    return true;
}

bool ChatHandler::HandleReloadConfigCommand(const char* /*args*/)
{
    sLog.outString("Re-Loading config settings...");
    sWorld.LoadConfigSettings(true);
    MapManager::Instance().InitializeVisibilityDistanceInfo();
    SendGlobalGMSysMessage("World config settings reloaded.");
    return true;
}

bool ChatHandler::HandleReloadAreaTriggerTavernCommand(const char*)
{
    sLog.outString("Re-Loading Tavern Area Triggers...");
    sObjectMgr.LoadTavernAreaTriggers();
    SendGlobalGMSysMessage("DB table areatrigger_tavern reloaded.");
    return true;
}

bool ChatHandler::HandleReloadAreaTriggerTeleportCommand(const char*)
{
    sLog.outString("Re-Loading AreaTrigger teleport definitions...");
    sObjectMgr.LoadAreaTriggerTeleports();
    SendGlobalGMSysMessage("DB table areatrigger_teleport reloaded.");
    return true;
}

bool ChatHandler::HandleReloadAccessRequirementCommand(const char*)
{
    sLog.outString("Re-Loading Access Requirement definitions...");
    sObjectMgr.LoadAccessRequirements();
    SendGlobalGMSysMessage("DB table access_requirement reloaded.");
    return true;
}

bool ChatHandler::HandleReloadAutobroadcastCommand(const char*)
{
    sLog.outString("Re-Loading Autobroadcast...");
    sWorld.LoadAutobroadcasts();
    SendGlobalGMSysMessage("DB table autobroadcast reloaded.");
    return true;
}

bool ChatHandler::HandleReloadCommandCommand(const char*)
{
    load_command_table = true;
    SendGlobalGMSysMessage("DB table command will be reloaded at next chat command use.");
    return true;
}

bool ChatHandler::HandleReloadCreatureQuestStarterCommand(const char*)
{
    sLog.outString("Loading Quests Relations... (creature_queststarter)");
    sObjectMgr.LoadCreatureQuestStarters();
    SendGlobalGMSysMessage("DB table creature_queststarter reloaded.");
    return true;
}

bool ChatHandler::HandleReloadCreatureLinkedRespawnCommand(const char* /*args*/)
{
    sLog.outString("Loading Linked Respawns... (creature_linked_respawn)");
    sObjectMgr.LoadCreatureLinkedRespawn();
    SendGlobalGMSysMessage("DB table creature_linked_respawn (creature linked respawns) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadGossipMenuCommand(const char*)
{
    sLog.outString( "Re-Loading `gossip_menu` Table!" );
    sObjectMgr.LoadGossipMenu();
    SendGlobalSysMessage("DB table `gossip_menu` reloaded.");
    sConditionMgr.LoadConditions(true);
    return true;
}

bool ChatHandler::HandleReloadGossipMenuOptionCommand(const char*)
{
    sLog.outString( "Re-Loading `gossip_menu_option` Table!" );
    sObjectMgr.LoadGossipMenuItems();
    SendGlobalSysMessage("DB table `gossip_menu_option` reloaded.");
    sConditionMgr.LoadConditions(true);
    return true;
}

bool ChatHandler::HandleReloadCreatureQuestEnderCommand(const char*)
{
    sLog.outString("Loading Quests Relations... (creature_questender)");
    sObjectMgr.LoadCreatureQuestEnders();
    SendGlobalGMSysMessage("DB table creature_questender reloaded.");
    return true;
}

bool ChatHandler::HandleReloadGOQuestStarterCommand(const char*)
{
    sLog.outString("Loading Quests Relations... (gameobject_queststarter)");
    sObjectMgr.LoadGameobjectQuestStarters();
    SendGlobalGMSysMessage("DB table gameobject_queststarter reloaded.");
    return true;
}

bool ChatHandler::HandleReloadGOQuestEnderCommand(const char*)
{
    sLog.outString("Loading Quests Relations... (gameobject_questender)");
    sObjectMgr.LoadGameobjectQuestEnders();
    SendGlobalGMSysMessage("DB table gameobject_questender reloaded.");
    return true;
}

bool ChatHandler::HandleReloadQuestAreaTriggersCommand(const char*)
{
    sLog.outString("Re-Loading Quest Area Triggers...");
    sObjectMgr.LoadQuestAreaTriggers();
    SendGlobalGMSysMessage("DB table areatrigger_involvedrelation (quest area triggers) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadQuestTemplateCommand(const char*)
{
    sLog.outString("Re-Loading Quest Templates...");
    sObjectMgr.LoadQuests();
    SendGlobalGMSysMessage("DB table quest_template (quest definitions) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLootTemplatesCreatureCommand(const char*)
{
    sLog.outString("Re-Loading Loot Tables... (creature_loot_template)");
    LoadLootTemplates_Creature();
    LootTemplates_Creature.CheckLootRefs();
    SendGlobalGMSysMessage("DB table creature_loot_template reloaded.");
    sConditionMgr.LoadConditions(true);
    return true;
}

bool ChatHandler::HandleReloadLootTemplatesDisenchantCommand(const char*)
{
    sLog.outString("Re-Loading Loot Tables... (disenchant_loot_template)");
    LoadLootTemplates_Disenchant();
    LootTemplates_Disenchant.CheckLootRefs();
    SendGlobalGMSysMessage("DB table disenchant_loot_template reloaded.");
    sConditionMgr.LoadConditions(true);
    return true;
}

bool ChatHandler::HandleReloadLootTemplatesFishingCommand(const char*)
{
    sLog.outString("Re-Loading Loot Tables... (fishing_loot_template)");
    LoadLootTemplates_Fishing();
    LootTemplates_Fishing.CheckLootRefs();
    SendGlobalGMSysMessage("DB table fishing_loot_template reloaded.");
    sConditionMgr.LoadConditions(true);
    return true;
}

bool ChatHandler::HandleReloadLootTemplatesGameobjectCommand(const char*)
{
    sLog.outString("Re-Loading Loot Tables... (gameobject_loot_template)");
    LoadLootTemplates_Gameobject();
    LootTemplates_Gameobject.CheckLootRefs();
    SendGlobalGMSysMessage("DB table gameobject_loot_template reloaded.");
    sConditionMgr.LoadConditions(true);
    return true;
}

bool ChatHandler::HandleReloadLootTemplatesItemCommand(const char*)
{
    sLog.outString("Re-Loading Loot Tables... (item_loot_template)");
    LoadLootTemplates_Item();
    LootTemplates_Item.CheckLootRefs();
    SendGlobalGMSysMessage("DB table item_loot_template reloaded.");
    sConditionMgr.LoadConditions(true);
    return true;
}

bool ChatHandler::HandleReloadLootTemplatesPickpocketingCommand(const char*)
{
    sLog.outString("Re-Loading Loot Tables... (pickpocketing_loot_template)");
    LoadLootTemplates_Pickpocketing();
    LootTemplates_Pickpocketing.CheckLootRefs();
    SendGlobalGMSysMessage("DB table pickpocketing_loot_template reloaded.");
    sConditionMgr.LoadConditions(true);
    return true;
}

bool ChatHandler::HandleReloadLootTemplatesProspectingCommand(const char*)
{
    sLog.outString("Re-Loading Loot Tables... (prospecting_loot_template)");
    LoadLootTemplates_Prospecting();
    LootTemplates_Prospecting.CheckLootRefs();
    SendGlobalGMSysMessage("DB table prospecting_loot_template reloaded.");
    sConditionMgr.LoadConditions(true);
    return true;
}

bool ChatHandler::HandleReloadLootTemplatesMailCommand(const char*)
{
    sLog.outString( "Re-Loading Loot Tables... (`mail_loot_template`)" );
    LoadLootTemplates_Mail();
    LootTemplates_Mail.CheckLootRefs();
    SendGlobalSysMessage("DB table `mail_loot_template` reloaded.");
    sConditionMgr.LoadConditions(true);
    return true;
}

bool ChatHandler::HandleReloadLootTemplatesReferenceCommand(const char*)
{
    sLog.outString("Re-Loading Loot Tables... (reference_loot_template)");
    LoadLootTemplates_Reference();
    SendGlobalGMSysMessage("DB table reference_loot_template reloaded.");
    sConditionMgr.LoadConditions(true);
    return true;
}

bool ChatHandler::HandleReloadLootTemplatesSkinningCommand(const char*)
{
    sLog.outString("Re-Loading Loot Tables... (skinning_loot_template)");
    LoadLootTemplates_Skinning();
    LootTemplates_Skinning.CheckLootRefs();
    SendGlobalGMSysMessage("DB table skinning_loot_template reloaded.");
    sConditionMgr.LoadConditions(true);
    return true;
}

bool ChatHandler::HandleReloadOregonStringCommand(const char*)
{
    sLog.outString("Re-Loading oregon_string Table!");
    sObjectMgr.LoadOregonStrings();
    SendGlobalGMSysMessage("DB table oregon_string reloaded.");
    return true;
}

bool ChatHandler::HandleReloadNpcGossipCommand(const char*)
{
    sLog.outString("Re-Loading npc_gossip Table!");
    sObjectMgr.LoadNpcTextId();
    SendGlobalGMSysMessage("DB table npc_gossip reloaded.");
    return true;
}

bool ChatHandler::HandleReloadNpcTrainerCommand(const char*)
{
    sLog.outString("Re-Loading npc_trainer Table!");
    sObjectMgr.LoadTrainerSpell();
    SendGlobalGMSysMessage("DB table npc_trainer reloaded.");
    return true;
}

bool ChatHandler::HandleReloadNpcVendorCommand(const char*)
{
    sLog.outString("Re-Loading npc_vendor Table!");
    sObjectMgr.LoadVendors();
    SendGlobalGMSysMessage("DB table npc_vendor reloaded.");
    return true;
}

bool ChatHandler::HandleReloadReservedNameCommand(const char*)
{
    sLog.outString("Loading ReservedNames... (reserved_name)");
    sObjectMgr.LoadReservedPlayersNames();
    SendGlobalGMSysMessage("DB table reserved_name (player reserved names) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadReputationSpilloverTemplateCommand(const char*)
{
    sLog.outString("Re-Loading `reputation_spillover_template` Table!");
    sObjectMgr.LoadReputationSpilloverTemplate();
    SendGlobalSysMessage("DB table `reputation_spillover_template` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSkillDiscoveryTemplateCommand(const char* /*args*/)
{
    sLog.outString("Re-Loading Skill Discovery Table...");
    LoadSkillDiscoveryTable();
    SendGlobalGMSysMessage("DB table skill_discovery_template (recipes discovered at crafting) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSkillExtraItemTemplateCommand(const char* /*args*/)
{
    sLog.outString("Re-Loading Skill Extra Item Table...");
    LoadSkillExtraItemTable();
    SendGlobalGMSysMessage("DB table skill_extra_item_template (extra item creation when crafting) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSkillFishingBaseLevelCommand(const char* /*args*/)
{
    sLog.outString("Re-Loading Skill Fishing base level requirements...");
    sObjectMgr.LoadFishingBaseSkillLevel();
    SendGlobalGMSysMessage("DB table skill_fishing_base_level (fishing base level for zone/subzone) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellAffectCommand(const char*)
{
    sLog.outString("Re-Loading SpellAffect definitions...");
    sSpellMgr.LoadSpellAffects();
    SendGlobalGMSysMessage("DB table spell_affect (spell mods apply requirements) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellRequiredCommand(const char*)
{
    sLog.outString("Re-Loading Spell Required Data... ");
    sSpellMgr.LoadSpellRequired();
    SendGlobalGMSysMessage("DB table spell_required reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellGroupsCommand(const char*)
{
    sLog.outString("Re-Loading Spell Groups...");
    sSpellMgr.LoadSpellGroups();
    SendGlobalGMSysMessage("DB table `spell_group` (spell elixir types) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellGroupStackRulesCommand(const char*)
{
    sLog.outString( "Re-Loading Spell Group Stack Rules..." );
    sSpellMgr.LoadSpellGroupStackRules();
    SendGlobalGMSysMessage("DB table `spell_group_stack_rules` (spell stacking definitions) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellLearnSpellCommand(const char*)
{
    sLog.outString("Re-Loading Spell Learn Spells...");
    sSpellMgr.LoadSpellLearnSpells();
    SendGlobalGMSysMessage("DB table spell_learn_spell reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellLinkedSpellCommand(const char*)
{
    sLog.outString("Re-Loading Spell Linked Spells...");
    sSpellMgr.LoadSpellLinked();
    SendGlobalGMSysMessage("DB table spell_linked_spell reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellProcEventCommand(const char*)
{
    sLog.outString("Re-Loading Spell Proc Event conditions...");
    sSpellMgr.LoadSpellProcEvents();
    SendGlobalGMSysMessage("DB table spell_proc_event (spell proc trigger requirements) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellTargetPositionCommand(const char*)
{
    sLog.outString("Re-Loading Spell target coordinates...");
    sSpellMgr.LoadSpellTargetPositions();
    SendGlobalGMSysMessage("DB table spell_target_position (destination coordinates for spell targets) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellThreatsCommand(const char*)
{
    sLog.outString("Re-Loading Aggro Spells Definitions...");
    sSpellMgr.LoadSpellThreats();
    SendGlobalGMSysMessage("DB table spell_threat (spell aggro definitions) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellPetAurasCommand(const char*)
{
    sLog.outString("Re-Loading Spell pet auras...");
    sSpellMgr.LoadSpellPetAuras();
    SendGlobalGMSysMessage("DB table spell_pet_auras reloaded.");
    return true;
}

bool ChatHandler::HandleReloadPageTextsCommand(const char*)
{
    sLog.outString("Re-Loading Page Texts...");
    sObjectMgr.LoadPageTexts();
    SendGlobalGMSysMessage("DB table page_texts reloaded.");
    return true;
}

bool ChatHandler::HandleReloadItemEnchantementsCommand(const char*)
{
    sLog.outString("Re-Loading Item Random Enchantments Table...");
    LoadRandomEnchantmentsTable();
    SendGlobalGMSysMessage("DB table item_enchantment_template reloaded.");
    return true;
}

bool ChatHandler::HandleReloadGameObjectScriptsCommand(const char* arg)
{
    if (sWorld.IsScriptScheduled())
    {
        SendSysMessage("DB scripts used currently, please attempt reload later.");
        SetSentErrorMessage(true);
        return false;
    }

    if (*arg != 'a')
        sLog.outString("Re-Loading Scripts from gameobject_scripts...");

    sObjectMgr.LoadGameObjectScripts();

    if (*arg != 'a')
        SendGlobalGMSysMessage("DB table gameobject_scripts reloaded.");

    return true;
}

bool ChatHandler::HandleReloadEventScriptsCommand(const char* arg)
{
    if (sWorld.IsScriptScheduled())
    {
        SendSysMessage("DB scripts used currently, please attempt reload later.");
        SetSentErrorMessage(true);
        return false;
    }

    if (*arg != 'a')
        sLog.outString("Re-Loading Scripts from event_scripts...");

    sObjectMgr.LoadEventScripts();

    if (*arg != 'a')
        SendGlobalGMSysMessage("DB table event_scripts reloaded.");

    return true;
}

bool ChatHandler::HandleReloadEventAITextsCommand(const char* /*args*/)
{

    sLog.outString("Re-Loading Texts from `creature_ai_texts`...");
    CreatureEAI_Mgr.LoadCreatureEventAI_Texts(true);
    SendGlobalSysMessage("DB table `creature_ai_texts` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadEventAISummonsCommand(const char* /*args*/)
{
    sLog.outString("Re-Loading Summons from `creature_ai_summons`...");
    CreatureEAI_Mgr.LoadCreatureEventAI_Summons(true);
    SendGlobalSysMessage("DB table `creature_ai_summons` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadEventAIScriptsCommand(const char* /*args*/)
{
    sLog.outString("Re-Loading Scripts from `creature_ai_scripts`...");
    CreatureEAI_Mgr.LoadCreatureEventAI_Scripts();
    SendGlobalSysMessage("DB table `creature_ai_scripts` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadWpScriptsCommand(const char* arg)
{
    if (sWorld.IsScriptScheduled())
    {
        SendSysMessage("DB scripts used currently, please attempt reload later.");
        SetSentErrorMessage(true);
        return false;
    }

    if (*arg != 'a')
        sLog.outString("Re-Loading Scripts from waypoint_scripts...");

    sObjectMgr.LoadWaypointScripts();

    if (*arg != 'a')
        SendGlobalGMSysMessage("DB table waypoint_scripts reloaded.");

    return true;
}

bool ChatHandler::HandleReloadQuestEndScriptsCommand(const char* arg)
{
    if (sWorld.IsScriptScheduled())
    {
        SendSysMessage("DB scripts used currently, please attempt reload later.");
        SetSentErrorMessage(true);
        return false;
    }

    if (*arg != 'a')
        sLog.outString("Re-Loading Scripts from quest_end_scripts...");

    sObjectMgr.LoadQuestEndScripts();

    if (*arg != 'a')
        SendGlobalGMSysMessage("DB table quest_end_scripts reloaded.");

    return true;
}

bool ChatHandler::HandleReloadQuestStartScriptsCommand(const char* arg)
{
    if (sWorld.IsScriptScheduled())
    {
        SendSysMessage("DB scripts used currently, please attempt reload later.");
        SetSentErrorMessage(true);
        return false;
    }

    if (*arg != 'a')
        sLog.outString("Re-Loading Scripts from quest_start_scripts...");

    sObjectMgr.LoadQuestStartScripts();

    if (*arg != 'a')
        SendGlobalGMSysMessage("DB table quest_start_scripts reloaded.");

    return true;
}

bool ChatHandler::HandleReloadSpellScriptsCommand(const char* arg)
{
    if (sWorld.IsScriptScheduled())
    {
        SendSysMessage("DB scripts used currently, please attempt reload later.");
        SetSentErrorMessage(true);
        return false;
    }

    if (*arg != 'a')
        sLog.outString("Re-Loading Scripts from spell_scripts...");

    sObjectMgr.LoadSpellScripts();

    if (*arg != 'a')
        SendGlobalGMSysMessage("DB table spell_scripts reloaded.");

    return true;
}

bool ChatHandler::HandleReloadDbScriptStringCommand(const char* /*arg*/)
{
    sLog.outString("Re-Loading Script strings from db_script_string...");
    sObjectMgr.LoadDbScriptStrings();
    SendGlobalGMSysMessage("DB table db_script_string reloaded.");
    return true;
}

bool ChatHandler::HandleReloadGameGraveyardZoneCommand(const char* /*arg*/)
{
    sLog.outString("Re-Loading Graveyard-zone links...");

    sObjectMgr.LoadGraveyardZones();

    SendGlobalGMSysMessage("DB table `graveyard_zone` reloaded.");

    return true;
}

bool ChatHandler::HandleReloadGameTeleCommand(const char* /*arg*/)
{
    sLog.outString("Re-Loading Game Tele coordinates...");

    sObjectMgr.LoadGameTele();

    SendGlobalGMSysMessage("DB table game_tele reloaded.");

    return true;
}

bool ChatHandler::HandleReloadDisablesCommand(const char* /*arg*/)
{
    sLog.outString("Re-Loading disables table...");
    sDisableMgr.LoadDisables();
    sLog.outString("Checking quest disables...");
    sDisableMgr.CheckQuestDisables();
    SendGlobalGMSysMessage("DB table `disables` reloaded.");

    return true;
}

bool ChatHandler::HandleReloadLocalesCreatureCommand(const char* /*arg*/)
{
    sLog.outString("Re-Loading Locales Creature ...");
    sObjectMgr.LoadCreatureLocales();
    SendGlobalGMSysMessage("DB table locales_creature reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLocalesGameobjectCommand(const char* /*arg*/)
{
    sLog.outString("Re-Loading Locales Gameobject ... ");
    sObjectMgr.LoadGameObjectLocales();
    SendGlobalGMSysMessage("DB table locales_gameobject reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLocalesItemCommand(const char* /*arg*/)
{
    sLog.outString("Re-Loading Locales Item ... ");
    sObjectMgr.LoadItemLocales();
    SendGlobalGMSysMessage("DB table locales_item reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLocalesNpcTextCommand(const char* /*arg*/)
{
    sLog.outString("Re-Loading Locales NPC Text ... ");
    sObjectMgr.LoadNpcTextLocales();
    SendGlobalGMSysMessage("DB table locales_npc_text reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLocalesPageTextCommand(const char* /*arg*/)
{
    sLog.outString("Re-Loading Locales Page Text ... ");
    sObjectMgr.LoadPageTextLocales();
    SendGlobalGMSysMessage("DB table locales_page_text reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLocalesQuestCommand(const char* /*arg*/)
{
    sLog.outString("Re-Loading Locales Quest ... ");
    sObjectMgr.LoadQuestLocales();
    SendGlobalGMSysMessage("DB table locales_quest reloaded.");
    return true;
}

bool ChatHandler::HandleReloadAuctionsCommand(const char* /*args*/)
{
    // Reload dynamic data tables from the database
    sLog.outString("Re-Loading Auctions...");
    sAuctionMgr->LoadAuctionItems();
    sAuctionMgr->LoadAuctions();
    SendGlobalGMSysMessage("Auctions reloaded.");
    return true;
}

bool ChatHandler::HandleReloadConditions(const char* args)
{
    sLog.outString("Re-Loading Conditions...");
    sConditionMgr.LoadConditions(true);
    SendGlobalGMSysMessage("Conditions reloaded.");
    return true;
}

bool ChatHandler::HandleAccountSetGmLevelCommand(const char* args)
{
    if (!*args)
        return false;

    std::string targetAccountName;
    uint32 targetAccountId = 0;
    uint32 targetSecurity = 0;
    int32 gm = 0;
    char* arg1 = strtok((char*)args, " ");
    char* arg2 = strtok(NULL, " ");
    char* arg3 = strtok(NULL, " ");

    if (getSelectedPlayer() && arg1 && !arg3)
    {
        targetAccountId = getSelectedPlayer()->GetSession()->GetAccountId();
        sAccountMgr->GetName(targetAccountId, targetAccountName);
        Player* targetPlayer = getSelectedPlayer();
        gm = atoi(arg1);
        uint32 gmRealmID = arg2 ? strtoul(arg2, NULL, 10) : realmID;

        // Check for invalid specified GM level.
        if (gm < SEC_PLAYER || gm > SEC_ADMINISTRATOR)
        {
            SendSysMessage(LANG_BAD_VALUE);
            SetSentErrorMessage(true);
            return false;
        }

        // Check if targets GM level and specified GM level is not higher than current gm level
        targetSecurity = targetPlayer->GetSession()->GetSecurity();
        if (targetSecurity >= m_session->GetSecurity() ||
            uint32(gm) >= m_session->GetSecurity()             ||
            (gmRealmID != realmID && m_session->GetSecurity() < SEC_CONSOLE))
        {
            SendSysMessage(LANG_YOURS_SECURITY_IS_LOW);
            SetSentErrorMessage(true);
            return false;
        }

        // Decide which string to show
        if (m_session->GetPlayer() != targetPlayer)
            PSendSysMessage(LANG_YOU_CHANGE_SECURITY, targetAccountName.c_str(), gm);
        else
            PSendSysMessage(LANG_YOURS_SECURITY_CHANGED, m_session->GetPlayer()->GetName(), gm);

        // If gmRealmID is -1, delete all values for the account id, else, insert values for the specific realmID
        if (gmRealmID == uint32(-1))
        {
            LoginDatabase.PExecute("DELETE FROM account_access WHERE id = '%u'", targetAccountId);
            LoginDatabase.PExecute("INSERT INTO account_access VALUES ('%u', '%d', -1)", targetAccountId, gm);
        }
        else
        {
            LoginDatabase.PExecute("DELETE FROM account_access WHERE id = '%u' AND RealmID = '%d'", targetAccountId, realmID);
            LoginDatabase.PExecute("INSERT INTO account_access VALUES ('%u','%d','%d')", targetAccountId, gm, realmID);
        }
        return true;
    }
    else
    {
        // Check for second parameter
        if (!arg2)
            return false;

        // Check for account
        targetAccountName = arg1;
        if (!AccountMgr::normalizeString(targetAccountName))
        {
            PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, targetAccountName.c_str());
            SetSentErrorMessage(true);
            return false;
        }

        // Check for username not exist
        targetAccountId = sAccountMgr->GetId(targetAccountName);
        if (!targetAccountId)
        {
            PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, targetAccountName.c_str());
            SetSentErrorMessage(true);
            return false;
        }

        // Check for invalid specified GM level.
        gm = atoi(arg2);
        if (gm < SEC_PLAYER || gm > SEC_ADMINISTRATOR)
        {
            SendSysMessage(LANG_BAD_VALUE);
            SetSentErrorMessage(true);
            return false;
        }

        uint32 gmRealmID = arg3 ? atoi(arg3) : realmID;
        // Check if provided realmID is not current realmID, or isn't -1
        if (gmRealmID != realmID && gmRealmID != uint32(-1))
        {
            SendSysMessage(LANG_INVALID_REALMID);
            SetSentErrorMessage(true);
            return false;
        }

        targetAccountId = sAccountMgr->GetId(arg1);
        // m_session == NULL only for console
        uint32 plSecurity = m_session ? m_session->GetSecurity() : uint32(SEC_CONSOLE);

        // can set security level only for target with less security and to less security that we have
        // This is also reject self apply in fact
        targetSecurity = sAccountMgr->GetSecurity(targetAccountId);
        if (targetSecurity >= plSecurity || uint32(gm) >= plSecurity)
        {
            SendSysMessage(LANG_YOURS_SECURITY_IS_LOW);
            SetSentErrorMessage(true);
            return false;
        }

        PSendSysMessage(LANG_YOU_CHANGE_SECURITY, targetAccountName.c_str(), gm);
        // If gmRealmID is -1, delete all values for the account id, else, insert values for the specific realmID
        if (gmRealmID == uint32(-1))
        {
            LoginDatabase.PExecute("DELETE FROM account_access WHERE id = '%u'", targetAccountId);
            LoginDatabase.PExecute("INSERT INTO account_access VALUES ('%u', '%d', -1)", targetAccountId, gm);
        }
        else
        {
            LoginDatabase.PExecute("DELETE FROM account_access WHERE id = '%u' AND RealmID = '%d'", targetAccountId, realmID);
            LoginDatabase.PExecute("INSERT INTO account_access VALUES ('%u','%d','%d')", targetAccountId, gm, realmID);
        }
        return true;
    }
}

// Set password for account
bool ChatHandler::HandleAccountSetPasswordCommand(const char* args)
{
    if (!*args)
        return false;

    // Get the command line arguments
    char* szAccount = strtok ((char*)args, " ");
    char* szPassword1 =  strtok (NULL, " ");
    char* szPassword2 =  strtok (NULL, " ");

    if (!szAccount || !szPassword1 || !szPassword2)
        return false;

    std::string account_name = szAccount;
    if (!AccountMgr::normalizeString(account_name))
    {
        PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
        SetSentErrorMessage(true);
        return false;
    }

    uint32 targetAccountId = sAccountMgr->GetId(account_name);
    if (!targetAccountId)
    {
        PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
        SetSentErrorMessage(true);
        return false;
    }

    uint32 targetSecurity = sAccountMgr->GetSecurity(targetAccountId);

    // m_session == NULL only for console
    uint32 plSecurity = m_session ? m_session->GetSecurity() : uint32(SEC_CONSOLE);

    // can set password only for target with less security
    // This is also reject self apply in fact
    if (targetSecurity >= plSecurity)
    {
        SendSysMessage (LANG_YOURS_SECURITY_IS_LOW);
        SetSentErrorMessage (true);
        return false;
    }

    if (strcmp(szPassword1, szPassword2))
    {
        SendSysMessage (LANG_NEW_PASSWORDS_NOT_MATCH);
        SetSentErrorMessage (true);
        return false;
    }

    AccountOpResult result = sAccountMgr->ChangePassword(targetAccountId, szPassword1);

    switch (result)
    {
    case AOR_OK:
        SendSysMessage(LANG_COMMAND_PASSWORD);
        break;
    case AOR_NAME_NOT_EXIST:
        PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
        SetSentErrorMessage(true);
        return false;
    case AOR_PASS_TOO_LONG:
        SendSysMessage(LANG_PASSWORD_TOO_LONG);
        SetSentErrorMessage(true);
        return false;
    default:
        SendSysMessage(LANG_COMMAND_NOTCHANGEPASSWORD);
        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

bool ChatHandler::HandleAllowMovementCommand(const char* /*args*/)
{
    if (sWorld.getAllowMovement())
    {
        sWorld.SetAllowMovement(false);
        SendSysMessage(LANG_CREATURE_MOVE_DISABLED);
    }
    else
    {
        sWorld.SetAllowMovement(true);
        SendSysMessage(LANG_CREATURE_MOVE_ENABLED);
    }
    return true;
}

bool ChatHandler::HandleMaxSkillCommand(const char* /*args*/)
{
    Player* SelectedPlayer = getSelectedPlayer();
    if (!SelectedPlayer)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    // each skills that have max skill value dependent from level seted to current level max skill value
    SelectedPlayer->UpdateSkillsToMaxSkillsForLevel();
    return true;
}

bool ChatHandler::HandleSetSkillCommand(const char* args)
{
    // number or [name] Shift-click form |color|Hskill:skill_id|h[name]|h|r
    char* skill_p = extractKeyFromLink((char*)args, "Hskill");
    if (!skill_p)
        return false;

    char* level_p = strtok (NULL, " ");

    if (!level_p)
        return false;

    char* max_p   = strtok (NULL, " ");

    int32 skill = atoi(skill_p);
    if (skill <= 0)
    {
        PSendSysMessage(LANG_INVALID_SKILL_ID, skill);
        SetSentErrorMessage(true);
        return false;
    }

    int32 level = atol (level_p);

    Player* target = getSelectedPlayerOrSelf();
    if (!target)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    SkillLineEntry const* sl = sSkillLineStore.LookupEntry(skill);
    if (!sl)
    {
        PSendSysMessage(LANG_INVALID_SKILL_ID, skill);
        SetSentErrorMessage(true);
        return false;
    }

    if (!target->GetSkillValue(skill))
    {
        PSendSysMessage(LANG_SET_SKILL_ERROR, target->GetName(), skill, sl->name[m_session->GetSessionDbcLocale()]);
        SetSentErrorMessage(true);
        return false;
    }

    int32 max   = max_p ? atol (max_p) : target->GetPureMaxSkillValue(skill);

    if (level <= 0 || level > max || max <= 0)
        return false;

    target->SetSkill(skill, level, max);
    PSendSysMessage(LANG_SET_SKILL, skill, sl->name[m_session->GetSessionDbcLocale()], target->GetName(), level, max);

    return true;
}

bool ChatHandler::HandleUnLearnCommand(const char* args)
{
    if (!*args)
        return false;

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r
    uint32 min_id = extractSpellIdFromLink((char*)args);
    if (!min_id)
        return false;

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r
    char* tail = strtok(NULL, "");

    uint32 max_id = extractSpellIdFromLink(tail);

    if (!max_id)
    {
        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r
        max_id =  min_id + 1;
    }
    else
    {
        if (max_id < min_id)
            std::swap(min_id, max_id);

        max_id = max_id + 1;
    }

    Player* target = getSelectedPlayer();
    if (!target)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    for (uint32 spell = min_id; spell < max_id; spell++)
    {
        if (target->HasSpell(spell))
            target->RemoveSpell(spell);
        else
            SendSysMessage(LANG_FORGET_SPELL);
    }

    return true;
}

bool ChatHandler::HandleCooldownCommand(const char* args)
{
    Player* target = getSelectedPlayerOrSelf();
    if (!target)
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    if (!*args)
    {
        target->RemoveAllSpellCooldown();
        PSendSysMessage(LANG_REMOVEALL_COOLDOWN, target->GetName());
    }
    else
    {
        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
        uint32 spell_id = extractSpellIdFromLink((char*)args);
        if (!spell_id)
            return false;

        if (!sSpellStore.LookupEntry(spell_id))
        {
            PSendSysMessage(LANG_UNKNOWN_SPELL, target == m_session->GetPlayer() ? GetOregonString(LANG_YOU) : target->GetName());
            SetSentErrorMessage(true);
            return false;
        }

        target->RemoveSpellCooldown(spell_id, true);
        PSendSysMessage(LANG_REMOVE_COOLDOWN, spell_id, target == m_session->GetPlayer() ? GetOregonString(LANG_YOU) : target->GetName());
    }
    return true;
}

bool ChatHandler::HandleLearnAllCommand(const char* /*args*/)
{
    static const char* allSpellList[] =
    {
        "3365",
        "6233",
        "6247",
        "6246",
        "6477",
        "6478",
        "22810",
        "8386",
        "21651",
        "21652",
        "522",
        "7266",
        "8597",
        "2479",
        "22027",
        "6603",
        "5019",
        "133",
        "168",
        "227",
        "5009",
        "9078",
        "668",
        "203",
        "20599",
        "20600",
        "81",
        "20597",
        "20598",
        "20864",
        "1459",
        "5504",
        "587",
        "5143",
        "118",
        "5505",
        "597",
        "604",
        "1449",
        "1460",
        "2855",
        "1008",
        "475",
        "5506",
        "1463",
        "12824",
        "8437",
        "990",
        "5145",
        "8450",
        "1461",
        "759",
        "8494",
        "8455",
        "8438",
        "6127",
        "8416",
        "6129",
        "8451",
        "8495",
        "8439",
        "3552",
        "8417",
        "10138",
        "12825",
        "10169",
        "10156",
        "10144",
        "10191",
        "10201",
        "10211",
        "10053",
        "10173",
        "10139",
        "10145",
        "10192",
        "10170",
        "10202",
        "10054",
        "10174",
        "10193",
        "12826",
        "2136",
        "143",
        "145",
        "2137",
        "2120",
        "3140",
        "543",
        "2138",
        "2948",
        "8400",
        "2121",
        "8444",
        "8412",
        "8457",
        "8401",
        "8422",
        "8445",
        "8402",
        "8413",
        "8458",
        "8423",
        "8446",
        "10148",
        "10197",
        "10205",
        "10149",
        "10215",
        "10223",
        "10206",
        "10199",
        "10150",
        "10216",
        "10207",
        "10225",
        "10151",
        "116",
        "205",
        "7300",
        "122",
        "837",
        "10",
        "7301",
        "7322",
        "6143",
        "120",
        "865",
        "8406",
        "6141",
        "7302",
        "8461",
        "8407",
        "8492",
        "8427",
        "8408",
        "6131",
        "7320",
        "10159",
        "8462",
        "10185",
        "10179",
        "10160",
        "10180",
        "10219",
        "10186",
        "10177",
        "10230",
        "10181",
        "10161",
        "10187",
        "10220",
        "2018",
        "2663",
        "12260",
        "2660",
        "3115",
        "3326",
        "2665",
        "3116",
        "2738",
        "3293",
        "2661",
        "3319",
        "2662",
        "9983",
        "8880",
        "2737",
        "2739",
        "7408",
        "3320",
        "2666",
        "3323",
        "3324",
        "3294",
        "22723",
        "23219",
        "23220",
        "23221",
        "23228",
        "23338",
        "10788",
        "10790",
        "5611",
        "5016",
        "5609",
        "2060",
        "10963",
        "10964",
        "10965",
        "22593",
        "22594",
        "596",
        "996",
        "499",
        "768",
        "17002",
        "1448",
        "1082",
        "16979",
        "1079",
        "5215",
        "20484",
        "5221",
        "15590",
        "17007",
        "6795",
        "6807",
        "5487",
        "1446",
        "1066",
        "5421",
        "3139",
        "779",
        "6811",
        "6808",
        "1445",
        "5216",
        "1737",
        "5222",
        "5217",
        "1432",
        "6812",
        "9492",
        "5210",
        "3030",
        "1441",
        "783",
        "6801",
        "20739",
        "8944",
        "9491",
        "22569",
        "5226",
        "6786",
        "1433",
        "8973",
        "1828",
        "9495",
        "9006",
        "6794",
        "8993",
        "5203",
        "16914",
        "6784",
        "9635",
        "22830",
        "20722",
        "9748",
        "6790",
        "9753",
        "9493",
        "9752",
        "9831",
        "9825",
        "9822",
        "5204",
        "5401",
        "22831",
        "6793",
        "9845",
        "17401",
        "9882",
        "9868",
        "20749",
        "9893",
        "9899",
        "9895",
        "9832",
        "9902",
        "9909",
        "22832",
        "9828",
        "9851",
        "9883",
        "9869",
        "17406",
        "17402",
        "9914",
        "20750",
        "9897",
        "9848",
        "3127",
        "107",
        "204",
        "9116",
        "2457",
        "78",
        "18848",
        "331",
        "403",
        "2098",
        "1752",
        "11278",
        "11288",
        "11284",
        "6461",
        "2344",
        "2345",
        "6463",
        "2346",
        "2352",
        "775",
        "1434",
        "1612",
        "71",
        "2468",
        "2458",
        "2467",
        "7164",
        "7178",
        "7367",
        "7376",
        "7381",
        "21156",
        "5209",
        "3029",
        "5201",
        "9849",
        "9850",
        "20719",
        "22568",
        "22827",
        "22828",
        "22829",
        "6809",
        "8972",
        "9005",
        "9823",
        "9827",
        "6783",
        "9913",
        "6785",
        "6787",
        "9866",
        "9867",
        "9894",
        "9896",
        "6800",
        "8992",
        "9829",
        "9830",
        "780",
        "769",
        "6749",
        "6750",
        "9755",
        "9754",
        "9908",
        "20745",
        "20742",
        "20747",
        "20748",
        "9746",
        "9745",
        "9880",
        "9881",
        "5391",
        "842",
        "3025",
        "3031",
        "3287",
        "3329",
        "1945",
        "3559",
        "4933",
        "4934",
        "4935",
        "4936",
        "5142",
        "5390",
        "5392",
        "5404",
        "5420",
        "6405",
        "7293",
        "7965",
        "8041",
        "8153",
        "9033",
        "9034",
        //"9036", problems with ghost state
        "16421",
        "21653",
        "22660",
        "5225",
        "9846",
        "2426",
        "5916",
        "6634",
        //"6718", phasing stealth, annoying for learn all case.
        "6719",
        "8822",
        "9591",
        "9590",
        "10032",
        "17746",
        "17747",
        "8203",
        "11392",
        "12495",
        "16380",
        "23452",
        "4079",
        "4996",
        "4997",
        "4998",
        "4999",
        "5000",
        "6348",
        "6349",
        "6481",
        "6482",
        "6483",
        "6484",
        "11362",
        "11410",
        "11409",
        "12510",
        "12509",
        "12885",
        "13142",
        "21463",
        "23460",
        "11421",
        "11416",
        "11418",
        "1851",
        "10059",
        "11423",
        "11417",
        "11422",
        "11419",
        "11424",
        "11420",
        "27",
        "31",
        "33",
        "34",
        "35",
        "15125",
        "21127",
        "22950",
        "1180",
        "201",
        "12593",
        "12842",
        "16770",
        "6057",
        "12051",
        "18468",
        "12606",
        "12605",
        "18466",
        "12502",
        "12043",
        "15060",
        "12042",
        "12341",
        "12848",
        "12344",
        "12353",
        "18460",
        "11366",
        "12350",
        "12352",
        "13043",
        "11368",
        "11113",
        "12400",
        "11129",
        "16766",
        "12573",
        "15053",
        "12580",
        "12475",
        "12472",
        "12953",
        "12488",
        "11189",
        "12985",
        "12519",
        "16758",
        "11958",
        "12490",
        "11426",
        "3565",
        "3562",
        "18960",
        "3567",
        "3561",
        "3566",
        "3563",
        "1953",
        "2139",
        "12505",
        "13018",
        "12522",
        "12523",
        "5146",
        "5144",
        "5148",
        "8419",
        "8418",
        "10213",
        "10212",
        "10157",
        "12524",
        "13019",
        "12525",
        "13020",
        "12526",
        "13021",
        "18809",
        "13031",
        "13032",
        "13033",
        "4036",
        "3920",
        "3919",
        "3918",
        "7430",
        "3922",
        "3923",
        "7411",
        "7418",
        "7421",
        "13262",
        "7412",
        "7415",
        "7413",
        "7416",
        "13920",
        "13921",
        "7745",
        "7779",
        "7428",
        "7457",
        "7857",
        "7748",
        "7426",
        "13421",
        "7454",
        "13378",
        "7788",
        "14807",
        "14293",
        "7795",
        "6296",
        "20608",
        "755",
        "444",
        "427",
        "428",
        "442",
        "447",
        "3578",
        "3581",
        "19027",
        "3580",
        "665",
        "3579",
        "3577",
        "6755",
        "3576",
        "2575",
        "2577",
        "2578",
        "2579",
        "2580",
        "2656",
        "2657",
        "2576",
        "3564",
        "10248",
        "8388",
        "2659",
        "14891",
        "3308",
        "3307",
        "10097",
        "2658",
        "3569",
        "16153",
        "3304",
        "10098",
        "4037",
        "3929",
        "3931",
        "3926",
        "3924",
        "3930",
        "3977",
        "3925",
        "136",
        "228",
        "5487",
        "43",
        "202",
        "0"
    };

    int loop = 0;
    while (strcmp(allSpellList[loop], "0"))
    {
        uint32 spell = atol((char*)allSpellList[loop++]);

        if (m_session->GetPlayer()->HasSpell(spell))
            continue;

        SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell);
        if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, m_session->GetPlayer()))
        {
            PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spell);
            continue;
        }

        m_session->GetPlayer()->LearnSpell(spell);
    }

    SendSysMessage(LANG_COMMAND_LEARN_MANY_SPELLS);

    return true;
}

bool ChatHandler::HandleLearnAllGMCommand(const char* /*args*/)
{
    static const char* gmSpellList[] =
    {
        "24347",                                            // Become A Fish, No Breath Bar
        "35132",                                            // Visual Boom
        "38488",                                            // Attack 4000-8000 AOE
        "38795",                                            // Attack 2000 AOE + Slow Down 90%
        "15712",                                            // Attack 200
        "1852",                                             // GM Spell Silence
        "31899",                                            // Kill
        "31924",                                            // Kill
        "29878",                                            // Kill My Self
        "26644",                                            // More Kill

        "28550",                                            //Invisible 24
        "23452",                                            //Invisible + Target
        "0"
    };

    uint16 gmSpellIter = 0;
    while (strcmp(gmSpellList[gmSpellIter], "0"))
    {
        uint32 spell = atol((char*)gmSpellList[gmSpellIter++]);

        SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell);
        if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, m_session->GetPlayer()))
        {
            PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spell);
            continue;
        }

        m_session->GetPlayer()->LearnSpell(spell);
    }

    SendSysMessage(LANG_LEARNING_GM_SKILLS);
    return true;
}

bool ChatHandler::HandleLearnAllMyClassCommand(const char* /*args*/)
{
    HandleLearnAllMyTalentsCommand("");
    HandleLearnAllMySpellsCommand("");
    return true;
}

bool ChatHandler::HandleLearnAllMySpellsCommand(const char* /*args*/)
{
    Player* player = m_session->GetPlayer();
    uint8 level = player->getLevel();
    uint32 teamID = player->GetTeamId();
    uint32 trainerID;

    switch (player->getClass())
    {
    case CLASS_WARRIOR:
        if (teamID == TEAM_ALLIANCE)
            trainerID = 17504;
        else
            trainerID = 985;
        break;
    case CLASS_ROGUE:
        if (teamID == TEAM_ALLIANCE)
            trainerID = 13283;
        else
            trainerID = 3401;
        break;
    case CLASS_SHAMAN:
        if (teamID == TEAM_ALLIANCE)
            trainerID = 20407;
        else
            trainerID = 13417;
        break;
    case CLASS_PRIEST:
        if (teamID == TEAM_ALLIANCE)
            trainerID = 11406;
        else
            trainerID = 16658;
        break;
    case CLASS_MAGE:
        if (teamID == TEAM_ALLIANCE)
            trainerID = 7312;
        else
            trainerID = 16653;
        break;
    case CLASS_WARLOCK:
        if (teamID == TEAM_ALLIANCE)
            trainerID = 5172;
        else
            trainerID = 16648;
        break;
    case CLASS_HUNTER:
        if (teamID == TEAM_ALLIANCE)
            trainerID = 5516;
        else
            trainerID = 3039;
        break;
    case CLASS_DRUID:
        if (teamID == TEAM_ALLIANCE)
            trainerID = 5504;
        else
            trainerID = 16655;
        break;
    case CLASS_PALADIN:
        if (teamID == TEAM_ALLIANCE)
            trainerID = 928;
        else
            trainerID = 16681;
        break;
    default:
        sLog.outDebug("HandleLearnAllMySpellsCommand Failed. Invalid Class %u.", player->getClass());
        return false;
    }

    QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT spell FROM npc_trainer WHERE reqlevel BETWEEN 1 AND %i AND entry = %i", level, trainerID);

    if (!result)
    {
        sLog.outErrorDb("0 spells found for HandleLearnAllMySpellsCommand function.");
        return false;
    }

    do
    {
        Field* fields = result->Fetch();

        uint32 spellID = fields[0].GetUInt32();

        SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellID);

        if (!spellInfo)
            continue;

        // skip wrong class/race skills
        if (!player->IsSpellFitByClassAndRace(spellInfo->Id))
            continue;

        // Skip known spells
        if (player->HasSpell(spellInfo->Id))
            continue;

        // Skip spells with first rank learned as talent (and all talents then also)
        if (GetTalentSpellCost(sSpellMgr.GetFirstSpellInChain(spellInfo->Id)) > 0)
            continue;

        // Skip broken spells
        if (!SpellMgr::IsSpellValid(spellInfo, player, false))
            continue;

        player->LearnSpell(spellInfo->Id);
    }
    while (result->NextRow());

    SendSysMessage(LANG_COMMAND_LEARN_CLASS_SPELLS);
    return true;
}

bool ChatHandler::HandleLearnAllMyTalentsCommand(const char* /*args*/)
{
    Player* player = m_session->GetPlayer();
    uint32 classMask = player->getClassMask();

    for (uint32 i = 0; i < sTalentStore.GetNumRows(); i++)
    {
        TalentEntry const* talentInfo = sTalentStore.LookupEntry(i);
        if (!talentInfo)
            continue;

        TalentTabEntry const* talentTabInfo = sTalentTabStore.LookupEntry(talentInfo->TalentTab);
        if (!talentTabInfo)
            continue;

        if ((classMask & talentTabInfo->ClassMask) == 0)
            continue;

        // search highest talent rank
        uint32 spellId = 0;
        int rank = 4;
        for (; rank >= 0; --rank)
        {
            if (talentInfo->RankID[rank] != 0)
            {
                spellId = talentInfo->RankID[rank];
                break;
            }
        }

        if (!spellId)                                        // ??? none spells in talent
            continue;

        SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);
        if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, m_session->GetPlayer(), false))
            continue;

        // learn highest rank of talent
        player->LearnSpellHighestRank(spellId);
    }

    player->SetFreeTalentPoints(0);

    SendSysMessage(LANG_COMMAND_LEARN_CLASS_TALENTS);
    return true;
}

bool ChatHandler::HandleLearnAllLangCommand(const char* /*args*/)
{
    // skipping UNIVERSAL language (0)
    for (uint8 i = 1; i < LANGUAGES_COUNT; ++i)
        m_session->GetPlayer()->LearnSpell(lang_description[i].spell_id);

    SendSysMessage(LANG_COMMAND_LEARN_ALL_LANG);
    return true;
}

bool ChatHandler::HandleLearnAllDefaultCommand(const char* args)
{
    char* pName = strtok((char*)args, "");
    Player* player = NULL;
    if (pName)
    {
        std::string name = pName;

        if (!normalizePlayerName(name))
        {
            SendSysMessage(LANG_PLAYER_NOT_FOUND);
            SetSentErrorMessage(true);
            return false;
        }

        player = sObjectMgr.GetPlayer(name.c_str());
    }
    else
        player = getSelectedPlayer();

    if (!player)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    player->LearnDefaultSpells();
    player->LearnQuestRewardedSpells();

    PSendSysMessage(LANG_COMMAND_LEARN_ALL_DEFAULT_AND_QUEST, player->GetName());
    return true;
}

bool ChatHandler::HandleLearnCommand(const char* args)
{
    Player* targetPlayer = getSelectedPlayerOrSelf();

    if (!targetPlayer)
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
    uint32 spell = extractSpellIdFromLink((char*)args);
    if (!spell || !sSpellStore.LookupEntry(spell))
        return false;

    if (targetPlayer->HasSpell(spell))
    {
        if (targetPlayer == m_session->GetPlayer())
            SendSysMessage(LANG_YOU_KNOWN_SPELL);
        else
            PSendSysMessage(LANG_TARGET_KNOWN_SPELL, targetPlayer->GetName());
        SetSentErrorMessage(true);
        return false;
    }

    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell);
    if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, m_session->GetPlayer()))
    {
        PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spell);
        SetSentErrorMessage(true);
        return false;
    }

    targetPlayer->LearnSpell(spell);

    return true;
}

bool ChatHandler::HandleAddItemCommand(const char* args)
{
    if (!*args)
        return false;

    uint32 itemId = 0;

    if (args[0] == '[')                                        // [name] manual form
    {
        char* citemName = strtok((char*)args, "]");

        if (citemName && citemName[0])
        {
            std::string itemName = citemName + 1;
            WorldDatabase.escape_string(itemName);
            QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT entry FROM item_template WHERE name = '%s'", itemName.c_str());
            if (!result)
            {
                PSendSysMessage(LANG_COMMAND_COULDNOTFIND, citemName + 1);
                SetSentErrorMessage(true);
                return false;
            }
            itemId = result->Fetch()->GetUInt16();
        }
        else
            return false;
    }
    else                                                    // item_id or [name] Shift-click form |color|Hitem:item_id:0:0:0|h[name]|h|r
    {
        char* cId = extractKeyFromLink((char*)args, "Hitem");
        if (!cId)
            return false;
        itemId = atol(cId);
    }

    char* ccount = strtok(NULL, " ");

    int32 count = 1;

    if (ccount)
        count = strtol(ccount, NULL, 10);

    if (count == 0)
        count = 1;

    Player* pl = m_session->GetPlayer();
    Player* plTarget = getSelectedPlayer();
    if (!plTarget)
        plTarget = pl;

    sLog.outDetail(GetOregonString(LANG_ADDITEM), itemId, count);

    ItemTemplate const* pProto = sObjectMgr.GetItemTemplate(itemId);
    if (!pProto)
    {
        PSendSysMessage(LANG_COMMAND_ITEMIDINVALID, itemId);
        SetSentErrorMessage(true);
        return false;
    }

    //Subtract
    if (count < 0)
    {
        plTarget->DestroyItemCount(itemId, -count, true, false);
        PSendSysMessage(LANG_REMOVEITEM, itemId, -count, plTarget->GetName());
        return true;
    }

    //Adding items
    uint32 noSpaceForCount = 0;

    // check space and find places
    ItemPosCountVec dest;
    uint8 msg = plTarget->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, count, &noSpaceForCount);
    if (msg != EQUIP_ERR_OK)                               // convert to possible store amount
        count -= noSpaceForCount;

    if (count == 0 || dest.empty())                         // can't add any
    {
        PSendSysMessage(LANG_ITEM_CANNOT_CREATE, itemId, noSpaceForCount);
        SetSentErrorMessage(true);
        return false;
    }

    Item* item = plTarget->StoreNewItem(dest, itemId, true, Item::GenerateItemRandomPropertyId(itemId));

    // remove binding (let GM give it to another player later)
    if (pl == plTarget)
        for (ItemPosCountVec::const_iterator itr = dest.begin(); itr != dest.end(); ++itr)
            if (Item* item1 = pl->GetItemByPos(itr->pos))
                item1->SetBinding(false);

    if (count > 0 && item)
    {
        pl->SendNewItem(item, count, false, true);
        if (pl != plTarget)
            plTarget->SendNewItem(item, count, true, false);
    }

    if (noSpaceForCount > 0)
        PSendSysMessage(LANG_ITEM_CANNOT_CREATE, itemId, noSpaceForCount);

    return true;
}

bool ChatHandler::HandleAddItemSetCommand(const char* args)
{
    if (!*args)
        return false;

    char* cId = extractKeyFromLink((char*)args, "Hitemset"); // number or [name] Shift-click form |color|Hitemset:itemset_id|h[name]|h|r
    if (!cId)
        return false;

    uint32 itemsetId = atol(cId);

    // prevent generation all items with itemset field value '0'
    if (itemsetId == 0)
    {
        PSendSysMessage(LANG_NO_ITEMS_FROM_ITEMSET_FOUND, itemsetId);
        SetSentErrorMessage(true);
        return false;
    }

    Player* pl = m_session->GetPlayer();
    Player* plTarget = getSelectedPlayer();
    if (!plTarget)
        plTarget = pl;

    sLog.outDetail(GetOregonString(LANG_ADDITEMSET), itemsetId);

    bool found = false;
    for (uint32 id = 0; id < sItemStorage.MaxEntry; id++)
    {
        ItemTemplate const* pProto = sItemStorage.LookupEntry<ItemTemplate>(id);
        if (!pProto)
            continue;

        if (pProto->ItemSet == itemsetId)
        {
            found = true;
            ItemPosCountVec dest;
            uint8 msg = plTarget->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, pProto->ItemId, 1);
            if (msg == EQUIP_ERR_OK)
            {
                Item* item = plTarget->StoreNewItem(dest, pProto->ItemId, true);

                // remove binding (let GM give it to another player later)
                if (pl == plTarget)
                    item->SetBinding(false);

                pl->SendNewItem(item, 1, false, true);
                if (pl != plTarget)
                    plTarget->SendNewItem(item, 1, true, false);
            }
            else
            {
                pl->SendEquipError(msg, NULL, NULL);
                PSendSysMessage(LANG_ITEM_CANNOT_CREATE, pProto->ItemId, 1);
            }
        }
    }

    if (!found)
    {
        PSendSysMessage(LANG_NO_ITEMS_FROM_ITEMSET_FOUND, itemsetId);

        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

bool ChatHandler::HandleListItemCommand(const char* args)
{
    if (!*args)
        return false;

    char* cId = extractKeyFromLink((char*)args, "Hitem");
    if (!cId)
        return false;

    uint32 item_id = atol(cId);
    if (!item_id)
    {
        PSendSysMessage(LANG_COMMAND_ITEMIDINVALID, item_id);
        SetSentErrorMessage(true);
        return false;
    }

    ItemTemplate const* itemProto = sObjectMgr.GetItemTemplate(item_id);
    if (!itemProto)
    {
        PSendSysMessage(LANG_COMMAND_ITEMIDINVALID, item_id);
        SetSentErrorMessage(true);
        return false;
    }

    char* c_count = strtok(NULL, " ");
    int count = c_count ? atol(c_count) : 10;

    if (count < 0)
        return false;

    // inventory case
    uint32 inv_count = 0;
    QueryResult_AutoPtr result = CharacterDatabase.PQuery("SELECT COUNT(item_template) FROM character_inventory WHERE item_template='%u'", item_id);
    if (result)
        inv_count = (*result)[0].GetUInt32();

    result = CharacterDatabase.PQuery(
                 //          0        1             2             3        4                  5
                 "SELECT ci.item, cibag.slot AS bag, ci.slot, ci.guid, characters.account,characters.name "
                 "FROM character_inventory AS ci LEFT JOIN character_inventory AS cibag ON (cibag.item=ci.bag),characters "
                 "WHERE ci.item_template='%u' AND ci.guid = characters.guid LIMIT %u ",
                 item_id, uint32(count));

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 item_guid = fields[0].GetUInt32();
            uint32 item_bag = fields[1].GetUInt32();
            uint32 item_slot = fields[2].GetUInt32();
            uint32 owner_guid = fields[3].GetUInt32();
            uint32 owner_acc = fields[4].GetUInt32();
            std::string owner_name = fields[5].GetCppString();

            char const* item_pos = 0;
            if (Player::IsEquipmentPos(item_bag, item_slot))
                item_pos = "[equipped]";
            else if (Player::IsInventoryPos(item_bag, item_slot))
                item_pos = "[in inventory]";
            else if (Player::IsBankPos(item_bag, item_slot))
                item_pos = "[in bank]";
            else
                item_pos = "";

            PSendSysMessage(LANG_ITEMLIST_SLOT,
                            item_guid, owner_name.c_str(), owner_guid, owner_acc, item_pos);
        }
        while (result->NextRow());

        int64 res_count = result->GetRowCount();

        if (count > res_count)
            count -= res_count;
        else if (count)
            count = 0;
    }

    // mail case
    uint32 mail_count = 0;
    result = CharacterDatabase.PQuery("SELECT COUNT(item_template) FROM mail_items WHERE item_template='%u'", item_id);
    if (result)
        mail_count = (*result)[0].GetUInt32();

    if (count > 0)
    {
        result = CharacterDatabase.PQuery(
                     //          0                     1            2              3               4            5               6
                     "SELECT mail_items.item_guid, mail.sender, mail.receiver, char_s.account, char_s.name, char_r.account, char_r.name "
                     "FROM mail,mail_items,characters as char_s,characters as char_r "
                     "WHERE mail_items.item_template='%u' AND char_s.guid = mail.sender AND char_r.guid = mail.receiver AND mail.id=mail_items.mail_id LIMIT %u",
                     item_id, uint32(count));
    }
    else
        result = QueryResult_AutoPtr(NULL);

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 item_guid        = fields[0].GetUInt32();
            uint32 item_s           = fields[1].GetUInt32();
            uint32 item_r           = fields[2].GetUInt32();
            uint32 item_s_acc       = fields[3].GetUInt32();
            std::string item_s_name = fields[4].GetCppString();
            uint32 item_r_acc       = fields[5].GetUInt32();
            std::string item_r_name = fields[6].GetCppString();

            char const* item_pos = "[in mail]";

            PSendSysMessage(LANG_ITEMLIST_MAIL,
                            item_guid, item_s_name.c_str(), item_s, item_s_acc, item_r_name.c_str(), item_r, item_r_acc, item_pos);
        }
        while (result->NextRow());

        int64 res_count = result->GetRowCount();

        if (count > res_count)
            count -= res_count;
        else if (count)
            count = 0;
    }

    // auction case
    uint32 auc_count = 0;
    result = CharacterDatabase.PQuery("SELECT COUNT(item_template) FROM auctionhouse WHERE item_template='%u'", item_id);
    if (result)
        auc_count = (*result)[0].GetUInt32();

    if (count > 0)
    {
        result = CharacterDatabase.PQuery(
                     //           0                      1                       2                   3
                     "SELECT  auctionhouse.itemguid, auctionhouse.itemowner, characters.account, characters.name "
                     "FROM auctionhouse,characters WHERE auctionhouse.item_template='%u' AND characters.guid = auctionhouse.itemowner LIMIT %u",
                     item_id, uint32(count));
    }
    else
        result = QueryResult_AutoPtr(NULL);

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 item_guid       = fields[0].GetUInt32();
            uint32 owner           = fields[1].GetUInt32();
            uint32 owner_acc       = fields[2].GetUInt32();
            std::string owner_name = fields[3].GetCppString();

            char const* item_pos = "[in auction]";

            PSendSysMessage(LANG_ITEMLIST_AUCTION, item_guid, owner_name.c_str(), owner, owner_acc, item_pos);
        }
        while (result->NextRow());
    }

    // guild bank case
    uint32 guild_count = 0;
    result = CharacterDatabase.PQuery("SELECT COUNT(item_entry) FROM guild_bank_item WHERE item_entry='%u'", item_id);
    if (result)
        guild_count = (*result)[0].GetUInt32();

    result = CharacterDatabase.PQuery(
                 //      0             1           2
                 "SELECT gi.item_guid, gi.guildid, guild.name "
                 "FROM guild_bank_item AS gi, guild WHERE gi.item_entry='%u' AND gi.guildid = guild.guildid LIMIT %u ",
                 item_id, uint32(count));

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 item_guid = fields[0].GetUInt32();
            uint32 guild_guid = fields[1].GetUInt32();
            std::string guild_name = fields[2].GetCppString();

            char const* item_pos = "[in guild bank]";

            PSendSysMessage(LANG_ITEMLIST_GUILD, item_guid, guild_name.c_str(), guild_guid, item_pos);
        }
        while (result->NextRow());

        int64 res_count = result->GetRowCount();

        if (count > res_count)
            count -= res_count;
        else if (count)
            count = 0;
    }

    if (inv_count + mail_count + auc_count + guild_count == 0)
    {
        SendSysMessage(LANG_COMMAND_NOITEMFOUND);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_COMMAND_LISTITEMMESSAGE, item_id, inv_count + mail_count + auc_count + guild_count, inv_count, mail_count, auc_count, guild_count);

    return true;
}

bool ChatHandler::HandleListObjectCommand(const char* args)
{
    if (!*args)
        return false;

    // number or [name] Shift-click form |color|Hgameobject_entry:go_id|h[name]|h|r
    char* cId = extractKeyFromLink((char*)args, "Hgameobject_entry");
    if (!cId)
        return false;

    uint32 go_id = atol(cId);
    if (!go_id)
    {
        PSendSysMessage(LANG_COMMAND_LISTOBJINVALIDID, go_id);
        SetSentErrorMessage(true);
        return false;
    }

    GameObjectInfo const* gInfo = sObjectMgr.GetGameObjectInfo(go_id);
    if (!gInfo)
    {
        PSendSysMessage(LANG_COMMAND_LISTOBJINVALIDID, go_id);
        SetSentErrorMessage(true);
        return false;
    }

    char* c_count = strtok(NULL, " ");
    int count = c_count ? atol(c_count) : 10;

    if (count < 0)
        return false;

    uint32 obj_count = 0;
    QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT COUNT(guid) FROM gameobject WHERE id='%u'", go_id);
    if (result)
        obj_count = (*result)[0].GetUInt32();

    if (m_session)
    {
        Player* pl = m_session->GetPlayer();
        result = WorldDatabase.PQuery("SELECT guid, position_x, position_y, position_z, map, id, (POW(position_x - '%f', 2) + POW(position_y - '%f', 2) + POW(position_z - '%f', 2)) AS order_ FROM gameobject WHERE id = '%u' ORDER BY order_ ASC LIMIT %u",
                                      pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(), go_id, uint32(count));
    }
    else
        result = WorldDatabase.PQuery("SELECT guid, position_x, position_y, position_z, map, id FROM gameobject WHERE id = '%u' LIMIT %u",
                                      go_id, uint32(count));

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 guid = fields[0].GetUInt32();
            float x = fields[1].GetFloat();
            float y = fields[2].GetFloat();
            float z = fields[3].GetFloat();
            int mapid = fields[4].GetUInt16();
            uint32 entry = fields[5].GetUInt32();

            if (m_session)
                PSendSysMessage(LANG_GO_LIST_CHAT, guid, entry, guid, gInfo->name, x, y, z, mapid);
            else
                PSendSysMessage(LANG_GO_LIST_CONSOLE, guid, gInfo->name, x, y, z, mapid);
        }
        while (result->NextRow());
    }

    PSendSysMessage(LANG_COMMAND_LISTOBJMESSAGE, go_id, obj_count);
    return true;
}

bool ChatHandler::HandleNearObjectCommand(const char* args)
{
    float distance = (!*args) ? 10 : atol(args);
    uint32 count = 0;

    Player* pl = m_session->GetPlayer();
    QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT guid, id, position_x, position_y, position_z, map, "
                                 "(POW(position_x - '%f', 2) + POW(position_y - '%f', 2) + POW(position_z - '%f', 2)) AS order_ "
                                 "FROM gameobject WHERE map='%u' AND (POW(position_x - '%f', 2) + POW(position_y - '%f', 2) + POW(position_z - '%f', 2)) <= '%f' ORDER BY order_",
                                 pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(),
                                 pl->GetMapId(), pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(), distance * distance);

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 guid = fields[0].GetUInt32();
            uint32 entry = fields[1].GetUInt32();
            float x = fields[2].GetFloat();
            float y = fields[3].GetFloat();
            float z = fields[4].GetFloat();
            int mapid = fields[5].GetUInt16();

            GameObjectInfo const* gInfo = sObjectMgr.GetGameObjectInfo(entry);

            if (!gInfo)
                continue;

            PSendSysMessage(LANG_GO_LIST_CHAT, guid, entry, guid, gInfo->name, x, y, z, mapid);

            ++count;
        }
        while (result->NextRow());
    }

    PSendSysMessage(LANG_COMMAND_NEAROBJMESSAGE, distance, count);
    return true;
}

bool ChatHandler::HandleObjectStateCommand(const char* args)
{
    // number or [name] Shift-click form |color|Hgameobject:go_id|h[name]|h|r
    char* cId = extractKeyFromLink((char*)args, "Hgameobject");
    if (!cId)
        return false;

    uint32 lowguid = atoi(cId);
    if (!lowguid)
        return false;

    GameObject* gobj = NULL;

    if (GameObjectData const* goData = sObjectMgr.GetGOData(lowguid))
        gobj = GetObjectGlobalyWithGuidOrNearWithDbGuid(lowguid, goData->id);

    if (!gobj)
    {
        PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, lowguid);
        SetSentErrorMessage(true);
        return false;
    }

    char* cstate = strtok(NULL, " ");
    if (!cstate)
        return false;

    int32 state = atoi(cstate);
    if (state < 0)
        gobj->SendObjectDeSpawnAnim(gobj->GetGUID());
    else
        gobj->SetGoState((GOState)state);

    return true;

    return true;
}

bool ChatHandler::HandleListCreatureCommand(const char* args)
{
    if (!*args)
        return false;

    // number or [name] Shift-click form |color|Hcreature_entry:creature_id|h[name]|h|r
    char* cId = extractKeyFromLink((char*)args, "Hcreature_entry");
    if (!cId)
        return false;

    uint32 cr_id = atol(cId);
    if (!cr_id)
    {
        PSendSysMessage(LANG_COMMAND_INVALIDCREATUREID, cr_id);
        SetSentErrorMessage(true);
        return false;
    }

    CreatureInfo const* cInfo = sObjectMgr.GetCreatureTemplate(cr_id);
    if (!cInfo)
    {
        PSendSysMessage(LANG_COMMAND_INVALIDCREATUREID, cr_id);
        SetSentErrorMessage(true);
        return false;
    }

    char* c_count = strtok(NULL, " ");
    int count = c_count ? atol(c_count) : 10;

    if (count < 0)
        return false;

    uint32 cr_count = 0;
    QueryResult_AutoPtr result = WorldDatabase.PQuery("SELECT COUNT(guid) FROM creature WHERE id='%u'", cr_id);
    if (result)
        cr_count = (*result)[0].GetUInt32();

    if (m_session)
    {
        Player* pl = m_session->GetPlayer();
        result = WorldDatabase.PQuery("SELECT guid, position_x, position_y, position_z, map, (POW(position_x - '%f', 2) + POW(position_y - '%f', 2) + POW(position_z - '%f', 2)) AS order_ FROM creature WHERE id = '%u' ORDER BY order_ ASC LIMIT %u",
                                      pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(), cr_id, uint32(count));
    }
    else
        result = WorldDatabase.PQuery("SELECT guid, position_x, position_y, position_z, map FROM creature WHERE id = '%u' LIMIT %u",
                                      cr_id, uint32(count));

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 guid = fields[0].GetUInt32();
            float x = fields[1].GetFloat();
            float y = fields[2].GetFloat();
            float z = fields[3].GetFloat();
            int mapid = fields[4].GetUInt16();

            if  (m_session)
                PSendSysMessage(LANG_CREATURE_LIST_CHAT, guid, guid, cInfo->Name, x, y, z, mapid);
            else
                PSendSysMessage(LANG_CREATURE_LIST_CONSOLE, guid, cInfo->Name, x, y, z, mapid);
        }
        while (result->NextRow());
    }

    PSendSysMessage(LANG_COMMAND_LISTCREATUREMESSAGE, cr_id, cr_count);
    return true;
}

bool ChatHandler::HandleLookupItemCommand(const char* args)
{
    if (!*args)
        return false;

    std::string namepart = args;
    std::wstring wnamepart;

    // converting string that we try to find to lower case
    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    wstrToLower(wnamepart);

    bool found = false;
    uint32 count = 0;
    uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

    // Search in item_template
    for (uint32 id = 0; id < sItemStorage.MaxEntry; id++)
    {
        ItemTemplate const* pProto = sItemStorage.LookupEntry<ItemTemplate >(id);
        if (!pProto)
            continue;

        int loc_idx = m_session ? m_session->GetSessionDbLocaleIndex() : sObjectMgr.GetDBCLocaleIndex();
        if (loc_idx >= 0)
        {
            ItemLocale const* il = sObjectMgr.GetItemLocale(pProto->ItemId);
            if (il)
            {
                if (il->Name.size() > uint32(loc_idx) && !il->Name[loc_idx].empty())
                {
                    std::string name = il->Name[loc_idx];

                    if (Utf8FitTo(name, wnamepart))
                    {
                        if (maxResults && count++ == maxResults)
                        {
                            PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                            return true;
                        }

                        if (m_session)
                            PSendSysMessage(LANG_ITEM_LIST_CHAT, id, id, name.c_str());
                        else
                            PSendSysMessage(LANG_ITEM_LIST_CONSOLE, id, name.c_str());

                        if (!found)
                            found = true;

                        continue;
                    }
                }
            }
        }

        std::string name = pProto->Name1;
        if (name.empty())
            continue;

        if (Utf8FitTo(name, wnamepart))
        {
            if (maxResults && count++ == maxResults)
            {
                PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                return true;
            }

            if (m_session)
                PSendSysMessage(LANG_ITEM_LIST_CHAT, id, id, name.c_str());
            else
                PSendSysMessage(LANG_ITEM_LIST_CONSOLE, id, name.c_str());

            if (!found)
                found = true;
        }
    }

    if (!found)
        SendSysMessage(LANG_COMMAND_NOITEMFOUND);

    return true;
}

bool ChatHandler::HandleLookupItemSetCommand(const char* args)
{
    if (!*args)
        return false;

    std::string namepart = args;
    std::wstring wnamepart;

    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    // converting string that we try to find to lower case
    wstrToLower(wnamepart);

    bool found = false;
    uint32 count = 0;
    uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

    // Search in ItemSet.dbc
    for (uint32 id = 0; id < sItemSetStore.GetNumRows(); id++)
    {
        ItemSetEntry const* set = sItemSetStore.LookupEntry(id);
        if (set)
        {
            int loc = m_session ? int(m_session->GetSessionDbcLocale()) : sWorld.GetDefaultDbcLocale();
            std::string name = set->name[loc];
            if (name.empty())
                continue;

            if (!Utf8FitTo(name, wnamepart))
            {
                loc = 0;
                for (; loc < MAX_LOCALE; ++loc)
                {
                    if (m_session && loc == m_session->GetSessionDbcLocale())
                        continue;

                    name = set->name[loc];
                    if (name.empty())
                        continue;

                    if (Utf8FitTo(name, wnamepart))
                        break;
                }
            }

            if (loc < MAX_LOCALE)
            {
                if (maxResults && count++ == maxResults)
                {
                    PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                    return true;
                }

                // send item set in "id - [namedlink locale]" format
                if (m_session)
                    PSendSysMessage(LANG_ITEMSET_LIST_CHAT, id, id, name.c_str(), localeNames[loc]);
                else
                    PSendSysMessage(LANG_ITEMSET_LIST_CONSOLE, id, name.c_str(), localeNames[loc]);

                if (!found)
                    found = true;
            }
        }
    }
    if (!found)
        SendSysMessage(LANG_COMMAND_NOITEMSETFOUND);
    return true;
}

bool ChatHandler::HandleLookupSkillCommand(const char* args)
{
    if (!*args)
        return false;

    // can be NULL in console call
    Player* target = getSelectedPlayerOrSelf();

    std::string namepart = args;
    std::wstring wnamepart;

    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    // converting string that we try to find to lower case
    wstrToLower(wnamepart);

    bool found = false;
    uint32 count = 0;
    uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

    // Search in SkillLine.dbc
    for (uint32 id = 0; id < sSkillLineStore.GetNumRows(); id++)
    {
        SkillLineEntry const* skillInfo = sSkillLineStore.LookupEntry(id);
        if (skillInfo)
        {
            int loc = m_session ? int(m_session->GetSessionDbcLocale()) : sWorld.GetDefaultDbcLocale();
            std::string name = skillInfo->name[loc];
            if (name.empty())
                continue;

            if (!Utf8FitTo(name, wnamepart))
            {
                loc = 0;
                for (; loc < MAX_LOCALE; ++loc)
                {
                    if (m_session && loc == m_session->GetSessionDbcLocale())
                        continue;

                    name = skillInfo->name[loc];
                    if (name.empty())
                        continue;

                    if (Utf8FitTo(name, wnamepart))
                        break;
                }
            }

            if (loc < MAX_LOCALE)
            {
                if (maxResults && count++ == maxResults)
                {
                    PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                    return true;
                }

                char const* knownStr = "";
                if (target && target->HasSkill(id))
                    knownStr = GetOregonString(LANG_KNOWN);

                // send skill in "id - [namedlink locale]" format
                if (m_session)
                    PSendSysMessage(LANG_SKILL_LIST_CHAT, id, id, name.c_str(), localeNames[loc], knownStr);
                else
                    PSendSysMessage(LANG_SKILL_LIST_CONSOLE, id, name.c_str(), localeNames[loc], knownStr);

                if (!found)
                    found = true;
            }
        }
    }
    if (!found)
        SendSysMessage(LANG_COMMAND_NOSKILLFOUND);
    return true;
}

bool ChatHandler::HandleLookupSpellCommand(const char* args)
{
    if (!*args)
        return false;

    // can be NULL at console call
    Player* target = getSelectedPlayer();

    std::string namepart = args;
    std::wstring wnamepart;

    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    // converting string that we try to find to lower case
    wstrToLower(wnamepart);

    bool found = false;
    uint32 count = 0;
    uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

    // Search in Spell.dbc
    for (uint32 id = 0; id < sSpellStore.GetNumRows(); id++)
    {
        SpellEntry const* spellInfo = sSpellStore.LookupEntry(id);
        if (spellInfo)
        {
            int loc = m_session ? int(m_session->GetSessionDbcLocale()) : sWorld.GetDefaultDbcLocale();
            std::string name = spellInfo->SpellName[loc];
            if (name.empty())
                continue;

            if (!Utf8FitTo(name, wnamepart))
            {
                loc = 0;
                for (; loc < MAX_LOCALE; ++loc)
                {
                    if (m_session && loc == m_session->GetSessionDbcLocale())
                        continue;

                    name = spellInfo->SpellName[loc];
                    if (name.empty())
                        continue;

                    if (Utf8FitTo(name, wnamepart))
                        break;
                }
            }

            if (loc < MAX_LOCALE)
            {
                if (maxResults && count++ == maxResults)
                {
                    PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                    return true;
                }

                bool known = target && target->HasSpell(id);
                bool learn = (spellInfo->Effect[0] == SPELL_EFFECT_LEARN_SPELL);

                uint32 talentCost = GetTalentSpellCost(id);

                bool talent = (talentCost > 0);
                bool passive = IsPassiveSpell(id);
                bool active = target && (target->HasAura(id, 0) || target->HasAura(id, 1) || target->HasAura(id, 2));

                // unit32 used to prevent interpreting uint8 as char at output
                // find rank of learned spell for learning spell, or talent rank
                uint32 rank = talentCost ? talentCost : sSpellMgr.GetSpellRank(learn ? spellInfo->EffectTriggerSpell[0] : id);

                // send spell in "id - [name, rank N] [talent] [passive] [learn] [known]" format
                std::ostringstream ss;
                if (m_session)
                    ss << id << " - |cffffffff|Hspell:" << id << "|h[" << name;
                else
                    ss << id << " - " << name;

                // include rank in link name
                if (rank)
                    ss << GetOregonString(LANG_SPELL_RANK) << rank;

                if (m_session)
                    ss << " " << localeNames[loc] << "]|h|r";
                else
                    ss << " " << localeNames[loc];

                if (talent)
                    ss << GetOregonString(LANG_TALENT);
                if (passive)
                    ss << GetOregonString(LANG_PASSIVE);
                if (learn)
                    ss << GetOregonString(LANG_LEARN);
                if (known)
                    ss << GetOregonString(LANG_KNOWN);
                if (active)
                    ss << GetOregonString(LANG_ACTIVE);

                SendSysMessage(ss.str().c_str());

                if (!found)
                    found = true;
            }
        }
    }
    if (!found)
        SendSysMessage(LANG_COMMAND_NOSPELLFOUND);
    return true;
}

bool ChatHandler::HandleLookupQuestCommand(const char* args)
{
    if (!*args)
        return false;

    // can be NULL at console call
    Player* target = getSelectedPlayer();

    std::string namepart = args;
    std::wstring wnamepart;

    // converting string that we try to find to lower case
    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    wstrToLower(wnamepart);

    bool found = false;
    uint32 count = 0;
    uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

    ObjectMgr::QuestMap const& qTemplates = sObjectMgr.GetQuestTemplates();
    for (ObjectMgr::QuestMap::const_iterator iter = qTemplates.begin(); iter != qTemplates.end(); ++iter)
    {
        Quest* qinfo = iter->second;

        int loc_idx = m_session ? m_session->GetSessionDbLocaleIndex() : sObjectMgr.GetDBCLocaleIndex();
        if (loc_idx >= 0)
        {
            QuestLocale const* il = sObjectMgr.GetQuestLocale(qinfo->GetQuestId());
            if (il)
            {
                if (il->Title.size() > uint32(loc_idx) && !il->Title[loc_idx].empty())
                {
                    std::string title = il->Title[loc_idx];

                    if (Utf8FitTo(title, wnamepart))
                    {
                        if (maxResults && count++ == maxResults)
                        {
                            PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                            return true;
                        }

                        char const* statusStr = "";

                        if (target)
                        {
                            QuestStatus status = target->GetQuestStatus(qinfo->GetQuestId());

                            if (status == QUEST_STATUS_COMPLETE)
                            {
                                if (target->GetQuestRewardStatus(qinfo->GetQuestId()))
                                    statusStr = GetOregonString(LANG_COMMAND_QUEST_REWARDED);
                                else
                                    statusStr = GetOregonString(LANG_COMMAND_QUEST_COMPLETE);
                            }
                            else if (status == QUEST_STATUS_INCOMPLETE)
                                statusStr = GetOregonString(LANG_COMMAND_QUEST_ACTIVE);
                        }

                        if (m_session)
                            PSendSysMessage(LANG_QUEST_LIST_CHAT, qinfo->GetQuestId(), qinfo->GetQuestId(), title.c_str(), statusStr);
                        else
                            PSendSysMessage(LANG_QUEST_LIST_CONSOLE, qinfo->GetQuestId(), title.c_str(), statusStr);

                        if (!found)
                            found = true;

                        continue;
                    }
                }
            }
        }

        std::string title = qinfo->GetTitle();
        if (title.empty())
            continue;

        if (Utf8FitTo(title, wnamepart))
        {
            if (maxResults && count++ == maxResults)
            {
                PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                return true;
            }

            char const* statusStr = "";

            if (target)
            {
                QuestStatus status = target->GetQuestStatus(qinfo->GetQuestId());

                if (status == QUEST_STATUS_COMPLETE)
                {
                    if (target->GetQuestRewardStatus(qinfo->GetQuestId()))
                        statusStr = GetOregonString(LANG_COMMAND_QUEST_REWARDED);
                    else
                        statusStr = GetOregonString(LANG_COMMAND_QUEST_COMPLETE);
                }
                else if (status == QUEST_STATUS_INCOMPLETE)
                    statusStr = GetOregonString(LANG_COMMAND_QUEST_ACTIVE);
            }

            if (m_session)
                PSendSysMessage(LANG_QUEST_LIST_CHAT, qinfo->GetQuestId(), qinfo->GetQuestId(), title.c_str(), statusStr);
            else
                PSendSysMessage(LANG_QUEST_LIST_CONSOLE, qinfo->GetQuestId(), title.c_str(), statusStr);

            if (!found)
                found = true;
        }
    }

    if (!found)
        SendSysMessage(LANG_COMMAND_NOQUESTFOUND);

    return true;
}

bool ChatHandler::HandleLookupCreatureCommand(const char* args)
{
    if (!*args)
        return false;

    std::string namepart = args;
    std::wstring wnamepart;

    // converting string that we try to find to lower case
    if (!Utf8toWStr (namepart, wnamepart))
        return false;

    wstrToLower (wnamepart);

    bool found = false;
    uint32 count = 0;
    uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

    for (uint32 id = 0; id < sCreatureStorage.MaxEntry; ++id)
    {
        CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo> (id);
        if (!cInfo)
            continue;

        int loc_idx = m_session ? m_session->GetSessionDbLocaleIndex() : sObjectMgr.GetDBCLocaleIndex();
        if (loc_idx >= 0)
        {
            CreatureLocale const* cl = sObjectMgr.GetCreatureLocale (id);
            if (cl)
            {
                if (cl->Name.size() > uint32(loc_idx) && !cl->Name[loc_idx].empty ())
                {
                    std::string name = cl->Name[loc_idx];

                    if (Utf8FitTo (name, wnamepart))
                    {
                        if (maxResults && count++ == maxResults)
                        {
                            PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                            return true;
                        }

                        if (m_session)
                            PSendSysMessage (LANG_CREATURE_ENTRY_LIST_CHAT, id, id, name.c_str ());
                        else
                            PSendSysMessage (LANG_CREATURE_ENTRY_LIST_CONSOLE, id, name.c_str ());

                        if (!found)
                            found = true;

                        continue;
                    }
                }
            }
        }

        std::string name = cInfo->Name;
        if (name.empty ())
            continue;

        if (Utf8FitTo(name, wnamepart))
        {
            if (maxResults && count++ == maxResults)
            {
                PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                return true;
            }

            if (m_session)
                PSendSysMessage (LANG_CREATURE_ENTRY_LIST_CHAT, id, id, name.c_str ());
            else
                PSendSysMessage (LANG_CREATURE_ENTRY_LIST_CONSOLE, id, name.c_str ());

            if (!found)
                found = true;
        }
    }

    if (!found)
        SendSysMessage (LANG_COMMAND_NOCREATUREFOUND);

    return true;
}

bool ChatHandler::HandleLookupObjectCommand(const char* args)
{
    if (!*args)
        return false;

    std::string namepart = args;
    std::wstring wnamepart;

    // converting string that we try to find to lower case
    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    wstrToLower(wnamepart);

    bool found = false;
    uint32 count = 0;
    uint32 maxResults = sWorld.getConfig(CONFIG_MAX_RESULTS_LOOKUP_COMMANDS);

    for (uint32 id = 0; id < sGOStorage.MaxEntry; id++)
    {
        GameObjectInfo const* gInfo = sGOStorage.LookupEntry<GameObjectInfo>(id);
        if (!gInfo)
            continue;

        int loc_idx = m_session ? m_session->GetSessionDbLocaleIndex() : sObjectMgr.GetDBCLocaleIndex();
        if (loc_idx >= 0)
        {
            GameObjectLocale const* gl = sObjectMgr.GetGameObjectLocale(id);
            if (gl)
            {
                if (gl->Name.size() > uint32(loc_idx) && !gl->Name[loc_idx].empty())
                {
                    std::string name = gl->Name[loc_idx];

                    if (Utf8FitTo(name, wnamepart))
                    {
                        if (maxResults && count++ == maxResults)
                        {
                            PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                            return true;
                        }

                        if (m_session)
                            PSendSysMessage(LANG_GO_ENTRY_LIST_CHAT, id, id, name.c_str());
                        else
                            PSendSysMessage(LANG_GO_ENTRY_LIST_CONSOLE, id, name.c_str());

                        if (!found)
                            found = true;

                        continue;
                    }
                }
            }
        }

        std::string name = gInfo->name;
        if (name.empty())
            continue;

        if (Utf8FitTo(name, wnamepart))
        {
            if (maxResults && count++ == maxResults)
            {
                PSendSysMessage(LANG_COMMAND_LOOKUP_MAX_RESULTS, maxResults);
                return true;
            }

            if (m_session)
                PSendSysMessage(LANG_GO_ENTRY_LIST_CHAT, id, id, name.c_str());
            else
                PSendSysMessage(LANG_GO_ENTRY_LIST_CONSOLE, id, name.c_str());

            if (!found)
                found = true;
        }
    }

    if (!found)
        SendSysMessage(LANG_COMMAND_NOGAMEOBJECTFOUND);

    return true;
}

/** \brief GM command level 3 - Create a guild.
 *
 * This command allows a GM (level 3) to create a guild.
 *
 * The "args" parameter contains the name of the guild leader
 * and then the name of the guild.
 *
 */
bool ChatHandler::HandleGuildCreateCommand(const char* args)
{
    if (!*args)
        return false;

    char* lname = strtok ((char*)args, " ");
    char* gname = strtok (NULL, "");

    if (!lname)
        return false;

    if (!gname)
    {
        SendSysMessage (LANG_INSERT_GUILD_NAME);
        SetSentErrorMessage (true);
        return false;
    }

    std::string guildname = gname;

    Player* player = ObjectAccessor::Instance ().FindPlayerByName (lname);
    if (!player)
    {
        SendSysMessage (LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage (true);
        return false;
    }

    if (player->GetGuildId())
    {
        SendSysMessage (LANG_PLAYER_IN_GUILD);
        return true;
    }

    Guild* guild = new Guild;
    if (!guild->Create(player, guildname))
    {
        delete guild;
        SendSysMessage (LANG_GUILD_NOT_CREATED);
        SetSentErrorMessage (true);
        return false;
    }

    sObjectMgr.AddGuild (guild);
    return true;
}

bool ChatHandler::HandleGuildInviteCommand(const char* args)
{
    if (!*args)
        return false;

    char* par1 = strtok ((char*)args, " ");
    char* par2 = strtok (NULL, "");
    if (!par1 || !par2)
        return false;

    std::string glName = par2;
    Guild* targetGuild = sObjectMgr.GetGuildByName (glName);
    if (!targetGuild)
        return false;

    std::string plName = par1;
    if (!normalizePlayerName (plName))
    {
        SendSysMessage (LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage (true);
        return false;
    }

    uint64 plGuid = 0;
    if (Player* targetPlayer = ObjectAccessor::Instance ().FindPlayerByName (plName.c_str ()))
        plGuid = targetPlayer->GetGUID ();
    else
        plGuid = sObjectMgr.GetPlayerGUIDByName (plName.c_str ());

    if (!plGuid)
        return false;

    // player's guild membership checked in AddMember before add
    if (!targetGuild->AddMember (plGuid, targetGuild->GetLowestRank ()))
        return false;

    return true;
}

bool ChatHandler::HandleGuildUninviteCommand(const char* args)
{
    if (!*args)
        return false;

    char* par1 = strtok ((char*)args, " ");
    if (!par1)
        return false;

    std::string plName = par1;
    if (!normalizePlayerName (plName))
    {
        SendSysMessage (LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage (true);
        return false;
    }

    uint64 plGuid = 0;
    uint32 glId   = 0;
    if (Player* targetPlayer = ObjectAccessor::Instance ().FindPlayerByName (plName.c_str ()))
    {
        plGuid = targetPlayer->GetGUID ();
        glId   = targetPlayer->GetGuildId ();
    }
    else
    {
        plGuid = sObjectMgr.GetPlayerGUIDByName (plName.c_str ());
        glId = Player::GetGuildIdFromDB (plGuid);
    }

    if (!plGuid || !glId)
        return false;

    Guild* targetGuild = sObjectMgr.GetGuildById (glId);
    if (!targetGuild)
        return false;

    targetGuild->DelMember (plGuid);
    return true;
}

bool ChatHandler::HandleGuildRankCommand(const char* args)
{
    if (!*args)
        return false;

    char* par1 = strtok ((char*)args, " ");
    char* par2 = strtok (NULL, " ");
    if (!par1 || !par2)
        return false;
    std::string plName = par1;
    if (!normalizePlayerName (plName))
    {
        SendSysMessage (LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage (true);
        return false;
    }

    uint64 plGuid = 0;
    uint32 glId   = 0;
    if (Player* targetPlayer = ObjectAccessor::Instance ().FindPlayerByName (plName.c_str ()))
    {
        plGuid = targetPlayer->GetGUID ();
        glId   = targetPlayer->GetGuildId ();
    }
    else
    {
        plGuid = sObjectMgr.GetPlayerGUIDByName (plName.c_str ());
        glId = Player::GetGuildIdFromDB (plGuid);
    }

    if (!plGuid || !glId)
        return false;

    Guild* targetGuild = sObjectMgr.GetGuildById (glId);
    if (!targetGuild)
        return false;

    uint32 newrank = uint32 (atoi (par2));
    if (newrank > targetGuild->GetLowestRank ())
        return false;

    targetGuild->ChangeRank (plGuid, newrank);
    return true;
}

bool ChatHandler::HandleGuildDeleteCommand(const char* args)
{
    if (!*args)
        return false;

    char* par1 = strtok ((char*)args, " ");
    if (!par1)
        return false;

    std::string gld = par1;

    Guild* targetGuild = sObjectMgr.GetGuildByName (gld);
    if (!targetGuild)
        return false;

    targetGuild->Disband();
    delete targetGuild;

    return true;
}

bool ChatHandler::HandleGetDistanceCommand(const char* /*args*/)
{
    Unit* pUnit = getSelectedUnit();

    if (!pUnit)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_DISTANCE, m_session->GetPlayer()->GetDistance(pUnit), m_session->GetPlayer()->GetDistance2d(pUnit));

    return true;
}

// FIX-ME!!!

bool ChatHandler::HandleAddWeaponCommand(const char* /*args*/)
{
    /*if (!*args)
        return false;

    uint64 guid = m_session->GetPlayer()->GetSelection();
    if (guid == 0)
    {
        SendSysMessage(LANG_NO_SELECTION);
        return true;
    }

    Creature* pCreature = ObjectAccessor::GetCreature(*m_session->GetPlayer(), guid);

    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        return true;
    }

    char* pSlotID = strtok((char*)args, " ");
    if (!pSlotID)
        return false;

    char* pItemID = strtok(NULL, " ");
    if (!pItemID)
        return false;

    uint32 ItemID = atoi(pItemID);
    uint32 SlotID = atoi(pSlotID);

    ItemTemplate* tmpItem = sObjectMgr.GetItemTemplate(ItemID);

    bool added = false;
    if (tmpItem)
    {
        switch(SlotID)
        {
            case 1:
                pCreature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, ItemID);
                added = true;
                break;
            case 2:
                pCreature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY_01, ItemID);
                added = true;
                break;
            case 3:
                pCreature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY_02, ItemID);
                added = true;
                break;
            default:
                PSendSysMessage(LANG_ITEM_SLOT_NOT_EXIST,SlotID);
                added = false;
                break;
        }
        if (added)
        {
            PSendSysMessage(LANG_ITEM_ADDED_TO_SLOT,ItemID,tmpItem->Name1,SlotID);
        }
    }
    else
    {
        PSendSysMessage(LANG_ITEM_NOT_FOUND,ItemID);
        return true;
    }
    */
    return true;
}

bool ChatHandler::HandleDieCommand(const char* /*args*/)
{
    Unit* target = getSelectedUnit();

    if (!target || !m_session->GetPlayer()->GetSelection())
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    if (target->IsAlive())
    {
        if (sWorld.getConfig(CONFIG_DIE_COMMAND_MODE))
            target->CastSpell(target, SPELL_SUICIDE, true);
        else
            m_session->GetPlayer()->CastSpell(target, SPELL_DEATH_TOUCH, true);
    }

    return true;
}

bool ChatHandler::HandleDamageCommand(const char* args)
{
    if (!*args)
        return false;

    Unit* target = getSelectedUnit();

    if (!target || !m_session->GetPlayer()->GetSelection())
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    if (!target->IsAlive())
        return true;

    char* damageStr = strtok((char*)args, " ");
    if (!damageStr)
        return false;

    int32 damage = atoi((char*)damageStr);
    if (damage <= 0)
        return true;

    char* schoolStr = strtok((char*)NULL, " ");

    // flat melee damage without resistence/etc reduction
    if (!schoolStr)
    {
        m_session->GetPlayer()->DealDamage(target, damage, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        m_session->GetPlayer()->SendAttackStateUpdate (HITINFO_NORMALSWING2, target, 1, SPELL_SCHOOL_MASK_NORMAL, damage, 0, 0, VICTIMSTATE_NORMAL, 0);
        return true;
    }

    uint32 school = schoolStr ? atoi((char*)schoolStr) : SPELL_SCHOOL_NORMAL;
    if (school >= MAX_SPELL_SCHOOL)
        return false;

    SpellSchoolMask schoolmask = SpellSchoolMask(1 << school);

    if (schoolmask & SPELL_SCHOOL_MASK_NORMAL)
        damage = m_session->GetPlayer()->CalcArmorReducedDamage(target, damage);

    char* spellStr = strtok((char*)NULL, " ");

    // melee damage by specific school
    if (!spellStr)
    {
        uint32 absorb = 0;
        uint32 resist = 0;

        m_session->GetPlayer()->CalcAbsorbResist(target, schoolmask, SPELL_DIRECT_DAMAGE, damage, &absorb, &resist);

        if (damage <= int32(absorb + resist))
            return true;

        damage -= absorb + resist;

        m_session->GetPlayer()->DealDamage(target, damage, NULL, DIRECT_DAMAGE, schoolmask, NULL, false);
        m_session->GetPlayer()->SendAttackStateUpdate (HITINFO_NORMALSWING2, target, 1, schoolmask, damage, absorb, resist, VICTIMSTATE_NORMAL, 0);
        return true;
    }

    // non-melee damage

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
    uint32 spellid = extractSpellIdFromLink((char*)args);
    if (!spellid || !sSpellStore.LookupEntry(spellid))
        return false;

    m_session->GetPlayer()->SpellNonMeleeDamageLog(target, spellid, damage, false);
    return true;
}

bool ChatHandler::HandleModifyArenaCommand(const char* args)
{
    if (!*args)
        return false;

    Player* target = getSelectedPlayer();
    if (!target)
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    int32 amount = (uint32)atoi(args);

    target->ModifyArenaPoints(amount);

    PSendSysMessage(LANG_COMMAND_MODIFY_ARENA, target->GetName(), target->GetArenaPoints());

    return true;
}

bool ChatHandler::HandleReviveCommand(const char* args)
{
    Player* player = NULL;
    uint32 player_guid = 0;

    if (*args)
    {
        std::string name = args;
        if (!normalizePlayerName(name))
        {
            SendSysMessage(LANG_PLAYER_NOT_FOUND);
            SetSentErrorMessage(true);
            return false;
        }

        player = sObjectMgr.GetPlayer(name.c_str());
        if (!player)
            player_guid = sObjectMgr.GetPlayerGUIDByName(name);
    }
    else
        player = getSelectedPlayer();

    if (player)
    {
        player->ResurrectPlayer(player->GetSession()->GetSecurity() > SEC_PLAYER ? 1.0f : 0.5f);
        player->SpawnCorpseBones();
        player->SaveToDB();
    }
    else if (player_guid)
    {
        // will resurrected at login without corpse
        ObjectAccessor::Instance().ConvertCorpseForPlayer(player_guid);
    }
    else
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

bool ChatHandler::HandleAuraCommand(const char* args)
{
    char* px = strtok((char*)args, " ");
    if (!px)
        return false;

    Unit* target = getSelectedUnit();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 spellID = (uint32)atoi(px);
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellID);
    if (spellInfo)
    {
        for (uint32 i = 0; i < 3; i++)
        {
            uint8 eff = spellInfo->Effect[i];
            if (eff >= TOTAL_SPELL_EFFECTS)
                continue;
            if (IsAreaAuraEffect(eff)           ||
                eff == SPELL_EFFECT_APPLY_AURA  ||
                eff == SPELL_EFFECT_PERSISTENT_AREA_AURA)
            {
                Aura* Aur = CreateAura(spellInfo, i, NULL, target);
                target->AddAura(Aur);
            }
        }
    }

    return true;
}

bool ChatHandler::HandleUnAuraCommand(const char* args)
{
    char* px = strtok((char*)args, " ");
    if (!px)
        return false;

    Unit* target = getSelectedUnit();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    std::string argstr = args;
    if (argstr == "all")
    {
        target->RemoveAllAuras();
        return true;
    }

    uint32 spellID = (uint32)atoi(px);
    target->RemoveAurasDueToSpell(spellID);

    return true;
}

bool ChatHandler::HandleLinkGraveCommand(const char* args)
{
    if (!*args)
        return false;

    char* px = strtok((char*)args, " ");
    if (!px)
        return false;

    uint32 g_id = (uint32)atoi(px);

    uint32 g_team;

    char* px2 = strtok(NULL, " ");

    if (!px2)
        g_team = 0;
    else if (strncmp(px2, "horde", 6) == 0)
        g_team = HORDE;
    else if (strncmp(px2, "alliance", 9) == 0)
        g_team = ALLIANCE;
    else
        return false;

    WorldSafeLocsEntry const* graveyard =  sWorldSafeLocsStore.LookupEntry(g_id);

    if (!graveyard)
    {
        PSendSysMessage(LANG_COMMAND_GRAVEYARDNOEXIST, g_id);
        SetSentErrorMessage(true);
        return false;
    }

    Player* player = m_session->GetPlayer();

    uint32 zoneId = player->GetZoneId();

    AreaTableEntry const* areaEntry = GetAreaEntryByAreaID(zoneId);
    if (!areaEntry || areaEntry->zone != 0)
    {
        PSendSysMessage(LANG_COMMAND_GRAVEYARDWRONGZONE, g_id, zoneId);
        SetSentErrorMessage(true);
        return false;
    }

    if (sObjectMgr.AddGraveYardLink(g_id, player->GetZoneId(), g_team))
        PSendSysMessage(LANG_COMMAND_GRAVEYARDLINKED, g_id, zoneId);
    else
        PSendSysMessage(LANG_COMMAND_GRAVEYARDALRLINKED, g_id, zoneId);

    return true;
}

bool ChatHandler::HandleNearGraveCommand(const char* args)
{
    uint32 g_team;

    size_t argslen = strlen(args);

    if (!*args)
        g_team = 0;
    else if (strncmp((char*)args, "horde", argslen) == 0)
        g_team = HORDE;
    else if (strncmp((char*)args, "alliance", argslen) == 0)
        g_team = ALLIANCE;
    else
        return false;

    Player* player = m_session->GetPlayer();

    WorldSafeLocsEntry const* graveyard = sObjectMgr.GetClosestGraveYard(
            player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetMapId(), g_team);

    if (graveyard)
    {
        uint32 g_id = graveyard->ID;

        GraveYardData const* data = sObjectMgr.FindGraveYardData(g_id, player->GetZoneId());
        if (!data)
        {
            PSendSysMessage(LANG_COMMAND_GRAVEYARDERROR, g_id);
            SetSentErrorMessage(true);
            return false;
        }

        g_team = data->team;

        std::string team_name = GetOregonString(LANG_COMMAND_GRAVEYARD_NOTEAM);

        if (g_team == 0)
            team_name = GetOregonString(LANG_COMMAND_GRAVEYARD_ANY);
        else if (g_team == HORDE)
            team_name = GetOregonString(LANG_COMMAND_GRAVEYARD_HORDE);
        else if (g_team == ALLIANCE)
            team_name = GetOregonString(LANG_COMMAND_GRAVEYARD_ALLIANCE);

        PSendSysMessage(LANG_COMMAND_GRAVEYARDNEAREST, g_id, team_name.c_str(), player->GetZoneId());
    }
    else
    {
        std::string team_name;

        if (g_team == 0)
            team_name = GetOregonString(LANG_COMMAND_GRAVEYARD_ANY);
        else if (g_team == HORDE)
            team_name = GetOregonString(LANG_COMMAND_GRAVEYARD_HORDE);
        else if (g_team == ALLIANCE)
            team_name = GetOregonString(LANG_COMMAND_GRAVEYARD_ALLIANCE);

        if (g_team == ~uint32(0))
            PSendSysMessage(LANG_COMMAND_ZONENOGRAVEYARDS, player->GetZoneId());
        else
            PSendSysMessage(LANG_COMMAND_ZONENOGRAFACTION, player->GetZoneId(), team_name.c_str());
    }

    return true;
}

//play npc emote
bool ChatHandler::HandleNpcPlayEmoteCommand(const char* args)
{
    uint32 emote = atoi((char*)args);

    Creature* target = getSelectedCreature();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    target->SetUInt32Value(UNIT_NPC_EMOTESTATE, emote);

    return true;
}

bool ChatHandler::HandleNpcInfoCommand(const char* /*args*/)
{
    Creature* target = getSelectedCreature();

    if (!target)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 faction = target->GetFaction();
    uint32 npcflags = target->GetUInt32Value(UNIT_NPC_FLAGS);
    uint32 displayid = target->GetDisplayId();
    uint32 nativeid = target->GetNativeDisplayId();
    uint32 Entry = target->GetEntry();
    CreatureInfo const* cInfo = target->GetCreatureTemplate();

    int32 curRespawnDelay = target->GetRespawnTimeEx() - time(NULL);
    if (curRespawnDelay < 0)
        curRespawnDelay = 0;
    std::string curRespawnDelayStr = secsToTimeString(curRespawnDelay, true);
    std::string defRespawnDelayStr = secsToTimeString(target->GetRespawnDelay(), true);

    PSendSysMessage(LANG_NPCINFO_CHAR,  target->GetDBTableGUIDLow(), faction, npcflags, Entry, displayid, nativeid);
    PSendSysMessage(LANG_NPCINFO_LEVEL, target->getLevel());
    PSendSysMessage(LANG_NPCINFO_HEALTH, target->GetCreateHealth(), target->GetMaxHealth(), target->GetHealth());
    PSendSysMessage(LANG_NPCINFO_INHABIT_TYPE, cInfo->InhabitType);
    PSendSysMessage(LANG_NPCINFO_FLAGS, target->GetUInt32Value(UNIT_FIELD_FLAGS), target->GetUInt32Value(UNIT_DYNAMIC_FLAGS), target->GetFaction());
    PSendSysMessage(LANG_COMMAND_RAWPAWNTIMES, defRespawnDelayStr.c_str(), curRespawnDelayStr.c_str());
    PSendSysMessage(LANG_NPCINFO_LOOT,  cInfo->lootid, cInfo->pickpocketLootId, cInfo->SkinLootId);
    PSendSysMessage(LANG_NPCINFO_DUNGEON_ID, target->GetInstanceId());
    PSendSysMessage(LANG_NPCINFO_POSITION, float(target->GetPositionX()), float(target->GetPositionY()), float(target->GetPositionZ()));
    PSendSysMessage(LANG_NPCINFO_AIINFO, target->GetAIName().c_str(), target->GetScriptName().c_str());
    if (const CreatureData* const linked = target->GetLinkedRespawnCreatureData())
        if (CreatureInfo const* master = GetCreatureTemplate(linked->id))
            PSendSysMessage(LANG_NPCINFO_LINKGUID, sObjectMgr.GetLinkedRespawnGuid(target->GetDBTableGUIDLow()), linked->id, master->Name);

    if ((npcflags & UNIT_NPC_FLAG_VENDOR))
        SendSysMessage(LANG_NPCINFO_VENDOR);

    if ((npcflags & UNIT_NPC_FLAG_TRAINER))
        SendSysMessage(LANG_NPCINFO_TRAINER);

    return true;
}

bool ChatHandler::HandleExploreCheatCommand(const char* args)
{
    if (!*args)
        return false;

    int flag = atoi((char*)args);

    Player* chr = getSelectedPlayer();
    if (chr == NULL)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (flag != 0)
    {
        PSendSysMessage(LANG_YOU_SET_EXPLORE_ALL, chr->GetName());
        if (needReportToTarget(chr))
            ChatHandler(chr).PSendSysMessage(LANG_YOURS_EXPLORE_SET_ALL, GetName());
    }
    else
    {
        PSendSysMessage(LANG_YOU_SET_EXPLORE_NOTHING, chr->GetName());
        if (needReportToTarget(chr))
            ChatHandler(chr).PSendSysMessage(LANG_YOURS_EXPLORE_SET_NOTHING, GetName());
    }

    for (uint8 i = 0; i < 128; ++i)
    {
        if (flag != 0)
            m_session->GetPlayer()->SetFlag(PLAYER_EXPLORED_ZONES_1 + i, 0xFFFFFFFF);
        else
            m_session->GetPlayer()->SetFlag(PLAYER_EXPLORED_ZONES_1 + i, 0);
    }

    return true;
}

bool ChatHandler::HandleHoverCommand(const char* args)
{
    char* px = strtok((char*)args, " ");
    uint32 flag;
    if (!px)
        flag = 1;
    else
        flag = atoi(px);

    m_session->GetPlayer()->SetHover(flag);

    if (flag)
        SendSysMessage(LANG_HOVER_ENABLED);
    else
        SendSysMessage(LANG_HOVER_DISABLED);

    return true;
}

bool ChatHandler::HandleGodModeCheatCommand(const char *args)
{
    if (!m_session || !m_session->GetPlayer())
        return false;

    std::string argstr = (char*)args;

    if (!*args)
        argstr = (m_session->GetPlayer()->GetCommandStatus(CHEAT_GOD)) ? "off" : "on";

    if (argstr == "off")
    {
        m_session->GetPlayer()->SetCommandStatusOff(CHEAT_GOD);
        SendSysMessage("Godmode is OFF. You can take damage.");
        return true;
    }
    else if (argstr == "on")
    {
        m_session->GetPlayer()->SetCommandStatusOn(CHEAT_GOD);
        SendSysMessage("Godmode is ON. You won't take damage.");
        return true;
    }

    return false;
}

bool ChatHandler::HandleCasttimeCheatCommand(const char *args)
{
    if (!m_session || !m_session->GetPlayer())
        return false;

    std::string argstr = (char*)args;

    if (!*args)
        argstr = (m_session->GetPlayer()->GetCommandStatus(CHEAT_CASTTIME)) ? "off" : "on";

    if (argstr == "off")
    {
        m_session->GetPlayer()->SetCommandStatusOff(CHEAT_CASTTIME);
        SendSysMessage("CastTime Cheat is OFF. Your spells will have a casttime.");
        return true;
    }
    else if (argstr == "on")
    {
        m_session->GetPlayer()->SetCommandStatusOn(CHEAT_CASTTIME);
        SendSysMessage("CastTime Cheat is ON. Your spells won't have a casttime.");
        return true;
    }

    return false;
}

bool ChatHandler::HandleCoolDownCheatCommand(const char *args)
{
    if (!m_session || !m_session->GetPlayer())
        return false;

    std::string argstr = (char*)args;

    if (!*args)
        argstr = (m_session->GetPlayer()->GetCommandStatus(CHEAT_COOLDOWN)) ? "off" : "on";

    if (argstr == "off")
    {
        m_session->GetPlayer()->SetCommandStatusOff(CHEAT_COOLDOWN);
        SendSysMessage("Cooldown Cheat is OFF. You are on the global cooldown.");
        return true;
    }
    else if (argstr == "on")
    {
        m_session->GetPlayer()->SetCommandStatusOn(CHEAT_COOLDOWN);
        SendSysMessage("Cooldown Cheat is ON. You are not on the global cooldown.");
        return true;
    }

    return false;
}

bool ChatHandler::HandlePowerCheatCommand(const char *args)
{
    if (!m_session || !m_session->GetPlayer())
        return false;

    std::string argstr = (char*)args;

    if (!*args)
        argstr = (m_session->GetPlayer()->GetCommandStatus(CHEAT_POWER)) ? "off" : "on";

    if (argstr == "off")
    {
        m_session->GetPlayer()->SetCommandStatusOff(CHEAT_POWER);
        SendSysMessage("Power Cheat is OFF. You need mana/rage/energy to use spells.");
        return true;
    }
    else if (argstr == "on")
    {
        m_session->GetPlayer()->SetCommandStatusOn(CHEAT_POWER);
        SendSysMessage("Power Cheat is ON. You don't need mana/rage/energy to use spells.");
        return true;
    }

    return false;
}

bool ChatHandler::HandleWaterwalkCheatCommand(const char* args)
{
    if (!*args)
        return false;

    Player* player = getSelectedPlayer();
    if (!player)
    {
        PSendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (strncmp(args, "on", 3) == 0)
        player->SetWaterWalking(true);               // ON
    else if (strncmp(args, "off", 4) == 0)
        player->SetWaterWalking(false);              // OFF
    else
    {
        SendSysMessage(LANG_USE_BOL);
        return false;
    }

    PSendSysMessage(LANG_YOU_SET_WATERWALK, args, player->GetName());
    if (needReportToTarget(player))
        ChatHandler(player).PSendSysMessage(LANG_YOUR_WATERWALK_SET, args, GetName());
    return true;

}

bool ChatHandler::HandleLevelUpCommand(const char* args)
{
    char* px = strtok((char*)args, " ");
    char* py = strtok((char*)NULL, " ");

    // command format parsing
    char* pname = (char*)NULL;
    int addlevel = 1;

    if (px && py)                                            // .levelup name level
    {
        addlevel = atoi(py);
        pname = px;
    }
    else if (px && !py)                                      // .levelup name OR .levelup level
    {
        if (isalpha(px[0]))                                  // .levelup name
            pname = px;
        else                                                // .levelup level
            addlevel = atoi(px);
    }
    // else .levelup - nothing do for preparing

    // player
    Player* chr = NULL;
    uint64 chr_guid = 0;

    std::string name;

    if (pname)                                               // player by name
    {
        name = pname;
        if (!normalizePlayerName(name))
        {
            SendSysMessage(LANG_PLAYER_NOT_FOUND);
            SetSentErrorMessage(true);
            return false;
        }

        chr = sObjectMgr.GetPlayer(name.c_str());
        if (!chr)                                            // not in game
        {
            chr_guid = sObjectMgr.GetPlayerGUIDByName(name);
            if (chr_guid == 0)
            {
                SendSysMessage(LANG_PLAYER_NOT_FOUND);
                SetSentErrorMessage(true);
                return false;
            }
        }
    }
    else                                                    // player by selection
    {
        chr = getSelectedPlayer();

        if (chr == NULL)
        {
            SendSysMessage(LANG_NO_CHAR_SELECTED);
            SetSentErrorMessage(true);
            return false;
        }

        name = chr->GetName();
    }

    ASSERT(chr || chr_guid);

    int32 oldlevel = chr ? chr->getLevel() : Player::GetUInt32ValueFromDB(UNIT_FIELD_LEVEL, chr_guid);
    int32 newlevel = oldlevel + addlevel;

    if (newlevel < 1)
        newlevel = 1;

    if (newlevel > STRONG_MAX_LEVEL)                         // hardcoded maximum level
        newlevel = STRONG_MAX_LEVEL;

    if (chr)
    {
        chr->GiveLevel(newlevel);
        chr->InitTalentForLevel();
        chr->SetUInt32Value(PLAYER_XP, 0);

        if (oldlevel == newlevel)
            ChatHandler(chr).SendSysMessage(LANG_YOURS_LEVEL_PROGRESS_RESET);
        else if (oldlevel < newlevel)
            ChatHandler(chr).PSendSysMessage(LANG_YOURS_LEVEL_UP, newlevel - oldlevel);
        else if (oldlevel > newlevel)
            ChatHandler(chr).PSendSysMessage(LANG_YOURS_LEVEL_DOWN, newlevel - oldlevel);
    }
    else
    {
        // update level and XP at level, all other will be updated at loading
        CharacterDatabase.PExecute("UPDATE characters SET level = '%u', xp = 0 WHERE guid = '" UI64FMTD "'", newlevel, chr_guid);
    }

    if (m_session->GetPlayer() != chr)                       // including chr == NULL
        PSendSysMessage(LANG_YOU_CHANGE_LVL, name.c_str(), newlevel);
    return true;
}

bool ChatHandler::HandleShowAreaCommand(const char* args)
{
    if (!*args)
        return false;

    int area = atoi((char*)args);

    Player* chr = getSelectedPlayer();
    if (chr == NULL)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    int offset = area / 32;
    uint32 val = (uint32)(1 << (area % 32));

    if (offset >= 128)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 currFields = chr->GetUInt32Value(PLAYER_EXPLORED_ZONES_1 + offset);
    chr->SetUInt32Value(PLAYER_EXPLORED_ZONES_1 + offset, (uint32)(currFields | val));

    SendSysMessage(LANG_EXPLORE_AREA);
    return true;
}

bool ChatHandler::HandleHideAreaCommand(const char* args)
{
    if (!*args)
        return false;

    int area = atoi((char*)args);

    Player* chr = getSelectedPlayer();
    if (chr == NULL)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    int offset = area / 32;
    uint32 val = (uint32)(1 << (area % 32));

    if (offset >= 128)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 currFields = chr->GetUInt32Value(PLAYER_EXPLORED_ZONES_1 + offset);
    chr->SetUInt32Value(PLAYER_EXPLORED_ZONES_1 + offset, (uint32)(currFields ^ val));

    SendSysMessage(LANG_UNEXPLORE_AREA);
    return true;
}

bool ChatHandler::HandleUpdate(const char* args)
{
    if (!*args)
        return false;

    uint32 updateIndex;
    uint32 value;

    char* pUpdateIndex = strtok((char*)args, " ");

    Unit* chr = getSelectedUnit();
    if (chr == NULL)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    if (!pUpdateIndex)
        return true;
    updateIndex = atoi(pUpdateIndex);
    //check updateIndex
    if (chr->GetTypeId() == TYPEID_PLAYER)
    {
        if (updateIndex >= PLAYER_END) return true;
    }
    else
    {
        if (updateIndex >= UNIT_END) return true;
    }

    char* pvalue = strtok(NULL, " ");
    if (!pvalue)
    {
        value = chr->GetUInt32Value(updateIndex);

        PSendSysMessage(LANG_UPDATE, chr->GetGUIDLow(), updateIndex, value);
        return true;
    }

    value = atoi(pvalue);

    PSendSysMessage(LANG_UPDATE_CHANGE, chr->GetGUIDLow(), updateIndex, value);

    chr->SetUInt32Value(updateIndex, value);

    return true;
}

bool ChatHandler::HandleBankCommand(const char* /*args*/)
{
    m_session->SendShowBank(m_session->GetPlayer()->GetGUID());

    return true;
}

bool ChatHandler::HandleChangeWeather(const char* args)
{
    if (!*args)
        return false;

    //Weather is OFF
    if (!sWorld.getConfig(CONFIG_WEATHER))
    {
        SendSysMessage(LANG_WEATHER_DISABLED);
        SetSentErrorMessage(true);
        return false;
    }

    //*Change the weather of a cell
    char* px = strtok((char*)args, " ");
    char* py = strtok(NULL, " ");

    if (!px || !py)
        return false;

    uint32 type = (uint32)atoi(px);                         //0 to 3, 0: fine, 1: rain, 2: snow, 3: sand
    float grade = (float)atof(py);                          //0 to 1, sending -1 is instand good weather

    Player* player = m_session->GetPlayer();
    uint32 zoneid = player->GetZoneId();

    Weather* wth = sWorld.FindWeather(zoneid);

    if (!wth)
        wth = sWorld.AddWeather(zoneid);
    if (!wth)
    {
        SendSysMessage(LANG_NO_WEATHER);
        SetSentErrorMessage(true);
        return false;
    }

    wth->SetWeather(WeatherType(type), grade);

    return true;
}

bool ChatHandler::HandleSetValue(const char* args)
{
    if (!*args)
        return false;

    char* px = strtok((char*)args, " ");
    char* py = strtok(NULL, " ");
    char* pz = strtok(NULL, " ");

    if (!px || !py)
        return false;

    Unit* target = getSelectedUnit();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    uint64 guid = target->GetGUID();

    uint32 Opcode = (uint32)atoi(px);
    if (Opcode >= target->GetValuesCount())
    {
        PSendSysMessage(LANG_TOO_BIG_INDEX, Opcode, GUID_LOPART(guid), target->GetValuesCount());
        return false;
    }
    uint32 iValue;
    float fValue;
    bool isint32 = true;
    if (pz)
        isint32 = (bool)atoi(pz);
    if (isint32)
    {
        iValue = (uint32)atoi(py);
        sLog.outDebug(GetOregonString(LANG_SET_UINT), GUID_LOPART(guid), Opcode, iValue);
        target->SetUInt32Value(Opcode , iValue);
        PSendSysMessage(LANG_SET_UINT_FIELD, GUID_LOPART(guid), Opcode, iValue);
    }
    else
    {
        fValue = (float)atof(py);
        sLog.outDebug(GetOregonString(LANG_SET_FLOAT), GUID_LOPART(guid), Opcode, fValue);
        target->SetFloatValue(Opcode , fValue);
        PSendSysMessage(LANG_SET_FLOAT_FIELD, GUID_LOPART(guid), Opcode, fValue);
    }

    return true;
}

bool ChatHandler::HandleGetValue(const char* args)
{
    if (!*args)
        return false;

    char* px = strtok((char*)args, " ");
    char* pz = strtok(NULL, " ");

    if (!px)
        return false;

    Unit* target = getSelectedUnit();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    uint64 guid = target->GetGUID();

    uint32 Opcode = (uint32)atoi(px);
    if (Opcode >= target->GetValuesCount())
    {
        PSendSysMessage(LANG_TOO_BIG_INDEX, Opcode, GUID_LOPART(guid), target->GetValuesCount());
        return false;
    }
    uint32 iValue;
    float fValue;
    bool isint32 = true;
    if (pz)
        isint32 = (bool)atoi(pz);

    if (isint32)
    {
        iValue = target->GetUInt32Value(Opcode);
        sLog.outDebug(GetOregonString(LANG_GET_UINT), GUID_LOPART(guid), Opcode, iValue);
        PSendSysMessage(LANG_GET_UINT_FIELD, GUID_LOPART(guid), Opcode,    iValue);
    }
    else
    {
        fValue = target->GetFloatValue(Opcode);
        sLog.outDebug(GetOregonString(LANG_GET_FLOAT), GUID_LOPART(guid), Opcode, fValue);
        PSendSysMessage(LANG_GET_FLOAT_FIELD, GUID_LOPART(guid), Opcode, fValue);
    }

    return true;
}

bool ChatHandler::HandleSet32Bit(const char* args)
{
    if (!*args)
        return false;

    char* px = strtok((char*)args, " ");
    char* py = strtok(NULL, " ");

    if (!px || !py)
        return false;

    uint32 Opcode = (uint32)atoi(px);
    uint32 Value = (uint32)atoi(py);
    if (Value > 32)                                         //uint32 = 32 bits
        return false;

    sLog.outDebug(GetOregonString(LANG_SET_32BIT), Opcode, Value);

    m_session->GetPlayer()->SetUInt32Value(Opcode , 2 ^ Value);

    PSendSysMessage(LANG_SET_32BIT_FIELD, Opcode, 1);
    return true;
}

bool ChatHandler::HandleDebugRAFError(const char* args)
{
    uint32 id = strtoul(args, NULL, 10);
    if (!id || !m_session)
        return false;

    WorldPacket packet(SMSG_REFER_A_FRIEND_FAILURE, 4);
    packet << id;
    m_session->SendPacket(&packet);
    return true;
}

bool ChatHandler::HandleMod32Value(const char* args)
{
    if (!*args)
        return false;

    char* px = strtok((char*)args, " ");
    char* py = strtok(NULL, " ");

    if (!px || !py)
        return false;

    uint32 Opcode = (uint32)atoi(px);
    int Value = atoi(py);

    if (Opcode >= m_session->GetPlayer()->GetValuesCount())
    {
        PSendSysMessage(LANG_TOO_BIG_INDEX, Opcode, m_session->GetPlayer()->GetGUIDLow(), m_session->GetPlayer()->GetValuesCount());
        return false;
    }

    sLog.outDebug(GetOregonString(LANG_CHANGE_32BIT), Opcode, Value);

    int CurrentValue = (int)m_session->GetPlayer()->GetUInt32Value(Opcode);

    CurrentValue += Value;
    m_session->GetPlayer()->SetUInt32Value(Opcode , (uint32)CurrentValue);

    PSendSysMessage(LANG_CHANGE_32BIT_FIELD, Opcode, CurrentValue);

    return true;
}

bool ChatHandler::HandleAddTeleCommand(const char* args)
{
    if (!*args)
        return false;

    Player* player = m_session->GetPlayer();
    if (!player)
        return false;

    std::string name = args;

    if (sObjectMgr.GetGameTele(name))
    {
        SendSysMessage(LANG_COMMAND_TP_ALREADYEXIST);
        SetSentErrorMessage(true);
        return false;
    }

    GameTele tele;
    tele.position_x  = player->GetPositionX();
    tele.position_y  = player->GetPositionY();
    tele.position_z  = player->GetPositionZ();
    tele.orientation = player->GetOrientation();
    tele.mapId       = player->GetMapId();
    tele.name        = name;

    if (sObjectMgr.AddGameTele(tele))
        SendSysMessage(LANG_COMMAND_TP_ADDED);
    else
    {
        SendSysMessage(LANG_COMMAND_TP_ADDEDERR);
        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

bool ChatHandler::HandleDelTeleCommand(const char* args)
{
    if (!*args)
        return false;

    std::string name = args;

    if (!sObjectMgr.DeleteGameTele(name))
    {
        SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
        SetSentErrorMessage(true);
        return false;
    }

    SendSysMessage(LANG_COMMAND_TP_DELETED);
    return true;
}

bool ChatHandler::HandleListAurasCommand (const char* /*args*/)
{
    Unit* unit = getSelectedUnit();
    if (!unit)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    char const* talentStr = GetOregonString(LANG_TALENT);
    char const* passiveStr = GetOregonString(LANG_PASSIVE);

    Unit::AuraMap const& uAuras = unit->GetAuras();
    PSendSysMessage(LANG_COMMAND_TARGET_LISTAURAS, uAuras.size());
    for (Unit::AuraMap::const_iterator itr = uAuras.begin(); itr != uAuras.end(); ++itr)
    {
        bool talent = GetTalentSpellCost(itr->second->GetId()) > 0;
        PSendSysMessage(LANG_COMMAND_TARGET_AURADETAIL, itr->second->GetId(), itr->second->GetEffIndex(),
                        itr->second->GetModifier()->m_auraname, itr->second->GetAuraDuration(), itr->second->GetAuraMaxDuration(),
                        itr->second->GetSpellProto()->SpellName[m_session->GetSessionDbcLocale()],
                        (itr->second->IsPassive() ? passiveStr : ""), (talent ? talentStr : ""),
                        IS_PLAYER_GUID(itr->second->GetCasterGUID()) ? "player" : "creature", GUID_LOPART(itr->second->GetCasterGUID()));
    }
    for (uint16 i = 0; i < TOTAL_AURAS; ++i)
    {
        Unit::AuraList const& uAuraList = unit->GetAurasByType(AuraType(i));
        if (uAuraList.empty()) continue;
        PSendSysMessage(LANG_COMMAND_TARGET_LISTAURATYPE, uAuraList.size(), i);
        for (Unit::AuraList::const_iterator itr = uAuraList.begin(); itr != uAuraList.end(); ++itr)
        {
            bool talent = GetTalentSpellCost((*itr)->GetId()) > 0;
            PSendSysMessage(LANG_COMMAND_TARGET_AURASIMPLE, (*itr)->GetId(), (*itr)->GetEffIndex(),
                            (*itr)->GetSpellProto()->SpellName[m_session->GetSessionDbcLocale()], ((*itr)->IsPassive() ? passiveStr : ""), (talent ? talentStr : ""),
                            IS_PLAYER_GUID((*itr)->GetCasterGUID()) ? "player" : "creature", GUID_LOPART((*itr)->GetCasterGUID()));
        }
    }
    return true;
}

bool ChatHandler::HandleResetHonorCommand (const char* args)
{
    char* pName = strtok((char*)args, "");
    Player* player = NULL;
    if (pName)
    {
        std::string name = pName;
        if (!normalizePlayerName(name))
        {
            SendSysMessage(LANG_PLAYER_NOT_FOUND);
            SetSentErrorMessage(true);
            return false;
        }

        uint64 guid = sObjectMgr.GetPlayerGUIDByName(name.c_str());
        player = sObjectMgr.GetPlayer(guid);
    }
    else
        player = getSelectedPlayer();

    if (!player)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        return true;
    }

    player->SetUInt32Value(PLAYER_FIELD_KILLS, 0);
    player->SetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS, 0);
    player->SetUInt32Value(PLAYER_FIELD_HONOR_CURRENCY, 0);
    player->SetUInt32Value(PLAYER_FIELD_TODAY_CONTRIBUTION, 0);
    player->SetUInt32Value(PLAYER_FIELD_YESTERDAY_CONTRIBUTION, 0);

    return true;
}

static bool HandleResetStatsOrLevelHelper(Player* player)
{
    PlayerInfo const* info = sObjectMgr.GetPlayerInfo(player->getRace(), player->getClass());
    if (!info) return false;

    ChrClassesEntry const* cEntry = sChrClassesStore.LookupEntry(player->getClass());
    if (!cEntry)
    {
        sLog.outError("Class %u not found in DBC (Wrong DBC files?)", player->getClass());
        return false;
    }

    uint8 powertype = cEntry->powerType;

    uint32 unitfield;
    if (powertype == POWER_RAGE)
        unitfield = 0x1100EE00;
    else if (powertype == POWER_ENERGY)
        unitfield = 0x00000000;
    else if (powertype == POWER_MANA)
        unitfield = 0x0000EE00;
    else
    {
        sLog.outError("Invalid default powertype %u for player (class %u)", powertype, player->getClass());
        return false;
    }

    // reset m_form if no aura
    if (!player->HasAuraType(SPELL_AURA_MOD_SHAPESHIFT))
        player->m_form = FORM_NONE;

    player->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, DEFAULT_WORLD_OBJECT_SIZE);
    player->SetFloatValue(UNIT_FIELD_COMBATREACH, DEFAULT_COMBAT_REACH);

    player->setFactionForRace(player->getRace());

    player->SetUInt32Value(UNIT_FIELD_BYTES_0, ((player->getRace()) | (player->getClass() << 8) | (player->getGender() << 16) | (powertype << 24)));

    // reset only if player not in some form;
    if (player->m_form == FORM_NONE)
    {
        switch (player->getGender())
        {
        case GENDER_FEMALE:
            player->SetDisplayId(info->displayId_f);
            player->SetNativeDisplayId(info->displayId_f);
            break;
        case GENDER_MALE:
            player->SetDisplayId(info->displayId_m);
            player->SetNativeDisplayId(info->displayId_m);
            break;
        default:
            break;
        }
    }

    // set UNIT_FIELD_BYTES_1 to init state but preserve m_form value
    player->SetUInt32Value(UNIT_FIELD_BYTES_1, unitfield);
    player->SetByteValue(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_SANCTUARY | UNIT_BYTE2_FLAG_UNK5);
    player->SetByteValue(UNIT_FIELD_BYTES_2, 3, player->m_form);

    player->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);

    //-1 is default value
    player->SetInt32Value(PLAYER_FIELD_WATCHED_FACTION_INDEX, -1);

    //player->SetUInt32Value(PLAYER_FIELD_BYTES, 0xEEE00000);
    return true;
}

bool ChatHandler::HandleResetLevelCommand(const char* args)
{
    char* pName = strtok((char*)args, "");
    Player* player = NULL;
    if (pName)
    {
        std::string name = pName;
        if (!normalizePlayerName(name))
        {
            SendSysMessage(LANG_PLAYER_NOT_FOUND);
            SetSentErrorMessage(true);
            return false;
        }

        uint64 guid = sObjectMgr.GetPlayerGUIDByName(name.c_str());
        player = sObjectMgr.GetPlayer(guid);
    }
    else
        player = getSelectedPlayer();

    if (!player)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (!HandleResetStatsOrLevelHelper(player))
        return false;

    // set starting level
    uint32 startLevel = sWorld.getConfig(CONFIG_START_PLAYER_LEVEL);

    player->SetLevel(startLevel);
    player->InitStatsForLevel(true);
    player->InitTaxiNodesForLevel();
    player->InitTalentForLevel();
    player->SetUInt32Value(PLAYER_XP, 0);

    // reset level to summoned pet
    Guardian* pet = player->GetGuardianPet();
    if (pet)
        pet->InitStatsForLevel(startLevel);

    return true;
}

bool ChatHandler::HandleResetStatsCommand(const char* args)
{
    char* pName = strtok((char*)args, "");
    Player* player = NULL;
    if (pName)
    {
        std::string name = pName;
        if (!normalizePlayerName(name))
        {
            SendSysMessage(LANG_PLAYER_NOT_FOUND);
            SetSentErrorMessage(true);
            return false;
        }

        uint64 guid = sObjectMgr.GetPlayerGUIDByName(name.c_str());
        player = sObjectMgr.GetPlayer(guid);
    }
    else
        player = getSelectedPlayer();

    if (!player)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (!HandleResetStatsOrLevelHelper(player))
        return false;

    player->InitStatsForLevel(true);
    player->InitTaxiNodesForLevel();
    player->InitTalentForLevel();

    return true;
}

bool ChatHandler::HandleResetSpellsCommand(const char* args)
{
    char* pName = strtok((char*)args, "");
    Player* player = NULL;
    uint64 playerGUID = 0;
    if (pName)
    {
        std::string name = pName;

        if (!normalizePlayerName(name))
        {
            SendSysMessage(LANG_PLAYER_NOT_FOUND);
            SetSentErrorMessage(true);
            return false;
        }

        player = sObjectMgr.GetPlayer(name.c_str());
        if (!player)
            playerGUID = sObjectMgr.GetPlayerGUIDByName(name.c_str());
    }
    else
        player = getSelectedPlayer();

    if (!player && !playerGUID)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (player)
    {
        player->ResetSpells();

        ChatHandler(player).SendSysMessage(LANG_RESET_SPELLS);

        if (m_session->GetPlayer() != player)
            PSendSysMessage(LANG_RESET_SPELLS_ONLINE, player->GetName());
    }
    else
    {
        CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '%u' WHERE guid = '%u'", uint32(AT_LOGIN_RESET_SPELLS), GUID_LOPART(playerGUID));
        PSendSysMessage(LANG_RESET_SPELLS_OFFLINE, pName);
    }

    return true;
}

bool ChatHandler::HandleResetTalentsCommand(const char* args)
{
    char* pName = strtok((char*)args, "");
    Player* player = NULL;
    uint64 playerGUID = 0;
    if (pName)
    {
        std::string name = pName;
        if (!normalizePlayerName(name))
        {
            SendSysMessage(LANG_PLAYER_NOT_FOUND);
            SetSentErrorMessage(true);
            return false;
        }

        player = sObjectMgr.GetPlayer(name.c_str());
        if (!player)
            playerGUID = sObjectMgr.GetPlayerGUIDByName(name.c_str());
    }
    else
        player = getSelectedPlayer();

    if (!player && !playerGUID)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (player)
    {
        player->ResetTalents(true);

        ChatHandler(player).SendSysMessage(LANG_RESET_TALENTS);

        if (m_session->GetPlayer() != player)
            PSendSysMessage(LANG_RESET_TALENTS_ONLINE, player->GetName());
    }
    else
    {
        CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '%u' WHERE guid = '%u'", uint32(AT_LOGIN_RESET_TALENTS), GUID_LOPART(playerGUID));
        PSendSysMessage(LANG_RESET_TALENTS_OFFLINE, pName);
    }

    return true;
}

bool ChatHandler::HandleResetAllCommand(const char* args)
{
    if (!*args)
        return false;

    std::string casename = args;

    AtLoginFlags atLogin;

    // Command specially created as single command to prevent using short case names
    if (casename == "spells")
    {
        atLogin = AT_LOGIN_RESET_SPELLS;
        sWorld.SendWorldText(LANG_RESETALL_SPELLS);
    }
    else if (casename == "talents")
    {
        atLogin = AT_LOGIN_RESET_TALENTS;
        sWorld.SendWorldText(LANG_RESETALL_TALENTS);
    }
    else
    {
        PSendSysMessage(LANG_RESETALL_UNKNOWN_CASE, args);
        SetSentErrorMessage(true);
        return false;
    }

    CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '%u' WHERE (at_login & '%u') = '0'", atLogin, atLogin);

    ObjectAccessor::Guard guard(*HashMapHolder<Player>::GetLock());
    HashMapHolder<Player>::MapType const& plist = ObjectAccessor::Instance().GetPlayers();
    for (HashMapHolder<Player>::MapType::const_iterator itr = plist.begin(); itr != plist.end(); ++itr)
        itr->second->SetAtLoginFlag(atLogin);

    return true;
}

bool ChatHandler::HandleServerShutDownCancelCommand(const char* /*args*/)
{
    sWorld.ShutdownCancel();
    return true;
}

bool ChatHandler::HandleServerShutDownCommand(const char* args)
{
    if (!*args)
        return false;

    char* time_str = strtok ((char*) args, " ");
    char* exitcode_str = strtok (NULL, "");

    int32 time = atoi (time_str);

    // Prevent interpret wrong arg value as 0 secs shutdown time
    if ((time == 0 && (time_str[0] != '0' || time_str[1] != '\0')) || time < 0)
        return false;

    if (exitcode_str)
    {
        int32 exitcode = atoi (exitcode_str);

        // Handle atoi() errors
        if (exitcode == 0 && (exitcode_str[0] != '0' || exitcode_str[1] != '\0'))
            return false;

        // Exit code should be in range of 0-125, 126-255 is used
        // in many shells for their own return codes and code > 255
        // is not supported in many others
        if (exitcode < 0 || exitcode > 125)
            return false;

        sWorld.ShutdownServ (time, 0, exitcode);
    }
    else
        sWorld.ShutdownServ(time, 0, SHUTDOWN_EXIT_CODE);
    return true;
}

bool ChatHandler::HandleServerRestartCommand(const char* args)
{
    if (!*args)
        return false;

    char* time_str = strtok ((char*) args, " ");
    char* exitcode_str = strtok (NULL, "");

    int32 time = atoi (time_str);

    // Prevent interpret wrong arg value as 0 secs shutdown time
    if ((time == 0 && (time_str[0] != '0' || time_str[1] != '\0')) || time < 0)
        return false;

    if (exitcode_str)
    {
        int32 exitcode = atoi (exitcode_str);

        // Handle atoi() errors
        if (exitcode == 0 && (exitcode_str[0] != '0' || exitcode_str[1] != '\0'))
            return false;

        // Exit code should be in range of 0-125, 126-255 is used
        // in many shells for their own return codes and code > 255
        // is not supported in many others
        if (exitcode < 0 || exitcode > 125)
            return false;

        sWorld.ShutdownServ (time, SHUTDOWN_MASK_RESTART, exitcode);
    }
    else
        sWorld.ShutdownServ(time, SHUTDOWN_MASK_RESTART, RESTART_EXIT_CODE);
    return true;
}

bool ChatHandler::HandleServerIdleRestartCommand(const char* args)
{
    if (!*args)
        return false;

    char* time_str = strtok ((char*) args, " ");
    char* exitcode_str = strtok (NULL, "");

    int32 time = atoi (time_str);

    // Prevent interpret wrong arg value as 0 secs shutdown time
    if ((time == 0 && (time_str[0] != '0' || time_str[1] != '\0')) || time < 0)
        return false;

    if (exitcode_str)
    {
        int32 exitcode = atoi (exitcode_str);

        // Handle atoi() errors
        if (exitcode == 0 && (exitcode_str[0] != '0' || exitcode_str[1] != '\0'))
            return false;

        // Exit code should be in range of 0-125, 126-255 is used
        // in many shells for their own return codes and code > 255
        // is not supported in many others
        if (exitcode < 0 || exitcode > 125)
            return false;

        sWorld.ShutdownServ (time, SHUTDOWN_MASK_RESTART | SHUTDOWN_MASK_IDLE, exitcode);
    }
    else
        sWorld.ShutdownServ(time, SHUTDOWN_MASK_RESTART | SHUTDOWN_MASK_IDLE, RESTART_EXIT_CODE);
    return true;
}

bool ChatHandler::HandleServerIdleShutDownCommand(const char* args)
{
    if (!*args)
        return false;

    char* time_str = strtok ((char*) args, " ");
    char* exitcode_str = strtok (NULL, "");

    int32 time = atoi (time_str);

    // Prevent interpret wrong arg value as 0 secs shutdown time
    if ((time == 0 && (time_str[0] != '0' || time_str[1] != '\0')) || time < 0)
        return false;

    if (exitcode_str)
    {
        int32 exitcode = atoi (exitcode_str);

        // Handle atoi() errors
        if (exitcode == 0 && (exitcode_str[0] != '0' || exitcode_str[1] != '\0'))
            return false;

        // Exit code should be in range of 0-125, 126-255 is used
        // in many shells for their own return codes and code > 255
        // is not supported in many others
        if (exitcode < 0 || exitcode > 125)
            return false;

        sWorld.ShutdownServ (time, SHUTDOWN_MASK_IDLE, exitcode);
    }
    else
        sWorld.ShutdownServ(time, SHUTDOWN_MASK_IDLE, SHUTDOWN_EXIT_CODE);
    return true;
}

bool ChatHandler::HandleAddQuest(const char* args)
{
    Player* player = getSelectedPlayerOrSelf();
    if (!player)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    // .addquest #entry'
    // number or [name] Shift-click form |color|Hquest:quest_id|h[name]|h|r
    char* cId = extractKeyFromLink((char*)args, "Hquest");
    if (!cId)
        return false;

    uint32 entry = atol(cId);

    Quest const* pQuest = sObjectMgr.GetQuestTemplate(entry);

    if (!pQuest)
    {
        PSendSysMessage(LANG_COMMAND_QUEST_NOTFOUND, entry);
        SetSentErrorMessage(true);
        return false;
    }

    // check item starting quest (it can work incorrectly if added without item in inventory)
    for (uint32 id = 0; id < sItemStorage.MaxEntry; id++)
    {
        ItemTemplate const* pProto = sItemStorage.LookupEntry<ItemTemplate>(id);
        if (!pProto)
            continue;

        if (pProto->StartQuest == entry)
        {
            PSendSysMessage(LANG_COMMAND_QUEST_STARTFROMITEM, entry, pProto->ItemId);
            SetSentErrorMessage(true);
            return false;
        }
    }

    // ok, normal (creature/GO starting) quest
    if (player->CanAddQuest(pQuest, true))
    {
        player->AddQuest(pQuest, NULL);

        if (player->CanCompleteQuest(entry))
            player->CompleteQuest(entry);
    }

    return true;
}

bool ChatHandler::HandleRemoveQuest(const char* args)
{
    Player* player = getSelectedPlayerOrSelf();
    if (!player)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    // .removequest #entry'
    // number or [name] Shift-click form |color|Hquest:quest_id|h[name]|h|r
    char* cId = extractKeyFromLink((char*)args, "Hquest");
    if (!cId)
        return false;

    uint32 entry = atol(cId);

    Quest const* pQuest = sObjectMgr.GetQuestTemplate(entry);

    if (!pQuest)
    {
        PSendSysMessage(LANG_COMMAND_QUEST_NOTFOUND, entry);
        SetSentErrorMessage(true);
        return false;
    }

    // remove all quest entries for 'entry' from quest log
    for (uint8 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        uint32 quest = player->GetQuestSlotQuestId(slot);
        if (quest == entry)
        {
            player->SetQuestSlot(slot, 0);

            // we ignore unequippable quest items in this case, its' still be equipped
            player->TakeQuestSourceItem(quest, false);
        }
    }

    // set quest status to not started (will updated in DB at next save)
    player->SetQuestStatus(entry, QUEST_STATUS_NONE);

    // reset rewarded for restart repeatable quest
    player->getQuestStatusMap()[entry].m_rewarded = false;

    SendSysMessage(LANG_COMMAND_QUEST_REMOVED);
    return true;
}

bool ChatHandler::HandleCompleteQuest(const char* args)
{
    Player* player = getSelectedPlayerOrSelf();
    if (!player)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    // .quest complete #entry
    // number or [name] Shift-click form |color|Hquest:quest_id|h[name]|h|r
    char* cId = extractKeyFromLink((char*)args, "Hquest");
    if (!cId)
        return false;

    uint32 entry = atol(cId);

    Quest const* pQuest = sObjectMgr.GetQuestTemplate(entry);

    // If player doesn't have the quest
    if (!pQuest || player->GetQuestStatus(entry) == QUEST_STATUS_NONE)
    {
        PSendSysMessage(LANG_COMMAND_QUEST_NOTFOUND, entry);
        SetSentErrorMessage(true);
        return false;
    }

    // Add quest items for quests that require items
    for (uint8 x = 0; x < QUEST_OBJECTIVES_COUNT; ++x)
    {
        uint32 id = pQuest->ReqItemId[x];
        uint32 count = pQuest->ReqItemCount[x];
        if (!id || !count)
            continue;

        uint32 curItemCount = player->GetItemCount(id, true);

        ItemPosCountVec dest;
        uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, id, count - curItemCount);
        if (msg == EQUIP_ERR_OK)
        {
            Item* item = player->StoreNewItem(dest, id, true);
            player->SendNewItem(item, count - curItemCount, true, false);
        }
    }

    // All creature/GO slain/casted (not required, but otherwise it will display "Creature slain 0/10")
    for (uint8 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
    {
        int32 creature = pQuest->ReqCreatureOrGOId[i];
        uint32 creaturecount = pQuest->ReqCreatureOrGOCount[i];

        if (uint32 spell_id = pQuest->ReqSpell[i])
        {
            for (uint16 z = 0; z < creaturecount; ++z)
                player->CastedCreatureOrGO(creature, 0, spell_id);
        }
        else if (creature > 0)
        {
            if (CreatureInfo const* cInfo = sObjectMgr.GetCreatureTemplate(creature))
                for (uint16 z = 0; z < creaturecount; ++z)
                    player->KilledMonster(cInfo, 0);
        }
        else if (creature < 0)
        {
            for (uint16 z = 0; z < creaturecount; ++z)
                player->CastedCreatureOrGO(creature, 0, 0);
        }
    }

    // If the quest requires reputation to complete
    if (uint32 repFaction = pQuest->GetRepObjectiveFaction())
    {
        uint32 repValue = pQuest->GetRepObjectiveValue();
        uint32 curRep = player->GetReputationMgr().GetReputation(repFaction);
        if (curRep < repValue)
        {
            FactionEntry const* factionEntry = sFactionStore.LookupEntry(repFaction);
            player->GetReputationMgr().SetReputation(factionEntry, repValue);
        }
    }

    // If the quest requires money
    int32 ReqOrRewMoney = pQuest->GetRewOrReqMoney();
    if (ReqOrRewMoney < 0)
        player->ModifyMoney(-ReqOrRewMoney);

    player->CompleteQuest(entry);
    return true;
}

bool ChatHandler::HandleBanAccountCommand(const char* args)
{
    return HandleBanHelper(BAN_ACCOUNT, args);
}

bool ChatHandler::HandleBanCharacterCommand(const char* args)
{
    return HandleBanHelper(BAN_CHARACTER, args);
}

bool ChatHandler::HandleBanIPCommand(const char* args)
{
    return HandleBanHelper(BAN_IP, args);
}

bool ChatHandler::HandleBanHelper(BanMode mode, const char* args)
{
    if (!*args)
        return false;

    char* cnameOrIP = strtok ((char*)args, " ");
    if (!cnameOrIP)
        return false;

    std::string nameOrIP = cnameOrIP;

    char* duration = strtok (NULL, " ");
    if (!duration || !atoi(duration))
        return false;

    char* reason = strtok (NULL, "");
    if (!reason)
        return false;

    switch (mode)
    {
    case BAN_ACCOUNT:
        if (!AccountMgr::normalizeString(nameOrIP))
        {
            PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, nameOrIP.c_str());
            SetSentErrorMessage(true);
            return false;
        }
        break;
    case BAN_CHARACTER:
        if (!normalizePlayerName(nameOrIP))
        {
            SendSysMessage(LANG_PLAYER_NOT_FOUND);
            SetSentErrorMessage(true);
            return false;
        }
        break;
    case BAN_IP:
        if (!IsIPAddress(nameOrIP.c_str()))
            return false;
        break;
    }

    switch (sWorld.BanAccount(mode, nameOrIP, duration, reason, m_session ? m_session->GetPlayerName() : ""))
    {
    case BAN_SUCCESS:
        if (atoi(duration) > 0)
            PSendSysMessage(LANG_BAN_YOUBANNED, nameOrIP.c_str(), secsToTimeString(TimeStringToSecs(duration), true).c_str(), reason);
        else
            PSendSysMessage(LANG_BAN_YOUPERMBANNED, nameOrIP.c_str(), reason);
        break;
    case BAN_SYNTAX_ERROR:
        return false;
    case BAN_NOTFOUND:
        switch (mode)
        {
        default:
            PSendSysMessage(LANG_BAN_NOTFOUND, "account", nameOrIP.c_str());
            break;
        case BAN_CHARACTER:
            PSendSysMessage(LANG_BAN_NOTFOUND, "character", nameOrIP.c_str());
            break;
        case BAN_IP:
            PSendSysMessage(LANG_BAN_NOTFOUND, "ip", nameOrIP.c_str());
            break;
        }
        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

bool ChatHandler::HandleUnBanAccountCommand(const char* args)
{
    return HandleUnBanHelper(BAN_ACCOUNT, args);
}

bool ChatHandler::HandleUnBanCharacterCommand(const char* args)
{
    return HandleUnBanHelper(BAN_CHARACTER, args);
}

bool ChatHandler::HandleUnBanIPCommand(const char* args)
{
    return HandleUnBanHelper(BAN_IP, args);
}

bool ChatHandler::HandleUnBanHelper(BanMode mode, const char* args)
{
    if (!*args)
        return false;

    char* cnameOrIP = strtok ((char*)args, " ");
    if (!cnameOrIP)
        return false;

    std::string nameOrIP = cnameOrIP;

    switch (mode)
    {
    case BAN_ACCOUNT:
        if (!AccountMgr::normalizeString(nameOrIP))
        {
            PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, nameOrIP.c_str());
            SetSentErrorMessage(true);
            return false;
        }
        break;
    case BAN_CHARACTER:
        if (!normalizePlayerName(nameOrIP))
        {
            SendSysMessage(LANG_PLAYER_NOT_FOUND);
            SetSentErrorMessage(true);
            return false;
        }
        break;
    case BAN_IP:
        if (!IsIPAddress(nameOrIP.c_str()))
            return false;
        break;
    }

    if (sWorld.RemoveBanAccount(mode, nameOrIP))
        PSendSysMessage(LANG_UNBAN_UNBANNED, nameOrIP.c_str());
    else
        PSendSysMessage(LANG_UNBAN_ERROR, nameOrIP.c_str());

    return true;
}

bool ChatHandler::HandleBanInfoAccountCommand(const char* args)
{
    if (!*args)
        return false;

    char* cname = strtok((char*)args, "");
    if (!cname)
        return false;

    std::string account_name = cname;
    if (!AccountMgr::normalizeString(account_name))
    {
        PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
        SetSentErrorMessage(true);
        return false;
    }

    uint32 accountid = sAccountMgr->GetId(account_name);
    if (!accountid)
    {
        PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
        return true;
    }

    return HandleBanInfoHelper(accountid, account_name.c_str());
}

bool ChatHandler::HandleBanInfoCharacterCommand(const char* args)
{
    if (!*args)
        return false;

    char* cname = strtok ((char*)args, "");
    if (!cname)
        return false;

    std::string name = cname;
    if (!normalizePlayerName(name))
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 accountid = sObjectMgr.GetPlayerAccountIdByPlayerName(name);
    if (!accountid)
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    std::string accountname;
    if (!sAccountMgr->GetName(accountid, accountname))
    {
        PSendSysMessage(LANG_BANINFO_NOCHARACTER);
        return true;
    }

    return HandleBanInfoHelper(accountid, accountname.c_str());
}

bool ChatHandler::HandleBanInfoHelper(uint32 accountid, char const* accountname)
{
    QueryResult_AutoPtr result = LoginDatabase.PQuery("SELECT FROM_UNIXTIME(bandate), unbandate-bandate, active, unbandate,banreason,bannedby FROM account_banned WHERE id = '%u' ORDER BY bandate ASC", accountid);
    if (!result)
    {
        PSendSysMessage(LANG_BANINFO_NOACCOUNTBAN, accountname);
        return true;
    }

    PSendSysMessage(LANG_BANINFO_BANHISTORY, accountname);
    do
    {
        Field* fields = result->Fetch();

        time_t unbandate = time_t(fields[3].GetUInt64());
        bool active = false;
        if (fields[2].GetBool() && (fields[1].GetUInt64() == (uint64)0 || unbandate >= time(NULL)))
            active = true;
        bool permanent = (fields[1].GetUInt64() == (uint64)0);
        std::string bantime = permanent ? GetOregonString(LANG_BANINFO_INFINITE) : secsToTimeString(fields[1].GetUInt64(), true);
        PSendSysMessage(LANG_BANINFO_HISTORYENTRY,
                        fields[0].GetString(), bantime.c_str(), active ? GetOregonString(LANG_BANINFO_YES) : GetOregonString(LANG_BANINFO_NO), fields[4].GetString(), fields[5].GetString());
    }
    while (result->NextRow());

    return true;
}

bool ChatHandler::HandleBanInfoIPCommand(const char* args)
{
    if (!*args)
        return false;

    char* cIP = strtok ((char*)args, "");
    if (!cIP)
        return false;

    if (!IsIPAddress(cIP))
        return false;

    std::string IP = cIP;

    LoginDatabase.escape_string(IP);
    QueryResult_AutoPtr result = LoginDatabase.PQuery("SELECT ip, FROM_UNIXTIME(bandate), FROM_UNIXTIME(unbandate), unbandate-UNIX_TIMESTAMP(), banreason,bannedby,unbandate-bandate FROM ip_banned WHERE ip = '%s'", IP.c_str());
    if (!result)
    {
        PSendSysMessage(LANG_BANINFO_NOIP);
        return true;
    }

    Field* fields = result->Fetch();
    bool permanent = !fields[6].GetUInt64();
    PSendSysMessage(LANG_BANINFO_IPENTRY,
                    fields[0].GetString(), fields[1].GetString(), permanent ? GetOregonString(LANG_BANINFO_NEVER) : fields[2].GetString(),
                    permanent ? GetOregonString(LANG_BANINFO_INFINITE) : secsToTimeString(fields[3].GetUInt64(), true).c_str(), fields[4].GetString(), fields[5].GetString());

    return true;
}

bool ChatHandler::HandleBanListCharacterCommand(const char* args)
{
    LoginDatabase.Execute("DELETE FROM ip_banned WHERE unbandate <= UNIX_TIMESTAMP() AND unbandate<>bandate");

    char* cFilter = strtok ((char*)args, " ");
    if (!cFilter)
        return false;

    std::string filter = cFilter;
    LoginDatabase.escape_string(filter);
    QueryResult_AutoPtr result = CharacterDatabase.PQuery("SELECT account FROM characters WHERE name " _LIKE_ " " _CONCAT3_("'%%'", "'%s'", "'%%'"), filter.c_str());
    if (!result)
    {
        PSendSysMessage(LANG_BANLIST_NOCHARACTER);
        return true;
    }

    return HandleBanListHelper(result);
}

bool ChatHandler::HandleBanListAccountCommand(const char* args)
{
    LoginDatabase.Execute("DELETE FROM ip_banned WHERE unbandate <= UNIX_TIMESTAMP() AND unbandate<>bandate");

    char* cFilter = strtok((char*)args, " ");
    std::string filter = cFilter ? cFilter : "";
    LoginDatabase.escape_string(filter);

    QueryResult_AutoPtr result;

    if (filter.empty())
    {
        result = LoginDatabase.Query("SELECT account.id, username FROM account, account_banned"
                                     " WHERE account.id = account_banned.id AND active = 1 GROUP BY account.id");
    }
    else
    {
        result = LoginDatabase.PQuery("SELECT account.id, username FROM account, account_banned"
                                      " WHERE account.id = account_banned.id AND active = 1 AND username " _LIKE_ " " _CONCAT3_("'%%'", "'%s'", "'%%'")" GROUP BY account.id",
                                      filter.c_str());
    }

    if (!result)
    {
        PSendSysMessage(LANG_BANLIST_NOACCOUNT);
        return true;
    }

    return HandleBanListHelper(result);
}

bool ChatHandler::HandleBanListHelper(QueryResult_AutoPtr result)
{
    PSendSysMessage(LANG_BANLIST_MATCHINGACCOUNT);

    // Chat short output
    if (m_session)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 accountid = fields[0].GetUInt32();

            QueryResult_AutoPtr banresult = LoginDatabase.PQuery("SELECT account.username FROM account,account_banned WHERE account_banned.id='%u' AND account_banned.id=account.id", accountid);
            if (banresult)
            {
                Field* fields2 = banresult->Fetch();
                PSendSysMessage("%s", fields2[0].GetString());
            }
        }
        while (result->NextRow());
    }
    // Console wide output
    else
    {
        SendSysMessage(LANG_BANLIST_ACCOUNTS);
        SendSysMessage(" ===============================================================================");
        SendSysMessage(LANG_BANLIST_ACCOUNTS_HEADER);
        do
        {
            SendSysMessage("-------------------------------------------------------------------------------");
            Field* fields = result->Fetch();
            uint32 account_id = fields[0].GetUInt32 ();

            std::string account_name;

            // "account" case, name can be get in same query
            if (result->GetFieldCount() > 1)
                account_name = fields[1].GetCppString();
            // "character" case, name need extract from another DB
            else
                sAccountMgr->GetName (account_id, account_name);

            // No SQL injection. id is uint32.
            QueryResult_AutoPtr banInfo = LoginDatabase.PQuery("SELECT bandate,unbandate,bannedby,banreason FROM account_banned WHERE id = %u ORDER BY unbandate", account_id);
            if (banInfo)
            {
                Field* fields2 = banInfo->Fetch();
                do
                {
                    time_t t_ban = fields2[0].GetUInt64();
                    tm* aTm_ban = localtime(&t_ban);

                    if (fields2[0].GetUInt64() == fields2[1].GetUInt64())
                    {
                        PSendSysMessage("|%-15.15s|%02d-%02d-%02d %02d:%02d|   permanent  |%-15.15s|%-15.15s|",
                                        account_name.c_str(), aTm_ban->tm_year % 100, aTm_ban->tm_mon + 1, aTm_ban->tm_mday, aTm_ban->tm_hour, aTm_ban->tm_min,
                                        fields2[2].GetString(), fields2[3].GetString());
                    }
                    else
                    {
                        time_t t_unban = fields2[1].GetUInt64();
                        tm* aTm_unban = localtime(&t_unban);
                        PSendSysMessage("|%-15.15s|%02d-%02d-%02d %02d:%02d|%02d-%02d-%02d %02d:%02d|%-15.15s|%-15.15s|",
                                        account_name.c_str(), aTm_ban->tm_year % 100, aTm_ban->tm_mon + 1, aTm_ban->tm_mday, aTm_ban->tm_hour, aTm_ban->tm_min,
                                        aTm_unban->tm_year % 100, aTm_unban->tm_mon + 1, aTm_unban->tm_mday, aTm_unban->tm_hour, aTm_unban->tm_min,
                                        fields2[2].GetString(), fields2[3].GetString());
                    }
                }
                while (banInfo->NextRow());
            }
        }
        while (result->NextRow());
        SendSysMessage(" ===============================================================================");
    }
    return true;
}

bool ChatHandler::HandleBanListIPCommand(const char* args)
{
    LoginDatabase.Execute("DELETE FROM ip_banned WHERE unbandate <= UNIX_TIMESTAMP() AND unbandate<>bandate");

    char* cFilter = strtok((char*)args, " ");
    std::string filter = cFilter ? cFilter : "";
    LoginDatabase.escape_string(filter);

    QueryResult_AutoPtr result;

    if (filter.empty())
    {
        result = LoginDatabase.Query ("SELECT ip,bandate,unbandate,bannedby,banreason FROM ip_banned"
                                      " WHERE (bandate=unbandate OR unbandate>UNIX_TIMESTAMP())"
                                      " ORDER BY unbandate");
    }
    else
    {
        result = LoginDatabase.PQuery("SELECT ip,bandate,unbandate,bannedby,banreason FROM ip_banned"
                                      " WHERE (bandate=unbandate OR unbandate>UNIX_TIMESTAMP()) AND ip " _LIKE_ " " _CONCAT3_("'%%'", "'%s'", "'%%'")
                                      " ORDER BY unbandate", filter.c_str());
    }

    if (!result)
    {
        PSendSysMessage(LANG_BANLIST_NOIP);
        return true;
    }

    PSendSysMessage(LANG_BANLIST_MATCHINGIP);
    // Chat short output
    if (m_session)
    {
        do
        {
            Field* fields = result->Fetch();
            PSendSysMessage("%s", fields[0].GetString());
        }
        while (result->NextRow());
    }
    // Console wide output
    else
    {
        SendSysMessage(LANG_BANLIST_IPS);
        SendSysMessage(" ===============================================================================");
        SendSysMessage(LANG_BANLIST_IPS_HEADER);
        do
        {
            SendSysMessage("-------------------------------------------------------------------------------");
            Field* fields = result->Fetch();
            time_t t_ban = fields[1].GetUInt64();
            tm* aTm_ban = localtime(&t_ban);
            if (fields[1].GetUInt64() == fields[2].GetUInt64())
            {
                PSendSysMessage("|%-15.15s|%02d-%02d-%02d %02d:%02d|   permanent  |%-15.15s|%-15.15s|",
                                fields[0].GetString(), aTm_ban->tm_year % 100, aTm_ban->tm_mon + 1, aTm_ban->tm_mday, aTm_ban->tm_hour, aTm_ban->tm_min,
                                fields[3].GetString(), fields[4].GetString());
            }
            else
            {
                time_t t_unban = fields[2].GetUInt64();
                tm* aTm_unban = localtime(&t_unban);
                PSendSysMessage("|%-15.15s|%02d-%02d-%02d %02d:%02d|%02d-%02d-%02d %02d:%02d|%-15.15s|%-15.15s|",
                                fields[0].GetString(), aTm_ban->tm_year % 100, aTm_ban->tm_mon + 1, aTm_ban->tm_mday, aTm_ban->tm_hour, aTm_ban->tm_min,
                                aTm_unban->tm_year % 100, aTm_unban->tm_mon + 1, aTm_unban->tm_mday, aTm_unban->tm_hour, aTm_unban->tm_min,
                                fields[3].GetString(), fields[4].GetString());
            }
        }
        while (result->NextRow());
        SendSysMessage(" ===============================================================================");
    }

    return true;
}

bool ChatHandler::HandleRespawnCommand(const char* /*args*/)
{
    Player* pl = m_session->GetPlayer();

    // accept only explicitly selected target (not implicitly self targeting case)
    Unit* target = getSelectedUnit();
    if (pl->GetSelection() && target)
    {
        if (target->GetTypeId() != TYPEID_UNIT)
        {
            SendSysMessage(LANG_SELECT_CREATURE);
            SetSentErrorMessage(true);
            return false;
        }
        else if (!target->isDead())
        {
            SendSysMessage(LANG_CREATURE_NOT_DEAD);
            SetSentErrorMessage(true);
            return false;
        }
        target->ToCreature()->Respawn();
        return true;
    }

    CellCoord p(Oregon::ComputeCellCoord(pl->GetPositionX(), pl->GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Oregon::RespawnDo u_do;
    Oregon::WorldObjectWorker<Oregon::RespawnDo> worker(u_do);

    TypeContainerVisitor<Oregon::WorldObjectWorker<Oregon::RespawnDo>, GridTypeMapContainer > obj_worker(worker);
    cell.Visit(p, obj_worker, *pl->GetMap(), *pl, pl->GetGridActivationRange());

    return true;
}

bool ChatHandler::HandleFlyModeCommand(const char* args)
{
    if (!*args)
        return false;

    Unit* unit = getSelectedUnit();
    if (!unit || (unit->GetTypeId() != TYPEID_PLAYER))
        unit = m_session->GetPlayer();

    WorldPacket data(12);
    if (strncmp(args, "on", 3) == 0)
        data.SetOpcode(SMSG_MOVE_SET_CAN_FLY);
    else if (strncmp(args, "off", 4) == 0)
        data.SetOpcode(SMSG_MOVE_UNSET_CAN_FLY);
    else
    {
        SendSysMessage(LANG_USE_BOL);
        return false;
    }
    data << unit->GetPackGUID();
    data << uint32(0);                                      // unknown
    unit->SendMessageToSet(&data, true);
    PSendSysMessage(LANG_COMMAND_FLYMODE_STATUS, unit->GetName(), args);
    return true;
}

bool ChatHandler::HandleLoadPDumpCommand(const char* args)
{
    if (!*args)
        return false;

    char* file = strtok((char*)args, " ");
    if (!file)
        return false;

    char* account = strtok(NULL, " ");
    if (!account)
        return false;

    std::string account_name = account;
    if (!AccountMgr::normalizeString(account_name))
    {
        PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
        SetSentErrorMessage(true);
        return false;
    }

    uint32 account_id = sAccountMgr->GetId(account_name);
    if (!account_id)
    {
        account_id = atoi(account);                             // use original string
        if (!account_id)
        {
            PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
            SetSentErrorMessage(true);
            return false;
        }
    }

    if (!sAccountMgr->GetName(account_id, account_name))
    {
        PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
        SetSentErrorMessage(true);
        return false;
    }

    char* guid_str = NULL;
    char* name_str = strtok(NULL, " ");

    std::string name;
    if (name_str)
    {
        name = name_str;
        // normalize the name if specified and check if it exists
        if (!normalizePlayerName(name))
        {
            PSendSysMessage(LANG_INVALID_CHARACTER_NAME);
            SetSentErrorMessage(true);
            return false;
        }

        if (!ObjectMgr::IsValidName(name, true))
        {
            PSendSysMessage(LANG_INVALID_CHARACTER_NAME);
            SetSentErrorMessage(true);
            return false;
        }

        guid_str = strtok(NULL, " ");
    }

    uint32 guid = 0;

    if (guid_str)
    {
        guid = atoi(guid_str);
        if (!guid)
        {
            PSendSysMessage(LANG_INVALID_CHARACTER_GUID);
            SetSentErrorMessage(true);
            return false;
        }

        if (sObjectMgr.GetPlayerAccountIdByGUID(guid))
        {
            PSendSysMessage(LANG_CHARACTER_GUID_IN_USE, guid);
            SetSentErrorMessage(true);
            return false;
        }
    }

    switch (PlayerDumpReader().LoadDump(file, account_id, name, guid))
    {
    case DUMP_SUCCESS:
        PSendSysMessage(LANG_COMMAND_IMPORT_SUCCESS);
        break;
    case DUMP_FILE_OPEN_ERROR:
        PSendSysMessage(LANG_FILE_OPEN_FAIL, file);
        SetSentErrorMessage(true);
        return false;
    case DUMP_FILE_BROKEN:
        PSendSysMessage(LANG_DUMP_BROKEN, file);
        SetSentErrorMessage(true);
        return false;
    case DUMP_TOO_MANY_CHARS:
        PSendSysMessage(LANG_ACCOUNT_CHARACTER_LIST_FULL, account_name.c_str(), account_id);
        SetSentErrorMessage(true);
        return false;
    default:
        PSendSysMessage(LANG_COMMAND_IMPORT_FAILED);
        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

bool ChatHandler::HandleNpcChangeEntryCommand(const char* args)
{
    if (!*args)
        return false;

    uint32 newEntryNum = atoi(args);
    if (!newEntryNum)
        return false;

    Unit* unit = getSelectedUnit();
    if (!unit || unit->GetTypeId() != TYPEID_UNIT)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }
    Creature* creature = unit->ToCreature();
    if (creature->UpdateEntry(newEntryNum))
        SendSysMessage(LANG_DONE);
    else
        SendSysMessage(LANG_ERROR);
    return true;
}

bool ChatHandler::HandleWritePDumpCommand(const char* args)
{
    if (!*args)
        return false;

    char* file = strtok((char*)args, " ");
    char* p2 = strtok(NULL, " ");

    if (!file || !p2)
        return false;

    uint32 guid;
    // character name can't start from number
    if (isNumeric(p2))
        guid = atoi(p2);
    else
    {
        std::string name = p2;

        if (!normalizePlayerName (name))
        {
            SendSysMessage(LANG_PLAYER_NOT_FOUND);
            SetSentErrorMessage(true);
            return false;
        }

        guid = sObjectMgr.GetPlayerGUIDByName(name);
    }

    if (!sObjectMgr.GetPlayerAccountIdByGUID(guid))
    {
        PSendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    switch (PlayerDumpWriter().WriteDump(file, guid))
    {
    case DUMP_SUCCESS:
        PSendSysMessage(LANG_COMMAND_EXPORT_SUCCESS);
        break;
    case DUMP_FILE_OPEN_ERROR:
        PSendSysMessage(LANG_FILE_OPEN_FAIL, file);
        SetSentErrorMessage(true);
        return false;
    default:
        PSendSysMessage(LANG_COMMAND_EXPORT_FAILED);
        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

bool ChatHandler::HandleMovegensCommand(const char* /*args*/)
{
    Unit* unit = getSelectedUnit();
    if (!unit)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_MOVEGENS_LIST, (unit->GetTypeId() == TYPEID_PLAYER ? "Player" : "Creature"), unit->GetGUIDLow());

    MotionMaster* mm = unit->GetMotionMaster();
    for (uint8 i = 0; i < MAX_MOTION_SLOT; ++i)
    {
        MovementGenerator* mg = mm->GetMotionSlot(i);
        if (!mg)
        {
            SendSysMessage("Empty");
            continue;
        }
        switch (mg->GetMovementGeneratorType())
        {
        case IDLE_MOTION_TYPE:
            SendSysMessage(LANG_MOVEGENS_IDLE);
            break;
        case RANDOM_MOTION_TYPE:
            SendSysMessage(LANG_MOVEGENS_RANDOM);
            break;
        case WAYPOINT_MOTION_TYPE:
            SendSysMessage(LANG_MOVEGENS_WAYPOINT);
            break;
        case ANIMAL_RANDOM_MOTION_TYPE:
            SendSysMessage(LANG_MOVEGENS_ANIMAL_RANDOM);
            break;
        case CONFUSED_MOTION_TYPE:
            SendSysMessage(LANG_MOVEGENS_CONFUSED);
            break;
        case CHASE_MOTION_TYPE:
            {
                if (unit->GetTypeId() == TYPEID_PLAYER)
                {
                ChaseMovementGenerator<Player> const* mgen = static_cast<ChaseMovementGenerator<Player> const*>(mg);
                    Unit* target = mgen->GetTarget();
                    if (target)
                        PSendSysMessage(LANG_MOVEGENS_TARGETED_PLAYER, target->GetName(), target->GetGUIDLow());
                    else
                        SendSysMessage(LANG_MOVEGENS_TARGETED_NULL);
                }
                else
                {
                ChaseMovementGenerator<Creature> const* mgen = static_cast<ChaseMovementGenerator<Creature> const*>(mg);
                    Unit* target = mgen->GetTarget();
                    if (target)
                        PSendSysMessage(LANG_MOVEGENS_TARGETED_CREATURE, target->GetName(), target->GetGUIDLow());
                    else
                        SendSysMessage(LANG_MOVEGENS_TARGETED_NULL);
                }
                break;
            }
        case FOLLOW_MOTION_TYPE:
        {
            if (unit->GetTypeId() == TYPEID_PLAYER)
            {
                FollowMovementGenerator<Player> const* mgen = static_cast<FollowMovementGenerator<Player> const*>(mg);
                Unit* target = mgen->GetTarget();
                if (target)
                    PSendSysMessage(LANG_MOVEGENS_TARGETED_PLAYER, target->GetName(), target->GetGUIDLow());
                else
                    SendSysMessage(LANG_MOVEGENS_TARGETED_NULL);
            }
            else
            {
                FollowMovementGenerator<Creature> const* mgen = static_cast<FollowMovementGenerator<Creature> const*>(mg);
                Unit* target = mgen->GetTarget();
                if (target)
                    PSendSysMessage(LANG_MOVEGENS_TARGETED_CREATURE, target->GetName(), target->GetGUIDLow());
                else
                    SendSysMessage(LANG_MOVEGENS_TARGETED_NULL);
            }
            break;
        }
        case HOME_MOTION_TYPE:
            if (unit->GetTypeId() == TYPEID_UNIT)
            {
                float x, y, z;
                mm->GetDestination(x, y, z);
                PSendSysMessage(LANG_MOVEGENS_HOME_CREATURE, x, y, z);
            }
            else
                SendSysMessage(LANG_MOVEGENS_HOME_PLAYER);
            break;
        case FLIGHT_MOTION_TYPE:
            SendSysMessage(LANG_MOVEGENS_FLIGHT);
            break;
        case POINT_MOTION_TYPE:
            {
                float x, y, z;
                mm->GetDestination(x, y, z);
                PSendSysMessage(LANG_MOVEGENS_POINT, x, y, z);
                break;
            }
        case FLEEING_MOTION_TYPE:
            SendSysMessage(LANG_MOVEGENS_FEAR);
            break;
        case DISTRACT_MOTION_TYPE:
            SendSysMessage(LANG_MOVEGENS_DISTRACT);
            break;
        default:
            PSendSysMessage(LANG_MOVEGENS_UNKNOWN, mg->GetMovementGeneratorType());
            break;
        }
    }
    return true;
}

bool ChatHandler::HandleServerPLimitCommand(const char* args)
{
    if (*args)
    {
        char* param = strtok((char*)args, " ");
        if (!param)
            return false;

        int l = strlen(param);

        if (strncmp(param, "player", l) == 0)
            sWorld.SetPlayerSecurityLimit(SEC_PLAYER);
        else if (strncmp(param, "moderator", l) == 0)
            sWorld.SetPlayerSecurityLimit(SEC_MODERATOR);
        else if (strncmp(param, "gamemaster", l) == 0)
            sWorld.SetPlayerSecurityLimit(SEC_GAMEMASTER);
        else if (strncmp(param, "administrator", l) == 0)
            sWorld.SetPlayerSecurityLimit(SEC_ADMINISTRATOR);
        else if (strncmp(param, "reset", l) == 0)
            sWorld.SetPlayerLimit(sConfig.GetIntDefault("PlayerLimit", DEFAULT_PLAYER_LIMIT));
        else
        {
            int val = atoi(param);
            if (val < 0)
                sWorld.SetPlayerSecurityLimit(AccountTypes(uint32(-val)));
            else
                sWorld.SetPlayerLimit(val);
        }

        // kick all low security level players
        if (sWorld.GetPlayerSecurityLimit() > SEC_PLAYER)
            sWorld.KickAllLess(sWorld.GetPlayerSecurityLimit());
    }

    uint32 pLimit = sWorld.GetPlayerAmountLimit();
    AccountTypes allowedAccountType = sWorld.GetPlayerSecurityLimit();
    char const* secName = "";
    switch (allowedAccountType)
    {
    case SEC_PLAYER:
        secName = "Player";
        break;
    case SEC_MODERATOR:
        secName = "Moderator";
        break;
    case SEC_GAMEMASTER:
        secName = "Gamemaster";
        break;
    case SEC_ADMINISTRATOR:
        secName = "Administrator";
        break;
    default:
        secName = "<unknown>";
        break;
    }

    PSendSysMessage("Player limits: amount %u, min. security level %s.", pLimit, secName);

    return true;
}

bool ChatHandler::HandleCastCommand(const char* args)
{
    if (!*args)
        return false;

    Unit* target = getSelectedUnit();

    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
    uint32 spell = extractSpellIdFromLink((char*)args);
    if (!spell)
        return false;

    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell);
    if (!spellInfo)
    {
        PSendSysMessage(LANG_COMMAND_NOSPELLFOUND);
        SetSentErrorMessage(true);
        return false;
    }

    if (!SpellMgr::IsSpellValid(spellInfo, m_session->GetPlayer()))
    {
        PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spell);
        SetSentErrorMessage(true);
        return false;
    }

    char* trig_str = strtok(NULL, " ");
    if (trig_str)
    {
        int l = strlen(trig_str);
        if (strncmp(trig_str, "triggered", l) != 0)
            return false;
    }

    bool triggered = (trig_str != NULL);

    m_session->GetPlayer()->CastSpell(target, spell, triggered);

    return true;
}

bool ChatHandler::HandleCastBackCommand(const char* args)
{
    Creature* caster = getSelectedCreature();

    if (!caster)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r
    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
    uint32 spell = extractSpellIdFromLink((char*)args);
    if (!spell || !sSpellStore.LookupEntry(spell))
    {
        PSendSysMessage(LANG_COMMAND_NOSPELLFOUND);
        SetSentErrorMessage(true);
        return false;
    }

    char* trig_str = strtok(NULL, " ");
    if (trig_str)
    {
        int l = strlen(trig_str);
        if (strncmp(trig_str, "triggered", l) != 0)
            return false;
    }

    bool triggered = (trig_str != NULL);

    caster->SetFacingToObject(m_session->GetPlayer());

    caster->CastSpell(m_session->GetPlayer(), spell, triggered);

    return true;
}

bool ChatHandler::HandleCastDistCommand(const char* args)
{
    if (!*args)
        return false;

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
    uint32 spell = extractSpellIdFromLink((char*)args);
    if (!spell)
        return false;

    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell);
    if (!spellInfo)
    {
        PSendSysMessage(LANG_COMMAND_NOSPELLFOUND);
        SetSentErrorMessage(true);
        return false;
    }

    if (!SpellMgr::IsSpellValid(spellInfo, m_session->GetPlayer()))
    {
        PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spell);
        SetSentErrorMessage(true);
        return false;
    }

    char* distStr = strtok(NULL, " ");

    float dist = 0;

    if (distStr)
        sscanf(distStr, "%f", &dist);

    char* trig_str = strtok(NULL, " ");
    if (trig_str)
    {
        int l = strlen(trig_str);
        if (strncmp(trig_str, "triggered", l) != 0)
            return false;
    }

    bool triggered = (trig_str != NULL);

    float x, y, z;
    m_session->GetPlayer()->GetClosePoint(x, y, z, dist);

    m_session->GetPlayer()->CastSpell(x, y, z, spell, triggered);
    return true;
}

bool ChatHandler::HandleCastTargetCommand(const char* args)
{
    Creature* caster = getSelectedCreature();

    if (!caster)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    if (!caster->GetVictim())
    {
        SendSysMessage(LANG_SELECTED_TARGET_NOT_HAVE_VICTIM);
        SetSentErrorMessage(true);
        return false;
    }

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
    uint32 spell = extractSpellIdFromLink((char*)args);
    if (!spell || !sSpellStore.LookupEntry(spell))
    {
        PSendSysMessage(LANG_COMMAND_NOSPELLFOUND);
        SetSentErrorMessage(true);
        return false;
    }

    char* trig_str = strtok(NULL, " ");
    if (trig_str)
    {
        int l = strlen(trig_str);
        if (strncmp(trig_str, "triggered", l) != 0)
            return false;
    }

    bool triggered = (trig_str != NULL);

    caster->SetFacingToObject(m_session->GetPlayer());

    caster->CastSpell(caster->GetVictim(), spell, triggered);

    return true;
}

/*
ComeToMe command REQUIRED for 3rd party scripting library to have access to PointMovementGenerator
Without this function 3rd party scripting library will get linking errors (unresolved external)
when attempting to use the PointMovementGenerator
*/
bool ChatHandler::HandleComeToMeCommand(const char* args)
{
    char* newFlagStr = strtok((char*)args, " ");

    if (!newFlagStr)
        return false;

    uint32 newFlags = (uint32)strtoul(newFlagStr, NULL, 0);

    Creature* caster = getSelectedCreature();
    if (!caster)
    {
        m_session->GetPlayer()->SetUnitMovementFlags(newFlags);
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    caster->SetUnitMovementFlags(newFlags);

    Player* pl = m_session->GetPlayer();

    caster->GetMotionMaster()->MovePoint(0, pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ());
    return true;
}

bool ChatHandler::HandleCastSelfCommand(const char* args)
{
    if (!*args)
        return false;

    Unit* target = getSelectedUnit();

    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
    uint32 spell = extractSpellIdFromLink((char*)args);
    if (!spell)
        return false;

    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell);
    if (!spellInfo)
        return false;

    if (!SpellMgr::IsSpellValid(spellInfo, m_session->GetPlayer()))
    {
        PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spell);
        SetSentErrorMessage(true);
        return false;
    }

    target->CastSpell(target, spell, false);

    return true;
}

std::string GetTimeString(uint32 time)
{
    uint16 days = time / DAY, hours = (time % DAY) / HOUR, minute = (time % HOUR) / MINUTE;
    std::ostringstream ss;
    if (days) ss << days << "d ";
    if (hours) ss << hours << "h ";
    ss << minute << "m";
    return ss.str();
}

bool ChatHandler::HandleInstanceListBindsCommand(const char* /*args*/)
{
    Player* player = getSelectedPlayer();
    if (!player) player = m_session->GetPlayer();
    uint32 counter = 0;
    for (uint8 i = 0; i < TOTAL_DIFFICULTIES; ++i)
    {
        Player::BoundInstancesMap& binds = player->GetBoundInstances(i);
        for (Player::BoundInstancesMap::iterator itr = binds.begin(); itr != binds.end(); ++itr)
        {
            InstanceSave* save = itr->second.save;
            std::string timeleft = GetTimeString(save->GetResetTime() - time(NULL));
            PSendSysMessage("map: %d inst: %d perm: %s diff: %s canReset: %s TTR: %s", itr->first, save->GetInstanceId(), itr->second.perm ? "yes" : "no",  save->GetDifficulty() == DIFFICULTY_NORMAL ? "normal" : "heroic", save->CanReset() ? "yes" : "no", timeleft.c_str());
            counter++;
        }
    }
    PSendSysMessage("player binds: %d", counter);
    counter = 0;
    Group* group = player->GetGroup();
    if (group)
    {
        for (uint8 i = 0; i < TOTAL_DIFFICULTIES; ++i)
        {
            Group::BoundInstancesMap& binds = group->GetBoundInstances(DungeonDifficulty(i));
            for (Group::BoundInstancesMap::iterator itr = binds.begin(); itr != binds.end(); ++itr)
            {
                InstanceSave* save = itr->second.save;
                std::string timeleft = GetTimeString(save->GetResetTime() - time(NULL));
                PSendSysMessage("map: %d inst: %d perm: %s diff: %s canReset: %s TTR: %s", itr->first, save->GetInstanceId(), itr->second.perm ? "yes" : "no",  save->GetDifficulty() == DIFFICULTY_NORMAL ? "normal" : "heroic", save->CanReset() ? "yes" : "no", timeleft.c_str());
                counter++;
            }
        }
    }
    PSendSysMessage("group binds: %d", counter);

    return true;
}

bool ChatHandler::HandleInstanceUnbindCommand(const char* args)
{
    if (!*args)
        return false;

    std::string cmd = args;
    if (cmd == "all")
    {
        Player* player = getSelectedPlayer();
        if (!player) player = m_session->GetPlayer();
        uint32 counter = 0;
        for (uint8 i = 0; i < TOTAL_DIFFICULTIES; i++)
        {
            Player::BoundInstancesMap& binds = player->GetBoundInstances(i);
            for (Player::BoundInstancesMap::iterator itr = binds.begin(); itr != binds.end();)
            {
                if (itr->first != player->GetMapId())
                {
                    InstanceSave* save = itr->second.save;
                    std::string timeleft = GetTimeString(save->GetResetTime() - time(NULL));
                    PSendSysMessage("unbinding map: %d inst: %d perm: %s diff: %s canReset: %s TTR: %s", itr->first, save->GetInstanceId(), itr->second.perm ? "yes" : "no",  save->GetDifficulty() == DIFFICULTY_NORMAL ? "normal" : "heroic", save->CanReset() ? "yes" : "no", timeleft.c_str());
                    player->UnbindInstance(itr, i);
                    counter++;
                }
                else
                    ++itr;
            }
        }
        PSendSysMessage("instances unbound: %d", counter);
    }
    return true;
}

bool ChatHandler::HandleInstanceStatsCommand(const char* /*args*/)
{
    PSendSysMessage("instances loaded: %d", MapManager::Instance().GetNumInstances());
    PSendSysMessage("players in instances: %d", MapManager::Instance().GetNumPlayersInInstances());
    PSendSysMessage("instance saves: %d", sInstanceSaveMgr.GetNumInstanceSaves());
    PSendSysMessage("players bound: %d", sInstanceSaveMgr.GetNumBoundPlayersTotal());
    PSendSysMessage("groups bound: %d", sInstanceSaveMgr.GetNumBoundGroupsTotal());
    return true;
}

bool ChatHandler::HandleInstanceSaveDataCommand(const char* /*args*/)
{
    Player* pl = m_session->GetPlayer();

    Map* map = pl->GetMap();
    if (!map->IsDungeon())
    {
        PSendSysMessage("Map is not a dungeon.");
        SetSentErrorMessage(true);
        return false;
    }

    if (!((InstanceMap*)map)->GetInstanceData())
    {
        PSendSysMessage("Map has no instance data.");
        SetSentErrorMessage(true);
        return false;
    }

    ((InstanceMap*)map)->GetInstanceData()->SaveToDB();
    return true;
}

// Display the list of GMs
bool ChatHandler::HandleGMListFullCommand(const char* /*args*/)
{
    // Get the accounts with GM Level >0
    QueryResult_AutoPtr result = LoginDatabase.Query("SELECT a.username,aa.gmlevel FROM account a, account_access aa WHERE a.id=aa.id AND aa.gmlevel > 0");
    if (result)
    {
        SendSysMessage(LANG_GMLIST);
        SendSysMessage(" ======================== ");
        SendSysMessage(LANG_GMLIST_HEADER);
        SendSysMessage(" ======================== ");

        // Circle through them. Display username and GM level
        do
        {
            Field* fields = result->Fetch();
            PSendSysMessage("|%15s|%6s|", fields[0].GetString(), fields[1].GetString());
        }
        while (result->NextRow());

        PSendSysMessage(" ======================== ");
    }
    else
        PSendSysMessage(LANG_GMLIST_EMPTY);
    return true;
}

// Define the 'Message of the day' for the realm
bool ChatHandler::HandleServerSetMotdCommand(const char* args)
{
    sWorld.SetMotd(args);
    PSendSysMessage(LANG_MOTD_NEW, args);
    return true;
}

// Set/Unset the expansion level for an account
bool ChatHandler::HandleAccountSetAddonCommand(const char* args)
{
    // Get the command line arguments
    char* szAcc = strtok((char*)args, " ");
    char* szExp = strtok(NULL, " ");

    if (!szAcc)
        return false;

    std::string account_name;
    uint32 account_id;

    if (!szExp)
    {
        Player* player = getSelectedPlayer();
        if (!player)
            return false;

        account_id = player->GetSession()->GetAccountId();
        sAccountMgr->GetName(account_id, account_name);
        szExp = szAcc;
    }
    else
    {
        // Convert Account name to Upper Format
        account_name = szAcc;
        if (!AccountMgr::normalizeString(account_name))
        {
            PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
            SetSentErrorMessage(true);
            return false;
        }

        account_id = sAccountMgr->GetId(account_name);
        if (!account_id)
        {
            PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
            SetSentErrorMessage(true);
            return false;
        }
    }

    int lev = atoi(szExp);                                  //get int anyway (0 if error)
    if (lev < 0)
        return false;

    // No SQL injection
    LoginDatabase.PExecute("UPDATE account SET expansion = '%d' WHERE id = '%u'", lev, account_id);
    PSendSysMessage(LANG_ACCOUNT_SETADDON, account_name.c_str(), account_id, lev);
    return true;
}

//Send items by mail
bool ChatHandler::HandleSendItemsCommand(const char* args)
{
    if (!*args)
        return false;

    // format: name "subject text" "mail text" item1[:count1] item2[:count2] ... item12[:count12]

    char* pName = strtok((char*)args, " ");
    if (!pName)
        return false;

    char* tail1 = strtok(NULL, "");
    if (!tail1)
        return false;

    char* msgSubject;
    if (*tail1 == '"')
        msgSubject = strtok(tail1 + 1, "\"");
    else
    {
        char* space = strtok(tail1, "\"");
        if (!space)
            return false;
        msgSubject = strtok(NULL, "\"");
    }

    if (!msgSubject)
        return false;

    char* tail2 = strtok(NULL, "");
    if (!tail2)
        return false;

    char* msgText;
    if (*tail2 == '"')
        msgText = strtok(tail2 + 1, "\"");
    else
    {
        char* space = strtok(tail2, "\"");
        if (!space)
            return false;
        msgText = strtok(NULL, "\"");
    }

    if (!msgText)
        return false;

    // pName, msgSubject, msgText isn't NUL after prev. check
    std::string name    = pName;
    std::string subject = msgSubject;
    std::string text    = msgText;

    // extract items
    typedef std::pair<uint32, uint32> ItemPair;
    typedef std::list< ItemPair > ItemPairs;
    ItemPairs items;

    // get all tail string
    char* tail = strtok(NULL, "");

    // get from tail next item str
    while (char* itemStr = strtok(tail, " "))
    {
        // and get new tail
        tail = strtok(NULL, "");

        // parse item str
        char* itemIdStr = strtok(itemStr, ":");
        char* itemCountStr = strtok(NULL, " ");

        uint32 item_id = atoi(itemIdStr);
        if (!item_id)
            return false;

        ItemTemplate const* item_proto = sObjectMgr.GetItemTemplate(item_id);
        if (!item_proto)
        {
            PSendSysMessage(LANG_COMMAND_ITEMIDINVALID, item_id);
            SetSentErrorMessage(true);
            return false;
        }

        uint32 item_count = itemCountStr ? atoi(itemCountStr) : 1;
        if (item_count < 1 || (item_proto->MaxCount && item_count > item_proto->MaxCount))
        {
            PSendSysMessage(LANG_COMMAND_INVALID_ITEM_COUNT, item_count, item_id);
            SetSentErrorMessage(true);
            return false;
        }

        while (item_count > item_proto->Stackable)
        {
            items.push_back(ItemPair(item_id, item_proto->Stackable));
            item_count -= item_proto->Stackable;
        }

        items.push_back(ItemPair(item_id, item_count));

        if (items.size() > MAX_MAIL_ITEMS)
        {
            PSendSysMessage(LANG_COMMAND_MAIL_ITEMS_LIMIT, MAX_MAIL_ITEMS);
            SetSentErrorMessage(true);
            return false;
        }
    }

    if (!normalizePlayerName(name))
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    uint64 receiver_guid = sObjectMgr.GetPlayerGUIDByName(name);
    if (!receiver_guid)
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    // from console show not existed sender
    MailSender sender(MAIL_NORMAL, m_session ? m_session->GetPlayer()->GetGUIDLow() : 0, MAIL_STATIONERY_GM);

    uint32 itemTextId = !text.empty() ? sObjectMgr.CreateItemText(text) : 0;

    Player* receiver = sObjectMgr.GetPlayer(receiver_guid);

    // fill mail
    MailDraft draft(subject, itemTextId);

    for (ItemPairs::const_iterator itr = items.begin(); itr != items.end(); ++itr)
    {
        if (Item* item = Item::CreateItem(itr->first, itr->second, m_session ? m_session->GetPlayer() : 0))
        {
            item->SaveToDB();                               // save for prevent lost at next mail load, if send fail then item will deleted
            draft.AddItem(item);
        }
    }

    draft.SendMailTo(MailReceiver(receiver, GUID_LOPART(receiver_guid)), sender);

    PSendSysMessage(LANG_MAIL_SENT, name.c_str());
    return true;
}

// Send money by mail
bool ChatHandler::HandleSendMoneyCommand(const char* args)
{
    if (!*args)
        return false;

    // format: name "subject text" "mail text" money

    char* pName = strtok((char*)args, " ");
    if (!pName)
        return false;

    char* tail1 = strtok(NULL, "");
    if (!tail1)
        return false;

    char* msgSubject;
    if (*tail1 == '"')
        msgSubject = strtok(tail1 + 1, "\"");
    else
    {
        char* space = strtok(tail1, "\"");
        if (!space)
            return false;
        msgSubject = strtok(NULL, "\"");
    }

    if (!msgSubject)
        return false;

    char* tail2 = strtok(NULL, "");
    if (!tail2)
        return false;

    char* msgText;
    if (*tail2 == '"')
        msgText = strtok(tail2 + 1, "\"");
    else
    {
        char* space = strtok(tail2, "\"");
        if (!space)
            return false;
        msgText = strtok(NULL, "\"");
    }

    if (!msgText)
        return false;

    char* money_str = strtok(NULL, "");
    int32 money = money_str ? atoi(money_str) : 0;
    if (money <= 0)
        return false;

    // pName, msgSubject, msgText isn't NUL after prev. check
    std::string name    = pName;
    std::string subject = msgSubject;
    std::string text    = msgText;

    if (!normalizePlayerName(name))
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    uint64 receiver_guid = sObjectMgr.GetPlayerGUIDByName(name);
    if (!receiver_guid)
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    sObjectMgr.GenerateMailID();

    // from console show not existed sender
    MailSender sender(MAIL_NORMAL, m_session ? m_session->GetPlayer()->GetGUIDLow() : 0, MAIL_STATIONERY_GM);

    uint32 itemTextId = !text.empty() ? sObjectMgr.CreateItemText(text) : 0;

    Player* receiver = sObjectMgr.GetPlayer(receiver_guid);

    MailDraft(subject, itemTextId)
    .AddMoney(money)
    .SendMailTo(MailReceiver(receiver, GUID_LOPART(receiver_guid)), sender);

    PSendSysMessage(LANG_MAIL_SENT, name.c_str());
    return true;
}

// Send a message to a player in game
bool ChatHandler::HandleSendMessageCommand(const char* args)
{
    // Get the command line arguments
    char* name_str = strtok((char*)args, " ");
    char* msg_str = strtok(NULL, "");

    if (!name_str || !msg_str)
        return false;

    std::string name = name_str;

    if (!normalizePlayerName(name))
        return false;

    // Find the player and check that he is not logging out.
    Player* rPlayer = sObjectMgr.GetPlayer(name.c_str());
    if (!rPlayer)
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    if (rPlayer->GetSession()->isLogingOut())
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    // Send the message
    // Use SendAreaTriggerMessage for fastest delivery.
    rPlayer->GetSession()->SendAreaTriggerMessage("%s", msg_str);
    rPlayer->GetSession()->SendAreaTriggerMessage("|cffff0000[Message from administrator]:|r");

    //Confirmation message
    PSendSysMessage(LANG_SENDMESSAGE, name.c_str(), msg_str);
    return true;
}

bool ChatHandler::HandleFlushArenaPointsCommand(const char* /*args*/)
{
    sBattlegroundMgr.DistributeArenaPoints();
    return true;
}

bool ChatHandler::HandleModifyGenderCommand(const char* args)
{
    if (!*args)
        return false;

    Player* player = getSelectedPlayer();

    if (!player)
    {
        PSendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    char const* gender_str = (char*)args;
    int gender_len = strlen(gender_str);

    uint32 displayId = player->GetNativeDisplayId();
    char const* gender_full = NULL;
    uint32 new_displayId = displayId;
    Gender gender;

    if (!strncmp(gender_str, "male", gender_len))            // MALE
    {
        if (player->getGender() == GENDER_MALE)
            return true;

        gender_full = "male";
        new_displayId = player->getRace() == RACE_BLOODELF ? displayId + 1 : displayId - 1;
        gender = GENDER_MALE;
    }
    else if (!strncmp(gender_str, "female", gender_len))    // FEMALE
    {
        if (player->getGender() == GENDER_FEMALE)
            return true;

        gender_full = "female";
        new_displayId = player->getRace() == RACE_BLOODELF ? displayId - 1 : displayId + 1;
        gender = GENDER_FEMALE;
    }
    else
    {
        SendSysMessage(LANG_MUST_MALE_OR_FEMALE);
        SetSentErrorMessage(true);
        return false;
    }

    // Set gender
    player->SetByteValue(UNIT_FIELD_BYTES_0, 2, gender);
    player->SetByteValue(PLAYER_BYTES_3, 0, gender);

    // Change display ID
    player->SetDisplayId(new_displayId);
    player->SetNativeDisplayId(new_displayId);

    PSendSysMessage(LANG_YOU_CHANGE_GENDER, player->GetName(), gender_full);
    if (needReportToTarget(player))
        ChatHandler(player).PSendSysMessage(LANG_YOUR_GENDER_CHANGED, gender_full, GetName());

    return true;
}

/*------------------------------------------
 *-------------Oregon----------------------
 *-------------------------------------*/

bool ChatHandler::HandlePlayAllCommand(const char* args)
{
    if (!*args)
        return false;

    uint32 soundId = atoi((char*)args);

    if (!sSoundEntriesStore.LookupEntry(soundId))
    {
        PSendSysMessage(LANG_SOUND_NOT_EXIST, soundId);
        SetSentErrorMessage(true);
        return false;
    }

    WorldPacket data(SMSG_PLAY_SOUND, 4);
    data << uint32(soundId) << m_session->GetPlayer()->GetGUID();
    sWorld.SendGlobalMessage(&data);

    PSendSysMessage(LANG_COMMAND_PLAYED_TO_ALL, soundId);
    return true;
}

bool ChatHandler::HandleFreezeCommand(const char* args)
{
    std::string name;
    Player* player;
    char* TargetName = strtok((char*)args, " "); //get entered name
    if (!TargetName) //if no name entered use target
    {
        player = getSelectedPlayer();
        if (player) //prevent crash with creature as target
        {
            name = player->GetName();
            normalizePlayerName(name);
        }
    }
    else // if name entered
    {
        name = TargetName;
        normalizePlayerName(name);
        player = sObjectMgr.GetPlayer(name.c_str()); //get player by name
    }

    if (!player)
    {
        SendSysMessage(LANG_COMMAND_FREEZE_WRONG);
        return true;
    }

    if (player == m_session->GetPlayer())
    {
        SendSysMessage(LANG_COMMAND_FREEZE_ERROR);
        return true;
    }

    //effect
    if (player && player != m_session->GetPlayer())
    {
        PSendSysMessage(LANG_COMMAND_FREEZE, name.c_str());

        //stop combat + make player unattackable + duel stop + stop some spells
        player->SetFaction(35);
        player->CombatStop();
        if (player->IsNonMeleeSpellCast(true))
            player->InterruptNonMeleeSpells(true);
        player->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        //if player class = hunter || warlock remove pet if alive
        if ((player->getClass() == CLASS_HUNTER) || (player->getClass() == CLASS_WARLOCK))
        {
            if (Pet* pet = player->GetPet())
            {
                pet->SavePetToDB(PET_SAVE_AS_CURRENT);
                // not let dismiss dead pet
                if (pet->IsAlive())
                    player->RemovePet(pet, PET_SAVE_NOT_IN_SLOT);
            }
        }

        //m_session->GetPlayer()->CastSpell(player,spellID,false);
        if (SpellEntry const* spellInfo = sSpellStore.LookupEntry(9454))
        {
            for (uint32 i = 0; i < 3; i++)
            {
                uint8 eff = spellInfo->Effect[i];
                if (eff >= TOTAL_SPELL_EFFECTS)
                    continue;
                if (eff == SPELL_EFFECT_APPLY_AREA_AURA_PARTY || eff == SPELL_EFFECT_APPLY_AURA ||
                    eff == SPELL_EFFECT_PERSISTENT_AREA_AURA || eff == SPELL_EFFECT_APPLY_AREA_AURA_FRIEND ||
                    eff == SPELL_EFFECT_APPLY_AREA_AURA_ENEMY)
                {
                    Aura* Aur = CreateAura(spellInfo, i, NULL, player);
                    player->AddAura(Aur);
                }
            }
        }

        //save player
        player->SaveToDB();
    }
    return true;
}

bool ChatHandler::HandleUnFreezeCommand(const char* args)
{
    std::string name;
    Player* player;
    char* TargetName = strtok((char*)args, " "); //get entered name
    if (!TargetName) //if no name entered use target
    {
        player = getSelectedPlayer();
        if (player) //prevent crash with creature as target
            name = player->GetName();
    }

    else // if name entered
    {
        name = TargetName;
        normalizePlayerName(name);
        player = sObjectMgr.GetPlayer(name.c_str()); //get player by name
    }

    //effect
    if (player)
    {
        PSendSysMessage(LANG_COMMAND_UNFREEZE, name.c_str());

        //Reset player faction + allow combat + allow duels
        player->setFactionForRace(player->getRace());
        player->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        //allow movement and spells
        player->RemoveAurasDueToSpell(9454);

        //save player
        player->SaveToDB();
    }

    if (!player)
    {
        if (TargetName)
        {
            //check for offline players
            QueryResult_AutoPtr result = CharacterDatabase.PQuery("SELECT characters.guid FROM characters WHERE characters.name = '%s'", name.c_str());
            if (!result)
            {
                SendSysMessage(LANG_COMMAND_FREEZE_WRONG);
                return true;
            }
            //if player found: delete his freeze aura
            Field* fields = result->Fetch();
            uint64 pguid = fields[0].GetUInt64();

            CharacterDatabase.PQuery("DELETE FROM character_aura WHERE character_aura.spell = 9454 AND character_aura.guid = '" UI64FMTD "'", pguid);
            PSendSysMessage(LANG_COMMAND_UNFREEZE, name.c_str());
            return true;
        }
        else
        {
            SendSysMessage(LANG_COMMAND_FREEZE_WRONG);
            return true;
        }
    }

    return true;
}

bool ChatHandler::HandleListFreezeCommand(const char* /*args*/)
{
    //Get names from DB
    QueryResult_AutoPtr result = CharacterDatabase.Query("SELECT characters.name FROM characters LEFT JOIN character_aura ON (characters.guid = character_aura.guid) WHERE character_aura.spell = 9454");
    if (!result)
    {
        SendSysMessage(LANG_COMMAND_NO_FROZEN_PLAYERS);
        return true;
    }
    //Header of the names
    PSendSysMessage(LANG_COMMAND_LIST_FREEZE);

    //Output of the results
    do
    {
        Field* fields = result->Fetch();
        std::string fplayers = fields[0].GetCppString();
        PSendSysMessage(LANG_COMMAND_FROZEN_PLAYERS, fplayers.c_str());
    }
    while (result->NextRow());

    return true;
}

bool ChatHandler::HandleGroupLeaderCommand(const char* args)
{
    Player* plr  = NULL;
    Group* group = NULL;
    uint64 guid  = 0;
    char* cname  = strtok((char*)args, " ");

    if (GetPlayerGroupAndGUIDByName(cname, plr, group, guid))
        if (group && group->GetLeaderGUID() != guid)
            group->ChangeLeader(guid);

    return true;
}

bool ChatHandler::HandleGroupDisbandCommand(const char* args)
{
    Player* plr  = NULL;
    Group* group = NULL;
    uint64 guid  = 0;
    char* cname  = strtok((char*)args, " ");

    if (GetPlayerGroupAndGUIDByName(cname, plr, group, guid))
        if (group)
            group->Disband();

    return true;
}

bool ChatHandler::HandleGroupRemoveCommand(const char* args)
{
    Player* plr  = NULL;
    Group* group = NULL;
    uint64 guid  = 0;
    char* cname  = strtok((char*)args, " ");

    if (GetPlayerGroupAndGUIDByName(cname, plr, group, guid, true))
        if (group)
            group->RemoveMember(guid);

    return true;
}

bool ChatHandler::HandleGroupJoinCommand(const char* args)
{
    if (!*args)
        return false;

    Player* playerSource = NULL;
    Player* playerTarget = NULL;
    Group* groupSource = NULL;
    Group* groupTarget = NULL;
    uint64 guidSource = 0;
    uint64 guidTarget = 0;
    char* nameplgrStr = strtok((char*)args, " ");
    char* nameplStr = strtok(NULL, " ");

    if (GetPlayerGroupAndGUIDByName(nameplgrStr, playerSource, groupSource, guidSource, true))
    {
        if (groupSource)
        {
            if (GetPlayerGroupAndGUIDByName(nameplStr, playerTarget, groupTarget, guidTarget, true))
            {
                if (!groupTarget && playerTarget->GetGroup() != groupSource)
                {
                    if (!groupSource->IsFull())
                    {
                        groupSource->AddMember(guidTarget, playerTarget->GetName());
                        groupSource->BroadcastGroupUpdate();
                        PSendSysMessage(LANG_GROUP_PLAYER_JOINED, playerTarget->GetName(), playerSource->GetName());
                        return true;
                    }
                    else
                    {
                        // group is full
                        PSendSysMessage(LANG_GROUP_FULL);
                        return true;
                    }
                }
                else
                {
                    // group is full or target player already in a group
                    PSendSysMessage(LANG_GROUP_ALREADY_IN_GROUP, playerTarget->GetName());
                    return true;
                }
            }
        }
        else
        {
            // specified source player is not in a group
            PSendSysMessage(LANG_GROUP_NOT_IN_GROUP, playerSource->GetName());
            return true;
        }
    }

    return true;
}

bool ChatHandler::HandlePossessCommand(const char* /*args*/)
{
    Unit* pUnit = getSelectedUnit();
    if (!pUnit)
        return false;

    m_session->GetPlayer()->CastSpell(pUnit, 530, true);
    return true;
}

bool ChatHandler::HandleUnPossessCommand(const char* /*args*/)
{
    Unit* pUnit = getSelectedUnit();
    if (!pUnit)
        pUnit = m_session->GetPlayer();

    pUnit->RemoveSpellsCausingAura(SPELL_AURA_MOD_CHARM);
    pUnit->RemoveSpellsCausingAura(SPELL_AURA_MOD_POSSESS_PET);
    pUnit->RemoveSpellsCausingAura(SPELL_AURA_MOD_POSSESS);

    return true;
}

bool ChatHandler::HandleBindSightCommand(const char* /*args*/)
{
    Unit* pUnit = getSelectedUnit();
    if (!pUnit)
        return false;

    m_session->GetPlayer()->CastSpell(pUnit, 6277, true);
    return true;
}

bool ChatHandler::HandleUnbindSightCommand(const char* /*args*/)
{
    if (m_session->GetPlayer()->isPossessing())
        return false;

    m_session->GetPlayer()->StopCastingBindSight();
    return true;
}

