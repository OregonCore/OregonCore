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
#include "Player.h"
#include "BattlegroundMgr.h"
#include "BattlegroundAV.h"
#include "BattlegroundAB.h"
#include "BattlegroundEY.h"
#include "BattlegroundWS.h"
#include "BattlegroundNA.h"
#include "BattlegroundBE.h"
#include "BattlegroundAA.h"
#include "BattlegroundRL.h"
#include "SharedDefines.h"
#include "MapManager.h"
#include "Map.h"
#include "ObjectMgr.h"
#include "World.h"
#include "Chat.h"
#include "ArenaTeam.h"
#include "DisableMgr.h"

INSTANTIATE_SINGLETON_1(BattlegroundMgr);

/*********************************************************/
/***           BATTLEGROUND QUEUE SYSTEM              ***/
/*********************************************************/

BattlegroundQueue::BattlegroundQueue()
{
    //queues are empty, we don't have to call clear()
    /*   for (int i = 0; i < MAX_BATTLEGROUND_QUEUES; i++)
        {
            //m_QueuedPlayers[i].Horde = 0;
            //m_QueuedPlayers[i].Alliance = 0;
            //m_QueuedPlayers[i].AverageTime = 0;
        }*/
}

BattlegroundQueue::~BattlegroundQueue()
{
    for (int i = 0; i < MAX_BATTLEGROUND_QUEUES; i++)
    {
        m_QueuedPlayers[i].clear();
        for (QueuedGroupsList::iterator itr = m_QueuedGroups[i].begin(); itr != m_QueuedGroups[i].end(); ++itr)
            delete (*itr);
        m_QueuedGroups[i].clear();
    }
}

// initialize eligible groups from the given source matching the given specifications
void BattlegroundQueue::EligibleGroups::Init(BattlegroundQueue::QueuedGroupsList* source, uint32 BgTypeId, uint32 side, uint32 MaxPlayers, uint8 ArenaType, bool IsRated, uint32 MinRating, uint32 MaxRating, uint32 DisregardTime, uint32 excludeTeam)
{
    // clear from prev initialization
    clear();
    BattlegroundQueue::QueuedGroupsList::iterator itr, next;
    // iterate through the source
    for (itr = source->begin(); itr != source->end(); itr = next)
    {
        next = itr;
        ++next;
        if ((*itr)->BgTypeId == BgTypeId &&     // bg type must match
            (*itr)->ArenaType == ArenaType &&   // arena type must match
            (*itr)->IsRated == IsRated &&       // israted must match
            (*itr)->IsInvitedToBGInstanceGUID == 0 && // leave out already invited groups
            (*itr)->Team == side &&             // match side
            (*itr)->Players.size() <= MaxPlayers &&   // the group must fit in the bg
            (!excludeTeam || (*itr)->ArenaTeamId != excludeTeam) && // if excludeTeam is specified, leave out those arena team ids
            (!IsRated || (*itr)->Players.size() == MaxPlayers) &&   // if rated, then pass only if the player count is exact NEEDS TESTING! (but now this should never happen)
            (!DisregardTime || (*itr)->JoinTime <= DisregardTime              // pass if disregard time is greater than join time
             || (*itr)->ArenaTeamRating == 0                 // pass if no rating info
             || ((*itr)->ArenaTeamRating >= MinRating       // pass if matches the rating range
                 && (*itr)->ArenaTeamRating <= MaxRating)))
        {
            // the group matches the conditions
            // using push_back for proper selecting when inviting
            push_back((*itr));
        }
    }
}

// selection pool initialization, used to clean up from prev selection
void BattlegroundQueue::SelectionPool::Init(EligibleGroups* curr)
{
    m_CurrEligGroups = curr;
    SelectedGroups.clear();
    PlayerCount = 0;
}

// remove group info from selection pool
void BattlegroundQueue::SelectionPool::RemoveGroup(GroupQueueInfo* ginfo)
{
    // find what to remove
    for (std::list<GroupQueueInfo*>::iterator itr = SelectedGroups.begin(); itr != SelectedGroups.end(); ++itr)
    {
        if ((*itr) == ginfo)
        {
            SelectedGroups.erase(itr);
            // decrease selected players count
            PlayerCount -= ginfo->Players.size();
            return;
        }
    }
}

// add group to selection
// used when building selection pools
void BattlegroundQueue::SelectionPool::AddGroup(GroupQueueInfo* ginfo)
{
    SelectedGroups.push_back(ginfo);
    // increase selected players count
    PlayerCount += ginfo->Players.size();
}

// add group to bg queue with the given leader and bg specifications
GroupQueueInfo* BattlegroundQueue::AddGroup(Player* leader, uint32 BgTypeId, uint8 ArenaType, bool isRated, uint32 arenaRating, uint32 arenateamid)
{
    uint32 queue_id = leader->GetBattlegroundQueueIdFromLevel();

    // create new ginfo
    // cannot use the method like in addplayer, because that could modify an in-queue group's stats
    // (e.g. leader leaving queue then joining as individual again)
    GroupQueueInfo* ginfo = new GroupQueueInfo;
    ginfo->BgTypeId                  = BgTypeId;
    ginfo->ArenaType                 = ArenaType;
    ginfo->ArenaTeamId               = arenateamid;
    ginfo->IsRated                   = isRated;
    ginfo->IsInvitedToBGInstanceGUID = 0;                       // maybe this should be modifiable by function arguments to enable selection of running instances?
    ginfo->JoinTime                  = getMSTime();
    ginfo->Team                      = leader->GetTeam();
    ginfo->ArenaTeamRating           = arenaRating;
    ginfo->OpponentsTeamRating       = 0;                       //initialize it to 0

    ginfo->Players.clear();

    m_QueuedGroups[queue_id].push_back(ginfo);

    // return ginfo, because it is needed to add players to this group info
    return ginfo;
}

void BattlegroundQueue::AddPlayer(Player* plr, GroupQueueInfo* ginfo)
{
    uint32 queue_id = plr->GetBattlegroundQueueIdFromLevel();

    //if player isn't in queue, he is added, if already is, then values are overwritten, no memory leak
    PlayerQueueInfo& info = m_QueuedPlayers[queue_id][plr->GetGUID()];
    info.InviteTime                 = 0;
    info.LastInviteTime             = 0;
    info.LastOnlineTime             = getMSTime();
    info.GroupInfo                  = ginfo;

    // add the pinfo to ginfo's list
    ginfo->Players[plr->GetGUID()]  = &info;

    if (sWorld.getConfig(CONFIG_BATTLEGROUND_QUEUE_ANNOUNCER_ENABLE))
    {
        //announce only once in a time
        if (!sWorld.getConfig(CONFIG_BATTLEGROUND_QUEUE_ANNOUNCER_PLAYERONLY) && m_QueuedPlayers[queue_id].size() % 5 != 0) return;
        Battleground* bg = sBattlegroundMgr.GetBattlegroundTemplate(ginfo->BgTypeId);
        if (!bg) return;

        char const* bgName = bg->GetName();

        uint32 q_min_level = Player::GetMinLevelForBattlegroundQueueId(queue_id);
        uint32 q_max_level = Player::GetMaxLevelForBattlegroundQueueId(queue_id);

        // replace hardcoded max level by player max level for nice output
        if (q_max_level > sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL))
            q_max_level = sWorld.getConfig(CONFIG_MAX_PLAYER_LEVEL);

        uint32 MinPlayers = bg->GetMinPlayersPerTeam();
        uint32 MaxPlayers = bg->GetMaxPlayersPerTeam();

        uint32 qHorde = 0;
        uint32 qAlliance = 0;

        for (std::map<uint64, PlayerQueueInfo>::iterator itr = m_QueuedPlayers[queue_id].begin(); itr != m_QueuedPlayers[queue_id].end(); ++itr)
        {
            Player* _player = sObjectMgr.GetPlayer((uint64)itr->first);
            if (_player)
            {
                if (_player->GetTeam() == ALLIANCE)
                    qAlliance++;
                else
                    qHorde++;
            }
        }

        // Show queue status to player only (when joining queue)
        if (sWorld.getConfig(CONFIG_BATTLEGROUND_QUEUE_ANNOUNCER_PLAYERONLY))
        {
            uint32 needAlliance = (MinPlayers < qAlliance) ? 0 : MinPlayers - qAlliance;
            uint32 needHorde = (MinPlayers < qHorde) ? 0 : MinPlayers - qHorde;
            ChatHandler(plr).PSendSysMessage(LANG_BG_QUEUE_ANNOUNCE_SELF,
                                             bgName, q_min_level, q_max_level, qAlliance, needAlliance, qHorde, needHorde);
        }
        // System message
        else
        {
            sWorld.SendWorldText(LANG_BG_QUEUE_ANNOUNCE_WORLD,
                                 bgName, q_min_level, q_max_level, qAlliance, MaxPlayers, qHorde, MaxPlayers);
        }
    }
}

