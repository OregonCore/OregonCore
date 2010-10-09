/* Copyright (C) 2009 OREGON <http://www.oregoncore.com/>
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

/* ScriptData
SDName: Boss_Kiljaeden
SD%Complete: 70
SDComment: Phase4, Phase5, Shadow Spike, Armageddon
SDCategory: Sunwell_Plateau
EndScriptData */

//TODO rewrite Armageddon
//TODO Remove blue visual from Orbs on reset and if it is used

#include "ScriptPCH.h"
#include "sunwell_plateau.h"
#include <math.h>

/*** Speech and sounds***/
enum Yells
{
    // These are used throughout Sunwell and Magisters(?). Players can hear this while running through the instances.
    SAY_KJ_OFFCOMBAT1                           = -1580066,
    SAY_KJ_OFFCOMBAT2                           = -1580067,
    SAY_KJ_OFFCOMBAT3                           = -1580068,
    SAY_KJ_OFFCOMBAT4                           = -1580069,
    SAY_KJ_OFFCOMBAT5                           = -1580070,

    // Encounter speech and sounds
    SAY_KJ_EMERGE                               = -1580071,
    SAY_KJ_SLAY1                                = -1580072,
    SAY_KJ_SLAY2                                = -1580073,
    SAY_KJ_REFLECTION1                          = -1580074,
    SAY_KJ_REFLECTION2                          = -1580075,
    SAY_KJ_DARKNESS1                            = -1580076,
    SAY_KJ_DARKNESS2                            = -1580077,
    SAY_KJ_DARKNESS3                            = -1580078,
    SAY_KJ_PHASE3                               = -1580079,
    SAY_KJ_PHASE4                               = -1580080,
    SAY_KJ_PHASE5                               = -1580081,
    SAY_KJ_DEATH                                = -1580093,
    EMOTE_KJ_DARKNESS                           = -1580094,

    /*** Kalecgos - Anveena speech at the beginning of Phase 5; Anveena's sacrifice ***/
    SAY_KALECGOS_AWAKEN                         = -1580082,
    SAY_ANVEENA_IMPRISONED                      = -1580083,
    SAY_KALECGOS_LETGO                          = -1580084,
    SAY_ANVEENA_LOST                            = -1580085,
    SAY_KALECGOS_FOCUS                          = -1580086,
    SAY_ANVEENA_KALEC                           = -1580087,
    SAY_KALECGOS_FATE                           = -1580088,
    SAY_ANVEENA_GOODBYE                         = -1580089,
    SAY_KALECGOS_GOODBYE                        = -1580090,
    SAY_KALECGOS_ENCOURAGE                      = -1580091,

    /*** Kalecgos says throughout the fight ***/
    SAY_KALECGOS_JOIN                           = -1580092,
    SAY_KALEC_ORB_READY1                        = -1580095,
    SAY_KALEC_ORB_READY2                        = -1580096,
    SAY_KALEC_ORB_READY3                        = -1580097,
    SAY_KALEC_ORB_READY4                        = -1580098
};

/*** Spells used during the encounter ***/
enum Spells
{
    /* Hand of the Deceiver's spells and cosmetics */
    SPELL_SHADOW_BOLT_VOLLEY                    = 45770, // ~30 yard range Shadow Bolt Volley for ~2k(?) damage
    SPELL_SHADOW_INFUSION                       = 45772, // They gain this at 20% - Immunity to Stun/Silence and makes them look angry!
    SPELL_FELFIRE_PORTAL                        = 46875, // Creates a portal that spawns Felfire Fiends (LIVE FOR THE SWARM!1 FOR THE OVERMIND!)
    SPELL_SHADOW_CHANNELING                     = 46757, // Channeling animation out of combat

    /* Volatile Felfire Fiend's spells */
    SPELL_FELFIRE_FISSION                       = 45779, // Felfire Fiends explode when they die or get close to target.

    /* Kil'Jaeden's spells and cosmetics */
    SPELL_TRANS                                 = 23188, // Surprisingly, this seems to be the right spell.. (Where is it used?)
    SPELL_REBIRTH                               = 44200, // Emerge from the Sunwell
    SPELL_SOUL_FLAY                             = 45442, // 9k Shadow damage over 3 seconds. Spammed throughout all the fight.
    SPELL_SOUL_FLAY_SLOW                        = 47106,
    SPELL_LEGION_LIGHTNING                      = 45664, // Chain Lightning, 4 targets, ~3k Shadow damage, 1.5k mana burn
    SPELL_FIRE_BLOOM                            = 45641, // Places a debuff on 5 raid members, which causes them to deal 2k Fire damage to nearby allies and selves. MIGHT NOT WORK
    SPELL_DESTROY_ALL_DRAKES                    = 46707, // when he use it?

    SPELL_SINISTER_REFLECTION                   = 45785, // Summon shadow copies of 5 raid members that fight against KJ's enemies//dont work
                                                //  45892  // right one for SPELL_SINISTER_REFLECTION but no EffectScriptEffect
    SPELL_COPY_WEAPON                           = 41055, // }
    SPELL_COPY_WEAPON2                          = 41054, // }
    SPELL_COPY_OFFHAND                          = 45206, // }- Spells used in Sinister Reflection creation
    SPELL_COPY_OFFHAND_WEAPON                   = 45205, // }

    SPELL_SHADOW_SPIKE                          = 46680, // Bombard random raid members with Shadow Spikes (Very similar to Void Reaver orbs)
    SPELL_FLAME_DART                            = 45737, // Bombards the raid with flames every 3(?) seconds
    SPELL_DARKNESS_OF_A_THOUSAND_SOULS          = 46605, // Begins a 8-second channeling, after which he will deal 50'000 damage to the raid
    SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE   = 45657,

    /* Armageddon spells wrong visual */
    SPELL_ARMAGEDDON_TRIGGER                    = 45909, // Meteor spell trigger missile should cast Creature on himself
    SPELL_ARMAGEDDON_VISUAL                     = 45911, // Does the hellfire visual to indicate where the meteor missle lands
    SPELL_ARMAGEDDON_VISUAL2                    = 45914, // Does the light visual to indicate where the meteor missle lands
    SPELL_ARMAGEDDON_VISUAL3                    = 24207, // This shouldn't correct but same as seen on the movie
    SPELL_ARMAGEDDON_SUMMON_TRIGGER             = 45921, // Summons the triggers that cast the spells on himself need random target select
    SPELL_ARMAGEDDON_DAMAGE                     = 45915, // This does the area damage

