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

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npcs_ashyen_and_keleth
######*/

#define GOSSIP_ITEM_BLESS_ASH     "Grant me your mark, wise ancient."
#define GOSSIP_ITEM_BLESS_KEL     "Grant me your mark, mighty ancient."
#define GOSSIP_REWARD_BLESS       -1000359
//#define TEXT_BLESSINGS        "<You need higher standing with Cenarion Expedition to recive a blessing.>"

bool GossipHello_npcs_ashyen_and_keleth(Player *player, Creature *_Creature)
{
    if (player->GetReputationRank(942) > REP_NEUTRAL)
    {
        if (_Creature->GetEntry() == 17900)
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_BLESS_ASH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        if (_Creature->GetEntry() == 17901)
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_BLESS_KEL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    }
    player->SEND_GOSSIP_MENU(player->GetGossipTextId(_Creature), _Creature->GetGUID());

    return true;
}

bool GossipSelect_npcs_ashyen_and_keleth(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF+1)
    {
        _Creature->setPowerType(POWER_MANA);
        _Creature->SetMaxPower(POWER_MANA,200);             //set a "fake" mana value, we can't depend on database doing it in this case
        _Creature->SetPower(POWER_MANA,200);

        if (_Creature->GetEntry() == 17900)                //check which creature we are dealing with
        {
            switch (player->GetReputationRank(942))
            {                                               //mark of lore
                case REP_FRIENDLY:
                    _Creature->CastSpell(player, 31808, true);
                    DoScriptText(GOSSIP_REWARD_BLESS, _Creature);
                    break;
                case REP_HONORED:
                    _Creature->CastSpell(player, 31810, true);
                    DoScriptText(GOSSIP_REWARD_BLESS, _Creature);
                    break;
                case REP_REVERED:
                    _Creature->CastSpell(player, 31811, true);
                    DoScriptText(GOSSIP_REWARD_BLESS, _Creature);
                    break;
                case REP_EXALTED:
                    _Creature->CastSpell(player, 31815, true);
                    DoScriptText(GOSSIP_REWARD_BLESS, _Creature);
                    break;
            }
        }

        if (_Creature->GetEntry() == 17901)
        {
            switch (player->GetReputationRank(942))         //mark of war
            {
                case REP_FRIENDLY:
                    _Creature->CastSpell(player, 31807, true);
                    DoScriptText(GOSSIP_REWARD_BLESS, _Creature);
                    break;
                case REP_HONORED:
                    _Creature->CastSpell(player, 31812, true);
                    DoScriptText(GOSSIP_REWARD_BLESS, _Creature);
                    break;
                case REP_REVERED:
                    _Creature->CastSpell(player, 31813, true);
                    DoScriptText(GOSSIP_REWARD_BLESS, _Creature);
                    break;
                case REP_EXALTED:
                    _Creature->CastSpell(player, 31814, true);
                    DoScriptText(GOSSIP_REWARD_BLESS, _Creature);
                    break;
            }
        }
        player->CLOSE_GOSSIP_MENU();
        player->TalkedToCreature(_Creature->GetEntry(), _Creature->GetGUID());
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

struct OREGON_DLL_DECL npc_cooshcooshAI : public ScriptedAI
{
    npc_cooshcooshAI(Creature* c) : ScriptedAI(c) {}

    uint32 LightningBolt_Timer;

    void Reset()
    {
        LightningBolt_Timer = 2000;
        me->setFaction(FACTION_FRIENDLY_CO);
    }

    void EnterCombat(Unit *who) {}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (LightningBolt_Timer <= diff)
        {
            DoCast(me->getVictim(),SPELL_LIGHTNING_BOLT);
            LightningBolt_Timer = 5000;
        } else LightningBolt_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_npc_cooshcoosh(Creature *_Creature)
{
    return new npc_cooshcooshAI (_Creature);
}

bool GossipHello_npc_cooshcoosh(Player *player, Creature *_Creature)
{
    if (player->GetQuestStatus(10009) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(1, GOSSIP_COOSH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    player->SEND_GOSSIP_MENU(9441, _Creature->GetGUID());
    return true;
}

bool GossipSelect_npc_cooshcoosh(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF)
    {
        player->CLOSE_GOSSIP_MENU();
        _Creature->setFaction(FACTION_HOSTILE_CO);
        ((npc_cooshcooshAI*)_Creature->AI())->AttackStart(player);
    }
    return true;
}

/*######
## npc_elder_kuruti
######*/

#define GOSSIP_ITEM_KUR1 "Offer treat"
#define GOSSIP_ITEM_KUR2 "Im a messenger for Draenei"
#define GOSSIP_ITEM_KUR3 "Get message"

bool GossipHello_npc_elder_kuruti(Player *player, Creature *_Creature)
{
    if (player->GetQuestStatus(9803) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KUR1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    player->SEND_GOSSIP_MENU(9226,_Creature->GetGUID());

    return true;
}

bool GossipSelect_npc_elder_kuruti(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KUR2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(9227, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 1:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_KUR3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(9229, _Creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
        {
            if (!player->HasItemCount(24573,1))
            {
                ItemPosCountVec dest;
                uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 24573, 1, false);
                if (msg == EQUIP_ERR_OK)
                {
                    player->StoreNewItem(dest, 24573, true);
                }
                else
                    player->SendEquipError(msg,NULL,NULL);
            }
            player->SEND_GOSSIP_MENU(9231, _Creature->GetGUID());
            break;
        }
    }
    return true;
}

/*######
## npc_mortog_steamhead
######*/

bool GossipHello_npc_mortog_steamhead(Player *player, Creature *_Creature)
{
    if (_Creature->isVendor() && player->GetReputationRank(942) == REP_EXALTED)
        player->ADD_GOSSIP_ITEM(1, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    player->SEND_GOSSIP_MENU(player->GetGossipTextId(_Creature), _Creature->GetGUID());

    return true;
}

bool GossipSelect_npc_mortog_steamhead(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    if (action == GOSSIP_ACTION_TRADE)
    {
        player->SEND_VENDORLIST(_Creature->GetGUID());
    }
    return true;
}

/*######
## npc_kayra_longmane
######*/

enum eKayra
{
    SAY_START           = -1000360,
    SAY_AMBUSH1         = -1000361,
    SAY_PROGRESS        = -1000362,
    SAY_AMBUSH2         = -1000363,
    SAY_NEAR_END        = -1000364,
    SAY_END             = -1000365, //this is signed for 10646

    QUEST_ESCAPE_FROM   = 9752,
    NPC_SLAVEBINDER     = 18042
};

struct OREGON_DLL_DECL npc_kayra_longmaneAI : public npc_escortAI
{
    npc_kayra_longmaneAI(Creature* c) : npc_escortAI(c) {}

    void Reset() { }

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch(i)
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
                DoScriptText(SAY_NEAR_END, me, pPlayer);
                break;
            case 25:
                DoScriptText(SAY_END, me, pPlayer);
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

CreatureAI* GetAI_npc_kayra_longmaneAI(Creature* _Creature)
{
    npc_kayra_longmaneAI* thisAI = new npc_kayra_longmaneAI(_Creature);

    thisAI->AddWaypoint(0, -931.88, 5283.56, 23.98);//SAY_PROGRESS_1
    thisAI->AddWaypoint(1, -930.52, 5287.57, 23.82);
    thisAI->AddWaypoint(2, -924.98, 5297.94, 17.78);
    thisAI->AddWaypoint(3, -928.83, 5316.07, 18.18);
    thisAI->AddWaypoint(4, -930.07, 5323.10, 18.63);
    thisAI->AddWaypoint(5, -926.58, 5331.24, 18.74, 6000);//SAY_PROGRESS_2
    thisAI->AddWaypoint(6, -926.58, 5331.24, 18.74, 3000);//SAY_PROGRESS_3 Run to the hills!
    thisAI->AddWaypoint(7, -931.24, 5358.89, 18.14);
    thisAI->AddWaypoint(8, -934.43, 5370.20, 22.41);
    thisAI->AddWaypoint(9, -943.01, 5400.55, 22.74);
    thisAI->AddWaypoint(10, -929.62, 5417.98, 23.07);
    thisAI->AddWaypoint(11, -901.92, 5420.38, 24.19);
    thisAI->AddWaypoint(12, -859.03, 5415.36, 23.64);
    thisAI->AddWaypoint(13, -808.94, 5401.93, 23.11);
    thisAI->AddWaypoint(14, -772.74, 5390.53, 22.97);
    thisAI->AddWaypoint(15, -749.71, 5385.39, 22.75);
    thisAI->AddWaypoint(16, -721.23, 5380.38, 22.47);
    thisAI->AddWaypoint(17, -687.96, 5379.08, 22.16);
    thisAI->AddWaypoint(18, -680.87, 5378.95, 21.99, 6000);//SAY_PROGRESS_4 Summon Ambush
    thisAI->AddWaypoint(19, -680.87, 5378.95, 21.99, 6000);//SAY_PROGRESS_5
    thisAI->AddWaypoint(20, -636.14, 5385.25, 22.20);
    thisAI->AddWaypoint(21, -602.94, 5411.36, 21.48);
    thisAI->AddWaypoint(22, -566.86, 5421.87, 21.25);
    thisAI->AddWaypoint(23, -547.27, 5427.87, 21.10);
    thisAI->AddWaypoint(24, -520.59, 5444.83, 21.15);
    thisAI->AddWaypoint(25, -488.45, 5447.83, 22.38);
    thisAI->AddWaypoint(26, -449.65, 5463.78, 21.77, 11000);//SAY_PROGRESS_6

    return (CreatureAI*)thisAI;
}
/*######
## AddSC
######*/

void AddSC_zangarmarsh()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="npcs_ashyen_and_keleth";
    newscript->pGossipHello =  &GossipHello_npcs_ashyen_and_keleth;
    newscript->pGossipSelect = &GossipSelect_npcs_ashyen_and_keleth;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_cooshcoosh";
    newscript->pGossipHello =  &GossipHello_npc_cooshcoosh;
    newscript->pGossipSelect = &GossipSelect_npc_cooshcoosh;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_elder_kuruti";
    newscript->pGossipHello =  &GossipHello_npc_elder_kuruti;
    newscript->pGossipSelect = &GossipSelect_npc_elder_kuruti;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_mortog_steamhead";
    newscript->pGossipHello =  &GossipHello_npc_mortog_steamhead;
    newscript->pGossipSelect = &GossipSelect_npc_mortog_steamhead;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_kayra_longmane";
    newscript->GetAI = &GetAI_npc_kayra_longmaneAI;
    newscript->pQuestAccept = &QuestAccept_npc_kayra_longmane;
    newscript->RegisterSelf();
}

