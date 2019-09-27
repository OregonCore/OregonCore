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

#include "QuestDef.h"
#include "GossipDef.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Formulas.h"

GossipMenu::GossipMenu()
{
    m_gItems.reserve(16);                                   // can be set for max from most often sizes to speedup push_back and less memory use
    m_gMenuId = 0;
}

GossipMenu::~GossipMenu()
{
    ClearMenu();
}

void GossipMenu::AddMenuItem(uint8 Icon, const std::string& Message, uint32 dtSender, uint32 dtAction, const std::string& BoxMessage, uint32 BoxMoney, bool Coded)
{
    ASSERT(m_gItems.size() <= GOSSIP_MAX_MENU_ITEMS);

    GossipMenuItem gItem;

    gItem.m_gIcon       = Icon;
    gItem.m_gMessage    = Message;
    gItem.m_gCoded      = Coded;
    gItem.m_gSender     = dtSender;
    gItem.m_gOptionId   = dtAction;
    gItem.m_gBoxMessage = BoxMessage;
    gItem.m_gBoxMoney   = BoxMoney;

    m_gItems.push_back(gItem);
}

void GossipMenu::AddGossipMenuItemData(uint32 action_menu, uint32 action_poi, uint32 action_script)
{
    GossipMenuItemData pItemData;

    pItemData.m_gAction_menu    = action_menu;
    pItemData.m_gAction_poi     = action_poi;
    pItemData.m_gAction_script  = action_script;

    m_gItemsData.push_back(pItemData);
}

void GossipMenu::AddMenuItem(uint8 Icon, const std::string& Message, bool Coded)
{
    AddMenuItem(Icon, Message, 0, 0, "", 0, Coded);
}

void GossipMenu::AddMenuItem(uint8 Icon, char const* Message, bool Coded)
{
    AddMenuItem(Icon, std::string(Message ? Message : ""), Coded);
}

void GossipMenu::AddMenuItem(uint8 Icon, char const* Message, uint32 dtSender, uint32 dtAction, char const* BoxMessage, uint32 BoxMoney, bool Coded)
{
    AddMenuItem(Icon, std::string(Message ? Message : ""), dtSender, dtAction, std::string(BoxMessage ? BoxMessage : ""), BoxMoney, Coded);
}

uint32 GossipMenu::MenuItemSender(unsigned int ItemId)
{
    if (ItemId >= m_gItems.size()) return 0;

    return m_gItems[ ItemId ].m_gSender;
}

uint32 GossipMenu::MenuItemAction(unsigned int ItemId)
{
    if (ItemId >= m_gItems.size()) return 0;

    return m_gItems[ ItemId ].m_gOptionId;
}

bool GossipMenu::MenuItemCoded(unsigned int ItemId)
{
    if (ItemId >= m_gItems.size()) return 0;

    return m_gItems[ ItemId ].m_gCoded;
}

void GossipMenu::ClearMenu()
{
    m_gItems.clear();
    m_gItemsData.clear();
}

PlayerMenu::PlayerMenu(WorldSession* session) : pSession(session)
{
}

PlayerMenu::~PlayerMenu()
{
    ClearMenus();
}

void PlayerMenu::ClearMenus()
{
    mGossipMenu.ClearMenu();
    mQuestMenu.ClearMenu();
}

uint32 PlayerMenu::GossipOptionSender(unsigned int Selection)
{
    return mGossipMenu.MenuItemSender(Selection);
}

uint32 PlayerMenu::GossipOptionAction(unsigned int Selection)
{
    return mGossipMenu.MenuItemAction(Selection);
}

bool PlayerMenu::GossipOptionCoded(unsigned int Selection)
{
    return mGossipMenu.MenuItemCoded(Selection);
}

