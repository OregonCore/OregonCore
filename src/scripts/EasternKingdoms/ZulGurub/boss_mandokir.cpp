/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * Copyright (C) 2011 Oregon <http://www.oregoncore.com/>
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

/* ScriptData
SDName: Boss_Mandokir
SD%Complete: 100
SDComment: Core related resurrection issue, see comment. Random charge timer might be inaccurate.
SDCategory: Zul'Gurub
EndScriptData */

#include "ScriptPCH.h"
#include "zulgurub.h"

enum Texts
{
    SAY_AGGRO           =   -1309015,
    SAY_DING_KILL       =   -1309016,
    SAY_GRATS_JINDO     =   -1309017,
    SAY_GAZE            =   -1309018,
    SAY_GAZE_WHISPER    =   -1309019,
    SAY_REVIVE          =   -1309024
};

enum Spells
{
    SPELL_CHARGE          = 24408,
    SPELL_CHARGE_GAZE     = 24315,
    SPELL_CLEAVE          = 20691,
    SPELL_FEAR            = 29321,
    SPELL_WHIRLWIND       = 24236,
    SPELL_MORTAL_STRIKE   = 24573,
    SPELL_ENRAGE          = 23537,
    SPELL_GAZE            = 24314,
    SPELL_LEVEL_UP        = 24312,
    SPELL_MOUNT           = 23243,

    // Ohgan's Spells
    SPELL_SUNDERARMOR     = 24317,
    SPELL_THRASH          = 3391,
    SPELL_EXECUTE         = 7160,

    // Chained Spirit's Spell
    SPELL_REVIVE          = 24341
};

enum Summons
{
    OHGAN                   = 14988,
    CHAINED_SPIRIT          = 15117
};

Unit* CHAINED_SPIRIT_SUMMONS [15];

float CHAINED_SPIRIT_LOC [15][4] =
{
    {-12272.1f, -1942.250f, 135.231f, 0.56704f},
    {-12277.2f, -1935.940f, 136.778f, 0.26859f},
    {-12282.2f, -1920.320f, 131.593f, 6.03892f},
    {-12283.4f, -1928.490f, 135.307f, 0.09580f},
    {-12246.1f, -1893.090f, 134.157f, 4.84119f},
    {-12254.5f, -1895.100f, 133.670f, 4.79799f},
    {-12262.5f, -1899.160f, 131.824f, 5.24567f},
    {-12263.5f, -1945.960f, 132.431f, 0.64166f},
    {-12258.0f, -1951.820f, 131.170f, 0.77125f},
    {-12250.6f, -1958.700f, 132.913f, 0.85371f},
    {-12241.2f, -1966.670f, 134.090f, 1.17652f},
    {-12232.5f, -1972.890f, 132.928f, 1.10191f},
    {-12220.4f, -1973.710f, 132.390f, 1.52367f},
    {-12209.5f, -1973.580f, 132.171f, 1.78677f},
    {-12197.1f, -1974.770f, 131.102f, 1.45926f}
};

struct boss_mandokirAI : public ScriptedAI
{
    boss_mandokirAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    uint32 KillCount;
    uint32 Gaze_Timer;
    uint32 TargetInRange;
    uint32 Cleave_Timer;
    uint32 Whirlwind_Timer;
    uint32 Fear_Timer;
    uint32 MortalStrike_Timer;
    uint32 Check_Timer;
    uint32 Charge_Timer;


    float Threat_Count;
    bool  Threat_stored;

    ScriptedInstance *pInstance;

    bool endGaze;
    bool someGazed;
    bool RaptorDead;
    bool CombatStart;

    uint64 GazeTarget;

    // Spirit Resurrection
    struct Spirit_Resurrection
    {
        Unit* pUnit;
        int Spirit;
        bool isBeingRezzed;
    };

    uint32 Spirit_Number; 

    uint32 Move_Timer;

    std::vector<Spirit_Resurrection> Resurrection;
    std::vector<std::vector<Spirit_Resurrection>::iterator> eraseUnit;