    /* Shield Orb Spells*/
    SPELL_SHADOW_BOLT                           = 45680, //45679 would be correct but triggers to often //TODO fix console error

    /* Anveena's spells and cosmetics (Or, generally, everything that has "Anveena" in name) */
    SPELL_ANVEENA_PRISON                        = 46367, // She hovers locked within a bubble
    SPELL_ANVEENA_ENERGY_DRAIN                  = 46410, // Sunwell energy glow animation (Control mob uses this)
    SPELL_SACRIFICE_OF_ANVEENA                  = 46474, // This is cast on Kil'Jaeden when Anveena sacrifices herself into the Sunwell

    /* Sinister Reflection Spells */
    SPELL_SR_CURSE_OF_AGONY                     = 46190,
    SPELL_SR_SHADOW_BOLT                        = 47076,

    SPELL_SR_EARTH_SHOCK                        = 47071,

    SPELL_SR_FIREBALL                           = 47074,

    SPELL_SR_HEMORRHAGE                         = 45897,

    SPELL_SR_HOLY_SHOCK                         = 38921,
    SPELL_SR_HAMMER_OF_JUSTICE                  = 37369,

    SPELL_SR_HOLY_SMITE                         = 47077,
    SPELL_SR_RENEW                              = 47079,

    SPELL_SR_SHOOT                              = 16496,
    SPELL_SR_MULTI_SHOT                         = 48098,
    SPELL_SR_WING_CLIP                          = 40652,

    SPELL_SR_WHIRLWIND                          = 17207,

    SPELL_SR_MOONFIRE                           = 47072,
    //SPELL_SR_PLAGU STRIKE                       = 58843, Dk Spell!

    /*** Other Spells (used by players, etc) ***/
    SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT          = 45839, // Possess the blue dragon from the orb to help the raid.
    SPELL_ENTROPIUS_BODY                        = 46819, // Visual for Entropius at the Epilogue
    SPELL_RING_OF_BLUE_FLAMES                   = 45825  //Cast this spell when the go is activated
};

/*** Error messages ***/
#define ERROR_KJ_NOT_SUMMONED "TSCR ERROR: Unable to summon Kil'Jaeden for some reason"

/*** Others ***/
#define FLOOR_Z         28.050388
#define SHIELD_ORB_Z    45.000

enum Phase
{
    PHASE_DECEIVERS     = 1, // Fight 3 adds
    PHASE_NORMAL        = 2, // Kil'Jaeden emerges from the sunwell
    PHASE_DARKNESS      = 3, // At 85%, he gains few abilities; Kalecgos joins the fight
    PHASE_ARMAGEDDON    = 4, // At 55%, he gains even more abilities
    PHASE_SACRIFICE     = 5, // At 25%, Anveena sacrifices herself into the Sunwell; at this point he becomes enraged and has *significally* shorter cooldowns.
};

//Timers
enum KilJaedenTimers
{
    TIMER_SPEECH           = 0,

    //Phase 2 Timer
    TIMER_SOUL_FLAY        = 1,
    TIMER_LEGION_LIGHTNING = 2,
    TIMER_FIRE_BLOOM       = 3,
    TIMER_SUMMON_SHILEDORB = 4,

    //Phase 3 Timer
    TIMER_SHADOW_SPIKE     = 5,
    TIMER_FLAME_DART       = 6,
    TIMER_DARKNESS         = 7,
    TIMER_ORBS_EMPOWER     = 8,

    //Phase 4 Timer
    TIMER_ARMAGEDDON       = 9
};

// Locations of the Hand of Deceiver adds
float DeceiverLocations[3][3]=
{
    {1682.045, 631.299, 5.936},
    {1684.099, 618.848, 0.589},
    {1694.170, 612.272, 1.416},
};

// Locations, where Shield Orbs will spawn
float ShieldOrbLocations[4][2]=
{
    {1698.900, 627.870},    // middle pont of Sunwell
    {12, 3.14},             // First one spawns northeast of KJ
    {12, 3.14/0.7},         // Second one spawns southeast
    {12, 3.14*3.8}          // Third one spawns (?)
};

float OrbLocations[4][5] = {
    (1694.48, 674.29,  28.0502, 4.86985),
    (1745.68, 621.823, 28.0505, 2.93777),
    (1704.14, 583.591, 28.1696, 1.59003),
    (1653.12, 635.41,  28.0932, 0.0977725),
};

struct Speech
{
    int32 textid;
    uint32 pCreature, timer;
};

// Timers
static Speech Sacrifice[]=
{
    {SAY_KALECGOS_AWAKEN,       CREATURE_KALECGOS,  5000},
    {SAY_ANVEENA_IMPRISONED,    CREATURE_ANVEENA,   5000},
    {SAY_KALECGOS_LETGO,        CREATURE_KALECGOS,  8000},
    {SAY_ANVEENA_LOST,          CREATURE_ANVEENA,   5000},
    {SAY_KALECGOS_FOCUS,        CREATURE_KALECGOS,  7000},
    {SAY_ANVEENA_KALEC,         CREATURE_ANVEENA,   2000},
    {SAY_KALECGOS_FATE,         CREATURE_KALECGOS,  3000},
    {SAY_ANVEENA_GOODBYE,       CREATURE_ANVEENA,   6000},
    {SAY_KALECGOS_GOODBYE,      CREATURE_KALECGOS,  12000},
    {SAY_KJ_PHASE5,             CREATURE_KILJAEDEN, 8000},
    {SAY_KALECGOS_ENCOURAGE,    CREATURE_KALECGOS,  5000}
};

class AllOrbsInGrid
{
public:
    AllOrbsInGrid() {}
    bool operator() (GameObject* go)
    {
        if (go->GetEntry() == GAMEOBJECT_ORB_OF_THE_BLUE_DRAGONFLIGHT)
            return true;
        return false;
    }
};