void PlayerMenu::SendGossipMenu(uint32 TitleTextId, uint64 objectGUID)
{
    WorldPacket data(SMSG_GOSSIP_MESSAGE, (100));           // guess size
    data << uint64(objectGUID);
    data << uint32(mGossipMenu.GetMenuId());                // new 2.4.0
    data << uint32(TitleTextId);
    data << uint32(mGossipMenu.MenuItemCount());            // max count 0x20

    for (uint32 iI = 0; iI < mGossipMenu.MenuItemCount(); ++iI)
    {
        GossipMenuItem const& gItem = mGossipMenu.GetItem(iI);
        data << uint32(iI);
        data << uint8(gItem.m_gIcon);
        data << uint8(gItem.m_gCoded);                      // makes pop up box password
        data << uint32(gItem.m_gBoxMoney);                  // money required to open menu, 2.0.3
        data << gItem.m_gMessage;                           // text for gossip item, max 0x800
        data << gItem.m_gBoxMessage;                        // accept text (related to money) pop up box, 2.0.3, max 0x800
    }

    data << uint32(mQuestMenu.MenuItemCount());           // max count 0x20

    // Store this instead of checking the Singleton every loop iteration
    bool questLevelInTitle = sWorld.getConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS);

    for (uint32 iI = 0; iI < mQuestMenu.MenuItemCount(); ++iI)
    {
        QuestMenuItem const& qItem = mQuestMenu.GetItem(iI);
        uint32 questID = qItem.m_qId;
        Quest const* pQuest = sObjectMgr.GetQuestTemplate(questID);

        data << uint32(questID);
        data << uint32(qItem.m_qIcon);
        data << int32(pQuest->GetQuestLevel());
        std::string Title = pQuest->GetTitle();

        int loc_idx = pSession->GetSessionDbLocaleIndex();
        if (loc_idx >= 0)
            if (QuestLocale const* ql = sObjectMgr.GetQuestLocale(questID))
                if (ql->Title.size() > uint32(loc_idx) && !ql->Title[loc_idx].empty())
                    Title = ql->Title[loc_idx];

        if (questLevelInTitle)
            AddQuestLevelToTitle(Title, pQuest->GetQuestLevel());

        data << Title;
    }

    pSession->SendPacket(&data);
}

void PlayerMenu::CloseGossip()
{
    WorldPacket data(SMSG_GOSSIP_COMPLETE, 0);
    pSession->SendPacket(&data);
}

void PlayerMenu::SendPointOfInterest(float X, float Y, uint32 Icon, uint32 Flags, uint32 Data, char const* locName)
{
    WorldPacket data(SMSG_GOSSIP_POI, (4 + 4 + 4 + 4 + 4 + 10)); // guess size
    data << uint32(Flags);
    data << float(X);
    data << float(Y);
    data << uint32(Icon);
    data << uint32(Data);
    data << locName;

    pSession->SendPacket(&data);
    //sLog.outDebug("WORLD: Sent SMSG_GOSSIP_POI");
}

void PlayerMenu::SendTalking(uint32 textID)
{
    GossipText const* pGossip = sObjectMgr.GetGossipText(textID);

    WorldPacket data(SMSG_NPC_TEXT_UPDATE, 100);          // guess size
    data << textID;                                         // can be < 0

    if (!pGossip)
    {
        for (uint32 i = 0; i < 8; ++i)
        {
            data << float(0);
            data << "Greetings $N";
            data << "Greetings $N";
            data << uint32(0);
            data << uint32(0);
            data << uint32(0);
            data << uint32(0);
            data << uint32(0);
            data << uint32(0);
            data << uint32(0);
        }
    }
    else
    {
        std::string Text_0[8], Text_1[8];
        for (int i = 0; i < 8; ++i)
        {
            Text_0[i] = pGossip->Options[i].Text_0;
            Text_1[i] = pGossip->Options[i].Text_1;
        }
        int loc_idx = pSession->GetSessionDbLocaleIndex();
        if (loc_idx >= 0)
        {
            if (NpcTextLocale const* nl = sObjectMgr.GetNpcTextLocale(textID))
            {
                for (int i = 0; i < 8; ++i)
                {
                    if (nl->Text_0[i].size() > uint32(loc_idx) && !nl->Text_0[i][loc_idx].empty())
                        Text_0[i] = nl->Text_0[i][loc_idx];
                    if (nl->Text_1[i].size() > uint32(loc_idx) && !nl->Text_1[i][loc_idx].empty())
                        Text_1[i] = nl->Text_1[i][loc_idx];
                }
            }
        }
        for (int i = 0; i < 8; ++i)
        {
            data << pGossip->Options[i].Probability;

            if (Text_0[i].empty())
                data << Text_1[i];
            else
                data << Text_0[i];

            if (Text_1[i].empty())
                data << Text_0[i];
            else
                data << Text_1[i];

            data << pGossip->Options[i].Language;

            data << pGossip->Options[i].Emotes[0]._Delay;
            data << pGossip->Options[i].Emotes[0]._Emote;

            data << pGossip->Options[i].Emotes[1]._Delay;
            data << pGossip->Options[i].Emotes[1]._Emote;

            data << pGossip->Options[i].Emotes[2]._Delay;
            data << pGossip->Options[i].Emotes[2]._Emote;
        }
    }
    pSession->SendPacket(&data);

    sLog.outDebug("WORLD: Sent SMSG_NPC_TEXT_UPDATE ");
}

