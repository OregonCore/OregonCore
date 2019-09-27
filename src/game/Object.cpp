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

#include "Common.h"
#include "SharedDefines.h"
#include "WorldPacket.h"
#include "Opcodes.h"
#include "World.h"
#include "Object.h"
#include "Creature.h"
#include "Player.h"
#include "ObjectMgr.h"
#include "WorldSession.h"
#include "UpdateData.h"
#include "UpdateMask.h"
#include "Utilities/Util.h"
#include "ObjectAccessor.h"
#include "Transports.h"
#include "VMapFactory.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "TemporarySummon.h"
#include "Totem.h"
#include "OutdoorPvPMgr.h"
#include "packet_builder.h"
#include "MapManager.h"

uint32 GuidHigh2TypeId(uint32 guid_hi)
{
    switch (guid_hi)
    {
    case HIGHGUID_ITEM:
        return TYPEID_ITEM;
    //case HIGHGUID_CONTAINER:    return TYPEID_CONTAINER; HIGHGUID_CONTAINER == HIGHGUID_ITEM currently
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
    }
    return TYPEID_OBJECT;                                   // unknown
}

Object::Object()
{
    m_objectTypeId      = TYPEID_OBJECT;
    m_objectType        = TYPEMASK_OBJECT;

    m_uint32Values      = 0;
    m_uint32Values_mirror = 0;
    m_valuesCount       = 0;

    m_inWorld           = false;
    m_objectUpdated     = false;
}

WorldObject::~WorldObject()
{
    // this may happen because there are many !create/delete
    if (IsWorldObject() && m_currMap)
    {
        if (GetTypeId() == TYPEID_CORPSE)
        {
            sLog.outError("Crash alert! Object::~Object Corpse guid=" UI64FMTD ", type=%d, entry=%u deleted but still in map!!", GetGUID(), ((Corpse*)this)->GetType(), GetEntry());
            ASSERT(false);
        }
        ResetMap();
    }
}

Object::~Object()
{
    if (IsInWorld())
    {
        // Do NOT call RemoveFromWorld here, if the object is a player it will crash
        sLog.outError("Crash alert! Object::~Object (GUID: %u TypeId: %u) deleted but still in world!!", GetGUIDLow(), GetTypeId());
        ASSERT(false);
    }

    if (m_objectUpdated)
    {
        sLog.outError("Crash alert! Object::~Object (GUID: %u TypeId: %u) deleted but still has updated status!!", GetGUIDLow(), GetTypeId());
        ASSERT(false);
    }

    delete [] m_uint32Values;
    m_uint32Values = 0;
}

void Object::_InitValues()
{
    m_uint32Values = new uint32[ m_valuesCount ];
    memset(m_uint32Values, 0, m_valuesCount * sizeof(uint32));

    m_uint32Values_mirror = new uint32[ m_valuesCount ];
    memset(m_uint32Values_mirror, 0, m_valuesCount * sizeof(uint32));

    m_objectUpdated = false;
}

void Object::_Create(uint32 guidlow, uint32 entry, HighGuid guidhigh)
{
    if (!m_uint32Values)
        _InitValues();

    uint64 guid = MAKE_NEW_GUID(guidlow, entry, guidhigh);
    SetUInt64Value(OBJECT_FIELD_GUID, guid);
    SetUInt32Value(OBJECT_FIELD_TYPE, m_objectType);
    m_PackGUID.Set(guid);
}

void Object::BuildMovementUpdateBlock(UpdateData* data, uint32 flags) const
{
    ByteBuffer buf(500);

    buf << uint8(UPDATETYPE_MOVEMENT);
    buf << GetGUID();

    _BuildMovementUpdate(&buf, flags);

    data->AddUpdateBlock(buf);
}

void Object::BuildCreateUpdateBlockForPlayer(UpdateData* data, Player* target) const
{
    if (!target)
        return;

    uint8  updatetype = UPDATETYPE_CREATE_OBJECT;
    uint8  flags      = m_updateFlag;

    /** lower flag1 **/
    if (target == this)                                      // building packet for oneself
        flags |= UPDATEFLAG_SELF;

    if (flags & UPDATEFLAG_HAS_POSITION)
    {
        // UPDATETYPE_CREATE_OBJECT2 dynamic objects, corpses...
        if (isType(TYPEMASK_DYNAMICOBJECT) || isType(TYPEMASK_CORPSE) || isType(TYPEMASK_PLAYER))
            updatetype = UPDATETYPE_CREATE_OBJECT2;

        // UPDATETYPE_CREATE_OBJECT2 for pets...
        if (target->GetPetGUID() == GetGUID())
            updatetype = UPDATETYPE_CREATE_OBJECT2;

        // UPDATETYPE_CREATE_OBJECT2 for some gameobject types...
        if (isType(TYPEMASK_GAMEOBJECT))
        {
            switch (((GameObject*)this)->GetGoType())
            {
            case GAMEOBJECT_TYPE_TRAP:
            case GAMEOBJECT_TYPE_DUEL_ARBITER:
            case GAMEOBJECT_TYPE_FLAGSTAND:
            case GAMEOBJECT_TYPE_FLAGDROP:
                updatetype = UPDATETYPE_CREATE_OBJECT2;
                break;
            case GAMEOBJECT_TYPE_TRANSPORT:
                flags |= UPDATEFLAG_TRANSPORT;
                break;
            default:
                break;
            }
        }
    }

    //sLog.outDebug("BuildCreateUpdate: update-type: %u, object-type: %u got flags: %X, flags2: %X", updatetype, m_objectTypeId, flags, flags2);

    ByteBuffer buf(500);
    buf << (uint8)updatetype;
    //buf << GetPackGUID();                                 //client crashes when using this
    buf << (uint8)0xFF << GetGUID();
    buf << (uint8)m_objectTypeId;

    _BuildMovementUpdate(&buf, flags);

    UpdateMask updateMask;
    updateMask.SetCount(m_valuesCount);
    _SetCreateBits(&updateMask, target);
    _BuildValuesUpdate(updatetype, &buf, &updateMask, target);
    data->AddUpdateBlock(buf);
}

void Object::SendUpdateToPlayer(Player* player)
{
    // send create update to player
    UpdateData upd;
    WorldPacket packet;

    BuildCreateUpdateBlockForPlayer(&upd, player);
    upd.BuildPacket(&packet);
    player->GetSession()->SendPacket(&packet);
}

void Object::BuildValuesUpdateBlockForPlayer(UpdateData* data, Player* target) const
{
    ByteBuffer buf(500);

    buf << (uint8) UPDATETYPE_VALUES;
    //buf << GetPackGUID();                                 //client crashes when using this. but not have crash in debug mode
    buf << (uint8)0xFF;
    buf << GetGUID();

    UpdateMask updateMask;
    updateMask.SetCount(m_valuesCount);

    _SetUpdateBits(&updateMask, target);
    _BuildValuesUpdate(UPDATETYPE_VALUES, &buf, &updateMask, target);

    data->AddUpdateBlock(buf);
}

void Object::BuildOutOfRangeUpdateBlock(UpdateData* data) const
{
    data->AddOutOfRangeGUID(GetGUID());
}

void Object::DestroyForPlayer(Player* target, bool onDeath) const
{
    ASSERT(target);

    WorldPacket data(SMSG_DESTROY_OBJECT, 8 + 1);
    data << uint64(GetGUID());
    //! If the following bool is true, the client will call "void CGUnit_C::OnDeath()" for this object.
    //! OnDeath() does for eg trigger death animation and interrupts certain spells/missiles/auras/sounds...
    data << uint8(onDeath ? 1 : 0);
    target->GetSession()->SendPacket(&data);
}

void Object::_BuildMovementUpdate(ByteBuffer* data, uint8 updateFlags) const
{
    uint32 moveFlags = MOVEMENTFLAG_NONE;

    *data << uint8(updateFlags);                            // update flags

    if (updateFlags & UPDATEFLAG_LIVING)
    {
        switch (GetTypeId())
        {
        case TYPEID_UNIT:
            {
                moveFlags = ((Unit*)this)->GetUnitMovementFlags();
                moveFlags &= ~MOVEMENTFLAG_ONTRANSPORT;
            }
            break;
        case TYPEID_PLAYER:
            {
                moveFlags = ToPlayer()->GetUnitMovementFlags();

                if (ToPlayer()->GetTransport())
                    moveFlags |= MOVEMENTFLAG_ONTRANSPORT;
                else
                    moveFlags &= ~MOVEMENTFLAG_ONTRANSPORT;

            }
            break;
        }

        *data << uint32(moveFlags);                            // movement flags
        *data << uint8(0);                                  // movemoveFlags
        *data << uint32(getMSTime());                       // time (in milliseconds)
    }

    // 0x40
    if (updateFlags & UPDATEFLAG_HAS_POSITION)
    {
        // 0x02
        if (updateFlags & UPDATEFLAG_TRANSPORT && ((GameObject*)this)->GetGoType() == GAMEOBJECT_TYPE_MO_TRANSPORT)
        {
            *data << float(0);
            *data << float(0);
            *data << float(0);
            *data << float(((WorldObject*)this)->GetOrientation());
        }
        else
        {
            *data << float(((WorldObject*)this)->GetPositionX());
            *data << float(((WorldObject*)this)->GetPositionY());
            *data << float(((WorldObject*)this)->GetPositionZ());
            *data << float(((WorldObject*)this)->GetOrientation());
        }
    }

    // 0x20
    if (updateFlags & UPDATEFLAG_LIVING)
    {
        // 0x00000200
        if (moveFlags & MOVEMENTFLAG_ONTRANSPORT)
        {
            if (GetTypeId() == TYPEID_PLAYER)
            {
                *data << (uint64)ToPlayer()->GetTransport()->GetGUID();
                *data << (float)ToPlayer()->GetTransOffsetX();
                *data << (float)ToPlayer()->GetTransOffsetY();
                *data << (float)ToPlayer()->GetTransOffsetZ();
                *data << (float)ToPlayer()->GetTransOffsetO();
                *data << (uint32)ToPlayer()->GetTransTime();
            }
            //Oregon currently not have support for other than player on transport
        }

        // 0x02200000
        if (moveFlags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING2))
        {
            if (GetTypeId() == TYPEID_PLAYER)
                *data << (float)ToPlayer()->m_movementInfo.s_pitch;
            else
                *data << float(0);                          // is't part of movement packet, we must store and send it...
        }

        if (GetTypeId() == TYPEID_PLAYER)
            *data << (uint32)ToPlayer()->m_movementInfo.GetFallTime();
        else
            *data << uint32(0);                             // last fall time

        // 0x00001000
        if (moveFlags & MOVEMENTFLAG_FALLING)
        {
            if (GetTypeId() == TYPEID_PLAYER)
            {
                *data << float(ToPlayer()->m_movementInfo.j_velocity);
                *data << float(ToPlayer()->m_movementInfo.j_sinAngle);
                *data << float(ToPlayer()->m_movementInfo.j_cosAngle);
                *data << float(ToPlayer()->m_movementInfo.j_xyspeed);
            }
            else
            {
                *data << float(0);
                *data << float(0);
                *data << float(0);
                *data << float(0);
            }
        }

        // 0x04000000
        if (moveFlags & MOVEMENTFLAG_SPLINE_ELEVATION)
        {
            if (GetTypeId() == TYPEID_PLAYER)
                *data << float(ToPlayer()->m_movementInfo.u_unk1);
            else
                *data << float(0);
        }

        // Unit speeds
        *data << ((Unit*)this)->GetSpeed(MOVE_WALK);
        *data << ((Unit*)this)->GetSpeed(MOVE_RUN);
        *data << ((Unit*)this)->GetSpeed(MOVE_RUN_BACK);
        *data << ((Unit*)this)->GetSpeed(MOVE_SWIM);
        *data << ((Unit*)this)->GetSpeed(MOVE_SWIM_BACK);
        *data << ((Unit*)this)->GetSpeed(MOVE_FLIGHT);
        *data << ((Unit*)this)->GetSpeed(MOVE_FLIGHT_BACK);
        *data << ((Unit*)this)->GetSpeed(MOVE_TURN_RATE);

        // 0x08000000
        if (moveFlags & MOVEMENTFLAG_SPLINE_ENABLED)
            Movement::PacketBuilder::WriteCreate(*((Unit*)this)->movespline, *data);
    }

    // 0x8
    if (updateFlags & UPDATEFLAG_LOWGUID)
    {
        switch (GetTypeId())
        {
        case TYPEID_OBJECT:
        case TYPEID_ITEM:
        case TYPEID_CONTAINER:
        case TYPEID_GAMEOBJECT:
        case TYPEID_DYNAMICOBJECT:
        case TYPEID_CORPSE:
            *data << uint32(GetGUIDLow());              // GetGUIDLow()
            break;
        case TYPEID_UNIT:
            *data << uint32(0x0000000B);                // unk, can be 0xB or 0xC
            break;
        case TYPEID_PLAYER:
            if (updateFlags & UPDATEFLAG_SELF)
                *data << uint32(0x00000015);            // unk, can be 0x15 or 0x22
            else
                *data << uint32(0x00000008);            // unk, can be 0x7 or 0x8
            break;
        default:
            *data << uint32(0x00000000);                // unk
            break;
        }
    }

    // 0x10
    if (updateFlags & UPDATEFLAG_HIGHGUID)
    {
        switch (GetTypeId())
        {
        case TYPEID_OBJECT:
        case TYPEID_ITEM:
        case TYPEID_CONTAINER:
        case TYPEID_GAMEOBJECT:
        case TYPEID_DYNAMICOBJECT:
        case TYPEID_CORPSE:
            *data << uint32(GetGUIDHigh());             // GetGUIDHigh()
            break;
        default:
            *data << uint32(0x00000000);                // unk
            break;
        }
    }

    // 0x4
    if (updateFlags & UPDATEFLAG_HAS_ATTACKING_TARGET)  // packed guid (probably target guid)
    {
        if (Unit const* me = ToUnit())
        {
            if (me->GetVictim())
                *data << me->GetVictim()->GetPackGUID();
            else
                *data << uint8(0);
        }
        else
            *data << uint8(0);
    }

    // 0x2
    if (updateFlags & UPDATEFLAG_TRANSPORT)
    {
        *data << uint32(getMSTime());                       // ms time
    }
}

