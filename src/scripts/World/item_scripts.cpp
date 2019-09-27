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

/* ScriptData
SDName: Item_Scripts
SD%Complete: 100
SDComment: Items for a range of different items. See content below (in script)
SDCategory: Items
EndScriptData */

/* ContentData
item_attuned_crystal_cores(i34368)  Prevent abuse(quest 11524 & 11525)
item_draenei_fishing_net(i23654)    Hacklike implements chance to spawn item or creature
item_disciplinary_rod               Prevents abuse
item_nether_wraith_beacon(i31742)   Summons creatures for quest Becoming a Spellfire Tailor (q10832)
item_flying_machine(i34060,i34061)  Engineering crafted flying machines
item_gor_dreks_ointment(i30175)     Protecting Our Own(q10488)
item_muiseks_vessel                 Cast on creature, they must be dead(q 3123,3124,3125,3126,3127)
item_only_for_flight                Items which should only useable while flying
item_razorthorn_flayer_gland        Quest Discovering Your Roots (q11520) and Rediscovering Your Roots (q11521). Prevents abuse
item_tame_beast_rods(many)          Prevent cast on any other creature than the intended (for all tame beast quests)
item_soul_cannon(i32825)            Prevents abuse of this item
item_sparrowhawk_net(i32321)        Quest To Catch A Sparrowhawk (q10987). Prevents abuse
item_voodoo_charm                   Provide proper error message and target(q2561)
item_yehkinyas_bramble(i10699)      Allow cast spell on vale screecher only and remove corpse if cast sucessful (q3520)
item_inoculating_crystal            Quest Inoculating. Prevent abuse
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "SpellMgr.h"
#include "Spell.h"
#include "WorldPacket.h"

/*#####
# item_only_for_flight
#####*/

bool ItemUse_item_only_for_flight(Player* pPlayer, Item* pItem, SpellCastTargets const& /*targets*/)
{
    uint32 itemId = pItem->GetEntry();
    bool disabled = false;

    //for special scripts
    switch (itemId)
    {
    case 24538:
        if (pPlayer->GetAreaId() != 3628)
            disabled = true;
        break;
    case 34489:
        if (pPlayer->GetZoneId() != 4080)
            disabled = true;
        break;
    }

    // allow use in flight only
    if (pPlayer->IsInFlight() && !disabled)
        return false;

    // error
    pPlayer->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, pItem, NULL);
    return true;
}

/*#####
# item_attuned_crystal_cores
#####*/

enum CrystalCores
{
    ERRATIC_BEHAVIOUR   = 11524,
    FURTHER_CONVERSIONS = 11525,
    ERRATIC_SENTRY      = 24972
};

bool ItemUse_item_attuned_crystal_cores(Player* player, Item* _Item, SpellCastTargets const& targets)
{
    if (targets.getUnitTarget() && targets.getUnitTarget()->GetTypeId() == TYPEID_UNIT &&
        targets.getUnitTarget()->GetEntry() == ERRATIC_SENTRY && targets.getUnitTarget()->isDead() &&
        (player->GetQuestStatus(ERRATIC_BEHAVIOUR) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(FURTHER_CONVERSIONS) == QUEST_STATUS_INCOMPLETE))
        return false;

    player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, _Item, NULL);
    return true;
}

/*#####
# item_draenei_fishing_net
#####*/

//This is just a hack and should be removed from here.
//Creature/Item are in fact created before spell are sucessfully casted, without any checks at all to ensure proper/expected behavior.
bool ItemUse_item_draenei_fishing_net(Player* pPlayer, Item* /*pItem*/, SpellCastTargets const& /*targets*/)
{
    //if (targets.getGOTarget() && targets.getGOTarget()->GetTypeId() == TYPEID_GAMEOBJECT &&
    //targets.getGOTarget()->GetGOInfo()->type == GAMEOBJECT_TYPE_SPELL_FOCUS && targets.getGOTarget()->GetEntry() == 181616)
    //{
    if (pPlayer->GetQuestStatus(9452) == QUEST_STATUS_INCOMPLETE)
    {
        if (urand(0, 99) < 35)
        {
            Creature* Murloc = pPlayer->SummonCreature(17102, pPlayer->GetPositionX(), pPlayer->GetPositionY() + 20, pPlayer->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
            if (Murloc)
                Murloc->AI()->AttackStart(pPlayer);
        }
        else
        {
            ItemPosCountVec dest;
            uint8 msg = pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 23614, 1);
            if (msg == EQUIP_ERR_OK)
            {
                if (Item* item = pPlayer->StoreNewItem(dest, 23614, true))
                    pPlayer->SendNewItem(item, 1, false, true);
            }
            else
                pPlayer->SendEquipError(msg, NULL, NULL);
        }
    }
    //}
    return false;
}

