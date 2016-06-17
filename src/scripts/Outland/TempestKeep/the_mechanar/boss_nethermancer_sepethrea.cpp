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
SDName: Boss_Nethermancer_Sepethrea
SD%Complete: 90
SDComment: Need adjustments to initial summons
SDCategory: Tempest Keep, The Mechanar
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "mechanar.h"

#define SAY_AGGRO                       -1554013
#define SAY_SUMMON                      -1554014
#define SAY_DRAGONS_BREATH_1            -1554015
#define SAY_DRAGONS_BREATH_2            -1554016
#define SAY_SLAY1                       -1554017
#define SAY_SLAY2                       -1554018
#define SAY_DEATH                       -1554019

#define SPELL_SUMMON_RAGIN_FLAMES       35275
#define H_SPELL_SUMMON_RAGIN_FLAMES     39084

#define SPELL_FROST_ATTACK              35263
#define SPELL_ARCANE_BLAST              35314
#define SPELL_DRAGONS_BREATH            35250
#define SPELL_KNOCKBACK                 37317
#define SPELL_SOLARBURN                 35267

struct boss_nethermancer_sepethreaAI : public ScriptedAI
{
    boss_nethermancer_sepethreaAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();
    }

    ScriptedInstance* pInstance;

    bool HeroicMode;

    uint32 frost_attack_Timer;
    uint32 arcane_blast_Timer;
    uint32 dragons_breath_Timer;
    uint32 knockback_Timer;
    uint32 solarburn_Timer;

    void Reset()
    {
        frost_attack_Timer = 7000 + rand() % 3000;
        arcane_blast_Timer = 12000 + rand() % 6000;
        dragons_breath_Timer = 18000 + rand() % 4000;
        knockback_Timer = 22000 + rand() % 6000;
        solarburn_Timer = 30000;

        if (pInstance)
            pInstance->SetData(DATA_NETHERMANCER_EVENT, NOT_STARTED);
    }

    void EnterCombat(Unit* who)
    {
        if (pInstance)
            pInstance->SetData(DATA_NETHERMANCER_EVENT, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, me);
        DoCast(who, HeroicMode ? H_SPELL_SUMMON_RAGIN_FLAMES : SPELL_SUMMON_RAGIN_FLAMES);
        DoScriptText(SAY_SUMMON, me);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        switch (rand() % 2)
        {
        case 0:
            DoScriptText(SAY_SLAY1, me);
            break;
        case 1:
            DoScriptText(SAY_SLAY2, me);
            break;
        }
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance)
            pInstance->SetData(DATA_NETHERMANCER_EVENT, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!UpdateVictim())
            return;

        //Frost Attack
        if (frost_attack_Timer <= diff)
        {
            DoCastVictim(SPELL_FROST_ATTACK);
            frost_attack_Timer = 7000 + rand() % 30000;
        }
        else frost_attack_Timer -= diff;

        //Arcane Blast
        if (arcane_blast_Timer <= diff)
        {
            DoCastVictim( SPELL_ARCANE_BLAST);
            arcane_blast_Timer = 15000;
        }
        else arcane_blast_Timer -= diff;

        //Dragons Breath
        if (dragons_breath_Timer <= diff)
        {
            DoCastVictim(SPELL_DRAGONS_BREATH);
            {
                if (rand() % 2)
                    return;

                switch (rand() % 2)
                {
                case 0:
                    DoScriptText(SAY_DRAGONS_BREATH_1, me);
                    break;
                case 1:
                    DoScriptText(SAY_DRAGONS_BREATH_2, me);
                    break;
                }
            }
            dragons_breath_Timer = 12000 + rand() % 10000;
        }
        else dragons_breath_Timer -= diff;

        //Knockback
        if (knockback_Timer <= diff)
        {
            DoCastVictim(SPELL_KNOCKBACK);
            knockback_Timer = 15000 + rand() % 10000;
        }
        else knockback_Timer -= diff;

        //Solarburn
        if (solarburn_Timer <= diff)
        {
            DoCastVictim(SPELL_SOLARBURN);
            solarburn_Timer = 30000;
        }
        else solarburn_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_nethermancer_sepethrea(Creature* pCreature)
{
    return GetInstanceAI<boss_nethermancer_sepethreaAI>(pCreature);
}

#define SPELL_INFERNO                   35268
#define H_SPELL_INFERNO                 39346
#define SPELL_FIRE_TAIL                 35278

struct mob_ragin_flamesAI : public ScriptedAI
{
    mob_ragin_flamesAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();
    }

    ScriptedInstance* pInstance;

    bool HeroicMode;

    uint32 inferno_Timer;
    uint32 flame_timer;
    uint32 Check_Timer;

    bool onlyonce;

    void Reset()
    {
        inferno_Timer = 10000;
        flame_timer = 500;
        Check_Timer = 2000;
        onlyonce = false;
        me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);
        me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, true);
        me->SetSpeed(MOVE_RUN, HeroicMode ? 0.7f : 0.5f);
    }

    void EnterCombat(Unit* /*who*/)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        //Check_Timer
        if (Check_Timer <= diff)
        {
            if (pInstance)
            {
                if (pInstance->GetData(DATA_NETHERMANCER_EVENT) != IN_PROGRESS)
                {
                    //remove
                    me->setDeathState(JUST_DIED);
                    me->RemoveCorpse();
                }
            }
            Check_Timer = 1000;
        }
        else Check_Timer -= diff;

        if (!UpdateVictim())
            return;

        if (!onlyonce)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                me->GetMotionMaster()->MoveChase(pTarget);
            onlyonce = true;
        }

        if (inferno_Timer <= diff)
        {
            DoCastVictim(HeroicMode ? H_SPELL_INFERNO : SPELL_INFERNO);
            me->TauntApply(me->GetVictim());
            inferno_Timer = 10000;
        }
        else inferno_Timer -= diff;

        if (flame_timer <= diff)
        {
            DoCast(me, SPELL_FIRE_TAIL);
            flame_timer = 500;
        }
        else flame_timer -= diff;

        DoMeleeAttackIfReady();
    }

};
CreatureAI* GetAI_mob_ragin_flames(Creature* pCreature)
{
    return GetInstanceAI<mob_ragin_flamesAI>(pCreature);
}
void AddSC_boss_nethermancer_sepethrea()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_nethermancer_sepethrea";
    newscript->GetAI = &GetAI_boss_nethermancer_sepethrea;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ragin_flames";
    newscript->GetAI = &GetAI_mob_ragin_flames;
    newscript->RegisterSelf();
}

