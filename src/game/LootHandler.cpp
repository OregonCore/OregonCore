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
#include "WorldPacket.h"
#include "Log.h"
#include "Corpse.h"
#include "GameObject.h"
#include "Player.h"
#include "ObjectAccessor.h"
#include "WorldSession.h"
#include "LootMgr.h"
#include "Object.h"
#include "Group.h"
#include "World.h"
#include "Utilities/Util.h"

void WorldSession::HandleAutostoreLootItemOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("WORLD: CMSG_AUTOSTORE_LOOT_ITEM");
    Player* player = GetPlayer();
    uint64 lguid = player->GetLootGUID();
    Loot* loot = NULL;
    uint8 lootSlot = 0;

    recv_data >> lootSlot;

    if (IS_GAMEOBJECT_GUID(lguid))
    {
        GameObject* go = player->GetMap()->GetGameObject(lguid);

        // not check distance for GO in case owned GO (fishing bobber case, for example) or Fishing hole GO
        if (!go || ((go->GetOwnerGUID() != _player->GetGUID() && go->GetGoType() != GAMEOBJECT_TYPE_FISHINGHOLE) && !go->IsWithinDistInMap(_player, INTERACTION_DISTANCE)))
        {
            player->SendLootRelease(lguid);
            return;
        }

        loot = &go->loot;
    }
    else if (IS_ITEM_GUID(lguid))
    {
        Item* pItem = player->GetItemByGuid(lguid);

        if (!pItem)
        {
            player->SendLootRelease(lguid);
            return;
        }

        loot = &pItem->loot;
    }
    else if (IS_CORPSE_GUID(lguid))
    {
        Corpse* bones = ObjectAccessor::GetCorpse(*player, lguid);
        if (!bones)
        {
            player->SendLootRelease(lguid);
            return;
        }
        loot = &bones->loot;
    }
    else
    {
        Creature* pCreature = GetPlayer()->GetMap()->GetCreature(lguid);

        bool ok_loot = pCreature && (pCreature->IsAlive() == (player->getClass() == CLASS_ROGUE && pCreature->loot.loot_type == LOOT_SKINNING) || (!pCreature->IsAlive() == (pCreature->GetSkinner() && pCreature->loot.loot_type == LOOT_SKINNING)));

        if (!ok_loot || !pCreature->IsWithinDistInMap(_player, INTERACTION_DISTANCE))
        {
            player->SendLootError(lguid, ok_loot ? LOOT_ERROR_TOO_FAR : LOOT_ERROR_DIDNT_KILL);
            return;
        }

        loot = &pCreature->loot;
    }

    QuestItem* qitem = NULL;
    QuestItem* ffaitem = NULL;
    QuestItem* conditem = NULL;

    LootItem* item = loot->LootItemInSlot(lootSlot, player, &qitem, &ffaitem, &conditem);

    if (!item)
    {
        player->SendEquipError(EQUIP_ERR_ALREADY_LOOTED, NULL, NULL);
        return;
    }

    // questitems use the blocked field for other purposes
    if (!qitem && item->is_blocked)
    {
        player->SendLootRelease(lguid);
        return;
    }

    ItemPosCountVec dest;
    uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, item->itemid, item->count);
    if (msg == EQUIP_ERR_OK)
    {
        Item* newitem = player->StoreNewItem(dest, item->itemid, true, item->randomPropertyId);

        if (qitem)
        {
            qitem->is_looted = true;
            //freeforall is 1 if everyone's supposed to get the quest item.
            if (item->freeforall || loot->GetPlayerQuestItems().size() == 1)
                player->SendNotifyLootItemRemoved(lootSlot);
            else
                loot->NotifyQuestItemRemoved(qitem->index);
        }
        else
        {
            if (ffaitem)
            {
                //freeforall case, notify only one player of the removal
                ffaitem->is_looted = true;
                player->SendNotifyLootItemRemoved(lootSlot);
            }
            else
            {
                //not freeforall, notify everyone
                if (conditem)
                    conditem->is_looted = true;
                loot->NotifyItemRemoved(lootSlot);
            }
        }

        //if only one person is supposed to loot the item, then set it to looted
        if (!item->freeforall)
            item->is_looted = true;

        --loot->unlootedCount;

        player->SendNewItem(newitem, uint32(item->count), false, false, true);
    }
    else
        player->SendEquipError(msg, NULL, NULL);
}

