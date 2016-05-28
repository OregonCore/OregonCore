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
SDName: Boss_High_King_Maulgar
SD%Complete: 90
SDComment: Correct timers, after whirlwind melee attack bug
SDCategory: Gruul's Lair
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "gruuls_lair.h"

#define SAY_AGGRO               -1565000
#define SAY_ENRAGE              -1565001
#define SAY_OGRE_DEATH1         -1565002
#define SAY_OGRE_DEATH2         -1565003
#define SAY_OGRE_DEATH3         -1565004
#define SAY_OGRE_DEATH4         -1565005
#define SAY_SLAY1               -1565006
#define SAY_SLAY2               -1565007
#define SAY_SLAY3               -1565008
#define SAY_DEATH               -1565009

// High King Maulgar
#define SPELL_ARCING_SMASH      39144
#define SPELL_MIGHTY_BLOW       33230
#define SPELL_WHIRLWIND         33238
#define SPELL_BERSERKER_C       26561
#define SPELL_ROAR              16508
#define SPELL_FLURRY            33232
#define SPELL_DUAL_WIELD        29651 //used in phase

// Olm the Summoner
#define SPELL_DARK_DECAY        33129
#define SPELL_DEATH_COIL        33130
#define SPELL_SUMMON_WFH        33131

//Kiggler the Craed
#define SPELL_GREATER_POLYMORPH     33173
#define SPELL_LIGHTNING_BOLT        42024
#define SPELL_ARCANE_SHOCK          33175
#define SPELL_ARCANE_EXPLOSION      33237

//Blindeye the Seer
#define SPELL_GREATER_PW_SHIELD         33147
#define SPELL_HEAL                      33144
#define SPELL_PRAYER_OH                 33152

//Krosh Firehand
#define SPELL_GREATER_FIREBALL  33051
#define SPELL_SPELLSHIELD       33054
#define SPELL_BLAST_WAVE        33061

bool CheckAllBossDied(ScriptedInstance* pInstance, Creature* me)
{
    if (!pInstance || !me)
        return false;

    uint64 MaulgarGUID = 0;
    uint64 KigglerGUID = 0;
    uint64 BlindeyeGUID = 0;
    uint64 OlmGUID = 0;
    uint64 KroshGUID = 0;

    Creature* Maulgar = NULL;
    Creature* Kiggler = NULL;
    Creature* Blindeye = NULL;
    Creature* Olm = NULL;
    Creature* Krosh = NULL;

    MaulgarGUID = pInstance->GetData64(DATA_MAULGAR);
    KigglerGUID = pInstance->GetData64(DATA_KIGGLERTHECRAZED);
    BlindeyeGUID = pInstance->GetData64(DATA_BLINDEYETHESEER);
    OlmGUID = pInstance->GetData64(DATA_OLMTHESUMMONER);
    KroshGUID = pInstance->GetData64(DATA_KROSHFIREHAND);

    Maulgar = (Unit::GetCreature((*me), MaulgarGUID));
    Kiggler = (Unit::GetCreature((*me), KigglerGUID));
    Blindeye = (Unit::GetCreature((*me), BlindeyeGUID));
    Olm = (Unit::GetCreature((*me), OlmGUID));
    Krosh = (Unit::GetCreature((*me), KroshGUID));

    if (!Maulgar || !Kiggler || !Blindeye || !Olm || !Krosh)
        return false;

    if (!Maulgar->IsAlive() && !Kiggler->IsAlive() && !Blindeye->IsAlive() && !Olm->IsAlive() && !Krosh->IsAlive())
        return true;

    return false;
}

