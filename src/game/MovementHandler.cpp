/*
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
 *
 * Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010 Oregon  <https://www.oregoncore.com/>
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
    DEBUG_LOG("WORLD: got MSG_MOVE_WORLDPORT_ACK.");
    HandleMoveWorldportAckOpcode();
}

void WorldSession::HandleMoveWorldportAckOpcode()
{
    DEBUG_LOG("WORLD: HandleMoveWorldportAckOpcode.");
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
    GetPlayer()->m_anti_justteleported = true;

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
    if (GetPlayer()->InBattleGround())
    {
        // cleanup setting if outdated
        if (!mEntry->IsBattleGroundOrArena())
        {
            // We're not in BG
            GetPlayer()->SetBattleGroundId(0);                          // We're not in BG.
            // reset destination bg team
            GetPlayer()->SetBGTeam(0);
        }
        // join to bg case
        else if (BattleGround *bg = GetPlayer()->GetBattleGround())
        {
            if (GetPlayer()->IsInvitedForBattleGroundInstance(GetPlayer()->GetBattleGroundId()))
                bg->AddPlayer(GetPlayer());
        }
    }

    GetPlayer()->SendInitialPacketsAfterAddToMap();

    // flight fast teleport case
    if (GetPlayer()->GetMotionMaster()->GetCurrentMovementGeneratorType() == FLIGHT_MOTION_TYPE)
    {
        if (!GetPlayer()->InBattleGround())
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
        GetPlayer()->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

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
    uint64 guid;
    uint32 flags, time;

    recv_data >> guid;
    recv_data >> flags >> time;
    DEBUG_LOG("Guid " UI64FMTD, guid);
    DEBUG_LOG("Flags %u, time %u", flags, time/IN_MILLISECONDS);

    Player* plMover = GetPlayer();

    if (!plMover || !plMover->IsBeingTeleportedNear())
        return;

    if (guid != plMover->GetGUID())
        return;

    //reset falltimer at teleport
    GetPlayer()->m_anti_justteleported = true;

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
    uint32 timediff = getMSTime();

    // ignore, waiting processing in WorldSession::HandleMoveWorldportAckOpcode and WorldSession::HandleMoveTeleportAck
    if (GetPlayer()->IsBeingTeleported())
    {
        GetPlayer()->m_anti_justteleported = true;
        recv_data.rpos(recv_data.wpos());                   // prevent warnings spam
        return;
    }

    //get opcode
    uint16 opcode = recv_data.GetOpcode();

    /* extract packet */
    MovementInfo movementInfo;
    recv_data >> movementInfo;
    /*----------------*/

    /*sLog.outBasic("MAC: %s, tel %d, fym %d, tax %d [%d,%d,%d] [%x]",
         LookupOpcodeName(opcode), GetPlayer()->m_anti_justteleported, GetPlayer()->m_anti_flymounted, GetPlayer()->m_anti_ontaxipath,
         movementInfo.HasMovementFlag(MOVEFLAG_NONE), movementInfo.HasMovementFlag(MOVEFLAG_MOVING), movementInfo.HasMovementFlag(MOVEFLAG_TURNING), movementInfo.GetMovementFlags());
    */

    if (recv_data.size() != recv_data.rpos())
    {
        sLog.outError("MovementHandler: player %s (guid %d, account %u) sent a packet (opcode %u) that is %u bytes larger than it should be. Kicked as cheater.", GetPlayer()->GetName(), GetPlayer()->GetGUIDLow(), GetPlayer()->GetSession()->GetAccountId(), opcode, recv_data.size() - recv_data.rpos());
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

    bool updateOrientationOnly = false;
    bool skipAnticheat = false;
    bool useFallingFlag = false;
    bool forcemovement = false;

    if (!movementInfo.HasMovementFlag(MOVEFLAG_MOVING) && !GetPlayer()->HasUnitMovementFlag(MOVEFLAG_MOVING) && !(opcode == MSG_MOVE_FALL_LAND))
    {
        #ifdef MOVEMENT_ANTICHEAT_DEBUG
        sLog.outDetail("Update Orientation Only");
        #endif
        skipAnticheat = true;
        updateOrientationOnly = true;
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
                        GetPlayer()->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

                        GetPlayer()->m_transport = (*iter);
                        (*iter)->AddPassenger(GetPlayer());
                        break;
                    }
                }
            }

            //GetPlayer()->m_anti_transportGUID = GUID_LOPART(movementInfo.t_guid);
            GameObject *obj = HashMapHolder<GameObject>::Find(movementInfo.t_guid);
            if (obj)
                GetPlayer()->m_anti_transportGUID = obj->GetDBTableGUIDLow();
            else
                GetPlayer()->m_anti_transportGUID = GUID_LOPART(movementInfo.t_guid);

            #ifdef MOVEMENT_ANTICHEAT_DEBUG
            sLog.outDetail("On Transport %d", GetPlayer()->m_anti_transportGUID ? GetPlayer()->m_anti_transportGUID : 0);
            #endif
        }
    }
    else if (GetPlayer()->m_transport)                      // if we were on a transport, leave
    {
        #ifdef MOVEMENT_ANTICHEAT_DEBUG
        sLog.outDetail("Left the Transport %d", GetPlayer()->m_anti_transportGUID ? GetPlayer()->m_anti_transportGUID : 0);
        #endif

        GetPlayer()->m_transport->RemovePassenger(GetPlayer());
        GetPlayer()->m_transport = NULL;
        GetPlayer()->m_anti_transportGUID = 0;
        movementInfo.ClearTransportData();
    }
    else if (GetPlayer()->m_anti_transportGUID != 0)
    {
        #ifdef MOVEMENT_ANTICHEAT_DEBUG
        sLog.outDetail("No more Transport %d", GetPlayer()->m_anti_transportGUID ? GetPlayer()->m_anti_transportGUID : 0);
        #endif

        GetPlayer()->m_anti_transportGUID = 0;
    }

    // fall damage generation (ignore in flight case that can be triggered also at lags in moment teleportation to another map).
    if (opcode == MSG_MOVE_FALL_LAND && !GetPlayer()->isInFlight())
    {
        if (!World::GetEnableMvAnticheat() || (!GetPlayer()->m_anti_ontaxipath && !GetPlayer()->m_anti_justteleported && !GetPlayer()->m_anti_flymounted))
        {
            GetPlayer()->HandleFallDamage(movementInfo);
        }

        // Fix Blink / Shadowstep
        if (GetPlayer()->m_anti_justteleported)
            skipAnticheat = true;

        // Fix KnockBack
        if (GetPlayer()->m_anti_isknockedback)
            forcemovement = true;

        GetPlayer()->m_anti_justteleported = false;
        GetPlayer()->m_anti_ontaxipath = false;
        GetPlayer()->m_anti_wasflymounted = false;
        GetPlayer()->m_anti_justjumped = 0;
        GetPlayer()->m_anti_jumpbase = 0;
        GetPlayer()->m_anti_isjumping = false;
        GetPlayer()->m_anti_isknockedback = false;
    }
    else if (opcode == MSG_MOVE_START_SWIM)
    {
        GetPlayer()->m_anti_isjumping = false;
    }
    else if (opcode == MSG_MOVE_JUMP)
    {
        GetPlayer()->m_anti_isjumping = true;
    }

    if (GetPlayer()->m_anti_isknockedback)
        forcemovement = true;

    if (movementInfo.HasMovementFlag(MOVEFLAG_SWIMMING) != GetPlayer()->IsInWater())
    {
        // now client not include swimming flag in case jumping under water
        GetPlayer()->SetInWater(!GetPlayer()->IsInWater() || GetPlayer()->GetBaseMap()->IsUnderWater(movementInfo.GetPos()->GetPositionX(), movementInfo.GetPos()->GetPositionY(), movementInfo.GetPos()->GetPositionZ()));
        GetPlayer()->m_anti_justjumped = 0;
        //if (GetPlayer()->GetBaseMap()->IsUnderWater(movementInfo.GetPos()->GetPositionX(), movementInfo.GetPos()->GetPositionY(), movementInfo.GetPos()->GetPositionZ()-7.0f))
        //    GetPlayer()->m_anti_BeginFallZ = INVALID_HEIGHT;
    }
    /*----------------------*/

    #ifdef MOVEMENT_ANTICHEAT_DEBUG
    sLog.outBasic("%s newcoord: tm:%d ftm:%d | %f, %f, %fo(%f) [%X][%s]| transport: %f, %f, %fo(%f)", GetPlayer()->GetName(), movementInfo.time, movementInfo.GetFallTime(), movementInfo.GetPos()->GetPositionX(), movementInfo.GetPos()->GetPositionY(), movementInfo.GetPos()->GetPositionZ(), movementInfo.GetPos()->GetOrientation(), movementInfo.GetMovementFlags(), LookupOpcodeName(opcode), movementInfo.GetTransportPos()->GetPositionX(), movementInfo.GetTransportPos()->GetPositionY(), movementInfo.GetTransportPos()->GetPositionZ(), movementInfo.GetTransportPos()->GetOrientation());
    sLog.outBasic("Transport: %d |  tguid: %d - %d", GetPlayer()->m_anti_transportGUID, GUID_LOPART(movementInfo.t_guid), GUID_HIPART(movementInfo.t_guid));
    #endif

    //---- anti-cheat features -->>>
    bool check_passed = true;

    if (World::GetEnableMvAnticheat() && updateOrientationOnly && GetPlayer()->m_anti_transportGUID == 0)
    {
        if ((abs(GetPlayer()->GetPositionX() - movementInfo.GetPos()->GetPositionX()) > 0.1f) ||
            (abs(GetPlayer()->GetPositionY() - movementInfo.GetPos()->GetPositionY()) > 0.1f) ||
            (abs(GetPlayer()->GetPositionZ() - movementInfo.GetPos()->GetPositionZ()) > 0.1f))
        {
            #ifdef MOVEMENT_ANTICHEAT_DEBUG
            sLog.outError("Passiv Movement: dx=%f, dy=%f, dz=%f",
                abs(GetPlayer()->GetPositionX() - movementInfo.GetPos()->GetPositionX()),
                abs(GetPlayer()->GetPositionY() - movementInfo.GetPos()->GetPositionY()),
                abs(GetPlayer()->GetPositionZ() - movementInfo.GetPos()->GetPositionZ()));
            #endif
            GetPlayer()->m_anti_lastcheat = "Passiv Movement Hack";
            check_passed = false;
        }
    }

    //uint32 curDest = GetPlayer()->m_taxi.GetTaxiDestination(); // check taxi flight
    //if ((GetPlayer()->m_anti_transportGUID == 0) && World::GetEnableMvAnticheat() && !curDest)
    if (World::GetEnableMvAnticheat() && (GetPlayer()->m_anti_transportGUID == 0) && !GetPlayer()->m_anti_ontaxipath && !skipAnticheat)
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

        float time_delta = movementInfo.time - GetPlayer()->m_anti_lastmovetime;
        if (time_delta > 0)
            GetPlayer()->m_anti_lastmovetime = movementInfo.time;
        else
            time_delta = 0;

        time_delta = (time_delta < 1500) ? time_delta / 1000 : 1.5f; // normalize time - 1.5 second allowed for heavy loaded server

        float tg_z = -99999; // tangens
        if (!(movementInfo.GetMovementFlags() & (MOVEFLAG_FLYING | MOVEFLAG_SWIMMING)))
            tg_z = (real_delta != 0) ? (delta_z * delta_z / real_delta) : tg_z;

        if (current_speed < GetPlayer()->m_anti_last_hspeed)
        {
            allowed_delta = GetPlayer()->m_anti_last_hspeed;
            if (GetPlayer()->m_anti_lastspeed_changetime == 0)
                GetPlayer()->m_anti_lastspeed_changetime = movementInfo.time + (uint32)floor(((GetPlayer()->m_anti_last_hspeed / current_speed) * 1000)) + 100; //100ms above for random fluctuating =)))
        }
        else
            allowed_delta = current_speed;

        allowed_delta = allowed_delta * time_delta;
        allowed_delta = allowed_delta * allowed_delta + 2;

        // Anti-Gravitation (thanks to Meekro)
        float JumpHeight = ((GetPlayer()->m_anti_jumpbase > 0) ? GetPlayer()->m_anti_jumpbase : 0) - movementInfo.GetPos()->GetPositionZ();
        if ((GetPlayer()->m_anti_jumpbase != 0)
                    && (GetPlayer()->m_anti_justjumped > 0)
                    && (!GetPlayer()->m_anti_justteleported)
                    && !(movementInfo.HasMovementFlag(MOVEFLAG_SWIMMING) || movementInfo.HasMovementFlag(MOVEFLAG_FLYING)
                        || movementInfo.HasMovementFlag(MOVEFLAG_FLYING2))
                    && (JumpHeight < -1.7f)) //max jumpheight is constant for each movementtype
        {
            #ifdef MOVEMENT_ANTICHEAT_DEBUG
            sLog.outError("Movement anticheat: %s is GraviJump exception. anti_jumpbase=%f, anti_justjumped=%u, JumpHeight=%f, last_vspeed=%f",
                GetPlayer()->GetName(), GetPlayer()->m_anti_jumpbase, GetPlayer()->m_anti_justjumped, JumpHeight, GetPlayer()->m_anti_last_vspeed);
            #endif
            GetPlayer()->m_anti_lastcheat = "GraviJump Hack";
            check_passed = false;
        }

        // Anti-Jumphack
        if (opcode == MSG_MOVE_JUMP && !GetPlayer()->IsInWater())
        {
            if (GetPlayer()->m_anti_justjumped == 0)
            {
                GetPlayer()->m_anti_justjumped += 1;
                GetPlayer()->m_anti_jumpbase = movementInfo.GetPos()->GetPositionZ();
            }
            else
            {
                #ifdef MOVEMENT_ANTICHEAT_DEBUG
                sLog.outError("Movement anticheat: %s is Jumphack exception. count=%u",GetPlayer()->GetName(), GetPlayer()->m_anti_justjumped);
                #endif
                GetPlayer()->m_anti_justjumped = 0;
                GetPlayer()->m_anti_lastcheat = "Jump Hack";
                check_passed = false; //don't process new jump packet
            }
        }
        else if (GetPlayer()->IsInWater())
            GetPlayer()->m_anti_justjumped = 0;

        // Anti-Speedhack
        if ((real_delta > allowed_delta) && (delta_z < (GetPlayer()->m_anti_last_vspeed * time_delta) || delta_z < 1))
        {
            #ifdef MOVEMENT_ANTICHEAT_DEBUG
            sLog.outError("Movement anticheat: %s is speed exception. real_delta=%f, allowed_delta=%f, delta_z=%f, last_vspeed=%f", GetPlayer()->GetName(), real_delta, allowed_delta, delta_z, GetPlayer()->m_anti_last_vspeed * time_delta);
            #endif
            GetPlayer()->m_anti_lastcheat = "Speed Hack";
            check_passed = false;
        }

        // Anti-Teleport
        if ((real_delta > allowed_delta) && (real_delta > (time_delta * 100)))
        {
            #ifdef MOVEMENT_ANTICHEAT_DEBUG
            sLog.outError("Movement anticheat: %s is teleport exception. real_delta=%f, allowed_delta=%f, min_delta=%f ", GetPlayer()->GetName(), real_delta, allowed_delta, time_delta * 100);
            #endif
            GetPlayer()->m_anti_lastcheat = "Teleport Hack";
            check_passed = false;
        }

        // Anti-Wallhack
        // Known issues: jump+up, and walking up with low delta_z (one and only way to make it right is to calculate the delta_z of the terrain)
        if (!GetPlayer()->m_anti_isjumping && (tg_z > 1.6f) && (delta_z < (GetPlayer()->m_anti_last_vspeed * time_delta)))
        {
            #ifdef MOVEMENT_ANTICHEAT_DEBUG
            sLog.outError("Movement anticheat: %s is a wall-climb cheater. tg_z=%f, delta_z=%f, last_vspeed=%f", GetPlayer()->GetName(), tg_z, delta_z, GetPlayer()->m_anti_last_vspeed * time_delta);
            #endif
            GetPlayer()->m_anti_lastcheat = "Wall-climbing Hack";
            check_passed = false;
        }

        if (movementInfo.time > GetPlayer()->m_anti_lastspeed_changetime)
        {
            GetPlayer()->m_anti_last_hspeed = current_speed; // store current speed
            GetPlayer()->m_anti_last_vspeed = -2.3f;
            GetPlayer()->m_anti_lastspeed_changetime = 0;
        }

        // Anti-Flyhack
        if (((movementInfo.GetMovementFlags() & (MOVEFLAG_CAN_FLY | MOVEFLAG_FLYING | MOVEFLAG_FLYING2)) != 0) && !(GetPlayer()->HasAuraType(SPELL_AURA_FLY) || GetPlayer()->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED)))
        {
            #ifdef MOVEMENT_ANTICHEAT_DEBUG
            sLog.outError("Movement anticheat: %s is a fly cheater. {SPELL_AURA_FLY=[%X]} {SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED=[%X]} {SPELL_AURA_MOD_FLIGHT_SPEED_STACKING=[%X]} {SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_STACKING=[%X]} {SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_NOT_STACKING=[%X]}",
               GetPlayer()->GetName(),
               GetPlayer()->HasAuraType(SPELL_AURA_FLY), GetPlayer()->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED),
               GetPlayer()->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_STACKING), GetPlayer()->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_STACKING),
               GetPlayer()->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_NOT_STACKING));
            #endif
            GetPlayer()->m_anti_lastcheat = "Fly Hack";
            useFallingFlag = true;
            check_passed = false;
        }

        // Anti-Waterwalk
        if (movementInfo.HasMovementFlag(MOVEFLAG_WATERWALKING) && !(GetPlayer()->HasAuraType(SPELL_AURA_WATER_WALK) | GetPlayer()->HasAuraType(SPELL_AURA_GHOST)))
        {
            #ifdef MOVEMENT_ANTICHEAT_DEBUG
            sLog.outError("Movement anticheat: %s is a water-walk cheater. MovementFlags=[%X], SPELL_AURA_WATER_WALK=[%X]", GetPlayer()->GetName(), movementInfo.GetMovementFlags(), GetPlayer()->HasAuraType(SPELL_AURA_WATER_WALK));
            #endif
            GetPlayer()->m_anti_lastcheat = "Water Walk Hack";
            check_passed = false;
        }

        // Anti-Featherfall
        if (movementInfo.HasMovementFlag(MOVEFLAG_SAFE_FALL) && !(GetPlayer()->HasAuraType(SPELL_AURA_FEATHER_FALL)))
        {
            #ifdef MOVEMENT_ANTICHEAT_DEBUG
            sLog.outError("Movement anticheat: %s is a featherfall-fall cheater. MovementFlags=[%X], SPELL_AURA_FEATHER_FALL=[%X]", GetPlayer()->GetName(), movementInfo.GetMovementFlags(), GetPlayer()->HasAuraType(SPELL_AURA_FEATHER_FALL));
            #endif
            GetPlayer()->m_anti_lastcheat = "Feather Fall Hack";
            check_passed = false;
        }

        // Anti-TeleportToPlane
        if (movementInfo.GetPos()->GetPositionZ() < 0.0001f && movementInfo.GetPos()->GetPositionZ() > -0.0001f && ((movementInfo.GetMovementFlags() & (MOVEFLAG_SWIMMING | MOVEFLAG_CAN_FLY | MOVEFLAG_FLYING | MOVEFLAG_FLYING2)) == 0))
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
                    sLog.outError("Movement anticheat: %s uses teleport to plane. plane_z: %f ", GetPlayer()->GetName(), plane_z);
                    #endif
                    GetPlayer()->m_anti_lastcheat = "Teleport to Plane Hack";
                    if (GetPlayer()->m_anti_teletoplane_count > World::GetTeleportToPlaneAlarms())
                    {
                        GetPlayer()->GetSession()->KickPlayer();
                        sLog.outError("Movement anticheat: %s is kicked for use to plane exception. count: %d.", GetPlayer()->GetName(), GetPlayer()->m_anti_teletoplane_count);
                    }
                }
            }
        }
        else
            GetPlayer()->m_anti_teletoplane_count = 0;

        /*if (!check_passed)
        {
            static char const* move_type_name[MAX_MOVE_TYPE] = {"Walk", "Run", "Runback", "Swim", "Swimback", "Turn", "Fly", "Flyback"};
            sLog.outError("curspeed: %f, tg_z %f, allowed_delta %f, real_delta %f, time_delta %f, delta_x %f, delta_y %f, delta_z %f, [%X][%s]$%s",
                current_speed, tg_z, allowed_delta, real_delta, time_delta, delta_x, delta_y, delta_z, movementInfo.GetMovementFlags(), LookupOpcodeName(opcode), move_type_name[move_type]);
        }*/
    }
    else if (movementInfo.HasMovementFlag(MOVEFLAG_ONTRANSPORT))
    {
        // antiwrap
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
                #ifdef MOVEMENT_ANTICHEAT_DEBUG
                sLog.outError("Movement anticheat: %s on some transport. xyz: %f, %f, %f", GetPlayer()->GetName(), go_data->posX, go_data->posY, go_data->posZ);
                #endif

                int mapid = go_data->mapid;
                if (GetPlayer()->GetMapId() != mapid)
                {
                    check_passed = false;
                }
                else if (mapid != 369)
                {
                    float delta_gox = go_data->posX - movementInfo.GetPos()->GetPositionX();
                    float delta_goy = go_data->posY - movementInfo.GetPos()->GetPositionY();
                    float delta_go = delta_gox*delta_gox + delta_goy*delta_goy;
                    if (delta_go > 3600.0f)
                        check_passed = false;
                }
            }
            else
            {
                #ifdef MOVEMENT_ANTICHEAT_DEBUG
                sLog.outError("Movement anticheat: %s on undefined transport.", GetPlayer()->GetName());
                #endif
                check_passed = false;
            }
        }

        if (!check_passed)
        {
            GetPlayer()->m_anti_lastcheat = "Transport";
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
    if (check_passed || GetPlayer()->isGameMaster() || forcemovement)
    {
        if (updateOrientationOnly)
        {
            /*
            WorldPacket data;
            GetPlayer()->BuildHeartBeatMsg(&data);
            GetPlayer()->SendMessageToSet(&data, true);
            */

            recv_data.put<uint32>(5, getMSTime());                  // offset flags(4) + unk(1)
            WorldPacket data(opcode, (GetPlayer()->GetPackGUID().size() + recv_data.size()));
            data << GetPlayer()->GetPackGUID();
            data.append(recv_data.contents(), recv_data.size());
            GetPlayer()->SendMessageToSet(&data, false);

            GetPlayer()->SetPosition(GetPlayer()->GetPositionX(), GetPlayer()->GetPositionY(), GetPlayer()->GetPositionZ(), movementInfo.GetPos()->GetOrientation());
        }
        else
        {
            recv_data.put<uint32>(5, getMSTime());                  // offset flags(4) + unk(1)
            WorldPacket data(opcode, (GetPlayer()->GetPackGUID().size() + recv_data.size()));
            data << GetPlayer()->GetPackGUID();
            data.append(recv_data.contents(), recv_data.size());
            GetPlayer()->SendMessageToSet(&data, false);
            GetPlayer()->SetPosition(movementInfo.GetPos()->GetPositionX(), movementInfo.GetPos()->GetPositionY(), movementInfo.GetPos()->GetPositionZ(), movementInfo.GetPos()->GetOrientation());
        }

        GetPlayer()->m_movementInfo = movementInfo;

        if (opcode == MSG_MOVE_FALL_LAND || GetPlayer()->m_lastFallTime > movementInfo.GetFallTime() || GetPlayer()->m_lastFallZ < movementInfo.GetPos()->GetPositionZ())
            GetPlayer()->SetFallInformation(movementInfo.GetFallTime(), movementInfo.GetPos()->GetPositionZ());

        // we should add the check only for class hunter
        if (GetPlayer()->isMovingOrTurning())
            GetPlayer()->RemoveSpellsCausingAura(SPELL_AURA_FEIGN_DEATH);

        if (movementInfo.GetPos()->GetPositionZ() < -500.0f)
            GetPlayer()->HandleFallUnderMap();

        if (GetPlayer()->m_anti_alarmcount > 0 && !GetPlayer()->isGameMaster())
        {
            if (GetPlayer()->m_anti_alarmcount > World::GetMaxMovementAlarms())
            {
                GetPlayer()->GetSession()->KickPlayer();
                sLog.outError("Movement anticheat: %s kicked: %d anticheat alarms!", GetPlayer()->GetName(), GetPlayer()->m_anti_alarmcount);
            }
            else
            {
                sLog.outError("Movement anticheat: %s produces a anticheat alarm for %s.", GetPlayer()->GetName(), GetPlayer()->m_anti_lastcheat.c_str());
            }
        }

        GetPlayer()->m_anti_alarmcount = 0;
    }
    else
    {
        GetPlayer()->m_anti_alarmcount++;
        WorldPacket data;
        if (useFallingFlag)
            GetPlayer()->SetUnitMovementFlags(MOVEFLAG_FALLING);
        else
            GetPlayer()->SetUnitMovementFlags(MOVEFLAG_NONE);
        GetPlayer()->BuildTeleportAckMsg(&data, GetPlayer()->GetPositionX(), GetPlayer()->GetPositionY(), GetPlayer()->GetPositionZ(), GetPlayer()->GetOrientation());
        GetPlayer()->GetSession()->SendPacket(&data);
        GetPlayer()->BuildHeartBeatMsg(&data);
        GetPlayer()->SendMessageToSet(&data, true);
    }

    if (getMSTimeDiff(timediff, getMSTime()) > 20)
        sLog.outError("Anticheat Process Time: %ums : %s [%x]", getMSTimeDiff(timediff, getMSTime()), LookupOpcodeName(opcode), movementInfo.GetMovementFlags());
}