void Object::_BuildValuesUpdate(uint8 updatetype, ByteBuffer* data, UpdateMask* updateMask, Player* target) const
{
    if (!target)
        return;

    bool IsActivateToQuest = false;
    if (updatetype == UPDATETYPE_CREATE_OBJECT || updatetype == UPDATETYPE_CREATE_OBJECT2)
    {
        if (isType(TYPEMASK_GAMEOBJECT) && !((GameObject*)this)->IsTransport())
        {
            if (((GameObject*)this)->ActivateToQuest(target) || target->IsGameMaster())
                IsActivateToQuest = true;

            updateMask->SetBit(GAMEOBJECT_DYN_FLAGS);

            if (GetUInt32Value(GAMEOBJECT_ARTKIT))
                updateMask->SetBit(GAMEOBJECT_ARTKIT);
        }
    }
    else                                                    //case UPDATETYPE_VALUES
    {
        if (isType(TYPEMASK_GAMEOBJECT) && !((GameObject*)this)->IsTransport())
        {
            if (((GameObject*)this)->ActivateToQuest(target) || target->IsGameMaster())
                IsActivateToQuest = true;

            updateMask->SetBit(GAMEOBJECT_DYN_FLAGS);
            updateMask->SetBit(GAMEOBJECT_ANIMPROGRESS);
        }
    }

    ASSERT(updateMask && updateMask->GetCount() == m_valuesCount);

    *data << (uint8)updateMask->GetBlockCount();
    data->append(updateMask->GetMask(), updateMask->GetLength());

    // 2 specialized loops for speed optimization in non-unit case
    if (isType(TYPEMASK_UNIT))                               // unit (creature/player) case
    {
        for (uint16 index = 0; index < m_valuesCount; index ++)
        {
            if (updateMask->GetBit(index))
            {
                // remove custom flag before send
                if (index == UNIT_NPC_FLAGS)
                    *data << uint32(m_uint32Values[index]);
                // FIXME: Some values at server stored in float format but must be sent to client in uint32 format
                else if (index >= UNIT_FIELD_BASEATTACKTIME && index <= UNIT_FIELD_RANGEDATTACKTIME)
                {
                    // convert from float to uint32 and send
                    *data << uint32(m_floatValues[ index ] < 0 ? 0 : m_floatValues[ index ]);
                }
                // there are some float values which may be negative or can't get negative due to other checks
                else if ((index >= UNIT_FIELD_NEGSTAT0   && index <= UNIT_FIELD_NEGSTAT4) ||
                         (index >= UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE  && index <= (UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + 6)) ||
                         (index >= UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE  && index <= (UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + 6)) ||
                         (index >= UNIT_FIELD_POSSTAT0   && index <= UNIT_FIELD_POSSTAT4))
                    *data << uint32(m_floatValues[ index ]);
                // Gamemasters should be always able to select units - remove not selectable flag
                else if (index == UNIT_FIELD_FLAGS && target->IsGameMaster())
                    *data << (m_uint32Values[ index ] & ~UNIT_FLAG_NOT_SELECTABLE);
                // use modelid_a if not gm, _h if gm for CREATURE_FLAG_EXTRA_TRIGGER creatures
                else if (index == UNIT_FIELD_DISPLAYID && GetTypeId() == TYPEID_UNIT)
                {
                    const CreatureInfo* cinfo = ToCreature()->GetCreatureTemplate();
                    if (cinfo->flags_extra & CREATURE_FLAG_EXTRA_TRIGGER)
                    {
                        if (target->IsGameMaster())
                        {
                            if (cinfo->modelid1)
                                *data << cinfo->modelid1;
                            else
                                *data << 17519; // world invisible trigger's model
                        }
                        else
                        {
                            *data << 11686; // world invisible trigger's model
                        }
                    }
                    else
                        *data << m_uint32Values[ index ];
                }
                // hide lootable animation for unallowed players
                else if (index == UNIT_DYNAMIC_FLAGS && GetTypeId() == TYPEID_UNIT)
                {
                    uint32 value = m_uint32Values[index];

                    if (Creature* creature = (Creature*)this)
                        if (!creature->loot.isLooted())
                            if (!(value & UNIT_DYNFLAG_LOOTABLE))
                            {
                                creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                                value = value | UNIT_DYNFLAG_LOOTABLE;
                            }

                    if (!target->isAllowedToLoot((Creature*)this))
                        if (value & UNIT_DYNFLAG_LOOTABLE)
                            value = value & ~UNIT_DYNFLAG_LOOTABLE;

                    bool tapped = ToCreature()->isTappedBy(target->ToPlayer());

                    if (value & UNIT_DYNFLAG_OTHER_TAGGER && tapped)
                        value = value & ~UNIT_DYNFLAG_OTHER_TAGGER;

                    *data << value;

                }

                // hide RAF menu to non-RAF linked friends
                else if (index == UNIT_DYNAMIC_FLAGS && GetTypeId() == TYPEID_PLAYER)
                {
                    if (sObjectMgr.GetRAFLinkStatus(target->ToPlayer(), this->ToPlayer()) != RAF_LINK_NONE)
                        *data << (m_uint32Values[ index ]);
                    else
                        *data << (m_uint32Values[ index ] & ~UNIT_DYNFLAG_REFER_A_FRIEND);
                }
                // FG: pretend that OTHER players in own group are friendly ("blue")
                else if (index == UNIT_FIELD_BYTES_2 || index == UNIT_FIELD_FACTIONTEMPLATE)
                {
                    bool ch = false;
                    if (target->GetTypeId() == TYPEID_PLAYER && GetTypeId() == TYPEID_PLAYER && target != this)
                    {
                        if (target->IsInSameGroupWith(ToPlayer()) || target->IsInSameRaidWith(ToPlayer()))
                        {
                            if (index == UNIT_FIELD_BYTES_2)
                            {
                                DEBUG_LOG("-- VALUES_UPDATE: Sending '%s' the blue-group-fix from '%s' (flag)", target->GetName(), ToPlayer()->GetName());
                                *data << (m_uint32Values[ index ] & ((UNIT_BYTE2_FLAG_SANCTUARY | UNIT_BYTE2_FLAG_AURAS | UNIT_BYTE2_FLAG_UNK5) << 8)); // this flag is at uint8 offset 1 !!

                                ch = true;
                            }
                            else if (index == UNIT_FIELD_FACTIONTEMPLATE)
                            {
                                FactionTemplateEntry const* ft1, *ft2;
                                ft1 = ToPlayer()->GetFactionTemplateEntry();
                                ft2 = target->ToPlayer()->GetFactionTemplateEntry();
                                if (ft1 && ft2 && !ft1->IsFriendlyTo(*ft2))
                                {
                                    uint32 faction = target->ToPlayer()->GetFaction(); // pretend that all other HOSTILE players have own faction, to allow follow, heal, rezz (trade wont work)
                                    DEBUG_LOG("-- VALUES_UPDATE: Sending '%s' the blue-group-fix from '%s' (faction %u)", target->GetName(), ToPlayer()->GetName(), faction);
                                    *data << uint32(faction);
                                    ch = true;
                                }
                            }
                        }
                    }
                    if (!ch)
                        *data << m_uint32Values[ index ];
                }
                else if (index == UNIT_FIELD_HEALTH)
                {
                    if (GetTypeId() == TYPEID_UNIT || GetTypeId() == TYPEID_PLAYER)
                    {
                        const Unit* me = reinterpret_cast<const Unit*>(this);
                        if (me->ShouldRevealHealthTo(target))
                            *data << m_uint32Values[ index ];
                        else
                            *data << uint32(std::ceil(me->GetHealthPct()));
                    }
                    else
                        *data << m_uint32Values[ index ];
                }
                else if (index == UNIT_FIELD_MAXHEALTH)
                {
                    if (GetTypeId() == TYPEID_UNIT || GetTypeId() == TYPEID_PLAYER)
                    {
                        const Unit* me = reinterpret_cast<const Unit*>(this);
                        if (me->ShouldRevealHealthTo(target))
                            *data << m_uint32Values[ index ];
                        else
                            *data << uint32(100);
                    }
                    else
                        *data << m_uint32Values[ index ];
                }
                else
                {
                    // send in current format (float as float, uint32 as uint32)
                    *data << m_uint32Values[ index ];
                }
            }
        }
    }
    else if (isType(TYPEMASK_GAMEOBJECT))                    // gameobject case
    {
        for (uint16 index = 0; index < m_valuesCount; ++index)
        {
            if (updateMask->GetBit(index))
            {
                // send in current format (float as float, uint32 as uint32)
                if (index == GAMEOBJECT_DYN_FLAGS)
                {
                    if (IsActivateToQuest)
                    {
                        switch (((GameObject*)this)->GetGoType())
                        {
                        case GAMEOBJECT_TYPE_CHEST:
                        case GAMEOBJECT_TYPE_GOOBER:
                            *data << uint16(GO_DYNFLAG_LO_ACTIVATE | GO_DYNFLAG_LO_SPARKLE);
                            *data << uint16(-1);
                            break;
                        default:
                            *data << uint32(0);         // unknown, not happen.
                            break;
                        }
                    }
                    else
                        *data << uint32(0);                 // disable quest object
                }
                else
                    *data << m_uint32Values[ index ];       // other cases
            }
        }
    }
    else                                                    // other objects case (no special index checks)
    {
        for (uint16 index = 0; index < m_valuesCount; ++index)
        {
            if (updateMask->GetBit(index))
            {
                // send in current format (float as float, uint32 as uint32)
                *data << m_uint32Values[ index ];
            }
        }
    }
}

