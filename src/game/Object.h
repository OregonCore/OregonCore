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

#ifndef _OBJECT_H
#define _OBJECT_H

#include "Common.h"
#include "Position.h"
#include "ByteBuffer.h"
#include "UpdateFields.h"
#include "UpdateData.h"
#include "GameSystem/GridReference.h"
#include "ObjectGuid.h"
#include "GridDefines.h"
#include "Map.h"

#include <set>
#include <string>

#define CONTACT_DISTANCE            0.5f
#define INTERACTION_DISTANCE        5.0f
#define ATTACK_DISTANCE             5.0f
#define INSPECT_DISTANCE            28.0f
#define MAX_VISIBILITY_DISTANCE     SIZE_OF_GRIDS       // max distance for visible object show
#define SIGHT_RANGE_UNIT            50.0f
#define DEFAULT_VISIBILITY_DISTANCE 90.0f                   // default visible distance, 90 yards on continents
#define DEFAULT_VISIBILITY_INSTANCE 170.0f              // default visible distance in instances, 120 yards
#define DEFAULT_VISIBILITY_BGARENAS 533.0f              // default visible distance in BG/Arenas, 180 yards

#define DEFAULT_WORLD_OBJECT_SIZE   0.388999998569489f      // player size, also currently used (correctly?) for any non Unit world objects
#define DEFAULT_COMBAT_REACH        1.5f
#define MIN_MELEE_REACH             2.0f
#define NOMINAL_MELEE_RANGE         5.0f
#define MELEE_RANGE                 (NOMINAL_MELEE_RANGE - MIN_MELEE_REACH * 2) //center to center for players

#define GRID_SEARCH_ALL_ENTRIES     0

uint32 GuidHigh2TypeId(uint32 guid_hi);

enum NotifyFlags
{
    NOTIFY_NONE                     = 0x00,
    NOTIFY_AI_RELOCATION            = 0x01,
    NOTIFY_VISIBILITY_CHANGED       = 0x02,
    NOTIFY_ALL                      = 0xFF
};

enum MovementFlags
{
    MOVEMENTFLAG_NONE                   = 0x00000000,
    MOVEMENTFLAG_FORWARD                = 0x00000001,
    MOVEMENTFLAG_BACKWARD               = 0x00000002,
    MOVEMENTFLAG_STRAFE_LEFT            = 0x00000004,
    MOVEMENTFLAG_STRAFE_RIGHT           = 0x00000008,
    MOVEMENTFLAG_TURN_LEFT              = 0x00000010,
    MOVEMENTFLAG_TURN_RIGHT             = 0x00000020,
    MOVEMENTFLAG_PITCH_UP               = 0x00000040,
    MOVEMENTFLAG_PITCH_DOWN             = 0x00000080,
    MOVEMENTFLAG_WALK_MODE              = 0x00000100,                   // Walking
    MOVEMENTFLAG_ONTRANSPORT            = 0x00000200,                   // Used for flying on some creatures
    MOVEMENTFLAG_LEVITATING             = 0x00000400,
    MOVEMENTFLAG_ROOT                   = 0x00000800,
    MOVEMENTFLAG_FALLING                = 0x00001000,
    MOVEMENTFLAG_FALLINGFAR             = 0x00004000,
    MOVEMENTFLAG_SWIMMING               = 0x00200000,                   // appears with fly flag also
    MOVEMENTFLAG_ASCENDING              = 0x00400000,                   // swim up also
    MOVEMENTFLAG_CAN_FLY                = 0x00800000,
    MOVEMENTFLAG_FLYING                 = 0x01000000,
    MOVEMENTFLAG_FLYING2                = 0x02000000,                   // Actual flying mode
    MOVEMENTFLAG_SPLINE_ELEVATION       = 0x04000000,                   // used for flight paths
    MOVEMENTFLAG_SPLINE_ENABLED         = 0x08000000,                   // used for flight paths
    MOVEMENTFLAG_WATERWALKING           = 0x10000000,                   // prevent unit from falling through water
    MOVEMENTFLAG_SAFE_FALL              = 0x20000000,                   // Feather Fall (spell)
    MOVEMENTFLAG_HOVER                  = 0x40000000,

    MOVEMENTFLAG_MOVING                 =
        MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD  | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT |
        MOVEMENTFLAG_PITCH_UP | MOVEMENTFLAG_PITCH_DOWN |
        MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FALLINGFAR | MOVEMENTFLAG_ASCENDING   |
        MOVEMENTFLAG_SPLINE_ELEVATION,

    MOVEMENTFLAG_TURNING        =
        MOVEMENTFLAG_TURN_LEFT | MOVEMENTFLAG_TURN_RIGHT,