//High King Maulgar AI
struct boss_high_king_maulgarAI : public ScriptedAI
{
    boss_high_king_maulgarAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        for (uint8 i = 0; i < 4; ++i)
            Council[i] = 0;
    }

    ScriptedInstance* pInstance;

    uint32 ArcingSmash_Timer;
    uint32 MightyBlow_Timer;
    uint32 Whirlwind_Timer;
    uint32 Charging_Timer;
    uint32 Roar_Timer;

    bool Phase2;

    uint64 Council[4];

    void Reset()
    {
        ArcingSmash_Timer = 10000;
        MightyBlow_Timer = 40000;
        Whirlwind_Timer = 30000;
        Charging_Timer = 0;
        Roar_Timer = 0;

		const CreatureInfo* cinfo = me->GetCreatureTemplate();
		me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->mindmg + ((cinfo->mindmg / 100) * 45)));
		me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg + ((cinfo->maxdmg / 100) * 45)));
		me->UpdateDamagePhysical(BASE_ATTACK);

		if (me->HasAura(SPELL_DUAL_WIELD))
			me->RemoveAurasDueToSpell(SPELL_DUAL_WIELD);

        Phase2 = false;

        Creature* pCreature = NULL;
        for (uint8 i = 0; i < 4; i++)
        {
            if (Council[i])
            {
                pCreature = (Creature*)(Unit::GetUnit((*me), Council[i]));
                if (pCreature && !pCreature->IsAlive())
                {
                    pCreature->Respawn();
                    pCreature->AI()->EnterEvadeMode();
                }
            }
        }

        //reset encounter
        if (pInstance)
            pInstance->SetData(DATA_MAULGAREVENT, NOT_STARTED);
    }

    void KilledUnit()
    {
        switch (rand() % 3)
        {
        case 0:
            DoScriptText(SAY_SLAY1, me);
            break;
        case 1:
            DoScriptText(SAY_SLAY2, me);
            break;
        case 2:
            DoScriptText(SAY_SLAY3, me);
            break;
        }
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DEATH, me);

        if (CheckAllBossDied(pInstance, me))
            pInstance->SetData(DATA_MAULGAREVENT, DONE);
    }

    void AddDeath()
    {
        switch (rand() % 4)
        {
        case 0:
            DoScriptText(SAY_OGRE_DEATH1, me);
            break;
        case 1:
            DoScriptText(SAY_OGRE_DEATH2, me);
            break;
        case 2:
            DoScriptText(SAY_OGRE_DEATH3, me);
            break;
        case 3:
            DoScriptText(SAY_OGRE_DEATH4, me);
            break;
        }
    }


    void EnterCombat(Unit* who)
    {
        StartEvent(who);
    }

    void GetCouncil()
    {
        //get council member's guid to respawn them if needed
        Council[0] = pInstance->GetData64(DATA_KIGGLERTHECRAZED);
        Council[1] = pInstance->GetData64(DATA_BLINDEYETHESEER);
        Council[2] = pInstance->GetData64(DATA_OLMTHESUMMONER);
        Council[3] = pInstance->GetData64(DATA_KROSHFIREHAND);
    }

    void StartEvent(Unit* who)
    {
        if (!pInstance)
            return;

        GetCouncil();

        DoScriptText(SAY_AGGRO, me);

        pInstance->SetData64(DATA_MAULGAREVENT_TANK, who->GetGUID());
        pInstance->SetData(DATA_MAULGAREVENT, IN_PROGRESS);

        DoZoneInCombat();
    }

    void UpdateAI(const uint32 diff)
    {
        //Only if not incombat check if the event is started
        if (!me->IsInCombat() && pInstance && pInstance->GetData(DATA_MAULGAREVENT))
        {
            Unit* pTarget = Unit::GetUnit((*me), pInstance->GetData64(DATA_MAULGAREVENT_TANK));

            if (pTarget)
            {
                AttackStart(pTarget);
                GetCouncil();
            }
        }

        //Return since we have no target
        if (!UpdateVictim())
            return;

        //ArcingSmash_Timer
        if (ArcingSmash_Timer <= diff)
        {
            DoCastVictim( SPELL_ARCING_SMASH);
            ArcingSmash_Timer = 10000;
        }
        else ArcingSmash_Timer -= diff;

        //Whirlwind_Timer
        if (Whirlwind_Timer <= diff)
        {
            DoCastVictim( SPELL_WHIRLWIND);
            Whirlwind_Timer = 55000;
        }
        else Whirlwind_Timer -= diff;

        //MightyBlow_Timer
        if (MightyBlow_Timer <= diff)
        {
            DoCastVictim( SPELL_MIGHTY_BLOW);
            MightyBlow_Timer = 30000 + rand() % 10000;
        }
        else MightyBlow_Timer -= diff;

        //Entering Phase 2
        if (!Phase2 && (me->GetHealth() * 100 / me->GetMaxHealth()) < 50)
        {
            Phase2 = true;
            DoScriptText(SAY_ENRAGE, me);
			DoCast(me, SPELL_FLURRY);

            me->CastSpell(me, SPELL_DUAL_WIELD, true);
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY, 0);
            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + 1, 0);
        }

        if (Phase2)
        {
			const CreatureInfo* cinfo = me->GetCreatureTemplate();
			me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, (cinfo->mindmg + ((cinfo->mindmg / 100) * 1)));
			me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, (cinfo->maxdmg + ((cinfo->maxdmg / 100) * 1)));
			me->UpdateDamagePhysical(BASE_ATTACK);

            //Charging_Timer
            if (Charging_Timer <= diff)
            {
                Unit* pTarget = NULL;
                pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
                if (pTarget)
                {
                    AttackStart(pTarget);
                    DoCast(pTarget, SPELL_BERSERKER_C);
                }
                Charging_Timer = 20000;
            }
            else Charging_Timer -= diff;

            //Intimidating Roar
            if (Roar_Timer <= diff)
            {
                DoCast(me, SPELL_ROAR);
                Roar_Timer = 40000 + (rand() % 10000);
            }
            else Roar_Timer -= diff;
        }

        DoMeleeAttackIfReady();
    }
};

