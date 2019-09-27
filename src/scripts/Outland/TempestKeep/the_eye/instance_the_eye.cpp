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
SDName: Instance_The_Eye
SD%Complete: 100
SDComment:
SDCategory: Tempest Keep, The Eye
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "the_eye.h"

#define ENCOUNTERS 5

/* The Eye encounters:
0 - Kael'thas event
1 - Al' ar event
2 - Solarian Event
3 - Void Reaver event
*/

struct instance_the_eye : public ScriptedInstance
{
    instance_the_eye(Map* map) : ScriptedInstance(map)
    {
        Initialize();
    };

    uint64 ThaladredTheDarkener;
    uint64 LordSanguinar;
    uint64 GrandAstromancerCapernian;
    uint64 MasterEngineerTelonicus;
    uint64 Kaelthas;
    uint64 Astromancer;
    uint64 Alar;

    uint8 KaelthasEventPhase;
    uint8 AlarEventPhase;

    uint32 Encounters[ENCOUNTERS];

    void Initialize()
    {
        ThaladredTheDarkener = 0;
        LordSanguinar = 0;
        GrandAstromancerCapernian = 0;
        MasterEngineerTelonicus = 0;
        Kaelthas = 0;
        Astromancer = 0;
        Alar = 0;

        KaelthasEventPhase = 0;
        AlarEventPhase = 0;

        for (uint8 i = 0; i < ENCOUNTERS; i++)
            Encounters[i] = NOT_STARTED;
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < ENCOUNTERS; i++)
            if (Encounters[i] == IN_PROGRESS) return true;

        return false;
    }

    void OnCreatureCreate(Creature* pCreature, bool /*add*/)
    {
        switch (pCreature->GetEntry())
        {
        case 20064:
            ThaladredTheDarkener = pCreature->GetGUID();
            break;
        case 20063:
            MasterEngineerTelonicus = pCreature->GetGUID();
            break;
        case 20062:
            GrandAstromancerCapernian = pCreature->GetGUID();
            break;
        case 20060:
            LordSanguinar = pCreature->GetGUID();
            break;
        case 19622:
            Kaelthas = pCreature->GetGUID();
            break;
        case 18805:
            Astromancer = pCreature->GetGUID();
            break;
        case 19514:
            Alar = pCreature->GetGUID();
            break;
        }
    }

    uint64 GetData64(uint32 identifier)
    {
        switch (identifier)
        {
            case DATA_THALADREDTHEDARKENER:         return ThaladredTheDarkener;
            case DATA_LORDSANGUINAR:                return LordSanguinar;
            case DATA_GRANDASTROMANCERCAPERNIAN:    return GrandAstromancerCapernian;
            case DATA_MASTERENGINEERTELONICUS:      return MasterEngineerTelonicus;
            case DATA_KAELTHAS:                     return Kaelthas;
            case DATA_ASTROMANCER:                  return Astromancer;
            case DATA_ALAR:                         return Alar;
        }
        return 0;
    }

    void SetData(uint32 type, uint32 data)
    {
        switch (type)
        {
        case DATA_ALAREVENT:
            AlarEventPhase = data;
            if (Encounters[0] != DONE)
                Encounters[0] = data;
            break;
        case DATA_HIGHASTROMANCERSOLARIANEVENT:
            if (Encounters[1] != DONE)
                Encounters[1] = data;
            break;
        case DATA_VOIDREAVEREVENT:
            if (Encounters[2] != DONE)
                Encounters[2] = data;
            break;
        case DATA_KAELTHASEVENT:
            KaelthasEventPhase = data;
            if (Encounters[3] != DONE)
                Encounters[3] = data;
            break;
        }
        if (data == DONE)
            SaveToDB();
    }

    uint32 GetData(uint32 type)
    {
        switch (type)
        {
            case DATA_ALAREVENT:                        return AlarEventPhase;
            case DATA_HIGHASTROMANCERSOLARIANEVENT:     return Encounters[1];
            case DATA_VOIDREAVEREVENT:                  return Encounters[2];
            case DATA_KAELTHASEVENT:                    return KaelthasEventPhase;
        }
        return 0;
    }

    std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;
        std::ostringstream stream;
        stream << Encounters[0] << " " << Encounters[1] << " " << Encounters[2] << " " << Encounters[3];
        char* out = new char[stream.str().length() + 1];
        strcpy(out, stream.str().c_str());
        if (out)
        {
            OUT_SAVE_INST_DATA_COMPLETE;
            return out;
        }
        return NULL;
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
        stream >> Encounters[0] >> Encounters[1] >> Encounters[2] >> Encounters[3];
        for (uint8 i = 0; i < ENCOUNTERS; ++i)
            if (Encounters[i] == IN_PROGRESS)                // Do not load an encounter as "In Progress" - reset it instead.
                Encounters[i] = NOT_STARTED;
        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_the_eye(Map* map)
{
    return new instance_the_eye(map);
}

void AddSC_instance_the_eye()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "instance_the_eye";
    newscript->GetInstanceData = &GetInstanceData_instance_the_eye;
    newscript->RegisterSelf();
}

