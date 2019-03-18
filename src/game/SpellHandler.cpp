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
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "Common.h"
#include "DBCStores.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Log.h"
#include "Opcodes.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "Battleground.h"
#include "MapManager.h"
#include "Totem.h"
#include "ScriptMgr.h"
#include "CreatureAI.h"

void WorldSession::HandleUseItemOpcode(WorldPacket& recvPacket)
{
    Player* pUser = _player;

    // ignore for remote control state
    if (pUser->m_mover != pUser)
        return;

    uint8 bagIndex, slot;
    uint8 spellCount;                                       // number of spells at item, not used
    uint8 castCount;                                        // next cast if exists (single or not)
    uint64 itemGUID;

    recvPacket >> bagIndex >> slot >> spellCount >> castCount >> itemGUID;

    Item* pItem = pUser->GetItemByPos(bagIndex, slot);
    if (!pItem)
    {
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
        return;
    }

    if (pItem->GetGUID() != itemGUID)
    {
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
        return;
    }

    sLog.outDetail("WORLD: CMSG_USE_ITEM packet, bagIndex: %u, slot: %u, spell_count: %u , cast_count: %u, Item: %u, data length = %i", bagIndex, slot, spellCount, castCount, pItem->GetEntry(), recvPacket.size());

    ItemTemplate const* proto = pItem->GetProto();
    if (!proto)
    {
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, pItem, NULL);
        return;
    }

    // some item classes can be used only in equipped state
    if (proto->InventoryType != INVTYPE_NON_EQUIP && !pItem->IsEquipped())
    {
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, pItem, NULL);
        return;
    }

    uint8 msg = pUser->CanUseItem(pItem);
    if (msg != EQUIP_ERR_OK)
    {
        pUser->SendEquipError(msg, pItem, NULL);
        return;
    }

    // only allow conjured consumable, bandage, poisons (all should have the 2^21 item flag set in DB)
    if (proto->Class == ITEM_CLASS_CONSUMABLE && !(proto->Flags & ITEM_PROTO_FLAG_USEABLE_IN_ARENA) && pUser->InArena())
    {
        pUser->SendEquipError(EQUIP_ERR_NOT_DURING_ARENA_MATCH, pItem, NULL);
        return;
    }

    // don't allow items banned in arena
    if (proto->Flags & ITEM_PROTO_FLAG_NOT_USEABLE_IN_ARENA && pUser->InArena())
    {
        pUser->SendEquipError(EQUIP_ERR_NOT_DURING_ARENA_MATCH, pItem, NULL);
        return;
    }

    if (pUser->IsInCombat())
    {
        for (int i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        {
            if (SpellEntry const* spellInfo = sSpellStore.LookupEntry(proto->Spells[i].SpellId))
            {
                if (IsNonCombatSpell(spellInfo))
                {
                    pUser->SendEquipError(EQUIP_ERR_NOT_IN_COMBAT, pItem, NULL);
                    return;
                }
            }
        }
    }

    // check also BIND_WHEN_PICKED_UP and BIND_QUEST_ITEM for .additem or .additemset case by GM (not binded at adding to inventory)
    if (pItem->GetProto()->Bonding == BIND_WHEN_USE || pItem->GetProto()->Bonding == BIND_WHEN_PICKED_UP || pItem->GetProto()->Bonding == BIND_QUEST_ITEM)
    {
        if (!pItem->IsSoulBound())
        {
            pItem->SetState(ITEM_CHANGED, pUser);
            pItem->SetBinding(true);
        }
    }

    SpellCastTargets targets;
    recvPacket >> targets.ReadForCaster(pUser);

    //Note: If script stop casting it must send appropriate data to client to prevent stuck item in gray state.
    if (!sScriptMgr.ItemUse(pUser, pItem, targets))
    {
        // no script or script not process request by self

        // special learning case
        if (pItem->GetProto()->Spells[0].SpellId == SPELL_GENERIC_LEARN)
        {
            uint32 learning_spell_id = pItem->GetProto()->Spells[1].SpellId;

            SpellEntry const* spellInfo = sSpellStore.LookupEntry(SPELL_GENERIC_LEARN);
            if (!spellInfo)
            {
                sLog.outError("Item (Entry: %u) has invalid spell id %u, ignoring ", proto->ItemId, SPELL_GENERIC_LEARN);
                pUser->SendEquipError(EQUIP_ERR_NONE, pItem, NULL);
                return;
            }

            Spell* spell = new Spell(pUser, spellInfo, false);
            spell->m_CastItem = pItem;
            spell->m_cast_count = castCount;               //set count of casts
            spell->m_currentBasePoints[0] = learning_spell_id;
            spell->prepare(&targets);
            return;
        }

        // use triggered flag only for items with many spell casts and for not first cast
        int count = 0;

        for (int i = 0; i < 5; ++i)
        {
            _Spell const& spellData = pItem->GetProto()->Spells[i];

            // no spell
            if (!spellData.SpellId)
                continue;

            // wrong triggering type
            if (spellData.SpellTrigger != ITEM_SPELLTRIGGER_ON_USE && spellData.SpellTrigger != ITEM_SPELLTRIGGER_ON_NO_DELAY_USE)
                continue;

            SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellData.SpellId);
            if (!spellInfo)
            {
                sLog.outError("Item (Entry: %u) has invalid spell id %u, ignoring ", proto->ItemId, spellData.SpellId);
                continue;
            }

            Spell* spell = new Spell(pUser, spellInfo, (count > 0));
            spell->m_CastItem = pItem;
            spell->m_cast_count = castCount;               //set count of casts
            spell->prepare(&targets);

            ++count;
        }
    }
}

