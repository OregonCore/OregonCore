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
SDName: Blades_Edge_Mountains
SD%Complete: 90
SDComment: Quest support: 10556, 10609, 10682, 10980, 10512. Ogri'la->Skettis Flight. (npc_daranelle needs bit more work before consider complete)
SDCategory: Blade's Edge Mountains
EndScriptData */

/* ContentData
mobs_nether_drake
npc_daranelle
npc_overseer_nuaar
npc_saikkal_the_elder
npc_ogre_brute
npc_bloodmaul_brutebane
npc_light_orb_collector
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"
#include "ScriptedFollowerAI.h"

/*######
## mobs_nether_drake
######*/

enum eDrake
{
    SAY_NIHIL_1                 = -1000169,
    SAY_NIHIL_2                 = -1000170,
    SAY_NIHIL_3                 = -1000171,
    SAY_NIHIL_4                 = -1000172,
    SAY_NIHIL_INTERRUPT         = -1000173,

    ENTRY_WHELP                 = 20021,
    ENTRY_PROTO                 = 21821,
    ENTRY_ADOLE                 = 21817,
    ENTRY_MATUR                 = 21820,
    ENTRY_NIHIL                 = 21823,

    SPELL_T_PHASE_MODULATOR     = 37573,

    SPELL_ARCANE_BLAST          = 38881,
    SPELL_MANA_BURN             = 38884,
    SPELL_INTANGIBLE_PRESENCE   = 36513
};

struct mobs_nether_drakeAI : public ScriptedAI
{
    mobs_nether_drakeAI(Creature* c) : ScriptedAI(c) {}

    bool IsNihil;
    uint32 NihilSpeech_Timer;
    uint32 NihilSpeech_Phase;

    uint32 ArcaneBlast_Timer;
    uint32 ManaBurn_Timer;
    uint32 IntangiblePresence_Timer;

    void Reset()
    {
        NihilSpeech_Timer = 2000;
        IsNihil = false;
        if (me->GetEntry() == ENTRY_NIHIL)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            IsNihil = true;
        }
        NihilSpeech_Phase = 1;

