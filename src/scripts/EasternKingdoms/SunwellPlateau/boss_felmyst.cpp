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
SDName: Boss_Felmyst
SD%Complete: 97
SDComment:
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Cell.h"
#include "CellImpl.h"
#include "sunwell_plateau.h"

enum Yells
{
    YELL_BIRTH                                    = -1580036,
    YELL_KILL1                                    = -1580037,
    YELL_KILL2                                    = -1580038,
    YELL_BREATH                                   = -1580039,
    YELL_TAKEOFF                                  = -1580040,
    YELL_BERSERK                                  = -1580041,
    YELL_DEATH                                    = -1580042,
    YELL_KALECGOS                                 = -1580043, // after felmyst's death spawned and say this
};
enum Spells
{
    //Aura
    AURA_SUNWELL_RADIANCE                         = 45769,
    AURA_NOXIOUS_FUMES                            = 47002,

    //Land phase
    SPELL_CLEAVE                                  = 19983,
    SPELL_CORROSION                               = 45866,
    SPELL_GAS_NOVA                                = 45855,
    SPELL_ENCAPSULATE_CHANNEL                     = 45661,
    SPELL_ENCAPSULATE_EFFECT                      = 45665,
    SPELL_ENCAPSULATE_AOE                         = 45662,

    //Flight phase
    SPELL_VAPOR_SELECT                            = 45391,   // fel to player, force cast 45392, 50000y selete target
    SPELL_VAPOR_SUMMON                            = 45392,   // player summon vapor, radius around caster, 5y,
    SPELL_VAPOR_FORCE                             = 45388,   // vapor to fel, force cast 45389
    SPELL_VAPOR_CHANNEL                           = 45389,   // fel to vapor, green beam channel
    SPELL_VAPOR_TRIGGER                           = 45411,   // linked to 45389, vapor to self, trigger 45410 and 46931
    SPELL_VAPOR_DAMAGE                            = 46931,   // vapor damage, 4000
    SPELL_TRAIL_SUMMON                            = 45410,   // vapor summon trail
    SPELL_TRAIL_TRIGGER                           = 45399,   // trail to self, trigger 45402
    SPELL_TRAIL_DAMAGE                            = 45402,   // trail damage, 2000 + 2000 dot
    SPELL_DEAD_SUMMON                             = 45400,   // summon blazing dead, 5min
    SPELL_DEAD_PASSIVE                            = 45415,
    SPELL_FOG_BREATH                              = 45495,   // fel to self, speed burst
    SPELL_FOG_TRIGGER                             = 45582,   // fog to self, trigger 45782
    SPELL_FOG_FORCE                               = 45782,   // fog to player, force cast 45714
    SPELL_FOG_INFORM                              = 45714,   // player let fel cast 45717, script effect
    SPELL_FOG_CHARM                               = 45717,   // fel to player
    SPELL_FOG_CHARM2                              = 45726,   // link to 45717
    SPELL_TRANSFORM_TRIGGER                       = 44885,   // madrigosa to self, trigger 46350
    SPELL_TRANSFORM_VISUAL                        = 46350,   // 46411stun?
    SPELL_TRANSFORM_FELMYST                       = 45068,   // become fel
    SPELL_FELMYST_SUMMON                          = 45069,

    //Other
    SPELL_BERSERK                                 = 45078,
    SPELL_CLOUD_VISUAL                            = 45212,
    SPELL_CLOUD_SUMMON                            = 45884,
};

enum PhaseFelmyst
{
    PHASE_NONE,
    PHASE_GROUND,
    PHASE_FLIGHT,
};

enum EventFelmyst
{
    EVENT_NONE,
    EVENT_BERSERK,
    EVENT_CLEAVE,
    EVENT_CORROSION,
    EVENT_GAS_NOVA,
    EVENT_ENCAPSULATE,
    EVENT_FLIGHT,
    EVENT_SUMMON_DEAD,
    EVENT_FOG_CORRUPTION,
    EVENT_DEMONIC_VAPOR,
};

Position flightMobLeft = {1468.380005, 730.267029, 60.083302};
Position flightMobRight = {1458.170044, 501.295013, 60.083302};

Position lefts[3] = { {1446.540039, 702.570007, 52.083302},    //south
                      {1469.939941, 704.239014, 52.083302},    //middle
                      {1494.760010, 705.000000, 52.083302}, }; //north

Position rights[3] = { {1441.640015, 520.520020, 52.083302},   //south
                       {1467.219971, 516.318970, 52.083302},   //middle
                       {1492.819946, 515.668030, 52.083302}, };//north