    MOVEMENTFLAG_MASK_MOVING_FLY =
        MOVEMENTFLAG_FLYING2 | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_CAN_FLY,
};

namespace Movement
{
    class MoveSpline;
}

class WorldPacket;
class UpdateData;
class ByteBuffer;
class WorldSession;
class Creature;
class Player;
class UpdateMask;
class InstanceData;
class GameObject;
class TempSummon;
class CreatureAI;
class ZoneScript;
class Unit;

typedef UNORDERED_MAP<Player*, UpdateData> UpdateDataMapType;

class Object
{
    public:
        virtual ~Object ();

        const bool& IsInWorld() const
        {
            return m_inWorld;
        }
        virtual void AddToWorld()
        {
            if (m_inWorld)
                return;

            ASSERT(m_uint32Values);

            m_inWorld = true;

            // synchronize values mirror with values array (changes will send in updatecreate opcode any way
            ClearUpdateMask(true);
        }
        virtual void RemoveFromWorld()
        {
            if (!m_inWorld)
                return;

            m_inWorld = false;

            // if we remove from world then sending changes not required
            ClearUpdateMask(true);
        }

        const uint64& GetGUID() const
        {
            return GetUInt64Value(0);
        }
        uint32 GetGUIDLow() const
        {
            return GUID_LOPART(GetUInt64Value(0));
        }
        uint32 GetGUIDMid() const
        {
            return GUID_ENPART(GetUInt64Value(0));
        }
        uint32 GetGUIDHigh() const
        {
            return GUID_HIPART(GetUInt64Value(0));
        }
        PackedGuid const& GetPackGUID() const
        {
            return m_PackGUID;
        }
        ObjectGuid const& GetObjectGUID() const
        {
            return GetGuidValue(OBJECT_FIELD_GUID);
        }

        std::string GetGuidStr() const
        {
            return GetObjectGUID().GetString();
        }

        uint32 GetEntry() const
        {
            return GetUInt32Value(OBJECT_FIELD_ENTRY);
        }
        void SetEntry(uint32 entry)
        {
            SetUInt32Value(OBJECT_FIELD_ENTRY, entry);
        }

        float GetObjectScale() const
        {
            return GetFloatValue(OBJECT_FIELD_SCALE_X);
        }
        void SetObjectScale(float scale)
        {
            SetFloatValue(OBJECT_FIELD_SCALE_X, scale);
        }

        uint8 GetTypeId() const
        {
            return m_objectTypeId;
        }
        bool isType(uint16 mask) const
        {
            return (mask & m_objectType);
        }

        virtual void BuildCreateUpdateBlockForPlayer(UpdateData* data, Player* target) const;
        void SendUpdateToPlayer(Player* player);

        void BuildValuesUpdateBlockForPlayer(UpdateData* data, Player* target) const;
        void BuildOutOfRangeUpdateBlock(UpdateData* data) const;
        void BuildMovementUpdateBlock(UpdateData* data, uint32 flags = 0) const;

        virtual void DestroyForPlayer(Player* target, bool onDeath = false) const;

        const int32& GetInt32Value(uint16 index) const
        {
            ASSERT(index < m_valuesCount || PrintIndexError(index , false));
            return m_int32Values[ index ];
        }

        const uint32& GetUInt32Value(uint16 index) const
        {
            ASSERT(index < m_valuesCount || PrintIndexError(index , false));
            return m_uint32Values[ index ];
        }

        const uint64& GetUInt64Value(uint16 index) const
        {
            ASSERT(index + 1 < m_valuesCount || PrintIndexError(index , false));
            return *((uint64*) & (m_uint32Values[ index ]));
        }

        const float& GetFloatValue(uint16 index) const
        {
            ASSERT(index < m_valuesCount || PrintIndexError(index , false));
            return m_floatValues[ index ];
        }

        uint8 GetByteValue(uint16 index, uint8 offset) const
        {
            ASSERT(index < m_valuesCount || PrintIndexError(index , false));
            ASSERT(offset < 4);
            return *(((uint8*)&m_uint32Values[ index ]) + offset);
        }

        ObjectGuid const& GetGuidValue(uint16 index) const
        {
            return *reinterpret_cast<ObjectGuid const*>(&GetUInt64Value(index));
        }

        uint16 GetUInt16Value(uint16 index, uint8 offset) const
        {
            ASSERT(index < m_valuesCount || PrintIndexError(index , false));
            ASSERT(offset < 2);
            return *(((uint16*)&m_uint32Values[ index ]) + offset);
        }

