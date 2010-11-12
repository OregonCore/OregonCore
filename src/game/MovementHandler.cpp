/*
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
 *
 * Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Common.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"
#include "World.h"
#include "Corpse.h"
#include "Player.h"
#include "MapManager.h"
#include "Transports.h"
#include "BattleGround.h"
#include "WaypointMovementGenerator.h"
#include "InstanceSaveMgr.h"

//Uncomment to see anti cheat debug messages in console
//#define MOVEMENT_ANTICHEAT_DEBUG

void WorldSession::HandleMoveWorldportAckOpcode(WorldPacket & /*recv_data*/)
{
    sLog.outDebug("WORLD: got MSG_MOVE_WORLDPORT_ACK.");
    HandleMoveWorldportAckOpcode();
}

void WorldSession::HandleMoveWorldportAckOpcode()
{
    // ignore unexpected far teleports
    if (!GetPlayer()->IsBeingTeleportedFar())
        return;

    // get the teleport destination
    WorldLocation &loc = GetPlayer()->GetTeleportDest();

    // possible errors in the coordinate validity check
    if (!MapManager::IsValidMapCoord(loc))
    {
        sLog.outError("WorldSession::HandleMoveWorldportAckOpcode: player %s (%d) was teleported far to a not valid location. (map:%u, x:%f, y:%f, "
            "z:%f) We port him to his homebind instead..", GetPlayer()->GetName(), GetPlayer()->GetGUIDLow(), loc.GetMapId(), loc.GetPositionX(), loc.GetPositionY(), loc.GetPositionZ());
        // stop teleportation else we would try this again and again in LogoutPlayer...
        GetPlayer()->SetSemaphoreTeleportFar(false);
        // and teleport the player to a valid place
        GetPlayer()->TeleportToHomebind();
        return;
    }

    //reset falltimer at teleport
    GetPlayer()->m_anti_justteleported = 1;

    // get the destination map entry, not the current one, this will fix homebind and reset greeting
    MapEntry const* mEntry = sMapStore.LookupEntry(loc.GetMapId());
    InstanceTemplate const* mInstance = objmgr.GetInstanceTemplate(loc.GetMapId());

    // reset instance validity, except if going to an instance inside an instance
    if (GetPlayer()->m_InstanceValid == false && !mInstance)
        GetPlayer()->m_InstanceValid = true;

    GetPlayer()->SetSemaphoreTeleportFar(false);

    Map * oldMap = GetPlayer()->GetMap();
    ASSERT(oldMap);
    if (GetPlayer()->IsInWorld())
    {
        sLog.outCrash("Player is still in world when teleported from map %u! to new map %u", oldMap->GetId(), loc.GetMapId());
        oldMap->Remove(GetPlayer(), false);
    }

    // relocate the player to the teleport destination
    Map * newMap = MapManager::Instance().CreateMap(loc.GetMapId(), GetPlayer(), 0);
    // the CanEnter checks are done in TeleporTo but conditions may change
    // while the player is in transit, for example the map may get full
    if (!newMap || !newMap->CanEnter(GetPlayer()))
    {
        sLog.outError("Map %d could not be created for player %d, porting player to homebind", loc.GetMapId(), GetPlayer()->GetGUIDLow());
        GetPlayer()->TeleportToHomebind();
        return;
    }
    else
        GetPlayer()->Relocate(&loc);

    GetPlayer()->ResetMap();
    GetPlayer()->SetMap(newMap);

    // check this before Map::Add(player), because that will create the instance save!
    bool reset_notify = (GetPlayer()->GetBoundInstance(GetPlayer()->GetMapId(), GetPlayer()->GetDifficulty()) == NULL);

    GetPlayer()->SendInitialPacketsBeforeAddToMap();
    if (!GetPlayer()->GetMap()->Add(GetPlayer()))
    {
        sLog.outError("WORLD: failed to teleport player %s (%d) to map %d because of unknown reason!", GetPlayer()->GetName(), GetPlayer()->GetGUIDLow(), loc.GetMapId());
        GetPlayer()->ResetMap();
        GetPlayer()->SetMap(oldMap);
        GetPlayer()->TeleportToHomebind();
        return;
    }

    // battleground state prepare (in case join to BG), at relogin/tele player not invited
    // only add to bg group and object, if the player was invited (else he entered through command)
    if (_player->InBattleGround())
    {
        // cleanup setting if outdated
        if (!mEntry->IsBattleGroundOrArena())
        {
            // We're not in BG
            _player->SetBattleGroundId(0);                          // We're not in BG.
            // reset destination bg team
            _player->SetBGTeam(0);
        }
        // join to bg case
        else if (BattleGround *bg = _player->GetBattleGround())
        {
            if (_player->IsInvitedForBattleGroundInstance(_player->GetBattleGroundId()))
                bg->AddPlayer(_player);
        }
    }

    GetPlayer()->SendInitialPacketsAfterAddToMap();

    // flight fast teleport case
    if (GetPlayer()->GetMotionMaster()->GetCurrentMovementGeneratorType() == FLIGHT_MOTION_TYPE)
    {
        if (!_player->InBattleGround())
        {
            // short preparations to continue flight
            FlightPathMovementGenerator* flight = (FlightPathMovementGenerator*)(GetPlayer()->GetMotionMaster()->top());
            flight->Initialize(*GetPlayer());
            return;
        }

        // battleground state prepare, stop flight
        GetPlayer()->GetMotionMaster()->MovementExpired();
        GetPlayer()->CleanupAfterTaxiFlight();
    }

    // resurrect character at enter into instance where his corpse exist after add to map
    Corpse *corpse = GetPlayer()->GetCorpse();
    if (corpse && corpse->GetType() != CORPSE_BONES && corpse->GetMapId() == GetPlayer()->GetMapId())
    {
        if (mEntry->IsDungeon())
        {
            GetPlayer()->ResurrectPlayer(0.5f, false);
            GetPlayer()->SpawnCorpseBones();
        }
    }

    if (mEntry->IsRaid() && mInstance)
    {
        if (reset_notify)
        {
            uint32 timeleft = sInstanceSaveManager.GetResetTimeFor(GetPlayer()->GetMapId()) - time(NULL);
            GetPlayer()->SendInstanceResetWarning(GetPlayer()->GetMapId(), timeleft); // greeting at the entrance of the resort raid instance
        }
    }

    // mount allow check
    if (!mEntry->IsMountAllowed())
        _player->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

    // honorless target
    if (GetPlayer()->pvpInfo.inHostileArea)
        GetPlayer()->CastSpell(GetPlayer(), 2479, true);

    // resummon pet
    if (GetPlayer()->m_temporaryUnsummonedPetNumber)
    {
        Pet* NewPet = new Pet;
        if (!NewPet->LoadPetFromDB(GetPlayer(), 0, GetPlayer()->m_temporaryUnsummonedPetNumber, true))
            delete NewPet;

        GetPlayer()->m_temporaryUnsummonedPetNumber = 0;
    }

    //lets process all delayed operations on successful teleport
    GetPlayer()->ProcessDelayedOperations();
}

