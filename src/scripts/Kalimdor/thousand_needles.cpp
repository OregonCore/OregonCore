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
SDName: Thousand Needles
SD%Complete: 100
SDComment: Support for Quest: 4770, 1950, 4904, 4966, 5151.
SDCategory: Thousand Needles
EndScriptData */

/* ContentData
npc_kanati
npc_lakota_windsong
npc_swiftmountain
npc_plucky
go_panther_cage
npc_enraged_panther
EndContentData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"


/*#####
# npc_kanati
######*/

enum eKanati
{
    SAY_KAN_START              = -1600410,

    QUEST_PROTECT_KANATI        = 4966,
    NPC_GALAK_ASS               = 10720
};

const float m_afGalakLoc[]= {-4867.387695, -1357.353760, -48.226 };

struct npc_kanatiAI : public npc_escortAI
{
    npc_kanatiAI(Creature* pCreature) : npc_escortAI(pCreature) { }

    void Reset() { }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 0:
                DoScriptText(SAY_KAN_START, me);
                DoSpawnGalak();
                break;
            case 1:
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_PROTECT_KANATI, me);
                break;
        }
    }

    void DoSpawnGalak()
    {
        for (int i = 0; i < 3; ++i)
            me->SummonCreature(NPC_GALAK_ASS,
            m_afGalakLoc[0], m_afGalakLoc[1], m_afGalakLoc[2], 0.0f,
            TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(me);
    }
};

CreatureAI* GetAI_npc_kanati(Creature* pCreature)
{
    npc_kanatiAI* kanatiAI = new npc_kanatiAI(pCreature);

    kanatiAI->AddWaypoint(0, -4903.52, -1368.34, -52.611, 5000);
    kanatiAI->AddWaypoint(1, -4906, -1367.05, -52.611);

    return (CreatureAI*)kanatiAI;
}

bool QuestAccept_npc_kanati(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_PROTECT_KANATI)
    {
        if (npc_kanatiAI* pEscortAI = CAST_AI(npc_kanatiAI, pCreature->AI()))
            pEscortAI->Start(false, false, pPlayer->GetGUID(), pQuest, true);
    }
    return true;
}

/*######
# npc_lakota_windsong
######*/

enum eLakota
{
    SAY_LAKO_START              = -1600365,
    SAY_LAKO_LOOK_OUT           = -1600366,
    SAY_LAKO_HERE_COME          = -1600367,
    SAY_LAKO_MORE               = -1600368,
    SAY_LAKO_END                = -1600369,

    QUEST_FREE_AT_LAST          = 4904,
    NPC_GRIM_BANDIT             = 10758,
    FACTION_ESCORTEE_LAKO       = 232,                      //guessed

    ID_AMBUSH_1                 = 0,
    ID_AMBUSH_2                 = 2,
    ID_AMBUSH_3                 = 4
};

float m_afBanditLoc[6][6]=
{
    {-4905.479492, -2062.732666, 84.352},
    {-4915.201172, -2073.528320, 84.733},
    {-4878.883301, -1986.947876, 91.966},
    {-4877.503906, -1966.113403, 91.859},
    {-4767.985352, -1873.169189, 90.192},
    {-4788.861328, -1888.007813, 89.888}
};

struct npc_lakota_windsongAI : public npc_escortAI
{
    npc_lakota_windsongAI(Creature* pCreature) : npc_escortAI(pCreature) { }

    void Reset() { }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 8:
                DoScriptText(SAY_LAKO_LOOK_OUT, me);
                DoSpawnBandits(ID_AMBUSH_1);
                break;
            case 14:
                DoScriptText(SAY_LAKO_HERE_COME, me);
                DoSpawnBandits(ID_AMBUSH_2);
                break;
            case 21:
                DoScriptText(SAY_LAKO_MORE, me);
                DoSpawnBandits(ID_AMBUSH_3);
                break;
            case 45:
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_FREE_AT_LAST, me);
                break;
        }
    }

    void DoSpawnBandits(int uiAmbushId)
    {
        for (int i = 0; i < 2; ++i)
            me->SummonCreature(NPC_GRIM_BANDIT,
            m_afBanditLoc[i+uiAmbushId][0], m_afBanditLoc[i+uiAmbushId][1], m_afBanditLoc[i+uiAmbushId][2], 0.0f,
            TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
    }
};

