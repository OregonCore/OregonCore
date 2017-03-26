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
#include "Item.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "Database/DatabaseEnv.h"
#include "ItemEnchantmentMgr.h"

void AddItemsSetItem(Player* player, Item* item)
{
    ItemTemplate const* proto = item->GetProto();
    uint32 setid = proto->ItemSet;

    ItemSetEntry const* set = sItemSetStore.LookupEntry(setid);

    if (!set)
    {
        sLog.outErrorDb("Item set %u for item (id %u) not found, mods not applied.", setid, proto->ItemId);
        return;
    }

    if (set->required_skill_id && player->GetSkillValue(set->required_skill_id) < set->required_skill_value)
        return;

    ItemSetEffect* eff = NULL;

    for (size_t x = 0; x < player->ItemSetEff.size(); ++x)
    {
        if (player->ItemSetEff[x] && player->ItemSetEff[x]->setid == setid)
        {
            eff = player->ItemSetEff[x];
            break;
        }
    }

    if (!eff)
    {
        eff = new ItemSetEffect;
        memset(eff, 0, sizeof(ItemSetEffect));
        eff->setid = setid;

        size_t x = 0;
        for (; x < player->ItemSetEff.size(); x++)
            if (!player->ItemSetEff[x])
                break;

        if (x < player->ItemSetEff.size())
            player->ItemSetEff[x] = eff;
        else
            player->ItemSetEff.push_back(eff);
    }

    ++eff->item_count;

    for (uint32 x = 0; x < 8; x++)
    {
        if (!set->spells [x])
            continue;
        //not enough for  spell
        if (set->items_to_triggerspell[x] > eff->item_count)
            continue;

        uint32 z = 0;
        for (; z < 8; z++)
            if (eff->spells[z] && eff->spells[z]->Id == set->spells[x])
                break;

        if (z < 8)
            continue;

        //new spell
        for (uint32 y = 0; y < 8; y++)
        {
            if (!eff->spells[y])                             // free slot
            {
                SpellEntry const* spellInfo = sSpellStore.LookupEntry(set->spells[x]);
                if (!spellInfo)
                {
                    sLog.outError("WORLD: unknown spell id %u in items set %u effects", set->spells[x], setid);
                    break;
                }

                // spell casted only if fit form requirement, in other case will casted at form change
                player->ApplyEquipSpell(spellInfo, NULL, true);
                eff->spells[y] = spellInfo;
                break;
            }
        }
    }
}

void RemoveItemsSetItem(Player* player, ItemTemplate const* proto)
{
    uint32 setid = proto->ItemSet;

    ItemSetEntry const* set = sItemSetStore.LookupEntry(setid);

    if (!set)
    {
        sLog.outErrorDb("Item set #%u for item #%u not found, mods not removed.", setid, proto->ItemId);
        return;
    }

    ItemSetEffect* eff = NULL;
    size_t setindex = 0;
    for (; setindex < player->ItemSetEff.size(); setindex++)
    {
        if (player->ItemSetEff[setindex] && player->ItemSetEff[setindex]->setid == setid)
        {
            eff = player->ItemSetEff[setindex];
            break;
        }
    }

    // can be in case now enough skill requirement for set appling but set has been appliend when skill requirement not enough
    if (!eff)
        return;

    --eff->item_count;

    for (uint32 x = 0; x < 8; x++)
    {
        if (!set->spells[x])
            continue;

        // enough for spell
        if (set->items_to_triggerspell[x] <= eff->item_count)
            continue;

        for (uint32 z = 0; z < 8; z++)
        {
            if (eff->spells[z] && eff->spells[z]->Id == set->spells[x])
            {
                // spell can be not active if not fit form requirement
                player->ApplyEquipSpell(eff->spells[z], NULL, false);
                eff->spells[z] = NULL;
                break;
            }
        }
    }

    if (!eff->item_count)                                    //all items of a set were removed
    {
        ASSERT(eff == player->ItemSetEff[setindex]);
        delete eff;
        player->ItemSetEff[setindex] = NULL;
    }
}

