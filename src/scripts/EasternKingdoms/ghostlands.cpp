/*
 * Copyright (C) 2008-2010 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Ghostlands
SD%Complete: 100
SDComment: Quest support: 9692, 9212. Obtain Budd's Guise of Zul'aman. Vendor Rathis Tomber
SDCategory: Ghostlands
EndScriptData */

/* ContentData
npc_blood_knight_dawnstar
npc_budd_nedreck
npc_rathis_tomber
npc_ranger_lilatha
EndContentData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"

/*######
## npc_blood_knight_dawnstar
######*/

#define GOSSIP_H_BKD "Take Blood Knight Insignia"

bool GossipHello_npc_blood_knight_dawnstar(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(9692) == QUEST_STATUS_INCOMPLETE && !pPlayer->HasItemCount(24226,1,true))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_H_BKD, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_blood_knight_dawnstar(Player* pPlayer, Creature* /*pCreature*/, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        ItemPosCountVec dest;
        uint8 msg = pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 24226, 1, false);
        if (msg == EQUIP_ERR_OK)
        {
            pPlayer->StoreNewItem(dest, 24226, 1, true);
            pPlayer->PlayerTalkClass->ClearMenus();
        }
    }
    return true;
}

/*######
## npc_budd_nedreck
######*/

#define GOSSIP_HBN "You gave the crew disguises?"

bool GossipHello_npc_budd_nedreck(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(11166) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HBN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_budd_nedreck(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, 42540, false);
    }
    return true;
}

/*######
## npc_rathis_tomber
######*/

bool GossipHello_npc_rathis_tomber(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pCreature->isVendor() && pPlayer->GetQuestRewardStatus(9152))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
        pPlayer->SEND_GOSSIP_MENU(8432, pCreature->GetGUID());
    }else
    pPlayer->SEND_GOSSIP_MENU(8431, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_rathis_tomber(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetGUID());
    return true;
}

/*#####
## go_gilded_brazier (Paladin First Trail quest (9678))
#####*/

bool GOHello_gilded_brazier(Player *player, GameObject* _GO)
{
    if (player->GetQuestStatus(9678) == QUEST_STATUS_INCOMPLETE)
    {
        Creature *Stillblade = player->SummonCreature(17716, 8106.11, -7542.06, 151.775, 3.02598, TEMPSUMMON_DEAD_DESPAWN, 60000);
        if (Stillblade)
            ((CreatureAI*)Stillblade->AI())->AttackStart(player);
    }
    return true;
};

/*######
## npc_ranger_lilatha
######*/

enum eEnums
{
    SAY_START           = -1000140,
    SAY_PROGRESS1       = -1000141,
    SAY_PROGRESS2       = -1000142,
    SAY_PROGRESS3       = -1000143,
    SAY_END1            = -1000144,
    SAY_END2            = -1000145,
    SAY_CAPTAIN_ANSWER      = -1000146,

    QUEST_ESCAPE_FROM_THE_CATACOMBS     = 9212,
    GO_CAGE             = 181152,
    NPC_CAPTAIN_HELIOS  = 16220,
    FACTION_SMOON_E     = 1603,
};

struct npc_ranger_lilathaAI : public npc_escortAI
{
    npc_ranger_lilathaAI(Creature *c) : npc_escortAI(c) {}

    std::list<GameObject*> CageList;

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch(i)
        {
        case 0:
            {
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
            if (GameObject* Cage = me->FindNearestGameObject(GO_CAGE, 20))
                Cage->SetGoState(GO_STATE_ACTIVE);
            DoScriptText(SAY_START, me, pPlayer);
            break;
            }
        case 5:
            DoScriptText(SAY_PROGRESS1, me, pPlayer);
        case 11:
            DoScriptText(SAY_PROGRESS2, me, pPlayer);
            me->SetOrientation(4.762841);
            break;
        case 18:
            {
            DoScriptText(SAY_PROGRESS3, me, pPlayer);
            Creature* Summ1 = me->SummonCreature(16342, 7627.083984, -7532.538086, 152.128616, 1.082733, TEMPSUMMON_DEAD_DESPAWN, 0);
            Creature* Summ2 = me->SummonCreature(16343, 7620.432129, -7532.550293, 152.454865, 0.827478, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (Summ1 && Summ2)
            {
                Summ1->Attack(me, true);
                Summ2->Attack(pPlayer, true);
            }
            me->AI()->AttackStart(Summ1);
            break;
            }
        case 19: me->RemoveUnitMovementFlag(MOVEFLAG_WALK_MODE); break;
        case 25: me->AddUnitMovementFlag(MOVEFLAG_WALK_MODE); break;
        case 30:
            if (pPlayer && pPlayer->GetTypeId() == TYPEID_PLAYER)
                CAST_PLR(pPlayer)->GroupEventHappens(QUEST_ESCAPE_FROM_THE_CATACOMBS,me);
            break;
        case 32:
            me->SetOrientation(2.978281);
            DoScriptText(SAY_END1, me, pPlayer);
            break;
        case 33:
            me->SetOrientation(5.858011);
            DoScriptText(SAY_END2, me, pPlayer);
            Unit* CaptainHelios = me->FindNearestCreature(NPC_CAPTAIN_HELIOS, 50);
            if (CaptainHelios)
            DoScriptText(SAY_CAPTAIN_ANSWER, CaptainHelios, pPlayer);
            break;
        }
    }

    void Reset()
    {
        if (GameObject* Cage = me->FindNearestGameObject(GO_CAGE, 20))
            Cage->SetGoState(GO_STATE_READY);
    }
};