void WorldSession::HandleMoveTeleportAck(WorldPacket& recv_data)
{
    sLog.outDebug("MSG_MOVE_TELEPORT_ACK");

    uint64 guid;
    uint32 flags, time;

    recv_data >> guid;
    recv_data >> flags >> time;
    sLog.outDebug("Guid " UI64FMTD, guid);
    sLog.outDebug("Flags %u, time %u", flags, time/IN_MILLISECONDS);

    Player* plMover = GetPlayer();

    if (!plMover || !plMover->IsBeingTeleportedNear())
        return;

    if (guid != plMover->GetGUID())
        return;

    plMover->SetSemaphoreTeleportNear(false);

    uint32 old_zone = plMover->GetZoneId();

    WorldLocation const& dest = plMover->GetTeleportDest();

    plMover->SetPosition(dest.GetPositionX(), dest.GetPositionY(), dest.GetPositionZ(), dest.GetOrientation(), true);

    uint32 newzone = plMover->GetZoneId();

    plMover->UpdateZone(newzone);

    // new zone
    if (old_zone != newzone)
    {
        // honorless target
        if (plMover->pvpInfo.inHostileArea)
            plMover->CastSpell(plMover, 2479, true);
    }

    // resummon pet
    if (GetPlayer()->m_temporaryUnsummonedPetNumber)
    {
        Pet* NewPet = new Pet;
        if (!NewPet->LoadPetFromDB(GetPlayer(), 0, GetPlayer()->m_temporaryUnsummonedPetNumber, true))
            delete NewPet;

        GetPlayer()->m_temporaryUnsummonedPetNumber = 0;
    }

    //lets process all delayed operations on successful teleport
    GetPlayer()->ProcessDelayedOperations();
}