Position prepareLandingLoc[2] = {  {1482.709961, 649.406006, 21.081100},
                                   {1491.119995, 553.672974, 24.921900}  };

struct boss_felmystAI : public ScriptedAI
{
    boss_felmystAI(Creature* c) : ScriptedAI(c), Summons(me)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        // wait for core patch be accepted
        SpellEntry* TempSpell = GET_SPELL(SPELL_ENCAPSULATE_EFFECT);
        if (TempSpell->SpellIconID == 2294)
            TempSpell->SpellIconID = 2295;
        TempSpell = GET_SPELL(SPELL_VAPOR_TRIGGER);
        if ((TempSpell->Attributes & SPELL_ATTR0_PASSIVE) == 0)
            TempSpell->Attributes |= SPELL_ATTR0_PASSIVE;
        TempSpell = GET_SPELL(SPELL_FOG_CHARM2);
        if ((TempSpell->Attributes & SPELL_ATTR0_PASSIVE) == 0)
            TempSpell->Attributes |= SPELL_ATTR0_PASSIVE;
    }

    ScriptedInstance* pInstance;
    SummonList Summons;

    EventMap events;

    uint32 uiFlightTimer;
    uint32 uiFlightCount;
    uint32 uiBreathCount;
    uint32 KalecTimer;
    uint32 KalecE;
    uint32 demonicCount;

    bool direction;
    bool origin;
    bool fogactivated;

    uint8 chosenLane; //0-2, south - center - north

    void Reset()
    {
        events.Reset();
        events.SetPhase(PHASE_NONE);

        direction = false;
        origin = false;
        fogactivated = false;

        chosenLane = 0;

        uiFlightTimer = 0;
        uiFlightCount = 0;
        demonicCount = 0;
        me->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 10);
        me->SetFloatValue(UNIT_FIELD_COMBATREACH, 10);

        Summons.DespawnAll();

        DoScriptText(YELL_BIRTH, me);
        KalecE = false;
        me->SetSpeed(MOVE_RUN, 1.0f, true);
        me->GetMotionMaster()->MovePath(34000, true);

        if (pInstance)
            pInstance->SetData(DATA_FELMYST_EVENT, NOT_STARTED);
    }

    void EnterCombat(Unit* /*who*/)
    {
        events.ScheduleEvent(EVENT_BERSERK, 600000);
        DoZoneInCombat();
        EnterPhase(PHASE_GROUND);
        DoCast(me, AURA_SUNWELL_RADIANCE, true);
        DoCast(me, AURA_NOXIOUS_FUMES, true);

        if (pInstance)
            pInstance->SetData(DATA_FELMYST_EVENT, IN_PROGRESS);
    }

    void AttackStart(Unit* who)
    {
        if (events.IsInPhase(PHASE_GROUND))
            ScriptedAI::AttackStart(who);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (events.IsInPhase(PHASE_GROUND))
            ScriptedAI::MoveInLineOfSight(who);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        DoScriptText(RAND(YELL_KILL1, YELL_KILL2), me);
    }

    void JustRespawned()
    {
        DoScriptText(YELL_BIRTH, me);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(YELL_DEATH, me);

        if (pInstance)
        {
            pInstance->SetData(DATA_FELMYST_EVENT, DONE);
            me->SummonCreature(25319, me->GetPositionX(), me->GetPositionY() + 10, me->GetPositionZ() + 20, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 10000);
            Creature* Kalec = Unit::GetCreature(*me, pInstance ? pInstance->GetData64(DATA_KALECGOS_KJ) : 0);
            DoScriptText(YELL_KALECGOS, Kalec);
            //Kalec->
        }
    }

    void JustSummoned(Creature* summoned)
    {
        Summons.Summon(summoned);
        if(summoned->GetEntry() == MOB_DEAD)
        {
            summoned->AI()->AttackStart(SelectTarget(SELECT_TARGET_RANDOM, 0));
            summoned->CastSpell(summoned, SPELL_DEAD_PASSIVE, true);
        }
        else if (summoned->GetEntry() == MOB_VAPOR)
            me->SetTarget(summoned->GetGUID());
    }

    void SummonedCreatureDespawn(Creature* unit)
    {
        if (unit->GetEntry() == MOB_VAPOR)
            me->SetTarget(0);

        Summons.Despawn(unit);
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE)
            return;

        switch (id)
        {
            case 1:
                uiFlightCount++;
                uiFlightTimer = 1;
                break;
            case 2:	
                me->RemoveAurasDueToSpell(SPELL_FOG_BREATH);
                fogactivated = false;

                uiFlightCount++;
                uiFlightTimer = 3000;
                break;
            case 3:
                me->HandleEmoteCommand(EMOTE_ONESHOT_LAND);

                float x, y, z;
                me->GetPosition(x, y, z);
                me->UpdateGroundPositionZ(x, y, z);
                me->Relocate(x, y, z);

                me->SendMovementFlagUpdate();

                EnterPhase(PHASE_GROUND);

                if (Unit *target = SelectTarget(SELECT_TARGET_TOPAGGRO, 0))
                {
                    AttackStart(target);
                    me->GetMotionMaster()->MoveChase(target);
                }
                else
                    EnterEvadeMode();
                break;
        }
    }

    void DamageTaken(Unit* /*pDone_by*/, uint32& damage)
    {
        if (!events.IsInPhase(PHASE_GROUND) && damage >= me->GetHealth())
            damage = 0;
    }

    void EnterPhase(PhaseFelmyst NextPhase)
    {
        switch (NextPhase)
        {
        case PHASE_GROUND:
            me->SetSpeed(MOVE_RUN, 2.0f);
            events.ScheduleEvent(EVENT_CLEAVE, urand(5000, 10000), 0, PHASE_GROUND);
            events.ScheduleEvent(EVENT_CORROSION, urand(10000, 20000), 0, PHASE_GROUND);
            events.ScheduleEvent(EVENT_GAS_NOVA, urand(15000, 20000), 0, PHASE_GROUND);
            events.ScheduleEvent(EVENT_ENCAPSULATE, 30000, 0, PHASE_GROUND);
            events.ScheduleEvent(EVENT_FLIGHT, 60000, 0, PHASE_GROUND);
            break;
        case PHASE_FLIGHT:
            me->SetSpeed(MOVE_RUN, 1.3f, true);

            switch (rand()%2)
            {
                case 0:
                    origin = true;
                    direction = true;
                    break;
                case 1:
                    origin = false;
                    direction = false;
                    break;
            }

            me->StopMoving();
            me->GetMotionMaster()->Initialize();
            me->SetWalk(false);

            uiFlightTimer = 300;
            demonicCount = 0;
            uiFlightCount = 0;
            uiBreathCount = 0;
            break;
        }

        events.SetPhase(NextPhase);
    }

    void HandleFlightSequence()
    {
        switch (uiFlightCount)
        {
        case 0:
            me->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
            DoScriptText(YELL_TAKEOFF, me);
            uiFlightTimer = 2000;
            break;
        case 1:
            me->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 10);
            uiFlightTimer = 30000;
            events.ScheduleEvent(EVENT_DEMONIC_VAPOR, 3000, 0, PHASE_FLIGHT);
            break;
        case 2:
            me->SetSpeed(MOVE_RUN, 4.0f, true);

            if (!direction)
                me->GetMotionMaster()->MovePoint(1, flightMobRight.GetPositionX(), flightMobRight.GetPositionY(), flightMobRight.GetPositionZ(), false);
            else
                me->GetMotionMaster()->MovePoint(1, flightMobLeft.GetPositionX(), flightMobLeft.GetPositionY(), flightMobLeft.GetPositionZ(), false);

            uiFlightTimer = 20000;
            break;
        case 3:
            uiFlightTimer = 1;
            break;
        case 4:
            me->SetSpeed(MOVE_RUN, 1.3f, true);
            chosenLane = rand()%3;

            if (!direction)
                me->GetMotionMaster()->MovePoint(1, rights[chosenLane].GetPositionX(), rights[chosenLane].GetPositionY(), rights[chosenLane].GetPositionZ(), false);
            else
                me->GetMotionMaster()->MovePoint(1, lefts[chosenLane].GetPositionX(), lefts[chosenLane].GetPositionY(), lefts[chosenLane].GetPositionZ(), false);

            uiFlightTimer = 20000;
            break;
        case 5:
            uiFlightTimer = 1;
            break;
        case 6:
            DoScriptText(YELL_BREATH, me);

            if (!direction)
                me->SetFacingTo(me->GetAngle(lefts[chosenLane].GetPositionX(), lefts[chosenLane].GetPositionY()));
            else
                me->SetFacingTo(me->GetAngle(rights[chosenLane].GetPositionX(), rights[chosenLane].GetPositionY()));
            me->SetSpeed(MOVE_RUN, 4.0f, true);

            uiFlightTimer = 4000;
            break;
        case 7:
            if (!direction)
                me->GetMotionMaster()->MovePoint(2, lefts[chosenLane].GetPositionX(), lefts[chosenLane].GetPositionY(), lefts[chosenLane].GetPositionZ(), false);
            else
                me->GetMotionMaster()->MovePoint(2, rights[chosenLane].GetPositionX(), rights[chosenLane].GetPositionY(), rights[chosenLane].GetPositionZ(), false);

            DoCast(me, SPELL_FOG_BREATH, false);
            uiFlightTimer = 1500;
            break;
        case 8:
            fogactivated = true;
            uiFlightTimer = 20000;
            events.ScheduleEvent(EVENT_FOG_CORRUPTION, 50, 0, PHASE_FLIGHT);
            direction = !direction;
            break;
        case 9:
            me->RemoveAurasDueToSpell(SPELL_FOG_BREATH);
            fogactivated = false;
            uiFlightTimer = 1;
            break;
        case 10:
            uiBreathCount++;
            me->SetSpeed(MOVE_RUN, 1.3f, true);
            uiFlightTimer = 2000;
            if(uiBreathCount < 3)
                uiFlightCount = 3;
            break;
        case 11:
            if (!origin)
                me->GetMotionMaster()->MovePoint(3, prepareLandingLoc[0].GetPositionX(),prepareLandingLoc[0].GetPositionY(),prepareLandingLoc[0].GetPositionZ(), false);
            else
                me->GetMotionMaster()->MovePoint(3, prepareLandingLoc[1].GetPositionX(),prepareLandingLoc[1].GetPositionY(),prepareLandingLoc[1].GetPositionZ(), false);
			
            uiFlightTimer = 15000;
            break;
        default:
            uiFlightCount = 11;
            break;
        }
        ++uiFlightCount;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
        {
            if (events.IsInPhase(PHASE_FLIGHT) && !me->IsInEvadeMode())
                EnterEvadeMode();
            return;
        }

        if (me->IsNonMeleeSpellCast(false))
            return;

        events.Update(diff);

        switch (events.ExecuteEvent())
        {
            case EVENT_BERSERK:
                if (!me->HasAura(SPELL_BERSERK))
                {
                    DoScriptText(YELL_BERSERK, me);
                    DoCast(me, SPELL_BERSERK, true);
                }
                events.ScheduleEvent(EVENT_BERSERK, 10000);
                break;
            case EVENT_CLEAVE:
                DoCastVictim( SPELL_CLEAVE, false);
                events.ScheduleEvent(EVENT_CLEAVE, urand(5000, 10000), 0, PHASE_GROUND);
                break;
            case EVENT_CORROSION:
                DoCastVictim( SPELL_CORROSION, false);
                events.ScheduleEvent(EVENT_CORROSION, urand(20000, 30000), 0, PHASE_GROUND);
                break;
            case EVENT_GAS_NOVA:
                DoCast(me, SPELL_GAS_NOVA, false);
                events.ScheduleEvent(EVENT_GAS_NOVA, urand(21000, 26000), 0, PHASE_GROUND);
                break;
            case EVENT_ENCAPSULATE:
                if (Unit* encapsTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 150.0f, true))
                    DoCast(encapsTarget, SPELL_ENCAPSULATE_CHANNEL, false);

                if (Unit* tank = SelectTarget(SELECT_TARGET_TOPAGGRO, 0, 150.0f, true))
                    me->SetTarget(tank->GetGUID());

                events.ScheduleEvent(EVENT_ENCAPSULATE, 33000, 0, PHASE_GROUND);
                break;
            case EVENT_DEMONIC_VAPOR:
                DoCast(me, SPELL_VAPOR_SELECT, true);

                demonicCount++;

                if (demonicCount < 2)
                    events.ScheduleEvent(EVENT_DEMONIC_VAPOR, 11000, 0, PHASE_FLIGHT);
                else
                    events.CancelEvent(EVENT_DEMONIC_VAPOR);

                break;
            case EVENT_FLIGHT:
                EnterPhase(PHASE_FLIGHT);
                break;
            case EVENT_FOG_CORRUPTION:
                if (!fogactivated)
                {
                    events.CancelEvent(EVENT_FOG_CORRUPTION);
                    break;
                }

                if (pInstance)
                {
                    switch (chosenLane)
                    {
                        case 0:
                            pInstance->SetData((direction ? DATA_ACTIVATE_SOUTH_TO_LEFT : DATA_ACTIVATE_SOUTH_TO_RIGHT), (uint32) me->GetPositionY());
                            break;
                        case 1:
                            pInstance->SetData((direction ? DATA_ACTIVATE_CENTER_TO_LEFT : DATA_ACTIVATE_CENTER_TO_RIGHT), (uint32) me->GetPositionY());
                            break;
                        case 2:
                            pInstance->SetData((direction ? DATA_ACTIVATE_NORTH_TO_LEFT : DATA_ACTIVATE_NORTH_TO_RIGHT), (uint32) me->GetPositionY());
                            break;
                    }
                }
                events.ScheduleEvent(EVENT_FOG_CORRUPTION, 50, 0, PHASE_FLIGHT);
                break;
            default:
                if (events.IsInPhase(PHASE_GROUND))
                    DoMeleeAttackIfReady();
                else if (events.IsInPhase(PHASE_FLIGHT))
                {
                    if (uiFlightTimer)
                    {
                        if (uiFlightTimer <= diff)
                        {
                            uiFlightTimer = 0;
                            HandleFlightSequence();
                        }
                        else
                            uiFlightTimer -= diff;
                    }
                }
                break;
        }
    }
};