void BattlegroundQueue::RemovePlayer(uint64 guid, bool decreaseInvitedCount)
{
    Player* plr = sObjectMgr.GetPlayer(guid);

    int32 queue_id = 0;                                     // signed for proper for-loop finish
    QueuedPlayersMap::iterator itr;
    GroupQueueInfo* group;
    QueuedGroupsList::iterator group_itr;
    bool IsSet = false;
    if (plr)
    {
        queue_id = plr->GetBattlegroundQueueIdFromLevel();

        itr = m_QueuedPlayers[queue_id].find(guid);
        if (itr != m_QueuedPlayers[queue_id].end())
            IsSet = true;
    }

    if (!IsSet)
    {
        // either player is offline, or he levelled up to another queue category
        // sLog.outError("Battleground: removing offline player from BG queue - this might not happen, but it should not cause crash");
        for (uint32 i = 0; i < MAX_BATTLEGROUND_QUEUES; i++)
        {
            itr = m_QueuedPlayers[i].find(guid);
            if (itr != m_QueuedPlayers[i].end())
            {
                queue_id = i;
                IsSet = true;
                break;
            }
        }
    }

    // couldn't find the player in bg queue, return
    if (!IsSet)
    {
        sLog.outError("Battleground: couldn't find player to remove.");
        return;
    }

    group = itr->second.GroupInfo;

    for (group_itr = m_QueuedGroups[queue_id].begin(); group_itr != m_QueuedGroups[queue_id].end(); ++group_itr)
    {
        if (group == (GroupQueueInfo*)(*group_itr))
            break;
    }

    // variables are set (what about leveling up when in queue????)
    // remove player from group
    // if only player there, remove group

    // remove player queue info from group queue info
    std::map<uint64, PlayerQueueInfo*>::iterator pitr = group->Players.find(guid);

    if (pitr != group->Players.end())
        group->Players.erase(pitr);

    // check for iterator correctness
    if (group_itr != m_QueuedGroups[queue_id].end() && itr != m_QueuedPlayers[queue_id].end())
    {
        // used when player left the queue, NOT used when porting to bg
        if (decreaseInvitedCount)
        {
            // if invited to bg, and should decrease invited count, then do it
            if (group->IsInvitedToBGInstanceGUID)
            {
                Battleground* bg = sBattlegroundMgr.GetBattleground(group->IsInvitedToBGInstanceGUID);
                if (bg)
                    bg->DecreaseInvitedCount(group->Team);
                if (bg && !bg->GetPlayersSize() && !bg->GetInvitedCount(ALLIANCE) && !bg->GetInvitedCount(HORDE))
                {
                    // no more players on battleground, set delete it
                    bg->SetDeleteThis();
                }
            }
            // update the join queue, maybe now the player's group fits in a queue!
            // not yet implemented (should store bgTypeId in group queue info?)
        }
        //if player leaves queue and he is invited to rated arena match, then he has to loose
        if (group->IsInvitedToBGInstanceGUID && group->IsRated && decreaseInvitedCount)
        {
            ArenaTeam* at = sObjectMgr.GetArenaTeamById(group->ArenaTeamId);
            if (at)
            {
                sLog.outDebug("UPDATING memberLost's personal arena rating for %u by opponents rating: %u", GUID_LOPART(guid), group->OpponentsTeamRating);
                Player* plr = sObjectMgr.GetPlayer(guid);
                if (plr)
                    at->MemberLost(plr, group->OpponentsTeamRating);
                else
                    at->OfflineMemberLost(guid, group->OpponentsTeamRating);
                at->SaveToDB();
            }
        }
        // remove player queue info
        m_QueuedPlayers[queue_id].erase(itr);
        // remove group queue info if needed
        if (group->Players.empty())
        {
            m_QueuedGroups[queue_id].erase(group_itr);
            delete group;
        }
        // NEEDS TESTING!
        // group wasn't empty, so it wasn't deleted, and player have left a rated queue -> everyone from the group should leave too
        // don't remove recursively if already invited to bg!
        else if (!group->IsInvitedToBGInstanceGUID && decreaseInvitedCount && group->IsRated)
        {
            // remove next player, this is recursive
            // first send removal information
            if (Player* plr2 = sObjectMgr.GetPlayer(group->Players.begin()->first))
            {
                Battleground* bg = sBattlegroundMgr.GetBattlegroundTemplate(group->BgTypeId);
                uint32 bgQueueTypeId = sBattlegroundMgr.BGQueueTypeId(group->BgTypeId, group->ArenaType);
                uint32 queueSlot = plr2->GetBattlegroundQueueIndex(bgQueueTypeId);
                plr2->RemoveBattlegroundQueueId(bgQueueTypeId); // must be called this way, because if you move this call to queue->removeplayer, it causes bugs
                WorldPacket data;
                sBattlegroundMgr.BuildBattlegroundStatusPacket(&data, bg, plr2->GetTeam(), queueSlot, STATUS_NONE, 0, 0);
                plr2->GetSession()->SendPacket(&data);
            }
            // then actually delete, this may delete the group as well!
            RemovePlayer(group->Players.begin()->first, decreaseInvitedCount);
        }
    }
}

bool BattlegroundQueue::InviteGroupToBG(GroupQueueInfo* ginfo, Battleground* bg, uint32 side)
{
    // set side if needed
    if (side)
        ginfo->Team = side;

    if (!ginfo->IsInvitedToBGInstanceGUID)
    {
        // not yet invited
        // set invitation
        ginfo->IsInvitedToBGInstanceGUID = bg->GetInstanceID();
        uint32 bgQueueTypeId = sBattlegroundMgr.BGQueueTypeId(bg->GetTypeID(), bg->GetArenaType());
        // loop through the players
        for (std::map<uint64, PlayerQueueInfo*>::iterator itr = ginfo->Players.begin(); itr != ginfo->Players.end(); ++itr)
        {
            // set status
            itr->second->InviteTime = getMSTime();
            itr->second->LastInviteTime = getMSTime();

            // get the player
            Player* plr = sObjectMgr.GetPlayer(itr->first, true);
            // if offline, skip him
            if (!plr)
                continue;

            // invite the player
            sBattlegroundMgr.InvitePlayer(plr, bg->GetInstanceID(), ginfo->Team);

            WorldPacket data;

            uint32 queueSlot = plr->GetBattlegroundQueueIndex(bgQueueTypeId);

            sLog.outDebug("Battleground: invited plr %s (%u) to BG instance %u queueindex %u bgtype %u, I can't help it if they don't press the enter battle button.", plr->GetName(), plr->GetGUIDLow(), bg->GetInstanceID(), queueSlot, bg->GetTypeID());

            // send status packet
            sBattlegroundMgr.BuildBattlegroundStatusPacket(&data, bg, side ? side : plr->GetTeam(), queueSlot, STATUS_WAIT_JOIN, INVITE_ACCEPT_WAIT_TIME, 0);
            plr->GetSession()->SendPacket(&data);
        }
        return true;
    }

    return false;
}

// used to recursively select groups from eligible groups
bool BattlegroundQueue::SelectionPool::Build(uint32 MinPlayers, uint32 MaxPlayers, EligibleGroups::iterator startitr)
{
    // start from the specified start iterator
    for (EligibleGroups::iterator itr1 = startitr; itr1 != m_CurrEligGroups->end(); ++itr1)
    {
        // if it fits in, select it
        if (GetPlayerCount() + (*itr1)->Players.size() <= MaxPlayers)
        {
            EligibleGroups::iterator next = itr1;
            ++next;
            AddGroup((*itr1));
            if (GetPlayerCount() >= MinPlayers)
            {
                // enough players are selected
                return true;
            }
            // try building from the rest of the elig. groups
            // if that succeeds, return true
            if (Build(MinPlayers, MaxPlayers, next))
                return true;
            // the rest didn't succeed, so this group cannot be included
            RemoveGroup((*itr1));
        }
    }
    // build didn't succeed
    return false;
}

// this function is responsible for the selection of queued groups when trying to create new battlegrounds
bool BattlegroundQueue::BuildSelectionPool(uint32 bgTypeId, uint32 queue_id, uint32 MinPlayers, uint32 MaxPlayers,  SelectionPoolBuildMode mode, uint8 ArenaType, bool isRated, uint32 MinRating, uint32 MaxRating, uint32 DisregardTime, uint32 excludeTeam)
{
    uint32 side;
    switch (mode)
    {
    case NORMAL_ALLIANCE:
    case ONESIDE_ALLIANCE_TEAM1:
    case ONESIDE_ALLIANCE_TEAM2:
        side = ALLIANCE;
        break;
    case NORMAL_HORDE:
    case ONESIDE_HORDE_TEAM1:
    case ONESIDE_HORDE_TEAM2:
        side = HORDE;
        break;
    default:
        //unknown mode, return false
        sLog.outDebug("Battleground: unknown selection pool build mode, returning...");
        return false;
        break;
    }

    // initiate the groups eligible to create the bg
    m_EligibleGroups.Init(&(m_QueuedGroups[queue_id]), bgTypeId, side, MaxPlayers, ArenaType, isRated, MinRating, MaxRating, DisregardTime, excludeTeam);
    // init the selected groups (clear)
    // and set m_CurrEligGroups pointer
    // we set it this way to only have one EligibleGroups object to save some memory
    m_SelectionPools[mode].Init(&m_EligibleGroups);
    // build succeeded
    if (m_SelectionPools[mode].Build(MinPlayers, MaxPlayers, m_EligibleGroups.begin()))
    {
        // the selection pool is set, return
        sLog.outDebug("Battleground-debug: pool build succeeded, return true");
        sLog.outDebug("Battleground-debug: Player size for mode %u is %u", mode, m_SelectionPools[mode].GetPlayerCount());
        for (std::list<GroupQueueInfo* >::iterator itr = m_SelectionPools[mode].SelectedGroups.begin(); itr != m_SelectionPools[mode].SelectedGroups.end(); ++itr)
        {
            sLog.outDebug("Battleground-debug: queued group in selection with %lu players", (*itr)->Players.size());
            for (std::map<uint64, PlayerQueueInfo* >::iterator itr2 = (*itr)->Players.begin(); itr2 != (*itr)->Players.end(); ++itr2)
                sLog.outDebug("Battleground-debug:    player in above group GUID %u", (uint32)(itr2->first));
        }
        return true;
    }
    // failed to build a selection pool matching the given values
    return false;
}

