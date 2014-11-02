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
SDName: instance_zulaman
SD%Complete: 95%
SDComment:
SDCategory: Zul'Aman
EndScriptData */

#include "ScriptPCH.h"
#include "zulaman.h"

// Chests spawn at bear/eagle/dragonhawk/lynx bosses
// The loots depend on how many bosses have been killed, but not the entries of the chests
// But we cannot add loots to gameobject, so we have to use the fixed loot_template
struct SHostageInfo
{
    uint32 npc, pGo;
    float x, y, z, o;
};

static SHostageInfo HostageInfo[] =
{
    {23790, 186648, -57, 1343, 40.77f, 3.2f}, // bear
    {23999, 187021, 400, 1414, 74.36f, 3.3f}, // eagle
    {24024, 186667, -35, 1134, 18.71f, 1.9f}, // dragonhawk
    {24001, 186672, 413, 1117,  6.32f, 3.1f}  // lynx
};

struct instance_zulaman : public ScriptedInstance
{
    instance_zulaman(Map* pMap) : ScriptedInstance(pMap)
    {
        Initialize();
    };

    uint64 HarkorsSatchel;
    uint64 TanzarsTrunk;
    uint64 AshlisBag;
    uint64 KrazsPackage;

    uint64 MassiveGate;
    uint64 HexLordEntryDoor;
    uint64 HexLordExitDoor;
    uint64 ZulJinDoor;
    uint64 AkilzonDoor;
    uint64 HalazziEntryDoor;
    uint64 HalazziExitDoor;

    uint32 QuestTimer;
    uint32 BossKillCounter;
    uint32 QuestMinute;

    uint32 Encounters[ENCOUNTERS];

    void Initialize()
    {
        HarkorsSatchel = 0;
        TanzarsTrunk   = 0;
        AshlisBag      = 0;
        KrazsPackage   = 0;

        HexLordEntryDoor = 0;
        HexLordExitDoor  = 0;
        ZulJinDoor       = 0;
        AkilzonDoor      = 0;
        HalazziEntryDoor = 0;
        HalazziExitDoor  = 0;

        QuestTimer       = 60 * 60 * 1000; // 1 min
        QuestMinute      = 0; // Will be set when gong event is done
        BossKillCounter  = 0;

        for (uint32 i = 0; i < ENCOUNTERS; ++i)
            Encounters[i] = NOT_STARTED;
    }

    //bool IsEncounterInProgress() const
    //{
    //    for (uint8 i = 0; i < ENCOUNTERS; i++)
    //        if (Encounters[i] == IN_PROGRESS)
    //            return true;

    //    return false;
    //}

    bool IsEliteFourDown() const
    {
        for (uint32 i = 0; i < ENCOUNTER_ELITE_FOUR; i++)
            if (Encounters[i] != DONE)
                return false;

        return true;
    }

    void OnGameObjectCreate(GameObject* pGo, bool /*add*/)
    {
        switch (pGo->GetEntry())
        {
        case ENTRY_MASSIVE_GATE:
            MassiveGate      = pGo->GetGUID();
            break;
        case ENTRY_HALAZZI_EXIT_DOOR:
            HalazziExitDoor  = pGo->GetGUID();
            break;
        case ENTRY_HALAZZI_ENTRANCE_DOOR:
            HalazziEntryDoor = pGo->GetGUID();
            break;
        case ENTRY_HEXLORD_ENTRANCE_DOOR:
            HexLordEntryDoor = pGo->GetGUID();
            break;
        case ENTRY_HEXLORD_EXIT_DOOR:
            HexLordExitDoor  = pGo->GetGUID();
            break;
        case ENTRY_AKILZON_DOOR:
            AkilzonDoor      = pGo->GetGUID();
            break;
        case ENTRY_ZULJIN_DOOR:
            ZulJinDoor       = pGo->GetGUID();
            break;
        case ENTRY_CHEST_HARKORS:
            HarkorsSatchel   = pGo->GetGUID();
            break;
        case ENTRY_CHEST_TANZARS:
            TanzarsTrunk     = pGo->GetGUID();
            break;
        case ENTRY_CHEST_ASHLIS:
            AshlisBag        = pGo->GetGUID();
            break;
        case ENTRY_CHEST_KRAZS:
            KrazsPackage     = pGo->GetGUID();
            break;
        }

        CheckInstanceStatus();
    }

    void OpenDoor(uint64 guid, bool open)
    {
        if (GameObject* door = instance->GetGameObject(guid))
            door->SetGoState(open ? GO_STATE_ACTIVE : GO_STATE_READY);
    }