//AI for Kalecgos
struct boss_kalecgos_kjAI : public ScriptedAI
{
    boss_kalecgos_kjAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    GameObject* Orb[4];
    ScriptedInstance* pInstance;
    uint8 OrbsEmpowered;
    uint8 EmpowerCount;

    bool Searched;

    void InitializeAI(){
        for (uint8 i = 0; i < 4; ++i)
            Orb[i] = NULL;
        FindOrbs();
        OrbsEmpowered = 0;
        EmpowerCount = 0;
        me->AddUnitMovementFlag(MOVEFLAG_ONTRANSPORT | MOVEFLAG_LEVITATING);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->setActive(true);
        Searched = false;
        FindOrbs();
    }

    void Reset(){}

    void EnterCombat(Unit* who) {}

    void FindOrbs()
    {
        CellPair pair(Oregon::ComputeCellPair(me->GetPositionX(), me->GetPositionY()));
        Cell cell(pair);
        cell.data.Part.reserved = ALL_DISTRICT;
        cell.SetNoCreate();
        std::list<GameObject*> orbList;
        AllOrbsInGrid check;
        Oregon::GameObjectListSearcher<AllOrbsInGrid> searcher(orbList, check);
        TypeContainerVisitor<Oregon::GameObjectListSearcher<AllOrbsInGrid>, GridTypeMapContainer> visitor(searcher);
        cell.Visit(pair, visitor, *(me->GetMap()));
        if (orbList.empty())
            return;
        uint8 i = 0;
        for (std::list<GameObject*>::iterator itr = orbList.begin(); itr != orbList.end(); ++itr, ++i){
            Orb[i] = GameObject::GetGameObject(*me, (*itr)->GetGUID());
        }
    }

    void ResetOrbs()
    {
        me->RemoveDynObject(SPELL_RING_OF_BLUE_FLAMES);
        for (uint8 i = 0; i < 4; ++i)
            if (Orb[i]) Orb[i]->SetUInt32Value(GAMEOBJECT_FACTION, 0);
    }

    void EmpowerOrb(bool all)
    {
        if (!Orb[OrbsEmpowered])
            return;
        uint8 random = rand()%3;
        if (all)
        {
            me->RemoveDynObject(SPELL_RING_OF_BLUE_FLAMES);
            for (uint8 i = 0; i < 4; ++i)
            {
                if (!Orb[i]) return;
                Orb[i]->CastSpell(me, SPELL_RING_OF_BLUE_FLAMES);
                Orb[i]->SetUInt32Value(GAMEOBJECT_FACTION, 35);
                Orb[i]->setActive(true);
                Orb[i]->Refresh();
            }
        }
        else
        {
            float x,y,z, dx,dy,dz;
            Orb[random]->GetPosition(x,y,z);
            for (uint8 i = 0; i < 4; ++i){
                DynamicObject* Dyn = me->GetDynObject(SPELL_RING_OF_BLUE_FLAMES);
                if (Dyn){
                    Dyn->GetPosition(dx,dy,dz);
                    if (x == dx && dy == y && dz == z){
                        Dyn->RemoveFromWorld();
                        break;
                     }
                 }
            }
        Orb[random]->CastSpell(me, SPELL_RING_OF_BLUE_FLAMES);
        Orb[random]->SetUInt32Value(GAMEOBJECT_FACTION, 35);
        Orb[random]->setActive(true);
        Orb[random]->Refresh();
        ++OrbsEmpowered;
        }
        ++EmpowerCount;

        switch(EmpowerCount)

        {
            case 1: DoScriptText(SAY_KALEC_ORB_READY1, me); break;
            case 2: DoScriptText(SAY_KALEC_ORB_READY2, me); break;
            case 3: DoScriptText(SAY_KALEC_ORB_READY3, me); break;
            case 4: DoScriptText(SAY_KALEC_ORB_READY4, me); break;
        }
    }

    void UpdateAI(const uint32 diff){
        if (!Searched){
            FindOrbs();
            Searched = true;
            }

        if (OrbsEmpowered == 4) OrbsEmpowered = 0;
    }
};

CreatureAI* GetAI_boss_kalecgos_kj(Creature* pCreature)
{
    return new boss_kalecgos_kjAI (pCreature);
}

bool GOHello_go_orb_of_the_blue_flight(Player *plr, GameObject* go)
{
    if (go->GetUInt32Value(GAMEOBJECT_FACTION) == 35){
        ScriptedInstance* pInstance = (go->GetInstanceData());
        float x,y,z, dx,dy,dz;
        go->SummonCreature(CREATURE_POWER_OF_THE_BLUE_DRAGONFLIGHT, plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 121000);
        plr->CastSpell(plr, SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT, true);
        go->SetUInt32Value(GAMEOBJECT_FACTION, 0);
        Unit* Kalec = CAST_CRE(Unit::GetUnit(*plr, pInstance->GetData64(DATA_KALECGOS_KJ)));
        //Kalec->RemoveDynObject(SPELL_RING_OF_BLUE_FLAMES);
        go->GetPosition(x,y,z);
        for (uint8 i = 0; i < 4; ++i){
            DynamicObject* Dyn = Kalec->GetDynObject(SPELL_RING_OF_BLUE_FLAMES);
            if (Dyn){
                Dyn->GetPosition(dx,dy,dz);
                if (x == dx && dy == y && dz == z){
                    Dyn->RemoveFromWorld();
                    break;
                }
            }
        }
        go->Refresh();
    }
    return true;
}

//AI for Kil'jaeden Event Controller
struct mob_kiljaeden_controllerAI : public Scripted_NoMovementAI
{
    mob_kiljaeden_controllerAI(Creature* c) : Scripted_NoMovementAI(c), summons(me)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    Creature* KalecKJ;
    SummonList summons;

    bool bSummonedDeceivers;
    bool bKiljaedenDeath;

    uint32 uiRandomSayTimer;
    uint32 phase;
    uint8 deceiverDeathCount;