        void SetInt32Value( uint16 index,        int32  value);
        void SetUInt32Value(uint16 index,       uint32  value);
        void SetUInt64Value(uint16 index, const uint64& value);
        void SetFloatValue( uint16 index,       float   value);
        void SetByteValue(  uint16 index, uint8 offset, uint8 value);
        void SetUInt16Value(uint16 index, uint8 offset, uint16 value);
        void SetInt16Value( uint16 index, uint8 offset, int16 value)
        {
            SetUInt16Value(index, offset, (uint16)value);
        }
        void SetStatFloatValue(uint16 index, float value);
        void SetStatInt32Value(uint16 index, int32 value);

        bool AddUInt64Value(uint16 index, const uint64& value);
        bool RemoveUInt64Value(uint16 index, const uint64& value);

        void ApplyModUInt32Value(uint16 index, int32 val, bool apply);
        void ApplyModInt32Value(uint16 index, int32 val, bool apply);
        void ApplyModUInt64Value(uint16 index, int32 val, bool apply);
        void ApplyModPositiveFloatValue(uint16 index, float val, bool apply);
        void ApplyModSignedFloatValue(uint16 index, float val, bool apply);
        void ApplyPercentModFloatValue(uint16 index, float val, bool apply);

        void SetFlag(uint16 index, uint32 newFlag);
        void RemoveFlag(uint16 index, uint32 oldFlag);

        void ToggleFlag(uint16 index, uint32 flag)
        {
            if (HasFlag(index, flag))
                RemoveFlag(index, flag);
            else
                SetFlag(index, flag);
        }

        bool HasFlag(uint16 index, uint32 flag) const
        {
            ASSERT(index < m_valuesCount || PrintIndexError(index , false));
            return (m_uint32Values[ index ] & flag) != 0;
        }

        void SetByteFlag(uint16 index, uint8 offset, uint8 newFlag);
        void RemoveByteFlag(uint16 index, uint8 offset, uint8 newFlag);

        void ToggleFlag(uint16 index, uint8 offset, uint8 flag)
        {
            if (HasByteFlag(index, offset, flag))
                RemoveByteFlag(index, offset, flag);
            else
                SetByteFlag(index, offset, flag);
        }

        bool HasByteFlag(uint16 index, uint8 offset, uint8 flag) const
        {
            ASSERT(index < m_valuesCount || PrintIndexError(index , false));
            ASSERT(offset < 4);
            return (((uint8*)&m_uint32Values[index])[offset] & flag) != 0;
        }

        void ApplyModFlag(uint16 index, uint32 flag, bool apply)
        {
            if (apply) SetFlag(index, flag);
            else RemoveFlag(index, flag);
        }

        void SetFlag64(uint16 index, uint64 newFlag)
        {
            uint64 oldval = GetUInt64Value(index);
            uint64 newval = oldval | newFlag;
            SetUInt64Value(index, newval);
        }

        void RemoveFlag64(uint16 index, uint64 oldFlag)
        {
            uint64 oldval = GetUInt64Value(index);
            uint64 newval = oldval & ~oldFlag;
            SetUInt64Value(index, newval);
        }

        void ToggleFlag64(uint16 index, uint64 flag)
        {
            if (HasFlag64(index, flag))
                RemoveFlag64(index, flag);
            else
                SetFlag64(index, flag);
        }

        bool HasFlag64(uint16 index, uint64 flag) const
        {
            ASSERT(index < m_valuesCount || PrintIndexError(index , false));
            return (GetUInt64Value(index) & flag) != 0;
        }

        void ApplyModFlag64(uint16 index, uint64 flag, bool apply)
        {
            if (apply) SetFlag64(index, flag);
            else RemoveFlag64(index, flag);
        }

        void ClearUpdateMask(bool remove);

        bool LoadValues(const char* data);

        uint16 GetValuesCount() const
        {
            return m_valuesCount;
        }

        virtual bool hasQuest(uint32 /* quest_id */) const
        {
            return false;
        }
        virtual bool hasInvolvedQuest(uint32 /* quest_id */) const
        {
            return false;
        }
        virtual void BuildUpdate(UpdateDataMapType&) {}
        void BuildFieldsUpdate(Player*, UpdateDataMapType&) const;

        // FG: some hacky helpers
        void ForceValuesUpdateAtIndex(uint32);

        Player* ToPlayer() { if (GetTypeId() == TYPEID_PLAYER) return reinterpret_cast<Player*>(this); else return NULL; }
        Player const* ToPlayer() const { if (GetTypeId() == TYPEID_PLAYER) return reinterpret_cast<Player const*>(this); else return NULL; }

        Creature* ToCreature() { if (GetTypeId() == TYPEID_UNIT) return reinterpret_cast<Creature*>(this); else return NULL; }
        Creature const* ToCreature() const { if (GetTypeId() == TYPEID_UNIT) return reinterpret_cast<Creature const*>(this); else return NULL; }