        ArcaneBlast_Timer = 7500;
        ManaBurn_Timer = 10000;
        IntangiblePresence_Timer = 15000;
    }

    void EnterCombat(Unit* /*who*/) { }

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (spell->Id == SPELL_T_PHASE_MODULATOR && caster->GetTypeId() == TYPEID_PLAYER)
        {
            uint32 cEntry = 0;

            switch (me->GetEntry())
            {
            case ENTRY_WHELP:
                switch (rand() % 4)
                {
                case 0:
                    cEntry = ENTRY_PROTO;
                    break;
                case 1:
                    cEntry = ENTRY_ADOLE;
                    break;
                case 2:
                    cEntry = ENTRY_MATUR;
                    break;
                case 3:
                    cEntry = ENTRY_NIHIL;
                    break;
                }
                break;
            case ENTRY_PROTO:
                switch (rand() % 3)
                {
                case 0:
                    cEntry = ENTRY_ADOLE;
                    break;
                case 1:
                    cEntry = ENTRY_MATUR;
                    break;
                case 2:
                    cEntry = ENTRY_NIHIL;
                    break;
                }
                break;
            case ENTRY_ADOLE:
                switch (rand() % 3)
                {
                case 0:
                    cEntry = ENTRY_PROTO;
                    break;
                case 1:
                    cEntry = ENTRY_MATUR;
                    break;
                case 2:
                    cEntry = ENTRY_NIHIL;
                    break;
                }
                break;
            case ENTRY_MATUR:
                switch (rand() % 3)
                {
                case 0:
                    cEntry = ENTRY_PROTO;
                    break;
                case 1:
                    cEntry = ENTRY_ADOLE;
                    break;
                case 2:
                    cEntry = ENTRY_NIHIL;
                    break;
                }
                break;
            case ENTRY_NIHIL:
                if (NihilSpeech_Phase)
                {
                    DoScriptText(SAY_NIHIL_INTERRUPT, me);
                    IsNihil = false;
                    switch (rand() % 3)
                    {
                    case 0:
                        cEntry = ENTRY_PROTO;
                        break;
                    case 1:
                        cEntry = ENTRY_ADOLE;
                        break;
                    case 2:
                        cEntry = ENTRY_MATUR;
                        break;
                    }
                }
                break;
            }

            if (cEntry)
            {
                me->UpdateEntry(cEntry);

                if (cEntry == ENTRY_NIHIL)
                {
                    me->InterruptNonMeleeSpells(true);
                    me->RemoveAllAuras();
                    me->DeleteThreatList();
                    me->CombatStop(true);
                    Reset();
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (IsNihil)
        {
            if (NihilSpeech_Phase)
            {
                if (NihilSpeech_Timer <= diff)
                {
                    switch (NihilSpeech_Phase)
                    {
                    case 1:
                        DoScriptText(SAY_NIHIL_1, me);
                        ++NihilSpeech_Phase;
                        break;
                    case 2:
                        DoScriptText(SAY_NIHIL_2, me);
                        ++NihilSpeech_Phase;
                        break;
                    case 3:
                        DoScriptText(SAY_NIHIL_3, me);
                        ++NihilSpeech_Phase;
                        break;
                    case 4:
                        DoScriptText(SAY_NIHIL_4, me);
                        ++NihilSpeech_Phase;
                        break;
                    case 5:
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        me->SetLevitate(true);
                        //then take off to random location. creature is initially summoned, so don't bother do anything else.
                        me->GetMotionMaster()->MovePoint(0, me->GetPositionX() + 100, me->GetPositionY(), me->GetPositionZ() + 100);
						++NihilSpeech_Phase;
                        break;
					case 6:
						me->ForcedDespawn();
                        NihilSpeech_Phase = 0;
                        break;
                    }
                    NihilSpeech_Timer = 5000;
                }
                else NihilSpeech_Timer -= diff;
            }
            return;                                         //anything below here is not interesting for Nihil, so skip it
        }

        if (!UpdateVictim())
            return;

        if (IntangiblePresence_Timer <= diff)
        {
            DoCastVictim(SPELL_INTANGIBLE_PRESENCE);
            IntangiblePresence_Timer = 15000 + rand() % 15000;
        }
        else IntangiblePresence_Timer -= diff;

        if (ManaBurn_Timer <= diff)
        {
            Unit* pTarget = me->getVictim();
            if (pTarget && pTarget->getPowerType() == POWER_MANA)
                DoCast(pTarget, SPELL_MANA_BURN);
            ManaBurn_Timer = 8000 + rand() % 8000;
        }
        else ManaBurn_Timer -= diff;

        if (ArcaneBlast_Timer <= diff)
        {
            DoCastVictim(SPELL_ARCANE_BLAST);
            ArcaneBlast_Timer = 2500 + rand() % 5000;
        }
        else ArcaneBlast_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_mobs_nether_drake(Creature* pCreature)
{
    return new mobs_nether_drakeAI (pCreature);
}

/*######
## npc_daranelle
######*/

#define SAY_DARANELLE -1000174

struct npc_daranelleAI : public ScriptedAI
{
    npc_daranelleAI(Creature* c) : ScriptedAI(c) {}

    void Reset()
    {
    }

    void EnterCombat(Unit* /*who*/)
    {
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (who->GetTypeId() == TYPEID_PLAYER)
        {
            if (who->HasAura(36904, 0))
            {
                DoScriptText(SAY_DARANELLE, me, who);
                //@todo Move the below to updateAI and run if this statement == true
                CAST_PLR(who)->KilledMonsterCredit(21511, me->GetGUID());
                CAST_PLR(who)->RemoveAurasDueToSpell(36904);
            }
        }

        ScriptedAI::MoveInLineOfSight(who);
    }
};

CreatureAI* GetAI_npc_daranelle(Creature* pCreature)
{
    return new npc_daranelleAI (pCreature);
}

/*######
## npc_overseer_nuaar
######*/

#define GOSSIP_HON "Overseer, I am here to negotiate on behalf of the Cenarion Expedition."

bool GossipHello_npc_overseer_nuaar(Player* player, Creature* pCreature)
{
    if (player->GetQuestStatus(10682) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SEND_GOSSIP_MENU(10532, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_overseer_nuaar(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        player->SEND_GOSSIP_MENU(10533, pCreature->GetGUID());
        player->AreaExploredOrEventHappens(10682);
    }
    return true;
}

/*######
## npc_saikkal_the_elder
######*/

#define GOSSIP_HSTE "Yes... yes, it's me."
#define GOSSIP_SSTE "Yes elder. Tell me more of the book."

bool GossipHello_npc_saikkal_the_elder(Player* player, Creature* pCreature)
{
    if (player->GetQuestStatus(10980) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HSTE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SEND_GOSSIP_MENU(10794, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_saikkal_the_elder(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF+1:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SSTE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(10795, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        player->TalkedToCreature(pCreature->GetEntry(), pCreature->GetGUID());
        player->SEND_GOSSIP_MENU(10796, pCreature->GetGUID());
        break;
    }
    return true;
}

/*######
## npc_bloodmaul_brutebane
######*/

enum eBloodmaul
{
    NPC_OGRE_BRUTE        = 19995,
    NPC_QUEST_CREDIT      = 21241,
    GO_KEG                = 184315
};

struct npc_bloodmaul_brutebaneAI : public ScriptedAI
{
    npc_bloodmaul_brutebaneAI(Creature* c) : ScriptedAI(c)
    {
        if (Creature* pOgre = me->FindNearestCreature(NPC_OGRE_BRUTE, 50, true))
        {
            pOgre->SetReactState(REACT_DEFENSIVE);
            pOgre->GetMotionMaster()->MovePoint(1, me->GetPositionX() - 1, me->GetPositionY() + 1, me->GetPositionZ());
        }
    }

    uint64 OgreGUID;

    void Reset()
    {
        OgreGUID = 0;
    }

    void UpdateAI(const uint32 /*uiDiff*/) {}
};

CreatureAI* GetAI_npc_bloodmaul_brutebane(Creature* pCreature)
{
    return new npc_bloodmaul_brutebaneAI (pCreature);
}

/*######
## npc_ogre_brute
######*/

struct npc_ogre_bruteAI : public ScriptedAI
{
    npc_ogre_bruteAI(Creature* c) : ScriptedAI(c) {}

    uint64 PlayerGUID;
	uint32 cleave_timer;

    void Reset()
    {
        PlayerGUID = 0;
		cleave_timer = 6000;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho || (!pWho->IsAlive()))
            return;

        if (me->IsWithinDistInMap(pWho, 50.0f) && (pWho->GetTypeId() == TYPEID_PLAYER) && pWho->ToPlayer()->GetQuestStatus(10512) == QUEST_STATUS_INCOMPLETE)
            PlayerGUID = pWho->GetGUID();

		if (me->IsWithinDistInMap(pWho, 50.0f) && (pWho->GetTypeId() == TYPEID_PLAYER) && pWho->ToPlayer()->GetQuestStatus(10545) == QUEST_STATUS_INCOMPLETE)
			PlayerGUID = pWho->GetGUID();
    }

    void MovementInform(uint32 /*uiMotionType*/, uint32 uiPointId)
    {
        if (Player* pPlayer = Unit::GetPlayer(*me, PlayerGUID))
        {
            if (uiPointId == 1)
            {
                if (GameObject* pKeg = me->FindNearestGameObject(GO_KEG, 20))
                    pKeg->Delete();
                me->HandleEmoteCommand(ANIM_EMOTE_EAT);
                me->SetReactState(REACT_AGGRESSIVE);
				DoCast(me, 37591);
                me->GetMotionMaster()->MoveTargetedHome();
                if (Creature* pCredit = me->FindNearestCreature(NPC_QUEST_CREDIT, 50, true))
                    pPlayer->KilledMonsterCredit(NPC_QUEST_CREDIT, pCredit->GetGUID());
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

		if (cleave_timer <= diff)
		{
			DoCastVictim(15496);
			cleave_timer = 10000;
		}
		else cleave_timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_ogre_brute(Creature* pCreature)
{
    return new npc_ogre_bruteAI(pCreature);
}

/*#########
# go_thunderspike
# UPDATE `gameobject_template` SET `ScriptName` = "go_thunderspike" WHERE `entry` = 184729;
#########*/

#define Q_THE_THUNDERSPIKE 10526
#define GOR_GRIMGUT_ENTRY  21319

bool GOUse_go_thunderspike(Player* player, GameObject* /*_GO*/)
{
    if (player->GetQuestStatus(Q_THE_THUNDERSPIKE) == QUEST_STATUS_INCOMPLETE)
    {
        // to prevent spawn spam :)
        if (Creature* pGor = GetClosestCreatureWithEntry(player, GOR_GRIMGUT_ENTRY, 50.0f, true))
        {
            if (!pGor->getVictim())
                pGor->AI()->AttackStart(player);

            return false;
        }

        Position dest;
        //player->GetValidPointInAngle(dest, 5.0f, frand(0.0f, 2*M_PI), true);
        player->GetPosition(&dest);
        if (Creature* pGor = player->SummonCreature(GOR_GRIMGUT_ENTRY, dest.m_positionX, dest.m_positionY, dest.m_positionZ, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000))
            pGor->AI()->AttackStart(player);
    }

    return false;
}

/*######
## npc_light_orb_collector
######*/

enum RazaniLightOrb
{
    // Creatures
    NPC_LIGHT_ORB_MINI          = 20771,
    NPC_KILL_CREDIT_TRIGGER     = 21929,

    // Quests
    QUEST_LIGHT_FANTASTIC       = 10674,
    QUEST_GATHER_THE_ORBS       = 10859,

    // Misc
    MAX_PULL_DISTANCE           = 20
};

struct npc_light_orb_collectorAI : public ScriptedAI
{
    npc_light_orb_collectorAI(Creature* c) : ScriptedAI(c)
    {
        Reset();
    }

    uint64 selectedOrbGUID;
    bool orbPulled;
    uint32 startTimer;

    void Reset()
    {
        orbPulled    = false;
        startTimer  = 0;
    }

    void RewardCredit(uint64 playerGUID)
    { 
        Map* map = me->GetMap();
        Map::PlayerList const &PlayerList = map->GetPlayers();

        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
            if (Player* player = itr->getSource())
                if (player->GetGUID() == playerGUID)
                    if (me->IsWithinDistInMap(player, 15.0f))
                        if (player->GetQuestStatus(QUEST_LIGHT_FANTASTIC) || player->GetQuestStatus(QUEST_GATHER_THE_ORBS) == QUEST_STATUS_INCOMPLETE)
                            player->KilledMonsterCredit(NPC_KILL_CREDIT_TRIGGER, me->GetGUID());
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (who->GetTypeId() != TYPEID_UNIT || who->GetEntry() != NPC_LIGHT_ORB_MINI)
            return;

        // Select an nearby orb to collect
        if (!startTimer && !orbPulled)
        {
            if (me->GetDistance(who) <= MAX_PULL_DISTANCE)
            {
                selectedOrbGUID = who->GetGUID();
                startTimer = 2000;
            }
        }
        else if (orbPulled && who->GetGUID() == selectedOrbGUID && me->IsWithinDistInMap(who, 15.5f))
        {
            // Despawn the collected orb if close enough
            ((Creature*)who)->ForcedDespawn();

            // Give kill credit to the player
            RewardCredit(who->GetOwnerGUID());

            // Despawn collector
            me->ForcedDespawn();
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (startTimer)
        {
            // Start collecting after some delay
            if (startTimer <= diff)
            {
                Creature* selectedOrb = me->GetMap()->GetCreature(selectedOrbGUID);
                if (!selectedOrb)
                    return;

                // Orb is pulled fast
                selectedOrb->SetWalk(false);

                // Move orb to the collector
                float fX, fY, fZ;;
                selectedOrb->GetMotionMaster()->MoveIdle();
                me->GetContactPoint(selectedOrb, fX, fY, fZ);
                selectedOrb->GetMotionMaster()->MovePoint(0, fX, fY, fZ);

                orbPulled = true;
                startTimer = 0;
            }
            else
                startTimer -= diff;
        }
    }
};

CreatureAI* GetAI_npc_light_orb_collector(Creature* creature)
{
    return new npc_light_orb_collectorAI(creature);
}

/*######
## Q 10821
######*/

enum
{
	SPELL_GREEN_BEAM = 35846,

	QUEST_FIRED = 10821,

	NPC_ANGER = 22422,
	NPC_INVISB = 20736,
	NPC_DOOMCRYER = 19963
};

struct npc_anger_campAI : public ScriptedAI
{
	npc_anger_campAI(Creature* creature) : ScriptedAI(creature) {}

	uint8 Count;

	void Reset()
	{
		Count = 0;
	}

	void Activate()
	{
		++Count;

		if (Count == 5)
		{
			me->SummonCreature(NPC_DOOMCRYER, 2878.467, 4817.107, 282.800, 0.329514, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
			Reset();
		}
	}
};

CreatureAI* GetAI_npc_anger_camp(Creature* creature)
{
	return new npc_anger_campAI(creature);
}

bool GOUse_go_obeliks(Player* player, GameObject* go)
{
	if (player->GetQuestStatus(QUEST_FIRED) == QUEST_STATUS_INCOMPLETE)
	{
		Map* tmpMap = go->GetMap();

		if (!tmpMap)
			return true;

		Creature* trigger = go->FindNearestCreature(NPC_INVISB, 10.0f, true);
		if (!trigger)
		{
			if (Creature* bunny = go->SummonCreature(NPC_INVISB, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), go->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 181000))
			{
				if (Creature* anger = bunny->FindNearestCreature(NPC_ANGER, 100.0f, true))
				{
					bunny->CastSpell(anger, SPELL_GREEN_BEAM, true);
					CAST_AI(npc_anger_campAI, anger->AI())->Activate();
				}
			}
		}
	}
	return false;
}

struct npc_bloodmaul_direwolfAI : public ScriptedAI
{
	npc_bloodmaul_direwolfAI(Creature* pCreature) : ScriptedAI(pCreature) { }

	void Reset()
	{
		spellHit = false;
		me->setFaction(1781);

		rend_timer = 6000;
		PlayerGUID = 0;
		reset_timer = 0;
	}

	bool spellHit;
	uint32 reset_timer;
	uint32 rend_timer;
	uint64  PlayerGUID;

	void MoveInLineOfSight(Unit* pWho)
	{
		if (!pWho || (!pWho->IsAlive()))
			return;

		if (me->IsWithinDistInMap(pWho, 50.0f) && (pWho->GetTypeId() == TYPEID_PLAYER) && pWho->ToPlayer()->GetQuestStatus(10506) == QUEST_STATUS_INCOMPLETE)
			PlayerGUID = pWho->GetGUID();
	}

	void SpellHit(Unit* Hitter, const SpellEntry* Spellkind)
	{
		if (Spellkind->Id == 36310 && !spellHit)
		{
			if (Player* pPlayer = Unit::GetPlayer(*me, PlayerGUID))
			{
				if (Creature* pCredit = me->FindNearestCreature(21176, 50, true))
					pPlayer->KilledMonsterCredit(21176, pCredit->GetGUID());

				me->setFaction(35);
				me->CombatStop();
				spellHit = true;
				reset_timer = 60000;
			}
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (reset_timer <= diff)
			{
				Reset();
			}
			reset_timer -= diff;
		}

		if (UpdateVictim())
		{
			if (rend_timer <= diff)
			{
				DoCastVictim(13443);
				rend_timer = 15000;
			}
			rend_timer -= diff;
		}
		
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_bloodmaul_direwolf(Creature* pCreature)
{
	return new npc_bloodmaul_direwolfAI(pCreature);
}

/*########
# npc_kolphis_darkscale
#########*/

#define GOSSIP_ITEM_KOLPHIS1 "I'm fine, thank you. You asked for me?"
#define GOSSIP_ITEM_KOLPHIS2 "Oh, it's not my fault. I can assure you."
#define GOSSIP_ITEM_KOLPHIS3 "Um, no...no, I don't want that at all."
#define GOSSIP_ITEM_KOLPHIS4 "Impressive. When do we attack?"
#define GOSSIP_ITEM_KOLPHIS5 "Absolutely!"

bool GossipHello_npc_kolphis_darkscale(Player* player, Creature* creature)
{
	if (creature->isQuestGiver())
		player->PrepareQuestMenu(creature->GetGUID());
	if (player->GetQuestStatus(10722) == QUEST_STATUS_INCOMPLETE)
		player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KOLPHIS1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
	player->SEND_GOSSIP_MENU(10539, creature->GetGUID());

	return true;
}

bool GossipSelect_npc_kolphis_darkscale(Player* player, Creature* creature, uint32 sender, uint32 action)
{
	switch (action)
	{
	case GOSSIP_ACTION_INFO_DEF + 1:
		player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KOLPHIS2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
		player->SEND_GOSSIP_MENU(10541, creature->GetGUID());
		break;
	case GOSSIP_ACTION_INFO_DEF + 2:
		player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KOLPHIS3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
		player->SEND_GOSSIP_MENU(10542, creature->GetGUID());
		break;
	case GOSSIP_ACTION_INFO_DEF + 3:
		player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KOLPHIS4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
		player->SEND_GOSSIP_MENU(10543, creature->GetGUID());
		break;
	case GOSSIP_ACTION_INFO_DEF + 4:
		player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KOLPHIS5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
		player->SEND_GOSSIP_MENU(10544, creature->GetGUID());
		break;
	case GOSSIP_ACTION_INFO_DEF + 5:
		player->CompleteQuest(10722);
		player->SEND_GOSSIP_MENU(10545, creature->GetGUID());
		break;
	}
	return true;
}


/*#########
# npc_prophecy_trigger
#########*/

struct npc_prophecy_triggerAI : public ScriptedAI
{
	npc_prophecy_triggerAI(Creature *c) : ScriptedAI(c)
	{
		me->SetReactState(REACT_AGGRESSIVE);
	}

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(10607) == QUEST_STATUS_INCOMPLETE && plWho->HasAura(37642, 0) && plWho->GetDistance(me) < 20.0f)
			{
				switch (me->GetEntry())
				{
				case 22798:
					me->Whisper("From the darkest night shall rise again the raven, shall take flight in the shadows, shall reveal the nature of its kind. Prepare yourself for its coming, for the faithful shall be elevated to take flight with the raven, the rest be forgotten to walk upon the ground, clipped wings and shame.", plWho->GetGUID());
					break;
				case 22799:
					me->Whisper("Steel your minds and guard your thoughts. The dark wings will cloud and consume the minds of the weak, a flock of thralls whose feet may never leave the ground.", plWho->GetGUID());
					break;
				case 22800:
					me->Whisper("The old blood will flow once again with the coming of the raven, the return of the darkness in the skies. Scarlet night, and the rise of the old.", plWho->GetGUID());
					break;
				case 22801:
					me->Whisper("The raven was struck down once for flying too high, unready. The eons have prepared the Dark Watcher for its ascent, to draw the dark cloak across the horizon.", plWho->GetGUID());
					break;
				}

				plWho->KilledMonsterCredit(me->GetEntry(), me->GetGUID());
				me->DisappearAndDie();
			}
		}
	}
};

CreatureAI* GetAI_npc_prophecy_trigger(Creature *_Creature)
{
	return new npc_prophecy_triggerAI(_Creature);
}

bool GOUse_go_prophecy(Player* player, GameObject* go)
{
	if (player->GetQuestStatus(10607) == QUEST_STATUS_INCOMPLETE && player->HasAura(37642, 0))
	{
		if (Creature* prophecy = player->FindNearestCreature(22798, 5.0f, true))
		{
			CAST_AI(npc_prophecy_triggerAI, prophecy->AI())->MoveInLineOfSight(player);
		}
		else
			if (Creature* prophecy = player->FindNearestCreature(22799, 5.0f, true))
			{
				CAST_AI(npc_prophecy_triggerAI, prophecy->AI())->MoveInLineOfSight(player);
			}
			else
				if (Creature* prophecy = player->FindNearestCreature(22800, 5.0f, true))
				{
					CAST_AI(npc_prophecy_triggerAI, prophecy->AI())->MoveInLineOfSight(player);
				}
				else
					if (Creature* prophecy = player->FindNearestCreature(22801, 5.0f, true))
					{
						CAST_AI(npc_prophecy_triggerAI, prophecy->AI())->MoveInLineOfSight(player);
					}
	}
	return false;
}

/*######
## npc_grishna_arakkoa
######*/

struct npc_grishna_arakkoaAI : public ScriptedAI
{
	npc_grishna_arakkoaAI(Creature *c) : ScriptedAI(c) {}

	void Reset() { }

	void JustDied(Unit* killer)
	{
		if (Player *plWho = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(10607) == QUEST_STATUS_INCOMPLETE)
				if (urand(0, 2))
					plWho->CastSpell(plWho, 37642, true, 0, 0, 0);
		}
	}

	void UpdateAI(const uint32 /*diff*/)
	{
		if (!UpdateVictim())
			return;
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_grishna_arakkoa(Creature* pCreature)
{
	return new npc_grishna_arakkoaAI(pCreature);
}

/*######
## npc_koikoi_spirit
######*/

#define EMOTE_THANKS -1910171
#define SPELL_GHOSTLY_STRIKE 36093

struct npc_koikoi_spiritAI : public ScriptedAI
{
	npc_koikoi_spiritAI(Creature *c) : ScriptedAI(c) {}

	void Reset() 
	{
		ghostlystrike_timer = 3000;
	}

	uint32 ghostlystrike_timer;

	void JustDied(Unit* killer)
	{
		if (Creature* leafbeard = me->FindNearestCreature(21326, 15.0f, true))
		{
			leafbeard->setFaction(35);
			DoScriptText(EMOTE_THANKS, leafbeard);
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (ghostlystrike_timer <= diff)
		{
			DoCastVictim(SPELL_GHOSTLY_STRIKE);
			ghostlystrike_timer = 8000;
		}
		else ghostlystrike_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_koikoi_spirit(Creature* pCreature)
{
	return new npc_koikoi_spiritAI(pCreature);
}

/*######
## npc_raven_leafbeard
######*/

#define SPELL_FORCE_OF_NATURE 37696

struct npc_raven_leafbeardAI : public ScriptedAI
{
	npc_raven_leafbeardAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		me->setFaction(1846);
		reset_timer = 0;
		summon_timer = 5000;
	}

	uint32 reset_timer;
	uint32 summon_timer;

	void JustDied(Unit* killer) { }

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (me->FindNearestCreature(22226, 20.0f, false))
			{
				reset_timer = 60000;
				me->CombatStop();
			}

			if (reset_timer <= diff)
			{
				Reset();
			}
			else reset_timer -= diff;
		}

		if (UpdateVictim())
		{
			if (summon_timer <= diff)
			{
				DoCast(SPELL_FORCE_OF_NATURE);
				summon_timer = 45000;
			}
			else summon_timer -= diff;
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_raven_leafbeard(Creature* pCreature)
{
	return new npc_raven_leafbeardAI(pCreature);
}

/*######
## npc_wyrmkult_zealot
######*/

#define SPELL_BLESSING_OF_THE_BLACK 37635
#define SPELL_ZEALOT_ENRAGE 8599

#define NPC_DRAGONKIN 21492
#define NPC_ZEALOT 21382

struct npc_wyrmkult_zealotAI : public ScriptedAI
{
	npc_wyrmkult_zealotAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		enraged = false;
		transformed = false;
		updated = false;

		wait_timer = 0;

		if (me->GetEntry() == NPC_DRAGONKIN)
			me->UpdateEntry(NPC_ZEALOT);
	}

	bool enraged;
	bool transformed;
	bool updated;

	uint32 wait_timer;

	void JustDied(Unit* killer) { }

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (HealthBelowPct(50) && enraged == false)
		{
			DoCast(me, SPELL_ZEALOT_ENRAGE);
			enraged = true;
		}

		if (HealthBelowPct(20) && transformed == false)
		{
			DoCast(me, SPELL_BLESSING_OF_THE_BLACK);
			transformed = true;	
			wait_timer = 4000;
		}
			
		if (transformed == true && wait_timer <= diff && updated == false)
		{
			me->UpdateEntry(21492);
			updated = true;
		}
		else wait_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_wyrmkult_zealot(Creature* pCreature)
{
	return new npc_wyrmkult_zealotAI(pCreature);
}

/*######
## npc_wyrmkult_acolyte
######*/

#define SPELL_BLESSING_OF_THE_BLACK 37635
#define SPELL_FIREBALL_ACOLYTE 9053
#define SPELL_POWER_WORD_SHIELD 17139

#define NPC_DRAGONKIN 21492
#define NPC_ACOLYTE 21383

struct npc_wyrmkult_acolyteAI : public ScriptedAI
{
	npc_wyrmkult_acolyteAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		transformed = false;
		updated = false;

		wait_timer = 0;
		shield_timer = 6000;
		fireball_timer = 7500;

		if (me->GetEntry() == NPC_DRAGONKIN)
			me->UpdateEntry(NPC_ACOLYTE);
	}

	bool transformed;
	bool updated;

	uint32 wait_timer;
	uint32 shield_timer;
	uint32 fireball_timer;

	void JustDied(Unit* killer) { }

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (fireball_timer <= diff)
		{
			DoCastVictim(SPELL_FIREBALL_ACOLYTE);
			fireball_timer = 8000;
		}
		else fireball_timer -= diff;

		if (shield_timer <= diff)
		{
			DoCast(me, SPELL_POWER_WORD_SHIELD);
			shield_timer = 20000;
		}
		else shield_timer -= diff;

		if (HealthBelowPct(20) && transformed == false)
		{
			DoCast(me, SPELL_BLESSING_OF_THE_BLACK);
			transformed = true;
			wait_timer = 4500;
		}

		if (transformed == true && wait_timer <= diff && updated == false)
		{
			me->UpdateEntry(21492);
			updated = true;
		}
		else wait_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_wyrmkult_acolyte(Creature* pCreature)
{
	return new npc_wyrmkult_acolyteAI(pCreature);
}

/*######
## npc_evergrove_ancient
######*/

#define SPELL_TRAMPLE 39274

struct npc_evergrove_ancientAI : public ScriptedAI
{
	npc_evergrove_ancientAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		trample_timer = 6000;
		me->setFaction(113);
		me->SetReactState(REACT_AGGRESSIVE);
	}

	uint32 trample_timer;

	void JustDied(Unit* killer) { }

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (Creature* baelmon = me->FindNearestCreature(19747, 30.0f, true))
		{
			if (!baelmon->getVictim())
				me->AI()->AttackStart(baelmon);
		}

		if (trample_timer <= diff)
		{
			DoCast(SPELL_TRAMPLE);
			trample_timer = 15000;
		}
		else trample_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_evergrove_ancient(Creature* pCreature)
{
	return new npc_evergrove_ancientAI(pCreature);
}

/*######
## npc_baelmon
######*/

#define SAY_BAELMON_AGGRO -1910172
#define SAY_SUMMON_HOUND -1910173

#define SPELL_CRIPPLE				11443
#define SPELL_RAINOFFIRE			31598
#define SPELL_SUMMON_WRATH_HOUND	39272
#define SPELL_ETHEREAL_CHANNEL		35518

struct npc_baelmonAI : public ScriptedAI
{
	npc_baelmonAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		cripple_timer = 2000;
		rainoffire_timer = 18000;
		SummonHound = false;	
	}

	uint32 cripple_timer;
	uint32 rainoffire_timer;

	bool SummonHound;

	void EnterCombat(Unit* /*who*/)
	{
		DoScriptText(SAY_BAELMON_AGGRO, me);
	}

	void JustDied(Unit* killer) { }

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (Creature* channel = me->FindNearestCreature(20736, 40.0f, true))
			{
				DoCast(channel, SPELL_ETHEREAL_CHANNEL);
			}
		}

		if (UpdateVictim())
		{
			if (HealthBelowPct(20) && SummonHound == false)
			{
				DoScriptText(SAY_SUMMON_HOUND, me);
				DoCast(SPELL_SUMMON_WRATH_HOUND);
				SummonHound = true;
			}

			if (cripple_timer <= diff)
			{
				DoCastVictim(SPELL_CRIPPLE);
				cripple_timer = 17000;
			}
			else cripple_timer -= diff;

			if (rainoffire_timer <= diff)
			{
				DoCast(SPELL_RAINOFFIRE);
				rainoffire_timer = 30000;
			}
			else rainoffire_timer -= diff;
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_baelmon(Creature* pCreature)
{
	return new npc_baelmonAI(pCreature);
}

/*#########
# npc_groc_trigger
#########*/

struct npc_groc_triggerAI : public ScriptedAI
{
	npc_groc_triggerAI(Creature *c) : ScriptedAI(c)
	{
		me->SetReactState(REACT_AGGRESSIVE);
	}

	void Reset()
	{
		summoned = false;
	}

	bool summoned;

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(10806) == QUEST_STATUS_INCOMPLETE && plWho->HasItemCount(31808, 1, false) && plWho->GetDistance(me) < 5.0f)
			{
				switch (me->GetEntry())
				{
				case 61002:
					me->SummonCreature(20555, 3713.08f, 5396.28f, -3.85f, 4.94f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000), plWho->GetGUID();
					me->SummonCreature(22473, 3718.914f, 5357.06f, -8.11f, 1.82f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 35000), plWho->GetGUID();
					summoned = true;
					me->DisappearAndDie();
					break;
				}
			}

			if (plWho->GetQuestStatus(10742) == QUEST_STATUS_INCOMPLETE && plWho->HasItemCount(31146, 1, false) && plWho->GetDistance(me) < 5.0f)
			{
				switch (me->GetEntry())
				{
				case 61002:
					me->SummonCreature(20555, 3713.08f, 5396.28f, -3.85f, 4.94f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000), plWho->GetGUID();
					me->SummonCreature(22448, 3718.914f, 5357.06f, -8.11f, 1.82f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 35000), plWho->GetGUID();
					summoned = true;
					me->DisappearAndDie();
					break;
				}
			}
		}
	}
};

CreatureAI* GetAI_npc_groc_trigger(Creature *_Creature)
{
	return new npc_groc_triggerAI(_Creature);
}

/*######
## npc_goc
######*/

#define SAY_GROC_ENTRY -1910174
#define SAY_GROC_NAME -1910175
#define SAY_GOC_DIED -1910176
#define SAY_GOC_DIED2 -1910186
#define SAY_GROC_CRUSH "I will crush you!"

#define SPELL_GROUND_SMASH 38784
#define SPELL_BOULDER_VOLLEY 38783		

struct npc_gocAI : public ScriptedAI
{
	npc_gocAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		smash_timer = 12000;
		boulder_timer = 3000;

		uiSumTimer = 5000;
		uiStepsTimer = 0;
		uiSteps = 0;

		Event = true;
	}

	bool Event;

	uint32 uiSumTimer;
	uint32 uiStepsTimer;
	uint32 uiSteps;

	uint32 smash_timer;
	uint32 boulder_timer;

	int32 NextStep(uint32 uiSteps)
	{
		switch (uiSteps)
		{
		case 1:
			me->SetVisibility(VISIBILITY_ON);		
			return 100;
		case 2:
			DoScriptText(SAY_GROC_ENTRY, me, 0);
			me->HandleEmoteCommand(ANIM_EMOTE_ROAR);
			return 14000;
		case 3:
			DoScriptText(SAY_GROC_NAME, me, 0);
			me->HandleEmoteCommand(ANIM_EMOTE_ROAR);
			return 10000;
		case 4:
			me->setFaction(14);
			me->GetMotionMaster()->MovePath(600600600, false);
			me->SetReactState(REACT_AGGRESSIVE);
			return 1000;
		default:
			return 0;
		}
	}

	void EnterCombat(Unit* /*who*/)
	{
		me->Say(SAY_GROC_CRUSH, LANG_UNIVERSAL, 0);
	}

	void JustDied(Unit* killer)
	{
		if (Creature* sabelian = me->FindNearestCreature(22496, 50.0f, true))
			DoScriptText(SAY_GOC_DIED, sabelian, 0);

		if (Creature* rexxar = me->FindNearestCreature(22448, 50.0f, true))
			DoScriptText(SAY_GOC_DIED2, rexxar, 0);
	}

	void UpdateAI(const uint32 diff)
	{
		if (uiStepsTimer <= diff)
		{
			if (Event)
				uiStepsTimer = NextStep(++uiSteps);
		}
		else uiStepsTimer -= diff;

		if (!UpdateVictim())
			return;

		if (smash_timer <= diff)
		{
			DoCast(SPELL_GROUND_SMASH);
			smash_timer = 15000;
		}
		else smash_timer -= diff;

		if (boulder_timer <= diff)
		{
			if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 40, true))
			DoCast(pTarget, SPELL_BOULDER_VOLLEY);
			boulder_timer = 18000;
		}
		else boulder_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_goc(Creature* pCreature)
{
	return new npc_gocAI(pCreature);
}

/*######
## npc_sabelian_human
######*/

#define YELL_SABELIAN_LINE_1 -1910177
#define YELL_SABELIAN_LINE_2 -1910178
#define YELL_SABELIAN_LINE_3 -1910179
#define SABELIAN_TRANSFORM_YELL -1910180
#define SABELIAN_TRANSFORM_EMOTE -1910181

struct npc_sabelian_humanAI : public ScriptedAI
{
	npc_sabelian_humanAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		me->setFaction(35);
		transform_timer = 50000;
		summoned = false;
		transformed = false;
		GocEvent = false;
		SayLine1 = false;
		SayLine2 = false;
		Say = false;
	}

	uint32 transform_timer;

	uint32 conversation_timer;
	uint32 conv_timer;
	uint32 conversa_timer;
	uint32 summon_timer;

	bool SayLine1;
	bool SayLine2;
	bool Say;
	bool transformed;
	bool GocEvent;
	bool summoned;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (!GocEvent)
			{
				if (Creature* goc = me->FindNearestCreature(20555, 50.0f, true))
				{
					GocEvent = true;
					conversa_timer = 5000;
				}
			}
		
			if (conversa_timer <= diff && GocEvent == true && Say == false)
			{
				DoScriptText(YELL_SABELIAN_LINE_1, me);
				me->HandleEmoteCommand(ANIM_EMOTE_POINT);
				conversation_timer = 12000;
				Say = true;
			}
			else conversa_timer -= diff;

			if (conversation_timer <= diff && SayLine1 == false && GocEvent == true)
			{
				DoScriptText(YELL_SABELIAN_LINE_2, me);
				me->HandleEmoteCommand(ANIM_EMOTE_POINT);
				SayLine1 = true;
				conv_timer = 6000;
			}
			else conversation_timer -= diff;

			if (conv_timer <= diff && SayLine2 == false && GocEvent == true && SayLine1 == true)
			{
				DoScriptText(YELL_SABELIAN_LINE_3, me);
				me->HandleEmoteCommand(ANIM_EMOTE_ROAR);
				me->SetReactState(REACT_AGGRESSIVE);
				me->setFaction(11);
				SayLine2 = true;
			}
			else conv_timer -= diff;			
		}
		
		if (transformed == false && transform_timer <= diff)
		{
			DoScriptText(SABELIAN_TRANSFORM_YELL, me);
			DoScriptText(SABELIAN_TRANSFORM_EMOTE, me);
			DoCast(me, 39255);
			transformed = true;
			summon_timer = 5000;
		}
		else transform_timer -= diff;

		if (summoned == false && transformed == true && summon_timer <= diff)
		{
			me->SummonCreature(22496, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
			me->DisappearAndDie();
			summoned = true;
		}
		else summon_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_sabelian_human(Creature* pCreature)
{
	return new npc_sabelian_humanAI(pCreature);
}

/*######
## npc_sabelian_dragon
######*/

struct npc_sabelian_dragonAI : public ScriptedAI
{
	npc_sabelian_dragonAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		knockdown_timer = 10000;
		flamebreath_timer = 6000;

		me->setFaction(11);
		me->SetReactState(REACT_AGGRESSIVE);
	}

	uint32 knockdown_timer;
	uint32 flamebreath_timer;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (flamebreath_timer <= diff)
		{
			DoCastVictim(43582);
			flamebreath_timer = 20000;
		}
		else flamebreath_timer -= diff;

		if (knockdown_timer <= diff)
		{
			DoCastVictim(35783);
			knockdown_timer = 23000;
		}
		else knockdown_timer -= diff;
		
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_sabelian_dragon(Creature* pCreature)
{
	return new npc_sabelian_dragonAI(pCreature);
}

/*######
## npc_rexxar
######*/

#define SAY_REXXAR_1 -1910183
#define SAY_REXXAR_2 -1910184
#define SAY_REXXAR_3 -1910185

#define SPELL_SUMMON_HUFFER 39251
#define SPELL_SUMMON_MISHA 39269

struct npc_rexxarAI : public ScriptedAI
{
	npc_rexxarAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		me->setFaction(83);
		summoned = false;
		summoned2 = false;
		GocEvent = false;
		SayLine1 = false;
		SayLine2 = false;
		Say = false;

		summon_timer = 35000;
		summon_timer2 = 50000;
	}

	uint32 conversation_timer;
	uint32 conv_timer;
	uint32 conversa_timer;
	uint32 summon_timer;
	uint32 summon_timer2;

	bool SayLine1;
	bool SayLine2;
	bool Say;
	bool GocEvent;
	bool summoned;
	bool summoned2;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (!GocEvent)
			{
				if (Creature* goc = me->FindNearestCreature(20555, 50.0f, true))
				{
					GocEvent = true;
					conversa_timer = 5000;
				}
			}

			if (conversa_timer <= diff && GocEvent == true && Say == false)
			{
				DoScriptText(SAY_REXXAR_1, me);
				me->HandleEmoteCommand(ANIM_EMOTE_POINT);
				conversation_timer = 7000;
				Say = true;
			}
			else conversa_timer -= diff;

			if (conversation_timer <= diff && SayLine1 == false && GocEvent == true)
			{
				DoScriptText(SAY_REXXAR_2, me);
				me->HandleEmoteCommand(ANIM_EMOTE_POINT);
				SayLine1 = true;
				conv_timer = 11000;
			}
			else conversation_timer -= diff;

			if (conv_timer <= diff && SayLine2 == false && GocEvent == true && SayLine1 == true)
			{
				DoScriptText(SAY_REXXAR_3, me);
				me->HandleEmoteCommand(ANIM_EMOTE_ROAR);
				me->SetReactState(REACT_AGGRESSIVE);
				me->setFaction(85);
				SayLine2 = true;
			}
			else conv_timer -= diff;
		}

		if (UpdateVictim())
		{
			if (!summoned && summon_timer <= diff)
			{
				DoCast(SPELL_SUMMON_HUFFER);
				summoned = true;
			}
			else summon_timer -= diff;

			if (!summoned2 && summon_timer2 <= diff)
			{
				DoCast(SPELL_SUMMON_MISHA);
				summoned2 = true;
			}
			else summon_timer2 -= diff;
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_rexxar(Creature* pCreature)
{
	return new npc_rexxarAI(pCreature);
}

/*#########
# npc_thunderstrike_trigger
#########*/

#define QUEST_VISION_GUIDE 10525

struct npc_thunderstrike_triggerAI : public ScriptedAI
{
	npc_thunderstrike_triggerAI(Creature *c) : ScriptedAI(c)
	{
		me->SetReactState(REACT_AGGRESSIVE);
	}

	void Reset() { }

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player* plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(QUEST_VISION_GUIDE) == QUEST_STATUS_INCOMPLETE && plWho->HasItemCount(30481, 1, false) && plWho->GetDistance(me) < 8.0f)
			{
				switch (me->GetEntry())
				{
				case 61003:
					plWho->CompleteQuest(QUEST_VISION_GUIDE);
					break;
				}
			}		

			if (plWho->GetQuestStatus(QUEST_VISION_GUIDE) == QUEST_STATUS_COMPLETE && plWho->HasItemCount(30481, 1, false) && plWho->GetDistance(me) < 8.0f)
			{
				switch (me->GetEntry())
				{
				case 61003:
					plWho->TeleportTo(530, 2280.67f, 5983.65f, 142.49f, 3.04f, 0);
					break;
				}
			}
		}
	}
};

CreatureAI* GetAI_npc_thunderstrike_trigger(Creature* pCreature)
{
	return new npc_thunderstrike_triggerAI(pCreature);
}

/*######
## npc_grimgut
######*/

#define SAY_PICKUP_SPEAR -1910182

struct npc_grimgutAI : public ScriptedAI
{
	npc_grimgutAI(Creature *c) : ScriptedAI(c) {}

	void Reset() { }

	void EnterCombat(Unit* /*who*/)
	{
		DoScriptText(SAY_PICKUP_SPEAR, me);
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_grimgut(Creature* pCreature)
{
	return new npc_grimgutAI(pCreature);
}

/*#########
# npc_onspiritswings_trigger
#########*/

#define QUEST_ON_SPIRITS_WINGS 10714

struct npc_onspiritswings_triggerAI : public ScriptedAI
{
	npc_onspiritswings_triggerAI(Creature *c) : ScriptedAI(c)
	{
		me->SetReactState(REACT_AGGRESSIVE);
	}

	void Reset() { }

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(QUEST_ON_SPIRITS_WINGS) == QUEST_STATUS_INCOMPLETE && plWho->HasItemCount(31128, 1, false) && plWho->GetDistance(me) < 22.0f)
			{
				switch (me->GetEntry())
				{
				case 61004:
					plWho->CompleteQuest(QUEST_ON_SPIRITS_WINGS);
					me->SummonCreature(23134, 3085.1f, 5854.1f, -0.0f, 0.47f, TEMPSUMMON_TIMED_DESPAWN, 20000);
					break;
				}
			}							
		}
	}
};

CreatureAI* GetAI_npc_onspiritswings_trigger(Creature* pCreature)
{
	return new npc_onspiritswings_triggerAI(pCreature);
}

/*#########
# Quest_The_Smallest_Creatures
#########*/

#define QUEST_THE_SMALLEST_CREATURES 10720
#define MOONSHINE_TRIGGER 22367
#define GREEN_TRIGGER 22356
#define FERMENTED_TRIGGER 22368

bool GOUse_go_moonshine_keg(Player* player, GameObject* go)
{
	if (player->GetQuestStatus(QUEST_THE_SMALLEST_CREATURES) == QUEST_STATUS_INCOMPLETE)
	{
		Creature* moonshine = go->FindNearestCreature(MOONSHINE_TRIGGER, 5.0f, true);
		{
			player->KilledMonsterCredit(MOONSHINE_TRIGGER, 0);
		}
	}
	return false;
}

bool GOUse_go_greenspot_keg(Player* player, GameObject* go)
{
	if (player->GetQuestStatus(QUEST_THE_SMALLEST_CREATURES) == QUEST_STATUS_INCOMPLETE)
	{
		Creature* moonshine = go->FindNearestCreature(GREEN_TRIGGER, 5.0f, true);
		{
			player->KilledMonsterCredit(GREEN_TRIGGER, 0);
		}
	}
	return false;
}

bool GOUse_go_fermentedseed_keg(Player* player, GameObject* go)
{
	if (player->GetQuestStatus(QUEST_THE_SMALLEST_CREATURES) == QUEST_STATUS_INCOMPLETE)
	{
		Creature* moonshine = go->FindNearestCreature(FERMENTED_TRIGGER, 5.0f, true);
		{
			player->KilledMonsterCredit(FERMENTED_TRIGGER, 0);
		}
	}
	return false;
}

/*######
## npc_huffer
######*/

struct npc_hufferAI : public ScriptedAI
{
	npc_hufferAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		me->setFaction(85);
		me->GetMotionMaster()->MovePath(600600601, true);
	}
};

CreatureAI* GetAI_npc_huffer(Creature* pCreature)
{
	return new npc_hufferAI(pCreature);
}

/*######
## QUEST_PICKING_UP_SOME_POWER_CONVERTERS
######*/

#define QUEST_PICKING_UP_SOME_POWER_CONVERTERS 10584
#define NPC_ENCASED_ELECTROMENTAL 21731

bool GOUse_go_power_converter(Player* player, GameObject* go)
{
	if (player->GetQuestStatus(QUEST_PICKING_UP_SOME_POWER_CONVERTERS) == QUEST_STATUS_INCOMPLETE)
	{
		go->SummonCreature(NPC_ENCASED_ELECTROMENTAL, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
	}
	return false;
}

/*######
## GO_NETHER_DRAKE_EGG
######*/

#define QUEST_WHAT_CAME_FIRST_THE_DRAKE_OR_THE_EGG 10609
#define NPC_NETHER_WHELP 20021

bool GOUse_go_netherdrake_egg(Player* player, GameObject* go)
{
	if (player->GetQuestStatus(QUEST_WHAT_CAME_FIRST_THE_DRAKE_OR_THE_EGG) == QUEST_STATUS_INCOMPLETE)
	{
		go->SummonCreature(NPC_NETHER_WHELP, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
	}
	return false;
}

/*######
## npc_nexusprince_razaan
######*/

#define YELL_AGGRO "Who dares to interrupt my operation?"
#define SPELL_ENERGY_FLUX 35925

struct npc_nexusprince_razaanAI : public ScriptedAI
{
	npc_nexusprince_razaanAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		energy_timer = 5000;
	}

	uint32 energy_timer;

	void EnterCombat(Unit* /*who*/)
	{
		me->Yell(YELL_AGGRO, LANG_UNIVERSAL, 0);
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (energy_timer <= diff)
		{
			DoCastVictim(SPELL_ENERGY_FLUX);
			energy_timer = 10000;
		}
		else energy_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_nexusprince_razaan(Creature* pCreature)
{
	return new npc_nexusprince_razaanAI(pCreature);
}

/*######
## npc_vimgol_circle_trigger
######*/

struct npc_summon_circle_triggerAI : public ScriptedAI
{
	npc_summon_circle_triggerAI(Creature *c) : ScriptedAI(c) {}

	uint8 Count;

	void Reset()
	{
		Count = 0;
	}

	void Activate()
	{
		++Count;

		if (Count == 5)
		{
			me->SummonCreature(22911, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
			me->DisappearAndDie();
		}
	}
};

CreatureAI* GetAI_npc_summon_circle_trigger(Creature* pCreature)
{
	return new npc_summon_circle_triggerAI(pCreature);
}

struct npc_vimgol_circle_triggerAI : public ScriptedAI
{
	npc_vimgol_circle_triggerAI(Creature *c) : ScriptedAI(c) {}

	void Reset() { }

	void MoveInLineOfSight(Unit *pWho)
	{
		Map* tmpMap = me->GetMap();

		if (!tmpMap)
			return;

		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->IsAlive() && plWho->GetDistance(me) < 1.0f)
			{
				switch (me->GetEntry())
				{
				case 23040:
					if (Creature* summontrigger = me->FindNearestCreature(23081, 25.0f, true))				
						CAST_AI(npc_summon_circle_triggerAI, summontrigger->AI())->Activate();				
					break;
				}
			}
		}
	}
};

CreatureAI* GetAI_npc_vimgol_circle_trigger(Creature* pCreature)
{
	return new npc_vimgol_circle_triggerAI(pCreature);
}

/*######
## npc_vimgol_the_vile
######*/

#define YELL_AGGRO "YOU DARE SUMMON ME!?"
#define YELL_ENRAGE "Now me grow bigger and crush you!"
#define SPELL_SHADOW_BOLT_VOLLEY 46082
#define SPELL_GROWTH 40545
#define SPELL_BLOODLUST 28902

struct npc_vimgol_the_vileAI : public ScriptedAI
{
	npc_vimgol_the_vileAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		enrage = false;

		volley_timer = 7000;
		bloodlust_timer = 60000;
	}

	bool enrage;

	uint32 volley_timer;
	uint32 bloodlust_timer;

	void EnterCombat(Unit* /*who*/)
	{
		me->Yell(YELL_AGGRO, LANG_UNIVERSAL, 0);
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (volley_timer <= diff)
		{
			DoCastVictim(SPELL_SHADOW_BOLT_VOLLEY);
			volley_timer = 7000;
		}
		else volley_timer -= diff;

		if (bloodlust_timer <= diff)
		{
			DoCast(me, SPELL_BLOODLUST);
			bloodlust_timer = 90000;
		}
		else bloodlust_timer -= diff;

		if (!enrage && HealthBelowPct(50))
		{
			me->GetMotionMaster()->MovePoint(1, 3280.1f, 4640.3f, 216.5f, 4.8f);
			me->Yell(YELL_ENRAGE, LANG_UNIVERSAL, 0);
			DoCast(me, SPELL_GROWTH);
			enrage = true;
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_vimgol_the_vile(Creature* pCreature)
{
	return new npc_vimgol_the_vileAI(pCreature);
}

/*######
## the_soulgrinder
######*/

#define QUEST_INTO_THE_SOULGRINDER 11000

#define SPELL_BANISH 39622
#define SPELL_CORRUPTION 39621
#define CHANNEL_VISUAL 39920

#define YELL_SKULLOC_AGGRO "Argh! I will shred your soul and grind it to dust!"

#define NPC_SUNDERED_SPIRIT 22912
#define NPC_SKULLOC 22910
#define NPC_BUNNY_1 23037
#define NPC_BUNNY_2 61006
#define NPC_BUNNY_3 61007
#define NPC_BUNNY_4 61008
#define NPC_HELPER_1 61009
#define NPC_HELPER_2 61010
#define NPC_HELPER_3 61011

#define SKULLOC_LOC 3482.1f, 5551.9f, 7.5f, 0.7f
#define SUMMON_LOC_1 3538.8f, 5572.1f, -2.3f, 2.2f
#define SUMMON_LOC_2 3517.9f, 5594.9f, -1.8f, 5.4f
#define SUMMON_LOC_3 3546.4f, 5603.3f, -3.1f, 4.0f
#define SUMMONED_GO_LOC 3538.8f, 5594.4f, -2.3f, 4.0f

struct the_soulgrinderAI : public ScriptedAI
{
	the_soulgrinderAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		me->SetReactState(REACT_AGGRESSIVE);

		summoned = false;
	}

	bool summoned;

	void CastVisual()
	{
		if (Creature* visual_1 = me->FindNearestCreature(NPC_BUNNY_1, 100.0f, true))
		{
			DoCast(visual_1, CHANNEL_VISUAL, true);
		}
	}

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(QUEST_INTO_THE_SOULGRINDER) == QUEST_STATUS_INCOMPLETE && plWho->GetDistance(me) < 5.0f)
			{
				switch (me->GetEntry())
				{
				case 23019:
					if (!summoned)
					{
						DoCast(me, 39918);
						me->SummonCreature(NPC_SKULLOC, SKULLOC_LOC, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 300000);
						summoned = true;
					}
					break;
				}
			}
		}
	}

	void JustDied(Unit* /*killer*/)
	{
		if (Creature* skulloc = me->FindNearestCreature(NPC_SKULLOC, 120.0f, true))
		{
			skulloc->DisappearAndDie();
		}

		if (Creature* visual_1 = me->FindNearestCreature(NPC_HELPER_1, 75.0f, true))
		{
			visual_1->DisappearAndDie();
		}

		if (Creature* visual_2 = me->FindNearestCreature(NPC_HELPER_2, 75.0f, true))
		{
			visual_2->DisappearAndDie();
		}

		if (Creature* visual_3 = me->FindNearestCreature(NPC_HELPER_3, 75.0f, true))
		{
			visual_3->DisappearAndDie();
		}
	}
};

CreatureAI* GetAI_the_soulgrinder(Creature* pCreature)
{
	return new the_soulgrinderAI(pCreature);
}

struct helper_bunny_1AI : public ScriptedAI
{
	helper_bunny_1AI(Creature *c) : ScriptedAI(c) {}

	void Reset() { }

	void CastVisual1()
	{
		if (Creature* visual_2 = me->FindNearestCreature(NPC_BUNNY_2, 100.0f, true))
		{
			DoCast(visual_2, CHANNEL_VISUAL, true);
		}
	}
};

CreatureAI* GetAI_helper_bunny_1(Creature* pCreature)
{
	return new helper_bunny_1AI(pCreature);
}

struct helper_bunny_2AI : public ScriptedAI
{
	helper_bunny_2AI(Creature *c) : ScriptedAI(c) {}

	void Reset() { }

	void CastVisual2()
	{
		if (Creature* visual_3 = me->FindNearestCreature(NPC_BUNNY_3, 100.0f, true))
		{
			DoCast(visual_3, CHANNEL_VISUAL, true);
		}
	}
};

CreatureAI* GetAI_helper_bunny_2(Creature* pCreature)
{
	return new helper_bunny_2AI(pCreature);
}

struct helper_bunny_3AI : public ScriptedAI
{
	helper_bunny_3AI(Creature *c) : ScriptedAI(c) {}

	void Reset() { }

	void CastVisual3()
	{
		if (Creature* visual_4 = me->FindNearestCreature(NPC_BUNNY_4, 100.0f, true))
		{
			DoCast(visual_4, CHANNEL_VISUAL, true);
		}
	}
};

CreatureAI* GetAI_helper_bunny_3(Creature* pCreature)
{
	return new helper_bunny_3AI(pCreature);
}

/*######
## skulloc_soulgrinder
######*/

struct skulloc_soulgrinderAI : public ScriptedAI
{
	skulloc_soulgrinderAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		me->SetReactState(REACT_AGGRESSIVE);
		me->setFaction(35);

		DoCast(me, 39947);

		//Event
		EventStart = true;
		Yelled = false;

		summoned_spirit1 = false;
		summoned_spirit2 = false;
		summoned_spirit3 = false;
		summoned_spirit4 = false;
		summoned_spirit5 = false;
		summoned_spirit6 = false;
		summoned_spirit7 = false;
		summoned_spirit8 = false;
		summoned_spirit9 = false;
		summoned_spirit10 = false;
		summoned_spirit11 = false;
		summoned_spirit12 = false;
		summoned_spirit13 = false;
		summoned_spirit14 = false;
		summoned_spirit15 = false;
		summoned_spirit16 = false;
		summoned_spirit17 = false;
		summoned_spirit18 = false;
		summoned_spirit19 = false;
		summoned_spirit20 = false;

		//Skills
		banish_timer = 15000;
		corruption_timer = 5000;
		help_timer = 45000;

		Summoned_Help = false;
	}

	//Event
	bool EventStart;
	bool Yelled;

	bool summoned_spirit1;
	bool summoned_spirit2;
	bool summoned_spirit3;
	bool summoned_spirit4;
	bool summoned_spirit5;
	bool summoned_spirit6;
	bool summoned_spirit7;
	bool summoned_spirit8;
	bool summoned_spirit9;
	bool summoned_spirit10;
	bool summoned_spirit11;
	bool summoned_spirit12;
	bool summoned_spirit13;
	bool summoned_spirit14;
	bool summoned_spirit15;
	bool summoned_spirit16;
	bool summoned_spirit17;
	bool summoned_spirit18;
	bool summoned_spirit19;
	bool summoned_spirit20;

	uint32 skulloc_attack_timer;

	uint32 summon_timer;
	uint32 summon_timer2;
	uint32 summon_timer3;
	uint32 summon_timer4;
	uint32 summon_timer5;
	uint32 summon_timer6;
	uint32 summon_timer7;
	uint32 summon_timer8;
	uint32 summon_timer9;
	uint32 summon_timer10;
	uint32 summon_timer11;
	uint32 summon_timer12;
	uint32 summon_timer13;
	uint32 summon_timer14;
	uint32 summon_timer15;
	uint32 summon_timer16;
	uint32 summon_timer17;
	uint32 summon_timer18;
	uint32 summon_timer19;

	//Skills
	uint32 banish_timer;
	uint32 corruption_timer;
	uint32 help_timer;

	bool Summoned_Help;

	void JustSummoned(Creature* pSummoned)
	{
		if (Creature* soulgrinder = me->FindNearestCreature(23019, 100.0f, true))
		{
			pSummoned->AI()->AttackStart(soulgrinder);
		}
	}

	void JustDied(Unit* /*killer*/)
	{
		if (Creature* soulgrinder = me->FindNearestCreature(23019, 200.0f, true))
		{
			soulgrinder->DisappearAndDie();
		}

		DoCast(39909);
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (!summoned_spirit1 && EventStart == true)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_1, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer = 10000;
				summoned_spirit1 = true;
			}

			if (!summoned_spirit2 && summoned_spirit1 == true && summon_timer <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_2, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer2 = 10000;
				summoned_spirit2 = true;
			}
			else summon_timer -= diff;

			if (!summoned_spirit3 && summoned_spirit2 == true && summon_timer2 <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_3, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer3 = 10000;
				summoned_spirit3 = true;
			}
			else summon_timer2 -= diff;

			if (!summoned_spirit4 && summoned_spirit3 == true && summon_timer3 <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_1, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer4 = 10000;
				summoned_spirit4 = true;
			}
			else summon_timer3 -= diff;

			if (!summoned_spirit5 && summoned_spirit4 == true && summon_timer4 <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_2, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer5 = 10000;
				summoned_spirit5 = true;	

				if (Creature* soulgrinder = me->FindNearestCreature(23019, 100.0f, true))
				{
					CAST_AI(the_soulgrinderAI, soulgrinder->AI())->CastVisual();
				}
			}
			else summon_timer4 -= diff;

			if (!summoned_spirit6 && summoned_spirit5 == true && summon_timer5 <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_3, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer6 = 10000;
				summoned_spirit6 = true;
			}
			else summon_timer5 -= diff;

			if (!summoned_spirit7 && summoned_spirit6 == true && summon_timer6 <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_1, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer7 = 10000;
				summoned_spirit7 = true;
			}
			else summon_timer6 -= diff;

			if (!summoned_spirit8 && summoned_spirit7 == true && summon_timer7 <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_2, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer8 = 10000;
				summoned_spirit8 = true;
			}
			else summon_timer7 -= diff;

			if (!summoned_spirit9 && summoned_spirit8 == true && summon_timer8 <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_3, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer9 = 10000;
				summoned_spirit9 = true;
			}
			else summon_timer8 -= diff;

			if (!summoned_spirit10 && summoned_spirit9 == true && summon_timer9 <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_1, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer10 = 10000;
				summoned_spirit10 = true;

				if (Creature* helper = me->FindNearestCreature(61009, 100.0f, true))
				{
					CAST_AI(helper_bunny_1AI, helper->AI())->CastVisual1();
				}
			}
			else summon_timer9 -= diff;

			if (!summoned_spirit11 && summoned_spirit10 == true && summon_timer10 <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_2, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer11 = 10000;
				summoned_spirit11 = true;
			}
			else summon_timer10 -= diff;

			if (!summoned_spirit12 && summoned_spirit11 == true && summon_timer11 <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_3, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer12 = 10000;
				summoned_spirit12 = true;
			}
			else summon_timer11 -= diff;

			if (!summoned_spirit13 && summoned_spirit12 == true && summon_timer12 <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_1, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer13 = 10000;
				summoned_spirit13 = true;
			}
			else summon_timer12 -= diff;

			if (!summoned_spirit14 && summoned_spirit13 == true && summon_timer13 <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_2, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer14 = 10000;
				summoned_spirit14 = true;
			}
			else summon_timer13 -= diff;

			if (!summoned_spirit15 && summoned_spirit14 == true && summon_timer14 <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_3, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer15 = 10000;
				summoned_spirit15 = true;

				if (Creature* helper2 = me->FindNearestCreature(61010, 100.0f, true))
				{
					CAST_AI(helper_bunny_2AI, helper2->AI())->CastVisual2();
				}
			}
			else summon_timer14 -= diff;

			if (!summoned_spirit16 && summoned_spirit15 == true && summon_timer15 <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_1, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer16 = 10000;
				summoned_spirit16 = true;
			}
			else summon_timer15 -= diff;

			if (!summoned_spirit17 && summoned_spirit16 == true && summon_timer16 <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_2, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer17 = 10000;
				summoned_spirit17 = true;
			}
			else summon_timer16 -= diff;

			if (!summoned_spirit18 && summoned_spirit17 == true && summon_timer17 <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_3, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer18 = 10000;
				summoned_spirit18 = true;
			}
			else summon_timer17 -= diff;

			if (!summoned_spirit19 && summoned_spirit18 == true && summon_timer18 <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_1, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				summon_timer19 = 10000;
				summoned_spirit19 = true;
			}
			else summon_timer18 -= diff;

			if (!summoned_spirit20 && summoned_spirit19 == true && summon_timer19 <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_2, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 45000);
				skulloc_attack_timer = 8000;
				summoned_spirit20 = true;

				if (Creature* helper3 = me->FindNearestCreature(61011, 100.0f, true))
				{
					CAST_AI(helper_bunny_3AI, helper3->AI())->CastVisual3();
				}
			}
			else summon_timer19 -= diff;

			if (!Yelled && skulloc_attack_timer <= diff && summoned_spirit20 == true)
			{
				me->setFaction(14);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				me->Yell(YELL_SKULLOC_AGGRO, LANG_UNIVERSAL, 0);
				Yelled = true;
			}
			else skulloc_attack_timer -= diff;
		}

		if (UpdateVictim())
		{
			if (banish_timer <= diff)
			{
				if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 1))
				DoCast(target, SPELL_BANISH);
				banish_timer = 25000;
			}
			else banish_timer -= diff;

			if (corruption_timer <= diff)
			{
				if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0))
				DoCast(target, SPELL_CORRUPTION);
				corruption_timer = 12000;
			}
			else corruption_timer -= diff;

			if (!Summoned_Help && help_timer <= diff)
			{
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_1, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_2, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_3, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_1, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_2, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);
				me->SummonCreature(NPC_SUNDERED_SPIRIT, SUMMON_LOC_3, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);

				Summoned_Help = true;
			}
			else help_timer -= diff;
		}
		
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_skulloc_soulgrinder(Creature* pCreature)
{
	return new skulloc_soulgrinderAI(pCreature);
}

