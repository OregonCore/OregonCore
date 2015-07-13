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
#include "World.h"
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
void WaypointMovementGenerator<Creature>::Interrupt(Creature& creature)
{
    creature.InterruptMoving();
    creature.ClearUnitState(UNIT_STATE_ROAMING);
    creature.SetWalk(!creature.HasUnitState(UNIT_STATE_RUNNING_STATE));
}

template<>
void WaypointMovementGenerator<Player>::Interrupt(Player& /*unit*/) {}

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

    if (unit.canFly())
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
        else
        {
            //Determine waittime
            if (node->delay)
                i_nextMoveTime.Reset(node->delay);

            //note: disable "start" for mtmap
            if (node->event_id && urand(0,99) < node->event_chance)
                unit.GetMap()->ScriptsStart(sWaypointScripts, node->event_id, &unit, NULL/*, false*/);

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
    player.getHostileRefManager().setOnlineOfflineState(false);
    player.AddUnitState(UNIT_STATE_IN_FLIGHT);
    player.SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_TAXI_FLIGHT);

    Movement::MoveSplineInit init(player);
    // For preloading end grid
    InitEndGridInfo();
    uint32 pathEndPoint = GetPathAtMapEnd();
    for (uint32 i = GetCurrentNode(); i != pathEndPoint; ++i)
    {
        G3D::Vector3 vertice((*i_path)[i].x, (*i_path)[i].y, (*i_path)[i].z);
        init.Path().push_back(vertice);
    }
    init.SetFirstPointId(GetCurrentNode());
    init.SetFly();
    init.SetVelocity(32.0f);
    init.Launch();
}

void FlightPathMovementGenerator::Finalize(Player& player)
{
    // remove flag to prevent send object build movement packets for flight state and crash (movement generator already not at top of stack)
    player.ClearUnitState(UNIT_FLAG_DISABLE_MOVE | UNIT_STATE_IN_FLIGHT);

    player.Dismount();

    if (player.m_taxi.empty())
    {
        player.getHostileRefManager().setOnlineOfflineState(true);
        if (player.pvpInfo.inHostileArea)
            player.CastSpell(&player, 2479, true);

        player.StopMoving(true);
    }
}

void FlightPathMovementGenerator::Interrupt(Player& player)
{
    player.ClearUnitState(UNIT_STATE_IN_FLIGHT);
}

bool FlightPathMovementGenerator::Update(Player& player, const uint32& diff)
{
    uint32 pointId = (uint32)player.movespline->currentPathIdx();
    if (pointId > i_currentNode)
    {
        ++i_currentNode;
        // check if it's time to preload the flightmaster grid at path end
        if (i_currentNode == m_preloadTargetNode)
            PreloadEndGrid();
    }

    // we have arrived at the end of the path
    return i_currentNode < (i_path->Size() - 1);
}

void FlightPathMovementGenerator::SetCurrentNodeAfterTeleport()
{
    if (i_path->empty())
        return;

    uint32 map0 = (*i_path)[0].mapid;
    for (uint32 i = 1; i < i_path->size(); ++i)
    {
        if ((*i_path)[i].mapid != map0)
        {
            i_currentNode = i;
            return;
        }
    }
}

void FlightPathMovementGenerator::InitEndGridInfo()
{
    // Storage to preload flightmaster grid at end of flight. For multi-stop flights, this will
    // be reinitialized for each flightmaster at the end of each spline (or stop) in the flight.

    uint32 nodeCount = (*i_path).size();        // Get the number of nodes in the path.
    m_endMapId = (*i_path)[nodeCount - 1].mapid; // Get the map ID from the last node
    m_preloadTargetNode = nodeCount - 3;        // 2 nodes before the final node, we pre-load the grid
    m_endGridX = (*i_path)[nodeCount - 1].x;    // Get the X position from the last node
    m_endGridY = (*i_path)[nodeCount - 1].y;    // Get the Y position from the last node
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
