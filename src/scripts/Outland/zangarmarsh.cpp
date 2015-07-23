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
SDName: Zangarmarsh
SD%Complete: 100
SDComment: Quest support: 9785, 9803, 10009, 9752. Mark Of ... buffs.
SDCategory: Zangarmarsh
EndScriptData */

/* ContentData
npcs_ashyen_and_keleth
npc_cooshcoosh
npc_elder_kuruti
npc_mortog_steamhead
npc_kayra_longmane
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ObjectMgr.h"
#include "ScriptedGossip.h"

/*######
## npcs_ashyen_and_keleth
######*/

#define GOSSIP_ITEM_BLESS_ASH     "Grant me your mark, wise ancient."
#define GOSSIP_ITEM_BLESS_KEL     "Grant me your mark, mighty ancient."
#define GOSSIP_REWARD_BLESS       -1000207
//#define TEXT_BLESSINGS        "<You need higher standing with Cenarion Expedition to recive a blessing.>"

bool GossipHello_npcs_ashyen_and_keleth(Player* player, Creature* pCreature)
{
    if (player->GetReputationRank(942) > REP_NEUTRAL)
    {
        if (pCreature->GetEntry() == 17900)
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_BLESS_ASH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        if (pCreature->GetEntry() == 17901)
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_BLESS_KEL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    }
    player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npcs_ashyen_and_keleth(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pCreature->setPowerType(POWER_MANA);
        pCreature->SetMaxPower(POWER_MANA, 200);            //set a "fake" mana value, we can't depend on database doing it in this case
        pCreature->SetPower(POWER_MANA, 200);

        if (pCreature->GetEntry() == 17900)                //check which creature we are dealing with
        {
            switch (player->GetReputationRank(942))
            {
            //mark of lore
            case REP_FRIENDLY:
                pCreature->CastSpell(player, 31808, true);
                DoScriptText(GOSSIP_REWARD_BLESS, pCreature);
                break;
            case REP_HONORED:
                pCreature->CastSpell(player, 31810, true);
                DoScriptText(GOSSIP_REWARD_BLESS, pCreature);
                break;
            case REP_REVERED:
                pCreature->CastSpell(player, 31811, true);
                DoScriptText(GOSSIP_REWARD_BLESS, pCreature);
                break;
            case REP_EXALTED:
                pCreature->CastSpell(player, 31815, true);
                DoScriptText(GOSSIP_REWARD_BLESS, pCreature);
                break;
            default:
                break;
            }
        }

        if (pCreature->GetEntry() == 17901)
        {
            switch (player->GetReputationRank(942))         //mark of war
            {
            case REP_FRIENDLY:
                pCreature->CastSpell(player, 31807, true);
                DoScriptText(GOSSIP_REWARD_BLESS, pCreature);
                break;
            case REP_HONORED:
                pCreature->CastSpell(player, 31812, true);
                DoScriptText(GOSSIP_REWARD_BLESS, pCreature);
                break;
            case REP_REVERED:
                pCreature->CastSpell(player, 31813, true);
                DoScriptText(GOSSIP_REWARD_BLESS, pCreature);
                break;
            case REP_EXALTED:
                pCreature->CastSpell(player, 31814, true);
                DoScriptText(GOSSIP_REWARD_BLESS, pCreature);
                break;
            default:
                break;
            }
        }
        player->CLOSE_GOSSIP_MENU();
        player->TalkedToCreature(pCreature->GetEntry(), pCreature->GetGUID());
    }
    return true;
}

/*######
## npc_cooshcoosh
######*/

#define GOSSIP_COOSH            "You owe Sim'salabim money. Hand them over or die!"

#define FACTION_HOSTILE_CO      45
#define FACTION_FRIENDLY_CO     35

#define SPELL_LIGHTNING_BOLT    9532

struct npc_cooshcooshAI : public ScriptedAI
{
    npc_cooshcooshAI(Creature* c) : ScriptedAI(c) {}

    uint32 LightningBolt_Timer;

    void Reset()
    {
        LightningBolt_Timer = 2000;
        me->setFaction(FACTION_FRIENDLY_CO);
    }

