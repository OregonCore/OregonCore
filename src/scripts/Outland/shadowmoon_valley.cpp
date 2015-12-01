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
SDName: Shadowmoon_Valley
SD%Complete: 98
SDComment: Quest support: 10519, 10583, 10601, 10814, 10804, 10854, 10458, 10481, 10480, 11082, 10781, 10451. Vendor Drake Dealer Hurlunk, 10563, 10596.
SDCategory: Shadowmoon Valley
EndScriptData */

/* ContentData
mob_mature_netherwing_drake
mob_enslaved_netherwing_drake
npc_drake_dealer_hurlunk
npcs_flanis_swiftwing_and_kagrosh
npc_murkblood_overseer
npc_neltharaku
npc_karynaku
npc_oronok_tornheart
npc_overlord_morghor
npc_earthmender_wilda
mob_torloth_the_magnificent
mob_illidari_spawn
npc_lord_illidan_stormrage
go_crystal_prison
npc_enraged_spirit
npc_jovaan
npc_azaloth
npc_sunfurywarlock
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"

/*#####
# mob_mature_netherwing_drake
#####*/

enum eDrake
{
    SPELL_PLACE_CARCASS             = 38439,
    SPELL_JUST_EATEN                = 38502,
    SPELL_NETHER_BREATH             = 38467,

    SAY_JUST_EATEN                  = -1000175
};

struct mob_mature_netherwing_drakeAI : public ScriptedAI
{
    mob_mature_netherwing_drakeAI(Creature* c) : ScriptedAI(c)
    {
        PlayerGUID = 0;
    }

    uint64 PlayerGUID;

    bool IsEating;
    bool Evade;

    uint32 ResetTimer;
    uint32 CastTimer;
    uint32 EatTimer;

    void Reset()
    {
        IsEating = false;
        Evade = false;

        ResetTimer = 120000;
        EatTimer = 5000;
        CastTimer = 5000;
    }

    void EnterCombat(Unit*) { }

    void MoveInLineOfSight(Unit* who)
    {
        if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == POINT_MOTION_TYPE)
            return;

        ScriptedAI::MoveInLineOfSight(who);
    }

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (!caster)
            return;

        if (caster->GetTypeId() == TYPEID_PLAYER && spell->Id == SPELL_PLACE_CARCASS && !me->HasAura(SPELL_JUST_EATEN, 0) && !PlayerGUID)
        {
            float PlayerX, PlayerY, PlayerZ;
            caster->GetClosePoint(PlayerX, PlayerY, PlayerZ, me->GetObjectSize());
            me->GetMotionMaster()->MovePoint(1, PlayerX, PlayerY, PlayerZ);
            PlayerGUID = caster->GetGUID();
        }
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (id == 1)
        {
            IsEating = true;
            EatTimer = 5000;
            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_ATTACKUNARMED);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (IsEating)
        {
            if (EatTimer <= diff)
            {
                IsEating = false;
                DoCast(me, SPELL_JUST_EATEN);
                me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                DoScriptText(SAY_JUST_EATEN, me);
                if (PlayerGUID)
                {
                    Player* plr = Unit::GetPlayer(*me, PlayerGUID);
                    if (plr && plr->GetQuestStatus(10804) == QUEST_STATUS_INCOMPLETE)
                    {
                        plr->KilledMonsterCredit(22131, 0);
                        Evade = true;
                        PlayerGUID = 0;
                    }
                }
            }
            else EatTimer -= diff;
        }

        if (Evade)
        {
            if (ResetTimer <= diff)
            {
                EnterEvadeMode();
                return;
            }
            else ResetTimer -= diff;
        }

        if (!UpdateVictim())
            return;

        if (CastTimer <= diff)
        {
            DoCastVictim( SPELL_NETHER_BREATH);
            CastTimer = 5000;
        }
        else CastTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_mature_netherwing_drake(Creature* pCreature)
{
    return new mob_mature_netherwing_drakeAI(pCreature);
}

/*###
# mob_enslaved_netherwing_drake
####*/

enum eDrak
{
    FACTION_DEFAULT     = 62,
    FACTION_FRIENDLY    = 1840,                           // Not sure if this is correct, it was taken off of Mordenai.

    SPELL_HIT_FORCE_OF_NELTHARAKU   = 38762,
    SPELL_FORCE_OF_NELTHARAKU       = 38775,

    CREATURE_DRAGONMAW_SUBJUGATOR   = 21718,
    CREATURE_ESCAPE_DUMMY           = 22317
};

struct mob_enslaved_netherwing_drakeAI : public ScriptedAI
{
    mob_enslaved_netherwing_drakeAI(Creature* c) : ScriptedAI(c)
    {
        PlayerGUID = 0;
        Tapped = false;
    }

    uint64 PlayerGUID;
    uint32 FlyTimer;
    bool Tapped;

    void Reset()
    {
        if (!Tapped)
            me->setFaction(FACTION_DEFAULT);

        FlyTimer = 10000;
        me->SetLevitate(false);
        me->SetVisible(true);
    }

    void EnterCombat(Unit*) { }

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (!caster)
            return;

        if (caster->GetTypeId() == TYPEID_PLAYER && spell->Id == SPELL_HIT_FORCE_OF_NELTHARAKU && !Tapped)
        {
            Tapped = true;
            PlayerGUID = caster->GetGUID();

            me->setFaction(FACTION_FRIENDLY);
            DoCast(caster, SPELL_FORCE_OF_NELTHARAKU, true);

            Unit* Dragonmaw = me->FindNearestCreature(CREATURE_DRAGONMAW_SUBJUGATOR, 50);

            if (Dragonmaw)
            {
                me->AddThreat(Dragonmaw, 100000.0f);
                AttackStart(Dragonmaw);
            }

            HostileReference* ref = me->getThreatManager().getOnlineContainer().getReferenceByTarget(caster);
            if (ref)
                ref->removeReference();
        }
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (id == 1)
        {
            if (PlayerGUID)
            {
                Unit* plr = Unit::GetUnit((*me), PlayerGUID);
                if (plr)
                    DoCast(plr, SPELL_FORCE_OF_NELTHARAKU, true);

                PlayerGUID = 0;
            }
            me->SetVisible(false);
            me->SetLevitate(false);
            me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            me->RemoveCorpse();
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
        {
            if (Tapped)
            {
                if (FlyTimer <= diff)
                {
                    Tapped = false;
                    if (PlayerGUID)
                    {
                        Player* plr = Unit::GetPlayer(*me, PlayerGUID);
                        if (plr && plr->GetQuestStatus(10854) == QUEST_STATUS_INCOMPLETE)
                        {
                            plr->KilledMonsterCredit(22316, 0);
                            /*
                            float x,y,z;
                            me->GetPosition(x,y,z);

                            float dx,dy,dz;
                            me->GetRandomPoint(x, y, z, 20, dx, dy, dz);
                            dz += 20; // so it's in the air, not ground*/

                            Position pos;
                            if (Unit* EscapeDummy = me->FindNearestCreature(CREATURE_ESCAPE_DUMMY, 30))
                                EscapeDummy->GetPosition(&pos);
                            else
                            {
                                me->GetRandomNearPosition(pos, 20);
                                pos.m_positionZ += 25;
                            }

                            me->SetLevitate(true);
                            me->GetMotionMaster()->MovePoint(1, pos);
                        }
                    }
                }
                else FlyTimer -= diff;
            }
            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_enslaved_netherwing_drake(Creature* pCreature)
{
    return new mob_enslaved_netherwing_drakeAI(pCreature);
}

/*#####
# mob_dragonmaw_peon
#####*/

struct mob_dragonmaw_peonAI : public ScriptedAI
{
    mob_dragonmaw_peonAI(Creature* c) : ScriptedAI(c) {}

    uint64 PlayerGUID;
    bool Tapped;
    uint32 PoisonTimer;
	uint32 kick_timer;
	uint32 sunder_timer;

    void Reset()
    {
		kick_timer = 12000;
		sunder_timer = 10000;
        PlayerGUID = 0;
        Tapped = false;
        PoisonTimer = 0;
    }

    void EnterCombat(Unit*) { }

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (!caster)
            return;

        if (caster->GetTypeId() == TYPEID_PLAYER && spell->Id == 40468 && !Tapped)
        {
            PlayerGUID = caster->GetGUID();

            Tapped = true;
            float x, y, z;
            caster->GetClosePoint(x, y, z, me->GetObjectSize());

            me->SetWalk(false);
            me->GetMotionMaster()->MovePoint(1, x, y, z);
        }
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE)
            return;

        if (id)
        {
            me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_EAT);
            PoisonTimer = 15000;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (PoisonTimer)
        {
            if (PoisonTimer <= diff)
            {
                if (PlayerGUID)
                {
                    Player* plr = Unit::GetPlayer(*me, PlayerGUID);
                    if (plr && plr->GetQuestStatus(11020) == QUEST_STATUS_INCOMPLETE)
                        plr->KilledMonsterCredit(23209, 0);
                }
                PoisonTimer = 0;
                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
            else PoisonTimer -= diff;
        }

		if (UpdateVictim())
		{
			if (kick_timer <= diff)
			{
				DoCastVictim(34802);
				kick_timer = 12000;
			}
			else kick_timer -= diff;

			if (sunder_timer <= diff)
			{
				DoCastVictim(15572);
				sunder_timer = 10000;
			}
			else sunder_timer -= diff;
		}
    }
};

CreatureAI* GetAI_mob_dragonmaw_peon(Creature* pCreature)
{
    return new mob_dragonmaw_peonAI(pCreature);
}

/*######
## npc_drake_dealer_hurlunk
######*/

bool GossipHello_npc_drake_dealer_hurlunk(Player* player, Creature* pCreature)
{
    if (pCreature->isVendor() && player->GetReputationRank(1015) == REP_EXALTED)
        player->ADD_GOSSIP_ITEM(1, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_drake_dealer_hurlunk(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_TRADE)
        player->SEND_VENDORLIST(pCreature->GetGUID());

    return true;
}

/*######
## npc_flanis_swiftwing_and_kagrosh
######*/

#define GOSSIP_HSK1 "Take Flanis's Pack"
#define GOSSIP_HSK2 "Take Kagrosh's Pack"

bool GossipHello_npcs_flanis_swiftwing_and_kagrosh(Player* player, Creature* pCreature)
{
    if (player->GetQuestStatus(10583) == QUEST_STATUS_INCOMPLETE && !player->HasItemCount(30658, 1, true))
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HSK1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    if (player->GetQuestStatus(10601) == QUEST_STATUS_INCOMPLETE && !player->HasItemCount(30659, 1, true))
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HSK2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npcs_flanis_swiftwing_and_kagrosh(Player* player, Creature* /*pCreature*/, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 30658, 1, NULL);
        if (msg == EQUIP_ERR_OK)
        {
            player->StoreNewItem(dest, 30658, 1, true);
            player->PlayerTalkClass->ClearMenus();
        }
    }
    if (action == GOSSIP_ACTION_INFO_DEF + 2)
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 30659, 1, NULL);
        if (msg == EQUIP_ERR_OK)
        {
            player->StoreNewItem(dest, 30659, 1, true);
            player->PlayerTalkClass->ClearMenus();
        }
    }
    return true;
}

/*######
## npc_murkblood_overseer
######*/

#define QUEST_11082     11082

#define GOSSIP_HMO "I am here for you, overseer."
#define GOSSIP_SMO1 "How dare you question an overseer of the Dragonmaw!"
#define GOSSIP_SMO2 "Who speaks of me? What are you talking about, broken?"
#define GOSSIP_SMO3 "Continue please."
#define GOSSIP_SMO4 "Who are these bidders?"
#define GOSSIP_SMO5 "Well... yes."