void WorldSession::HandleLootMoneyOpcode(WorldPacket& /*recv_data*/)
{
    sLog.outDebug("WORLD: CMSG_LOOT_MONEY");

    Player* player = GetPlayer();
    uint64 guid = player->GetLootGUID();
    if (!guid)
        return;

    Loot* pLoot = NULL;
    bool shareMoney = true;

    switch (GUID_HIPART(guid))
    {
    case HIGHGUID_GAMEOBJECT:
        {
            GameObject* pGameObject = GetPlayer()->GetMap()->GetGameObject(guid);

            // not check distance for GO in case owned GO (fishing bobber case, for example)
            if (pGameObject && ((pGameObject->GetOwnerGUID() == _player->GetGUID() || pGameObject->IsWithinDistInMap(_player, INTERACTION_DISTANCE))))
                pLoot = &pGameObject->loot;

            break;
        }
    case HIGHGUID_CORPSE:                               // remove insignia ONLY in BG
        {
            Corpse* bones = ObjectAccessor::GetCorpse(*GetPlayer(), guid);

            if (bones && bones->IsWithinDistInMap(_player, INTERACTION_DISTANCE))
            {
                pLoot = &bones->loot;
                shareMoney = false;
            }

            break;
        }
    case HIGHGUID_ITEM:
        {
            if (Item* item = GetPlayer()->GetItemByGuid(guid))
            {
                pLoot = &item->loot;
                shareMoney = false;
            }
            break;
        }
    case HIGHGUID_UNIT:
        {
            Creature* creature = GetPlayer()->GetMap()->GetCreature(guid);

            bool ok_loot = creature && creature->IsAlive() == (player->getClass() == CLASS_ROGUE && creature->loot.loot_type == LOOT_SKINNING);

            if (ok_loot && creature->IsWithinDistInMap(_player, INTERACTION_DISTANCE))
            {
                pLoot = &creature->loot;
                if (creature->IsAlive())
                    shareMoney = false;
            }
            else
                player->SendLootError(guid, ok_loot ? LOOT_ERROR_TOO_FAR : LOOT_ERROR_DIDNT_KILL);
            break;
        }
    default:
        return;                                         // unlootable type
    }

    if (pLoot)
    {
        pLoot->NotifyMoneyRemoved();
        if (shareMoney && player->GetGroup())      //item, pickpocket and players can be looted only single player
        {
            Group* group = player->GetGroup();

            std::vector<Player*> playersNear;
            for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* playerGroup = itr->GetSource();
                if (!playerGroup)
                    continue;
                if (player->GetDistance2d(playerGroup) < sWorld.getConfig(CONFIG_GROUP_XP_DISTANCE))
                    playersNear.push_back(playerGroup);
            }

            uint32 money_per_player = uint32((pLoot->gold) / (playersNear.size()));

            for (std::vector<Player*>::const_iterator i = playersNear.begin(); i != playersNear.end(); ++i)
            {
                (*i)->ModifyMoney(money_per_player);

                WorldPacket data(SMSG_LOOT_MONEY_NOTIFY, 4);
                data << uint32(money_per_player);

                (*i)->GetSession()->SendPacket(&data);
            }
        }
        else
            player->ModifyMoney(pLoot->gold);

        pLoot->gold = 0;
    }
}

void WorldSession::HandleLootOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("WORLD: CMSG_LOOT");

    uint64 guid;
    recv_data >> guid;

    // Check possible cheat
    if (!GetPlayer()->IsAlive())
        return;

    GetPlayer()->SendLoot(guid, LOOT_CORPSE);

    // Interrupt spell casting on loot
    if (GetPlayer()->IsNonMeleeSpellCast(false))
        GetPlayer()->InterruptNonMeleeSpells(false);
}

void WorldSession::HandleLootReleaseOpcode(WorldPacket& recv_data)
{
    sLog.outDebug("WORLD: CMSG_LOOT_RELEASE");

    // cheaters can modify lguid to prevent correct apply loot release code and re-loot
    // use internal stored guid
    uint64 guid;
    recv_data >> guid;

    if (uint64 lguid = GetPlayer()->GetLootGUID())
        if (lguid == guid)
            DoLootRelease(lguid);
}

