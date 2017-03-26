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
SDName: Boss_Terestian_Illhoof
SD%Complete: 95
SDComment: Complete! Needs adjustments to use spell though.
SDCategory: Karazhan
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "karazhan.h"
#include "PassiveAI.h"

#define SAY_SLAY1                   -1532065
#define SAY_SLAY2                   -1532066
#define SAY_DEATH                   -1532067
#define SAY_AGGRO                   -1532068
#define SAY_SACRIFICE1              -1532069
#define SAY_SACRIFICE2              -1532070
#define SAY_SUMMON1                 -1532071
#define SAY_SUMMON2                 -1532072
#define EMOTE_KILREK_DIE			"Kil'rek shrieks in pain and points at his master."

#define SPELL_SUMMON_DEMONCHAINS    30120                   // Summons demonic chains that maintain the ritual of sacrifice.
#define SPELL_DEMON_CHAINS          30206                   // Instant - Visual Effect
#define SPELL_ENRAGE                23537                   // Increases the caster's attack speed by 50% and the Physical damage it deals by 219 to 281 for 10 min.
#define SPELL_SHADOW_BOLT           30055                   // Hurls a bolt of dark magic at an enemy, inflicting Shadow damage.
#define SPELL_SACRIFICE             30115                   // Teleports and adds the debuff
#define SPELL_BERSERK               32965                   // Increases attack speed by 75%. Periodically casts Shadow Bolt Volley.
#define SPELL_SUMMON_FIENDISIMP     30184                   // Summons a Fiendish Imp.
#define SPELL_SUMMON_IMP            30066                   // Summons Kil'rek

#define SPELL_FIENDISH_PORTAL       30171                   // Opens portal and summons Fiendish Portal, 2 sec cast
#define SPELL_FIENDISH_PORTAL_1     30179                   // Opens portal and summons Fiendish Portal, instant cast

#define SPELL_FIREBOLT              30050                   // Blasts a target for 150 Fire damage.
#define SPELL_BROKEN_PACT           30065                   // All damage taken increased by 25%.
#define SPELL_AMPLIFY_FLAMES        30053                   // Increases the Fire damage taken by an enemy by 500 for 25 sec.

#define CREATURE_DEMONCHAINS    17248
#define CREATURE_FIENDISHIMP    17267
#define CREATURE_PORTAL         17265
#define CREATURE_KILREK         17229

#define PORTAL_Z        179.434

float PortalLocations[2][2] =
{
	{ -11249.6933, -1704.61023 },
	{ -11242.1160, -1713.33325 },
};

struct mob_demon_chainAI : public ScriptedAI
{
	mob_demon_chainAI(Creature* c) : ScriptedAI(c) {}

	uint64 SacrificeGUID;

	void Reset()
	{
		SacrificeGUID = 0;
	}

	void EnterCombat(Unit* /*who*/) {}
	void AttackStart(Unit* /*who*/) {}
	void MoveInLineOfSight(Unit* /*who*/) {}

	void JustDied(Unit* /*killer*/)
	{
		if (SacrificeGUID)
		{
			Unit* Sacrifice = Unit::GetUnit((*me), SacrificeGUID);
			if (Sacrifice)
				Sacrifice->RemoveAurasDueToSpell(SPELL_SACRIFICE);
		}
	}
};

struct mob_fiendish_portalAI : public PassiveAI
{
	mob_fiendish_portalAI(Creature* c) : PassiveAI(c), summons(me) {}

	SummonList summons;

	void Reset()
	{
		summons.DespawnAll();
	}

	void JustSummoned(Creature* summon)
	{
		summons.Summon(summon);
		DoZoneInCombat(summon);
	}

	void DespawnAllImp()
	{
		summons.DespawnAll();
	}
};

struct boss_terestianAI : public ScriptedAI
{
	boss_terestianAI(Creature* c) : ScriptedAI(c)
	{
		for (uint8 i = 0; i < 2; ++i)
			PortalGUID[i] = 0;
		pInstance = (ScriptedInstance*)c->GetInstanceData();
	}

	ScriptedInstance* pInstance;

	uint64 PortalGUID[2];
	uint8 PortalsCount;

	uint32 SummonKilrekTimer;
	uint32 SacrificeTimer;
	uint32 ShadowboltTimer;
	uint32 SummonTimer;
	uint32 BerserkTimer;
	uint32 ReSummonKilrek_Timer;

	bool SummonedPortals;
	bool Berserk;
	bool SummonedKilrek;
	bool ReSummonedKilrek;