    void EnterCombat(Unit* /*who*/) {}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (LightningBolt_Timer <= diff)
        {
            DoCastVictim(SPELL_LIGHTNING_BOLT);
            LightningBolt_Timer = 5000;
        }
        else LightningBolt_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_npc_cooshcoosh(Creature* pCreature)
{
    return new npc_cooshcooshAI (pCreature);
}

bool GossipHello_npc_cooshcoosh(Player* player, Creature* pCreature)
{
    if (player->GetQuestStatus(10009) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(1, GOSSIP_COOSH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    player->SEND_GOSSIP_MENU(9441, pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_cooshcoosh(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF)
    {
        player->CLOSE_GOSSIP_MENU();
        pCreature->setFaction(FACTION_HOSTILE_CO);
        ((npc_cooshcooshAI*)pCreature->AI())->AttackStart(player);
    }
    return true;
}

/*######
## npc_elder_kuruti
######*/

#define GOSSIP_ITEM_KUR1 "Offer treat"
#define GOSSIP_ITEM_KUR2 "Im a messenger for Draenei"
#define GOSSIP_ITEM_KUR3 "Get message"

bool GossipHello_npc_elder_kuruti(Player* player, Creature* pCreature)
{
    if (player->GetQuestStatus(9803) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KUR1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    player->SEND_GOSSIP_MENU(9226, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_elder_kuruti(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    switch (action)
    {
    case GOSSIP_ACTION_INFO_DEF:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KUR2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(9227, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 1:
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KUR3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->SEND_GOSSIP_MENU(9229, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:
        {
            if (!player->HasItemCount(24573, 1))
            {
                ItemPosCountVec dest;
                uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 24573, 1, NULL);
                if (msg == EQUIP_ERR_OK)
                    player->StoreNewItem(dest, 24573, true);
                else
                    player->SendEquipError(msg, NULL, NULL);
            }
            player->SEND_GOSSIP_MENU(9231, pCreature->GetGUID());
            break;
        }
    }
    return true;
}

/*######
## npc_mortog_steamhead
######*/

bool GossipHello_npc_mortog_steamhead(Player* player, Creature* pCreature)
{
    if (pCreature->isVendor() && player->GetReputationRank(942) == REP_EXALTED)
        player->ADD_GOSSIP_ITEM(1, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_mortog_steamhead(Player* player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_TRADE)
        player->SEND_VENDORLIST(pCreature->GetGUID());
    return true;
}

/*######
## npc_kayra_longmane
######*/

enum eKayra
{
    SAY_START           = -1000343,
    SAY_AMBUSH1         = -1000344,
    SAY_PROGRESS        = -1000345,
    SAY_AMBUSH2         = -1000346,
    SAY_END             = -1000347,

    QUEST_ESCAPE_FROM   = 9752,
    NPC_SLAVEBINDER     = 18042
};

struct npc_kayra_longmaneAI : public npc_escortAI
{
    npc_kayra_longmaneAI(Creature* c) : npc_escortAI(c) {}

    void Reset() { }

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {
        case 4:
            DoScriptText(SAY_AMBUSH1, me, pPlayer);
            DoSpawnCreature(NPC_SLAVEBINDER, -10.0f, -5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            DoSpawnCreature(NPC_SLAVEBINDER, -8.0f, 5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            break;
        case 5:
            DoScriptText(SAY_PROGRESS, me, pPlayer);
            SetRun();
            break;
        case 16:
            DoScriptText(SAY_AMBUSH2, me, pPlayer);
            DoSpawnCreature(NPC_SLAVEBINDER, -10.0f, -5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            DoSpawnCreature(NPC_SLAVEBINDER, -8.0f, 5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            break;
        case 17:
            SetRun(false);
            DoScriptText(SAY_END, me, pPlayer);
            break;
        case 25:
            pPlayer->GroupEventHappens(QUEST_ESCAPE_FROM, me);
            break;
        }
    }
};

bool QuestAccept_npc_kayra_longmane(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPE_FROM)
    {
        DoScriptText(SAY_START, pCreature, pPlayer);

        if (npc_escortAI* pEscortAI = CAST_AI(npc_kayra_longmaneAI, pCreature->AI()))
            pEscortAI->Start(false, false, pPlayer->GetGUID());
    }
    return true;
}

CreatureAI* GetAI_npc_kayra_longmaneAI(Creature* pCreature)
{
    return new npc_kayra_longmaneAI(pCreature);
}

/*######
## npc_timothy_daniels
######*/

#define GOSSIP_TIMOTHY_DANIELS_ITEM1    "Specialist, eh? Just what kind of specialist are you, anyway?"
#define GOSSIP_TEXT_BROWSE_POISONS      "Let me browse your reagents and poison supplies."

enum eTimothy
{
    GOSSIP_TEXTID_TIMOTHY_DANIELS1      = 9239
};

bool GossipHello_npc_timothy_daniels(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pCreature->isVendor())
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_POISONS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TIMOTHY_DANIELS_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_timothy_daniels(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    pPlayer->PlayerTalkClass->ClearMenus();
    switch (uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF+1:
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_TIMOTHY_DANIELS1, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_TRADE:
        pPlayer->SEND_VENDORLIST(pCreature->GetGUID());
        break;
    }

    return true;
};

/*######
## npc_baby_murloc
######*/

enum
{
    NPC_PURPLE_MURLOC          = 15357,
    NPC_GREEN_MURLOC           = 15360,
    NPC_BLUE_MURLOC            = 15356,
    NPC_PINK_MURLOC            = 15359,
    NPC_ORANGE_MURLOC          = 15361,
    NPC_CREDIT                 = 18152,

    QUEST_HAVE_YOU_EVER        = 9816,

    SPELL_SING                 = 32041
};

struct Pos
{
    float x, y, z;
};

static Pos M[]=
{
    {1206.926f, 8139.298f, 19.70f},
    {1206.927f, 8158.908f, 19.51f},
    {1220.742f, 8093.757f, 18.120f},
    {1128.926f, 8137.008f, 20.664f},
    {1230.289f, 8156.368f, 18.40f},
    {1216.511f, 8188.199f, 18.70f}
};

struct npc_baby_murlocAI : public ScriptedAI
{
    npc_baby_murlocAI(Creature* creature) : ScriptedAI(creature) {}

    uint64 PlayerGUID;
    uint32 CheckTimer;
    uint32 EndTimer;

    void Reset()
    {
        PlayerGUID = 0;
        CheckTimer = 7000;
        EndTimer = 15000;
        DoSummon();
        me->GetMotionMaster()->MovePoint(0, M[0].x, M[0].y, M[0].z);
        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_DANCE);
    }

    void DoSummon()
    {
        float x, y, z;
        x = me->GetPositionX();
        y = me->GetPositionY();
        z = me->GetPositionZ();

        me->SummonCreature(NPC_PURPLE_MURLOC, x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 60000);
        me->SummonCreature(NPC_GREEN_MURLOC , x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 9000);
        me->SummonCreature(NPC_BLUE_MURLOC  , x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 9000);
        me->SummonCreature(NPC_PINK_MURLOC  , x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 9000);
        me->SummonCreature(NPC_ORANGE_MURLOC, x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 9000);
    }

    void JustSummoned(Creature* summoned)
    {  
        summoned->SetWalk(false);

        switch(summoned->GetEntry())
        {
            case NPC_PURPLE_MURLOC:
                summoned->GetMotionMaster()->MovePoint(0, M[1].x, M[1].y, M[1].z);
                summoned->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_DANCE);
                break;
            case NPC_GREEN_MURLOC:
                summoned->GetMotionMaster()->MovePoint(0, M[2].x, M[2].y, M[2].z);
                break;
            case NPC_BLUE_MURLOC:
                summoned->GetMotionMaster()->MovePoint(0, M[3].x, M[3].y, M[3].z);
                break;
            case NPC_ORANGE_MURLOC:
                summoned->GetMotionMaster()->MovePoint(0, M[4].x, M[4].y, M[4].z);
                break;
            case NPC_PINK_MURLOC:
                summoned->GetMotionMaster()->MovePoint(0, M[5].x, M[5].y, M[5].z);
                break;
        }
    }

    void MoveInLineOfSight(Unit *who)
    {
        if (who->GetTypeId() == TYPEID_PLAYER)
        {
            if (((Player*)who)->GetQuestStatus(QUEST_HAVE_YOU_EVER) == QUEST_STATUS_INCOMPLETE)
            {
                if (me->IsWithinDistInMap(((Player*)who), 15))
                {
                    PlayerGUID = who->GetGUID();
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (CheckTimer <= diff)
        {
            if (Player* player = sObjectMgr.GetPlayer(PlayerGUID))
            {
                me->SetFacingToObject(player);
                DoCast(me, SPELL_SING);

                if (Creature* Purple = GetClosestCreatureWithEntry(me, NPC_PURPLE_MURLOC, 20.0f))
                {
                    Purple->SetFacingToObject(player);
                    Purple->CastSpell(Purple, SPELL_SING, true);
                }
            }

            CheckTimer = 15000;
        }
        else
            CheckTimer -= diff;

        if (EndTimer <= diff)
        {
            if (Player* player = sObjectMgr.GetPlayer(PlayerGUID))
                player->KilledMonsterCredit(NPC_CREDIT, me->GetGUID());;
        }
        else
            EndTimer -= diff;
    }
};

CreatureAI* GetAI_npc_baby_murloc(Creature* creature)
{
    return new npc_baby_murlocAI(creature);
}

void AddSC_zangarmarsh()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npcs_ashyen_and_keleth";
    newscript->pGossipHello =  &GossipHello_npcs_ashyen_and_keleth;
    newscript->pGossipSelect = &GossipSelect_npcs_ashyen_and_keleth;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_cooshcoosh";
    newscript->pGossipHello =  &GossipHello_npc_cooshcoosh;
    newscript->pGossipSelect = &GossipSelect_npc_cooshcoosh;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_elder_kuruti";
    newscript->pGossipHello =  &GossipHello_npc_elder_kuruti;
    newscript->pGossipSelect = &GossipSelect_npc_elder_kuruti;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_mortog_steamhead";
    newscript->pGossipHello =  &GossipHello_npc_mortog_steamhead;
    newscript->pGossipSelect = &GossipSelect_npc_mortog_steamhead;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_kayra_longmane";
    newscript->GetAI = &GetAI_npc_kayra_longmaneAI;
    newscript->pQuestAccept = &QuestAccept_npc_kayra_longmane;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_timothy_daniels";
    newscript->pGossipHello =  &GossipHello_npc_timothy_daniels;
    newscript->pGossipSelect = &GossipSelect_npc_timothy_daniels;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_baby_murloc";
    newscript->GetAI = &GetAI_npc_baby_murloc;
    newscript->RegisterSelf();
}

