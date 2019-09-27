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

#ifndef OREGON_MOVESPLINEINIT_H
#define OREGON_MOVESPLINEINIT_H

#include "MoveSplineInitArgs.h"
#include "PathFinder.h"

class Unit;

namespace Movement
{
    /*  Initializes and launches spline movement
     */
    class MoveSplineInit
    {
        public:

            explicit MoveSplineInit(Unit& m);

            /* Final pass of initialization that launches spline movement.
             * @return duration - estimated travel time
             */
            int32 Launch();

            /* Stop any creature movement */
            void Stop();

            /* Adds final facing animation
             * sets unit's facing to specified point/angle after all path done
             * you can have only one final facing: previous will be overriden
             */
            void SetFacing(float angle);
            void SetFacing(Vector3 const& point);
            void SetFacing(const Unit* target);

            /* Initializes movement by path
             * @param path - array of points, shouldn't be empty
             * @param pointId - Id of fisrt point of the path. Example: when third path point will be done it will notify that pointId + 3 done
             */
            void MovebyPath(const PointsArray& path, int32 pointId = 0);

            /* Initializes simple A to B mition, A is current unit's position, B is destination
             */
            void MoveTo(const Vector3& destination, bool generatePath = false, bool forceDestination = false);
            void MoveTo(float x, float y, float z, bool generatePath = false, bool forceDestination = false);

            /* Sets Id of fisrt point of the path. When N-th path point will be done ILisener will notify that pointId + N done
             * Needed for waypoint movement where path splitten into parts
             */
            void SetFirstPointId(int32 pointId) { args.path_Idx_offset = pointId; }

            /* Enables CatmullRom spline interpolation mode, enables flying animation. Disabled by default
             */
            void SetFly();
            /* Enables walk mode. Disabled by default
             */
            void SetWalk(bool enable);
            /* Makes movement cyclic. Disabled by default
             */
            void SetCyclic();
            /* Enables falling mode. Disabled by default
             */
            void SetFall();

            /* Sets the velocity (in case you want to have custom movement velocity)
             * if no set, speed will be selected based on unit's speeds and current movement mode
             * Has no effect if falling mode enabled
             * velocity shouldn't be negative
             */
            void SetVelocity(float velocity);

            PointsArray& Path() { return args.path; }
        protected:

            MoveSplineInitArgs args;
            Unit&  unit;
    };

    inline void MoveSplineInit::SetFly() { args.flags.flying = true;}
    inline void MoveSplineInit::SetWalk(bool enable) { args.flags.runmode = !enable;}
    inline void MoveSplineInit::SetCyclic() { args.flags.cyclic = true;}
    inline void MoveSplineInit::SetFall() { args.flags.falling = true;}
    inline void MoveSplineInit::SetVelocity(float vel) { args.velocity = vel;}

    inline void MoveSplineInit::MovebyPath(const PointsArray& controls, int32 path_offset)
    {
        args.path_Idx_offset = path_offset;
        args.path.assign(controls.begin(), controls.end());
    }

    inline void MoveSplineInit::MoveTo(float x, float y, float z, bool generatePath, bool forceDestination)
    {
        MoveTo(G3D::Vector3(x, y, z), generatePath, forceDestination);
    }

    inline void MoveSplineInit::SetFacing(Vector3 const& spot)
    {
        args.facing.f.x = spot.x;
        args.facing.f.y = spot.y;
        args.facing.f.z = spot.z;
        args.flags.EnableFacingPoint();
    }
}
#endif // OREGON_MOVESPLINEINIT_H
