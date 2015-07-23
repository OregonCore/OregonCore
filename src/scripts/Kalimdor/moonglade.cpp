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
SDName: Moonglade
SD%Complete: 100
SDComment: Quest support: 30, 272, 5929, 5930, 10965. Special Flight Paths for Druid class.
SDCategory: Moonglade
EndScriptData */

/* ContentData
npc_bunthen_plainswind
npc_great_bear_spirit
npc_silva_filnaveth
npc_clintar_spirit
npc_clintar_dreamwalker
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "ScriptedGossip.h"

/*######
## npc_bunthen_plainswind
######*/

enum eBunthen
{
    QUEST_SEA_LION_HORDE        = 30,
    QUEST_SEA_LION_ALLY         = 272,
    TAXI_PATH_ID_ALLY           = 315,
    TAXI_PATH_ID_HORDE          = 316
};

#define GOSSIP_ITEM_THUNDER     "I'd like to fly to Thunder Bluff."
#define GOSSIP_ITEM_AQ_END      "Do you know where I can find Half Pendant of Aquatic Endurance?"

bool GossipHello_npc_bunthen_plainswind(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->getClass() != CLASS_DRUID)
        pPlayer->SEND_GOSSIP_MENU(4916, pCreature->GetGUID());
    else if (pPlayer->GetTeam() != HORDE)
    {
        if (pPlayer->GetQuestStatus(QUEST_SEA_LION_ALLY) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_AQ_END, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

        pPlayer->SEND_GOSSIP_MENU(4917, pCreature->GetGUID());
    }
    else if (pPlayer->getClass() == CLASS_DRUID && pPlayer->GetTeam() == HORDE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_THUNDER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        if (pPlayer->GetQuestStatus(QUEST_SEA_LION_HORDE) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_AQ_END, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

        pPlayer->SEND_GOSSIP_MENU(4918, pCreature->GetGUID());
    }
    return true;
}

bool GossipSelect_npc_bunthen_plainswind(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:
        pPlayer->CLOSE_GOSSIP_MENU();
        if (pPlayer->getClass() == CLASS_DRUID && pPlayer->GetTeam() == HORDE)
        {
            std::vector<uint32> nodes;

            nodes.resize(2);
            nodes[0] = 63;                              // Nighthaven, Moonglade
            nodes[1] = 22;                              // Thunder Bluff, Mulgore
            pPlayer->ActivateTaxiPathTo(nodes);
        }
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:
        pPlayer->SEND_GOSSIP_MENU(5373, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:
        pPlayer->SEND_GOSSIP_MENU(5376, pCreature->GetGUID());
        break;
    }
    return true;
}

/*######
## npc_great_bear_spirit
######*/

#define GOSSIP_BEAR1 "What do you represent, spirit?"
#define GOSSIP_BEAR2 "I seek to understand the importance of strength of the body."
#define GOSSIP_BEAR3 "I seek to understand the importance of strength of the heart."
#define GOSSIP_BEAR4 "I have heard your words, Great Bear Spirit, and I understand. I now seek your blessings to fully learn the way of the Claw."

bool GossipHello_npc_great_bear_spirit(Player* pPlayer, Creature* pCreature)
{
    //ally or horde quest
    if (pPlayer->GetQuestStatus(5929) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(5930) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BEAR1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        pPlayer->SEND_GOSSIP_MENU(4719, pCreature->GetGUID());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(4718, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_great_bear_spirit(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BEAR2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(4721, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 1:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BEAR3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        pPlayer->SEND_GOSSIP_MENU(4733, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BEAR4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        pPlayer->SEND_GOSSIP_MENU(4734, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:
        pPlayer->SEND_GOSSIP_MENU(4735, pCreature->GetGUID());
        if (pPlayer->GetQuestStatus(5929) == QUEST_STATUS_INCOMPLETE)
            pPlayer->AreaExploredOrEventHappens(5929);
        if (pPlayer->GetQuestStatus(5930) == QUEST_STATUS_INCOMPLETE)
            pPlayer->AreaExploredOrEventHappens(5930);
        break;
    }
    return true;
}

/*######
## npc_silva_filnaveth
######*/

#define GOSSIP_ITEM_RUTHERAN    "I'd like to fly to Rut'theran Village."
#define GOSSIP_ITEM_AQ_AGI      "Do you know where I can find Half Pendant of Aquatic Agility?"

bool GossipHello_npc_silva_filnaveth(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->getClass() != CLASS_DRUID)
        pPlayer->SEND_GOSSIP_MENU(4913, pCreature->GetGUID());
    else if (pPlayer->GetTeam() != ALLIANCE)
    {
        if (pPlayer->GetQuestStatus(QUEST_SEA_LION_HORDE) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_AQ_AGI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

        pPlayer->SEND_GOSSIP_MENU(4915, pCreature->GetGUID());
    }
    else if (pPlayer->getClass() == CLASS_DRUID && pPlayer->GetTeam() == ALLIANCE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RUTHERAN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        if (pPlayer->GetQuestStatus(QUEST_SEA_LION_ALLY) == QUEST_STATUS_INCOMPLETE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_AQ_AGI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

        pPlayer->SEND_GOSSIP_MENU(4914, pCreature->GetGUID());
    }
    return true;
}

bool GossipSelect_npc_silva_filnaveth(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    switch (uiAction)
    {
    case GOSSIP_ACTION_INFO_DEF + 1:
        pPlayer->CLOSE_GOSSIP_MENU();
        if (pPlayer->getClass() == CLASS_DRUID && pPlayer->GetTeam() == ALLIANCE)
        {
            std::vector<uint32> nodes;

            nodes.resize(2);
            nodes[0] = 62;                              // Nighthaven, Moonglade
            nodes[1] = 27;                              // Rut'theran Village, Teldrassil
            pPlayer->ActivateTaxiPathTo(nodes);
        }
        break;
    case GOSSIP_ACTION_INFO_DEF + 2:
        pPlayer->SEND_GOSSIP_MENU(5374, pCreature->GetGUID());
        break;
    case GOSSIP_ACTION_INFO_DEF + 3:
        pPlayer->SEND_GOSSIP_MENU(5375, pCreature->GetGUID());
        break;
    }
    return true;
}

/*######
## npc_clintar_spirit
######*/

float Clintar_spirit_WP[41][5] =
{
    //pos_x   pos_y    pos_z    orien waitTime
    {7465.28f, -3115.46f, 439.327f, 0.83f, 4000},
    {7476.49f, -3101,    443.457f, 0.89f, 0},
    {7486.57f, -3085.59f, 439.478f, 1.07f, 0},
    {7472.19f, -3085.06f, 443.142f, 3.07f, 0},
    {7456.92f, -3085.91f, 438.862f, 3.24f, 0},
    {7446.68f, -3083.43f, 438.245f, 2.40f, 0},
    {7446.17f, -3080.21f, 439.826f, 1.10f, 6000},
    {7452.41f, -3085.8f,  438.984f, 5.78f, 0},
    {7469.11f, -3084.94f, 443.048f, 6.25f, 0},
    {7483.79f, -3085.44f, 439.607f, 6.25f, 0},
    {7491.14f, -3090.96f, 439.983f, 5.44f, 0},
    {7497.62f, -3098.22f, 436.854f, 5.44f, 0},
    {7498.72f, -3113.41f, 434.596f, 4.84f, 0},
    {7500.06f, -3122.51f, 434.749f, 5.17f, 0},
    {7504.96f, -3131.53f, 434.475f, 4.74f, 0},
    {7504.31f, -3133.53f, 435.693f, 3.84f, 6000},
    {7504.55f, -3133.27f, 435.476f, 0.68f, 15000},
    {7501.99f, -3126.01f, 434.93f,  1.83f, 0},
    {7490.76f, -3114.97f, 434.431f, 2.51f, 0},
    {7479.64f, -3105.51f, 431.123f, 1.83f, 0},
    {7474.63f, -3086.59f, 428.994f, 1.83f, 2000},
    {7472.96f, -3074.18f, 427.566f, 1.57f, 0},
    {7472.25f, -3063,    428.268f, 1.55f, 0},
    {7473.46f, -3054.22f, 427.588f, 0.36f, 0},
    {7475.08f, -3053.6f,  428.653f, 0.36f, 6000},
    {7474.66f, -3053.56f, 428.433f, 3.19f, 4000},
    {7471.81f, -3058.84f, 427.073f, 4.29f, 0},
    {7472.16f, -3064.91f, 427.772f, 4.95f, 0},
    {7471.56f, -3085.36f, 428.924f, 4.72f, 0},
    {7473.56f, -3093.48f, 429.294f, 5.04f, 0},
    {7478.94f, -3104.29f, 430.638f, 5.23f, 0},
    {7484.46f, -3109.61f, 432.769f, 5.79f, 0},
    {7490.23f, -3111.08f, 434.431f, 0.02f, 0},
    {7496.29f, -3108,    434.783f, 1.15f, 0},
    {7497.46f, -3100.66f, 436.191f, 1.50f, 0},
    {7495.64f, -3093.39f, 438.349f, 2.10f, 0},
    {7492.44f, -3086.01f, 440.267f, 1.38f, 0},
    {7498.26f, -3076.44f, 440.808f, 0.71f, 0},
    {7506.4f,  -3067.35f, 443.64f,  0.77f, 0},
    {7518.37f, -3057.42f, 445.584f, 0.74f, 0},
    {7517.51f, -3056.3f,  444.568f, 2.49f, 4500}
};

#define ASPECT_RAVEN 22915

#define ASPECT_RAVEN_SUMMON_X 7472.96f
#define ASPECT_RAVEN_SUMMON_Y -3074.18f
#define ASPECT_RAVEN_SUMMON_Z 427.566f
#define CLINTAR_SPIRIT_SUMMON_X 7459.2275f
#define CLINTAR_SPIRIT_SUMMON_Y -3122.5632f
#define CLINTAR_SPIRIT_SUMMON_Z 438.9842f
#define CLINTAR_SPIRIT_SUMMON_O 0.8594f

enum eSpirit
{
    CLINTAR_SPIRIT_SAY_START           = -1000280,
    CLINTAR_SPIRIT_SAY_UNDER_ATTACK_1  = -1000281,
    CLINTAR_SPIRIT_SAY_UNDER_ATTACK_2  = -1000282,
    CLINTAR_SPIRIT_SAY_GET_ONE         = -1000283,
    CLINTAR_SPIRIT_SAY_GET_TWO         = -1000284,
    CLINTAR_SPIRIT_SAY_GET_THREE       = -1000285,
    CLINTAR_SPIRIT_SAY_GET_FINAL       = -1000286
};

struct npc_clintar_spiritAI : public npc_escortAI
{
    public:
        npc_clintar_spiritAI(Creature* c) : npc_escortAI(c) {}

        uint32 Step;
        uint32 CurrWP;
        uint32 Event_Timer;
        uint32 checkPlayer_Timer;

        uint64 PlayerGUID;

        bool Event_onWait;

        void Reset()
        {
            if (!PlayerGUID)
            {
                Step = 0;
                CurrWP = 0;
                Event_Timer = 0;
                PlayerGUID = 0;
                checkPlayer_Timer = 1000;
                Event_onWait = false;
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            if (!PlayerGUID)
                return;

            Player* pPlayer = Unit::GetPlayer(*me, PlayerGUID);
            if (pPlayer && pPlayer->GetQuestStatus(10965) == QUEST_STATUS_INCOMPLETE)
            {
                pPlayer->FailQuest(10965);
                PlayerGUID = 0;
                Reset();
            }
        }

        void EnterEvadeMode()
        {
            Player* pPlayer = Unit::GetPlayer(*me, PlayerGUID);
            if (pPlayer && pPlayer->IsInCombat() && pPlayer->getAttackerForHelper())
            {
                AttackStart(pPlayer->getAttackerForHelper());
                return;
            }
            npc_escortAI::EnterEvadeMode();
        }

        void EnterCombat(Unit* who)
        {
            uint32 rnd = rand() % 2;
            switch (rnd)
            {
            case 0:
                DoScriptText(CLINTAR_SPIRIT_SAY_UNDER_ATTACK_1, me, who);
                break;
            case 1:
                DoScriptText(CLINTAR_SPIRIT_SAY_UNDER_ATTACK_2, me, who);
                break;
            }
        }

        void StartEvent(Player* pPlayer)
        {
            if (!pPlayer)
                return;
            if (pPlayer->GetQuestStatus(10965) == QUEST_STATUS_INCOMPLETE)
            {
                for (uint8 i = 0; i < 41; ++i)
                    AddWaypoint(i, Clintar_spirit_WP[i][0], Clintar_spirit_WP[i][1], Clintar_spirit_WP[i][2], (uint32)Clintar_spirit_WP[i][4]);
                PlayerGUID = pPlayer->GetGUID();
                Start(true, false, PlayerGUID);
            }
            return;
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!PlayerGUID)
            {
                me->setDeathState(JUST_DIED);
                return;
            }

            if (!me->IsInCombat() && !Event_onWait)
            {
                if (checkPlayer_Timer <= diff)
                {
                    Player* pPlayer = Unit::GetPlayer(*me, PlayerGUID);
                    if (pPlayer && pPlayer->IsInCombat() && pPlayer->getAttackerForHelper())
                        AttackStart(pPlayer->getAttackerForHelper());
                    checkPlayer_Timer = 1000;
                }
                else checkPlayer_Timer -= diff;
            }

            if (Event_onWait && Event_Timer <= diff)
            {

                Player* pPlayer = Unit::GetPlayer(*me, PlayerGUID);
                if (!pPlayer || (pPlayer && pPlayer->GetQuestStatus(10965) == QUEST_STATUS_NONE))
                {
                    me->setDeathState(JUST_DIED);
                    return;
                }

                switch (CurrWP)
                {
                case 0:
                    switch (Step)
                    {
                    case 0:
                        me->Say(CLINTAR_SPIRIT_SAY_START, 0, PlayerGUID);
                        Event_Timer = 8000;
                        Step = 1;
                        break;
                    case 1:
                        Event_onWait = false;
                        break;
                    }
                    break;
                case 6:
                    switch (Step)
                    {
                    case 0:
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 133);
                        Event_Timer = 5000;
                        Step = 1;
                        break;
                    case 1:
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                        DoScriptText(CLINTAR_SPIRIT_SAY_GET_ONE, me, pPlayer);
                        Event_onWait = false;
                        break;
                    }
                    break;
                case 15:
                    switch (Step)
                    {
                    case 0:
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 133);
                        Event_Timer = 5000;
                        Step = 1;
                        break;
                    case 1:
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                        Event_onWait = false;
                        break;
                    }
                    break;
                case 16:
                    switch (Step)
                    {
                    case 0:
                        DoScriptText(CLINTAR_SPIRIT_SAY_GET_TWO, me, pPlayer);
                        Event_Timer = 15000;
                        Step = 1;
                        break;
                    case 1:
                        Event_onWait = false;
                        break;
                    }
                    break;
                case 20:
                    switch (Step)
                    {
                    case 0:
                        {
                            Creature* mob = me->SummonCreature(ASPECT_RAVEN, ASPECT_RAVEN_SUMMON_X, ASPECT_RAVEN_SUMMON_Y, ASPECT_RAVEN_SUMMON_Z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 2000);
                            if (mob)
                            {
                                mob->AddThreat(me, 10000.0f);
                                mob->AI()->AttackStart(me);
                            }
                            Event_Timer = 2000;
                            Step = 1;
                            break;
                        }
                    case 1:
                        Event_onWait = false;
                        break;
                    }
                    break;
                case 24:
                    switch (Step)
                    {
                    case 0:
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 133);
                        Event_Timer = 5000;
                        Step = 1;
                        break;
                    case 1:
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                        Event_onWait = false;
                        break;
                    }
                    break;
                case 25:
                    switch (Step)
                    {
                    case 0:
                        DoScriptText(CLINTAR_SPIRIT_SAY_GET_THREE, me, pPlayer);
                        Event_Timer = 4000;
                        Step = 1;
                        break;
                    case 1:
                        Event_onWait = false;
                        break;
                    }
                    break;
                case 40:
                    switch (Step)
                    {
                    case 0:
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 2);
                        DoScriptText(CLINTAR_SPIRIT_SAY_GET_FINAL, me, pPlayer);
                        pPlayer->CompleteQuest(10965);
                        Event_Timer = 1500;
                        Step = 1;
                        break;
                    case 1:
                        me->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                        Event_Timer = 3000;
                        Step = 2;
                        break;
                    case 2:
                        pPlayer->TalkedToCreature(me->GetEntry(), me->GetGUID());
                        PlayerGUID = 0;
                        Reset();
                        me->setDeathState(JUST_DIED);
                        break;
                    }
                    break;
                default:
                    Event_onWait = false;
                    break;
                }

            }
            else if (Event_onWait) Event_Timer -= diff;
        }

        void WaypointReached(uint32 id)
        {
            CurrWP = id;
            Event_Timer = 0;
            Step = 0;
            Event_onWait = true;
        }
};

CreatureAI* GetAI_npc_clintar_spirit(Creature* pCreature)
{
    return new npc_clintar_spiritAI (pCreature);
}

/*####
# npc_clintar_dreamwalker
####*/

#define CLINTAR_SPIRIT 22916

bool QuestAccept_npc_clintar_dreamwalker(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == 10965)
    {
        Creature* clintar_spirit = pCreature->SummonCreature(CLINTAR_SPIRIT, CLINTAR_SPIRIT_SUMMON_X, CLINTAR_SPIRIT_SUMMON_Y, CLINTAR_SPIRIT_SUMMON_Z, CLINTAR_SPIRIT_SUMMON_O, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 100000);
        if (clintar_spirit)
            CAST_AI(npc_clintar_spiritAI, clintar_spirit->AI())->StartEvent(pPlayer);
    }
    return true;
}

/*####
##                      Quest: The Nightmare Manifests
## When we created this script. We tried to make it as much as possible blizzlike.
## But at the same time, we had some things replaced, for example, the coordinates
## of the Nightmares. The coordinates of the Nightmares had to do more clustered, because
## the current system scripting and muvemaps not allow to place Nightmares blizzlike.
####*/

enum TheNightmareManifests
{
    REMULOS_SAY_1                        = -1000672,
    REMULOS_SAY_2                        = -1000673,
    REMULOS_SAY_3                        = -1000674,
    REMULOS_SAY_4                        = -1000675,
    REMULOS_SAY_5                        = -1000676,
    ERANIKUS_ZONE_EMOTE_ENTER            = -1000677,
    ERANIKUS_YELL_1                      = -1000678,
    REMULOS_YELL_1                       = -1000679,
    ERANIKUS_EMOTE_LAUGHT                = -1000680,
    ERANIKUS_YELL_2                      = -1000681,
    REMULOS_YELL_2                       = -1000682,
    ERANIKUS_YELL_3                      = -1000683,
    ERANIKUS_EMOTE_ROAR                  = -1000684,
    REMULOS_SAY_6                        = -1000685,
    REMULOS_SAY_7                        = -1000686,
    ERANIKUS_YELL_4                      = -1000687,
    REMULOS_SAY_8                        = -1000688,
    ERANIKUS_YELL_5                      = -1000689,
    ERANIKUS_YELL_6                      = -1000690,
    ERANIKUS_YELL_7                      = -1000691,
    TYRANDE_YELL_1                       = -1000692,
    TYRANDE_SAY_1                        = -1000693,
    TYRANDE_YELL_2                       = -1000694,
    TYRANDE_YELL_3                       = -1000695,
    TYRANDE_SAY_2                        = -1000696,
    ERANIKUS_YELL_8                      = -1000697,
    ERANIKUS_YELL_9                      = -1000698,
    ERANIKUS_YELL_10                     = -1000699,
    ERANIKUS_ZONE_EMOTE_CONSUMED         = -1000700,
    TYRANDE_EMOTE_KNEE                   = -1000701,
    TYRANDE_YELL_4                       = -1000702,
    ERANIKUS_SAY_1                       = -1000703,
    ERANIKUS_SAY_2                       = -1000704,
    ERANIKUS_SAY_3                       = -1000705,
    ERANIKUS_SAY_4                       = -1000706,
    REMULOS_SAY_9                        = -1000707,
    REMULOS_SAY_10                       = -1000708,

    QUEST_NIGHTMARE_MANIFESTS            = 8736,

    NPC_REMULOS                          = 11832,
    NPC_ERANIKUS                         = 15491,
    NPC_TYRANDE                          = 15633,
    NPC_NIGHTMARE_PHANTASM               = 15629,
    NPC_MOONGLADE_WARDEN                 = 11822,
    NPC_ERANIKUS_THE_REDEEMED            = 15628,
    NPC_PRIESTESS_OF_THE_MOON            = 15634,

    NPC_NIGHTMARE_PHANTASMS_COUNT        = 15,
    NPC_MOONGLADE_WARDENS_COUNT          = 10,
    NPC_PRIESTESS_OF_THE_MOON_COUNT      = 7,


    SPELL_C0NJURE_DREAM_RIFT             = 25813,
    SPELL_STARFIRE                       = 21668,
    SPELL_HEALING_TOUCH                  = 23381,
    SPELL_REGROWTH                       = 20665,
    SPELL_REJUVENATION                   = 20664,
    SPELL_TRANQUILITY                    = 25817,

    SPELL_SPOTLIGHT                      = 35259,
    ERANIKUS_SPELL_ACID_BREATH           = 24839,
    ERANIKUS_SPELL_NOXIUS_BREATH         = 24818,
    ERANIKUS_SPELL_SHADOW_BOLT_VOLLEY    = 25586,

    POINT_COMBAT_START                   = 0xFFFFFF,
    POINT_NIGHTRAVEN                     = 0xFFFFF0
};

float PhantasmsSpawnpos[6][4] =
{
    {7865.17f, -2549.79f, 486.685f, 5.0f},
    {7889.45f, -2580.95f, 487.039f, 1.7f},
    {7916.65f, -2556.21f, 487.336f, 1.1f},
    {7926.25f, -2574.34f, 489.651f, 2.9f},
    {7855.32f, -2595.67f, 486.713f, 0.5f},
    {7890.27f, -2529.94f, 483.771f, 4.7f},
};

struct Location
{
    float m_fX, m_fY, m_fZ, m_fO;
};

Location PristessHomePoint[] =
{
    {7883.12f, -2560.05f, 486.837f, 0.02f},
    {7882.61f, -2562.55f, 486.947f, 6.17f},
    {7882.21f, -2565.84f, 486.947f, 6.13f},
    {7882.12f, -2569.81f, 486.947f, 6.18f},
    {7878.62f, -2563.07f, 486.947f, 0.03f},
    {7878.14f, -2566.98f, 486.947f, 6.13f},
    {7880.23f, -2565.02f, 486.947f, 0.06f}
};

/*######
## npc_remulos
######*/

struct npc_remulosAI : public npc_escortAI
{
    npc_remulosAI(Creature* c) : npc_escortAI(c) {}

    uint64 EranikusGUID;
    uint32 uiPhase;
    uint32 EventTimer;
    uint32 FindVictimTimer;
    uint32 StarfireTimer;
    uint32 HealingTouchTimer;
    uint32 RegrowthTimer;
    uint32 RejuvenationTimer;
    uint32 TranquilityTimer;
    uint32 DeadPhantasmsCount;
    bool PhantasmPhase;
    bool EranikusPhase;

    std::list<uint64> PhantasmsList;
    std::list<uint64> MoongladeWardenList;

    void Reset()
    {
        PhantasmPhase = false;
        EranikusPhase = false;
        EventTimer = 0;
        uiPhase = 0;
        FindVictimTimer = 2000;
        DeadPhantasmsCount = 0;
        StarfireTimer = urand(7000, 10000);
        HealingTouchTimer = 4000;
        RegrowthTimer = urand(5000, 10000);
        RejuvenationTimer = urand(5000, 10000);
        TranquilityTimer = urand(15000, 30000);
        EranikusGUID = 0;

        PhantasmsList.clear();
        MoongladeWardenList.clear();
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_NIGHTMARE_PHANTASM)
        {
            PhantasmsList.push_back(pSummoned->GetGUID());
            pSummoned->GetMotionMaster()->MoveChase(me);
            pSummoned->AI()->AttackStart(me);
        }

        if (pSummoned->GetEntry() == NPC_MOONGLADE_WARDEN)
            MoongladeWardenList.push_back(pSummoned->GetGUID());
    }

    void ClearSummonedCreatures()
    {
        if (!MoongladeWardenList.empty())
        {
            for (std::list<uint64>::iterator itr = MoongladeWardenList.begin(); itr != MoongladeWardenList.end(); ++itr)
            {
                if (Creature* pWarden = Unit::GetCreature(*me, *itr))
                    pWarden->ForcedDespawn();
            }
        }
        MoongladeWardenList.clear();

        if (!PhantasmsList.empty())
        {
            for (std::list<uint64>::iterator itr = PhantasmsList.begin(); itr != PhantasmsList.end(); ++itr)
            {
                if (Creature* pPhantasm = Unit::GetCreature(*me, *itr))
                    pPhantasm->ForcedDespawn();
            }
        }
        PhantasmsList.clear();
    }

    void JustDied(Unit* /*killer*/)
    {
        ClearSummonedCreatures();
        if (Creature* pEranikus = Unit::GetCreature(*me, EranikusGUID))
            pEranikus->ForcedDespawn();
    }

    void FindVictim(Creature* pCreature)
    {
        if (!pCreature->getVictim())
        {
            if (PhantasmPhase)
            {
                if (Unit* pTarget = pCreature->FindNearestCreature(NPC_NIGHTMARE_PHANTASM, 50.0f))
                {
                    pCreature->GetMotionMaster()->MoveChase(pTarget);
                    pCreature->AI()->AttackStart(pTarget);
                    return;
                }
            }

            if (EranikusPhase)
            {
                if (Creature* pEranikus = Unit::GetCreature(*me, EranikusGUID))
                {
                    pCreature->GetMotionMaster()->MoveChase(pEranikus);
                    pCreature->AI()->AttackStart(pEranikus);
                    return;
                }
            }
            return;
        }
    }

    void CheckNightmare()
    {
        if (!PhantasmsList.empty())
        {
            for (std::list<uint64>::iterator itr = PhantasmsList.begin(); itr != PhantasmsList.end(); ++itr)
            {
                if (Creature* pPhantasm = Unit::GetCreature(*me, *itr))
                {
                    if (!pPhantasm->IsAlive())
                    {
                        PhantasmsList.erase(itr);
                        DeadPhantasmsCount++;
                        break;
                    }
                }
            }
        }
        if (DeadPhantasmsCount >= 15)
        {
            PhantasmsList.clear();
            EventTimer = 10000;
            uiPhase = 14;
        }
    }

    void WaypointReached(uint32 uiPointId)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (uiPointId)
        {
        case 18:
            SetEscortPaused(true);
            DoScriptText(REMULOS_SAY_3, me, pPlayer);
            EventTimer = 5000;
            uiPhase = 2;
            break;
        case 23:
            SetEscortPaused(true);
            me->SetOrientation(0.044f);
            me->SendMovementFlagUpdate();
            DoScriptText(REMULOS_SAY_7, me);
            EventTimer = 7000;
            uiPhase = 11;
            break;
        case 24:
            SetEscortPaused(true);
            break;
        case 26:
            SetEscortPaused(true);
            if (/*Creature* pEranikus = */Unit::GetCreature(*me, EranikusGUID))
            {
                me->SetOrientation(1.46);
                me->SendMovementFlagUpdate();
            }
            break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);

        if (uiPhase)
        {
            if (EventTimer <= diff)
            {
                Player* pPlayer = GetPlayerForEscort();

                if (!pPlayer)
                    return;

                switch (uiPhase)
                {
                case 1:
                    DoScriptText(REMULOS_SAY_2, me);
                    uiPhase = 0;
                    SetEscortPaused(false);
                    break;
                case 2:
                    DoScriptText(REMULOS_SAY_4, me);
                    uiPhase = 3;
                    EventTimer = 10000;
                    break;
                case 3:
                    DoScriptText(REMULOS_SAY_5, me);
                    DoCast(SPELL_C0NJURE_DREAM_RIFT);
                    uiPhase = 4;
                    EventTimer = 10000;
                    break;
                case 4:
                    if (Creature* pEranikus = me->SummonCreature(NPC_ERANIKUS, 7867.44f, -2671.37f, 498.042f, 0.51f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
                    {
                        //@todo now must be server-wide emote
                        DoScriptText(ERANIKUS_ZONE_EMOTE_ENTER, pEranikus);
                        pEranikus->SetReactState(REACT_PASSIVE);
                        pEranikus->SetCanFly(true);
                        pEranikus->AI()->IsSummonedBy(me);
                        pEranikus->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        EranikusGUID = pEranikus->GetGUID();
                    }
                    else me->ForcedDespawn();
                    uiPhase = 5;
                    EventTimer = 10000;
                    break;
                case 5:
                    if (Creature* pEranikus = Unit::GetCreature(*me, EranikusGUID))
                        DoScriptText(ERANIKUS_YELL_1, pEranikus);
                    uiPhase = 6;
                    EventTimer = 10000;
                    break;
                case 6:
                    DoScriptText(REMULOS_YELL_1, me);
                    uiPhase = 7;
                    EventTimer = 5000;
                    break;
                case 7:
                    if (Creature* pEranikus = Unit::GetCreature(*me, EranikusGUID))
                    {
                        DoScriptText(ERANIKUS_EMOTE_LAUGHT, pEranikus);
                        DoScriptText(ERANIKUS_YELL_2, pEranikus);
                    }
                    uiPhase = 8;
                    EventTimer = 8000;
                    break;
                case 8:
                    DoScriptText(REMULOS_YELL_2, me);
                    uiPhase = 9;
                    EventTimer = 9000;
                    break;
                case 9:
                    if (Creature* pEranikus = Unit::GetCreature(*me, EranikusGUID))
                    {
                        DoScriptText(ERANIKUS_EMOTE_LAUGHT, pEranikus);
                        DoScriptText(ERANIKUS_YELL_3, pEranikus);
                        pEranikus->GetMotionMaster()->MovePoint(POINT_NIGHTRAVEN, 7926.28f, -2573.13f, 501.655f);
                        pEranikus->SetHomePosition(7926.28f, -2573.13f, 501.655f, 2.88f);
                    }
                    uiPhase = 10;
                    EventTimer = 5000;
                    break;
                case 10:
                    DoScriptText(REMULOS_SAY_6, me, pPlayer);
                    SetEscortPaused(false);
                    uiPhase = 0;
                    break;
                case 11:
                    SetEscortPaused(false);
                    if (Creature* pEranikus = Unit::GetCreature(*me, EranikusGUID))
                    {
                        DoScriptText(ERANIKUS_YELL_4, pEranikus);
                        DoScriptText(REMULOS_SAY_8, me);
                    }

                    for (int i = 0; i < NPC_NIGHTMARE_PHANTASMS_COUNT; i++)
                    {
                        uint32 r = urand(0, 5);
                        float collision = (float)urand(4, 8);
                        me->SummonCreature(NPC_NIGHTMARE_PHANTASM,
                                           PhantasmsSpawnpos[r][0] + collision, PhantasmsSpawnpos[r][1] + collision,
                                           PhantasmsSpawnpos[r][2] + 2, PhantasmsSpawnpos[r][3], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                    }
                    SetCanAttack(true);
                    PhantasmPhase = true;
                    me->SetSpeed(MOVE_RUN, 2.0f);
                    SetRun(true);
                    uiPhase = 12;
                    EventTimer = 5000;
                    break;
                case 12:
                    if (Creature* pEranikus = Unit::GetCreature(*me, EranikusGUID))
                        DoScriptText(ERANIKUS_YELL_5, pEranikus);
                    for (int i = 0; i < NPC_MOONGLADE_WARDENS_COUNT; i++)
                    {
                        float collision = (float)urand(4, 8);
                        uint32 r = urand(0, 1) ? 1 : 0;
                        if (Creature* pWarden = me->SummonCreature(NPC_MOONGLADE_WARDEN,
                                                PhantasmsSpawnpos[r][0] + collision, PhantasmsSpawnpos[r][1] + collision,
                                                PhantasmsSpawnpos[r][2], PhantasmsSpawnpos[r][3], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000))
                            FindVictim(pWarden);
                    }
                    uiPhase = 13;
                    EventTimer = 10000;
                    break;
                case 13:
                    CheckNightmare();
                    EventTimer = 500;
                    break;
                case 14:
                    if (DeadPhantasmsCount >= NPC_NIGHTMARE_PHANTASMS_COUNT)
                    {
                        EranikusPhase = true;
                        PhantasmPhase = false;
                        SetCanAttack(false);
                        SetEscortPaused(false);
                        SetRun(false);
                        uiPhase = 15;
                        if (Creature* pEranikus = Unit::GetCreature(*me, EranikusGUID))
                        {
                            DoScriptText(ERANIKUS_YELL_6, pEranikus);
                            pEranikus->GetMotionMaster()->MovePoint(POINT_COMBAT_START, 7901.51f, -2565.71f, 488.046f);
                            pEranikus->SetHomePosition(7901.51f, -2565.71f, 488.046f, 3.17f);
                        }
                    }
                    EventTimer = 10000;
                    break;
                case 15:
                    if (Creature* pEranikus = Unit::GetCreature(*me, EranikusGUID))
                    {
                        for (int i = 0; i < NPC_MOONGLADE_WARDENS_COUNT; i++)
                            me->SummonCreature(NPC_MOONGLADE_WARDEN,
                                               pEranikus->GetPositionX(), pEranikus->GetPositionY(),
                                               pEranikus->GetPositionZ(), 0.02, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                    }
                    EventTimer = 60000;
                    break;
                }
            }
            else EventTimer -= diff;
        }

        if (GetTargetForHeal())
        {
            if (RegrowthTimer <= diff)
            {
                DoCast(GetTargetForHeal(), SPELL_REGROWTH);
                RegrowthTimer = urand(5000, 10000);
            }
            else RegrowthTimer -= diff;

            if (RejuvenationTimer <= diff)
            {
                DoCast(GetTargetForHeal(), SPELL_REJUVENATION);
                RejuvenationTimer = urand(5000, 10000);
            }
            else RejuvenationTimer -= diff;

            if (TranquilityTimer <= diff)
            {
                DoCast(GetTargetForHeal(), SPELL_TRANQUILITY);
                TranquilityTimer = urand(20000, 30000);
            }
            else TranquilityTimer -= diff;

            if (HealingTouchTimer <= diff)
            {
                DoCast(GetTargetForHeal(), SPELL_HEALING_TOUCH);
                HealingTouchTimer = 3500;
            }
            else HealingTouchTimer -= diff;
        }

        if (!UpdateVictim())
        {
            if (FindVictimTimer <= diff)
            {
                FindVictim(me);
                FindVictimTimer = 2000;
                return;
            }
            else FindVictimTimer -= diff;
            return;
        }

        if (StarfireTimer <= diff)
        {
            DoCastVictim( SPELL_STARFIRE);
            StarfireTimer = urand(7000, 10000);
        }
        else StarfireTimer -= diff;

        DoMeleeAttackIfReady();
    }

    Unit* GetTargetForHeal()
    {
        if (HealthBelowPct(90))
            return me;

        if (PhantasmPhase)
        {
            if (Unit* pTarget = GetPlayerForEscort())
                if (pTarget->GetHealth() * 100 < pTarget->GetMaxHealth() * 90)
                    return pTarget;
            return NULL;
        }

        if (EranikusPhase)
        {
            if (Creature* pEranikus = Unit::GetCreature(*me, EranikusGUID))
            {
                if (Unit* pTarget = pEranikus->getVictim())
                    if (pTarget->GetHealth() * 100 < pTarget->GetMaxHealth() * 90)
                        return pTarget;
                return NULL;
            }
        }
        return NULL;
    }

    void EventComplete()
    {
        if (Player* pPlayer = GetPlayerForEscort())
            pPlayer->GroupEventHappens(QUEST_NIGHTMARE_MANIFESTS, me);
    }
};

CreatureAI* GetAI_npc_remulos(Creature* pCreature)
{
    return new npc_remulosAI(pCreature);
}

bool QuestAccept_npc_remulos(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_NIGHTMARE_MANIFESTS)
    {
        DoScriptText(REMULOS_SAY_1, pCreature, pPlayer);

        if (npc_remulosAI* pEscortAI = CAST_AI(npc_remulosAI, pCreature->AI()))
        {
            pEscortAI->Start(true, false, pPlayer->GetGUID(), pQuest);
            pEscortAI->SetEscortPaused(true);
            pEscortAI->SetDespawnAtEnd(false);
            pEscortAI->SetDespawnAtFar(false);
            pEscortAI->uiPhase = 1;
            pEscortAI->EventTimer = 10000;
        }
    }
    return true;
}

struct npc_eranikusAI : public ScriptedAI
{
    public:
        npc_eranikusAI(Creature* c) : ScriptedAI(c) {}

        uint32 ShadowBoltTimer;
        uint32 AcidBreathTimer;
        uint32 NoxiusBreathTimer;
        uint32 EventTimer;
        uint32 FindVictimTimer;
        uint32 uiPhase;
        uint64 TyrandeGUID;
        uint64 RemulosGUID;
        bool CanAttack;

        std::list<uint64> PriestessOfTheMoonList;

        void Reset()
        {
            ShadowBoltTimer = urand(5000, 12000);
            AcidBreathTimer = urand(5000, 15000);
            NoxiusBreathTimer = urand(10000, 20000);
            EventTimer = 0;
            FindVictimTimer = 2000;
            uiPhase = 0;
            TyrandeGUID = 0;
            RemulosGUID = 0;
            CanAttack = false;

            PriestessOfTheMoonList.clear();
        }

        void UpdateAI(const uint32 diff)
        {
            if (EventTimer <= diff)
            {
                switch (uiPhase)
                {
                case 0:
                    if (HealthBelowPct(80))
                    {
                        DoScriptText(ERANIKUS_YELL_7, me);
                        if (Creature* pTyrande = me->SummonCreature(NPC_TYRANDE, 7927.01f, -2573.36f, 489.652f, 2.81f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
                        {
                            TyrandeGUID = pTyrande->GetGUID();
                            pTyrande->SetVisibility(VISIBILITY_OFF);
                            DoScriptText(TYRANDE_YELL_1, pTyrande);
                        }
                        uiPhase++;
                    }
                    EventTimer = 3000;
                    break;
                case 1:
                    if (HealthBelowPct(70))
                    {
                        if (Creature* pTyrande = Unit::GetCreature(*me, TyrandeGUID))
                            DoScriptText(TYRANDE_YELL_2, pTyrande);
                        uiPhase++;
                    }
                    EventTimer = 3000;
                    break;
                case 2:
                    if (HealthBelowPct(60))
                    {
                        if (Creature* pTyrande = Unit::GetCreature(*me, TyrandeGUID))
                            DoScriptText(TYRANDE_YELL_3, pTyrande);
                        uiPhase++;
                    }
                    EventTimer = 3000;
                    break;
                case 3:
                    if (HealthBelowPct(40))
                    {
                        if (Creature* pTyrande = Unit::GetCreature(*me, TyrandeGUID))
                            DoScriptText(TYRANDE_SAY_2, pTyrande);
                        uiPhase++;
                    }
                    EventTimer = 3000;
                    break;
                case 4:
                    if (HealthBelowPct(25))
                    {
                        DoScriptText(ERANIKUS_YELL_8, me);
                        uiPhase++;
                    }
                    EventTimer = 3000;
                    break;
                case 5:
                    if (HealthBelowPct(22))
                    {
                        DoScriptText(ERANIKUS_YELL_9, me);
                        uiPhase++;
                    }
                    EventTimer = 3000;
                    break;
                case 6:
                    if (HealthBelowPct(20))
                    {
                        if (Creature* pRemulos = Unit::GetCreature(*me, RemulosGUID))
                        {
                            CAST_AI(npc_remulosAI, pRemulos->AI())->EranikusPhase = false;
                            CAST_AI(npc_remulosAI, pRemulos->AI())->uiPhase = 0;
                            CAST_AI(npc_remulosAI, pRemulos->AI())->SetEscortPaused(false);
                        }
                        DoScriptText(ERANIKUS_YELL_10, me);
                        for (std::set<Unit*>::const_iterator itr = me->getAttackers().begin(); itr != me->getAttackers().end(); ++itr)
                        {
                            float collision = (float)urand(-8, 8);
                            if ((*itr))
                                (*itr)->RemoveAllAuras();

                            if ((*itr)->GetEntry() == NPC_MOONGLADE_WARDEN)
                            {
                                if (Creature* pWarden = Unit::GetCreature(*me, (*itr)->GetGUID()))
                                {
                                    pWarden->SetHomePosition(7877.72f + collision, -2581.84f + collision, 486.948f, 0.0f + collision);
                                    pWarden->GetMotionMaster()->MoveTargetedHome();
                                    pWarden->SetStandState(UNIT_STAND_STATE_SIT);
                                }
                            }
                        }
                        CanAttack = false;
                        me->CombatStop(true);
                        me->setFaction(35);
                        me->DeleteThreatList();
                        me->RemoveAllAuras();
                        me->SetReactState(REACT_PASSIVE);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        if (Creature* pTyrande = Unit::GetCreature(*me, TyrandeGUID))
                        {
                            pTyrande->SetVisibility(VISIBILITY_ON);
                            pTyrande->SetHomePosition(7886.63f, -2565.8f, 486.965f, 6.16066f);
                            pTyrande->GetMotionMaster()->MoveTargetedHome();
                        }
                        for (int i = 0; i < NPC_PRIESTESS_OF_THE_MOON_COUNT    ; i++)
                        {
                            float collision = (float)urand(-4, 4);
                            if (Creature* pPristess = me->SummonCreature(NPC_PRIESTESS_OF_THE_MOON, 7927.01f + collision, -2573.36f + collision, 489.652f + collision, 2.81f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000))
                            {
                                PriestessOfTheMoonList.push_back(pPristess->GetGUID());
                                pPristess->SetHomePosition(PristessHomePoint[i].m_fX, PristessHomePoint[i].m_fY,
                                                           PristessHomePoint[i].m_fZ, PristessHomePoint[i].m_fO);
                                pPristess->GetMotionMaster()->MoveTargetedHome();
                            }
                        }
                        EventTimer = 3000;
                        uiPhase++;
                    }
                    else
                        EventTimer = 3000;
                    break;
                case 7:
                    me->SetUInt64Value(UNIT_FIELD_TARGET, 0);
                    me->SetStandState(UNIT_STAND_STATE_DEAD);
                    DoTeleportTo(7901.51f, -2565.71f, 488.046f);
                    DoCast(SPELL_SPOTLIGHT);
                    //@todo now must be server-wide emote
                    DoScriptText(ERANIKUS_ZONE_EMOTE_CONSUMED, me);
                    if (Creature* pTyrande = Unit::GetCreature(*me, TyrandeGUID))
                        me->SetFacingToObject(pTyrande);
                    EventTimer = 3000;
                    uiPhase++;
                    break;
                case 8:
                    if (Creature* pTyrande = Unit::GetCreature(*me, TyrandeGUID))
                    {
                        pTyrande->SetFacingToObject(me);
                        DoScriptText(TYRANDE_EMOTE_KNEE, pTyrande);
                        pTyrande->SetStandState(UNIT_STAND_STATE_KNEEL);
                        DoScriptText(TYRANDE_YELL_4, pTyrande);
                    }
                    EventTimer = 3000;
                    uiPhase++;
                    break;
                case 9:
                    if (Creature* pTyrande = Unit::GetCreature(*me, TyrandeGUID))
                    {
                        pTyrande->SetStandState(UNIT_STAND_STATE_STAND);
                        me->SetFacingToObject(pTyrande);
                    }
                    if (Creature* pRemulos = Unit::GetCreature(*me, RemulosGUID))
                        pRemulos->SetFacingToObject(me);
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    me->UpdateEntry(NPC_ERANIKUS_THE_REDEEMED);
                    me->SetDisplayId(6984);
                    DoScriptText(ERANIKUS_SAY_1, me);
                    EventTimer = 8000;
                    uiPhase++;
                    break;
                case 10:
                    DoScriptText(ERANIKUS_SAY_2, me);
                    EventTimer = 5000;
                    uiPhase++;
                    break;
                case 11:
                    DoScriptText(ERANIKUS_SAY_3, me);
                    EventTimer = 10000;
                    uiPhase++;
                    break;
                case 12:
                    DoScriptText(ERANIKUS_SAY_4, me);
                    EventTimer = 8000;
                    uiPhase++;
                    break;
                case 13:
                    if (Creature* pRemulos = Unit::GetCreature(*me, RemulosGUID))
                    {
                        DoScriptText(REMULOS_SAY_9, pRemulos);
                        CAST_AI(npc_remulosAI, pRemulos->AI())->EventComplete();
                    }
                    if (Creature* pTyrande = Unit::GetCreature(*me, TyrandeGUID))
                        pTyrande->ForcedDespawn();
                    if (!PriestessOfTheMoonList.empty())
                    {
                        for (std::list<uint64>::iterator itr = PriestessOfTheMoonList.begin(); itr != PriestessOfTheMoonList.end(); ++itr)
                        {
                            if (Creature* pPristess = Unit::GetCreature(*me, *itr))
                                pPristess->ForcedDespawn();
                        }
                    }
                    PriestessOfTheMoonList.clear();
                    if (Creature* pRemulos = Unit::GetCreature(*me, RemulosGUID))
                    {
                        DoScriptText(REMULOS_SAY_10, pRemulos);
                        CAST_AI(npc_remulosAI, pRemulos->AI())->ClearSummonedCreatures();
                        pRemulos->ForcedDespawn(5000);
                    }
                    Reset();
                    me->ForcedDespawn();
                    break;
                }
            }
            else EventTimer -= diff;

            if (CanAttack)
            {
                if (!UpdateVictim())
                {
                    if (FindVictimTimer <= diff)
                    {
                        FindVictim();
                        FindVictimTimer = 2000;
                        return;
                    }
                    else FindVictimTimer -= diff;
                    return;
                }

                if (ShadowBoltTimer <= diff)
                {
                    DoCastVictim( ERANIKUS_SPELL_SHADOW_BOLT_VOLLEY);
                    ShadowBoltTimer = urand(7000, 12000);
                }
                else ShadowBoltTimer -= diff;

                if (AcidBreathTimer <= diff)
                {
                    DoCastVictim( ERANIKUS_SPELL_ACID_BREATH);
                    AcidBreathTimer = urand(5000, 15000);
                }
                else AcidBreathTimer -= diff;

                if (NoxiusBreathTimer <= diff)
                {
                    DoCastVictim( ERANIKUS_SPELL_NOXIUS_BREATH);
                    NoxiusBreathTimer = urand(10000, 20000);
                }
                else NoxiusBreathTimer -= diff;

                DoMeleeAttackIfReady();
            }
        }

        void MovementInform(uint32 uiMotionType, uint32 uiPointId)
        {
            if (uiMotionType != POINT_MOTION_TYPE)
                return;

            if (uiPointId == POINT_NIGHTRAVEN)
            {
                me->SetOrientation(2.88f);
                me->SendMovementFlagUpdate();
            }

            if (uiPointId == POINT_COMBAT_START)
            {
                CanAttack = true;
                me->SetCanFly(false);
                me->SetReactState(REACT_AGGRESSIVE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            }
        }

        void JustDied()
        {
            if (Creature* pTyrande = Unit::GetCreature(*me, TyrandeGUID))
            {
                if (pTyrande->IsAlive())
                    pTyrande->ForcedDespawn();
            }

            if (!PriestessOfTheMoonList.empty())
            {
                for (std::list<uint64>::iterator itr = PriestessOfTheMoonList.begin(); itr != PriestessOfTheMoonList.end(); ++itr)
                {
                    if (Creature* pPristess = Unit::GetCreature(*me, *itr))
                        pPristess->ForcedDespawn();
                }
            }
            PriestessOfTheMoonList.clear();
        }

        void IsSummonedBy(Unit* pSummoner)
        {
            if (pSummoner->GetEntry() == NPC_REMULOS)
            {
                RemulosGUID = pSummoner->GetGUID();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveCharge(7867.44f, -2671.37f, 503.042f);
                return;
            }
        }

        void FindVictim()
        {
            if (!me->getVictim())
            {
                if (Unit* pTarget = me->SelectNearestTarget(20))
                {
                    me->GetMotionMaster()->MoveChase(pTarget);
                    AttackStart(pTarget);
                    return;
                }

                if (Creature* pRemulos = Unit::GetCreature(*me, RemulosGUID))
                {
                    me->GetMotionMaster()->MoveChase(pRemulos);
                    AttackStart(pRemulos);
                    return;
                }
            }
        }
};

CreatureAI* GetAI_npc_eranikus(Creature* pCreature)
{
    return new npc_eranikusAI(pCreature);
}

void AddSC_moonglade()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_bunthen_plainswind";
    newscript->pGossipHello =  &GossipHello_npc_bunthen_plainswind;
    newscript->pGossipSelect = &GossipSelect_npc_bunthen_plainswind;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_great_bear_spirit";
    newscript->pGossipHello =  &GossipHello_npc_great_bear_spirit;
    newscript->pGossipSelect = &GossipSelect_npc_great_bear_spirit;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_silva_filnaveth";
    newscript->pGossipHello =  &GossipHello_npc_silva_filnaveth;
    newscript->pGossipSelect = &GossipSelect_npc_silva_filnaveth;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_clintar_dreamwalker";
    newscript->pQuestAccept = &QuestAccept_npc_clintar_dreamwalker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_clintar_spirit";
    newscript->GetAI = &GetAI_npc_clintar_spirit;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_remulos";
    newscript->GetAI = &GetAI_npc_remulos;
    newscript->pQuestAccept = &QuestAccept_npc_remulos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_eranikus";
    newscript->GetAI = &GetAI_npc_eranikus;
    newscript->RegisterSelf();
}