void WorldSession::HandlePossessedMovement(WorldPacket& recv_data, MovementInfo& movementInfo)
{
    DEBUG_LOG("WORLD: Recvd HandlePossessedMovement");
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
    DEBUG_LOG("WORLD: Recvd CMSG_SPEED_CHANGE_ACK");
    /* extract packet */
    ObjectGuid guid;
    MovementInfo movementInfo;
    float newspeed;

    recv_data >> guid;
    recv_data >> Unused<uint32>();                          // counter or moveEvent
    recv_data >> movementInfo;
    recv_data >> newspeed;

    // now can skip not our packet
    if (GetPlayer()->GetGUID() != guid.GetRawValue())
        return;
    /*----------------*/

    // Save movement flags
    GetPlayer()->SetUnitMovementFlags(movementInfo.GetMovementFlags());

    #ifdef MOVEMENT_ANTICHEAT_DEBUG
    sLog.outBasic("%s CMSG_SPEED_CHANGE_ACK: tm:%d ftm:%d | %f, %f, %fo(%f) [%X]", GetPlayer()->GetName(), movementInfo.time, movementInfo.GetFallTime(), movementInfo.GetPos()->GetPositionX(), movementInfo.GetPos()->GetPositionY(), movementInfo.GetPos()->GetPositionZ(), movementInfo.GetPos()->GetOrientation(), movementInfo.GetMovementFlags());
    sLog.outBasic("%s CMSG_SPEED_CHANGE_ACK additional: vspeed:%f, hspeed:%f, xdir:%f ydir:%f", GetPlayer()->GetName(), movementInfo.j_velocity, movementInfo.j_xyspeed, movementInfo.j_sinAngle, movementInfo.j_cosAngle);
    #endif

    // skip not personal message;
    GetPlayer()->m_movementInfo = movementInfo;
    GetPlayer()->m_anti_last_hspeed = movementInfo.j_xyspeed;
    GetPlayer()->m_anti_last_vspeed = movementInfo.j_velocity < 3.2f ? movementInfo.j_xyspeed - 1.0f : 3.2f;
    GetPlayer()->m_anti_lastspeed_changetime = movementInfo.time + 1750;

    // client ACK send one packet for mounted/run case and need skip all except last from its
    // in other cases anti-cheat check can be fail in false case
    UnitMoveType move_type;
    UnitMoveType force_move_type;

    static char const* move_type_name[MAX_MOVE_TYPE] = {"Walk", "Run", "RunBack", "Swim", "SwimBack", "TurnRate", "Flight", "FlightBack"};

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
    // in run/mounted case used one ACK and it must be skipped.m_forced_speed_changes[MOVE_RUN] store both.
    if (GetPlayer()->m_forced_speed_changes[force_move_type] > 0)
    {
        --GetPlayer()->m_forced_speed_changes[force_move_type];
        if (GetPlayer()->m_forced_speed_changes[force_move_type] > 0)
            return;
    }

/*  Not needed any more, the anticheat handles the speed and the if will never be true

    if (!GetPlayer()->GetTransport() && fabs(GetPlayer()->GetSpeed(move_type) - newspeed) > 0.01f)
    {
        if (GetPlayer()->GetSpeed(move_type) > newspeed)         // must be greater - just correct
        {
            sLog.outError("%sSpeedChange player %s is NOT correct (must be %f instead %f), force set to correct value",
                move_type_name[move_type], GetPlayer()->GetName(), GetPlayer()->GetSpeed(move_type), newspeed);
            GetPlayer()->SetSpeed(move_type, GetPlayer()->GetSpeedRate(move_type), true);
        }
        else                                                // must be lesser - cheating
        {
            sLog.outBasic("Player %s from account id %u kicked for incorrect speed (must be %f instead %f)",
                GetPlayer()->GetName(), GetPlayer()->GetSession()->GetAccountId(), GetPlayer()->GetSpeed(move_type), newspeed);
            GetPlayer()->GetSession()->KickPlayer();
        }
    }
*/
}

