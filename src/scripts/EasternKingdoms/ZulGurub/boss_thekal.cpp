/*
 * Copyright (C) 2010-2014 OregonCore <http://www.oregoncore.com/>
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
SDName: Boss_Thekal
SD%Complete: 100
SDComment: Combat animation after resurrection is missing. Same issue with the Mograine and Whitemane fight. Core Issue?
SDCategory: Zul'Gurub
EndScriptData */

#include "ScriptPCH.h"
#include "zulgurub.h"

enum Texts
{
    SAY_TRANSFORM       =       -1309009,
    SAY_DEATH           =       -1309010
};

enum Spells
{
    // High Priest Thekal - Humanoid
    SPELL_MORTALCLEAVE      =   22859,
    SPELL_RESURRECT         =   24173,
    SPELL_SILENCE           =   22666,

    // High Priest Thekal - Tiger
    SPELL_FRENZY            =   8269,
    SPELL_FORCEPUNCH        =   24189,
    SPELL_CHARGE            =   24193,
    SPELL_ENRAGE            =   8269,
    SPELL_SUMMONTIGERS      =   24183,
    SPELL_TIGER_FORM        =   24169,
                          

    // Zealot Lor'Khan Spells
    SPELL_SHIELD            =   20545,
    SPELL_BLOODLUST         =   24185,
    SPELL_GREATERHEAL       =   24208,
    SPELL_DISARM            =   6713,

    // Zealot Zath Spells
    SPELL_SWEEPINGSTRIKES   =   18765,
    SPELL_SINISTERSTRIKE    =   15581,
    SPELL_GOUGE             =   12540,
    SPELL_KICK              =   15614,
    SPELL_BLIND             =   21060
};

uint32 CheckDeath_Timer;

// Not properly being read from instance_zulgurub.cpp, so handle internally
uint64 ThekalGUID;
uint64 LorkhanGUID;
uint64 ZathGUID;

bool ThekalDead;
bool LorkhanDead;
bool ZathDead;

bool PhaseTwo;

bool IsResurrecting;

void FakeDeath(Unit* Target)
{
    Target->GetMotionMaster()->MovementExpired();
    Target->GetMotionMaster()->MoveIdle();

    Target->SetHealth(0);

    if (Target->IsNonMeleeSpellCasted(false))
        Target->InterruptNonMeleeSpells(false);

    Target->ClearComboPointHolders();
    Target->RemoveAllAuras();
    Target->ClearAllReactives();

    Target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    Target->SetStandState(UNIT_STAND_STATE_DEAD);
}

void Resurrect(Unit* Target)
{
    Target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    Target->SetStandState(UNIT_STAND_STATE_STAND);

    if (Target->getVictim())
        Target->GetMotionMaster()->MoveChase(Target->getVictim());

    Target->SetHealth(int(Target->GetMaxHealth() * 1.0f));
    if (Target->GetMaxPower(POWER_MANA) > 0)
        Target->SetPower(POWER_MANA, int(Target->GetMaxPower(POWER_MANA) * 1.0f));
}


struct boss_thekalAI : public ScriptedAI
{
    boss_thekalAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    // Humanoid Timer
    uint32 MortalCleave_Timer;
    uint32 Silence_Timer;

    // Tiger Timer
    uint32 ForcePunch_Timer;
    uint32 Charge_Timer;
    uint32 SummonTigers_Timer;
    uint32 Enrage_Timer;
    uint32 Frenzy_Timer;

    ScriptedInstance *pInstance;

    bool Enraged;

    void Reset()
    {
        MortalCleave_Timer = 4000;
        Silence_Timer = 9000;
        Frenzy_Timer = 30000;
        ForcePunch_Timer = 4000;
        Charge_Timer = 12000;
        SummonTigers_Timer = 25000;
        CheckDeath_Timer = 10000;

        PhaseTwo = false;
        Enraged = false;

        ThekalGUID = me->GetGUID();
        ThekalDead = false;

        IsResurrecting = false;

        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetStandState(UNIT_STAND_STATE_STAND);
     }

    void JustDied(Unit* /*Killer*/)
    {
        if (pInstance)
        {
            pInstance->SetData(TYPE_THEKAL, DONE);
            DoScriptText(SAY_DEATH, me);
        }
    }

    void JustReachedHome()
    {
        if (pInstance)
            pInstance->SetData(TYPE_THEKAL, NOT_STARTED);
    }

