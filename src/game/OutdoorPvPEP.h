/*
 * Copyright (C) 2008 Trinity <http://www.trinitycore.org/>
 *
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

#ifndef OUTDOOR_PVP_EP_
#define OUTDOOR_PVP_EP_

#include "OutdoorPvPImpl.h"

#define OutdoorPvPEPBuffZonesNum 3
#define OutdoorPvPEPTeamsBuffNum 4
                                                         //  Eastern Plaguelands, Stratholme and Scholomance
const uint32 OutdoorPvPEPBuffZones[OutdoorPvPEPBuffZonesNum] = {139, 2017, 2057};

const int EP_AllianceBuffs[OutdoorPvPEPTeamsBuffNum] = {11413, 11414, 11415, 1386};

const int EP_HordeBuffs[OutdoorPvPEPTeamsBuffNum] = {30880, 30683, 30682, 29520};

const uint32 EP_GraveYardZone = 139;

const uint32 EP_GraveYardId = 927;

enum OutdoorPvPEPTowerType{
    EP_TOWER_CROWNGUARD = 0,
    EP_TOWER_EASTWALL,
    EP_TOWER_NORTHPASS,
    EP_TOWER_PLAGUEWOOD,
    EP_TOWER_NUM
};

const uint32 EP_CREDITMARKER[EP_TOWER_NUM] = {17689,17690,17696,17698};

const uint32 EP_MAP_N[EP_TOWER_NUM] = {2355,2361,2352,2353};
const uint32 EP_MAP_A[EP_TOWER_NUM] = {2378,2354,2372,2370};
const uint32 EP_MAP_H[EP_TOWER_NUM] = {2379,2356,2373,2371};
const uint32 EP_MAP_N_A[EP_TOWER_NUM] = {2374,2359,2362,2366};
const uint32 EP_MAP_N_H[EP_TOWER_NUM] = {2375,2360,2363,2353}; // plaguewood tower has incorrect display value (2353 as well as logical 2367 is wrong) 

const uint32 EP_TAXI_NODE[EP_TOWER_NUM] = {84,85,86,87};

enum OutdoorPvPEPWorldStates{
    EP_UI_TOWER_SLIDER_DISPLAY = 2426,
    EP_UI_TOWER_SLIDER_POS = 2427,
    EP_UI_TOWER_SLIDER_N = 2428,

    EP_UI_TOWER_COUNT_A = 2327,
    EP_UI_TOWER_COUNT_H = 2328
};

enum EP_Summons {
    EP_EWT_COMMANDER = 0,
    EP_EWT_SOLDIER1,
    EP_EWT_SOLDIER2,
    EP_EWT_SOLDIER3,
    EP_EWT_SOLDIER4,
    EP_PWT_FLIGHTMASTER,
};

enum EP_GoSummons {
    EP_NPT_BUFF = 0,
    EP_NPT_FLAGS,
    EP_EWT_FLAGS,
    EP_CGT_FLAGS,
    EP_PWT_FLAGS
};

const go_type EPCapturePoints[EP_TOWER_NUM] = {
    {182096,0,1860.85,-3731.23,196.716,-2.53214,0.033967,-0.131914,0.944741,-0.298177},
    {182097,0,2574.51,-4794.89,144.704,-1.45003,-0.097056,0.095578,-0.656229,0.742165},
    {181899,0,3181.08,-4379.36,174.123,-2.03472,-0.065392,0.119494,-0.842275,0.521553},
    {182098,0,2962.71,-3042.31,154.789,2.08426,-0.074807,-0.113837,0.855928,0.49883}
};

const go_type EPTowerFlags[EP_TOWER_NUM] = {
    {182106,0,1838.42,-3703.56,167.713,0.890118,0,0,0.430511,0.902585},
    {182106,0,2569.60,-4772.93,115.399,2.72271,0,0,0.978148,0.207912},
    {182106,0,3148.17,-4365.51,145.029,1.53589,0,0,0.694658,0.71934},
    {182106,0,2992.63,-3022.95,125.593,3.03687,0,0,0.99863,0.052336}
};

const uint32 EPTowerPlayerEnterEvents[EP_TOWER_NUM] = {10705,10691,10699,10701};

const uint32 EPTowerPlayerLeaveEvents[EP_TOWER_NUM] = {10704,10692,10698,10700};

const uint32 EP_NUM_CREATURES = 6;
const uint32 EP_EWT_NUM_CREATURES = 5;

// one lordaeron commander, 4 soldiers
// should be spawned at EWT and follow a path, but trans-grid pathing isn't safe, so summon them directly at NPT
const creature_type EP_EWT_Summons_A[EP_EWT_NUM_CREATURES] = {
    {17635,469,0, 3167.61,-4352.09,138.20,4.5811},
    {17647,469,0, 3172.74,-4352.99,139.14,4.9873},
    {17647,469,0, 3165.89,-4354.46,138.67,3.7244},
    {17647,469,0, 3164.65,-4350.26,138.22,2.4794},
    {17647,469,0, 3169.91,-4349.68,138.37,0.7444}
};

const creature_type EP_EWT_Summons_H[EP_EWT_NUM_CREATURES] = {
    {17995,67,0, 3167.61,-4352.09,138.20,4.5811},
    {17996,67,0, 3172.74,-4352.99,139.14,4.9873},
    {17996,67,0, 3165.89,-4354.46,138.67,3.7244},
    {17996,67,0, 3164.65,-4350.26,138.22,2.4794},
    {17996,67,0, 3169.91,-4349.68,138.37,0.7444}
};

enum EP_TowerStates {
    EP_TS_N = 1,
    EP_TS_A = 32,
    EP_TS_H = 64
};

// when spawning, pay attention at setting the faction manually!
const creature_type EP_PWT_FlightMaster = {17209,0,0,2987.5,-3049.11,120.126,5.75959};

// after spawning, modify the faction so that only the controller will be able to use it with SetUInt32Value(GAMEOBJECT_FACTION, faction_id);
const go_type EP_NPT_LordaeronShrine = {181682,0,3167.72,-4355.91,138.785,1.69297,0,0,0.748956,0.66262};

class OPvPCapturePointEP : public OPvPCapturePoint
{
public:
    OPvPCapturePointEP(OutdoorPvP * pvp, OutdoorPvPEPTowerType type);
    void ChangeState();
    void SendChangePhase();
    void FillInitialWorldStates(WorldPacket & data);
    // used when player is activated/inactivated in the area
    bool HandlePlayerEnter(Player * plr);
    void HandlePlayerLeave(Player * plr);
protected:
    void UpdateTowerEvents();
    void LinkGraveYard(uint32 team);
    void SummonShrine(uint32 team);
    void SummonSupportUnits(uint32 team);
    void SummonFlightMaster(uint32 team);
    bool HandleGossipOption(Player *plr, uint64 guid, uint32 gossipid);
    bool CanTalkTo(Player * plr, Creature * c, GossipOption &gso);
    void UnlinkGraveYard();
    void UnsummonShrine();
    void UnsummonSupportUnits();
    void UnsummonFlightMaster();

	uint32 EP_TOWER_EVENT_TEAM[EP_TOWER_NUM];
private:
    OutdoorPvPEPTowerType m_TowerType;
};

class OutdoorPvPEP : public OutdoorPvP
{
friend class OPvPCapturePointEP;
public:
    OutdoorPvPEP();
    bool SetupOutdoorPvP();
    void HandlePlayerEnterZone(Player *plr, uint32 zone);
    void HandlePlayerLeaveZone(Player *plr, uint32 zone);
    bool Update(uint32 diff);
    void FillInitialWorldStates(WorldPacket &data);
    void SendRemoveWorldStates(Player * plr);
private:
    // how many towers are controlled
    uint32 m_AllianceTowersControlled;
    uint32 m_HordeTowersControlled;
};

#endif
