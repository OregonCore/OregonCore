/* Copyright (C) 2006,2007 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: instance_uldaman
SD%Complete: 99%
SDComment: Need some cosmetics updates when archeadas door are closing (Guardians Waypoints).
SDCategory: Uldaman
EndScriptData */

#include "ScriptPCH.h"
#include "uldaman.h"

#define SPELL_ARCHAEDAS_AWAKEN        10347
#define SPELL_AWAKEN_VAULT_WALKER     10258

#define ARCHAEDAS_TEMPLE_DOOR           141869
#define ALTAR_OF_ARCHAEDAS              133234

#define ALTAR_OF_THE_KEEPER_TEMPLE_DOOR 124367
#define ALTAR_OF_THE_KEEPER_TEMPLE      130511

#define ANCIENT_VAULT_DOOR              124369
#define IRONAYA_SEAL_DOOR               124372

#define KEYSTONE_GO                     124371

#define ENCOUNTERS 3

struct instance_uldaman : public ScriptedInstance
{
    instance_uldaman(Map *map) : ScriptedInstance(map){};

    void Initialize()
    {
        archaedasGUID = 0;
        ironayaGUID = 0;
        whoWokeArchaedasGUID = 0;

        altarOfTheKeeperTempleDoor = 0;
        archaedasTempleDoor = 0;
        ancientVaultDoor = 0;
        ironayaSealDoor = 0;

        keystoneGUID = 0;

        ironayaSealDoorTimer = 26000;
        keystoneCheck = false;

        for (uint8 i=0; i < ENCOUNTERS; ++i)
            Encounters[i] = NOT_STARTED;

    }

    uint64 archaedasGUID;
    uint64 ironayaGUID;
    uint64 whoWokeArchaedasGUID;

    uint64 altarOfTheKeeperTempleDoor;
    uint64 archaedasTempleDoor;
    uint64 ancientVaultDoor;
    uint64 ironayaSealDoor;

    uint64 keystoneGUID;

    uint32 ironayaSealDoorTimer;
    bool keystoneCheck;

    std::vector<uint64> stoneKeeper;
    std::vector<uint64> altarOfTheKeeperCount;
    std::vector<uint64> vaultWalker;
    std::vector<uint64> earthenGuardian;
    std::vector<uint64> archaedasWallMinions;    // minions lined up around the wall

    uint32 Encounters[ENCOUNTERS];
    std::string str_data;

    void OnObjectCreate (GameObject* go)
    {
        switch (go->GetEntry())
        {
            case ALTAR_OF_THE_KEEPER_TEMPLE_DOOR:         // lock the door
                altarOfTheKeeperTempleDoor = go->GetGUID();

                if (Encounters[0] == DONE)
                    HandleGameObject(NULL,true,go);
                break;

            case ARCHAEDAS_TEMPLE_DOOR:
                archaedasTempleDoor = go->GetGUID();

                if (Encounters[0] == DONE)
                    HandleGameObject(NULL,true,go);
                break;

            case ANCIENT_VAULT_DOOR:
                go->SetGoState(GO_STATE_READY);
                go->SetUInt32Value(GAMEOBJECT_FLAGS, 33);
                ancientVaultDoor = go->GetGUID();

                if (Encounters[1] == DONE)
                    HandleGameObject(NULL,true,go);
                break;

        case IRONAYA_SEAL_DOOR:
                ironayaSealDoor = go->GetGUID();

                if (Encounters[2] == DONE)
                    HandleGameObject(NULL,true,go);
            break;

        case KEYSTONE_GO:
            keystoneGUID = go->GetGUID();

                if (Encounters[2] == DONE)
                {
                    HandleGameObject(NULL,true,go);
                    go->SetUInt32Value(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
        }
            break;
        }
    }

    void SetFrozenState(Creature *creature)
    {
        creature->setFaction(35);
        creature->RemoveAllAuras();
        //creature->RemoveFlag (UNIT_FIELD_FLAGS,UNIT_FLAG_ANIMATION_FROZEN);
        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
    }

    void SetDoor(uint64 guid, bool open)
    {
        GameObject *go = instance->GetGameObject(guid);
        if (!go)
            return;

        HandleGameObject(NULL,open,go);
    }

    void BlockGO(uint64 guid)
    {
        GameObject *go = instance->GetGameObject(guid);
        if (!go)
            return;
        go->SetUInt32Value(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);

    }

    void ActivateStoneKeepers()
    {
        for (std::vector<uint64>::iterator i = stoneKeeper.begin(); i != stoneKeeper.end(); ++i)
        {
            Creature *pTarget = instance->GetCreature(*i);
            if (!pTarget || !pTarget->isAlive() || pTarget->getFaction() == 14)
                continue;
            pTarget->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_DISABLE_MOVE);
            pTarget->setFaction(14);
            pTarget->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            return;        // only want the first one we find
        }
        // if we get this far than all four are dead so open the door
        SetData (DATA_ALTAR_DOORS, DONE);
        SetDoor (archaedasTempleDoor, true); //open next the door too
    }