void WorldSession::HandleSetActiveMoverOpcode(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: Recvd CMSG_SET_ACTIVE_MOVER");

    recv_data >> Unused<uint64>();                          // guid
}

void WorldSession::HandleMoveNotActiveMoverOpcode(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: Recvd CMSG_MOVE_NOT_ACTIVE_MOVER");
    recv_data.hexlike();

    uint64 old_mover_guid;
    MovementInfo mi;

    old_mover_guid = recv_data.readPackGUID();
    recv_data >> mi;

    if (!old_mover_guid)
        return;

    GetPlayer()->m_movementInfo = mi;
}

void WorldSession::HandleMountSpecialAnimOpcode(WorldPacket& /*recvdata*/)
{
    DEBUG_LOG("WORLD: Recvd CMSG_MOUNTSPECIAL_ANIM");

    WorldPacket data(SMSG_MOUNTSPECIAL_ANIM, 8);
    data << uint64(GetPlayer()->GetGUID());

    GetPlayer()->SendMessageToSet(&data, false);
}

void WorldSession::HandleMoveKnockBackAck(WorldPacket & recv_data)
{
    // Currently not used but maybe use later for recheck final player position
    // (must be at call same as into "recv_data >> x >> y >> z >> orientation;"
    DEBUG_LOG("CMSG_MOVE_KNOCK_BACK_ACK");

    MovementInfo movementInfo;
    uint64 guid;

    recv_data >> guid;                                      // guid
    recv_data >> Unused<uint32>();                          // unk
    recv_data >> movementInfo;

    if (GetPlayer()->GetGUID() != guid)
        return;

    // Save movement flags
    GetPlayer()->SetUnitMovementFlags(movementInfo.GetMovementFlags());
    GetPlayer()->m_anti_isknockedback = true;
    //Todo: Store the final Position and check it with the Position of MSG_MOVE_FALL_LAND in HandleMovementOpcodes

    #ifdef MOVEMENT_ANTICHEAT_DEBUG
    sLog.outBasic("%s CMSG_MOVE_KNOCK_BACK_ACK: tm:%d ftm:%d | %f, %f, %fo(%f) [%X]", GetPlayer()->GetName(), movementInfo.time, movementInfo.GetFallTime(), movementInfo.GetPos()->GetPositionX(), movementInfo.GetPos()->GetPositionY(), movementInfo.GetPos()->GetPositionZ(), movementInfo.GetPos()->GetOrientation(), movementInfo.GetMovementFlags());
    sLog.outBasic("%s CMSG_MOVE_KNOCK_BACK_ACK additional: vspeed:%f, hspeed:%f, xdir:%f ydir:%f", GetPlayer()->GetName(), movementInfo.j_velocity, movementInfo.j_xyspeed, movementInfo.j_sinAngle, movementInfo.j_cosAngle);
    #endif

    // skip not personal message;
    GetPlayer()->m_movementInfo = movementInfo;
    GetPlayer()->m_anti_last_hspeed = movementInfo.j_xyspeed;
    GetPlayer()->m_anti_last_vspeed = movementInfo.j_velocity < 3.2f ? movementInfo.j_xyspeed - 1.0f : 3.2f;
    GetPlayer()->m_anti_lastspeed_changetime = movementInfo.time + 1750;
}