        Unit* ToUnit() { if (isType(TYPEMASK_UNIT)) return reinterpret_cast<Unit*>(this); else return NULL; }
        Unit const* ToUnit() const { if (isType(TYPEMASK_UNIT)) return reinterpret_cast<Unit const*>(this); else return NULL; }

        GameObject* ToGameObject() { if (GetTypeId() == TYPEID_GAMEOBJECT) return reinterpret_cast<GameObject*>(this); else return NULL; }
        GameObject const* ToGameObject() const { if (GetTypeId() == TYPEID_GAMEOBJECT) return reinterpret_cast<GameObject const*>(this); else return NULL; }

        Corpse* ToCorpse() { if (GetTypeId() == TYPEID_CORPSE) return reinterpret_cast<Corpse*>(this); else return NULL; }
        Corpse const* ToCorpse() const { if (GetTypeId() == TYPEID_CORPSE) return reinterpret_cast<Corpse const*>(this); else return NULL; }

        DynamicObject* ToDynObject() { if (GetTypeId() == TYPEID_DYNAMICOBJECT) return reinterpret_cast<DynamicObject*>(this); else return NULL; }
        DynamicObject const* ToDynObject() const { if (GetTypeId() == TYPEID_DYNAMICOBJECT) return reinterpret_cast<DynamicObject const*>(this); else return NULL; }

    protected:

        Object ();

        void _InitValues();
        void _Create (uint32 guidlow, uint32 entry, HighGuid guidhigh);
        void _LoadIntoDataField(const char* data, uint32 startOffset, uint32 count);

        virtual void _SetUpdateBits(UpdateMask* updateMask, Player* target) const;

        virtual void _SetCreateBits(UpdateMask* updateMask, Player* target) const;
        void _BuildMovementUpdate(ByteBuffer* data, uint8 updateFlags) const;
        void _BuildValuesUpdate(uint8 updatetype, ByteBuffer* data, UpdateMask* updateMask, Player* target) const;

        uint16 m_objectType;

        uint8 m_objectTypeId;
        uint8 m_updateFlag;

        union
        {
            int32*  m_int32Values;
            uint32* m_uint32Values;
            float*  m_floatValues;
        };

        uint32* m_uint32Values_mirror;

        uint16 m_valuesCount;

        bool m_objectUpdated;

    private:
        bool m_inWorld;

        PackedGuid m_PackGUID;

        // for output helpfull error messages from asserts
        bool PrintIndexError(uint32 index, bool set) const;
        Object(const Object&);                              // prevent generation copy constructor
        Object& operator=(Object const&);                   // prevent generation assigment operator
};

struct MovementInfo
{
    uint32  moveFlags;                                      // see enum MovementFlags
    uint8   moveFlags2;
    uint32  time;
    Position pos;
    // transport
    uint64  t_guid;
    Position t_pos;
    uint32  t_time;
    // swimming and unknown
    float   s_pitch;
    // last fall time
    uint32  fallTime;
    // jumping
    float   j_velocity, j_sinAngle, j_cosAngle, j_xyspeed;
    // spline
    float   u_unk1;
    MovementInfo() : moveFlags(MOVEMENTFLAG_NONE), moveFlags2(0), time(0), t_guid(0),
        t_time(0), s_pitch(0.0f), fallTime(0), j_velocity(0.0f), j_sinAngle(0.0f),
        j_cosAngle(0.0f), j_xyspeed(0.0f), u_unk1(0.0f)
    {
        t_pos.Relocate(0.0f, 0.0f, 0.0f, 0.0f);
    }

    // Read/Write methods
    void Read(ByteBuffer& data);
    void Write(ByteBuffer& data) const;

    // Movement flags manipulations
    void AddMovementFlag(MovementFlags f)
    {
        moveFlags |= f;
    }
    void RemoveMovementFlag(MovementFlags f)
    {
        moveFlags &= ~f;
    }
    bool HasMovementFlag(MovementFlags f) const
    {
        return moveFlags & f;
    }
    MovementFlags GetMovementFlags() const
    {
        return MovementFlags(moveFlags);
    }
    void SetMovementFlags(MovementFlags f)
    {
        moveFlags = f;
    }

    // Position manipulations
    Position const* GetPos() const
    {
        return &pos;
    }
    void SetTransportData(uint64 guid, float x, float y, float z, float o, uint32 time)
    {
        t_guid = guid;
        t_pos.m_positionX = x;
        t_pos.m_positionY = y;
        t_pos.m_positionZ = z;
        t_pos.SetOrientation(o);
        t_time = time;
    }
    void ClearTransportData()
    {
        t_guid = 0;
        t_pos.m_positionX = 0.0f;
        t_pos.m_positionY = 0.0f;
        t_pos.m_positionZ = 0.0f;
        t_pos.SetOrientation(0.0f);
        t_time = 0;
    }
    uint64 const& GetTransportGuid() const
    {
        return t_guid;
    }
    Position const* GetTransportPos() const
    {
        return &t_pos;
    }
    uint32 GetTransportTime() const
    {
        return t_time;
    }
    uint32 GetFallTime() const
    {
        return fallTime;
    }
    void ChangePosition(float x, float y, float z, float o)
    {
        pos.m_positionX = x;
        pos.m_positionY = y;
        pos.m_positionZ = z;
        pos.SetOrientation(o);
    }
    void UpdateTime(uint32 _time)
    {
        time = _time;
    }

