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

#include "OutdoorPvPEP.h"
#include "OutdoorPvP.h"
#include "OutdoorPvPMgr.h"
#include "Player.h"
#include "WorldPacket.h"
#include "World.h"
#include "ObjectMgr.h"
#include "Language.h"
#include "GossipDef.h"

const uint32 EP_LANG_LOOSE_A[EP_TOWER_NUM] = {LANG_OPVP_EP_LOOSE_CROWNGUARD_A, LANG_OPVP_EP_LOOSE_EASTWALL_A, LANG_OPVP_EP_LOOSE_NORTHPASS_A, LANG_OPVP_EP_LOOSE_PLAGUEWOOD_A};

const uint32 EP_LANG_LOOSE_H[EP_TOWER_NUM] = {LANG_OPVP_EP_LOOSE_CROWNGUARD_H, LANG_OPVP_EP_LOOSE_EASTWALL_H, LANG_OPVP_EP_LOOSE_NORTHPASS_H, LANG_OPVP_EP_LOOSE_PLAGUEWOOD_H};

const uint32 EP_LANG_CAPTURE_A[EP_TOWER_NUM] = {LANG_OPVP_EP_CAPTURE_CROWNGUARD_A, LANG_OPVP_EP_CAPTURE_EASTWALL_A, LANG_OPVP_EP_CAPTURE_NORTHPASS_A, LANG_OPVP_EP_CAPTURE_PLAGUEWOOD_A};

const uint32 EP_LANG_CAPTURE_H[EP_TOWER_NUM] = {LANG_OPVP_EP_CAPTURE_CROWNGUARD_H, LANG_OPVP_EP_CAPTURE_EASTWALL_H, LANG_OPVP_EP_CAPTURE_NORTHPASS_H, LANG_OPVP_EP_CAPTURE_PLAGUEWOOD_H};

const uint32 EP_LANG_FLIGHT_GOSSIPS[EP_TOWER_NUM - 1] = {LANG_OPVP_EP_FLIGHT_NORTHPASS, LANG_OPVP_EP_FLIGHT_EASTWALL, LANG_OPVP_EP_FLIGHT_CROWNGUARD};

OPvPCapturePointEP::OPvPCapturePointEP(OutdoorPvP* pvp, OutdoorPvPEPTowerType type)
    : OPvPCapturePoint(pvp), m_TowerType(type)
{
    SetCapturePointData(EPCapturePoints[type].entry,
                        EPCapturePoints[type].map,
                        EPCapturePoints[type].x,
                        EPCapturePoints[type].y,
                        EPCapturePoints[type].z,
                        EPCapturePoints[type].o,
                        EPCapturePoints[type].rot0,
                        EPCapturePoints[type].rot1,
                        EPCapturePoints[type].rot2,
                        EPCapturePoints[type].rot3);

    for (uint8 i = 0; i < 2; i++)
        AddObject(type + EP_TOWER_NUM * i,
                  EPTowerFlags[type + EP_TOWER_NUM * i].entry,
                  21,
                  EPTowerFlags[type + EP_TOWER_NUM * i].map,
                  EPTowerFlags[type + EP_TOWER_NUM * i].x,
                  EPTowerFlags[type + EP_TOWER_NUM * i].y,
                  EPTowerFlags[type + EP_TOWER_NUM * i].z,
                  EPTowerFlags[type + EP_TOWER_NUM * i].o,
                  EPTowerFlags[type + EP_TOWER_NUM * i].rot0,
                  EPTowerFlags[type + EP_TOWER_NUM * i].rot1,
                  EPTowerFlags[type + EP_TOWER_NUM * i].rot2,
                  EPTowerFlags[type + EP_TOWER_NUM * i].rot3);

    EP_TOWER_EVENT_TEAM[type] = 0;
}

OutdoorPvPEP::OutdoorPvPEP()
{
    m_TypeId = OUTDOOR_PVP_EP;
}

