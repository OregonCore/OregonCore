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

#ifndef OREGON_OBJECT_GUID_H
#define OREGON_OBJECT_GUID_H

#include "Common.h"
#include "ByteBuffer.h"

enum TypeID
{
    TYPEID_OBJECT        = 0,
    TYPEID_ITEM          = 1,
    TYPEID_CONTAINER     = 2,
    TYPEID_UNIT          = 3,
    TYPEID_PLAYER        = 4,
    TYPEID_GAMEOBJECT    = 5,
    TYPEID_DYNAMICOBJECT = 6,
    TYPEID_CORPSE        = 7
};

#define MAX_TYPE_ID        8

enum TypeMask
{
    TYPEMASK_OBJECT         = 0x0001,
    TYPEMASK_ITEM           = 0x0002,
    TYPEMASK_CONTAINER      = 0x0006,                       // TYPEMASK_ITEM | 0x0004
    TYPEMASK_UNIT           = 0x0008,                       // players also have it
    TYPEMASK_PLAYER         = 0x0010 | TYPEMASK_UNIT,
    TYPEMASK_GAMEOBJECT     = 0x0020,
    TYPEMASK_DYNAMICOBJECT  = 0x0040,
    TYPEMASK_CORPSE         = 0x0080,
    TYPEMASK_SEER           = TYPEMASK_UNIT | TYPEMASK_DYNAMICOBJECT
};

enum HighGuid
{
    HIGHGUID_ITEM           = 0x4000,                       // blizz 4000
    HIGHGUID_CONTAINER      = 0x4000,                       // blizz 4000
    HIGHGUID_PLAYER         = 0x0000,                       // blizz 0000
    HIGHGUID_GAMEOBJECT     = 0xF110,                       // blizz F110
    HIGHGUID_TRANSPORT      = 0xF120,                       // blizz F120 (for GAMEOBJECT_TYPE_TRANSPORT)
    HIGHGUID_UNIT           = 0xF130,                       // blizz F130
    HIGHGUID_PET            = 0xF140,                       // blizz F140
    HIGHGUID_DYNAMICOBJECT  = 0xF100,                       // blizz F100
    HIGHGUID_CORPSE         = 0xF101,                       // blizz F100
    HIGHGUID_MO_TRANSPORT   = 0x1FC0,                       // blizz 1FC0 (for GAMEOBJECT_TYPE_MO_TRANSPORT)
};

//*** Must be replaced by ObjectGuid use ***
#define IS_CREATURE_GUID(Guid)       (GUID_HIPART(Guid) == HIGHGUID_UNIT)
#define IS_PET_GUID(Guid)            (GUID_HIPART(Guid) == HIGHGUID_PET)
#define IS_CREATURE_OR_PET_GUID(Guid)(IS_CREATURE_GUID(Guid) || IS_PET_GUID(Guid))
#define IS_PLAYER_GUID(Guid)         (GUID_HIPART(Guid) == HIGHGUID_PLAYER && Guid != 0)
#define IS_UNIT_GUID(Guid)           (IS_CREATURE_OR_PET_GUID(Guid) || IS_PLAYER_GUID(Guid))
// special case for empty guid need check
#define IS_ITEM_GUID(Guid)           (GUID_HIPART(Guid) == HIGHGUID_ITEM)
#define IS_GAMEOBJECT_GUID(Guid)     (GUID_HIPART(Guid) == HIGHGUID_GAMEOBJECT)
#define IS_CORPSE_GUID(Guid)         (GUID_HIPART(Guid) == HIGHGUID_CORPSE)
#define IS_MO_TRANSPORT(Guid)        (GUID_HIPART(Guid) == HIGHGUID_MO_TRANSPORT)

// l - OBJECT_FIELD_GUID
// e - OBJECT_FIELD_ENTRY for GO (except GAMEOBJECT_TYPE_MO_TRANSPORT) and creatures or UNIT_FIELD_PETNUMBER for pets
// h - OBJECT_FIELD_GUID + 1
#define MAKE_NEW_GUID(l, e, h)   uint64(uint64(l) | (uint64(e) << 24) | (uint64(h) << 48))

