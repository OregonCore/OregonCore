/*
 * Copyright (C) 2010-2014 OregonCore <http://www.oregoncore.com/>
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
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
#include "Corpse.h"
#include "Player.h"
#include "UpdateMask.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "Database/DatabaseEnv.h"
#include "Opcodes.h"
#include "WorldSession.h"
#include "WorldPacket.h"
#include "GossipDef.h"
#include "World.h"

Corpse::Corpse(CorpseType type) : WorldObject()
, m_type(type)
{
    m_objectType |= TYPEMASK_CORPSE;
    m_objectTypeId = TYPEID_CORPSE;
                                                            // 2.3.2 - 0x58
    m_updateFlag = (UPDATEFLAG_LOWGUID | UPDATEFLAG_HIGHGUID | UPDATEFLAG_HAS_POSITION);

    m_valuesCount = CORPSE_END;

    m_time = time(NULL);

    lootForBody = false;

    if (type != CORPSE_BONES)
        m_isWorldObject = true;
}

Corpse::~Corpse()
{
}

void Corpse::AddToWorld()
{
    // Register the corpse for guid lookup
    if (!IsInWorld())
        ObjectAccessor::Instance().AddObject(this);

    Object::AddToWorld();
}

void Corpse::RemoveFromWorld()
{
    // Remove the corpse from the accessor
    if (IsInWorld())
        ObjectAccessor::Instance().RemoveObject(this);

    Object::RemoveFromWorld();
}

bool Corpse::Create(uint32 guidlow, Map *map)
{
    SetMap(map);
    Object::_Create(guidlow, 0, HIGHGUID_CORPSE);
    return true;
}

bool Corpse::Create(uint32 guidlow, Player *owner, uint32 /*mapid*/, float x, float y, float z, float ang)
{
    ASSERT(owner);

    Relocate(x,y,z,ang);

    if (!IsPositionValid())
    {
        sLog.outError("Corpse (guidlow %d, owner %s) not created. Suggested coordinates isn't valid (X: %f Y: %f)",
            guidlow, owner->GetName(), x, y);
        return false;
    }

    //we need to assign owner's map for corpse
    //in other way we will get a crash in Corpse::SaveToDB()
    SetMap(owner->GetMap());

    WorldObject::_Create(guidlow, HIGHGUID_CORPSE);

    SetFloatValue(OBJECT_FIELD_SCALE_X, 1);
    SetFloatValue(CORPSE_FIELD_POS_X, x);
    SetFloatValue(CORPSE_FIELD_POS_Y, y);
    SetFloatValue(CORPSE_FIELD_POS_Z, z);
    SetFloatValue(CORPSE_FIELD_FACING, ang);
    SetUInt64Value(CORPSE_FIELD_OWNER, owner->GetGUID());

    m_grid = Oregon::ComputeGridPair(GetPositionX(), GetPositionY());

    return true;
}

void Corpse::SaveToDB()
{
    // prevent DB data inconsistance problems and duplicates
    CharacterDatabase.BeginTransaction();
    DeleteFromDB();

    std::ostringstream ss;
    ss  << "INSERT INTO corpse (guid,player,position_x,position_y,position_z,orientation,zone,map,displayId,itemCache,bytes1,bytes2,guild,flags,dynFlags,time,corpse_type,instance) VALUES ("
        << GetGUIDLow() << ", "
        << GUID_LOPART(GetOwnerGUID()) << ", "
        << GetPositionX() << ", "
        << GetPositionY() << ", "
        << GetPositionZ() << ", "
        << GetOrientation() << ", "
        << GetZoneId() << ", "
        << GetMapId() << ", "
        << GetUInt32Value(CORPSE_FIELD_DISPLAY_ID) << ", '";
    for (uint16 i = 0; i < EQUIPMENT_SLOT_END; ++i)
        ss << GetUInt32Value(CORPSE_FIELD_ITEM+i) << " ";
    ss  << "', "
        << GetUInt32Value(CORPSE_FIELD_BYTES_1) << ", "
        << GetUInt32Value(CORPSE_FIELD_BYTES_2) << ", "
        << GetUInt32Value(CORPSE_FIELD_GUILD) << ", "
        << GetUInt32Value(CORPSE_FIELD_FLAGS) << ", "
        << GetUInt32Value(CORPSE_FIELD_DYNAMIC_FLAGS) << ", "
        << uint64(m_time) << ", "
        << uint32(GetType()) << ", "
        << int(GetInstanceId()) << ")";
    CharacterDatabase.Execute(ss.str().c_str());
    CharacterDatabase.CommitTransaction();
}

