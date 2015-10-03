#include "ScriptedCreature.h"
#include "ScriptMgr.h"

#define SAY_NEXT   -1910118
#define SAY_NEXT1  -1910119
#define SAY_NEXT2  -1910120

// HONOR HOLD DEFENDER CORPSE //

struct npc_honorfallendefenderAI : public ScriptedAI
{
	npc_honorfallendefenderAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
		pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		HeroicMode = me->GetMap()->IsHeroic();
	}

	ScriptedInstance* pInstance;
	bool HeroicMode;

	void Reset()
	{
		me->SetStandState(UNIT_STAND_STATE_DEAD);
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
	}

	void UpdateAI(const uint32 uiDiff) { }
};

CreatureAI* GetAI_npc_honorfallendefender(Creature* creature)
{
	return new npc_honorfallendefenderAI(creature);
}

// HONOR HOLD INFANTRY CORPSE //

struct npc_honorfalleninfantryAI : public ScriptedAI
{
	npc_honorfalleninfantryAI(Creature* pCreature) : ScriptedAI(pCreature) 
	{
		pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		HeroicMode = me->GetMap()->IsHeroic();
	}

	ScriptedInstance* pInstance;
	bool HeroicMode;

	void Reset()
	{
		me->SetStandState(UNIT_STAND_STATE_DEAD);
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
	}

	void UpdateAI(const uint32 uiDiff) { }
};

CreatureAI* GetAI_npc_honorfalleninfantry(Creature* creature)
{
	return new npc_honorfalleninfantryAI(creature);
}

// Shattered Halls - Legionnaire //

enum Legionnaire
{
	EMOTE_ENRAGED = -1910121,
	SAY_FORMATION_1 = -1910112,
	/*SAY_FORMATION_2 =
	SAY_FORMATION_3 =
	SAY_FORMATION_4 =
	SAY_FORMATION_5 =
	SAY_FORMATION_6 =
	SAY_FORMATION_7 =
	SAY_FORMATION_8 =
	SAY_FORMATION_9 =
	SAY_FORMATION_10 =
	SAY_FORMATION_11 =
	SAY_FORMATION_12 =
	SAY_FORMATION_13 =
	SAY_FORMATION_14 =
	SAY_FORMATION_15 = ,*/

	SPELL_ENRAGE = 30485,
	SPELL_PUMMEL = 15615,
	SPELL_AURA_OF_DISCIPLINE = 30472,

	NPC_TRIGGER_LEGION_INTRO = 32996
};