/*#####
# item_disciplinary_rod
#####*/

bool ItemUse_item_disciplinary_rod(Player* player, Item* _Item, SpellCastTargets const& targets)
{
    if (targets.getUnitTarget() && targets.getUnitTarget()->GetTypeId() == TYPEID_UNIT &&
        (targets.getUnitTarget()->GetEntry() == 15941 || targets.getUnitTarget()->GetEntry() == 15945))
        return false;

    player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, _Item, NULL);
    return true;
}

/*#####
# item_gor_dreks_ointment
#####*/

bool ItemUse_item_gor_dreks_ointment(Player* pPlayer, Item* pItem, SpellCastTargets const& targets)
{
    if (targets.getUnitTarget() && targets.getUnitTarget()->GetTypeId() == TYPEID_UNIT &&
        targets.getUnitTarget()->GetEntry() == 20748 && !targets.getUnitTarget()->HasAura(32578, 0))
        return false;

    pPlayer->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, pItem, NULL);
    return true;
}

/*#####
# item_muiseks_vessel
#####*/

bool ItemUse_item_muiseks_vessel(Player* player, Item* _Item, SpellCastTargets const& targets)
{
    Unit* uTarget = targets.getUnitTarget();
    uint32 itemSpell = _Item->GetProto()->Spells[0].SpellId;
    uint32 cEntry = 0;
    uint32 cEntry2 = 0;
    uint32 cEntry3 = 0;
    uint32 cEntry4 = 0;

    if (itemSpell)
    {
        switch (itemSpell)
        {
        case 11885:                                     //Wandering Forest Walker
            cEntry =  7584;
            break;
        case 11886:                                     //Owlbeasts
            cEntry =  2927;
            cEntry2 = 2928;
            cEntry3 = 2929;
            cEntry4 = 7808;
            break;
        case 11887:                                     //Freyfeather Hippogryphs
            cEntry =  5300;
            cEntry2 = 5304;
            cEntry3 = 5305;
            cEntry4 = 5306;
            break;
        case 11888:                                     //Sprite Dragon Sprite Darters
            cEntry =  5276;
            cEntry2 = 5278;
            break;
        case 11889:                                     //Zapped Land Walker Land Walker Zapped Cliff Giant Cliff Giant
            cEntry =  5357;
            cEntry2 = 5358;
            cEntry3 = 14640;
            cEntry4 = 14604;
            break;
        }
        if (uTarget && uTarget->GetTypeId() == TYPEID_UNIT && uTarget->isDead() &&
            (uTarget->GetEntry() == cEntry || uTarget->GetEntry() == cEntry2 || uTarget->GetEntry() == cEntry3 || uTarget->GetEntry() == cEntry4))
        {
            CAST_CRE(uTarget)->RemoveCorpse();
            return false;
        }
    }

    WorldPacket data(SMSG_CAST_FAILED, (4 + 2));            // prepare packet error message
    data << uint32(_Item->GetEntry());                      // itemId
    data << uint8(SPELL_FAILED_BAD_TARGETS);                // reason
    player->GetSession()->SendPacket(&data);                // send message: Invalid target

    player->SendEquipError(EQUIP_ERR_NONE, _Item, NULL);    // break spell
    return true;
}

/*#####
# item_chest_of_containment_coffers
#####*/

enum ContainmentCoffer
{
    // Creatures
    MOB_RIFT_SPAWN         = 6492,

    // Spells
    SPELL_SELF_STUN_30SEC  = 9032
};

bool ItemUse_item_chest_of_containment_coffers(Player* player, Item* _Item, SpellCastTargets const& targets)
{
    if (targets.getUnitTarget() && targets.getUnitTarget()->GetTypeId() == TYPEID_UNIT && targets.getUnitTarget()->IsAlive() &&
        targets.getUnitTarget()->GetEntry() == MOB_RIFT_SPAWN && targets.getUnitTarget()->HasAura(SPELL_SELF_STUN_30SEC))
        return false;

    WorldPacket data(SMSG_CAST_FAILED, (4 + 2));            // prepare packet error message
    data << uint32(_Item->GetEntry());                      // itemId
    data << uint8(SPELL_FAILED_NOT_READY);                  // reason
    player->GetSession()->SendPacket(&data);                // send message: Not ready yet

    player->SendEquipError(EQUIP_ERR_NONE, _Item, NULL);    // break spell
    return true;
}