#define GUID_HIPART(x)   (uint32)((uint64(x) >> 48) & 0x0000FFFF)

// We have different low and middle part size for different guid types
#define _GUID_ENPART_2(x) 0
#define _GUID_ENPART_3(x) (uint32)((uint64(x) >> 24) & UI64LIT(0x0000000000FFFFFF))
#define _GUID_LOPART_2(x) (uint32)(uint64(x)         & UI64LIT(0x00000000FFFFFFFF))
#define _GUID_LOPART_3(x) (uint32)(uint64(x)         & UI64LIT(0x0000000000FFFFFF))

inline bool IsGuidHaveEnPart(uint64 const& guid)
{
    switch (GUID_HIPART(guid))
    {
    case HIGHGUID_ITEM:
    case HIGHGUID_PLAYER:
    case HIGHGUID_DYNAMICOBJECT:
    case HIGHGUID_CORPSE:
    case HIGHGUID_MO_TRANSPORT:
        return false;
    case HIGHGUID_GAMEOBJECT:
    case HIGHGUID_TRANSPORT:
    case HIGHGUID_UNIT:
    case HIGHGUID_PET:
    default:
        return true;
    }
}

#define GUID_ENPART(x) (IsGuidHaveEnPart(x) ? _GUID_ENPART_3(x) : _GUID_ENPART_2(x))
#define GUID_LOPART(x) (IsGuidHaveEnPart(x) ? _GUID_LOPART_3(x) : _GUID_LOPART_2(x))

//*** Must be replaced by ObjectGuid use END ***
class ObjectGuid;
class PackedGuid;

struct PackedGuidReader
{
    explicit PackedGuidReader(ObjectGuid& guid) : m_guidPtr(&guid) {}
    ObjectGuid* m_guidPtr;
};

class ObjectGuid
{
    public:                                                 // constructors
        ObjectGuid() : m_guid(0) {}
        ObjectGuid(uint64 const& guid) : m_guid(guid) {}    // NOTE: must be explicit in future for more strict control type conversions
        ObjectGuid(HighGuid hi, uint32 entry, uint32 counter) : m_guid(uint64(counter) | (uint64(entry) << 24) | (uint64(hi) << 48)) {}

    public:                                                 // modifiers
        operator uint64() const { return m_guid; }

        PackedGuidReader ReadAsPacked()
        {
            return PackedGuidReader(*this);
        }

        void Set(uint64 const& guid)
        {
            m_guid = guid;
        }
        void Clear()
        {
            m_guid = 0;
        }

        // Possible removed in future for more strict control type conversions
        void operator= (uint64 const& guid)
        {
            m_guid = guid;
        }

        PackedGuid WriteAsPacked() const;
    public:                                                 // accessors
        uint64 const& GetRawValue() const
        {
            return m_guid;
        }
        HighGuid GetHigh() const
        {
            return HighGuid((m_guid >> 48) & 0x0000FFFF);
        }
        uint32   GetEntry() const
        {
            return HasEntry() ? uint32((m_guid >> 24) & UI64LIT(0x0000000000FFFFFF)) : 0;
        }
        uint32   GetCounter()  const
        {
            return HasEntry()
                   ? uint32(m_guid & UI64LIT(0x0000000000FFFFFF))
                   : uint32(m_guid & UI64LIT(0x00000000FFFFFFFF));
        }

        bool IsEmpty()         const
        {
            return m_guid == 0;
        }
        bool IsCreature()      const
        {
            return GetHigh() == HIGHGUID_UNIT;
        }
        bool IsPet()           const
        {
            return GetHigh() == HIGHGUID_PET;
        }
        bool IsCreatureOrPet() const
        {
            return IsCreature() || IsPet();
        }
        bool IsPlayer()        const
        {
            return !IsEmpty() && GetHigh() == HIGHGUID_PLAYER;
        }
        bool IsUnit()          const
        {
            return IsCreatureOrPet() || IsPlayer();
        }
        bool IsItem()          const
        {
            return GetHigh() == HIGHGUID_ITEM;
        }
        bool IsGameobject()    const
        {
            return GetHigh() == HIGHGUID_GAMEOBJECT;
        }
        bool IsDynamicObject() const
        {
            return GetHigh() == HIGHGUID_DYNAMICOBJECT;
        }
        bool IsCorpse()        const
        {
            return GetHigh() == HIGHGUID_CORPSE;
        }
        bool IsTransport()     const
        {
            return GetHigh() == HIGHGUID_TRANSPORT;
        }
        bool IsMOTransport()   const
        {
            return GetHigh() == HIGHGUID_MO_TRANSPORT;
        }