bool GossipHello_npc_murkblood_overseer(Player* player, Creature* pCreature)
{
    if (player->GetQuestStatus(QUEST_11082) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HMO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SEND_GOSSIP_MENU(10940, pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_murkblood_overseer(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF+1:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SMO1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        //correct id not known
        player->SEND_GOSSIP_MENU(10940, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SMO2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        //correct id not known
        player->SEND_GOSSIP_MENU(10940, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+3:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SMO3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
        //correct id not known
        player->SEND_GOSSIP_MENU(10940, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+4:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SMO4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
        //correct id not known
        player->SEND_GOSSIP_MENU(10940, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+5:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SMO5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
        //correct id not known
        player->SEND_GOSSIP_MENU(10940, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+6:
        //correct id not known
        player->SEND_GOSSIP_MENU(10940, pCreature->GetGUID());
        pCreature->CastSpell(player, 41121, false);
        player->AreaExploredOrEventHappens(QUEST_11082);
        break;
    }
    return true;
}

/*######
## npc_neltharaku
######*/

#define GOSSIP_HN "I am listening, dragon"
#define GOSSIP_SN1 "But you are dragons! How could orcs do this to you?"
#define GOSSIP_SN2 "Your mate?"
#define GOSSIP_SN3 "I have battled many beasts, dragon. I will help you."

bool GossipHello_npc_neltharaku(Player* player, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        player->PrepareQuestMenu(pCreature->GetGUID());

    if (player->GetQuestStatus(10814) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SEND_GOSSIP_MENU(10613, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_neltharaku(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF+1:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SN1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(10614, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SN2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->SEND_GOSSIP_MENU(10615, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+3:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SN3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->SEND_GOSSIP_MENU(10616, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+4:
        player->CLOSE_GOSSIP_MENU();
        player->AreaExploredOrEventHappens(10814);
        break;
    }
    return true;
}

/*######
## npc_oronok
######*/

#define GOSSIP_ORONOK1 "I am ready to hear your story, Oronok."
#define GOSSIP_ORONOK2 "How do I find the cipher?"
#define GOSSIP_ORONOK3 "How do you know all of this?"
#define GOSSIP_ORONOK4 "Yet what? What is it, Oronok?"
#define GOSSIP_ORONOK5 "Continue, please."
#define GOSSIP_ORONOK6 "So what of the cipher now? And your boys?"
#define GOSSIP_ORONOK7 "I will find your boys and the cipher, Oronok."
#define ORONOK_SHEEP "You're not getting my sheep, scoundrel!"

struct npc_oronok_tornheartAI : public ScriptedAI
{
	npc_oronok_tornheartAI(Creature* c) : ScriptedAI(c) {}

	bool Sheep;
	uint32 sheep_timer;

	void Reset()
	{
		Sheep = false;
		sheep_timer = 120000;
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (!Sheep && sheep_timer <= diff)
			{
				me->MonsterSay(ORONOK_SHEEP, LANG_UNIVERSAL, 0);
				me->AI()->EnterEvadeMode();
				Sheep = true;
			}
			else sheep_timer -= diff;
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_oronok_tornheart(Creature* pCreature)
{
	return new npc_oronok_tornheartAI(pCreature);
}

bool GossipHello_npc_oronok_tornheart(Player* player, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        player->PrepareQuestMenu(pCreature->GetGUID());
    if (pCreature->isVendor())
        player->ADD_GOSSIP_ITEM(1, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    if (player->GetQuestStatus(10519) == QUEST_STATUS_INCOMPLETE)
    {
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        player->SEND_GOSSIP_MENU(10312, pCreature->GetGUID());
    }
    else
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_oronok_tornheart(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_TRADE:
        player->SEND_VENDORLIST(pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(10313, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+1:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(10314, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->SEND_GOSSIP_MENU(10315, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+3:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->SEND_GOSSIP_MENU(10316, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+4:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->SEND_GOSSIP_MENU(10317, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+5:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->SEND_GOSSIP_MENU(10318, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+6:
        player->CLOSE_GOSSIP_MENU();
        player->AreaExploredOrEventHappens(10519);
        break;
    }
    return true;
}

/*####
# npc_karynaku
####*/

struct npc_karynakuAI : public ScriptedAI
{
	npc_karynakuAI(Creature* c) : ScriptedAI(c) {}

	void Reset()
	{
		ZuluStart = false;
	}

	bool ZuluStart;

	void JustSummoned(Creature* summoned)
	{
		summoned->CastSpell(summoned, 42510, true);
	}

	void UpdateAI(const uint32 diff)
	{
		if (ZuluStart)
		{
			//Summon Zuluhed the Whacked
			me->SummonCreature(11980, -4207.5f, 311.24f, 123.1f, 1.2f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 45000);
			ZuluStart = false;
		}
	}
};

CreatureAI* GetAI_npc_karynaku(Creature* pCreature)
{
	return new npc_karynakuAI(pCreature);
}

bool QuestAccept_npc_karynaku(Player* player, Creature* creature, Quest const* quest)
{
	if (quest->GetQuestId() == 10866)
	{
		CAST_AI(npc_karynakuAI, creature->AI())->ZuluStart = true;
		//Summon Archers, Left Wall
		creature->SummonCreature(22251, -4180.2f, 321.97f, 135.5f, 1.95f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 45000);
		creature->SummonCreature(22251, -4186.6f, 318.94f, 135.5f, 2.07f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 45000);
		creature->SummonCreature(22251, -4191.8f, 314.29f, 136.1f, 2.06f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 45000);
		//Main Gate
		creature->SummonCreature(22251, -4201.5f, 309.77f, 138.2f, 1.33f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 45000);
		creature->SummonCreature(22251, -4206.1f, 310.84f, 137.8f, 1.41f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 45000);
		creature->SummonCreature(22251, -4210.7f, 311.59f, 137.3f, 1.41f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 45000);
		//Right Wall
		creature->SummonCreature(22251, -4225.8f, 336.32f, 135.6f, 6.03f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 45000);
		creature->SummonCreature(22251, -4224.3f, 341.04f, 135.6f, 5.80f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 45000);		
	}

    if (quest->GetQuestId() == 10870)                        // Ally of the Netherwing
    {
        std::vector<uint32> nodes;

        nodes.resize(2);
        nodes[0] = 161;                                     // From Karynaku
        nodes[1] = 162;                                     // To Mordenai
        error_log("OSCR: Player %s started quest 10870 which has disabled taxi node, need to be fixed in core", player->GetName());
        //player->ActivateTaxiPathTo(nodes, 20811);
    }

    return true;
}

/*####
# npc_overlord_morghor
####*/

#define QUEST_LORD_ILLIDAN_STORMRAGE 11108

#define C_ILLIDAN 22083
#define C_YARZILL 23141

#define SPELL_ONE 39990 // Red Lightning Bolt
#define SPELL_TWO 41528 // Mark of Stormrage
#define SPELL_THREE 40216 // Dragonaw Faction
#define SPELL_FOUR 42016 // Dragonaw Trasform

#define OVERLORD_SAY_1 -1000206
#define OVERLORD_SAY_2 -1000207
#define OVERLORD_SAY_3 -1000208
#define OVERLORD_SAY_4 -1000209
#define OVERLORD_SAY_5 -1000210
#define OVERLORD_SAY_6 -1000211

#define OVERLORD_YELL_1 -1000212
#define OVERLORD_YELL_2 -1000213

#define LORD_ILLIDAN_SAY_1 -1000214
#define LORD_ILLIDAN_SAY_2 -1000215
#define LORD_ILLIDAN_SAY_3 -1000216
#define LORD_ILLIDAN_SAY_4 -1000217
#define LORD_ILLIDAN_SAY_5 -1000218
#define LORD_ILLIDAN_SAY_6 -1000219
#define LORD_ILLIDAN_SAY_7 -1000220

#define YARZILL_THE_MERC_SAY -1000221

struct npc_overlord_morghorAI : public ScriptedAI
{
    npc_overlord_morghorAI(Creature* c) : ScriptedAI(c) {}

    uint64 PlayerGUID;
    uint64 IllidanGUID;

    uint32 ConversationTimer;
    uint32 Step;

    bool Event;

    void Reset()
    {
        PlayerGUID = 0;
        IllidanGUID = 0;

        ConversationTimer = 0;
        Step = 0;

        Event = false;
    }

    void EnterCombat(Unit* /*who*/) {}

    void StartEvent()
    {
        me->SetUInt32Value(UNIT_NPC_FLAGS, 0);
        me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
        Unit* Illidan = me->SummonCreature(C_ILLIDAN, -5107.83f, 602.584f, 85.2393f, 4.92598f, TEMPSUMMON_CORPSE_DESPAWN, 0);
        if (Illidan)
        {
            IllidanGUID = Illidan->GetGUID();
            Illidan->SetVisible(false);
        }
        if (PlayerGUID)
        {
            Player* player = Unit::GetPlayer(*me, PlayerGUID);
            if (player)
                DoScriptText(OVERLORD_SAY_1, me, player);
        }
        ConversationTimer = 4200;
        Step = 0;
        Event = true;
    }

    uint32 NextStep(uint32 Step)
    {
        Player* plr = Unit::GetPlayer(*me, PlayerGUID);

        Unit* Illi = Unit::GetUnit((*me), IllidanGUID);

        if (!plr || !Illi)
        {
            EnterEvadeMode();
            return 0;
        }

        switch (Step)
        {
        case 0:
            return 0;
            break;
        case 1:
            me->GetMotionMaster()->MovePoint(0, -5104.41f, 595.297f, 85.6838f);
            return 9000;
            break;
        case 2:
            DoScriptText(OVERLORD_YELL_1, me, plr);
            return 4500;
            break;
        case 3:
            me->SetInFront(plr);
            return 3200;
            break;
        case 4:
            DoScriptText(OVERLORD_SAY_2, me, plr);
            return 2000;
            break;
        case 5:
            Illi->SetVisible(true);
            Illi->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            return 350;
            break;
        case 6:
            Illi->CastSpell(Illi, SPELL_ONE, true);
            Illi->SetUInt64Value(UNIT_FIELD_TARGET, me->GetGUID());
            me->SetUInt64Value(UNIT_FIELD_TARGET, IllidanGUID);
            return 2000;
            break;
        case 7:
            DoScriptText(OVERLORD_YELL_2, me);
            return 4500;
            break;
        case 8:
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, 8);
            return 2500;
            break;
        case 9:
            DoScriptText(OVERLORD_SAY_3, me);
            return 6500;
            break;
        case 10:
            DoScriptText(LORD_ILLIDAN_SAY_1, Illi);
            return 5000;
            break;
        case 11:
            DoScriptText(OVERLORD_SAY_4, me, plr);
            return 6000;
            break;
        case 12:
            DoScriptText(LORD_ILLIDAN_SAY_2, Illi);
            return 5500;
            break;
        case 13:
            DoScriptText(LORD_ILLIDAN_SAY_3, Illi);
            return 4000;
            break;
        case 14:
            Illi->SetUInt64Value(UNIT_FIELD_TARGET, PlayerGUID);
            return 1500;
            break;
        case 15:
            DoScriptText(LORD_ILLIDAN_SAY_4, Illi);
            return 1500;
            break;
        case 16:
            if (plr)
            {
                Illi->CastSpell(plr, SPELL_TWO, true);
                plr->RemoveAurasDueToSpell(SPELL_THREE);
                plr->RemoveAurasDueToSpell(SPELL_FOUR);
                return 5000;
            }
            else
            {
                // if !plr we can't do that!
                //   plr->FailQuest(QUEST_LORD_ILLIDAN_STORMRAGE);
                Step = 30;
                return 100;
            }
            break;
        case 17:
            DoScriptText(LORD_ILLIDAN_SAY_5, Illi);
            return 5000;
            break;
        case 18:
            DoScriptText(LORD_ILLIDAN_SAY_6, Illi);
            return 5000;
            break;
        case 19:
            DoScriptText(LORD_ILLIDAN_SAY_7, Illi);
            return 5000;
            break;
        case 20:
            Illi->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
            Illi->SetLevitate(true);
            return 500;
            break;
        case 21:
            DoScriptText(OVERLORD_SAY_5, me);
            return 500;
            break;
        case 22:
            Illi->SetVisible(false);
            Illi->setDeathState(JUST_DIED);
            return 1000;
            break;
        case 23:
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
            return 2000;
            break;
        case 24:
            me->SetUInt64Value(UNIT_FIELD_TARGET, PlayerGUID);
            return 5000;
            break;
        case 25:
            DoScriptText(OVERLORD_SAY_6, me);
            return 2000;
            break;
        case 26:
            if (plr)
                plr->GroupEventHappens(QUEST_LORD_ILLIDAN_STORMRAGE, me);
            return 6000;
            break;
        case 27:
            {
                Unit* Yarzill = me->FindNearestCreature(C_YARZILL, 50);
                if (Yarzill)
                    Yarzill->SetUInt64Value(UNIT_FIELD_TARGET, PlayerGUID);
                return 500;
            }
            break;
        case 28:
            plr->RemoveAurasDueToSpell(SPELL_TWO);
            plr->RemoveAurasDueToSpell(41519);
            plr->CastSpell(plr, SPELL_THREE, true);
            plr->CastSpell(plr, SPELL_FOUR, true);
            return 1000;
            break;
        case 29:
            {
                Unit* Yarzill = me->FindNearestCreature(C_YARZILL, 50);
                if (Yarzill)
                    DoScriptText(YARZILL_THE_MERC_SAY, Yarzill, plr);
                return 5000;
            }
            break;
        case 30:
            {
                Unit* Yarzill = me->FindNearestCreature(C_YARZILL, 50);
                if (Yarzill)
                    Yarzill->SetUInt64Value(UNIT_FIELD_TARGET, 0);
                return 5000;
            }
            break;
        case 31:
            {
                Unit* Yarzill = me->FindNearestCreature(C_YARZILL, 50);
                if (Yarzill)
                    Yarzill->CastSpell(plr, 41540, true);
                return 1000;
            }
            break;
        case 32:
            me->GetMotionMaster()->MovePoint(0, -5085.77f, 577.231f, 86.6719f);
            return 5000;
            break;
        case 33:
            Reset();
            return 100;
            break;

        default :
            return 0;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!ConversationTimer)
            return;

        if (ConversationTimer <= diff)
        {
            if (Event && IllidanGUID && PlayerGUID)
                ConversationTimer = NextStep(++Step);
        }
        else ConversationTimer -= diff;
    }
};

CreatureAI* GetAI_npc_overlord_morghorAI(Creature* pCreature)
{
    return new npc_overlord_morghorAI(pCreature);
}

bool QuestAccept_npc_overlord_morghor(Player* player, Creature* pCreature, const Quest* _Quest)
{
    if (_Quest->GetQuestId() == QUEST_LORD_ILLIDAN_STORMRAGE)
    {
        ((npc_overlord_morghorAI*)pCreature->AI())->PlayerGUID = player->GetGUID();
        ((npc_overlord_morghorAI*)pCreature->AI())->StartEvent();
        return true;
    }
    return false;
}

/*####
# npc_earthmender_wilda
####*/

enum eWilda
{
    SAY_START               = -1000381,
    SAY_AGGRO1              = -1000382,
    SAY_AGGRO2              = -1000383,
    ASSASSIN_SAY_AGGRO1     = -1000600,
    ASSASSIN_SAY_AGGRO2     = -1000601,
    SAY_PROGRESS1           = -1000384,
    SAY_PROGRESS2           = -1000385,
    SAY_PROGRESS3           = -1000386,
    SAY_PROGRESS4           = -1000387,
    SAY_PROGRESS5           = -1000388,
    SAY_PROGRESS6           = -1000389,
    SAY_END                 = -1000390,

    QUEST_ESCAPE_FROM_COILSKAR_CISTERN  = 10451,
    NPC_COILSKAR_ASSASSIN               = 21044
};

struct npc_earthmender_wildaAI : public npc_escortAI
{
    npc_earthmender_wildaAI(Creature* c) : npc_escortAI(c) {}

    bool Completed;

    void EnterCombat(Unit* who)
    {
        Player* player = GetPlayerForEscort();

        if (who->GetTypeId() == TYPEID_UNIT && who->GetEntry() == NPC_COILSKAR_ASSASSIN)
            DoScriptText(SAY_AGGRO2, me, player);
        else DoScriptText(SAY_AGGRO1, me, player);
    }

    void Reset()
    {
        me->setFaction(1726);
        Completed = false;
    }

    void WaypointReached(uint32 i)
    {
        Player* player = GetPlayerForEscort();

        if (!player)
            return;

        switch (i)
        {
        case 0:
            DoScriptText(SAY_START, me, player);
            break;
        case 13:
            DoScriptText(SAY_PROGRESS1, me, player);
            SummonAssassin();
            break;
        case 14:
            SummonAssassin();
            break;
        case 15:
            DoScriptText(SAY_PROGRESS3, me, player);
            break;
        case 19:
            switch (rand() % 3)
            {
            case 0:
                DoScriptText(SAY_PROGRESS2, me, player);
                break;
            case 1:
                DoScriptText(SAY_PROGRESS4, me, player);
                break;
            case 2:
                DoScriptText(SAY_PROGRESS5, me, player);
                break;
            }
            break;
        case 20:
            SummonAssassin();
            break;
        case 26:
            switch (rand() % 3)
            {
            case 0:
                DoScriptText(SAY_PROGRESS2, me, player);
                break;
            case 1:
                DoScriptText(SAY_PROGRESS4, me, player);
                break;
            case 2:
                DoScriptText(SAY_PROGRESS5, me, player);
                break;
            }
            break;
        case 27:
            SummonAssassin();
            break;
        case 33:
            switch (rand() % 3)
            {
            case 0:
                DoScriptText(SAY_PROGRESS2, me, player);
                break;
            case 1:
                DoScriptText(SAY_PROGRESS4, me, player);
                break;
            case 2:
                DoScriptText(SAY_PROGRESS5, me, player);
                break;
            }
            break;
        case 34:
            SummonAssassin();
            break;
        case 37:
            switch (rand() % 3)
            {
            case 0:
                DoScriptText(SAY_PROGRESS2, me, player);
                break;
            case 1:
                DoScriptText(SAY_PROGRESS4, me, player);
                break;
            case 2:
                DoScriptText(SAY_PROGRESS5, me, player);
                break;
            }
            break;
        case 38:
            SummonAssassin();
            break;
        case 39:
            DoScriptText(SAY_PROGRESS6, me, player);
            break;
        case 43:
            switch (rand() % 3)
            {
            case 0:
                DoScriptText(SAY_PROGRESS2, me, player);
                break;
            case 1:
                DoScriptText(SAY_PROGRESS4, me, player);
                break;
            case 2:
                DoScriptText(SAY_PROGRESS5, me, player);
                break;
            }
            break;
        case 44:
            SummonAssassin();
            break;
        case 50:
            DoScriptText(SAY_END, me, player);
            player->GroupEventHappens(QUEST_ESCAPE_FROM_COILSKAR_CISTERN, me);
            Completed = true;
            break;
        }
    }

    void SummonAssassin()
    {
        Player* player = GetPlayerForEscort();

        Unit* CoilskarAssassin = me->SummonCreature(NPC_COILSKAR_ASSASSIN, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 0);
        if (CoilskarAssassin)
        {
            switch (rand() % 2)
            {
            case 0:
                DoScriptText(ASSASSIN_SAY_AGGRO1, CoilskarAssassin, player);
                break;
            case 1:
                DoScriptText(ASSASSIN_SAY_AGGRO2, CoilskarAssassin, player);
                break;
            }
            CAST_CRE(CoilskarAssassin)->AI()->AttackStart(me);
        }
        else error_log("OSCR ERROR: Coilskar Assassin couldn't be summmoned");
    }

    void JustDied(Unit* /*killer*/)
    {
        if (!Completed)
        {
            Player* player = GetPlayerForEscort();
            if (player)
                player->FailQuest(QUEST_ESCAPE_FROM_COILSKAR_CISTERN);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);
    }
};

CreatureAI* GetAI_npc_earthmender_wildaAI(Creature* pCreature)
{
    return new npc_earthmender_wildaAI(pCreature);
}

bool QuestAccept_npc_earthmender_wilda(Player* player, Creature* creature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_ESCAPE_FROM_COILSKAR_CISTERN)
    {
        creature->setFaction(113);
        if (npc_earthmender_wildaAI* pEscortAI = CAST_AI(npc_earthmender_wildaAI, creature->AI()))
            pEscortAI->Start(false, false, player->GetGUID(), quest);
    }
    return true;
}

/*######
## Quest: Battle of the crimson watch
######*/

enum
{
    QUEST_BATTLE_OF_THE_CRIMSON_WATCH = 10781,

    EVENT_COOLDOWN                    = 30000,

    SAY_TORLOTH_DIALOGUE1             = -1000532,
    SAY_TORLOTH_DIALOGUE2             = -1000533,
    SAY_TORLOTH_DIALOGUE3             = -1000534,
    SAY_ILLIDAN_DIALOGUE              = -1000535,
    SAY_ILLIDAN_SUMMON1               = -1000536,
    SAY_ILLIDAN_SUMMON2               = -1000537,
    SAY_ILLIDAN_SUMMON3               = -1000538,
    SAY_ILLIDAN_SUMMON4               = -1000539,
    SAY_EVENT_COMPLETED               = -1000540,

    MODEL_ID_FELGUARD                 = 18654,
    MODEL_ID_DREADLORD                = 19991,

    NPC_ILLIDARI_SOLDIER              = 22075,
    NPC_ILLIDARI_MIND_BREAKER         = 22074,
    NPC_ILLIDARI_HIGHLORD             = 19797,
    NPC_TORLOTH_THE_MAGNIFICENT       = 22076,
    NPC_LORD_ILLIDAN                  = 22083
};

enum CinematicCreature
{
    LORD_ILLIDAN = 1,
    TORLOTH      = 0
};

const float EVENT_AREA_RADIUS = 65.0;

struct TorlothCinematic
{
    int32  iTextId;
    uint32 uiCreature;
    uint32 uiTimer;
};

static TorlothCinematic TorlothAnim[] =
{
    {SAY_TORLOTH_DIALOGUE1, TORLOTH, 2000},
    {SAY_ILLIDAN_DIALOGUE, LORD_ILLIDAN, 7000},
    {SAY_TORLOTH_DIALOGUE2, TORLOTH, 3000},
    {0, TORLOTH, 2000},                                  // Torloth stand
    {SAY_TORLOTH_DIALOGUE3, TORLOTH, 1000},
    {0, TORLOTH, 3000},
    {0, TORLOTH, 0}
};

struct Location
{
    float fLocX;
    float fLocY;
    float fLocZ;
    float fOrient;
};

static Location SpawnLocation[] =
{
    { -4615.8556f, 1342.2532f, 139.9f, 1.612f},             // Illidari Soldier
    { -4598.9365f, 1377.3182f, 139.9f, 3.917f},             // Illidari Soldier
    { -4598.4697f, 1360.8999f, 139.9f, 2.427f},             // Illidari Soldier
    { -4589.3599f, 1369.1061f, 139.9f, 3.165f},             // Illidari Soldier
    { -4608.3477f, 1386.0076f, 139.9f, 4.108f},             // Illidari Soldier
    { -4633.1889f, 1359.8033f, 139.9f, 0.949f},             // Illidari Soldier
    { -4623.5791f, 1351.4574f, 139.9f, 0.971f},             // Illidari Soldier
    { -4607.2988f, 1351.6099f, 139.9f, 2.416f},             // Illidari Soldier
    { -4633.7764f, 1376.0417f, 139.9f, 5.608f},             // Illidari Soldier
    { -4600.2461f, 1369.1240f, 139.9f, 3.056f},             // Illidari Mind Breaker
    { -4631.7808f, 1367.9459f, 139.9f, 0.020f},             // Illidari Mind Breaker
    { -4600.2461f, 1369.1240f, 139.9f, 3.056f},             // Illidari Highlord
    { -4631.7808f, 1367.9459f, 139.9f, 0.020f},             // Illidari Highlord
    { -4615.5586f, 1353.0031f, 139.9f, 1.540f},             // Illidari Highlord
    { -4616.4736f, 1384.2170f, 139.9f, 4.971f},             // Illidari Highlord
    { -4627.1240f, 1378.8752f, 139.9f, 2.544f}              // Torloth The Magnificent
};

struct WaveData
{
    uint8  uiSpawnCount;
    uint8  uiUsedSpawnPoint;
    uint32 uiCreatureId;
    uint32 uiSpawnTimer;
    uint32 uiYellTimer;
    int32  iTextId;
};

static WaveData WavesInfo[] =
{
    // Illidari Soldier
    {9, 0, NPC_ILLIDARI_SOLDIER, 10000, 7000, SAY_ILLIDAN_SUMMON1},
    // Illidari Mind Breaker
    {2, 9, NPC_ILLIDARI_MIND_BREAKER, 10000, 7000, SAY_ILLIDAN_SUMMON2},
    // Illidari Highlord
    {4, 11, NPC_ILLIDARI_HIGHLORD, 10000, 7000, SAY_ILLIDAN_SUMMON3},
    // Torloth The Magnificent
    {1, 15, NPC_TORLOTH_THE_MAGNIFICENT, 10000, 7000, SAY_ILLIDAN_SUMMON4}
};

/*######
# mob_torloth
#####*/

enum
{
    SPELL_CLEAVE           = 15284,
    SPELL_SHADOWFURY       = 39082,
    SPELL_SPELL_REFLECTION = 33961
};

struct mob_torlothAI : public ScriptedAI
{
    mob_torlothAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint64 m_uiLordIllidanGUID;
    uint64 m_uiPlayerGUID;

    uint32 m_uiCleaveTimer;
    uint32 m_uiShadowfuryTimer;
    uint32 m_uiSpellReflectionTimer;
    uint8  m_uiAnimationCount;
    uint32 m_uiAnimationTimer;

    void Reset()
    {
        m_uiLordIllidanGUID = 0;
        m_uiPlayerGUID = 0;

        m_uiAnimationCount = 0;
        m_uiAnimationTimer = 4000;
        m_uiCleaveTimer = 10000;
        m_uiShadowfuryTimer = 18000;
        m_uiSpellReflectionTimer = 25000;

        // make him not attackable for the time of animation
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetReactState(REACT_PASSIVE);
        SetCombatMovement(false);
    }

    void EnterEvadeMode()
    {
        me->ForcedDespawn();
    }

    void HandleAnimation()
    {
        Creature* pCreature = me;

        if (TorlothAnim[m_uiAnimationCount].uiCreature == LORD_ILLIDAN)
        {
            pCreature = me->GetMap()->GetCreature(m_uiLordIllidanGUID);

            if (!pCreature)
            {
                me->ForcedDespawn();
                return;
            }
        }

        if (TorlothAnim[m_uiAnimationCount].iTextId)
            DoScriptText(TorlothAnim[m_uiAnimationCount].iTextId, pCreature);

        m_uiAnimationTimer = TorlothAnim[m_uiAnimationCount].uiTimer;

        switch (m_uiAnimationCount)
        {
        case 0:
            me->SetStandState(UNIT_STAND_STATE_KNEEL);
            break;
        case 3:
            me->SetStandState(UNIT_STAND_STATE_STAND);
            break;
        case 5:
            if (Player* pTarget = Unit::GetPlayer(*me, m_uiPlayerGUID))
            {
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetUInt64Value(UNIT_FIELD_TARGET, pTarget->GetGUID());
                me->AddThreat(pTarget, 1);
                me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
            }
            break;
        case 6:
            {
                if (Player* pTarget = Unit::GetPlayer(*me, m_uiPlayerGUID))
                {
                    SetCombatMovement(true);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    float fLocX, fLocY, fLocZ;
                    pTarget->GetPosition(fLocX, fLocY, fLocZ);
                    me->GetMotionMaster()->MovePoint(0, fLocX, fLocY, fLocZ);
                }
                break;
            }
        }

        ++m_uiAnimationCount;
    }

    void JustDied(Unit* pKiller)
    {
        if (Player* pPlayer = pKiller->GetCharmerOrOwnerPlayerOrPlayerItself())
        {
            pPlayer->GroupEventHappens(QUEST_BATTLE_OF_THE_CRIMSON_WATCH, me);

            if (Creature* pLordIllidan = me->GetMap()->GetCreature(m_uiLordIllidanGUID))
            {
				pLordIllidan->MonsterYellToZone(SAY_EVENT_COMPLETED, LANG_UNIVERSAL, m_uiPlayerGUID);
                pLordIllidan->AI()->EnterEvadeMode();
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiAnimationCount < 7)
        {
            if (m_uiAnimationTimer < uiDiff)
                HandleAnimation();
            else
                m_uiAnimationTimer -= uiDiff;
        }
        else
        {
            if (!UpdateVictim())
                return;

            if (m_uiCleaveTimer < uiDiff)
            {
                DoCastVictim( SPELL_CLEAVE);
                m_uiCleaveTimer = 15000;
            }
            else
                m_uiCleaveTimer -= uiDiff;

            if (m_uiShadowfuryTimer < uiDiff)
            {
                DoCastVictim( SPELL_SHADOWFURY);
                m_uiShadowfuryTimer = 20000;
            }
            else
                m_uiShadowfuryTimer -= uiDiff;

            if (m_uiSpellReflectionTimer < uiDiff)
            {
                DoCast(me, SPELL_SPELL_REFLECTION);
                m_uiSpellReflectionTimer = 30000;
            }
            else
                m_uiSpellReflectionTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    }
};

CreatureAI* GetAI_mob_torloth(Creature* pCreature)
{
    return new mob_torlothAI(pCreature);
}

/*#####
# npc_lord_illidan_stormrage
#####*/

#define WHISPER_MARCUS "Quickly, get to the middle of the platform! Illidan's fury is soon to be unleashed!"

struct npc_lord_illidan_stormrageAI : public Scripted_NoMovementAI
{
    npc_lord_illidan_stormrageAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        Reset();
    }

    uint64 m_uiPlayerGUID;
    uint32 m_uiWaveTimer;
    uint32 m_uiAnnounceTimer;
    uint32 m_uiCheckTimer;
    uint8  m_uiMobCount;
    uint8  m_uiWaveCount;

    bool m_bEventStarted;
    bool m_bEventFailed;
    bool m_bWaveAnnounced;

    void Reset()
    {
        m_uiPlayerGUID = 0;

        m_uiWaveTimer = 10000;
        m_uiAnnounceTimer = 7000;
        m_uiCheckTimer = 2000;

        m_uiMobCount = 0;
        m_uiWaveCount = 0;

        m_bEventStarted = false;
        m_bEventFailed = false;
        m_bWaveAnnounced = false;
    }

    void StartEvent(Player* pPlayer)
    {
        m_bEventStarted = true;
        m_uiPlayerGUID = pPlayer->GetGUID();

		if (Creature* marcus = GetClosestCreatureWithEntry(pPlayer, 22073, 50.0))
			marcus->Whisper(WHISPER_MARCUS, m_uiPlayerGUID, false);
    }

    void SummonWave()
    {
        uint8 uiCount = WavesInfo[m_uiWaveCount].uiSpawnCount;
        uint8 uiLocIndex = WavesInfo[m_uiWaveCount].uiUsedSpawnPoint;
        uint8 uiFelguardCount = 0;
        uint8 uiDreadlordCount = 0;

        for (uint8 i = 0; i < uiCount; ++i)
        {
            float fLocX, fLocY, fLocZ, fOrient;
            fLocX = SpawnLocation[uiLocIndex + i].fLocX;
            fLocY = SpawnLocation[uiLocIndex + i].fLocY;
            fLocZ = SpawnLocation[uiLocIndex + i].fLocZ;
            fOrient = SpawnLocation[uiLocIndex + i].fOrient;

            if (Creature* pSpawn = me->SummonCreature(WavesInfo[m_uiWaveCount].uiCreatureId, fLocX, fLocY, fLocZ, fOrient, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000))
            {

                if (m_uiWaveCount)                          // only in first wave
                    continue;

                if (!urand(0, 2) && uiFelguardCount < 2)
                {
                    pSpawn->SetDisplayId(MODEL_ID_FELGUARD);
                    ++uiFelguardCount;
                }
                else if (uiDreadlordCount < 3)
                {
                    pSpawn->SetDisplayId(MODEL_ID_DREADLORD);
                    ++uiDreadlordCount;
                }
                else if (uiFelguardCount < 2)
                {
                    pSpawn->SetDisplayId(MODEL_ID_FELGUARD);
                    ++uiFelguardCount;
                }
            }
        }

        ++m_uiWaveCount;
        m_uiWaveTimer = WavesInfo[m_uiWaveCount].uiSpawnTimer;
        m_uiAnnounceTimer = WavesInfo[m_uiWaveCount].uiYellTimer;
    }

    void JustSummoned(Creature* pSummoned)
    {
        // increment mob count
        ++m_uiMobCount;

        if (!m_uiPlayerGUID)
            return;

        if (pSummoned->GetEntry() == NPC_TORLOTH_THE_MAGNIFICENT)
        {
            if (mob_torlothAI* pTorlothAI = dynamic_cast<mob_torlothAI*>(pSummoned->AI()))
            {
                pTorlothAI->m_uiLordIllidanGUID = me->GetGUID();
                pTorlothAI->m_uiPlayerGUID = m_uiPlayerGUID;
            }
        }
        else
        {
            if (Player* pTarget = Unit::GetPlayer(*me, m_uiPlayerGUID))
            {
                float fLocX, fLocY, fLocZ;
                pTarget->GetPosition(fLocX, fLocY, fLocZ);
                pSummoned->GetMotionMaster()->MovePoint(0, fLocX, fLocY, fLocZ);
            }
        }
    }

    void SummonedCreatureDespawn(Creature* /*pCreature*/)
    {
        // decrement mob count
        --m_uiMobCount;

        if (!m_uiMobCount)
            m_bWaveAnnounced = false;
    }

    void CheckEventFail()
    {
        Player* pPlayer = Unit::GetPlayer(*me, m_uiPlayerGUID);

        if (!pPlayer)
            return;

        if (Group* pEventGroup = pPlayer->GetGroup())
        {
            uint8 uiDeadMemberCount = 0;
            uint8 uiFailedMemberCount = 0;

            for (GroupReference* pRef = pEventGroup->GetFirstMember(); pRef != NULL; pRef = pRef->next())
            {
                if (Player* pMember = pRef->getSource())
                {
                    if (!pMember->IsAlive())
                        ++uiDeadMemberCount;

                    // if we already failed no need to check other things
                    if (pMember->GetQuestStatus(QUEST_BATTLE_OF_THE_CRIMSON_WATCH) == QUEST_STATUS_FAILED)
                    {
                        ++uiFailedMemberCount;
                        continue;
                    }

                    // we left event area fail quest
                    if (!pMember->IsWithinDistInMap(me, EVENT_AREA_RADIUS))
                    {
                        pMember->FailQuest(QUEST_BATTLE_OF_THE_CRIMSON_WATCH);
                        ++uiFailedMemberCount;
                    }
                }
            }

            if (pEventGroup->GetMembersCount() == uiFailedMemberCount)
            {
                m_bEventFailed = true;
                return;
            }

            if (pEventGroup->GetMembersCount() == uiDeadMemberCount)
            {
                for (GroupReference* pRef = pEventGroup->GetFirstMember(); pRef != NULL; pRef = pRef->next())
                {
                    if (Player* pMember = pRef->getSource())
                    {
                        if (pMember->GetQuestStatus(QUEST_BATTLE_OF_THE_CRIMSON_WATCH) == QUEST_STATUS_INCOMPLETE)
                            pMember->FailQuest(QUEST_BATTLE_OF_THE_CRIMSON_WATCH);
                    }
                }

                m_bEventFailed = true;
            }
        }
        else if (pPlayer->isDead() || !pPlayer->IsWithinDistInMap(me, EVENT_AREA_RADIUS))
        {
            pPlayer->FailQuest(QUEST_BATTLE_OF_THE_CRIMSON_WATCH);
            m_bEventFailed = true;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_uiPlayerGUID || !m_bEventStarted)
            return;

        if (!m_uiMobCount && m_uiWaveCount < 4)
        {
            if (!m_bWaveAnnounced && m_uiAnnounceTimer < uiDiff)
            {
                DoScriptText(WavesInfo[m_uiWaveCount].iTextId, me);
                m_bWaveAnnounced = true;
            }
            else
                m_uiAnnounceTimer -= uiDiff;

            if (m_uiWaveTimer < uiDiff)
                SummonWave();
            else
                m_uiWaveTimer -= uiDiff;
        }

        if (m_uiCheckTimer < uiDiff)
        {
            CheckEventFail();
            m_uiCheckTimer = 2000;
        }
        else
            m_uiCheckTimer -= uiDiff;

        if (m_bEventFailed)
            Reset();
    }
};

CreatureAI* GetAI_npc_lord_illidan_stormrage(Creature * (pCreature))
{
    return new npc_lord_illidan_stormrageAI(pCreature);
}

/*#####
# go_crystal_prison : GameObject that begins the event and hands out quest
######*/
bool GOQuestAccept_GO_crystal_prison(Player* pPlayer, GameObject* /*pGo*/, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_BATTLE_OF_THE_CRIMSON_WATCH )
        if (Creature* pLordIllidan = GetClosestCreatureWithEntry(pPlayer, NPC_LORD_ILLIDAN, 50.0))
            if (npc_lord_illidan_stormrageAI* pIllidanAI = dynamic_cast<npc_lord_illidan_stormrageAI*>(pLordIllidan->AI()))
                if (!pIllidanAI->m_bEventStarted)
                    pIllidanAI->StartEvent(pPlayer);

    return true;
}

/*####
# npc_enraged_spirits
####*/

/* QUESTS */
#define QUEST_ENRAGED_SPIRITS_FIRE_EARTH 10458
#define QUEST_ENRAGED_SPIRITS_AIR 10481
#define QUEST_ENRAGED_SPIRITS_WATER 10480

/* Totem */
#define ENTRY_TOTEM_OF_SPIRITS 21071
#define RADIUS_TOTEM_OF_SPIRITS 15

/* SPIRITS */
#define ENTRY_ENRAGED_EARTH_SPIRIT 21050
#define ENTRY_ENRAGED_FIRE_SPIRIT 21061
#define ENTRY_ENRAGED_AIR_SPIRIT 21060
#define ENTRY_ENRAGED_WATER_SPIRIT 21059

/* SOULS */
#define ENTRY_EARTHEN_SOUL 21073
#define ENTRY_FIERY_SOUL 21097
#define ENTRY_ENRAGED_AIRY_SOUL 21116
#define ENTRY_ENRAGED_WATERY_SOUL 21109  // wrong model

/* SPELL KILLCREDIT - not working!?! - using KilledMonsterCredit */
#define SPELL_EARTHEN_SOUL_CAPTURED_CREDIT 36108
#define SPELL_FIERY_SOUL_CAPTURED_CREDIT 36117
#define SPELL_AIRY_SOUL_CAPTURED_CREDIT 36182
#define SPELL_WATERY_SOUL_CAPTURED_CREDIT 36171

/* KilledMonsterCredit Workaround */
#define CREDIT_FIRE 21094
#define CREDIT_WATER 21095
#define CREDIT_AIR 21096
#define CREDIT_EARTH 21092

/* Captured Spell/Buff */
#define SPELL_SOUL_CAPTURED 36115

/* Factions */
#define ENRAGED_SOUL_FRIENDLY 35
#define ENRAGED_SOUL_HOSTILE 14

struct npc_enraged_spiritAI : public ScriptedAI
{
    npc_enraged_spiritAI(Creature* c) : ScriptedAI(c) {}

    void Reset()   { }

    void EnterCombat(Unit* /*who*/) {}

    void JustDied(Unit* /*killer*/)
    {
        // always spawn spirit on death
        // if totem around
        // move spirit to totem and cast kill count
        uint32 entry = 0;
        uint32 credit = 0;

        switch (me->GetEntry())
        {
        case ENTRY_ENRAGED_FIRE_SPIRIT:
            entry  = ENTRY_FIERY_SOUL;
            //credit = SPELL_FIERY_SOUL_CAPTURED_CREDIT;
            credit = CREDIT_FIRE;
            break;
        case ENTRY_ENRAGED_EARTH_SPIRIT:
            entry  = ENTRY_EARTHEN_SOUL;
            //credit = SPELL_EARTHEN_SOUL_CAPTURED_CREDIT;
            credit = CREDIT_EARTH;
            break;
        case ENTRY_ENRAGED_AIR_SPIRIT:
            entry  = ENTRY_ENRAGED_AIRY_SOUL;
            //credit = SPELL_AIRY_SOUL_CAPTURED_CREDIT;
            credit = CREDIT_AIR;
            break;
        case ENTRY_ENRAGED_WATER_SPIRIT:
            entry  = ENTRY_ENRAGED_WATERY_SOUL;
            //credit = SPELL_WATERY_SOUL_CAPTURED_CREDIT;
            credit = CREDIT_WATER;
            break;
        }

        // Spawn Soul on Kill ALWAYS!
        Creature* Summoned = NULL;
        Unit* totemOspirits = NULL;

        if (entry != 0)
            Summoned = DoSpawnCreature(entry, 0, 0, 1, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 5000);

        // FIND TOTEM, PROCESS QUEST
        if (Summoned)
        {
            totemOspirits = me->FindNearestCreature(ENTRY_TOTEM_OF_SPIRITS, RADIUS_TOTEM_OF_SPIRITS);
            if (totemOspirits)
            {
                Summoned->setFaction(ENRAGED_SOUL_FRIENDLY);
                Summoned->GetMotionMaster()->MovePoint(0, totemOspirits->GetPositionX(), totemOspirits->GetPositionY(), Summoned->GetPositionZ());

                Player* Owner = (Player*)totemOspirits->GetOwner();
                if (Owner)
                    // DoCast(Owner, credit); -- not working!
                    Owner->KilledMonsterCredit(credit, 0);
                DoCast(totemOspirits, SPELL_SOUL_CAPTURED);
            }
        }
    }
};

CreatureAI* GetAI_npc_enraged_spirit(Creature* pCreature)
{
    return new npc_enraged_spiritAI(pCreature);
}

/*######
# npc_jovaan
#####*/

enum
{
    SPELL_BOX             = 37097,
    NPC_WARBRINGER        = 21502,

    SAY_JOVAAN1           = -1900140,
    SAY_JOVAAN2           = -1900142,
    SAY_JOVAAN3           = -1900144,
    SAY_JOVAAN4           = -1900146,
    SAY_WARBRINGER1       = -1900141,
    SAY_WARBRINGER2       = -1900143,
    SAY_WARBRINGER3       = -1900145,
    SAY_WARBRINGER4       = -1900147,

    QUEST_LEGION_HOLD1    = 10563,
    QUEST_LEGION_HOLD2    = 10596,

    GO_INFERNAL           = 184834,
    GO_INFERNAL_TRAP      = 184835
};

struct npc_jovaanAI : public ScriptedAI
{
    npc_jovaanAI(Creature* pCreature) : ScriptedAI(pCreature) {}

    bool Image;

    uint64 uiPlayerGUID;
    uint32 uiStepsTimer;
    uint32 uiSteps;

    void Reset()
    {
        me->setFaction(7);
        Image = false;
        uiPlayerGUID = 0;
        uiStepsTimer = 0;
        uiSteps = 0;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER)
        {
            if (me->IsWithinDistInMap(((Player*)pWho), 15) && ((Player*)pWho)->HasAura(SPELL_BOX, 0))
            {
                uiPlayerGUID = pWho->GetGUID();
                if ((CAST_PLR(pWho)->GetQuestStatus(QUEST_LEGION_HOLD1) == QUEST_STATUS_INCOMPLETE) || (CAST_PLR(pWho)->GetQuestStatus(QUEST_LEGION_HOLD2) == QUEST_STATUS_INCOMPLETE))
                    Image = true;
            }
            else uiPlayerGUID = 0;
        }
    }

    uint32 NextStep(uint32 uiSteps)
    {
        Creature* pWarbringer = me->FindNearestCreature(NPC_WARBRINGER, 25);
        GameObject* pInfernal = me->FindNearestGameObject(GO_INFERNAL, 15);
        GameObject* pInfernalTrap = me->FindNearestGameObject(GO_INFERNAL_TRAP, 15);

        if (!pWarbringer || !pInfernal || !pInfernalTrap)
        {
            Reset();
            return 0;
        }

        switch (uiSteps)
        {
        case 1:
            pInfernal->SetRespawnTime(61);
            pInfernal->UpdateObjectVisibility();
            break;
        case 2:
            pInfernalTrap->SetRespawnTime(61);
            pInfernalTrap->UpdateObjectVisibility();
            return 500;
        case 3:
            me->setFaction(35);
            me->SummonCreature(NPC_WARBRINGER, -3300.479, 2927.177, 173.894, 2.5f, TEMPSUMMON_TIMED_DESPAWN, 60000);
            return 1000;
        case 4:
            DoScriptText(SAY_JOVAAN1, me, 0);
            return 6000;
        case 5:
            DoScriptText(SAY_WARBRINGER1, pWarbringer, 0);
            pWarbringer->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
            return 6800;
        case 6:
            DoScriptText(SAY_JOVAAN2, me, 0);
            return 6800;
        case 7:
            DoScriptText(SAY_WARBRINGER2, pWarbringer, 0);
            return 6800;
        case 8:
            DoScriptText(SAY_JOVAAN3, me, 0);
            return 6800;
        case 9:
            DoScriptText(SAY_WARBRINGER3, pWarbringer, 0);
            return 6800;
        case 10:
            DoScriptText(SAY_JOVAAN4, me, 0);
            return 6800;
        case 11:
            DoScriptText(SAY_WARBRINGER4, pWarbringer, 0);
            return 6000;
        case 12:
            if (Player* pPlayer = Unit::GetPlayer(*me, uiPlayerGUID))
                pPlayer->KilledMonsterCredit(NPC_WARBRINGER, 0);;
            return 2000;
        case 13:
            Reset();
        default:
            return 0;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (uiStepsTimer <= uiDiff)
        {
            if (Image)
                uiStepsTimer = NextStep(++uiSteps);
        }
        else uiStepsTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_jovaan(Creature* pCreature)
{
    return new npc_jovaanAI(pCreature);
}

/*######
## npc_grand_commander_ruusk
######*/

#define QUEST_10577    10577

#define GOSSIP_HGCR "I bring word from Lord Illidan."
#define GOSSIP_SGCR1 "The cipher fragment is to be moved. Have it delivered to Zuluhed."
#define GOSSIP_SGCR2 "Perhaps you did not hear me, Ruusk. I am giving you an order from Illidan himself!"
#define GOSSIP_SGCR3 "Very well. I will return to the Black Temple and notify Lord Illidan of your unwillingness to carry out his wishes. I suggest you make arrangements with your subordinates and let them know that you will soon be leaving this world."
#define GOSSIP_SGCR4 "Do I need to go into all the gory details? I think we are both well aware of what Lord Illidan does with those that would oppose his word. Now, I must be going! Farewell, Ruusk! Forever..."
#define GOSSIP_SGCR5 "Ah, good of you to come around, Ruusk. Thank you and farewell."

bool GossipHello_npc_grand_commander_ruusk(Player* player, Creature* _Creature)
{
    if (player->GetQuestStatus(QUEST_10577) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM( 0, GOSSIP_HGCR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SEND_GOSSIP_MENU(10401, _Creature->GetGUID());
    return true;
}

bool GossipSelect_npc_grand_commander_ruusk(Player* player, Creature* _Creature, uint32 sender, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF+1:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SGCR1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(10405, _Creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+2:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SGCR2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->SEND_GOSSIP_MENU(10406, _Creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+3:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SGCR3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
        player->SEND_GOSSIP_MENU(10407, _Creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+4:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SGCR4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
        player->SEND_GOSSIP_MENU(10408, _Creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+5:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_SGCR5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
        player->SEND_GOSSIP_MENU(10409, _Creature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF+6:
        player->CLOSE_GOSSIP_MENU();
        player->AreaExploredOrEventHappens(QUEST_10577);
        break;
    }
    return true;
}

/*######
## npc_azaloth
######*/

enum Azaloth
{
    // NPCs
    NPC_SUNFURY_WARLOCK     = 21503,
    NPC_AZALOTH             = 21506,
    NPC_AZALOTH_CREDIT      = 21892,

    // Spells
    SPELL_UNBANISH_AZALOTH  = 37834,
    SPELL_BANISH            = 37833,
    SPELL_SPELLBIND_AZALOTH = 38722,
    SPELL_AZALOTH_CLEAVE    = 40504,
    SPELL_CRIPPLE           = 11443,
    SPELL_RAIN_OF_FIRE      = 38741,
    SPELL_WARSTOMP          = 38750
};

struct npc_azalothAI : public ScriptedAI
{
	bool free;
	uint64 PlayerGUID;

	uint32 cleaveTimer;
	uint32 crippleTimer;
	uint32 warstompTimer;
	uint32 rainOfFireTimer;
	uint32 banishTimer;

	void Reset()
	{
		free = false;
		PlayerGUID = 0;
		me->CombatStop();
		me->DeleteThreatList();
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC); // passive
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
		me->setFaction(1813);

		cleaveTimer = 6000;
		crippleTimer = 1000;
		rainOfFireTimer = 15000;
		warstompTimer = 10000;
		banishTimer = 60000;
	}

	npc_azalothAI(Creature* c) : ScriptedAI(c){}

	void SpellHit(Unit* caster, const SpellEntry* spell)
	{
		if (caster->GetTypeId() == TYPEID_PLAYER && spell->Id == 37834 && !free)
		{
			if (me->HasAura(37833, 0))
				me->RemoveAurasDueToSpell(37833);

			free = true;
			PlayerGUID = caster->GetGUID();
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_NPC); // passive
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);		
			me->setFaction(954);

			Unit* Warlock = me->FindNearestCreature(21503, 50);

			if (Warlock)
			{
				me->AddThreat(Warlock, 100000.0f);
				AttackStart(Warlock);
			}
			DoMeleeAttackIfReady();
		}
	}

	void UpdateAI(const uint32 diff)
	{
		Unit* Warlock = me->FindNearestCreature(21503, 50);

		if (!Warlock && !UpdateVictim())
		{
			me->ForcedDespawn();
		}

		if (UpdateVictim())
		{
			if (banishTimer < diff)
			{
				DoCast(SPELL_BANISH);
				banishTimer = 60000;
			}

			if (!UpdateVictim())
			{
				banishTimer -= diff;
				return;
			}

			if (cleaveTimer < diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				cleaveTimer = 6000;
			}
			else
				cleaveTimer -= diff;

			if (crippleTimer < diff)
			{
				DoCastVictim(SPELL_CRIPPLE);
				crippleTimer = 20000;
			}
			else
				crippleTimer -= diff;

			if (rainOfFireTimer < diff)
			{
				DoCastVictim(SPELL_RAIN_OF_FIRE);
				rainOfFireTimer = 25000;
			}
			else
				rainOfFireTimer -= diff;

			if (warstompTimer < diff)
			{
				DoCastVictim(SPELL_WARSTOMP);
				warstompTimer = 22000;
			}
			else
				warstompTimer -= diff;

			DoMeleeAttackIfReady();
		}
	}
};
CreatureAI* GetAI_npc_azaloth(Creature* creature)
{
    return new npc_azalothAI(creature);
}


/*######
## npc_sunfurywarlock
######*/

enum sunfurywarlock
{
	SPELL_BEAM_BANISH = 39123
};

struct npc_sunfurywarlockAI : public ScriptedAI
{
	bool CastBeam;

	void Reset()
	{
		CastBeam = false;
	}

	npc_sunfurywarlockAI(Creature* c) : ScriptedAI(c) {}

	void EnterCombat(Unit* who)
	{
		me->InterruptNonMeleeSpells(true);
	}

	void UpdateAI(const uint32 diff)
	{
		if (Unit* azaloth = me->FindNearestCreature(NPC_AZALOTH, 50.0f))
		{
			if (azaloth->IsAlive() && !CastBeam && !me->IsInCombat() && azaloth->HasAura(37833, 0))
			{
				CastBeam = true;
				DoCast(azaloth, SPELL_BEAM_BANISH);
			}
		}
		DoMeleeAttackIfReady();

		if (!UpdateVictim())
			return;
	}

	void MoveInLineOfSight(Unit* who)
	{
		if (Unit* pCombat = me->FindNearestCreature(21506, 50.0f))
		{
			if (!pCombat->HasAura(37833, 0))
				AttackStart(pCombat);
		}
	}
};
CreatureAI* GetAI_npc_sunfurywarlock(Creature* pCreature)
{
	return new npc_sunfurywarlockAI(pCreature);
}

/*######
## legion_infernal_summon_trigger
######*/

struct legion_infernal_summon_triggerAI : public ScriptedAI
{
	legion_infernal_summon_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->SetCanFly(true);
		me->canFly();
		infernal_summon_timer = 5000;
	}

	uint32 infernal_summon_timer;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (Creature* summoner = me->FindNearestCreature(61030, 100.0f, true))
			{
				if (infernal_summon_timer <= diff)
				{
					DoCast(summoner, 33814);
					infernal_summon_timer = urand(30000, 90000);
				}
				else infernal_summon_timer -= diff;
			}
		}
	}
};

CreatureAI* GetAI_legion_infernal_summon_trigger(Creature* pCreature)
{
	return new legion_infernal_summon_triggerAI(pCreature);
}

struct legion_infernal_summoner_triggerAI : public ScriptedAI
{
	legion_infernal_summoner_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		spellHit = false;
	}

	bool spellHit;

	//Doesn't work as of now
	void SpellHit(Unit* Hitter, const SpellEntry* Spellkind)
	{
		if (Spellkind->Id == 33814 && !spellHit)
		{		
			me->SummonCreature(21419, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 180000);
			me->DisappearAndDie();
			spellHit = true;		
		}
	}
};

CreatureAI* GetAI_legion_infernal_summoner_trigger(Creature* pCreature)
{
	return new legion_infernal_summoner_triggerAI(pCreature);
}

/*######
## legion_hold_device_trigger
######*/

struct legion_hold_device_triggerAI : public ScriptedAI
{
	legion_hold_device_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->SetReactState(REACT_AGGRESSIVE);
		LegionEventStart = false;
	}

	bool LegionEventStart;

	void JustSummoned(Creature* summoned)
	{
		summoned->CastSpell(summoned, 41232, true);
	}

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player* plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(QUEST_LEGION_HOLD2) == QUEST_STATUS_INCOMPLETE || plWho->GetQuestStatus(QUEST_LEGION_HOLD1) == QUEST_STATUS_INCOMPLETE && plWho->GetDistance(me) < 15.0f && plWho->HasItemCount(30638, 1, false) && plWho->HasAura(SPELL_BOX))
			{
				switch (me->GetEntry())
				{
				case 61031:
					if (!LegionEventStart)
					{
						me->SummonCreature(21633, -3312.7f, 2949.5f, 171.1f, 4.78f, TEMPSUMMON_MANUAL_DESPAWN, 0);
						me->DisappearAndDie();
						LegionEventStart = true;
					}
					break;
				}
			}
		}
	}
};

CreatureAI* GetAI_legion_hold_device_trigger(Creature* pCreature)
{
	return new legion_hold_device_triggerAI(pCreature);
}

struct npc_deathbringer_jovaanAI : public ScriptedAI
{
	npc_deathbringer_jovaanAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	bool Image;

	uint64 uiPlayerGUID;
	uint32 uiStepsTimer;
	uint32 uiSteps;

	void Reset()
	{		
		me->setFaction(7);
		Image = true;
		uiPlayerGUID = 0;
		uiStepsTimer = 0;
		uiSteps = 0;
	}

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(QUEST_LEGION_HOLD2) == QUEST_STATUS_INCOMPLETE || plWho->GetQuestStatus(QUEST_LEGION_HOLD1) == QUEST_STATUS_INCOMPLETE && plWho->GetDistance(me) < 15.0f && plWho->HasAura(SPELL_BOX))
			{
				switch (me->GetEntry())
				{
				case 21633:
					uiPlayerGUID = plWho->GetGUID();
				break;
				}
			}
		}
	}

	uint32 NextStep(uint32 uiSteps)
	{
		Creature* pWarbringer = me->FindNearestCreature(NPC_WARBRINGER, 30);
		GameObject* pInfernal = me->FindNearestGameObject(GO_INFERNAL, 30);
		GameObject* pInfernalTrap = me->FindNearestGameObject(GO_INFERNAL_TRAP, 30);

		if (!pInfernal || !pInfernalTrap)
		{
			Reset();
			return 0;
		}

		switch (uiSteps)
		{
		case 1:
			me->GetMotionMaster()->MovePoint(0, -3305.8f, 2931.3f, 170.9f);
			pInfernal->SetRespawnTime(61);
			pInfernal->UpdateObjectVisibility();
			return 500;
		case 2:
			pInfernalTrap->SetRespawnTime(61);
			pInfernalTrap->UpdateObjectVisibility();
			return 500;
		case 3:
			me->setFaction(35);
			me->SummonCreature(NPC_WARBRINGER, -3300.479, 2927.177, 173.894, 2.5f, TEMPSUMMON_TIMED_DESPAWN, 60000);
			return 1000;
		case 4:
			DoScriptText(SAY_JOVAAN1, me, 0);
			return 6000;
		case 5:
			DoScriptText(SAY_WARBRINGER1, pWarbringer, 0);
			pWarbringer->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
			return 5800;
		case 6:
			DoScriptText(SAY_JOVAAN2, me, 0);
			return 5800;
		case 7:
			DoScriptText(SAY_WARBRINGER2, pWarbringer, 0);
			return 5800;
		case 8:
			DoScriptText(SAY_JOVAAN3, me, 0);
			return 6800;
		case 9:
			DoScriptText(SAY_WARBRINGER3, pWarbringer, 0);
			return 6800;
		case 10:
			DoScriptText(SAY_JOVAAN4, me, 0);
			return 6800;
		case 11:
			DoScriptText(SAY_WARBRINGER4, pWarbringer, 0);
			return 6000;
		case 12:
			if (Player* pPlayer = Unit::GetPlayer(*me, uiPlayerGUID))
				pPlayer->KilledMonsterCredit(NPC_WARBRINGER, 0);;
			return 2000;
		case 13:
			me->DisappearAndDie();
		default:
			return 0;
		}
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (uiStepsTimer <= uiDiff)
		{
			if (Image)
				uiStepsTimer = NextStep(++uiSteps);
		}
		else uiStepsTimer -= uiDiff;
	}
};

CreatureAI* GetAI_npc_deathbringer_jovaan(Creature* pCreature)
{
	return new npc_deathbringer_jovaanAI(pCreature);
}

/*######
## npc_shadowcouncil_warlock
######*/

struct npc_shadowcouncil_warlockAI : public ScriptedAI
{
	npc_shadowcouncil_warlockAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		shadowbolt_timer = 2500;
		drain_timer = 10000;
	}

	uint32 shadowbolt_timer;
	uint32 drain_timer;

	void UpdateAI(const uint32 diff)
	{
		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (!UpdateVictim())
		{
			if (Creature* visual = me->FindNearestCreature(61025, 20.0f, true))
			{
				DoCast(visual, 33346);
			}
		}

		if (UpdateVictim())
		{
			if (HealthBelowPct(75) && drain_timer <= diff)
			{
				DoCastVictim(37992);
				drain_timer = urand(12000, 18000);
			}
			else drain_timer -= diff;

			if (shadowbolt_timer <= diff)
			{
				DoCastVictim(9613);
				shadowbolt_timer = urand(6000, 8000);
			}
			else shadowbolt_timer -= diff;
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_shadowcouncil_warlock(Creature* pCreature)
{
	return new npc_shadowcouncil_warlockAI(pCreature);
}

/*######
## npc_plexi
######*/

#define QUEST_FEL_AND_FURIOUS_ALLY 10612
#define SAY_TIMER_START "I've rigged a few control consoles down the slope. Get to one quickly and take control of the fel reaver! You'll only have a limited amount of time."

bool QuestAccept_npc_plexi(Player* player, Creature* creature, Quest const* quest)
{
	if (quest->GetQuestId() == QUEST_FEL_AND_FURIOUS_ALLY)
	{
		creature->MonsterSay(SAY_TIMER_START, LANG_UNIVERSAL, 0);
		
	}
	return true;
}

/*######
## npc_nakansi
######*/

#define QUEST_FEL_AND_FURIOUS_HORDE 10613

bool QuestAccept_npc_nakansi(Player* player, Creature* creature, Quest const* quest)
{
	if (quest->GetQuestId() == QUEST_FEL_AND_FURIOUS_HORDE)
	{
		creature->MonsterSay(SAY_TIMER_START, LANG_UNIVERSAL, 0);
	}
	return true;
}

/*####
# npc_zuluhed_the_whacked
####*/

#define QUEST_ZULUHED_THE_WHACKED 10866
#define ZUHULED_TEXT_1 "Foolish mortals. Did you think that I would not strike you down for your transgressions?"
#define ZUHULED_TEXT_2 "Indeed, the time has come to end this charade."
#define ZUHULED_TEXT_3 "Destroy them! Destroy them all!"
#define ZUHULED_TEXT_4 "Lord Illidan, bless me with the power of the flight!"
#define KARYNAKU_YELL "ATTACK WITH THE FULL FORCE OF THE NETHERWING, MY CHILDREN ATTACK!"

struct npc_zuluhed_the_whackedAI : public ScriptedAI
{
	npc_zuluhed_the_whackedAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		ZuhuledEvent = true;
		yell = false;
		yell1 = false;
		combat_start = false;
		kar_yell = false;
		transform = false;
		portal = false;
		arcubus = false;

		me->MonsterYell(ZUHULED_TEXT_1, LANG_UNIVERSAL, 0);
		start_timer = 6000;
		rain_timer = 20000;
		summon_timer = 15000;

		playerGUID = 0;
	}

	uint32 start_timer;
	uint32 start_timer1;
	uint32 combat_timer;
	uint32 rain_timer;
	uint32 portal_timer;
	uint32 summon_timer;

	uint64 playerGUID;

	bool ZuhuledEvent;
	bool yell;
	bool yell1;
	bool combat_start;
	bool kar_yell;
	bool transform;
	bool portal;
	bool arcubus;

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(QUEST_ZULUHED_THE_WHACKED) == QUEST_STATUS_INCOMPLETE && plWho->GetDistance(me) < 35.0f)
			{
				switch (me->GetEntry())
				{
				case 11980:
					playerGUID = plWho->GetGUID();
					break;
				}
			}
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (!UpdateVictim())
		{
			if (ZuhuledEvent)
			{
				Player* player = Unit::GetPlayer(*me, playerGUID);

				if (!yell && start_timer <= diff)
				{
					me->MonsterYell(ZUHULED_TEXT_2, LANG_UNIVERSAL, 0);
					start_timer1 = 5000;
					yell = true;
				}
				else start_timer -= diff;

				if (yell && !yell1 && start_timer1 <= diff)
				{
					me->MonsterYell(ZUHULED_TEXT_3, LANG_UNIVERSAL, 0);
					combat_timer = 3000;
					yell1 = true;
				}
				else start_timer1 -= diff;

				if (!combat_start && yell1 && combat_timer <= diff)
				{
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);					
					me->AI()->AttackStart(player);
					combat_timer = 3000;
					combat_start = true;
					ZuhuledEvent = false;
				}
				else combat_timer -= diff;
			}
		}

		if (UpdateVictim())
		{
			if (summon_timer <= diff && !arcubus)
			{
				DoCast(38877);
				arcubus = true;
			}
			else summon_timer -= diff;

			if (HealthBelowPct(50) && !kar_yell)
			{
				if (Creature* karynaku = me->FindNearestCreature(22112, 100.0f, true))
				{
					karynaku->MonsterYell(KARYNAKU_YELL, LANG_UNIVERSAL, 0);
					kar_yell = true;
				}
			}

			if (HealthBelowPct(30) && !transform)
			{ 
				me->MonsterYell(ZUHULED_TEXT_4, LANG_UNIVERSAL, 0);
				DoCast(me, 38853);
				transform = true;
			}

			if (rain_timer <= diff)
			{
				DoCastVictim(19717);
				rain_timer = 20000;
				portal_timer = 5000;
			}
			else rain_timer -= diff;

			if (portal_timer <= diff && !portal)
			{
				DoCast(38876);
				portal = true;
			}
			else portal_timer -= diff;
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_zuluhed_the_whacked(Creature* pCreature)
{
	return new npc_zuluhed_the_whackedAI(pCreature);
}

/*####
# npc_dragonmaw_archer
####*/

struct npc_dragonmaw_archerAI : public ScriptedAI
{
	npc_dragonmaw_archerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		ZuhuledEventCombat = true;
		combat_start = false;

		shoot_timer = 1000;
		serpent_timer = 6000;
		scatter_timer = 10000;
		aimed_timer = 13000;
		start_timer = 16000;
		playerGUID = 0;
	}

	uint32 start_timer;
	uint32 shoot_timer;
	uint32 serpent_timer;
	uint32 scatter_timer;
	uint32 aimed_timer;

	uint64 playerGUID;

	bool ZuhuledEventCombat;
	bool combat_start;

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(QUEST_ZULUHED_THE_WHACKED) == QUEST_STATUS_INCOMPLETE && plWho->GetDistance(me) < 35.0f)
			{
				switch (me->GetEntry())
				{
				case 22251:
					playerGUID = plWho->GetGUID();
					break;
				}
			}
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (me->HasUnitState(UNIT_STATE_CASTING))
			return;

		if (!UpdateVictim())
		{
			if (ZuhuledEventCombat)
			{
				Player* player = Unit::GetPlayer(*me, playerGUID);

				if (!combat_start && start_timer <= diff)
				{
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);									
					me->AI()->AttackStart(player);
					combat_start = true;					
				}
				else start_timer -= diff;
			}
		}

		if (UpdateVictim())
		{
			if (me->FindNearestCreature(11980, 100.0f, false))
			{
				me->DisappearAndDie();
			}

			if (shoot_timer <= diff)
			{
				DoCastVictim(6660);
				shoot_timer = 2000;
			}
			else shoot_timer -= diff;

			if (serpent_timer <= diff)
			{
				DoCastVictim(38859);
				serpent_timer = urand(16000, 23000);
			}
			else serpent_timer -= diff;

			if (scatter_timer <= diff)
			{
				DoCastVictim(19503);
				scatter_timer = urand(10000, 30000);
			}
			else scatter_timer -= diff;

			if (scatter_timer <= diff)
			{
				DoCastVictim(38861);
				scatter_timer = urand(20000, 35000);
			}
			else scatter_timer -= diff;		
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_dragonmaw_archer(Creature* pCreature)
{
	return new npc_dragonmaw_archerAI(pCreature);
}

/*######
## QUEST_TERON_GOREFIEND_I_AM
######*/

#define QUEST_TERON_GOREFIEND_I_AM_ALLIANCE 10645
#define QUEST_TERON_GOREFIEND_I_AM_HORDE 10639

bool QuestAccept_npc_shadowmoon_ancient_spirit(Player* player, Creature* creature, Quest const* quest)
{
	if (quest->GetQuestId() == QUEST_TERON_GOREFIEND_I_AM_ALLIANCE)
	{
		player->CastSpell(player, 37747, true); // Change into Teron Gorefiend
	}

	if (quest->GetQuestId() == QUEST_TERON_GOREFIEND_I_AM_HORDE)
	{
		player->CastSpell(player, 37747, true); // Change into Teron Gorefiend
	}

	return true;
}

#define KARSIUS_SAY_TEXT_1 "Your pathetic attempt to escape will be short lived, Gorefiend. Let the $s go and submit! Even with your armor and weapons, you cannot defeat the ancients!"
#define KARSIUS_SAY_TEXT_2 "What... have you done..."
#define TERON_WHISPER "Waste no time, fool! Use our power to kill those that stand in our way! Destroy Karsius before you are subdued!"

struct npc_karsius_the_ancient_watcherAI : public ScriptedAI
{
	npc_karsius_the_ancient_watcherAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->SetReactState(REACT_AGGRESSIVE);
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
		
		me->MonsterSay(KARSIUS_SAY_TEXT_1, LANG_UNIVERSAL, playerGUID);

		EventCombat = true;
		Combat_Timer = 5000;

		playerGUID = 0;
	}

	uint64 playerGUID;
	uint32 Combat_Timer;

	bool EventCombat;

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(QUEST_TERON_GOREFIEND_I_AM_ALLIANCE) == QUEST_STATUS_INCOMPLETE || plWho->GetQuestStatus(QUEST_TERON_GOREFIEND_I_AM_HORDE) == QUEST_STATUS_INCOMPLETE && plWho->GetDistance(me) < 35.0f)
			{
				switch (me->GetEntry())
				{
				case 21877:
					playerGUID = plWho->GetGUID();
					break;
				}
			}
		}
	}

	void JustDied(Unit* /*killer*/)
	{
		if (Creature* teron = me->FindNearestCreature(21867, 50.0f, true))
		{
			teron->DisappearAndDie();
		}

		me->MonsterYell(KARSIUS_SAY_TEXT_2, LANG_UNIVERSAL, 0);
		me->SummonCreature(21876, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 15000);
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (Creature* teron = me->FindNearestCreature(21867, 100.0f, false))
			{
				me->DisappearAndDie();				
			}

			if (EventCombat)
			{
				if (Combat_Timer <= diff)
				{
					me->setFaction(14);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

					if (Creature* teron = me->FindNearestCreature(21867, 100.0f, true))
					{
						me->AI()->AttackStart(teron);
						teron->Whisper(TERON_WHISPER, playerGUID, false);
					}
				}
				Combat_Timer -= diff;
			}
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_karsius_the_ancient_watcher(Creature* pCreature)
{
	return new npc_karsius_the_ancient_watcherAI(pCreature);
}

struct teron_triggerAI : public ScriptedAI
{
	teron_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->SetReactState(REACT_AGGRESSIVE);
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);	

		teron = false;
	}

	bool teron;

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(QUEST_TERON_GOREFIEND_I_AM_ALLIANCE) == QUEST_STATUS_COMPLETE || plWho->GetQuestStatus(QUEST_TERON_GOREFIEND_I_AM_HORDE) == QUEST_STATUS_COMPLETE && plWho->GetDistance(me) < 65.0f)
			{
				switch (me->GetEntry())
				{
				case 61033:
					if (!teron)
					{
						if (me->FindNearestCreature(21876, 30.0f, true))
						{
							me->SummonCreature(61034, -4539.0f, 1025.99f, 9.28f, 0.71f, TEMPSUMMON_TIMED_DESPAWN, 45000);
							me->DisappearAndDie();
							teron = true;
						}
					}
					break;
				}
			}
		}
	}
};

CreatureAI* GetAI_teron_trigger(Creature* pCreature)
{
	return new teron_triggerAI(pCreature);
}

#define TERON_TEXT_1 "We will meet again... Someday."
#define TERON_TEXT_2 -1910192

struct teron_gorefiend_eventAI : public ScriptedAI
{
	teron_gorefiend_eventAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->SetReactState(REACT_AGGRESSIVE);
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

		TeronGorefiend = true;
		TeronGorefiendSay = false;
		TeronGorefiendYell = false;

		outro_timer = 2000;
	}

	uint32 outro_timer;
	uint32 outro1_timer;
	uint32 leave_timer;

	bool TeronGorefiend;
	bool TeronGorefiendSay;
	bool TeronGorefiendYell;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (TeronGorefiend)
			{
				if (outro_timer <= diff && !TeronGorefiendSay)
				{
					me->MonsterSay(TERON_TEXT_1, LANG_UNIVERSAL, 0);
					outro1_timer = 6000;
					TeronGorefiendSay = true;
				}
				else outro_timer -= diff;

				if (outro1_timer <= diff && !TeronGorefiendYell && TeronGorefiendSay)
				{
					me->MonsterYellToZone(TERON_TEXT_2, LANG_UNIVERSAL, 0);
					leave_timer = 4000;
					TeronGorefiendYell = true;
				}
				else outro1_timer -= diff;

				if (leave_timer <= diff && TeronGorefiendYell)
				{
					me->GetMotionMaster()->MovePath(607607608, false);				
					me->ForcedDespawn(10000);
				}
				else leave_timer -= diff;				
			}
		}
	}
};

CreatureAI* GetAI_teron_gorefiend_event(Creature* pCreature)
{
	return new teron_gorefiend_eventAI(pCreature);
}

/*######
## GO_FORGED_ILLIDARI_BANE_SWORD
######*/

bool GOHello_go_forged_illidari_bane_sword(Player* player, GameObject* go)
{
	player->AddItem(30876, 1);
	return true;
}

/*######
## QUEST_CIPHER_OF_DAMNATION_ARTOR_QUESTS
######*/

#define QUEST_DEMONIC_CRYSTAL_PRISON 10528
#define QUEST_ARTOR_BOW 10537
#define QUEST_CIPHER_OF_DAMNATION_ARTORS_CHARGE 10540
#define ARTOR_SPAWN -1910193

struct npc_artorAI : public ScriptedAI
{
	npc_artorAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->SetLevitate(true);		
		artok = false;
		PlayerGUID = 0;
	}

	uint64 PlayerGUID;
	bool artok;

	void JustSummoned(Creature* summoned)
	{
		summoned->MonsterSay(ARTOR_SPAWN, LANG_UNIVERSAL, PlayerGUID);
	}

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(QUEST_ARTOR_BOW) == QUEST_STATUS_INCOMPLETE || plWho->GetQuestStatus(QUEST_CIPHER_OF_DAMNATION_ARTORS_CHARGE) == QUEST_STATUS_INCOMPLETE && plWho->GetDistance(me) < 10.0f)
			{
				switch (me->GetEntry())
				{
				case 21292:
					if (!artok)
					{
						if (me->FindNearestCreature(21318, 30.0f, true))
							return;

							PlayerGUID = plWho->GetGUID();
						
							me->SummonCreature(21318, -3803.7f, 2602.3f, 90.38f, 5.59f, TEMPSUMMON_TIMED_DESPAWN, 600000);
							me->DealDamage(me, me->GetHealth());
							artok = true;						
					}
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

CreatureAI* GetAI_npc_artor(Creature* pCreature)
{
	return new npc_artorAI(pCreature);
}

bool QuestAccept_npc_artor_spirit(Player* player, Creature* creature, Quest const* quest)
{
	if (quest->GetQuestId() == QUEST_CIPHER_OF_DAMNATION_ARTORS_CHARGE)
	{
		player->CastSpell(player, 36620, true); // Summon Spirit Hunter
	}
	return true;
}

bool ChooseReward_npc_artor(Player* pPlayer, Creature* pCreature, const Quest* quest, uint32 /*item*/)
{
	if (quest->GetQuestId() == QUEST_DEMONIC_CRYSTAL_PRISON)
	{
		CAST_AI(npc_artorAI, pCreature->AI())->PlayerGUID = pPlayer->GetGUID();
		pCreature->SummonCreature(21318, -3803.7f, 2602.3f, 90.38f, 5.59f, TEMPSUMMON_TIMED_DESPAWN, 600000);
		pCreature->DisappearAndDie();
	}
	
	return true;
}

struct npc_tiny_triggerAI : public ScriptedAI
{
	npc_tiny_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		visual_timer = 1000;
	}

	uint32 visual_timer;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (visual_timer <= diff)
			{
				if (Creature* artor = me->FindNearestCreature(21292, 30.0f, true))
				{
					me->CastSpell(artor, 40228, true);
				}
				visual_timer = 5000;
			}
			else visual_timer -= diff;
		}
	}
};

CreatureAI* GetAI_npc_tiny_trigger(Creature* pCreature)
{
	return new npc_tiny_triggerAI(pCreature);
}

/*######
## npc_painmistress_gabrisa
######*/

#define SPELL_CURSE_OF_PAIN 38048

struct npc_painmistress_gabrisaAI : public ScriptedAI
{
	npc_painmistress_gabrisaAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		visual_timer = 1000;
		pain_timer = 5000;
	}

	uint32 visual_timer;
	uint32 pain_timer;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (visual_timer <= diff)
			{
				if (Creature* tiny = me->FindNearestCreature(21310, 30.0f, true))
				{
					me->CastSpell(tiny, 40639, true);
				}
				visual_timer = 5000;
			}
			else visual_timer -= diff;
		}

		if (UpdateVictim())
		{
			if (pain_timer <= diff)
			{
				DoCastVictim(SPELL_CURSE_OF_PAIN);
				pain_timer = 35000;
			}
			else pain_timer -= diff;
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_painmistress_gabrisa(Creature* pCreature)
{
	return new npc_painmistress_gabrisaAI(pCreature);
}

/*######
## npc_illidari_shocktrooper
######*/

#define VENERATUS_SPAWN "There! Destroy him! The Cipher must be recovered!"
#define VENERATUS_DEATH "It is dying! The second part of the Cipher of Damnation is ours, I..."
#define SPIRIT_DESPAWN "I am fading... Return to Ar'tor... Ret... rn... to..."

struct npc_illidari_shocktrooperAI : public ScriptedAI
{
	npc_illidari_shocktrooperAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		canSummon = false;
		veneratus = false;
		charge_timer = 1000;
		cleave_timer = 5000;
	}

	bool canSummon;
	bool veneratus;

	uint32 charge_timer;
	uint32 cleave_timer;

	void JustSummoned(Creature* summoned)
	{
		summoned->MonsterSay(VENERATUS_SPAWN, LANG_UNIVERSAL, 0);
	}

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(QUEST_CIPHER_OF_DAMNATION_ARTORS_CHARGE) == QUEST_STATUS_INCOMPLETE && plWho->GetDistance(me) < 40.0f)
			{
				switch (me->GetEntry())
				{
				case 19802:
					if (!veneratus && !canSummon)
					{
						if (Creature* spirit = me->FindNearestCreature(21332, 40.0f, false))
							return;

						if (Creature* spirit = me->FindNearestCreature(21332, 40.0f, true))
						{
							canSummon = true;
							veneratus = true;
						}														
					}
					break;
				}
			}
		}
	}

	void JustDied(Unit* /*killer*/)
	{
		if (canSummon)
		{
			switch (urand(0, 1))
			{
				case 0:				
					break;
				case 1:
					me->SummonCreature(20427, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
					break;
			}
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;

		if (charge_timer <= diff)
		{
			if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 4, 30, true))
				DoCast(pTarget, 22120);
			charge_timer = 15000;
		}
		else charge_timer -= diff;

		if (cleave_timer <= diff)
		{
			DoCastVictim(15496);
			cleave_timer = 7500;
		}
		else cleave_timer -= diff;

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_illidari_shocktrooper(Creature* pCreature)
{
	return new npc_illidari_shocktrooperAI(pCreature);
}

/*######
## npc_veneratus_the_many
######*/

struct npc_veneratusAI : public ScriptedAI
{
	npc_veneratusAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		VeneDied = false;
	}

	bool VeneDied;
	
	void JustDied(Unit* /*killer*/)
	{
		if (!VeneDied)
		{
			if (Creature* spirit = me->FindNearestCreature(21332, 30.0f, true))
			{
				spirit->MonsterSay(VENERATUS_DEATH, LANG_UNIVERSAL, 0);
				spirit->MonsterSay(SPIRIT_DESPAWN, LANG_UNIVERSAL, 0);
				spirit->DisappearAndDie();
				
				VeneDied = true;
			}
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
			return;
		
		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_veneratus(Creature* pCreature)
{
	return new npc_veneratusAI(pCreature);
}

/*######
## QUEST_CIPHER_OF_DAMNAATION_BORAK_CHARGE
######*/

#define QUEST_TO_CATCH_A_THISTLEHEAD 10570
#define BOKAR_TEXT "Hide! Hide before they see you! Back up!"
#define BOKAR_TEXT_1 "The time to strike is at hand... Terminate Icarus!"

struct npc_icarus_triggerAI : public ScriptedAI
{
	npc_icarus_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
		me->SetReactState(REACT_AGGRESSIVE);

		PlayerGUID = 0;

		WhisperAllow = false;
		Whisper1 = false;
		Whisper2 = false;
	}

	bool WhisperAllow;
	bool Whisper1;
	bool Whisper2;

	uint64 PlayerGUID;

	uint32 Combat_timer;

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(QUEST_TO_CATCH_A_THISTLEHEAD) == QUEST_STATUS_INCOMPLETE && plWho->GetDistance(me) < 30.0f)
			{
				switch (me->GetEntry())
				{
				case 61035:
					PlayerGUID = plWho->GetGUID();
					break;
				}
			}
		}
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (WhisperAllow)
			{
				if (!Whisper1)
				{
					if (Creature* borak = me->FindNearestCreature(21293, 120.0f, true))
					{
						borak->Whisper(BOKAR_TEXT, PlayerGUID, false);
						Combat_timer = 34000;
						Whisper1 = true;
					}
				}

				if (Whisper1 && !Whisper2 && Combat_timer <= diff)
				{
					if (Creature* borak = me->FindNearestCreature(21293, 120.0f, true))
					{
						borak->Whisper(BOKAR_TEXT_1, PlayerGUID, false);
						Whisper2 = true;
						me->DisappearAndDie();
					}
				}
				else Combat_timer -= diff;
			}
		}
	}
};