CreatureAI* GetAI_npc_lakota_windsong(Creature* pCreature)
{
    npc_lakota_windsongAI* lakota_windsongAI = new npc_lakota_windsongAI(pCreature);

    lakota_windsongAI->AddWaypoint(0, -4792.4, -2137.78, 82.423);
    lakota_windsongAI->AddWaypoint(1, -4813.51, -2141.54, 80.774);
    lakota_windsongAI->AddWaypoint(2, -4828.63, -2154.31, 82.074);
    lakota_windsongAI->AddWaypoint(3, -4833.77, -2149.18, 81.676);
    lakota_windsongAI->AddWaypoint(4, -4846.42, -2136.05, 77.871);
    lakota_windsongAI->AddWaypoint(5, -4865.08, -2116.55, 76.483);
    lakota_windsongAI->AddWaypoint(6, -4888.43, -2090.73, 80.907);
    lakota_windsongAI->AddWaypoint(7, -4893.07, -2085.47, 82.094);
    lakota_windsongAI->AddWaypoint(8, -4907.26, -2074.93, 84.437, 5000);
    lakota_windsongAI->AddWaypoint(9, -4899.9, -2062.14, 83.78);
    lakota_windsongAI->AddWaypoint(10, -4897.76, -2056.52, 84.184);
    lakota_windsongAI->AddWaypoint(11, -4888.33, -2033.18, 83.654);
    lakota_windsongAI->AddWaypoint(12, -4876.34, -2003.92, 90.887);
    lakota_windsongAI->AddWaypoint(13, -4872.23, -1994.17, 91.513);
    lakota_windsongAI->AddWaypoint(14, -4879.57, -1976.99, 92.185, 5000);
    lakota_windsongAI->AddWaypoint(15, -4879.05, -1964.35, 92.001);
    lakota_windsongAI->AddWaypoint(16, -4874.72, -1956.94, 90.737);
    lakota_windsongAI->AddWaypoint(17, -4869.47, -1952.61, 89.206);
    lakota_windsongAI->AddWaypoint(18, -4842.47, -1929, 84.147);
    lakota_windsongAI->AddWaypoint(19, -4804.44, -1897.3, 89.362);
    lakota_windsongAI->AddWaypoint(20, -4798.07, -1892.38, 89.368);
    lakota_windsongAI->AddWaypoint(21, -4779.45, -1882.76, 90.169, 5000);
    lakota_windsongAI->AddWaypoint(22, -4762.08, -1866.53, 89.481);
    lakota_windsongAI->AddWaypoint(23, -4766.27, -1861.87, 87.847);
    lakota_windsongAI->AddWaypoint(24, -4782.93, -1852.17, 78.354);
    lakota_windsongAI->AddWaypoint(25, -4793.61, -1850.96, 77.658);
    lakota_windsongAI->AddWaypoint(26, -4803.32, -1855.1, 78.958);
    lakota_windsongAI->AddWaypoint(27, -4807.97, -1854.5, 77.743);
    lakota_windsongAI->AddWaypoint(28, -4837.21, -1848.49, 64.488);
    lakota_windsongAI->AddWaypoint(29, -4884.62, -1840.4, 56.219);
    lakota_windsongAI->AddWaypoint(30, -4889.71, -1839.62, 54.417);
    lakota_windsongAI->AddWaypoint(31, -4893.9, -1843.69, 53.012);
    lakota_windsongAI->AddWaypoint(32, -4903.14, -1872.38, 32.266);
    lakota_windsongAI->AddWaypoint(33, -4910.94, -1879.86, 29.94);
    lakota_windsongAI->AddWaypoint(34, -4920.05, -1880.94, 30.597);
    lakota_windsongAI->AddWaypoint(35, -4924.46, -1881.45, 29.292);
    lakota_windsongAI->AddWaypoint(36, -4966.12, -1886.03, 10.977);
    lakota_windsongAI->AddWaypoint(37, -4999.37, -1890.85, 4.43);
    lakota_windsongAI->AddWaypoint(38, -5007.27, -1891.67, 2.771);
    lakota_windsongAI->AddWaypoint(39, -5013.33, -1879.59, -1.947);
    lakota_windsongAI->AddWaypoint(40, -5023.33, -1855.96, -17.103);
    lakota_windsongAI->AddWaypoint(41, -5038.51, -1825.99, -35.821);
    lakota_windsongAI->AddWaypoint(42, -5048.73, -1809.8, -46.457);
    lakota_windsongAI->AddWaypoint(43, -5053.19, -1791.68, -57.186);
    lakota_windsongAI->AddWaypoint(44, -5062.09, -1794.4, -56.515);
    lakota_windsongAI->AddWaypoint(45, -5052.66, -1797.04, -54.734, 5000);

    return (CreatureAI*)lakota_windsongAI;
}

