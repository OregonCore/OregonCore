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
SDName: Instance_Blood_Furnace
SD%Complete: 95
SDComment:
SDCategory: Hellfire Citadel, Blood Furnace
EndScriptData */

#include "ScriptPCH.h"
#include "blood_furnace.h"

#define  MAX_ORC_WAVES               4
#define  MAX_BROGGOK_WAVES           5

#define NPC_BROGGOK                  17380
#define NPC_NASCENT_FEL_ORC          17398

#define SAY_BROGGOK_INTRO            -1542015

#define ENCOUNTERS                   3

struct instance_blood_furnace : public ScriptedInstance
{
    instance_blood_furnace(Map *map) : ScriptedInstance(map){Initialize();}

    struct BroggokEventInfo
    {
        BroggokEventInfo() : IsCellOpened(false), KilledOrcCount(0) {}

        uint64 CellGuid;
        bool IsCellOpened;
        uint8 KilledOrcCount;
        std::set<uint64> SortedOrcGuids;
    };

    uint32 Encounter[ENCOUNTERS];
    std::string str_data;
    BroggokEventInfo BroggokEvent[MAX_ORC_WAVES];
    std::vector<uint64> NascentOrcGuids;
    uint32 BroggokEventTimer;
    uint32 BroggokEventPhase;
    uint32 DoorTimer;
    uint64 BroggokGUID;
    uint64 Sewer1GUID;
    uint64 Sewer2GUID;
    uint64 Maker1GUID;
    uint64 Maker2GUID;
    uint64 Brog1GUID;
    uint64 Brog2GUID;
    uint64 LeverGUID;

    void Initialize()
    {
        Sewer1GUID = 0;
        Sewer2GUID = 0;
        Maker1GUID = 0;
        Maker2GUID = 0;
        Brog1GUID  = 0;
        Brog2GUID  = 0;
        LeverGUID  = 0;
        DoorTimer = 0;
        BroggokEventTimer = 0;
        BroggokEventPhase = 0;

        for (uint8 i = 0; i < ENCOUNTERS; i++)
            Encounter[i] = NOT_STARTED;
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < ENCOUNTERS; i++)
            if (Encounter[i] == IN_PROGRESS) return true;