/*#####
# item_inoculating_crystal
#####*/

bool ItemUse_item_inoculating_crystal(Player* player, Item* _Item, SpellCastTargets const& targets)
{
    if (targets.getUnitTarget() && targets.getUnitTarget()->GetTypeId() == TYPEID_UNIT &&
        targets.getUnitTarget()->GetEntry() == 16518)
        return false;

    WorldPacket data(SMSG_CAST_FAILED, (4 + 2));            // prepare packet error message
    data << uint32(_Item->GetEntry());                      // itemId
    data << uint8(SPELL_FAILED_BAD_TARGETS);                // reason
    player->GetSession()->SendPacket(&data);                // send message: Invalid target

    player->SendEquipError(EQUIP_ERR_NONE, _Item, NULL);    // break spell
    return true;
}

/*#####
# item_razorthorn_flayer_gland
#####*/

bool ItemUse_item_razorthorn_flayer_gland(Player* player, Item* _Item, SpellCastTargets const& targets)
{
    if (targets.getUnitTarget() && targets.getUnitTarget()->GetTypeId() == TYPEID_UNIT &&
        targets.getUnitTarget()->GetEntry() == 24922)
        return false;

    player->SendEquipError(EQUIP_ERR_YOU_CAN_NEVER_USE_THAT_ITEM, _Item, NULL);
    return true;
}

/*#####
# item_tame_beast_rods
#####*/

bool ItemUse_item_tame_beast_rods(Player* player, Item* _Item, SpellCastTargets const& targets)
{
    uint32 itemSpell = _Item->GetProto()->Spells[0].SpellId;
    uint32 cEntry = 0;

    if (itemSpell)
    {
        switch (itemSpell)
        {
        case 19548:
            cEntry =  1196;
            break;              //Ice Claw Bear
        case 19674:
            cEntry =  1126;
            break;              //Large Crag Boar
        case 19687:
            cEntry =  1201;
            break;              //Snow Leopard
        case 19688:
            cEntry =  2956;
            break;              //Adult Plainstrider
        case 19689:
            cEntry =  2959;
            break;              //Prairie Stalker
        case 19692:
            cEntry =  2970;
            break;              //Swoop
        case 19693:
            cEntry =  1998;
            break;              //Webwood Lurker
        case 19694:
            cEntry =  3099;
            break;              //Dire Mottled Boar
        case 19696:
            cEntry =  3107;
            break;              //Surf Crawler
        case 19697:
            cEntry =  3126;
            break;              //Armored Scorpid
        case 19699:
            cEntry =  2043;
            break;              //Nightsaber Stalker
        case 19700:
            cEntry =  1996;
            break;              //Strigid Screecher
        case 30646:
            cEntry = 17217;
            break;              //Barbed Crawler
        case 30653:
            cEntry = 17374;
            break;              //Greater Timberstrider
        case 30654:
            cEntry = 17203;
            break;              //Nightstalker
        case 30099:
            cEntry = 15650;
            break;              //Crazed Dragonhawk
        case 30102:
            cEntry = 15652;
            break;              //Elder Springpaw
        case 30105:
            cEntry = 16353;
            break;              //Mistbat
        }
        if (targets.getUnitTarget() && targets.getUnitTarget()->GetTypeId() == TYPEID_UNIT &&
            targets.getUnitTarget()->GetEntry() == cEntry)
            return false;
    }

    WorldPacket data(SMSG_CAST_FAILED, (4 + 2));            // prepare packet error message
    data << uint32(_Item->GetEntry());                      // itemId
    data << uint8(SPELL_FAILED_BAD_TARGETS);                // reason
    player->GetSession()->SendPacket(&data);                // send message: Invalid target

    player->SendEquipError(EQUIP_ERR_NONE, _Item, NULL);    // break spell
    return true;
}

/*#####
# item_soul_cannon
#####*/

bool ItemUse_item_soul_cannon(Player* player, Item* _Item, SpellCastTargets const& targets)
{
    // allow use
    if (targets.getUnitTarget() && targets.getUnitTarget()->GetTypeId() == TYPEID_UNIT &&
        targets.getUnitTarget()->GetEntry() == 22357)
        return false;

    // error
    player->SendEquipError(EQUIP_ERR_YOU_CAN_NEVER_USE_THAT_ITEM, _Item, NULL);
    return true;
}

/*#####
# item_sparrowhawk_net
#####*/

