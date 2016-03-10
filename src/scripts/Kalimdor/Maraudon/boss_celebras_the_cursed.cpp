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
SDName: Boss_Celebras_the_Cursed
SD%Complete: 100
SDComment:
SDCategory: Maraudon
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "Maraudon.h"

#define SPELL_WRATH                 21807
#define SPELL_ENTANGLINGROOTS       12747
#define SPELL_CORRUPT_FORCES        21968
#define SPELL_TRANQUILITY			21793

#define NPC_CELEBRAS				13716
#define SAY_SUMMONED			 -1910034

struct celebras_the_cursedAI : public ScriptedAI
{
    celebras_the_cursedAI(Creature* c) : ScriptedAI(c) {}

    uint32 Wrath_Timer;
    uint32 EntanglingRoots_Timer;
    uint32 CorruptForces_Timer;
	uint32 tranquil_timer;

	uint64 celebrasGUID;

    void Reset()
    {
        Wrath_Timer = 8000;
        EntanglingRoots_Timer = 2000;
        CorruptForces_Timer = 30000;
		tranquil_timer = urand(20000, 30000);

		celebrasGUID = 0;
    }

    void EnterCombat(Unit* /*who*/) { }

	void SetData(uint32 type, uint32 data) override
	{		
		if (Creature* celebras = me->FindNearestCreature(NPC_CELEBRAS, 100.0, true))
			celebrasGUID = celebras->GetGUID();
	}

    void JustDied(Unit* /*Killer*/)
    {
		me->SummonCreature(NPC_CELEBRAS, 651.491, 84.702, -86.831, 6.05, TEMPSUMMON_TIMED_DESPAWN, 600000);

		if (Creature* celebras = me->FindNearestCreature(NPC_CELEBRAS, 100.0, true))	
			celebras->Yell(SAY_SUMMONED, LANG_UNIVERSAL, celebrasGUID);	
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //Wrath
        if (Wrath_Timer <= diff)
        {
            Unit* pTarget = NULL;
            pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
            if (pTarget)
                DoCast(pTarget, SPELL_WRATH);
            Wrath_Timer = 11000;
        }
        else Wrath_Timer -= diff;

        //EntanglingRoots
        if (EntanglingRoots_Timer <= diff)
        {
			Unit* pTarget = NULL;
			pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0);
			DoCast(pTarget, SPELL_ENTANGLINGROOTS);
            EntanglingRoots_Timer = 13000;
        }
        else EntanglingRoots_Timer -= diff;

        //CorruptForces
        if (CorruptForces_Timer <= diff)
        {
            me->InterruptNonMeleeSpells(false);
            DoCast(me, SPELL_CORRUPT_FORCES);
            CorruptForces_Timer = 20000;
        }
        else CorruptForces_Timer -= diff;

		if (tranquil_timer <= diff)
		{
			DoCastAOE(SPELL_TRANQUILITY);
			tranquil_timer = urand(20000, 30000);
		}
		else tranquil_timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_celebras_the_cursed(Creature* pCreature)
{
    return new celebras_the_cursedAI (pCreature);
}

/////////////////////////////////
/// NPC_CELEBRAS_THE_REDEMEED ///
/////////////////////////////////

enum QuestScepter
{
	QUEST_THE_SCEPTER_OF_CELEBERAS = 7046,
	SPELL_NARALEXS_AWAKENING = 6271,
};

enum text
{
	SAY_START = -1910035,
	SAY_INSTRUCT = -1910080,
	SAY_READTOME = -1910081,
	SAY_TOGETHER = -1910082,
	SAY_CELEBRAS = -1910083,
	SAY_END = -1910084
};

struct npc_celebrasAI : public npc_escortAI
{
	npc_celebrasAI(Creature* pCreature) : npc_escortAI(pCreature) {}

	void Reset() 
	{
		GoBook = 0;
		GoStone = 0;
		GoLight = 0;
		awaken_timer = 30000;
	}

	ScriptedInstance* instance;

	uint64 GoBook;
	uint64 GoStone;
	uint64 GoLight;

	uint32 awaken_timer;

	void OnGameObjectCreate(GameObject* pGo, bool /*add*/)
	{
		switch (pGo->GetEntry())
		{
		case GO_BOOK:
			GoBook = pGo->GetGUID();
			break;
		case GO_STAFF_STONE:
			GoStone = pGo->GetGUID();
			break;
		case GO_LIGHT:
			GoLight = pGo->GetGUID();
			break;
		}
	}

	void EscortContinue()
	{
		SetEscortPaused(false);
	}

	void BookUsed()
	{
		if (instance->GetData(DATA_BOOK))
		{
			if (GameObject* book = instance->GetGameObject(GoBook))
				EscortContinue();
		}
	}