    void InitializeAI()
    {
        KalecKJ = Unit::GetCreature((*me), pInstance->GetData64(DATA_KALECGOS_KJ));
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->addUnitState(UNIT_STAT_STUNNED);
    }

    void Reset()
    {
        phase = PHASE_DECEIVERS;
        if (KalecKJ)((boss_kalecgos_kjAI*)KalecKJ->AI())->ResetOrbs();
        deceiverDeathCount = 0;
        bSummonedDeceivers = false;
        bKiljaedenDeath = false;
        uiRandomSayTimer = 30000;
        summons.DespawnAll();
    }

    void JustSummoned(Creature* summoned)
    {
        switch(summoned->GetEntry())
        {
            case CREATURE_HAND_OF_THE_DECEIVER:
                summoned->CastSpell(summoned, SPELL_SHADOW_CHANNELING, false);
                break;
            case CREATURE_ANVEENA:
                summoned->AddUnitMovementFlag(MOVEFLAG_ONTRANSPORT | MOVEFLAG_LEVITATING);
                summoned->CastSpell(summoned, SPELL_ANVEENA_PRISON, true);
                summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                break;
            case CREATURE_KILJAEDEN:
                summoned->CastSpell(summoned, SPELL_REBIRTH, false);
                summoned->AddThreat(me->getVictim(), 1.0f);
                break;
        }
        summons.Summon(summoned);
    }

    void UpdateAI(const uint32 diff)
    {
        if (uiRandomSayTimer <= diff && pInstance->GetData(DATA_MURU_EVENT) != DONE && pInstance->GetData(DATA_KILJAEDEN_EVENT) == NOT_STARTED){
            switch(rand()%5){
                case 0: DoScriptText(SAY_KJ_OFFCOMBAT1, me); break;
                case 1: DoScriptText(SAY_KJ_OFFCOMBAT2, me); break;
                case 2: DoScriptText(SAY_KJ_OFFCOMBAT3, me); break;
                case 3: DoScriptText(SAY_KJ_OFFCOMBAT4, me); break;
                case 4: DoScriptText(SAY_KJ_OFFCOMBAT5, me); break;
            }
            uiRandomSayTimer = 30000;
        } else uiRandomSayTimer -= diff;

        if (!bSummonedDeceivers)
        {
            for (uint8 i = 0; i < 3; ++i)
                me->SummonCreature(CREATURE_HAND_OF_THE_DECEIVER, DeceiverLocations[i][0], DeceiverLocations[i][1], FLOOR_Z, DeceiverLocations[i][2], TEMPSUMMON_DEAD_DESPAWN, 0);

            DoSpawnCreature(CREATURE_ANVEENA,  0, 0, 40, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
            DoCast(me, SPELL_ANVEENA_ENERGY_DRAIN);
            bSummonedDeceivers = true;
        }

        if (deceiverDeathCount > 2 && phase == PHASE_DECEIVERS)
        {
            me->RemoveAurasDueToSpell(SPELL_ANVEENA_ENERGY_DRAIN);
            phase = PHASE_NORMAL;
            DoSpawnCreature(CREATURE_KILJAEDEN, 0, 0,0, 0, TEMPSUMMON_MANUAL_DESPAWN, 0);
        }
    }
};

CreatureAI* GetAI_mob_kiljaeden_controller(Creature* pCreature)
{
    return new mob_kiljaeden_controllerAI (pCreature);
}

//AI for Kil'jaeden
struct boss_kiljaedenAI : public Scripted_NoMovementAI
{
    boss_kiljaedenAI(Creature* c) : Scripted_NoMovementAI(c), summons(me)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    SummonList summons;
    Creature*  Kalec;

    uint8 Phase;
    uint8 ActiveTimers;

    uint32 Timer[10];
    uint32 WaitTimer;

    /* Boolean */
    bool IsKalecJoined;
    bool IsInDarkness;
    bool TimerIsDeactivated[10];
    bool IsWaiting;
    bool OrbActivated;

    void Reset()
    {
        // TODO: Fix timers
        Timer[TIMER_SPEECH]       = 26000;

        //Phase 2 Timer
        Timer[TIMER_SOUL_FLAY]        = 20000;
        Timer[TIMER_LEGION_LIGHTNING] = 40000;
        Timer[TIMER_FIRE_BLOOM]       = 30000;
        Timer[TIMER_SUMMON_SHILEDORB] = 45000;

        //Phase 3 Timer
        Timer[TIMER_SHADOW_SPIKE]     = 4000;
        Timer[TIMER_FLAME_DART]       = 3000;
        Timer[TIMER_DARKNESS]         = 45000;
        Timer[TIMER_ORBS_EMPOWER]     = 35000;

        //Phase 4 Timer
        Timer[TIMER_ARMAGEDDON]       = 2000;

        ActiveTimers = 5;
        WaitTimer    = 0;

        Phase = PHASE_DECEIVERS;

        IsKalecJoined = false;
        IsInDarkness  = false;
        IsWaiting     = false;
        OrbActivated  = false;

        Kalec = CAST_CRE(Unit::GetUnit(*me, pInstance->GetData64(DATA_KALECGOS_KJ)));
        ChangeTimers(false, 0);
    }

    void ChangeTimers(bool status, uint32 WTimer)
    {
        for (uint8 i = 0; i < 10;++i)
            TimerIsDeactivated[i] = status;
        TimerIsDeactivated[TIMER_SPEECH] = IsKalecJoined;

        if (WTimer > 0)
        {
            IsWaiting = true;
            WaitTimer = WTimer;
        }

        if (OrbActivated)
            TimerIsDeactivated[TIMER_ORBS_EMPOWER] = true;
        if (Timer[TIMER_SHADOW_SPIKE] == 0)
            TimerIsDeactivated[TIMER_SHADOW_SPIKE] = true;
        if (Phase == PHASE_SACRIFICE)
            TimerIsDeactivated[TIMER_SUMMON_SHILEDORB] = true;
    }

    void JustSummoned(Creature* summoned)
    {
        if (summoned->GetEntry() == CREATURE_ARMAGEDDON_TARGET)
        {
            summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }
        else
            summoned->SetLevel(me->getLevel());

        summoned->setFaction(me->getFaction());
        summons.Summon(summoned);
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_KJ_DEATH, me);