bool ItemUse_item_sparrowhawk_net(Player* player, Item* _Item, SpellCastTargets const& targets)
{
    if (targets.getUnitTarget() && targets.getUnitTarget()->GetTypeId() == TYPEID_UNIT &&
        targets.getUnitTarget()->GetEntry() == 22979)
        return false;

    player->SendEquipError(EQUIP_ERR_YOU_CAN_NEVER_USE_THAT_ITEM, _Item, NULL);
    return true;
}

/*#####
# item_voodoo_charm
#####*/

bool ItemUse_item_voodoo_charm(Player* player, Item* _Item, SpellCastTargets const& targets)
{
    if (targets.getUnitTarget() && targets.getUnitTarget()->GetTypeId() == TYPEID_UNIT && targets.getUnitTarget()->isDead() &&
        targets.getUnitTarget()->GetEntry() == 7318)
        return false;

    WorldPacket data(SMSG_CAST_FAILED, (4 + 2));            // prepare packet error message
    data << uint32(_Item->GetEntry());                      // itemId
    data << uint8(SPELL_FAILED_BAD_TARGETS);                // reason
    player->GetSession()->SendPacket(&data);                // send message: Invalid target

    player->SendEquipError(EQUIP_ERR_NONE, _Item, NULL);    // break spell
    return true;
}

/*#####
# item_yehkinyas_bramble
#####*/

bool ItemUse_item_yehkinyas_bramble(Player* player, Item* _Item, SpellCastTargets const& targets)
{
    if (player->GetQuestStatus(3520) == QUEST_STATUS_INCOMPLETE)
    {
        Unit* unit_target = targets.getUnitTarget();
        if (unit_target &&
            unit_target->GetTypeId() == TYPEID_UNIT &&
            unit_target->isDead() &&
            // cast only on corpse 5307 or 5308
            (unit_target->GetEntry() == 5307 || unit_target->GetEntry() == 5308))
        {
            CAST_CRE(unit_target)->RemoveCorpse();       // remove corpse for cancelling second use
            return false;                                   // all ok
        }
    }
    WorldPacket data(SMSG_CAST_FAILED, (4 + 2));            // prepare packet error message
    data << uint32(10699);                                  // itemId
    data << uint8(SPELL_FAILED_BAD_TARGETS);                // reason
    player->GetSession()->SendPacket(&data);                // send message: Bad target
    player->SendEquipError(EQUIP_ERR_NONE, _Item, NULL);    // break spell
    return true;
}

/*#####
# item_battery
#####*/

bool ItemUse_item_battery(Player* player, Item* _Item, SpellCastTargets const& targets)
{
    if (targets.getUnitTarget() && targets.getUnitTarget()->GetTypeId() == TYPEID_UNIT &&
        targets.getUnitTarget()->GetEntry() == 18879 && player->GetQuestStatus(10190) == QUEST_STATUS_INCOMPLETE &&
        (targets.getUnitTarget()->GetHealthPct() <= 25))
        return false;

    player->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, _Item, NULL);
    return true;
}

void AddSC_item_scripts()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "item_only_for_flight";
    newscript->pItemUse = &ItemUse_item_only_for_flight;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_attuned_crystal_cores";
    newscript->pItemUse = &ItemUse_item_attuned_crystal_cores;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_disciplinary_rod";
    newscript->pItemUse = &ItemUse_item_disciplinary_rod;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_draenei_fishing_net";
    newscript->pItemUse = &ItemUse_item_draenei_fishing_net;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_gor_dreks_ointment";
    newscript->pItemUse = &ItemUse_item_gor_dreks_ointment;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_muiseks_vessel";
    newscript->pItemUse = &ItemUse_item_muiseks_vessel;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_inoculating_crystal";
    newscript->pItemUse = &ItemUse_item_inoculating_crystal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_razorthorn_flayer_gland";
    newscript->pItemUse = &ItemUse_item_razorthorn_flayer_gland;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_tame_beast_rods";
    newscript->pItemUse = &ItemUse_item_tame_beast_rods;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_soul_cannon";
    newscript->pItemUse = &ItemUse_item_soul_cannon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_sparrowhawk_net";
    newscript->pItemUse = &ItemUse_item_sparrowhawk_net;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_voodoo_charm";
    newscript->pItemUse = &ItemUse_item_voodoo_charm;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_yehkinyas_bramble";
    newscript->pItemUse = &ItemUse_item_yehkinyas_bramble;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="item_chest_of_containment_coffers";
    newscript->pItemUse = &ItemUse_item_chest_of_containment_coffers;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_battery";
    newscript->pItemUse = &ItemUse_item_battery;
    newscript->RegisterSelf();
}

