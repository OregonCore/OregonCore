/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Shadowmoon_Valley
SD%Complete: 100
SDComment: Quest support: 10519, 10583, 10601, 10814, 10804, 10854, 10458, 10481, 10480, 11082, 10781, 10451. Vendor Drake Dealer Hurlunk.
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
EndContentData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"

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

    void EnterCombat(Unit* who) { }

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
            me->AddUnitMovementFlag(MOVEFLAG_ONTRANSPORT | MOVEFLAG_LEVITATING);
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
            me->RemoveUnitMovementFlag(MOVEFLAG_ONTRANSPORT | MOVEFLAG_LEVITATING);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (IsEating)
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
                    plr->KilledMonsterCredit(22131, me->GetGUID());
                    Evade = true;
                    PlayerGUID = 0;
                }
            }
        } else EatTimer -= diff;

        if (Evade)
            if (ResetTimer <= diff)
            {
                EnterEvadeMode();
                return;
            } else ResetTimer -= diff;

        if (!UpdateVictim())
            return;

        if (CastTimer <= diff)
        {
            DoCast(me->getVictim(), SPELL_NETHER_BREATH);
            CastTimer = 5000;
        } else CastTimer -= diff;

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
        me->RemoveUnitMovementFlag(MOVEFLAG_ONTRANSPORT | MOVEFLAG_LEVITATING);
        me->SetVisibility(VISIBILITY_ON);
    }

    void EnterCombat(Unit* who) { }

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
            me->SetVisibility(VISIBILITY_OFF);
            me->RemoveUnitMovementFlag(MOVEFLAG_ONTRANSPORT | MOVEFLAG_LEVITATING);
            me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            me->RemoveCorpse();
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
        {
            if (Tapped)
                if (FlyTimer <= diff)
            {
                Tapped = false;
                if (PlayerGUID)
                {
                    Player* plr = Unit::GetPlayer(*me, PlayerGUID);
                    if (plr && plr->GetQuestStatus(10854) == QUEST_STATUS_INCOMPLETE)
                    {
                        plr->KilledMonsterCredit(22316, me->GetGUID());
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

                        me->AddUnitMovementFlag(MOVEFLAG_ONTRANSPORT | MOVEFLAG_LEVITATING);
                        me->GetMotionMaster()->MovePoint(1, pos);
                    }
                }
            } else FlyTimer -= diff;
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

    void Reset()
    {
        PlayerGUID = 0;
        Tapped = false;
        PoisonTimer = 0;
    }

    void EnterCombat(Unit* who) { }

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

            me->RemoveUnitMovementFlag(MOVEFLAG_WALK_MODE);
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
            if (PoisonTimer <= diff)
        {
            if (PlayerGUID)
            {
                Player* plr = Unit::GetPlayer(*me, PlayerGUID);
                if (plr && plr->GetQuestStatus(11020) == QUEST_STATUS_INCOMPLETE)
                    plr->KilledMonsterCredit(23209, me->GetGUID());
            }
            PoisonTimer = 0;
            me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        } else PoisonTimer -= diff;
    }
};

CreatureAI* GetAI_mob_dragonmaw_peon(Creature* pCreature)
{
    return new mob_dragonmaw_peonAI(pCreature);
}

/*######
## npc_drake_dealer_hurlunk
######*/

bool GossipHello_npc_drake_dealer_hurlunk(Player *player, Creature* pCreature)
{
    if (pCreature->isVendor() && player->GetReputationRank(1015) == REP_EXALTED)
        player->ADD_GOSSIP_ITEM(1, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_drake_dealer_hurlunk(Player *player, Creature* pCreature, uint32 sender, uint32 action)
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

bool GossipHello_npcs_flanis_swiftwing_and_kagrosh(Player *player, Creature* pCreature)
{
    if (player->GetQuestStatus(10583) == QUEST_STATUS_INCOMPLETE && !player->HasItemCount(30658,1,true))
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HSK1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    if (player->GetQuestStatus(10601) == QUEST_STATUS_INCOMPLETE && !player->HasItemCount(30659,1,true))
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HSK2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npcs_flanis_swiftwing_and_kagrosh(Player *player, Creature* pCreature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF+1)
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 30658, 1, false);
        if (msg == EQUIP_ERR_OK)
        {
            player->StoreNewItem(dest, 30658, 1, true);
            player->PlayerTalkClass->ClearMenus();
        }
    }
    if (action == GOSSIP_ACTION_INFO_DEF+2)
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 30659, 1, false);
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

