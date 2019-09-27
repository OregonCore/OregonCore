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

#ifndef OREGONCORE_CHAT_H
#define OREGONCORE_CHAT_H

#include "SharedDefines.h"


class ChatHandler;
class WorldSession;
class Creature;
class Player;
class Unit;
struct GameTele;

class ChatCommand
{
    public:
        const char*        Name;
        uint32             SecurityLevel;                   // function pointer required correct align (use uint32)
        bool               AllowConsole;
        bool (ChatHandler::*Handler)(const char* args);
        std::string        Help;
        ChatCommand*       ChildCommands;
};

class ChatHandler
{
    public:
        explicit ChatHandler(WorldSession* session) : m_session(session), sentErrorMessage(false) { }
        explicit ChatHandler(Player* player) : m_session(player->GetSession()) { }
        ~ChatHandler() {}

        static void FillMessageData(WorldPacket* data, WorldSession* session, uint8 type, uint32 language, const char* channelName, uint64 target_guid, const char* message, Unit* speaker);

        void FillMessageData(WorldPacket* data, uint8 type, uint32 language, uint64 target_guid, const char* message)
        {
            FillMessageData(data, m_session, type, language, NULL, target_guid, message, NULL);
        }

        void FillSystemMessageData(WorldPacket* data, const char* message)
        {
            FillMessageData(data, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, 0, message);
        }

        static char* LineFromMessage(char*& pos)
        {
            char* start = strtok(pos, "\n");
            pos = NULL;
            return start;
        }

        // function with different implementation for chat/console
        virtual const char* GetOregonString(int32 entry) const;
        virtual void SendSysMessage(const char* str);

        char* extractQuotedArg(char* args);

        void SendSysMessage(int32     entry);
        void PSendSysMessage(const char* format, ...) ATTR_PRINTF(2, 3);
        void PSendSysMessage(int32     entry, ...);
        std::string PGetParseString(int32 entry, ...);

        int ParseCommands(const char* text);

        static ChatCommand* getCommandTable();

        bool isValidChatMessage(const char* msg);
        bool HasSentErrorMessage()
        {
            return sentErrorMessage;
        }
        virtual char const* GetName() const;
    protected:
        explicit ChatHandler() : m_session(NULL), sentErrorMessage(false) { }      // for CLI subclass

        bool hasStringAbbr(const char* name, const char* part);

        // function with different implementation for chat/console
        virtual bool isAvailable(ChatCommand const& cmd) const;
        virtual bool needReportToTarget(Player* chr) const;

        void SendGlobalSysMessage(const char* str);
        void SendGlobalGMSysMessage(const char* str);

        bool ExecuteCommandInTable(ChatCommand* table, const char* text, const std::string& fullcommand);
        bool ShowHelpForCommand(ChatCommand* table, const char* cmd);
        bool ShowHelpForSubCommands(ChatCommand* table, char const* cmd, char const* subcmd);

        bool HandleAccountCommand(const char* args);
        bool HandleAccountCreateCommand(const char* args);
        bool HandleAccountDeleteCommand(const char* args);
        bool HandleAccountLockCommand(const char* args);
        bool HandleAccountOnlineListCommand(const char* args);
        bool HandleAccountPasswordCommand(const char* args);
        bool HandleAccountSetAddonCommand(const char* args);
        bool HandleAccountSetGmLevelCommand(const char* args);
        bool HandleAccountSetPasswordCommand(const char* args);

        bool HandleHelpCommand(const char* args);
        bool HandleAHBotOptionsCommand(const char* args);
        bool HandleCommandsCommand(const char* args);
        bool HandleStartCommand(const char* args);
        bool HandleDismountCommand(const char* args);
        bool HandleSaveCommand(const char* args);
        bool HandleGMListIngameCommand(const char* args);
        bool HandleGMListFullCommand(const char* args);