void WorldSession::HandleOpenItemOpcode(WorldPacket& recvPacket)
{
    sLog.outDetail("WORLD: CMSG_OPEN_ITEM packet, data length = %i", recvPacket.size());

    Player* pUser = _player;
    uint8 bagIndex, slot;

    recvPacket >> bagIndex >> slot;

    sLog.outDetail("bagIndex: %u, slot: %u", bagIndex, slot);

    Item* pItem = pUser->GetItemByPos(bagIndex, slot);
    if (!pItem)
    {
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
        return;
    }

    ItemTemplate const* proto = pItem->GetProto();
    if (!proto)
    {
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, pItem, NULL);
        return;
    }

    // locked item
    uint32 lockId = proto->LockID;
    if (lockId)
    {
        LockEntry const* lockInfo = sLockStore.LookupEntry(lockId);

        if (!lockInfo)
        {
            pUser->SendEquipError(EQUIP_ERR_ITEM_LOCKED, pItem, NULL);
            sLog.outError("WORLD::OpenItem: item [guid = %u] has an unknown lockId: %u!", pItem->GetGUIDLow(), lockId);
            return;
        }

        // was not unlocked yet
        if (pItem->IsLocked())
        {
            pUser->SendEquipError(EQUIP_ERR_ITEM_LOCKED, pItem, NULL);
            return;
        }
    }

    if (pItem->HasFlag(ITEM_FIELD_FLAGS, ITEM_FLAG_WRAPPED))// wrapped?
    {
        QueryResult_AutoPtr result = CharacterDatabase.PQuery("SELECT entry, flags FROM character_gifts WHERE item_guid = '%u'", pItem->GetGUIDLow());
        if (result)
        {
            Field* fields = result->Fetch();
            uint32 entry = fields[0].GetUInt32();
            uint32 flags = fields[1].GetUInt32();

            pItem->SetUInt64Value(ITEM_FIELD_GIFTCREATOR, 0);
            pItem->SetEntry(entry);
            pItem->SetUInt32Value(ITEM_FIELD_FLAGS, flags);
            pItem->SetState(ITEM_CHANGED, pUser);
        }
        else
        {
            sLog.outError("Wrapped item %u does not have record in character_gifts table and will be deleted", pItem->GetGUIDLow());
            pUser->DestroyItem(pItem->GetBagSlot(), pItem->GetSlot(), true);
            return;
        }
        CharacterDatabase.PExecute("DELETE FROM character_gifts WHERE item_guid = '%u'", pItem->GetGUIDLow());
    }
    else
        pUser->SendLoot(pItem->GetGUID(), LOOT_CORPSE);
}

void WorldSession::HandleGameObjectUseOpcode(WorldPacket& recv_data)
{
    uint64 guid;

    recv_data >> guid;

    DEBUG_LOG("WORLD: Recvd CMSG_GAMEOBJ_USE Message [guid=%u]", GUID_LOPART(guid));

    GameObject* obj = GetPlayer()->GetMap()->GetGameObject(guid);
    if (!obj)
        return;

    FactionTemplateEntry const* faction = sFactionTemplateStore.LookupEntry(obj->GetGOInfo()->faction);
    if (faction &&
        !_player->IsGameMaster() &&
        faction->IsHostileTo(*sFactionTemplateStore.LookupEntry(_player->GetFaction())) &&
        !faction->IsNeutralToAll() &&
        !obj->GetOwner())
        return;

    obj->Use(_player);
}