void PlayerMenu::SendTalking(char const* title, char const* text)
{
    WorldPacket data(SMSG_NPC_TEXT_UPDATE, 50);           // guess size
    data << uint32(0);
    for (uint32 i = 0; i < 8; ++i)
    {
        data << float(0);
        data << title;
        data << text;
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
    }

    pSession->SendPacket(&data);

    sLog.outDebug("WORLD: Sent SMSG_NPC_TEXT_UPDATE ");
}

/*********************************************************/
/***                   QUEST SYSTEM                   ***/
/*********************************************************/

QuestMenu::QuestMenu()
{
    m_qItems.reserve(16);                                   // can be set for max from most often sizes to speedup push_back and less memory use
}

QuestMenu::~QuestMenu()
{
    ClearMenu();
}

void QuestMenu::AddMenuItem(uint32 QuestId, uint8 Icon)
{
    Quest const* qinfo = sObjectMgr.GetQuestTemplate(QuestId);
    if (!qinfo) return;

    ASSERT(m_qItems.size() <= GOSSIP_MAX_MENU_ITEMS);

    QuestMenuItem qItem;

    qItem.m_qId        = QuestId;
    qItem.m_qIcon      = Icon;

    m_qItems.push_back(qItem);
}

bool QuestMenu::HasItem(uint32 questid)
{
    for (QuestMenuItemList::iterator i = m_qItems.begin(); i != m_qItems.end(); ++i)
        if (i->m_qId == questid)
            return true;
    return false;
}

void QuestMenu::ClearMenu()
{
    m_qItems.clear();
}

void PlayerMenu::SendQuestGiverQuestList(QEmote eEmote, const std::string& Title, uint64 npcGUID)
{
    WorldPacket data(SMSG_QUESTGIVER_QUEST_LIST, 100);    // guess size
    data << uint64(npcGUID);
    data << Title;
    data << uint32(eEmote._Delay);                         // player emote
    data << uint32(eEmote._Emote);                         // NPC emote
    data << uint8 (mQuestMenu.MenuItemCount());

    // Store this instead of checking the Singleton every loop iteration
    bool questLevelInTitle = sWorld.getConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS);

    for (uint32 iI = 0; iI < mQuestMenu.MenuItemCount(); iI++)
    {
        QuestMenuItem const& qmi = mQuestMenu.GetItem(iI);

        uint32 questID = qmi.m_qId;
        Quest const* pQuest = sObjectMgr.GetQuestTemplate(questID);

        std::string title = pQuest ? pQuest->GetTitle() : "";

        int loc_idx = pSession->GetSessionDbLocaleIndex();
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
        data << uint32(qmi.m_qIcon);
        data << int32(pQuest->GetQuestLevel());
        data << title;
    }
    pSession->SendPacket(&data);
    DEBUG_LOG("WORLD: Sent SMSG_QUESTGIVER_QUEST_LIST NPC Guid=%u", GUID_LOPART(npcGUID));
}

void PlayerMenu::SendQuestGiverStatus(uint8 questStatus, uint64 npcGUID)
{
    WorldPacket data(SMSG_QUESTGIVER_STATUS, 9);
    data << uint64(npcGUID);
    data << uint8(questStatus);

    pSession->SendPacket(&data);
    DEBUG_LOG("WORLD: Sent SMSG_QUESTGIVER_STATUS NPC Guid=%u, status=%u", GUID_LOPART(npcGUID), questStatus);
}

