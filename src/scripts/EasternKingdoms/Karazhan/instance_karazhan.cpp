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
SDName: Instance_Karazhan
SD%Complete: 70
SDComment: Instance Script for Karazhan to help in various m_auiEncounter. @todo GameObject visibility for Opera event.
SDCategory: Karazhan
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedInstance.h"
#include "karazhan.h"

#define MAX_ENCOUNTER      12

/*
0  - Attumen + Midnight (optional)
1  - Moroes
2  - Maiden of Virtue (optional)
3  - Hyakiss the Lurker /  Rokad the Ravager  / Shadikith the Glider
4  - Opera Event
5  - Curator
6  - Shade of Aran (optional)
7  - Terestian Illhoof (optional)
8  - Netherspite (optional)
9  - Chess Event
10 - Prince Malchezzar
11 - Nightbane
*/

struct instance_karazhan : public ScriptedInstance
{
    instance_karazhan(Map* pMap) : ScriptedInstance(pMap)
    {
        Initialize();
    }

    uint32 m_auiEncounter[MAX_ENCOUNTER];

    uint32 m_uiOperaEvent;
    uint32 m_uiOzDeathCount;

    uint64 m_uiCurtainGUID;
    uint64 m_uiStageDoorLeftGUID;
    uint64 m_uiStageDoorRightGUID;
    uint64 m_uiKilrekGUID;
    uint64 m_uiTerestianGUID;
    uint64 m_uiMoroesGUID;
    uint64 m_uiNightBaneGUID;
    uint64 EchoOfMedivhGUID;

    uint64 m_uiLibraryDoor;                                     // Door at Shade of Aran
    uint64 m_uiMassiveDoor;                                     // Door at Netherspite
    uint64 m_uiSideEntranceDoor;                                // Side Entrance
    uint64 m_uiGamesmansDoor;                                   // Door before Chess
    uint64 m_uiGamesmansExitDoor;                               // Door after Chess
    uint64 m_uiNetherspaceDoor;                                // Door at Malchezaar
    uint64 m_uiServantsAccessDoor;                              // Door to Brocken Stair
    uint64 MastersTerraceDoor[2];
    uint64 ImageGUID;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

        // 1 - OZ, 2 - HOOD, 3 - RAJ, this never gets altered.
        m_uiOperaEvent      = urand(1, 3);
        m_uiOzDeathCount    = 0;

        m_uiCurtainGUID         = 0;
        m_uiStageDoorLeftGUID   = 0;
        m_uiStageDoorRightGUID  = 0;

        m_uiKilrekGUID      = 0;
        m_uiTerestianGUID   = 0;
        m_uiMoroesGUID      = 0;
        m_uiNightBaneGUID = 0;
        EchoOfMedivhGUID = 0;

        m_uiLibraryDoor         = 0;
        m_uiMassiveDoor         = 0;
        m_uiSideEntranceDoor    = 0;
        m_uiGamesmansDoor       = 0;
        m_uiGamesmansExitDoor   = 0;
        m_uiNetherspaceDoor     = 0;
        m_uiServantsAccessDoor  = 0;
        MastersTerraceDoor[0] = 0;
        MastersTerraceDoor[1] = 0;
        ImageGUID = 0;
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            if (m_auiEncounter[i] == IN_PROGRESS)
                return true;