	void Reset()
	{
		for (uint8 i = 0; i < 2; ++i)
		{
			if (PortalGUID[i])
			{
				if (Creature* pPortal = Unit::GetCreature(*me, PortalGUID[i]))
				{
					CAST_AI(mob_fiendish_portalAI, pPortal->AI())->DespawnAllImp();
					pPortal->ForcedDespawn();
				}

				PortalGUID[i] = 0;
			}
		}

		SummonKilrekTimer = 5000;
		PortalsCount = 0;
		SacrificeTimer = 30000;
		ShadowboltTimer = 5000;
		SummonTimer = 10000;
		BerserkTimer = 600000;
		ReSummonKilrek_Timer = 0;

		SummonedPortals = false;
		SummonedKilrek = false;
		ReSummonedKilrek = false;
		Berserk = false;

		if (pInstance)
			pInstance->SetData(TYPE_TERESTIAN, NOT_STARTED);

		me->RemoveAurasDueToSpell(SPELL_BROKEN_PACT);
	}

	void EnterCombat(Unit* /*who*/)
	{
		DoScriptText(SAY_AGGRO, me);
	}

	void JustSummoned(Creature* pSummoned)
	{
		if (pSummoned->GetEntry() == CREATURE_PORTAL)
		{
			PortalGUID[PortalsCount] = pSummoned->GetGUID();
			++PortalsCount;

			DoScriptText(SAY_SUMMON1, me);
			/* DoScriptText(SAY_SUMMON2, me); */ /* Needs to be added later to randomize summon portal text: between SUMMON1 AND SUMMON2 */
		}
	}

	void KilledUnit(Unit* /*victim*/)
	{
		DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2), me);
	}

	void JustDied(Unit* /*killer*/)
	{
		for (uint8 i = 0; i < 2; ++i)
		{
			if (PortalGUID[i])
			{
				if (Creature* pPortal = Unit::GetCreature((*me), PortalGUID[i]))
					pPortal->ForcedDespawn();

				PortalGUID[i] = 0;
			}
		}

		DoScriptText(SAY_DEATH, me);

		if (pInstance)
			pInstance->SetData(TYPE_TERESTIAN, DONE);
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (SummonKilrekTimer <= diff && SummonedKilrek == false)
		{
			Creature* Kilrek = me->SummonCreature(CREATURE_KILREK, -11237.955078, -1693.597412, 179.237823, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 15000);
			if (Kilrek)
			{
				Kilrek->SetReactState(REACT_AGGRESSIVE);
				Kilrek->AddThreat(me->GetVictim(), 1.0f);
				Kilrek->AI()->AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 1));
				SummonedKilrek = true;
			}			
		}
		else SummonKilrekTimer -= diff;

		if (ReSummonKilrek_Timer <= diff && SummonedKilrek == true && ReSummonedKilrek == false)
		{
			if (me->HasAura(SPELL_BROKEN_PACT, 0))
				me->RemoveAurasDueToSpell(SPELL_BROKEN_PACT);

			Creature* Kilrek = me->SummonCreature(CREATURE_KILREK, -11237.955078, -1693.597412, 179.237823, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 15000);
			if (Kilrek)				
			{
				Kilrek->SetReactState(REACT_AGGRESSIVE);
				Kilrek->AddThreat(me->GetVictim(), 1.0f);
				Kilrek->AI()->AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 1));
				ReSummonedKilrek = true;
			}
		}
		else ReSummonKilrek_Timer -= diff;


		if (SacrificeTimer <= diff)
		{
			Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true);  // target tank as well patch pre 2.1.0
			if (pTarget && pTarget->IsAlive())
			{
				DoCast(pTarget, SPELL_SACRIFICE, true);
				me->SummonCreature(17248, -11233.81f, -1698.38f, 179.236f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30500);

				if (Creature* Chains = me->FindNearestCreature(CREATURE_DEMONCHAINS, 5000))
				{
					CAST_AI(mob_demon_chainAI, Chains->AI())->SacrificeGUID = pTarget->GetGUID();
					Chains->CastSpell(Chains, SPELL_DEMON_CHAINS, true);
					DoScriptText(RAND(SAY_SACRIFICE1, SAY_SACRIFICE2), me);
					SacrificeTimer = 30000;
				}
			}
		}
		else SacrificeTimer -= diff;

		if (ShadowboltTimer <= diff)
		{
			DoCast(SelectUnit(SELECT_TARGET_TOPAGGRO, 0), SPELL_SHADOW_BOLT);
			ShadowboltTimer = 10000;
		}
		else ShadowboltTimer -= diff;

		if (SummonTimer <= diff)
		{
			if (!SummonedPortals)
			{
				for (uint8 i = 0; i < 2; ++i)
				{
					Creature* Portal = me->SummonCreature(CREATURE_PORTAL, PortalLocations[i][0], PortalLocations[i][1], PORTAL_Z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
					if (Portal)
						PortalGUID[i] = Portal->GetGUID();
				}

				SummonedPortals = true;
			}

			uint32 random = rand() % 2;
			Creature* Imp = me->SummonCreature(CREATURE_FIENDISHIMP, PortalLocations[random][0], PortalLocations[random][1], PORTAL_Z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 15000);
			if (Imp)
			{
				Imp->AddThreat(me->GetVictim(), 1.0f);
				Imp->AI()->AttackStart(SelectUnit(SELECT_TARGET_RANDOM, 0));
			}
			SummonTimer = urand(1500, 5000);
		}
		else SummonTimer -= diff;

		if (!Berserk)
		{
			if (BerserkTimer <= diff)
			{
				DoCast(me, SPELL_BERSERK);
				Berserk = true;
			}
			else BerserkTimer -= diff;
		}

		DoMeleeAttackIfReady();
	}
};

