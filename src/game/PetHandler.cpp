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
#include "WorldSession.h"
#include "World.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Log.h"
#include "Opcodes.h"
#include "Spell.h"
#include "ObjectAccessor.h"
#include "MapManager.h"
#include "CreatureAI.h"
#include "Utilities/Util.h"
#include "Pet.h"
#include "Language.h"

void WorldSession::HandlePetAction(WorldPacket& recv_data)
{
    uint64 guid1;
    uint16 spellid;
    uint16 flag;
    uint64 guid2;
    recv_data >> guid1;                                     //pet guid
    recv_data >> spellid;
    recv_data >> flag;                                      //delete = 0x0700 CastSpell = C100
    recv_data >> guid2;                                     //tag guid

    // used also for charmed creature
    Unit* pet = ObjectAccessor::GetUnit(*_player, guid1);
    sLog.outDetail("HandlePetAction.Pet %u flag is %u, spellid is %u, target %u.", uint32(GUID_LOPART(guid1)), flag, spellid, uint32(GUID_LOPART(guid2)));
    if (!pet)
    {
        sLog.outError("Pet %u not exist.", uint32(GUID_LOPART(guid1)));
        return;
    }

    if (pet != GetPlayer()->GetFirstControlled())
    {
        sLog.outError("HandlePetAction.Pet %u isn't pet of player %s.", uint32(GUID_LOPART(guid1)), GetPlayer()->GetName());
        return;
    }

    if (pet->GetTypeId() == TYPEID_PLAYER)
    {
        // controller player can only do melee attack
        if (!(flag == ACT_COMMAND && spellid == COMMAND_ATTACK))
            return;
    }
    else if (((Creature*)pet)->IsPet())
    {
        // pet can have action bar disabled
        if (((Pet*)pet)->GetModeFlags() & PET_MODE_DISABLE_ACTIONS)
            return;
    }

    if (GetPlayer()->m_Controlled.size() == 1)
        HandlePetActionHelper(pet, guid1, spellid, flag, guid2);
    else
    {
        //If a pet is dismissed, m_Controlled will change
        std::vector<Unit*> controlled;
        for (Unit::ControlList::iterator itr = GetPlayer()->m_Controlled.begin(); itr != GetPlayer()->m_Controlled.end(); ++itr)
            if ((*itr)->GetEntry() == pet->GetEntry() && (*itr)->IsAlive())
                controlled.push_back(*itr);
        for (std::vector<Unit*>::iterator itr = controlled.begin(); itr != controlled.end(); ++itr)
            HandlePetActionHelper(*itr, guid1, spellid, flag, guid2);
    }
}

