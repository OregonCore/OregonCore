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
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "QuestDef.h"
#include "GossipDef.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Formulas.h"

GossipMenu::GossipMenu()
{
    _menuId = 0;
}

GossipMenu::~GossipMenu()
{
    ClearMenu();
}

void GossipMenu::AddMenuItem(int32 menuItemId, uint8 icon, std::string const& message, uint32 sender, uint32 action, std::string const& boxMessage, uint32 boxMoney, bool coded /*= false*/)
{
    ASSERT(_menuItems.size() <= GOSSIP_MAX_MENU_ITEMS);

    // Find a free new id - script case
    if (menuItemId == -1)
    {
        menuItemId = 0;
        if (!_menuItems.empty())
        {
            for (GossipMenuItemContainer::const_iterator itr = _menuItems.begin(); itr != _menuItems.end(); ++itr)
            {
                if (int32(itr->first) > menuItemId)
                {
                    menuItemId = menuItemId;
                    break;
                }

                menuItemId = itr->first + 1;
            }
        }
    }

    GossipMenuItem& menuItem = _menuItems[menuItemId];

    menuItem.MenuItemIcon    = icon;
    menuItem.Message         = message;
    menuItem.IsCoded         = coded;
    menuItem.Sender          = sender;
    menuItem.OptionType      = action;
    menuItem.BoxMessage      = boxMessage;
    menuItem.BoxMoney        = boxMoney;
}

void GossipMenu::AddGossipMenuItemData(uint32 menuItemId, uint32 gossipActionMenuId, uint32 gossipActionPoi, uint32 gossipActionScript)
{
    GossipMenuItemData& itemData = _menuItemData[menuItemId];

    itemData.GossipActionMenuId = gossipActionMenuId;
    itemData.GossipActionPoi = gossipActionPoi;
    itemData.GossipActionScript = gossipActionScript;
}

uint32 GossipMenu::GetMenuItemSender(uint32 menuItemId) const
{
    GossipMenuItemContainer::const_iterator itr = _menuItems.find(menuItemId);
    if (itr == _menuItems.end())
        return 0;

    return itr->second.Sender;
}

uint32 GossipMenu::GetMenuItemAction(uint32 menuItemId) const
{
    GossipMenuItemContainer::const_iterator itr = _menuItems.find(menuItemId);
    if (itr == _menuItems.end())
        return 0;

    return itr->second.OptionType;
}

bool GossipMenu::IsMenuItemCoded(uint32 menuItemId) const
{
    GossipMenuItemContainer::const_iterator itr = _menuItems.find(menuItemId);
    if (itr == _menuItems.end())
        return false;

    return itr->second.IsCoded;
}

void GossipMenu::ClearMenu()
{
    _menuItems.clear();
    _menuItemData.clear();
}

PlayerMenu::PlayerMenu(WorldSession *session) : _session(session)
{
}

PlayerMenu::~PlayerMenu()
{
    ClearMenus();
}

void PlayerMenu::ClearMenus()
{
    _gossipMenu.ClearMenu();
    _questMenu.ClearMenu();
}

void PlayerMenu::SendGossipMenu(uint32 titleTextId, uint64 objectGUID) const
{
    WorldPacket data(SMSG_GOSSIP_MESSAGE, 100);           // guess size
    data << uint64(objectGUID);
    data << uint32(_gossipMenu.GetMenuId());                // new 2.4.0
    data << uint32(titleTextId);
    data << uint32(_gossipMenu.GetMenuItemCount());            // max count 0x20

    for (GossipMenuItemContainer::const_iterator itr = _gossipMenu.GetMenuItems().begin(); itr != _gossipMenu.GetMenuItems().end(); ++itr)
    {
        GossipMenuItem const& item = itr->second;
        data << uint32(itr->first);
        data << uint8(item.MenuItemIcon);
        data << uint8(0);                      // makes pop up box password
        data << uint32(item.BoxMoney);                  // money required to open menu, 2.0.3
        data << item.Message;                           // text for gossip item, max 0x800
        data << item.BoxMessage;                        // accept text (related to money) pop up box, 2.0.3, max 0x800
    }

    data << uint32(_questMenu.GetMenuItemCount());      // max count 0x20

    // Store this instead of checking the Singleton every loop iteration
    bool questLevelInTitle = sWorld.getConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS);

    for (uint32 iI = 0; iI < _questMenu.GetMenuItemCount(); ++iI)
    {
        QuestMenuItem const& item = _questMenu.GetItem(iI);
        uint32 questID = item.QuestId;
        Quest const* quest = sObjectMgr.GetQuestTemplate(questID);

        data << uint32(questID);
        data << uint32(item.QuestIcon);
        data << int32(quest->GetQuestLevel());
        std::string title = quest->GetTitle();

        int locale = _session->GetSessionDbLocaleIndex();
        if (locale >= 0)
            if (QuestLocale const* localeData = sObjectMgr.GetQuestLocale(questID))
                if (localeData->Title.size() > uint32(locale) && !localeData->Title[locale].empty())
                    title = localeData->Title[locale];

        if (questLevelInTitle)
            AddQuestLevelToTitle(title, quest->GetQuestLevel());

        data << title;
    }

    _session->SendPacket(&data);
}