    void SpellHit(Unit* /*pWho*/, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_RESURRECT)
        {
            Resurrect(me);
            IsResurrecting = false;
            ThekalDead = false;
        }
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
        if (uiDamage < me->GetHealth() || ThekalDead)
            return;

        if (!pInstance)
            return;

        ThekalDead = true;

        FakeDeath(me);

        if (!ZathDead && !LorkhanDead)
            CheckDeath_Timer = 10000;

        uiDamage = 0;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (!PhaseTwo)
        {
            // Transform to Phase 2
            if (LorkhanDead && ZathDead && ThekalDead)
            {
                me->InterruptNonMeleeSpells(false);
                DoCast(me, SPELL_TIGER_FORM);
                Resurrect(me);
                me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                const CreatureInfo *cinfo = me->GetCreatureInfo();
                me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->mindmg +((cinfo->mindmg / 100) * 40)));
                me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg +((cinfo->maxdmg / 100) * 40)));
                me->UpdateDamagePhysical(BASE_ATTACK);
                DoResetThreat();
                PhaseTwo = true;
                DoScriptText(SAY_TRANSFORM,me);
            }

            if (ThekalDead)
                return;

            // Resurrecting
            if (LorkhanDead || ZathDead)
            {
                if (!IsResurrecting && (CheckDeath_Timer - 2000) <= diff)
                {
                    if (ZathDead)
                        if (Unit* pZath = Unit::GetUnit(*me, ZathGUID))
                            pZath->CastSpell(pZath, SPELL_RESURRECT, false, 0, 0, pZath->GetGUID());
                    if (LorkhanDead)
                        if (Unit* pLorkhan = Unit::GetUnit(*me, LorkhanGUID))
                            pLorkhan->CastSpell(pLorkhan, SPELL_RESURRECT, false, 0, 0, pLorkhan->GetGUID());
                    IsResurrecting = true;
                }
                else
                    CheckDeath_Timer -= diff;
            }

            // Mortal Cleave Timer
            if (MortalCleave_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_MORTALCLEAVE);
                MortalCleave_Timer = 15000 + rand()%5000;
            } 
            else 
                MortalCleave_Timer -= diff;

            // Silence Timer
            if (Silence_Timer <= diff)  // Cast on a random target in range?
            {
                DoCast(me->getVictim(), SPELL_SILENCE);
                Silence_Timer = 20000 + rand()%5000;
            }
            else 
                Silence_Timer -= diff;

        }
        else
        // Phase 2 spells
        {
            // Enrage below 25% health
            if (!Enraged && (me->GetHealth() < (me->GetMaxHealth() * 0.25)))
            {
                DoCast(me, SPELL_ENRAGE);
                Enraged = true;
            }

            // Charge Timer
            if (Charge_Timer <= diff)
            {
                if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                {
                    DoCast(pTarget, SPELL_CHARGE);
                    DoResetThreat();
                    AttackStart(pTarget);
                }
                Charge_Timer = 15000 + rand()%7000;
            }
            else
                Charge_Timer -= diff;

            // Frenzy Timer
            if (Frenzy_Timer <= diff)
            {
                DoCast(me, SPELL_FRENZY);
                Frenzy_Timer = 30000;
            } 
            else
                Frenzy_Timer -= diff;

            // Force Punch Timer
            if (ForcePunch_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_SILENCE);
                ForcePunch_Timer = 16000 + rand()%5000;
            } 
            else
                ForcePunch_Timer -= diff;

            // Summon Zulian Guardians
            if (SummonTigers_Timer <= diff)
            {
                DoCast(me->getVictim(), SPELL_SUMMONTIGERS);
                SummonTigers_Timer = 10000 + rand()%4000;
            } 
            else
                SummonTigers_Timer -= diff;
        }

        DoMeleeAttackIfReady();
    }
};

struct mob_zealot_lorkhanAI : public ScriptedAI
{
    mob_zealot_lorkhanAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    uint32 Shield_Timer;
    uint32 BloodLust_Timer;
    uint32 GreaterHeal_Timer;
    uint32 Disarm_Timer;

    ScriptedInstance *pInstance;

    void Reset()
    {
        Shield_Timer = 1000;
        BloodLust_Timer = 16000;
        GreaterHeal_Timer = 32000;
        Disarm_Timer = 6000;

        LorkhanGUID = me->GetGUID();
        LorkhanDead = false;

        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetStandState(UNIT_STAND_STATE_STAND);
    }