void WorldSession::HandlePetActionHelper(Unit* pet, uint64 guid1, uint16 spellid, uint16 flag, uint64 guid2)
{
    CharmInfo* charmInfo = pet->GetCharmInfo();
    if (!charmInfo)
    {
        sLog.outError("WorldSession::HandlePetAction: object (GUID: %u TypeId: %u) is considered pet-like but doesn't have a charminfo!", pet->GetGUIDLow(), pet->GetTypeId());
        return;
    }

    switch (flag)
    {
    case ACT_COMMAND:                                   //0x0700
        switch (spellid)
        {
        case COMMAND_STAY:                          //flat=1792  //STAY
            pet->AttackStop();
            pet->InterruptNonMeleeSpells(false);
            pet->GetMotionMaster()->MoveIdle();
            charmInfo->SetCommandState(COMMAND_STAY);
            charmInfo->SetIsCommandFollow(false);
            charmInfo->SetIsCommandAttack(false);
            charmInfo->SetIsAtStay(true);
            charmInfo->SetIsFollowing(false);
            charmInfo->SetIsReturning(false);
            charmInfo->SaveStayPosition();
            break;
        case COMMAND_FOLLOW:                        //spellid=1792  //FOLLOW
            pet->AttackStop();
            pet->InterruptNonMeleeSpells(false);
            pet->ClearInPetCombat();
            pet->GetMotionMaster()->MoveFollow(_player, PET_FOLLOW_DIST, pet->GetFollowAngle());
            charmInfo->SetCommandState(COMMAND_FOLLOW);
            charmInfo->SetIsCommandFollow(true);
            charmInfo->SetIsCommandAttack(false);
            charmInfo->SetIsAtStay(false);
            charmInfo->SetIsReturning(true);
            charmInfo->SetIsFollowing(false);
            break;
        case COMMAND_ATTACK:                        //spellid=1792  //ATTACK
            {
                // Can't attack if owner is pacified
                if (_player->HasAuraType(SPELL_AURA_MOD_PACIFY))
                {
                    //pet->SendPetCastFail(spellid, SPELL_FAILED_PACIFIED);
                    //@todo Send proper error message to client
                    return;
                }

                // only place where pet can be player
                Unit* TargetUnit = ObjectAccessor::GetUnit(*_player, guid2);
                if (!TargetUnit)
                    return;

                if (Unit* owner = pet->GetOwner())
                    if (!owner->IsValidAttackTarget(TargetUnit))
                        return;

                pet->ClearUnitState(UNIT_STATE_FOLLOW);
                // This is true if pet has no target or has target but targets differs.
                if (pet->GetVictim() != TargetUnit || (pet->GetVictim() == TargetUnit && !pet->GetCharmInfo()->IsCommandAttack()))
                {
                    if (pet->GetVictim())
                        pet->AttackStop();

                    if (pet->GetTypeId() != TYPEID_PLAYER && pet->ToCreature()->IsAIEnabled)
                    {
                        charmInfo->SetIsCommandAttack(true);
                        charmInfo->SetIsAtStay(false);
                        charmInfo->SetIsFollowing(false);
                        charmInfo->SetIsReturning(false);
                        charmInfo->SetIsCommandFollow(false);

                        pet->ToCreature()->AI()->AttackStart(TargetUnit);

                        //10% chance to play special pet attack talk, else growl
                        if (pet->IsPet() && ((Pet*)pet)->getPetType() == SUMMON_PET && pet != TargetUnit && urand(0, 100) < 10)
                            pet->SendPetTalk((uint32)PET_TALK_ATTACK);
                        else
                        {
                            // 90% chance for pet and 100% chance for charmed creature
                            pet->SendPetAIReaction(guid1);
                        }
                    }
                    else                                // charmed player
                    {
                        charmInfo->SetIsCommandAttack(true);
                        charmInfo->SetIsAtStay(false);
                        charmInfo->SetIsFollowing(false);
                        charmInfo->SetIsReturning(false);
                        charmInfo->SetIsCommandFollow(false);
                        pet->Attack(TargetUnit, true);
                        pet->SendPetAIReaction(guid1);
                    }
                }
                break;
            }
        case COMMAND_ABANDON:                       // abandon (hunter pet) or dismiss (summoned pet)
            if (pet->GetCharmerGUID() == GetPlayer()->GetGUID())
                _player->StopCastingCharm();
            else if (pet->GetOwnerGUID() == GetPlayer()->GetGUID())
            {
                assert(pet->GetTypeId() == TYPEID_UNIT);
                if (pet->IsPet())
                {
                    if (((Pet*)pet)->getPetType() == HUNTER_PET)
                        GetPlayer()->RemovePet((Pet*)pet, PET_SAVE_AS_DELETED);
                    else
                        //dismissing a summoned pet is like killing them (this prevents returning a soulshard...)
                        pet->setDeathState(CORPSE);
                }
                else if (pet->ToCreature()->HasUnitTypeMask(UNIT_MASK_MINION))
                    ((Minion*)pet)->UnSummon();
            }
            break;
        default:
            sLog.outError("WORLD: unknown PET flag Action %i and spellid %i.", flag, spellid);
        }
        break;
    case ACT_REACTION:                                  // 0x600
        switch (spellid)
        {
        case REACT_PASSIVE:                         //passive
            pet->AttackStop();
            pet->ClearInPetCombat();
            // no break
        case REACT_DEFENSIVE:                       //recovery
        case REACT_AGGRESSIVE:                      //activete
            if (pet->GetTypeId() == TYPEID_UNIT)
                pet->ToCreature()->SetReactState(ReactStates(spellid));
            break;
        }
        break;
    case ACT_DISABLED:                                  //0x8100    spell (disabled), ignore
    case ACT_CAST:                                      //0x0100
    case ACT_ENABLED:                                   //0xc100    spell
        {
            Unit* unit_target = NULL;

            if (pet->ToCreature()->GetGlobalCooldown() > 0)
                return;

            if (guid2)
                unit_target = ObjectAccessor::GetUnit(*_player, guid2);

            // do not cast unknown spells
            SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellid);
            if (!spellInfo)
            {
                sLog.outError("WORLD: unknown PET spell id %i", spellid);
                return;
            }

            for (uint32 i = 0; i < 3; ++i)
            {
                if (spellInfo->EffectImplicitTargetA[i] == TARGET_UNIT_AREA_ENEMY_SRC || spellInfo->EffectImplicitTargetA[i] == TARGET_UNIT_AREA_ENEMY_DST || spellInfo->EffectImplicitTargetA[i] == TARGET_DEST_DYNOBJ_ENEMY)
                    return;
            }

            // do not cast not learned spells
            if (!pet->HasSpell(spellid) || IsPassiveSpell(spellid))
                return;

            //  Clear the flags as if owner clicked 'attack'. AI will reset them
            //  after AttackStart, even if spell failed
            if (pet->GetCharmInfo())
            {
                pet->GetCharmInfo()->SetIsAtStay(false);
                pet->GetCharmInfo()->SetIsCommandAttack(true);
                pet->GetCharmInfo()->SetIsReturning(false);
                pet->GetCharmInfo()->SetIsFollowing(false);
            }

            Spell* spell = new Spell(pet, spellInfo, false);

            SpellCastResult result = spell->CheckPetCast(unit_target);

            //auto turn to target unless possessed
            if (result == SPELL_FAILED_UNIT_NOT_INFRONT && !pet->isPossessed())
            {
                pet->SetInFront(unit_target);
                if (unit_target->GetTypeId() == TYPEID_PLAYER)
                    pet->SendUpdateToPlayer(unit_target->ToPlayer());
                if (Unit* powner = pet->GetCharmerOrOwner())
                    if (powner->GetTypeId() == TYPEID_PLAYER)
                        pet->SendUpdateToPlayer(powner->ToPlayer());
                result = SPELL_CAST_OK;
            }

            if (result == SPELL_CAST_OK)
            {
                pet->ToCreature()->AddCreatureSpellCooldown(spellid);
                if (pet->IsPet())
                    ((Pet*)pet)->CheckLearning(spellid);

                unit_target = spell->m_targets.getUnitTarget();

                //10% chance to play special pet attack talk, else growl
                //actually this only seems to happen on special spells, fire shield for imp, torment for voidwalker, but it's stupid to check every spell
                if (pet->IsPet() && (((Pet*)pet)->getPetType() == SUMMON_PET) && (pet != unit_target) && (urand(0, 100) < 10))
                    pet->SendPetTalk((uint32)PET_TALK_SPECIAL_SPELL);
                else
                    pet->SendPetAIReaction(guid1);

                if (unit_target && !GetPlayer()->IsFriendlyTo(unit_target) && !pet->isPossessed())
                {
                    // This is true if pet has no target or has target but targets differs.
                    if (pet->GetVictim() != unit_target)
                    {
                        pet->GetMotionMaster()->Clear();
                        if (pet->ToCreature()->IsAIEnabled)
                            pet->ToCreature()->AI()->AttackStart(unit_target);
                    }
                }

                spell->prepare(&(spell->m_targets));
            }
            else
            {
                if (pet->isPossessed())
                {
                    WorldPacket data(SMSG_CAST_FAILED, (4 + 1 + 1));
                    data << uint32(spellid) << uint8(2) << uint8(result);
                    switch (result)
                    {
                    case SPELL_FAILED_REQUIRES_SPELL_FOCUS:
                        data << uint32(spellInfo->RequiresSpellFocus);
                        break;
                    case SPELL_FAILED_REQUIRES_AREA:
                        data << uint32(spellInfo->AreaId);
                        break;
                    default:
                        break;
                    }
                    SendPacket(&data);
                }
                else
                    pet->SendPetCastFail(spellid, result);

                if (!pet->ToCreature()->HasSpellCooldown(spellid))
                    pet->SendPetClearCooldown(spellid);

                spell->finish(false);
                delete spell;

                // reset specific flags in case of spell fail. AI will reset other flags
                if (pet->GetCharmInfo())
                    pet->GetCharmInfo()->SetIsCommandAttack(false);
            }
            break;
        }
    default:
        sLog.outError("WORLD: unknown PET flag Action %i and spellid %i.", flag, spellid);
    }
}
void WorldSession::HandlePetStopAttack(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Received CMSG_PET_STOP_ATTACK");

    ObjectGuid petGuid;
    recv_data >> petGuid;

    Unit* pet = ObjectAccessor::GetUnit(*_player, petGuid);    // pet or controlled creature/player
    if (!pet)
    {
        sLog.outError("HandlePetStopAttack: %s doesn't exist.", petGuid.GetString().c_str());
        return;
    }

    if (GetPlayer()->GetGUID() != pet->GetCharmerOrOwnerGUID())
    {
        sLog.outError("HandlePetStopAttack: %s isn't charm/pet of %s.", petGuid.GetString().c_str(), _player->GetGUIDLow());
        return;
    }

    if (!pet->IsAlive())
        return;

    pet->AttackStop();
}

