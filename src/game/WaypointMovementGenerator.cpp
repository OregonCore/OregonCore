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

#include "WaypointMovementGenerator.h"
#include "ObjectMgr.h"
#include "MapManager.h" // for flightmaster grid preloading
#include "Creature.h"
#include "CreatureAI.h"
#include "CreatureGroups.h"
#include "Player.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"

template<class T>
void WaypointMovementGenerator<T>::Initialize(T& /*u*/) {}

template<>
void WaypointMovementGenerator<Creature>::Finalize(Creature& u)
{
    u.ClearUnitState(UNIT_STATE_ROAMING);
    u.SetWalk(!u.HasUnitState(UNIT_STATE_RUNNING_STATE));
}

template<>
void WaypointMovementGenerator<Player>::Finalize(Player& /*u*/) {}

template<class T>
void WaypointMovementGenerator<T>::MovementInform(T& /*unit*/) {}

template<>
void WaypointMovementGenerator<Creature>::MovementInform(Creature& unit)
{
    unit.AI()->MovementInform(WAYPOINT_MOTION_TYPE, i_currentNode);
}

template<>
void WaypointMovementGenerator<Creature>::Reset(Creature& /*unit*/)
{
    StopedByPlayer = true;
    i_nextMoveTime.Reset(0);
}

template<>
void WaypointMovementGenerator<Player>::Reset(Player& /*unit*/) {}

template<>
void
WaypointMovementGenerator<Creature>::MoveToNextNode(Creature& unit, const WaypointData& node)
{
    Movement::MoveSplineInit init(unit);
    init.MoveTo(node.x, node.y, node.z, true);

    if (node.orientation != 100 && node.delay != 0)
        init.SetFacing(node.orientation);

    if (!node.run)
        init.SetWalk(true);
    else
        init.SetWalk(false);

    if (unit.CanFly())
        init.SetFly();

    int32 traveltime = init.Launch();

    i_nextMoveTime.Reset(traveltime);
}

template<>
void WaypointMovementGenerator<Creature>::InitTraveller(Creature& unit)
{
    unit.SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
    unit.SetUInt32Value(UNIT_FIELD_BYTES_1, 0);

    unit.AddUnitState(UNIT_STATE_ROAMING);
}

template<>
void WaypointMovementGenerator<Creature>::Initialize(Creature& u)
{
    u.StopMoving();
    //i_currentNode = -1; // uint32, become 0 in the first update
    //i_nextMoveTime.Reset(0);
    StopedByPlayer = false;
    if (!path_id)
        path_id = u.GetWaypointPath();
    waypoints = sWaypointMgr->GetPath(path_id);
    i_currentNode = 0;
    if (waypoints && waypoints->size())
    {
        node = waypoints->front();
        InitTraveller(u);
        MoveToNextNode(u, *node);

        //Call for creature group update
        if (u.GetFormation() && u.GetFormation()->getLeader() == &u)
            u.GetFormation()->LeaderMoveTo(node->x, node->y, node->z);
    }
    else
        node = NULL;
}

template<>
void WaypointMovementGenerator<Player>::InitTraveller(Player& /*unit*/) {}

template<class T>
bool
WaypointMovementGenerator<T>::Update(T& /*unit*/, const uint32& /*diff*/)
{
    return false;
}