void Object::ClearUpdateMask(bool remove)
{
    memcpy(m_uint32Values_mirror, m_uint32Values, m_valuesCount * sizeof(uint32));

    if (m_objectUpdated)
    {
        if (remove)
            ObjectAccessor::Instance().RemoveUpdateObject(this);
        m_objectUpdated = false;
    }
}

void Object::BuildFieldsUpdate(Player* pl, UpdateDataMapType& data_map) const
{
    UpdateDataMapType::iterator iter = data_map.find(pl);

    if (iter == data_map.end())
    {
        std::pair<UpdateDataMapType::iterator, bool> p = data_map.insert(UpdateDataMapType::value_type(pl, UpdateData()));
        ASSERT(p.second);
        iter = p.first;
    }

    BuildValuesUpdateBlockForPlayer(&iter->second, iter->first);
}

bool Object::LoadValues(const char* data)
{
    if (!m_uint32Values) _InitValues();

    Tokens tokens = StrSplit(data, " ");

    if (tokens.size() != m_valuesCount)
        return false;

    Tokens::iterator iter;
    int index;
    for (iter = tokens.begin(), index = 0; index < m_valuesCount; ++iter, ++index)
        m_uint32Values[index] = atol((*iter).c_str());

    return true;
}

void Object::_LoadIntoDataField(const char* data, uint32 startOffset, uint32 count)
{
    if (!data)
        return;

    Tokens tokens = StrSplit(data, " ");

    if (tokens.size() != count)
        return;

    Tokens::iterator iter;
    uint32 index;
    for (iter = tokens.begin(), index = 0; index < count; ++iter, ++index)
        m_uint32Values[startOffset + index] = atol((*iter).c_str());
}

void Object::_SetUpdateBits(UpdateMask* updateMask, Player* /*target*/) const
{
    uint32* value = m_uint32Values;
    uint32* mirror = m_uint32Values_mirror;

    for (uint16 index = 0; index < m_valuesCount; ++index, ++value, ++mirror)
    {
        if (*mirror != *value)
            updateMask->SetBit(index);
    }
}

void Object::_SetCreateBits(UpdateMask* updateMask, Player* /*target*/) const
{
    uint32* value = m_uint32Values;

    for (uint16 index = 0; index < m_valuesCount; ++index, ++value)
    {
        if (*value)
            updateMask->SetBit(index);
    }
}

void Object::SetInt32Value(uint16 index, int32 value)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (m_int32Values[ index ] != value)
    {
        m_int32Values[ index ] = value;

        if (m_inWorld)
        {
            if (!m_objectUpdated)
            {
                ObjectAccessor::Instance().AddUpdateObject(this);
                m_objectUpdated = true;
            }
        }
    }
}

void Object::SetUInt32Value(uint16 index, uint32 value)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (m_uint32Values[ index ] != value)
    {
        m_uint32Values[ index ] = value;

        if (m_inWorld)
        {
            if (!m_objectUpdated)
            {
                ObjectAccessor::Instance().AddUpdateObject(this);
                m_objectUpdated = true;
            }
        }
    }
}

void Object::SetUInt64Value(uint16 index, const uint64& value)
{
    ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, true));
    if (*((uint64*) & (m_uint32Values[ index ])) != value)
    {
        m_uint32Values[ index ] = *((uint32*)&value);
        m_uint32Values[ index + 1 ] = *(((uint32*)&value) + 1);

        if (m_inWorld)
        {
            if (!m_objectUpdated)
            {
                ObjectAccessor::Instance().AddUpdateObject(this);
                m_objectUpdated = true;
            }
        }
    }
}

bool Object::AddUInt64Value(uint16 index, const uint64& value)
{
    ASSERT(index + 1 < m_valuesCount || PrintIndexError(index , true));
    if (value && !*((uint64*) & (m_uint32Values[index])))
    {
        m_uint32Values[ index ] = *((uint32*)&value);
        m_uint32Values[ index + 1 ] = *(((uint32*)&value) + 1);

        if (m_inWorld)
        {
            if (!m_objectUpdated)
            {
                ObjectAccessor::Instance().AddUpdateObject(this);
                m_objectUpdated = true;
            }
        }
        return true;
    }
    return false;
}

bool Object::RemoveUInt64Value(uint16 index, const uint64& value)
{
    ASSERT(index + 1 < m_valuesCount || PrintIndexError(index , true));
    if (value && *((uint64*) & (m_uint32Values[index])) == value)
    {
        m_uint32Values[ index ] = 0;
        m_uint32Values[ index + 1 ] = 0;

        if (m_inWorld)
        {
            if (!m_objectUpdated)
            {
                ObjectAccessor::Instance().AddUpdateObject(this);
                m_objectUpdated = true;
            }
        }
        return true;
    }
    return false;
}

void Object::SetFloatValue(uint16 index, float value)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (m_floatValues[ index ] != value)
    {
        m_floatValues[ index ] = value;

        if (m_inWorld)
        {
            if (!m_objectUpdated)
            {
                ObjectAccessor::Instance().AddUpdateObject(this);
                m_objectUpdated = true;
            }
        }
    }
}

void Object::SetByteValue(uint16 index, uint8 offset, uint8 value)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 4)
    {
        sLog.outError("Object::SetByteValue: wrong offset %u", offset);
        return;
    }

    if (uint8(m_uint32Values[ index ] >> (offset * 8)) != value)
    {
        m_uint32Values[ index ] &= ~uint32(uint32(0xFF) << (offset * 8));
        m_uint32Values[ index ] |= uint32(uint32(value) << (offset * 8));

        if (m_inWorld)
        {
            if (!m_objectUpdated)
            {
                ObjectAccessor::Instance().AddUpdateObject(this);
                m_objectUpdated = true;
            }
        }
    }
}

void Object::SetUInt16Value(uint16 index, uint8 offset, uint16 value)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 2)
    {
        sLog.outError("Object::SetUInt16Value: wrong offset %u", offset);
        return;
    }

    if (uint16(m_uint32Values[ index ] >> (offset * 16)) != value)
    {
        m_uint32Values[ index ] &= ~uint32(uint32(0xFFFF) << (offset * 16));
        m_uint32Values[ index ] |= uint32(uint32(value) << (offset * 16));

        if (m_inWorld)
        {
            if (!m_objectUpdated)
            {
                ObjectAccessor::Instance().AddUpdateObject(this);
                m_objectUpdated = true;
            }
        }
    }
}

void Object::SetStatFloatValue(uint16 index, float value)
{
    if (value < 0)
        value = 0.0f;

    SetFloatValue(index, value);
}

void Object::SetStatInt32Value(uint16 index, int32 value)
{
    if (value < 0)
        value = 0;

    SetUInt32Value(index, uint32(value));
}

void Object::ApplyModUInt32Value(uint16 index, int32 val, bool apply)
{
    int32 cur = GetUInt32Value(index);
    cur += (apply ? val : -val);
    if (cur < 0)
        cur = 0;
    SetUInt32Value(index, cur);
}

void Object::ApplyModInt32Value(uint16 index, int32 val, bool apply)
{
    int32 cur = GetInt32Value(index);
    cur += (apply ? val : -val);
    SetInt32Value(index, cur);
}

void Object::ApplyModSignedFloatValue(uint16 index, float  val, bool apply)
{
    float cur = GetFloatValue(index);
    cur += (apply ? val : -val);
    SetFloatValue(index, cur);
}

void Object::ApplyPercentModFloatValue(uint16 index, float val, bool apply)
{
    float value = GetFloatValue(index);
    ApplyPercentModFloatVar(value, val, apply);
    SetFloatValue(index, value);
}

void Object::ApplyModPositiveFloatValue(uint16 index, float  val, bool apply)
{
    float cur = GetFloatValue(index);
    cur += (apply ? val : -val);
    if (cur < 0)
        cur = 0;
    SetFloatValue(index, cur);
}

void Object::SetFlag(uint16 index, uint32 newFlag)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));
    uint32 oldval = m_uint32Values[ index ];
    uint32 newval = oldval | newFlag;

    if (oldval != newval)
    {
        m_uint32Values[ index ] = newval;

        if (m_inWorld)
        {
            if (!m_objectUpdated)
            {
                ObjectAccessor::Instance().AddUpdateObject(this);
                m_objectUpdated = true;
            }
        }
    }
}

void Object::RemoveFlag(uint16 index, uint32 oldFlag)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));
    ASSERT(m_uint32Values);

    uint32 oldval = m_uint32Values[ index ];
    uint32 newval = oldval & ~oldFlag;

    if (oldval != newval)
    {
        m_uint32Values[ index ] = newval;

        if (m_inWorld)
        {
            if (!m_objectUpdated)
            {
                ObjectAccessor::Instance().AddUpdateObject(this);
                m_objectUpdated = true;
            }
        }
    }
}

void Object::SetByteFlag(uint16 index, uint8 offset, uint8 newFlag)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 4)
    {
        sLog.outError("Object::SetByteFlag: wrong offset %u", offset);
        return;
    }

    if (!(uint8(m_uint32Values[ index ] >> (offset * 8)) & newFlag))
    {
        m_uint32Values[ index ] |= uint32(uint32(newFlag) << (offset * 8));

        if (m_inWorld)
        {
            if (!m_objectUpdated)
            {
                ObjectAccessor::Instance().AddUpdateObject(this);
                m_objectUpdated = true;
            }
        }
    }
}

void Object::RemoveByteFlag(uint16 index, uint8 offset, uint8 oldFlag)
{
    ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 4)
    {
        sLog.outError("Object::RemoveByteFlag: wrong offset %u", offset);
        return;
    }

    if (uint8(m_uint32Values[ index ] >> (offset * 8)) & oldFlag)
    {
        m_uint32Values[ index ] &= ~uint32(uint32(oldFlag) << (offset * 8));

        if (m_inWorld)
        {
            if (!m_objectUpdated)
            {
                ObjectAccessor::Instance().AddUpdateObject(this);
                m_objectUpdated = true;
            }
        }
    }
}

bool Object::PrintIndexError(uint32 index, bool set) const
{
    sLog.outError("Attempt %s invalid value field: %u (count: %u) for object typeid: %u type mask: %u", (set ? "set value to" : "get value from"), index, m_valuesCount, GetTypeId(), m_objectType);

    // assert must fail after function call
    return false;
}

WorldObject::WorldObject(bool isWorldObject):
      WorldLocation()
    , m_groupLootTimer(0)
    , lootingGroupLeaderGUID(0)
    , m_isWorldObject(isWorldObject)
    , m_name("")
    , m_isActive(false)
    , m_zoneScript(NULL)
    , m_currMap(NULL)
    , m_InstanceId(0)
    , m_notifyflags(0), m_executed_notifies(0)
{
    m_serverSideVisibility.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_ALIVE | GHOST_VISIBILITY_GHOST);
    m_serverSideVisibilityDetect.SetValue(SERVERSIDE_VISIBILITY_GHOST, GHOST_VISIBILITY_ALIVE);
}

void WorldObject::SetWorldObject(bool on)
{
    if (!IsInWorld())
        return;

    GetMap()->AddObjectToSwitchList(this, on);
}