//Olm The Summoner AI
struct boss_olm_the_summonerAI : public ScriptedAI
{
    boss_olm_the_summonerAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    uint32 DarkDecay_Timer;
    uint32 Summon_Timer;
    uint32 DeathCoil_Timer;

    ScriptedInstance* pInstance;

    void Reset()
    {
		DarkDecay_Timer = urand(5000, 8000);
        Summon_Timer = 500;
        DeathCoil_Timer = 7000;

        //reset encounter
        if (pInstance)
            pInstance->SetData(DATA_MAULGAREVENT, NOT_STARTED);
    }

    void EnterCombat(Unit* who)
    {
		DoCast(me, SPELL_SUMMON_WFH);

        if (pInstance)
        {
            pInstance->SetData64(DATA_MAULGAREVENT_TANK, who->GetGUID());
            pInstance->SetData(DATA_MAULGAREVENT, IN_PROGRESS);
        }
    }

    void JustDied(Unit* /*Killer*/)
    {
        if (pInstance)
        {
            Creature* Maulgar = NULL;
            Maulgar = (Creature*)(Unit::GetUnit((*me), pInstance->GetData64(DATA_MAULGAR)));

            if (Maulgar)
                ((boss_high_king_maulgarAI*)Maulgar->AI())->AddDeath();

            if (CheckAllBossDied(pInstance, me))
                pInstance->SetData(DATA_MAULGAREVENT, DONE);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Only if not incombat check if the event is started
        if (!me->IsInCombat() && pInstance && pInstance->GetData(DATA_MAULGAREVENT))
        {
            Unit* pTarget = Unit::GetUnit((*me), pInstance->GetData64(DATA_MAULGAREVENT_TANK));

            if (pTarget)
                AttackStart(pTarget);
        }

		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

        //Return since we have no target
        if (!UpdateVictim())
            return;

        //someone evaded!
        if (pInstance && !pInstance->GetData(DATA_MAULGAREVENT))
        {
            EnterEvadeMode();
            return;
        }

        //DarkDecay_Timer
        if (DarkDecay_Timer <= diff)
        {
            DoCastVictim( SPELL_DARK_DECAY);
			DarkDecay_Timer = urand(5000, 8000);
        }
        else DarkDecay_Timer -= diff;

        //Summon_Timer
        if (Summon_Timer <= diff)
        {
            DoCast(me, SPELL_SUMMON_WFH);
            Summon_Timer = 30000;
        }
        else Summon_Timer -= diff;

        //DeathCoil Timer /need correct timer
        if (DeathCoil_Timer <= diff)
        {
            Unit* pTarget = NULL;
            pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
            if (pTarget)
                DoCast(pTarget, SPELL_DEATH_COIL);
			DeathCoil_Timer = 7000;
        }
        else DeathCoil_Timer -= diff;


        DoMeleeAttackIfReady();
    }
};

//Kiggler The Crazed AI
struct boss_kiggler_the_crazedAI : public ScriptedAI
{
    boss_kiggler_the_crazedAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    uint32 GreaterPolymorph_Timer;
    uint32 LightningBolt_Timer;
    uint32 ArcaneShock_Timer;
    uint32 ArcaneExplosion_Timer;