bool QuestAccept_npc_lakota_windsong(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_FREE_AT_LAST)
    {
        DoScriptText(SAY_LAKO_START, pCreature, pPlayer);
        pCreature->setFaction(FACTION_ESCORTEE_LAKO);

        if (npc_lakota_windsongAI* pEscortAI = CAST_AI(npc_lakota_windsongAI, pCreature->AI()))
            pEscortAI->Start(false, false, pPlayer->GetGUID(), pQuest);
    }
    return true;
}
/*#####
# npc_swiftmountain
######*/

enum ePacka
{
    SAY_START           = -1000147,
    SAY_WYVERN          = -1000148,
    SAY_COMPLETE        = -1000149,

    QUEST_HOMEWARD      = 4770,
    NPC_WYVERN          = 4107,
    FACTION_ESCORTEE    = 232                               //guessed
};

float m_afWyvernLoc[3][3]=
{
    {-4990.606, -906.057, -5.343},
    {-4970.241, -927.378, -4.951},
    {-4985.364, -952.528, -5.199}
};

struct npc_swiftmountainAI : public npc_escortAI
{
    npc_swiftmountainAI(Creature* pCreature) : npc_escortAI(pCreature) { }

    void Reset() { }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 15:
                DoScriptText(SAY_WYVERN, me);
                DoSpawnWyvern();
                break;
            case 26:
                DoScriptText(SAY_COMPLETE, me);
                break;
            case 27:
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_HOMEWARD, me);
                break;
        }
    }

    void DoSpawnWyvern()
    {
        for (int i = 0; i < 3; ++i)
            me->SummonCreature(NPC_WYVERN,
            m_afWyvernLoc[i][0], m_afWyvernLoc[i][1], m_afWyvernLoc[i][2], 0.0f,
            TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
    }
};


bool QuestAccept_npc_swiftmountain(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_HOMEWARD)
    {
        DoScriptText(SAY_START, pCreature, pPlayer);
        pCreature->setFaction(FACTION_ESCORTEE);

        if (npc_swiftmountainAI* pEscortAI = CAST_AI(npc_swiftmountainAI,pCreature->AI()))
            pEscortAI->Start(false, false, pPlayer->GetGUID(), pQuest);
    }
    return true;
}

