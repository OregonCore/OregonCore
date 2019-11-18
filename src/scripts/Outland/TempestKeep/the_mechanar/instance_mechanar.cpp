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
SDName: Instance_Mechanar
SD%Complete: 100
SDComment:
SDCategory: Mechanar
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "mechanar.h"

#define ENCOUNTERS      1

bool passagePhase1 = false;
bool passagePhase2 = false;
bool passagePhase3 = false;
bool passagePhase4 = false;
bool passagePhase5 = false;
bool passagePhase6 = false;
bool passagePhase7 = false;

struct instance_mechanar : public ScriptedInstance
{
    instance_mechanar(Map* map) : ScriptedInstance(map)
    {
        Initialize();
    };


    uint32 Encounters[ENCOUNTERS];

    void OnCreatureCreate (Creature* /*creature*/, uint32 /*creature_entry*/)
    {
    }

    void Initialize()
    {
        for (uint8 i = 0; i < ENCOUNTERS; ++i)
            Encounters[i] = NOT_STARTED;

        _pathaleonGUID = 0;
        _passageEncounter = 0;
        _passageTimer = 0;
        _moarg2Door = 0;
        _passageGUIDs.clear();
    }

    void OnGameObjectCreate(GameObject* pGo, bool /*add*/)
    {
        switch (pGo->GetEntry())
        {
        case GO_DOOR_MOARG_1:
        case GO_DOOR_MOARG_2:
            _moarg2Door = pGo->GetGUID();
        case GO_DOOR_NETHERMANCER:
            AddDoor(pGo, true);
            break;
        default:
            break;
        }
    }

    void OnGameObjectRemove(GameObject* gameObject)
    {
        switch (gameObject->GetEntry())
        {
        case GO_DOOR_MOARG_1:
        case GO_DOOR_MOARG_2:
        case GO_DOOR_NETHERMANCER:
            AddDoor(gameObject, false);
            break;
        default:
            break;
        }
    }

    void OnCreatureCreate(Creature* pCreature, bool /*add*/)
    {
        if (pCreature->GetEntry() == NPC_PATHALEON_THE_CALCULATOR)
            _pathaleonGUID = pCreature->GetGUID();
    }

    void OnCreatureDeath(Creature* pCreature)
    {
            if (_passageEncounter > ENCOUNTER_PASSAGE_NOT_STARTED && _passageEncounter < ENCOUNTER_PASSAGE_DONE)
                if (_passageGUIDs.find(pCreature->GetGUID()) != _passageGUIDs.end())
                    _passageGUIDs.erase(pCreature->GetGUID());

            if (pCreature->GetEntry() == 19219)
                if (GameObject* pGo = instance->GetGameObject(_moarg2Door))
                    pGo->SetGoState(GO_STATE_ACTIVE);

    }

    Player* GetPassagePlayer(float x)
    {
        Map::PlayerList const& pl = instance->GetPlayers();
        for (Map::PlayerList::const_iterator itr = pl.begin(); itr != pl.end(); ++itr)
            if (Player* player = itr->GetSource())
                if (player->GetPositionX() < x && player->GetPositionZ() > 24.0f && player->GetPositionY() > -30.0f)
                    return player;
        return NULL;
    }

    void DoSummonAction(Creature* summon, Player* player)
    {
        summon->CastSpell(summon, SPELL_TELEPORT_VISUAL, true);
        summon->AI()->AttackStart(player);
        _passageGUIDs.insert(summon->GetGUID());
    }