void WorldSession::HandlePetNameQuery(WorldPacket& recv_data)
{
    sLog.outDetail("HandlePetNameQuery. CMSG_PET_NAME_QUERY");

    uint32 petnumber;
    uint64 petguid;

    recv_data >> petnumber;
    recv_data >> petguid;

    SendPetNameQuery(petguid, petnumber);
}

void WorldSession::SendPetNameQuery(uint64 petguid, uint32 petnumber)
{
    Creature* pet = ObjectAccessor::GetCreatureOrPet(*_player, petguid);
    if (!pet || !pet->GetCharmInfo() || pet->GetCharmInfo()->GetPetNumber() != petnumber)
        return;

    std::string name = pet->GetName();

    WorldPacket data(SMSG_PET_NAME_QUERY_RESPONSE, (4 + 4 + name.size() + 1));
    data << uint32(petnumber);
    data << name.c_str();
    data << uint32(pet->GetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP));

    if (pet->IsPet() && ((Pet*)pet)->GetDeclinedNames())
    {
        data << uint8(1);
        for (int i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            data << ((Pet*)pet)->GetDeclinedNames()->name[i];
    }
    else
        data << uint8(0);

    _player->GetSession()->SendPacket(&data);
}

void WorldSession::HandlePetSetAction(WorldPacket& recv_data)
{
    sLog.outDetail("HandlePetSetAction. CMSG_PET_SET_ACTION");

    uint64 petguid;
    uint32 position;
    uint16 spell_id;
    uint16 act_state;
    uint8  count;

    recv_data >> petguid;

    Unit* pet = ObjectAccessor::GetUnit(*_player, petguid);
    if (!pet || pet != _player->GetFirstControlled())
    {
        sLog.outError("HandlePetSetAction: Unknown pet or pet owner.");
        return;
    }

    // pet can have action bar disabled
    if (pet->IsPet() && ((Pet*)pet)->GetModeFlags() & PET_MODE_DISABLE_ACTIONS)
        return;

    CharmInfo* charmInfo = pet->GetCharmInfo();
    if (!charmInfo)
    {
        sLog.outError("WorldSession::HandlePetSetAction: object (GUID: %u TypeId: %u) is considered pet-like but doesn't have a charminfo!", pet->GetGUIDLow(), pet->GetTypeId());
        return;
    }

    count = (recv_data.size() == 24) ? 2 : 1;
    for (uint8 i = 0; i < count; i++)
    {
        recv_data >> position;
        recv_data >> spell_id;
        recv_data >> act_state;

        sLog.outDetail("Player %s has changed pet spell action. Position: %u, Spell: %u, State: 0x%X", _player->GetName(), position, spell_id, act_state);

        //if it's act for spell (en/disable/cast) and there is a spell given (0 = remove spell) which pet doesn't know, don't add
        if (!((act_state == ACT_ENABLED || act_state == ACT_DISABLED || act_state == ACT_CAST) && spell_id && !pet->HasSpell(spell_id)))
        {
            //sign for autocast
            if (act_state == ACT_ENABLED && spell_id)
            {
                if (pet->GetTypeId() == TYPEID_UNIT && pet->IsPet())
                    ((Pet*)pet)->ToggleAutocast(spell_id, true);
                else
                    charmInfo->ToggleCreatureAutocast(spell_id, true);
            }
            //sign for no/turn off autocast
            else if (act_state == ACT_DISABLED && spell_id)
            {
                if (pet->GetTypeId() == TYPEID_UNIT && pet->IsPet())
                    ((Pet*)pet)->ToggleAutocast(spell_id, false);
                else
                    charmInfo->ToggleCreatureAutocast(spell_id, false);
            }

            charmInfo->GetActionBarEntry(position)->Type = act_state;
            charmInfo->GetActionBarEntry(position)->SpellOrAction = spell_id;
        }
    }
}

