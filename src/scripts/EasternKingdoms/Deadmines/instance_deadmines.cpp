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
SDName: Instance_Deadmines
SD%Complete: 100
SDComment:
SDCategory: Deadmines
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "deadmines.h"
#include "ScriptedInstance.h"
#include "WorldPacket.h"

enum Sounds
{
    SOUND_CANNONFIRE                                     = 1400,
    SOUND_DESTROYDOOR                                    = 3079,
    SOUND_MR_SMITE_ALARM1                                = 5775,
    SOUND_MR_SMITE_ALARM2                                = 5777
};

#define SAY_MR_SMITE_ALARM1 "You there, check out that noise!"
#define SAY_MR_SMITE_ALARM2 "We're under attack! A vast, ye swabs! Repel the invaders!"

enum Misc
{
    DATA_CANNON_BLAST_TIMER                                = 3000,
    DATA_PIRATES_DELAY_TIMER                               = 1000,
    DATA_SMITE_ALARM_DELAY_TIMER                           = 5000
};

struct instance_deadmines : public ScriptedInstance
{
    instance_deadmines(Map* pMap) : ScriptedInstance(pMap)
    {
        Initialize();
    };

    uint64 FactoryDoorGUID;
    uint64 MastRoomDoorGUID;
    uint64 FoundryDoorGUID;
    uint64 IronCladDoorGUID;
    uint64 DefiasCannonGUID;
    uint64 DoorLeverGUID;
    uint64 DefiasPirate1GUID;
    uint64 DefiasPirate2GUID;
    uint64 DefiasCompanionGUID;
    uint64 MrSmiteGUID;

    uint32 CannonEventState;
    uint32 CannonBlast_Timer;
    uint32 PiratesDelay_Timer;
    uint32 SmiteAlarmDelay_Timer;
    uint64 uiSmiteChestGUID;

    void Initialize()
    {
        FactoryDoorGUID = 0;
        MastRoomDoorGUID = 0;
        FoundryDoorGUID = 0;
        IronCladDoorGUID = 0;
        DefiasCannonGUID = 0;
        DoorLeverGUID = 0;
        DefiasPirate1GUID = 0;
        DefiasPirate2GUID = 0;
        DefiasCompanionGUID = 0;

        CannonEventState = CANNON_NOT_USED;
        uiSmiteChestGUID = 0;
    }

    virtual void Update(uint32 diff)
    {
        if (!IronCladDoorGUID || !DefiasCannonGUID || !DoorLeverGUID)
            return;

        GameObject* pIronCladDoor = instance->GetGameObject(IronCladDoorGUID);
        if (!pIronCladDoor)
            return;

        switch (CannonEventState)
        {
        case CANNON_GUNPOWDER_USED:
            CannonBlast_Timer = DATA_CANNON_BLAST_TIMER;
            CannonEventState = CANNON_BLAST_INITIATED;
            break;
        case CANNON_BLAST_INITIATED:
            PiratesDelay_Timer = DATA_PIRATES_DELAY_TIMER;
            SmiteAlarmDelay_Timer = DATA_SMITE_ALARM_DELAY_TIMER;
            if (CannonBlast_Timer <= diff)
            {
                SummonCreatures();
                ShootCannon();
                BlastOutDoor();
                LeverStucked();
                pIronCladDoor->SetName("Mr. Smite");
                pIronCladDoor->MonsterYell(SAY_MR_SMITE_ALARM1, LANG_UNIVERSAL, 0);
                DoPlaySound(pIronCladDoor, SOUND_MR_SMITE_ALARM1);
                CannonEventState = PIRATES_ATTACK;
            }
            else CannonBlast_Timer -= diff;
            break;
        case PIRATES_ATTACK:
            if (PiratesDelay_Timer <= diff)
            {
                MoveCreaturesInside();
                CannonEventState = SMITE_ALARMED;
            }
            else PiratesDelay_Timer -= diff;
            break;
        case SMITE_ALARMED:
            if (SmiteAlarmDelay_Timer <= diff)
            {
                pIronCladDoor->MonsterYell(SAY_MR_SMITE_ALARM2, LANG_UNIVERSAL, 0);
                DoPlaySound(pIronCladDoor, SOUND_MR_SMITE_ALARM2);
                CannonEventState = EVENT_DONE;
            } else SmiteAlarmDelay_Timer -= diff;
            break;
        }
    }