    //private:
    // common
};

inline ByteBuffer& operator<< (ByteBuffer& buf, MovementInfo const& mi)
{
    mi.Write(buf);
    return buf;
}

inline ByteBuffer& operator>> (ByteBuffer& buf, MovementInfo& mi)
{
    mi.Read(buf);
    return buf;
}

#define MAPID_INVALID 0xFFFFFFFF

class WorldLocation : public Position
{
    public:
        explicit WorldLocation(uint32 _mapid = MAPID_INVALID, float _x = 0, float _y = 0, float _z = 0, float _o = 0)
            : m_mapId(_mapid)
        {
            Relocate(_x, _y, _z, _o);
        }
        WorldLocation(const WorldLocation& loc)
        {
            WorldRelocate(loc);
        }

        void WorldRelocate(const WorldLocation& loc)
        {
            m_mapId = loc.GetMapId();
            Relocate(loc);
        }
        uint32 GetMapId() const
        {
            return m_mapId;
        }

        uint32 m_mapId;
};

template<class T>
class GridObject
{
    public:
        bool IsInGrid() const { return _gridRef.isValid(); }
        void AddToGrid(GridRefManager<T>& m) { ASSERT(!IsInGrid()); _gridRef.link(&m, (T*)this); }
        void RemoveFromGrid() { ASSERT(IsInGrid()); _gridRef.unlink(); }
    private:
        GridReference<T> _gridRef;
};

template <class T_VALUES, class T_FLAGS, class FLAG_TYPE, uint8 ARRAY_SIZE>
class FlaggedValuesArray32
{
    public:
        FlaggedValuesArray32()
        {
            memset(&m_values, 0x00, sizeof(T_VALUES) * ARRAY_SIZE);
            m_flags = 0;
        }

        T_FLAGS  GetFlags() const { return m_flags; }
        bool     HasFlag(FLAG_TYPE flag) const { return m_flags & (1 << flag); }
        void     AddFlag(FLAG_TYPE flag) { m_flags |= (1 << flag); }
        void     DelFlag(FLAG_TYPE flag) { m_flags &= ~(1 << flag); }

        T_VALUES GetValue(FLAG_TYPE flag) const { return m_values[flag]; }
        void     SetValue(FLAG_TYPE flag, T_VALUES value) { m_values[flag] = value; }
        void     AddValue(FLAG_TYPE flag, T_VALUES value) { m_values[flag] += value; }

    private:
        T_VALUES m_values[ARRAY_SIZE];
        T_FLAGS m_flags;
};

class WorldObject : public Object, public WorldLocation
{
    public:
        ~WorldObject() override;

        virtual void Update (uint32 /*time_diff*/) { }

        void _Create(uint32 guidlow, HighGuid guidhigh);
        virtual void RemoveFromWorld() override;

        void GetNearPoint2D(float& x, float& y, float distance, float absAngle) const;
        void GetNearPoint(WorldObject const* searcher, float& x, float& y, float& z, float searcher_size, float distance2d, float absAngle) const;
        void GetClosePoint(float& x, float& y, float& z, float size, float distance2d = 0, float angle = 0) const
        {
            // angle calculated from current orientation
            GetNearPoint(NULL, x, y, z, size, distance2d, GetOrientation() + angle);
        }
        void MovePosition(Position& pos, float dist, float angle);
        Position GetNearPosition(float dist, float angle);
        void MovePositionToFirstCollision(Position& pos, float dist, float angle);
        Position GetFirstCollisionPosition(float dist, float angle);
        float GetPositionZTarget(Position& pos, float destx, float desty);
        Position GetRandomNearPosition(float radius);

        void GetContactPoint(const WorldObject* obj, float& x, float& y, float& z, float distance2d = CONTACT_DISTANCE) const
        {
            // angle to face `obj` to `this` using distance includes size of `obj`
            GetNearPoint(obj, x, y, z, obj->GetObjectSize(), distance2d, GetAngle(obj));
        }

        void GetChargeContactPoint(const WorldObject* obj, float& x, float& y, float& z, float distance2d = CONTACT_DISTANCE) const;