void WorldSession::DoLootRelease(uint64 lguid)
{
    Player*  player = GetPlayer();
    Loot*    loot;

    player->SetLootGUID(0);
    player->SendLootRelease(lguid);

    player->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_LOOTING);

    if (!player->IsInWorld())
        return;

    if (IS_GAMEOBJECT_GUID(lguid))
    {
        GameObject* go = GetPlayer()->GetMap()->GetGameObject(lguid);

        // not check distance for GO in case owned GO (fishing bobber case, for example) or Fishing hole GO
        if (!go || ((go->GetOwnerGUID() != _player->GetGUID() && go->GetGoType() != GAMEOBJECT_TYPE_FISHINGHOLE) && !go->IsWithinDistInMap(_player, INTERACTION_DISTANCE)))
            return;

        loot = &go->loot;

        if (go->GetGoType() == GAMEOBJECT_TYPE_DOOR)
        {
            // locked doors are opened with spelleffect openlock, prevent remove its as looted
            go->UseDoorOrButton();
        }
        else if (loot->isLooted() || go->GetGoType() == GAMEOBJECT_TYPE_FISHINGNODE)
        {
            // GO is mineral vein? so it is not removed after its looted
            if (go->GetGoType() == GAMEOBJECT_TYPE_CHEST)
            {
                uint32 go_min = go->GetGOInfo()->chest.minSuccessOpens;
                uint32 go_max = go->GetGOInfo()->chest.maxSuccessOpens;

                // only vein pass this check
                if (go_min != 0 && go_max > go_min)
                {
                    float amount_rate = sWorld.getRate(RATE_MINING_AMOUNT);
                    float min_amount = go_min * amount_rate;
                    float max_amount = go_max * amount_rate;

                    go->AddUse();
                    float uses = float(go->GetUseCount());

                    if (uses < max_amount)
                    {
                        if (uses >= min_amount)
                        {
                            float chance_rate = sWorld.getRate(RATE_MINING_NEXT);

                            int32 ReqValue = 175;
                            LockEntry const* lockInfo = sLockStore.LookupEntry(go->GetGOInfo()->chest.lockId);
                            if (lockInfo)
                                ReqValue = lockInfo->requiredminingskill;
                            float skill = float(player->GetSkillValue(SKILL_MINING)) / (ReqValue + 25);
                            double chance = pow(0.8 * chance_rate, 4 * (1 / double(max_amount)) * double(uses));
                            if (roll_chance_f(100 * chance + skill))
                                go->SetLootState(GO_READY);
                            else                            // not have more uses
                                go->SetLootState(GO_JUST_DEACTIVATED);
                        }
                        else                                // 100% chance until min uses
                            go->SetLootState(GO_READY);
                    }
                    else                                    // max uses already
                        go->SetLootState(GO_JUST_DEACTIVATED);
                }
                else                                        // not vein
                    go->SetLootState(GO_JUST_DEACTIVATED);
            }
            else if (go->GetGoType() == GAMEOBJECT_TYPE_FISHINGHOLE)
            {
                // The fishing hole used once more
                go->AddUse();                               // if the max usage is reached, will be despawned in next tick
                if (int32(go->GetUseCount()) >= irand(go->GetGOInfo()->fishinghole.minSuccessOpens, go->GetGOInfo()->fishinghole.maxSuccessOpens))
                    go->SetLootState(GO_JUST_DEACTIVATED);
                else
                    go->SetLootState(GO_READY);
            }
            else // not chest (or vein/herb/etc)
                go->SetLootState(GO_JUST_DEACTIVATED);

            loot->clear();
        }
        else
        {
            // not fully looted object
            go->SetLootState(GO_ACTIVATED, player);

            if (player->GetGUID() == loot->roundRobinPlayer)
                loot->roundRobinPlayer = 0;
        }
    }
    else if (IS_CORPSE_GUID(lguid))        // ONLY remove insignia at BG
    {
        Corpse* corpse = ObjectAccessor::GetCorpse(*player, lguid);
        if (!corpse || !corpse->IsWithinDistInMap(_player, INTERACTION_DISTANCE))
            return;

        loot = &corpse->loot;

        if (loot->isLooted())
        {
            loot->clear();
            corpse->RemoveFlag(CORPSE_FIELD_DYNAMIC_FLAGS, CORPSE_DYNFLAG_LOOTABLE);
        }
    }
    else if (IS_ITEM_GUID(lguid))
    {
        Item* pItem = player->GetItemByGuid(lguid);
        if (!pItem)
            return;

        if (pItem->GetProto()->Flags & ITEM_PROTO_FLAG_PROSPECTABLE)
        {
            pItem->m_lootGenerated = false;
            pItem->loot.clear();

            uint32 count = pItem->GetCount();

            // >=5 checked in spell code, but will work for cheating cases also with removing from another stacks.
            if (count > 5)
                count = 5;

            player->DestroyItemCount(pItem, count, true);
        }
        else
            // FIXME: item must not be deleted in case not fully looted state. But this pre-request implement loot saving in DB at item save. Or cheating possible.
            player->DestroyItem(pItem->GetBagSlot(), pItem->GetSlot(), true);
        return;                                             // item can be looted only single player
    }
    else
    {
        Creature* pCreature = GetPlayer()->GetMap()->GetCreature(lguid);

        bool ok_loot = pCreature && pCreature->IsAlive() == (player->getClass() == CLASS_ROGUE && pCreature->loot.loot_type == LOOT_PICKPOCKETING);
        if (!ok_loot || !pCreature->IsWithinDistInMap(_player, INTERACTION_DISTANCE))
            return;

        loot = &pCreature->loot;

        if (loot->isLooted())
        {
            pCreature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

            // skip pickpocketing loot for speed, skinning timer reduction is no-op in fact
            if (!pCreature->IsAlive())
                pCreature->AllLootRemovedFromCorpse();

            loot->clear();
        }
        else
        {
            // if the round robin player release, reset it.
            if (player->GetGUID() == loot->roundRobinPlayer)
            {
                loot->roundRobinPlayer = 0;

                if (Group* group = player->GetGroup())
                    group->SendLooter(pCreature, NULL);
            }
            // force update of dynamic flags, otherwise other group's players still not able to loot.
            pCreature->ForceValuesUpdateAtIndex(UNIT_DYNAMIC_FLAGS);
        }
    }

    //Player is not looking at loot list, he doesn't need to see updates on the loot list
    loot->RemoveLooter(player->GetGUID());
}