bool OutdoorPvPEP::SetupOutdoorPvP()
{
    m_AllianceTowersControlled = 0;
    m_HordeTowersControlled = 0;

    // add the zones affected by the pvp buff
    for (uint8 i = 0; i < OutdoorPvPEPBuffZonesNum; ++i)
        RegisterZone(OutdoorPvPEPBuffZones[i]);

    AddCapturePoint(new OPvPCapturePointEP(this, EP_TOWER_CROWNGUARD));
    AddCapturePoint(new OPvPCapturePointEP(this, EP_TOWER_EASTWALL));
    AddCapturePoint(new OPvPCapturePointEP(this, EP_TOWER_NORTHPASS));
    AddCapturePoint(new OPvPCapturePointEP(this, EP_TOWER_PLAGUEWOOD));

    return true;
}

void OutdoorPvPEP::HandlePlayerEnterZone(Player* plr, uint32 zone)
{
    // add buffs
    if (plr->GetTeam() == ALLIANCE)
    {
        if (m_AllianceTowersControlled > 0)
            plr->CastSpell(plr, EP_AllianceBuffs[m_AllianceTowersControlled - 1], true);
    }
    else
    {
        if (m_HordeTowersControlled > 0)
            plr->CastSpell(plr, EP_HordeBuffs[m_HordeTowersControlled - 1], true);
    }
    OutdoorPvP::HandlePlayerEnterZone(plr, zone);
}

void OutdoorPvPEP::HandlePlayerLeaveZone(Player* plr, uint32 zone)
{
    // remove buffs
    if (plr->GetTeam() == ALLIANCE)
    {
        for (uint8 i = 0; i < OutdoorPvPEPTeamsBuffNum; ++i)
            plr->RemoveAurasDueToSpell(EP_AllianceBuffs[i]);
    }
    else
    {
        for (uint8 i = 0; i < OutdoorPvPEPTeamsBuffNum; ++i)
            plr->RemoveAurasDueToSpell(EP_HordeBuffs[i]);
    }
    OutdoorPvP::HandlePlayerLeaveZone(plr, zone);
}

bool OutdoorPvPEP::Update(uint32 diff)
{
    bool changed = false;
    if ((changed = OutdoorPvP::Update(diff)))
    {
        for (uint8 i = 0; i < OutdoorPvPEPTeamsBuffNum; ++i)
        {
            TeamCastSpell(TEAM_ALLIANCE, -EP_AllianceBuffs[i]);
            TeamCastSpell(TEAM_HORDE, -EP_HordeBuffs[i]);
        }

        uint32 AllianceBuff = m_AllianceTowersControlled > 0 ? EP_AllianceBuffs[m_AllianceTowersControlled - 1] : 0;
        uint32 HordeBuff = m_HordeTowersControlled > 0 ? EP_HordeBuffs[m_HordeTowersControlled - 1] : 0;
        if (m_AllianceTowersControlled > 0)
            TeamApplyBuff(TEAM_ALLIANCE, AllianceBuff, HordeBuff);
        if (m_HordeTowersControlled > 0)
            TeamApplyBuff(TEAM_HORDE, HordeBuff, AllianceBuff);

        SendUpdateWorldState(EP_UI_TOWER_COUNT_A, m_AllianceTowersControlled);
        SendUpdateWorldState(EP_UI_TOWER_COUNT_H, m_HordeTowersControlled);
    }
    return changed;
}

void OutdoorPvPEP::SendRemoveWorldStates(Player* plr)
{
    plr->SendUpdateWorldState(EP_UI_TOWER_COUNT_A, 0);
    plr->SendUpdateWorldState(EP_UI_TOWER_COUNT_H, 0);
    plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_DISPLAY, 0);
    plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_POS, 0);
    plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_N, 0);

    for (uint8 i = 0; i < EP_TOWER_NUM; ++i)
    {
        plr->SendUpdateWorldState(EP_MAP_N[i], 0);
        plr->SendUpdateWorldState(EP_MAP_A[i], 0);
        plr->SendUpdateWorldState(EP_MAP_H[i], 0);
        plr->SendUpdateWorldState(EP_MAP_N_A[i], 0);
        plr->SendUpdateWorldState(EP_MAP_N_H[i], 0);
    }
}

