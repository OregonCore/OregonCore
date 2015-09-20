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
SDName: Instance_Sunwell_Plateau
SD%Complete: 75
SDComment: VERIFY SCRIPT
SDCategory: Sunwell_Plateau
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "sunwell_plateau.h"
#define MAX_ENCOUNTER 6
/* Sunwell Plateau:
0 - Kalecgos and Sathrovarr
1 - Brutallus
2 - Felmyst
3 - Eredar Twins (Alythess and Sacrolash)
4 - M'uru
5 - Kil'Jaeden
*/

struct instance_sunwell_plateau : public ScriptedInstance
{
    instance_sunwell_plateau(Map* pMap) : ScriptedInstance(pMap)
    {
        Initialize();
    };
    uint32 m_auiEncounter[MAX_ENCOUNTER];
    uint32 m_outro_kj;
    /** Creatures **/
    uint64 Kalecgos_Dragon;
    uint64 Kalecgos_Human;
    uint64 Sathrovarr;
    uint64 Brutallus;
    uint64 BrutallusTrigger;
    uint64 Madrigosa;
    uint64 Felmyst;
    uint64 Alythess;
    uint64 Sacrolash;
    uint64 Muru;
    uint64 KilJaeden;
    uint64 KilJaedenController;
    uint64 Anveena;
    uint64 KalecgosKJ;
    uint32 SpectralPlayers;
    /** GameObjects **/
    uint64 ForceField;                                      // Kalecgos Encounter
    uint64 KalecgosWall[2];
    uint64 IceBarrier;
    uint64 FireBarrier;                                     // Felmysts Encounter
    uint64 MurusGate[2];                                    // Murus Encounter
    uint64 DragonOrb[4];                                    // Kil'Jaeden Encounter
    /*** Misc ***/
    uint32 SpectralRealmTimer;
    uint32 m_Outrotimer;
    std::vector<uint64> SpectralRealmList;
    std::vector<uint64> northList, centerList, southList;

    uint64 riftGuid[2];
    uint64 riftTargets[2];
    uint64 soldiersGuid[20];
    uint64 portalGuid;
    uint64 prophetGuid;
    uint64 entropiusCoreGuid;
    uint64 liadrinGuid;
    float m_currentAngleFirst;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

        memset(&riftGuid, 0, sizeof(riftGuid));
        memset(&riftTargets, 0, sizeof(riftTargets));
        memset(&soldiersGuid, 0, sizeof(soldiersGuid));
        portalGuid = 0;
        m_outro_kj = 0;
        prophetGuid = 0;
        entropiusCoreGuid = 0;
        liadrinGuid = 0;
        m_currentAngleFirst = 0.0f;

        northList.clear();
        centerList.clear();
        southList.clear();

        /*** Creatures ***/
        Kalecgos_Dragon         = 0;
        Kalecgos_Human          = 0;
        Sathrovarr              = 0;
        Brutallus               = 0;
        BrutallusTrigger        = 0;
        Madrigosa               = 0;
        Felmyst                 = 0;
        Alythess                = 0;
        Sacrolash               = 0;
        Muru                    = 0;
        KilJaeden               = 0;
        KilJaedenController     = 0;
        Anveena                 = 0;
        KalecgosKJ              = 0;
        SpectralPlayers         = 0;