enum ApexisRelic
{
	NPC_SIMON_BUNNY = 22923,
	NPC_APEXIS_GUARDIAN = 22275,

	GO_APEXIS_MONUMENT = 185944,

	QUEST_CRYSTALS = 11058,
	QUEST_EMNATION = 11080,
	GOSSIP_TEXT_ID = 10948,

	ITEM_APEXIS_SHARD = 32569,
};

#define GOSSIP_S_GUARDIAN   "Insert 35 Apexis Shards, and begin!"
#define GOSSIP_APEXIS   "Insert an Apexis Shard, and begin!"

bool GossipHello_go_apexis_monument(Player *player, GameObject* _GO)
{	
	player->ADD_GOSSIP_ITEM(0, GOSSIP_S_GUARDIAN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

	player->SEND_GOSSIP_MENU(10949, _GO->GetGUID());
	return true;
}

void SendActionMenu_go_apexis_monument(Player *player, GameObject* go, uint32 action)
{
	switch (action)
	{
	case GOSSIP_ACTION_INFO_DEF + 1:
		if (player->HasItemCount(ITEM_APEXIS_SHARD, 35))
		{
			player->DestroyItemCount(ITEM_APEXIS_SHARD, 35, true);
			player->SummonCreature(NPC_APEXIS_GUARDIAN, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
		}
		break;
	}
}

bool GossipSelect_go_apexis_monument(Player *player, GameObject* _GO, uint32 sender, uint32 action)
{
	switch (sender)
	{
	case GOSSIP_SENDER_MAIN:    SendActionMenu_go_apexis_monument(player, _GO, action); break;
	}
	return true;
}

bool GossipHello_go_apexis_relic(Player *player, GameObject* _GO)
{
	player->ADD_GOSSIP_ITEM(0, GOSSIP_APEXIS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

	player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID, _GO->GetGUID());
	return true;
}

void SendActionMenu_go_apexis_relic(Player *player, GameObject* go, uint32 action)
{
	switch (action)
	{
	case GOSSIP_ACTION_INFO_DEF + 1:
		if (player->HasItemCount(ITEM_APEXIS_SHARD, 1))
		{
			player->DestroyItemCount(ITEM_APEXIS_SHARD, 1, true);
			player->CompleteQuest(QUEST_CRYSTALS);
			player->CompleteQuest(QUEST_EMNATION);
		}
		break;
	}
}

bool GossipSelect_go_apexis_relic(Player *player, GameObject* _GO, uint32 sender, uint32 action)
{
	switch (sender)
	{
	case GOSSIP_SENDER_MAIN:    SendActionMenu_go_apexis_relic(player, _GO, action); break;
	}
	return true;
}

/*######
## QUEST_A_FEL_WHIP_FOR_GAZH
######*/

#define GOSSIP_PRISM_CRYSTAL "Place 35 Apexis Shards into the prison."
#define NPC_GALVANOTH 22281

bool GossipHello_go_fel_crystal_prism(Player *player, GameObject* _GO)
{
	player->ADD_GOSSIP_ITEM(0, GOSSIP_PRISM_CRYSTAL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

	player->SEND_GOSSIP_MENU(10951, _GO->GetGUID());
	return true;
}

void SendActionMenu_go_fel_crystal_prism(Player *player, GameObject* go, uint32 action)
{
	switch (action)
	{
	case GOSSIP_ACTION_INFO_DEF + 1:
		if (player->HasItemCount(ITEM_APEXIS_SHARD, 35))
		{
			player->DestroyItemCount(ITEM_APEXIS_SHARD, 35, true);
			player->SummonCreature(NPC_GALVANOTH, go->GetPositionX() -5, go->GetPositionY() -5, go->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 300000);
		}
		break;
	}
}

bool GossipSelect_go_fel_crystal_prism(Player *player, GameObject* _GO, uint32 sender, uint32 action)
{
	switch (sender)
	{
	case GOSSIP_SENDER_MAIN:    SendActionMenu_go_fel_crystal_prism(player, _GO, action); break;
	}
	return true;
}

/*######
## npc_aether_ray
######*/

struct npc_aether_rayAI : public ScriptedAI
{
	npc_aether_rayAI(Creature *c) : ScriptedAI(c) {}

	void Reset() 
	{
		spellHit = false;
		tailswipe_timer = 8000;
	}

	bool spellHit;
	uint32 tailswipe_timer;

	void SpellHit(Unit* Hitter, const SpellEntry* Spellkind)
	{
		if (Spellkind->Id == 40856 && !spellHit)
		{
			if (Player *plWho = Hitter->GetCharmerOrOwnerPlayerOrPlayerItself())
			{
				if (Creature* pCredit = me->FindNearestCreature(22181, 15, true))
					plWho->KilledMonsterCredit(22181, pCredit->GetGUID());
				me->CastSpell(plWho, 40917, true);
				me->ForcedDespawn(2000);
				spellHit = true;			
			}
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (tailswipe_timer <= diff)
		{
			DoCastVictim(35333);
			tailswipe_timer = 9000;
		}
		else tailswipe_timer -= diff;
	}
};

CreatureAI* GetAI_npc_aether_ray(Creature* pCreature)
{
	return new npc_aether_rayAI(pCreature);
}

struct npc_felburn_triggerAI : public ScriptedAI
{
	npc_felburn_triggerAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		spellHit = false;
		me->SetCanFly(true);
		me->canFly();
	}

	bool spellHit;

	void SpellHit(Unit* Hitter, const SpellEntry* Spellkind)
	{
		if (Spellkind->Id == 40672 && !spellHit)
		{						
			DoCast(41603);
			me->ForcedDespawn(10000);
			spellHit = true;
		}
	}
};

CreatureAI* GetAI_npc_felburn_trigger(Creature* pCreature)
{
	return new npc_felburn_triggerAI(pCreature);
}

#define NPC_FELBURN_TRIGGER 61013

struct npc_legion_flak_cannonAI : public ScriptedAI
{
	npc_legion_flak_cannonAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		summoned = false;
		Blaze_Timer = 10000;
		reset_timer = 0;

		me->SetUnitMovementFlags(MOVEMENTFLAG_ROOT);
	}

	bool summoned;
	uint32 Blaze_Timer;
	uint32 reset_timer;

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetDistance(me) < 45.0f)
			{
				switch (me->GetEntry())
				{
				case 23076:		
					if (!summoned)
					{
						float x, y, z;
						plWho->GetPosition(x, y, z);
						Creature* summon = me->SummonCreature(NPC_FELBURN_TRIGGER, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN, 10000);
						reset_timer = 5000;
					}
				break;
				}
			}
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{		
			if (Creature* felburn = me->FindNearestCreature(NPC_FELBURN_TRIGGER, 45.0f, true))
			{
				DoCast(felburn, 40672);
			}

			if (reset_timer <= diff)
			{
				Reset();
			}
			else reset_timer -= diff;
		}

		if (UpdateVictim())
		{
			if (Blaze_Timer <= diff)
			{
				if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0))
				{
					float x, y, z;
					target->GetPosition(x, y, z);
					Creature* summon = me->SummonCreature(NPC_FELBURN_TRIGGER, x, y, z, 0, TEMPSUMMON_MANUAL_DESPAWN, 0);
				}
				Blaze_Timer = 10000;
			}
			else
				Blaze_Timer -= diff;
		}
	}	
};

