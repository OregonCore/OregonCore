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
SDName: Boss_ZulJin
SD%Complete: 85%
SDComment:
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "zulaman.h"

enum //Sounds
{
    SOUND_TRANSFORM_TO_LYNX       = 12094,
    SOUND_TRANSFORM_TO_BEAR       = 12092,
    SOUND_TRANSFORM_TO_DRAGONHAWK = 12095,
    SOUND_TRANSFORM_TO_EAGLE      = 12093,
    SOUND_KILL_ONE                = 12098,
    SOUND_KILL_TWO                = 12099,
    SOUND_FIRE_BRETH              = 12096,
    SOUND_AGGRO                   = 12091,
    SOUND_BERSERK                 = 12097,
    SOUND_DEATH                   = 12100,
    SOUND_INTRO                   = 12090
};

static const char YELL_TRANSFORM_TO_LYNX[]       = "Let me introduce to you my new bruddahs: fang and claw!";
static const char YELL_TRANSFORM_TO_BEAR[]       = "Got me some new tricks...like me bruddah bear!";
static const char YELL_TRANSFORM_TO_DRAGONHAWK[] = "Ya don' have to look to da sky to see da dragonhawk!";
static const char YELL_TRANSFORM_TO_EAGLE[]      = "Dere be no hidin' from da eagle!";
static const char YELL_KILL_ONE[]                = "Da Amani de chuka!";
static const char YELL_KILL_TWO[]                = "Lot more gonna fall like you!";
static const char YELL_FIRE_BREATH[]             = "Fire kill you just as quick!";
static const char YELL_AGGRO[]                   = "Nobody badduh dan me!";
static const char YELL_BERSERK[]                 = "You too slow! Me too strong!";
static const char YELL_DEATH[]                   = "Mebbe me fall...but da Amani empire...never gonna die...";

//Still not used, need more info
static const char YELL_INTRO[] = "Everybody always wanna take from us. "
                                 "Now we gonna start takin' back. "
                                 "Anybody who get in our way...gonna drown in their own blood! "
                                 "De Amani empire be back now...seekin' vengeance. "
                                 "And we gonna start...with you!";

enum //Spells
{
    // ====== Troll Form
    SPELL_WHIRLWIND               = 17207,
    SPELL_GRIEVOUS_THROW          = 43093,   // remove debuff after full healed
    // ====== Bear Form
    SPELL_CREEPING_PARALYSIS      = 43095,   // should cast on the whole raid
    SPELL_OVERPOWER               = 43456,   // use after melee attack dodged
    // ====== Eagle Form
    SPELL_ENERGY_STORM            = 43983,   // enemy area aura, trigger 42577
    SPELL_ZAP_INFORM              = 42577,
    SPELL_ZAP_DAMAGE              = 43137,   // 1250 damage
    SPELL_SUMMON_CYCLONE          = 43112,   // summon four feather vortex
    CREATURE_FEATHER_VORTEX       = 24136,
    SPELL_CYCLONE_VISUAL          = 43119,   // trigger 43147 visual
    SPELL_CYCLONE_PASSIVE         = 43120,   // trigger 43121 (4y aoe) every second
    // ====== Lynx Form
    SPELL_CLAW_RAGE_HASTE         = 42583,
    SPELL_CLAW_RAGE_TRIGGER       = 43149,
    SPELL_CLAW_RAGE_DAMAGE        = 43150,
    SPELL_LYNX_RUSH_HASTE         = 43152,
    SPELL_LYNX_RUSH_DAMAGE        = 43153,
    // ====== Dragonhawk Form
    SPELL_FLAME_WHIRL             = 43213,   // trigger two spells
    SPELL_FLAME_BREATH            = 43215,
    SPELL_SUMMON_PILLAR           = 43216,   // summon 24187
    CREATURE_COLUMN_OF_FIRE       = 24187,
    SPELL_PILLAR_TRIGGER          = 43218,   // trigger 43217
    // ====== Cosmetic
    SPELL_SPIRIT_AURA             = 42466,
    SPELL_SIPHON_SOUL             = 43501,
    // ====== Transforms:
    SPELL_SHAPE_OF_THE_BEAR       = 42594,   // 15% dmg
    SPELL_SHAPE_OF_THE_EAGLE      = 42606,
    SPELL_SHAPE_OF_THE_LYNX       = 42607,   // haste melee 30%
    SPELL_SHAPE_OF_THE_DRAGONHAWK = 42608,