        bool HandleSummonCommand(const char* args);
        bool HandleAppearCommand(const char* args);
        bool HandleGroupSummonCommand(const char* args);
        bool HandleRecallCommand(const char* args);
        bool HandleNameAnnounceCommand(const char* args);
        bool HandleGMNameAnnounceCommand(const char* args);
        bool HandleAnnounceCommand(const char* args);
        bool HandleGMAnnounceCommand(const char* args);
        bool HandleNotifyCommand(const char* args);
        bool HandleGMNotifyCommand(const char* args);
        bool HandleGMmodeCommand(const char* args);
        bool HandleGMChatCommand(const char* args);
        bool HandleVisibleCommand(const char* args);
        bool HandleGPSCommand(const char* args);
        bool HandleTaxiCheatCommand(const char* args);
        bool HandleWhispersCommand(const char* args);
        bool HandleNameTeleCommand(const char* args);
        bool HandleGroupTeleCommand(const char* args);
        bool HandleDrunkCommand(const char* args);
        bool HandleSendItemsCommand(const char* args);
        bool HandleSendMailCommand(const char* args);
        bool HandleSendMoneyCommand(const char* args);

        bool HandleDebugPlayCinematicCommand(const char* args);
        bool HandleDebugPlaySoundCommand(const char* args);
        bool HandleEventActiveListCommand(const char* args);
        bool HandleEventStartCommand(const char* args);
        bool HandleEventStopCommand(const char* args);
        bool HandleEventInfoCommand(const char* args);

        bool HandleLearnCommand(const char* args);
        bool HandleLearnAllCommand(const char* args);
        bool HandleLearnAllGMCommand(const char* args);
        bool HandleLearnAllCraftsCommand(const char* args);
        bool HandleLearnAllRecipesCommand(const char* args);
        bool HandleLearnAllDefaultCommand(const char* args);
        bool HandleLearnAllLangCommand(const char* args);
        bool HandleLearnAllMyClassCommand(const char* args);
        bool HandleLearnAllMySpellsCommand(const char* args);
        bool HandleLearnAllMyTalentsCommand(const char* args);

        bool HandleLookupAreaCommand(const char* args);
        bool HandleLookupCreatureCommand(const char* args);
        bool HandleLookupEventCommand(const char* args);
        bool HandleLookupFactionCommand(const char* args);
        bool HandleLookupItemCommand(const char* args);
        bool HandleLookupItemSetCommand(const char* args);
        bool HandleLookupObjectCommand(const char* args);
        bool HandleLookupPlayerIpCommand(const char* args);
        bool HandleLookupPlayerAccountCommand(const char* args);
        bool HandleLookupPlayerEmailCommand(const char* args);
        bool HandleLookupQuestCommand(const char* args);
        bool HandleLookupSkillCommand(const char* args);
        bool HandleLookupSpellCommand(const char* args);
        bool HandleLookupTeleCommand(const char* args);

        bool HandleCharacterTitlesCommand(const char* args);
        bool HandleTitleCommand(const char* args);
        bool HandleLookupTitleCommand(const char* args);
        bool HandleTitlesAddCommand(const char* args);
        bool HandleTitlesCurrentCommand(const char* args);
        bool HandleTitlesRemoveCommand(const char* args);
        bool HandleTitlesSetMaskCommand(const char* args);

        bool HandleModifyKnownTitlesCommand(const char* args);
        bool HandleModifyHPCommand(const char* args);
        bool HandleModifyManaCommand(const char* args);
        bool HandleModifyRageCommand(const char* args);
        bool HandleModifyEnergyCommand(const char* args);
        bool HandleModifyMoneyCommand(const char* args);
        bool HandleModifyASpeedCommand(const char* args);
        bool HandleModifySpeedCommand(const char* args);
        bool HandleModifyBWalkCommand(const char* args);
        bool HandleModifyFlyCommand(const char* args);
        bool HandleModifySwimCommand(const char* args);
        bool HandleModifyScaleCommand(const char* args);
        bool HandleModifyMountCommand(const char* args);
        bool HandleModifyBitCommand(const char* args);
        bool HandleModifyFactionCommand(const char* args);
        bool HandleModifySpellCommand(const char* args);
        bool HandleModifyTalentCommand (const char* args);
        bool HandleModifyHonorCommand (const char* args);
        bool HandleModifyRepCommand(const char* args);
        bool HandleModifyArenaCommand(const char* args);
        bool HandleModifyGenderCommand(const char* args);