void WorldSession::HandlePetRename(WorldPacket& recv_data)
{
    sLog.outDetail("HandlePetRename. CMSG_PET_RENAME");

    uint64 petguid;
    uint8 isdeclined;

    std::string name;
    DeclinedName declinedname;

    recv_data >> petguid;
    recv_data >> name;
    recv_data >> isdeclined;

    Pet* pet = ObjectAccessor::FindPet(petguid);
    // check it!
    if (!pet || !pet->IsPet() || ((Pet*)pet)->getPetType() != HUNTER_PET ||
        pet->GetByteValue(UNIT_FIELD_BYTES_2, 2) != UNIT_RENAME_ALLOWED ||
        pet->GetOwnerGUID() != _player->GetGUID() || !pet->GetCharmInfo())
        return;

    if (!ObjectMgr::IsValidPetName(name))
    {
        SendPetNameInvalid(PET_NAME_INVALID, name, NULL);
        return;
    }

    if (sObjectMgr.IsReservedName(name))
    {
        SendPetNameInvalid(PET_NAME_RESERVED, name, NULL);
        return;
    }

    pet->SetName(name);

    Unit* owner = pet->GetOwner();
    if (owner && (owner->GetTypeId() == TYPEID_PLAYER) && owner->ToPlayer()->GetGroup())
        owner->ToPlayer()->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_NAME);

    pet->SetByteValue(UNIT_FIELD_BYTES_2, 2, UNIT_RENAME_NOT_ALLOWED);

    if (isdeclined)
    {
        for (int i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            recv_data >> declinedname.name[i];

        std::wstring wname;
        Utf8toWStr(name, wname);
        if (!ObjectMgr::CheckDeclinedNames(GetMainPartOfName(wname, 0), declinedname))
        {
            SendPetNameInvalid(PET_NAME_DECLENSION_DOESNT_MATCH_BASE_NAME, name, &declinedname);
            return;
        }
    }

    CharacterDatabase.BeginTransaction();
    if (isdeclined)
    {
        for (int i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            CharacterDatabase.escape_string(declinedname.name[i]);
        CharacterDatabase.PExecute("DELETE FROM character_pet_declinedname WHERE owner = '%u' AND id = '%u'", _player->GetGUIDLow(), pet->GetCharmInfo()->GetPetNumber());
        CharacterDatabase.PExecute("INSERT INTO character_pet_declinedname (id, owner, genitive, dative, accusative, instrumental, prepositional) VALUES ('%u','%u','%s','%s','%s','%s','%s')",
                                   pet->GetCharmInfo()->GetPetNumber(), _player->GetGUIDLow(), declinedname.name[0].c_str(), declinedname.name[1].c_str(), declinedname.name[2].c_str(), declinedname.name[3].c_str(), declinedname.name[4].c_str());
    }

    CharacterDatabase.escape_string(name);
    CharacterDatabase.PExecute("UPDATE character_pet SET name = '%s', renamed = '1' WHERE owner = '%u' AND id = '%u'", name.c_str(), _player->GetGUIDLow(), pet->GetCharmInfo()->GetPetNumber());
    CharacterDatabase.CommitTransaction();

    pet->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, time(NULL));
}

