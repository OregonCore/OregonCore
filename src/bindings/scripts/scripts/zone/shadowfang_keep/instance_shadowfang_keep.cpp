/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Instance_Shadowfang_Keep
SD%Complete: 100
SDComment: Complete, Doors checked
SDCategory: Shadowfang Keep
EndScriptData */

#include "precompiled.h"
#include "def_shadowfang_keep.h"

#define ENCOUNTERS              4

struct OREGON_DLL_DECL instance_shadowfang_keep : public ScriptedInstance
{
    instance_shadowfang_keep(Map *map) : ScriptedInstance(map) {Initialize();};

    uint32 Encounters[ENCOUNTERS];
    std::string str_data;

    uint64 DoorCourtyardGUID;
    uint64 DoorSorcererGUID;
    uint64 DoorArugalGUID;

    void Initialize()
    {
        DoorCourtyardGUID = 0;
        DoorSorcererGUID = 0;
        DoorArugalGUID = 0;

         for (uint8 i=0; i < ENCOUNTERS; ++i)
             Encounters[i] = NOT_STARTED;
    }

    void OnObjectCreate(GameObject *go)
    {
        switch(go->GetEntry())
        {
        case 18895: DoorCourtyardGUID = go->GetGUID();
            if (Encounters[0] == DONE) HandleGameObject(NULL,true,go); break;
        case 18972: DoorSorcererGUID = go->GetGUID();
            if (Encounters[2] == DONE) HandleGameObject(NULL,true,go); break;
        case 18971: DoorArugalGUID = go->GetGUID();
            if (Encounters[3] == DONE) HandleGameObject(NULL,true,go); break;

        }
    }

    void SetData(uint32 type, uint32 data)
    {
        switch(type)
        {
            case TYPE_FREE_NPC:
                if (data == DONE)
                    HandleGameObject(DoorCourtyardGUID,true);
                Encounters[0] = data;
                break;
            case TYPE_RETHILGORE:
                Encounters[1] = data;
                break;
            case TYPE_FENRUS:
                if (data == DONE)
                    HandleGameObject(DoorSorcererGUID,true);
                Encounters[2] = data;
                break;
            case TYPE_NANDOS:
                if (data == DONE)
                    HandleGameObject(DoorArugalGUID,true);
                Encounters[3] = data;
                break;
        }

        if (data == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << Encounters[0] << " " << Encounters[1] << " " << Encounters[2] << " " << Encounters[3];

            str_data = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint32 GetData(uint32 type)
    {
        switch(type)
        {
            case TYPE_FREE_NPC:
                return Encounters[0];
            case TYPE_RETHILGORE:
                return Encounters[1];
            case TYPE_FENRUS:
                return Encounters[2];
            case TYPE_NANDOS:
                return Encounters[3];
        }
        return 0;
    }

    const char* Save()
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
        loadStream >> Encounters[0] >> Encounters[1] >> Encounters[2] >> Encounters[3];

        for (uint8 i = 0; i < ENCOUNTERS; ++i)
        {
            if (Encounters[i] == IN_PROGRESS)
                Encounters[i] = NOT_STARTED;

        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_shadowfang_keep(Map* map)
{
    return new instance_shadowfang_keep(map);
}

void AddSC_instance_shadowfang_keep()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_shadowfang_keep";
    newscript->GetInstanceData = &GetInstanceData_instance_shadowfang_keep;
    newscript->RegisterSelf();
}