struct npc_shattered_legionnaireAI : public ScriptedAI
{
	npc_shattered_legionnaireAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		HeroicMode = me->GetMap()->IsHeroic();
	}

	ScriptedInstance* pInstance;
	bool HeroicMode;

	void Reset()
	{
		pummel_timer = 8000;
		aura_timer = 5000;
		randomsay_timer = 25000;

		enrage = false;
		yelled = false;
		yelled1 = false;
		yelled2 = false;
		yelled3 = false;
	}

	uint32 pummel_timer;
	uint32 aura_timer;
	uint32 randomsay_timer;

	bool enrage;
	bool yelled;
	bool yelled1;
	bool yelled2;
	bool yelled3;

	void EnterCombat(Unit* /*who*/)
	{
		switch (rand() % 9)
		{
		case 0:
			DoPlaySoundToSet(me, 10237);
			break;
		case 1:
			DoPlaySoundToSet(me, 10238);
			break;
		case 2:
			DoPlaySoundToSet(me, 10240);
			break;
		case 3:
			DoPlaySoundToSet(me, 10241);
			break;
		case 4:
			DoPlaySoundToSet(me, 10242);
			break;
		case 5:
			DoPlaySoundToSet(me, 10243);
			break;
		case 6:
			DoPlaySoundToSet(me, 10244);
			break;
		case 7:
			DoPlaySoundToSet(me, 10245);
			break;
		case 8:
			DoPlaySoundToSet(me, 10246);
			break;
		}
	}

	void JustSummoned(Creature* summoned)
	{
		if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
			summoned->AI()->AttackStart(pTarget);
	}

	void SummonReplacement()
	{
		me->SummonCreature(16699, me->GetPositionX() - 10.0f, me->GetPositionY() - 10.0f, me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 120000);
	}

	void SummonReplacement1()
	{
		me->SummonCreature(17420, me->GetPositionX() - 10.0f, me->GetPositionY() - 10.0f, me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 120000);
	}

	void SummonReplacement2()
	{
		me->SummonCreature(17694, me->GetPositionX() - 10.0f, me->GetPositionY() - 10.0f, me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 120000);
	}

	void SummonReplacement3()
	{
		me->SummonCreature(16594, me->GetPositionX() - 10.0f, me->GetPositionY() - 10.0f, me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 120000);
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (!yelled)
			{
				if (me->FindNearestCreature(NPC_TRIGGER_LEGION_INTRO, 50.0f, true))
				{
					DoScriptText(SAY_FORMATION_1, me);
					yelled = true;
				}
			}
			/*
			if (me->FindNearestCreature(TRIGGER_LEGION_1, 20.0f, true))
			{
				DoScriptText(SAY_FORMATION_2, me);
				yelled1 = true;
			}
			*/
		}

		if (UpdateVictim())
		{
			if (randomsay_timer <= diff)
			{
				switch (rand() % 9)
				{
				case 0:
					DoPlaySoundToSet(me, 10237);
					break;
				case 1:
					DoPlaySoundToSet(me, 10238);
					break;
				case 2:
					DoPlaySoundToSet(me, 10240);
					break;
				case 3:
					DoPlaySoundToSet(me, 10241);
					break;
				case 4:
					DoPlaySoundToSet(me, 10242);
					break;
				case 5:
					DoPlaySoundToSet(me, 10243);
					break;
				case 6:
					DoPlaySoundToSet(me, 10244);
					break;
				case 7:
					DoPlaySoundToSet(me, 10245);
					break;
				case 8:
					DoPlaySoundToSet(me, 10246);
					break;
				}
				randomsay_timer = 25000;
			}
			else randomsay_timer -= diff;

			if (pummel_timer <= diff)
			{
				Unit* victim = SelectUnit(SELECT_TARGET_RANDOM, 1);
				pummel_timer = 10000;
			}
			else pummel_timer -= diff;

			if (aura_timer <= diff)
			{
				DoCast(SPELL_AURA_OF_DISCIPLINE);
				aura_timer = 300000;
			}
			else aura_timer -= diff;

			if (!enrage && me->FindNearestCreature(16699, 12.0f, false))
			{
				DoCast(me, SPELL_ENRAGE);
				DoScriptText(SAY_NEXT, me);
				DoScriptText(EMOTE_ENRAGED, me);
				SummonReplacement();
				enrage = true;
			}

			if (!enrage && me->FindNearestCreature(17420, 12.0f, false))
			{
				DoCast(me, SPELL_ENRAGE);
				DoScriptText(SAY_NEXT1, me);
				DoScriptText(EMOTE_ENRAGED, me);
				SummonReplacement1();
				enrage = true;
			}

			if (!enrage && me->FindNearestCreature(17694, 12.0f, false))
			{
				DoCast(me, SPELL_ENRAGE);
				DoScriptText(SAY_NEXT, me);
				DoScriptText(EMOTE_ENRAGED, me);
				SummonReplacement2();
				enrage = true;
			}

			if (!enrage && me->FindNearestCreature(16594, 12.0f, false))
			{
				DoCast(me, SPELL_ENRAGE);
				DoScriptText(SAY_NEXT2, me);
				DoScriptText(EMOTE_ENRAGED, me);
				SummonReplacement3();
				enrage = true;
			}		
	
			DoMeleeAttackIfReady();
		}
	}
};

CreatureAI* GetAI_npc_shattered_legionnaire(Creature* creature)
{
	return new npc_shattered_legionnaireAI(creature);
}

//* SHATTERED HAND SCOUT - EVENT AI *// 

enum Scout
{
	RUN_PATH = 300300302,

	SAY_SPOTTED = -1910113
};

struct npc_shatteredscoutAI : public ScriptedAI
{
	npc_shatteredscoutAI(Creature* pCreature) : ScriptedAI(pCreature)
	{
		pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
		HeroicMode = me->GetMap()->IsHeroic();
	}

	void Reset()
	{
		spotted = false;
	}

	ScriptedInstance* pInstance;
	bool HeroicMode;

	bool spotted;

	void MoveInLineOfSight(Unit* who)
	{
		if (!me->getVictim() && who->isTargetableForAttack() && (me->IsHostileTo(who)) && who->isInAccessiblePlaceFor(me))
		{
			if (!spotted && me->IsWithinDistInMap(who, 65))
			{
				DoScriptText(SAY_SPOTTED, me);
				me->GetMotionMaster()->MovePath(RUN_PATH, false);
				spotted = true;					
			}

			if (!me->canFly() && me->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
				return;

			float attackRadius = me->GetAttackDistance(who);
			if (me->IsWithinDistInMap(who, attackRadius) && me->IsWithinLOSInMap(who))
			{
				//who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
				me->InterruptNonMeleeSpells(false);
				AttackStart(who);
			}
		}
	}

	void UpdateAI(const uint32 uiDiff) { }
};

CreatureAI* GetAI_npc_shatteredscout(Creature* creature)
{
	return new npc_shatteredscoutAI(creature);
}

void AddSC_shattered_halls()
{
	Script* newscript;

	newscript = new Script;
	newscript->Name = "npc_honorfallendefender";
	newscript->GetAI = &GetAI_npc_honorfallendefender;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_honorfalleninfantry";
	newscript->GetAI = &GetAI_npc_honorfalleninfantry;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_shattered_legionnaire";
	newscript->GetAI = &GetAI_npc_shattered_legionnaire;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_shatteredscout";
	newscript->GetAI = &GetAI_npc_shatteredscout;
	newscript->RegisterSelf();
}