    void Update(uint32 diff)
    {
        if (_passageEncounter == ENCOUNTER_PASSAGE_DONE)
            return;

        _passageTimer += diff;
        if (_passageTimer >= 1000)
        {
            _passageTimer = 0;
            if (_passageEncounter == ENCOUNTER_PASSAGE_NOT_STARTED)
            {
                if (Player* player = GetPassagePlayer(250.0f))
                {
                    _passageEncounter++;
                    for (uint8 i = 0; i < 4; ++i)
                    {
                        Position pos = { 238.0f, -27.0f + 3.0f*i, 26.328f, 0.0f };
                        if (Creature* creature = instance->SummonCreature(i == 1 || i == 2 ? NPC_SUNSEEKER_ASTROMAGE : NPC_BLOODWARDER_CENTURION, pos))
                            DoSummonAction(creature, player);
                    }
                }
            }

            if (!_passageGUIDs.empty())
                return;

            if (_passageEncounter < ENCOUNTER_PASSAGE_PHASE3)
            {
                if (Player* player = GetPassagePlayer(250.0f))
                {
                    if (_passageEncounter == ENCOUNTER_PASSAGE_PHASE1)
                    {
                        Position pos = { 214.37f, -23.5f, 24.88f, 0.0f };
                        if (Creature* creature = instance->SummonCreature(NPC_TEMPEST_KEEPER_DESTROYER, pos))
                            DoSummonAction(creature, player);
                    }
                    else if (_passageEncounter == ENCOUNTER_PASSAGE_PHASE2)
                    {
                        for (uint8 i = 0; i < 3; ++i)
                        {
                            Position pos = { 199.76f, -26.0f + 2.5f*i, 24.88f, 0.0f };
                            if (Creature* creature = instance->SummonCreature(i == 1 ? NPC_SUNSEEKER_ENGINEER : NPC_BLOODWARDER_PHYSICIAN, pos))
                                DoSummonAction(creature, player);
                        }
                    }
                    _passageEncounter++;
                }
            }
            else
            {
                if (Player* player = GetPassagePlayer(148.0f))
                {
                    if (_passageEncounter == ENCOUNTER_PASSAGE_PHASE3)
                    {
                        for (uint8 i = 0; i < 3; ++i)
                        {
                            Position pos = { 135.0f + 2.5f*i, 36.76f, 24.88f, M_PI*1.5f };
                            if (Creature* creature = instance->SummonCreature(i == 1 ? NPC_SUNSEEKER_ASTROMAGE : NPC_BLOODWARDER_PHYSICIAN, pos))
                                DoSummonAction(creature, player);
                        }
                    }
                    else if (_passageEncounter == ENCOUNTER_PASSAGE_PHASE4)
                    {
                        Position pos = { 137.62f, 62.23f, 24.88f, M_PI*1.5f };
                        if (Creature* creature = instance->SummonCreature(NPC_TEMPEST_KEEPER_DESTROYER, pos))
                            DoSummonAction(creature, player);
                    }
                    else if (_passageEncounter == ENCOUNTER_PASSAGE_PHASE5)
                    {
                        for (uint8 i = 0; i < 4; ++i)
                        {
                            Position pos = { 133.0f + 3.5f*i, 92.88f, 26.38f, M_PI*1.5f };
                            if (Creature* creature = instance->SummonCreature(i == 1 || i == 2 ? NPC_SUNSEEKER_ASTROMAGE : NPC_SUNSEEKER_ENGINEER, pos))
                                DoSummonAction(creature, player);
                        }
                    }
                    else if (_passageEncounter == ENCOUNTER_PASSAGE_PHASE6)
                    {
                        if (Creature* creature = instance->GetCreature(_pathaleonGUID))
                            creature->AI()->DoAction(1);
                    }
                    _passageEncounter++;
                }
            }
        }
    }

    bool SetBossState(uint32 type, EncounterState state)
    {
        if (!ScriptedInstance::SetBossState(type, state))
            return false;

        return true;
    }

    std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << "M E " << GetBossSaveData() << _passageEncounter;

        OUT_SAVE_INST_DATA_COMPLETE;
        return saveStream.str();
    }

    void Load(const char* str)
    {
        if (!str)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(str);

        char dataHead1, dataHead2;

        std::istringstream loadStream(str);
        loadStream >> dataHead1 >> dataHead2;

        if (dataHead1 == 'M' && dataHead2 == 'E')
        {
            for (uint32 i = 0; i < MAX_ENCOUNTER; ++i)
            {
                uint32 tmpState;
                loadStream >> tmpState;
                if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                    tmpState = NOT_STARTED;
                SetBossState(i, EncounterState(tmpState));
            }

            loadStream >> _passageEncounter;
            if (_passageEncounter == ENCOUNTER_PASSAGE_DONE)
                _passageEncounter = ENCOUNTER_PASSAGE_PHASE6;
        }
        else
            OUT_LOAD_INST_DATA_FAIL;

        OUT_LOAD_INST_DATA_COMPLETE;
    } 

private:
    uint64 _moarg2Door;
    uint64 _pathaleonGUID;
    uint32 _passageTimer;
    uint32 _passageEncounter;
    std::set<uint64> _passageGUIDs;
};

InstanceData* GetInstanceData_instance_mechanar(Map* map)
{
    return new instance_mechanar(map);
}

void AddSC_instance_mechanar()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "instance_mechanar";
    newscript->GetInstanceData = &GetInstanceData_instance_mechanar;
    newscript->RegisterSelf();
}