        return false;
    }

    void OnCreatureCreate(Creature* pCreature, bool /*add*/)
    {
        switch (pCreature->GetEntry())
        {
        case 17229:
            m_uiKilrekGUID = pCreature->GetGUID();
            break;
        case 15688:
            m_uiTerestianGUID = pCreature->GetGUID();
            break;
        case 15687:
            m_uiMoroesGUID = pCreature->GetGUID();
            break;
        case 16816:
            EchoOfMedivhGUID = pCreature->GetGUID();
            break;
        }
    }

    void SetData(uint32 type, uint32 uiData)
    {
        switch (type)
        {
        case TYPE_ATTUMEN:
            if (m_auiEncounter[0] != DONE)
                m_auiEncounter[0]  = uiData;
            break;
        case TYPE_MOROES:
            if (m_auiEncounter[1] != DONE)
                m_auiEncounter[1] = uiData;
            break;
        case TYPE_MAIDEN:
            if (m_auiEncounter[2] != DONE)
                m_auiEncounter[2]  = uiData;
            break;
        case TYPE_OPTIONAL_BOSS:
            if (m_auiEncounter[3] != DONE)
                m_auiEncounter[3]  = uiData;
            break;
        case TYPE_OPERA:
            if (m_auiEncounter[4] != DONE)
                m_auiEncounter[4]  = uiData;
            break;
        case TYPE_CURATOR:
            if (m_auiEncounter[5] != DONE)
                m_auiEncounter[5]  = uiData;
            break;
        case TYPE_ARAN:
            if (m_auiEncounter[6] != DONE)
                m_auiEncounter[6]  = uiData;
            break;
        case TYPE_TERESTIAN:
            if (m_auiEncounter[7] != DONE)
                m_auiEncounter[7]  = uiData;
            break;
        case TYPE_NETHERSPITE:
            if (m_auiEncounter[8] != DONE)
                m_auiEncounter[8]  = uiData;
            break;
        case TYPE_CHESS:
            if (m_auiEncounter[9] != DONE)
                m_auiEncounter[9]  = uiData;
            else
                if (GameObject* door = instance->GetGameObject(GetData64(DATA_GO_GAME_EXIT_DOOR)))
                    door->UseDoorOrButton();
            break;
        case TYPE_MALCHEZZAR:
            if (m_auiEncounter[10] != DONE)
                m_auiEncounter[10] = uiData;
            break;
        case TYPE_NIGHTBANE:
            if (m_auiEncounter[11] != DONE)
                m_auiEncounter[11] = uiData;
            break;
        case DATA_OPERA_OZ_DEATHCOUNT:
            ++m_uiOzDeathCount;
            break;
        }

        if (uiData == DONE)
            SaveToDB();
    }

    void SetData64(uint32 identifier, uint64 data)
    {
        switch (identifier)
        {
        case DATA_IMAGE_OF_MEDIVH:
            ImageGUID = data;
        case DATA_NIGHTBANE:
            m_uiNightBaneGUID = data;
        }
    }

    void OnGameObjectCreate(GameObject* pGo, bool /*add*/)
    {
        switch (pGo->GetEntry())
        {
        case 183932:
            m_uiCurtainGUID          = pGo->GetGUID();
            break;
        case 184278:
            m_uiStageDoorLeftGUID = pGo->GetGUID();
            if (m_auiEncounter[4] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case 184279:
            m_uiStageDoorRightGUID = pGo->GetGUID();
            if (m_auiEncounter[4] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case 184517:
            m_uiLibraryDoor          = pGo->GetGUID();
            break;
        case 185521:
            m_uiMassiveDoor          = pGo->GetGUID();
            break;
        case 184276:
            m_uiGamesmansDoor        = pGo->GetGUID();
            break;
        case 184277:
            m_uiGamesmansExitDoor    = pGo->GetGUID();
            break;
        case 185134:
            m_uiNetherspaceDoor      = pGo->GetGUID();
            break;
        case 184274:
            MastersTerraceDoor[0] = pGo->GetGUID();
            break;
        case 184280:
            MastersTerraceDoor[1] = pGo->GetGUID();
            break;
        case 184275:
            m_uiSideEntranceDoor = pGo->GetGUID();
            if (m_auiEncounter[4] == DONE)
                pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
            else
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
            break;
        case 184281:
            m_uiServantsAccessDoor   = pGo->GetGUID();
            break;
        }

        switch (m_uiOperaEvent)
        {
        //@todo Set Object visibilities for Opera based on performance
        case EVENT_OZ:
            break;

        case EVENT_HOOD:
            break;

        case EVENT_RAJ:
            break;
        }
    }

    std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;
        std::ostringstream stream;
        stream << "K Z " << " "
               << m_auiEncounter[0] << " " << m_auiEncounter[1] << " "
               << m_auiEncounter[2] << " " << m_auiEncounter[3] << " "
               << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
               << m_auiEncounter[6] << " " << m_auiEncounter[7] << " "
               << m_auiEncounter[8] << " " << m_auiEncounter[9] << " "
               << m_auiEncounter[10] << " " << m_auiEncounter[11];
        char* out = new char[stream.str().length() + 1];
        strcpy(out, stream.str().c_str());
        if (out)
        {
            OUT_SAVE_INST_DATA_COMPLETE;
            return out;
        }

        return NULL;
    }

    uint32 GetData(uint32 uiData)
    {
        switch (uiData)
        {
        case TYPE_ATTUMEN:
            return m_auiEncounter[0];
        case TYPE_MOROES:
            return m_auiEncounter[1];
        case TYPE_MAIDEN:
            return m_auiEncounter[2];
        case TYPE_OPTIONAL_BOSS:
            return m_auiEncounter[3];
        case TYPE_OPERA:
            return m_auiEncounter[4];
        case TYPE_CURATOR:
            return m_auiEncounter[5];
        case TYPE_ARAN:
            return m_auiEncounter[6];
        case TYPE_TERESTIAN:
            return m_auiEncounter[7];
        case TYPE_NETHERSPITE:
            return m_auiEncounter[8];
        case TYPE_CHESS:
            return m_auiEncounter[9];
        case TYPE_MALCHEZZAR:
            return m_auiEncounter[10];
        case TYPE_NIGHTBANE:
            return m_auiEncounter[11];
        case DATA_OPERA_PERFORMANCE:
            return m_uiOperaEvent;
        case DATA_OPERA_OZ_DEATHCOUNT:
            return m_uiOzDeathCount;
        case DATA_IMAGE_OF_MEDIVH:
            return ImageGUID;
        }

        return 0;
    }

    uint64 GetData64(uint32 uiData)
    {
        switch (uiData)
        {
        case DATA_KILREK:
            return m_uiKilrekGUID;
        case DATA_TERESTIAN:
            return m_uiTerestianGUID;
        case DATA_MOROES:
            return m_uiMoroesGUID;
        case DATA_GO_STAGEDOORLEFT:
            return m_uiStageDoorLeftGUID;
        case DATA_GO_STAGEDOORRIGHT:
            return m_uiStageDoorRightGUID;
        case DATA_GO_CURTAINS:
            return m_uiCurtainGUID;
        case DATA_GO_LIBRARY_DOOR:
            return m_uiLibraryDoor;
        case DATA_GO_MASSIVE_DOOR:
            return m_uiMassiveDoor;
        case DATA_GO_SIDE_ENTRANCE_DOOR:
            return m_uiSideEntranceDoor;
        case DATA_GO_GAME_DOOR:
            return m_uiGamesmansDoor;
        case DATA_GO_GAME_EXIT_DOOR:
            return m_uiGamesmansExitDoor;
        case DATA_GO_NETHER_DOOR:
            return m_uiNetherspaceDoor;
        case DATA_IMAGE_OF_MEDIVH:
            return ImageGUID;
        case DATA_MASTERS_TERRACE_DOOR_1:
            return MastersTerraceDoor[0];
        case DATA_MASTERS_TERRACE_DOOR_2:
            return MastersTerraceDoor[1];
        case DATA_NIGHTBANE:
            return m_uiNightBaneGUID;
        case DATA_ECHO_OF_MEDIVH:
            return EchoOfMedivhGUID;
        }

        return 0;
    }

    void Load(const char* in)
    {
        if (!in)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(in);
        std::istringstream stream(in);
        char dataHead1, dataHead2;
        stream >> dataHead1 >> dataHead2 >>
               m_auiEncounter[0] >> m_auiEncounter[1] >>
               m_auiEncounter[2] >> m_auiEncounter[3] >>
               m_auiEncounter[4] >> m_auiEncounter[5] >>
               m_auiEncounter[6] >> m_auiEncounter[7] >>
               m_auiEncounter[8] >> m_auiEncounter[9] >>
               m_auiEncounter[10] >> m_auiEncounter[11];
        if (dataHead1 != 'K' || dataHead2 != 'Z')
        {
            error_log("SD2: Karazhan corrupted save uiData.");
            for (int i = 0; i < MAX_ENCOUNTER; i++)
                m_auiEncounter[i] = 0;
        }
        else OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_karazhan(Map* pMap)
{
    return new instance_karazhan(pMap);
}

void AddSC_instance_karazhan()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "instance_karazhan";
    newscript->GetInstanceData = &GetInstanceData_instance_karazhan;
    newscript->RegisterSelf();
}