// used to remove the Enter Battle window if the battle has already, but someone still has it
// (this can happen in arenas mainly, since the preparation is shorter than the timer for the bgqueueremove event
void BattlegroundQueue::BGEndedRemoveInvites(Battleground* bg)
{
    uint32 queue_id = bg->GetQueueType();
    uint32 bgInstanceId = bg->GetInstanceID();
    uint32 bgQueueTypeId = sBattlegroundMgr.BGQueueTypeId(bg->GetTypeID(), bg->GetArenaType());
    QueuedGroupsList::iterator itr, next;
    for (itr = m_QueuedGroups[queue_id].begin(); itr != m_QueuedGroups[queue_id].end(); itr = next)
    {
        // must do this way, because the groupinfo will be deleted when all playerinfos are removed
        GroupQueueInfo* ginfo = (*itr);
        next = itr;
        ++next;
        // if group was invited to this bg instance, then remove all references
        if (ginfo->IsInvitedToBGInstanceGUID == bgInstanceId)
        {
            // after removing this much playerinfos, the ginfo will be deleted, so we'll use a for loop
            uint32 to_remove = ginfo->Players.size();
            uint32 team = ginfo->Team;
            for (uint32 i = 0; i < to_remove; ++i)
            {
                // always remove the first one in the group
                std::map<uint64, PlayerQueueInfo* >::iterator itr2 = ginfo->Players.begin();
                if (itr2 == ginfo->Players.end())
                {
                    sLog.outError("Empty Players in ginfo, this should never happen!");
                    return;
                }

                // get the player
                Player* plr = sObjectMgr.GetPlayer(itr2->first);
                if (!plr)
                {
                    sLog.outError("Player offline when trying to remove from GroupQueueInfo, this should never happen.");
                    continue;
                }

                // get the queueslot
                uint32 queueSlot = plr->GetBattlegroundQueueIndex(bgQueueTypeId);
                if (queueSlot < PLAYER_MAX_BATTLEGROUND_QUEUES) // player is in queue
                {
                    plr->RemoveBattlegroundQueueId(bgQueueTypeId);
                    // remove player from queue, this might delete the ginfo as well! don't use that pointer after this!
                    RemovePlayer(itr2->first, true);
                    // this is probably unneeded, since this player was already invited -> does not fit when initing eligible groups
                    // but updating the queue can't hurt
                    Update(bgQueueTypeId, bg->GetQueueType());
                    // send info to client
                    WorldPacket data;
                    sBattlegroundMgr.BuildBattlegroundStatusPacket(&data, bg, team, queueSlot, STATUS_NONE, 0, 0);
                    plr->GetSession()->SendPacket(&data);
                }
            }
        }
    }
}