bool WorldObject::IsWorldObject() const
{
    if (m_isWorldObject)
        return true;

    if (ToCreature() && ToCreature()->m_isTempWorldObject)
        return true;

    return false;
}

void WorldObject::setActive(bool on)
{
    if (m_isActive == on)
        return;

    if (GetTypeId() == TYPEID_PLAYER)
        return;

    m_isActive = on;

    if (!IsInWorld())
        return;

    Map* map = FindMap();
    if (!map)
        return;

    if (on)
    {
        if (GetTypeId() == TYPEID_UNIT)
            map->AddToActive(ToCreature());
        else if (GetTypeId() == TYPEID_DYNAMICOBJECT)
            map->AddToActive((DynamicObject*)this);
    }
    else
    {
        if (GetTypeId() == TYPEID_UNIT)
            map->RemoveFromActive(ToCreature());
        else if (GetTypeId() == TYPEID_DYNAMICOBJECT)
            map->RemoveFromActive((DynamicObject*)this);
    }
}

void WorldObject::CleanupsBeforeDelete()
{
    if (IsInWorld())
        RemoveFromWorld();
}

void WorldObject::_Create(uint32 guidlow, HighGuid guidhigh)
{
    Object::_Create(guidlow, 0, guidhigh);
}

void WorldObject::RemoveFromWorld()
{
    if (!IsInWorld())
        return;

    DestroyForNearbyPlayers();

    Object::RemoveFromWorld();
}

uint32 WorldObject::GetZoneId() const
{
    return GetBaseMap()->GetZoneId(m_positionX, m_positionY, m_positionZ);
}

uint32 WorldObject::GetAreaId() const
{
    return GetBaseMap()->GetAreaId(m_positionX, m_positionY, m_positionZ);
}

InstanceData* WorldObject::GetInstanceData()
{
    Map* map = GetMap();
    return map->IsDungeon() ? ((InstanceMap*)map)->GetInstanceData() : NULL;
}

float WorldObject::GetDistanceZ(const WorldObject* obj) const
{
    float dz = fabs(GetPositionZ() - obj->GetPositionZ());
    float sizefactor = GetObjectSize() + obj->GetObjectSize();
    float dist = dz - sizefactor;
    return (dist > 0 ? dist : 0);
}

float WorldObject::GetDistanceSqr(float x, float y, float z) const
{
    float dx = GetPositionX() - x;
    float dy = GetPositionY() - y;
    float dz = GetPositionZ() - z;
    float sizefactor = GetObjectSize();
    float dist = dx * dx + dy * dy + dz * dz - sizefactor;
    return (dist > 0 ? dist : 0);
}

bool WorldObject::_IsWithinDist(WorldObject const* obj, float dist2compare, bool is3D) const
{
    float dx = GetPositionX() - obj->GetPositionX();
    float dy = GetPositionY() - obj->GetPositionY();
    float distsq = dx * dx + dy * dy;
    if (is3D)
    {
        float dz = GetPositionZ() - obj->GetPositionZ();
        distsq += dz * dz;
    }
    float sizefactor = GetObjectSize() + obj->GetObjectSize();
    float maxdist = dist2compare + sizefactor;

    return distsq < maxdist * maxdist;
}

bool WorldObject::IsWithinLOSInMap(const WorldObject* obj) const
{
    if (!IsInMap(obj))
        return false;

    float x, y, z;
    if (obj->GetTypeId() == TYPEID_PLAYER)
        obj->GetPosition(x, y, z);
    else
        obj->GetHitSpherePointFor(GetPosition(), x, y, z);

    return IsWithinLOS(x, y, z);
}

bool WorldObject::IsWithinLOS(float ox, float oy, float oz) const
{
    if (IsInWorld())
    {
        float x, y, z;
        if (GetTypeId() == TYPEID_PLAYER)
            GetPosition(x, y, z);
        else
            GetHitSpherePointFor({ ox, oy, oz }, x, y, z);

        return GetMap()->isInLineOfSight(x, y, z + 2.0f, ox, oy, oz + 2.0f);
    }

    return true;
}

Position WorldObject::GetHitSpherePointFor(Position const& dest) const
{
    G3D::Vector3 vThis(GetPositionX(), GetPositionY(), GetPositionZ());
    G3D::Vector3 vObj(dest.GetPositionX(), dest.GetPositionY(), dest.GetPositionZ());
    G3D::Vector3 contactPoint = vThis + (vObj - vThis).directionOrZero() * GetObjectSize();

    return Position(contactPoint.x, contactPoint.y, contactPoint.z, GetAngle(contactPoint.x, contactPoint.y));
}

void WorldObject::GetHitSpherePointFor(Position const& dest, float& x, float& y, float& z) const
{
    Position pos = GetHitSpherePointFor(dest);
    x = pos.GetPositionX();
    y = pos.GetPositionY();
    z = pos.GetPositionZ();
}
 
bool WorldObject::GetDistanceOrder(WorldObject const* obj1, WorldObject const* obj2, bool is3D /* = true */) const
{
    float dx1 = GetPositionX() - obj1->GetPositionX();
    float dy1 = GetPositionY() - obj1->GetPositionY();
    float distsq1 = dx1 * dx1 + dy1 * dy1;
    if (is3D)
    {
        float dz1 = GetPositionZ() - obj1->GetPositionZ();
        distsq1 += dz1 * dz1;
    }

    float dx2 = GetPositionX() - obj2->GetPositionX();
    float dy2 = GetPositionY() - obj2->GetPositionY();
    float distsq2 = dx2 * dx2 + dy2 * dy2;
    if (is3D)
    {
        float dz2 = GetPositionZ() - obj2->GetPositionZ();
        distsq2 += dz2 * dz2;
    }

    return distsq1 < distsq2;
}

bool WorldObject::IsInRange(WorldObject const* obj, float minRange, float maxRange, bool is3D /* = true */) const
{
    float dx = GetPositionX() - obj->GetPositionX();
    float dy = GetPositionY() - obj->GetPositionY();
    float distsq = dx * dx + dy * dy;
    if (is3D)
    {
        float dz = GetPositionZ() - obj->GetPositionZ();
        distsq += dz * dz;
    }

    float sizefactor = GetObjectSize() + obj->GetObjectSize();

    // check only for real range
    if (minRange > 0.0f)
    {
        float mindist = minRange + sizefactor;
        if (distsq < mindist * mindist)
            return false;
    }

    float maxdist = maxRange + sizefactor;
    return distsq < maxdist * maxdist;
}

bool WorldObject::IsInRange2d(float x, float y, float minRange, float maxRange) const
{
    float dx = GetPositionX() - x;
    float dy = GetPositionY() - y;
    float distsq = dx * dx + dy * dy;

    float sizefactor = GetObjectSize();

    // check only for real range
    if (minRange > 0.0f)
    {
        float mindist = minRange + sizefactor;
        if (distsq < mindist * mindist)
            return false;
    }

    float maxdist = maxRange + sizefactor;
    return distsq < maxdist * maxdist;
}

bool WorldObject::IsInRange3d(float x, float y, float z, float minRange, float maxRange) const
{
    float dx = GetPositionX() - x;
    float dy = GetPositionY() - y;
    float dz = GetPositionZ() - z;
    float distsq = dx * dx + dy * dy + dz * dz;

    float sizefactor = GetObjectSize();

    // check only for real range
    if (minRange > 0.0f)
    {
        float mindist = minRange + sizefactor;
        if (distsq < mindist * mindist)
            return false;
    }

    float maxdist = maxRange + sizefactor;
    return distsq < maxdist * maxdist;
}

bool WorldObject::HasInArc(const float arcangle, const float x, const float y) const
{
    // always have self in arc
    if (x == m_positionX && y == m_positionY)
        return true;

    float arc = arcangle;

    // move arc to range 0.. 2*pi
    while ( arc >= 2.0f * float(M_PI) )
        arc -=  2.0f * float(M_PI);
    while ( arc < 0 )
        arc +=  2.0f * float(M_PI);

    float angle = GetAngle( x, y );
    angle -= GetOrientation();

    // move angle to range -pi ... +pi
    while ( angle > float(M_PI))
        angle -= 2.0f * float(M_PI);
    while (angle < -M_PI)
        angle += 2.0f * float(M_PI);

    float lborder =  -1 * (arc / 2.0f);                     // in range -pi..0
    float rborder = (arc / 2.0f);                           // in range 0..pi
    return (( angle >= lborder ) && ( angle <= rborder ));
}

bool WorldObject::HasInArc(float arc, const Position* obj) const
{
    return this->HasInArc(arc, obj->GetPositionX(), obj->GetPositionY());
}

void WorldObject::GetRandomPoint(const Position& pos, float distance, float& rand_x, float& rand_y, float& rand_z) const
{
    if (!distance)
    {
        pos.GetPosition(rand_x, rand_y, rand_z);
        return;
    }

    // angle to face `obj` to `this`
    float angle = rand_norm() * 2 * M_PI;
    float new_dist = rand_norm() * distance;

    rand_x = pos.m_positionX + new_dist * cos(angle);
    rand_y = pos.m_positionY + new_dist * sin(angle);
    rand_z = pos.m_positionZ;

    Oregon::NormalizeMapCoord(rand_x);
    Oregon::NormalizeMapCoord(rand_y);
    UpdateGroundPositionZ(rand_x, rand_y, rand_z);          // update to LOS height if available
}

void WorldObject::UpdateAllowedPositionZ(float x, float y, float& z) const
{
    switch (GetTypeId())
    {
    case TYPEID_UNIT:
        {
            // non fly unit don't must be in air
            // non swim unit must be at ground (mostly speedup, because it don't must be in water and water level check less fast
            if (!((Creature const*)this)->CanFly())
            {
                bool canSwim = ((Creature const*)this)->CanSwim();
                float ground_z = z;
                float max_z = canSwim
                              ? GetBaseMap()->GetWaterOrGroundLevel(x, y, z, &ground_z, !((Unit const*)this)->HasAuraType(SPELL_AURA_WATER_WALK))
                              : ((ground_z = GetBaseMap()->GetHeight(x, y, z, true)));
                if (max_z > INVALID_HEIGHT)
                {
                    if (z > max_z)
                        z = max_z;
                    else if (z < ground_z)
                        z = ground_z;
                }
            }
            else
            {
                float ground_z = GetBaseMap()->GetHeight(x, y, z, true);
                if (z < ground_z)
                    z = ground_z;
            }
            break;
        }
    case TYPEID_PLAYER:
        {
            // for server controlled moves playr work same as creature (but it can always swim)
            if (!((Player const*)this)->CanFly())
            {
                float ground_z = z;
                float max_z = GetBaseMap()->GetWaterOrGroundLevel(x, y, z, &ground_z, !((Unit const*)this)->HasAuraType(SPELL_AURA_WATER_WALK));
                if (max_z > INVALID_HEIGHT)
                {
                    if (z > max_z)
                        z = max_z;
                    else if (z < ground_z)
                        z = ground_z;
                }
            }
            else
            {
                float ground_z = GetBaseMap()->GetHeight(x, y, z, true);
                if (z < ground_z)
                    z = ground_z;
            }
            break;
        }
    default:
        {
            float ground_z = GetBaseMap()->GetHeight(x, y, z, true);
            if (ground_z > INVALID_HEIGHT)
                z = ground_z;
            break;
        }
    }
}

void WorldObject::UpdateGroundPositionZ(float x, float y, float& z) const
{
    float new_z = GetBaseMap()->GetHeight(x, y, z, true);
    if (new_z > INVALID_HEIGHT)
        z = new_z + 0.05f;                                  // just to be sure that we are not a few pixel under the surface
}

