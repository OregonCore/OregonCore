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

#include "Totem.h"
#include "WorldPacket.h"
#include "MapManager.h"
#include "Group.h"
#include "Player.h"
#include "SpellMgr.h"
#include "SpellAuras.h"

Totem::Totem(SummonPropertiesEntry const* properties, Unit* owner) : Minion(properties, owner, false)
{
    m_unitTypeMask |= UNIT_MASK_TOTEM;
    m_duration = 0;
    m_type = TOTEM_PASSIVE;
}

void Totem::Update(uint32 time)
{
    if (!m_owner->IsAlive() || !IsAlive())
    {
        UnSummon();                                         // remove self
        return;
    }

    if (m_duration <= time)
    {
        UnSummon();                                         // remove self
        return;
    }
    else
        m_duration -= time;

    Creature::Update(time);
}

void Totem::InitStats(uint32 duration)
{
    if (m_owner->GetTypeId() == TYPEID_PLAYER
        && m_Properties->Slot >= SUMMON_SLOT_TOTEM
        && m_Properties->Slot < MAX_TOTEM_SLOT)
    {
        WorldPacket data(SMSG_TOTEM_CREATED, 1 + 8 + 4 + 4);
        data << uint8(m_Properties->Slot - 1);
        data << uint64(GetGUID());
        data << uint32(duration);
        data << uint32(GetUInt32Value(UNIT_CREATED_BY_SPELL));
        m_owner->ToPlayer()->SendDirectMessage(&data);

        CreatureInfo const* cinfo = GetCreatureTemplate();
        if (cinfo)
        {
            uint32 modelid = 0;
            if (m_owner->ToPlayer()->GetTeam() == HORDE)
            {
                if (cinfo->modelid3)
                    modelid = cinfo->modelid3;
                else if (cinfo->modelid4)
                    modelid = cinfo->modelid4;
            }
            else
            {
                if (cinfo->modelid1)
                    modelid = cinfo->modelid1;
                else if (cinfo->modelid2)
                    modelid = cinfo->modelid2;
            }
            if (modelid)
                SetDisplayId(modelid);
            else
                sLog.outErrorDb("Totem::Summon: Missing modelid information for entry %u, team %u, totem will use default values.", GetEntry(), m_owner->ToPlayer()->GetTeam());
         }
    }

    Minion::InitStats(duration);

    // Get spell casted by totem
    SpellEntry const* totemSpell = sSpellStore.LookupEntry(GetSpell());
    if (totemSpell)
    {
        // If spell have cast time -> so its active totem
        if (GetSpellCastTime(totemSpell))
            m_type = TOTEM_ACTIVE;
    }

    if (GetEntry() == SENTRY_TOTEM_ENTRY)
        SetReactState(REACT_AGGRESSIVE);

    m_duration = duration;

    SetLevel(m_owner->getLevel());
}

void Totem::InitSummon()
{
    WorldPacket data(SMSG_GAMEOBJECT_SPAWN_ANIM_OBSOLETE, 8);
    data << GetGUID();
    SendMessageToSet(&data, true);

    if (m_type == TOTEM_PASSIVE && GetSpell())
    {
        CastSpell(this, GetSpell(), true);
    }

    // Some totems can have both instant effect and passive spell
    if (GetSpell(1))
        CastSpell(this, GetSpell(1), true);
}

void Totem::UnSummon()
{
    SendObjectDeSpawnAnim(GetGUID());

    CombatStop();
    RemoveAurasDueToSpell(GetSpell());

    // clear owner's totem slot
    for (int i = SUMMON_SLOT_TOTEM; i < MAX_TOTEM_SLOT; ++i)
    {
        if (m_owner->m_SummonSlot[i] == GetGUID())
        {
            m_owner->m_SummonSlot[i] = 0;
            break;
        }
    }

    m_owner->RemoveAurasDueToSpell(GetSpell());

    //remove aura all party members too
    Group* group = NULL;
    if (m_owner->GetTypeId() == TYPEID_PLAYER)
    {
        // Not only the player can summon the totem (scripted AI)
        group = m_owner->ToPlayer()->GetGroup();
        if (group)
        {
            for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* target = itr->GetSource();
                if (target && target->IsInMap(m_owner->ToPlayer()) && group->SameSubGroup(m_owner->ToPlayer(), target))
                    target->RemoveAurasDueToSpell(GetSpell());
            }
        }
    }

    // any totem unsummon look like as totem kill, req. for proper animation
    if (IsAlive())
        setDeathState(DEAD);

    AddObjectToRemoveList();
}

bool Totem::IsImmuneToSpellEffect(SpellEntry const* spellInfo, uint32 index, bool castOnSelf) const
{
    switch (spellInfo->Effect[index])
    {
    case SPELL_EFFECT_ATTACK_ME:
    // immune to any type of regeneration effects hp/mana etc.
    case SPELL_EFFECT_HEAL:
    case SPELL_EFFECT_HEAL_MAX_HEALTH:
    case SPELL_EFFECT_HEAL_MECHANICAL:
    case SPELL_EFFECT_HEAL_PCT:
    case SPELL_EFFECT_ENERGIZE:
    case SPELL_EFFECT_ENERGIZE_PCT:
        return true;
    default:
        break;
    }

    // @todo possibly all negative auras immuned?
    switch (spellInfo->EffectApplyAuraName[index])
    {
    case SPELL_AURA_PERIODIC_DAMAGE:
    case SPELL_AURA_PERIODIC_LEECH:
    case SPELL_AURA_MOD_FEAR:
    case SPELL_AURA_TRANSFORM:
        return true;
    default:
        break;
    }

    return Creature::IsImmuneToSpellEffect(spellInfo, index, castOnSelf);
}