        if (pInstance)
            pInstance->SetData(DATA_KILJAEDEN_EVENT, DONE);
    }

    void KilledUnit(Unit* victim)
    {
        switch(rand()%2)
        {
        case 0: DoScriptText(SAY_KJ_SLAY1, me); break;
        case 1: DoScriptText(SAY_KJ_SLAY2, me); break;
        }
    }

    void EnterEvadeMode()
    {
        Scripted_NoMovementAI::EnterEvadeMode();
        summons.DespawnAll();

        // Reset the controller
        if (pInstance)
        {
            if (Creature* pControl = Unit::GetCreature(*me, pInstance->GetData64(DATA_KILJAEDEN_CONTROLLER)))
                ((Scripted_NoMovementAI*)pControl->AI())->Reset();
        }
    }

    void EnterCombat(Unit* who)
    {
        DoZoneInCombat();
        DoScriptText(SAY_KJ_EMERGE, me);
    }

    void CastSinisterReflection()
    {
        switch(rand()%2){
        case 0: DoScriptText(SAY_KJ_REFLECTION1, me); break;
        case 1: DoScriptText(SAY_KJ_REFLECTION2, me); break;
        }
        DoCast(me, SPELL_SINISTER_REFLECTION, true);
        for (uint8 i = 0; i < 4; ++i)
        {
            float x,y,z;
            Unit *pTarget;
            for (uint8 z = 0; z < 6; ++z)
            {
                pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);
                if (!pTarget || !pTarget->HasAura(SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT,0)) break;
            }
            if (pTarget)
            {
                pTarget->GetPosition(x,y,z);
                if (Creature* pSinisterReflection = me->SummonCreature(CREATURE_SINISTER_REFLECTION, x,y,z,0, TEMPSUMMON_CORPSE_DESPAWN, 0))
                    pSinisterReflection->AI()->AttackStart(pTarget);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim() || Phase < PHASE_NORMAL)
            return;

        if (IsWaiting)
        {
            if (WaitTimer <= diff)
            {
                IsWaiting = false;
                ChangeTimers(false, 0);
            } else WaitTimer -= diff;
        }

        for (uint8 t = 0; t < ActiveTimers; ++t)
        {
            if (Timer[t] <= diff && !TimerIsDeactivated[t])
            {
                switch(t)
                {
                    case TIMER_SPEECH:
                        if (Kalec){
                            DoScriptText(SAY_KALECGOS_JOIN, Kalec);
                            IsKalecJoined = true;
                            TimerIsDeactivated[TIMER_SPEECH] = true;
                        }
                        break;
                    case TIMER_SOUL_FLAY:
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            me->CastSpell(me->getVictim(), SPELL_SOUL_FLAY, false);
                            me->getVictim()->CastSpell(me->getVictim(), SPELL_SOUL_FLAY_SLOW, true);
                            Timer[TIMER_SOUL_FLAY] = 3500;
                        }
                        break;
                    case TIMER_LEGION_LIGHTNING:
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            Unit* pRandomPlayer;

                            me->RemoveAurasDueToSpell(SPELL_SOUL_FLAY);
                            for (uint8 z = 0; z < 6; ++z)
                            {
                                pRandomPlayer = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);
                                if (!pRandomPlayer || !pRandomPlayer->HasAura(SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT,0))
                                    break;
                            }

                            if (pRandomPlayer)
                                DoCast(pRandomPlayer, SPELL_LEGION_LIGHTNING, false);
                            else
                                error_log("try to cast SPELL_LEGION_LIGHTNING on invalid target");

                            Timer[TIMER_LEGION_LIGHTNING] = (Phase == PHASE_SACRIFICE) ? 18000 : 30000; // 18 seconds in PHASE_SACRIFICE
                            Timer[TIMER_SOUL_FLAY] = 2500;
                        }
                        break;
                    case TIMER_FIRE_BLOOM:
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            me->RemoveAurasDueToSpell(SPELL_SOUL_FLAY);
                            DoCastAOE(SPELL_FIRE_BLOOM, false);
                            Timer[TIMER_FIRE_BLOOM] = (Phase == PHASE_SACRIFICE) ? 25000 : 40000; // 25 seconds in PHASE_SACRIFICE
                            Timer[TIMER_SOUL_FLAY] = 1000;
                        }
                        break;
                    case TIMER_SUMMON_SHILEDORB:
                        for (uint8 i = 1; i < Phase; ++i)
                        {
                            float sx, sy;
                            sx = ShieldOrbLocations[0][0] + sin(ShieldOrbLocations[i][0]);
                            sy = ShieldOrbLocations[0][1] + sin(ShieldOrbLocations[i][1]);
                            me->SummonCreature(CREATURE_SHIELD_ORB, sx, sy, SHIELD_ORB_Z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 45000);
                        }
                        Timer[TIMER_SUMMON_SHILEDORB] = 30000+rand()%30*1000; // 30-60seconds cooldown
                        Timer[TIMER_SOUL_FLAY] = 2000;
                        break;
                    case TIMER_SHADOW_SPIKE: //Phase 3
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            DoCastAOE(SPELL_SHADOW_SPIKE, false);
                            Timer[TIMER_SHADOW_SPIKE] = 0;
                            TimerIsDeactivated[TIMER_SHADOW_SPIKE] = true;
                            ChangeTimers(true, 30000);
                        }
                        break;
                    case TIMER_FLAME_DART: //Phase 3
                        DoCastAOE(SPELL_FLAME_DART, false);
                        Timer[TIMER_FLAME_DART] = 3000; //TODO Timer
                        break;
                    case TIMER_DARKNESS: //Phase 3
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            // Begins to channel for 8 seconds, then deals 50'000 damage to all raid members.
                            if (!IsInDarkness)
                            {
                                DoScriptText(EMOTE_KJ_DARKNESS, me);
                                DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS, false);
                                ChangeTimers(true, 9000);
                                Timer[TIMER_DARKNESS] = 8750;
                                TimerIsDeactivated[TIMER_DARKNESS] = false;
                                if (Phase == PHASE_SACRIFICE)
                                    TimerIsDeactivated[TIMER_ARMAGEDDON] = false;
                                IsInDarkness = true;
                            }
                            else
                            {
                                Timer[TIMER_DARKNESS] = (Phase == PHASE_SACRIFICE) ? 20000 + rand()%15000 : 40000 + rand()%30000;
                                IsInDarkness = false;
                                DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                                switch(rand()%3){
                                    case 0: DoScriptText(SAY_KJ_DARKNESS1, me); break;
                                    case 1: DoScriptText(SAY_KJ_DARKNESS2, me); break;
                                    case 2: DoScriptText(SAY_KJ_DARKNESS3, me); break;
                                }
                            }
                            Timer[TIMER_SOUL_FLAY] = 9000;
                        }
                        break;
                    case TIMER_ORBS_EMPOWER: //Phase 3
                        if (Phase == PHASE_SACRIFICE){
                            if (Kalec)((boss_kalecgos_kjAI*)Kalec->AI())->EmpowerOrb(true);
                        } else if (Kalec)((boss_kalecgos_kjAI*)Kalec->AI())->EmpowerOrb(false);
                        Timer[TIMER_ORBS_EMPOWER]= (Phase == PHASE_SACRIFICE) ? 45000 : 35000;
                        OrbActivated = true;
                        TimerIsDeactivated[TIMER_ORBS_EMPOWER] = true;
                        break;
                    case TIMER_ARMAGEDDON: //Phase 4
                        Unit *pTarget;
                        for (uint8 z = 0; z < 6; ++z)
                        {
                            pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);
                            if (!pTarget || !pTarget->HasAura(SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT,0)) break;
                        }
                        if (pTarget)
                        {
                            float x, y, z;
                            pTarget->GetPosition(x, y, z);
                            me->SummonCreature(CREATURE_ARMAGEDDON_TARGET, x,y,z,0, TEMPSUMMON_TIMED_DESPAWN,15000);
                        }
                        Timer[TIMER_ARMAGEDDON] = 2000; // No, I'm not kidding
                        break;
                 }
            }
        }
        //Time runs over!
        for (uint8 i = 0; i < ActiveTimers; ++i)
            if (!TimerIsDeactivated[i])
            {
                Timer[i] -= diff;
                if (((int32)Timer[i]) < 0) Timer[i] = 0;
            }

        //Phase 3
        if (Phase <= PHASE_NORMAL)
        {
            if (Phase == PHASE_NORMAL && HealthBelowPct(85))
            {
                CastSinisterReflection();
                DoScriptText(SAY_KJ_PHASE3, me);
                Phase = PHASE_DARKNESS;
                OrbActivated = false;
                ActiveTimers = 9;
            }
            else return;
        }

        //Phase 4
        if (Phase <= PHASE_DARKNESS)
        {
            if (Phase == PHASE_DARKNESS && HealthBelowPct(55))
            {
                DoScriptText(SAY_KJ_PHASE4, me);
                Phase = PHASE_ARMAGEDDON;
                OrbActivated = false;
                ActiveTimers = 10;
            }
            else return;
        }

        //Phase 5 specific spells all we can
        if (Phase <= PHASE_ARMAGEDDON)
        {
            if (Phase == PHASE_ARMAGEDDON && HealthBelowPct(25))
            {
                Phase = PHASE_SACRIFICE;
                Creature* Anveena = (Creature*)(Unit::GetUnit((*me), pInstance->GetData64(DATA_ANVEENA)));
                if (Anveena)Anveena->CastSpell(me, SPELL_SACRIFICE_OF_ANVEENA, false);
                OrbActivated = false;
                ChangeTimers(true, 10000);// He shouldn't cast spells for ~10 seconds after Anveena's sacrifice. This will be done within Anveena's script
            }
            else return;
        }
    }
};