#define SPELL_FIREBOLT  30050   // Blasts a target for 181-209 Fire damage.

struct mob_fiendish_impAI : public ScriptedAI
{
	mob_fiendish_impAI(Creature* c) : ScriptedAI(c) {}

	uint32 FireboltTimer;

	void Reset()
	{
		FireboltTimer = 100;

		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
	}

	void EnterCombat(Unit* /*who*/) {}

	void UpdateAI(const uint32 diff)
	{
		//Return since we have no target
		if (!UpdateVictim())
			return;

		if (FireboltTimer <= diff)
		{
			DoCastVictim(SPELL_FIREBOLT);
			FireboltTimer = 2200;
		}
		else FireboltTimer -= diff;

		DoMeleeAttackIfReady();
	}
};

struct mob_kilrekAI : public ScriptedAI
{
	mob_kilrekAI(Creature* c) : ScriptedAI(c)
	{
		pInstance = (ScriptedInstance*)c->GetInstanceData();
	}

	ScriptedInstance* pInstance;

	uint64 TerestianGUID;

	uint32 AmplifyTimer;

	void Reset()
	{
		TerestianGUID = 0;
		AmplifyTimer = 2000;
	}

	void EnterCombat(Unit* /*who*/)
	{
		if (!pInstance)
		{
			ERROR_INST_DATA(me);
			return;
		}
	}

	void JustDied(Unit* /*Killer*/)
	{
		me->TextEmote(EMOTE_KILREK_DIE, false);
		if (pInstance)
		{
			uint64 TerestianGUID = pInstance->GetData64(DATA_TERESTIAN);
			if (TerestianGUID)
			{
				Unit* Terestian = Unit::GetUnit((*me), TerestianGUID);
				if (Terestian && Terestian->IsAlive())
					DoCast(Terestian, SPELL_BROKEN_PACT, true);

				/*Resummon Kil'rek*/
				if (Creature* Terestian = me->FindNearestCreature(15688, 100.0f, true))
				{
					CAST_AI(boss_terestianAI, Terestian->AI())->ReSummonKilrek_Timer = 30000;
					CAST_AI(boss_terestianAI, Terestian->AI())->SummonedKilrek = true;
					CAST_AI(boss_terestianAI, Terestian->AI())->ReSummonedKilrek = false;
				}
			}
		}
		else ERROR_INST_DATA(me);
	}

	void UpdateAI(const uint32 diff)
	{
		//Return since we have no target
		if (!UpdateVictim())
			return;

		if (AmplifyTimer <= diff)
		{
			me->InterruptNonMeleeSpells(false);
			DoCastVictim(SPELL_AMPLIFY_FLAMES);

			AmplifyTimer = urand(10000, 20000);
		}
		else AmplifyTimer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_mob_kilrek(Creature* pCreature)
{
	return new mob_kilrekAI(pCreature);
}

CreatureAI* GetAI_mob_fiendish_imp(Creature* pCreature)
{
	return new mob_fiendish_impAI(pCreature);
}

CreatureAI* GetAI_mob_fiendish_portal(Creature* pCreature)
{
	return new mob_fiendish_portalAI(pCreature);
}

CreatureAI* GetAI_boss_terestian_illhoof(Creature* pCreature)
{
	return new boss_terestianAI(pCreature);
}

CreatureAI* GetAI_mob_demon_chain(Creature* pCreature)
{
	return new mob_demon_chainAI(pCreature);
}

void AddSC_boss_terestian_illhoof()
{
	Script* newscript;
	newscript = new Script;
	newscript->Name = "boss_terestian_illhoof";
	newscript->GetAI = &GetAI_boss_terestian_illhoof;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "mob_fiendish_imp";
	newscript->GetAI = &GetAI_mob_fiendish_imp;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "mob_fiendish_portal";
	newscript->GetAI = &GetAI_mob_fiendish_portal;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "mob_kilrek";
	newscript->GetAI = &GetAI_mob_kilrek;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "mob_demon_chain";
	newscript->GetAI = &GetAI_mob_demon_chain;
	newscript->RegisterSelf();
}