bool GossipHello_npc_murkblood_overseer(Player *player, Creature* pCreature)
{
    if (player->GetQuestStatus(QUEST_11082) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HMO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    player->SEND_GOSSIP_MENU(10940, pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_murkblood_overseer(Player *player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_SMO1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                                                            //correct id not known
            player->SEND_GOSSIP_MENU(10940, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_SMO2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                                                            //correct id not known
            player->SEND_GOSSIP_MENU(10940, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_SMO3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
                                                            //correct id not known
            player->SEND_GOSSIP_MENU(10940, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_SMO4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
                                                            //correct id not known
            player->SEND_GOSSIP_MENU(10940, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_SMO5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
                                                            //correct id not known
            player->SEND_GOSSIP_MENU(10940, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
                                                            //correct id not known
            player->SEND_GOSSIP_MENU(10940, pCreature->GetGUID());
            pCreature->CastSpell(player,41121,false);
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

bool GossipHello_npc_neltharaku(Player *player, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        player->PrepareQuestMenu(pCreature->GetGUID());

    if (player->GetQuestStatus(10814) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    player->SEND_GOSSIP_MENU(10613, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_neltharaku(Player *player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_SN1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(10614, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_SN2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            player->SEND_GOSSIP_MENU(10615, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_SN3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
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

bool GossipHello_npc_oronok_tornheart(Player *player, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        player->PrepareQuestMenu(pCreature->GetGUID());
    if (pCreature->isVendor())
        player->ADD_GOSSIP_ITEM(1, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    if (player->GetQuestStatus(10519) == QUEST_STATUS_INCOMPLETE)
    {
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        player->SEND_GOSSIP_MENU(10312, pCreature->GetGUID());
    } else
    player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_oronok_tornheart(Player *player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_TRADE:
            player->SEND_VENDORLIST(pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(10313, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->SEND_GOSSIP_MENU(10314, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            player->SEND_GOSSIP_MENU(10315, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            player->SEND_GOSSIP_MENU(10316, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            player->SEND_GOSSIP_MENU(10317, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
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

bool QuestAccept_npc_karynaku(Player* player, Creature* creature, Quest const* quest)
{
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
    npc_overlord_morghorAI(Creature *c) : ScriptedAI(c) {}

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

    void EnterCombat(Unit* who){}

    void StartEvent()
    {
        me->SetUInt32Value(UNIT_NPC_FLAGS, 0);
        me->SetUInt32Value(UNIT_FIELD_BYTES_1,0);
        Unit* Illidan = me->SummonCreature(C_ILLIDAN, -5107.83f, 602.584f, 85.2393f, 4.92598f, TEMPSUMMON_CORPSE_DESPAWN, 0);
        if (Illidan)
        {
            IllidanGUID = Illidan->GetGUID();
            Illidan->SetVisibility(VISIBILITY_OFF);
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

        switch(Step)
        {
        case 0: return 0; break;
        case 1: me->GetMotionMaster()->MovePoint(0, -5104.41f, 595.297f, 85.6838f); return 9000; break;
        case 2: DoScriptText(OVERLORD_YELL_1, me, plr); return 4500; break;
        case 3: me->SetInFront(plr); return 3200;  break;
        case 4: DoScriptText(OVERLORD_SAY_2, me, plr); return 2000; break;
        case 5: Illi->SetVisibility(VISIBILITY_ON);
             Illi->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE); return 350; break;
        case 6:
            Illi->CastSpell(Illi, SPELL_ONE, true);
            Illi->SetUInt64Value(UNIT_FIELD_TARGET, me->GetGUID());
            me->SetUInt64Value(UNIT_FIELD_TARGET, IllidanGUID);
            return 2000; break;
        case 7: DoScriptText(OVERLORD_YELL_2, me); return 4500; break;
        case 8: me->SetUInt32Value(UNIT_FIELD_BYTES_1, 8); return 2500; break;
        case 9: DoScriptText(OVERLORD_SAY_3, me); return 6500; break;
        case 10: DoScriptText(LORD_ILLIDAN_SAY_1, Illi); return 5000;  break;
        case 11: DoScriptText(OVERLORD_SAY_4, me, plr); return 6000; break;
        case 12: DoScriptText(LORD_ILLIDAN_SAY_2, Illi); return 5500; break;
        case 13: DoScriptText(LORD_ILLIDAN_SAY_3, Illi); return 4000; break;
        case 14: Illi->SetUInt64Value(UNIT_FIELD_TARGET, PlayerGUID); return 1500; break;
        case 15: DoScriptText(LORD_ILLIDAN_SAY_4, Illi); return 1500; break;
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
                Step = 30; return 100;
            }
            break;
        case 17: DoScriptText(LORD_ILLIDAN_SAY_5, Illi); return 5000; break;
        case 18: DoScriptText(LORD_ILLIDAN_SAY_6, Illi); return 5000; break;
        case 19: DoScriptText(LORD_ILLIDAN_SAY_7, Illi); return 5000; break;
        case 20:
            Illi->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
            Illi->AddUnitMovementFlag(MOVEFLAG_ONTRANSPORT | MOVEFLAG_LEVITATING);
            return 500; break;
        case 21: DoScriptText(OVERLORD_SAY_5, me); return 500; break;
        case 22:
            Illi->SetVisibility(VISIBILITY_OFF);
            Illi->setDeathState(JUST_DIED);
            return 1000; break;
        case 23: me->SetUInt32Value(UNIT_FIELD_BYTES_1,0); return 2000; break;
        case 24: me->SetUInt64Value(UNIT_FIELD_TARGET, PlayerGUID); return 5000; break;
        case 25: DoScriptText(OVERLORD_SAY_6, me); return 2000; break;
        case 26:
            if (plr)
                plr->GroupEventHappens(QUEST_LORD_ILLIDAN_STORMRAGE, me);
            return 6000; break;
        case 27:
            {
            Unit* Yarzill = me->FindNearestCreature(C_YARZILL, 50);
            if (Yarzill)
                Yarzill->SetUInt64Value(UNIT_FIELD_TARGET, PlayerGUID);
            return 500; }break;
        case 28:
            plr->RemoveAurasDueToSpell(SPELL_TWO);
            plr->RemoveAurasDueToSpell(41519);
            plr->CastSpell(plr, SPELL_THREE, true);
            plr->CastSpell(plr, SPELL_FOUR, true);
            return 1000; break;
        case 29:
            {
            Unit* Yarzill = me->FindNearestCreature(C_YARZILL, 50);
            if (Yarzill)
                DoScriptText(YARZILL_THE_MERC_SAY, Yarzill, plr);
            return 5000; }break;
        case 30:
            {
            Unit* Yarzill = me->FindNearestCreature(C_YARZILL, 50);
            if (Yarzill)
                Yarzill->SetUInt64Value(UNIT_FIELD_TARGET, 0);
            return 5000; }break;
        case 31:
            {
            Unit* Yarzill = me->FindNearestCreature(C_YARZILL, 50);
            if (Yarzill)
                Yarzill->CastSpell(plr, 41540, true);
            return 1000;}break;
        case 32: me->GetMotionMaster()->MovePoint(0, -5085.77f, 577.231f, 86.6719f); return 5000; break;
        case 33: Reset(); return 100; break;

        default : return 0;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!ConversationTimer)
            return;

        if (ConversationTimer <= diff)
        {
            if (Event && IllidanGUID && PlayerGUID)
            {
                ConversationTimer = NextStep(++Step);
            }
        } else ConversationTimer -= diff;
    }
};

CreatureAI* GetAI_npc_overlord_morghorAI(Creature* pCreature)
{
return new npc_overlord_morghorAI(pCreature);
}

bool QuestAccept_npc_overlord_morghor(Player *player, Creature* pCreature, const Quest *_Quest)
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
    npc_earthmender_wildaAI(Creature *c) : npc_escortAI(c) {}

    bool Completed;

    void EnterCombat(Unit *who)
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

        switch(i)
        {
               case 0: DoScriptText(SAY_START, me, player); break;
               case 13: DoScriptText(SAY_PROGRESS1, me, player);
                   SummonAssassin();
                   break;
               case 14: SummonAssassin(); break;
               case 15: DoScriptText(SAY_PROGRESS3, me, player); break;
               case 19:
                   switch(rand()%3)
                   {
                   case 0: DoScriptText(SAY_PROGRESS2, me, player); break;
                   case 1: DoScriptText(SAY_PROGRESS4, me, player); break;
                   case 2: DoScriptText(SAY_PROGRESS5, me, player); break;
                   }
                   break;
               case 20: SummonAssassin(); break;
               case 26:
                   switch(rand()%3)
                   {
                   case 0: DoScriptText(SAY_PROGRESS2, me, player); break;
                   case 1: DoScriptText(SAY_PROGRESS4, me, player); break;
                   case 2: DoScriptText(SAY_PROGRESS5, me, player); break;
                   }
                   break;
               case 27: SummonAssassin(); break;
               case 33:
                   switch(rand()%3)
                   {
                   case 0: DoScriptText(SAY_PROGRESS2, me, player); break;
                   case 1: DoScriptText(SAY_PROGRESS4, me, player); break;
                   case 2: DoScriptText(SAY_PROGRESS5, me, player); break;
                   }
                   break;
               case 34: SummonAssassin(); break;
               case 37:
                   switch(rand()%3)
                   {
                   case 0: DoScriptText(SAY_PROGRESS2, me, player); break;
                   case 1: DoScriptText(SAY_PROGRESS4, me, player); break;
                   case 2: DoScriptText(SAY_PROGRESS5, me, player); break;
                   }
                   break;
               case 38: SummonAssassin(); break;
               case 39: DoScriptText(SAY_PROGRESS6, me, player); break;
               case 43:
                   switch(rand()%3)
                   {
                   case 0: DoScriptText(SAY_PROGRESS2, me, player); break;
                   case 1: DoScriptText(SAY_PROGRESS4, me, player); break;
                   case 2: DoScriptText(SAY_PROGRESS5, me, player); break;
                   }
                   break;
               case 44: SummonAssassin(); break;
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
               switch(rand()%2)
               {
               case 0: DoScriptText(ASSASSIN_SAY_AGGRO1, CoilskarAssassin, player); break;
               case 1: DoScriptText(ASSASSIN_SAY_AGGRO2, CoilskarAssassin, player); break;
               }
               CAST_CRE(CoilskarAssassin)->AI()->AttackStart(me);
           }
           else error_log("OSCR ERROR: Coilskar Assassin couldn't be summmoned");
       }

       void JustDied(Unit* killer)
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

static TorlothCinematic TorlothAnim[]=
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

static Location SpawnLocation[]=
{
    {-4615.8556f, 1342.2532f, 139.9f, 1.612f},              // Illidari Soldier
    {-4598.9365f, 1377.3182f, 139.9f, 3.917f},              // Illidari Soldier
    {-4598.4697f, 1360.8999f, 139.9f, 2.427f},              // Illidari Soldier
    {-4589.3599f, 1369.1061f, 139.9f, 3.165f},              // Illidari Soldier
    {-4608.3477f, 1386.0076f, 139.9f, 4.108f},              // Illidari Soldier
    {-4633.1889f, 1359.8033f, 139.9f, 0.949f},              // Illidari Soldier
    {-4623.5791f, 1351.4574f, 139.9f, 0.971f},              // Illidari Soldier
    {-4607.2988f, 1351.6099f, 139.9f, 2.416f},              // Illidari Soldier
    {-4633.7764f, 1376.0417f, 139.9f, 5.608f},              // Illidari Soldier
    {-4600.2461f, 1369.1240f, 139.9f, 3.056f},              // Illidari Mind Breaker
    {-4631.7808f, 1367.9459f, 139.9f, 0.020f},              // Illidari Mind Breaker
    {-4600.2461f, 1369.1240f, 139.9f, 3.056f},              // Illidari Highlord
    {-4631.7808f, 1367.9459f, 139.9f, 0.020f},              // Illidari Highlord
    {-4615.5586f, 1353.0031f, 139.9f, 1.540f},              // Illidari Highlord
    {-4616.4736f, 1384.2170f, 139.9f, 4.971f},              // Illidari Highlord
    {-4627.1240f, 1378.8752f, 139.9f, 2.544f}               // Torloth The Magnificent
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

static WaveData WavesInfo[]=
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
    mob_torlothAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

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

        switch(m_uiAnimationCount)
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
                DoScriptText(SAY_EVENT_COMPLETED, pLordIllidan, pPlayer);
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
                DoCast(me->getVictim(), SPELL_CLEAVE);
                m_uiCleaveTimer = 15000;
            }
            else
                m_uiCleaveTimer -= uiDiff;

            if (m_uiShadowfuryTimer < uiDiff)
            {
                DoCast(me->getVictim(), SPELL_SHADOWFURY);
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

struct npc_lord_illidan_stormrageAI : public Scripted_NoMovementAI
{
    npc_lord_illidan_stormrageAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature) {Reset();}

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

                if (!urand(0,2) && uiFelguardCount < 2)
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

    void SummonedCreatureDespawn(Creature* pCreature)
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
                    if (!pMember->isAlive())
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

CreatureAI* GetAI_npc_lord_illidan_stormrage(Creature* (pCreature))
{
    return new npc_lord_illidan_stormrageAI(pCreature);
}

/*#####
# go_crystal_prison : GameObject that begins the event and hands out quest
######*/
bool GOQuestAccept_GO_crystal_prison(Player* pPlayer, GameObject* pGo, Quest const* pQuest)
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
    npc_enraged_spiritAI(Creature *c) : ScriptedAI(c) {}

    void Reset()   { }

    void EnterCombat(Unit *who){}

    void JustDied(Unit* killer)
    {
        // always spawn spirit on death
        // if totem around
        // move spirit to totem and cast kill count
        uint32 entry = 0;
        uint32 credit = 0;

        switch(me->GetEntry()) {
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
                 Summoned->GetMotionMaster()->MovePoint(0,totemOspirits->GetPositionX(), totemOspirits->GetPositionY(), Summoned->GetPositionZ());

                 Player* Owner = (Player*)totemOspirits->GetOwner();
                 if (Owner)
                     // DoCast(Owner, credit); -- not working!
                     Owner->KilledMonsterCredit(credit, Summoned->GetGUID());
                 DoCast(totemOspirits,SPELL_SOUL_CAPTURED);
             }
        }
    }
};

CreatureAI* GetAI_npc_enraged_spirit(Creature* pCreature)
{
return new npc_enraged_spiritAI(pCreature);
}

/*#####
#
######*/

void AddSC_shadowmoon_valley()
{
    Script *newscript;

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
    newscript->pQuestAccept = &QuestAccept_npc_karynaku;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_oronok_tornheart";
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
}