bool ItemCanGoIntoBag(ItemTemplate const* pProto, ItemTemplate const* pBagProto)
{
    if (!pProto || !pBagProto)
        return false;

    switch (pBagProto->Class)
    {
    case ITEM_CLASS_CONTAINER:
        switch (pBagProto->SubClass)
        {
        case ITEM_SUBCLASS_CONTAINER:
            return true;
        case ITEM_SUBCLASS_SOUL_CONTAINER:
            if (!(pProto->BagFamily & BAG_FAMILY_MASK_SOUL_SHARDS))
                return false;
            return true;
        case ITEM_SUBCLASS_HERB_CONTAINER:
            if (!(pProto->BagFamily & BAG_FAMILY_MASK_HERBS))
                return false;
            return true;
        case ITEM_SUBCLASS_ENCHANTING_CONTAINER:
            if (!(pProto->BagFamily & BAG_FAMILY_MASK_ENCHANTING_SUPP))
                return false;
            return true;
        case ITEM_SUBCLASS_MINING_CONTAINER:
            if (!(pProto->BagFamily & BAG_FAMILY_MASK_MINING_SUPP))
                return false;
            return true;
        case ITEM_SUBCLASS_ENGINEERING_CONTAINER:
            if (!(pProto->BagFamily & BAG_FAMILY_MASK_ENGINEERING_SUPP))
                return false;
            return true;
        case ITEM_SUBCLASS_GEM_CONTAINER:
            if (!(pProto->BagFamily & BAG_FAMILY_MASK_GEMS))
                return false;
            return true;
        case ITEM_SUBCLASS_LEATHERWORKING_CONTAINER:
            if (!(pProto->BagFamily & BAG_FAMILY_MASK_LEATHERWORKING_SUPP))
                return false;
            return true;
        default:
            return false;
        }
    case ITEM_CLASS_QUIVER:
        switch (pBagProto->SubClass)
        {
        case ITEM_SUBCLASS_QUIVER:
            if (!(pProto->BagFamily & BAG_FAMILY_MASK_ARROWS))
                return false;
            return true;
        case ITEM_SUBCLASS_AMMO_POUCH:
            if (!(pProto->BagFamily & BAG_FAMILY_MASK_BULLETS))
                return false;
            return true;
        default:
            return false;
        }
    }
    return false;
}

Item::Item()
{
    m_objectType |= TYPEMASK_ITEM;
    m_objectTypeId = TYPEID_ITEM;
    // 2.3.2 - 0x18
    m_updateFlag = (UPDATEFLAG_LOWGUID | UPDATEFLAG_HIGHGUID);

    m_valuesCount = ITEM_END;
    m_slot = 0;
    uState = ITEM_NEW;
    uQueuePos = -1;
    m_container = NULL;
    m_lootGenerated = false;
    mb_in_trade = false;
}

bool Item::Create(uint32 guidlow, uint32 itemid, Player const* owner)
{
    Object::_Create(guidlow, 0, HIGHGUID_ITEM);

    SetEntry(itemid);
    SetObjectScale(1.0f);

    SetUInt64Value(ITEM_FIELD_OWNER, owner ? owner->GetGUID() : 0);
    SetUInt64Value(ITEM_FIELD_CONTAINED, owner ? owner->GetGUID() : 0);

    ItemTemplate const* itemProto = sObjectMgr.GetItemTemplate(itemid);
    if (!itemProto)
        return false;

    SetUInt32Value(ITEM_FIELD_STACK_COUNT, 1);
    SetUInt32Value(ITEM_FIELD_MAXDURABILITY, itemProto->MaxDurability);
    SetUInt32Value(ITEM_FIELD_DURABILITY, itemProto->MaxDurability);

    for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        SetSpellCharges(i, itemProto->Spells[i].SpellCharges);

    SetUInt32Value(ITEM_FIELD_DURATION, abs(itemProto->Duration));

    return true;
}

void Item::UpdateDuration(Player* owner, uint32 diff)
{
    if (!GetUInt32Value(ITEM_FIELD_DURATION))
        return;

    sLog.outDebug("Item::UpdateDuration Item (Entry: %u Duration %u Diff %u)", GetEntry(), GetUInt32Value(ITEM_FIELD_DURATION), diff);

    if (GetUInt32Value(ITEM_FIELD_DURATION) <= diff)
    {
        owner->DestroyItem(GetBagSlot(), GetSlot(), true);
        return;
    }

    SetUInt32Value(ITEM_FIELD_DURATION, GetUInt32Value(ITEM_FIELD_DURATION) - diff);
    SetState(ITEM_CHANGED, owner);                          // save new time in database
}