void WorldObject::MonsterSay(const char* text, uint32 language, uint64 TargetGuid)
{
    WorldPacket data(SMSG_MESSAGECHAT, 200);
    BuildMonsterChat(&data, CHAT_MSG_MONSTER_SAY, text, language, GetName(), TargetGuid);
    SendMessageToSetInRange(&data, sWorld.getConfig(CONFIG_LISTEN_RANGE_SAY), true);
}

void WorldObject::MonsterYell(const char* text, uint32 language, uint64 TargetGuid)
{
    WorldPacket data(SMSG_MESSAGECHAT, 200);
    BuildMonsterChat(&data, CHAT_MSG_MONSTER_YELL, text, language, GetName(), TargetGuid);
    SendMessageToSetInRange(&data, sWorld.getConfig(CONFIG_LISTEN_RANGE_YELL), true);
}

void WorldObject::MonsterTextEmote(const char* text, uint64 TargetGuid, bool IsBossEmote)
{
    WorldPacket data(SMSG_MESSAGECHAT, 200);
    BuildMonsterChat(&data, IsBossEmote ? CHAT_MSG_RAID_BOSS_EMOTE : CHAT_MSG_MONSTER_EMOTE, text, LANG_UNIVERSAL, GetName(), TargetGuid);
    SendMessageToSetInRange(&data, sWorld.getConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE), true);
}

void WorldObject::MonsterWhisper(const char* text, uint64 receiver, bool IsBossWhisper)
{
    Player* player = sObjectMgr.GetPlayer(receiver);
    if (!player || !player->GetSession())
        return;

    WorldPacket data(SMSG_MESSAGECHAT, 200);
    BuildMonsterChat(&data, IsBossWhisper ? CHAT_MSG_RAID_BOSS_WHISPER : CHAT_MSG_MONSTER_WHISPER, text, LANG_UNIVERSAL, GetName(), receiver);

    player->GetSession()->SendPacket(&data);
}

bool WorldObject::isValid() const
{
    if (!IsInWorld())
        return false;

    return true;
}

float WorldObject::GetGridActivationRange() const
{
    if (ToPlayer())
    {
        if (ToPlayer()->GetCinematicMgr()->IsOnCinematic())
            return DEFAULT_VISIBILITY_INSTANCE;
        return GetMap()->GetVisibilityRange();
    }
    else if (ToCreature())
        return ToCreature()->m_SightDistance;
    else if (ToDynObject())
    {
        if (isActiveObject())
            return GetMap()->GetVisibilityRange();
        else
            return 0.0f;
    }
    else
        return 0.0f;
}

float WorldObject::GetVisibilityRange() const
{
    if (isActiveObject() && !ToPlayer())
        return MAX_VISIBILITY_DISTANCE;
    else
        return GetMap()->GetVisibilityRange();
}

float WorldObject::GetSightRange(const WorldObject* target) const
{
    if (ToUnit())
    {
        if (ToPlayer())
        {
            if (target && target->isActiveObject() && !target->ToPlayer())
                return MAX_VISIBILITY_DISTANCE;
            else if (ToPlayer()->GetCinematicMgr()->IsOnCinematic())
                return DEFAULT_VISIBILITY_INSTANCE;
            else
                return GetMap()->GetVisibilityRange();
        }
        else if (ToCreature())
            return ToCreature()->m_SightDistance;
        else
            return SIGHT_RANGE_UNIT;
    }

    if (ToDynObject() && isActiveObject())
        return GetMap()->GetVisibilityRange();

    return 0.0f;
}

bool WorldObject::CanSeeOrDetect(WorldObject const* obj, bool ignoreStealth, bool distanceCheck, bool checkAlert) const
{
    if (this == obj)
        return true;
    
    if (obj->IsNeverVisible() || CanNeverSee(obj))
        return false;

    if (obj->IsAlwaysVisibleFor(this) || CanAlwaysSee(obj))
        return true;

    bool corpseVisibility = false;
    if (distanceCheck)
    {
        bool corpseCheck = false;
        if (const Player* thisPlayer = ToPlayer())
        {
            if (thisPlayer->isDead() && thisPlayer->GetHealth() > 0 && // Cheap way to check for ghost state
                !(obj->m_serverSideVisibility.GetValue(SERVERSIDE_VISIBILITY_GHOST) & m_serverSideVisibility.GetValue(SERVERSIDE_VISIBILITY_GHOST) & GHOST_VISIBILITY_GHOST))
            {
                if (Corpse* corpse = thisPlayer->GetCorpse())
                {
                    corpseCheck = true;
                    if (corpse->IsWithinDist(thisPlayer, GetSightRange(obj), false))
                        if (corpse->IsWithinDist(obj, GetSightRange(obj), false))
                            corpseVisibility = true;
                }
            }
        }

        WorldObject const* viewpoint = this;
        if (Player const* player = this->ToPlayer())
            viewpoint = player->GetViewpoint();

        if (!viewpoint)
            viewpoint = this;

        if (!corpseCheck && !viewpoint->IsWithinDist(obj, GetSightRange(obj), false))
            return false;
    }

    // GM visibility off or hidden NPC
    if (!obj->m_serverSideVisibility.GetValue(SERVERSIDE_VISIBILITY_GM))
    {
        // Stop checking other things for GMs
        if (m_serverSideVisibilityDetect.GetValue(SERVERSIDE_VISIBILITY_GM))
            return true;
    }
    else
        return m_serverSideVisibilityDetect.GetValue(SERVERSIDE_VISIBILITY_GM) >= obj->m_serverSideVisibility.GetValue(SERVERSIDE_VISIBILITY_GM);

    // Ghost players, Spirit Healers, and some other NPCs
    if (!corpseVisibility && !(obj->m_serverSideVisibility.GetValue(SERVERSIDE_VISIBILITY_GHOST) & m_serverSideVisibilityDetect.GetValue(SERVERSIDE_VISIBILITY_GHOST)))
    {
        // Alive players can see dead players in some cases, but other objects can't do that
        if (const Player* thisPlayer = ToPlayer())
        {
            if (const Player* objPlayer = obj->ToPlayer())
            {
                if (thisPlayer->GetTeam() != objPlayer->GetTeam() || !thisPlayer->IsGroupVisibleFor(objPlayer))
                    return false;
            }
            else
                return false;
        }
        else
            return false;
    }

    if (obj->IsInvisibleDueToDespawn())
        return false;

    if (!CanDetect(obj, ignoreStealth, checkAlert))
        return false;

    return true;
}

bool WorldObject::CanNeverSee(WorldObject const* obj) const
{
    return GetMap() != obj->GetMap();
}

bool WorldObject::CanDetect(WorldObject const* obj, bool ignoreStealth, bool checkAlert) const
{
    const WorldObject* seer = this;

    // Pets don't have detection, they use the detection of their masters
    if (const Unit* thisUnit = ToUnit())
        if (Unit* controller = thisUnit->GetCharmerOrOwner())
            seer = controller;

    if (obj->IsAlwaysDetectableFor(seer))
        return true;

    if (!ignoreStealth && !seer->CanDetectInvisibilityOf(obj))
        return false;

    if (!ignoreStealth && !seer->CanDetectStealthOf(obj, checkAlert))
        return false;

    return true;
}

bool WorldObject::CanDetectInvisibilityOf(WorldObject const* obj) const
{
    uint32 mask = obj->m_invisibility.GetFlags() & m_invisibilityDetect.GetFlags();

    // Check for not detected types
    if (mask != obj->m_invisibility.GetFlags())
        return false;

    // It isn't possible in invisibility to detect something that can't detect the invisible object
    // (it's at least true for spell: 66)
    // It seems like that only Units are affected by this check (couldn't see arena doors with preparation invisibility)
    if (obj->ToUnit())
        if ((m_invisibility.GetFlags() & obj->m_invisibilityDetect.GetFlags()) != m_invisibility.GetFlags())
            return false;

    for (uint32 i = 0; i < TOTAL_INVISIBILITY_TYPES; ++i)
    {
        if (!(mask & (1 << i)))
            continue;

        int32 objInvisibilityValue = obj->m_invisibility.GetValue(InvisibilityType(i));
        int32 ownInvisibilityDetectValue = m_invisibilityDetect.GetValue(InvisibilityType(i));

        // Too low value to detect
        if (ownInvisibilityDetectValue < objInvisibilityValue)
            return false;
    }

    return true;
}

bool WorldObject::CanDetectStealthOf(WorldObject const* obj, bool checkAlert) const
{
    // Combat reach is the minimal distance (both in front and behind),
    //   and it is also used in the range calculation.
    // One stealth point increases the visibility range by 0.3 yard.

    if (!obj->m_stealth.GetFlags())
        return true;

    float distance = GetExactDist(obj);
    float combatReach = 0.0f;

    Unit const* unit = ToUnit();
    if (unit)
        combatReach = unit->GetCombatReach();

    if (distance < combatReach)
        return true;

    if (!HasInArc(float(M_PI), obj))
        return false;

    GameObject const* go = ToGameObject();
    for (uint32 i = 0; i < TOTAL_STEALTH_TYPES; ++i)
    {
        if (!(obj->m_stealth.GetFlags() & (1 << i)))
            continue;

        if (unit && unit->HasAuraTypeWithMiscvalue(SPELL_AURA_DETECT_STEALTH, i))
            return true;

        // Starting points
        int32 detectionValue = 30;

        // Level difference: 5 point / level, starting from level 1.
        // There may be spells for this and the starting points too, but
        //   not in the DBCs of the client.
        detectionValue += int32(getLevelForTarget(obj) - 1) * 5;

        // Apply modifiers
        detectionValue += m_stealthDetect.GetValue(StealthType(i));
        if (go)
            if (Unit* owner = go->GetOwner())
                detectionValue -= int32(owner->getLevelForTarget(this) - 1) * 5;

        detectionValue -= obj->m_stealth.GetValue(StealthType(i));

        // Calculate max distance
        float visibilityRange = float(detectionValue) * 0.3f + combatReach;

        // If this unit is an NPC then player detect range doesn't apply
        if (unit && unit->GetTypeId() == TYPEID_PLAYER && visibilityRange > MAX_PLAYER_STEALTH_DETECT_RANGE)
            visibilityRange = MAX_PLAYER_STEALTH_DETECT_RANGE;

        // When checking for alert state, look 8% further, and then 1.5 yards more than that.
        if (checkAlert)
            visibilityRange += (visibilityRange * 0.08f) + 1.5f;

        // If checking for alert, and creature's visibility range is greater than aggro distance, No alert
        Unit const* tunit = obj->ToUnit();
        if (checkAlert && unit && unit->ToCreature() && visibilityRange >= unit->ToCreature()->GetAttackDistance(tunit) + unit->ToCreature()->m_CombatDistance)
            return false;

        if (distance > visibilityRange)
            return false;
    }

    return true;
}

void WorldObject::SendPlaySound(uint32 Sound, bool OnlySelf)
{
    WorldPacket data(SMSG_PLAY_SOUND, 4);
    data << Sound;
    if (OnlySelf && GetTypeId() == TYPEID_PLAYER)
        ToPlayer()->GetSession()->SendPacket(&data);
    else
        SendMessageToSet(&data, true); // ToSelf ignored in this case
}

void Object::ForceValuesUpdateAtIndex(uint32 i)
{
    m_uint32Values_mirror[i] = GetUInt32Value(i) + 1; // makes server think the field changed
    if (m_inWorld)
    {
        if (!m_objectUpdated)
        {
            ObjectAccessor::Instance().AddUpdateObject(this);
            m_objectUpdated = true;
        }
    }
}