        //-----------------------Npc Commands-----------------------
        bool HandleNpcAddCommand(const char* args);
        bool HandleNpcChangeEntryCommand(const char* args);
        bool HandleNpcDeleteCommand(const char* args);
        bool HandleNpcFactionIdCommand(const char* args);
        bool HandleNpcFlagCommand(const char* args);
        bool HandleNpcFollowCommand(const char* args);
        bool HandleNpcInfoCommand(const char* args);
        bool HandleNpcMoveCommand(const char* args);
        bool HandleNpcPlayEmoteCommand(const char* args);
        bool HandleNpcSayCommand(const char* args);
        bool HandleNpcSetModelCommand(const char* args);
        bool HandleNpcSetMoveTypeCommand(const char* args);
        bool HandleNpcSpawnDistCommand(const char* args);
        bool HandleNpcSpawnTimeCommand(const char* args);
        bool HandleNpcTameCommand(const char* args);
        bool HandleNpcTextEmoteCommand(const char* args);
        bool HandleNpcUnFollowCommand(const char* args);
        bool HandleNpcWhisperCommand(const char* args);
        bool HandleNpcYellCommand(const char* args);
        bool HandleNpcSetLinkCommand(const char* args);

        //----------------------------------------------------------

        bool HandleReloadCommand(const char* args);
        bool HandleReloadAllCommand(const char* args);
        bool HandleReloadAllAreaCommand(const char* args);
        bool HandleReloadAllItemCommand(const char* args);
        bool HandleReloadAllLootCommand(const char* args);
        bool HandleReloadAllNpcCommand(const char* args);
        bool HandleReloadAllQuestCommand(const char* args);
        bool HandleReloadAllScriptsCommand(const char* args);
        bool HandleReloadAllSpellCommand(const char* args);
        bool HandleReloadAllLocalesCommand(const char* args);

        bool HandleReloadConfigCommand(const char* args);

        bool HandleReloadWpScriptsCommand(const char* args);
        bool HandleReloadAreaTriggerTavernCommand(const char* args);
        bool HandleReloadAutobroadcastCommand(const char* args);
        bool HandleReloadAreaTriggerTeleportCommand(const char* args);
        bool HandleReloadAccessRequirementCommand(const char* args);
        bool HandleReloadEventScriptsCommand(const char* args);
        bool HandleReloadEventAITextsCommand(const char* args);
        bool HandleReloadEventAISummonsCommand(const char* args);
        bool HandleReloadEventAIScriptsCommand(const char* args);
        bool HandleReloadCommandCommand(const char* args);
        bool HandleReloadCreatureQuestStarterCommand(const char* args);
        bool HandleReloadCreatureQuestEnderCommand(const char* args);
        bool HandleReloadCreatureLinkedRespawnCommand(const char* args);
        bool HandleReloadDbScriptStringCommand(const char* args);
        bool HandleReloadGameGraveyardZoneCommand(const char* args);
        bool HandleReloadGameObjectScriptsCommand(const char* args);
        bool HandleReloadGameTeleCommand(const char* args);
        bool HandleReloadGossipMenuCommand(const char* args);
        bool HandleReloadGossipMenuOptionCommand(const char* args);
        bool HandleReloadGOQuestStarterCommand(const char* args);
        bool HandleReloadGOQuestEnderCommand(const char* args);
        bool HandleReloadLootTemplatesCreatureCommand(const char* args);
        bool HandleReloadLootTemplatesDisenchantCommand(const char* args);
        bool HandleReloadLootTemplatesFishingCommand(const char* args);
        bool HandleReloadLootTemplatesGameobjectCommand(const char* args);
        bool HandleReloadLootTemplatesItemCommand(const char* args);
        bool HandleReloadLootTemplatesMailCommand(const char* args);
        bool HandleReloadLootTemplatesPickpocketingCommand(const char* args);
        bool HandleReloadLootTemplatesProspectingCommand(const char* args);
        bool HandleReloadLootTemplatesReferenceCommand(const char* args);
        bool HandleReloadLootTemplatesSkinningCommand(const char* args);
        bool HandleReloadOregonStringCommand(const char* args);
        bool HandleReloadNpcGossipCommand(const char* args);
        bool HandleReloadNpcTrainerCommand(const char* args);
        bool HandleReloadNpcVendorCommand(const char* args);
        bool HandleReloadQuestAreaTriggersCommand(const char* args);
        bool HandleReloadQuestEndScriptsCommand(const char* args);
        bool HandleReloadQuestStartScriptsCommand(const char* args);
        bool HandleReloadQuestTemplateCommand(const char* args);
        bool HandleReloadReservedNameCommand(const char*);
        bool HandleReloadReputationSpilloverTemplateCommand(const char* args);
        bool HandleReloadSkillDiscoveryTemplateCommand(const char* args);
        bool HandleReloadSkillExtraItemTemplateCommand(const char* args);
        bool HandleReloadSkillFishingBaseLevelCommand(const char* args);
        bool HandleReloadSpellAffectCommand(const char* args);
        bool HandleReloadSpellRequiredCommand(const char* args);
        bool HandleReloadSpellGroupsCommand(const char* args);
        bool HandleReloadSpellGroupStackRulesCommand(const char* args);
        bool HandleReloadSpellLearnSpellCommand(const char* args);
        bool HandleReloadSpellLinkedSpellCommand(const char* args);
        bool HandleReloadSpellProcEventCommand(const char* args);
        bool HandleReloadSpellScriptsCommand(const char* args);
        bool HandleReloadSpellTargetPositionCommand(const char* args);
        bool HandleReloadSpellThreatsCommand(const char* args);
        bool HandleReloadSpellPetAurasCommand(const char* args);
        bool HandleReloadDisablesCommand(const char* args);
        bool HandleReloadPageTextsCommand(const char* args);
        bool HandleReloadItemEnchantementsCommand(const char* args);
        bool HandleReloadLocalesCreatureCommand(const char* args);
        bool HandleReloadLocalesGameobjectCommand(const char* args);
        bool HandleReloadLocalesItemCommand(const char* args);
        bool HandleReloadLocalesNpcTextCommand(const char* args);
        bool HandleReloadLocalesPageTextCommand(const char* args);
        bool HandleReloadLocalesQuestCommand(const char* args);
        bool HandleReloadAuctionsCommand(const char* args);
        bool HandleReloadConditions(const char* args);