    void Reset()
    {
        KillCount = 0;
        Gaze_Timer = 33000;
        Cleave_Timer = 7000;
        Whirlwind_Timer = 20000;
        Fear_Timer = 1000;
        MortalStrike_Timer = 1000;
        Check_Timer = 1000;
        Charge_Timer = 20000;            // correct cooldown is unknown, based on information

        Threat_Count = 0;
        Threat_stored = false;

        TargetInRange = 0;

        GazeTarget = 0;

        someGazed = false;
        endGaze = false;
        RaptorDead = false;
        CombatStart = false;

        DoCast(me, SPELL_MOUNT);

        Move_Timer = 3000;

        // remove all the Chained Spirits
        uint32 i;
        for (i = 0; i < 15; ++i)
        {
            if (CHAINED_SPIRIT_SUMMONS[i])
            CHAINED_SPIRIT_SUMMONS[i]->RemoveFromWorld();
        }

        Resurrection.clear();
        eraseUnit.clear();
        Spirit_Number = 0;

    }

    void JustDied(Unit* Killer)
    {
        // remove all the Chained Spirits
        uint32 i;
        for (i = 0; i < 15; ++i)
        {
            if (CHAINED_SPIRIT_SUMMONS[i])
            CHAINED_SPIRIT_SUMMONS[i]->RemoveFromWorld();
        }
    }