CreatureAI* GetAI_npc_icarus_trigger(Creature* pCreature)
{
	return new npc_icarus_triggerAI(pCreature);
}

#define ICARIUS_SAY "Their loss is most definitely my gain..."
#define ICARIUS_SAY_1 "Ah, damn it all! Clever trap. Too bad you'll never live to tell the tale..."
#define ICARIUS_SAY_2 "Zarath, I am perfectly capable of making it up this road to Eclipse Point. If we do not deliver the missive, Lord Illidan will have both of our heads! You are dismissed!"

struct npc_envoy_icariusAI : public ScriptedAI
{
	npc_envoy_icariusAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	void Reset()
	{
        me->SetVisible(false);
		me->GetMotionMaster()->MoveTargetedHome();
		me->setFaction(14);
		me->SetReactState(REACT_DEFENSIVE);
		me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

		if (Creature* zarath = me->FindNearestCreature(21410, 30.0f, true))
		{
			zarath->setFaction(35);
            me->SetVisible(false);
			zarath->GetMotionMaster()->MoveTargetedHome();
		}

		ThistleFound = false;
		IkarusSay = false;
		GoObject = false;
		DelObject = false;
	}

	bool ThistleFound;
	bool IkarusSay;
	bool GoObject;
	bool DelObject;