void OutdoorPvPEP::FillInitialWorldStates(WorldPacket& data)
{
    data << EP_UI_TOWER_COUNT_A << m_AllianceTowersControlled;
    data << EP_UI_TOWER_COUNT_H << m_HordeTowersControlled;
    data << EP_UI_TOWER_SLIDER_DISPLAY << uint32(0);
    data << EP_UI_TOWER_SLIDER_POS << uint32(50);
    data << EP_UI_TOWER_SLIDER_N << uint32(100);
    for (OPvPCapturePointMap::iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
        itr->second->FillInitialWorldStates(data);
}

void OPvPCapturePointEP::ChangeState()
{
    uint32 field = 0;
    switch (m_OldState)
    {
    case OBJECTIVESTATE_NEUTRAL:
        field = EP_MAP_N[m_TowerType];
        break;
    case OBJECTIVESTATE_ALLIANCE:
        field = EP_MAP_A[m_TowerType];
        if (((OutdoorPvPEP*)m_PvP)->m_AllianceTowersControlled)
            ((OutdoorPvPEP*)m_PvP)->m_AllianceTowersControlled--;
        sWorld.SendZoneText(OutdoorPvPEPBuffZones[0], sObjectMgr.GetOregonStringForDBCLocale(EP_LANG_LOOSE_A[m_TowerType]));
        break;
    case OBJECTIVESTATE_HORDE:
        field = EP_MAP_H[m_TowerType];
        if (((OutdoorPvPEP*)m_PvP)->m_HordeTowersControlled)
            ((OutdoorPvPEP*)m_PvP)->m_HordeTowersControlled--;
        sWorld.SendZoneText(OutdoorPvPEPBuffZones[0], sObjectMgr.GetOregonStringForDBCLocale(EP_LANG_LOOSE_H[m_TowerType]));
        break;
    case OBJECTIVESTATE_NEUTRAL_ALLIANCE_CHALLENGE:
    case OBJECTIVESTATE_ALLIANCE_HORDE_CHALLENGE:
        field = EP_MAP_N_A[m_TowerType];
        break;
    case OBJECTIVESTATE_NEUTRAL_HORDE_CHALLENGE:
    case OBJECTIVESTATE_HORDE_ALLIANCE_CHALLENGE:
        field = EP_MAP_N_H[m_TowerType];
        break;
    }

    // send world state update
    if (field)
    {
        m_PvP->SendUpdateWorldState(field, 0);
        field = 0;
    }

    uint32 artkit = 21;
    switch (m_State)
    {
    case OBJECTIVESTATE_NEUTRAL:
        field = EP_MAP_N[m_TowerType];
        break;
    case OBJECTIVESTATE_ALLIANCE:
        field = EP_MAP_A[m_TowerType];
        artkit = 2;
        if (((OutdoorPvPEP*)m_PvP)->m_AllianceTowersControlled < 4)
            ((OutdoorPvPEP*)m_PvP)->m_AllianceTowersControlled++;
        sWorld.SendZoneText(OutdoorPvPEPBuffZones[0], sObjectMgr.GetOregonStringForDBCLocale(EP_LANG_CAPTURE_A[m_TowerType]));
        break;
    case OBJECTIVESTATE_HORDE:
        field = EP_MAP_H[m_TowerType];
        artkit = 1;
        if (((OutdoorPvPEP*)m_PvP)->m_HordeTowersControlled < 4)
            ((OutdoorPvPEP*)m_PvP)->m_HordeTowersControlled++;
        sWorld.SendZoneText(OutdoorPvPEPBuffZones[0], sObjectMgr.GetOregonStringForDBCLocale(EP_LANG_CAPTURE_H[m_TowerType]));
        break;
    case OBJECTIVESTATE_NEUTRAL_ALLIANCE_CHALLENGE:
    case OBJECTIVESTATE_ALLIANCE_HORDE_CHALLENGE:
        field = EP_MAP_N_A[m_TowerType];
        break;
    case OBJECTIVESTATE_NEUTRAL_HORDE_CHALLENGE:
    case OBJECTIVESTATE_HORDE_ALLIANCE_CHALLENGE:
        field = EP_MAP_N_H[m_TowerType];
        break;
    }

    UpdateTowerEvents();

    GameObject* flag = HashMapHolder<GameObject>::Find(m_Objects[m_TowerType]);
    GameObject* flag2 = HashMapHolder<GameObject>::Find(m_Objects[m_TowerType + EP_TOWER_NUM]);
    if (m_capturePoint)
        m_capturePoint->SetGoArtKit(artkit);
    if (flag)
        flag->SetGoArtKit(artkit);
    if (flag2)
        flag2->SetGoArtKit(artkit);

    // send world state update
    if (field)
        m_PvP->SendUpdateWorldState(field, 1);

    // complete quest objective
    if (m_State == OBJECTIVESTATE_ALLIANCE || m_State == OBJECTIVESTATE_HORDE)
        SendObjectiveComplete(EP_CREDITMARKER[m_TowerType], 0);
}

void OPvPCapturePointEP::SendChangePhase()
{
    SendUpdateWorldState(EP_UI_TOWER_SLIDER_N, m_neutralValuePct);
    // send these updates to only the ones in this objective
    uint32 phase = (uint32)ceil((m_value + m_maxValue) / (2 * m_maxValue) * 100.0f);
    SendUpdateWorldState(EP_UI_TOWER_SLIDER_POS, phase);
    SendUpdateWorldState(EP_UI_TOWER_SLIDER_DISPLAY, 1);
}

void OPvPCapturePointEP::FillInitialWorldStates(WorldPacket& data)
{
    switch (m_State)
    {
    case OBJECTIVESTATE_ALLIANCE:
        data << uint32(EP_MAP_N[m_TowerType]) << uint32(0);
        data << uint32(EP_MAP_A[m_TowerType]) << uint32(1);
        data << uint32(EP_MAP_H[m_TowerType]) << uint32(0);
        data << uint32(EP_MAP_N_A[m_TowerType]) << uint32(0);
        data << uint32(EP_MAP_N_H[m_TowerType]) << uint32(0);
        break;
    case OBJECTIVESTATE_HORDE:
        data << uint32(EP_MAP_N[m_TowerType]) << uint32(0);
        data << uint32(EP_MAP_A[m_TowerType]) << uint32(0);
        data << uint32(EP_MAP_H[m_TowerType]) << uint32(1);
        data << uint32(EP_MAP_N_A[m_TowerType]) << uint32(0);
        data << uint32(EP_MAP_N_H[m_TowerType]) << uint32(0);
        break;
    case OBJECTIVESTATE_NEUTRAL_ALLIANCE_CHALLENGE:
    case OBJECTIVESTATE_ALLIANCE_HORDE_CHALLENGE:
        data << uint32(EP_MAP_N[m_TowerType]) << uint32(0);
        data << uint32(EP_MAP_A[m_TowerType]) << uint32(0);
        data << uint32(EP_MAP_H[m_TowerType]) << uint32(0);
        data << uint32(EP_MAP_N_A[m_TowerType]) << uint32(1);
        data << uint32(EP_MAP_N_H[m_TowerType]) << uint32(0);
        break;
    case OBJECTIVESTATE_NEUTRAL_HORDE_CHALLENGE:
    case OBJECTIVESTATE_HORDE_ALLIANCE_CHALLENGE:
        data << uint32(EP_MAP_N[m_TowerType]) << uint32(0);
        data << uint32(EP_MAP_A[m_TowerType]) << uint32(0);
        data << uint32(EP_MAP_H[m_TowerType]) << uint32(0);
        data << uint32(EP_MAP_N_A[m_TowerType]) << uint32(0);
        data << uint32(EP_MAP_N_H[m_TowerType]) << uint32(1);
        break;
    case OBJECTIVESTATE_NEUTRAL:
    default:
        data << uint32(EP_MAP_N[m_TowerType]) << uint32(1);
        data << uint32(EP_MAP_A[m_TowerType]) << uint32(0);
        data << uint32(EP_MAP_H[m_TowerType]) << uint32(0);
        data << uint32(EP_MAP_N_A[m_TowerType]) << uint32(0);
        data << uint32(EP_MAP_N_H[m_TowerType]) << uint32(0);
        break;
    }
}

bool OPvPCapturePointEP::HandlePlayerEnter(Player* plr)
{
    if (OPvPCapturePoint::HandlePlayerEnter(plr))
    {
        plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_DISPLAY, 1);
        uint32 phase = (uint32)ceil((m_value + m_maxValue) / (2 * m_maxValue) * 100.0f);
        plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_POS, phase);
        plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_N, m_neutralValuePct);
        return true;
    }
    return false;
}

