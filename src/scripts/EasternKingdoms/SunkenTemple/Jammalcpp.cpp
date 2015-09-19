#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Player.h"
#include "Spell.h"

enum Spells
{
	// Jammal
	SPELL_TOTEM = 8376,
	SPELL_FLAMESTRIKE = 12468,
	SPELL_HEALING_WAVE = 12492,
	SPELL_TRANSFORM = 12480
};

#define NPC_WEAVER				 5720
#define NPC_DREAMSCYTHE			 5721

#define NPC_DEATHWALKER			 5271 
#define NPC_MUMMIFIED			 5263 

#define SAY_AGGRO               -1910073
#define SAY_HP                  -1910074
#define SAY_TRANSFORM			-1910075
#define SAY_KILL				-1910076

struct boss_JammalAI : public ScriptedAI
{
	boss_JammalAI(Creature* creature) : ScriptedAI(creature)
	{
		Initialize();
	}

	void Initialize()
	{
		healingwave_timer = 12000;
		transform_timer = 15000;
		totem_Timer = 30000;
		flamestrike_timer = 25000;

		hp = false;
	}

	ScriptedInstance* instance;

	uint32 transform_timer;
	uint32 totem_Timer;
	uint32 healingwave_timer;
	uint32 flamestrike_timer;

	bool hp;

	void Reset() override
	{
		Initialize();
	}

	void EnterCombat(Unit* victim) override
	{
		me->InterruptNonMeleeSpells(false);
		
		std::list<Creature*> creatureList;
		GetCreatureListWithEntryInGrid(creatureList, me, NPC_DEATHWALKER, 60.0f);
		GetCreatureListWithEntryInGrid(creatureList, me, NPC_MUMMIFIED, 60.0f);
		for (std::list<Creature*>::iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
		{
			if (Creature* creature = *itr)
			{
				creature->AI()->AttackStart(me->getVictim());
			}
		}

		DoScriptText(SAY_AGGRO, me);
		DoCast(SPELL_TOTEM);
	}

	void DamageTaken(Unit* /*attacker*/, uint32& damage) override { }

	void JustDied(Unit* /*killer*/) override 
	{
		me->SummonCreature(NPC_WEAVER, -467.098, 85.294, -94.731, 3.95, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 90000);
		me->SummonCreature(NPC_DREAMSCYTHE, -472.019, 105.822, -94.629, 0.03, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 90000);
	}

	void KilledUnit(Unit* /*victim*/)
	{
		DoScriptText(SAY_KILL, me);
	}

	void UpdateAI(const uint32 diff) override
	{
		if (!UpdateVictim())
			return;

		if (totem_Timer <= diff)
		{
			DoCast(SPELL_TOTEM);
			totem_Timer = 30000;
		}
		else
			totem_Timer -= diff;

		if (transform_timer <= diff)
		{
			if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
				DoCast(target, SPELL_TRANSFORM);
			transform_timer = 15000;

			DoScriptText(SAY_TRANSFORM, me);
		}
		else
			transform_timer -= diff;

		if (HealthBelowPct(60) && healingwave_timer <= diff)
		{
			DoCast(me, SPELL_HEALING_WAVE);
			healingwave_timer = 12000;
		}
		else
			healingwave_timer -= diff;

		if (flamestrike_timer <= diff)
		{
			if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
			DoCast(target, SPELL_FLAMESTRIKE);
			flamestrike_timer = 25000;
		}
		else
			flamestrike_timer -= diff;

		if (!hp)
		{
			if (HealthBelowPct(10))
			{
				DoScriptText(SAY_HP, me);
				hp = true;
			}
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_Jammal(Creature* pCreature)
{
	return new boss_JammalAI(pCreature);
}

void AddSC_boss_Jammal()
{
	Script* newscript;
	newscript = new Script;
	newscript->Name = "boss_Jammal";
	newscript->GetAI = &GetAI_boss_Jammal;
	newscript->RegisterSelf();
}