        bool HandleInstanceListBindsCommand(const char* args);
        bool HandleInstanceUnbindCommand(const char* args);
        bool HandleInstanceStatsCommand(const char* args);
        bool HandleInstanceSaveDataCommand(const char* args);

        bool HandleServerCorpsesCommand(const char* args);
        bool HandleServerExitCommand(const char* args);
        bool HandleServerIdleRestartCommand(const char* args);
        bool HandleServerIdleShutDownCommand(const char* args);
        bool HandleServerInfoCommand(const char* args);
        bool HandleServerMotdCommand(const char* args);
        bool HandleServerPLimitCommand(const char* args);
        bool HandleServerRestartCommand(const char* args);
        bool HandleServerSetLogMaskCommand(const char* args);
        bool HandleServerSetMotdCommand(const char* args);
        bool HandleServerSetDiffTimeCommand(const char* args);
        bool HandleServerShutDownCommand(const char* args);
        bool HandleServerShutDownCancelCommand(const char* args);

        bool HandleAddHonorCommand(const char* args);
        bool HandleHonorAddKillCommand(const char* args);
        bool HandleUpdateHonorFieldsCommand(const char* args);

        bool HandleSendQuestPartyMsgCommand(const char* args);
        bool HandleSendQuestInvalidMsgCommand(const char* args);

        bool HandleDebugInArcCommand(const char* args);
        bool HandleDebugSpellFailCommand(const char* args);