	void WaypointReached(uint32 uiPointId)
	{
		switch (uiPointId)
		{
		case 1:
			DoScriptText(SAY_INSTRUCT, me);
			break;
		case 3:
			DoScriptText(SAY_READTOME, me);			
			break;
		case 4:
			DoScriptText(SAY_TOGETHER, me);
			me->SummonGameObject(GO_BOOK, 652.272, 74.053, -85.335, 6.08, 0, 0, 0, 0, 0);		
			DoCast(me, SPELL_NARALEXS_AWAKENING, true);
			//SetEscortPaused(true);
			break;
		case 5:
			me->SummonGameObject(GO_LIGHT, 650.746, 74.470, -82.271, 6.21, 0, 0, 0, 0, 0);
			break;
		case 6:
			DoScriptText(SAY_CELEBRAS, me);		
			break;
		case 7:
			DoScriptText(SAY_END, me);
			break;
		case 8:
			if (GameObject* stone = me->FindNearestGameObject(GO_STAFF_STONE, 20.0f))
				stone->SetGoState(GO_STATE_ACTIVE);
			break;
		case 11:
			//check
			me->RemoveGameObject(GO_LIGHT, GoLight);
			me->RemoveGameObject(GO_BOOK, GoBook);
			break;
		case 13:			
			if (Player* pPlayer = GetPlayerForEscort())
				pPlayer->GroupEventHappens(QUEST_THE_SCEPTER_OF_CELEBERAS, me);
			me->SummonCreature(NPC_CELEBRAS, 651.491, 84.702, -86.831, 6.05, TEMPSUMMON_TIMED_DESPAWN, 600000);
			break;
		}
	}

	void UpdateAI(const uint32 uiDiff)
	{
		npc_escortAI::UpdateAI(uiDiff);

		if (!UpdateVictim())
		{
			if (awaken_timer <= uiDiff)
			{
				if (me->HasAura(SPELL_NARALEXS_AWAKENING, 0))
					me->RemoveAurasDueToSpell(SPELL_NARALEXS_AWAKENING);
				awaken_timer = 10000;
			}
			else awaken_timer -= uiDiff;
		}
	}
};

CreatureAI* GetAI_npc_celebras(Creature* pCreature)
{
	npc_celebrasAI* celebrasAI = new npc_celebrasAI(pCreature);

	celebrasAI->AddWaypoint(0, 651.491, 84.702, -86.831, 2000);
	celebrasAI->AddWaypoint(1, 657.493, 79.669, -86.830, 0);
	celebrasAI->AddWaypoint(2, 656.661, 73.410, -86.830, 0);
	celebrasAI->AddWaypoint(3, 655.510, 73.600, -86.830, 3000);
	celebrasAI->AddWaypoint(4, 655.510, 73.600, -86.830, 0); // pause -> start cast etc.
	celebrasAI->AddWaypoint(5, 655.510, 73.600, -86.830, 3000);
	celebrasAI->AddWaypoint(6, 653.309, 73.900, -85.858, 3000);
	celebrasAI->AddWaypoint(7, 656.779, 73.386, -86.828, 0);
	celebrasAI->AddWaypoint(8, 655.616, 66.945, -86.828, 0);
	celebrasAI->AddWaypoint(9, 647.230, 66.300, -86.710, 0);
	celebrasAI->AddWaypoint(10, 647.974, 66.642, -86.709, 5000);
	celebrasAI->AddWaypoint(11, 655.518, 66.717, -86.828, 0);
	celebrasAI->AddWaypoint(12, 657.083, 80.657, -86.830, 0);
	celebrasAI->AddWaypoint(13, 651.491, 84.702, -86.831, 0);

	return celebrasAI;
}

bool QuestAccept_npc_celebras(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
	if (pQuest->GetQuestId() == QUEST_THE_SCEPTER_OF_CELEBERAS)
	{
		pCreature->SetHealth(pCreature->GetMaxHealth());
		pCreature->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
		pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		DoScriptText(SAY_START, pCreature);

		if (npc_escortAI* pEscortAI = CAST_AI(npc_celebrasAI, pCreature->AI()))
			pEscortAI->Start(false, false, pPlayer->GetGUID());
	}
	return true;
}

bool GOHello_go_incantbook(Player* /*pPlayer*/, GameObject* pGO)
{
	ScriptedInstance* instance = (ScriptedInstance*)pGO->GetInstanceData();

	if (instance)
	{
		instance->SetData(DATA_BOOK, instance->GetData(DATA_BOOK));
		return true;
	}

	return false;
}

void AddSC_boss_celebras_the_cursed()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "celebras_the_cursed";
    newscript->GetAI = &GetAI_celebras_the_cursed;
    newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_celebras";
	newscript->GetAI = &GetAI_npc_celebras;
	newscript->pQuestAccept = &QuestAccept_npc_celebras;
    newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "go_incantbook";
	newscript->pGOHello = &GOHello_go_incantbook;
	newscript->RegisterSelf();
}