void WorldSession::HandleMovementOpcodes(WorldPacket & recv_data)
{
    // ignore, waiting processing in WorldSession::HandleMoveWorldportAckOpcode and WorldSession::HandleMoveTeleportAck
    if (GetPlayer()->IsBeingTeleported())
    {
        GetPlayer()->m_anti_justteleported = 1;
        recv_data.rpos(recv_data.wpos());                   // prevent warnings spam
        return;
    }

    //get opcode
    uint16 opcode = recv_data.GetOpcode();
    /* extract packet */
    MovementInfo movementInfo;
    recv_data >> movementInfo;
    /*----------------*/

    if (recv_data.size() != recv_data.rpos())
    {
        sLog.outError("MovementHandler: player %s (guid %d, account %u) sent a packet (opcode %u) that is %u bytes larger than it should be. Kicked as cheater.", _player->GetName(), _player->GetGUIDLow(), _player->GetSession()->GetAccountId(), opcode, recv_data.size() - recv_data.rpos());
        KickPlayer();
        return;
    }

    if (!Oregon::IsValidMapCoord(movementInfo.GetPos()->GetPositionX(), movementInfo.GetPos()->GetPositionY(), movementInfo.GetPos()->GetPositionZ(), movementInfo.GetPos()->GetOrientation()))
        return;

    // Handle possessed unit movement separately
    Unit* pos_unit = GetPlayer()->GetCharm();
    if (pos_unit && pos_unit->isPossessed()) // can be charmed but not possessed
    {
        HandlePossessedMovement(recv_data, movementInfo);
        return;
    }

    //Save movement flags
    GetPlayer()->SetUnitMovementFlags(movementInfo.GetMovementFlags());

    /* handle special cases */
    if (movementInfo.HasMovementFlag(MOVEFLAG_ONTRANSPORT))
    {
        // transports size limited
        // (also received at zeppelin/lift leave by some reason with t_* as absolute in continent coordinates, can be safely skipped)
        if (movementInfo.GetTransportPos()->GetPositionX() > 60 || movementInfo.GetTransportPos()->GetPositionY() > 60 || movementInfo.GetTransportPos()->GetPositionZ() > 60)
            return;

        if (!Oregon::IsValidMapCoord(movementInfo.GetPos()->GetPositionX() + movementInfo.GetTransportPos()->GetPositionX(), movementInfo.GetPos()->GetPositionY() + movementInfo.GetTransportPos()->GetPositionY(),
            movementInfo.GetPos()->GetPositionZ() + movementInfo.GetTransportPos()->GetPositionZ(), movementInfo.GetPos()->GetOrientation() + movementInfo.GetTransportPos()->GetOrientation()))
            return;
                
        if ((GetPlayer()->m_anti_transportGUID == 0) && (movementInfo.t_guid != 0))
        {
            // if we boarded a transport, add us to it
            if (!GetPlayer()->m_transport)
            {
                float trans_rad = movementInfo.GetTransportPos()->GetPositionX()*movementInfo.GetTransportPos()->GetPositionX() + movementInfo.GetTransportPos()->GetPositionY()*movementInfo.GetTransportPos()->GetPositionY() + movementInfo.GetTransportPos()->GetPositionZ()*movementInfo.GetTransportPos()->GetPositionZ();
                if (trans_rad > 3600.0f) // transport radius = 60 yards //cheater with on_transport_flag
                {
                    return;
                }
                // elevators also cause the client to send MOVEFLAG_ONTRANSPORT - just unmount if the guid can be found in the transport list
                for (MapManager::TransportSet::iterator iter = MapManager::Instance().m_Transports.begin(); iter != MapManager::Instance().m_Transports.end(); ++iter)
                {
                    if ((*iter)->GetGUID() == movementInfo.t_guid)
                    {
                        // unmount before boarding
                        _player->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

                        GetPlayer()->m_transport = (*iter);
                        (*iter)->AddPassenger(GetPlayer());
                        break;
                    }
                }
            }

            //GetPlayer()->m_anti_transportGUID = GUID_LOPART(movementInfo.t_guid);
            GameObject *obj = HashMapHolder<GameObject>::Find(movementInfo.t_guid);
            if(obj)
                GetPlayer()->m_anti_transportGUID = obj->GetDBTableGUIDLow();
            else
                GetPlayer()->m_anti_transportGUID = GUID_LOPART(movementInfo.t_guid);
        
            #ifdef MOVEMENT_ANTICHEAT_DEBUG
            sLog.outBasic("On Transport %d", GetPlayer()->m_anti_transportGUID ? GetPlayer()->m_anti_transportGUID : 0);
            #endif
        }
    }
    else if (GetPlayer()->m_transport)                      // if we were on a transport, leave
    {
        GetPlayer()->m_transport->RemovePassenger(GetPlayer());
        GetPlayer()->m_transport = NULL;
        GetPlayer()->m_anti_transportGUID = 0;
        movementInfo.ClearTransportData();
    }
    else if (GetPlayer()->m_anti_transportGUID != 0)
    {
        #ifdef MOVEMENT_ANTICHEAT_DEBUG
        sLog.outBasic("No more Transport %d", GetPlayer()->m_anti_transportGUID ? GetPlayer()->m_anti_transportGUID : 0);
        #endif
        GetPlayer()->m_anti_transportGUID = 0;
    }

    // fall damage generation (ignore in flight case that can be triggered also at lags in moment teleportation to another map).
    if (opcode == MSG_MOVE_FALL_LAND && !GetPlayer()->isInFlight())
    {
        GetPlayer()->m_anti_justjumped = 0;
        GetPlayer()->m_anti_jumpbase = 0;
        GetPlayer()->HandleFallDamage(movementInfo);
    }

    if (movementInfo.HasMovementFlag(MOVEFLAG_SWIMMING) != GetPlayer()->IsInWater())
    {
        // now client not include swimming flag in case jumping under water
        GetPlayer()->SetInWater(!GetPlayer()->IsInWater() || GetPlayer()->GetBaseMap()->IsUnderWater(movementInfo.GetPos()->GetPositionX(), movementInfo.GetPos()->GetPositionY(), movementInfo.GetPos()->GetPositionZ()));

        if (GetPlayer()->GetBaseMap()->IsUnderWater(movementInfo.GetPos()->GetPositionX(), movementInfo.GetPos()->GetPositionY(), movementInfo.GetPos()->GetPositionZ()-7.0f))
            GetPlayer()->m_anti_BeginFallZ = INVALID_HEIGHT;
    }

    #ifdef MOVEMENT_ANTICHEAT_DEBUG
    sLog.outBasic("%s newcoord: tm:%d ftm:%d | %f, %f, %fo(%f) [%X][%s]| transport: %f, %f, %fo(%f)", GetPlayer()->GetName(), movementInfo.time, movementInfo.GetFallTime(), movementInfo.GetPos()->GetPositionX(), movementInfo.GetPos()->GetPositionY(), movementInfo.GetPos()->GetPositionZ(), movementInfo.GetPos()->GetOrientation(), movementInfo.GetMovementFlags(), LookupOpcodeName(opcode), movementInfo.GetTransportPos()->GetPositionX(), movementInfo.GetTransportPos()->GetPositionY(), movementInfo.GetTransportPos()->GetPositionZ(), movementInfo.GetTransportPos()->GetOrientation());
    sLog.outBasic("Transport: %d |  tguid: %d - %d", GetPlayer()->m_anti_transportGUID, GUID_LOPART(movementInfo.t_guid), GUID_HIPART(movementInfo.t_guid));
    #endif

    /*----------------------*/

    //---- anti-cheat features -->>>
    bool check_passed = true;
    uint32 curDest = GetPlayer()->m_taxi.GetTaxiDestination(); // check taxi flight
    if ((GetPlayer()->m_anti_transportGUID == 0) && World::GetEnableMvAnticheat() && !curDest)
    {
        UnitMoveType move_type;

        if (movementInfo.HasMovementFlag(MOVEFLAG_FLYING))
            move_type = movementInfo.HasMovementFlag(MOVEFLAG_BACKWARD) ? MOVE_FLIGHT_BACK : MOVE_FLIGHT;
        else if (movementInfo.HasMovementFlag(MOVEFLAG_SWIMMING))
            move_type = movementInfo.HasMovementFlag(MOVEFLAG_BACKWARD) ? MOVE_SWIM_BACK : MOVE_SWIM;
        else if (movementInfo.HasMovementFlag(MOVEFLAG_WALK_MODE))
            move_type = MOVE_WALK;
        else    //hmm... in first time after login player has MOVE_SWIMBACK instead MOVE_WALKBACK
            move_type = movementInfo.HasMovementFlag(MOVEFLAG_BACKWARD) ? MOVE_SWIM_BACK : MOVE_RUN;

        float allowed_delta = 0;
        float current_speed = GetPlayer()->GetSpeed(move_type);

        float delta_x = GetPlayer()->GetPositionX() - movementInfo.GetPos()->GetPositionX();
        float delta_y = GetPlayer()->GetPositionY() - movementInfo.GetPos()->GetPositionY();
        float delta_z = GetPlayer()->GetPositionZ() - movementInfo.GetPos()->GetPositionZ();
        float real_delta = delta_x * delta_x + delta_y * delta_y;

        float tg_z = -99999; // tangens
        float time_delta = movementInfo.time - GetPlayer()->m_anti_lastmovetime;
        if (time_delta > 0)
            GetPlayer()->m_anti_lastmovetime = movementInfo.time;
        else
            time_delta = 0;
        time_delta = (time_delta < 1500) ? time_delta / 1000 : 1.5f; // normalize time - 1.5 second allowed for heavy loaded server

        if (!(movementInfo.GetMovementFlags() & (MOVEFLAG_FLYING | MOVEFLAG_SWIMMING)))
            tg_z = (real_delta != 0) ? (delta_z * delta_z / real_delta) : -99999;

        if (current_speed < GetPlayer()->m_anti_last_hspeed)
        {
            allowed_delta = GetPlayer()->m_anti_last_hspeed;
            if (GetPlayer()->m_anti_lastspeed_changetime == 0)
                GetPlayer()->m_anti_lastspeed_changetime = movementInfo.time + (uint32)floor(((GetPlayer()->m_anti_last_hspeed / current_speed) * 1000)) + 100; //100ms above for random fluctuating =)))
        }
        else allowed_delta = current_speed;
        allowed_delta = allowed_delta * time_delta;
        allowed_delta = allowed_delta * allowed_delta + 2;

       // static char const* move_type_name[MAX_MOVE_TYPE] = {  "Walk", "Run", "Walkback", "Swim", "Swimback", "Turn", "Fly", "Flyback" };
       // sLog.outBasic("%s newcoord: tm:%d ftm:%d | %f, %f, %fo(%f) [%X][%s]$%s", GetPlayer()->GetName(), movementInfo.time, movementInfo.GetFallTime(), movementInfo.GetPos()->x, movementInfo.GetPos()->y, movementInfo.GetPos()->z, movementInfo.GetPos()->o, movementFlags, LookupOpcodeName(opcode), move_type_name[move_type]);
       // sLog.outBasic("%f", tg_z);
       //AntiGravitation (thanks to Meekro)
        float JumpHeight = GetPlayer()->m_anti_jumpbase - movementInfo.GetPos()->GetPositionZ();
        if ((GetPlayer()->m_anti_jumpbase != 0)
                    && !(movementInfo.HasMovementFlag(MOVEFLAG_SWIMMING) || movementInfo.HasMovementFlag(MOVEFLAG_FLYING)
                        || movementInfo.HasMovementFlag(MOVEFLAG_FLYING2))
                    && (JumpHeight < GetPlayer()->m_anti_last_vspeed))
        {
            #ifdef MOVEMENT_ANTICHEAT_DEBUG
            sLog.outError("Movement anticheat: %s is graviJump exception. dz=%f",GetPlayer()->GetName(), movementInfo.GetPos()->z - GetPlayer()->m_anti_jumpbase);
            #endif
            check_passed = false;
        }

        if (opcode == MSG_MOVE_JUMP && !GetPlayer()->IsInWater())
        {
            if (GetPlayer()->m_anti_justjumped >= 1)
            {
                //GetPlayer()->m_anti_justjumped = 0;
                check_passed = false; //don't process new jump packet
            } 
            else
            {
                GetPlayer()->m_anti_justjumped += 1;
                GetPlayer()->m_anti_jumpbase = movementInfo.GetPos()->GetPositionZ();
            }
        } 
        else if (GetPlayer()->IsInWater())
        {
             GetPlayer()->m_anti_justjumped = 0;
        }

        if ((real_delta > allowed_delta) && (delta_z < 1))
        {
            #ifdef MOVEMENT_ANTICHEAT_DEBUG
            sLog.outDebug("Movement anticheat: %s is speed exception. {real_delta=%f allowed_delta=%f | current_speed=%f preview_speed=%f time=%f}(%f %f %f %d)[%s]", GetPlayer()->GetName(), real_delta, allowed_delta, current_speed, GetPlayer()->m_anti_last_hspeed, time_delta, GetPlayer()->GetPositionX(), GetPlayer()->GetPositionY(), GetPlayer()->GetPositionZ(), GetPlayer()->GetMapId(), LookupOpcodeName(opcode));
            #endif
            check_passed = false;
        }
        if ((real_delta > 4900.0f) && !(real_delta < allowed_delta))
        {
            #ifdef MOVEMENT_ANTICHEAT_DEBUG
            sLog.outDebug("Movement anticheat: %s is teleport exception. {real_delta=%f allowed_delta=%f | current_speed=%f preview_speed=%f time=%f}(%f %f %f %d)", GetPlayer()->GetName(), real_delta, allowed_delta, current_speed, GetPlayer()->m_anti_last_hspeed, time_delta, GetPlayer()->GetPositionX(), GetPlayer()->GetPositionY(), GetPlayer()->GetPositionZ(), GetPlayer()->GetMapId());
            #endif
            check_passed = false;
        }
        if (movementInfo.time>GetPlayer()->m_anti_lastspeed_changetime)
        {
            GetPlayer()->m_anti_last_hspeed = current_speed; // store current speed
            GetPlayer()->m_anti_last_vspeed = -3.2f;
            if (GetPlayer()->m_anti_lastspeed_changetime != 0) GetPlayer()->m_anti_lastspeed_changetime = 0;
        }
        if ((tg_z > 1.56f) && (delta_z < GetPlayer()->m_anti_last_vspeed))
        {
            #ifdef MOVEMENT_ANTICHEAT_DEBUG
            sLog.outDebug("Movement anticheat: %s is mountain exception. {tg_z=%f} (%f %f %f %d)", GetPlayer()->GetName(), tg_z, GetPlayer()->GetPositionX(), GetPlayer()->GetPositionY(), GetPlayer()->GetPositionZ(), GetPlayer()->GetMapId());
            #endif
            check_passed = false;
        }
        if (((movementInfo.GetMovementFlags() & (MOVEFLAG_CAN_FLY | MOVEFLAG_FLYING | MOVEFLAG_FLYING2)) != 0) && !GetPlayer()->isGameMaster() && !(GetPlayer()->HasAuraType(SPELL_AURA_FLY) || GetPlayer()->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED)))
        {
            #ifdef MOVEMENT_ANTICHEAT_DEBUG
            sLog.outDebug("Movement anticheat: %s is fly cheater. {SPELL_AURA_FLY=[%X]} {SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED=[%X]} {SPELL_AURA_MOD_FLIGHT_SPEED_STACKING=[%X]} {SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_STACKING=[%X]} {SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_NOT_STACKING=[%X]}",
               GetPlayer()->GetName(),
               GetPlayer()->HasAuraType(SPELL_AURA_FLY), GetPlayer()->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED),
               GetPlayer()->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_STACKING), GetPlayer()->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_STACKING),
               GetPlayer()->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_NOT_STACKING));
            #endif
            check_passed = false;
        }
        if (movementInfo.HasMovementFlag(MOVEFLAG_WATERWALKING) && !GetPlayer()->isGameMaster() && !(GetPlayer()->HasAuraType(SPELL_AURA_WATER_WALK) | GetPlayer()->HasAuraType(SPELL_AURA_GHOST)))
        {
            #ifdef MOVEMENT_ANTICHEAT_DEBUG
            sLog.outDebug("Movement anticheat: %s is water-walk exception. [%X]{SPELL_AURA_WATER_WALK=[%X]}", GetPlayer()->GetName(), movementInfo.GetMovementFlags(), GetPlayer()->HasAuraType(SPELL_AURA_WATER_WALK));
            #endif
            check_passed = false;
        }
        if (movementInfo.GetPos()->GetPositionZ() < 0.0001f && movementInfo.GetPos()->GetPositionZ() > -0.0001f && ((movementInfo.GetMovementFlags() & (MOVEFLAG_SWIMMING | MOVEFLAG_CAN_FLY | MOVEFLAG_FLYING | MOVEFLAG_FLYING2)) == 0) && !GetPlayer()->isGameMaster())
        {
            // Prevent using TeleportToPlane.
            Map *map = GetPlayer()->GetMap();
            if (map)
            {
                float plane_z = map->GetHeight(movementInfo.GetPos()->GetPositionX(), movementInfo.GetPos()->GetPositionY(), MAX_HEIGHT) - movementInfo.GetPos()->GetPositionZ();
                plane_z = (plane_z < -500.0f) ? 0 : plane_z; // check holes in height map
                if (plane_z > 0.1f || plane_z < -0.1f)
                {
                    GetPlayer()->m_anti_teletoplane_count++;
                    check_passed = false;
                    #ifdef MOVEMENT_ANTICHEAT_DEBUG
                    sLog.outDebug("Movement anticheat: %s is teleport to plane exception. plane_z: %f ", GetPlayer()->GetName(), plane_z);
                    #endif
                    if (GetPlayer()->m_anti_teletoplane_count > World::GetTeleportToPlaneAlarms())
                    {
                        GetPlayer()->GetSession()->KickPlayer();
                        sLog.outError("Movement anticheat: %s is teleport to plane exception. Exception count: %d ", GetPlayer()->GetName(), GetPlayer()->m_anti_teletoplane_count);
                    }
                }
            }
        }
        else
        {
            if (GetPlayer()->m_anti_teletoplane_count != 0)
                GetPlayer()->m_anti_teletoplane_count = 0;
        }
    }
    else if (movementInfo.HasMovementFlag(MOVEFLAG_ONTRANSPORT))
    {
        // antiwrap =)
        if (GetPlayer()->m_transport)
        {
            float trans_rad = movementInfo.GetTransportPos()->GetPositionX() * movementInfo.GetTransportPos()->GetPositionX() + movementInfo.GetTransportPos()->GetPositionY() * movementInfo.GetTransportPos()->GetPositionY() + movementInfo.GetTransportPos()->GetPositionZ() * movementInfo.GetTransportPos()->GetPositionZ();
            if (trans_rad > 3600.0f)
                check_passed = false;
        }
        else
        {
            if (GameObjectData const* go_data = objmgr.GetGOData(GetPlayer()->m_anti_transportGUID))
            {
                float delta_gox = go_data->posX - movementInfo.GetPos()->GetPositionX();
                float delta_goy = go_data->posY - movementInfo.GetPos()->GetPositionY();
                float delta_goz = go_data->posZ - movementInfo.GetPos()->GetPositionZ();
                int mapid = go_data->mapid;
                #ifdef MOVEMENT_ANTICHEAT_DEBUG
                sLog.outDebug("Movement anticheat: %s on some transport. xyzo: %f, %f, %f", GetPlayer()->GetName(), go_data->posX, go_data->posY, go_data->posZ);
                #endif
                if (GetPlayer()->GetMapId() != mapid)
                {
                    check_passed = false;
                }
                else if (mapid != 369)
                {
                    float delta_go = delta_gox*delta_gox + delta_goy*delta_goy;
                    if (delta_go > 3600.0f)
                        check_passed = false;
                }

            }
            else
            {
                #ifdef MOVEMENT_ANTICHEAT_DEBUG
                sLog.outDebug("Movement anticheat: %s on undefined transport.", GetPlayer()->GetName());
                #endif
                check_passed = false;
            }
        }

        if (!check_passed)
        {
            if (GetPlayer()->m_transport)
            {
                GetPlayer()->m_transport->RemovePassenger(GetPlayer());
                GetPlayer()->m_transport = NULL;
            }
            movementInfo.ClearTransportData();
            GetPlayer()->m_anti_transportGUID = 0;
        }
    }

    /* process position-change */
    if (check_passed)
    {
        recv_data.put<uint32>(5, getMSTime());                  // offset flags(4) + unk(1)
        WorldPacket data(opcode, (GetPlayer()->GetPackGUID().size()+recv_data.size()));
        data << GetPlayer()->GetPackGUID();
        data.append(recv_data.contents(), recv_data.size());
        GetPlayer()->SendMessageToSet(&data, false);

        GetPlayer()->SetPosition(movementInfo.GetPos()->GetPositionX(), movementInfo.GetPos()->GetPositionY(), movementInfo.GetPos()->GetPositionZ(), movementInfo.GetPos()->GetOrientation());
        GetPlayer()->m_movementInfo = movementInfo;
        if (GetPlayer()->m_lastFallTime > movementInfo.GetFallTime() || GetPlayer()->m_lastFallZ < movementInfo.GetPos()->GetPositionZ() || opcode == MSG_MOVE_FALL_LAND)
            GetPlayer()->SetFallInformation(movementInfo.GetFallTime(), movementInfo.GetPos()->GetPositionZ());

        if (GetPlayer()->isMovingOrTurning())
            GetPlayer()->RemoveSpellsCausingAura(SPELL_AURA_FEIGN_DEATH);

        if (movementInfo.GetPos()->GetPositionZ() < -500.0f)
            GetPlayer()->HandleFallUnderMap();

        if (GetPlayer()->m_anti_alarmcount > 0)
        {
            sLog.outError("Movement anticheat: %s produce %d anticheat alarms", GetPlayer()->GetName(), GetPlayer()->m_anti_alarmcount);
            GetPlayer()->m_anti_alarmcount = 0;
        }
    }
    else
    {
        GetPlayer()->m_anti_alarmcount++;
        WorldPacket data;
        GetPlayer()->SetUnitMovementFlags(0);
        GetPlayer()->BuildTeleportAckMsg(&data, GetPlayer()->GetPositionX(), GetPlayer()->GetPositionY(), GetPlayer()->GetPositionZ(), GetPlayer()->GetOrientation());
        GetPlayer()->GetSession()->SendPacket(&data);
        GetPlayer()->BuildHeartBeatMsg(&data);
        GetPlayer()->SendMessageToSet(&data, true);
    }
}