        bool HandleGUIDCommand(const char* args);
        bool HandleNameCommand(const char* args);
        bool HandleSubNameCommand(const char* args);
        bool HandleItemMoveCommand(const char* args);
        bool HandleDeMorphCommand(const char* args);
        bool HandleAddVendorItemCommand(const char* args);
        bool HandleDelVendorItemCommand(const char* args);
        bool HandleChangeLevelCommand(const char* args);
        bool HandleSetPoiCommand(const char* args);
        bool HandleEquipErrorCommand(const char* args);
        bool HandleGoCreatureCommand(const char* args);
        bool HandleGoObjectCommand(const char* args);
        bool HandleGoTicketCommand(const char* args);
        bool HandleGoTriggerCommand(const char* args);
        bool HandleGoGraveyardCommand(const char* args);
        bool HandleTargetObjectCommand(const char* args);
        bool HandleDelObjectCommand(const char* args);
        bool HandleMoveObjectCommand(const char* args);
        bool HandleTurnObjectCommand(const char* args);
        bool HandleObjectStateCommand(const char* args);
        bool HandlePInfoCommand(const char* args);
        bool HandleMuteCommand(const char* args);
        bool HandleUnmuteCommand(const char* args);
        bool HandleMovegensCommand(const char* args);
        bool HandleFreezeCommand(const char* args);
        bool HandleUnFreezeCommand(const char* args);
        bool HandleListFreezeCommand(const char* args);

        bool HandleBanAccountCommand(const char* args);
        bool HandleBanCharacterCommand(const char* args);
        bool HandleBanIPCommand(const char* args);
        bool HandleUnBanAccountCommand(const char* args);
        bool HandleUnBanCharacterCommand(const char* args);
        bool HandleUnBanIPCommand(const char* args);
        bool HandleBanInfoAccountCommand(const char* args);
        bool HandleBanInfoCharacterCommand(const char* args);
        bool HandleBanInfoIPCommand(const char* args);
        bool HandleBanListAccountCommand(const char* args);
        bool HandleBanListCharacterCommand(const char* args);
        bool HandleBanListIPCommand(const char* args);
        bool HandleGoXYCommand(const char* args);
        bool HandleGoXYZCommand(const char* args);
        bool HandleGoZoneXYCommand(const char* args);
        bool HandleGoGridCommand(const char* args);
        bool HandleAddWeaponCommand(const char* args);
        bool HandleAllowMovementCommand(const char* args);
        bool HandleGoCommand(const char* args);

        bool HandleCooldownCommand(const char* args);
        bool HandleUnLearnCommand(const char* args);
        bool HandleGetDistanceCommand(const char* args);
        bool HandleGameObjectCommand(const char* args);
        bool HandleAnimCommand(const char* args);
        bool HandleStandStateCommand(const char* args);
        bool HandleDieCommand(const char* args);
        bool HandleDamageCommand(const char* args);
        bool HandleReviveCommand(const char* args);
        bool HandleMorphCommand(const char* args);
        bool HandleAuraCommand(const char* args);
        bool HandleUnAuraCommand(const char* args);
        bool HandleLinkGraveCommand(const char* args);
        bool HandleNearGraveCommand(const char* args);
        bool HandleActivateObjectCommand(const char* args);
        bool HandleSpawnTransportCommand(const char* args);
        bool HandleHoverCommand(const char* args);
        bool HandleLevelUpCommand(const char* args);
        bool HandleShowAreaCommand(const char* args);
        bool HandleHideAreaCommand(const char* args);
        bool HandleAddItemCommand(const char* args);
        bool HandleAddItemSetCommand(const char* args);
        bool HandlePetTpCommand(const char* args);
        bool HandlePetUnlearnCommand(const char* args);
        bool HandlePetLearnCommand(const char* args);
        bool HandleCreatePetCommand(const char* args);

        // Cheat Related
        bool HandlePowerCheatCommand(const char *args);
        bool HandleCoolDownCheatCommand(const char *args);
        bool HandleCasttimeCheatCommand(const char *args);
        bool HandleGodModeCheatCommand(const char *args);
        bool HandleExploreCheatCommand(const char* args);
        bool HandleWaterwalkCheatCommand(const char* args);

        // Group Related
        bool HandleGroupLeaderCommand(const char* args);
        bool HandleGroupDisbandCommand(const char* args);
        bool HandleGroupRemoveCommand(const char* args);
        bool HandleGroupJoinCommand(const char* args);

        // Guild Related
        bool HandleGuildCreateCommand(const char* args);
        bool HandleGuildInviteCommand(const char* args);
        bool HandleGuildUninviteCommand(const char* args);
        bool HandleGuildRankCommand(const char* args);
        bool HandleGuildDeleteCommand(const char* args);