/*
this method is called when group is inserted, or player / group is removed from BG Queue - there is only one player's status changed, so we don't use while (true) cycles to invite whole queue
it must be called after fully adding the members of a group to ensure group joining
should be called after removeplayer functions in some cases
*/
void BattlegroundQueue::Update(uint32 bgTypeId, uint32 queue_id, uint8 arenatype, bool isRated, uint32 arenaRating)
{
    if (queue_id >= MAX_BATTLEGROUND_QUEUES)
    {
        //this is error, that caused crashes (not in , but now it shouldn't)
        sLog.outError("BattlegroundQueue::Update() called for invalid queue type - this can cause crash, pls report problem, if this is the last line of error log before crash");
        return;
    }

    //if no players in queue ... do nothing
    if (m_QueuedGroups[queue_id].empty())
        return;

    //battleground with free slot for player should be always the last in this queue
    BGFreeSlotQueueType::iterator itr, next;
    for (itr = sBattlegroundMgr.BGFreeSlotQueue[bgTypeId].begin(); itr != sBattlegroundMgr.BGFreeSlotQueue[bgTypeId].end(); itr = next)
    {
        next = itr;
        ++next;
        // battleground is running, so if:
        // DO NOT allow queue manager to invite new player to running arena
        if ((*itr)->isBattleground() && (*itr)->GetTypeID() == bgTypeId && (*itr)->GetQueueType() == queue_id && (*itr)->GetStatus() > STATUS_WAIT_QUEUE && (*itr)->GetStatus() < STATUS_WAIT_LEAVE)
        {
            //we must check both teams
            Battleground* bg = *itr; //we have to store battleground pointer here, because when battleground is full, it is removed from free queue (not yet implemented!!)
            // and iterator is invalid

            for (QueuedGroupsList::iterator itr = m_QueuedGroups[queue_id].begin(); itr != m_QueuedGroups[queue_id].end(); ++itr)
            {
                // did the group join for this bg type?
                if ((*itr)->BgTypeId != bgTypeId)
                    continue;
                // if so, check if fits in
                if (bg->GetFreeSlotsForTeam((*itr)->Team) >= (*itr)->Players.size())
                {
                    // if group fits in, invite it
                    InviteGroupToBG((*itr), bg, (*itr)->Team);
                }
            }

            if (!bg->HasFreeSlots())
            {
                //remove BG from BGFreeSlotQueue
                bg->RemoveFromBGFreeSlotQueue();
            }
        }
    }

    // finished iterating through the bgs with free slots, maybe we need to create a new bg

    Battleground* bg_template = sBattlegroundMgr.GetBattlegroundTemplate(bgTypeId);
    if (!bg_template)
    {
        sLog.outError("Battleground: Update: bg template not found for %u", bgTypeId);
        return;
    }

    // get the min. players per team, properly for larger arenas as well. (must have full teams for arena matches!)
    uint32 MinPlayersPerTeam = bg_template->GetMinPlayersPerTeam();
    uint32 MaxPlayersPerTeam = bg_template->GetMaxPlayersPerTeam();
    if (bg_template->isArena())
    {
        if (sBattlegroundMgr.isArenaTesting())
        {
            MaxPlayersPerTeam = 1;
            MinPlayersPerTeam = 1;
        }
        else
        {
            switch (arenatype)
            {
            case ARENA_TYPE_2v2:
                MaxPlayersPerTeam = 2;
                MinPlayersPerTeam = 2;
                break;
            case ARENA_TYPE_3v3:
                MaxPlayersPerTeam = 3;
                MinPlayersPerTeam = 3;
                break;
            case ARENA_TYPE_5v5:
                MaxPlayersPerTeam = 5;
                MinPlayersPerTeam = 5;
                break;
            }
        }
    }
    else
    {
        if (sBattlegroundMgr.isTesting())
            MinPlayersPerTeam = 1;
    }

    // found out the minimum and maximum ratings the newly added team should battle against
    // arenaRating is the rating of the latest joined team
    uint32 arenaMinRating = (arenaRating <= sBattlegroundMgr.GetMaxRatingDifference()) ? 0 : arenaRating - sBattlegroundMgr.GetMaxRatingDifference();
    // if no rating is specified, set maxrating to 0
    uint32 arenaMaxRating = (arenaRating == 0) ? 0 : arenaRating + sBattlegroundMgr.GetMaxRatingDifference();
    uint32 discardTime = 0;
    // if max rating difference is set and the time past since server startup is greater than the rating discard time
    // (after what time the ratings aren't taken into account when making teams) then
    // the discard time is current_time - time_to_discard, teams that joined after that, will have their ratings taken into account
    // else leave the discard time on 0, this way all ratings will be discarded
    if (sBattlegroundMgr.GetMaxRatingDifference() && getMSTime() >= sBattlegroundMgr.GetRatingDiscardTimer())
        discardTime = getMSTime() - sBattlegroundMgr.GetRatingDiscardTimer();

    // try to build the selection pools
    bool bAllyOK = BuildSelectionPool(bgTypeId, queue_id, MinPlayersPerTeam, MaxPlayersPerTeam, NORMAL_ALLIANCE, arenatype, isRated, arenaMinRating, arenaMaxRating, discardTime);
    if (bAllyOK)
        sLog.outDebug("Battleground: ally pool successfully built");
    else
        sLog.outDebug("Battleground: ally pool wasn't created");
    bool bHordeOK = BuildSelectionPool(bgTypeId, queue_id, MinPlayersPerTeam, MaxPlayersPerTeam, NORMAL_HORDE, arenatype, isRated, arenaMinRating, arenaMaxRating, discardTime);
    if (bHordeOK)
        sLog.outDebug("Battleground: horde pool successfully built");
    else
        sLog.outDebug("Battleground: horde pool wasn't created");

    // if selection pools are ready, create the new bg
    if ((bAllyOK && bHordeOK) || (sBattlegroundMgr.isTesting() && (bAllyOK || bHordeOK)))
    {
        Battleground* bg2 = 0;
        // special handling for arenas
        if (bg_template->isArena())
        {
            // Find a random arena, that can be created
            uint8 arenas[] = {BATTLEGROUND_NA, BATTLEGROUND_BE, BATTLEGROUND_RL};
            uint32 arena_num = urand(0, 2);
            if (!(bg2 = sBattlegroundMgr.CreateNewBattleground(arenas[arena_num % 3], arenatype, isRated)) &&
                !(bg2 = sBattlegroundMgr.CreateNewBattleground(arenas[(arena_num + 1) % 3], arenatype, isRated)) &&
                !(bg2 = sBattlegroundMgr.CreateNewBattleground(arenas[(arena_num + 2) % 3], arenatype, isRated)))
            {
                sLog.outError("Battleground: couldn't create any arena instance!");
                return;
            }

            // set the MaxPlayersPerTeam values based on arenatype
            // setting the min player values isn't needed, since we won't be using that value later on.
            if (sBattlegroundMgr.isArenaTesting())
            {
                bg2->SetMaxPlayersPerTeam(1);
                bg2->SetMaxPlayers(2);
            }
            else
            {
                switch (arenatype)
                {
                case ARENA_TYPE_2v2:
                    bg2->SetMaxPlayersPerTeam(2);
                    bg2->SetMaxPlayers(4);
                    break;
                case ARENA_TYPE_3v3:
                    bg2->SetMaxPlayersPerTeam(3);
                    bg2->SetMaxPlayers(6);
                    break;
                case ARENA_TYPE_5v5:
                    bg2->SetMaxPlayersPerTeam(5);
                    bg2->SetMaxPlayers(10);
                    break;
                default:
                    break;
                }
            }
        }
        else
        {
            // create new battleground
            bg2 = sBattlegroundMgr.CreateNewBattleground(bgTypeId, arenatype, false);
        }

        if (!bg2)
        {
            sLog.outError("Battleground: couldn't create bg %u", bgTypeId);
            return;
        }

        bg2->SetQueueType(queue_id);

        std::list<GroupQueueInfo* >::iterator itr;

        // Send amount of invites based on the difference between the sizes of the two faction's queues
        uint32 QUEUED_HORDE = m_SelectionPools[NORMAL_HORDE].SelectedGroups.size();
        uint32 QUEUED_ALLIANCE = m_SelectionPools[NORMAL_ALLIANCE].SelectedGroups.size();
        uint16 maxbginvites = 0;

        if (QUEUED_ALLIANCE <= QUEUED_HORDE)
            maxbginvites = QUEUED_ALLIANCE;
        else
            maxbginvites = QUEUED_HORDE;

        // invite groups from horde selection pool
        uint16 invitecounter = 0;
        for (itr = m_SelectionPools[NORMAL_HORDE].SelectedGroups.begin(); itr != m_SelectionPools[NORMAL_HORDE].SelectedGroups.end(); ++itr)
        {
            if (invitecounter >= maxbginvites)
                return;
            InviteGroupToBG((*itr), bg2, HORDE);
            ++invitecounter;
        }

        // invite groups from ally selection pool
        invitecounter = 0;
        for (itr = m_SelectionPools[NORMAL_ALLIANCE].SelectedGroups.begin(); itr != m_SelectionPools[NORMAL_ALLIANCE].SelectedGroups.end(); ++itr)
        {
            if (invitecounter >= maxbginvites)
                return;
            InviteGroupToBG((*itr), bg2, ALLIANCE);
            ++invitecounter;
        }

        if (isRated)
        {
            std::list<GroupQueueInfo* >::iterator itr_alliance = m_SelectionPools[NORMAL_ALLIANCE].SelectedGroups.begin();
            std::list<GroupQueueInfo* >::iterator itr_horde = m_SelectionPools[NORMAL_HORDE].SelectedGroups.begin();
            (*itr_alliance)->OpponentsTeamRating = (*itr_horde)->ArenaTeamRating;
            sLog.outDebug("setting opposite team rating for team %u to %u", (*itr_alliance)->ArenaTeamId, (*itr_alliance)->OpponentsTeamRating);
            (*itr_horde)->OpponentsTeamRating = (*itr_alliance)->ArenaTeamRating;
            sLog.outDebug("setting opposite team rating for team %u to %u", (*itr_horde)->ArenaTeamId, (*itr_horde)->OpponentsTeamRating);
        }

        // start the battleground
        bg2->StartBattleground();
    }

    // there weren't enough players for a "normal" match
    // if arena, enable horde versus horde or alliance versus alliance teams here

    else if (bg_template->isArena())
    {
        bool bOneSideHordeTeam1 = false, bOneSideHordeTeam2 = false;
        bool bOneSideAllyTeam1 = false, bOneSideAllyTeam2 = false;
        bOneSideHordeTeam1 = BuildSelectionPool(bgTypeId, queue_id, MaxPlayersPerTeam, MaxPlayersPerTeam, ONESIDE_HORDE_TEAM1, arenatype, isRated, arenaMinRating, arenaMaxRating, discardTime);
        if (bOneSideHordeTeam1)
        {
            // one team has been selected, find out if other can be selected too
            std::list<GroupQueueInfo* >::iterator itr;
            // temporarily change the team side to enable building the next pool excluding the already selected groups
            for (itr = m_SelectionPools[ONESIDE_HORDE_TEAM1].SelectedGroups.begin(); itr != m_SelectionPools[ONESIDE_HORDE_TEAM1].SelectedGroups.end(); ++itr)
                (*itr)->Team = ALLIANCE;

            bOneSideHordeTeam2 = BuildSelectionPool(bgTypeId, queue_id, MaxPlayersPerTeam, MaxPlayersPerTeam, ONESIDE_HORDE_TEAM2, arenatype, isRated, arenaMinRating, arenaMaxRating, discardTime, (*(m_SelectionPools[ONESIDE_HORDE_TEAM1].SelectedGroups.begin()))->ArenaTeamId);

            // change back the team to horde
            for (itr = m_SelectionPools[ONESIDE_HORDE_TEAM1].SelectedGroups.begin(); itr != m_SelectionPools[ONESIDE_HORDE_TEAM1].SelectedGroups.end(); ++itr)
                (*itr)->Team = HORDE;

            if (!bOneSideHordeTeam2)
                bOneSideHordeTeam1 = false;
        }
        if (!bOneSideHordeTeam1)
        {
            // check for one sided ally
            bOneSideAllyTeam1 = BuildSelectionPool(bgTypeId, queue_id, MaxPlayersPerTeam, MaxPlayersPerTeam, ONESIDE_ALLIANCE_TEAM1, arenatype, isRated, arenaMinRating, arenaMaxRating, discardTime);
            if (bOneSideAllyTeam1)
            {
                // one team has been selected, find out if other can be selected too
                std::list<GroupQueueInfo* >::iterator itr;
                // temporarily change the team side to enable building the next pool excluding the already selected groups
                for (itr = m_SelectionPools[ONESIDE_ALLIANCE_TEAM1].SelectedGroups.begin(); itr != m_SelectionPools[ONESIDE_ALLIANCE_TEAM1].SelectedGroups.end(); ++itr)
                    (*itr)->Team = HORDE;

                bOneSideAllyTeam2 = BuildSelectionPool(bgTypeId, queue_id, MaxPlayersPerTeam, MaxPlayersPerTeam, ONESIDE_ALLIANCE_TEAM2, arenatype, isRated, arenaMinRating, arenaMaxRating, discardTime, (*(m_SelectionPools[ONESIDE_ALLIANCE_TEAM1].SelectedGroups.begin()))->ArenaTeamId);

                // change back the team to ally
                for (itr = m_SelectionPools[ONESIDE_ALLIANCE_TEAM1].SelectedGroups.begin(); itr != m_SelectionPools[ONESIDE_ALLIANCE_TEAM1].SelectedGroups.end(); ++itr)
                    (*itr)->Team = ALLIANCE;
            }

            if (!bOneSideAllyTeam2)
                bOneSideAllyTeam1 = false;
        }
        // 1-sided BuildSelectionPool() will work, because the MinPlayersPerTeam == MaxPlayersPerTeam in every arena!!!!
        if ((bOneSideHordeTeam1 && bOneSideHordeTeam2) ||
            (bOneSideAllyTeam1 && bOneSideAllyTeam2))
        {
            // which side has enough players?
            SelectionPoolBuildMode mode1, mode2;
            // find out what pools are we using
            if (bOneSideAllyTeam1 && bOneSideAllyTeam2)
            {
                mode1 = ONESIDE_ALLIANCE_TEAM1;
                mode2 = ONESIDE_ALLIANCE_TEAM2;
            }
            else
            {
                mode1 = ONESIDE_HORDE_TEAM1;
                mode2 = ONESIDE_HORDE_TEAM2;
            }

            // create random arena
            uint8 arenas[] = {BATTLEGROUND_NA, BATTLEGROUND_BE, BATTLEGROUND_RL};
            uint32 arena_num = urand(0, 2);
            Battleground* bg2 = NULL;
            if (!(bg2 = sBattlegroundMgr.CreateNewBattleground(arenas[arena_num % 3], arenatype, isRated)) &&
                !(bg2 = sBattlegroundMgr.CreateNewBattleground(arenas[(arena_num + 1) % 3], arenatype, isRated)) &&
                !(bg2 = sBattlegroundMgr.CreateNewBattleground(arenas[(arena_num + 2) % 3], arenatype, isRated)))
            {
                sLog.outError("Could not create arena.");
                return;
            }

            sLog.outDebug("Battleground: One-faction arena created.");
            // init stats
            if (sBattlegroundMgr.isArenaTesting())
            {
                bg2->SetMaxPlayersPerTeam(1);
                bg2->SetMaxPlayers(2);
            }
            else
            {
                switch (arenatype)
                {
                case ARENA_TYPE_2v2:
                    bg2->SetMaxPlayersPerTeam(2);
                    bg2->SetMaxPlayers(4);
                    break;
                case ARENA_TYPE_3v3:
                    bg2->SetMaxPlayersPerTeam(3);
                    bg2->SetMaxPlayers(6);
                    break;
                case ARENA_TYPE_5v5:
                    bg2->SetMaxPlayersPerTeam(5);
                    bg2->SetMaxPlayers(10);
                    break;
                default:
                    break;
                }
            }

            bg2->SetQueueType(queue_id);

            std::list<GroupQueueInfo* >::iterator itr;

            // invite players from the first group as horde players (actually green team)
            for (itr = m_SelectionPools[mode1].SelectedGroups.begin(); itr != m_SelectionPools[mode1].SelectedGroups.end(); ++itr)
                InviteGroupToBG((*itr), bg2, HORDE);

            // invite players from the second group as ally players (actually gold team)
            for (itr = m_SelectionPools[mode2].SelectedGroups.begin(); itr != m_SelectionPools[mode2].SelectedGroups.end(); ++itr)
                InviteGroupToBG((*itr), bg2, ALLIANCE);

            if (isRated)
            {
                std::list<GroupQueueInfo* >::iterator itr_alliance = m_SelectionPools[mode1].SelectedGroups.begin();
                std::list<GroupQueueInfo* >::iterator itr_horde = m_SelectionPools[mode2].SelectedGroups.begin();
                (*itr_alliance)->OpponentsTeamRating = (*itr_horde)->ArenaTeamRating;
                (*itr_horde)->OpponentsTeamRating = (*itr_alliance)->ArenaTeamRating;
            }

            bg2->StartBattleground();
        }
    }
}

/*********************************************************/
/***           BATTLEGROUND QUEUE EVENTS              ***/
/*********************************************************/

