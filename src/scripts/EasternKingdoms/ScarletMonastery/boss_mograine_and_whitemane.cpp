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
SDName: Boss_Mograine_And_Whitemane
SD%Complete: 100 Door Closes when reset mograin resses back up after 1st death whitemane resets to orginal spawn point, whitemane does not inturupt spell casting.
SDComment: Edited by Lee
SDCategory: Scarlet Monastery
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "scarlet_monastery.h"

enum eEnums
{
	//Mograine says
	SAY_MO_AGGRO = -1189005,
	SAY_MO_KILL = -1189006,
	SAY_MO_RESSURECTED = -1189007,

	//Whitemane says
	SAY_WH_INTRO = -1189008,
	SAY_WH_KILL = -1189009,
	SAY_WH_RESSURECT = -1189010,

	//Mograine Spells
	SPELL_CRUSADERSTRIKE = 14518,
	SPELL_HAMMEROFJUSTICE = 5589,
	SPELL_LAYONHANDS = 9257,
	SPELL_RETRIBUTIONAURA = 8990,
	

	//Whitemanes Spells
	SPELL_DEEPSLEEP = 9256,
	SPELL_SCARLETRESURRECTION = 9232,
	SPELL_DOMINATEMIND = 14515,
	SPELL_HOLYSMITE = 9481,
	SPELL_HEAL = 12039,
	SPELL_POWERWORDSHIELD = 22187
};

struct boss_scarlet_commander_mograineAI : public ScriptedAI
{
	boss_scarlet_commander_mograineAI(Creature* creature) : ScriptedAI(creature)
	{
		Initialize();
		instance = (ScriptedInstance*)creature->GetInstanceData();
	}

	void Initialize()
	{
		CrusaderStrike_Timer = 10000;
		HammerOfJustice_Timer = 10000;
		_bHasDied = false;
		_bHeal = false;
		_bFakeDeath = false;
	}

	ScriptedInstance* instance;

	uint32 CrusaderStrike_Timer;
	uint32 HammerOfJustice_Timer;

	bool _bHasDied;
	bool _bHeal;
	bool _bFakeDeath;

	void Reset() override
	{
		Initialize();

		//Incase wipe during phase that mograine fake death
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		me->SetStandState(UNIT_STAND_STATE_STAND);

		if (me->IsAlive())
			instance->SetData(TYPE_MOGRAINE_AND_WHITE_EVENT, NOT_STARTED);
			instance->SetData(DATA_CHAPEL_DOOR, FAIL);
			
			
	}

	void JustReachedHome() override
	{
		if (instance->GetData(TYPE_MOGRAINE_AND_WHITE_EVENT) != NOT_STARTED);

	}
	void JustDied()
	{
		instance->SetData(DATA_CHAPEL_DOOR, FAIL);
		
	}

	void EnterCombat(Unit* /*who*/) override
	{
		DoScriptText(SAY_MO_AGGRO, me);
		DoCast(me, SPELL_RETRIBUTIONAURA);
		instance->SetData(DATA_CHAPEL_DOOR, IN_PROGRESS);
		me->CallForHelp(VISIBLE_RANGE);
	}

	void KilledUnit(Unit* /*victim*/) override
	{
		DoScriptText(SAY_MO_KILL, me);
	}

	void DamageTaken(Unit* /*doneBy*/, uint32 &damage) override
	{
		if (damage < me->GetHealth() || _bHasDied || _bFakeDeath)
			return;

		//On first death, fake death and open door, as well as initiate whitemane if exist
		if (Unit* Whitemane = ObjectAccessor::GetUnit(*me, instance->GetData64(DATA_WHITEMANE)))
		{
			instance->SetData(TYPE_MOGRAINE_AND_WHITE_EVENT, IN_PROGRESS);

			Whitemane->GetMotionMaster()->MovePoint(1, 1163.113370f, 1398.856812f, 32.527786f);

			me->GetMotionMaster()->MovementExpired();
			me->GetMotionMaster()->MoveIdle();

			me->SetHealth(0);

			if (me->IsNonMeleeSpellCast(false))
				me->InterruptNonMeleeSpells(false);

			me->ClearComboPointHolders();
			me->RemoveAllAuras();
			me->ClearAllReactives();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->SetStandState(UNIT_STAND_STATE_DEAD);
			
			_bHasDied = true;
			_bFakeDeath = true;

			damage = 0;
		}
	}

