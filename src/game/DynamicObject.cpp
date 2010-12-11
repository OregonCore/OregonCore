/*
 * Copyright (C) 2005-2008 MaNGOS <http://www.mangosproject.org/>
 *
 * Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010 Oregon <http://www.oregoncore.com/>
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
#include "GameObject.h"
#include "UpdateMask.h"
#include "Opcodes.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "ObjectAccessor.h"
#include "Database/DatabaseEnv.h"
#include "SpellAuras.h"
#include "MapManager.h"
#include "GridNotifiers.h"
#include "CellImpl.h"
#include "GridNotifiersImpl.h"

DynamicObject::DynamicObject() : WorldObject()
{
    m_objectType |= TYPEMASK_DYNAMICOBJECT;
    m_objectTypeId = TYPEID_DYNAMICOBJECT;
                                                            // 2.3.2 - 0x58
    m_updateFlag = (UPDATEFLAG_LOWGUID | UPDATEFLAG_HIGHGUID | UPDATEFLAG_HAS_POSITION);

    m_valuesCount = DYNAMICOBJECT_END;
}

void DynamicObject::AddToWorld()
{
    // Register the dynamicObject for guid lookup
    if (!IsInWorld())
    {
        ObjectAccessor::Instance().AddObject(this);
        WorldObject::AddToWorld();
    }
}

void DynamicObject::RemoveFromWorld()
{
    // Remove the dynamicObject from the accessor
    if (IsInWorld())
    {
        if (m_isWorldObject)
        {
            if (Unit *caster = GetCaster())
            {
                if (caster->GetTypeId() == TYPEID_PLAYER)
                    ((Player*)caster)->SetViewpoint(this, false);
            }
            else
            {
                sLog.outCrash("DynamicObject::RemoveFromWorld cannot find viewpoint owner");
            }
        }
        WorldObject::RemoveFromWorld();
        ObjectAccessor::Instance().RemoveObject(this);
    }
}

bool DynamicObject::Create(uint32 guidlow, Unit *caster, uint32 spellId, uint32 effIndex, const Position &pos, int32 duration, float radius)
{
    SetMap(caster->GetMap());
    Relocate(pos);
    if (!IsPositionValid())
    {
        sLog.outError("DynamicObject (spell %u eff %u) not created. Suggested coordinates isn't valid (X: %f Y: %f)",spellId,effIndex,GetPositionX(),GetPositionY());
        return false;
    }

    WorldObject::_Create(guidlow, HIGHGUID_DYNAMICOBJECT);

    SetEntry(spellId);
    SetFloatValue(OBJECT_FIELD_SCALE_X, 1);
    SetUInt64Value(DYNAMICOBJECT_CASTER, caster->GetGUID());

    // The lower word of DYNAMICOBJECT_BYTES must be 0x0001. This value means that the visual radius will be overriden
    // by client for most of the "ground patch" visual effect spells and a few "skyfall" ones like Hurricane.
    // If any other value is used, the client will _always_ use the radius provided in DYNAMICOBJECT_RADIUS, but
    // precompensation is necessary (eg radius *= 2) for many spells. Anyway, blizz sends 0x0001 for all the spells
    // I saw sniffed...
    SetUInt32Value(DYNAMICOBJECT_BYTES, 0x00000001);
    SetUInt32Value(DYNAMICOBJECT_SPELLID, spellId);
    SetFloatValue(DYNAMICOBJECT_RADIUS, radius);
    SetFloatValue(DYNAMICOBJECT_POS_X, pos.m_positionX);
    SetFloatValue(DYNAMICOBJECT_POS_Y, pos.m_positionY);
    SetFloatValue(DYNAMICOBJECT_POS_Z, pos.m_positionZ);
    SetUInt32Value(DYNAMICOBJECT_CASTTIME, getMSTime());

    m_aliveDuration = duration;
    m_radius = radius;
    m_effIndex = effIndex;
    m_spellId = spellId;
    m_casterGuid = caster->GetGUID();
    m_updateTimer = 0;

    if (m_effIndex == 4)
        m_isWorldObject = true;

    return true;
}

Unit* DynamicObject::GetCaster() const
{
    // can be not found in some cases
    return ObjectAccessor::GetUnit(*this, GetCasterGUID());
}

void DynamicObject::Update(uint32 p_time)
{
    // caster can be not in world at time dynamic object update, but dynamic object not yet deleted in Unit destructor
    Unit* caster = GetCaster();
    if (!caster)
    {
        Delete();
        return;
    }

    bool deleteThis = false;

    if (m_aliveDuration > int32(p_time))
        m_aliveDuration -= p_time;
    else
        deleteThis = true;

    if (m_effIndex < 4)
    {
        if (m_updateTimer < p_time)
        {
            Oregon::DynamicObjectUpdater notifier(*this,caster);
            VisitNearbyObject(GetRadius(), notifier);
            m_updateTimer = 500; // is this official-like?
        }else m_updateTimer -= p_time;
    }

    if (deleteThis)
    {
        caster->RemoveDynObjectWithGUID(GetGUID());
        Delete();
    }
}

void DynamicObject::Delete()
{
    SendObjectDeSpawnAnim(GetGUID());
    RemoveFromWorld();
    AddObjectToRemoveList();
}

void DynamicObject::Delay(int32 delaytime)
{
    m_aliveDuration -= delaytime;
    for (AffectedSet::iterator iunit= m_affected.begin();iunit != m_affected.end();++iunit)
        if (*iunit)
            (*iunit)->DelayAura(m_spellId, m_effIndex, delaytime);
}

bool DynamicObject::isVisibleForInState(Player const* u, bool inVisibleList) const
{
    return IsInWorld() && u->IsInWorld()
        && (IsWithinDistInMap(u->m_seer, World::GetMaxVisibleDistanceForObject() + (inVisibleList ? World::GetVisibleObjectGreyDistance() : 0.0f), false));
}