void WorldSession::HandleMoveFlyModeChangeAckOpcode(WorldPacket & recv_data)
{
    // fly mode on/off
    DEBUG_LOG("WORLD: CMSG_MOVE_SET_CAN_FLY_ACK");
    //recv_data.hexlike();

    MovementInfo movementInfo;

    recv_data.read_skip<uint64>();                          // guid
    recv_data.read_skip<uint32>();                          // unk
    recv_data >> movementInfo;
    recv_data.read_skip<uint32>();                          // unk2

    // movement anticheat code
    if (movementInfo.HasMovementFlag(MOVEFLAG_FLYING))
    {
        GetPlayer()->m_anti_flymounted = true;
    }
    else
    {
        GetPlayer()->m_anti_flymounted = false;
        GetPlayer()->m_anti_wasflymounted = true;
    }
    //<<< end movement anticheat

    GetPlayer()->SetUnitMovementFlags(movementInfo.GetMovementFlags());
}

void WorldSession::HandleMoveHoverAck(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_MOVE_HOVER_ACK");

    MovementInfo movementInfo;

    recv_data >> Unused<uint64>();                          // guid
    recv_data >> Unused<uint32>();                          // unk
    recv_data >> movementInfo;
    recv_data >> Unused<uint32>();                          // unk2
}

void WorldSession::HandleMoveWaterWalkAck(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_MOVE_WATER_WALK_ACK");

    MovementInfo movementInfo;

    recv_data >> Unused<uint64>();                          // guid
    recv_data >> Unused<uint32>();                          // unk
    recv_data >> movementInfo;
    recv_data >> Unused<uint32>();                          // unk2
}

void WorldSession::HandleSummonResponseOpcode(WorldPacket& recv_data)
{
    if (!GetPlayer()->isAlive() || GetPlayer()->isInCombat())
        return;

    uint64 summoner_guid;
    bool agree;
    recv_data >> summoner_guid;
    recv_data >> agree;

    GetPlayer()->SummonIfPossible(agree);
}