        /*** GameObjects ***/
        ForceField  = 0;
        FireBarrier = 0;
        MurusGate[0] = 0;
        MurusGate[1] = 0;
        IceBarrier = 0;
        KalecgosWall[0] = 0;
        KalecgosWall[1] = 0;
        DragonOrb[0] = 0;
        DragonOrb[1] = 0;
        DragonOrb[2] = 0;
        DragonOrb[3] = 0;
        /*** Misc ***/
        SpectralRealmTimer = 5000;
        m_Outrotimer = 0;
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            if (m_auiEncounter[i] == IN_PROGRESS)
                return true;
        return false;
    }

    Player* GetPlayerInMap()
    {
        Map::PlayerList const& players = instance->GetPlayers();
        if (!players.isEmpty())
        {
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                Player* plr = itr->getSource();
                if (plr && !plr->HasAura(45839, 0))
                    return plr;
            }
        }
        debug_log("OSCR: Instance Sunwell Plateau: GetPlayerInMap, but PlayerList is empty!");
        return NULL;
    }

    void OnCreatureCreate(Creature* pCreature, bool /*add*/)
    {
        switch (pCreature->GetEntry())
        {
        case 24850:
            Kalecgos_Dragon     = pCreature->GetGUID();
            break;
        case 24891:
            Kalecgos_Human      = pCreature->GetGUID();
            break;
        case 24892:
            Sathrovarr          = pCreature->GetGUID();
            break;
        case 24882:
            Brutallus           = pCreature->GetGUID();
            break;
        case 19871:
            BrutallusTrigger   = pCreature->GetGUID();
            break;
        case 25160:
            Madrigosa           = pCreature->GetGUID();
            break;
        case 25038:
            Felmyst             = pCreature->GetGUID();
            break;
        case 25166:
            Alythess            = pCreature->GetGUID();
            break;
        case 25165:
            Sacrolash           = pCreature->GetGUID();
            break;
        case 25741:
            Muru                = pCreature->GetGUID();
            break;
        case 25315:
            KilJaeden           = pCreature->GetGUID();
            break;
        case 25608:
            KilJaedenController = pCreature->GetGUID();
            break;
        case 26046:
            Anveena             = pCreature->GetGUID();
            break;
        case 25319:
            KalecgosKJ          = pCreature->GetGUID();
            break;
        case 23472:
            if (pCreature->GetPositionX() > 1480.0f)
                northList.push_back(pCreature->GetGUID());
            else if (pCreature->GetPositionX() > 1460.0f)
                centerList.push_back(pCreature->GetGUID());
            else
                southList.push_back(pCreature->GetGUID());

            break;
        }
    }

    void OnGameObjectCreate(GameObject* pGo, bool /*add*/)
    {
        switch (pGo->GetEntry())
        {
        case 188421:
            ForceField     = pGo->GetGUID();
            break;
        case 188523:
            KalecgosWall[0] = pGo->GetGUID();
            break;
        case 188524:
            KalecgosWall[0] = pGo->GetGUID();
            break;
        case 188119:
            IceBarrier = pGo->GetGUID();
            break;
        case 188075:
            if (m_auiEncounter[2] == DONE)
                HandleGameObject(NULL, true, pGo);
            FireBarrier = pGo->GetGUID();
            break;
        case 187990:
            MurusGate[0]   = pGo->GetGUID();
            break;
        case 188118:
            if (m_auiEncounter[4] == DONE)
                HandleGameObject(NULL, true, pGo);
            MurusGate[1] = pGo->GetGUID();
            break;
        case 188415:
            switch (pGo->GetDBTableGUIDLow())
            {
            case 100000:
                DragonOrb[0] = pGo->GetGUID();
                break;
            case 100001:
                DragonOrb[1] = pGo->GetGUID();
                break;
            case 100002:
                DragonOrb[2] = pGo->GetGUID();
                break;
            case 100003:
                DragonOrb[3] = pGo->GetGUID();
                break;
            }
        }
    }

    uint32 GetData(uint32 id)
    {
        switch (id)
        {
        case DATA_KALECGOS_EVENT:
            return m_auiEncounter[0];
        case DATA_BRUTALLUS_EVENT:
            return m_auiEncounter[1];
        case DATA_FELMYST_EVENT:
            return m_auiEncounter[2];
        case DATA_EREDAR_TWINS_EVENT:
            return m_auiEncounter[3];
        case DATA_MURU_EVENT:
            return m_auiEncounter[4];
        case DATA_KILJAEDEN_EVENT:
            return m_auiEncounter[5];
        case DATA_OUTRO_KJ:
            return m_outro_kj;
        }
        return 0;
    }

    uint64 GetData64(uint32 id)
    {
        switch (id)
        {
        case DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_1:
            return DragonOrb[0];
        case DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_2:
            return DragonOrb[1];
        case DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_3:
            return DragonOrb[2];
        case DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_4:
            return DragonOrb[3];
        case DATA_KALECGOS_DRAGON:
            return Kalecgos_Dragon;
        case DATA_KALECGOS_HUMAN:
            return Kalecgos_Human;
        case DATA_SATHROVARR:
            return Sathrovarr;
        case DATA_GO_FORCEFIELD:
            return ForceField;
        case DATA_BRUTALLUS:
            return Brutallus;
        case DATA_MADRIGOSA:
            return Madrigosa;
        case DATA_FELMYST:
            return Felmyst;
        case DATA_ALYTHESS:
            return Alythess;
        case DATA_SACROLASH:
            return Sacrolash;
        case DATA_MURU:
            return Muru;
        case DATA_KILJAEDEN:
            return KilJaeden;
        case DATA_KILJAEDEN_CONTROLLER:
            return KilJaedenController;
        case DATA_ANVEENA:
            return Anveena;
        case DATA_KALECGOS_KJ:
            return KalecgosKJ;
        case DATA_PLAYER_GUID:
            if (Player* Target = GetPlayerInMap())
                return Target->GetGUID();
            else
                return 0;
        }
        return 0;
    }

    void SetData(uint32 id, uint32 data)
    {
        switch (id)
        {
        case DATA_KALECGOS_EVENT:
            {
                if (data == NOT_STARTED || data == DONE)
                {
                    HandleGameObject(ForceField, true);
                    HandleGameObject(KalecgosWall[0], true);
                    HandleGameObject(KalecgosWall[1], true);
                }
                else if (data == IN_PROGRESS)
                {
                    HandleGameObject(ForceField, false);
                    HandleGameObject(KalecgosWall[0], false);
                    HandleGameObject(KalecgosWall[1], false);
                }
                m_auiEncounter[0] = data;
            }
            break;
        case DATA_BRUTALLUS_EVENT:
            if (data == SPECIAL)
                DoUseDoorOrButton(IceBarrier, MINUTE);
            m_auiEncounter[1] = data;
            break;
        case DATA_FELMYST_EVENT:
            if (data == DONE)
                HandleGameObject(FireBarrier, true);
            m_auiEncounter[2] = data;
            break;
        case DATA_ACTIVATE_NORTH_TO_LEFT:
            for (std::vector<uint64>::iterator itr = northList.begin(); itr != northList.end(); itr++)
            {
                if (Creature *trigger = instance->GetCreature(*itr))
                {
                    if (trigger->GetPositionY() > data)
                        trigger->CastSpell(trigger, 45582, true);
                }
            }
            break;
        case DATA_ACTIVATE_CENTER_TO_LEFT:
            for (std::vector<uint64>::iterator itr = centerList.begin(); itr != centerList.end(); itr++)
            {
                if (Creature *trigger = instance->GetCreature(*itr))
                {
                    if (trigger->GetPositionY() > data)
                        trigger->CastSpell(trigger, 45582, true);
                }
            }
            break;
        case DATA_ACTIVATE_SOUTH_TO_LEFT:
            for (std::vector<uint64>::iterator itr = southList.begin(); itr != southList.end(); itr++)
            {
                if (Creature *trigger = instance->GetCreature(*itr))
                {
                    if (trigger->GetPositionY() > data)
                        trigger->CastSpell(trigger, 45582, true);
                }
            }
            break;
        case DATA_ACTIVATE_NORTH_TO_RIGHT:
            for (std::vector<uint64>::iterator itr = northList.begin(); itr != northList.end(); itr++)
            {
                if (Creature *trigger = instance->GetCreature(*itr))
                {
                    if (trigger->GetPositionY() < data)
                        trigger->CastSpell(trigger, 45582, true);
                }
            }
            break;
        case DATA_ACTIVATE_CENTER_TO_RIGHT:
            for (std::vector<uint64>::iterator itr = centerList.begin(); itr != centerList.end(); itr++)
            {
                if (Creature *trigger = instance->GetCreature(*itr))
                {
                    if (trigger->GetPositionY() < data)
                        trigger->CastSpell(trigger, 45582, true);
                }
            }
            break;
        case DATA_ACTIVATE_SOUTH_TO_RIGHT:
            for (std::vector<uint64>::iterator itr = southList.begin(); itr != southList.end(); itr++)
            {
                if (Creature *trigger = instance->GetCreature(*itr))
                {
                    if (trigger->GetPositionY() < data)
                        trigger->CastSpell(trigger, 45582, true);
                }
            }
            break;
        case DATA_EREDAR_TWINS_EVENT:
            m_auiEncounter[3] = data;
            break;
        case DATA_MURU_EVENT:
            switch (data)
            {
            case DONE:
                HandleGameObject(MurusGate[0], true);
                HandleGameObject(MurusGate[1], true);
                break;
            case IN_PROGRESS:
                HandleGameObject(MurusGate[0], false);
                HandleGameObject(MurusGate[1], false);
                break;
            case NOT_STARTED:
                HandleGameObject(MurusGate[0], true);
                HandleGameObject(MurusGate[1], false);
                break;
            }
            m_auiEncounter[4] = data;
            break;
        case DATA_OUTRO_KJ:
            m_outro_kj = data;
            break;
        case DATA_KILJAEDEN_EVENT:
            switch (data)
            {
                case DONE:
                    SetData(DATA_OUTRO_KJ, POINT_KILJAEDEN_DIE);
                    m_Outrotimer = 0;
                    break;
            }
            m_auiEncounter[5] = data;
            break;
        }
        if (data == DONE)
            SaveToDB();
    }

    void MovementInform(Creature* creature, uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        switch (uiPointId)
        {
        case 0:
            switch (creature->GetEntry())
            {
            case NPC_RIFTWALKER:
                if (Creature* riftTarget = GetCreature(riftTargets[0]))
                {
                    creature->CastSpell(riftTarget, SPELL_OPEN_PORTAL_KJ, false);
                    riftTarget->GetMotionMaster()->MovePoint(0, aOutroLocations[9].m_fX, aOutroLocations[9].m_fY, aOutroLocations[9].m_fZ + 13.0f);
                }
                break;
            case NPC_SOLDIER:
                if (creature->GetGUID() == soldiersGuid[0] || creature->GetGUID() == soldiersGuid[10])
                    creature->SetStandState(UNIT_STAND_STATE_KNEEL);

                if (Creature* portal = GetCreature(portalGuid))
                    creature->SetFacingTo(creature->GetAngle(aOutroLocations[5].m_fX, aOutroLocations[5].m_fY));

                break;
            }
            break;
        case 1:
            switch (creature->GetEntry())
            {
            case NPC_RIFTWALKER:
                if (Creature* riftTarget = GetCreature(riftTargets[1]))
                {
                    creature->CastSpell(riftTarget, SPELL_OPEN_PORTAL_KJ, false);
                    riftTarget->GetMotionMaster()->MovePoint(0, aOutroLocations[10].m_fX, aOutroLocations[10].m_fY, aOutroLocations[10].m_fZ + 13.0f);
                }
                break;
            case NPC_CORE_ENTROPIUS:
                if (Creature* pVelen = GetCreature(prophetGuid))
                    pVelen->InterruptNonMeleeSpells(false);
                break;
            case CREATURE_PROPHET:
                creature->ForcedDespawn(1000);

                // Note: portal should despawn only after all the soldiers have reached this point and "teleported" outside
                if (Creature* pPortal = GetCreature(portalGuid))
                    pPortal->ForcedDespawn(30000);

                for (uint8 i = 0; i < 2; i++)
                {
                    if (Creature* rift = GetCreature(riftGuid[i]))
                        rift->ForcedDespawn(1000);
                }

                if (Creature *controller = GetCreature(GetData64(DATA_KILJAEDEN_CONTROLLER)))
                    controller->ForcedDespawn(300000);
                break;
            }
            break;
        case 2:
            if (creature->GetEntry() == NPC_SOLDIER)
                creature->ForcedDespawn(1000);
            break;
        case 10:
            if (creature->GetEntry() == NPC_SOLDIER)
                 creature->SetFacingTo(SoldierMiddle[0].m_fO);
            break;
        case 11:
            if (creature->GetEntry() == NPC_SOLDIER)
                creature->SetFacingTo(SoldierMiddle[1].m_fO);
            break;
        }
    }

    void Update(uint32 diff)
    {
        // KilJaeden Outro
        if (m_Outrotimer <= diff)
        {
            switch (GetData(DATA_OUTRO_KJ))
            {
                case POINT_KILJAEDEN_DIE:
                    // While Kil'Jaeden die
                    if (Creature* Anveena = GetCreature(GetData64(DATA_ANVEENA)))
                        Anveena->ForcedDespawn(); //this should already be done but let's do it again in case phase was gm rushed

                    m_Outrotimer = 15000;
                    SetData(DATA_OUTRO_KJ, POINT_TELEPORT_KALECGOS);
                    break;
                case POINT_TELEPORT_KALECGOS:
                    if (Creature* pKalec = GetCreature(GetData64(DATA_KALECGOS_KJ)))
                    {
                        pKalec->SetVisibility(VISIBILITY_ON);
                        pKalec->CastSpell(pKalec, SPELL_KALEC_TELEPORT, true);
                        pKalec->SendMovementFlagUpdate();
                    }

                    if (Creature* pKJ = GetCreature(GetData64(DATA_KILJAEDEN)))
                        pKJ->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE); //allow loot

                    m_Outrotimer = 2000;
                    SetData(DATA_OUTRO_KJ, SAY_KALECGOS_GOODBYE);
                    break;
                case SAY_KALECGOS_GOODBYE:
                    if (Creature* pKalec = GetCreature(GetData64(DATA_KALECGOS_KJ)))
                        DoScriptText(-1580090, pKalec);

                    m_Outrotimer = 15000;
                    SetData(DATA_OUTRO_KJ, POINT_SUMMON_SHATTERED);
                    break;
                case POINT_SUMMON_SHATTERED:
                    if (Creature *controller = GetCreature(GetData64(DATA_KILJAEDEN_CONTROLLER)))
                    {
                        if (Creature* portal = controller->SummonCreature(NPC_BOSS_PORTAL, aOutroLocations[0].m_fX, aOutroLocations[0].m_fY, aOutroLocations[0].m_fZ, aOutroLocations[0].m_fO, TEMPSUMMON_CORPSE_DESPAWN, 0))
                            portalGuid = portal->GetGUID();

                        for (uint8 i = 1; i < 3; i++)
                        {
                            if (TempSummon * riftWalker = controller->SummonCreature(NPC_RIFTWALKER, aOutroLocations[i].m_fX, aOutroLocations[i].m_fY, aOutroLocations[i].m_fZ, aOutroLocations[i].m_fO, TEMPSUMMON_CORPSE_DESPAWN, 0))
                            {
                                riftGuid[i - 1] = riftWalker->GetGUID();

                                if (i == 1)
                                    riftWalker->GetMotionMaster()->MovePoint(0, aOutroLocations[7].m_fX, aOutroLocations[7].m_fY, aOutroLocations[7].m_fZ);
                                else
                                    riftWalker->GetMotionMaster()->MovePoint(1, aOutroLocations[8].m_fX, aOutroLocations[8].m_fY, aOutroLocations[8].m_fZ);

                                if (Creature * riftTarget = controller->SummonCreature(WORLD_TRIGGER, aOutroLocations[8 + i].m_fX, aOutroLocations[8 + i].m_fY, aOutroLocations[8 + i].m_fZ, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0))
                                {
                                    riftTargets[i - 1] = riftTarget->GetGUID();
                                    riftTarget->SetSpeed(MOVE_RUN, 0.5f);
                                }
                            }
                        }
                    }

                    m_Outrotimer = 15000;
                    SetData(DATA_OUTRO_KJ, POINT_SUMMON_PORTAL);
                    break;
                case POINT_SUMMON_PORTAL:
                    if (Creature* portal = GetCreature(portalGuid))
                    {
                        portal->SetDisplayId(DISPLAYID_PORTAL_OPENING);
                        for (uint8 i = 0; i < 2; i++)
                        {
                            if (Creature* riftTarget = GetCreature(riftTargets[i]))
                                riftTarget->ForcedDespawn(1000);
                        }
                    }

                    m_Outrotimer = 500;
                    SetData(DATA_OUTRO_KJ, POINT_SUMMON_PORTAL_ENDOPENANIM);
                    break;
                case POINT_SUMMON_PORTAL_ENDOPENANIM:
                    if (Creature* portal = GetCreature(portalGuid))
                        portal->SetStandState(UNIT_STAND_STATE_SIT); //this smoothly stop the explosion effect and just let the smokes continues

                    m_Outrotimer = 3500;
                    SetData(DATA_OUTRO_KJ, POINT_SUMMON_SOLDIERS_RIGHT);
                    break;
                case POINT_SUMMON_SOLDIERS_RIGHT:
                    for (uint8 i = 0; i < 2; i++)
                    {
                        if (Creature* rift = GetCreature(riftGuid[i]))
                        {
                            rift->RemoveAurasDueToSpell(SPELL_OPEN_PORTAL_KJ);
                            rift->InterruptNonMeleeSpells(false);
                        }
                    }
                    if (Creature *controller = GetCreature(GetData64(DATA_KILJAEDEN_CONTROLLER)))
                    {
                        for (uint8 i = 0; i < 10; i++)
                        {
                            if (TempSummon *soldier = controller->SummonCreature(NPC_SOLDIER, SoldierLocations[i].m_fX, SoldierLocations[i].m_fY, SoldierLocations[i].m_fZ, SoldierLocations[i].m_fO, TEMPSUMMON_CORPSE_DESPAWN, 0))
                            {
                                soldiersGuid[i] = soldier->GetGUID();
                                soldier->GetMotionMaster()->MovePoint(0, SoldierMiddle[0].m_fX, SoldierMiddle[0].m_fY, SoldierMiddle[0].m_fZ, false);

                                if (i == 0)
                                    soldier->GetMotionMaster()->MovePoint(0, SoldierMiddle[0].m_fX, SoldierMiddle[0].m_fY, SoldierMiddle[0].m_fZ, false);
                                else
                                {
                                    float sx, sy;
                                    float angle = m_currentAngleFirst * (2.0f * M_PI) / 360.0f;
                                    float rayon = 5.0f;
                                    sx = SoldierMiddle[0].m_fX + cos(angle) * rayon;
                                    sy = SoldierMiddle[0].m_fY + sin(angle) * rayon;
                                    soldier->GetMotionMaster()->MovePoint(0, sx, sy, SoldierMiddle[0].m_fZ, false);
                                    m_currentAngleFirst = m_currentAngleFirst + 36;
                                }
                            }
                        }
                    }

                    m_Outrotimer = 8000;
                    SetData(DATA_OUTRO_KJ, POINT_SUMMON_SOLDIERS_LEFT);
                    break;
                case POINT_SUMMON_SOLDIERS_LEFT:
                    if (Creature *controller = GetCreature(GetData64(DATA_KILJAEDEN_CONTROLLER)))
                    {
                        for (uint8 i = 10; i < 20; i++)
                        {
                            if (Creature *soldier = controller->SummonCreature(NPC_SOLDIER, SoldierLocations[i].m_fX, SoldierLocations[i].m_fY, SoldierLocations[i].m_fZ, SoldierLocations[i].m_fO, TEMPSUMMON_CORPSE_DESPAWN, 0))
                            {
                                soldiersGuid[i] = soldier->GetGUID();

                                if (i == 10)
                                    soldier->GetMotionMaster()->MovePoint(0, SoldierMiddle[1].m_fX, SoldierMiddle[1].m_fY, SoldierMiddle[1].m_fZ, false);
                                else
                                {
                                    float sx, sy;
                                    float angle = m_currentAngleFirst * (2 * M_PI) / 360;
                                    float rayon = 5.0f;
                                    sx = SoldierMiddle[1].m_fX + cos(angle) * rayon;
                                    sy = SoldierMiddle[1].m_fY + sin(angle) * rayon;
                                    soldier->GetMotionMaster()->MovePoint(0, sx, sy, SoldierMiddle[1].m_fZ, false);
                                    m_currentAngleFirst = m_currentAngleFirst + 36;
                                }
                            }
                        }
                    }

                    m_Outrotimer = 10000;
                    SetData(DATA_OUTRO_KJ, POINT_SUMMON_PROPHET);
                    break;
                case POINT_SUMMON_PROPHET:
                    if (Creature *controller = GetCreature(GetData64(DATA_KILJAEDEN_CONTROLLER)))
                    {
                        if (TempSummon* prophet = controller->SummonCreature(CREATURE_PROPHET, aOutroLocations[3].m_fX, aOutroLocations[3].m_fY, aOutroLocations[3].m_fZ, aOutroLocations[3].m_fO, TEMPSUMMON_CORPSE_DESPAWN, 0))
                            prophetGuid = prophet->GetGUID();

                        if (TempSummon* core = controller->SummonCreature(NPC_CORE_ENTROPIUS, controller->GetPositionX(), controller->GetPositionY(), 85.0f, 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
                            entropiusCoreGuid = core->GetGUID();
                    }

                    m_Outrotimer = 2000;
                    SetData(DATA_OUTRO_KJ, POINT_SUMMON_LIADRIN);
                    break;
                case POINT_SUMMON_LIADRIN:
                    if (Creature *controller = GetCreature(GetData64(DATA_KILJAEDEN_CONTROLLER)))
                        if (Creature* liadrin = controller->SummonCreature(CREATURE_LIADRIN, aOutroLocations[4].m_fX, aOutroLocations[4].m_fY, aOutroLocations[4].m_fZ, aOutroLocations[4].m_fO, TEMPSUMMON_TIMED_DESPAWN, 4 * MINUTE * IN_MILLISECONDS))
                            liadrinGuid = liadrin->GetGUID();
                    
                    m_Outrotimer = 4000;
                    SetData(DATA_OUTRO_KJ, SAY_OUTRO_1);
                    break;
                case SAY_OUTRO_1:
                    if (Creature* prophet = GetCreature(prophetGuid))
                        DoScriptText(-1580099, prophet);

                    m_Outrotimer = 25000;
                    SetData(DATA_OUTRO_KJ, SAY_OUTRO_2);
                    break;
                case SAY_OUTRO_2:
                    if (Creature* prophet = GetCreature(prophetGuid))
                        DoScriptText(-1580100, prophet);

                    m_Outrotimer = 14000;
                    SetData(DATA_OUTRO_KJ, SAY_OUTRO_3);
                    break;
                case SAY_OUTRO_3:
                    if (Creature* prophet = GetCreature(prophetGuid))
                        DoScriptText(-1580111, prophet);

                    m_Outrotimer = 10000;
                    SetData(DATA_OUTRO_KJ, POINT_CALL_ENTROPIUS);
                    break;
                case POINT_CALL_ENTROPIUS:
                    if (Creature *controller = GetCreature(GetData64(DATA_KILJAEDEN_CONTROLLER)))
                    {
                        if (Creature* pEntropius = GetCreature(entropiusCoreGuid))
                        {
                            if (Creature* pVelen = GetCreature(prophetGuid))
                                pVelen->CastSpell(pEntropius, SPELL_CALL_ENTROPIUS, false);

                            pEntropius->SetWalk(false);
                            pEntropius->GetMotionMaster()->MovePoint(1, controller->GetPositionX(), controller->GetPositionY(), 40.0f);
                        }
                    }

                    m_Outrotimer = 10000;
                    SetData(DATA_OUTRO_KJ, SAY_OUTRO_4);
                    break;
                case SAY_OUTRO_4:
                    if (Creature* prophet = GetCreature(prophetGuid))
                        DoScriptText(-1580101, prophet);

                    m_Outrotimer = 22000;
                    SetData(DATA_OUTRO_KJ, POINT_MOVE_LIADRIN);
                    break;
                case POINT_MOVE_LIADRIN:
                    if (Creature* pLiadrin = GetCreature(liadrinGuid))
                        pLiadrin->GetMotionMaster()->MovePoint(0, aOutroLocations[6].m_fX, aOutroLocations[6].m_fY, aOutroLocations[6].m_fZ);
                    
                    m_Outrotimer = 6000;
                    SetData(DATA_OUTRO_KJ, SAY_OUTRO_5);
                    break;
                case SAY_OUTRO_5:
                    if (Creature* pLiadrin = GetCreature(liadrinGuid))
                        DoScriptText(-1580107, pLiadrin);

                    m_Outrotimer = 10000;
                    SetData(DATA_OUTRO_KJ, SAY_OUTRO_6);
                    break;
                case SAY_OUTRO_6:
                    if (Creature* prophet = GetCreature(prophetGuid))
                        DoScriptText(-1580102, prophet);

                    m_Outrotimer = 15000;
                    SetData(DATA_OUTRO_KJ, SAY_OUTRO_7);
                    break;
                case SAY_OUTRO_7:
                    if (Creature* pLiadrin = GetCreature(liadrinGuid))
                        DoScriptText(-1580108, pLiadrin);

                    m_Outrotimer = 2500;
                    SetData(DATA_OUTRO_KJ, SAY_OUTRO_8);
                    break;
                case SAY_OUTRO_8:
                    if (Creature* prophet = GetCreature(prophetGuid))
                        DoScriptText(-1580103, prophet);

                    m_Outrotimer = 4000;
                    SetData(DATA_OUTRO_KJ, POINT_BLAZE);
                    break;
                case POINT_BLAZE:
                    if (Creature* pEntropius = GetCreature(entropiusCoreGuid))
                    {
                        pEntropius->CastSpell(pEntropius, SPELL_BLAZE_TO_LIGHT, true);
                        pEntropius->RemoveAurasDueToSpell(SPELL_ENTROPIUS_BODY);
                    }

                    m_Outrotimer = 10000;
                    SetData(DATA_OUTRO_KJ, POINT_IGNITE);
                    break;
                case POINT_IGNITE:
                    if (Creature *controller = GetCreature(GetData64(DATA_KILJAEDEN_CONTROLLER)))
                    {
                        // When the purified Muru reaches the ground the sunwell ignites and Muru despawns
                        controller->AI()->DoCast(controller, SPELL_SUNWELL_IGNITION);

                        if (Creature* pLiadrin = GetCreature(liadrinGuid))
                            pLiadrin->SetStandState(UNIT_STAND_STATE_KNEEL);

                        if (Creature* pEntropius = GetCreature(entropiusCoreGuid))
                            pEntropius->ForcedDespawn();
                    }

                    m_Outrotimer = 500;
                    SetData(DATA_OUTRO_KJ, SAY_OUTRO_9);
                    break;
                case SAY_OUTRO_9:
                    if (Creature* prophet = GetCreature(prophetGuid))
                        DoScriptText(-1580104, prophet);

                    m_Outrotimer = 15000;
                    SetData(DATA_OUTRO_KJ, SAY_OUTRO_10);
                    break;
                case SAY_OUTRO_10:
                    if (Creature* pLiadrin = GetCreature(liadrinGuid))
                        DoScriptText(-1580109, pLiadrin);

                    m_Outrotimer = 20000;
                    SetData(DATA_OUTRO_KJ, SAY_OUTRO_11);
                    break;
                case SAY_OUTRO_11:
                    if (Creature* prophet = GetCreature(prophetGuid))
                        DoScriptText(-1580105, prophet);

                    m_Outrotimer = 6000;
                    SetData(DATA_OUTRO_KJ, SAY_OUTRO_12);
                    break;
                case SAY_OUTRO_12:
                    if (Creature* prophet = GetCreature(prophetGuid))
                        DoScriptText(-1580106, prophet);

                    m_Outrotimer = 2000;
                    SetData(DATA_OUTRO_KJ, POINT_EVENT_SOLDIER_EXIT);
                    break;
                case POINT_EVENT_SOLDIER_EXIT:
                    for (uint8 i = 0; i < 20; i++)
                    {
                        if (Creature* soldier = GetCreature(soldiersGuid[i]))
                        {
                            soldier->SetWalk(false);
                            soldier->SetSpeed(MOVE_RUN, 1.0f);
                            soldier->GetMotionMaster()->MovePoint(2, SoldierLocations[i].m_fX, SoldierLocations[i].m_fY, SoldierLocations[i].m_fZ, false);
                        }
                    }

                    m_Outrotimer = 8000;
                    SetData(DATA_OUTRO_KJ, POINT_EVENT_VELEN_EXIT);
                    break;
                case POINT_EVENT_VELEN_EXIT:
                    if (Creature* pVelen = GetCreature(prophetGuid))
                        pVelen->GetMotionMaster()->MovePoint(1, aOutroLocations[3].m_fX, aOutroLocations[3].m_fY, aOutroLocations[3].m_fZ);

                    if (Creature* pKalec = GetCreature(GetData64(DATA_KALECGOS_KJ)))
                    {
                        pKalec->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        pKalec->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }

                    m_Outrotimer = 0;
                    SetData(DATA_OUTRO_KJ, OUTRO_DONE);
                    break;
            }
        }
        else
            m_Outrotimer -= diff;
    }

    std::string GetSaveData()
    {
        OUT_SAVE_INST_DATA;
        std::ostringstream stream;
        stream << m_auiEncounter[0] << " "  << m_auiEncounter[1] << " "  << m_auiEncounter[2] << " "  << m_auiEncounter[3] << " "
               << m_auiEncounter[4] << " "  << m_auiEncounter[5];
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
        stream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
               >> m_auiEncounter[4] >> m_auiEncounter[5];
        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            if (m_auiEncounter[i] == IN_PROGRESS)                // Do not load an encounter as "In Progress" - reset it instead.
                m_auiEncounter[i] = NOT_STARTED;
        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_sunwell_plateau(Map* pMap)
{
    return new instance_sunwell_plateau(pMap);
}

void AddSC_instance_sunwell_plateau()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "instance_sunwell_plateau";
    newscript->GetInstanceData = &GetInstanceData_instance_sunwell_plateau;
    newscript->RegisterSelf();
}