    ScriptedInstance* pInstance;

    void Reset()
    {	
        GreaterPolymorph_Timer = 5000;
		LightningBolt_Timer = 3000;
        ArcaneShock_Timer = 10000;
        ArcaneExplosion_Timer = 20000;

		me->SetRooted(true);
        //reset encounter
        if (pInstance)
            pInstance->SetData(DATA_MAULGAREVENT, NOT_STARTED);
    }

    void EnterCombat(Unit* who)
    {
        if (pInstance)
        {
            pInstance->SetData64(DATA_MAULGAREVENT_TANK, who->GetGUID());
            pInstance->SetData(DATA_MAULGAREVENT, IN_PROGRESS);
        }
    }

    void JustDied(Unit* /*Killer*/)
    {
        if (pInstance)
        {
            Creature* Maulgar = NULL;
            Maulgar = (Creature*)(Unit::GetUnit((*me), pInstance->GetData64(DATA_MAULGAR)));

            if (Maulgar)
                ((boss_high_king_maulgarAI*)Maulgar->AI())->AddDeath();

            if (CheckAllBossDied(pInstance, me))
                pInstance->SetData(DATA_MAULGAREVENT, DONE);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Only if not incombat check if the event is started
        if (!me->IsInCombat() && pInstance && pInstance->GetData(DATA_MAULGAREVENT))
        {
            Unit* pTarget = Unit::GetUnit((*me), pInstance->GetData64(DATA_MAULGAREVENT_TANK));

            if (pTarget)
                AttackStart(pTarget);
        }

        //Return since we have no target
        if (!UpdateVictim())
            return;

        //someone evaded!
        if (pInstance && !pInstance->GetData(DATA_MAULGAREVENT))
        {
            EnterEvadeMode();
            return;
        }

        //GreaterPolymorph_Timer
        if (GreaterPolymorph_Timer <= diff)
        {
            Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
            if (pTarget)
                DoCast(pTarget, SPELL_GREATER_POLYMORPH);

            GreaterPolymorph_Timer = 10000;
        }
        else GreaterPolymorph_Timer -= diff;

        //LightningBolt_Timer
		if (me->IsWithinMeleeRange(me->getVictim()))
		{
			//Make sure our attack is ready and we arn't currently casting
			if (me->isAttackReady() && !me->IsNonMeleeSpellCast(false))
			{
				me->AttackerStateUpdate(me->getVictim());
				me->resetAttackTimer();
			}
		}
		else
		{			
			if (LightningBolt_Timer <= diff)
			{
				if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0))
				{
					DoCastVictim(SPELL_LIGHTNING_BOLT);					
				}		

				LightningBolt_Timer = 3000;
			}
			else LightningBolt_Timer -= diff;
		}

