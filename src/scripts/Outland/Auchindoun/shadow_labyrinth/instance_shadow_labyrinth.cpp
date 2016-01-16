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
SDName: Instance_Shadow_Labyrinth
SD%Complete: 85
SDComment: Some cleanup left along with save
SDCategory: Auchindoun, Shadow Labyrinth
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "shadow_labyrinth.h"

#define ENCOUNTERS 5

#define REFECTORY_DOOR          183296                      //door opened when blackheart the inciter dies
#define SCREAMING_HALL_DOOR     183295                      //door opened when grandmaster vorpil dies

/* Shadow Labyrinth encounters:
1 - Ambassador Hellmaw event
2 - Blackheart the Inciter event
3 - Grandmaster Vorpil event
4 - Murmur event
*/

struct instance_shadow_labyrinth : public ScriptedInstance
{
    instance_shadow_labyrinth(Map* map) : ScriptedInstance(map)
    {
        Initialize();
    };

    uint32 Encounter[ENCOUNTERS];
    std::string str_data;

    uint64 RefectoryDoorGUID;
    uint64 ScreamingHallDoorGUID;
    std::list<uint64> RitualistGUIDList;

    uint64 GrandmasterVorpil;
    uint32 RitualistCount;

    bool check;

    void Initialize()
    {
        RefectoryDoorGUID = 0;
        ScreamingHallDoorGUID = 0;

        RitualistGUIDList.clear();

        GrandmasterVorpil = 0;
        RitualistCount = 0;

        for (uint8 i = 0; i < ENCOUNTERS; i++)
            Encounter[i] = NOT_STARTED;
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < ENCOUNTERS; i++)
            if (Encounter[i] == IN_PROGRESS) return true;

        return false;
    }

    void OnGameObjectCreate(GameObject* pGo, bool /*add*/)
    {
        switch (pGo->GetEntry())
        {
        case REFECTORY_DOOR:
            RefectoryDoorGUID = pGo->GetGUID();
            break;
        case SCREAMING_HALL_DOOR:
            ScreamingHallDoorGUID = pGo->GetGUID();
            break;
        }
    }

    void OnCreatureCreate(Creature* pCreature, bool /*add*/)
    {
        switch (pCreature->GetEntry())
        {
        case 18732:
            GrandmasterVorpil = pCreature->GetGUID();
            break;
        case 18794:
            if (pCreature->IsAlive())
            {
                RitualistGUIDList.push_back(pCreature->GetGUID());
                ++RitualistCount;
            }
            break;
        }
    }

    void OnCreatureDeath(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case 18794:
            //case 20645:
                    SetData(TYPE_RITUALIST, DONE);
                break;
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

        debug_log("OSCR: Instance Shadow Labyrinth: GetPlayerInMap, but PlayerList is empty!");
        return NULL;
    }

    void SetData(uint32 type, uint32 data)
    {
        switch (type)
        {
        case TYPE_HELLMAW:
            if (Encounter[0] != DONE)
                Encounter[0] = data;
            break;
            case TYPE_RITUALIST:
                if (data == NOT_STARTED)
                    Encounter[1] = NOT_STARTED;
                if (data == DONE && RitualistCount)
                    --RitualistCount;
                if (RitualistCount == 0)
                Encounter[1] = DONE;
            break;

        case DATA_BLACKHEARTTHEINCITEREVENT:
            if (data == DONE)
                DoUseDoorOrButton(RefectoryDoorGUID);
            if (Encounter[2] != DONE)
                Encounter[2] = data;
            break;

        case DATA_GRANDMASTERVORPILEVENT:
            if (data == DONE)
                HandleGameObject(ScreamingHallDoorGUID, true);
            if (Encounter[3] != DONE)
                Encounter[3] = data;
            break;

        case DATA_MURMUREVENT:
            if (Encounter[4] != DONE)
                Encounter[4] = data;
            break;
        }

        if (data == DONE)
        {
            if (type == TYPE_RITUALIST && RitualistCount != 0)
                return;

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

     uint32 GetData(uint32 type)
     {
         switch(type)
         {
             case TYPE_HELLMAW: return Encounter[0];
            case TYPE_RITUALIST: return Encounter[1];
             case DATA_GRANDMASTERVORPILEVENT: return Encounter[3];
             case DATA_MURMUREVENT: return Encounter[4];
         }
         return false;
     }

    uint64 GetData64(uint32 identifier)
    {
        if (identifier == DATA_GRANDMASTERVORPIL)
            return GrandmasterVorpil;

        return 0;
    }

    void Update(uint32 diff)
    {
        if (!check && !RitualistGUIDList.empty())
        {
            for (std::list<uint64>::iterator iter = RitualistGUIDList.begin(); iter != RitualistGUIDList.end(); ++iter)
            {
                Creature* ritualist = instance->GetCreature(*iter);
                if (ritualist && ritualist->isDead())
                    --RitualistCount;
            }

            if (RitualistCount <= 0)
                Encounter[1] = DONE;
            else
                Encounter[1] = NOT_STARTED;

            check = true;
        }
    }

    std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;
        std::ostringstream saveStream;

        saveStream << Encounter[0] << " " << Encounter[1] << " "
                   << Encounter[2] << " " << Encounter[3] << " " << Encounter[4];

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
        loadStream >> Encounter[0] >> Encounter[1] >> Encounter[2] >> Encounter[3] >> Encounter[4];

        for (uint8 i = 0; i < ENCOUNTERS; ++i)
            if (Encounter[i] == IN_PROGRESS)
                Encounter[i] = NOT_STARTED;

        OUT_LOAD_INST_DATA_COMPLETE;
    }

};

InstanceData* GetInstanceData_instance_shadow_labyrinth(Map* map)
{
    return new instance_shadow_labyrinth(map);
}

void AddSC_instance_shadow_labyrinth()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "instance_shadow_labyrinth";
    newscript->GetInstanceData = &GetInstanceData_instance_shadow_labyrinth;
    newscript->RegisterSelf();
}