void PlayerMenu::SendCloseGossip() const
{
    WorldPacket data(SMSG_GOSSIP_COMPLETE, 0);
    _session->SendPacket(&data);
}

// Outdated
void PlayerMenu::SendPointOfInterest(float X, float Y, uint32 Icon, uint32 Flags, uint32 Data, char* locName) const
{
    WorldPacket data(SMSG_GOSSIP_POI, (4 + 4 + 4 + 4 + 4 + 10));    // guess size
    data << Flags;
    data << X << Y;
    data << uint32(Icon);
    data << uint32(Data);
    data << locName;

    _session->SendPacket(&data);
    //sLog.outDebug("WORLD: Sent SMSG_GOSSIP_POI");
}

void PlayerMenu::SendPointOfInterest(uint32 poiId) const
{
    PointOfInterest const* poi = sObjectMgr.GetPointOfInterest(poiId);
    if (!poi)
    {
        sLog.outErrorDb("Request to send non-existing POI (Id: %u), ignored.", poiId);
        return;
    }

    std::string iconText = poi->icon_name;
    int32 locale = _session->GetSessionDbLocaleIndex();
    if (locale >= 0)
        if (PointOfInterestLocale const *localeData = sObjectMgr.GetPointOfInterestLocale(poiId))
            sObjectMgr.GetLocaleString(localeData->IconName, locale, iconText);

    WorldPacket data(SMSG_GOSSIP_POI, 4 + 4 + 4 + 4 + 4 + 10); // guess size
    data << uint32(poi->flags);
    data << float(poi->x);
    data << float(poi->y);
    data << uint32(poi->icon);
    data << uint32(poi->data);
    data << iconText;

    _session->SendPacket(&data);
}

/*********************************************************/
/***                   QUEST SYSTEM                   ***/
/*********************************************************/

QuestMenu::QuestMenu()
{
    _questMenuItems.reserve(16);                                   // can be set for max from most often sizes to speedup push_back and less memory use
}

QuestMenu::~QuestMenu()
{
    ClearMenu();
}

void QuestMenu::AddMenuItem(uint32 QuestId, uint8 Icon)
{
    if (!sObjectMgr.GetQuestTemplate(QuestId))
        return;

    ASSERT(_questMenuItems.size() <= GOSSIP_MAX_MENU_ITEMS);

    QuestMenuItem questMenuItem;

    questMenuItem.QuestId        = QuestId;
    questMenuItem.QuestIcon      = Icon;

    _questMenuItems.push_back(questMenuItem);
}

bool QuestMenu::HasItem(uint32 questId) const
{
    for (QuestMenuItemList::const_iterator i = _questMenuItems.begin(); i != _questMenuItems.end(); ++i)
        if (i->QuestId == questId)
            return true;

    return false;
}

void QuestMenu::ClearMenu()
{
    _questMenuItems.clear();
}