        bool HandleUpdate(const char* args);
        bool HandleBankCommand(const char* args);
        bool HandleChangeWeather(const char* args);
        bool HandleKickPlayerCommand(const char* args);
        bool HandleTeleCommand(const char* args);
        bool HandleAddTeleCommand(const char* args);
        bool HandleDelTeleCommand(const char* args);
        bool HandleListAurasCommand(const char* args);

        // Reset commands
        bool HandleResetHonorCommand(const char* args);
        bool HandleResetLevelCommand(const char* args);
        bool HandleResetSpellsCommand(const char* args);
        bool HandleResetStatsCommand(const char* args);
        bool HandleResetTalentsCommand(const char* args);
        bool HandleResetAllCommand(const char* args);


        // GM ticket command handlers
        bool HandleGMTicketListCommand(const char* args);
        bool HandleGMTicketListOnlineCommand(const char* args);
        bool HandleGMTicketListClosedCommand(const char* args);
        bool HandleGMTicketGetByIdCommand(const char* args);
        bool HandleGMTicketGetByNameCommand(const char* args);
        bool HandleGMTicketCloseByIdCommand(const char* args);
        bool HandleGMTicketAssignToCommand(const char* args);
        bool HandleGMTicketUnAssignCommand(const char* args);
        bool HandleGMTicketCommentCommand(const char* args);
        bool HandleGMTicketDeleteByIdCommand(const char* args);
        bool HandleGMTicketReloadCommand(const char*);

        bool HandleMaxSkillCommand(const char* args);
        bool HandleSetSkillCommand(const char* args);
        bool HandleListCreatureCommand(const char* args);
        bool HandleListItemCommand(const char* args);
        bool HandleListObjectCommand(const char* args);
        bool HandleNearObjectCommand(const char* args);
        bool HandleRespawnCommand(const char* args);

        //New Wp Commands
        bool HandleWpAddCommand(const char* args);
        bool HandleWpLoadPathCommand(const char* args);
        bool HandleWpUnLoadPathCommand(const char* args);
        bool HandleWpModifyCommand(const char* args);
        bool HandleWpEventCommand(const char* args);
        bool HandleWpShowCommand(const char* args);
        bool HandleReloadAllPaths(const char* args);

        bool HandleFlyModeCommand(const char* args);
        bool HandleSendOpcodeCommand(const char* args);
        bool HandleSellErrorCommand(const char* args);
        bool HandleBuyErrorCommand(const char* args);
        bool HandleUpdateWorldStateCommand(const char* args);
        bool HandleSendChannelNotifyCommand(const char* args);
        bool HandleSendChatMsgCommand(const char* args);
        bool HandleLoadPDumpCommand(const char* args);
        bool HandleWritePDumpCommand(const char* args);
        bool HandleCastCommand(const char* args);
        bool HandleCastBackCommand(const char* args);
        bool HandleCastDistCommand(const char* args);
        bool HandleCastSelfCommand(const char* args);
        bool HandleCastTargetCommand(const char* args);
        bool HandleCharacterDeletedDeleteCommand(const char* args);
        bool HandleCharacterDeletedListCommand(const char* args);
        bool HandleCharacterDeletedRestoreCommand(const char* args);
        bool HandleCharacterDeletedOldCommand(const char* args);
        bool HandleCharacterEraseCommand(const char* args);
        bool HandleCharacterRenameCommand(const char* args);
        bool HandleComeToMeCommand(const char* args);
        bool HandleCombatStopCommand(const char* args);
        bool HandleCharDeleteCommand(const char* args);
        bool HandleSendMessageCommand(const char* args);
        bool HandleFlushArenaPointsCommand(const char* args);
        bool HandlePlayAllCommand(const char* args);
        bool HandleRepairitemsCommand(const char* args);

        bool HandleTempGameObjectCommand(const char* args);
        bool HandleTempAddSpwCommand(const char* args);

        // Refer-A-Friend
        bool HandleRAFInfoCommand(const char* args);
        bool HandleRAFLinkCommand(const char* args);
        bool HandleRAFUnlinkCommand(const char* args);
        bool HandleRAFSummonCommand(const char* args);
        bool HandleRAFGrantLevelCommand(const char* args);
        bool HandleRAFReloadCommand(const char* args);