        virtual float GetObjectBoundingRadius() const
        {
            return DEFAULT_WORLD_OBJECT_SIZE;
        }

        float GetObjectSize() const
        {
            return (m_valuesCount > UNIT_FIELD_COMBATREACH) ? m_floatValues[UNIT_FIELD_COMBATREACH] : DEFAULT_WORLD_OBJECT_SIZE;
        }
        void UpdateGroundPositionZ(float x, float y, float& z) const;
        void UpdateAllowedPositionZ(float x, float y, float& z) const;

        void GetRandomPoint(const Position& srcPos, float distance, float& rand_x, float& rand_y, float& rand_z) const;
        void GetRandomPoint(const Position& srcPos, float distance, Position& pos) const
        {
            float x, y, z;
            GetRandomPoint(srcPos, distance, x, y, z);
            pos.Relocate(x, y, z, GetOrientation());
        }

        float GetDistanceSqr(float x, float y, float z) const;
        bool HasInArc(float arcangle, const Position* pos) const;
        bool HasInArc(const float arcangle, const float x, const float y) const;

        uint32 GetInstanceId() const
        {
            return m_InstanceId;
        }

        uint32 GetZoneId() const;
        uint32 GetAreaId() const;

        InstanceData* GetInstanceData();

        const char* GetName() const
        {
            return m_name.c_str();
        }
        void SetName(const std::string& newname)
        {
            m_name = newname;
        }

        virtual const char* GetNameForLocaleIdx(int32 /*locale_idx*/) const
        {
            return GetName();
        }

        float GetDistance(const WorldObject* obj) const
        {
            float d = GetExactDist(obj) - GetObjectSize() - obj->GetObjectSize();
            return d > 0.0f ? d : 0.0f;
        }
        float GetDistance(const Position& pos) const
        {
            float d = GetExactDist(&pos) - GetObjectSize();
            return d > 0.0f ? d : 0.0f;
        }
        float GetDistance(float x, float y, float z) const
        {
            float d = GetExactDist(x, y, z) - GetObjectSize();
            return d > 0.0f ? d : 0.0f;
        }
        float GetDistance2d(const WorldObject* obj) const
        {
            float d = GetExactDist2d(obj) - GetObjectSize() - obj->GetObjectSize();
            return d > 0.0f ? d : 0.0f;
        }
        float GetDistance2d(float x, float y) const
        {
            float d = GetExactDist2d(x, y) - GetObjectSize();
            return d > 0.0f ? d : 0.0f;
        }
        float GetDistanceZ(const WorldObject* obj) const;

        bool IsInMap(const WorldObject* obj) const
        {
            if (obj)
                return IsInWorld() && obj->IsInWorld() && (GetMap() == obj->GetMap());
            else
                return false;
        }
        bool IsWithinDist3d(float x, float y, float z, float dist) const
        {
            return IsInDist(x, y, z, dist + GetObjectSize());
        }
        bool IsWithinDist3d(const Position* pos, float dist) const
        {
            return IsInDist(pos, dist + GetObjectSize());
        }
        bool IsWithinDist2d(float x, float y, float dist) const
        {
            return IsInDist2d(x, y, dist + GetObjectSize());
        }
        bool IsWithinDist2d(const Position* pos, float dist) const
        {
            return IsInDist2d(pos, dist + GetObjectSize());
        }
        bool _IsWithinDist(WorldObject const* obj, float dist2compare, bool is3D) const;
        // use only if you will sure about placing both object at same map
        bool IsWithinDist(WorldObject const* obj, float dist2compare, bool is3D = true) const
        {
            return obj && _IsWithinDist(obj, dist2compare, is3D);
        }
        bool IsWithinDistInMap(WorldObject const* obj, float dist2compare, bool is3D = true) const
        {
            return obj && IsInMap(obj) && _IsWithinDist(obj, dist2compare, is3D);
        }
        bool IsWithinLOS(float x, float y, float z) const;
        bool IsWithinLOSInMap(const WorldObject* obj) const;
        Position GetHitSpherePointFor(Position const& dest) const;
        void GetHitSpherePointFor(Position const& dest, float& x, float& y, float& z) const;
        bool GetDistanceOrder(WorldObject const* obj1, WorldObject const* obj2, bool is3D = true) const;
        bool IsInRange(WorldObject const* obj, float minRange, float maxRange, bool is3D = true) const;
        bool IsInRange2d(float x, float y, float minRange, float maxRange) const;
        bool IsInRange3d(float x, float y, float z, float minRange, float maxRange) const;

        virtual void CleanupsBeforeDelete();                // used in destructor or explicitly before mass creature delete to remove cross-references to already deleted units

        virtual void SendMessageToSet(WorldPacket *data, bool self) { SendMessageToSetInRange(data, GetVisibilityRange(), self); }
        virtual void SendMessageToSetInRange(WorldPacket* data, float dist, bool self);