void OPvPCapturePointEP::HandlePlayerLeave(Player* plr)
{
    plr->SendUpdateWorldState(EP_UI_TOWER_SLIDER_DISPLAY, 0);
    OPvPCapturePoint::HandlePlayerLeave(plr);
}

// Tower specific events
void OPvPCapturePointEP::UpdateTowerEvents()
{
    if (m_State == OBJECTIVESTATE_ALLIANCE || m_State == OBJECTIVESTATE_HORDE)
    {
        uint32 team = (m_State == OBJECTIVESTATE_ALLIANCE) ? ALLIANCE : HORDE;
        switch (m_TowerType)
        {
        case EP_TOWER_CROWNGUARD:
            LinkGraveYard(team);
            break;
        case EP_TOWER_EASTWALL:
            SummonSupportUnits(team);
            break;
        case EP_TOWER_NORTHPASS:
            SummonShrine(team);
            break;
        case EP_TOWER_PLAGUEWOOD:
            SummonFlightMaster(team);
            break;
        default:
            break;
        }
    }
    else if (m_OldState == OBJECTIVESTATE_ALLIANCE || m_OldState == OBJECTIVESTATE_HORDE)
    {
        switch (m_TowerType)
        {
        case EP_TOWER_CROWNGUARD:
            UnlinkGraveYard();
            break;
        case EP_TOWER_EASTWALL:
            UnsummonSupportUnits();
            break;
        case EP_TOWER_NORTHPASS:
            UnsummonShrine();
            break;
        case EP_TOWER_PLAGUEWOOD:
            UnsummonFlightMaster();
            break;
        default:
            break;
        }
    }
}