void PlayerMenu::SendQuestGiverQuestDetails(Quest const* pQuest, uint64 npcGUID, bool ActivateAccept)
{
    std::string Title      = pQuest->GetTitle();
    std::string Details    = pQuest->GetDetails();
    std::string Objectives = pQuest->GetObjectives();
    std::string EndText    = pQuest->GetEndText();

    int loc_idx = pSession->GetSessionDbLocaleIndex();
    if (loc_idx >= 0)
    {
        QuestLocale const* ql = sObjectMgr.GetQuestLocale(pQuest->GetQuestId());
        if (ql)
        {
            if (ql->Title.size() > uint32(loc_idx) && !ql->Title[loc_idx].empty())
                Title = ql->Title[loc_idx];
            if (ql->Details.size() > uint32(loc_idx) && !ql->Details[loc_idx].empty())
                Details = ql->Details[loc_idx];
            if (ql->Objectives.size() > uint32(loc_idx) && !ql->Objectives[loc_idx].empty())
                Objectives = ql->Objectives[loc_idx];
            if (ql->EndText.size() > uint32(loc_idx) && !ql->EndText[loc_idx].empty())
                EndText = ql->EndText[loc_idx];
        }
    }

    if (sWorld.getConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS))
        AddQuestLevelToTitle(Title, pQuest->GetQuestLevel());

    WorldPacket data(SMSG_QUESTGIVER_QUEST_DETAILS, 100);   // guess size
    data << uint64(npcGUID);
    data << uint32(pQuest->GetQuestId());
    data << Title;
    data << Details;
    data << Objectives;
    data << uint32(ActivateAccept);
    data << uint32(pQuest->GetSuggestedPlayers());

    if (pQuest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS))
    {
        data << uint32(0);                                  // Rewarded chosen items hidden
        data << uint32(0);                                  // Rewarded items hidden
        data << uint32(0);                                  // Rewarded money hidden
    }
    else
    {
        ItemTemplate const* IProto;

        data << uint32(pQuest->GetRewChoiceItemsCount());
        for (uint32 i = 0; i < QUEST_REWARD_CHOICES_COUNT; ++i)
        {
            if (!pQuest->RewChoiceItemId[i])
                continue;

            data << uint32(pQuest->RewChoiceItemId[i]);
            data << uint32(pQuest->RewChoiceItemCount[i]);

            IProto = sObjectMgr.GetItemTemplate(pQuest->RewChoiceItemId[i]);
            if (IProto)
                data << uint32(IProto->DisplayInfoID);
            else
                data << uint32(0x00);
        }

        data << uint32(pQuest->GetRewItemsCount());

        for (uint32 i = 0; i < QUEST_REWARDS_COUNT; ++i)
        {
            if (!pQuest->RewItemId[i])
                continue;

            data << uint32(pQuest->RewItemId[i]);
            data << uint32(pQuest->RewItemCount[i]);

            IProto = sObjectMgr.GetItemTemplate(pQuest->RewItemId[i]);
            if (IProto)
                data << uint32(IProto->DisplayInfoID);
            else
                data << uint32(0);
        }

        data << uint32(pQuest->GetRewOrReqMoney());
    }

    // rewarded honor points. Multiply with 10 to satisfy client
    data << uint32(10 * Oregon::Honor::hk_honor_at_level(pSession->GetPlayer()->getLevel(), pQuest->GetRewHonorableKills()));
    data << uint32(pQuest->GetRewSpell());                  // reward spell, this spell will display (icon) (casted if RewSpellCast == 0)
    data << uint32(pQuest->GetRewSpellCast());              // casted spell
    data << uint32(pQuest->GetCharTitleId());               // CharTitleId, new 2.4.0, player gets this title (id from CharTitles)

    data << uint32(QUEST_EMOTE_COUNT);
    for (uint32 i = 0; i < QUEST_EMOTE_COUNT; ++i)
    {
        data << uint32(pQuest->DetailsEmote[i]);
        data << uint32(0);                                  // DetailsEmoteDelay
    }
    pSession->SendPacket(&data);

    DEBUG_LOG("WORLD: Sent SMSG_QUESTGIVER_QUEST_DETAILS NPCGuid=%u, questid=%u", GUID_LOPART(npcGUID), pQuest->GetQuestId());
}