        return false;
    }

    void OnCreatureCreate(Creature* pCreature, bool /*add*/)
    {
        switch(pCreature->GetEntry())
        {
            case NPC_BROGGOK: BroggokGUID = pCreature->GetGUID(); break;
            case NPC_NASCENT_FEL_ORC: NascentOrcGuids.push_back(pCreature->GetGUID()); break;
        }
    }

    void OnGameObjectCreate(GameObject* pGo, bool /*add*/)
    {
        switch(pGo->GetEntry())
        {
            case GO_SUMMON_DOOR: Sewer1GUID = pGo->GetGUID(); break;
            case GO_PRISON_DOOR_01: Sewer2GUID = pGo->GetGUID(); break;
            case GO_PRISON_DOOR_02: Maker1GUID = pGo->GetGUID(); break;
            case GO_PRISON_DOOR_03: Maker2GUID = pGo->GetGUID(); break;
            case GO_PRISON_DOOR_05: Brog1GUID = pGo->GetGUID(); break;
            case GO_PRISON_DOOR_04: Brog2GUID = pGo->GetGUID(); break;
            case GO_BROGGOK_LEVER: LeverGUID = pGo->GetGUID(); break;
            case GO_PRISON_CELL_DOOR_5: BroggokEvent[0].CellGuid = pGo->GetGUID(); return;
            case GO_PRISON_CELL_DOOR_7: BroggokEvent[1].CellGuid = pGo->GetGUID(); return;
            case GO_PRISON_CELL_DOOR_6: BroggokEvent[2].CellGuid = pGo->GetGUID(); return;
            case GO_PRISON_CELL_DOOR_8: BroggokEvent[3].CellGuid = pGo->GetGUID(); return;
        }
    }

    Player* GetPlayerInMap()
    {
        Map::PlayerList const& players = instance->GetPlayers();

        if (!players.isEmpty())
        {
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                if (Player* plr = itr->getSource())
                    return plr;
            }
        }

        debug_log("OSCR: Instance Blood Furnace: GetPlayerInMap, but PlayerList is empty!");
        return NULL;
    }

    void SetData(uint32 type, uint32 data)
    {
        switch(type)
        {
            case DATA_MAKEREVENT:
                if (data == IN_PROGRESS)
                {
                    HandleGameObject(Maker1GUID, false);
                }
                else HandleGameObject(Maker1GUID, true);
                if (data == DONE)
                {
                    HandleGameObject(Maker2GUID, true);
                }
                if (Encounter[0] != DONE)
                    Encounter[0] = data;
                break;
            case DATA_BROGGOKEVENT:
                if (data == IN_PROGRESS)
                {
                    if (Creature* pBroggok = instance->GetCreature(BroggokGUID))
                        DoScriptText(SAY_BROGGOK_INTRO, pBroggok);

                    HandleGameObject(Brog1GUID, false);

                    if (BroggokEventPhase <= MAX_ORC_WAVES)
                    {
                        DoSortBroggokOrcs();
                        DoNextBroggokEventPhase();
                    }
                }
                else 
                {
                    HandleGameObject(Brog1GUID, true);
                }
                if (data == FAIL)
                {
                    if (BroggokEventPhase <= MAX_BROGGOK_WAVES)
                    {
                        for (uint8 i = 0; i < MAX_ORC_WAVES; ++i)
                        {
                            if (!BroggokEvent[i].IsCellOpened)
                                continue;

                            BroggokEvent[i].KilledOrcCount = 0;
                            for (std::set<uint64>::const_iterator itr = BroggokEvent[i].SortedOrcGuids.begin(); itr != BroggokEvent[i].SortedOrcGuids.end(); ++itr)
                            {
                                if (Creature* pOrc = instance->GetCreature(*itr))
                                {
                                    if (!pOrc->isAlive())
                                    {
                                        pOrc->Respawn();
                                        BroggokEventPhase = 0;
                                    }
                                }
                            }

                            DoUseDoorOrButton(BroggokEvent[i].CellGuid);
                            BroggokEvent[i].IsCellOpened = false;
                            if (GameObject* Lever = instance->GetGameObject(LeverGUID))
                            {
                                if (Lever)
                                    Lever->ResetDoorOrButton();
                            }
                        }
                    }
           
                }
                if (data == DONE)
                {
                    HandleGameObject(Brog2GUID, true);
                }
                if (Encounter[1] != DONE)
                    Encounter[1] = data;
                break;
            case DATA_KELIDANEVENT:
                if (data == DONE)
                {
                    HandleGameObject(Sewer1GUID, true);
                    HandleGameObject(Sewer2GUID, true);
                }
                if (Encounter[2] != DONE)
                    Encounter[2] = data;
                break;
        }

        if (data == DONE)
        {
            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint32 GetData(uint32 type)
    {
        switch(type)
        {
            case DATA_MAKEREVENT: return Encounter[0];
            case DATA_BROGGOKEVENT: return Encounter[1];
            case DATA_KELIDANEVENT: return Encounter[2];
        }
        return false;
    }

    uint64 GetData64(uint32 /*identifier*/)
    {
        return 0;
    }

    std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;
        std::ostringstream saveStream;

        saveStream << Encounter[0] << " " << Encounter[1] << " " << Encounter[2];

        char* out = new char[saveStream.str().length() + 1];
        strcpy(out, saveStream.str().c_str());
        if (out)
        {
            OUT_SAVE_INST_DATA_COMPLETE;
            return out;
        }

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
        loadStream >> Encounter[0] >> Encounter[1] >> Encounter[2];

        for (uint8 i = 0; i < ENCOUNTERS; ++i)
            if (Encounter[i] == IN_PROGRESS)
                Encounter[i] = NOT_STARTED;

        OUT_LOAD_INST_DATA_COMPLETE;
    }

#if COMPILER == COMPILER_GNU
#pragma GCC diagnostic ignored "-Wuninitialized"
#endif

    void DoNextBroggokEventPhase()
    {
        float dx = .0f, dy = .0f;
        GetMovementDistanceForIndex(BroggokEventPhase, dx, dy);

        if (BroggokEventPhase >= MAX_ORC_WAVES)
        {
            DoUseDoorOrButton(Brog2GUID);

            if (Creature* pBroggok = instance->GetCreature(BroggokGUID))
                {
                    pBroggok->GetMotionMaster()->MovePoint(0, dx, dy, pBroggok->GetPositionZ());

                    DoorTimer = 5000;
                }
        }
        else
        {
            if (!BroggokEvent[BroggokEventPhase].IsCellOpened)
                DoUseDoorOrButton(BroggokEvent[BroggokEventPhase].CellGuid);
                BroggokEvent[BroggokEventPhase].IsCellOpened = true;
			

            for(std::set<uint64>::const_iterator itr = BroggokEvent[BroggokEventPhase].SortedOrcGuids.begin(); itr != BroggokEvent[BroggokEventPhase].SortedOrcGuids.end(); ++itr)
            {
                if (Creature* pOrc = instance->GetCreature(*itr))
                {
                    pOrc->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    pOrc->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    pOrc->GetMotionMaster()->MovePoint(0, pOrc->GetPositionX() + dx, pOrc->GetPositionY() + dy, pOrc->GetPositionZ());
                }
            }
        }
        BroggokEventTimer = 30000;
        ++BroggokEventPhase;
    }

#if COMPILER == COMPILER_GNU
#pragma GCC diagnostic warning "-Wuninitialized"
#endif

    void Update(uint32 diff)
    {
        if (BroggokEventTimer && BroggokEventPhase < MAX_ORC_WAVES && instance->IsHeroic())
        {
            if (BroggokEventTimer <= diff)
            {
                DoNextBroggokEventPhase();
            }
            else BroggokEventTimer -= diff;
        }

        if (DoorTimer)
        {
            if (DoorTimer <= diff)
            {
                if (GameObject* Brog2 = instance->GetGameObject(Brog2GUID))
                {
                    if (Brog2)
                        Brog2->ResetDoorOrButton();

                }
                if (Creature* pBroggok = instance->GetCreature(BroggokGUID))
                {
                    pBroggok->SetReactState(REACT_AGGRESSIVE);
                    pBroggok->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE | UNIT_FLAG_PASSIVE);
                    pBroggok->SetInCombatWithZone();
                }
            }
            else DoorTimer -= diff;
        }
    }

    void OnCreatureDeath(Creature* pCreature)
    {
        if (pCreature->GetEntry() == NPC_NASCENT_FEL_ORC)
        {
            uint8 uiClearedCells = 0;
            for (uint8 i = 0; i < std::min<uint32>(BroggokEventPhase, MAX_ORC_WAVES); ++i)
            {
                if (BroggokEvent[i].SortedOrcGuids.size() == BroggokEvent[i].KilledOrcCount)
                {
                    ++uiClearedCells;
                    continue;
                }

               if (BroggokEvent[i].SortedOrcGuids.find(pCreature->GetGUID()) != BroggokEvent[i].SortedOrcGuids.end())
                   BroggokEvent[i].KilledOrcCount++;

               if (BroggokEvent[i].SortedOrcGuids.size() == BroggokEvent[i].KilledOrcCount)
                   ++uiClearedCells;
            }

            if (uiClearedCells == BroggokEventPhase)
                DoNextBroggokEventPhase();
        }
    }

    void DoSortBroggokOrcs()
    {
        for (std::vector<uint64>::const_iterator itr = NascentOrcGuids.begin(); itr != NascentOrcGuids.end(); ++itr)
        {
            if (Creature* pOrc = instance->GetCreature(*itr))
            {
                for (uint8 i = 0; i < MAX_ORC_WAVES; ++i)
                {
                    if (GameObject* pDoor = instance->GetGameObject(BroggokEvent[i].CellGuid))
                    {
                        if (pOrc->IsWithinDistInMap(pDoor, 8.0f))
                        {
                            BroggokEvent[i].SortedOrcGuids.insert(pOrc->GetGUID());
                            if (!pOrc->isAlive())
                            {
                                pOrc->ForcedDespawn();
                                pOrc->Respawn();
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    void GetMovementDistanceForIndex(uint32 uiIndex, float& dx, float& dy)
    {
        GameObject* pDoor[2];

        if (uiIndex < MAX_ORC_WAVES)
        {
            pDoor[0] = instance->GetGameObject(BroggokEvent[(uiIndex / 2) * 2].CellGuid);
            pDoor[1] = instance->GetGameObject(BroggokEvent[(uiIndex / 2) * 2 + 1].CellGuid);
        }
        else
        {
            pDoor[0] = instance->GetGameObject(BroggokEvent[0].CellGuid);
            pDoor[1] = instance->GetGameObject(BroggokEvent[3].CellGuid);
        }

        if (!pDoor[0] || !pDoor[1])
            return;

        if (uiIndex < MAX_ORC_WAVES)
        {
            dx = (pDoor[0]->GetPositionX() + pDoor[1]->GetPositionX()) / 2 - pDoor[uiIndex % 2]->GetPositionX();
            dy = (pDoor[0]->GetPositionY() + pDoor[1]->GetPositionY()) / 2 - pDoor[uiIndex % 2]->GetPositionY();
        }
        else
        {
            dx = (pDoor[0]->GetPositionX() + pDoor[1]->GetPositionX()) / 2;
            dy = (pDoor[0]->GetPositionY() + pDoor[1]->GetPositionY()) / 2;
        }
    }
};

InstanceData* GetInstanceData_instance_blood_furnace(Map* map)
{
    return new instance_blood_furnace(map);
}

bool GOHello_go_prison_cell_lever(Player* /*pPlayer*/, GameObject* pGo)
{
    if (ScriptedInstance* instance = (ScriptedInstance*)pGo->GetInstanceData())
        if (instance->GetData(DATA_BROGGOKEVENT) != DONE && instance->GetData(DATA_BROGGOKEVENT) != IN_PROGRESS)
            instance->SetData(DATA_BROGGOKEVENT, IN_PROGRESS);

    return false;
}

void AddSC_instance_blood_furnace()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_blood_furnace";
    newscript->GetInstanceData = &GetInstanceData_instance_blood_furnace;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_prison_cell_lever";
    newscript->pGOHello = &GOHello_go_prison_cell_lever;
    newscript->RegisterSelf();
}