bool QuestAccept_npc_ranger_lilatha(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_ESCAPE_FROM_THE_CATACOMBS)
    {
        pCreature->setFaction(113);

        if (npc_escortAI* pEscortAI = CAST_AI(npc_ranger_lilathaAI, pCreature->AI()))
            pEscortAI->Start(true, false, pPlayer->GetGUID());
    }
    return true;
}

CreatureAI* GetAI_npc_ranger_lilathaAI(Creature* pCreature)
{
    npc_ranger_lilathaAI* ranger_lilathaAI = new npc_ranger_lilathaAI(pCreature);

    ranger_lilathaAI->AddWaypoint(0, 7545.07, -7359.87, 162.354, 4000); // Say0
    ranger_lilathaAI->AddWaypoint(1, 7550.048340, -7362.237793, 162.235657);
    ranger_lilathaAI->AddWaypoint(2, 7566.976074, -7364.315430, 161.738770);
    ranger_lilathaAI->AddWaypoint(3, 7578.830566, -7361.677734, 161.738770);
    ranger_lilathaAI->AddWaypoint(4, 7590.969238, -7359.053711, 162.257660);
    ranger_lilathaAI->AddWaypoint(5, 7598.354004, -7362.815430, 162.256683, 4000); // Say1
    ranger_lilathaAI->AddWaypoint(6, 7605.861328, -7380.424316, 161.937073);
    ranger_lilathaAI->AddWaypoint(7, 7605.295410, -7387.382813, 157.253998);
    ranger_lilathaAI->AddWaypoint(8, 7606.131836, -7393.893555, 156.941925);
    ranger_lilathaAI->AddWaypoint(9, 7615.207520, -7400.187012, 157.142639);
    ranger_lilathaAI->AddWaypoint(10, 7618.956543, -7402.652832, 158.202042);
    ranger_lilathaAI->AddWaypoint(11, 7636.850586, -7401.756836, 162.144791);
    ranger_lilathaAI->AddWaypoint(12, 7637.058105, -7404.944824, 162.206970, 4000);// Say2
    ranger_lilathaAI->AddWaypoint(13, 7636.910645, -7412.585449, 162.366425);
    ranger_lilathaAI->AddWaypoint(14, 7637.607910, -7425.591797, 162.630661);
    ranger_lilathaAI->AddWaypoint(15, 7637.816895, -7459.057129, 163.302704);
    ranger_lilathaAI->AddWaypoint(16, 7638.859863, -7470.902344, 162.517059);
    ranger_lilathaAI->AddWaypoint(17, 7641.395996, -7488.217285, 157.381287);
    ranger_lilathaAI->AddWaypoint(18, 7634.455566, -7505.451660, 154.682159);
    ranger_lilathaAI->AddWaypoint(19, 7631.906738, -7516.948730, 153.597382); // say3
    ranger_lilathaAI->AddWaypoint(20, 7622.231445, -7537.037598, 151.587112);
    ranger_lilathaAI->AddWaypoint(21, 7610.921875, -7550.670410, 149.639374);
    ranger_lilathaAI->AddWaypoint(22, 7598.229004, -7562.551758, 145.953888);
    ranger_lilathaAI->AddWaypoint(23, 7588.509277, -7577.755371, 148.294479);
    ranger_lilathaAI->AddWaypoint(24, 7567.339355, -7608.456055, 146.006485);
    ranger_lilathaAI->AddWaypoint(25, 7562.547852, -7617.417969, 148.097504);
    ranger_lilathaAI->AddWaypoint(26, 7561.508789, -7645.064453, 151.245163);
    ranger_lilathaAI->AddWaypoint(27, 7563.337402, -7654.652344, 151.227158);
    ranger_lilathaAI->AddWaypoint(28, 7565.533691, -7658.296387, 151.248886);
    ranger_lilathaAI->AddWaypoint(29, 7571.155762, -7659.118652, 151.244568);
    ranger_lilathaAI->AddWaypoint(30, 7579.119629, -7662.213867, 151.651505);
    ranger_lilathaAI->AddWaypoint(31, 7603.768066, -7667.000488, 153.997726);
    ranger_lilathaAI->AddWaypoint(32, 7603.768066, -7667.000488, 153.997726, 4000);  // Say4 & Set orientation
    ranger_lilathaAI->AddWaypoint(33, 7603.768066, -7667.000488, 153.997726, 8000);  // Say5 & Set orientation
    ranger_lilathaAI->AddWaypoint(34, 7603.768066, -7667.000488, 153.997726);

    return (CreatureAI*)ranger_lilathaAI;
}

void AddSC_ghostlands()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_blood_knight_dawnstar";
    newscript->pGossipHello = &GossipHello_npc_blood_knight_dawnstar;
    newscript->pGossipSelect = &GossipSelect_npc_blood_knight_dawnstar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_budd_nedreck";
    newscript->pGossipHello = &GossipHello_npc_budd_nedreck;
    newscript->pGossipSelect = &GossipSelect_npc_budd_nedreck;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_rathis_tomber";
    newscript->pGossipHello = &GossipHello_npc_rathis_tomber;
    newscript->pGossipSelect = &GossipSelect_npc_rathis_tomber;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_gilded_brazier";
    newscript->pGOHello = &GOHello_gilded_brazier;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_ranger_lilatha";
    newscript->GetAI = &GetAI_npc_ranger_lilathaAI;
    newscript->pQuestAccept = &QuestAccept_npc_ranger_lilatha;
    newscript->RegisterSelf();
}
