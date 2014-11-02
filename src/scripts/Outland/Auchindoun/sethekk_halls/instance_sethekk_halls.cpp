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
SDName: Instance - Sethekk Halls
SD%Complete: 95
SDComment: Instance Data for Sethekk Halls instance
SDCategory: Auchindoun, Sethekk Halls
EndScriptData */

#include "ScriptPCH.h"
#include "sethekk_halls.h"
#include "ScriptedEscortAI.h"

#define IKISS_DOOR       177203

#define NPC_LAKKA        18956

#define QUEST_BROTHER    10097

#define ENCOUNTERS       3

struct Location
{
    float fLocX;
    float fLocY;
    float fLocZ;
    float fOrient;
};

static Location SpawnLocation[] =
{
    { -158.226898f, 158.690933f, 0.009380f, 1.212f}
};

struct instance_sethekk_halls : public ScriptedInstance
{
    instance_sethekk_halls(Map* map) : ScriptedInstance(map)
    {
        Initialize();
    };

    uint64 IkissDoorGUID;
    uint32 Encounter[ENCOUNTERS];
    std::string str_data;

    bool SummonLakka;

    void Initialize()
    {
        IkissDoorGUID = 0;
        SummonLakka = false;

        for (uint8 i = 0; i < ENCOUNTERS; i++)
            Encounter[i] = NOT_STARTED;
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < ENCOUNTERS; i++)
            if (Encounter[i] == IN_PROGRESS) return true;

        return false;
    }

    void OnPlayerEnter(Player* pPlayer)
    {
        if ((CAST_PLR(pPlayer)->GetQuestStatus(QUEST_BROTHER) == QUEST_STATUS_INCOMPLETE) && !SummonLakka)
        {
            pPlayer->SummonCreature(NPC_LAKKA, SpawnLocation[0].fLocX, SpawnLocation[0].fLocY, SpawnLocation[0].fLocZ, SpawnLocation[0].fOrient, TEMPSUMMON_DEAD_DESPAWN, 0);
            SummonLakka = true;
        }
    }

    void OnGameObjectCreate(GameObject* pGo, bool /*add*/)
    {
        switch (pGo->GetEntry())
        {
        case IKISS_DOOR:
            IkissDoorGUID = pGo->GetGUID();
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

        debug_log("OSCR: Instance Sethekk Halls: GetPlayerInMap, but PlayerList is empty!");
        return NULL;
    }

    void SetData(uint32 type, uint32 data)
    {
        switch (type)
        {
        case DATA_SYTHEVENT:
            if (Encounter[0] != DONE)
                Encounter[0] = data;
            break;
        case DATA_ANZUEVENT:
            if (Encounter[1] != DONE)
                Encounter[1] = data;
            break;
        case DATA_IKISSEVENT:
            if (data == DONE)
                HandleGameObject(IkissDoorGUID, true);
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
        switch (type)
        {
        case DATA_SYTHEVENT:
            return Encounter[0];
        case DATA_ANZUEVENT:
            return Encounter[1];
        case DATA_IKISSEVENT:
            return Encounter[2];
        }
        return false;
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
};

InstanceData* GetInstanceData_instance_sethekk_halls(Map* map)
{
    return new instance_sethekk_halls(map);
}

/*#####
## npc_lakka
#####*/

// this is not escort quest !
struct npc_lakkaAI : public npc_escortAI
{
    npc_lakkaAI(Creature* pCreature) : npc_escortAI(pCreature) {}

    void Reset() { }

    void WaypointReached(uint32 uiPointId)
    {
        switch (uiPointId)
        {
        case 0:
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            SetRun(false);
            break;
        case 8:
            me->ForcedDespawn();
        }
    }
};

CreatureAI* GetAI_npc_lakka(Creature* pCreature)
{
    return new npc_lakkaAI(pCreature);
}

/*#####
## go_lakka_cage
#####*/

bool GOHello_go_lakka_cage(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_BROTHER) == QUEST_STATUS_INCOMPLETE)
    {
        if (Creature* pLakka = pGo->FindNearestCreature( NPC_LAKKA, 5, true))
            ((npc_lakkaAI*)pLakka->AI())->Start(false, false, pPlayer->GetGUID());
    }

    return false;
};

void AddSC_instance_sethekk_halls()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "instance_sethekk_halls";
    newscript->GetInstanceData = &GetInstanceData_instance_sethekk_halls;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_lakka_cage";
    newscript->pGOHello =  &GOHello_go_lakka_cage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_lakka";
    newscript->GetAI = &GetAI_npc_lakka;
    newscript->RegisterSelf();
}

