/*
 * Copyright (C) 2010-2012 OregonCore <http://www.oregoncore.com/>
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2012 ScriptDev2 <http://www.scriptdev2.com/>
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

enum
{
    NPC_EXECUTIONER                = 17301,

    NPC_SOLDIER_ALLIANCE_1         = 17288,
    NPC_SOLDIER_ALLIANCE_2         = 17289,
    NPC_SOLDIER_ALLIANCE_3         = 17292,
    NPC_OFFICER_ALLIANCE           = 17290,

    NPC_SOLDIER_HORDE_1            = 17294,
    NPC_SOLDIER_HORDE_2            = 17295,
    NPC_SOLDIER_HORDE_3            = 17297,
    NPC_OFFICER_HORDE              = 17296,

    SPELL_KARGATH_EXECUTIONER_1    = 39288,
    SPELL_KARGATH_EXECUTIONER_2    = 39289,
    SPELL_KARGATH_EXECUTIONER_3    = 39290,

    //SAY_KARGATH_EXECUTE_ALLY = ?,
    //SAY_KARGATH_EXECUTE_HORDE = ?
};

struct SpawnLocation
{
    uint32 uiAllianceEntry, uiHordeEntry;
    float m_fX, m_fY, m_fZ, m_fO;
};

const float afExecutionerLoc[4] = {151.443f, -84.439f, 1.938f, 6.283f};

static SpawnLocation aSoldiersLocs[]=
{
    {0, NPC_SOLDIER_HORDE_1, 119.609f, 256.127f, -45.254f, 5.133f},
    {NPC_SOLDIER_ALLIANCE_1, 0, 131.106f, 254.520f, -45.236f, 3.951f},
    {NPC_SOLDIER_ALLIANCE_3, NPC_SOLDIER_HORDE_3, 151.040f, -91.558f, 1.936f, 1.559f},
    {NPC_SOLDIER_ALLIANCE_2, NPC_SOLDIER_HORDE_2, 150.669f, -77.015f, 1.933f, 4.705f},
    {NPC_OFFICER_ALLIANCE, NPC_OFFICER_HORDE, 138.241f, -84.198f, 1.907f, 0.055f}
};

struct instance_shattered_halls : public ScriptedInstance
{
    instance_shattered_halls(Map *map) : ScriptedInstance(map) {Initialize();};

    uint32 Encounter[ENCOUNTERS];
    std::string str_data;

    uint64 nethekurseDoor1GUID;
    uint64 nethekurseDoor2GUID;
    uint64 nethekurseGUID;
    uint64 warbringerGUID;
    uint64 kargathGUID;
    uint64 executionerGUID;
    uint64 officeraGUID;
    uint64 officerhGUID;
    uint64 soldiera2GUID;
    uint64 soldiera3GUID;
    uint64 soldierh2GUID;
    uint64 soldierh3GUID;

    uint32 uiExecutionTimer;
    uint32 uiTeam;
    uint8 uiExecutionStage;

    void Initialize()
    {
        nethekurseDoor1GUID = 0;
        nethekurseDoor2GUID = 0;
        nethekurseGUID = 0;
        warbringerGUID = 0;
        kargathGUID = 0;
        executionerGUID = 0;
        officeraGUID = 0;
        officerhGUID = 0;
        soldiera2GUID = 0;
        soldiera3GUID = 0;
        soldierh2GUID = 0;
        soldierh3GUID = 0;

        uiTeam = 0;
        uiExecutionStage =0;
        uiExecutionTimer = 55*MINUTE*IN_MILLISECONDS;

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
        if (!instance->IsHeroic() || uiTeam)
            return;

        uiTeam = pPlayer->GetTeam();

        if (uiTeam == ALLIANCE)
            pPlayer->SummonCreature(aSoldiersLocs[1].uiAllianceEntry, aSoldiersLocs[1].m_fX, aSoldiersLocs[1].m_fY, aSoldiersLocs[1].m_fZ, aSoldiersLocs[1].m_fO, TEMPSUMMON_DEAD_DESPAWN, 0);
        else
            pPlayer->SummonCreature(aSoldiersLocs[0].uiHordeEntry, aSoldiersLocs[0].m_fX, aSoldiersLocs[0].m_fY, aSoldiersLocs[0].m_fZ, aSoldiersLocs[0].m_fO, TEMPSUMMON_DEAD_DESPAWN, 0);
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
            case 16808: kargathGUID = pCreature->GetGUID(); break;
            case NPC_EXECUTIONER: executionerGUID = pCreature->GetGUID(); break;
            case NPC_SOLDIER_ALLIANCE_2: soldiera2GUID = pCreature->GetGUID(); break;
            case NPC_SOLDIER_ALLIANCE_3: soldiera3GUID = pCreature->GetGUID(); break;
            case NPC_OFFICER_ALLIANCE: officeraGUID = pCreature->GetGUID(); break;
            case NPC_SOLDIER_HORDE_2: soldierh2GUID = pCreature->GetGUID(); break;
            case NPC_SOLDIER_HORDE_3: soldierh3GUID = pCreature->GetGUID(); break;
            case NPC_OFFICER_HORDE: officerhGUID = pCreature->GetGUID(); break;
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
                if (data == DONE)
                {
                    if (Creature* pExecutioner = instance->GetCreature(executionerGUID))
                        pExecutioner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                }
                if (Encounter[3] != DONE)
                    Encounter[3] = data;
                break;
            case TYPE_EXECUTION:
                if (data == IN_PROGRESS && !instance->GetCreature(executionerGUID))
                {
                    if (Player* pPlayer = GetPlayerInMap())
                    {
                        for (uint8 i = 2; i < 5; ++i)
                            Creature* pVictim = pPlayer->SummonCreature(uiTeam == ALLIANCE ? aSoldiersLocs[i].uiAllianceEntry : aSoldiersLocs[i].uiHordeEntry, aSoldiersLocs[i].m_fX, aSoldiersLocs[i].m_fY, aSoldiersLocs[i].m_fZ, aSoldiersLocs[i].m_fO, TEMPSUMMON_DEAD_DESPAWN, 0);

                        if (Creature* pExecutioner = pPlayer->SummonCreature(NPC_EXECUTIONER, afExecutionerLoc[0], afExecutionerLoc[1], afExecutionerLoc[2], afExecutionerLoc[3], TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 80*MINUTE*IN_MILLISECONDS))
                            pExecutioner->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);

                        DoCastGroupDebuff(SPELL_KARGATH_EXECUTIONER_1);
                        uiExecutionTimer = 55*MINUTE*IN_MILLISECONDS;
                    }
               }
               if (data == DONE)
               {
                   if (Creature* pOfficer = instance->GetCreature(uiTeam == ALLIANCE ? officeraGUID : officerhGUID))
                       pOfficer->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
               }
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

    void OnCreatureDeath(Creature* pCreature)
    {
        if (pCreature->GetEntry() == NPC_EXECUTIONER)
            SetData(TYPE_EXECUTION, DONE);
    }

    void Update(uint32 diff)
    {
        if (uiExecutionTimer)
        {
            if (uiExecutionTimer <= diff)
            {
                switch(uiExecutionStage)
                {
                    case 0:
                        if (Creature* pSoldier = instance->GetCreature(uiTeam == ALLIANCE ? officeraGUID : officerhGUID))
                            pSoldier->DealDamage(pSoldier, pSoldier->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

                        //DoScriptText(uiTeam == ALLIANCE ? SAY_KARGATH_EXECUTE_ALLY : SAY_KARGATH_EXECUTE_HORDE, instance->GetCreature(kargathGUID));

                        DoCastGroupDebuff(SPELL_KARGATH_EXECUTIONER_2);
                        uiExecutionTimer = 10*MINUTE*IN_MILLISECONDS;
                        break;
                    case 1:
                        if (Creature* pSoldier = instance->GetCreature(uiTeam == ALLIANCE ? soldiera2GUID : soldierh2GUID))
                            pSoldier->DealDamage(pSoldier, pSoldier->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

                        DoCastGroupDebuff(SPELL_KARGATH_EXECUTIONER_3);
                        uiExecutionTimer = 15*MINUTE*IN_MILLISECONDS;
                        break;
                     case 2:
                         if (Creature* pSoldier = instance->GetCreature(uiTeam == ALLIANCE ? soldiera3GUID : soldierh3GUID))
                             pSoldier->DealDamage(pSoldier, pSoldier->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

                         SetData(TYPE_EXECUTION, FAIL);
                         uiExecutionTimer = 0;
                         break;
                }
                ++uiExecutionStage;
            }
            else uiExecutionTimer -= diff;
        }
    }

    void DoCastGroupDebuff(uint32 uiSpellId)
    {
        Map::PlayerList const& lPlayers = instance->GetPlayers();

        for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
        {
            Player* pPlayer = itr->getSource();
            if (pPlayer && !pPlayer->HasAura(uiSpellId, 0))
                pPlayer->CastSpell(pPlayer, uiSpellId, true);
        }
    }
};

InstanceData* GetInstanceData_instance_shattered_halls(Map* map)
{
    return new instance_shattered_halls(map);
}

bool AreaTrigger_at_shattered_halls(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pPlayer->isGameMaster() || pPlayer->isDead())
        return false;

    instance_shattered_halls* pInstance = (instance_shattered_halls*)pPlayer->GetInstanceData();

    if (!pInstance)
        return false;

    if (!pInstance->instance->IsHeroic())
        return false;

    if (pInstance->GetData(DATA_KARGATH) == DONE || pInstance->GetData(TYPE_WARBRINGER) == DONE)
        return false;

    if (pInstance->GetData(TYPE_EXECUTION) == NOT_STARTED)
        pInstance->SetData(TYPE_EXECUTION, IN_PROGRESS);

    return true;
}

void AddSC_instance_shattered_halls()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_shattered_halls";
    newscript->GetInstanceData = &GetInstanceData_instance_shattered_halls;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "at_shattered_halls";
    newscript->pAreaTrigger = &AreaTrigger_at_shattered_halls;
    newscript->RegisterSelf();
}

