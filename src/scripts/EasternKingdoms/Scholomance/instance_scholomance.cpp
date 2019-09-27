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
SDName: Instance_Scholomance
SD%Complete: 100
SDComment:
SDCategory: Scholomance
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "scholomance.h"

#define GO_GATE_KIRTONOS    175570
#define GO_GATE_GANDLING    177374
#define GO_GATE_MALICIA     177375
#define GO_GATE_THEOLEN     177377
#define GO_GATE_POLKELT     177376
#define GO_GATE_RAVENIAN    177372
#define GO_GATE_BAROV       177373
#define GO_GATE_ILLUCIA     177371

#define MAX_ENCOUNTER          2

struct instance_scholomance : public ScriptedInstance
{
    instance_scholomance(Map* pMap) : ScriptedInstance(pMap)
    {
        Initialize();
    };

    //Lord Alexei Barov, Doctor Theolen Krastinov, The Ravenian, Lorekeeper Polkelt, Instructor Malicia and the Lady Illucia Barov.
    bool IsBossDied[6];
    uint32 m_auiEncounter[MAX_ENCOUNTER];

    uint64 GateKirtonosGUID;
    uint64 GateGandlingGUID;
    uint64 GateMiliciaGUID;
    uint64 GateTheolenGUID;
    uint64 GatePolkeltGUID;
    uint64 GateRavenianGUID;
    uint64 GateBarovGUID;
    uint64 GateIlluciaGUID;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

        GateKirtonosGUID = 0;
        GateGandlingGUID = 0;
        GateMiliciaGUID = 0;
        GateTheolenGUID = 0;
        GatePolkeltGUID = 0;
        GateRavenianGUID = 0;
        GateBarovGUID = 0;
        GateIlluciaGUID = 0;

        for (uint8 i = 0; i < 6; ++i)
            IsBossDied[i] = false;
    }

    void OnGameObjectCreate(GameObject* pGo, bool /*add*/)
    {
        switch (pGo->GetEntry())
        {
        case GO_GATE_KIRTONOS:
            GateKirtonosGUID = pGo->GetGUID();
            break;
        case GO_GATE_GANDLING:
            GateGandlingGUID = pGo->GetGUID();
            break;
        case GO_GATE_MALICIA:
            GateMiliciaGUID = pGo->GetGUID();
            break;
        case GO_GATE_THEOLEN:
            GateTheolenGUID = pGo->GetGUID();
            break;
        case GO_GATE_POLKELT:
            GatePolkeltGUID = pGo->GetGUID();
            break;
        case GO_GATE_RAVENIAN:
            GateRavenianGUID = pGo->GetGUID();
            break;
        case GO_GATE_BAROV:
            GateBarovGUID = pGo->GetGUID();
            break;
        case GO_GATE_ILLUCIA:
            GateIlluciaGUID = pGo->GetGUID();
            break;
        }
    }

    void SetData(uint32 type, uint32 data)
    {
        switch (type)
        {
        case DATA_LORDALEXEIBAROV_DEATH:
            IsBossDied[0] = true;
            break;
        case DATA_DOCTORTHEOLENKRASTINOV_DEATH:
            IsBossDied[1] = true;
            break;
        case DATA_THERAVENIAN_DEATH:
            IsBossDied[2] = true;
            break;
        case DATA_LOREKEEPERPOLKELT_DEATH:
            IsBossDied[3] = true;
            break;
        case DATA_INSTRUCTORMALICIA_DEATH:
            IsBossDied[4] = true;
            break;
        case DATA_LADYILLUCIABAROV_DEATH:
            IsBossDied[5] = true;
            break;
        case TYPE_GANDLING:
            m_auiEncounter[0] = data;
            break;
        case TYPE_KIRTONOS:
            m_auiEncounter[1] = data;
            break;
        }
    }

    uint32 GetData(uint32 type)
    {
        if (type == TYPE_GANDLING)
        {
            if (IsBossDied[0] && IsBossDied[1] && IsBossDied[2] && IsBossDied[3] && IsBossDied[4] && IsBossDied[5])
            {
                m_auiEncounter[0] = IN_PROGRESS;
                return IN_PROGRESS;
            }
        }

        return 0;
    }
};

InstanceData* GetInstanceData_instance_scholomance(Map* pMap)
{
    return new instance_scholomance(pMap);
}

void AddSC_instance_scholomance()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "instance_scholomance";
    newscript->GetInstanceData = &GetInstanceData_instance_scholomance;
    newscript->RegisterSelf();
}

