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

#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include "packet_builder.h"
#include "Unit.h"

namespace Movement
{
    UnitMoveType SelectSpeedType(uint32 moveFlags)
    {
        if (moveFlags & MOVEMENTFLAG_FLYING)
        {
            if (moveFlags & MOVEMENTFLAG_BACKWARD /*&& speed_obj.flight >= speed_obj.flight_back*/)
                return MOVE_FLIGHT_BACK;
            else
                return MOVE_FLIGHT;
        }
        else if (moveFlags & MOVEMENTFLAG_SWIMMING)
        {
            if (moveFlags & MOVEMENTFLAG_BACKWARD /*&& speed_obj.swim >= speed_obj.swim_back*/)
                return MOVE_SWIM_BACK;
            else
                return MOVE_SWIM;
        }
        else if (moveFlags & MOVEMENTFLAG_WALK_MODE)
        {
            // if ( speed_obj.run > speed_obj.walk )
            return MOVE_WALK;
        }
        else if (moveFlags & MOVEMENTFLAG_BACKWARD /*&& speed_obj.run >= speed_obj.run_back*/)
            return MOVE_RUN_BACK;

        return MOVE_RUN;
    }

    int32 MoveSplineInit::Launch()
    {
        MoveSpline& move_spline = *unit.movespline;

        Location real_position(unit.GetPositionX(), unit.GetPositionY(), unit.GetPositionZ(), unit.GetOrientation());


        // there is a big chane that current position is unknown if current state is not finalized, need compute it
        // this also allows calculate spline position and update map position in much greater intervals
        if (!move_spline.Finalized())
            real_position = move_spline.ComputePosition();

        if (args.path.empty())
        {
            // should i do the things that user should do?
            MoveTo(real_position);
        }

        // corrent first vertex
        args.path[0] = real_position;
        uint32 moveFlags = unit.m_movementInfo.GetMovementFlags();
        if (args.flags.runmode)
            moveFlags &= ~MOVEMENTFLAG_WALK_MODE;
        else
            moveFlags |= MOVEMENTFLAG_WALK_MODE;

        moveFlags |= (MOVEMENTFLAG_SPLINE_ENABLED | MOVEMENTFLAG_FORWARD);

        if (args.velocity == 0.f)
            args.velocity = unit.GetSpeed(SelectSpeedType(moveFlags));

        if (!args.Validate(&unit))
            return 0;

        unit.m_movementInfo.SetMovementFlags((MovementFlags)moveFlags);
        move_spline.Initialize(args);

        WorldPacket data(SMSG_MONSTER_MOVE, 64);
        data << unit.GetPackGUID();

        PacketBuilder::WriteMonsterMove(move_spline, data);
        unit.SendMessageToSet(&data, true);

        return move_spline.Duration();
    }

    void MoveSplineInit::Stop()
    {
        MoveSpline& move_spline = *unit.movespline;

        // No need to stop if we are not moving
        if (move_spline.Finalized())
            return;

        Location real_position(unit.GetPositionX(), unit.GetPositionY(), unit.GetPositionZ(), unit.GetOrientation());


        // there is a big chance that current position is unknown if current state is not finalized, need compute it
        // this also allows calculate spline position and update map position in much greater intervals
        if (!move_spline.Finalized())
            real_position = move_spline.ComputePosition();

        if (args.path.empty())
        {
            // should i do the things that user should do?
            MoveTo(real_position);
        }

        // current first vertex
        args.path[0] = real_position;

        args.flags = MoveSplineFlag::Done;
        unit.m_movementInfo.RemoveMovementFlag(MovementFlags(MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_SPLINE_ENABLED));
        move_spline.Initialize(args);

        WorldPacket data(SMSG_MONSTER_MOVE, 64);
        data << unit.GetPackGUID();

        data << real_position.x << real_position.y << real_position.z;
        data << move_spline.GetId();
        data << uint8(MonsterMoveStop);
        unit.SendMessageToSet(&data, true);
    }

    MoveSplineInit::MoveSplineInit(Unit& m) : unit(m)
    {
        // mix existing state into new
        args.flags.runmode = !unit.m_movementInfo.HasMovementFlag(MOVEMENTFLAG_WALK_MODE);
        args.flags.flying = unit.m_movementInfo.HasMovementFlag((MovementFlags)(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING | MOVEMENTFLAG_LEVITATING));
    }

    void MoveSplineInit::SetFacing(const Unit* target)
    {
        args.flags.EnableFacingTarget();
        args.facing.target = target->GetObjectGUID().GetRawValue();
    }

    void MoveSplineInit::SetFacing(float angle)
    {
        args.facing.angle = G3D::wrap(angle, 0.f, (float)G3D::twoPi());
        args.flags.EnableFacingAngle();
    }

    void MoveSplineInit::MoveTo(const Vector3& dest, bool generatePath, bool forceDestination)
    {
        if (generatePath)
        {
            PathInfo path(&unit);
            path.Update(dest.x, dest.y, dest.z, forceDestination);
            MovebyPath(path.getFullPath());

        }
        else
        {
            args.path_Idx_offset = 0;
            args.path.resize(2);
            args.path[1] = dest;
        }
    }
}