    void ActivateWallMinions()
    {
        Creature *archaedas = instance->GetCreature(archaedasGUID);
        if (!archaedas)
            return;

        for (std::vector<uint64>::iterator i = archaedasWallMinions.begin(); i != archaedasWallMinions.end(); ++i)
        {
            Creature *pTarget = instance->GetCreature(*i);
            if (!pTarget || !pTarget->isAlive() || pTarget->getFaction() == 14)
                continue;
            archaedas->CastSpell(pTarget, SPELL_AWAKEN_VAULT_WALKER, true);
            pTarget->CastSpell(pTarget, SPELL_ARCHAEDAS_AWAKEN,true);
            return;        // only want the first one we find
        }
    }

    // used when Archaedas dies.  All active minions must be despawned.
    void DeActivateMinions()
    {
        // first despawn any aggroed wall minions
        for (std::vector<uint64>::iterator i = archaedasWallMinions.begin(); i != archaedasWallMinions.end(); ++i)
        {
            Creature *pTarget = instance->GetCreature(*i);
            if (!pTarget || pTarget->isDead() || pTarget->getFaction() != 14)
                continue;
            pTarget->setDeathState(JUST_DIED);
            pTarget->RemoveCorpse();
        }

        // Vault Walkers
        for (std::vector<uint64>::iterator i = vaultWalker.begin(); i != vaultWalker.end(); ++i)
        {
            Creature *pTarget = instance->GetCreature(*i);
            if (!pTarget || pTarget->isDead() || pTarget->getFaction() != 14)
                continue;
            pTarget->setDeathState(JUST_DIED);
            pTarget->RemoveCorpse();
        }

        // Earthen Guardians
        for (std::vector<uint64>::iterator i = earthenGuardian.begin(); i != earthenGuardian.end(); ++i)
        {
            Creature *pTarget = instance->GetCreature(*i);
            if (!pTarget || pTarget->isDead() || pTarget->getFaction() != 14)
                continue;
            pTarget->setDeathState(JUST_DIED);
            pTarget->RemoveCorpse();
        }
    }

    void ActivateArchaedas(uint64 pTarget)
    {
        Creature *archaedas = instance->GetCreature(archaedasGUID);
        if (!archaedas)
            return;

        if (Unit *victim = Unit::GetUnit(*archaedas, pTarget))
        {
            archaedas->CastSpell(archaedas, SPELL_ARCHAEDAS_AWAKEN,false);
            whoWokeArchaedasGUID = pTarget;
        }
    }

    void ActivateIronaya()
    {
        Creature *ironaya = instance->GetCreature(ironayaGUID);
        if (!ironaya)
            return;

        ironaya->setFaction(415);
        ironaya->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_DISABLE_MOVE);
        ironaya->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void RespawnMinions()
    {
        // first respawn any aggroed wall minions
        for (std::vector<uint64>::iterator i = archaedasWallMinions.begin(); i != archaedasWallMinions.end(); ++i)
        {
            Creature *pTarget = instance->GetCreature(*i);
            if (pTarget && pTarget->isDead())
            {
                pTarget->Respawn();
                pTarget->GetMotionMaster()->MoveTargetedHome();
                SetFrozenState(pTarget);
            }
        }

        // Vault Walkers
        for (std::vector<uint64>::iterator i = vaultWalker.begin(); i != vaultWalker.end(); ++i)
        {
            Creature *pTarget = instance->GetCreature(*i);
            if (pTarget && pTarget->isDead())
            {
                pTarget->Respawn();
                pTarget->GetMotionMaster()->MoveTargetedHome();
                SetFrozenState(pTarget);
            }
        }

        // Earthen Guardians
        for (std::vector<uint64>::iterator i = earthenGuardian.begin(); i != earthenGuardian.end(); ++i)
        {
            Creature *pTarget = instance->GetCreature(*i);
            if (pTarget && pTarget->isDead())
            {
                pTarget->Respawn();
                pTarget->GetMotionMaster()->MoveTargetedHome();
                SetFrozenState(pTarget);
            }
        }
    }

