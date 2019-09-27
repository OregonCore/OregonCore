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
SDName: Instance_Arcatraz
SD%Complete: 80
SDComment: Mainly Harbringer Skyriss event
SDCategory: Tempest Keep, The Arcatraz
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "arcatraz.h"

/* Arcatraz encounters:
1 - Zereketh the Unbound event
2 - Dalliah the Doomsayer event
3 - Wrath-Scryer Soccothrates event
4 - Harbinger Skyriss event, 5 sub-events
*/

struct instance_arcatraz : public ScriptedInstance
{
    instance_arcatraz(Map* map) : ScriptedInstance(map)
    {
        Initialize();
    };

    uint32 Encounter[EncounterCount];

    GameObject* Containment_Core_Security_Field_Alpha;
    GameObject* Containment_Core_Security_Field_Beta;
    GameObject* Pod_Alpha;
    GameObject* Pod_Gamma;
    GameObject* Pod_Beta;
    GameObject* Pod_Delta;
    GameObject* Pod_Omega;
    GameObject* Wardens_Shield;

    uint64 GoSphereGUID;
    uint64 MellicharGUID;

    void Initialize()
    {
        Containment_Core_Security_Field_Alpha = NULL;
        Containment_Core_Security_Field_Beta  = NULL;
        Pod_Alpha = NULL;
        Pod_Beta  = NULL;
        Pod_Delta = NULL;
        Pod_Gamma = NULL;
        Pod_Omega = NULL;
        Wardens_Shield = NULL;

        GoSphereGUID = 0;
        MellicharGUID = 0;

        for (uint8 i = 0; i < EncounterCount; i++)
            Encounter[i] = NOT_STARTED;
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < EncounterCount; i++)
            if (Encounter[i] == IN_PROGRESS)
                return true;

        return false;
    }

    void OnGameObjectCreate(GameObject* pGo, bool /*add*/)
    {
        switch (pGo->GetEntry())
        {
        case CONTAINMENT_CORE_SECURITY_FIELD_ALPHA:
            Containment_Core_Security_Field_Alpha = pGo;
            break;
        case CONTAINMENT_CORE_SECURITY_FIELD_BETA:
            Containment_Core_Security_Field_Beta =  pGo;
            break;
        case POD_ALPHA:
            Pod_Alpha = pGo;
            break;
        case POD_BETA:
            Pod_Beta =  pGo;
            break;
        case POD_DELTA:
            Pod_Delta = pGo;
            break;
        case POD_GAMMA:
            Pod_Gamma = pGo;
            break;
        case POD_OMEGA:
            Pod_Omega = pGo;
            break;
        case SEAL_SPHERE:
            GoSphereGUID = pGo->GetGUID();
            break;
            //case WARDENS_SHIELD: Wardens_Shield = pGo; break;
        }
    }

    void OnCreatureCreate(Creature* pCreature, bool /*add*/)
    {
        if (pCreature->GetEntry() == NPC_MELLICHAR)
            MellicharGUID = pCreature->GetGUID();
    }

    void SetData(uint32 type, uint32 data)
    {
        switch (type)
        {
        case DATA_ZEREKETH:
            Encounter[0] = data;
            break;

        case DATA_DALLIAH:
            if (data == DONE)
                if (Containment_Core_Security_Field_Beta)
                    Containment_Core_Security_Field_Beta->UseDoorOrButton();
            Encounter[1] = data;
            break;

        case DATA_SOCCOTHRATES:
            if (data == DONE)
                if (Containment_Core_Security_Field_Alpha)
                    Containment_Core_Security_Field_Alpha->UseDoorOrButton();
            Encounter[2] = data;
            break;

        case DATA_HARBINGERSKYRISS:
            if (data == NOT_STARTED || data == FAIL)
            {
                Encounter[4] = NOT_STARTED;
                Encounter[5] = NOT_STARTED;
                Encounter[6] = NOT_STARTED;
                Encounter[7] = NOT_STARTED;
                Encounter[8] = NOT_STARTED;
            }
            Encounter[3] = data;
            break;

        case DATA_WARDEN_1:
            if (data == IN_PROGRESS)
                if (Pod_Alpha)
                    Pod_Alpha->UseDoorOrButton();
            Encounter[4] = data;
            break;

        case DATA_WARDEN_2:
            if (data == IN_PROGRESS)
                if (Pod_Beta)
                    Pod_Beta->UseDoorOrButton();
            Encounter[5] = data;
            break;

        case DATA_WARDEN_3:
            if (data == IN_PROGRESS)
                if (Pod_Delta)
                    Pod_Delta->UseDoorOrButton();
            Encounter[6] = data;
            break;

        case DATA_WARDEN_4:
            if (data == IN_PROGRESS)
                if (Pod_Gamma)
                    Pod_Gamma->UseDoorOrButton();
            Encounter[7] = data;
            break;

        case DATA_WARDEN_5:
            if (data == IN_PROGRESS)
                if (Pod_Omega)
                    Pod_Omega->UseDoorOrButton();
            Encounter[8] = data;
            break;

        case DATA_SHIELD_OPEN:
            if (data == IN_PROGRESS)
                if (Wardens_Shield)
                    Wardens_Shield->UseDoorOrButton();
            break;
        case DATA_CONVERSATION:
            Encounter[12] = data;
            break;
        }
    }

    uint32 GetData(uint32 type)
    {
        switch (type)
        {
            case DATA_HARBINGERSKYRISS: return Encounter[3];
            case DATA_WARDEN_1:         return Encounter[4];
            case DATA_WARDEN_2:         return Encounter[5];
            case DATA_WARDEN_3:         return Encounter[6];
            case DATA_WARDEN_4:         return Encounter[7];
            case DATA_WARDEN_5:         return Encounter[8];
            case DATA_CONVERSATION:     return Encounter[12];
        }
        return 0;
    }

    uint64 GetData64(uint32 data)
    {
        switch (data)
        {
        case DATA_MELLICHAR:
            return MellicharGUID;
        case DATA_SPHERE_SHIELD:
            return GoSphereGUID;
        }
        return 0;
    }
};

InstanceData* GetInstanceData_instance_arcatraz(Map* map)
{
    return new instance_arcatraz(map);
}

void AddSC_instance_arcatraz()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "instance_arcatraz";
    newscript->GetInstanceData = &GetInstanceData_instance_arcatraz;
    newscript->RegisterSelf();
}