void WorldSession::HandleCastSpellOpcode(WorldPacket& recvPacket)
{
    uint32 spellId;
    uint8  cast_count;
    recvPacket >> spellId;
    recvPacket >> cast_count;

    DEBUG_LOG("WORLD: got cast spell packet, spellId - %u, cast_count: %u data length = %i",
              spellId, cast_count, recvPacket.size());

    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);

    if (!spellInfo)
    {
        sLog.outError("WORLD: unknown spell id %u", spellId);
        return;
    }

    // not have spell or spell passive and not casted by client
    if (!_player->HasSpell (spellId) || IsPassiveSpell(spellId))
    {
        //cheater? kick? ban?
        return;
    }

    // can't use our own spells when we're in possession of another unit,
    if (_player->isPossessing())
        return;

    // client provided targets
    SpellCastTargets targets;

    recvPacket >> targets.ReadForCaster(_player);

    Unit* target = targets.getUnitTarget();
    if (target) // auto-selection buff level base at target level (in spellInfo)
    {
        // if rank not found then function return NULL but in explicit cast case original spell can be casted and later failed with appropriate error message
        if (SpellEntry const* actualSpellInfo = sSpellMgr.SelectAuraRankForPlayerLevel(spellInfo, target->getLevel()))
            spellInfo = actualSpellInfo;
    }

    if (spellInfo->AttributesEx2 & SPELL_ATTR2_AUTOREPEAT_FLAG)
    {
        if (_player->GetCurrentSpell(CURRENT_AUTOREPEAT_SPELL) && _player->GetCurrentSpell(CURRENT_AUTOREPEAT_SPELL)->m_spellInfo->Id == spellInfo->Id)
            return;
    }

    // When casting a combat spell the unit has to be flagged as initiating combat
    // No need to check if spell is self-cast because combat spells can only be cast on self with commands
    if (target && !IsNonCombatSpell(spellInfo))
        _player->SetInitiatingCombat(true);

    Spell* spell = new Spell(_player, spellInfo, false);
    spell->m_cast_count = cast_count;                       // set count of casts
    spell->prepare(&targets);
}

void WorldSession::HandleCancelCastOpcode(WorldPacket& recvPacket)
{
    uint32 spellId;
    recvPacket >> spellId;

    if (_player->IsNonMeleeSpellCast(false))
        _player->InterruptNonMeleeSpells(false, spellId, false);
}

void WorldSession::HandleCancelAuraOpcode(WorldPacket& recvPacket)
{
    uint32 spellId;
    recvPacket >> spellId;

    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
        return;

    // not allow remove non positive spells and spells with attr SPELL_ATTR_CANT_CANCEL
    if (spellInfo->Attributes & SPELL_ATTR0_CANT_CANCEL)
        return;

    // channeled spell case (it currently casted then)
    if (IsChanneledSpell(spellInfo))
    {
        if (Spell* spell = _player->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
        {
            if (spell->m_spellInfo->Id == spellId)
                spell->cancel();
        }
        return;
    }

    // non channeled case:
    // don't allow remove non positive spells
    // don't allow cancelling passive auras (some of them are visible)
    if (!IsPositiveSpell(spellId) || IsPassiveSpell(spellId))
        return;

    // non channeled case
    if (!IsPositiveSpell(spellId))
        return;

    _player->RemoveAurasDueToSpellByCancel(spellId);
}

void WorldSession::HandlePetCancelAuraOpcode(WorldPacket& recvPacket)
{
    uint64 guid;
    uint32 spellId;

    recvPacket >> guid;
    recvPacket >> spellId;

    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId);
    if (!spellInfo)
    {
        sLog.outError("WORLD: unknown PET spell id %u", spellId);
        return;
    }

    Creature* pet = ObjectAccessor::GetCreatureOrPet(*_player, guid);

    if (!pet)
    {
        sLog.outError("Pet %u not exist.", uint32(GUID_LOPART(guid)));
        return;
    }

    if (pet != GetPlayer()->GetGuardianPet() && pet != GetPlayer()->GetCharm())
    {
        sLog.outError("HandlePetCancelAura.Pet %u isn't pet of player %s", uint32(GUID_LOPART(guid)), GetPlayer()->GetName());
        return;
    }

    if (!pet->IsAlive())
    {
        pet->SendPetActionFeedback(FEEDBACK_PET_DEAD);
        return;
    }

    pet->RemoveAurasDueToSpell(spellId);

    pet->AddCreatureSpellCooldown(spellId);
}

void WorldSession::HandleCancelGrowthAuraOpcode(WorldPacket& /*recvPacket*/)
{
}

void WorldSession::HandleCancelAutoRepeatSpellOpcode(WorldPacket& /*recvPacket*/)
{
    // may be better send SMSG_CANCEL_AUTO_REPEAT?
    // cancel and prepare for deleting
    _player->InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
}

void WorldSession::HandleCancelChanneling(WorldPacket& recv_data)
{
    uint32 spellId;

    recv_data >> spellId;
    _player->InterruptSpell(CURRENT_CHANNELED_SPELL);
}