    void SummonCreatures()
    {
        if (GameObject* pIronCladDoor = instance->GetGameObject(IronCladDoorGUID))
        {
            Creature* DefiasPirate1 = pIronCladDoor->SummonCreature(657, pIronCladDoor->GetPositionX() - 2, pIronCladDoor->GetPositionY() - 7, pIronCladDoor->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
            Creature* DefiasPirate2 = pIronCladDoor->SummonCreature(657, pIronCladDoor->GetPositionX() + 3, pIronCladDoor->GetPositionY() - 6, pIronCladDoor->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
            Creature* DefiasCompanion = pIronCladDoor->SummonCreature(3450, pIronCladDoor->GetPositionX() + 2, pIronCladDoor->GetPositionY() - 6, pIronCladDoor->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);

            DefiasPirate1GUID = DefiasPirate1->GetGUID();
            DefiasPirate2GUID = DefiasPirate2->GetGUID();
            DefiasCompanionGUID = DefiasCompanion->GetGUID();
        }
    }

    void MoveCreaturesInside()
    {
        if (!DefiasPirate1GUID || !DefiasPirate2GUID || !DefiasCompanionGUID)
            return;

        Creature* pDefiasPirate1 = instance->GetCreature(DefiasPirate1GUID);
        Creature* pDefiasPirate2 = instance->GetCreature(DefiasPirate2GUID);
        Creature* pDefiasCompanion = instance->GetCreature(DefiasCompanionGUID);
        if (!pDefiasPirate1 || !pDefiasPirate2 || !pDefiasCompanion)
            return;

        MoveCreatureInside(pDefiasPirate1);
        MoveCreatureInside(pDefiasPirate2);
        MoveCreatureInside(pDefiasCompanion);
    }

    void MoveCreatureInside(Creature* pCreature)
    {
        pCreature->SetWalk(false);
        pCreature->GetMotionMaster()->MovePoint(0, -102.7f, -655.9f, pCreature->GetPositionZ());
    }

    void ShootCannon()
    {
        if (GameObject* pDefiasCannon = instance->GetGameObject(DefiasCannonGUID))
        {
            pDefiasCannon->SetGoState(GO_STATE_ACTIVE);
            DoPlaySound(pDefiasCannon, SOUND_CANNONFIRE);
        }
    }

    void BlastOutDoor()
    {
        if (GameObject* pIronCladDoor = instance->GetGameObject(IronCladDoorGUID))
        {
            pIronCladDoor->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
            DoPlaySound(pIronCladDoor, SOUND_DESTROYDOOR);
        }
    }

    void LeverStucked()
    {
        if (GameObject* pDoorLever = instance->GetGameObject(DoorLeverGUID))
            pDoorLever->SetUInt32Value(GAMEOBJECT_FLAGS, 4);
    }


    void OnCreatureCreate(Creature* pCreature, bool /*add*/)
    {
        switch (pCreature->GetEntry())
        {
            case NPC_MR_SMITE:
                MrSmiteGUID = pCreature->GetGUID();
                break;
            default:
                break;
        }
    }

    void OnGameObjectCreate(GameObject* pGo, bool /*add*/)
    {
        switch (pGo->GetEntry())
        {
        case GO_FACTORY_DOOR:
            FactoryDoorGUID  = pGo->GetGUID();
            break;
        case GO_MAST_ROOM_DOOR:
            MastRoomDoorGUID = pGo->GetGUID();
            break;
        case GO_FOUNDRY_DOOR:
            FoundryDoorGUID  = pGo->GetGUID();
            break;
        case GO_IRONCLAD_DOOR:
            IronCladDoorGUID = pGo->GetGUID();
            break;
        case GO_DEFIAS_CANNON:
            DefiasCannonGUID = pGo->GetGUID();
            break;
        case GO_DOOR_LEVER:
            DoorLeverGUID    = pGo->GetGUID();
            break;
        case GO_MR_SMITE_CHEST:
            uiSmiteChestGUID = pGo->GetGUID();
            break;
        }
    }

    void SetData(uint32 type, uint32 data)
    {
        switch (type)
        {
        case EVENT_CANNON:
            if (DefiasCannonGUID && IronCladDoorGUID)
                CannonEventState = data;
            break;
        case EVENT_RHAHKZOR:
            if (data == DONE)
                if (GameObject* pGo = instance->GetGameObject(FactoryDoorGUID))
                    pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case EVENT_SNEED:
            if (data == DONE)
                if (GameObject* pGo = instance->GetGameObject(MastRoomDoorGUID))
                    pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case EVENT_GILNID:
            if (data == DONE)
                if (GameObject* pGo = instance->GetGameObject(FoundryDoorGUID))
                    pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        }
    }

    uint32 GetData(uint32 type)
    {
        switch (type)
        {
        case EVENT_CANNON:
            return CannonEventState;
        }

        return 0;
    }

    void DoPlaySound(GameObject* unit, uint32 sound)
    {
        WorldPacket data(4);
        data.SetOpcode(SMSG_PLAY_SOUND);
        data << uint32(sound);
        unit->SendMessageToSet(&data, false);
    }

    void DoPlaySoundCreature(Unit* unit, uint32 sound)
    {
        WorldPacket data(4);
        data.SetOpcode(SMSG_PLAY_SOUND);
        data << uint32(sound);
        unit->SendMessageToSet(&data, false);
    }

    void OnCreatureDeath(Creature* pCreature)
    {
        switch (pCreature->GetEntry())
        {
        case 644:
            SetData(EVENT_RHAHKZOR, DONE);
            break;
        case 643:
            SetData(EVENT_SNEED, DONE);
            break;
        case 1763:
            SetData(EVENT_GILNID, DONE);
        default:
            break;
        }
    }
};

InstanceData* GetInstanceData_instance_deadmines(Map* pMap)
{
    return new instance_deadmines(pMap);
}

void AddSC_instance_deadmines()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "instance_deadmines";
    newscript->GetInstanceData = &GetInstanceData_instance_deadmines;
    newscript->RegisterSelf();
}