void PlayerMenu::SendQuestQueryResponse(Quest const* pQuest)
{
    std::string Title, Details, Objectives, EndText;
    std::string ObjectiveText[QUEST_OBJECTIVES_COUNT];
    Title = pQuest->GetTitle();
    Details = pQuest->GetDetails();
    Objectives = pQuest->GetObjectives();
    EndText = pQuest->GetEndText();
    for (int i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
        ObjectiveText[i] = pQuest->ObjectiveText[i];

    int loc_idx = pSession->GetSessionDbLocaleIndex();
    if (loc_idx >= 0)
    {
        if (QuestLocale const* ql = sObjectMgr.GetQuestLocale(pQuest->GetQuestId()))
        {
            if (ql->Title.size() > uint32(loc_idx) && !ql->Title[loc_idx].empty())
                Title = ql->Title[loc_idx];
            if (ql->Details.size() > uint32(loc_idx) && !ql->Details[loc_idx].empty())
                Details = ql->Details[loc_idx];
            if (ql->Objectives.size() > uint32(loc_idx) && !ql->Objectives[loc_idx].empty())
                Objectives = ql->Objectives[loc_idx];
            if (ql->EndText.size() > uint32(loc_idx) && !ql->EndText[loc_idx].empty())
                EndText = ql->EndText[loc_idx];

            for (int i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
                if (ql->ObjectiveText[i].size() > loc_idx && !ql->ObjectiveText[i][loc_idx].empty())
                    ObjectiveText[i] = ql->ObjectiveText[i][loc_idx];
        }
    }

    WorldPacket data(SMSG_QUEST_QUERY_RESPONSE, 100);     // guess size

    data << uint32(pQuest->GetQuestId());                   // quest id
    data << uint32(pQuest->GetQuestMethod());               // Accepted values: 0, 1 or 2. 0 == IsAutoComplete() (skip objectives/details)
    data << int32(pQuest->GetQuestLevel());                 // may be 0, -1, static data, in other cases must be used dynamic level: Player::GetQuestLevelForPlayer
    data << uint32(pQuest->GetZoneOrSort());                // zone or sort to display in quest log

    data << uint32(pQuest->GetType());                      // quest type
    data << uint32(pQuest->GetSuggestedPlayers());          // suggested players count

    data << uint32(pQuest->GetRepObjectiveFaction());       // shown in quest log as part of quest objective
    data << uint32(pQuest->GetRepObjectiveValue());         // shown in quest log as part of quest objective

    data << uint32(0);                                      // RequiredOpositeRepFaction
    data << uint32(0);                                      // RequiredOpositeRepValue, required faction value with another (oposite) faction (objective)

    data << uint32(pQuest->GetNextQuestInChain());          // client will request this quest from NPC, if not 0

    if (pQuest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS))
        data << uint32(0);                                  // Hide money rewarded
    else
        data << uint32(pQuest->GetRewOrReqMoney());         // reward money (below max lvl)

    data << uint32(pQuest->GetRewMoneyMaxLevel());          // used in XP calculation at client
    data << uint32(pQuest->GetRewSpell());                  // reward spell, this spell will display (icon) (casted if RewSpellCast == 0)
    data << uint32(pQuest->GetRewSpellCast());              // casted spell

    // rewarded honor points
    data << uint32(Oregon::Honor::hk_honor_at_level(pSession->GetPlayer()->getLevel(), pQuest->GetRewHonorableKills()));
    data << uint32(pQuest->GetSrcItemId());
    data << uint32(pQuest->GetFlags() & 0xFFFF);
    data << uint32(pQuest->GetCharTitleId());               // CharTitleId, new 2.4.0, player gets this title (id from CharTitles)

    int iI;

    if (pQuest->HasFlag(QUEST_FLAGS_HIDDEN_REWARDS))
    {
        for (iI = 0; iI < QUEST_REWARDS_COUNT; ++iI)
            data << uint32(0) << uint32(0);
        for (iI = 0; iI < QUEST_REWARD_CHOICES_COUNT; ++iI)
            data << uint32(0) << uint32(0);
    }
    else
    {
        for (iI = 0; iI < QUEST_REWARDS_COUNT; ++iI)
        {
            data << uint32(pQuest->RewItemId[iI]);
            data << uint32(pQuest->RewItemCount[iI]);
        }
        for (iI = 0; iI < QUEST_REWARD_CHOICES_COUNT; ++iI)
        {
            data << uint32(pQuest->RewChoiceItemId[iI]);
            data << uint32(pQuest->RewChoiceItemCount[iI]);
        }
    }

    data << pQuest->GetPointMapId();
    data << pQuest->GetPointX();
    data << pQuest->GetPointY();
    data << pQuest->GetPointOpt();

    data << Title;
    data << Objectives;
    data << Details;
    data << EndText;

    for (iI = 0; iI < QUEST_OBJECTIVES_COUNT; ++iI)
    {
        if (pQuest->ReqCreatureOrGOId[iI] < 0)
        {
            // client expected gameobject template id in form (id|0x80000000)
            data << uint32((pQuest->ReqCreatureOrGOId[iI] * (-1)) | 0x80000000);
        }
        else
            data << uint32(pQuest->ReqCreatureOrGOId[iI]);
        data << uint32(pQuest->ReqCreatureOrGOCount[iI]);
        data << uint32(pQuest->ReqItemId[iI]);
        data << uint32(pQuest->ReqItemCount[iI]);
    }

    for (iI = 0; iI < QUEST_OBJECTIVES_COUNT; ++iI)
        data << ObjectiveText[iI];

    pSession->SendPacket(&data);
    DEBUG_LOG("WORLD: Sent SMSG_QUEST_QUERY_RESPONSE questid=%u", pQuest->GetQuestId());
}

void PlayerMenu::SendQuestGiverOfferReward(Quest const* pQuest, uint64 npcGUID, bool EnableNext)
{
    std::string Title = pQuest->GetTitle();
    std::string OfferRewardText = pQuest->GetOfferRewardText();

    int loc_idx = pSession->GetSessionDbLocaleIndex();
    if (loc_idx >= 0)
    {
        if (QuestLocale const* ql = sObjectMgr.GetQuestLocale(pQuest->GetQuestId()))
        {
            if (ql->Title.size() > uint32(loc_idx) && !ql->Title[loc_idx].empty())
                Title = ql->Title[loc_idx];
            if (ql->OfferRewardText.size() > uint32(loc_idx) && !ql->OfferRewardText[loc_idx].empty())
                OfferRewardText = ql->OfferRewardText[loc_idx];
        }
    }

    WorldPacket data(SMSG_QUESTGIVER_OFFER_REWARD, 50);   // guess size

    data << uint64(npcGUID);
    data << uint32(pQuest->GetQuestId());
    data << Title;
    data << OfferRewardText;

    data << uint32(EnableNext);
    data << uint32(0);                                      // unk

    uint32 EmoteCount = 0;
    for (uint32 i = 0; i < QUEST_EMOTE_COUNT; ++i)
    {
        if (pQuest->OfferRewardEmote[i] <= 0)
            break;
        ++EmoteCount;
    }

    data << EmoteCount;                                     // Emote Count
    for (uint32 i = 0; i < EmoteCount; ++i)
    {
        data << uint32(0);                                  // Delay Emote
        data << uint32(pQuest->OfferRewardEmote[i]);
    }

    ItemTemplate const* pItem;

    data << uint32(pQuest->GetRewChoiceItemsCount());
    for (uint32 i = 0; i < pQuest->GetRewChoiceItemsCount(); ++i)
    {
        pItem = sObjectMgr.GetItemTemplate(pQuest->RewChoiceItemId[i]);

        data << uint32(pQuest->RewChoiceItemId[i]);
        data << uint32(pQuest->RewChoiceItemCount[i]);

        if (pItem)
            data << uint32(pItem->DisplayInfoID);
        else
            data << uint32(0);
    }

    data << uint32(pQuest->GetRewItemsCount());
    for (uint32 i = 0; i < pQuest->GetRewItemsCount(); ++i)
    {
        pItem = sObjectMgr.GetItemTemplate(pQuest->RewItemId[i]);
        data << uint32(pQuest->RewItemId[i]);
        data << uint32(pQuest->RewItemCount[i]);

        if (pItem)
            data << uint32(pItem->DisplayInfoID);
        else
            data << uint32(0);
    }

    data << uint32(pQuest->GetRewOrReqMoney());

    // rewarded honor points. Multiply with 10 to satisfy client
    data << uint32(10 * Oregon::Honor::hk_honor_at_level(pSession->GetPlayer()->getLevel(), pQuest->GetRewHonorableKills()));
    data << uint32(0x08);                                   // unused by client?
    data << uint32(pQuest->GetRewSpell());                  // reward spell, this spell will display (icon) (casted if RewSpellCast == 0)
    data << uint32(pQuest->GetRewSpellCast());              // casted spell
    data << uint32(0);                                      // unknown
    pSession->SendPacket(&data);
    sLog.outDebug("WORLD: Sent SMSG_QUESTGIVER_OFFER_REWARD NPCGuid=%u, questid=%u", GUID_LOPART(npcGUID), pQuest->GetQuestId());
}

void PlayerMenu::SendQuestGiverRequestItems(Quest const* pQuest, uint64 npcGUID, bool Completable, bool CloseOnCancel)
{
    // We can always call to RequestItems, but this packet only goes out if there are actually
    // items.  Otherwise, we'll skip straight to the OfferReward

    std::string Title = pQuest->GetTitle();
    std::string RequestItemsText = pQuest->GetRequestItemsText();

    int loc_idx = pSession->GetSessionDbLocaleIndex();
    if (loc_idx >= 0)
    {
        if (QuestLocale const* ql = sObjectMgr.GetQuestLocale(pQuest->GetQuestId()))
        {
            if (ql->Title.size() > uint32(loc_idx) && !ql->Title[loc_idx].empty())
                Title = ql->Title[loc_idx];
            if (ql->RequestItemsText.size() > uint32(loc_idx) && !ql->RequestItemsText[loc_idx].empty())
                RequestItemsText = ql->RequestItemsText[loc_idx];
        }
    }

    // We may wish a better check, perhaps checking the real quest requirements
    if (pQuest->GetRequestItemsText().empty() && Completable)
    {
        SendQuestGiverOfferReward(pQuest, npcGUID, true);
        return;
    }

    if (sWorld.getConfig(CONFIG_UI_QUESTLEVELS_IN_DIALOGS))
        AddQuestLevelToTitle(Title, pQuest->GetQuestLevel());

    WorldPacket data(SMSG_QUESTGIVER_REQUEST_ITEMS, 50);  // guess size
    data << uint64(npcGUID);
    data << uint32(pQuest->GetQuestId());
    data << Title;
    data << RequestItemsText;

    data << uint32(0x00);                                   // unknown

    if (Completable)
        data << pQuest->GetCompleteEmote();
    else
        data << pQuest->GetIncompleteEmote();

    // Close Window after cancel
    if (CloseOnCancel)
        data << uint32(0x01);
    else
        data << uint32(0x00);

    data << uint32(0x00);                                   // unknown

    // Required Money
    data << uint32(pQuest->GetRewOrReqMoney() < 0 ? -pQuest->GetRewOrReqMoney() : 0);

    data << uint32(pQuest->GetReqItemsCount());
    ItemTemplate const* pItem;
    for (int i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
    {
        if (!pQuest->ReqItemId[i])
            continue;

        pItem = sObjectMgr.GetItemTemplate(pQuest->ReqItemId[i]);

        data << uint32(pQuest->ReqItemId[i]);
        data << uint32(pQuest->ReqItemCount[i]);

        if (pItem)
            data << uint32(pItem->DisplayInfoID);
        else
            data << uint32(0);
    }

    if (!Completable)
        data << uint32(0x00);
    else
        data << uint32(0x03);

    data << uint32(0x04);
    data << uint32(0x08);
    data << uint32(0x10);

    pSession->SendPacket(&data);
    sLog.outDebug("WORLD: Sent SMSG_QUESTGIVER_REQUEST_ITEMS NPCGuid=%u, questid=%u", GUID_LOPART(npcGUID), pQuest->GetQuestId());
}

void PlayerMenu::AddQuestLevelToTitle(std::string& title, int32 level)
{
    // Adds the quest level to the front of the quest title
    // example: [13] Westfall Stew

    std::stringstream questTitlePretty;
    questTitlePretty << "[" << level << "] " << title;
    title = questTitlePretty.str();
}

