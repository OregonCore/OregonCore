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
SDName: Boss_Hydromancer_Thespia
SD%Complete: 80
SDComment: Needs additional adjustments (when instance script is adjusted)
SDCategory: Coilfang Resevoir, The Steamvault
EndScriptData */

/* ContentData
boss_hydromancer_thespia
mob_coilfang_waterelemental
EndContentData */

#include "ScriptPCH.h"
#include "steam_vault.h"

#define SAY_SUMMON                  -1545000
#define SAY_AGGRO_1                 -1545001
#define SAY_AGGRO_2                 -1545002
#define SAY_AGGRO_3                 -1545003
#define SAY_SLAY_1                  -1545004
#define SAY_SLAY_2                  -1545005
#define SAY_DEAD                    -1545006

#define SPELL_LIGHTNING_CLOUD       25033
#define SPELL_LUNG_BURST            31481
#define SPELL_ENVELOPING_WINDS      31718

struct boss_thespiaAI : public ScriptedAI
{
    boss_thespiaAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();
    }

    ScriptedInstance *pInstance;
    bool HeroicMode;

    uint32 LightningCloud_Timer;
    uint32 LungBurst_Timer;
    uint32 EnvelopingWinds_Timer;

    void Reset()
    {
        LightningCloud_Timer = 15000;
        LungBurst_Timer = 7000;
        EnvelopingWinds_Timer = 9000;

        if (pInstance && me->isAlive())
            pInstance->SetData(TYPE_HYDROMANCER_THESPIA, NOT_STARTED);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_DEAD, me);

        if (pInstance)
            pInstance->SetData(TYPE_HYDROMANCER_THESPIA, DONE);
    }

    void KilledUnit(Unit* victim)
    {
        switch(rand()%2)
        {
            case 0: DoScriptText(SAY_SLAY_1, me); break;
            case 1: DoScriptText(SAY_SLAY_2, me); break;
        }
    }

    void EnterCombat(Unit *who)
    {
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_AGGRO_1, me); break;
            case 1: DoScriptText(SAY_AGGRO_2, me); break;
            case 2: DoScriptText(SAY_AGGRO_3, me); break;
        }

        if (pInstance)
            pInstance->SetData(TYPE_HYDROMANCER_THESPIA, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //LightningCloud_Timer
        if (LightningCloud_Timer <= diff)
        {
            //cast twice in Heroic mode
            if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(pTarget, SPELL_LIGHTNING_CLOUD);
            if (HeroicMode)
                if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                    DoCast(pTarget, SPELL_LIGHTNING_CLOUD);
            LightningCloud_Timer = 15000+rand()%10000;
        } else LightningCloud_Timer -=diff;

        //LungBurst_Timer
        if (LungBurst_Timer <= diff)
        {
            if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(pTarget, SPELL_LUNG_BURST);
            LungBurst_Timer = 7000+rand()%5000;
        } else LungBurst_Timer -=diff;

        //EnvelopingWinds_Timer
        if (EnvelopingWinds_Timer <= diff)
        {
            //cast twice in Heroic mode
            if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                DoCast(pTarget, SPELL_ENVELOPING_WINDS);
            if (HeroicMode)
                if (Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM,0))
                    DoCast(pTarget, SPELL_ENVELOPING_WINDS);
            EnvelopingWinds_Timer = 10000+rand()%5000;
        } else EnvelopingWinds_Timer -=diff;

        DoMeleeAttackIfReady();
    }
};

#define SPELL_WATER_BOLT_VOLLEY     34449
#define H_SPELL_WATER_BOLT_VOLLEY   37924

struct mob_coilfang_waterelementalAI : public ScriptedAI
{
    mob_coilfang_waterelementalAI(Creature *c) : ScriptedAI(c) {}

    bool HeroicMode;
    uint32 WaterBoltVolley_Timer;

    void Reset()
    {
        HeroicMode = me->GetMap()->IsHeroic();
        WaterBoltVolley_Timer = 3000+rand()%3000;
    }

    void EnterCombat(Unit *who) { }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (WaterBoltVolley_Timer <= diff)
        {
            DoCast(me, HeroicMode ? H_SPELL_WATER_BOLT_VOLLEY : SPELL_WATER_BOLT_VOLLEY);
            WaterBoltVolley_Timer = 7000+rand()%5000;
        } else WaterBoltVolley_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_thespiaAI(Creature* pCreature)
{
    return new boss_thespiaAI (pCreature);
}

CreatureAI* GetAI_mob_coilfang_waterelementalAI(Creature* pCreature)
{
    return new mob_coilfang_waterelementalAI (pCreature);
}

void AddSC_boss_hydromancer_thespia()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_hydromancer_thespia";
    newscript->GetAI = &GetAI_boss_thespiaAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_coilfang_waterelemental";
    newscript->GetAI = &GetAI_mob_coilfang_waterelementalAI;
    newscript->RegisterSelf();
}