	void SpellHit(Unit* /*who*/, const SpellEntry* spell) override
	{
		//When hit with resurrection say text
		if (spell->Id == SPELL_SCARLETRESURRECTION)
		{
			DoScriptText(SAY_MO_RESSURECTED, me);
			_bFakeDeath = false;

			instance->SetData(TYPE_MOGRAINE_AND_WHITE_EVENT, SPECIAL);
		}
	}

	void UpdateAI(uint32 diff) override
	{
		if (!UpdateVictim())
			return;

		if (_bHasDied && !_bHeal && instance->GetData(TYPE_MOGRAINE_AND_WHITE_EVENT) == SPECIAL)
		{
			//On resurrection, stop fake death and heal whitemane and resume fight
			if (Unit* Whitemane = ObjectAccessor::GetUnit(*me, instance->GetData64(DATA_WHITEMANE)))
			{
				me->RemoveAllAuras();
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				me->SetStandState(UNIT_STAND_STATE_STAND);
				DoCast(Whitemane, SPELL_LAYONHANDS);
				
				CrusaderStrike_Timer = 10000;
				HammerOfJustice_Timer = 10000;

				if (me->GetVictim())
					me->GetMotionMaster()->MoveChase(me->GetVictim());

				_bHeal = true;
			}
		}

		//This if-check to make sure mograine does not attack while fake death
		if (_bFakeDeath)
			return;

		//CrusaderStrike_Timer
		if (CrusaderStrike_Timer <= diff)
		{
			DoCastVictim(SPELL_CRUSADERSTRIKE);
			CrusaderStrike_Timer = 10000;
		}
		else CrusaderStrike_Timer -= diff;

		//HammerOfJustice_Timer
		if (HammerOfJustice_Timer <= diff)
		{
			DoCastVictim(SPELL_HAMMEROFJUSTICE);
			HammerOfJustice_Timer = 60000;
		}
		else HammerOfJustice_Timer -= diff;

		DoMeleeAttackIfReady();
	}
};


struct boss_high_inquisitor_whitemaneAI : public ScriptedAI
{
	boss_high_inquisitor_whitemaneAI(Creature* creature) : ScriptedAI(creature)
	{
		Initialize();
		instance = (ScriptedInstance*)creature->GetInstanceData();
	}

	void Initialize()
	{
		Wait_Timer = 7000;
		Heal_Timer = 10000;
		PowerWordShield_Timer = 15000;
		HolySmite_Timer = 6000;

		_bCanResurrectCheck = false;
		_bCanResurrect = false;
	}

	ScriptedInstance* instance;

	uint32 Heal_Timer;
	uint32 PowerWordShield_Timer;
	uint32 HolySmite_Timer;
	uint32 Wait_Timer;

	bool _bCanResurrectCheck;
	bool _bCanResurrect;

	void Reset() override
	{
		Initialize();
		me->GetMotionMaster()->MovePoint(0, 1202.135254f, 1398.852905f, 29.009697f, true); //causes whitemane to go back to spawnpoint.
		if (me->IsAlive())
			instance->SetData(TYPE_MOGRAINE_AND_WHITE_EVENT, NOT_STARTED);
		instance->SetData(TYPE_MOGRAINE_AND_WHITE_EVENT, FAIL); //Makes it so the door closes if wupe on whitemane
	}

	void AttackStart(Unit* who) override
	{
		if (instance->GetData(TYPE_MOGRAINE_AND_WHITE_EVENT) == NOT_STARTED)
			return;

		ScriptedAI::AttackStart(who);
	}