bool BGQueueInviteEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    Player* plr = sObjectMgr.GetPlayer(m_PlayerGuid);

    // player logged off (we should do nothing, he is correctly removed from queue in another procedure)
    if (!plr)
        return true;

    // Player can be in another BG queue and must be removed in normal way in any case
    // // player is already in battleground ... do nothing (battleground queue status is deleted when player is teleported to BG)
    // if (plr->GetBattlegroundId() > 0)
    //    return true;

    Battleground* bg = sBattlegroundMgr.GetBattleground(m_BgInstanceGUID);
    if (!bg)
        return true;

    uint32 queueSlot = plr->GetBattlegroundQueueIndex(bg->GetTypeID());
    if (queueSlot < PLAYER_MAX_BATTLEGROUND_QUEUES)         // player is in queue
    {
        uint32 bgQueueTypeId = sBattlegroundMgr.BGQueueTypeId(bg->GetTypeID(), bg->GetArenaType());
        uint32 queueSlot = plr->GetBattlegroundQueueIndex(bgQueueTypeId);
        if (queueSlot < PLAYER_MAX_BATTLEGROUND_QUEUES) // player is in queue
        {
            // check if player is invited to this bg ... this check must be here, because when player leaves queue and joins another, it would cause a problems
            BattlegroundQueue::QueuedPlayersMap const& qpMap = sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].m_QueuedPlayers[plr->GetBattlegroundQueueIdFromLevel()];
            BattlegroundQueue::QueuedPlayersMap::const_iterator qItr = qpMap.find(m_PlayerGuid);
            if (qItr != qpMap.end() && qItr->second.GroupInfo->IsInvitedToBGInstanceGUID == m_BgInstanceGUID)
            {
                WorldPacket data;
                sBattlegroundMgr.BuildBattlegroundStatusPacket(&data, bg, qItr->second.GroupInfo->Team, queueSlot, STATUS_WAIT_JOIN, INVITE_ACCEPT_WAIT_TIME / 2, 0);
                plr->GetSession()->SendPacket(&data);
            }
        }
    }
    return true;                                            //event will be deleted
}

void BGQueueInviteEvent::Abort(uint64 /*e_time*/)
{
    //this should not be called
    sLog.outError("Battleground invite event ABORTED!");
}

bool BGQueueRemoveEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    Player* plr = sObjectMgr.GetPlayer(m_PlayerGuid);
    if (!plr)
        // player logged off (we should do nothing, he is correctly removed from queue in another procedure)
        return true;

    Battleground* bg = sBattlegroundMgr.GetBattleground(m_BgInstanceGUID);
    if (!bg)
        return true;

    sLog.outDebug("Battleground: removing player %u from bg queue for instance %u because of not pressing enter battle in time.", plr->GetGUIDLow(), m_BgInstanceGUID);

    uint32 bgQueueTypeId = sBattlegroundMgr.BGQueueTypeId(bg->GetTypeID(), bg->GetArenaType());
    uint32 queueSlot = plr->GetBattlegroundQueueIndex(bgQueueTypeId);
    if (queueSlot < PLAYER_MAX_BATTLEGROUND_QUEUES) // player is in queue
    {
        // check if player is invited to this bg ... this check must be here, because when player leaves queue and joins another, it would cause a problems
        BattlegroundQueue::QueuedPlayersMap::iterator qMapItr = sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].m_QueuedPlayers[plr->GetBattlegroundQueueIdFromLevel()].find(m_PlayerGuid);
        if (qMapItr != sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].m_QueuedPlayers[plr->GetBattlegroundQueueIdFromLevel()].end() && qMapItr->second.GroupInfo && qMapItr->second.GroupInfo->IsInvitedToBGInstanceGUID == m_BgInstanceGUID)
        {
            plr->RemoveBattlegroundQueueId(bgQueueTypeId);
            sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].RemovePlayer(m_PlayerGuid, true);
            sBattlegroundMgr.m_BattlegroundQueues[bgQueueTypeId].Update(bgQueueTypeId, bg->GetQueueType());
            WorldPacket data;
            sBattlegroundMgr.BuildBattlegroundStatusPacket(&data, bg, m_PlayersTeam, queueSlot, STATUS_NONE, 0, 0);
            plr->GetSession()->SendPacket(&data);
        }
    }
    else
        sLog.outDebug("Battleground: Player was already removed from queue");

    //event will be deleted
    return true;
}

void BGQueueRemoveEvent::Abort(uint64 /*e_time*/)
{
    //this should not be called
    sLog.outError("Battleground remove event ABORTED!");
}

/*********************************************************/
/***           BATTLEGROUND MANAGER                   ***/
/*********************************************************/

BattlegroundMgr::BattlegroundMgr()
{
    m_Battlegrounds.clear();
    m_AutoDistributePoints = (bool)sWorld.getConfig(CONFIG_ARENA_AUTO_DISTRIBUTE_POINTS);
    m_MaxRatingDifference = sWorld.getConfig(CONFIG_ARENA_MAX_RATING_DIFFERENCE);
    m_RatingDiscardTimer = sWorld.getConfig(CONFIG_ARENA_RATING_DISCARD_TIMER);
    m_PrematureFinishTimer = sWorld.getConfig(CONFIG_BATTLEGROUND_PREMATURE_FINISH_TIMER);
    m_NextRatingDiscardUpdate = m_RatingDiscardTimer;
    m_AutoDistributionTimeChecker = 0;
    m_ArenaTesting = false;
    m_Testing = false;
}

BattlegroundMgr::~BattlegroundMgr()
{
    DeleteAlllBattlegrounds();
}

void BattlegroundMgr::DeleteAlllBattlegrounds()
{
    for (BattlegroundSet::iterator itr = m_Battlegrounds.begin(); itr != m_Battlegrounds.end();)
    {
        Battleground* bg = itr->second;
        m_Battlegrounds.erase(itr++);
        delete bg;
    }

    // destroy template battlegrounds that listed only in queues (other already terminated)
    for (uint32 bgTypeId = 0; bgTypeId <= MAX_BATTLEGROUND_TYPE_ID; ++bgTypeId)
    {
        // ~Battleground call unregistring BG from queue
        while (!BGFreeSlotQueue[bgTypeId].empty())
            delete BGFreeSlotQueue[bgTypeId].front();
    }
}

// used to update running battlegrounds, and delete finished ones
void BattlegroundMgr::Update(uint32 diff)
{
    BattlegroundSet::iterator itr, next;
    for (itr = m_Battlegrounds.begin(); itr != m_Battlegrounds.end(); itr = next)
    {
        next = itr;
        ++next;
        itr->second->Update(diff);
        // use the SetDeleteThis variable
        // direct deletion caused crashes
        if (itr->second->m_SetDeleteThis)
        {
            Battleground* bg = itr->second;
            m_Battlegrounds.erase(itr);
            delete bg;
        }
    }
    // if rating difference counts, maybe force-update queues
    if (m_MaxRatingDifference)
    {
        // it's time to force update
        if (m_NextRatingDiscardUpdate < diff)
        {
            // forced update for level 70 rated arenas
            m_BattlegroundQueues[BATTLEGROUND_QUEUE_2v2].Update(BATTLEGROUND_AA, 6, ARENA_TYPE_2v2, true, 0);
            m_BattlegroundQueues[BATTLEGROUND_QUEUE_3v3].Update(BATTLEGROUND_AA, 6, ARENA_TYPE_3v3, true, 0);
            m_BattlegroundQueues[BATTLEGROUND_QUEUE_5v5].Update(BATTLEGROUND_AA, 6, ARENA_TYPE_5v5, true, 0);
            m_NextRatingDiscardUpdate = m_RatingDiscardTimer;
        }
        else
            m_NextRatingDiscardUpdate -= diff;
    }
    if (m_AutoDistributePoints)
    {
        if (m_AutoDistributionTimeChecker < diff)
        {
            if (uint32(time(NULL)) > m_NextAutoDistributionTime)
            {
                DistributeArenaPoints();
                m_NextAutoDistributionTime = time(NULL) + BATTLEGROUND_ARENA_POINT_DISTRIBUTION_DAY * sWorld.getConfig(CONFIG_ARENA_AUTO_DISTRIBUTE_INTERVAL_DAYS);
                CharacterDatabase.PExecute("UPDATE saved_variables SET NextArenaPointDistributionTime = '" UI64FMTD "'", m_NextAutoDistributionTime);
            }
            m_AutoDistributionTimeChecker = 600000; // check 10 minutes
        }
        else
            m_AutoDistributionTimeChecker -= diff;
    }
}

void BattlegroundMgr::BuildBattlegroundStatusPacket(WorldPacket* data, Battleground* bg, uint32 /*team*/, uint8 QueueSlot, uint8 StatusID, uint32 Time1, uint32 Time2, uint32 arenatype, uint8 israted)
{
    // we can be in 3 queues in same time...
    if (StatusID == 0)
    {
        data->Initialize(SMSG_BATTLEFIELD_STATUS, 4 * 3);
        *data << uint32(QueueSlot);                         // queue id (0...2)
        *data << uint64(0);
        return;
    }

    data->Initialize(SMSG_BATTLEFIELD_STATUS, (4 + 1 + 1 + 4 + 2 + 4 + 1 + 4 + 4 + 4));
    *data << uint32(QueueSlot);                             // queue id (0...2) - player can be in 3 queues in time
    // The following segment is read as uint64 in client but can be appended as their original type.
    *data << uint8(arenatype ? arenatype : bg->GetArenaType());
    *data << uint8(bg->isArena() ? 0x0D : 0x2);
    *data << uint32(bg->GetTypeID());
    *data << uint16(0x1F90);
    // End of uint64 segment, decomposed this way for simplicity
    *data << uint32(0);                                   // unknown
    // alliance/horde for BG and skirmish/rated for Arenas
    // following displays the minimap-icon 0 = faction icon 1 = arenaicon
    *data << uint8(israted ? israted : bg->isRated());                              // 1 for rated match, 0 for bg or non rated match
    /*   *data << uint8(arenatype ? arenatype : bg->GetArenaType());                     // team type (0=BG, 2=2x2, 3=3x3, 5=5x5), for arenas    // NOT PROPER VALUE IF ARENA ISN'T RUNNING YET!!!!
        switch(bg->GetTypeID())                                 // value depends on bg id
        {
            case BATTLEGROUND_AV:
                *data << uint8(1);
                break;
            case BATTLEGROUND_WS:
                *data << uint8(2);
                break;
            case BATTLEGROUND_AB:
                *data << uint8(3);
                break;
            case BATTLEGROUND_NA:
                *data << uint8(4);
                break;
            case BATTLEGROUND_BE:
                *data << uint8(5);
                break;
            case BATTLEGROUND_AA:
                *data << uint8(6);
                break;
            case BATTLEGROUND_EY:
                *data << uint8(7);
                break;
            case BATTLEGROUND_RL:
                *data << uint8(8);
                break;
            default:                                            // unknown
                *data << uint8(0);
                break;
        }

        if (bg->isArena() && (StatusID == STATUS_WAIT_QUEUE))
            *data << uint32(BATTLEGROUND_AA);                   // all arenas   I don't think so.
        else
        *data << uint32(bg->GetTypeID());                   // BG id from DBC

        *data << uint16(0x1F90);                                // unk value 8080
        *data << uint32(bg->GetInstanceID());                   // instance id

        if (bg->isBattleground())
            *data << uint8(bg->GetTeamIndexByTeamId(team));     // team
        else
            *data << uint8(israted?israted:bg->isRated());                      // is rated battle
    */
    *data << uint32(StatusID);                              // status
    switch (StatusID)
    {
    case STATUS_WAIT_QUEUE:                             // status_in_queue
        *data << uint32(Time1);                         // average wait time, milliseconds
        *data << uint32(Time2);                         // time in queue, updated every minute?
        break;
    case STATUS_WAIT_JOIN:                              // status_invite
        *data << uint32(bg->GetMapId());                // map id
        *data << uint32(Time1);                         // time to remove from queue, milliseconds
        break;
    case STATUS_IN_PROGRESS:                            // status_in_progress
        *data << uint32(bg->GetMapId());                // map id
        *data << uint32(Time1);                         // 0 at bg start, 120000 after bg end, time to bg auto leave, milliseconds
        *data << uint32(Time2);                         // time from bg start, milliseconds
        *data << uint8(0x1);                            // Lua_GetBattlefieldArenaFaction (bool)
        break;
    default:
        sLog.outError("Unknown BG status!");
        break;
    }
}