namespace Oregon
{
class MonsterChatBuilder
{
    public:
        MonsterChatBuilder(WorldObject const& obj, ChatMsg msgtype, int32 textId, uint32 language, uint64 targetGUID)
            : i_object(obj), i_msgtype(msgtype), i_textId(textId), i_language(language), i_targetGUID(targetGUID) {}
        void operator()(WorldPacket& data, int32 loc_idx)
        {
            char const* text = sObjectMgr.GetOregonString(i_textId, loc_idx);

            // @todo i_object.GetName() also must be localized?
            i_object.BuildMonsterChat(&data, i_msgtype, text, i_language, i_object.GetNameForLocaleIdx(loc_idx), i_targetGUID);
        }

    private:
        WorldObject const& i_object;
        ChatMsg i_msgtype;
        int32 i_textId;
        uint32 i_language;
        uint64 i_targetGUID;
};
}                                                           // namespace Oregon

void WorldObject::MonsterSay(int32 textId, uint32 language, uint64 TargetGuid)
{
    CellCoord p = Oregon::ComputeCellCoord(GetPositionX(), GetPositionY());

    Cell cell(p);
    cell.SetNoCreate();

    Oregon::MonsterChatBuilder say_build(*this, CHAT_MSG_MONSTER_SAY, textId, language, TargetGuid);
    Oregon::LocalizedPacketDo<Oregon::MonsterChatBuilder> say_do(say_build);
    Oregon::PlayerDistWorker<Oregon::LocalizedPacketDo<Oregon::MonsterChatBuilder> > say_worker(sWorld.getConfig(CONFIG_LISTEN_RANGE_SAY), say_do);
    TypeContainerVisitor<Oregon::PlayerDistWorker<Oregon::LocalizedPacketDo<Oregon::MonsterChatBuilder> >, WorldTypeMapContainer > message(say_worker);
    cell.Visit(p, message, *GetMap(), *this, sWorld.getConfig(CONFIG_LISTEN_RANGE_SAY));
}

void WorldObject::MonsterYell(int32 textId, uint32 language, uint64 TargetGuid)
{
    CellCoord p = Oregon::ComputeCellCoord(GetPositionX(), GetPositionY());

    Cell cell(p);
    cell.SetNoCreate();

    Oregon::MonsterChatBuilder say_build(*this, CHAT_MSG_MONSTER_YELL, textId, language, TargetGuid);
    Oregon::LocalizedPacketDo<Oregon::MonsterChatBuilder> say_do(say_build);
    Oregon::PlayerDistWorker<Oregon::LocalizedPacketDo<Oregon::MonsterChatBuilder> > say_worker(sWorld.getConfig(CONFIG_LISTEN_RANGE_YELL), say_do);
    TypeContainerVisitor<Oregon::PlayerDistWorker<Oregon::LocalizedPacketDo<Oregon::MonsterChatBuilder> >, WorldTypeMapContainer > message(say_worker);
    cell.Visit(p, message, *GetMap(), *this, sWorld.getConfig(CONFIG_LISTEN_RANGE_YELL));
}

void WorldObject::MonsterYellToZone(int32 textId, uint32 language, uint64 TargetGuid)
{
    Oregon::MonsterChatBuilder say_build(*this, CHAT_MSG_MONSTER_YELL, textId, language, TargetGuid);
    Oregon::LocalizedPacketDo<Oregon::MonsterChatBuilder> say_do(say_build);

    uint32 zoneid = GetZoneId();

    Map::PlayerList const& pList = GetMap()->GetPlayers();
    for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
        if (itr->GetSource()->GetZoneId() == zoneid)
            say_do(itr->GetSource());
}

void WorldObject::MonsterTextEmote(int32 textId, uint64 TargetGuid, bool IsBossEmote)
{
    CellCoord p = Oregon::ComputeCellCoord(GetPositionX(), GetPositionY());

    Cell cell(p);
    cell.SetNoCreate();

    Oregon::MonsterChatBuilder say_build(*this, IsBossEmote ? CHAT_MSG_RAID_BOSS_EMOTE : CHAT_MSG_MONSTER_EMOTE, textId, LANG_UNIVERSAL, TargetGuid);
    Oregon::LocalizedPacketDo<Oregon::MonsterChatBuilder> say_do(say_build);
    Oregon::PlayerDistWorker<Oregon::LocalizedPacketDo<Oregon::MonsterChatBuilder> > say_worker(sWorld.getConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE), say_do);
    TypeContainerVisitor<Oregon::PlayerDistWorker<Oregon::LocalizedPacketDo<Oregon::MonsterChatBuilder> >, WorldTypeMapContainer > message(say_worker);
    cell.Visit(p, message, *GetMap(), *this, sWorld.getConfig(CONFIG_LISTEN_RANGE_TEXTEMOTE));
}

void WorldObject::MonsterWhisper(int32 textId, uint64 receiver, bool IsBossWhisper)
{
    Player* player = sObjectMgr.GetPlayer(receiver);
    if (!player || !player->GetSession())
        return;

    uint32 loc_idx = player->GetSession()->GetSessionDbLocaleIndex();
    char const* text = sObjectMgr.GetOregonString(textId, loc_idx);

    WorldPacket data(SMSG_MESSAGECHAT, 200);
    BuildMonsterChat(&data, IsBossWhisper ? CHAT_MSG_RAID_BOSS_WHISPER : CHAT_MSG_MONSTER_WHISPER, text, LANG_UNIVERSAL, GetName(), receiver);

    player->GetSession()->SendPacket(&data);
}

void WorldObject::BuildMonsterChat(WorldPacket* data, uint8 msgtype, char const* text, uint32 language, char const* name, uint64 targetGuid) const
{
    *data << (uint8)msgtype;
    *data << (uint32)language;
    *data << (uint64)GetGUID();
    *data << (uint32)0;                                     //2.1.0
    *data << (uint32)(strlen(name) + 1);
    *data << name;
    *data << (uint64)targetGuid;                            //Unit Target
    if (targetGuid && !IS_PLAYER_GUID(targetGuid))
    {
        *data << (uint32)1;                                 // target name length
        *data << (uint8)0;                                  // target name
    }
    *data << (uint32)(strlen(text) + 1);
    *data << text;
    *data << (uint8)0;                                      // ChatTag
}

void Unit::BuildHeartBeatMsg(WorldPacket* data) const
{
    data->Initialize(MSG_MOVE_HEARTBEAT, 32);
    *data << GetPackGUID();
    BuildMovementPacket(data);
}

void WorldObject::SendMessageToSetInRange(WorldPacket* data, float dist, bool /*bToSelf*/)
{
    Oregon::MessageDistDeliverer notifier(this, data, dist);
    VisitNearbyWorldObject(dist, notifier);
}

void WorldObject::SendObjectDeSpawnAnim(uint64 guid)
{
    WorldPacket data(SMSG_GAMEOBJECT_DESPAWN_ANIM, 8);
    data << uint64(guid);
    SendMessageToSet(&data, true);
}

void WorldObject::SendObjectCustomAnim(uint64 guid, uint32 anim)
{
    WorldPacket data(SMSG_GAMEOBJECT_CUSTOM_ANIM, 8 + 4);
    data << uint64(guid);
    data << uint32(anim);
    SendMessageToSet(&data, true);
}

void WorldObject::SetMap(Map* map)
{
    ASSERT(map);
    ASSERT(!IsInWorld() || GetTypeId() == TYPEID_CORPSE);
    if (m_currMap == map) // command add npc: first create, than loadfromdb
        return;
    if (m_currMap)
    {
        sLog.outError("Crash alert! WorldObject::SetMap: obj %u new map %u %u, old map %u %u", (uint32)GetTypeId(), map->GetId(), map->GetInstanceId(), m_currMap->GetId(), m_currMap->GetInstanceId());
        ASSERT(false);
    }
    m_currMap = map;
    m_mapId = map->GetId();
    m_InstanceId = map->GetInstanceId();
    if (IsWorldObject())
        m_currMap->AddWorldObject(this);
}

void WorldObject::ResetMap()
{
    ASSERT(m_currMap);
    ASSERT(!IsInWorld());
    if (IsWorldObject())
        m_currMap->RemoveWorldObject(this);
    m_currMap = NULL;
    //maybe not for corpse
    //m_mapId = 0;
    //m_InstanceId = 0;
}

Map const* WorldObject::GetBaseMap() const
{
    ASSERT(m_currMap);
    return m_currMap->GetParent();
}

void WorldObject::AddObjectToRemoveList()
{
    ASSERT(m_uint32Values);

    Map* map = FindMap();
    if (!map)
    {
        sLog.outError("Object (TypeId: %u Entry: %u GUID: %u) at attempt add to move list has invalid map (Id: %u).", GetTypeId(), GetEntry(), GetGUIDLow(), GetMapId());
        return;
    }

    map->AddObjectToRemoveList(this);
}

TempSummon* Map::SummonCreature(uint32 entry, const Position& pos, SummonPropertiesEntry const* properties, uint32 duration, Unit* summoner, uint32 spellId, TempSummonType spwType)
{
    uint32 mask = UNIT_MASK_SUMMON;
    if (properties)
    {
        switch (properties->Category)
        {
        case SUMMON_CATEGORY_PET:
            mask = UNIT_MASK_GUARDIAN;
            break;
        case SUMMON_CATEGORY_PUPPET:
            mask = UNIT_MASK_PUPPET;
            break;
        case SUMMON_CATEGORY_WILD:
        case SUMMON_CATEGORY_ALLY:
            {
                switch (properties->Type)
                {
                case SUMMON_TYPE_MINION:
                case SUMMON_TYPE_GUARDIAN:
                case SUMMON_TYPE_GUARDIAN2:
                    mask = UNIT_MASK_GUARDIAN;
                    break;
                case SUMMON_TYPE_TOTEM:
                    mask = UNIT_MASK_TOTEM;
                    break;
                case SUMMON_TYPE_MINIPET:
                    mask = UNIT_MASK_MINION;
                    break;
                default:
                    if (properties->Flags & 512)
                        mask = UNIT_MASK_GUARDIAN;
                    break;
                }
                break;
            }
        default:
            return NULL;
        }
    }

    uint32 team = 0;
    if (summoner && summoner->GetTypeId() == TYPEID_PLAYER)
        team = summoner->ToPlayer()->GetTeam();

    TempSummon* summon = NULL;
    switch (mask)
    {
    case UNIT_MASK_SUMMON:
        summon = new TempSummon(properties, summoner, false);
        break;
    case UNIT_MASK_GUARDIAN:
        summon = new Guardian(properties, summoner, false);
        break;
    case UNIT_MASK_PUPPET:
        summon = new Puppet(properties, summoner);
        break;
    case UNIT_MASK_TOTEM:
        summon = new Totem(properties, summoner);
        break;
    case UNIT_MASK_MINION:
        summon = new Minion(properties, summoner, false);
        break;
    default:
        return NULL;
    }

    if (!summon->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_UNIT), this, entry, team, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation()))
    {
        delete summon;
        return NULL;
    }

    summon->SetUInt32Value(UNIT_CREATED_BY_SPELL, spellId);
    summon->SetHomePosition(pos);
    // CreatureAI::Reset was already called, check if TempSummonType changed, if
    // so, don't touch it or we may mess up the script. (Example Script: Inferno)
    // TEMPSUMMON_MANUL_DESPAWN is the default one so we check against it.
    if (summon->GetTempSummonType() == TEMPSUMMON_MANUAL_DESPAWN)
        summon->SetTempSummonType(spwType);
    summon->InitStats(duration);
    AddToMap(summon->ToCreature());
    summon->InitSummon();

    return summon;
}