struct mob_felmyst_vaporAI : public ScriptedAI
{
    mob_felmyst_vaporAI(Creature* c) : ScriptedAI(c)
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    bool startFollow;

    void Reset()
    {
        startFollow = false;
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetWalk(false);
    }

    void EnterCombat(Unit* /*who*/)
    {
        startFollow = true;
        DoZoneInCombat();
        me->CastSpell((Unit*)NULL, SPELL_VAPOR_FORCE, true);
        DoCast(me, SPELL_VAPOR_TRIGGER, true);
    }

    void UpdateAI(const uint32 /*diff*/)
    {
        if (startFollow)
        {
            if (!me->getVictim())
            {
                if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                {
                    me->Attack(pTarget, false);
                    me->GetMotionMaster()->MoveFollow(pTarget, 0.0f, 0);
                }
            }
        }
    }
};

enum EventTrail
{
    EVENT_DEAD            = 0
};

struct mob_felmyst_trailAI : public ScriptedAI
{
    mob_felmyst_trailAI(Creature* c) : ScriptedAI(c)
    {
        events.Reset();

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        DoCast(me, SPELL_TRAIL_TRIGGER, true);
        me->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 0.01f); // core bug

        events.ScheduleEvent(EVENT_DEAD, urand(7000, 8000));
    }

    EventMap events;

    void AttackStart(Unit* who) { }

    void UpdateAI(const uint32 diff)
    {
        events.Update(diff);

        switch (events.ExecuteEvent())
        {
            case EVENT_DEAD:
                me->CastSpell((Unit*)NULL, SPELL_DEAD_SUMMON, true);
                me->DisappearAndDie();
                break;
        }
    }
};

