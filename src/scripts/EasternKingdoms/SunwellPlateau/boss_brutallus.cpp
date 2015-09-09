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
SDName: Boss_Brutallus
SD%Complete: 80
SDComment: Find a way to start the intro, best code for the intro
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "sunwell_plateau.h"

enum Quotes
{
    YELL_INTRO                 =   -1580017,
    YELL_INTRO_BREAK_ICE       =   -1580018,
    YELL_INTRO_CHARGE          =   -1580019,
    YELL_INTRO_KILL_MADRIGOSA  =   -1580020,
    YELL_INTRO_TAUNT           =   -1580021,

    YELL_MADR_ICE_BARRIER      =   -1580031,
    YELL_MADR_INTRO            =   -1580032,
    YELL_MADR_ICE_BLOCK        =   -1580033,
    YELL_MADR_TRAP             =   -1580034,
    YELL_MADR_DEATH            =   -1580035,

    YELL_AGGRO                 =   -1580022,
    YELL_KILL1                 =   -1580023,
    YELL_KILL2                 =   -1580024,
    YELL_KILL3                 =   -1580025,
    YELL_LOVE1                 =   -1580026,
    YELL_LOVE2                 =   -1580027,
    YELL_LOVE3                 =   -1580028,
    YELL_BERSERK               =   -1580029,
    YELL_DEATH                 =   -1580030
};

enum Spells
{
    // Encounter related
    SPELL_METEOR_SLASH                 =   45150,
    SPELL_BURN                         =   46394,
    SPELL_STOMP                        =   45185,
    SPELL_BERSERK                      =   26662,
    SPELL_DUAL_WIELD                   =   42459,

    // Cosmetic related
    SPELL_INTRO_FROST_BLAST            =   45203,
    SPELL_INTRO_FROSTBOLT              =   44843,
    SPELL_INTRO_FREEZE                 =   45065,
    SPELL_INTRO_ENCAPSULATE            =   45665,
    SPELL_INTRO_ENCAPSULATE_CHANELLING =   45661,
    SPELL_SUMMON_FELBLAZE_VISUAL       =   44885,
    SPELL_SUMMON_FELBLAZE              =   45069,

    SPELL_SUMMON_FELMYST               =   45069,
    SPELL_SUMMON_DEATH_CLOUD           =   45884
};