template<>
bool WaypointMovementGenerator<Creature>::Update(Creature& unit, const uint32& diff)
{
    if (!path_id)
        return false;

    // Waypoint movement can be switched on/off
    // This is quite handy for escort quests and other stuff
    if (unit.HasUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED | UNIT_STATE_DISTRACTED))
        return true;

    // Clear the generator if the path doesn't exist
    if (!waypoints || !waypoints->size())
        return false;

    i_nextMoveTime.Update(diff);

    if (i_nextMoveTime.GetExpiry() < TIMEDIFF_NEXT_WP)
    {
        if (unit.IsStopped())
        {
            if (StopedByPlayer)
            {
                ASSERT(node);
                InitTraveller(unit);
                MoveToNextNode(unit, *node);
                StopedByPlayer = false;
                return true;
            }

            if (i_currentNode == waypoints->size() - 1) // If that's our last waypoint
            {
                if (repeating)         // If the movement is repeating
                    i_currentNode = 0; // Start moving all over again
                else
                {
                    unit.SetHomePosition(node->x, node->y, node->z, unit.GetOrientation());
                    unit.GetMotionMaster()->Initialize();
                    return false; // Clear the waypoint movement
                }
            }
            else
                ++i_currentNode;

            node = waypoints->at(i_currentNode);
            InitTraveller(unit);
            MoveToNextNode(unit, *node);

            //Call for creature group update
            if (unit.GetFormation() && unit.GetFormation()->getLeader() == &unit)
                unit.GetFormation()->LeaderMoveTo(node->x, node->y, node->z);
        }
        else if (i_nextMoveTime.Passed()) //don't begin delay until movement is actually finished
        {
            // Determine wait time
            if (node->delay)
                i_nextMoveTime.Reset(node->delay);

            if (node->event_id && urand(0, 99) < node->event_chance)
            {
                sLog.outDebug("Creature movement start script %u at point %u for " UI64FMTD ".", node->event_id, i_currentNode, unit.GetGUID());
                unit.GetMap()->ScriptsStart(sWaypointScripts, node->event_id, &unit, NULL);
            }

            MovementInform(unit);
            unit.UpdateWaypointID(i_currentNode);
            unit.ClearUnitState(UNIT_STATE_ROAMING);
        }
    }
    else
    {
        if (unit.IsStopped() && !unit.movespline->Finalized())
        {
            if (!StopedByPlayer)
            {
                i_nextMoveTime.Reset(STOP_TIME_FOR_PLAYER);
                StopedByPlayer = true;
            }
        }
    }
    return true;
}

template void WaypointMovementGenerator<Player>::Initialize(Player&);
template bool WaypointMovementGenerator<Player>::Update(Player&, const uint32&);
template void WaypointMovementGenerator<Player>::MovementInform(Player&);

//----------------------------------------------------//
uint32 FlightPathMovementGenerator::GetPathAtMapEnd() const
{
    if (i_currentNode >= i_path->size())
        return i_path->size();

    uint32 curMapId = (*i_path)[i_currentNode].mapid;
    for (uint32 i = i_currentNode; i < i_path->size(); ++i)
    {
        if ((*i_path)[i].mapid != curMapId)
            return i;
    }

    return i_path->size();
}

void FlightPathMovementGenerator::Initialize(Player& player)
{
    Reset(player);

    // For preloading end grid
    InitEndGridInfo();
}

void FlightPathMovementGenerator::Finalize(Player& player)
{
    // remove flag to prevent send object build movement packets for flight state and crash (movement generator already not at top of stack)
    player.ClearUnitState(UNIT_STATE_IN_FLIGHT);

    player.Dismount();
    player.RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL | UNIT_FLAG_TAXI_FLIGHT);
    player.SetClientControl(&player, 1);

    if (player.m_taxi.empty())
    {
        player.getHostileRefManager().setOnlineOfflineState(true);
        if (player.pvpInfo.inHostileArea)
            player.CastSpell(&player, 2479, true);

        // update z position to ground and orientation for landing point
        // this prevent cheating with landing  point at lags
        // when client side flight end early in comparison server side
        player.StopMoving();
    }
}

#define PLAYER_FLIGHT_SPEED        32.0f