        //! Development Commands
        bool HandleSetValue(const char* args);
        bool HandleGetValue(const char* args);
        bool HandleSet32Bit(const char* args);
        bool HandleMod32Value(const char* args);
        bool HandleAddQuest(const char* args);
        bool HandleRemoveQuest(const char* args);
        bool HandleCompleteQuest(const char* args);
        bool HandleSaveAllCommand(const char* args);
        bool HandleGetItemState(const char* args);
        bool HandleGetLootRecipient(const char* args);
        bool HandleDebugArenaCommand(const char* args);
        bool HandleDebugBattlegroundCommand(const char* args);
        bool HandleDebugThreatList(const char* args);
        bool HandleDebugHostilRefList(const char* args);
        bool HandlePossessCommand(const char* args);
        bool HandleUnPossessCommand(const char* args);
        bool HandleBindSightCommand(const char* args);
        bool HandleUnbindSightCommand(const char* args);
        bool HandleSetInstanceDataCommand(const char* args);
        bool HandleGetInstanceDataCommand(const char* args);
        bool HandleDebugRAFError(const char* args);
        bool HandleSpellCrashTestCommand(const char* args);
        bool HandlePartyResultCommand(const char* args);
        bool HandleDebugAnimationCommand(const char* args);

        Player*   getSelectedPlayer();
        Player*   getSelectedPlayerOrSelf();
        Creature* getSelectedCreature();
        Unit*     getSelectedUnit();

        char*     extractKeyFromLink(char* text, char const* linkType, char** something1 = NULL);
        char*     extractKeyFromLink(char* text, char const* const* linkTypes, int* found_idx, char** something1 = NULL);
        uint32    extractSpellIdFromLink(char* text);
        GameTele const* extractGameTeleFromLink(char* text);
        bool GetPlayerGroupAndGUIDByName(const char* cname, Player*& plr, Group*& group, uint64& guid, bool offline = false);

        GameObject* GetObjectGlobalyWithGuidOrNearWithDbGuid(uint32 lowguid, uint32 entry);

        // Utility methods for commands
        bool LookupPlayerSearchCommand(QueryResult_AutoPtr result, int32 limit);
        bool HandleBanListHelper(QueryResult_AutoPtr result);
        bool HandleBanHelper(BanMode mode, char const* args);
        bool HandleBanInfoHelper(uint32 accountid, char const* accountname);
        bool HandleUnBanHelper(BanMode mode, char const* args);

        /**
         * Stores informations about a deleted character
         */
        struct DeletedInfo
        {
            uint32      lowguid;                            ///< the low GUID from the character
            std::string name;                               ///< the character name
            uint32      accountId;                          ///< the account id
            std::string accountName;                        ///< the account name
            time_t      deleteDate;                         ///< the date at which the character has been deleted
        };

        typedef std::list<DeletedInfo> DeletedInfoList;
        bool GetDeletedCharacterInfoList(DeletedInfoList& foundList, std::string searchString = "");
        std::string GenerateDeletedCharacterGUIDsWhereStr(DeletedInfoList::const_iterator& itr, DeletedInfoList::const_iterator const& itr_end);
        void HandleCharacterDeletedListHelper(DeletedInfoList const& foundList);
        void HandleCharacterDeletedRestoreHelper(DeletedInfo const& delInfo);

        void SetSentErrorMessage(bool val)
        {
            sentErrorMessage = val;
        };
    private:
        WorldSession* m_session;                            // != NULL for chat command call and NULL for CLI command

        // common global flag
        static bool load_command_table;
        bool sentErrorMessage;
};

class CliHandler : public ChatHandler
{
    public:
        typedef void Print(void*, char const*);
        explicit CliHandler(void* callbackArg, Print* zprint) : m_callbackArg(callbackArg), m_print(zprint) {}

        // overwrite functions
        const char* GetOregonString(int32 entry) const;
        bool isAvailable(ChatCommand const& cmd) const;
        void SendSysMessage(const char* str);
        char const* GetName() const;
        bool needReportToTarget(Player* chr) const;

    private:
        void* m_callbackArg;
        Print* m_print;
};

char const* fmtstring(char const* format, ...);

#endif