CreatureAI* GetAI_npc_swiftmountain(Creature* pCreature)
{
    npc_swiftmountainAI* thisAI = new npc_swiftmountainAI(pCreature);

   thisAI->AddWaypoint(0, -5156.69, -1220.49, 48.78, 5000);
   thisAI->AddWaypoint(1, -5157.12, -1220.13, 48.67);
   thisAI->AddWaypoint(2, -5171.7, -1205.36, 47.43);
   thisAI->AddWaypoint(3, -5174.08, -1197.7, 46.90);
   thisAI->AddWaypoint(4, -5178.24, -1183.78, 45.97);
   thisAI->AddWaypoint(5, -5181.52, -1171.03, 45.29);
   thisAI->AddWaypoint(6, -5184.29, -1159.21, 44.62);
   thisAI->AddWaypoint(7, -5184.84, -1152.95, 44.84);
   thisAI->AddWaypoint(8, -5182.04, -1142.83, 44.07);
   thisAI->AddWaypoint(9, -5178.44, -1133.57, 43.91);
   thisAI->AddWaypoint(10, -5176.68, -1129.48, 43.81);
   thisAI->AddWaypoint(11, -5164.85, -1123.33, 43.99);
   thisAI->AddWaypoint(12, -5153.07, -1117.7, 43.66);
   thisAI->AddWaypoint(13, -5143.52, -1113.14, 43.78);
   thisAI->AddWaypoint(14, -5135.86, -1104.42, 47.23);
   thisAI->AddWaypoint(15, -5129.86, -1097.22, 49.52);
   thisAI->AddWaypoint(16, -5127.48, -1087.29, 49.03);
   thisAI->AddWaypoint(17, -5127.57, -1080.4, 46.64);
   thisAI->AddWaypoint(18, -5129.65, -1077.58, 45.29);
   thisAI->AddWaypoint(19, -5135.86, -1069.12, 39.53);
   thisAI->AddWaypoint(20, -5141.97, -1060.58, 32.70);
   thisAI->AddWaypoint(21, -5145.99, -1054.85, 28.98);
   thisAI->AddWaypoint(22, -5147.08, -1050.35, 26.36);
   thisAI->AddWaypoint(23, -5147.5, -1043.37, 21.84);
   thisAI->AddWaypoint(24, -5147.68, -1036.37, 17.05);
   thisAI->AddWaypoint(25, -5147.68, -1029.37, 14.59);
   thisAI->AddWaypoint(26, -5144.62, -1023.9, 11.67);
   thisAI->AddWaypoint(27, -5138.67, -1020.23, 7.81);
   thisAI->AddWaypoint(28, -5135.6, -1018.55, 6.19);
   thisAI->AddWaypoint(29, -5126.25, -1014.76, 1.08);
   thisAI->AddWaypoint(30, -5120.03, -1013.12, -1.11);
   thisAI->AddWaypoint(31, -5112.3, -1027.65, -5.39);
   thisAI->AddWaypoint(32, -5106.99, -1023.09, -5.10);
   thisAI->AddWaypoint(33, -5099.07, -1016.19, -4.92);
   thisAI->AddWaypoint(34, -5091.23, -1009.21, -5.22);
   thisAI->AddWaypoint(35, -5083.62, -1001.97, -5.22);
   thisAI->AddWaypoint(36, -5076.1, -994.652, -4.92);
   thisAI->AddWaypoint(37, -5066.71, -985.507, -4.97);
   thisAI->AddWaypoint(38, -5065.13, -978.689, -5.02);
   thisAI->AddWaypoint(39, -5062.33, -968.57, -5.08);
   thisAI->AddWaypoint(40, -5059.46, -958.469, -5.16);
   thisAI->AddWaypoint(41, -5056.59, -948.375, -5.10);
   thisAI->AddWaypoint(42, -5053.73, -938.274, -5.69);
   thisAI->AddWaypoint(43, -5043.06, -934.822, -5.35);
   thisAI->AddWaypoint(44, -5029.35, -932.007, -5.27);
   thisAI->AddWaypoint(45, -5024.58, -933.781, -5.40);
   thisAI->AddWaypoint(46, -5019.13, -938.172, -5.54);
   thisAI->AddWaypoint(47, -5011, -944.812, -5.47);
   thisAI->AddWaypoint(48, -5002.86, -951.455, -5.44);
   thisAI->AddWaypoint(49, -4994.73, -958.099, -5.41);
   thisAI->AddWaypoint(50, -4990.57, -963.782, -5.33);
   thisAI->AddWaypoint(51, -4987.43, -970.041, -5.17);
   thisAI->AddWaypoint(52, -4981.41, -982.678, -5.10);
   thisAI->AddWaypoint(53, -4977.03, -992.221, -4.97);
   thisAI->AddWaypoint(54, -4972.6, -1001.74, -5.24);
   thisAI->AddWaypoint(55, -4968.15, -1011.25, -5.49);
   thisAI->AddWaypoint(56, -4963.75, -1020.78, -5.07);
   thisAI->AddWaypoint(57, -4959.3, -1030.3, -5.28);
   thisAI->AddWaypoint(58, -4956.28, -1036.61, -5.84);
   thisAI->AddWaypoint(59, -4952.05, -1043.75, -9.04);
   thisAI->AddWaypoint(60, -4943.8, -1055.06, -17.91);
   thisAI->AddWaypoint(61, -4939.47, -1055.61, -20.73);
   thisAI->AddWaypoint(62, -4929.16, -1053.64, -25.65);
   thisAI->AddWaypoint(63, -4922.28, -1052.37, -28.72);
   thisAI->AddWaypoint(64, -4913.26, -1051.3, -31.80);
   thisAI->AddWaypoint(65, -4903.54, -1054.17, -33.48);
   thisAI->AddWaypoint(66, -4905.95, -1056.71, -33.68);
   thisAI->AddWaypoint(67, -4913.18, -1064.32, -39.46);
   thisAI->AddWaypoint(68, -4925.27, -1076.98, -47.39);
   thisAI->AddWaypoint(69, -4932.68, -1084.42, -51.00);
   thisAI->AddWaypoint(70, -4938.3, -1100.41, -50.71, 5000);
   thisAI->AddWaypoint(71, -4937.34, -1102.87, -49.82);

    return (CreatureAI*)thisAI;
}