void WorldSession::HandlePetAbandon(WorldPacket& recv_data)
{
    uint64 guid;
    recv_data >> guid;                                      //pet guid
    sLog.outDetail("HandlePetAbandon. CMSG_PET_ABANDON pet guid is %u", GUID_LOPART(guid));

    if (!_player->IsInWorld())
        return;

    // pet/charmed
    Creature* pet = ObjectAccessor::GetCreatureOrPet(*_player, guid);
    if (pet)
    {
        if (pet->IsPet())
        {
            if (pet->GetGUID() == _player->GetPetGUID())
            {
                uint32 feelty = pet->GetPower(POWER_HAPPINESS);
                pet->SetPower(POWER_HAPPINESS , (feelty - 50000) > 0 ? (feelty - 50000) : 0);
            }

            _player->RemovePet((Pet*)pet, PET_SAVE_AS_DELETED);
        }
        else if (pet->GetGUID() == _player->GetCharmGUID())
            _player->StopCastingCharm();
    }
}

void WorldSession::HandlePetUnlearnOpcode(WorldPacket& recvPacket)
{
    sLog.outDetail("CMSG_PET_UNLEARN");
    uint64 guid;
    recvPacket >> guid;

    Pet* pet = _player->GetPet();

    if (!pet || pet->getPetType() != HUNTER_PET || pet->m_spells.size() <= 1)
        return;

    if (guid != pet->GetGUID())
    {
        sLog.outError("HandlePetUnlearnOpcode.Pet %u isn't pet of player %s .", uint32(GUID_LOPART(guid)), GetPlayer()->GetName());
        return;
    }

    CharmInfo* charmInfo = pet->GetCharmInfo();
    if (!charmInfo)
    {
        sLog.outError("WorldSession::HandlePetUnlearnOpcode: object (GUID: %u TypeId: %u) is considered pet-like but doesn't have a charminfo!", pet->GetGUIDLow(), pet->GetTypeId());
        return;
    }

    uint32 cost = pet->resetTalentsCost();

    if (GetPlayer()->GetMoney() < cost)
    {
        GetPlayer()->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, 0, 0, 0);
        return;
    }

    for (PetSpellMap::iterator itr = pet->m_spells.begin(); itr != pet->m_spells.end();)
    {
        uint32 spell_id = itr->first;                       // Pet::removeSpell can invalidate iterator at erase NEW spell
        ++itr;
        pet->removeSpell(spell_id);
    }

    pet->SetTP(pet->getLevel() * (pet->GetLoyaltyLevel() - 1));

    for (uint8 i = 0; i < 10; i++)
    {
        if ((charmInfo->GetActionBarEntry(i)->SpellOrAction && charmInfo->GetActionBarEntry(i)->Type == ACT_ENABLED) || charmInfo->GetActionBarEntry(i)->Type == ACT_DISABLED)
            charmInfo->GetActionBarEntry(i)->SpellOrAction = 0;
    }

    // relearn pet passives
    pet->LearnPetPassives();

    pet->m_resetTalentsTime = time(NULL);
    pet->m_resetTalentsCost = cost;
    GetPlayer()->ModifyMoney(-(int32)cost);

    GetPlayer()->PetSpellInitialize();
}