    void Update(uint32 diff)
    {
        if (!keystoneCheck)
        return;

    if (ironayaSealDoorTimer <= diff)
        {
        ActivateIronaya();

        SetDoor(ironayaSealDoor, true);
        BlockGO(keystoneGUID);

            SetData(DATA_IRONAYA_DOOR, DONE); //save state
        keystoneCheck = false;
        }
    else
            ironayaSealDoorTimer -= diff;
    }

    void SetData (uint32 type, uint32 data)
    {
        switch(type)
        {
            case DATA_ALTAR_DOORS:
            Encounters[0] = data;
                if (data == DONE)
                    SetDoor (altarOfTheKeeperTempleDoor, true);
                break;

        case DATA_ANCIENT_DOOR:
            Encounters[1] = data;
            if (data == DONE) //archeadas defeat
                {
                    SetDoor (archaedasTempleDoor, true); //re open enter door
                    SetDoor (ancientVaultDoor, true);
                }
                break;

        case DATA_IRONAYA_DOOR:
            Encounters[2] = data;
                break;

        case DATA_STONE_KEEPERS:
            ActivateStoneKeepers();
                break;

        case DATA_MINIONS:
                switch(data)
                {
                    case NOT_STARTED:
                    if (Encounters[0] == DONE) //if players opened the doors
                            SetDoor (archaedasTempleDoor, true);

                        RespawnMinions();
                        break;
                    case IN_PROGRESS:
                        ActivateWallMinions();
                    break;
                    case SPECIAL:
                        DeActivateMinions();
                        break;
                }
                break;

            case DATA_IRONAYA_SEAL:
                keystoneCheck = true;
                break;
    }

    if (data == DONE)
    {

            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << Encounters[0] << " " << Encounters[1] << " " << Encounters[2];

            str_data = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;

    }
    }

    void SetData64 (uint32 type, uint64 data)
    {
        // Archaedas
        if (type == 0)
        {
            ActivateArchaedas (data);
            SetDoor (archaedasTempleDoor, false); //close when event is started
        }
    }


    void OnCreatureCreate (Creature *creature, uint32 creature_entry)
    {
        switch (creature_entry) {
            case 4857:    // Stone Keeper
                SetFrozenState (creature);
                stoneKeeper.push_back(creature->GetGUID());
                break;

            case 7309:    // Earthen Custodian
                archaedasWallMinions.push_back(creature->GetGUID());
                break;

            case 7077:    // Earthen Hallshaper
                archaedasWallMinions.push_back(creature->GetGUID());
                break;

            case 7076:    // Earthen Guardian
                earthenGuardian.push_back(creature->GetGUID());
                break;

            case 7228:   // Ironaya
                ironayaGUID = creature->GetGUID();

                if (Encounters[2] != DONE)
                    SetFrozenState (creature);
                break;

            case 10120:    // Vault Walker
                vaultWalker.push_back(creature->GetGUID());
                break;

            case 2748:    // Archaedas
                archaedasGUID = creature->GetGUID();
                break;

        } // end switch
    } // end OnCreatureCreate


    uint64 GetData64 (uint32 identifier)
    {
        if (identifier == 0) return whoWokeArchaedasGUID;
        if (identifier == 1) return vaultWalker[0];    // VaultWalker1
        if (identifier == 2) return vaultWalker[1];    // VaultWalker2
        if (identifier == 3) return vaultWalker[2];    // VaultWalker3
        if (identifier == 4) return vaultWalker[3];    // VaultWalker4

        if (identifier == 5) return earthenGuardian[0];
        if (identifier == 6) return earthenGuardian[1];
        if (identifier == 7) return earthenGuardian[2];
        if (identifier == 8) return earthenGuardian[3];
        if (identifier == 9) return earthenGuardian[4];
        if (identifier == 10) return earthenGuardian[5];

        return 0;
    } // end GetData64

    std::string GetSaveData()
    {
        return str_data.c_str();
    }

    void Load(const char* in)
    {
        if (!in)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(in);

        std::istringstream loadStream(in);
        loadStream >> Encounters[0] >> Encounters[1] >> Encounters[2];

        for (uint8 i = 0; i < ENCOUNTERS; ++i)
            if (Encounters[i] == IN_PROGRESS)
                Encounters[i] = NOT_STARTED;

        OUT_LOAD_INST_DATA_COMPLETE;

    }

};



InstanceData* GetInstanceData_instance_uldaman(Map* map)
{
    return new instance_uldaman(map);
}

void AddSC_instance_uldaman()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_uldaman";
    newscript->GetInstanceData = &GetInstanceData_instance_uldaman;
    newscript->RegisterSelf();
}