    void KilledUnit(Unit* victim)
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
        {
            ++KillCount;

            if (KillCount == 3)
            {
                DoScriptText(SAY_DING_KILL, me);

                if (pInstance)
                {
                    uint64 JindoGUID = pInstance->GetData64(DATA_JINDO);
                    if (JindoGUID)
                    {
                        if (Unit* jTemp = Unit::GetUnit(*me, JindoGUID))
                        {
                            if (jTemp->isAlive())
                                DoScriptText(SAY_GRATS_JINDO, jTemp);
                        }
                    }
                }
            DoCast(me, SPELL_LEVEL_UP, true);
            KillCount = 0;
            }

            if (Spirit_Number < 15)
            {
                Spirit_Resurrection Temp;
            
                Temp.isBeingRezzed = false;
                Temp.pUnit = victim;
                Temp.Spirit = Spirit_Number;
            
                Resurrection.push_back(Temp);

                ++Spirit_Number;
            }
        }
    }

    void EnterCombat(Unit * /*who*/)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (me->getVictim() && me->isAlive())
        {
            if (!CombatStart)
            {
                // At combat Start Mandokir is mounted so we must unmount it first
                me->Unmount();

                // And summon his raptor
                me->SummonCreature(OHGAN, me->getVictim()->GetPositionX(), me->getVictim()->GetPositionY(), me->getVictim()->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                CombatStart = true;

                uint32 i;
                for (i = 0; i < 15; ++i)
                    CHAINED_SPIRIT_SUMMONS[i] = me->SummonCreature(CHAINED_SPIRIT, CHAINED_SPIRIT_LOC[i][0], CHAINED_SPIRIT_LOC[i][1], CHAINED_SPIRIT_LOC[i][2], CHAINED_SPIRIT_LOC[i][3], TEMPSUMMON_CORPSE_TIMED_DESPAWN , 5000);
            }

            // Random Charge
            if (Charge_Timer <= diff)
            {
                uint32 i = 0;
                Unit* pTarget;
                do
                {
                    pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                    ++i;
                }
                while (!(pTarget->GetTypeId() == TYPEID_PLAYER) || (i < 15));

                if (DoGetThreat(me->getVictim()))
                    DoModifyThreatPercent(me->getVictim(), -50);

                DoCast(pTarget, SPELL_CHARGE);
                AttackStart(pTarget);

                Charge_Timer = 20000;
            }
            else
                Charge_Timer -= diff;

            if (Move_Timer <= diff)
            {
                   for (std::vector<Spirit_Resurrection>::iterator itr = Resurrection.begin(); itr != Resurrection.end(); ++itr)
                {
                    if (!itr->isBeingRezzed)
                    {
                        CHAINED_SPIRIT_SUMMONS[itr->Spirit]->GetMotionMaster()->MovePoint(itr->pUnit->GetMapId(), itr->pUnit->GetPositionX(), itr->pUnit->GetPositionY(), itr->pUnit->GetPositionZ());
                        itr->isBeingRezzed = true; 
                    }

                    if (itr->isBeingRezzed)
                    {
                        if (CHAINED_SPIRIT_SUMMONS[itr->Spirit]->IsWithinDist3d(itr->pUnit->GetPositionX(), itr->pUnit->GetPositionY(), itr->pUnit->GetPositionZ(), 10.0f))
                        {
                            CHAINED_SPIRIT_SUMMONS[itr->Spirit]->StopMoving();

                            CHAINED_SPIRIT_SUMMONS[itr->Spirit]->MonsterWhisper(SAY_REVIVE, itr->pUnit->GetGUID());

                            Player* pPlayer = Player::GetPlayer(*me, itr->pUnit->GetGUID());
                            //DoCast(pPlayer, SPELL_REVIVE);   // core not supporting npc resurrection requests? use other method:
                            me->getVictim()->CastSpell(itr->pUnit, SPELL_REVIVE, false, 0, 0, me->getVictim()->GetGUID());  // not great method, but no other way currently

                            CHAINED_SPIRIT_SUMMONS[itr->Spirit]->RemoveFromWorld();
                            eraseUnit.push_back(itr);
                        }
                    }
                }
                Move_Timer = 3000;

                for (std::vector<std::vector<Spirit_Resurrection>::iterator>::iterator itr = eraseUnit.begin(); itr != eraseUnit.end(); ++itr)
                Resurrection.erase(*itr);
                eraseUnit.clear();
            }
            else
                Move_Timer -= diff;

            if (Gaze_Timer <= diff)                         // Every 20 seconds Mandokir will check this
            {
                if (GazeTarget)
                {
                    Unit* pUnit = Unit::GetUnit(*me, GazeTarget);
                    if ((pUnit) && (pUnit->isInCombat()) && (DoGetThreat(pUnit) > Threat_Count))
                    {
                        DoCast(pUnit, SPELL_CHARGE_GAZE);
                        AttackStart(pUnit);
                        Threat_Count = 0.0f;
                    }
                }
                someGazed = false;
                Gaze_Timer = 20000;
            }
            else
                Gaze_Timer -= diff;

            if ((Gaze_Timer < 8000) && !someGazed)       // 8 second(cast time + expire time) before the check for the gaze effect Mandokir will cast gaze debuff on a random target
            {
                uint32 i = 0;
                Unit* pTarget;
                do
                {
                    pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                    ++i;
                }
                while (!(pTarget->GetTypeId() == TYPEID_PLAYER) || ( i < 15));   // 15 should be enough out of a 20-man raid to select a player instead of totem

                if (pTarget)  // did we find a player?
                {
                    DoScriptText(SAY_GAZE, me, pTarget);
                    DoCast(pTarget, SPELL_GAZE);
                    me->MonsterWhisper(SAY_GAZE_WHISPER, pTarget->GetGUID());
                    GazeTarget = pTarget->GetGUID();
                    someGazed = true;
                    endGaze = true;
                    Threat_stored = false;
                }
            }

            if (Gaze_Timer < 6000 && !Threat_stored)
            {
                Unit* pThreat = Unit::GetUnit(*me, GazeTarget);
                Threat_Count = DoGetThreat(pThreat);                // store threat when you're gazed
                Threat_stored = true;
            }

            if ((Gaze_Timer < 1000) && endGaze)               // 1 second before the debuff expires, check whether the GazeTarget is in LoS
            {
                Unit* pSight = Unit::GetUnit(*me, GazeTarget);
                Player* pPlayer = Player::GetPlayer(*me, pSight->GetGUID());
                if (pSight && !(me->IsWithinLOSInMap(pSight)))  // Is the target in our LOS? If not, teleport to me:
                    pPlayer->TeleportTo(pPlayer->GetMapId(), me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());

                endGaze = false;
            }

            if (!someGazed)
            {
                // Cleave
                if (Cleave_Timer <= diff)
                {
                    DoCast(me->getVictim(), SPELL_CLEAVE);
                    Cleave_Timer = 7000;
                }
                else
                    Cleave_Timer -= diff;

                // Whirlwind
                if (Whirlwind_Timer <= diff)
                {
                    DoCast(me, SPELL_WHIRLWIND);
                    Whirlwind_Timer = 18000;
                }
                else
                    Whirlwind_Timer -= diff;

                // If more than 3 targets in melee range Mandokir will cast fear
                if (Fear_Timer <= diff)
                {
                    TargetInRange = 0;

                    std::list<HostileReference*>::const_iterator i = me->getThreatManager().getThreatList().begin();
                    for (; i != me->getThreatManager().getThreatList().end(); ++i)
                    {
                        Unit* pUnit = Unit::GetUnit(*me, (*i)->getUnitGuid());
                        if (pUnit && me->IsWithinMeleeRange(pUnit))
                            ++TargetInRange;
                    }

                    if (TargetInRange > 3)
                        DoCast(me->getVictim(), SPELL_FEAR);

                    Fear_Timer = 4000;
                }
                else
                    Fear_Timer -=diff;

                // Mortal Strike if target is below 50% hp
                if (me->getVictim() && me->getVictim()->GetHealth() < me->getVictim()->GetMaxHealth() * 0.5f)
                {
                    if (MortalStrike_Timer <= diff)
                    {
                        DoCast(me->getVictim(), SPELL_MORTAL_STRIKE);
                        MortalStrike_Timer = 15000;
                    }
                    else
                        MortalStrike_Timer -= diff;
                }
                else
                    MortalStrike_Timer -= diff;
            }
            // Checking if Ohgan is dead. If yes Mandokir will enrage.
            if (Check_Timer <= diff)
            {
                if (pInstance)
                {
                    if (pInstance->GetData(TYPE_OHGAN) == DONE)
                    {
                        if (!RaptorDead)
                        {
                            DoCast(me, SPELL_ENRAGE);
                            RaptorDead = true;
                        }
                    }
                }

                Check_Timer = 1000;
            }
            else
                Check_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    }
};