void WorldSession::HandleLootMasterGiveOpcode(WorldPacket& recv_data)
{
    uint8 slotid;
    uint64 lootguid, target_playerguid;

    recv_data >> lootguid >> slotid >> target_playerguid;

    if (!_player->GetGroup() || _player->GetGroup()->GetMasterLooterGuid() != _player->GetGUID() || _player->GetGroup()->GetLootMethod() != MASTER_LOOT)
    {
        _player->SendLootError(lootguid, LOOT_ERROR_DIDNT_KILL);
        return;
    }

    Player* target = ObjectAccessor::FindPlayer(MAKE_NEW_GUID(target_playerguid, 0, HIGHGUID_PLAYER));
    if (!target)
    {
        _player->SendLootError(lootguid, LOOT_ERROR_PLAYER_NOT_FOUND);
        return;
    }

    // TODO : add some error message?
    if (_player->GetMapId() != target->GetMapId() || _player->GetDistance(target) > sWorld.getConfig(CONFIG_GROUP_XP_DISTANCE))
        return;

    sLog.outDebug("WorldSession::HandleLootMasterGiveOpcode (CMSG_LOOT_MASTER_GIVE, 0x02A3) Target = [%s].", target->GetName());

    if (_player->GetLootGUID() != lootguid)
    {
        _player->SendLootError(lootguid, LOOT_ERROR_DIDNT_KILL);
        return;
    }

    Loot* pLoot = NULL;

    if (IS_CREATURE_GUID(GetPlayer()->GetLootGUID()))
    {
        Creature* pCreature = GetPlayer()->GetMap()->GetCreature(lootguid);
        if (!pCreature)
            return;

        pLoot = &pCreature->loot;
    }
    else if (IS_GAMEOBJECT_GUID(GetPlayer()->GetLootGUID()))
    {
        GameObject* pGO = GetPlayer()->GetMap()->GetGameObject(lootguid);
        if (!pGO)
            return;

        pLoot = &pGO->loot;
    }

    if (!pLoot)
        return;

    if (slotid > pLoot->items.size())
    {
        sLog.outDebug("AutoLootItem: Player %s might be using a hack! (slot %d, size %lu)", GetPlayer()->GetName(), slotid, (unsigned long)pLoot->items.size());
        return;
    }

    LootItem& item = pLoot->items[slotid];

    ItemPosCountVec dest;
    uint8 msg = target->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, item.itemid, item.count);
    if (msg != EQUIP_ERR_OK)
    {
        if (msg == EQUIP_ERR_CANT_CARRY_MORE_OF_THIS)
            _player->SendLootError(lootguid, LOOT_ERROR_MASTER_UNIQUE_ITEM);
        else if (msg == EQUIP_ERR_INVENTORY_FULL)
            _player->SendLootError(lootguid, LOOT_ERROR_MASTER_INV_FULL);
        else
            _player->SendLootError(lootguid, LOOT_ERROR_MASTER_OTHER);

        target->SendEquipError(msg, NULL, NULL);
        return;
    }

    // now move item from loot to target inventory
    Item* newitem = target->StoreNewItem(dest, item.itemid, true, item.randomPropertyId);
    target->SendNewItem(newitem, uint32(item.count), false, false, true);

    // mark as looted
    item.count = 0;
    item.is_looted = true;

    pLoot->NotifyItemRemoved(slotid);
    --pLoot->unlootedCount;
}