CreatureAI* GetAI_boss_kiljaeden(Creature* pCreature)
{
    return new boss_kiljaedenAI (pCreature);
}

//AI for Hand of the Deceiver
struct mob_hand_of_the_deceiverAI : public ScriptedAI
{
    mob_hand_of_the_deceiverAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 ShadowBoltVolleyTimer;
    uint32 FelfirePortalTimer;

    void Reset()
    {
        // TODO: Timers!
        ShadowBoltVolleyTimer = 8000 + rand()%6000; // So they don't all cast it in the same moment.
        FelfirePortalTimer = 20000;
        if (pInstance)
            pInstance->SetData(DATA_KILJAEDEN_EVENT, NOT_STARTED);
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->setFaction(me->getFaction());
        summoned->SetLevel(me->getLevel());
    }

    void EnterCombat(Unit* who)
    {
        if (pInstance)
        {
            pInstance->SetData(DATA_KILJAEDEN_EVENT, IN_PROGRESS);
            if (Creature* pControl = Unit::GetCreature(*me, pInstance->GetData64(DATA_KILJAEDEN_CONTROLLER)))
                pControl->AddThreat(who, 1.0f);
        }
        me->InterruptNonMeleeSpells(true);
    }

    void JustDied(Unit* killer)
    {
        if (!pInstance)
            return;

        if (Creature* pControl = Unit::GetCreature(*me, pInstance->GetData64(DATA_KILJAEDEN_CONTROLLER)))
            ++(CAST_AI(mob_kiljaeden_controllerAI, pControl->AI())->deceiverDeathCount);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!me->isInCombat())
            DoCast(me, SPELL_SHADOW_CHANNELING);

        if (!UpdateVictim())
            return;

        // Gain Shadow Infusion at 20% health
        if (HealthBelowPct(20) && !me->HasAura(SPELL_SHADOW_INFUSION, 0))
            DoCast(me, SPELL_SHADOW_INFUSION, true);