    SPELL_BERSERK                 = 45078
};

enum //Phase
{
    PHASE_BEAR       = 0,
    PHASE_EAGLE      = 1,
    PHASE_LYNX       = 2,
    PHASE_DRAGONHAWK = 3,
    PHASE_TROLL      = 4
};

//coords for going for changing form
static const float CENTER_X = 120.148811f;
static const float CENTER_Y = 703.713684f;
static const float CENTER_Z =  45.111477f;

struct SpiritInfoStruct
{
    uint32 entry;
    float x, y, z, orient;
};

static SpiritInfoStruct SpiritInfo[] =
{
    {23878, 147.87f, 706.51f, 45.11f, 3.04f},
    {23880, 88.95f, 705.49f, 45.11f, 6.11f},
    {23877, 137.23f, 725.98f, 45.11f, 3.71f},
    {23879, 104.29f, 726.43f, 45.11f, 5.43f}
};

struct TransformStruct
{
    uint32 sound;
    const char* text;
    uint32 spell, unaura;
};

static TransformStruct Transform[] =
{
    {SOUND_TRANSFORM_TO_BEAR, YELL_TRANSFORM_TO_BEAR, SPELL_SHAPE_OF_THE_BEAR, SPELL_WHIRLWIND},
    {SOUND_TRANSFORM_TO_EAGLE, YELL_TRANSFORM_TO_EAGLE, SPELL_SHAPE_OF_THE_EAGLE, SPELL_SHAPE_OF_THE_BEAR},
    {SOUND_TRANSFORM_TO_LYNX, YELL_TRANSFORM_TO_LYNX, SPELL_SHAPE_OF_THE_LYNX, SPELL_SHAPE_OF_THE_EAGLE},
    {SOUND_TRANSFORM_TO_DRAGONHAWK, YELL_TRANSFORM_TO_DRAGONHAWK, SPELL_SHAPE_OF_THE_DRAGONHAWK, SPELL_SHAPE_OF_THE_LYNX}
};

