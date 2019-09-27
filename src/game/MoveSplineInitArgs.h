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

#ifndef OREGON_MOVESPLINEINIT_ARGS_H
#define OREGON_MOVESPLINEINIT_ARGS_H

#include "MoveSplineFlag.h"
#include "G3D/Vector3.h"

class Unit;

namespace Movement
{
    typedef std::vector<Vector3> PointsArray;

    union FacingInfo
    {
        struct
        {
            float x, y, z;
        } f;
        uint64  target;
        float   angle;

        FacingInfo(float o) : angle(o) {}
        FacingInfo(uint64 t) : target(t) {}
        FacingInfo() {}
    };

    struct MoveSplineInitArgs
    {
            MoveSplineInitArgs(size_t path_capacity = 16) : path_Idx_offset(0),
                velocity(0.f), splineId(0)
            {
                path.reserve(path_capacity);
            }

            PointsArray path;
            FacingInfo facing;
            MoveSplineFlag flags;
            int32 path_Idx_offset;
            float velocity;
            uint32 splineId;

            /** Returns true to show that the arguments were configured correctly and MoveSpline initialization will succeed. */
            bool Validate(Unit* unit) const;
        private:
            bool _checkPathBounds() const;
    };
}

#endif // OREGON_MOVESPLINEINIT_ARGS_H