        virtual uint8 getLevelForTarget(WorldObject const* /*target*/) const { return 1; }

        void MonsterSay(const char* text, uint32 language, uint64 TargetGuid);
        void MonsterYell(const char* text, uint32 language, uint64 TargetGuid);
        void MonsterTextEmote(const char* text, uint64 TargetGuid, bool IsBossEmote = false);
        void MonsterWhisper(const char* text, uint64 receiver, bool IsBossWhisper = false);
        void MonsterSay(int32 textId, uint32 language, uint64 TargetGuid);
        void MonsterYell(int32 textId, uint32 language, uint64 TargetGuid);
        void MonsterTextEmote(int32 textId, uint64 TargetGuid, bool IsBossEmote = false);
        void MonsterWhisper(int32 textId, uint64 receiver, bool IsBossWhisper = false);
        void MonsterYellToZone(int32 textId, uint32 language, uint64 TargetGuid);
        void BuildMonsterChat(WorldPacket* data, uint8 msgtype, char const* text, uint32 language, char const* name, uint64 TargetGuid) const;

        void PlayDistanceSound(uint32 sound_id, Player* target = NULL);
        void PlayDirectSound(uint32 sound_id, Player* target = NULL);

        void SendObjectDeSpawnAnim(uint64 guid);
        void SendObjectCustomAnim(uint64 guid, uint32 anim = 0);

        virtual void SaveRespawnTime() {}
        void AddObjectToRemoveList();

        virtual bool isValid() const;

        virtual bool IsNeverVisible() const { return !IsInWorld(); }
        virtual bool IsAlwaysVisibleFor(WorldObject const* /*seer*/) const { return false; }
        virtual bool IsInvisibleDueToDespawn() const { return false; }
        //difference from IsAlwaysVisibleFor: 1. after distance check; 2. use owner or charmer as seer
        virtual bool IsAlwaysDetectableFor(WorldObject const* /*seer*/) const { return false; }

        float GetGridActivationRange() const;
        float GetVisibilityRange() const;
        float GetSightRange(const WorldObject* target = NULL) const;
        bool CanSeeOrDetect(WorldObject const* obj, bool ignoreStealth = false, bool distanceCheck = false, bool checkAlert = false) const;

        FlaggedValuesArray32<int32, uint32, StealthType, TOTAL_STEALTH_TYPES> m_stealth;
        FlaggedValuesArray32<int32, uint32, StealthType, TOTAL_STEALTH_TYPES> m_stealthDetect;

        FlaggedValuesArray32<int32, uint32, InvisibilityType, TOTAL_INVISIBILITY_TYPES> m_invisibility;
        FlaggedValuesArray32<int32, uint32, InvisibilityType, TOTAL_INVISIBILITY_TYPES> m_invisibilityDetect;

        FlaggedValuesArray32<int32, uint32, ServerSideVisibilityType, TOTAL_SERVERSIDE_VISIBILITY_TYPES> m_serverSideVisibility;
        FlaggedValuesArray32<int32, uint32, ServerSideVisibilityType, TOTAL_SERVERSIDE_VISIBILITY_TYPES> m_serverSideVisibilityDetect;

        // Low Level Packets
        void SendPlaySound(uint32 Sound, bool OnlySelf);

        virtual void SetMap(Map* map);
        virtual void ResetMap();
        Map* GetMap() const { ASSERT(m_currMap); return m_currMap; }
        Map* FindMap() const
        {
            return m_currMap;
        }
        //used to check all object's GetMap() calls when object is not in world!

        //this function should be removed in nearest time...
        Map const* GetBaseMap() const;

        void SetZoneScript();

        TempSummon* SummonCreature(uint32 id, const Position& pos, TempSummonType spwtype = TEMPSUMMON_MANUAL_DESPAWN, uint32 despwtime = 0);
        TempSummon* SummonCreature(uint32 id, float x, float y, float z, float ang, TempSummonType spwtype = TEMPSUMMON_MANUAL_DESPAWN, uint32 despwtime = 0)
        {
            if (!x && !y && !z)
            {
                GetClosePoint(x, y, z, GetObjectSize());
                ang = GetOrientation();
            }
            Position pos = {x, y, z, ang};
            return SummonCreature(id, pos, spwtype, despwtime);
        }
        GameObject* SummonGameObject(uint32 entry, float x, float y, float z, float ang, float rotation0, float rotation1, float rotation2, float rotation3, uint32 respawnTime);
        Creature* SummonTrigger(float x, float y, float z, float ang, uint32 dur, CreatureAI * (*GetAI)(Creature*) = NULL);
        void SummonCreatureGroup(uint8 group, std::list<TempSummon*>* list = NULL);

