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
        _passageGUIDs.clear();
    }

    void OnGameObjectCreate(GameObject* gameObject)
    {
        switch (gameObject->GetEntry())
        {
        case GO_DOOR_MOARG_1:
        case GO_DOOR_MOARG_2:
        case GO_DOOR_NETHERMANCER:
            AddDoor(gameObject, true);
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

    void OnCreatureCreate(Creature* creature)
    {
        if (creature->GetEntry() == NPC_PATHALEON_THE_CALCULATOR)
            _pathaleonGUID = creature->GetGUID();
    }

    void OnUnitDeath(Unit* unit)
    {
        if (unit->GetTypeId() == TYPEID_UNIT)
            if (_passageEncounter > ENCOUNTER_PASSAGE_NOT_STARTED && _passageEncounter < ENCOUNTER_PASSAGE_DONE)
                if (_passageGUIDs.find(unit->GetGUID()) != _passageGUIDs.end())
                    _passageGUIDs.erase(unit->GetGUID());

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
            if (_passageEncounter == ENCOUNTER_PASSAGE_PHASE1)
            {
                if (!passagePhase1)
                {
                    if (Player* player = GetPassagePlayer(250.0f))
                    {
                        Position pos = { 190.92f, -23.5f, 24.88f, 0.0f };
                        if (Creature* creature = instance->SummonCreature(NPC_TEMPEST_KEEPER_DESTROYER, pos))
                            DoSummonAction(creature, player);
                        
                    }
                    _passageEncounter++;
                    passagePhase1 = true;
                }
            }
            if (_passageEncounter == ENCOUNTER_PASSAGE_PHASE2)
            {
                if (!passagePhase2)
                {
                    if (Player* player = GetPassagePlayer(250.0f))
                    {
                        for (uint8 i = 0; i < 3; ++i)
                        {
                            Position pos = { 146.61f, 3.03f + 2.5f*i, 24.88f, 5.30f };
                            if (Creature* creature = instance->SummonCreature(i == 1 ? NPC_SUNSEEKER_ENGINEER : NPC_BLOODWARDER_PHYSICIAN, pos))
                                DoSummonAction(creature, player);
                           
                        }
                        _passageEncounter++;
                        passagePhase2 = true;
                    }
                }
            }
            if (_passageEncounter == ENCOUNTER_PASSAGE_PHASE3)
            {
                if (!passagePhase3)
                {
                    if (Player* player = GetPassagePlayer(148.0f))
                    {
                        for (uint8 i = 0; i < 3; ++i)
                        {
                            Position pos = { 167.97f, -16.0f + 2.5f*i , 24.88f, 4.80f };
                            if (Creature* creature = instance->SummonCreature(i == 1 ? NPC_SUNSEEKER_ASTROMAGE : NPC_BLOODWARDER_PHYSICIAN, pos))
                                DoSummonAction(creature, player);
                        }
                        _passageEncounter++;
                        passagePhase3 = true;
                    }
                }
            }
            if (_passageEncounter == ENCOUNTER_PASSAGE_PHASE4)
            {
                if (!passagePhase4)
                {
                    if (Player* player = GetPassagePlayer(148.0f))
                    {
                        Position pos = { 137.65f, 29.9f + 2.5f , 24.88f, 4.70f };
                        if (Creature* creature = instance->SummonCreature(NPC_TEMPEST_KEEPER_DESTROYER, pos))
                            DoSummonAction(creature, player);

                    }
                    _passageEncounter++;
                    passagePhase4 = true;
                }
            }
            if (_passageEncounter == ENCOUNTER_PASSAGE_PHASE5)
            {
                if (!passagePhase5)
                {
                    if (Player* player = GetPassagePlayer(148.0f))
                    {
                        for (uint8 i = 0; i < 4; ++i)
                        {
                            Position pos = { 133.0f + 3.5f*i, 92.88f, 26.38f, 4.6f };
                            if (Creature* creature = instance->SummonCreature(i == 1 || i == 2 ? NPC_SUNSEEKER_ASTROMAGE : NPC_SUNSEEKER_ENGINEER, pos))
                                DoSummonAction(creature, player);

                        }
                        _passageEncounter++;
                        passagePhase5 = true;
                    }
                }
            }
            if (_passageEncounter == ENCOUNTER_PASSAGE_PHASE6)
            {
                if (!passagePhase6)
                {
                    if(Creature* creature = instance->GetCreature(_pathaleonGUID))
                        creature->AI()->DoAction(1);
                }
            }
            if (!_passageGUIDs.empty())
                return;
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