void BattlegroundMgr::BuildPvpLogDataPacket(WorldPacket* data, Battleground* bg)
{
    uint8 type = (bg->isArena() ? 1 : 0);
    // last check on 2.4.1
    data->Initialize(MSG_PVP_LOG_DATA, (1 + 1 + 4 + 40 * bg->GetPlayerScoresSize()));
    *data << uint8(type);                                   // seems to be type (battleground=0/arena=1)

    if (type)                                                // arena
    {
        // it seems this must be according to BG_WINNER_A/H and _NOT_ BG_TEAM_A/H
        ArenaTeam* at1 = sObjectMgr.GetArenaTeamById(bg->m_ArenaTeamIds[1]); // Winner
        ArenaTeam* at2 = sObjectMgr.GetArenaTeamById(bg->m_ArenaTeamIds[0]); // Loser

        *data << uint32(at1 ? uint32(at1->GetRating()) : 0);
        *data << uint32(at1 ? uint32(at1->GetRating() + bg->m_ArenaTeamRatingChanges[1]) : 0);
        *data << uint32(at2 ? uint32(at2->GetRating()) : 0);
        *data << uint32(at2 ? uint32(at2->GetRating() + bg->m_ArenaTeamRatingChanges[0]) : 0);
        
        *data << (at1 ? at1->GetName() : "Unknown");
        *data << (at2 ? at2->GetName() : "Unknown");
    }

    if (bg->GetWinner() == 2)
    {
        *data << uint8(0);                                  // bg in progress
    }
    else
    {
        *data << uint8(1);                                  // bg ended
        *data << uint8(bg->GetWinner());                    // who win
    }

    size_t wpos = data->wpos();
    uint32 scoreCount = 0;
    *data << uint32(scoreCount);                            // placeholder

    Battleground::BattlegroundScoreMap::const_iterator itr2 = bg->GetPlayerScoresBegin();
    for (Battleground::BattlegroundScoreMap::const_iterator itr = itr2; itr != bg->GetPlayerScoresEnd();)
    {
        itr2 = itr++;
        if (!bg->IsPlayerInBattleground(itr2->first))
        {
            sLog.outError("Player " UI64FMTD " has scoreboard entry for battleground %u but is not in battleground!", itr->first, bg->GetTypeID());
            continue;
        }

        *data << uint64(itr2->first);
        *data << uint32(itr2->second->KillingBlows);
        if (type == 0)
        {
            *data << uint32(itr2->second->HonorableKills);
            *data << uint32(itr2->second->Deaths);
            *data << uint32(itr2->second->BonusHonor);
        }
        else
        {
            Player* plr = sObjectMgr.GetPlayer(itr2->first);
            uint32 team = bg->GetPlayerTeam(itr2->first);
            if (!team && plr)
                team = plr->GetBGTeam();
            *data << uint8(team == ALLIANCE ? 1 : 0); // green or yellow
        }
        *data << uint32(itr2->second->DamageDone);              // damage done
        *data << uint32(itr2->second->HealingDone);             // healing done
        switch (bg->GetTypeID())                                // battleground specific things
        {
        case BATTLEGROUND_AV:
            *data << uint32(0x00000005);                    // count of next fields
            *data << uint32(((BattlegroundAVScore*)itr2->second)->GraveyardsAssaulted); // GraveyardsAssaulted
            *data << uint32(((BattlegroundAVScore*)itr2->second)->GraveyardsDefended);  // GraveyardsDefended
            *data << uint32(((BattlegroundAVScore*)itr2->second)->TowersAssaulted);     // TowersAssaulted
            *data << uint32(((BattlegroundAVScore*)itr2->second)->TowersDefended);      // TowersDefended
            *data << uint32(((BattlegroundAVScore*)itr2->second)->MinesCaptured);       // MinesCaptured
            break;
        case BATTLEGROUND_WS:
            *data << uint32(0x00000002);                    // count of next fields
            *data << uint32(((BattlegroundWGScore*)itr2->second)->FlagCaptures);        // flag captures
            *data << uint32(((BattlegroundWGScore*)itr2->second)->FlagReturns);         // flag returns
            break;
        case BATTLEGROUND_AB:
            *data << uint32(0x00000002);                    // count of next fields
            *data << uint32(((BattlegroundABScore*)itr2->second)->BasesAssaulted);      // bases asssulted
            *data << uint32(((BattlegroundABScore*)itr2->second)->BasesDefended);       // bases defended
            break;
        case BATTLEGROUND_EY:
            *data << uint32(0x00000001);                    // count of next fields
            *data << uint32(((BattlegroundEYScore*)itr2->second)->FlagCaptures);        // flag captures
            break;
        case BATTLEGROUND_NA:
        case BATTLEGROUND_BE:
        case BATTLEGROUND_AA:
        case BATTLEGROUND_RL:
            *data << uint32(0);
            break;
        default:
            sLog.outDebug("Unhandled MSG_PVP_LOG_DATA for BG id %u", bg->GetTypeID());
            *data << uint32(0);
            break;
        }
        // should never happen
        if (++scoreCount >= bg->GetMaxPlayers() && itr != bg->GetPlayerScoresEnd())
        {
            sLog.outError("Battleground %u scoreboard has more entries (%u) than allowed players in this bg (%u)", bg->GetTypeID(), bg->GetPlayerScoresSize(), bg->GetMaxPlayers());
            break;
        }
    }

    data->put(wpos, scoreCount);
}

void BattlegroundMgr::BuildGroupJoinedBattlegroundPacket(WorldPacket* data, uint32 bgTypeId)
{
    /*bgTypeId is:
    0 - Your group has joined a battleground queue, but you are not eligible
    1 - Your group has joined the queue for AV
    2 - Your group has joined the queue for WS
    3 - Your group has joined the queue for AB
    4 - Your group has joined the queue for NA
    5 - Your group has joined the queue for BE Arena
    6 - Your group has joined the queue for All Arenas
    7 - Your group has joined the queue for EotS*/
    data->Initialize(SMSG_GROUP_JOINED_BATTLEGROUND, 4);
    *data << uint32(bgTypeId);
}

void BattlegroundMgr::BuildUpdateWorldStatePacket(WorldPacket* data, uint32 field, uint32 value)
{
    data->Initialize(SMSG_UPDATE_WORLD_STATE, 4 + 4);
    *data << uint32(field);
    *data << uint32(value);
}

void BattlegroundMgr::BuildPlaySoundPacket(WorldPacket* data, uint32 soundid)
{
    data->Initialize(SMSG_PLAY_SOUND, 4);
    *data << uint32(soundid);
}

void BattlegroundMgr::BuildPlayerLeftBattlegroundPacket(WorldPacket* data, const uint64& guid)
{
    data->Initialize(SMSG_BATTLEGROUND_PLAYER_LEFT, 8);
    *data << uint64(guid);
}

void BattlegroundMgr::BuildPlayerJoinedBattlegroundPacket(WorldPacket* data, Player* plr)
{
    data->Initialize(SMSG_BATTLEGROUND_PLAYER_JOINED, 8);
    *data << uint64(plr->GetGUID());
}

void BattlegroundMgr::InvitePlayer(Player* plr, uint32 bgInstanceGUID, uint32 team)
{
    // set invited player counters:
    Battleground* bg = GetBattleground(bgInstanceGUID);
    if (!bg)
        return;
    bg->IncreaseInvitedCount(team);

    plr->SetInviteForBattlegroundQueueType(BGQueueTypeId(bg->GetTypeID(), bg->GetArenaType()), bgInstanceGUID);

    // set the arena teams for rated matches
    if (bg->isArena() && bg->isRated())
    {
        switch (bg->GetArenaType())
        {
        case ARENA_TYPE_2v2:
            bg->SetArenaTeamIdForTeam(team, plr->GetArenaTeamId(0));
            break;
        case ARENA_TYPE_3v3:
            bg->SetArenaTeamIdForTeam(team, plr->GetArenaTeamId(1));
            break;
        case ARENA_TYPE_5v5:
            bg->SetArenaTeamIdForTeam(team, plr->GetArenaTeamId(2));
            break;
        default:
            break;
        }
    }

    // create invite events:
    //add events to player's counters ---- this is not good way - there should be something like global event processor, where we should add those events
    BGQueueInviteEvent* inviteEvent = new BGQueueInviteEvent(plr->GetGUID(), bgInstanceGUID);
    plr->m_Events.AddEvent(inviteEvent, plr->m_Events.CalculateTime(INVITE_ACCEPT_WAIT_TIME / 2));
    BGQueueRemoveEvent* removeEvent = new BGQueueRemoveEvent(plr->GetGUID(), bgInstanceGUID, team);
    plr->m_Events.AddEvent(removeEvent, plr->m_Events.CalculateTime(INVITE_ACCEPT_WAIT_TIME));
}