// Ohgan
struct mob_ohganAI : public ScriptedAI
{
    mob_ohganAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    uint32 SunderArmor_Timer;
    uint32 Thrash_Timer;
    uint32 Execute_Timer;
    ScriptedInstance *pInstance;

    void Reset()
    {
        SunderArmor_Timer = 5000;
        Thrash_Timer = urand(5000, 9000);
        Execute_Timer = 1000;
    }

    void EnterCombat(Unit * /*who*/) {}

    void JustDied(Unit* /*Killer*/)
    {
        if (pInstance)
            pInstance->SetData(TYPE_OHGAN, DONE);
    }

    void UpdateAI (const uint32 diff)
    {
        // Return since we have no target
        if (!UpdateVictim())
            return;

        // SunderArmor_Timer
        if (SunderArmor_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_SUNDERARMOR);
            SunderArmor_Timer = 10000 + rand()%5000;
        }
        else
            SunderArmor_Timer -= diff;

        // Thrash_Timer
        if (Thrash_Timer <= diff)
        {
            DoCast(me, SPELL_THRASH);
            Thrash_Timer = urand(5000, 9000);
        }
        else
            Thrash_Timer -= diff;

        // Execute_Timer
        if (me->getVictim()->GetHealth() <= me->getVictim()->GetMaxHealth() * 0.2f)  // check health first
        {
            if (Execute_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_EXECUTE);
                Execute_Timer = 10000;
            }
            else
                Execute_Timer -= diff;
        }
        else
            Execute_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_mandokir(Creature* pCreature)
{
    return new boss_mandokirAI (pCreature);
}

CreatureAI* GetAI_mob_ohgan(Creature* pCreature)
{
    return new mob_ohganAI (pCreature);
}

void AddSC_boss_mandokir()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_mandokir";
    newscript->GetAI = &GetAI_boss_mandokir;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ohgan";
    newscript->GetAI = &GetAI_mob_ohgan;
    newscript->RegisterSelf();
}