void Item::SaveToDB()
{
    uint32 guid = GetGUIDLow();
    switch (uState)
    {
    case ITEM_NEW:
        {
            std::ostringstream ss;
            ss << "REPLACE INTO item_instance (guid,owner_guid,itemEntry,creatorGuid,giftCreatorGuid,count,duration,charges,flags,enchantments,randomPropertyId,durability,itemTextId) VALUES (";
            ss << guid << ",";
            ss << GUID_LOPART(GetOwnerGUID()) << ",";
            ss << GetEntry() << ",";
            ss << GUID_LOPART(GetUInt64Value(ITEM_FIELD_CREATOR)) << ",";
            ss << GUID_LOPART(GetUInt64Value(ITEM_FIELD_GIFTCREATOR)) << ",";
            ss << GetCount() << ",";
            ss << GetUInt32Value(ITEM_FIELD_DURATION) << ",'";
            for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
                ss << GetSpellCharges(i) << " ";

            ss << "'," << GetUInt32Value(ITEM_FIELD_FLAGS) << ",'";
            for (uint8 i = 0; i < MAX_ENCHANTMENT_SLOT; ++i)
            {
                ss << GetEnchantmentId(EnchantmentSlot(i)) << " ";
                ss << GetEnchantmentDuration(EnchantmentSlot(i)) << " ";
                ss << GetEnchantmentCharges(EnchantmentSlot(i)) << " ";
            }

            ss << "'," << GetItemRandomPropertyId() << ",";
            ss << GetUInt32Value(ITEM_FIELD_DURABILITY) << ",";
            ss << GetUInt32Value(ITEM_FIELD_ITEM_TEXT_ID) << ")";

            CharacterDatabase.Execute(ss.str().c_str());
        }
        break;
    case ITEM_CHANGED:
        {
            std::ostringstream ss;
            ss << "UPDATE item_instance SET owner_guid = " << GUID_LOPART(GetOwnerGUID());
            ss << ", itemEntry = " << GetEntry();
            ss << ", creatorGuid = " << GUID_LOPART(GetUInt64Value(ITEM_FIELD_CREATOR));
            ss << ", giftCreatorGuid = " << GUID_LOPART(GetUInt64Value(ITEM_FIELD_GIFTCREATOR));
            ss << ", count = " << GetCount();
            ss << ", duration = " << GetUInt32Value(ITEM_FIELD_DURATION);
            ss << ", charges = '";
            for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
                ss << GetSpellCharges(i) << " ";

            ss << "', flags = " << GetUInt32Value(ITEM_FIELD_FLAGS);
            ss << ", enchantments = '";
            for (uint8 i = 0; i < MAX_ENCHANTMENT_SLOT; ++i)
            {
                ss << GetEnchantmentId(EnchantmentSlot(i)) << " ";
                ss << GetEnchantmentDuration(EnchantmentSlot(i)) << " ";
                ss << GetEnchantmentCharges(EnchantmentSlot(i)) << " ";
            }

            ss << "', randomPropertyId = " << GetItemRandomPropertyId();
            ss << ", durability = " << GetUInt32Value(ITEM_FIELD_DURABILITY);
            ss << ", itemTextId = " << GetUInt32Value(ITEM_FIELD_ITEM_TEXT_ID);
            ss << " WHERE guid = " << guid;

            CharacterDatabase.Execute(ss.str().c_str());

            if (HasFlag(ITEM_FIELD_FLAGS, ITEM_FLAG_WRAPPED))
                CharacterDatabase.PExecute("UPDATE character_gifts SET guid = '%u' WHERE item_guid = '%u'", GUID_LOPART(GetOwnerGUID()), GetGUIDLow());
        }
        break;
    case ITEM_REMOVED:
        {
            if (GetUInt32Value(ITEM_FIELD_ITEM_TEXT_ID) > 0)
                CharacterDatabase.PExecute("DELETE FROM item_text WHERE id = '%u'", GetUInt32Value(ITEM_FIELD_ITEM_TEXT_ID));
            CharacterDatabase.PExecute("DELETE FROM item_instance WHERE guid = '%u'", guid);
            if (HasFlag(ITEM_FIELD_FLAGS, ITEM_FLAG_WRAPPED))
                CharacterDatabase.PExecute("DELETE FROM character_gifts WHERE item_guid = '%u'", GetGUIDLow());
            delete this;
            return;
        }
    case ITEM_UNCHANGED:
        break;
    }
    SetState(ITEM_UNCHANGED);
}