	uint32 Combat_Timer;
	uint32 GoToObject_timer;
	uint32 DelObject_timer;
	uint32 Despawn_Timer;

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (!ThistleFound)
			{
				if (me->FindNearestGameObject(184798, 100.0f))
				{
					if (Creature* IcarusTrig = me->FindNearestCreature(61035, 120.0f, true))
					{
						CAST_AI(npc_icarus_triggerAI, IcarusTrig->AI())->WhisperAllow = true;
					}

                    me->SetVisible(true);
					me->GetMotionMaster()->MovePoint(0, -4055.3f, 1522.17f, 92.348f);

					if (Creature* zarath = me->FindNearestCreature(21410, 30.0f, true))
					{
						zarath->setFaction(14);
						zarath->SetVisible(true);
						zarath->GetMotionMaster()->MovePoint(0, -4062.35f, 1517.71f, 91.783f);
					}

					Combat_Timer = 35000;

					ThistleFound = true;				
				}
			}

			if (ThistleFound)
			{
				if (Combat_Timer <= diff && !IkarusSay)
				{
					if (Creature* zarath = me->FindNearestCreature(21410, 30.0f, true))
					{
						me->MonsterSay(ICARIUS_SAY_2, LANG_UNIVERSAL, 0);
						zarath->DisappearAndDie();
						GoToObject_timer = 1500;
						IkarusSay = true;
					}
				}
				else Combat_Timer -= diff;

				if (!GoObject && IkarusSay && GoToObject_timer <= diff)
				{
					me->MonsterSay(ICARIUS_SAY, LANG_UNIVERSAL, 0);
					me->GetMotionMaster()->MovePoint(0, -4055.91f, 1536.8f, 94.970f);
					GoObject = true;
					DelObject_timer = 4000;

					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					me->SetReactState(REACT_AGGRESSIVE);

					if (GameObject* thistle = me->FindNearestGameObject(184798, 10))
						thistle->Delete();
				}
				else GoToObject_timer -= diff;

				if (!DelObject && GoObject && GoToObject_timer <= diff)
				{
					me->MonsterSay(ICARIUS_SAY_1, LANG_UNIVERSAL, 0);	
					Despawn_Timer = 10000;
					DelObject = true;
				}

				if (DelObject == true && Despawn_Timer <= diff)
				{
					me->DisappearAndDie();
				}
				else Despawn_Timer -= diff;
			}
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_envoy_icarius(Creature* pCreature)
{
	return new npc_envoy_icariusAI(pCreature);
}

/*######
## QUEST_CIPHER_OF_DAMNATION
######*/

//NPC
#define NPC_CYRUKH 21181
#define NPC_GULDAN 17008
#define NPC_SPIRIT 21049
#define NPC_ORONOK 21685
#define NPC_BORAK 21686
#define NPC_GROMTOR 21687
#define NPC_TORLOK 21024
#define NPC_REDEEMED_SPIRIT_OF_AIR 21738
#define NPC_REDEEMED_SPIRIT_OF_FIRE 21740
#define NPC_REDEEMED_SPIRIT_OF_WATER 21741
#define NPC_REDEEMED_SPIRIT_OF_EARTH 21739

//Text
#define GULDAN_1 -1910194
#define GULDAN_2 -1910195
#define GULDAN_3 -1910196
#define GULDAN_4 -1910197
#define GULDAN_5 -1910199
#define GULDAN_6 -1910200

#define ORONOK_1 -1910202
#define ORONOK_2 "We will fight when you are ready."
#define ORONOK_3 "We will set the elements free of your grasp by force!"
#define ORONOK_4 "What say the elements Torlok? I hear only silence."
#define ORONOK_5 "They are redemeed! Then we have won?"
#define ORONOK_6 "We leave, then. Torlok, I have only one request..."
#define ORONOK_7 "Give these to the heroes that made this possible."

#define SPIRIT_EARTH_1 "It is now as it should be, shaman. You have done well."
#define SPIRIT_EARTH_2 "Farewell, mortals... The earthmender knows what fire feels..."

#define SPIRIT_FIRE_1 "Yes... Well enough for the elements that are here but the cipher is known to another... The spirits of fire in turmoil... If this force is not stopped, the world where there these mortals came from will cease."

#define TORLOK_1 "I hear what you hear, brother. Look behind you..."

#define GROMTOR_1 "Could... Could it be over? Is he destroyed?"

#define SPIRIT_STOP -1910198

#define CYRUKH_1 -1910201
#define CYRUKH_2 "Little creature made of flesh, your wish is granted! Death comes for you!"

//Emote
#define EMOTE_VORTEX "A flaming vortex takes shape."

//Spells
#define SPELL_GULDAN_CHANNEL 35996
#define SPELL_CYRUKH_DEFEAT 37235

#define QUEST_CIPHER_OF_DAMNATION 10588

struct npc_guldanAI : public ScriptedAI
{
	npc_guldanAI(Creature* pCreature) : ScriptedAI(pCreature) {}