/*#####
# npc_plucky
######*/

#define GOSSIP_P    "<Learn Secret phrase>"

#define SPELL_TRANSFORM_HUMAN 9192
#define QUEST_GET_THE_SCOOP 1950

struct npc_pluckyAI : public ScriptedAI
{
    npc_pluckyAI(Creature *c) : ScriptedAI(c) {}

    bool Transformed;
    bool Chicken;

    uint32 Timer;
    uint32 ChickenTimer;

    void Reset()   {

       Transformed = false;
       Chicken     = false;
       me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
       Timer        = 0;
       ChickenTimer = 0;
       }

    void EnterCombat(Unit *who){}

    void TransformHuman(uint32 emoteid)
    {
         if (!Transformed)
         {
             Transformed = true;
             DoCast(me, SPELL_TRANSFORM_HUMAN);
             Timer = 120000;
             if (emoteid == TEXTEMOTE_BECKON)
                 me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
             else
             {
                 ChickenTimer = 1500;
                 Chicken = true;
             }
         }
    }

    void UpdateAI(const uint32 diff)
    {
        if (Transformed)
        {
            if (Timer <= diff)
                Reset();
            else Timer-=diff;
        }

       if (Chicken)
       {
           if (ChickenTimer <= diff)
           {
               me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
               Chicken = false;
           } else ChickenTimer-=diff;
       }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
   }
};

bool ReceiveEmote_npc_plucky(Player *player, Creature* pCreature, uint32 emote)
{
    if ((emote == TEXTEMOTE_BECKON || emote == TEXTEMOTE_CHICKEN &&
        player->GetQuestStatus(QUEST_GET_THE_SCOOP) == QUEST_STATUS_INCOMPLETE))
    {
        pCreature->SetInFront(player);
        ((npc_pluckyAI*)CAST_CRE(pCreature)->AI())->TransformHuman(emote);
    }

    return true;
}

bool GossipHello_npc_plucky(Player *player, Creature* pCreature)
{
    if (player->GetQuestStatus(QUEST_GET_THE_SCOOP) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_P, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    player->SEND_GOSSIP_MENU(738, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_plucky(Player *player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch(action)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            player->CLOSE_GOSSIP_MENU();
            player->CompleteQuest(QUEST_GET_THE_SCOOP);
        break;
    }
    return true;
}

CreatureAI* GetAI_npc_plucky(Creature* pCreature)
{
return new npc_pluckyAI(pCreature);
}

/*#####
#npc_panter
######*/

enum ePantherCage
{
    ENRAGED_PANTHER = 10992
};

bool go_panther_cage(Player* pPlayer, GameObject* pGo)
{

    if (pPlayer->GetQuestStatus(5151) == QUEST_STATUS_INCOMPLETE)
    {
        if (Creature* panther = pGo->FindNearestCreature(ENRAGED_PANTHER, 5, true))
        {
            panther->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
            panther->SetReactState(REACT_AGGRESSIVE);
            panther->AI()->AttackStart(pPlayer);
        }
    }

    return true ;
}

struct npc_enraged_pantherAI : public ScriptedAI
{
    npc_enraged_pantherAI(Creature *c) : ScriptedAI(c) {}

    void Reset()
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetReactState(REACT_PASSIVE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_enraged_panther(Creature* pCreature)
{
    return new npc_enraged_pantherAI(pCreature);
}
void AddSC_thousand_needles()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_kanati";
    newscript->GetAI = &GetAI_npc_kanati;
    newscript->pQuestAccept = &QuestAccept_npc_kanati;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_lakota_windsong";
    newscript->GetAI = &GetAI_npc_lakota_windsong;
    newscript->pQuestAccept = &QuestAccept_npc_lakota_windsong;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_swiftmountain";
    newscript->GetAI = &GetAI_npc_swiftmountain;
    newscript->pQuestAccept = &QuestAccept_npc_swiftmountain;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_plucky";
    newscript->GetAI = &GetAI_npc_plucky;
    newscript->pReceiveEmote =  &ReceiveEmote_npc_plucky;
    newscript->pGossipHello =   &GossipHello_npc_plucky;
    newscript->pGossipSelect = &GossipSelect_npc_plucky;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="npc_enraged_panther";
    newscript->GetAI = &GetAI_npc_enraged_panther;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="go_panther_cage";
    newscript->pGOHello = &go_panther_cage;
    newscript->RegisterSelf();
}