void WorldSession::HandleTotemDestroy(WorldPacket& recvPacket)
{
    uint8 slotId;

    recvPacket >> slotId;

    ++slotId;
    if (slotId >= MAX_TOTEM_SLOT)
        return;

    if (!_player->m_SummonSlot[slotId])
        return;

    Creature* totem = GetPlayer()->GetMap()->GetCreature(_player->m_SummonSlot[slotId]);
    // Don't unsummon sentry totem
    if (totem && totem->IsTotem() && totem->GetEntry() != SENTRY_TOTEM_ENTRY)
        ((Totem*)totem)->UnSummon();
}

void WorldSession::HandleSelfResOpcode(WorldPacket& /*recv_data*/)
{
    DEBUG_LOG("WORLD: CMSG_SELF_RES");                  // empty opcode

    if (_player->GetUInt32Value(PLAYER_SELF_RES_SPELL))
    {
        SpellEntry const* spellInfo = sSpellStore.LookupEntry(_player->GetUInt32Value(PLAYER_SELF_RES_SPELL));
        if (spellInfo)
            _player->CastSpell(_player, spellInfo, false, 0);

        _player->SetUInt32Value(PLAYER_SELF_RES_SPELL, 0);
    }
}

void WorldSession::HandleMirrorImageDataRequest(WorldPacket& recvData)
{
    DEBUG_LOG("WORLD: Recvd CMSG_GET_MIRRORIMAGE_DATA Message");

    uint64 guid;
    recvData >> guid;

    // Get unit for which data is needed by client
    Unit* unit = ObjectAccessor::GetObjectInWorld(guid, (Unit*)NULL);
    if (!unit)
        return;

    if (!unit->HasAuraType(SPELL_AURA_CLONE_CASTER))
        return;

    // Get creator of the unit (SPELL_AURA_CLONE_CASTER does not stack)
    Unit* creator = unit->GetAurasByType(SPELL_AURA_CLONE_CASTER).front()->GetCaster();
    if (!creator)
        return;

    WorldPacket data(SMSG_MIRRORIMAGE_DATA, 68);
    data << uint64(guid);
    data << uint32(creator->GetDisplayId());
    data << uint8(creator->getRace());
    data << uint8(creator->getGender());

    if (creator->GetTypeId() == TYPEID_PLAYER)
    {
        Player* player = creator->ToPlayer();
        data << uint8(player->GetByteValue(PLAYER_BYTES, 0));   // skin
        data << uint8(player->GetByteValue(PLAYER_BYTES, 1));   // face
        data << uint8(player->GetByteValue(PLAYER_BYTES, 2));   // hair
        data << uint8(player->GetByteValue(PLAYER_BYTES, 3));   // haircolor
        data << uint8(player->GetByteValue(PLAYER_BYTES_2, 0)); // facialhair
        data << uint32(player->GetGuildId());                   // guildId

        static EquipmentSlots const itemSlots[] =
        {
            EQUIPMENT_SLOT_HEAD,
            EQUIPMENT_SLOT_SHOULDERS,
            EQUIPMENT_SLOT_BODY,
            EQUIPMENT_SLOT_CHEST,
            EQUIPMENT_SLOT_WAIST,
            EQUIPMENT_SLOT_LEGS,
            EQUIPMENT_SLOT_FEET,
            EQUIPMENT_SLOT_WRISTS,
            EQUIPMENT_SLOT_HANDS,
            EQUIPMENT_SLOT_BACK,
            EQUIPMENT_SLOT_TABARD,
            EQUIPMENT_SLOT_END
        };

        // Display items in visible slots
        for (EquipmentSlots const* itr = &itemSlots[0]; *itr != EQUIPMENT_SLOT_END; ++itr)
        {
            if (*itr == EQUIPMENT_SLOT_HEAD && player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_HIDE_HELM))
                data << uint32(0);
            else if (*itr == EQUIPMENT_SLOT_BACK && player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_HIDE_CLOAK))
                data << uint32(0);
            else if (Item const* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, *itr))
                data << uint32(item->GetProto()->DisplayInfoID);
            else
                data << uint32(0);
        }
    }
    else
    {
        // Skip player data for creatures
        data << uint8(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
    }

    SendPacket(&data);
}

void WorldSession::HandleSpellClick(WorldPacket& recvData)
{
    DEBUG_LOG("WORLD: Recvd CMSG_SPELLCLICK Message");

    uint64 guid;
    recvData >> guid;

    // this will get something not in world. crash
    Creature* unit = ObjectAccessor::GetCreatureOrPet(*_player, guid);

    if (!unit)
        return;

    /// @todo Unit::SetCharmedBy: 28782 is not in world but 0 is trying to charm it! -> crash
    if (!unit->IsInWorld())
        return;

    Creature* creature = unit->ToCreature();
    if (creature && creature->IsAIEnabled)
        creature->AI()->OnSpellClick(_player);
}