void WorldSession::HandlePossessedMovement(WorldPacket& recv_data, MovementInfo& movementInfo)
{
    // Whatever the client is controlling, it will send the GUID of the original player.
    // If current player is controlling, it must be handled like the controlled player sent these opcodes

    Unit* pos_unit = GetPlayer()->GetCharm();

    if (pos_unit->GetTypeId() == TYPEID_PLAYER)
        return;

    //Save movement flags
    pos_unit->SetUnitMovementFlags(movementInfo.GetMovementFlags());

    // Remove possession if possessed unit enters a transport
    if (movementInfo.HasMovementFlag(MOVEFLAG_ONTRANSPORT))
    {
        GetPlayer()->Uncharm();
        return;
    }

    uint16 opcode = recv_data.GetOpcode();

    recv_data.put<uint32>(5, getMSTime());
    WorldPacket data(opcode, pos_unit->GetPackGUID().size()+recv_data.size());
    data << pos_unit->GetPackGUID();
    data.append(recv_data.contents(), recv_data.size());
    // Send the packet to self but not to the possessed player; for creatures the first bool is irrelevant
    pos_unit->SendMessageToSet(&data, true);

    // Possessed is a player
    if (pos_unit->GetTypeId() == TYPEID_PLAYER)
    {
        Player* plr = pos_unit->ToPlayer();

        if (opcode == MSG_MOVE_FALL_LAND)
            plr->HandleFallDamage(movementInfo);

        if (movementInfo.HasMovementFlag(MOVEFLAG_SWIMMING) != plr->IsInWater())
        {
            // Now client not include swimming flag in case jumping under water
            plr->SetInWater(!plr->IsInWater() || plr->GetBaseMap()->IsUnderWater(movementInfo.GetPos()->GetPositionX(), movementInfo.GetPos()->GetPositionY(), movementInfo.GetPos()->GetPositionZ()));
        }

        plr->SetPosition(movementInfo.GetPos()->GetPositionX(), movementInfo.GetPos()->GetPositionY(), movementInfo.GetPos()->GetPositionZ(), movementInfo.GetPos()->GetOrientation(), false);
        plr->m_movementInfo = movementInfo;

        if (plr->isMovingOrTurning())
            plr->RemoveSpellsCausingAura(SPELL_AURA_FEIGN_DEATH);

        if (movementInfo.GetPos()->GetPositionZ() < -500.0f)
        {
            GetPlayer()->Uncharm();
            plr->HandleFallUnderMap();
        }
    }
    else // Possessed unit is a creature
    {
        Map* map = pos_unit->GetMap();
        map->CreatureRelocation(pos_unit->ToCreature(), movementInfo.GetPos()->GetPositionX(), movementInfo.GetPos()->GetPositionY(), movementInfo.GetPos()->GetPositionZ(), movementInfo.GetPos()->GetOrientation());
    }
}

