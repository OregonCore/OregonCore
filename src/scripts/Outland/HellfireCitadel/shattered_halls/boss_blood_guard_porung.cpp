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
SDName: Boss_Blood_Guard_Porung
SD%Complete: 99
SDComment:
SDCategory: Hellfire Citadel, Shattered Halls
EndScriptData */

/* ContentData
boss_blood_guard_porung
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "shattered_halls.h"

#define NPC_SCOUT  17693
#define NPC_PORUNG 20923
#define NPC_ZEALOT 17462

#define SPELL_CLEAVE  15496

#define FORM_1	-1910114
#define FORM_2	-1910115
#define FORM_3	-1910116
#define FORM_4	-1910117

#define SPELL_EXPLO_ARROW 40872
#define SPELL_MULTI_SHOOT 30990
#define SPELL_HAMSTRING 30989

#define FIRE 30542
#define ARROW_DUMMY_TARGET 31995

#define WP_ZAELOT 300300303

struct npc_shattered_zealotAI : public ScriptedAI
{
	npc_shattered_zealotAI(Creature* c) : ScriptedAI(c)
	{
		pInstance = (ScriptedInstance*)c->GetInstanceData();
		HeroicMode = me->GetMap()->IsHeroic();
	}

	ScriptedInstance* pInstance;
	bool Heroic;

	void Reset()
	{
		Blaze_Timer = 30000;
		hamstring_timer = 4000;
	}

	uint32 Blaze_Timer;
	uint32 hamstring_timer;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (Blaze_Timer <= diff)
		{
			if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0))
			{
				float x, y, z;
				target->GetPosition(x, y, z);
				Creature* summon = me->SummonCreature(ARROW_DUMMY_TARGET, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN, 6000);
			}
			Blaze_Timer = 10000;
		}
		else
			Blaze_Timer -= diff;	

		if (hamstring_timer <= diff)
		{
			DoCastVictim(SPELL_HAMSTRING);
			hamstring_timer = 15000;
		}
		else hamstring_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_shattered_zealot(Creature* pCreature)
{
	return new npc_shattered_zealotAI(pCreature);
}

struct npc_shattered_helperAI : public ScriptedAI
{
	npc_shattered_helperAI(Creature* c) : ScriptedAI(c)
	{
		pInstance = (ScriptedInstance*)c->GetInstanceData();
		HeroicMode = me->GetMap()->IsHeroic();
	}

	ScriptedInstance* pInstance;
	bool Heroic;

	void Reset()
	{

	}

	void HelpShoot()
	{
		if (Unit* dummy = me->FindNearestCreature(ARROW_DUMMY_TARGET, 75.0f, true))
		{
			DoCast(dummy, SPELL_EXPLO_ARROW);
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;
	}
};

CreatureAI* GetAI_npc_shattered_helper(Creature* pCreature)
{
	return new npc_shattered_helperAI(pCreature);
}

struct npc_shattered_shooterAI : public ScriptedAI
{
	npc_shattered_shooterAI(Creature* c) : ScriptedAI(c)		
	{
		pInstance = (ScriptedInstance*)c->GetInstanceData();
		HeroicMode = me->GetMap()->IsHeroic();
	}

	uint32 porung;

	uint32 ready_timer;
	uint32 multishoot_timer;

	ScriptedInstance* pInstance;
	bool Heroic;

	void Reset()
	{
		ready_timer = 10000;
		multishoot_timer = 4000;
	}

	void EnterCombat(Unit* /*who*/) {}
	void MoveInLineOfSight(Unit* /*who*/) {}

	void Shoot(const uint32 /*diff*/)
	{
		if (Unit* dummy = me->FindNearestCreature(ARROW_DUMMY_TARGET, 200.0f, true))
		{
			DoCast(dummy, SPELL_EXPLO_ARROW);

		if (Creature* helper = me->FindNearestCreature(31994, 200.0f, true))
			CAST_AI(npc_shattered_helperAI, helper->AI())->HelpShoot();
		}	
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (ready_timer <= diff)
			{
				Shoot(diff);
			}
			else ready_timer -= diff;
		}

		if (UpdateVictim())
		{
			if (multishoot_timer <= diff)
			{
				Unit* victim = SelectUnit(SELECT_TARGET_RANDOM, 0);
				DoCast(victim, SPELL_MULTI_SHOOT);
				multishoot_timer = 9000;
			}
			else multishoot_timer -= diff;
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_shattered_shooter(Creature* pCreature)
{
	return new npc_shattered_shooterAI(pCreature);
}

struct boss_blood_guard_porungAI : public ScriptedAI
{
    boss_blood_guard_porungAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();
    }

    ScriptedInstance* pInstance;
    bool Heroic;

    uint32 Cleave_Timer;
	uint32 form2_timer;
	uint32 form3_timer;
	uint32 form4_timer;
	uint32 reinforcement_timer;

	bool form1;
	bool form2;
	bool form3;
	bool form4;

    void Reset()
    {
        Cleave_Timer = 9000;
		form2_timer = 5000;
		form3_timer = 7000;
		form4_timer = 9000;
		reinforcement_timer = 20000;

		form1 = false;
		form2 = false;
		form3 = false;
		form4 = false;

        if (pInstance)
            pInstance->SetData(DATA_PORUNG, NOT_STARTED);
    }

    void EnterCombat(Unit* /*who*/)
    {
        if (pInstance)
            pInstance->SetData(DATA_PORUNG, IN_PROGRESS);
    }

    void JustDied(Unit* /*Killer*/)
    {
        if (pInstance)
            pInstance->SetData(DATA_PORUNG, DONE);
    }

	void JustSummoned(Creature* summoned)
	{
		summoned->GetMotionMaster()->MovePath(WP_ZAELOT, false);

		if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
			summoned->AI()->AttackStart(pTarget);
	}

    void UpdateAI(const uint32 diff)
    {
		if (!UpdateVictim())
		{
			if (Creature* scout = me->FindNearestCreature(NPC_SCOUT, 17.0f, true))
			{
				Reset();
				DoScriptText(FORM_1, me);
				scout->DisappearAndDie();
				form1 = true;
			}

			if (!form2 && form1 == true && form2_timer <= diff)
			{
				DoScriptText(FORM_2, me);
				form2 = true;
			}
			else form2_timer -= diff;

			if (!form3 && form2 == true && form3_timer <= diff)
			{
				DoScriptText(FORM_3, me);
				form3 = true;
			}
			else form3_timer -= diff;

			if (!form4 && form3 == true && form4_timer <= diff)
			{
				DoScriptText(FORM_4, me);
				form4 = true;

				if (Creature* ranger = me->FindNearestCreature(17427, 20.0f, true))
					CAST_AI(npc_shattered_shooterAI, ranger->AI())->Shoot(diff);
			}
			form4_timer -= diff;

			if (form4 == true && reinforcement_timer <= diff)
			{
				me->SummonCreature(NPC_ZEALOT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
				me->SummonCreature(NPC_ZEALOT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);

				reinforcement_timer = 20000;
			}
			else reinforcement_timer -= diff;
		}
        
		if (UpdateVictim())
		{
			if (Cleave_Timer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				Cleave_Timer = 10000 + rand() % 5000;
			}
			else Cleave_Timer -= diff;
		}
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_blood_guard_porungAI(Creature* pCreature)
{
    return GetInstanceAI<boss_blood_guard_porungAI>(pCreature);
}

struct npc_blood_guardAI : public ScriptedAI
{
	npc_blood_guardAI(Creature* c) : ScriptedAI(c)
	{
		pInstance = (ScriptedInstance*)c->GetInstanceData();
		HeroicMode = me->GetMap()->IsHeroic();
	}

	ScriptedInstance* pInstance;
	bool Heroic;

	uint32 form2_timer;
	uint32 form3_timer;
	uint32 form4_timer;
	uint32 reinforcement_timer;

	bool form1;
	bool form2;
	bool form3;
	bool form4;

	void Reset()
	{
		form2_timer = 5000;
		form3_timer = 7000;
		form4_timer = 9000;
		reinforcement_timer = 20000;

		form1 = false;
		form2 = false;
		form3 = false;
		form4 = false;
	}

	void EnterCombat(Unit* /*who*/) { }

	void JustDied(Unit* /*Killer*/) { }
	
	void JustSummoned(Creature* summoned)
	{
		summoned->GetMotionMaster()->MovePath(WP_ZAELOT, false);

		if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
			summoned->AI()->AttackStart(pTarget);
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (Creature* scout = me->FindNearestCreature(NPC_SCOUT, 17.0f, true))
			{
				Reset();
				DoScriptText(FORM_1, me);
				scout->DisappearAndDie();
				form1 = true;
			}

			if (!form2 && form1 == true && form2_timer <= diff)
			{
				DoScriptText(FORM_2, me);
				form2 = true;
			}
			else form2_timer -= diff;

			if (!form3 && form2 == true && form3_timer <= diff)
			{
				DoScriptText(FORM_3, me);
				form3 = true;
			}
			else form3_timer -= diff;

			if (!form4 && form3 == true && form4_timer <= diff)
			{
				DoScriptText(FORM_4, me);
				form4 = true;

				if (Creature* ranger = me->FindNearestCreature(17427, 20.0f, true))
					CAST_AI(npc_shattered_shooterAI, ranger->AI())->Shoot(diff);
			}
			form4_timer -= diff;

			if (form4 == true && reinforcement_timer <= diff)
			{
				me->SummonCreature(NPC_ZEALOT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
				me->SummonCreature(NPC_ZEALOT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);

				reinforcement_timer = 20000;
			}
			else reinforcement_timer -= diff;
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_blood_guard(Creature* pCreature)
{
	return new npc_blood_guardAI(pCreature);
}

struct npc_shattered_targetAI : public ScriptedAI
{
	npc_shattered_targetAI(Creature* c) : ScriptedAI(c)
	{
		pInstance = (ScriptedInstance*)c->GetInstanceData();
		HeroicMode = me->GetMap()->IsHeroic();
	}

	ScriptedInstance* pInstance;
	bool Heroic;

	void Reset() { }

	void SpellHit(Unit* /*who*/, const SpellEntry* spell)
	{
		if (spell->Id == SPELL_EXPLO_ARROW)
		{
			DoCast(me, FIRE);
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;
	}
};

CreatureAI* GetAI_npc_shattered_target(Creature* pCreature)
{
	return new npc_shattered_targetAI(pCreature);
}

void AddSC_boss_blood_guard_porung()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_blood_guard_porung";
    newscript->GetAI = &GetAI_boss_blood_guard_porungAI;
    newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_shattered_shooter";
	newscript->GetAI = &GetAI_npc_shattered_shooter;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_shattered_target";
	newscript->GetAI = &GetAI_npc_shattered_target;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_shattered_zealot";
	newscript->GetAI = &GetAI_npc_shattered_zealot;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_shattered_helper";
	newscript->GetAI = &GetAI_npc_shattered_helper;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_blood_guard";
	newscript->GetAI = &GetAI_npc_blood_guard;
	newscript->RegisterSelf();
}
