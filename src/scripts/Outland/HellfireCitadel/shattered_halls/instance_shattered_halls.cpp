/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * Copyright (C) 2010-2011 OregonCore <http://www.oregoncore.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Instance_Shattered_Halls
SD%Complete: 95
SDComment:
SDCategory: Hellfire Citadel, Shattered Halls
EndScriptData */

#include "ScriptPCH.h"
#include "shattered_halls.h"

#define ENCOUNTERS           4

#define DOOR_NETHEKURSE1     182539
#define DOOR_NETHEKURSE2     182540

struct instance_shattered_halls : public ScriptedInstance
{
    instance_shattered_halls(Map *map) : ScriptedInstance(map) {Initialize();};

    uint32 Encounter[ENCOUNTERS];
    std::string str_data;
    uint64 nethekurseGUID;
    uint64 warbringerGUID;
    uint64 nethekurseDoor1GUID;
    uint64 nethekurseDoor2GUID;

    void Initialize()
    {
        nethekurseGUID = 0;
        warbringerGUID = 0;
        nethekurseDoor1GUID = 0;
        nethekurseDoor2GUID = 0;

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
        switch(pGo->GetEntry())
        {
            case DOOR_NETHEKURSE1: nethekurseDoor1GUID = pGo->GetGUID(); break;
            case DOOR_NETHEKURSE2: nethekurseDoor2GUID = pGo->GetGUID(); break;
        }
    }

    void OnCreatureCreate(Creature* pCreature, bool /*add*/)
    {
        switch(pCreature->GetEntry())
        {
            case 16807: nethekurseGUID = pCreature->GetGUID(); break;
            case 16809: warbringerGUID = pCreature->GetGUID(); break;
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

        debug_log("OSCR: Instance Shattered Halls: GetPlayerInMap, but PlayerList is empty!");
        return NULL;
    }

    void SetData(uint32 type, uint32 data)
    {
        switch(type)
        {
            case TYPE_NETHEKURSE:
                if (data == FAIL)
                {
                    HandleGameObject(nethekurseDoor1GUID, true);
                }
                if (data == IN_PROGRESS)
                {
                    HandleGameObject(nethekurseDoor1GUID, false);
                    HandleGameObject(nethekurseDoor2GUID, false);
                }
                if (data == DONE)
                {
                    HandleGameObject(nethekurseDoor1GUID, true);
                    HandleGameObject(nethekurseDoor2GUID, true);
                }
                if (Encounter[0] != DONE)
                    Encounter[0] = data;
                break;
            case DATA_PORUNG:
                if (Encounter[1] != DONE)
                    Encounter[1] = data;
                break;
            case TYPE_WARBRINGER:
                if (Encounter[2] != DONE)
                    Encounter[2] = data;
                break;
            case DATA_KARGATH:
                if (Encounter[3] != DONE)
                    Encounter[3] = data;
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
            case TYPE_NETHEKURSE:
                return Encounter[0];
            case DATA_PORUNG:
                return Encounter[1];
            case TYPE_WARBRINGER:
                return Encounter[2];
            case DATA_KARGATH:
                return Encounter[3];
        }
        return false;
    }

    uint64 GetData64(uint32 data)
    {
        switch(data)
        {
            case DATA_NETHEKURSE:
                return nethekurseGUID;
            case DATA_WARBRINGER:
                return warbringerGUID;
        }
        return 0;
    }

    std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;
        std::ostringstream saveStream;

        saveStream << Encounter[0] << " " << Encounter[1] << " " << Encounter[2] << " " << Encounter[3];

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
        loadStream >> Encounter[0] >> Encounter[1] >> Encounter[2] >> Encounter[3];

        for (uint8 i = 0; i < ENCOUNTERS; ++i)
            if (Encounter[i] == IN_PROGRESS)
                Encounter[i] = NOT_STARTED;

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_shattered_halls(Map* map)
{
    return new instance_shattered_halls(map);
}

void AddSC_instance_shattered_halls()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_shattered_halls";
    newscript->GetInstanceData = &GetInstanceData_instance_shattered_halls;
    newscript->RegisterSelf();
}