void OPvPCapturePointEP::LinkGraveYard(uint32 team)
{
    if (EP_TOWER_EVENT_TEAM[EP_TOWER_CROWNGUARD] != team)
    {
        EP_TOWER_EVENT_TEAM[EP_TOWER_CROWNGUARD] = team;
        sObjectMgr.RemoveGraveYardLink(EP_GraveYardId, EP_GraveYardZone, team, false);
        sObjectMgr.AddGraveYardLink(EP_GraveYardId, EP_GraveYardZone, team, false);
    }
}

void OPvPCapturePointEP::UnlinkGraveYard()
{
    sObjectMgr.RemoveGraveYardLink(EP_GraveYardId, EP_GraveYardZone, EP_TOWER_EVENT_TEAM[EP_TOWER_CROWNGUARD], false);
    EP_TOWER_EVENT_TEAM[EP_TOWER_CROWNGUARD] = 0;
}

void OPvPCapturePointEP::SummonSupportUnits(uint32 team)
{
    if (EP_TOWER_EVENT_TEAM[EP_TOWER_EASTWALL] != team)
    {
        EP_TOWER_EVENT_TEAM[EP_TOWER_EASTWALL] = team;
        const creature_type* ct = NULL;
        if (team == ALLIANCE)
            ct = EP_EWT_Summons_A;
        else
            ct = EP_EWT_Summons_H;

        for (uint8 i = 0; i < EP_EWT_NUM_CREATURES; ++i)
        {
            DelCreature(i);
            AddCreature(i, ct[i].entry, ct[i].teamval, ct[i].map, ct[i].x, ct[i].y, ct[i].z, ct[i].o, 1000000);
        }
    }
}