        //ArcaneShock_Timer
        if (ArcaneShock_Timer <= diff)
        {
            DoCastVictim( SPELL_ARCANE_SHOCK);
            ArcaneShock_Timer = 10000;
        }
        else ArcaneShock_Timer -= diff;

        //ArcaneExplosion_Timer
        if (ArcaneExplosion_Timer <= diff)
        {
            DoCastVictim( SPELL_ARCANE_EXPLOSION);
            ArcaneExplosion_Timer = 20000;
        }
        else ArcaneExplosion_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

//Blindeye The Seer AI
struct boss_blindeye_the_seerAI : public ScriptedAI
{
    boss_blindeye_the_seerAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    uint32 GreaterPowerWordShield_Timer;
    uint32 Heal_Timer;
	uint32 pOhHeal_Timer;

    ScriptedInstance* pInstance;

    void Reset()
    {
		GreaterPowerWordShield_Timer = 5000;
		Heal_Timer = 25000;
		pOhHeal_Timer = 31000;

        //reset encounter
        if (pInstance)
            pInstance->SetData(DATA_MAULGAREVENT, NOT_STARTED);
    }

    void EnterCombat(Unit* who)
    {
        if (pInstance)
        {
            pInstance->SetData64(DATA_MAULGAREVENT_TANK, who->GetGUID());
            pInstance->SetData(DATA_MAULGAREVENT, IN_PROGRESS);
        }
    }