    void SummonHostage(uint8 num)
    {
        if (!QuestMinute)
            return;

        Map::PlayerList const& PlayerList = instance->GetPlayers();
        if (PlayerList.isEmpty())
            return;

        Map::PlayerList::const_iterator i = PlayerList.begin();
        if (Player* i_pl = i->getSource())
        {
            if (Unit* Hostage = i_pl->SummonCreature(HostageInfo[num].npc, HostageInfo[num].x, HostageInfo[num].y, HostageInfo[num].z, HostageInfo[num].o, TEMPSUMMON_DEAD_DESPAWN, 0))
            {
                Hostage->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                Hostage->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
        }
    }

    void CheckInstanceStatus()
    {
        if (IsEliteFourDown())
        {
            OpenDoor(HexLordEntryDoor, (GetData(ENCOUNTER_HEXLORD) != IN_PROGRESS));
            QuestMinute = 0;
            UpdateWorldState(WORLD_STATE_QUEST_RUNNING, 0);
        }
        else
        {
            OpenDoor(HexLordEntryDoor, false);
            UpdateWorldState(WORLD_STATE_QUEST_RUNNING, QuestMinute > 0);
            UpdateWorldState(WORLD_STATE_QUEST_TIMER,   QuestMinute);
        }

        OpenDoor(MassiveGate,      (GetData(ENCOUNTER_GONG)    == DONE));
        OpenDoor(HexLordExitDoor,  (GetData(ENCOUNTER_HEXLORD) == DONE));
        OpenDoor(HalazziExitDoor,  (GetData(ENCOUNTER_HALAZZI) == DONE));
        OpenDoor(HalazziEntryDoor, (GetData(ENCOUNTER_HALAZZI) != IN_PROGRESS));
        OpenDoor(AkilzonDoor,      (GetData(ENCOUNTER_AKILZON) != IN_PROGRESS));
        OpenDoor(ZulJinDoor,       (GetData(ENCOUNTER_ZULJIN)  != IN_PROGRESS));
    }

    void UpdateWorldState(uint32 field, uint32 value)
    {
        WorldPacket data(SMSG_UPDATE_WORLD_STATE, 8);
        data << field << value;
        instance->SendToPlayers(&data);
    }

    std::string GetSaveData()
    {
        std::ostringstream ss;
        ss << ZA_MAGIC << ' ';

        for (uint32 i = 0; i < ENCOUNTERS; ++i)
            ss << Encounters[i] << ' ';

        ss << QuestMinute << ' ';
        ss << ZA_MAGIC;

        return ss.str().c_str();
    }

    void Load(const char* load)
    {
        if (!load)
            return;

        uint32 startMagic, endMagic;
        std::istringstream ss(load);
        ss >> startMagic;

        for (uint32 i = 0; i < ENCOUNTERS; ++i)
            ss >> Encounters[i];

        ss >> QuestMinute;
        ss >> endMagic;

        if (startMagic != ZA_MAGIC || endMagic != ZA_MAGIC)
            error_log("OSCR: Zul'Aman: corrupted save data.");
    }

    void SetData(uint32 type, uint32 data)
    {
        if (! (type < ENCOUNTERS))
            return;

        Encounters[type] = data;

        if (type < ENCOUNTER_ELITE_FOUR)
        {
            if (data == DONE)
            {
                if (QuestMinute)
                {
                    /* Only two bosses should grant extra time:
                       - Akil'Zon - 10 minutes
                       - Nalorakk - 15 minutse */
                    if (type == ENCOUNTER_AKILZON)
                        QuestMinute += ZA_TIMER_ADDITION_AKILZON;
                    else if (type == ENCOUNTER_NALORAKK)
                        QuestMinute += ZA_TIMER_ADDITION_NALORAKK;
                    // Will be sent to client in CheckInstanceStatus()
                }

                SummonHostage(type);
            }
        }

        if (type == ENCOUNTER_GONG && data == DONE)
            QuestMinute = ZA_TIMER_MINUTES;

        CheckInstanceStatus();

        if (type == ENCOUNTER_CHESTLOOTED || data == DONE)
            SaveToDB();
    }

    uint32 GetData(uint32 type)
    {
        if (type < ENCOUNTERS)
            return Encounters[type];

        return 0;
    }

    void Update(uint32 diff)
    {
        if (QuestMinute)
        {
            if (QuestTimer <= diff)
            {
                QuestMinute--;
                CheckInstanceStatus();
                SaveToDB();

                QuestTimer = 60 * 60 * 1000; // 1 min
            }
            else
                QuestTimer -= diff;
        }
    }
};

InstanceData* GetInstanceData_instance_zulaman(Map* pMap)
{
    return new instance_zulaman(pMap);
}

void AddSC_instance_zulaman()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "instance_zulaman";
    newscript->GetInstanceData = &GetInstanceData_instance_zulaman;
    newscript->RegisterSelf();
}