struct boss_zuljinAI : public ScriptedAI
{
    boss_zuljinAI(Creature* c) : ScriptedAI(c), Summons(me)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }
    ScriptedInstance* pInstance;

    uint64 SpiritGUID[4];
    uint64 ClawTargetGUID;
    uint64 TankGUID;

    uint32 Phase;
    uint32 health_20;

    uint32 Intro_Timer;
    uint32 Berserk_Timer;

    uint32 Whirlwind_Timer;
    uint32 Grievous_Throw_Timer;

    uint32 Creeping_Paralysis_Timer;
    uint32 Overpower_Timer;

    uint32 Claw_Rage_Timer;
    uint32 Lynx_Rush_Timer;
    uint32 Claw_Counter;
    uint32 Claw_Loop_Timer;

    uint32 Flame_Whirl_Timer;
    uint32 Flame_Breath_Timer;
    uint32 Pillar_Of_Fire_Timer;

    SummonList Summons;

    void Reset()
    {
        if (pInstance)
            pInstance->SetData(ENCOUNTER_ZULJIN, NOT_STARTED);

        Phase = 0;

        health_20 = me->GetMaxHealth() * 0.2f;

        Intro_Timer = 37000;
        Berserk_Timer = 600000;

        Whirlwind_Timer = 7000;
        Grievous_Throw_Timer = 8000;

        Creeping_Paralysis_Timer = 7000;
        Overpower_Timer = 0;

        Claw_Rage_Timer = 5000;
        Lynx_Rush_Timer = 14000;
        Claw_Loop_Timer = 0;
        Claw_Counter = 0;

        Flame_Whirl_Timer = 5000;
        Flame_Breath_Timer = 6000;
        Pillar_Of_Fire_Timer = 7000;

        ClawTargetGUID = 0;
        TankGUID = 0;

        Summons.DespawnAll();

        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, 47174);
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO, 218172674);
        me->SetSheath(SHEATH_STATE_MELEE);
    }

    void EnterCombat(Unit* /*who*/)
    {
        if (pInstance)
            pInstance->SetData(ENCOUNTER_ZULJIN, IN_PROGRESS);

        DoZoneInCombat();

        me->MonsterYell(YELL_INTRO, LANG_UNIVERSAL, 0);
        DoPlaySoundToSet(me, SOUND_INTRO);
        SpawnAdds();
        EnterPhase(0);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        if (Intro_Timer)
            return;

        switch (urand(0, 1))
        {
        case 0:
            me->MonsterYell(YELL_KILL_ONE, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, SOUND_KILL_ONE);
            break;
        case 1:
            me->MonsterYell(YELL_KILL_TWO, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, SOUND_KILL_TWO);
            break;
        }
    }

    void JustDied(Unit* /*Killer*/)
    {
        if (pInstance)
            pInstance->SetData(ENCOUNTER_ZULJIN, DONE);

        me->MonsterYell(YELL_DEATH, LANG_UNIVERSAL, 0);
        DoPlaySoundToSet(me, SOUND_DEATH);
        Summons.DespawnEntry(CREATURE_COLUMN_OF_FIRE);

        if (Unit* Temp = Unit::GetUnit(*me, SpiritGUID[3]))
            Temp->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_DEAD);
    }

    void AttackStart(Unit* who)
    {
        if (Phase == 2)
            AttackStartNoMove(who);
        else
            ScriptedAI::AttackStart(who);
    }

    void DoMeleeAttackIfReady()
    {
        if (!me->IsNonMeleeSpellCast(false))
        {
            if (me->isAttackReady() && me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (Phase == 1 && !Overpower_Timer)
                {
                    uint32 health = me->GetVictim()->GetHealth();
                    me->AttackerStateUpdate(me->GetVictim());
                    if (me->GetVictim() && health == me->GetVictim()->GetHealth())
                    {
                        DoCastVictim( SPELL_OVERPOWER, false);
                        Overpower_Timer = 5000;
                    }
                }
                else me->AttackerStateUpdate(me->GetVictim());
                me->resetAttackTimer();
            }
        }
    }

    void SpawnAdds()
    {
        Creature* pCreature = NULL;
        for (uint8 i = 0; i < 4; ++i)
        {
            pCreature = me->SummonCreature(SpiritInfo[i].entry, SpiritInfo[i].x, SpiritInfo[i].y, SpiritInfo[i].z, SpiritInfo[i].orient, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pCreature)
            {
                pCreature->CastSpell(pCreature, SPELL_SPIRIT_AURA, true);
                pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                pCreature->AddUnitState(UNIT_STATE_STUNNED);
                SpiritGUID[i] = pCreature->GetGUID();
            }
        }
    }

    void DespawnAdds()
    {
        for (uint8 i = 0; i < 4; ++i)
        {
            Unit* Temp = NULL;
            if (SpiritGUID[i])
            {
                if ((Temp = Unit::GetUnit(*me, SpiritGUID[i])))
                {
                    Temp->SetVisible(false);
                    Temp->setDeathState(DEAD);
                }
            }
            SpiritGUID[i] = 0;
        }
    }

    void JustSummoned(Creature* summon)
    {
        Summons.Summon(summon);
    }

    void SummonedCreatureDespawn(Creature* summon)
    {
        Summons.Despawn(summon);
    }

    void EnterPhase(uint32 NextPhase)
    {
        switch (NextPhase)
        {
        case 0:
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            DoTeleportTo(CENTER_X, CENTER_Y, CENTER_Z, 100);
            DoResetThreat();
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, 0);
            me->RemoveAurasDueToSpell(Transform[Phase].unaura);
            DoCast(me, Transform[Phase].spell);
            me->MonsterYell(Transform[Phase].text, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, Transform[Phase].sound);
            if (Phase > 0)
            {
                if (Unit* Temp = Unit::GetUnit(*me, SpiritGUID[Phase - 1]))
                    Temp->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_DEAD);
            }
            if (Unit* Temp = Unit::GetUnit(*me, SpiritGUID[NextPhase - 1]))
                Temp->CastSpell(me, SPELL_SIPHON_SOUL, false); // should m cast on temp
            if (NextPhase == 2)
            {
                me->GetMotionMaster()->Clear();
                DoCast(me, SPELL_ENERGY_STORM, true); // enemy aura
                for (uint8 i = 0; i < 4; ++i)
                {
                    Creature* Vortex = DoSpawnCreature(CREATURE_FEATHER_VORTEX, 0, 0, 0, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                    if (Vortex)
                    {
                        Vortex->CastSpell(Vortex, SPELL_CYCLONE_PASSIVE, true);
                        Vortex->CastSpell(Vortex, SPELL_CYCLONE_VISUAL, true);
                        Vortex->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        Vortex->SetSpeed(MOVE_RUN, 1.0f);
                        Vortex->AI()->AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
                        DoZoneInCombat(Vortex);
                    }
                }
            }
            else
                me->AI()->AttackStart(me->GetVictim());
            if (NextPhase == 3)
            {
                me->RemoveAurasDueToSpell(SPELL_ENERGY_STORM);
                Summons.DespawnEntry(CREATURE_FEATHER_VORTEX);
                me->GetMotionMaster()->MoveChase(me->GetVictim());
            }
            break;
        default:
            break;
        }
        Phase = NextPhase;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!TankGUID)
        {
            if (!UpdateVictim())
                return;

            if (me->GetHealth() < health_20 * (4 - Phase))
                EnterPhase(Phase + 1);
        }

        if (Berserk_Timer <= diff)
        {
            DoCast(me, SPELL_BERSERK, true);
            me->MonsterYell(YELL_BERSERK, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, SOUND_BERSERK);
            Berserk_Timer = 60000;
        }
        else Berserk_Timer -= diff;

        switch (Phase)
        {
        case 0:
            if (Intro_Timer)
            {
                if (Intro_Timer <= diff)
                {
                    me->MonsterYell(YELL_AGGRO, LANG_UNIVERSAL, 0);
                    DoPlaySoundToSet(me, SOUND_AGGRO);
                    Intro_Timer = 0;
                }
                else Intro_Timer -= diff;
            }

            if (Whirlwind_Timer <= diff)
            {
                DoCast(me, SPELL_WHIRLWIND);
                Whirlwind_Timer = 15000 + rand() % 5000;
            }
            else Whirlwind_Timer -= diff;

            if (Grievous_Throw_Timer <= diff)
            {
                if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    DoCast(pTarget, SPELL_GRIEVOUS_THROW, false);
                Grievous_Throw_Timer = 10000;
            }
            else Grievous_Throw_Timer -= diff;
            break;

        case 1:
            if (Creeping_Paralysis_Timer <= diff)
            {
                DoCast(me, SPELL_CREEPING_PARALYSIS);
                Creeping_Paralysis_Timer = 20000;
            }
            else Creeping_Paralysis_Timer -= diff;

            if (Overpower_Timer <= diff)
            {
                // implemented in DoMeleeAttackIfReady()
                Overpower_Timer = 0;
            }
            else Overpower_Timer -= diff;
            break;

        case 2:
            return;

        case 3:
            if (Claw_Rage_Timer <= diff)
            {
                if (!TankGUID)
                {
                    if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    {
                        TankGUID = me->GetVictim()->GetGUID();
                        me->SetSpeed(MOVE_RUN, 5.0f);
                        AttackStart(pTarget); // change victim
                        Claw_Rage_Timer = 0;
                        Claw_Loop_Timer = 500;
                        Claw_Counter = 0;
                    }
                }
                else if (!Claw_Rage_Timer) // do not do this when Lynx_Rush
                {
                    if (Claw_Loop_Timer <= diff)
                    {
                        Unit* pTarget = me->GetVictim();
                        if (!pTarget || !pTarget->isTargetableForAttack()) pTarget = Unit::GetUnit(*me, TankGUID);
                        if (!pTarget || !pTarget->isTargetableForAttack()) pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                        if (pTarget)
                        {
                            AttackStart(pTarget);
                            if (me->IsWithinMeleeRange(pTarget))
                            {
                                DoCast(pTarget, SPELL_CLAW_RAGE_DAMAGE, true);
                                ++Claw_Counter;
                                if (Claw_Counter == 12)
                                {
                                    Claw_Rage_Timer = 15000 + rand() % 5000;
                                    me->SetSpeed(MOVE_RUN, 1.2f);
                                    AttackStart(Unit::GetUnit(*me, TankGUID));
                                    TankGUID = 0;
                                    return;
                                }
                                else
                                    Claw_Loop_Timer = 500;
                            }
                        }
                        else
                        {
                            EnterEvadeMode(); // if (pTarget)
                            return;
                        }
                    }
                    else Claw_Loop_Timer -= diff;
                } //if (TankGUID)
            }
            else Claw_Rage_Timer -= diff;

            if (Lynx_Rush_Timer <= diff)
            {
                if (!TankGUID)
                {
                    if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    {
                        TankGUID = me->GetVictim()->GetGUID();
                        me->SetSpeed(MOVE_RUN, 5.0f);
                        AttackStart(pTarget); // change victim
                        Lynx_Rush_Timer = 0;
                        Claw_Counter = 0;
                    }
                }
                else if (!Lynx_Rush_Timer)
                {
                    Unit* pTarget = me->GetVictim();
                    if (!pTarget || !pTarget->isTargetableForAttack())
                    {
                        pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                        AttackStart(pTarget);
                    }
                    if (pTarget)
                    {
                        if (me->IsWithinMeleeRange(pTarget))
                        {
                            DoCast(pTarget, SPELL_LYNX_RUSH_DAMAGE, true);
                            ++Claw_Counter;
                            if (Claw_Counter == 9)
                            {
                                Lynx_Rush_Timer = 15000 + rand() % 5000;
                                me->SetSpeed(MOVE_RUN, 1.2f);
                                AttackStart(Unit::GetUnit(*me, TankGUID));
                                TankGUID = 0;
                            }
                            else
                                AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
                        }
                    }
                    else
                    {
                        EnterEvadeMode(); // if (pTarget)
                        return;
                    }
                } //if (TankGUID)
            }
            else Lynx_Rush_Timer -= diff;

            break;
        case 4:
            if (Flame_Whirl_Timer <= diff)
            {
                DoCast(me, SPELL_FLAME_WHIRL);
                Flame_Whirl_Timer = 12000;
            }
            Flame_Whirl_Timer -= diff;

            if (Pillar_Of_Fire_Timer <= diff)
            {
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_SUMMON_PILLAR);
                Pillar_Of_Fire_Timer = 10000;
            }
            else Pillar_Of_Fire_Timer -= diff;

            if (Flame_Breath_Timer <= diff)
            {
                if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                    me->SetInFront(pTarget);
                DoCast(me, SPELL_FLAME_BREATH);
                Flame_Breath_Timer = 10000;
            }
            else Flame_Breath_Timer -= diff;
            break;

        default:
            break;
        }

        if (!TankGUID)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_zuljin(Creature* pCreature)
{
    return new boss_zuljinAI (pCreature);
}