	uint64 uiPlayerGUID;
	uint32 uiStepsTimer;
	uint32 uiSteps;

	uint64 playerGUID;

	bool GuldanStarted;

	void Reset()
	{
		me->SetStandState(UNIT_STAND_STATE_KNEEL);
		
		GuldanStarted = false;
			 
		uiPlayerGUID = 0;
		uiStepsTimer = 0;
		uiSteps = 0;

		if (Creature* pOronok = me->FindNearestCreature(NPC_ORONOK, 90))
		{
			pOronok->SetVisible(false);
		}
	}

	void MoveInLineOfSight(Unit *pWho)
	{
		if (Player *plWho = pWho->GetCharmerOrOwnerPlayerOrPlayerItself())
		{
			if (plWho->GetQuestStatus(QUEST_CIPHER_OF_DAMNATION) == QUEST_STATUS_INCOMPLETE && plWho->GetDistance(me) < 10.0f)
			{
				switch (me->GetEntry())
				{
				case 17008:
					playerGUID = plWho->GetGUID();
					break;
				}
			}
		}
	}

	uint32 NextStep(uint32 uiSteps)
	{
		Creature* pCyrukh = me->FindNearestCreature(NPC_CYRUKH, 75);
		Creature* pSpirit = me->FindNearestCreature(NPC_SPIRIT, 30);
		Creature* pOronok = me->FindNearestCreature(NPC_ORONOK, 90);
		Creature* pBorak = me->FindNearestCreature(NPC_BORAK, 90);
		Creature* pGromtor = me->FindNearestCreature(NPC_GROMTOR, 90);
		Creature* pTrigger = me->FindNearestCreature(61037, 90);

		if (!pCyrukh && !pSpirit && !pTrigger)
		{
			Reset();
			return 0;
		}

		switch (uiSteps)
		{
		case 1:		
			pCyrukh->SetVisible(false);
			me->SetStandState(UNIT_STAND_STATE_STAND);
			return 1000;
		case 2:
			DoScriptText(GULDAN_1, me);
			me->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
			return 2000;
		case 3:
			me->MonsterTextEmote(EMOTE_VORTEX, 0, false);
			return 3000;
		case 4:
			DoCast(SPELL_CYRUKH_DEFEAT);
			me->CastSpell(me, SPELL_GULDAN_CHANNEL, true);
			DoScriptText(GULDAN_2, me);
			return 5500;
		case 5:
			DoScriptText(GULDAN_3, me);
			return 2000;
		case 6:
			pSpirit->HandleEmoteCommand(EMOTE_ONESHOT_BEG);
			DoScriptText(SPIRIT_STOP, pSpirit);
			return 4800;
		case 7:
			DoScriptText(GULDAN_4, me);
			pCyrukh->SetVisible(true);
			return 500;
		case 8:
			DoScriptText(CYRUKH_1, pCyrukh);
			return 6000;
		case 9:
			DoScriptText(GULDAN_5, me);
			return 1000;
		case 10:
			pTrigger->MonsterYellToZone(ORONOK_1, LANG_UNIVERSAL, playerGUID);
			return 5000;
		case 11:
			if (me->HasAura(SPELL_GULDAN_CHANNEL))
				me->RemoveAurasDueToSpell(SPELL_GULDAN_CHANNEL);
			return 2000;
		case 12:
			DoCast(SPELL_CYRUKH_DEFEAT);
			DoScriptText(GULDAN_6, me);
			me->SetStandState(UNIT_STAND_STATE_KNEEL);
			return 10000;
		case 13:
			me->SummonCreature(NPC_ORONOK, -3605.26f, 1924.91f, 49.53f, 4.88f, TEMPSUMMON_MANUAL_DESPAWN, 0);
			me->SummonCreature(NPC_BORAK, -3601.04f, 1928.46f, 50.054f, 4.85f, TEMPSUMMON_MANUAL_DESPAWN, 0);
			me->SummonCreature(NPC_GROMTOR, -3609.73f, 1926.59f, 50.007f, 4.88f, TEMPSUMMON_MANUAL_DESPAWN, 0);
			return 500;
		case 14:
			pOronok->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
			pOronok->GetMotionMaster()->MovePath(608608600, false);
			pBorak->GetMotionMaster()->MovePath(608608601, false);
			pGromtor->GetMotionMaster()->MovePath(608608602, false);
			return 500;
		default:
			return 0;
		}
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (!GuldanStarted)
		{
			if (me->FindNearestCreature(NPC_CYRUKH, 80.0f, true))
			{
				GuldanStarted = true;
			}
		}

		if (uiStepsTimer <= uiDiff)
		{
			if (GuldanStarted)
				uiStepsTimer = NextStep(++uiSteps);
		}
		else uiStepsTimer -= uiDiff;
	}
};

CreatureAI* GetAI_npc_guldan(Creature* pCreature)
{
	return new npc_guldanAI(pCreature);
}

struct npc_oronokAI : public ScriptedAI
{
	npc_oronokAI(Creature* pCreature) : ScriptedAI(pCreature) { }

