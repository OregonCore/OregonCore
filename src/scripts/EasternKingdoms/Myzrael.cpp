#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum Spells
{
	// Myzrael
	SPELL_EARTHQUAKE = 4938,
	SPELL_SUMMONEXILE = 10388,
	SPELL_SUMMONEXILES = 4937,

	// Prismatic Exile
	SPELL_FIREBALL = 34083,
	SPELL_FROSTNOVA = 11831,
	SPELL_LIGHTNINGSHIELD = 12550
};

#define SAY_INTRO	-1910022	
#define SAY_INTRO2	-1910023
#define SAY_INTRO3	-1910024

	struct npc_myzraelAI : public ScriptedAI
	{
		npc_myzraelAI(Creature* creature) : ScriptedAI(creature)
		{
			Initialize();
		}

		void Initialize()
		{
			earthquake_Timer = 6000;
			summon_prismaticexile_Timer = 1000;

			say_timer = 2000;
			say_timer2 = 8000;
			say_timer3 = 15000;
		}

		uint32 earthquake_Timer;
		uint32 summon_prismaticexile_Timer;		

		uint32 say_timer;
		uint32 say_timer2;
		uint32 say_timer3;

		bool intro;
		bool intro2;
		bool intro3;
		bool summoned;

		void Reset() override
		{
			Initialize();

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

			summoned = false;
			intro = false;
			intro2 = false;
			intro3 = false;
		}

		void EnterCombat(Unit* victim) override { }

		void DamageTaken(Unit* /*attacker*/, uint32& damage) override { }

		void UpdateAI(const uint32 diff) override
		{
			if (!UpdateVictim())
			{
				if (!intro && say_timer <= diff)
				{
					DoScriptText(SAY_INTRO, me);
					
					intro = true;
				}
				else say_timer -= diff;

				if (!intro2 && say_timer2 <= diff)
				{
					DoScriptText(SAY_INTRO2, me);					

					intro2 = true;
				}
				else say_timer2 -= diff;

				if (!intro3 && say_timer3 <= diff)
				{
					DoScriptText(SAY_INTRO3, me);			

					intro3 = true;
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
					me->setFaction(91);
					me->SetReactState(REACT_AGGRESSIVE);
				}
				else say_timer3 -= diff;
			}

			if (UpdateVictim())
			{
				if (earthquake_Timer <= diff)
				{
					DoCastVictim(SPELL_EARTHQUAKE);
					earthquake_Timer = 6000;
				}
				else
					earthquake_Timer -= diff;

				if (summon_prismaticexile_Timer <= diff)
				{
					DoCastVictim(SPELL_SUMMONEXILE);
					summon_prismaticexile_Timer = 20000;
				}
				else
					summon_prismaticexile_Timer -= diff;

				if (!summoned)
				{
					if (HealthBelowPct(10))
					{
						DoCast(SPELL_SUMMONEXILES);
						summoned = true;
					}
				}
			}
			DoMeleeAttackIfReady();
		}
	};

	struct npc_prismatic_exileAI : public ScriptedAI
	{
		npc_prismatic_exileAI(Creature* creature) : ScriptedAI(creature)
		{
			Initialize();
		}

		void Initialize()
		{
			fireball_Timer = 6000;
			frostnova_Timer = 12000;
			shield_Timer = 1000;
		}

		uint32 fireball_Timer;
		uint32 frostnova_Timer;
		uint32 shield_Timer;

		void Reset() override
		{
			Initialize();

			me->SetReactState(REACT_AGGRESSIVE);
		}

		void EnterCombat(Unit* victim) override
		{			
			me->SetReactState(REACT_AGGRESSIVE);
		}

		void DamageTaken(Unit* /*attacker*/, uint32& damage) override
		{
		}

		void UpdateAI(const uint32 diff) override
		{
			if (!UpdateVictim())
				return;

			if (fireball_Timer <= diff)
			{
				if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
					DoCast(target, SPELL_FIREBALL);
				fireball_Timer = 8000;
			}
			else
				fireball_Timer -= diff;

			if (frostnova_Timer <= diff)
			{
				DoCast(SPELL_FROSTNOVA);
				frostnova_Timer = 12000;
			}
			else
				frostnova_Timer -= diff;

			if (shield_Timer <= diff)
			{
				DoCast(me, SPELL_LIGHTNINGSHIELD);
				shield_Timer = 15000;
			}
			else
				shield_Timer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI_npc_myzrael(Creature* pCreature)
	{
		return new npc_myzraelAI(pCreature);
	}

	CreatureAI* GetAI_npc_prismatic_exile(Creature* pCreature)
	{
		return new npc_prismatic_exileAI(pCreature);
	}


void AddSC_npc_myzrael()
{
	Script* newscript;

	newscript = new Script;
	newscript->Name = "npc_myzrael";
	newscript->GetAI = &GetAI_npc_myzrael;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_prismatic_exile";
	newscript->GetAI = &GetAI_npc_prismatic_exile;
	newscript->RegisterSelf();
}