bool Item::LoadFromDB(uint32 guid, uint64 owner_guid, Field* fields)
{
    //                                          0          1            2                3      4         5        6      7             8                 9           10
    //result = CharacterDatabase.PQuery("SELECT itemEntry, creatorGuid, giftCreatorGuid, count, duration, charges, flags, enchantments, randomPropertyId, durability, itemTextId FROM item_instance WHERE guid = '%u'", guid);

    // create item before any checks for store correct guid
    // and allow use "FSetState(ITEM_REMOVED); SaveToDB();" for deleting item from DB
    Object::_Create(guid, 0, HIGHGUID_ITEM);

    // Set entry, MUST be before proto check
    SetEntry(fields[0].GetUInt32());
    SetObjectScale(1.0f);

    ItemTemplate const* proto = GetProto();
    if (!proto)
        return false;

    // set owner (not if item is only loaded for gbank/auction/mail)
    if (owner_guid)
        SetOwnerGUID(owner_guid);

    bool need_save = false;                                 // need explicit save data at load fixes
    SetUInt64Value(ITEM_FIELD_CREATOR, MAKE_NEW_GUID(fields[1].GetUInt32(), 0, HIGHGUID_PLAYER));
    SetUInt64Value(ITEM_FIELD_GIFTCREATOR, MAKE_NEW_GUID(fields[2].GetUInt32(), 0, HIGHGUID_PLAYER));
    SetCount(fields[3].GetUInt32());

    uint32 duration = fields[4].GetUInt32();
    SetUInt32Value(ITEM_FIELD_DURATION, duration);
    // update duration if need, and remove if not need
    if ((proto->Duration == 0) != (duration == 0))
    {
        SetUInt32Value(ITEM_FIELD_DURATION, abs(proto->Duration)); //why is abs needed here?
        need_save = true;
    }

    Tokens tokens = StrSplit(fields[5].GetCppString(), " ");
    if (tokens.size() == MAX_ITEM_PROTO_SPELLS)
        for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
            SetSpellCharges(i, atoi(tokens[i].c_str()));

    SetUInt32Value(ITEM_FIELD_FLAGS, fields[6].GetUInt32());
    // Remove bind flag for items vs NO_BIND set
    if (IsSoulBound() && proto->Bonding == NO_BIND)
    {
        ApplyModFlag(ITEM_FIELD_FLAGS, ITEM_FLAG_SOULBOUND, false);
        need_save = true;
    }

    _LoadIntoDataField(fields[7].GetString(), ITEM_FIELD_ENCHANTMENT, MAX_ENCHANTMENT_SLOT * MAX_ENCHANTMENT_OFFSET);
    SetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID, fields[8].GetInt16());
    
    // recalculate suffix factor
    if (GetItemRandomPropertyId() < 0)
        UpdateItemSuffixFactor();

    uint32 durability = fields[9].GetUInt16();
    SetUInt32Value(ITEM_FIELD_DURABILITY, durability);
    // update max durability (and durability) if need
    SetUInt32Value(ITEM_FIELD_MAXDURABILITY, proto->MaxDurability);
    if (durability > proto->MaxDurability)
    {
        SetUInt32Value(ITEM_FIELD_DURABILITY, proto->MaxDurability);
        need_save = true;
    }

    SetUInt32Value(ITEM_FIELD_ITEM_TEXT_ID, fields[10].GetUInt32());

    if (need_save)                                           // normal item changed state set not work at loading
    {
        std::ostringstream ss;
        ss << "UPDATE item_instance SET duration = " << GetUInt32Value(ITEM_FIELD_DURATION)
            << ", flags = " << GetUInt32Value(ITEM_FIELD_FLAGS)
            << ", durability = " << GetUInt32Value(ITEM_FIELD_DURABILITY)
            << " WHERE guid = " << guid;

        CharacterDatabase.Execute(ss.str().c_str());
    }

    return true;
}

void Item::DeleteFromDB()
{
    CharacterDatabase.PExecute("DELETE FROM item_instance WHERE guid = '%u'", GetGUIDLow());
}

void Item::DeleteFromInventoryDB()
{
    CharacterDatabase.PExecute("DELETE FROM character_inventory WHERE item = '%u'", GetGUIDLow());
}

ItemTemplate const* Item::GetProto() const
{
    return sObjectMgr.GetItemTemplate(GetEntry());
}

Player* Item::GetOwner()const
{
    return sObjectMgr.GetPlayer(GetOwnerGUID());
}