void PlayerMenu::SendQuestGiverQuestList(QEmote eEmote, const std::string& Title, uint64 npcGUID)
{
    WorldPacket data(SMSG_QUESTGIVER_QUEST_LIST, 100);    // guess size
    data << uint64(npcGUID);
    data << Title;
    data << uint32(eEmote._Delay);                         // player emote
    data << uint32(eEmote._Emote);                         // NPC emote
    data << uint8 (_questMenu.GetMenuItemCount());

    // Store this instead of checking the Singleton every loop iteration
    bool questLevelInTitle = sWorld.getConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS);

    for (uint32 iI = 0; iI < _questMenu.GetMenuItemCount(); iI++)
    {
        QuestMenuItem const& qmi = _questMenu.GetItem(iI);

        uint32 questID = qmi.QuestId;
        Quest const* pQuest = sObjectMgr.GetQuestTemplate(questID);

        std::string title = pQuest ? pQuest->GetTitle() : "";

        int loc_idx = _session->GetSessionDbLocaleIndex();
        if (loc_idx >= 0)
        {
            if (QuestLocale const* ql = sObjectMgr.GetQuestLocale(questID))
            {
                if (ql->Title.size() > uint32(loc_idx) && !ql->Title[loc_idx].empty())
                    title = ql->Title[loc_idx];
            }
        }

        if (questLevelInTitle)
            AddQuestLevelToTitle(title, pQuest->GetQuestLevel());

        data << uint32(questID);
        data << uint32(qmi.QuestIcon);
        data << int32(pQuest->GetQuestLevel());
        data << title;
    }
    _session->SendPacket(&data);
    DEBUG_LOG("WORLD: Sent SMSG_QUESTGIVER_QUEST_LIST NPC Guid=%u", GUID_LOPART(npcGUID));
}

void PlayerMenu::SendQuestGiverStatus(uint8 questStatus, uint64 npcGUID) const
{
    WorldPacket data(SMSG_QUESTGIVER_STATUS, 9);
    data << uint64(npcGUID);
    data << uint8(questStatus);

    _session->SendPacket(&data);
    DEBUG_LOG("WORLD: Sent SMSG_QUESTGIVER_STATUS NPC Guid=%u, status=%u", GUID_LOPART(npcGUID), questStatus);
}

void PlayerMenu::SendQuestGiverQuestDetails(Quest const *quest, uint64 npcGUID, bool activateAccept) const
{
    std::string questTitle      = quest->GetTitle();
    std::string questDetails    = quest->GetDetails();
    std::string questObjectives = quest->GetObjectives();
    std::string questEndText    = quest->GetEndText();

    int32 locale = _session->GetSessionDbLocaleIndex();
    if (locale >= 0)
    {
        if (QuestLocale const* localeData = sObjectMgr.GetQuestLocale(quest->GetQuestId()))
        {
            if (localeData->Title.size() > uint32(locale) && !localeData->Title[locale].empty())
                questTitle = localeData->Title[locale];
            if (localeData->Details.size() > uint32(locale) && !localeData->Details[locale].empty())
                questDetails = localeData->Details[locale];
            if (localeData->Objectives.size() > uint32(locale) && !localeData->Objectives[locale].empty())
                questObjectives = localeData->Objectives[locale];
            if (localeData->EndText.size() > uint32(locale) && !localeData->EndText[locale].empty())
                questEndText = localeData->EndText[locale];
        }
    }

    if (sWorld.getConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS))
        AddQuestLevelToTitle(questTitle, quest->GetQuestLevel());

    WorldPacket data(SMSG_QUESTGIVER_QUEST_DETAILS, 100);   // guess size
    data << uint64(npcGUID);
    data << uint32(quest->GetQuestId());
    data << questTitle;
    data << questDetails;
    data << questObjectives;
    data << uint32(activateAccept ? 1 : 0);
    data << uint32(quest->GetSuggestedPlayers());

    if (quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS))
    {
        data << uint32(0);                                  // Rewarded chosen items hidden
        data << uint32(0);                                  // Rewarded items hidden
        data << uint32(0);                                  // Rewarded money hidden
    }
    else
    {
        data << uint32(quest->GetRewChoiceItemsCount());
        for (uint32 i = 0; i < QUEST_REWARD_CHOICES_COUNT; ++i)
        {
            if (!quest->RewChoiceItemId[i])
                continue;

            data << uint32(quest->RewChoiceItemId[i]);
            data << uint32(quest->RewChoiceItemCount[i]);

            if (ItemTemplate const* itemTemplate = sObjectMgr.GetItemTemplate(quest->RewChoiceItemId[i]))
                data << uint32(itemTemplate->DisplayInfoID);
            else
                data << uint32(0x00);
        }

        data << uint32(quest->GetRewItemsCount());

        for (uint32 i = 0; i < QUEST_REWARDS_COUNT; ++i)
        {
            if (!quest->RewItemId[i])
                continue;

            data << uint32(quest->RewItemId[i]);
            data << uint32(quest->RewItemCount[i]);

            if (ItemTemplate const* itemTemplate = sObjectMgr.GetItemTemplate(quest->RewItemId[i]))
                data << uint32(itemTemplate->DisplayInfoID);
            else
                data << uint32(0);
        }

        data << uint32(quest->GetRewOrReqMoney());
    }

    // rewarded honor points. Multiply with 10 to satisfy client
    data << uint32(10 * Oregon::Honor::hk_honor_at_level(_session->GetPlayer()->getLevel(), quest->GetRewHonorableKills()));
    data << uint32(quest->GetRewSpell());                  // reward spell, this spell will display (icon) (casted if RewSpellCast == 0)
    data << uint32(quest->GetRewSpellCast());              // casted spell
    data << uint32(quest->GetCharTitleId());               // CharTitleId, new 2.4.0, player gets this title (id from CharTitles)

    data << uint32(QUEST_EMOTE_COUNT);
    for (uint32 i = 0; i < QUEST_EMOTE_COUNT; ++i)
    {
        data << uint32(quest->DetailsEmote[i]);
        data << uint32(0);                                  // DetailsEmoteDelay
    }
    _session->SendPacket(&data);

    DEBUG_LOG("WORLD: Sent SMSG_QUESTGIVER_QUEST_DETAILS NPCGuid=%u, questid=%u", GUID_LOPART(npcGUID), quest->GetQuestId());
}