TempSummon* WorldObject::SummonCreature(uint32 entry, const Position& pos, TempSummonType spwtype, uint32 duration)
{
    if (Map* map = FindMap())
    {
        if (TempSummon* summon = map->SummonCreature(entry, pos, NULL, duration, isType(TYPEMASK_UNIT) ? (Unit*)this : NULL, NULL, spwtype))
            return summon;
    }

    return NULL;
}

void WorldObject::SetZoneScript()
{
    if (Map* map = FindMap())
    {
        if (map->IsDungeon())
            m_zoneScript = (ZoneScript*)((InstanceMap*)map)->GetInstanceData();
        else if (!map->IsBattlegroundOrArena())
            m_zoneScript = sOutdoorPvPMgr.GetZoneScript(GetZoneId());
    }
}

Pet* Player::SummonPet(uint32 entry, float x, float y, float z, float ang, PetType petType, uint32 duration)
{
    Pet* pet = new Pet(this, petType);
    SetPetStatus(PET_STATUS_CURRENT);

    if (petType == SUMMON_PET && pet->LoadPetFromDB(this, entry))
    {
        // Remove Demonic Sacrifice auras (known pet)
        Unit::AuraList const& auraClassScripts = GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
        for (Unit::AuraList::const_iterator itr = auraClassScripts.begin(); itr != auraClassScripts.end();)
        {
            if ((*itr)->GetModifier()->m_miscvalue == 2228)
            {
                RemoveAurasDueToSpell((*itr)->GetId());
                itr = auraClassScripts.begin();
            }
            else
                ++itr;
        }

        if (duration > 0)
            pet->SetDuration(duration);

        return NULL;
    }

    // petentry == 0 for hunter "call pet" (current pet summoned if any)
    if (!entry)
    {
        delete pet;
        return NULL;
    }

    Map* map = GetMap();
    uint32 pet_number = sObjectMgr.GeneratePetNumber();
    if (!pet->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_PET), map, entry, pet_number))
    {
        sLog.outError("no such creature entry %u", entry);
        delete pet;
        return NULL;
    }

    pet->Relocate(x, y, z, ang);
    if (!pet->IsPositionValid())
    {
        sLog.outError("Pet (guidlow %d, entry %d) not summoned. Suggested coordinates isn't valid (X: %f Y: %f)", pet->GetGUIDLow(), pet->GetEntry(), pet->GetPositionX(), pet->GetPositionY());
        delete pet;
        return NULL;
    }

    pet->SetCreatorGUID(GetGUID());
    pet->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, GetFaction());

    // this enables pet details window (Shift+P)
    pet->GetCharmInfo()->SetPetNumber(pet_number, false);

    pet->setPowerType(POWER_MANA);
    pet->SetUInt32Value(UNIT_NPC_FLAGS , 0);
    pet->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
    pet->InitStatsForLevel(getLevel());

    SetMinion(pet, true);

    switch (petType)
    {
    case CLASS_PET:
    case SUMMON_PET:
        pet->SetUInt32Value(UNIT_FIELD_BYTES_0, 2048);
        pet->SetUInt32Value(UNIT_FIELD_PETEXPERIENCE, 0);
        pet->SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, 1000);
        pet->SetHealth(pet->GetMaxHealth());
        pet->SetPower(POWER_MANA, pet->GetMaxPower(POWER_MANA));
        pet->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, time(NULL));
    default:
        break;
    }

    map->AddToMap(pet->ToCreature());

    switch (petType)
    {
    case CLASS_PET:
    case SUMMON_PET:
        pet->InitPetCreateSpells();
        pet->SavePetToDB(PET_SAVE_AS_CURRENT);
        PetSpellInitialize();
    default:
        break;
    }

    if (petType == SUMMON_PET)
    {
        // Remove Demonic Sacrifice auras (known pet)
        Unit::AuraList const& auraClassScripts = GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
        for (Unit::AuraList::const_iterator itr = auraClassScripts.begin(); itr != auraClassScripts.end();)
        {
            if ((*itr)->GetModifier()->m_miscvalue == 2228)
            {
                RemoveAurasDueToSpell((*itr)->GetId());
                itr = auraClassScripts.begin();
            }
            else
                ++itr;
        }
    }

    if (duration > 0)
        pet->SetDuration(duration);

    return pet;
}

GameObject* WorldObject::SummonGameObject(uint32 entry, float x, float y, float z, float ang, float rotation0, float rotation1, float rotation2, float rotation3, uint32 respawnTime)
{
    if (!IsInWorld())
        return NULL;

    GameObjectInfo const* goinfo = sObjectMgr.GetGameObjectInfo(entry);
    if (!goinfo)
    {
        sLog.outErrorDb("Gameobject template %u not found in database!", entry);
        return NULL;
    }
    Map* map = GetMap();
    GameObject* go = new GameObject();
    if (!go->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_GAMEOBJECT), entry, map, x, y, z, ang, rotation0, rotation1, rotation2, rotation3, 100, GO_STATE_READY))
    {
        delete go;
        return NULL;
    }
    go->SetRespawnTime(respawnTime);
    if (GetTypeId() == TYPEID_PLAYER || GetTypeId() == TYPEID_UNIT) //not sure how to handle this
        ((Unit*)this)->AddGameObject(go);
    else
        go->SetSpawnedByDefault(false);
    map->AddToMap(go);

    return go;
}