uint32 Item::GetSkill()
{
    const static uint32 item_weapon_skills[MAX_ITEM_SUBCLASS_WEAPON] =
    {
        SKILL_AXES,     SKILL_2H_AXES,  SKILL_BOWS,          SKILL_GUNS,      SKILL_MACES,
        SKILL_2H_MACES, SKILL_POLEARMS, SKILL_SWORDS,        SKILL_2H_SWORDS, 0,
        SKILL_STAVES,   0,              0,                   SKILL_FIST_WEAPONS,   0,
        SKILL_DAGGERS,  SKILL_THROWN,   SKILL_ASSASSINATION, SKILL_CROSSBOWS, SKILL_WANDS,
        SKILL_FISHING
    };

    const static uint32 item_armor_skills[MAX_ITEM_SUBCLASS_ARMOR] =
    {
        0, SKILL_CLOTH, SKILL_LEATHER, SKILL_MAIL, SKILL_PLATE_MAIL, 0, SKILL_SHIELD, 0, 0, 0
    };

    ItemTemplate const* proto = GetProto();

    switch (proto->Class)
    {
    case ITEM_CLASS_WEAPON:
        if (proto->SubClass >= MAX_ITEM_SUBCLASS_WEAPON)
            return 0;
        else
            return item_weapon_skills[proto->SubClass];

    case ITEM_CLASS_ARMOR:
        if (proto->SubClass >= MAX_ITEM_SUBCLASS_ARMOR)
            return 0;
        else
            return item_armor_skills[proto->SubClass];

    default:
        return 0;
    }
}

uint32 Item::GetSpell()
{
    ItemTemplate const* proto = GetProto();

    switch (proto->Class)
    {
    case ITEM_CLASS_WEAPON:
        switch (proto->SubClass)
        {
        case ITEM_SUBCLASS_WEAPON_AXE:
            return  196;
        case ITEM_SUBCLASS_WEAPON_AXE2:
            return  197;
        case ITEM_SUBCLASS_WEAPON_BOW:
            return  264;
        case ITEM_SUBCLASS_WEAPON_GUN:
            return  266;
        case ITEM_SUBCLASS_WEAPON_MACE:
            return  198;
        case ITEM_SUBCLASS_WEAPON_MACE2:
            return  199;
        case ITEM_SUBCLASS_WEAPON_POLEARM:
            return  200;
        case ITEM_SUBCLASS_WEAPON_SWORD:
            return  201;
        case ITEM_SUBCLASS_WEAPON_SWORD2:
            return  202;
        case ITEM_SUBCLASS_WEAPON_STAFF:
            return  227;
        case ITEM_SUBCLASS_WEAPON_DAGGER:
            return 1180;
        case ITEM_SUBCLASS_WEAPON_THROWN:
            return 2567;
        case ITEM_SUBCLASS_WEAPON_SPEAR:
            return 3386;
        case ITEM_SUBCLASS_WEAPON_CROSSBOW:
            return 5011;
        case ITEM_SUBCLASS_WEAPON_WAND:
            return 5009;
        default:
            return 0;
        }
    case ITEM_CLASS_ARMOR:
        switch (proto->SubClass)
        {
        case ITEM_SUBCLASS_ARMOR_CLOTH:
            return 9078;
        case ITEM_SUBCLASS_ARMOR_LEATHER:
            return 9077;
        case ITEM_SUBCLASS_ARMOR_MAIL:
            return 8737;
        case ITEM_SUBCLASS_ARMOR_PLATE:
            return  750;
        case ITEM_SUBCLASS_ARMOR_SHIELD:
            return 9116;
        default:
            return 0;
        }
    }
    return 0;
}

int32 Item::GenerateItemRandomPropertyId(uint32 item_id)
{
    ItemTemplate const* itemProto = sItemStorage.LookupEntry<ItemTemplate>(item_id);

    if (!itemProto)
        return 0;

    // item must have one from this field values not null if it can have random enchantments
    if ((!itemProto->RandomProperty) && (!itemProto->RandomSuffix))
        return 0;

    // item can have not null only one from field values
    if ((itemProto->RandomProperty) && (itemProto->RandomSuffix))
    {
        sLog.outErrorDb("Item template %u has RandomProperty == %u and RandomSuffix == %u, but must have one from field =0", itemProto->ItemId, itemProto->RandomProperty, itemProto->RandomSuffix);
        return 0;
    }

    // RandomProperty case
    if (itemProto->RandomProperty)
    {
        uint32 randomPropId = GetItemEnchantMod(itemProto->RandomProperty);
        ItemRandomPropertiesEntry const* random_id = sItemRandomPropertiesStore.LookupEntry(randomPropId);
        if (!random_id)
        {
            sLog.outErrorDb("Enchantment id #%u used but it doesn't have a record in 'ItemRandomProperties.dbc'", randomPropId);
            return 0;
        }

        return random_id->ID;
    }
    // RandomSuffix case
    else
    {
        uint32 randomPropId = GetItemEnchantMod(itemProto->RandomSuffix);
        ItemRandomSuffixEntry const* random_id = sItemRandomSuffixStore.LookupEntry(randomPropId);
        if (!random_id)
        {
            sLog.outErrorDb("Enchantment id #%u used but it doesn't have a record in sItemRandomSuffixStore.", randomPropId);
            return 0;
        }

        return -int32(random_id->ID);
    }
}