struct npc_madrigosaAI : public ScriptedAI
{
    npc_madrigosaAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
    }

    ScriptedInstance* pInstance;

    uint32 IntroPhase;

    void Reset()
    {
        IntroPhase = 0;
    }

    void DamageTaken(Unit* done_by, uint32& damage)
    {
        if (done_by->GetEntry() == BOSS_BRUTALLUS)
            damage = 0;
    }

    void MoveInLineOfSight(Unit* unit)
    {
        if (unit->GetEntry() == BOSS_BRUTALLUS)
            return;

        CreatureAI::MoveInLineOfSight(unit);
    }

    void AttackStart(Unit* unit)
    {
        if (IntroPhase >= 5 && IntroPhase < 13)
            return;

        UnitAI::AttackStart(unit);
    }

    void UpdateAI(const uint32 diff)
    {
        if (IntroPhase >= 5 && IntroPhase < 13)
            return;

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_madrigosa(Creature* creature)
{
    return new npc_madrigosaAI(creature);
}

struct boss_brutallusAI : public ScriptedAI
{
    boss_brutallusAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        if (pInstance && pInstance->GetData(DATA_BRUTALLUS_EVENT) == DONE && pInstance->GetData(DATA_FELMYST_EVENT) != DONE)
        {
            float x, y, z;
            me->GetPosition(x, y, z);
            me->SummonCreature(NPC_FELMYST, x, y, z + 30, me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0);
        }
    }

    ScriptedInstance* pInstance;

    uint32 SlashTimer;
    uint32 BurnTimer;
    uint32 StompTimer;
    uint32 BerserkTimer;

    uint32 IntroPhase;
    uint32 IntroPhaseTimer;
    uint32 IntroAttackTimer;
    uint32 IntroFrostBoltTimer;
    uint32 IntroFlyTimer;

    bool Intro;
    bool Intro_Done;
    bool Enraged;

    void Reset()
    {
        SlashTimer = 11000;
        StompTimer = 30000;
        BurnTimer = 60000;
        BerserkTimer = 360000;

        IntroPhase = 0;
        IntroPhaseTimer = 0;
        IntroFrostBoltTimer = 0;
        IntroAttackTimer = 0;

        Intro = false;
        Intro_Done = false;
        Enraged = false;

        DoCast(me, SPELL_DUAL_WIELD, true);

        if (pInstance && pInstance->GetData(DATA_BRUTALLUS_EVENT) != DONE)
            pInstance->SetData(DATA_BRUTALLUS_EVENT, NOT_STARTED);
    }

    void EnterCombat(Unit* who)
    {
        if (Intro_Done)
            DoScriptText(YELL_AGGRO, me);

        if (pInstance && Intro_Done)
            pInstance->SetData(DATA_BRUTALLUS_EVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit* victim)
    {
        if (Intro_Done)
            DoScriptText(RAND(YELL_KILL1, YELL_KILL2, YELL_KILL3), me);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(YELL_DEATH, me);
        if (pInstance)
        {
            pInstance->SetData(DATA_BRUTALLUS_EVENT, DONE);
            if (Creature* Madrigosa = Unit::GetCreature(*me, pInstance ? pInstance->GetData64(DATA_MADRIGOSA) : 0))
                Madrigosa->CastSpell(Madrigosa, SPELL_SUMMON_DEATH_CLOUD, true);
        }
    }

    void EnterEvadeMode()
    {
        if (!Intro)
            ScriptedAI::EnterEvadeMode();
    }

    void StartIntro()
    {
        if (!Intro)
            return;

        Creature* Madrigosa = Unit::GetCreature(*me, pInstance ? pInstance->GetData64(DATA_MADRIGOSA) : 0);
        if (!Madrigosa)
        {
            //Madrigosa not found, end intro
            EndIntro();
        }
    }

    void EndIntro()
    {
        Intro = false;
        Intro_Done = true;

        if (pInstance)
            pInstance->SetData(DATA_BRUTALLUS_EVENT, SPECIAL);
    }

    void DoIntro()
    {
        Creature* Madrigosa = Unit::GetCreature(*me, pInstance ? pInstance->GetData64(DATA_MADRIGOSA) : 0);
        if (!Madrigosa)
            return;

        float x, y, z, ground_Z;
        switch (IntroPhase)
        {
        case 0:
            me->SetFacingToObject(Madrigosa);
            Madrigosa->SetFacingToObject(me);
            IntroPhaseTimer = 1000;
            break;
        case 1:
            DoScriptText(YELL_MADR_ICE_BARRIER, Madrigosa);
            IntroPhaseTimer = 8000;
            break;
        case 2:
            DoScriptText(YELL_MADR_INTRO, Madrigosa, me);
            IntroPhaseTimer = 7000;
            break;
        case 3:
            DoScriptText(YELL_INTRO, me, Madrigosa);
            IntroPhaseTimer = 4000;
            break;
        case 4:
            Madrigosa->CombatStart(me, true);
            IntroAttackTimer = 2000;
            IntroPhaseTimer = 10000;
            break;
        case 5:
            me->AttackStop();
            Madrigosa->AttackStop();
            Madrigosa->SetSpeed(MOVE_RUN, 3.0f, true);
            Madrigosa->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
            IntroPhaseTimer = 500;
            break;
        case 6:
            Madrigosa->GetMotionMaster()->MovePoint(0, Madrigosa->GetPositionX(), Madrigosa->GetPositionY() + 2, Madrigosa->GetPositionZ() + 8);
            IntroPhaseTimer = 2000;
            break;
        case 7:
            Madrigosa->SetInFront(me);
            Madrigosa->SendMovementFlagUpdate();
            IntroAttackTimer = 3500;
            IntroFrostBoltTimer = 3500;
            IntroPhaseTimer = 13000;
            break;
        case 8:
            DoScriptText(YELL_INTRO_BREAK_ICE, me);
            IntroPhaseTimer = 5000;
            break;
        case 9:
            Madrigosa->SetWalk(true);
            Madrigosa->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
            IntroPhaseTimer = 500;
            break;
        case 10:
            Madrigosa->GetPosition(x, y, z);
            ground_Z = me->GetMap()->GetHeight(x, y, MAX_HEIGHT, true);
            Madrigosa->GetMotionMaster()->MovePoint(1, x, y, ground_Z);
            IntroPhaseTimer = 2000;
            break;
        case 11:
            Madrigosa->SetInFront(me);
            Madrigosa->SendMovementFlagUpdate();
            Madrigosa->CastSpell(me, SPELL_INTRO_ENCAPSULATE_CHANELLING, true);
            DoScriptText(YELL_MADR_TRAP, Madrigosa);
            DoCast(me, SPELL_INTRO_ENCAPSULATE);
            me->SetSpeed(MOVE_RUN, 4.0f, true);

            me->GetPosition(x, y, z);
            me->GetMotionMaster()->MovePoint(1, x - 6, y - 15, z + 10);

            IntroAttackTimer = 3000;
            IntroPhaseTimer = 6000;
            break;
        case 12:
            DoScriptText(YELL_INTRO_CHARGE, me);
            me->SetSpeed(MOVE_RUN, 3.0f, true);
            //me->GetMotionMaster()->MovePath(30000, false);
            me->GetPosition(x, y, z);
            ground_Z = me->GetMap()->GetHeight(x, y, MAX_HEIGHT, true);
            me->GetMotionMaster()->MovePoint(2, x + 6, y + 15, ground_Z);
            IntroPhaseTimer = 5000;
            break;
        case 13:
            me->Kill(Madrigosa);
            DoScriptText(YELL_MADR_DEATH, Madrigosa);
            me->SetSpeed(MOVE_RUN, 2.0f, true);
            IntroPhaseTimer = 7000;
            break;
        case 14:
            DoScriptText(YELL_INTRO_KILL_MADRIGOSA, me);
            me->SetSpeed(MOVE_RUN, 1.0f, true);
            Madrigosa->setDeathState(CORPSE);
            IntroPhaseTimer = 8000;
            break;
        case 15:
            DoScriptText(YELL_INTRO_TAUNT, me);
            IntroPhaseTimer = 5000;
            break;
        case 16:
            EndIntro();
            break;
        }
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (me->isDead())
            return;

        if (who->GetTypeId() != TYPEID_PLAYER || !me->IsWithinDist(who, 70))
            return;

        if (!who->isTargetableForAttack() || !me->IsHostileTo(who))
            return;

        if (!Intro_Done)
        {
            if (Creature* Madrigosa = Unit::GetCreature(*me, pInstance ? pInstance->GetData64(DATA_MADRIGOSA) : 0))
            {
                if (!Madrigosa->isDead())
                    Intro = true;
                else
                    Intro_Done = true;
            }
        }

        if (Intro)
            StartIntro();

        if (Intro_Done)
            ScriptedAI::MoveInLineOfSight(who);
    }

    void AttackStart(Unit* unit)
    {
        if (unit->GetEntry() == NPC_MADRIGOSA)
            if (IntroPhase >= 5 && IntroPhase < 13)
                return;

        if (unit->GetTypeId() == TYPEID_PLAYER && !Intro_Done)
            return;

        UnitAI::AttackStart(unit);
    }

    void DamageTaken(Unit* done_by, uint32& damage)
    {
        if (!Intro_Done)
            damage = 0;
    }

    void UpdateAI(const uint32 diff)
    {
        if (Intro)
        {
            if (IntroPhaseTimer > diff)
                IntroPhaseTimer = IntroPhaseTimer - diff;
            else
            {
                ++IntroPhase;
                if (Creature* Madrigosa = Unit::GetCreature(*me, pInstance ? pInstance->GetData64(DATA_MADRIGOSA) : 0))
                    CAST_AI(npc_madrigosaAI, Madrigosa->AI())->IntroPhase = IntroPhase;

                DoIntro();
            }

            if (IntroPhase == 6 + 1)
            {
                if (IntroAttackTimer < diff)
                {
                    if (Creature* Madrigosa = Unit::GetCreature(*me, pInstance ? pInstance->GetData64(DATA_MADRIGOSA) : 0))
                    {
                        Madrigosa->CastSpell(me, SPELL_INTRO_FREEZE, true);
                        DoCast(me, SPELL_INTRO_FROST_BLAST);
                        IntroAttackTimer = 12000;
                    }
                }
                else IntroAttackTimer -= diff;

                if (IntroFrostBoltTimer < diff)
                {
                    if (Creature* Madrigosa = Unit::GetCreature(*me, pInstance ? pInstance->GetData64(DATA_MADRIGOSA) : 0))
                    {
                        Madrigosa->SetInCombatState(true, me);
                        Madrigosa->CastSpell(me, SPELL_INTRO_FROSTBOLT, true);
                        IntroFrostBoltTimer = 1100;
                    }
                }
                else IntroFrostBoltTimer -= diff;
            }

            if (IntroPhase == 11 + 1)
            {
                if (IntroAttackTimer < diff)
                {
                    if (Creature* Madrigosa = Unit::GetCreature(*me, pInstance ? pInstance->GetData64(DATA_MADRIGOSA) : 0))
                    {
                        me->Attack(Madrigosa, true);
                        me->CastSpell(Madrigosa, 45185, true);
                        IntroAttackTimer = 1700;
                    }
                }
                else IntroAttackTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            if (IntroPhase >= 5 && IntroPhase < 13)
                return;

            DoMeleeAttackIfReady();
            return;
        }

        if (!UpdateVictim())
            return;

        if (SlashTimer <= diff)
        {
            DoCastVictim( SPELL_METEOR_SLASH);
            SlashTimer = 11000;
        }
        else SlashTimer -= diff;

        if (StompTimer <= diff)
        {
            DoScriptText(RAND(YELL_LOVE1, YELL_LOVE2, YELL_LOVE3), me);
            DoCastVictim( SPELL_STOMP);
            StompTimer = 30000;
        }
        else StompTimer -= diff;

        if (BurnTimer <= diff)
        {
            std::list<Unit*> pTargets;
            SelectTargetList(pTargets, 10, SELECT_TARGET_RANDOM, 100, true);
            for (std::list<Unit*>::const_iterator i = pTargets.begin(); i != pTargets.end(); ++i)
                if (!(*i)->HasAura(SPELL_BURN, 0))
                {
                    (*i)->CastSpell((*i), SPELL_BURN, true);
                    break;
                }
            BurnTimer = urand(60000, 180000);
        }
        else BurnTimer -= diff;

        if (BerserkTimer <= diff && !Enraged)
        {
            DoScriptText(YELL_BERSERK, me);
            DoCast(me, SPELL_BERSERK);
            Enraged = true;
        }
        else BerserkTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_brutallus(Creature* pCreature)
{
    return new boss_brutallusAI (pCreature);
}

struct trigger_death_cloudAI : public ScriptedAI
{
    trigger_death_cloudAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
    }

    ScriptedInstance* pInstance;

    uint32 bornTimer;

    void Reset()
    {
        DoCast(me, 45212, true);

        me->SetSpeed(MOVE_WALK, 2.0f);
        me->SetSpeed(MOVE_RUN, 2.0f);

        bornTimer = 10000;

        if (Creature *Madrigosa = Unit::GetCreature(*me, pInstance ? pInstance->GetData64(DATA_MADRIGOSA) : 0))
            DoCast(Madrigosa, 44885, true);
    }

    void EnterCombat(Unit* who) {}

    void UpdateAI(uint32 const diff)
    {
        if (!bornTimer)
            return;

        if (bornTimer <= diff)
        {
            if (Creature *Madrigosa = Unit::GetCreature(*me, pInstance ? pInstance->GetData64(DATA_MADRIGOSA) : 0))
            {
                Madrigosa->RemoveAurasDueToSpell(44885);
                Madrigosa->SetVisibility(VISIBILITY_OFF);
                Madrigosa->CastSpell(Madrigosa, SPELL_SUMMON_FELMYST, true);
                bornTimer = 0;
                me->DisappearAndDie();
            }
        }
        else
            bornTimer -= diff;
    }
};

CreatureAI* GetAI_trigger_death_cloud(Creature* creature)
{
    return new trigger_death_cloudAI(creature);
}

void AddSC_boss_brutallus()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_brutallus";
    newscript->GetAI = &GetAI_boss_brutallus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_madrigosa";
    newscript->GetAI = &GetAI_npc_madrigosa;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "trigger_death_cloud";
    newscript->GetAI = &GetAI_trigger_death_cloud;
    newscript->RegisterSelf();
}