Creature* WorldObject::SummonTrigger(float x, float y, float z, float ang, uint32 duration, CreatureAI * (*GetAI)(Creature*))
{
    TempSummonType summonType = (duration == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_DESPAWN;
    Creature* summon = SummonCreature(WORLD_TRIGGER, x, y, z, ang, summonType, duration);
    if (!summon)
        return NULL;

    //summon->SetName(GetName());
    if (GetTypeId() == TYPEID_PLAYER || GetTypeId() == TYPEID_UNIT)
    {
        summon->SetFaction(((Unit*)this)->GetFaction());
        summon->SetLevel(((Unit*)this)->getLevel());
    }

    if (GetAI)
        summon->AIM_Initialize(GetAI(summon));
    return summon;
}

/**
* Summons group of creatures. Should be called only by instances of Creature and GameObject classes.
*
* @param group Id of group to summon.
* @param list  List to store pointers to summoned creatures.
*/
void WorldObject::SummonCreatureGroup(uint8 group, std::list<TempSummon*>* list /*= NULL*/)
{
    ASSERT((GetTypeId() == TYPEID_GAMEOBJECT || GetTypeId() == TYPEID_UNIT) && "Only GOs and creatures can summon npc groups!");

    std::vector<TempSummonData> const* data = sObjectMgr.GetSummonGroup(GetEntry(), GetTypeId() == TYPEID_GAMEOBJECT ? SUMMONER_TYPE_GAMEOBJECT : SUMMONER_TYPE_CREATURE, group);
    if (!data)
        return;

    for (std::vector<TempSummonData>::const_iterator itr = data->begin(); itr != data->end(); ++itr)
        if (TempSummon* summon = SummonCreature(itr->entry, itr->pos, itr->type, itr->time))
            if (list)
                list->push_back(summon);
}

Creature* WorldObject::FindNearestCreature(uint32 entry, float range, bool alive)
{
    Creature* creature = NULL;
    Oregon::NearestCreatureEntryWithLiveStateInObjectRangeCheck checker(*this, entry, alive, range);
    Oregon::CreatureLastSearcher<Oregon::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(creature, checker);
    VisitNearbyObject(range, searcher);
    return creature;
}

GameObject* WorldObject::FindNearestGameObject(uint32 entry, float range)
{
    GameObject* go = NULL;
    Oregon::NearestGameObjectEntryInObjectRangeCheck checker(*this, entry, range);
    Oregon::GameObjectLastSearcher<Oregon::NearestGameObjectEntryInObjectRangeCheck> searcher(go, checker);
    VisitNearbyGridObject(range, searcher);
    return go;
}

GameObject* WorldObject::FindNearestGameObjectOfType(GameobjectTypes type, float range) const
{
    GameObject* go = NULL;
    Oregon::NearestGameObjectTypeInObjectRangeCheck checker(*this, type, range);
    Oregon::GameObjectLastSearcher<Oregon::NearestGameObjectTypeInObjectRangeCheck> searcher(go, checker);
    VisitNearbyGridObject(range, searcher);
    return go;
}

void WorldObject::GetGameObjectListWithEntryInGrid(std::list<GameObject*>& gameobjectList, uint32 entry, float maxSearchRange) const
{
    CellCoord pair(Oregon::ComputeCellCoord(this->GetPositionX(), this->GetPositionY()));
    Cell cell(pair);
    cell.SetNoCreate();

    Oregon::AllGameObjectsWithEntryInRange check(this, entry, maxSearchRange);
    Oregon::GameObjectListSearcher<Oregon::AllGameObjectsWithEntryInRange> searcher(gameobjectList, check);
    TypeContainerVisitor<Oregon::GameObjectListSearcher<Oregon::AllGameObjectsWithEntryInRange>, GridTypeMapContainer> visitor(searcher);

    cell.Visit(pair, visitor, *(this->GetMap()), *this, maxSearchRange);
}

void WorldObject::GetCreatureListWithEntryInGrid(std::list<Creature*>& creatureList, uint32 entry, float maxSearchRange) const
{
    CellCoord pair(Oregon::ComputeCellCoord(this->GetPositionX(), this->GetPositionY()));
    Cell cell(pair);
    cell.SetNoCreate();

    Oregon::AllCreaturesOfEntryInRange check(this, entry, maxSearchRange);
    Oregon::CreatureListSearcher<Oregon::AllCreaturesOfEntryInRange> searcher(creatureList, check);
    TypeContainerVisitor<Oregon::CreatureListSearcher<Oregon::AllCreaturesOfEntryInRange>, GridTypeMapContainer> visitor(searcher);

    cell.Visit(pair, visitor, *(this->GetMap()), *this, maxSearchRange);
}

void WorldObject::GetNearPoint2D(float& x, float& y, float distance2d, float absAngle) const
{
    x = GetPositionX() + (GetObjectSize() + distance2d) * cos(absAngle);
    y = GetPositionY() + (GetObjectSize() + distance2d) * sin(absAngle);

    Oregon::NormalizeMapCoord(x);
    Oregon::NormalizeMapCoord(y);
}

void WorldObject::GetNearPoint(WorldObject const* searcher, float& x, float& y, float& z, float searcher_size, float distance2d, float absAngle) const
{
    GetNearPoint2D(x, y, distance2d + searcher_size, absAngle);
    z = GetPositionZ();
    if (searcher)
        searcher->UpdateAllowedPositionZ(x, y, z);
    else
        UpdateAllowedPositionZ(x, y, z);

    // return if the point is already in LoS
    if (IsWithinLOS(x, y, z))
        return;

    // remember first point
    float first_x = x;
    float first_y = y;
    float first_z = z;

    // loop in a circle to look for a point in LoS using small steps
    for (float angle = float(M_PI) / 8; angle < float(M_PI) * 2; angle += float(M_PI) / 8)
    {
        GetNearPoint2D(x, y, distance2d + searcher_size, absAngle);
        z = GetPositionZ();
        UpdateAllowedPositionZ(x, y, z);
        if (IsWithinLOS(x, y, z))
            return;
    }

    // still not in LoS, give up and return first position found
    x = first_x;
    y = first_y;
    z = first_z;
}

Position WorldObject::GetNearPosition(float dist, float angle)
{
    Position pos = GetPosition();
    MovePosition(pos, dist, angle);
    return pos;
}

Position WorldObject::GetFirstCollisionPosition(float dist, float angle)
{
    Position pos = GetPosition();
    MovePositionToFirstCollision(pos, dist, angle);
    return pos;
}

Position WorldObject::GetRandomNearPosition(float radius)
{
    Position pos = GetPosition();
    MovePosition(pos, radius * (float)rand_norm(), (float)rand_norm() * static_cast<float>(2 * M_PI));
    return pos;
}

void WorldObject::GetChargeContactPoint(const WorldObject* obj, float& x, float& y, float& z, float distance2d) const
{
    // angle to face `obj` to `this` using distance includes size of `obj`
    GetNearPoint(obj, x, y, z, obj->GetObjectSize(), distance2d, GetAngle(obj));

    if (fabs(this->GetPositionZ() - z) > 3.0f || !IsWithinLOS(x, y, z))
    {
        x = this->GetPositionX();
        y = this->GetPositionY();
        z = this->GetPositionZ();
        obj->UpdateGroundPositionZ(x, y, z);
    }
}

// @todo: replace with WorldObject::UpdateAllowedPositionZ
//   To use WorldObject::UpdateAllowedPositionZ at the moment causes a caster of
//     a leap effect to fall through the ground much too easily.
float WorldObject::GetPositionZTarget(Position& pos, float destx, float desty)
{

    // Added in Bitbucket issue #1105 in order to solve the problem of
    //   leap effects like Blink taking the caster to the bottom of a body of liquid.

    float bottom, ground, floor;

    ground = GetMap()->GetHeight(destx, desty, MAX_HEIGHT, true);
    floor = GetMap()->GetHeight(destx, desty, pos.m_positionZ, true);
    // If we were to ignore liquid, the WorldObject would be placed here.
    bottom = fabs(ground - pos.m_positionZ) <= fabs(floor - pos.m_positionZ) ? ground : floor;

    if(pos.m_positionZ > bottom)
    {

        // We are in the water or in the air.
        // Must be at least above ground.

        LiquidData dest_status;
        GetMap()->getLiquidStatus(destx, desty, pos.m_positionZ, MAP_ALL_LIQUIDS, &dest_status);

        // Source and destination are underwater.
        if(dest_status.level > pos.m_positionZ)
            return pos.m_positionZ;

        // When in doubt, send the user to water/ground level.
        return fabs(dest_status.level - pos.m_positionZ) <= fabs(bottom - pos.m_positionZ) ? dest_status.level : bottom;

    }
    else
    {
        // Destination is on or under the ground. Use ground Z.
        return bottom;
    }
}

void WorldObject::MovePosition(Position& pos, float dist, float angle)
{
    angle += GetOrientation();
    float destx, desty, destz, ground, floor;
    destx = pos.m_positionX + dist * cos(angle);
    desty = pos.m_positionY + dist * sin(angle);

    // Prevent invalid coordinates here, position is unchanged
    if (!Oregon::IsValidMapCoord(destx, desty, pos.m_positionZ))
    {
        sLog.outString("WorldObject::MovePosition invalid coordinates X: %f and Y: %f were passed!", destx, desty);
        return;
    }

    // Prevent invalid coordinates here, position is unchanged
    if (!Oregon::IsValidMapCoord(destx, desty))
    {
        sLog.outError("Crash alert! WorldObject::MovePositionToFirstCollision invalid coordinates X: %f and Y: %f were passed!", destx, desty);
        return;
    }

    ground = GetMap()->GetHeight(destx, desty, MAX_HEIGHT, true);
    floor = GetMap()->GetHeight(destx, desty, pos.m_positionZ, true);
    destz = fabs(ground - pos.m_positionZ) <= fabs(floor - pos.m_positionZ) ? ground : floor;

    float step = dist / 10.0f;

    for (uint8 j = 0; j < 10; ++j)
    {
        // do not allow too big z changes
        if (fabs(pos.m_positionZ - destz) > 6)
        {
            destx -= step * cos(angle);
            desty -= step * sin(angle);
            ground = GetMap()->GetHeight(destx, desty, MAX_HEIGHT, true);
            floor = GetMap()->GetHeight(destx, desty, pos.m_positionZ, true);
            destz = fabs(ground - pos.m_positionZ) <= fabs(floor - pos.m_positionZ) ? ground : floor;
        }
        // we have correct destz now
        else
        {
            pos.Relocate(destx, desty, destz);
            break;
        }
    }

    Oregon::NormalizeMapCoord(pos.m_positionX);
    Oregon::NormalizeMapCoord(pos.m_positionY);
    UpdateGroundPositionZ(pos.m_positionX, pos.m_positionY, pos.m_positionZ);
    pos.SetOrientation(GetOrientation());
}

void WorldObject::MovePositionToFirstCollision(Position& pos, float dist, float angle)
{
    angle += GetOrientation();
    float destx, desty, destz;
    destx = pos.m_positionX + dist * cos(angle);
    desty = pos.m_positionY + dist * sin(angle);

    // Prevent invalid coordinates here, position is unchanged
    if (!Oregon::IsValidMapCoord(destx, desty))
    {
        sLog.outFatal("WorldObject::MovePositionToFirstCollision invalid coordinates X: %f and Y: %f were passed!", destx, desty);
        return;
    }

    destz = GetPositionZTarget(pos, destx, desty);
    bool col = VMAP::VMapFactory::createOrGetVMapManager()->getObjectHitPos(GetMapId(), pos.m_positionX, pos.m_positionY, pos.m_positionZ + 0.5f, destx, desty, destz + 0.5f, destx, desty, destz, -0.5f);

    // collision occured
    if (col)
    {
        // move back a bit
        destx -= CONTACT_DISTANCE * cos(angle);
        desty -= CONTACT_DISTANCE * sin(angle);
        dist = sqrt((pos.m_positionX - destx) * (pos.m_positionX - destx) + (pos.m_positionY - desty) * (pos.m_positionY - desty));
    }

    // check dynamic collision
    col = GetMap()->getObjectHitPos(pos.m_positionX, pos.m_positionY, pos.m_positionZ + 0.5f, destx, desty, destz + 0.5f, destx, desty, destz, -0.5f);
    
    // Collided with a gameobject
    if (col)
    {
        destx -= CONTACT_DISTANCE * cos(angle);
        desty -= CONTACT_DISTANCE * sin(angle);
        dist = sqrt((pos.m_positionX - destx) * (pos.m_positionX - destx) + (pos.m_positionY - desty) * (pos.m_positionY - desty));
    }

    float step = dist / 10.0f;

    for (uint8 j = 0; j < 10; ++j)
    {
        // do not allow too big z changes
        if (fabs(pos.m_positionZ - destz) > 6.0f)
        {
            destx -= step * cos(angle);
            desty -= step * sin(angle);
            destz = GetPositionZTarget(pos, destx, desty);
        }
        // we have correct destz now
        else
        {
            pos.Relocate(destx, desty, destz);
            break;
        }
    }

    Oregon::NormalizeMapCoord(pos.m_positionX);
    Oregon::NormalizeMapCoord(pos.m_positionY);
    pos.m_positionZ = GetPositionZTarget(pos, destx, desty);
    pos.SetOrientation(GetOrientation());
}

void WorldObject::PlayDistanceSound(uint32 sound_id, Player* target /*= NULL*/)
{
    WorldPacket data(SMSG_PLAY_OBJECT_SOUND, 4 + 8);
    data << uint32(sound_id);
    data << uint64(GetGUID());
    if (target)
        target->SendDirectMessage(&data);
    else
        SendMessageToSet(&data, true);
}

void WorldObject::PlayDirectSound(uint32 sound_id, Player* target /*= NULL*/)
{
    WorldPacket data(SMSG_PLAY_SOUND, 4);
    data << uint32(sound_id);
    if (target)
        target->SendDirectMessage(&data);
    else
        SendMessageToSet(&data, true);
}

void WorldObject::DestroyForNearbyPlayers()
{
    if (!IsInWorld())
        return;

    std::list<Player*> targets;
    Oregon::AnyPlayerInObjectRangeCheck check(this, GetVisibilityRange(), false);
    Oregon::PlayerListSearcher<Oregon::AnyPlayerInObjectRangeCheck> searcher(targets, check);
    VisitNearbyWorldObject(GetVisibilityRange(), searcher);
    for (std::list<Player*>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter)
    {
        Player* player = (*iter);

        if (player == this)
            continue;

        if (!player->HaveAtClient(this))
            continue;

        if (isType(TYPEMASK_UNIT) && ((Unit*)this)->GetCharmerGUID() == player->GetGUID()) // @todo this is for puppet
            continue;

        if (GetTypeId() == TYPEID_UNIT)
        {
            // at remove from world (destroy) show kill animation
            DestroyForPlayer(player, ToUnit()->IsDuringRemoveFromWorld() && ToCreature()->isDead());
        }
        else
            DestroyForPlayer(player);

        player->m_clientGUIDs.erase(GetGUID());
    }
}

void WorldObject::UpdateObjectVisibility(bool /*forced*/)
{
    //updates object's visibility for nearby players
    Oregon::VisibleChangesNotifier notifier(*this);
    VisitNearbyWorldObject(GetVisibilityRange(), notifier);
}

struct WorldObjectChangeAccumulator
{
    UpdateDataMapType& i_updateDatas;
    WorldObject& i_object;
    std::set<uint64> plr_list;
    WorldObjectChangeAccumulator(WorldObject& obj, UpdateDataMapType& d) : i_updateDatas(d), i_object(obj) {}
    void Visit(PlayerMapType& m)
    {
        for (PlayerMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        {
            BuildPacket(iter->GetSource());
            if (!iter->GetSource()->GetSharedVisionList().empty())
            {
                SharedVisionList::const_iterator it = iter->GetSource()->GetSharedVisionList().begin();
                for (; it != iter->GetSource()->GetSharedVisionList().end(); ++it)
                    BuildPacket(*it);
            }
        }
    }

    void Visit(CreatureMapType& m)
    {
        for (CreatureMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        {
            if (!iter->GetSource()->GetSharedVisionList().empty())
            {
                SharedVisionList::const_iterator it = iter->GetSource()->GetSharedVisionList().begin();
                for (; it != iter->GetSource()->GetSharedVisionList().end(); ++it)
                    BuildPacket(*it);
            }
        }
    }
    void Visit(DynamicObjectMapType& m)
    {
        for (DynamicObjectMapType::iterator iter = m.begin(); iter != m.end(); ++iter)
        {
            uint64 guid = iter->GetSource()->GetCasterGUID();
            if (IS_PLAYER_GUID(guid))
            {
                //Caster may be NULL if DynObj is in removelist
                if (Player* caster = ObjectAccessor::FindPlayer(guid))
                    if (caster->GetUInt64Value(PLAYER_FARSIGHT) == iter->GetSource()->GetGUID())
                        BuildPacket(caster);
            }
        }
    }
    void BuildPacket(Player* plr)
    {
        // Only send update once to a player
        if (plr_list.find(plr->GetGUID()) == plr_list.end() && plr->HaveAtClient(&i_object))
        {
            i_object.BuildFieldsUpdate(plr, i_updateDatas);
            plr_list.insert(plr->GetGUID());
        }
    }

    template<class SKIP> void Visit(GridRefManager<SKIP>&) {}
};

void WorldObject::BuildUpdate(UpdateDataMapType& data_map)
{
    CellCoord p = Oregon::ComputeCellCoord(GetPositionX(), GetPositionY());
    Cell cell(p);
    cell.SetNoCreate();
    WorldObjectChangeAccumulator notifier(*this, data_map);
    TypeContainerVisitor<WorldObjectChangeAccumulator, WorldTypeMapContainer > player_notifier(notifier);
    Map& map = *GetMap();
    //we must build packets for all visible players
    cell.Visit(p, player_notifier, map, *this, GetVisibilityRange());

    ClearUpdateMask(false);
}