void Item::SetItemRandomProperties(int32 randomPropId)
{
    if (!randomPropId)
        return;

    if (randomPropId > 0)
    {
        ItemRandomPropertiesEntry const* item_rand = sItemRandomPropertiesStore.LookupEntry(randomPropId);
        if (item_rand)
        {
            if (GetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID) != int32(item_rand->ID))
            {
                SetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID, item_rand->ID);
                SetState(ITEM_CHANGED, GetOwner());
            }
            for (uint32 i = PROP_ENCHANTMENT_SLOT_2; i < PROP_ENCHANTMENT_SLOT_2 + 3; ++i)
                SetEnchantment(EnchantmentSlot(i), item_rand->enchant_id[i - PROP_ENCHANTMENT_SLOT_2], 0, 0);
        }
    }
    else
    {
        ItemRandomSuffixEntry const* item_rand = sItemRandomSuffixStore.LookupEntry(-randomPropId);
        if (item_rand)
        {
            if (GetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID) != -int32(item_rand->ID) ||
                !GetItemSuffixFactor())
            {
                SetInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID, -int32(item_rand->ID));
                UpdateItemSuffixFactor();
                SetState(ITEM_CHANGED, GetOwner());
            }

            for (uint32 i = PROP_ENCHANTMENT_SLOT_0; i < PROP_ENCHANTMENT_SLOT_0 + 3; ++i)
                SetEnchantment(EnchantmentSlot(i), item_rand->enchant_id[i - PROP_ENCHANTMENT_SLOT_0], 0, 0);
        }
    }
}

void Item::UpdateItemSuffixFactor()
{
    uint32 suffixFactor = GenerateEnchSuffixFactor(GetEntry());
    if (GetItemSuffixFactor() == suffixFactor)
        return;
    SetUInt32Value(ITEM_FIELD_PROPERTY_SEED, suffixFactor);
}

void Item::SetState(ItemUpdateState state, Player* forplayer)
{
    if (uState == ITEM_NEW && state == ITEM_REMOVED)
    {
        // pretend the item never existed
        if (forplayer)
            RemoveFromUpdateQueueOf(forplayer);
        delete this;
        return;
    }
    if (state != ITEM_UNCHANGED)
    {
        // new items must stay in new state until saved
        if (uState != ITEM_NEW)
            uState = state;
        if (forplayer)
            AddToUpdateQueueOf(forplayer);
    }
    else
    {
        // unset in queue
        // the item must be removed from the queue manually
        uQueuePos = -1;
        uState = ITEM_UNCHANGED;
    }
}

void Item::AddToUpdateQueueOf(Player* player)
{
    if (IsInUpdateQueue())
        return;

    ASSERT(player != NULL);

    if (player->GetGUID() != GetOwnerGUID())
    {
        sLog.outDebug("Item::AddToUpdateQueueOf - Owner's guid (%u) and player's guid (%u) don't match!", GUID_LOPART(GetOwnerGUID()), player->GetGUIDLow());
        return;
    }

    if (player->m_itemUpdateQueueBlocked)
        return;

    player->m_itemUpdateQueue.push_back(this);
    uQueuePos = player->m_itemUpdateQueue.size() - 1;
}

void Item::RemoveFromUpdateQueueOf(Player* player)
{
    if (!IsInUpdateQueue())
        return;

    ASSERT(player != NULL)

    if (player->GetGUID() != GetOwnerGUID())
    {
        sLog.outDebug("Item::RemoveFromUpdateQueueOf - Owner's guid (%u) and player's guid (%u) don't match!", GUID_LOPART(GetOwnerGUID()), player->GetGUIDLow());
        return;
    }

    if (player->m_itemUpdateQueueBlocked)
        return;

    player->m_itemUpdateQueue[uQueuePos] = NULL;
    uQueuePos = -1;
}

uint8 Item::GetBagSlot() const
{
    return m_container ? m_container->GetSlot() : uint8(INVENTORY_SLOT_BAG_0);
}