CreatureAI* GetAI_npc_legion_flak_cannon(Creature* pCreature)
{
	return new npc_legion_flak_cannonAI(pCreature);
}

struct npc_abyssal_flamebringerAI : public ScriptedAI
{
	npc_abyssal_flamebringerAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);

		abyssal_timer = 8000;
		immo_timer = 2000;

		// Poisons 
		me->ApplySpellImmune(0, IMMUNITY_ID, 27282, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 26892, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 26786, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 27283, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 26969, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 25347, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11343, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 13230, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11358, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11400, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11342, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 3421, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 13229, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11357, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11341, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 13228, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8694, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 2837, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8691, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 13220, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 2835, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8687, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 5763, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8681, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 3420, true);
	}

	uint32 abyssal_timer;
	uint32 immo_timer;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (abyssal_timer <= diff)
		{
			DoCastVictim(37633);
			abyssal_timer = 13000;
		}
		else abyssal_timer -= diff;

		if (immo_timer <= diff)
		{
			DoCastVictim(38999);
			immo_timer = 2000;
		}
		else immo_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_abyssal_flamebringer(Creature* pCreature)
{
	return new npc_abyssal_flamebringerAI(pCreature);
}

struct npc_vilefire_soulAI : public ScriptedAI
{
	npc_vilefire_soulAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);

		fireball_timer = 6000;
	}

	uint32 fireball_timer;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (fireball_timer <= diff)
		{
			DoCastVictim(9053);
			fireball_timer = 8000;
		}
		else fireball_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_vilefire_soul(Creature* pCreature)
{
	return new npc_vilefire_soulAI(pCreature);
}