	bool StopEvent;
	bool EventResume;
	bool CyrukhYell;
	bool CyrukhDied;

	uint32 uiStepsTimer;
	uint32 uiSteps;

	void Reset()
	{
		me->SetReactState(REACT_AGGRESSIVE);

		uiStepsTimer = 0;
		uiSteps = 0;

		StopEvent = false;
		EventResume = false;
		CyrukhYell = false;
	}

	uint32 NextStep(uint32 uiSteps)
	{
		Creature* pGuldan = me->FindNearestCreature(NPC_GULDAN, 80);
		Creature* pBorak = me->FindNearestCreature(NPC_BORAK, 80);
		Creature* pGromtor = me->FindNearestCreature(NPC_GROMTOR, 80);
		Creature* pTorlok = me->FindNearestCreature(NPC_TORLOK, 15);
		Creature* pSpiritEarth = me->FindNearestCreature(NPC_REDEEMED_SPIRIT_OF_EARTH, 15);
		Creature* pSpiritFire = me->FindNearestCreature(NPC_REDEEMED_SPIRIT_OF_FIRE, 15);

		switch (uiSteps)
		{
		case 1:
			pGromtor->GetMotionMaster()->MoveFollow(me, 1.5f, 1.5f);
			pBorak->GetMotionMaster()->MoveFollow(me, 1.5f, -1.5f);
			return 4000;
		case 2:
			pGromtor->MonsterSay(GROMTOR_1, LANG_UNIVERSAL, 0);
			pGromtor->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
			return 11000;
		case 3:
			me->GetMotionMaster()->MovePath(608608604, false);
			return 15000;
		case 4:
			me->MonsterSay(ORONOK_4, LANG_UNIVERSAL, 0);
			return 5000;
		case 5:
			pTorlok->MonsterSay(TORLOK_1, LANG_UNIVERSAL, 0);
			pTorlok->HandleEmoteCommand(ANIM_EMOTE_TALK);
			return 4000;
		case 6:
			pTorlok->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
			me->SummonCreature(NPC_REDEEMED_SPIRIT_OF_AIR, -3588.31f, 1892.76f, 47.240f, 2.56f, TEMPSUMMON_TIMED_DESPAWN, 34000);
			me->SummonCreature(NPC_REDEEMED_SPIRIT_OF_EARTH, -3599.34f, 1887.27f, 47.240f, 1.82f, TEMPSUMMON_TIMED_DESPAWN, 34000);
			me->SummonCreature(NPC_REDEEMED_SPIRIT_OF_FIRE, -3606.63f, 1887.06f, 47.240f, 1.28f, TEMPSUMMON_TIMED_DESPAWN, 34000);
			me->SummonCreature(NPC_REDEEMED_SPIRIT_OF_WATER, -3592.95f, 1887.88f, 47.240f, 2.044f, TEMPSUMMON_TIMED_DESPAWN, 34000);
			return 4000;
		case 7:
			me->GetMotionMaster()->MovePoint(0, -3600.242f, 1895.89f, 47.240f);
			me->MonsterSay(ORONOK_5, LANG_UNIVERSAL, 0);
			me->HandleEmoteCommand(EMOTE_ONESHOT_SHOUT);
			return 6000;
		case 8:
			pSpiritEarth->MonsterSay(SPIRIT_EARTH_1, LANG_UNIVERSAL, 0);
			return 8000;
		case 9:
			pSpiritFire->MonsterSay(SPIRIT_FIRE_1, LANG_UNIVERSAL, 0);
			return 10000;
		case 10:
			pSpiritEarth->MonsterSay(SPIRIT_EARTH_2, LANG_UNIVERSAL, 0);
			return 5000;
		case 11:
			me->GetMotionMaster()->MovePoint(0, -3600.41f, 1896.81f, 47.240f);
			me->MonsterSay(ORONOK_6, LANG_UNIVERSAL, 0);
			return 5000;
		case 12:
			me->MonsterSay(ORONOK_7, LANG_UNIVERSAL, 0);
			return 11000;
		case 13:
			pGuldan->AI()->EnterEvadeMode();
			return 1000;
		case 14:
			me->DisappearAndDie();
			pBorak->DisappearAndDie();
			pGromtor->DisappearAndDie();
		default:
			return 0;
		}
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (!UpdateVictim())
		{
			if (CyrukhDied)
			{
				if (Creature* gromtor = me->FindNearestCreature(NPC_GROMTOR, 20.0f, true))
				{
					gromtor->GetMotionMaster()->MoveFollow(me, 1.5f, 1.5f);
				}

				if (Creature* borak = me->FindNearestCreature(NPC_BORAK, 20.0f, true))
				{
					borak->GetMotionMaster()->MoveFollow(me, 1.5f, -1.5f);
				}				
			}

			if (uiStepsTimer <= uiDiff)
			{
				if (CyrukhDied)
					uiStepsTimer = NextStep(++uiSteps);
			}
			else uiStepsTimer -= uiDiff;

			if (!StopEvent)
			{
				if (me->FindNearestCreature(NPC_GULDAN, 10.0f, true))
				{
					me->MonsterSay(ORONOK_2, LANG_UNIVERSAL, 0);
					StopEvent = true;
				}
			}

			if (EventResume)
			{
				if (Creature* enraged = me->FindNearestCreature(21050, 100.0f, true))
				{					
					enraged->AI()->AttackStart(me);
				}

				me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
				me->GetMotionMaster()->MovePath(608608603, false);

				if (Creature* gromtor = me->FindNearestCreature(NPC_GROMTOR, 20.0f, true))
				{
					gromtor->GetMotionMaster()->MovePath(608608606, false);					
				}

				if (Creature* borak = me->FindNearestCreature(NPC_BORAK, 20.0f, true))
				{
					borak->GetMotionMaster()->MovePath(608608605, false);			
				}
			}

			if (me->FindNearestCreature(21348, 6.0f, true))
			{
				if (!CyrukhYell)
				{
					if (Creature* cyrukh = me->FindNearestCreature(NPC_CYRUKH, 100.0f, true))
					{
						cyrukh->MonsterYell(CYRUKH_2, LANG_UNIVERSAL, 0);
						cyrukh->setFaction(14);
						cyrukh->SetReactState(REACT_AGGRESSIVE);
						cyrukh->GetMotionMaster()->MovePoint(0, -3663.570f, 1831.090f, 60.117f);
						CyrukhYell = true;

						if (Creature* gromtor = me->FindNearestCreature(NPC_GROMTOR, 100.0f, true))
						{											
							gromtor->SetReactState(REACT_AGGRESSIVE);
						}

						if (Creature* borak = me->FindNearestCreature(NPC_BORAK, 100.0f, true))
						{					
							borak->SetReactState(REACT_AGGRESSIVE);
						}
					}
				}
			}
		}

		if (UpdateVictim())
		{
			
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_oronok(Creature* pCreature)
{
	return new npc_oronokAI(pCreature);
}

#define GOSSIP_OSE "I am ready, Oronok. Let us destroy Cyrukh and free the elements!"

bool GossipHello_npc_oronok(Player* player, Creature* _Creature)
{
	if (_Creature->FindNearestCreature(NPC_GULDAN, 9.0f, true))
	{
		if (player->GetQuestStatus(QUEST_CIPHER_OF_DAMNATION) == QUEST_STATUS_INCOMPLETE)
			player->ADD_GOSSIP_ITEM(0, GOSSIP_OSE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

		player->SEND_GOSSIP_MENU(10421, _Creature->GetGUID());
	}
	return true;
}

bool GossipSelect_npc_oronok(Player* player, Creature* _Creature, uint32 sender, uint32 action)
{
	switch (action)
	{
	case GOSSIP_ACTION_INFO_DEF + 1:
		_Creature->MonsterYell(ORONOK_3, LANG_UNIVERSAL, 0);
		CAST_AI(npc_oronokAI, _Creature->AI())->EventResume = true;
		player->CLOSE_GOSSIP_MENU();
		break;
	}
	return true;
}

struct npc_cyrukh_the_firelordAI : public ScriptedAI
{
	npc_cyrukh_the_firelordAI(Creature* pCreature) : ScriptedAI(pCreature) { }

	bool CyrukhDied;
	bool GetOroGUID;
	uint64 OronokGUID;

	void Reset()
	{
		CyrukhDied = false;
		GetOroGUID = false;
		OronokGUID = 0;
	}

	void JustDied(Unit* killer)
	{	
		if (Creature* oronok = me->GetMap()->GetCreature(OronokGUID))
		{
			CAST_AI(npc_oronokAI, oronok->AI())->CyrukhDied = true;
		}
	}

	void UpdateAI(const uint32 uiDiff)
	{
		if (!UpdateVictim())
		{
			if (!GetOroGUID)
			{
				if (Creature* oronok = me->FindNearestCreature(NPC_ORONOK, 100.0f, true))
				{
					OronokGUID = oronok->GetGUID();
					GetOroGUID = true;
				}
			}
		}

		DoMeleeAttackIfReady();
	}
};

CreatureAI* GetAI_npc_cyrukh_the_firelord(Creature* pCreature)
{
	return new npc_cyrukh_the_firelordAI(pCreature);
}

void AddSC_shadowmoon_valley()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "mob_mature_netherwing_drake";
    newscript->GetAI = &GetAI_mob_mature_netherwing_drake;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_enslaved_netherwing_drake";
    newscript->GetAI = &GetAI_mob_enslaved_netherwing_drake;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dragonmaw_peon";
    newscript->GetAI = &GetAI_mob_dragonmaw_peon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_drake_dealer_hurlunk";
    newscript->pGossipHello =  &GossipHello_npc_drake_dealer_hurlunk;
    newscript->pGossipSelect = &GossipSelect_npc_drake_dealer_hurlunk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npcs_flanis_swiftwing_and_kagrosh";
    newscript->pGossipHello =  &GossipHello_npcs_flanis_swiftwing_and_kagrosh;
    newscript->pGossipSelect = &GossipSelect_npcs_flanis_swiftwing_and_kagrosh;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_murkblood_overseer";
    newscript->pGossipHello =  &GossipHello_npc_murkblood_overseer;
    newscript->pGossipSelect = &GossipSelect_npc_murkblood_overseer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_neltharaku";
    newscript->pGossipHello =  &GossipHello_npc_neltharaku;
    newscript->pGossipSelect = &GossipSelect_npc_neltharaku;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_karynaku";
	newscript->GetAI = &GetAI_npc_karynaku;
    newscript->pQuestAccept = &QuestAccept_npc_karynaku;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_oronok_tornheart";
	newscript->GetAI = &GetAI_npc_oronok_tornheart;
    newscript->pGossipHello =  &GossipHello_npc_oronok_tornheart;
    newscript->pGossipSelect = &GossipSelect_npc_oronok_tornheart;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_overlord_morghor";
    newscript->GetAI = &GetAI_npc_overlord_morghorAI;
    newscript->pQuestAccept = &QuestAccept_npc_overlord_morghor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_earthmender_wilda";
    newscript->GetAI = &GetAI_npc_earthmender_wildaAI;
    newscript->pQuestAccept = &QuestAccept_npc_earthmender_wilda;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_lord_illidan_stormrage";
    newscript->GetAI = &GetAI_npc_lord_illidan_stormrage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_torloth";
    newscript->GetAI = &GetAI_mob_torloth;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_crystal_prison";
    newscript->pGOQuestAccept = &GOQuestAccept_GO_crystal_prison;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_enraged_spirit";
    newscript->GetAI = &GetAI_npc_enraged_spirit;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_jovaan";
    newscript->GetAI = &GetAI_npc_jovaan;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_grand_commander_ruusk";
    newscript->pGossipHello =  &GossipHello_npc_grand_commander_ruusk;
    newscript->pGossipSelect = &GossipSelect_npc_grand_commander_ruusk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_azaloth";
    newscript->GetAI = &GetAI_npc_azaloth;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_sunfurywarlock";
    newscript->GetAI = &GetAI_npc_sunfurywarlock;
    newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "legion_infernal_summon_trigger";
	newscript->GetAI = &GetAI_legion_infernal_summon_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "legion_infernal_summoner_trigger";
	newscript->GetAI = &GetAI_legion_infernal_summoner_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "legion_hold_device_trigger";
	newscript->GetAI = &GetAI_legion_hold_device_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_deathbringer_jovaan";
	newscript->GetAI = &GetAI_npc_deathbringer_jovaan;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_shadowcouncil_warlock";
	newscript->GetAI = &GetAI_npc_shadowcouncil_warlock;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_plexi";
	newscript->pQuestAccept = &QuestAccept_npc_plexi;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_nakansi";
	newscript->pQuestAccept = &QuestAccept_npc_nakansi;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_zuluhed_the_whacked";
	newscript->GetAI = &GetAI_npc_zuluhed_the_whacked;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_dragonmaw_archer";
	newscript->GetAI = &GetAI_npc_dragonmaw_archer;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_shadowmoon_ancient_spirit";
	newscript->pQuestAccept = &QuestAccept_npc_shadowmoon_ancient_spirit;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_karsius_the_ancient_watcher";
	newscript->GetAI = &GetAI_npc_karsius_the_ancient_watcher;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "teron_trigger";
	newscript->GetAI = &GetAI_teron_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "teron_gorefiend_event";
	newscript->GetAI = &GetAI_teron_gorefiend_event;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "go_forged_illidari_bane_sword";
	newscript->pGOHello = &GOHello_go_forged_illidari_bane_sword;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_artor";
	newscript->GetAI = &GetAI_npc_artor;
	newscript->pChooseReward = &ChooseReward_npc_artor;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_artor_spirit";
	newscript->pQuestAccept = &QuestAccept_npc_artor_spirit;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_tiny_trigger";
	newscript->GetAI = &GetAI_npc_tiny_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_painmistress_gabrisa";
	newscript->GetAI = &GetAI_npc_painmistress_gabrisa;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_illidari_shocktrooper";
	newscript->GetAI = &GetAI_npc_illidari_shocktrooper;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_veneratus";
	newscript->GetAI = &GetAI_npc_veneratus;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_envoy_icarius";
	newscript->GetAI = &GetAI_npc_envoy_icarius;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_icarus_trigger";
	newscript->GetAI = &GetAI_npc_icarus_trigger;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_guldan";
	newscript->GetAI = &GetAI_npc_guldan;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_oronok";
	newscript->GetAI = &GetAI_npc_oronok;
	newscript->pGossipHello = &GossipHello_npc_oronok;
	newscript->pGossipSelect = &GossipSelect_npc_oronok;
	newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_cyrukh_the_firelord";
	newscript->GetAI = &GetAI_npc_cyrukh_the_firelord;
    newscript->RegisterSelf();
}