struct mob_unyielding_deadAI : public ScriptedAI
{
    mob_unyielding_deadAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
    }

    ScriptedInstance* pInstance;

    void Reset()
    {
        if(pInstance)
            DoZoneInCombat();

        if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
        {
            me->AddThreat(target, 500.0f);
            AttackStart(target);
        }
        me->CastSpell((Unit*)NULL, SPELL_DEAD_PASSIVE, true);
    }

    void UpdateAI(const uint32 diff)
    {
        if (pInstance && pInstance->GetData(DATA_FELMYST_EVENT) != IN_PROGRESS)
            me->DisappearAndDie();

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_felmyst(Creature* pCreature)
{
    return new boss_felmystAI(pCreature);
}

CreatureAI* GetAI_mob_felmyst_vapor(Creature* pCreature)
{
    return new mob_felmyst_vaporAI(pCreature);
}

CreatureAI* GetAI_mob_felmyst_trail(Creature* pCreature)
{
    return new mob_felmyst_trailAI(pCreature);
}

CreatureAI* GetAI_mob_unyielding_dead(Creature* pCreature)
{
    return new mob_unyielding_deadAI(pCreature);
}

void AddSC_boss_felmyst()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_felmyst";
    newscript->GetAI = &GetAI_boss_felmyst;
    newscript->RegisterSelf();
    newscript = new Script;

    newscript->Name = "mob_felmyst_vapor";
    newscript->GetAI = &GetAI_mob_felmyst_vapor;
    newscript->RegisterSelf();
    newscript = new Script;

    newscript->Name = "mob_felmyst_trail";
    newscript->GetAI = &GetAI_mob_felmyst_trail;
    newscript->RegisterSelf();
    newscript = new Script;

    newscript->Name = "mob_unyielding_dead";
    newscript->GetAI = &GetAI_mob_unyielding_dead;
    newscript->RegisterSelf();
}