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
SDComment: Support for Quest: 1950, 4770, 4904, 4966, 5151.
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

/*######
# npc_paoka_swiftmountain
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

struct npc_paoka_swiftmountainAI : public npc_escortAI
{
    npc_paoka_swiftmountainAI(Creature* pCreature) : npc_escortAI(pCreature) { }

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

CreatureAI* GetAI_npc_paoka_swiftmountain(Creature* pCreature)
{
    npc_paoka_swiftmountainAI* paoka_swiftmountainAI = new npc_paoka_swiftmountainAI(pCreature);

    paoka_swiftmountainAI->AddWaypoint(0, -5156.69, -1220.49, 48.78, 5000);
    paoka_swiftmountainAI->AddWaypoint(1, -5157.12, -1220.13, 48.67);
    paoka_swiftmountainAI->AddWaypoint(2, -5171.7, -1205.36, 47.43);
    paoka_swiftmountainAI->AddWaypoint(3, -5174.08, -1197.7, 46.90);
    paoka_swiftmountainAI->AddWaypoint(4, -5178.24, -1183.78, 45.97);
    paoka_swiftmountainAI->AddWaypoint(5, -5181.52, -1171.03, 45.29);
    paoka_swiftmountainAI->AddWaypoint(6, -5184.29, -1159.21, 44.62);
    paoka_swiftmountainAI->AddWaypoint(7, -5184.84, -1152.95, 44.84);
    paoka_swiftmountainAI->AddWaypoint(8, -5182.04, -1142.83, 44.07);
    paoka_swiftmountainAI->AddWaypoint(9, -5178.44, -1133.57, 43.91);
    paoka_swiftmountainAI->AddWaypoint(10, -5176.68, -1129.48, 43.81);
    paoka_swiftmountainAI->AddWaypoint(11, -5164.85, -1123.33, 43.99);
    paoka_swiftmountainAI->AddWaypoint(12, -5153.07, -1117.7, 43.66);
    paoka_swiftmountainAI->AddWaypoint(13, -5143.52, -1113.14, 43.78);
    paoka_swiftmountainAI->AddWaypoint(14, -5135.86, -1104.42, 47.23);
    paoka_swiftmountainAI->AddWaypoint(15, -5129.86, -1097.22, 49.52);
    paoka_swiftmountainAI->AddWaypoint(16, -5127.48, -1087.29, 49.03);
    paoka_swiftmountainAI->AddWaypoint(17, -5127.57, -1080.4, 46.64);
    paoka_swiftmountainAI->AddWaypoint(18, -5129.65, -1077.58, 45.29);
    paoka_swiftmountainAI->AddWaypoint(19, -5135.86, -1069.12, 39.53);
    paoka_swiftmountainAI->AddWaypoint(20, -5141.97, -1060.58, 32.70);
    paoka_swiftmountainAI->AddWaypoint(21, -5145.99, -1054.85, 28.98);
    paoka_swiftmountainAI->AddWaypoint(22, -5147.08, -1050.35, 26.36);
    paoka_swiftmountainAI->AddWaypoint(23, -5147.5, -1043.37, 21.84);
    paoka_swiftmountainAI->AddWaypoint(24, -5147.68, -1036.37, 17.05);
    paoka_swiftmountainAI->AddWaypoint(25, -5147.68, -1029.37, 14.59);
    paoka_swiftmountainAI->AddWaypoint(26, -5144.62, -1023.9, 11.67);
    paoka_swiftmountainAI->AddWaypoint(27, -5138.67, -1020.23, 7.81);
    paoka_swiftmountainAI->AddWaypoint(28, -5135.6, -1018.55, 6.19);
    paoka_swiftmountainAI->AddWaypoint(29, -5126.25, -1014.76, 1.08);
    paoka_swiftmountainAI->AddWaypoint(30, -5120.03, -1013.12, -1.11);
    paoka_swiftmountainAI->AddWaypoint(31, -5112.3, -1027.65, -5.39);
    paoka_swiftmountainAI->AddWaypoint(32, -5106.99, -1023.09, -5.10);
    paoka_swiftmountainAI->AddWaypoint(33, -5099.07, -1016.19, -4.92);
    paoka_swiftmountainAI->AddWaypoint(34, -5091.23, -1009.21, -5.22);
    paoka_swiftmountainAI->AddWaypoint(35, -5083.62, -1001.97, -5.22);
    paoka_swiftmountainAI->AddWaypoint(36, -5076.1, -994.652, -4.92);
    paoka_swiftmountainAI->AddWaypoint(37, -5066.71, -985.507, -4.97);
    paoka_swiftmountainAI->AddWaypoint(38, -5065.13, -978.689, -5.02);
    paoka_swiftmountainAI->AddWaypoint(39, -5062.33, -968.57, -5.08);
    paoka_swiftmountainAI->AddWaypoint(40, -5059.46, -958.469, -5.16);
    paoka_swiftmountainAI->AddWaypoint(41, -5056.59, -948.375, -5.10);
    paoka_swiftmountainAI->AddWaypoint(42, -5053.73, -938.274, -5.69);
    paoka_swiftmountainAI->AddWaypoint(43, -5043.06, -934.822, -5.35);
    paoka_swiftmountainAI->AddWaypoint(44, -5029.35, -932.007, -5.27);
    paoka_swiftmountainAI->AddWaypoint(45, -5024.58, -933.781, -5.40);
    paoka_swiftmountainAI->AddWaypoint(46, -5019.13, -938.172, -5.54);
    paoka_swiftmountainAI->AddWaypoint(47, -5011, -944.812, -5.47);
    paoka_swiftmountainAI->AddWaypoint(48, -5002.86, -951.455, -5.44);
    paoka_swiftmountainAI->AddWaypoint(49, -4994.73, -958.099, -5.41);
    paoka_swiftmountainAI->AddWaypoint(50, -4990.57, -963.782, -5.33);
    paoka_swiftmountainAI->AddWaypoint(51, -4987.43, -970.041, -5.17);
    paoka_swiftmountainAI->AddWaypoint(52, -4981.41, -982.678, -5.10);
    paoka_swiftmountainAI->AddWaypoint(53, -4977.03, -992.221, -4.97);
    paoka_swiftmountainAI->AddWaypoint(54, -4972.6, -1001.74, -5.24);
    paoka_swiftmountainAI->AddWaypoint(55, -4968.15, -1011.25, -5.49);
    paoka_swiftmountainAI->AddWaypoint(56, -4963.75, -1020.78, -5.07);
    paoka_swiftmountainAI->AddWaypoint(57, -4959.3, -1030.3, -5.28);
    paoka_swiftmountainAI->AddWaypoint(58, -4956.28, -1036.61, -5.84);
    paoka_swiftmountainAI->AddWaypoint(59, -4952.05, -1043.75, -9.04);
    paoka_swiftmountainAI->AddWaypoint(60, -4943.8, -1055.06, -17.91);
    paoka_swiftmountainAI->AddWaypoint(61, -4939.47, -1055.61, -20.73);
    paoka_swiftmountainAI->AddWaypoint(62, -4929.16, -1053.64, -25.65);
    paoka_swiftmountainAI->AddWaypoint(63, -4922.28, -1052.37, -28.72);
    paoka_swiftmountainAI->AddWaypoint(64, -4913.26, -1051.3, -31.80);
    paoka_swiftmountainAI->AddWaypoint(65, -4903.54, -1054.17, -33.48);
    paoka_swiftmountainAI->AddWaypoint(66, -4905.95, -1056.71, -33.68);
    paoka_swiftmountainAI->AddWaypoint(67, -4913.18, -1064.32, -39.46);
    paoka_swiftmountainAI->AddWaypoint(68, -4925.27, -1076.98, -47.39);
    paoka_swiftmountainAI->AddWaypoint(69, -4932.68, -1084.42, -51.00);
    paoka_swiftmountainAI->AddWaypoint(70, -4938.3, -1100.41, -50.71, 5000);
    paoka_swiftmountainAI->AddWaypoint(71, -4937.34, -1102.87, -49.82);

    return (CreatureAI*)paoka_swiftmountainAI;
}

bool QuestAccept_npc_paoka_swiftmountain(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_HOMEWARD)
    {
        DoScriptText(SAY_START, pCreature, pPlayer);
        pCreature->setFaction(FACTION_ESCORTEE);

        if (npc_paoka_swiftmountainAI* pEscortAI = CAST_AI(npc_paoka_swiftmountainAI,pCreature->AI()))
            pEscortAI->Start(false, false, pPlayer->GetGUID(), pQuest);
    }
    return true;
}

/*#####
# npc_plucky
######*/