bool Item::IsEquipped() const
{
    return !IsInBag() && m_slot < EQUIPMENT_SLOT_END;
}

bool Item::CanBeTraded() const
{
    if (m_lootGenerated)
        return false;

    if (IsSoulBound())
        return false;

    if (IsBag() && (Player::IsBagPos(GetPos()) || !((Bag const*)this)->IsEmpty()))
        return false;

    if (Player* owner = GetOwner())
    {
        if (owner->CanUnequipItem(GetPos(), false) != EQUIP_ERR_OK)
            return false;
        if (owner->GetLootGUID() == GetGUID())
            return false;
    }

    if (IsBoundByEnchant())
        return false;

    return true;
}

bool Item::IsBoundByEnchant() const
{
    // Check all enchants for soulbound
    for (uint32 enchant_slot = PERM_ENCHANTMENT_SLOT; enchant_slot < MAX_ENCHANTMENT_SLOT; ++enchant_slot)
        if (uint32 enchant_id = GetEnchantmentId(EnchantmentSlot(enchant_slot)))
            if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id))
                if (enchantEntry->slot & ENCHANTMENT_CAN_SOULBOUND)
                    return true;
    return false;
}

uint8 Item::CanBeMergedPartlyWith(ItemTemplate const* proto) const
{
    // not allow merge looting currently items
    if (m_lootGenerated)
        return EQUIP_ERR_ALREADY_LOOTED;

    // check item type
    if (GetEntry() != proto->ItemId)
        return EQUIP_ERR_ITEM_CANT_STACK;

    // check free space (full stacks can't be target of merge
    if (GetCount() >= proto->Stackable)
        return EQUIP_ERR_ITEM_CANT_STACK;

    return EQUIP_ERR_OK;
}

bool Item::IsFitToSpellRequirements(SpellEntry const* spellInfo) const
{
    ItemTemplate const* proto = GetProto();

    if (spellInfo->EquippedItemClass != -1)                 // -1 == any item class
    {
        if (spellInfo->EquippedItemClass != int32(proto->Class))
            return false;                                   //  wrong item class

        if (spellInfo->EquippedItemSubClassMask != 0)        // 0 == any subclass
        {
            if ((spellInfo->EquippedItemSubClassMask & (1 << proto->SubClass)) == 0)
                return false;                               // subclass not present in mask
        }
    }

    if (spellInfo->EquippedItemInventoryTypeMask != 0)       // 0 == any inventory type
    {
        if ((spellInfo->EquippedItemInventoryTypeMask  & (1 << proto->InventoryType)) == 0)
            return false;                                   // inventory type not present in mask
    }

    return true;
}

void Item::SetEnchantment(EnchantmentSlot slot, uint32 id, uint32 duration, uint32 charges)
{
    // Better lost small time at check in comparison lost time at item save to DB.
    if ((GetEnchantmentId(slot) == id) && (GetEnchantmentDuration(slot) == duration) && (GetEnchantmentCharges(slot) == charges))
        return;

    SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot * MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_ID_OFFSET, id);
    SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot * MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_DURATION_OFFSET, duration);
    SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot * MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_CHARGES_OFFSET, charges);
    SetState(ITEM_CHANGED, GetOwner());
}

void Item::SetEnchantmentDuration(EnchantmentSlot slot, uint32 duration, Player* owner)
{
    if (GetEnchantmentDuration(slot) == duration)
        return;

    SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot * MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_DURATION_OFFSET, duration);
    SetState(ITEM_CHANGED, owner);
    // Cannot use GetOwner() here, has to be passed as an argument to avoid freeze due to hashtable locking
}

void Item::SetEnchantmentCharges(EnchantmentSlot slot, uint32 charges)
{
    if (GetEnchantmentCharges(slot) == charges)
        return;

    SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot * MAX_ENCHANTMENT_OFFSET + ENCHANTMENT_CHARGES_OFFSET, charges);
    SetState(ITEM_CHANGED, GetOwner());
}

void Item::ClearEnchantment(EnchantmentSlot slot)
{
    if (!GetEnchantmentId(slot))
        return;

    for (uint8 x = 0; x < 3; ++x)
        SetUInt32Value(ITEM_FIELD_ENCHANTMENT + slot * MAX_ENCHANTMENT_OFFSET + x, 0);
    SetState(ITEM_CHANGED, GetOwner());
}