void PlayerMenu::SendQuestQueryResponse(Quest const* quest) const
{
    std::string questTitle = quest->GetTitle();
    std::string questDetails = quest->GetDetails();
    std::string questObjectives = quest->GetObjectives();
    std::string questEndText = quest->GetEndText();
    //std::string questCompletedText = quest->GetCompletedText();

    std::string questObjectiveText[QUEST_OBJECTIVES_COUNT];
    for (uint32 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
        questObjectiveText[i] = quest->ObjectiveText[i];

    int32 locale = _session->GetSessionDbLocaleIndex();
    if (locale >= 0)
    {
        if (QuestLocale const* localeData = sObjectMgr.GetQuestLocale(quest->GetQuestId()))
        {
            if (localeData->Title.size() > uint32(locale) && !localeData->Title[locale].empty())
                questTitle = localeData->Title[locale];
            if (localeData->Details.size() > uint32(locale) && !localeData->Details[locale].empty())
                questDetails = localeData->Details[locale];
            if (localeData->Objectives.size() > uint32(locale) && !localeData->Objectives[locale].empty())
                questObjectives = localeData->Objectives[locale];
            if (localeData->EndText.size() > uint32(locale) && !localeData->EndText[locale].empty())
                questEndText = localeData->EndText[locale];

            for (int i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
                if (localeData->ObjectiveText[i].size() > locale && !localeData->ObjectiveText[i][locale].empty())
                    questObjectiveText[i] = localeData->ObjectiveText[i][locale];
        }
    }

    WorldPacket data(SMSG_QUEST_QUERY_RESPONSE, 100);     // guess size

    data << uint32(quest->GetQuestId());                   // quest id
    data << uint32(quest->GetQuestMethod());               // Accepted values: 0, 1 or 2. 0 == IsAutoComplete() (skip objectives/details)
    data << int32(quest->GetQuestLevel());                 // may be 0, -1, static data, in other cases must be used dynamic level: Player::GetQuestLevelForPlayer
    data << uint32(quest->GetZoneOrSort());                // zone or sort to display in quest log

    data << uint32(quest->GetType());                      // quest type
    data << uint32(quest->GetSuggestedPlayers());          // suggested players count

    data << uint32(quest->GetRepObjectiveFaction());       // shown in quest log as part of quest objective
    data << uint32(quest->GetRepObjectiveValue());         // shown in quest log as part of quest objective

    data << uint32(0);                                      // RequiredOpositeRepFaction
    data << uint32(0);                                      // RequiredOpositeRepValue, required faction value with another (oposite) faction (objective)

    data << uint32(quest->GetNextQuestInChain());          // client will request this quest from NPC, if not 0

    if (quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS))
        data << uint32(0);                                  // Hide money rewarded
    else
        data << uint32(quest->GetRewOrReqMoney());         // reward money (below max lvl)

    data << uint32(quest->GetRewMoneyMaxLevel());          // used in XP calculation at client
    data << uint32(quest->GetRewSpell());                  // reward spell, this spell will display (icon) (casted if RewSpellCast == 0)
    data << uint32(quest->GetRewSpellCast());              // casted spell

    // rewarded honor points
    data << uint32(Oregon::Honor::hk_honor_at_level(_session->GetPlayer()->getLevel(), quest->GetRewHonorableKills()));
    data << uint32(quest->GetSrcItemId());
    data << uint32(quest->GetFlags() & 0xFFFF);
    data << uint32(quest->GetCharTitleId());               // CharTitleId, new 2.4.0, player gets this title (id from CharTitles)

    if (quest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS))
    {
        for (uint32 i = 0; i < QUEST_REWARDS_COUNT; ++i)
            data << uint32(0) << uint32(0);
        for (uint32 i = 0; i < QUEST_REWARD_CHOICES_COUNT; ++i)
            data << uint32(0) << uint32(0);
    }
    else
    {
        for (uint32 i = 0; i < QUEST_REWARDS_COUNT; ++i)
        {
            data << uint32(quest->RewItemId[i]);
            data << uint32(quest->RewItemCount[i]);
        }
        for (uint32 i = 0; i < QUEST_REWARD_CHOICES_COUNT; ++i)
        {
            data << uint32(quest->RewChoiceItemId[i]);
            data << uint32(quest->RewChoiceItemCount[i]);
        }
    }

    data << quest->GetPointMapId();
    data << quest->GetPointX();
    data << quest->GetPointY();
    data << quest->GetPointOpt();

    data << questTitle;
    data << questObjectives;
    data << questDetails;
    data << questEndText;

    for (uint32 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
    {
        if (quest->ReqCreatureOrGOId[i] < 0)
            data << uint32((quest->ReqCreatureOrGOId[i] * (-1)) | 0x80000000);    // client expects gameobject template id in form (id|0x80000000)
        else
            data << uint32(quest->ReqCreatureOrGOId[i]);

        data << uint32(quest->ReqCreatureOrGOCount[i]);
        data << uint32(quest->RequiredItemId[i]);
        data << uint32(quest->ReqItemCount[i]);
    }

    for (uint32 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
        data << questObjectiveText[i];

    _session->SendPacket(&data);
    DEBUG_LOG("WORLD: Sent SMSG_QUEST_QUERY_RESPONSE questid=%u", quest->GetQuestId());
}

void PlayerMenu::SendQuestGiverOfferReward(Quest const* quest, uint64 npcGUID, bool enableNext) const
{
    std::string questTitle = quest->GetTitle();
    std::string questOfferRewardText = quest->GetOfferRewardText();

    int locale = _session->GetSessionDbLocaleIndex();
    if (locale >= 0)
    {
        if (QuestLocale const* localeData = sObjectMgr.GetQuestLocale(quest->GetQuestId()))
        {
            if (localeData->Title.size() > uint32(locale) && !localeData->Title[locale].empty())
                questTitle = localeData->Title[locale];
            if (localeData->OfferRewardText.size() > uint32(locale) && !localeData->OfferRewardText[locale].empty())
                questOfferRewardText = localeData->OfferRewardText[locale];
        }
    }

    WorldPacket data(SMSG_QUESTGIVER_OFFER_REWARD, 50);   // guess size

    data << uint64(npcGUID);
    data << uint32(quest->GetQuestId());
    data << questTitle;
    data << questOfferRewardText;

    data << uint32(enableNext ? 1 : 0);
    data << uint32(0);                                      // unk

    uint32 emoteCount = 0;
    for (uint32 i = 0; i < QUEST_EMOTE_COUNT; ++i)
    {
        if (quest->OfferRewardEmote[i] <= 0)
            break;
        ++emoteCount;
    }

    data << emoteCount;                                     // Emote Count
    for (uint32 i = 0; i < emoteCount; ++i)
    {
        data << uint32(0);                                  // Delay Emote
        data << uint32(quest->OfferRewardEmote[i]);
    }

    data << uint32(quest->GetRewChoiceItemsCount());
    for (uint32 i = 0; i < quest->GetRewChoiceItemsCount(); ++i)
    {
        data << uint32(quest->RewChoiceItemId[i]);
        data << uint32(quest->RewChoiceItemCount[i]);

        if (ItemTemplate const* itemTemplate = sObjectMgr.GetItemTemplate(quest->RewChoiceItemId[i]))
            data << uint32(itemTemplate->DisplayInfoID);
        else
            data << uint32(0);
    }

    data << uint32(quest->GetRewItemsCount());
    for (uint32 i = 0; i < quest->GetRewItemsCount(); ++i)
    {
        data << uint32(quest->RewItemId[i]);
        data << uint32(quest->RewItemCount[i]);

        if (ItemTemplate const* itemTemplate = sObjectMgr.GetItemTemplate(quest->RewItemId[i]))
            data << uint32(itemTemplate->DisplayInfoID);
        else
            data << uint32(0);
    }

    data << uint32(quest->GetRewOrReqMoney());

    // rewarded honor points. Multiply with 10 to satisfy client
    data << uint32(10 * Oregon::Honor::hk_honor_at_level(_session->GetPlayer()->getLevel(), quest->GetRewHonorableKills()));
    data << uint32(0x08);                                   // unused by client?
    data << uint32(quest->GetRewSpell());                  // reward spell, this spell will display (icon) (casted if RewSpellCast == 0)
    data << uint32(quest->GetRewSpellCast());              // casted spell
    data << uint32(0);                                      // unknown
    _session->SendPacket(&data);
    sLog.outDebug("WORLD: Sent SMSG_QUESTGIVER_OFFER_REWARD NPCGuid=%u, questid=%u", GUID_LOPART(npcGUID), quest->GetQuestId());
}

void PlayerMenu::SendQuestGiverRequestItems(Quest const *quest, uint64 npcGUID, bool canComplete, bool closeOnCancel) const
{
    // We can always call to RequestItems, but this packet only goes out if there are actually
    // items.  Otherwise, we'll skip straight to the OfferReward

    std::string questTitle = quest->GetTitle();
    std::string requestItemsText = quest->GetRequestItemsText();

    int32 locale = _session->GetSessionDbLocaleIndex();
    if (locale >= 0)
    {
        if (QuestLocale const* localeData = sObjectMgr.GetQuestLocale(quest->GetQuestId()))
        {
            if (localeData->Title.size() > uint32(locale) && !localeData->Title[locale].empty())
                questTitle = localeData->Title[locale];
            if (localeData->RequestItemsText.size() > uint32(locale) && !localeData->RequestItemsText[locale].empty())
                requestItemsText = localeData->RequestItemsText[locale];
        }
    }

    // We may wish a better check, perhaps checking the real quest requirements
    if (quest->GetRequestItemsText().empty() && canComplete)
    {
        SendQuestGiverOfferReward(quest, npcGUID, true);
        return;
    }

    if (sWorld.getConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS))
        AddQuestLevelToTitle(questTitle, quest->GetQuestLevel());

    WorldPacket data(SMSG_QUESTGIVER_REQUEST_ITEMS, 50);  // guess size
    data << uint64(npcGUID);
    data << uint32(quest->GetQuestId());
    data << questTitle;
    data << requestItemsText;

    data << uint32(0x00);                                   // unknown

    if (canComplete)
        data << quest->GetCompleteEmote();
    else
        data << quest->GetIncompleteEmote();

    // Close Window after cancel
    if (closeOnCancel)
        data << uint32(0x01);
    else
        data << uint32(0x00);

    data << uint32(0x00);                                   // unknown

    // Required Money
    data << uint32(quest->GetRewOrReqMoney() < 0 ? -quest->GetRewOrReqMoney() : 0);

    data << uint32(quest->GetReqItemsCount());

    for (int i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
    {
        if (!quest->RequiredItemId[i])
            continue;

        data << uint32(quest->RequiredItemId[i]);
        data << uint32(quest->ReqItemCount[i]);

        if (ItemTemplate const* itemTemplate = sObjectMgr.GetItemTemplate(quest->RequiredItemId[i]))
            data << uint32(itemTemplate->DisplayInfoID);
        else
            data << uint32(0);
    }

    if (!canComplete)
        data << uint32(0x00);
    else
        data << uint32(0x03);

    data << uint32(0x04);
    data << uint32(0x08);
    data << uint32(0x10);

    _session->SendPacket(&data);
    sLog.outDebug("WORLD: Sent SMSG_QUESTGIVER_REQUEST_ITEMS NPCGuid=%u, questid=%u", GUID_LOPART(npcGUID), quest->GetQuestId());
}

void PlayerMenu::AddQuestLevelToTitle(std::string& title, int32 level)
{
    // Adds the quest level to the front of the quest title
    // example: [13] Westfall Stew

    std::stringstream questTitlePretty;
    questTitlePretty << "[" << level << "] " << title;
    title = questTitlePretty.str();
}