struct npc_searing_elementalAI : public ScriptedAI
{
	npc_searing_elementalAI(Creature *c) : ScriptedAI(c) {}

	void Reset()
	{
		me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);		
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_searing_elemental(Creature* pCreature)
{
	return new npc_searing_elementalAI(pCreature);
}

// make it work -.-

#define QUEST_THE_STONES_OF_VEKHNIR 10565

struct npc_veknir_triggerAI : public ScriptedAI 
{
	npc_veknir_triggerAI(Creature *c) : ScriptedAI(c) {}

	void Reset() { }

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(QUEST_THE_STONES_OF_VEKHNIR) == QUEST_STATUS_INCOMPLETE && plWho->HasItemCount(30561, 1, false) && plWho->GetDistance(me) < 5.0f)
			{
				switch (me->GetEntry())
				{
				case 61015:
					plWho->AddItem(30567, 1);
					break;
				}
			}
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;
	}
};

CreatureAI* GetAI_npc_veknir_trigger(Creature* pCreature)
{
	return new npc_veknir_triggerAI(pCreature);
}

void AddSC_blades_edge_mountains()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "mobs_nether_drake";
    newscript->GetAI = &GetAI_mobs_nether_drake;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_daranelle";
    newscript->GetAI = &GetAI_npc_daranelle;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_overseer_nuaar";
    newscript->pGossipHello = &GossipHello_npc_overseer_nuaar;
    newscript->pGossipSelect = &GossipSelect_npc_overseer_nuaar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_saikkal_the_elder";
    newscript->pGossipHello = &GossipHello_npc_saikkal_the_elder;
    newscript->pGossipSelect = &GossipSelect_npc_saikkal_the_elder;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_bloodmaul_brutebane";
    newscript->GetAI = &GetAI_npc_bloodmaul_brutebane;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_ogre_brute";
    newscript->GetAI = &GetAI_npc_ogre_brute;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_thunderspike";
    newscript->pGOHello = &GOUse_go_thunderspike;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_light_orb_collector";
    newscript->GetAI = &GetAI_npc_light_orb_collector;
    newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_anger_camp";
	newscript->GetAI = &GetAI_npc_anger_camp;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "go_obeliks";
	newscript->pGOHello = &GOUse_go_obeliks;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_bloodmaul_direwolf";
	newscript->GetAI = &GetAI_npc_bloodmaul_direwolf;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_kolphis_darkscale";
	newscript->pGossipHello = &GossipHello_npc_kolphis_darkscale;
	newscript->pGossipSelect = &GossipSelect_npc_kolphis_darkscale;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_prophecy_trigger";
	newscript->GetAI = &GetAI_npc_prophecy_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "go_prophecy";
	newscript->pGOHello = &GOUse_go_prophecy;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_grishna_arakkoa";
	newscript->GetAI = &GetAI_npc_grishna_arakkoa;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_koikoi_spirit";
	newscript->GetAI = &GetAI_npc_koikoi_spirit;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_raven_leafbeard";
	newscript->GetAI = &GetAI_npc_raven_leafbeard;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_wyrmkult_zealot";
	newscript->GetAI = &GetAI_npc_wyrmkult_zealot;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_wyrmkult_acolyte";
	newscript->GetAI = &GetAI_npc_wyrmkult_acolyte;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_evergrove_ancient";
	newscript->GetAI = &GetAI_npc_evergrove_ancient;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_baelmon";
	newscript->GetAI = &GetAI_npc_baelmon;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_groc_trigger";
	newscript->GetAI = &GetAI_npc_groc_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_goc";
	newscript->GetAI = &GetAI_npc_goc;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_sabelian_human";
	newscript->GetAI = &GetAI_npc_sabelian_human;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_sabelian_dragon";
	newscript->GetAI = &GetAI_npc_sabelian_dragon;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_rexxar";
	newscript->GetAI = &GetAI_npc_rexxar;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_thunderstrike_trigger";
	newscript->GetAI = &GetAI_npc_thunderstrike_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_grimgut";
	newscript->GetAI = &GetAI_npc_grimgut;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_onspiritswings_trigger";
	newscript->GetAI = &GetAI_npc_onspiritswings_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "go_moonshine_keg";
	newscript->pGOHello = &GOUse_go_moonshine_keg;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "go_greenspot_keg";
	newscript->pGOHello = &GOUse_go_greenspot_keg;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "go_fermentedseed_keg";
	newscript->pGOHello = &GOUse_go_fermentedseed_keg;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_huffer";
	newscript->GetAI = &GetAI_npc_huffer;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "go_power_converter";
	newscript->pGOHello = &GOUse_go_power_converter;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "go_netherdrake_egg";
	newscript->pGOHello = &GOUse_go_netherdrake_egg;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_nexusprince_razaan";
	newscript->GetAI = &GetAI_npc_nexusprince_razaan;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_summon_circle_trigger";
	newscript->GetAI = &GetAI_npc_summon_circle_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_vimgol_circle_trigger";
	newscript->GetAI = &GetAI_npc_vimgol_circle_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_vimgol_the_vile";
	newscript->GetAI = &GetAI_npc_vimgol_the_vile;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "the_soulgrinder";
	newscript->GetAI = &GetAI_the_soulgrinder;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "skulloc_soulgrinder";
	newscript->GetAI = &GetAI_skulloc_soulgrinder;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "helper_bunny_1";
	newscript->GetAI = &GetAI_helper_bunny_1;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "helper_bunny_2";
	newscript->GetAI = &GetAI_helper_bunny_2;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "helper_bunny_3";
	newscript->GetAI = &GetAI_helper_bunny_3;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "go_apexis_monument";
	newscript->pGOHello = &GossipHello_go_apexis_monument;
	newscript->pGOSelect = &GossipSelect_go_apexis_monument;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "go_apexis_relic";
	newscript->pGOHello = &GossipHello_go_apexis_relic;
	newscript->pGOSelect = &GossipSelect_go_apexis_relic;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "go_fel_crystal_prism";
	newscript->pGOHello = &GossipHello_go_fel_crystal_prism;
	newscript->pGOSelect = &GossipSelect_go_fel_crystal_prism;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_aether_ray";
	newscript->GetAI = &GetAI_npc_aether_ray;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_felburn_trigger";
	newscript->GetAI = &GetAI_npc_felburn_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_legion_flak_cannon";
	newscript->GetAI = &GetAI_npc_legion_flak_cannon;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_abyssal_flamebringer";
	newscript->GetAI = &GetAI_npc_abyssal_flamebringer;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_vilefire_soul";
	newscript->GetAI = &GetAI_npc_vilefire_soul;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_searing_elemental";
	newscript->GetAI = &GetAI_npc_searing_elemental;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_veknir_trigger";
	newscript->GetAI = &GetAI_npc_veknir_trigger;
	newscript->RegisterSelf();
}