bool Item::GemsFitSockets() const
{
    bool fits = true;
    for (uint32 enchant_slot = SOCK_ENCHANTMENT_SLOT; enchant_slot < SOCK_ENCHANTMENT_SLOT + 3; ++enchant_slot)
    {
        uint8 SocketColor = GetProto()->Socket[enchant_slot - SOCK_ENCHANTMENT_SLOT].Color;

        uint32 enchant_id = GetEnchantmentId(EnchantmentSlot(enchant_slot));
        if (!enchant_id)
        {
            if (SocketColor) fits &= false;
            continue;
        }

        SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
        if (!enchantEntry)
        {
            if (SocketColor) fits &= false;
            continue;
        }

        uint8 GemColor = 0;

        uint32 gemid = enchantEntry->GemID;
        if (gemid)
        {
            ItemTemplate const* gemProto = sItemStorage.LookupEntry<ItemTemplate>(gemid);
            if (gemProto)
            {
                GemPropertiesEntry const* gemProperty = sGemPropertiesStore.LookupEntry(gemProto->GemProperties);
                if (gemProperty)
                    GemColor = gemProperty->color;
            }
        }

        fits &= (GemColor & SocketColor) ? true : false;
    }
    return fits;
}

uint8 Item::GetGemCountWithID(uint32 GemID) const
{
    uint8 count = 0;
    for (uint32 enchant_slot = SOCK_ENCHANTMENT_SLOT; enchant_slot < SOCK_ENCHANTMENT_SLOT + 3; ++enchant_slot)
    {
        uint32 enchant_id = GetEnchantmentId(EnchantmentSlot(enchant_slot));
        if (!enchant_id)
            continue;

        SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
        if (!enchantEntry)
            continue;

        if (GemID == enchantEntry->GemID)
            ++count;
    }
    return count;
}

bool Item::IsLimitedToAnotherMapOrZone(uint32 cur_mapId, uint32 cur_zoneId) const
{
    ItemTemplate const* proto = GetProto();
    return proto && ((proto->Map && proto->Map != cur_mapId) || (proto->Area && proto->Area != cur_zoneId));
}

// Though the client has the information in the item's data field,
// we have to send SMSG_ITEM_TIME_UPDATE to display the remaining
// time.
void Item::SendTimeUpdate(Player* owner)
{
    if (!GetUInt32Value(ITEM_FIELD_DURATION))
        return;

    WorldPacket data(SMSG_ITEM_TIME_UPDATE, (8 + 4));
    data << (uint64)GetGUID();
    data << (uint32)GetUInt32Value(ITEM_FIELD_DURATION);
    owner->GetSession()->SendPacket(&data);
}

Item* Item::CreateItem(uint32 item, uint32 count, Player const* player)
{
    if (count < 1)
        return NULL;                                        //don't create item at zero count

    ItemTemplate const* pProto = sObjectMgr.GetItemTemplate(item);
    if (pProto)
    {
        if (count > pProto->Stackable)
            count = pProto->Stackable;

        ASSERT(count != 0 && "pProto->Stackable == 0 but checked at loading already");

        Item* pItem = NewItemOrBag(pProto);
        if (pItem->Create(sObjectMgr.GenerateLowGuid(HIGHGUID_ITEM), item, player))
        {
            pItem->SetCount(count);
            return pItem;
        }
        else
            delete pItem;
    }
    else
        ASSERT(false);
    return NULL;
}

Item* Item::CloneItem(uint32 count, Player const* player) const
{
    Item* newItem = CreateItem(GetEntry(), count, player);
    if (!newItem)
        return NULL;

    newItem->SetUInt32Value(ITEM_FIELD_CREATOR,      GetUInt32Value(ITEM_FIELD_CREATOR));
    newItem->SetUInt32Value(ITEM_FIELD_GIFTCREATOR,  GetUInt32Value(ITEM_FIELD_GIFTCREATOR));
    newItem->SetUInt32Value(ITEM_FIELD_FLAGS,        GetUInt32Value(ITEM_FIELD_FLAGS));
    newItem->SetUInt32Value(ITEM_FIELD_DURATION,     GetUInt32Value(ITEM_FIELD_DURATION));
    newItem->SetItemRandomProperties(GetItemRandomPropertyId());
    return newItem;
}

bool Item::IsBindedNotWith(Player const* player) const
{
    // not binded item
    if (!IsSoulBound())
        return false;

    // own item
    if (GetOwnerGUID() == player->GetGUID())
        return false;

   return true;
}

void Item::BuildUpdate(UpdateDataMapType& data_map)
{
    if (Player* owner = GetOwner())
        BuildFieldsUpdate(owner, data_map);
    ClearUpdateMask(false);
}