void Corpse::DeleteBonesFromWorld()
{
    ASSERT(GetType() == CORPSE_BONES);
    Corpse* corpse = ObjectAccessor::GetCorpse(*this, GetGUID());

    if (!corpse)
    {
        sLog.outError("Bones %u not found in world.", GetGUIDLow());
        return;
    }

    AddObjectToRemoveList();
}

void Corpse::DeleteFromDB()
{
    if (GetType() == CORPSE_BONES)
        // only specific bones
        CharacterDatabase.PExecute("DELETE FROM corpse WHERE guid = '%d'", GetGUIDLow());
    else
        // all corpses (not bones)
        CharacterDatabase.PExecute("DELETE FROM corpse WHERE player = '%d' AND corpse_type <> '0'",  GUID_LOPART(GetOwnerGUID()));
}

bool Corpse::LoadFromDB(uint32 guid, Field *fields)
{
    //       0           1           2           3            4    5          6          7       8       9      10     11        12    13           14        15    16
    //SELECT position_x, position_y, position_z, orientation, map, displayId, itemCache, bytes1, bytes2, guild, flags, dynFlags, time, corpse_type, instance, guid, player FROM corpse WHERE corpse_type <> 0
    float positionX = fields[0].GetFloat();
    float positionY = fields[1].GetFloat();
    float positionZ = fields[2].GetFloat();
    float ort       = fields[3].GetFloat();
    uint32 mapid    = fields[4].GetUInt32();

    // Initialize the datastores for this object
    WorldObject::_Create(guid, HIGHGUID_CORPSE);

    SetUInt32Value(CORPSE_FIELD_DISPLAY_ID, fields[5].GetUInt32());
    _LoadIntoDataField(fields[6].GetString(), CORPSE_FIELD_ITEM, EQUIPMENT_SLOT_END);
    SetUInt32Value(CORPSE_FIELD_BYTES_1, fields[7].GetUInt32());
    SetUInt32Value(CORPSE_FIELD_BYTES_2, fields[8].GetUInt32());
    SetUInt32Value(CORPSE_FIELD_GUILD, fields[9].GetUInt32());
    SetUInt32Value(CORPSE_FIELD_FLAGS, fields[10].GetUInt32());
    SetUInt32Value(CORPSE_FIELD_DYNAMIC_FLAGS, fields[11].GetUInt32());
    SetUInt64Value(CORPSE_FIELD_OWNER, MAKE_NEW_GUID(fields[16].GetUInt32(), 0, HIGHGUID_PLAYER));

    m_time = time_t(fields[12].GetUInt64());
    m_type = CorpseType(fields[13].GetUInt32());

    if (m_type >= MAX_CORPSE_TYPE)
    {
        sLog.outError("Corpse (guidlow %d, owner %d) has wrong corpse type.  Not loaded.",GetGUIDLow(),GUID_LOPART(GetOwnerGUID()));
        return false;
    }

    if (m_type != CORPSE_BONES)
        m_isWorldObject = true;

    uint32 instanceid  = fields[14].GetUInt32();

    // place
    SetLocationInstanceId(instanceid);
    SetLocationMapId(mapid);
    Relocate(positionX, positionY, positionZ, ort);

    if (!IsPositionValid())
    {
        sLog.outError("Corpse (guidlow %d, owner %d) not created. Suggested coordinates isn't valid (X: %f Y: %f)",
            GetGUIDLow(), GUID_LOPART(GetOwnerGUID()), GetPositionX(), GetPositionY());
        return false;
    }

    m_grid = Oregon::ComputeGridPair(GetPositionX(), GetPositionY());
    return true;
}

bool Corpse::isVisibleForInState(Player const* u, bool inVisibleList) const
{
    return IsInWorld() && u->IsInWorld() && IsWithinDistInMap(u->m_seer, World::GetMaxVisibleDistanceForObject() + (inVisibleList ? World::GetVisibleObjectGreyDistance() : 0.0f), false);
}

bool Corpse::IsExpired(time_t t) const
{
    if (m_type == CORPSE_BONES)
        return m_time < t - 60 * MINUTE;
    else
        return m_time < t - 3 * DAY;
}