Battleground* BattlegroundMgr::GetBattlegroundTemplate(uint32 bgTypeId)
{
    return BGFreeSlotQueue[bgTypeId].empty() ? NULL : BGFreeSlotQueue[bgTypeId].back();
}

// create a new battleground that will really be used to play
Battleground* BattlegroundMgr::CreateNewBattleground(uint32 bgTypeId, uint8 arenaType, bool isRated)
{
    // get the template BG
    Battleground* bg_template = GetBattlegroundTemplate(bgTypeId);

    if (!bg_template)
    {
        sLog.outError("Battleground: CreateNewBattleground - bg template not found for %u", bgTypeId);
        return 0;
    }

    Battleground* bg = NULL;

    // create a copy of the BG template
    switch (bgTypeId)
    {
    case BATTLEGROUND_AV:
        bg = new BattlegroundAV(*(BattlegroundAV*)bg_template);
        break;
    case BATTLEGROUND_WS:
        bg = new BattlegroundWS(*(BattlegroundWS*)bg_template);
        break;
    case BATTLEGROUND_AB:
        bg = new BattlegroundAB(*(BattlegroundAB*)bg_template);
        break;
    case BATTLEGROUND_NA:
        bg = new BattlegroundNA(*(BattlegroundNA*)bg_template);
        break;
    case BATTLEGROUND_BE:
        bg = new BattlegroundBE(*(BattlegroundBE*)bg_template);
        break;
    case BATTLEGROUND_AA:
        bg = new BattlegroundAA(*(BattlegroundAA*)bg_template);
        break;
    case BATTLEGROUND_EY:
        bg = new BattlegroundEY(*(BattlegroundEY*)bg_template);
        break;
    case BATTLEGROUND_RL:
        bg = new BattlegroundRL(*(BattlegroundRL*)bg_template);
        break;
    default:
        //bg = new Battleground;
        return 0;
        break;             // placeholder for non implemented BG
    }

    // generate a new instance id
    bg->SetInstanceID(MapManager::Instance().GenerateInstanceId()); // set instance id

    // reset the new bg (set status to status_wait_queue from status_none)
    bg->Reset();

    /*  will be setup in BG::Update() when the first player is ported in
    if (!(bg->SetupBattleground()))
    {
        sLog.outError("Battleground: CreateNewBattleground: SetupBattleground failed for bg %u", bgTypeId);
        delete bg;
        return 0;
    }
    */

    // add BG to free slot queue
    bg->AddToBGFreeSlotQueue();

    bg->SetStatus(STATUS_WAIT_JOIN);
    bg->SetArenaType(arenaType);
    bg->SetRated(isRated);
    bg->SetTypeID(bgTypeId);

    // add bg to update list
    AddBattleground(bg->GetInstanceID(), bg);

    return bg;
}

// used to create the BG templates
uint32 BattlegroundMgr::CreateBattleground(uint32 bgTypeId, uint32 MinPlayersPerTeam, uint32 MaxPlayersPerTeam, uint32 LevelMin, uint32 LevelMax, char* BattlegroundName, uint32 MapID, float Team1StartLocX, float Team1StartLocY, float Team1StartLocZ, float Team1StartLocO, float Team2StartLocX, float Team2StartLocY, float Team2StartLocZ, float Team2StartLocO, float StartMaxDist)
{
    // Create the BG
    Battleground* bg = NULL;

    switch (bgTypeId)
    {
    case BATTLEGROUND_AV:
        bg = new BattlegroundAV;
        break;
    case BATTLEGROUND_WS:
        bg = new BattlegroundWS;
        break;
    case BATTLEGROUND_AB:
        bg = new BattlegroundAB;
        break;
    case BATTLEGROUND_NA:
        bg = new BattlegroundNA;
        break;
    case BATTLEGROUND_BE:
        bg = new BattlegroundBE;
        break;
    case BATTLEGROUND_AA:
        bg = new BattlegroundAA;
        break;
    case BATTLEGROUND_EY:
        bg = new BattlegroundEY;
        break;
    case BATTLEGROUND_RL:
        bg = new BattlegroundRL;
        break;
    default:
        bg = new Battleground;
        break;                           // placeholder for non implemented BG
    }

    bg->SetMapId(MapID);

    bg->Reset();

    BattlemasterListEntry const* bl = sBattlemasterListStore.LookupEntry(bgTypeId);
    //in previous method is checked if exists entry in sBattlemasterListStore, so no check needed
    if (bl)
        bg->SetArenaorBGType(bl->type == TYPE_ARENA);

    bg->SetTypeID(bgTypeId);
    bg->SetInstanceID(0);                               // template bg, instance id is 0
    bg->SetMinPlayersPerTeam(MinPlayersPerTeam);
    bg->SetMaxPlayersPerTeam(MaxPlayersPerTeam);
    bg->SetMinPlayers(MinPlayersPerTeam * 2);
    bg->SetMaxPlayers(MaxPlayersPerTeam * 2);
    bg->SetName(BattlegroundName);
    bg->SetTeamStartLoc(ALLIANCE, Team1StartLocX, Team1StartLocY, Team1StartLocZ, Team1StartLocO);
    bg->SetTeamStartLoc(HORDE,    Team2StartLocX, Team2StartLocY, Team2StartLocZ, Team2StartLocO);
    bg->SetStartMaxDist(StartMaxDist);
    bg->SetLevelRange(LevelMin, LevelMax);

    //add Battleground instance to FreeSlotQueue (.back() will return the template!)
    bg->AddToBGFreeSlotQueue();

    // do NOT add to update list, since this is a template battleground!

    // return some not-null value, bgTypeId is good enough for me
    return bgTypeId;
}