    void JustDied(Unit* /*Killer*/)
    {
        if (pInstance)
        {
            Creature* Maulgar = NULL;
            Maulgar = (Creature*)(Unit::GetUnit((*me), pInstance->GetData64(DATA_MAULGAR)));

            if (Maulgar)
                ((boss_high_king_maulgarAI*)Maulgar->AI())->AddDeath();

            if (CheckAllBossDied(pInstance, me))
                pInstance->SetData(DATA_MAULGAREVENT, DONE);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Only if not incombat check if the event is started
        if (!me->IsInCombat() && pInstance && pInstance->GetData(DATA_MAULGAREVENT))
        {
            Unit* pTarget = Unit::GetUnit((*me), pInstance->GetData64(DATA_MAULGAREVENT_TANK));

            if (pTarget)
                AttackStart(pTarget);
        }

        //Return since we have no target
        if (!UpdateVictim())
            return;

        //someone evaded!
        if (pInstance && !pInstance->GetData(DATA_MAULGAREVENT))
        {
            EnterEvadeMode();
            return;
        }

        //GreaterPowerWordShield_Timer
        if (GreaterPowerWordShield_Timer <= diff)
        {
            DoCast(me, SPELL_GREATER_PW_SHIELD);
            GreaterPowerWordShield_Timer = 25000;
        }
        else GreaterPowerWordShield_Timer -= diff;

        //Heal_Timer
        if (Heal_Timer <= diff)
        {
            DoCast(me, SPELL_HEAL);
            Heal_Timer = 26000;
        }
        else Heal_Timer -= diff;

		if (pOhHeal_Timer <= diff)
		{
			DoCast(me, SPELL_PRAYER_OH);
			pOhHeal_Timer = 31000;
		}
		else pOhHeal_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

//Krosh Firehand AI
struct boss_krosh_firehandAI : public ScriptedAI
{
    boss_krosh_firehandAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    uint32 GreaterFireball_Timer;
    uint32 SpellShield_Timer;
    uint32 BlastWave_Timer;

    ScriptedInstance* pInstance;

    void Reset()
    {
        GreaterFireball_Timer = 1000;
        SpellShield_Timer = 1500;
        BlastWave_Timer = 20000;

		me->SetRooted(true);
        //reset encounter
        if (pInstance)
            pInstance->SetData(DATA_MAULGAREVENT, NOT_STARTED);
    }

    void EnterCombat(Unit* who)
    {	
        if (pInstance)
        {
            pInstance->SetData64(DATA_MAULGAREVENT_TANK, who->GetGUID());
            pInstance->SetData(DATA_MAULGAREVENT, IN_PROGRESS);			
        }
    }

    void JustDied(Unit* /*Killer*/)
    {
        if (pInstance)
        {
            Creature* Maulgar = NULL;
            Maulgar = (Creature*)(Unit::GetUnit((*me), pInstance->GetData64(DATA_MAULGAR)));

            if (Maulgar)
                ((boss_high_king_maulgarAI*)Maulgar->AI())->AddDeath();

            if (CheckAllBossDied(pInstance, me))
                pInstance->SetData(DATA_MAULGAREVENT, DONE);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Only if not incombat check if the event is started
        if (!me->IsInCombat() && pInstance && pInstance->GetData(DATA_MAULGAREVENT))
        {
            Unit* pTarget = Unit::GetUnit((*me), pInstance->GetData64(DATA_MAULGAREVENT_TANK));

            if (pTarget)
                AttackStart(pTarget);
        }

        //Return since we have no target
        if (!UpdateVictim())
            return;

        //someone evaded!
        if (pInstance && !pInstance->GetData(DATA_MAULGAREVENT))
        {
            EnterEvadeMode();
            return;
        }

		me->ApplySpellImmune(0, IMMUNITY_ID, 11719, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 1714, true);

        //SpellShield_Timer
        if (SpellShield_Timer <= diff)
        {
            me->InterruptNonMeleeSpells(false);
            DoCastVictim( SPELL_SPELLSHIELD);
            SpellShield_Timer = 20000;
        }
        else SpellShield_Timer -= diff;

        //GreaterFireball_Timer
        if (GreaterFireball_Timer <= diff && me->GetDistance(me->getVictim()) < 100)
        {
            DoCastVictim( SPELL_GREATER_FIREBALL);
            GreaterFireball_Timer = 3000;
        }
        else GreaterFireball_Timer -= diff;

        //BlastWave_Timer
        if (BlastWave_Timer <= diff)
        {
            Unit* pTarget;
            std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
            std::vector<Unit* > target_list;
            for (std::list<HostileReference*>::iterator itr = t_list.begin(); itr != t_list.end(); ++itr)
            {
                pTarget = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                //15 yard radius minimum
                if (pTarget && pTarget->GetDistance2d(me) < 15)
                    target_list.push_back(pTarget);
                pTarget = NULL;
            }
            if (target_list.size())
                pTarget = *(target_list.begin() + rand() % target_list.size());

            me->InterruptNonMeleeSpells(false);
            DoCast(pTarget, SPELL_BLAST_WAVE);
            BlastWave_Timer = 5000;
        }
        else BlastWave_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_high_king_maulgar(Creature* pCreature)
{
    return new boss_high_king_maulgarAI (pCreature);
}

CreatureAI* GetAI_boss_olm_the_summoner(Creature* pCreature)
{
    return new boss_olm_the_summonerAI (pCreature);
}

CreatureAI* GetAI_boss_kiggler_the_crazed(Creature* pCreature)
{
    return new boss_kiggler_the_crazedAI (pCreature);
}

CreatureAI* GetAI_boss_blindeye_the_seer(Creature* pCreature)
{
    return new boss_blindeye_the_seerAI (pCreature);
}

CreatureAI* GetAI_boss_krosh_firehand(Creature* pCreature)
{
    return new boss_krosh_firehandAI (pCreature);
}

void AddSC_boss_high_king_maulgar()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_high_king_maulgar";
    newscript->GetAI = &GetAI_boss_high_king_maulgar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_kiggler_the_crazed";
    newscript->GetAI = &GetAI_boss_kiggler_the_crazed;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_blindeye_the_seer";
    newscript->GetAI = &GetAI_boss_blindeye_the_seer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_olm_the_summoner";
    newscript->GetAI = &GetAI_boss_olm_the_summoner;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_krosh_firehand";
    newscript->GetAI = &GetAI_boss_krosh_firehand;
    newscript->RegisterSelf();
}