void WorldSession::HandlePetSpellAutocastOpcode(WorldPacket& recvPacket)
{
    sLog.outDetail("CMSG_PET_SPELL_AUTOCAST");
    uint64 guid;
    uint16 spellid;
    uint16 spellid2;                                        //maybe second spell, automatically toggled off when first toggled on?
    uint8  state;                                           //1 for on, 0 for off
    recvPacket >> guid >> spellid >> spellid2 >> state;

    if (!_player->GetGuardianPet() && !_player->GetCharm())
        return;

    if (ObjectAccessor::FindPlayer(guid))
        return;

    Creature* pet = ObjectAccessor::GetCreatureOrPet(*_player, guid);

    if (!pet || (pet != _player->GetGuardianPet() && pet != _player->GetCharm()))
    {
        sLog.outError("HandlePetSpellAutocastOpcode.Pet %u isn't pet of player %s .", uint32(GUID_LOPART(guid)), GetPlayer()->GetName());
        return;
    }

    // do not add not learned spells/ passive spells
    if (!pet->HasSpell(spellid) || !IsAutocastableSpell(spellid))
        return;

    CharmInfo* charmInfo = pet->GetCharmInfo();
    if (!charmInfo)
    {
        sLog.outError("WorldSession::HandlePetSpellAutocastOpcod: object (GUID: %u TypeId: %u) is considered pet-like but doesn't have a charminfo!", pet->GetGUIDLow(), pet->GetTypeId());
        return;
    }

    if (pet->IsPet())
        ((Pet*)pet)->ToggleAutocast(spellid, state);
    else
        pet->GetCharmInfo()->ToggleCreatureAutocast(spellid, state);

    for (uint8 i = 0; i < 10; ++i)
    {
        if ((charmInfo->GetActionBarEntry(i)->Type == ACT_ENABLED || charmInfo->GetActionBarEntry(i)->Type == ACT_DISABLED) && spellid == charmInfo->GetActionBarEntry(i)->SpellOrAction)
            charmInfo->GetActionBarEntry(i)->Type = state ? ACT_ENABLED : ACT_DISABLED;
    }
}