void OPvPCapturePointEP::UnsummonSupportUnits()
{
    for (uint8 i = 0; i < EP_EWT_NUM_CREATURES; ++i)
        DelCreature(i);
    EP_TOWER_EVENT_TEAM[EP_TOWER_EASTWALL] = 0;
}

void OPvPCapturePointEP::SummonShrine(uint32 team)
{
    if (EP_TOWER_EVENT_TEAM[EP_TOWER_NORTHPASS] != team)
    {
        EP_TOWER_EVENT_TEAM[EP_TOWER_NORTHPASS] = team;
        DelObject(EP_NPT_SHRINE);
        AddObject(EP_NPT_SHRINE, EP_NPT_LordaeronShrine.entry, 0, EP_NPT_LordaeronShrine.map, EP_NPT_LordaeronShrine.x, EP_NPT_LordaeronShrine.y, EP_NPT_LordaeronShrine.z, EP_NPT_LordaeronShrine.o, EP_NPT_LordaeronShrine.rot0, EP_NPT_LordaeronShrine.rot1, EP_NPT_LordaeronShrine.rot2, EP_NPT_LordaeronShrine.rot3);
        GameObject* go = HashMapHolder<GameObject>::Find(m_Objects[EP_NPT_SHRINE]);
        if (go)
            go->SetUInt32Value(GAMEOBJECT_FACTION, (team == ALLIANCE ? 84 : 83));

        DelObject(EP_NPT_SHRINE_AURA);
        uint32 shrineEntry = (team == ALLIANCE ? EP_NPT_LordaeronShrineAura.entry : EP_NPT_LordaeronShrineAura.entry + 1);
        AddObject(EP_NPT_SHRINE_AURA, shrineEntry, 0, EP_NPT_LordaeronShrineAura.map, EP_NPT_LordaeronShrineAura.x, EP_NPT_LordaeronShrineAura.y, EP_NPT_LordaeronShrineAura.z, EP_NPT_LordaeronShrineAura.o, EP_NPT_LordaeronShrineAura.rot0, EP_NPT_LordaeronShrineAura.rot1, EP_NPT_LordaeronShrineAura.rot2, EP_NPT_LordaeronShrineAura.rot3);
    }
}

void OPvPCapturePointEP::UnsummonShrine()
{
    DelObject(EP_NPT_SHRINE);
    DelObject(EP_NPT_SHRINE_AURA);
    EP_TOWER_EVENT_TEAM[EP_TOWER_NORTHPASS] = 0;
}

void OPvPCapturePointEP::SummonFlightMaster(uint32 team)
{
    if (EP_TOWER_EVENT_TEAM[EP_TOWER_PLAGUEWOOD] != team)
    {
        EP_TOWER_EVENT_TEAM[EP_TOWER_PLAGUEWOOD] = team;
        DelCreature(EP_PWT_FLIGHTMASTER);
        AddCreature(EP_PWT_FLIGHTMASTER, EP_PWT_FlightMaster.entry, team, EP_PWT_FlightMaster.map, EP_PWT_FlightMaster.x, EP_PWT_FlightMaster.y, EP_PWT_FlightMaster.z, EP_PWT_FlightMaster.o);

        // Change the flightmasters's faction to horde and display a red aura around the npc
        Creature* c = HashMapHolder<Creature>::Find(m_Creatures[EP_PWT_FLIGHTMASTER]);
        if (c && team == HORDE)
        {
            c->SetUInt32Value(UNIT_FIELD_FACTIONTEMPLATE, c->GetCreatureTemplate()->faction);
            c->SetUInt32Value(UNIT_FIELD_AURA, EP_PWT_FlightMasterAura);
        }
    }
}

void OPvPCapturePointEP::UnsummonFlightMaster()
{
    DelCreature(EP_PWT_FLIGHTMASTER);
    EP_TOWER_EVENT_TEAM[EP_TOWER_PLAGUEWOOD] = 0;
}