	void EnterCombat(Unit* /*who*/) override
	{
		DoScriptText(SAY_WH_INTRO, me);
	}

	void KilledUnit(Unit* /*victim*/) override
	{
		DoScriptText(SAY_WH_KILL, me);
	}

	void DamageTaken(Unit* /*attacker*/, uint32& damage) override
	{
		if (!_bCanResurrectCheck && damage >= me->GetHealth())
			damage = me->GetHealth() - 1;
	}

	void UpdateAI(uint32 diff) override
	{
		if (!UpdateVictim())
			return;

		if (me->IsNonMeleeSpellCast(false)) //Checks if spell NPC is already casting a spell
			return;

		if (_bCanResurrect)
		{
			if (Creature* mograine = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_MOGRAINE)))
				me->GetMotionMaster()->MoveChase(mograine, 1.0f, mograine->GetAngle(mograine->GetPositionX(), mograine->GetPositionY()));
			//When casting resuruction make sure to delay so on rez when reinstate battle deepsleep runs out
			if (Wait_Timer <= diff)
			{
				if (Creature* mograine = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_MOGRAINE)))
				{
					if (me->IsNonMeleeSpellCast(false)) //If can ress Do not come out of this mode untill Spell has been castsed.
						me->InterruptNonMeleeSpells(false); // Inturupts other spells.
					DoCast(mograine, SPELL_SCARLETRESURRECTION);
					DoScriptText(SAY_WH_RESSURECT, me);
					if (me->GetVictim())
						me->GetMotionMaster()->MoveChase(me->GetVictim());

					_bCanResurrect = false;
				}
			}
			else Wait_Timer -= diff;
		}

		//Cast Deep sleep when health is less than 50%
		if (!_bCanResurrectCheck && !HealthAbovePct(50))
		{
			if (me->IsNonMeleeSpellCast(false))
				me->InterruptNonMeleeSpells(false);

			DoCastVictim(SPELL_DEEPSLEEP);
			_bCanResurrectCheck = true;
			_bCanResurrect = true;
			return;
		}

		//while in "resurrect-mode", don't do anything
		if (_bCanResurrect)
			return;

		//If we are <75% hp cast healing spells at self or Mograine
		if (Heal_Timer <= diff)
		{
			Creature* target = NULL;

			if (!HealthAbovePct(75))
				target = me;

			if (Creature* mograine = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_MOGRAINE)))
			{
				// checking _bCanResurrectCheck prevents her healing Mograine while he is "faking death"
				if (_bCanResurrectCheck && mograine->IsAlive() && !mograine->HealthAbovePct(75))
					target = mograine;
			}

			if (target)
				DoCast(target, SPELL_HEAL);

			Heal_Timer = 13000;
		}
		else Heal_Timer -= diff;

		//PowerWordShield_Timer
		if (PowerWordShield_Timer <= diff)
		{
			DoCast(me, SPELL_POWERWORDSHIELD);
			PowerWordShield_Timer = 15000;
		}
		else PowerWordShield_Timer -= diff;

		//HolySmite_Timer
		if (HolySmite_Timer <= diff)
		{
			DoCastVictim(SPELL_HOLYSMITE);
			HolySmite_Timer = 6000;
		}
		else HolySmite_Timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_boss_scarlet_commander_mograine(Creature* pCreature)
{
	return new boss_scarlet_commander_mograineAI(pCreature);
}

CreatureAI* GetAI_boss_high_inquisitor_whitemane(Creature* pCreature)
{
	return new boss_high_inquisitor_whitemaneAI(pCreature);
}

void AddSC_boss_mograine_and_whitemane()
{
	Script* newscript;

	newscript = new Script;
	newscript->Name = "boss_scarlet_commander_mograine";
	newscript->GetAI = &GetAI_boss_scarlet_commander_mograine;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "boss_high_inquisitor_whitemane";
	newscript->GetAI = &GetAI_boss_high_inquisitor_whitemane;
	newscript->RegisterSelf();
}