void WorldSession::HandlePetCastSpellOpcode(WorldPacket& recvPacket)
{
    sLog.outDetail("WORLD: CMSG_PET_CAST_SPELL");

    uint64 guid;
    uint32 spellid;

    recvPacket >> guid >> spellid;

    // This opcode is also sent from charmed and possessed units (players and creatures)
    if (!_player->GetGuardianPet() && !_player->GetCharm())
        return;

    Unit* caster = ObjectAccessor::GetUnit(*_player, guid);

    if (!caster || (caster != _player->GetGuardianPet() && caster != _player->GetCharm()))
    {
        sLog.outError("HandlePetCastSpellOpcode: Pet %u isn't pet of player %s .", uint32(GUID_LOPART(guid)), GetPlayer()->GetName());
        return;
    }

    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellid);
    if (!spellInfo)
    {
        sLog.outError("WORLD: unknown PET spell id %i", spellid);
        return;
    }

    // do not cast not learned spells
    if (!caster->HasSpell(spellid) || IsPassiveSpell(spellid))
        return;

    if (spellInfo->StartRecoveryCategory > 0) //Check if spell is affected by GCD
        if (caster->GetTypeId() == TYPEID_UNIT && caster->ToCreature()->GetGlobalCooldown() > 0)
        {
            caster->SendPetCastFail(spellid, SPELL_FAILED_NOT_READY);
            return;
        }

    SpellCastTargets targets;

    recvPacket >> targets.ReadForCaster(caster);

    caster->ClearUnitState(UNIT_STATE_MOVING);

    Spell* spell = new Spell(caster, spellInfo, false);
    spell->m_targets = targets;

    SpellCastResult result = spell->CheckPetCast(NULL);
    if (result == SPELL_CAST_OK)
    {
        if (caster->GetTypeId() == TYPEID_UNIT)
        {
            Creature* pet = caster->ToCreature();
            pet->AddCreatureSpellCooldown(spellid);
            if (pet->IsPet())
            {
                Pet* p = (Pet*)pet;
                p->CheckLearning(spellid);
                // 10% chance to play special pet attack talk, else growl
                // actually this only seems to happen on special spells, fire shield for imp, torment for voidwalker, but it's stupid to check every spell
                if (p->getPetType() == SUMMON_PET && (urand(0, 100) < 10))
                    pet->SendPetTalk((uint32)PET_TALK_SPECIAL_SPELL);
                else
                    pet->SendPetAIReaction(guid);
            }
        }

        spell->prepare(&(spell->m_targets));
    }
    else
    {
        caster->SendPetCastFail(spellid, result);
        if (caster->GetTypeId() == TYPEID_PLAYER)
        {
            if (!caster->ToPlayer()->HasSpellCooldown(spellid))
                caster->SendPetClearCooldown(spellid);
        }
        else
        {
            if (!caster->ToCreature()->HasSpellCooldown(spellid))
                caster->SendPetClearCooldown(spellid);
        }

        spell->finish(false);
        delete spell;
    }
}

void WorldSession::SendPetNameInvalid(uint32 error, const std::string& name, DeclinedName* declinedName)
{
    WorldPacket data(SMSG_PET_NAME_INVALID, 4 + name.size() + 1 + 1);
    data << uint32(error);
    data << name;
    if (declinedName)
    {
        data << uint8(1);
        for (uint32 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            data << declinedName->name[i];
    }
    else
        data << uint8(0);
    SendPacket(&data);
}

