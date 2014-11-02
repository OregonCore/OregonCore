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

#ifndef MANGOS_PATH_FINDER_H
#define MANGOS_PATH_FINDER_H

#include "Path.h"
#include "MoveMapSharedDefines.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"

class Unit;

// 74*4.0f=296y  number_of_points*interval = max_path_len
// this is way more than actual evade range
// I think we can safely cut those down even more
#define MAX_PATH_LENGTH         74
#define MAX_POINT_PATH_LENGTH   74

#define SMOOTH_PATH_STEP_SIZE   4.0f
#define SMOOTH_PATH_SLOP        0.3f

#define VERTEX_SIZE       3
#define INVALID_POLYREF   0

enum PathType
{
    PATHFIND_BLANK          = 0x0000,   // path not built yet
    PATHFIND_NORMAL         = 0x0001,   // normal path
    PATHFIND_SHORTCUT       = 0x0002,   // travel through obstacles, terrain, air, etc (old behavior)
    PATHFIND_INCOMPLETE     = 0x0004,   // we have partial path to follow - getting closer to target
    PATHFIND_NOPATH         = 0x0008,   // no valid path at all or error in generating one
    PATHFIND_NOT_USING_PATH = 0x0010,   // used when we are either flying/swiming or on map w/o mmaps
    PATHFIND_SHORT          = 0x0020    // too long path, truncating
};

class PathInfo
{
    public:
        PathInfo(Unit const* owner, float destX, float destY, float destZ, bool forceDest = false);
        ~PathInfo();

        // Calculate the path from owner to given destination
        // return: true if new path was calculated, false otherwise (no change needed)
        bool Update(float destX, float destY, float destZ, bool forceDest = false);

        // option setters - use optional
        void setUseStrightPath(bool useStraightPath)
        {
            m_useStraightPath = useStraightPath;
        };
        void setPathLengthLimit(float distance)
        {
            m_pointPathLimit = std::min<uint32>(uint32(distance / SMOOTH_PATH_STEP_SIZE), MAX_POINT_PATH_LENGTH);
        };

        // result getters
        void getStartPosition(float& x, float& y, float& z)
        {
            x = m_startPosition.x;
            y = m_startPosition.y;
            z = m_startPosition.z;
        }
        void getNextPosition(float& x, float& y, float& z)
        {
            x = m_nextPosition.x;
            y = m_nextPosition.y;
            z = m_nextPosition.z;
        }
        void getEndPosition(float& x, float& y, float& z)
        {
            x = m_endPosition.x;
            y = m_endPosition.y;
            z = m_endPosition.z;
        }
        void getActualEndPosition(float& x, float& y, float& z)
        {
            x = m_actualEndPosition.x;
            y = m_actualEndPosition.y;
            z = m_actualEndPosition.z;
        }

        PathNode getStartPosition() const
        {
            return m_startPosition;
        }
        PathNode getNextPosition() const
        {
            return m_nextPosition;
        }
        PathNode getEndPosition() const
        {
            return m_endPosition;
        }
        PathNode getActualEndPosition() const
        {
            return m_actualEndPosition;
        }

        PointPath& getFullPath()
        {
            return m_pathPoints;
        }
        PathType getPathType() const
        {
            return m_type;
        }

        bool inRange(const PathNode& p1, const PathNode& p2, float r, float h) const;
        float dist3DSqr(const PathNode& p1, const PathNode& p2) const;
    private:

        dtPolyRef       m_pathPolyRefs[MAX_PATH_LENGTH];   // array of detour polygon references
        uint32          m_polyLength;                      // number of polygons in the path

        PointPath       m_pathPoints;       // our actual (x,y,z) path to the target
        PathType        m_type;             // tells what kind of path this is

        bool            m_useStraightPath;  // type of path will be generated
        bool            m_forceDestination; // when set, we will always arrive at given point
        uint32          m_pointPathLimit;   // limit point path size; min(this, MAX_POINT_PATH_LENGTH)

        PathNode        m_startPosition;    // {x, y, z} of current location
        PathNode        m_nextPosition;     // {x, y, z} of next location on the path
        PathNode        m_endPosition;      // {x, y, z} of the destination
        PathNode        m_actualEndPosition;  // {x, y, z} of the closest possible point to given destination

        const Unit* const       m_sourceUnit;       // the unit that is moving
        const dtNavMesh*       m_navMesh;          // the nav mesh
        const dtNavMeshQuery*  m_navMeshQuery;     // the nav mesh query used to find the path

        dtQueryFilter m_filter;                     // use single filter for all movements, update it when needed

        void setNextPosition(PathNode point)
        {
            m_nextPosition = point;
        }
        void setStartPosition(PathNode point)
        {
            m_startPosition = point;
        }
        void setEndPosition(PathNode point)
        {
            m_actualEndPosition = point;
            m_endPosition = point;
        }
        void setActualEndPosition(PathNode point)
        {
            m_actualEndPosition = point;
        }

        void NormalizePath();

        void clear()
        {
            m_polyLength = 0;
            m_pathPoints.clear();
        }

        bool inRangeYZX(const float* v1, const float* v2, float r, float h) const;

        dtPolyRef getPathPolyByPosition(const dtPolyRef* polyPath, uint32 polyPathSize, const float* point, float* distance = NULL) const;
        dtPolyRef getPolyByLocation(const float* point, float* distance) const;
        bool HaveTile(const PathNode& p) const;

        void BuildPolyPath(const PathNode& startPos, const PathNode& endPos);
        void BuildPointPath(const float* startPoint, const float* endPoint);
        void BuildShortcut();

        NavTerrain getNavTerrain(float x, float y, float z);
        void createFilter();
        void updateFilter();

        // smooth path functions
        uint32 fixupCorridor(dtPolyRef* path, uint32 npath, uint32 maxPath,
                             const dtPolyRef* visited, uint32 nvisited);
        bool getSteerTarget(const float* startPos, const float* endPos, float minTargetDist,
                            const dtPolyRef* path, uint32 pathSize, float* steerPos,
                            unsigned char& steerPosFlag, dtPolyRef& steerPosRef);
        dtStatus findSmoothPath(const float* startPos, const float* endPos,
                                const dtPolyRef* polyPath, uint32 polyPathSize,
                                float* smoothPath, int* smoothPathSize, uint32 smoothPathMaxSize);
};

#endif