void WorldSession::HandleForceSpeedChangeAck(WorldPacket &recv_data)
{
    /* extract packet */
    ObjectGuid guid;
    MovementInfo movementInfo;
    float newspeed;

    recv_data >> guid;
    recv_data >> Unused<uint32>();                          // counter or moveEvent
    recv_data >> movementInfo;
    recv_data >> newspeed;

    // now can skip not our packet
    if (_player->GetGUID() != guid.GetRawValue())
        return;
    /*----------------*/

    // client ACK send one packet for mounted/run case and need skip all except last from its
    // in other cases anti-cheat check can be fail in false case
    UnitMoveType move_type;
    UnitMoveType force_move_type;

    static char const* move_type_name[MAX_MOVE_TYPE] = {  "Walk", "Run", "RunBack", "Swim", "SwimBack", "TurnRate", "Flight", "FlightBack" };

    uint16 opcode = recv_data.GetOpcode();
    switch(opcode)
    {
        case CMSG_FORCE_WALK_SPEED_CHANGE_ACK:          move_type = MOVE_WALK;          force_move_type = MOVE_WALK;        break;
        case CMSG_FORCE_RUN_SPEED_CHANGE_ACK:           move_type = MOVE_RUN;           force_move_type = MOVE_RUN;         break;
        case CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK:      move_type = MOVE_RUN_BACK;      force_move_type = MOVE_RUN_BACK;    break;
        case CMSG_FORCE_SWIM_SPEED_CHANGE_ACK:          move_type = MOVE_SWIM;          force_move_type = MOVE_SWIM;        break;
        case CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK:     move_type = MOVE_SWIM_BACK;     force_move_type = MOVE_SWIM_BACK;   break;
        case CMSG_FORCE_TURN_RATE_CHANGE_ACK:           move_type = MOVE_TURN_RATE;     force_move_type = MOVE_TURN_RATE;   break;
        case CMSG_FORCE_FLIGHT_SPEED_CHANGE_ACK:        move_type = MOVE_FLIGHT;        force_move_type = MOVE_FLIGHT;      break;
        case CMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK:   move_type = MOVE_FLIGHT_BACK;   force_move_type = MOVE_FLIGHT_BACK; break;
        default:
            sLog.outError("WorldSession::HandleForceSpeedChangeAck: Unknown move type opcode: %u", opcode);
            return;
    }

    // skip all forced speed changes except last and unexpected
    // in run/mounted case used one ACK and it must be skipped.m_forced_speed_changes[MOVE_RUN} store both.
    if (_player->m_forced_speed_changes[force_move_type] > 0)
    {
        --_player->m_forced_speed_changes[force_move_type];
        if (_player->m_forced_speed_changes[force_move_type] > 0)
            return;
    }

    if (!_player->GetTransport() && fabs(_player->GetSpeed(move_type) - newspeed) > 0.01f)
    {
        if (_player->GetSpeed(move_type) > newspeed)         // must be greater - just correct
        {
            sLog.outError("%sSpeedChange player %s is NOT correct (must be %f instead %f), force set to correct value",
                move_type_name[move_type], _player->GetName(), _player->GetSpeed(move_type), newspeed);
            _player->SetSpeed(move_type, _player->GetSpeedRate(move_type), true);
        }
        else                                                // must be lesser - cheating
        {
            sLog.outBasic("Player %s from account id %u kicked for incorrect speed (must be %f instead %f)",
                _player->GetName(), _player->GetSession()->GetAccountId(), _player->GetSpeed(move_type), newspeed);
            _player->GetSession()->KickPlayer();
        }
    }
}