        // Shadow Bolt Volley - Shoots Shadow Bolts at all enemies within 30 yards, for ~2k Shadow damage.
        if (ShadowBoltVolleyTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_SHADOW_BOLT_VOLLEY);
            ShadowBoltVolleyTimer = 12000;
        }
        else
            ShadowBoltVolleyTimer -= diff;

        // Felfire Portal - Creatres a portal, that spawns Volatile Felfire Fiends, which do suicide bombing.
        if (FelfirePortalTimer <= diff)
        {
            if (Creature* pPortal = DoSpawnCreature(CREATURE_FELFIRE_PORTAL, 0, 0,0, 0, TEMPSUMMON_TIMED_DESPAWN, 20000))
            {
                std::list<HostileReference*>::iterator itr;
                for (itr = me->getThreatManager().getThreatList().begin(); itr != me->getThreatManager().getThreatList().end(); ++itr)
                {
                    Unit* pUnit = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                    if (pUnit)
                        pPortal->AddThreat(pUnit, 1.0f);
                }
            }
            FelfirePortalTimer = 20000;
        } else FelfirePortalTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_hand_of_the_deceiver(Creature* pCreature)
{
    return new mob_hand_of_the_deceiverAI (pCreature);
}

//AI for Felfire Portal
struct mob_felfire_portalAI : public Scripted_NoMovementAI
{
    mob_felfire_portalAI(Creature* c) : Scripted_NoMovementAI(c) {}

    uint32 uiSpawnFiendTimer;

    void InitializeAI()
    {
        uiSpawnFiendTimer = 5000;
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_NON_ATTACKABLE);
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->setFaction(me->getFaction());
        summoned->SetLevel(me->getLevel());
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (uiSpawnFiendTimer <= diff)
        {
            if (Creature* pFiend = DoSpawnCreature(CREATURE_VOLATILE_FELFIRE_FIEND, 0, 0, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 20000))
                pFiend->AddThreat(SelectUnit(SELECT_TARGET_RANDOM,0), 100000.0f);
            uiSpawnFiendTimer = 4000 + rand()%4000;
        } else uiSpawnFiendTimer -= diff;
    }
};

CreatureAI* GetAI_mob_felfire_portal(Creature* pCreature)
{
    return new mob_felfire_portalAI (pCreature);
}

//AI for Felfire Fiend
struct mob_volatile_felfire_fiendAI : public ScriptedAI
{
    mob_volatile_felfire_fiendAI(Creature* c) : ScriptedAI(c) {}

    uint32 uiExplodeTimer;

    bool bLockedTarget;

    void Reset()
    {
        uiExplodeTimer = 2000;
        bLockedTarget = false;
    }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (damage > me->GetHealth())
            DoCast(me, SPELL_FELFIRE_FISSION, true);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (!bLockedTarget)
        {
            me->AddThreat(me->getVictim(), 10000000.0f);
            bLockedTarget = true;
        }

        if (uiExplodeTimer)
        {
            if (uiExplodeTimer <= diff)
                uiExplodeTimer = 0;
            else uiExplodeTimer -= diff;
        }
        else if (me->IsWithinDistInMap(me->getVictim(), 3)) // Explode if it's close enough to it's target
        {
            DoCast(me->getVictim(), SPELL_FELFIRE_FISSION);
            me->Kill(me);
        }
    }
};

CreatureAI* GetAI_mob_volatile_felfire_fiend(Creature* pCreature)
{
    return new mob_volatile_felfire_fiendAI (pCreature);
}

//AI for Armageddon target
struct mob_armageddonAI : public Scripted_NoMovementAI
{
    mob_armageddonAI(Creature* c) : Scripted_NoMovementAI(c) {}

    uint8 spell;
    uint32 uiTimer;

    void Reset()
    {
        spell = 0;
        uiTimer = 0;
    }

    void UpdateAI(const uint32 diff)
    {
        if (uiTimer <= diff)
        {
            switch(spell)
            {
                case 0:
                    DoCast(me, SPELL_ARMAGEDDON_VISUAL, true);
                    ++spell;
                    break;
                case 1:
                    DoCast(me, SPELL_ARMAGEDDON_VISUAL2, true);
                    uiTimer = 9000;
                    ++spell;
                    break;
                case 2:
                    DoCast(me, SPELL_ARMAGEDDON_TRIGGER, true);
                    ++spell;
                    uiTimer = 5000;
                    break;
                case 3:
                    me->Kill(me);
                    me->RemoveCorpse();
                    break;
            }
        } else uiTimer -=diff;
    }
};

CreatureAI* GetAI_mob_armageddon(Creature* pCreature)
{
    return new mob_armageddonAI (pCreature);
}

//AI for Shield Orbs
struct mob_shield_orbAI : public ScriptedAI
{
    mob_shield_orbAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    bool bPointReached;
    bool bClockwise;
    uint32 uiTimer;
    uint32 uiCheckTimer;
    float x, y, r, c, mx, my;

    void InitializeAI()
    {
        me->AddUnitMovementFlag(MOVEFLAG_ONTRANSPORT | MOVEFLAG_LEVITATING);
        bPointReached = true;
        uiTimer = 500+ rand()%500;
        uiCheckTimer = 1000;
        r = 17;
        c = 0;
        mx = ShieldOrbLocations[0][0];
        my = ShieldOrbLocations[0][1];
        bClockwise = (rand()%2 == 0);
    }

    void UpdateAI(const uint32 diff)
    {
        if (bPointReached)
        {
            if (bClockwise)
            {
                y = my - r * sin(c);
                x = mx - r * cos(c);
            }
            else
            {
                y = my + r * sin(c);
                x = mx + r * cos(c);
            }
            bPointReached = false;
            uiCheckTimer = 1000;
            me->GetMotionMaster()->MovePoint(1,x, y, SHIELD_ORB_Z);
            c += 3.1415926535/32;
            if (c >= 2*3.1415926535) c = 0;
        }
        else
        {
            if (uiCheckTimer <= diff)
            {
                DoTeleportTo(x,y,SHIELD_ORB_Z);
                bPointReached = true;
            }
            else uiCheckTimer -= diff;
        }

        if (uiTimer <= diff)
        {
            if (Unit* random = Unit::GetUnit(*me, pInstance ? pInstance->GetData64(DATA_PLAYER_GUID) : 0))
                DoCast(random, SPELL_SHADOW_BOLT, false);
            uiTimer = 500+ rand()%500;
        } else uiTimer -= diff;
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE)
            return;

        bPointReached = true;
    }
};