#define GOSSIP_P    "Please tell me the Phrase.."

enum ePlucky
{
    FACTION_FRIENDLY        = 35,
    QUEST_SCOOP             = 1950,
    SPELL_PLUCKY_HUMAN      = 9192,
    SPELL_PLUCKY_CHICKEN    = 9220
};

struct npc_pluckyAI : public ScriptedAI
{
    npc_pluckyAI(Creature *c) : ScriptedAI(c) { m_uiNormFaction = c->getFaction(); }

    uint32 m_uiNormFaction;
    uint32 m_uiResetTimer;

    void Reset()
    {
        m_uiResetTimer = 120000;

        if (me->getFaction() != m_uiNormFaction)
            me->setFaction(m_uiNormFaction);

        if (me->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        DoCast(me, SPELL_PLUCKY_CHICKEN, false);
    }

    void ReceiveEmote(Player* pPlayer, uint32 uiTextEmote)
    {
        if (pPlayer->GetQuestStatus(QUEST_SCOOP) == QUEST_STATUS_INCOMPLETE)
        {
            if (uiTextEmote == TEXTEMOTE_BECKON)
            {
                me->setFaction(FACTION_FRIENDLY);
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                DoCast(me, SPELL_PLUCKY_HUMAN, false);
            }
        }

        if (uiTextEmote == TEXTEMOTE_CHICKEN)
        {
            if (me->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
                return;
            else
            {
                me->setFaction(FACTION_FRIENDLY);
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                DoCast(me, SPELL_PLUCKY_HUMAN, false);
                me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (me->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
        {
            if (m_uiResetTimer <= uiDiff)
            {
                if (!me->getVictim())
                    EnterEvadeMode();
                else
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                return;
            }
            else
                m_uiResetTimer -= uiDiff;
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

bool GossipHello_npc_plucky(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_SCOOP) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_P, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    pPlayer->SEND_GOSSIP_MENU(738, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_plucky(Player* pPlayer, Creature* /*pCreature*/, uint32 /*uiSender*/, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CompleteQuest(QUEST_SCOOP);
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

    void UpdateAI(const uint32 /*diff*/)
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
    newscript->Name = "npc_paoka_swiftmountain";
    newscript->GetAI = &GetAI_npc_paoka_swiftmountain;
    newscript->pQuestAccept = &QuestAccept_npc_paoka_swiftmountain;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_plucky";
    newscript->GetAI = &GetAI_npc_plucky;
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