void FlightPathMovementGenerator::Reset(Player& player)
{
    player.getHostileRefManager().setOnlineOfflineState(false);
    player.AddUnitState(UNIT_STATE_IN_FLIGHT);
    player.SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_REMOVE_CLIENT_CONTROL | UNIT_FLAG_TAXI_FLIGHT);
    player.SetClientControl(&player, 0);

    Movement::MoveSplineInit init(player);
    uint32 end = GetPathAtMapEnd();
    for (uint32 i = GetCurrentNode(); i != end; ++i)
    {
        G3D::Vector3 vertice((*i_path)[i].x, (*i_path)[i].y, (*i_path)[i].z);
        init.Path().push_back(vertice);
    }
    init.SetFirstPointId(GetCurrentNode());
    init.SetFly();
    init.SetVelocity(PLAYER_FLIGHT_SPEED);
    init.Launch();
}

bool FlightPathMovementGenerator::Update(Player& player, const uint32& diff)
{
    uint32 pointId = (uint32)player.movespline->currentPathIdx();
    if (pointId > i_currentNode)
    {
        bool departureEvent = true;
        do
        {
            DoEventIfAny(player, (*i_path)[i_currentNode], departureEvent);
            if (pointId == i_currentNode)
                break;
            if (i_currentNode == m_preloadTargetNode)
                PreloadEndGrid();
            i_currentNode += (uint32)departureEvent;
            departureEvent = !departureEvent;
        }
        while (true);
    }

    const bool flying = (i_currentNode < (i_path->size() - 1));

    // Multi-map flight paths
    if (flying && (*i_path)[i_currentNode + 1].mapid != player.GetMapId())
    {
        // short preparations to continue flight
        SetCurrentNodeAfterTeleport();
        TaxiPathNodeEntry const& node = (*i_path)[i_currentNode];
        SkipCurrentNode();
        player.TeleportTo(node.mapid, node.x, node.y, node.z, player.GetOrientation());
    }

    return flying;
}

void FlightPathMovementGenerator::SetCurrentNodeAfterTeleport()
{
    if (i_path->empty())
        return;

    uint32 map0 = (*i_path)[0].mapid;
    for (size_t i = 1; i < i_path->size(); ++i)
    {
        if ((*i_path)[i].mapid != map0)
        {
            i_currentNode = i;
            return;
        }
    }
}

void FlightPathMovementGenerator::DoEventIfAny(Player& player, TaxiPathNodeEntry const& node, bool departure)
{
    if (uint32 eventid = departure ? node.departureEventID : node.arrivalEventID)
    {
        sLog.outDebug("Taxi %s event %u of node %u of path %u for player %s", departure ? "departure" : "arrival", eventid, node.index, node.path, player.GetName());
        player.GetMap()->ScriptsStart(sEventScripts, eventid, &player, &player);
    }
}

bool FlightPathMovementGenerator::GetResetPosition(Player&, float& x, float& y, float& z, float& o) const
{
    const TaxiPathNodeEntry& node = (*i_path)[i_currentNode];
    x = node.x; y = node.y; z = node.z;
    return true;
}

void FlightPathMovementGenerator::InitEndGridInfo()
{
    /*! Storage to preload flightmaster grid at end of flight. For multi-stop flights, this will
       be reinitialized for each flightmaster at the end of each spline (or stop) in the flight. */
    uint32 nodeCount = (*i_path).size();         //! Number of nodes in path.
    m_endMapId = (*i_path)[nodeCount - 1].mapid; //! MapId of last node
    m_preloadTargetNode = nodeCount - 3;
    m_endGridX = (*i_path)[nodeCount - 1].x;
    m_endGridY = (*i_path)[nodeCount - 1].y;
}

void FlightPathMovementGenerator::PreloadEndGrid()
{
    // used to preload the final grid where the flightmaster is
    Map* endMap = MapManager::Instance().FindMap(m_endMapId);

    // Load the grid
    if (endMap)
    {
        sLog.outDetail("Preloading flightmaster at grid (%f, %f) for map %u", m_endGridX, m_endGridY, m_endMapId);
        endMap->LoadGrid(m_endGridX, m_endGridY);
    }
    else
        sLog.outDetail("Unable to determine map to preload flightmaster grid");
}