struct feather_vortexAI : public ScriptedAI
{
    feather_vortexAI(Creature* c) : ScriptedAI(c) {}

    std::list<Player*> PlayerList;
    uint32 ResetTimer;
    uint32 ChangeTargetTimer;

    void Reset() {}

    void EnterCombat(Unit* /*pTarget*/ ) {}

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (caster->GetTypeId() == TYPEID_PLAYER && !PlayerIsInList(CAST_PLR(caster)))
        {
            if (spell->Id == SPELL_ZAP_INFORM)
            {
                DoCast(caster, SPELL_ZAP_DAMAGE, true);
                PlayerList.push_back(CAST_PLR(caster));
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //if the vortex reach the target, it change his target to another player
        if (me->IsWithinMeleeRange(me->GetVictim()))
            AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));

        if (ResetTimer < diff)
        {
            PlayerList.clear();
            ResetTimer = 500;
        }
        else
            ResetTimer -= diff;

        if (ChangeTargetTimer < diff)
        {
            AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
            ChangeTargetTimer = urand(3000, 6000);
        }
        else
            ChangeTargetTimer -= diff;
    }
    bool PlayerIsInList(Player* pl)
    {
        if (PlayerList.size())
        {
            for (std::list<Player*>::const_iterator plr = PlayerList.begin(); plr != PlayerList.end(); plr++)
            {
                if ((*plr) && pl && (*plr)->GetGUID() == pl->GetGUID())
                    return true;
            }
        }
        return false;
    }
};

CreatureAI* GetAI_feather_vortexAI(Creature* pCreature)
{
    return new feather_vortexAI (pCreature);
}

void AddSC_boss_zuljin()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_zuljin";
    newscript->GetAI = &GetAI_boss_zuljin;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_zuljin_vortex";
    newscript->GetAI = &GetAI_feather_vortexAI;
    newscript->RegisterSelf();
}