        TypeID GetTypeId()
        {
            switch (GetHigh())
            {
            case HIGHGUID_ITEM:
                return TYPEID_ITEM;
            //case HIGHGUID_CONTAINER:    return TYPEID_CONTAINER; HIGHGUID_CONTAINER==HIGHGUID_ITEM currently
            case HIGHGUID_UNIT:
                return TYPEID_UNIT;
            case HIGHGUID_PET:
                return TYPEID_UNIT;
            case HIGHGUID_PLAYER:
                return TYPEID_PLAYER;
            case HIGHGUID_GAMEOBJECT:
                return TYPEID_GAMEOBJECT;
            case HIGHGUID_DYNAMICOBJECT:
                return TYPEID_DYNAMICOBJECT;
            case HIGHGUID_CORPSE:
                return TYPEID_CORPSE;
            case HIGHGUID_MO_TRANSPORT:
                return TYPEID_GAMEOBJECT;
            // unknown
            default:
                return TYPEID_OBJECT;
            }
        }
        bool operator!() const { return IsEmpty(); }
        bool operator== (ObjectGuid const& guid) const { return GetRawValue() == guid.GetRawValue(); }
        bool operator!= (ObjectGuid const& guid) const { return GetRawValue() != guid.GetRawValue(); }
        bool operator< (ObjectGuid const& guid) const { return GetRawValue() < guid.GetRawValue(); }

    public:                                                 // accessors - for debug
        char const* GetTypeName() const;
        std::string GetString() const;
    private:                                                // internal functions
        bool HasEntry() const
        {
            switch (GetHigh())
            {
            case HIGHGUID_ITEM:
            case HIGHGUID_PLAYER:
            case HIGHGUID_DYNAMICOBJECT:
            case HIGHGUID_CORPSE:
            case HIGHGUID_MO_TRANSPORT:
                return false;
            case HIGHGUID_GAMEOBJECT:
            case HIGHGUID_TRANSPORT:
            case HIGHGUID_UNIT:
            case HIGHGUID_PET:
            default:
                return true;
            }
        }

    private:                                                // fields
        uint64 m_guid;
};

// Some Shared defines
typedef std::list<ObjectGuid> GuidList;

class PackedGuid
{
        friend ByteBuffer& operator<< (ByteBuffer& buf, PackedGuid const& guid);

    public:                                                 // constructors
        explicit PackedGuid()
        {
            m_packedGuid.appendPackGUID(0);
        }
        explicit PackedGuid(uint64 const& guid)
        {
            m_packedGuid.appendPackGUID(guid);
        }
        explicit PackedGuid(ObjectGuid const& guid)
        {
            m_packedGuid.appendPackGUID(guid.GetRawValue());
        }

    public:                                                 // modifiers
        void Set(uint64 const& guid)
        {
            m_packedGuid.wpos(0);
            m_packedGuid.appendPackGUID(guid);
        }
        void Set(ObjectGuid const& guid)
        {
            m_packedGuid.wpos(0);
            m_packedGuid.appendPackGUID(guid.GetRawValue());
        }

    public:                                                 // accessors
        size_t size() const
        {
            return m_packedGuid.size();
        }

    private:                                                // fields
        ByteBuffer m_packedGuid;
};

ByteBuffer& operator<< (ByteBuffer& buf, ObjectGuid const& guid);
ByteBuffer& operator>> (ByteBuffer& buf, ObjectGuid&       guid);

ByteBuffer& operator<< (ByteBuffer& buf, PackedGuid const& guid);
ByteBuffer& operator>> (ByteBuffer& buf, PackedGuidReader const& guid);

inline PackedGuid ObjectGuid::WriteAsPacked() const
{
    return PackedGuid(*this);
}

#endif

