#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Player.h"
#include "Spell.h"
#include "sunken_temple.h"

enum Spells
{
	// Eranikus
	SPELL_WAR_STOMP = 11876,
	SPELL_DEEP_SLUMBER = 12890,
	SPELL_ACID_BREATH = 12891,
	SPELL_TRASH = 8876
};

#define NPC_SCALEBANE 5277
#define NPC_WANDERER 5283
#define NPC_WHELP 8319

#define SAY_AGGRO -1910077

struct boss_eranikusAI : public ScriptedAI
{
	boss_eranikusAI(Creature* creature) : ScriptedAI(creature)
	{
		Initialize();
	}

	void Initialize()
	{
		Warstomp_timer = 10000;
		Deepslumber_Timer = 12000;
		Trash_timer = 10000;
		Acidbreath_timer = 8000;
	}

	ScriptedInstance* instance;

	uint32 Warstomp_timer;
	uint32 Deepslumber_Timer;
	uint32 Trash_timer;
	uint32 Acidbreath_timer;

	void Reset() override
	{
		Initialize();
	}

	void EnterCombat(Unit* victim) override
	{
		me->InterruptNonMeleeSpells(false);

		std::list<Creature*> creatureList;
		GetCreatureListWithEntryInGrid(creatureList, me, NPC_SCALEBANE, 5000.0f);
		GetCreatureListWithEntryInGrid(creatureList, me, NPC_WANDERER, 5000.0f);
		GetCreatureListWithEntryInGrid(creatureList, me, NPC_WHELP, 5000.0f);
		for (std::list<Creature*>::iterator itr = creatureList.begin(); itr != creatureList.end(); ++itr)
		{
			if (Creature* creature = *itr)
			{
				creature->AI()->AttackStart(me->getVictim());
			}
		}

		DoScriptText(SAY_AGGRO, me);
	}

	void DamageTaken(Unit* /*attacker*/, uint32& damage) override { }

	void JustDied(Unit* /*killer*/) override { }

	void UpdateAI(const uint32 diff) override
	{
		if (!UpdateVictim())
			return;

		if (Warstomp_timer <= diff)
		{
			DoCast(SPELL_WAR_STOMP);
			Warstomp_timer = 20000;
		}
		else
			Warstomp_timer -= diff;

		if (Deepslumber_Timer <= diff)
		{
			if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
				DoCast(target, SPELL_DEEP_SLUMBER);
			Deepslumber_Timer = 15000;
		}
		else
			Deepslumber_Timer -= diff;

		if (Trash_timer <= diff)
		{
			DoCastVictim(SPELL_TRASH);
			Trash_timer = 10000;
		}
		else
			Trash_timer -= diff;

		if (Acidbreath_timer <= diff)
		{
			DoCastAOE(SPELL_ACID_BREATH);
			Acidbreath_timer = 8000;
		}
		else
			Acidbreath_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_eranikus(Creature* pCreature)
{
	return new boss_eranikusAI(pCreature);
}

void AddSC_boss_eranikus()
{
	Script* newscript;
	newscript = new Script;
	newscript->Name = "boss_eranikus";
	newscript->GetAI = &GetAI_boss_eranikus;
	newscript->RegisterSelf();
}