void WorldSession::HandleSetActiveMoverOpcode(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: Recvd CMSG_SET_ACTIVE_MOVER");

    recv_data >> Unused<uint64>();                          // guid
}

void WorldSession::HandleMoveNotActiveMoverOpcode(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: Recvd CMSG_MOVE_NOT_ACTIVE_MOVER");
    recv_data.hexlike();

    uint64 old_mover_guid;
    MovementInfo mi;

    old_mover_guid = recv_data.readPackGUID();
    recv_data >> mi;

    if (!old_mover_guid)
        return;

    _player->m_movementInfo = mi;
}

void WorldSession::HandleMountSpecialAnimOpcode(WorldPacket& /*recvdata*/)
{
    //sLog.outDebug("WORLD: Recvd CMSG_MOUNTSPECIAL_ANIM");

    WorldPacket data(SMSG_MOUNTSPECIAL_ANIM, 8);
    data << uint64(GetPlayer()->GetGUID());

    GetPlayer()->SendMessageToSet(&data, false);
}

void WorldSession::HandleMoveKnockBackAck(WorldPacket & recv_data)
{
    sLog.outDebug("CMSG_MOVE_KNOCK_BACK_ACK");
    // Currently not used but maybe use later for recheck final player position
    // (must be at call same as into "recv_data >> x >> y >> z >> orientation;"

    MovementInfo movementInfo;
    uint64 guid;

    recv_data >> guid;                                      // guid
    recv_data >> Unused<uint32>();                          // unk
    recv_data >> movementInfo;

    if (GetPlayer()->GetGUID() != guid)
        return;

    // Save movement flags
    _player->SetUnitMovementFlags(movementInfo.GetMovementFlags());

    #ifdef MOVEMENT_ANTICHEAT_DEBUG
    sLog.outDebug("%s CMSG_MOVE_KNOCK_BACK_ACK: tm:%d ftm:%d | %f, %f, %fo(%f) [%X]", GetPlayer()->GetName(), movementInfo.time, movementInfo.GetFallTime(), movementInfo.GetPos()->GetPositionX(), movementInfo.GetPos()->GetPositionY(), movementInfo.GetPos()->GetPositionZ(), movementInfo.GetPos()->GetOrientation(), movementInfo.GetMovementFlags());
    sLog.outDebug("%s CMSG_MOVE_KNOCK_BACK_ACK additional: vspeed:%f, hspeed:%f, xdir:%f ydir:%f", GetPlayer()->GetName(), movementInfo.j_velocity, movementInfo.j_xyspeed, movementInfo.j_sinAngle, movementInfo.j_cosAngle);
    #endif

    // skip not personal message;
    GetPlayer()->m_movementInfo = movementInfo;
    GetPlayer()->m_anti_last_hspeed = movementInfo.j_xyspeed;
    GetPlayer()->m_anti_last_vspeed = movementInfo.j_velocity < 3.2f ? movementInfo.j_xyspeed - 1.0f : 3.2f;
    GetPlayer()->m_anti_lastspeed_changetime = movementInfo.time + 1750;
}

void WorldSession::HandleMoveHoverAck(WorldPacket& recv_data)
{
    sLog.outDebug("CMSG_MOVE_HOVER_ACK");

    MovementInfo movementInfo;

    recv_data >> Unused<uint64>();                          // guid
    recv_data >> Unused<uint32>();                          // unk
    recv_data >> movementInfo;
    recv_data >> Unused<uint32>();                          // unk2
}

void WorldSession::HandleMoveWaterWalkAck(WorldPacket& recv_data)
{
    sLog.outDebug("CMSG_MOVE_WATER_WALK_ACK");

    MovementInfo movementInfo;

    recv_data >> Unused<uint64>();                          // guid
    recv_data >> Unused<uint32>();                          // unk
    recv_data >> movementInfo;
    recv_data >> Unused<uint32>();                          // unk2
}

void WorldSession::HandleSummonResponseOpcode(WorldPacket& recv_data)
{
    if (!_player->isAlive() || _player->isInCombat())
        return;

    uint64 summoner_guid;
    bool agree;
    recv_data >> summoner_guid;
    recv_data >> agree;

    _player->SummonIfPossible(agree);
}
