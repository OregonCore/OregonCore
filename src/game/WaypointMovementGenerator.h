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

#ifndef OREGON_WAYPOINTMOVEMENTGENERATOR_H
#define OREGON_WAYPOINTMOVEMENTGENERATOR_H

#include "MovementGenerator.h"
#include "WaypointManager.h"
#include "Path.h"
#include "PathFinder.h"
#include "Player.h"

#include <vector>
#include <set>

#define STOP_TIME_FOR_PLAYER  3 * MINUTE * IN_MILLISECONDS          // 3 Minutes
#define TIMEDIFF_NEXT_WP      250

template<class T, class P>
class PathMovementBase
{
    public:
        PathMovementBase() : i_currentNode(0) {}
        virtual ~PathMovementBase() {};

        void LoadPath(T&);
        void ReloadPath(T&);
        uint32 GetCurrentNode() const
        {
            return i_currentNode;
        }

        void PreloadEndGrid();
        void InitEndGridInfo();

    protected:
        uint32 i_currentNode;
        P i_path;
};

template<class T>

class WaypointMovementGenerator
    : public MovementGeneratorMedium< T, WaypointMovementGenerator<T> >, public PathMovementBase<T, WaypointPath const*>
{
    public:
        WaypointMovementGenerator(uint32 _path_id = 0, bool _repeating = true) :
            node(NULL), path_id(_path_id), i_nextMoveTime(0), repeating(_repeating), StopedByPlayer(false) {}

        void Initialize(T&);
        void Finalize(T&);
        void MovementInform(T&);
        void InitTraveller(T&);
        void GeneratePathId(T&);
        void Reset(T& unit);
        bool Update(T&, const uint32&);
        MovementGeneratorType GetMovementGeneratorType()
        {
            return WAYPOINT_MOTION_TYPE;
        }

    private:
        void MoveToNextNode(T&, const WaypointData&);
        WaypointData* node;
        uint32 path_id;
        TimeTrackerSmall i_nextMoveTime;
        WaypointPath const* waypoints;
        bool repeating, StopedByPlayer;
};

/** FlightPathMovementGenerator generates movement of the player for the paths
 * and hence generates ground and activities for the player.
 */
class FlightPathMovementGenerator
    : public MovementGeneratorMedium< Player, FlightPathMovementGenerator >,
      public PathMovementBase<Player, TaxiPathNodeList const*>
{
        uint32 i_pathId;
        std::vector<uint32> i_mapIds;
    public:
        explicit FlightPathMovementGenerator(TaxiPathNodeList const& pathnodes, uint32 startNode = 0)
        {
            i_path = &pathnodes;
            i_currentNode = startNode;
            m_endGridX = 0.0f;
            m_endGridY = 0.0f;
            m_endMapId = 0;
        }
        void Initialize(Player&);
        void Finalize(Player&);
        void Reset(Player&);
        bool Update(Player&, const uint32&);
        MovementGeneratorType GetMovementGeneratorType()
        {
            return FLIGHT_MOTION_TYPE;
        }
        void LoadPath(Player&);
        void ReloadPath(Player&)
        {
            /* don't reload flight path */
        }

        TaxiPathNodeList const& GetPath() { return *i_path; }
        uint32 GetPathAtMapEnd() const;
        bool HasArrived() const { return (i_currentNode >= i_path->size()); }
        void SetCurrentNodeAfterTeleport();
        void SkipCurrentNode() { ++i_currentNode; }
        void DoEventIfAny(Player& player, TaxiPathNodeEntry const& node, bool departure);
        bool GetResetPosition(Player&, float& /*x*/, float& /*y*/, float& /*z*/, float& /*o*/) const;
    private:
        // storage for preloading the flightmaster grid at end
        // before reaching final waypoint
        uint32 m_endMapId;
        uint32 m_preloadTargetNode;
        float m_endGridX;
        float m_endGridY;
        void PreloadEndGrid();
        void InitEndGridInfo();
};
#endif