        Creature* FindNearestCreature(uint32 entry, float range, bool alive = true);
        GameObject* FindNearestGameObject(uint32 entry, float range);
        GameObject* FindNearestGameObjectOfType(GameobjectTypes type, float range) const;

        void GetGameObjectListWithEntryInGrid(std::list<GameObject*>& gameobjectList, uint32 entry = GRID_SEARCH_ALL_ENTRIES, float maxSearchRange = 250.0f) const;
        void GetCreatureListWithEntryInGrid(std::list<Creature*>& creatureList, uint32 entry = GRID_SEARCH_ALL_ENTRIES, float maxSearchRange = 250.0f) const;

        void DestroyForNearbyPlayers();
        virtual void UpdateObjectVisibility(bool forced = true);
        virtual void UpdateObjectVisibilityOnCreate()
        {
            UpdateObjectVisibility(true);
        }

        void BuildUpdate(UpdateDataMapType&) override;

        //relocation and visibility system functions
        void AddToNotify(uint16 f)
        {
            m_notifyflags |= f;
        }
        bool isNeedNotify(uint16 f) const
        {
            return m_notifyflags & f;
        }
        uint16 GetNotifyFlags() const
        {
            return m_notifyflags;
        }
        bool NotifyExecuted(uint16 f) const
        {
            return m_executed_notifies & f;
        }
        void SetNotified(uint16 f)
        {
            m_executed_notifies |= f;
        }
        void ResetAllNotifies()
        {
            m_notifyflags = 0;
            m_executed_notifies = 0;
        }

        bool isActiveObject() const
        {
            return m_isActive;
        }
        void setActive(bool isActiveObject);
        void SetWorldObject(bool apply);
        bool IsPermanentWorldObject() const { return m_isWorldObject; }
        bool IsWorldObject() const;

        template<class NOTIFIER> void VisitNearbyWorldObject(const float &radius, NOTIFIER &notifier) const { if (IsInWorld()) GetMap()->VisitWorld(GetPositionX(), GetPositionY(), radius, notifier); }
        template<class NOTIFIER> void VisitNearbyObject(float const& radius, NOTIFIER& notifier) const { if (IsInWorld()) GetMap()->VisitAll(GetPositionX(), GetPositionY(), radius, notifier); }
        template<class NOTIFIER> void VisitNearbyGridObject(float const& radius, NOTIFIER& notifier) const { if (IsInWorld()) GetMap()->VisitGrid(GetPositionX(), GetPositionY(), radius, notifier); }

        uint32 m_groupLootTimer;                            // (msecs)timer used for group loot
        uint64 lootingGroupLeaderGUID;                      // used to find group which is looting corpse

        MovementInfo m_movementInfo;

    protected:
        explicit WorldObject(bool isWorldObject); //note: here it means if it is in grid object list or world object list
        std::string m_name;
        bool m_isActive;
        const bool m_isWorldObject;
        ZoneScript* m_zoneScript;

        //these functions are used mostly for Relocate() and Corpse/Player specific stuff...
        //use them ONLY in LoadFromDB()/Create() funcs and nowhere else!
        //mapId/instanceId should be set in SetMap() function!
        void SetLocationMapId(uint32 _mapId)
        {
            m_mapId = _mapId;
        }
        void SetLocationInstanceId(uint32 _instanceId)
        {
            m_InstanceId = _instanceId;
        }

    private:
        Map* m_currMap;                                     //current object's Map location

        //uint32 m_mapId;                                     // object at map with map_id
        uint32 m_InstanceId;                                // in map copy with instance id

        uint16 m_notifyflags;
        uint16 m_executed_notifies;

        bool CanNeverSee(WorldObject const* obj) const;
        virtual bool CanAlwaysSee(WorldObject const* /*obj*/) const { return false; }
        bool CanDetect(WorldObject const* obj, bool ignoreStealth, bool checkAlert = false) const;
        bool CanDetectInvisibilityOf(WorldObject const* obj) const;
        bool CanDetectStealthOf(WorldObject const* obj, bool checkAlert = false) const;
};

namespace Oregon
{
// Binary predicate to sort WorldObjects based on the distance to a reference WorldObject
class ObjectDistanceOrderPred
{
    public:
        ObjectDistanceOrderPred(const WorldObject* pRefObj, bool ascending = true) : m_refObj(pRefObj), m_ascending(ascending) {}
        bool operator()(const WorldObject* pLeft, const WorldObject* pRight) const
        {
            return m_ascending ? m_refObj->GetDistanceOrder(pLeft, pRight) : !m_refObj->GetDistanceOrder(pLeft, pRight);
        }
    private:
        const WorldObject* m_refObj;
        const bool m_ascending;
};
}

#endif

