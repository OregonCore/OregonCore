/* Script Created by Lee 

Comments: Not sure on the timers. 
Scripted: 95%
Creature Scripted: Archmage Arugal

*/

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum eEnums
{
SPELL_SHADOWBOLT		=	 7588,
SPELL_SHADOWPORT1		=	 7587,
SPELL_SHADOWPORT2		=	 7586,
SPELL_SHADOWPORT3		=	 7136,
SPELL_THUNDERSHOCK		=	 7803,
SPELL_TRANSFORM			=	 7621,
SAY_TRANSFORM			= -1527522,
SAY_AGGRO				= -1542750,
SAY_KILL				= -1542751
};


struct boss_archmage_arugalAI : public ScriptedAI
{
	boss_archmage_arugalAI(Creature* c) : ScriptedAI(c) {}

	uint32 Wait_timer;
	uint32 ShadowBolt_Timer;
	uint32 Shadowport_Timer;
	uint32 Thundershock_Timer;
	uint32 Transform_Timer;

	void Reset()
	{
		
		ShadowBolt_Timer = 1000;
		Shadowport_Timer = 20000;
		Thundershock_Timer = 15000;
		Transform_Timer = 25000;
	}

	void EnterCombat(Unit* /*who*/)
	{
		DoScriptText(SAY_AGGRO, me);
		DoZoneInCombat();
	}

	void KilledUnit(Unit* /*Victim*/)
	{
		DoScriptText(SAY_KILL, me);
		me->SetPower(POWER_MANA, (me->GetMaxPower(POWER_MANA) / 2));
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;
		  
		if (me->IsNonMeleeSpellCast(false)) //Checks if spell NPC is already casting a spell
			return;

		// Casts Shadow bolt
		if (ShadowBolt_Timer <= diff)
		{
			DoCastVictim(SPELL_SHADOWBOLT);
			ShadowBolt_Timer = 1000;
		}
		else ShadowBolt_Timer -= diff;
		

		//Casts ThunderShock
		if (Thundershock_Timer <= diff)
		{
			DoCastVictim(SPELL_THUNDERSHOCK);
			Thundershock_Timer = 15000;
		}
		else Thundershock_Timer -= diff;

		// Casts Transmorm Spells
		if (Transform_Timer <= diff)
		{
			if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
			DoScriptText(SAY_TRANSFORM, me);
			DoCastVictim(SPELL_TRANSFORM);
			Transform_Timer = 25000;
		}
		else Transform_Timer -= diff;

		//Casts teleport
		if (Shadowport_Timer <= diff)
		{
			if (me->IsNonMeleeSpellCast(false))
				me->InterruptNonMeleeSpells(false);

			Wait_timer = 8000;
			switch (urand(1, 2))
			{

			case 1:
				{
				DoCast(me, SPELL_SHADOWPORT1);
				}
			case 2:
				{
				DoCast(me, SPELL_SHADOWPORT2);
				}
			}

			Shadowport_Timer = 20000;
		}
		else Shadowport_Timer  -= diff;

		if (Wait_timer <= diff)
		{
			if (me->IsNonMeleeSpellCast(false))
				me->InterruptNonMeleeSpells(false);
			DoCast(me, SPELL_SHADOWPORT3);
		}
		Wait_timer -= diff;

		DoMeleeAttackIfReady();
	}
};


	CreatureAI* GetAI_boss_archmage_arugal(Creature* pCreature)
	{
		return new boss_archmage_arugalAI(pCreature);
	}

	void AddSC_boss_archmage_arugal()
	{
		Script* newscript;
		newscript = new Script;
		newscript->Name = "boss_archmage_arugal";
		newscript->GetAI = &GetAI_boss_archmage_arugal;
		newscript->RegisterSelf();
	}