CreatureAI* GetAI_mob_shield_orb(Creature* pCreature)
{
    return new mob_shield_orbAI (pCreature);
}

//AI for Sinister Reflection
struct mob_sinster_reflectionAI : public ScriptedAI
{
    mob_sinster_reflectionAI(Creature* c) : ScriptedAI(c) {}

    uint8 victimClass;
    uint32 uiTimer[3];

    void Reset()
    {
        uiTimer[0] = 0;
        uiTimer[1] = 0;
        uiTimer[2] = 0;
        victimClass = 0;
    }

    void UpdateAI(const uint32 diff)
    {

        if (victimClass == 0){
            victimClass = me->getVictim()->getClass();
            switch (victimClass)
            {
                case CLASS_DRUID:
                    break;
                case CLASS_HUNTER:
                    break;
                case CLASS_MAGE:
                    break;
                case CLASS_WARLOCK:
                    break;
                case CLASS_WARRIOR:
                    me->SetCanDualWield(true);
                    break;
                case CLASS_PALADIN:
                    break;
                case CLASS_PRIEST:
                    break;
                case CLASS_SHAMAN:
                    me->SetCanDualWield(true);
                    break;
                case CLASS_ROGUE:
                    me->SetCanDualWield(true);
                    break;
            }
        }

        switch(victimClass) {
            case CLASS_DRUID:
                if (uiTimer[1] <= diff)
                {
                    DoCast(me->getVictim(), SPELL_SR_MOONFIRE, false);
                    uiTimer[1] = 3000;
                }
                DoMeleeAttackIfReady();
                break;
            case CLASS_HUNTER:
                if (uiTimer[1] <= diff)
                {
                    DoCast(me->getVictim(), SPELL_SR_MULTI_SHOT, false);
                    uiTimer[1] = 9000;
                }
                if (uiTimer[2] <= diff)
                {
                    DoCast(me->getVictim(), SPELL_SR_SHOOT, false);
                    uiTimer[2] = 5000;
                }
                if (me->IsWithinMeleeRange(me->getVictim(), 6))
                {
                    if (uiTimer[3] <= diff)
                    {
                        DoCast(me->getVictim(), SPELL_SR_MULTI_SHOT, false);
                        uiTimer[3] = 7000;
                    }
                    DoMeleeAttackIfReady();
                }
                break;
            case CLASS_MAGE:
                if (uiTimer[1] <= diff)
                {
                    DoCast(me->getVictim(), SPELL_SR_FIREBALL, false);
                    uiTimer[1] = 3000;
                }
                DoMeleeAttackIfReady();
                break;
            case CLASS_WARLOCK:
                if (uiTimer[1] <= diff)
                {
                    DoCast(me->getVictim(), SPELL_SR_SHADOW_BOLT, false);
                    uiTimer[1] = 4000;
                }
                if (uiTimer[2] <= diff)
                {
                    DoCast(SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true), SPELL_SR_CURSE_OF_AGONY, true);
                    uiTimer[2] = 3000;
                }
                DoMeleeAttackIfReady();
                break;
            case CLASS_WARRIOR:
                if (uiTimer[1] <= diff)
                {
                    DoCast(me->getVictim(), SPELL_SR_WHIRLWIND, false);
                    uiTimer[1] = 10000;
                }
                DoMeleeAttackIfReady();
                break;
            case CLASS_PALADIN:
                if (uiTimer[1] <= diff)
                {
                    DoCast(me->getVictim(), SPELL_SR_HAMMER_OF_JUSTICE, false);
                    uiTimer[1] = 7000;
                }
                if (uiTimer[2] <= diff)
                {
                    DoCast(me->getVictim(), SPELL_SR_HOLY_SHOCK, false);
                    uiTimer[2] = 3000;
                }
                DoMeleeAttackIfReady();
                break;
            case CLASS_PRIEST:
                if (uiTimer[1] <= diff)
                {
                    DoCast(me->getVictim(), SPELL_SR_HOLY_SMITE, false);
                    uiTimer[1] = 5000;
                }
                if (uiTimer[2] <= diff)
                {
                    DoCast(me, SPELL_SR_RENEW, false);
                    uiTimer[2] = 7000;
                }
                DoMeleeAttackIfReady();
                break;
            case CLASS_SHAMAN:
                if (uiTimer[1] <= diff)
                {
                    DoCast(me->getVictim(), SPELL_SR_EARTH_SHOCK, false);
                    uiTimer[1] = 5000;
                }
                DoMeleeAttackIfReady();
                break;
            case CLASS_ROGUE:
                if (uiTimer[1] <= diff)
                {
                    DoCast(me->getVictim(), SPELL_SR_HEMORRHAGE, true);
                    uiTimer[1] = 5000;
                }
                DoMeleeAttackIfReady();
                break;
            }
            debug_log("Sinister-Timer");
            for (uint8 i = 0; i < 3; ++i)
                uiTimer[i] -= diff;
        }
};

CreatureAI* GetAI_mob_sinster_reflection(Creature* pCreature)
{
    return new mob_sinster_reflectionAI (pCreature);
}

void AddSC_boss_kiljaeden()
{
    Script* newscript;

    newscript = new Script;
    newscript->pGOHello = &GOHello_go_orb_of_the_blue_flight;
    newscript->Name = "go_orb_of_the_blue_flight";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_boss_kalecgos_kj;
    newscript->Name = "boss_kalecgos_kj";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_boss_kiljaeden;
    newscript->Name = "boss_kiljaeden";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_mob_kiljaeden_controller;
    newscript->Name = "mob_kiljaeden_controller";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_mob_hand_of_the_deceiver;
    newscript->Name = "mob_hand_of_the_deceiver";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_mob_felfire_portal;
    newscript->Name = "mob_felfire_portal";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_mob_volatile_felfire_fiend;
    newscript->Name = "mob_volatile_felfire_fiend";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_mob_armageddon;
    newscript->Name = "mob_armageddon";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_mob_shield_orb;
    newscript->Name = "mob_shield_orb";
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->GetAI = &GetAI_mob_sinster_reflection;
    newscript->Name = "mob_sinster_reflection";
    newscript->RegisterSelf();
}