    void SpellHit(Unit* /*pWho*/, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_RESURRECT && !PhaseTwo)
        {
            Resurrect(me);
            IsResurrecting = false;
            LorkhanDead = false;
        }
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
        if (uiDamage < me->GetHealth() || LorkhanDead)
            return;

        if (!pInstance)
            return;

        LorkhanDead = true;

        FakeDeath(me);

        if (!ZathDead && !ThekalDead)
            CheckDeath_Timer = 10000;

        uiDamage = 0;
    }
    
    void UpdateAI (const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (PhaseTwo)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->setDeathState(JUST_DIED);
            me->SetHealth(0);
            me->GetMotionMaster()->Clear();
            me->clearUnitState(UNIT_STAT_ALL_STATE);
            me->LoadCreaturesAddon(true);
            return;
        }

        if (LorkhanDead)
            return;

        // Resurrect
        if (ThekalDead || ZathDead)
        {
            if (!IsResurrecting && (CheckDeath_Timer - 2000) <= diff)
            {
                if (ZathDead)
                    if (Unit* pZath = Unit::GetUnit(*me, ZathGUID))
                        pZath->CastSpell(pZath, SPELL_RESURRECT, false, 0, 0, pZath->GetGUID());
                if (ThekalDead)
                    if (Unit* pThekal = Unit::GetUnit(*me, ThekalGUID))
                        pThekal->CastSpell(pThekal, SPELL_RESURRECT, false, 0, 0, pThekal->GetGUID());
                IsResurrecting = true;
            }
            else
            if (ThekalDead)
                CheckDeath_Timer -= diff;
        }

        // Shield Timer
        if (Shield_Timer <= diff)
        {
            DoCast(me, SPELL_SHIELD);
            Shield_Timer = 61000;
        }
        else 
            Shield_Timer -= diff;

        // BloodLust Timer
        if (BloodLust_Timer <= diff)
        {
            DoCast(me, SPELL_BLOODLUST);
            BloodLust_Timer = 20000 + rand()%8000;
        }
        else
            BloodLust_Timer -= diff;

        // Cast Greater heal on Thekal or Zath if they are in melee range.
        if (GreaterHeal_Timer <= diff)
        {
            if (pInstance)
            {
                Unit *pThekal = Unit::GetUnit((*me), ThekalGUID);
                Unit *pZath = Unit::GetUnit((*me), ZathGUID);

                if (!pThekal || !pZath)
                    return;

                // Might use a loop and a vector in the future
                // Target lowest ally in range
                if (me->IsWithinMeleeRange(pThekal) && me->IsWithinMeleeRange(pZath))
                    if ((pThekal->GetHealth() / pThekal->GetMaxHealth()) < (pZath->GetHealth() / pZath->GetMaxHealth()))
                        if ((pThekal->GetHealth() / pThekal->GetMaxHealth()) < (me->GetHealth() / me->GetMaxHealth()))
                            DoCast(pThekal, SPELL_GREATERHEAL);
                        else
                            DoCast(me, SPELL_GREATERHEAL);
                    else
                        if ((pZath->GetHealth() / pZath->GetMaxHealth()) < (me->GetHealth() / me->GetMaxHealth()))
                            DoCast(pZath, SPELL_GREATERHEAL);
                        else
                            DoCast(me, SPELL_GREATERHEAL);
                else
                if (me->IsWithinMeleeRange(pThekal) && !me->IsWithinMeleeRange(pZath))
                    if ((pThekal->GetHealth() / pThekal->GetMaxHealth()) < (me->GetHealth() / me->GetMaxHealth()))
                            DoCast(pThekal, SPELL_GREATERHEAL);
                        else
                            DoCast(me, SPELL_GREATERHEAL);
                else
                if (!me->IsWithinMeleeRange(pThekal) && me->IsWithinMeleeRange(pZath))
                    if ((pZath->GetHealth() / pZath->GetMaxHealth()) < (me->GetHealth() / me->GetMaxHealth()))
                            DoCast(pZath, SPELL_GREATERHEAL);
                        else
                            DoCast(me, SPELL_GREATERHEAL);
                else
                if (!me->IsWithinMeleeRange(pThekal) && !me->IsWithinMeleeRange(pZath))
                    DoCast(me, SPELL_GREATERHEAL);
            }
            GreaterHeal_Timer = 15000 + rand()%5000;
        }
        else
            GreaterHeal_Timer -= diff;

        // Disarm Timer
        if (Disarm_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_DISARM);
            Disarm_Timer = 15000 + rand()%10000;
        }
        else
            Disarm_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct mob_zealot_zathAI : public ScriptedAI
{
    mob_zealot_zathAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    uint32 SweepingStrikes_Timer;
    uint32 SinisterStrike_Timer;
    uint32 Gouge_Timer;
    uint32 Kick_Timer;
    uint32 Blind_Timer;

    ScriptedInstance *pInstance;

    void Reset()
    {
        SweepingStrikes_Timer = 13000;
        SinisterStrike_Timer = 8000;
        Gouge_Timer = 25000;
        Kick_Timer = 18000;
        Blind_Timer = 5000;

        ZathGUID = me->GetGUID();
        ZathDead = false;

        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetStandState(UNIT_STAND_STATE_STAND);
    }

    void SpellHit(Unit* /*pWho*/, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_RESURRECT && !PhaseTwo)
        {
            Resurrect(me);
            IsResurrecting = false;
            ZathDead = false;
        }
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
        if (uiDamage < me->GetHealth() || ZathDead)
            return;

        if (!pInstance)
            return;

        ZathDead = true;

        FakeDeath(me);

        if (!LorkhanDead && !ThekalDead)
            CheckDeath_Timer = 10000;

        uiDamage = 0;
    }

    void UpdateAI (const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (PhaseTwo)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->setDeathState(JUST_DIED);
            me->SetHealth(0);
            me->GetMotionMaster()->Clear();
            me->clearUnitState(UNIT_STAT_ALL_STATE);
            me->LoadCreaturesAddon(true);
            return;
        }

        if (ZathDead)
            return;

        // Resurrect
        if (ThekalDead || LorkhanDead)
        {
            if (!IsResurrecting && (CheckDeath_Timer - 2000) <= diff)
            {
                if (LorkhanDead)
                    if (Unit* pLorkhan = Unit::GetUnit(*me, LorkhanGUID))
                        pLorkhan->CastSpell(pLorkhan, SPELL_RESURRECT, false, 0, 0, pLorkhan->GetGUID());
                if (ThekalDead)
                    if (Unit* pThekal = Unit::GetUnit(*me, ThekalGUID))
                        pThekal->CastSpell(pThekal, SPELL_RESURRECT, false, 0, 0, pThekal->GetGUID());
                IsResurrecting = true;
            }
            else
            if (LorkhanDead && ThekalDead)
                CheckDeath_Timer -= diff;
        }

        // Sweeping Strikes Timer
        if (SweepingStrikes_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_SWEEPINGSTRIKES);
            SweepingStrikes_Timer = 22000 + rand()%4000;
        }
        else
            SweepingStrikes_Timer -= diff;

        // Sinister Strike Timer
        if (SinisterStrike_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_SINISTERSTRIKE);
            SinisterStrike_Timer = 8000 + rand()%8000;
        }
        else
            SinisterStrike_Timer -= diff;

        // Gouge Timer
        if (Gouge_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_GOUGE);

            if (DoGetThreat(me->getVictim()))
                DoModifyThreatPercent(me->getVictim(), -100);

            Gouge_Timer = 17000 + rand()%10000;
        }
        else
            Gouge_Timer -= diff;

        // Kick Timer
        if (Kick_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_KICK);
            Kick_Timer = 15000 + rand()%10000;
        }
        else
            Kick_Timer -= diff;

        // Blind Timer
        if (Blind_Timer <= diff)
        {
            DoCast(me->getVictim(), SPELL_BLIND);
            Blind_Timer = 10000 + rand()%10000;
        }
        else
            Blind_Timer -= diff;


        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_thekal(Creature* pCreature)
{
    return new boss_thekalAI (pCreature);
}

CreatureAI* GetAI_mob_zealot_lorkhan(Creature* pCreature)
{
    return new mob_zealot_lorkhanAI (pCreature);
}

CreatureAI* GetAI_mob_zealot_zath(Creature* pCreature)
{
    return new mob_zealot_zathAI (pCreature);
}

void AddSC_boss_thekal()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_thekal";
    newscript->GetAI = &GetAI_boss_thekal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_zealot_lorkhan";
    newscript->GetAI = &GetAI_mob_zealot_lorkhan;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_zealot_zath";
    newscript->GetAI = &GetAI_mob_zealot_zath;
    newscript->RegisterSelf();
}