void BattlegroundMgr::CreateInitialBattlegrounds()
{
    float AStartLoc[4];
    float HStartLoc[4];
    float StartMaxDist;
    uint32 MaxPlayersPerTeam, MinPlayersPerTeam, MinLvl, MaxLvl, start1, start2;
    BattlemasterListEntry const* bl;
    WorldSafeLocsEntry const* start;

    uint32 count = 0;

    //                                                       0   1                 2                 3      4      5                6              7             8           9
    QueryResult_AutoPtr result = WorldDatabase.Query("SELECT id, MinPlayersPerTeam,MaxPlayersPerTeam,MinLvl,MaxLvl,AllianceStartLoc,AllianceStartO,HordeStartLoc,HordeStartO,StartMaxDist FROM battleground_template");

    if (!result)
    {


        sLog.outErrorDb(">> Loaded 0 battlegrounds. DB table battleground_template is empty.");
        return;
    }


    do
    {
        Field* fields = result->Fetch();

        uint32 bgTypeID = fields[0].GetUInt32();
        if (sDisableMgr.IsDisabledFor(DISABLE_TYPE_BATTLEGROUND, bgTypeID, NULL))
            continue;

        // can be overwritten by values from DB
        bl = sBattlemasterListStore.LookupEntry(bgTypeID);
        if (!bl)
        {
            sLog.outError("Battleground ID %u not found in BattlemasterList.dbc. Battleground not created.", bgTypeID);
            continue;
        }

        MaxPlayersPerTeam = bl->maxplayersperteam;
        MinPlayersPerTeam = bl->maxplayersperteam / 2;
        MinLvl = bl->minlvl;
        MaxLvl = bl->maxlvl;

        if (fields[1].GetUInt32())
            MinPlayersPerTeam = fields[1].GetUInt32();

        if (fields[2].GetUInt32())
            MaxPlayersPerTeam = fields[2].GetUInt32();

        if (fields[3].GetUInt32())
            MinLvl = fields[3].GetUInt32();

        if (fields[4].GetUInt32())
            MaxLvl = fields[4].GetUInt32();

        start1 = fields[5].GetUInt32();

        start = sWorldSafeLocsStore.LookupEntry(start1);
        if (start)
        {
            AStartLoc[0] = start->x;
            AStartLoc[1] = start->y;
            AStartLoc[2] = start->z;
            AStartLoc[3] = fields[6].GetFloat();
        }
        else if (bgTypeID == BATTLEGROUND_AA)
        {
            AStartLoc[0] = 0;
            AStartLoc[1] = 0;
            AStartLoc[2] = 0;
            AStartLoc[3] = fields[6].GetFloat();
        }
        else
        {
            sLog.outErrorDb("Table battleground_template for id %u has invalid WorldSafeLocs.dbc id %u in field AllianceStartLoc. BG not created.", bgTypeID, start1);
            continue;
        }

        start2 = fields[7].GetUInt32();

        start = sWorldSafeLocsStore.LookupEntry(start2);
        if (start)
        {
            HStartLoc[0] = start->x;
            HStartLoc[1] = start->y;
            HStartLoc[2] = start->z;
            HStartLoc[3] = fields[8].GetFloat();
        }
        else if (bgTypeID == BATTLEGROUND_AA)
        {
            HStartLoc[0] = 0;
            HStartLoc[1] = 0;
            HStartLoc[2] = 0;
            HStartLoc[3] = fields[8].GetFloat();
        }
        else
        {
            sLog.outErrorDb("Table battleground_template for id %u has invalid WorldSafeLocs.dbc id %u in field HordeStartLoc. BG not created.", bgTypeID, start2);
            continue;
        }

        StartMaxDist = fields[9].GetFloat();

        //sLog.outDetail("Creating battleground %s, %u-%u", bl->name[sWorld.GetDBClang()], MinLvl, MaxLvl);
        if (!CreateBattleground(bgTypeID, MinPlayersPerTeam, MaxPlayersPerTeam, MinLvl, MaxLvl, bl->name[sWorld.GetDefaultDbcLocale()], bl->mapid[0], AStartLoc[0], AStartLoc[1], AStartLoc[2], AStartLoc[3], HStartLoc[0], HStartLoc[1], HStartLoc[2], HStartLoc[3], StartMaxDist))
            continue;

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u battlegrounds", count);
}

void BattlegroundMgr::InitAutomaticArenaPointDistribution()
{
    if (m_AutoDistributePoints)
    {
        sLog.outDebug("Initializing Automatic Arena Point Distribution");
        QueryResult_AutoPtr result = CharacterDatabase.Query("SELECT NextArenaPointDistributionTime FROM saved_variables");
        if (!result)
        {
            sLog.outDebug("Battleground: Next arena point distribution time not found in SavedVariables, reseting it now.");
            m_NextAutoDistributionTime = time(NULL) + BATTLEGROUND_ARENA_POINT_DISTRIBUTION_DAY * sWorld.getConfig(CONFIG_ARENA_AUTO_DISTRIBUTE_INTERVAL_DAYS);
            CharacterDatabase.PExecute("INSERT INTO saved_variables (NextArenaPointDistributionTime) VALUES ('" UI64FMTD "')", m_NextAutoDistributionTime);
        }
        else
            m_NextAutoDistributionTime = (*result)[0].GetUInt64();

        sLog.outDebug("Automatic Arena Point Distribution initialized.");
    }
}

void BattlegroundMgr::DistributeArenaPoints()
{
    // used to distribute arena points based on last week's stats
    sWorld.SendGlobalText("Flushing Arena points based on team ratings, this may take a few minutes. Please stand by...", NULL);

    sWorld.SendGlobalText("Distributing arena points to players...", NULL);

    //temporary structure for storing maximum points to add values for all players
    std::map<uint32, uint32> PlayerPoints;

    //at first update all points for all team members
    for (ObjectMgr::ArenaTeamMap::iterator team_itr = sObjectMgr.GetArenaTeamMapBegin(); team_itr != sObjectMgr.GetArenaTeamMapEnd(); ++team_itr)
    {
        if (ArenaTeam* at = team_itr->second)
            at->UpdateArenaPointsHelper(PlayerPoints);
    }

    //cycle that gives points to all players
    for (std::map<uint32, uint32>::iterator plr_itr = PlayerPoints.begin(); plr_itr != PlayerPoints.end(); ++plr_itr)
    {
        //add points if player is online
        if (Player* player = HashMapHolder<Player>::Find(plr_itr->first))
            player->ModifyArenaPoints(plr_itr->second, true);
        else
            CharacterDatabase.PExecute("UPDATE characters SET arenaPoints = arenaPoints + '%u' WHERE guid = '%u'", plr_itr->second, plr_itr->first);
    }

    PlayerPoints.clear();

    sWorld.SendGlobalText("Finished setting arena points for online players.", NULL);

    sWorld.SendGlobalText("Modifying played count, arena points etc. for loaded arena teams, sending updated stats to online players...", NULL);
    for (ObjectMgr::ArenaTeamMap::iterator titr = sObjectMgr.GetArenaTeamMapBegin(); titr != sObjectMgr.GetArenaTeamMapEnd(); ++titr)
    {
        if (ArenaTeam* at = titr->second)
        {
            at->FinishWeek();                              // set played this week etc values to 0 in memory, too
            at->SaveToDB();                                // save changes
            at->NotifyStatsChanged();                      // notify the players of the changes
        }
    }

    sWorld.SendGlobalText("Modification done.", NULL);

    sWorld.SendGlobalText("Done flushing Arena points.", NULL);
}

void BattlegroundMgr::BuildBattlegroundListPacket(WorldPacket* data, uint64 guid, Player* plr, uint32 bgTypeId)
{
    uint32 PlayerLevel = 10;

    if (plr)
        PlayerLevel = plr->getLevel();

    data->Initialize(SMSG_BATTLEFIELD_LIST);
    *data << uint64(guid);                                  // battlemaster guid
    *data << uint32(bgTypeId);                              // battleground id
    if (bgTypeId == BATTLEGROUND_AA)                         // arena
    {
        *data << uint8(5);                                  // unk
        *data << uint32(0);                                 // unk
    }
    else                                                    // battleground
    {
        *data << uint8(0x00);                               // unk

        size_t count_pos = data->wpos();
        uint32 count = 0;
        *data << uint32(0x00);                              // number of bg instances

        for (std::map<uint32, Battleground*>::iterator itr = m_Battlegrounds.begin(); itr != m_Battlegrounds.end(); ++itr)
        {
            if (itr->second->GetTypeID() == bgTypeId && (PlayerLevel >= itr->second->GetMinLevel()) && (PlayerLevel <= itr->second->GetMaxLevel()))
            {
                *data << uint32(itr->second->GetInstanceID());
                ++count;
            }
        }
        data->put<uint32>(count_pos , count);
    }
}

void BattlegroundMgr::SendToBattleground(Player* pl, uint32 instanceId)
{
    Battleground* bg = GetBattleground(instanceId);
    if (bg)
    {
        uint32 mapid = bg->GetMapId();
        float x, y, z, O;
        uint32 team = pl->GetBGTeam();
        if (team == 0)
            team = pl->GetTeam();
        bg->GetTeamStartLoc(team, x, y, z, O);

        sLog.outDetail("BATTLEGROUND: Sending %s to map %u, X %f, Y %f, Z %f, O %f", pl->GetName(), mapid, x, y, z, O);
        pl->TeleportTo(mapid, x, y, z, O);
    }
    else
        sLog.outError("player %u trying to port to non-existent bg instance %u", pl->GetGUIDLow(), instanceId);
}

void BattlegroundMgr::SendAreaSpiritHealerQueryOpcode(Player* pl, Battleground* bg, uint64 guid)
{
    WorldPacket data(SMSG_AREA_SPIRIT_HEALER_TIME, 12);
    uint32 time_ = 30000 - bg->GetLastResurrectTime();      // resurrect every 30 seconds
    if (time_ == uint32(-1))
        time_ = 0;
    data << guid << time_;
    pl->GetSession()->SendPacket(&data);
}

void BattlegroundMgr::RemoveBattleground(uint32 instanceID)
{
    BattlegroundSet::iterator itr = m_Battlegrounds.find(instanceID);
    if (itr != m_Battlegrounds.end())
        m_Battlegrounds.erase(itr);
}

bool BattlegroundMgr::IsArenaType(uint32 bgTypeId) const
{
    return (bgTypeId == BATTLEGROUND_AA ||
            bgTypeId == BATTLEGROUND_BE ||
            bgTypeId == BATTLEGROUND_NA ||
            bgTypeId == BATTLEGROUND_RL);
}

bool BattlegroundMgr::IsBattlegroundType(uint32 bgTypeId) const
{
    return !IsArenaType(bgTypeId);
}

uint32 BattlegroundMgr::BGQueueTypeId(uint32 bgTypeId, uint8 arenaType)
{
    switch (bgTypeId)
    {
    case BATTLEGROUND_WS:
        return BATTLEGROUND_QUEUE_WS;
    case BATTLEGROUND_AB:
        return BATTLEGROUND_QUEUE_AB;
    case BATTLEGROUND_AV:
        return BATTLEGROUND_QUEUE_AV;
    case BATTLEGROUND_EY:
        return BATTLEGROUND_QUEUE_EY;
    case BATTLEGROUND_AA:
    case BATTLEGROUND_NA:
    case BATTLEGROUND_RL:
    case BATTLEGROUND_BE:
        switch (arenaType)
        {
        case ARENA_TYPE_2v2:
            return BATTLEGROUND_QUEUE_2v2;
        case ARENA_TYPE_3v3:
            return BATTLEGROUND_QUEUE_3v3;
        case ARENA_TYPE_5v5:
            return BATTLEGROUND_QUEUE_5v5;
        default:
            return 0;
        }
    default:
        return 0;
    }
}

uint32 BattlegroundMgr::BGTemplateId(uint32 bgQueueTypeId) const
{
    switch (bgQueueTypeId)
    {
    case BATTLEGROUND_QUEUE_WS:
        return BATTLEGROUND_WS;
    case BATTLEGROUND_QUEUE_AB:
        return BATTLEGROUND_AB;
    case BATTLEGROUND_QUEUE_AV:
        return BATTLEGROUND_AV;
    case BATTLEGROUND_QUEUE_EY:
        return BATTLEGROUND_EY;
    case BATTLEGROUND_QUEUE_2v2:
    case BATTLEGROUND_QUEUE_3v3:
    case BATTLEGROUND_QUEUE_5v5:
        return BATTLEGROUND_AA;
    default:
        return 0;
    }
}

uint8 BattlegroundMgr::BGArenaType(uint32 bgQueueTypeId) const
{
    switch (bgQueueTypeId)
    {
    case BATTLEGROUND_QUEUE_2v2:
        return ARENA_TYPE_2v2;
    case BATTLEGROUND_QUEUE_3v3:
        return ARENA_TYPE_3v3;
    case BATTLEGROUND_QUEUE_5v5:
        return ARENA_TYPE_5v5;
    default:
        return 0;
    }
}

void BattlegroundMgr::ToggleTesting()
{
    m_Testing = !m_Testing;
    if (m_Testing)
        sWorld.SendGlobalText("Battlegrounds are set to 1v0 for debugging.", NULL);
    else
        sWorld.SendGlobalText("Battlegrounds are set to normal playercount.", NULL);
}

void BattlegroundMgr::ToggleArenaTesting()
{
    m_ArenaTesting = !m_ArenaTesting;
    if (m_ArenaTesting)
        sWorld.SendGlobalText("Arenas are set to 1v1 for debugging. So, don't join as group.", NULL);
    else
        sWorld.SendGlobalText("Arenas are set to normal playercount.", NULL);
}

void BattlegroundMgr::SetHolidayWeekends(uint32 mask)
{
    for (uint32 bgtype = 1; bgtype <= 8; ++bgtype)
    {
        if (Battleground* bg = GetBattlegroundTemplate(bgtype))
            bg->SetHoliday(mask & (1 << bgtype));
    }
}

