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
SDName: Terokkar_Forest
SD%Complete: 80
SDComment: Quest support: 9889, 10009, 10873, 10896, 11096, 10052, 10051, 10898, 10446/10447, 10852, 10887, 10922, 11085, Skettis->Ogri'la Flight
SDCategory: Terokkar Forest
EndScriptData */

/* ContentData
mob_unkor_the_ruthless
mob_infested_root_walker
mob_rotting_forest_rager
mob_netherweb_victim
npc_floon
npc_isla_starmane
npc_skyguard_prisoner
EndContentData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"

/*######
## mob_unkor_the_ruthless
######*/

enum eUnkor
{
    SAY_SUBMIT                  = -1000194,

    FACTION_HOSTILE             = 45,
    FACTION_FRIENDLY            = 35,
    QUEST_DONTKILLTHEFATONE     = 9889,

    SPELL_PULVERIZE             = 2676
    //SPELL_QUID9889            = 32174
};

struct mob_unkor_the_ruthlessAI : public ScriptedAI
{
    mob_unkor_the_ruthlessAI(Creature* c) : ScriptedAI(c) {}

    bool CanDoQuest;
    uint32 UnkorUnfriendly_Timer;
    uint32 Pulverize_Timer;

    void Reset()
    {
        CanDoQuest = false;
        UnkorUnfriendly_Timer = 0;
        Pulverize_Timer = 3000;
        me->SetStandState(UNIT_STAND_STATE_STAND);
        me->setFaction(FACTION_HOSTILE);
    }

    void EnterCombat(Unit * /*who*/) {}

    void DoNice()
    {
        DoScriptText(SAY_SUBMIT, me);
        me->setFaction(FACTION_FRIENDLY);
        me->SetStandState(UNIT_STAND_STATE_SIT);
        me->RemoveAllAuras();
        me->DeleteThreatList();
        me->CombatStop();
        UnkorUnfriendly_Timer = 60000;
    }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (done_by->GetTypeId() == TYPEID_PLAYER)
            if ((me->GetHealth()-damage)*100 / me->GetMaxHealth() < 30)
        {
            if (Group* pGroup = CAST_PLR(done_by)->GetGroup())
            {
                for (GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
                {
                    Player *pGroupie = itr->getSource();
                    if (pGroupie &&
                        pGroupie->GetQuestStatus(QUEST_DONTKILLTHEFATONE) == QUEST_STATUS_INCOMPLETE &&
                        pGroupie->GetReqKillOrCastCurrentCount(QUEST_DONTKILLTHEFATONE, 18260) == 10)
                    {
                        pGroupie->AreaExploredOrEventHappens(QUEST_DONTKILLTHEFATONE);
                        if (!CanDoQuest)
                            CanDoQuest = true;
                    }
                }
            } else
            if (CAST_PLR(done_by)->GetQuestStatus(QUEST_DONTKILLTHEFATONE) == QUEST_STATUS_INCOMPLETE &&
                CAST_PLR(done_by)->GetReqKillOrCastCurrentCount(QUEST_DONTKILLTHEFATONE, 18260) == 10)
            {
                CAST_PLR(done_by)->AreaExploredOrEventHappens(QUEST_DONTKILLTHEFATONE);
                CanDoQuest = true;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (CanDoQuest)
        {
            if (!UnkorUnfriendly_Timer)
            {
                //DoCast(me,SPELL_QUID9889);        //not using spell for now
                DoNice();
            }
            else
            {
                if (UnkorUnfriendly_Timer <= diff)
                {
                    EnterEvadeMode();
                    return;
                } else UnkorUnfriendly_Timer -= diff;
            }
        }

        if (!UpdateVictim())
            return;

        if (Pulverize_Timer <= diff)
        {
            DoCast(me,SPELL_PULVERIZE);
            Pulverize_Timer = 9000;
        } else Pulverize_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_unkor_the_ruthless(Creature* pCreature)
{
    return new mob_unkor_the_ruthlessAI (pCreature);
}

/*######
## mob_infested_root_walker
######*/

struct mob_infested_root_walkerAI : public ScriptedAI
{
    mob_infested_root_walkerAI(Creature *c) : ScriptedAI(c) {}

    void Reset() { }
    void EnterCombat(Unit* /*who*/) { }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (done_by && done_by->GetTypeId() == TYPEID_PLAYER)
            if (me->GetHealth() <= damage)
                if (rand()%100 < 75)
                    //Summon Wood Mites
                    me->CastSpell(me,39130,true);
    }
};
CreatureAI* GetAI_mob_infested_root_walker(Creature* pCreature)
{
    return new mob_infested_root_walkerAI (pCreature);
}

/*######
## mob_rotting_forest_rager
######*/

struct mob_rotting_forest_ragerAI : public ScriptedAI
{
    mob_rotting_forest_ragerAI(Creature *c) : ScriptedAI(c) {}

    void Reset() { }
    void EnterCombat(Unit* /*who*/) { }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if (done_by->GetTypeId() == TYPEID_PLAYER)
            if (me->GetHealth() <= damage)
                if (rand()%100 < 75)
                    //Summon Lots of Wood Mights
                    me->CastSpell(me,39134,true);
    }
};
CreatureAI* GetAI_mob_rotting_forest_rager(Creature* pCreature)
{
    return new mob_rotting_forest_ragerAI (pCreature);
}

/*######
## mob_netherweb_victim
######*/

#define QUEST_TARGET        22459
//#define SPELL_FREE_WEBBED   38950

const uint32 netherwebVictims[6] =
{
    18470, 16805, 21242, 18452, 22482, 21285
};
struct mob_netherweb_victimAI : public ScriptedAI
{
    mob_netherweb_victimAI(Creature *c) : ScriptedAI(c) {}

    void Reset() { }
    void EnterCombat(Unit* /*who*/) { }
    void MoveInLineOfSight(Unit* /*who*/) { }

    void JustDied(Unit* Killer)
    {
        if (Killer->GetTypeId() == TYPEID_PLAYER)
        {
            if (CAST_PLR(Killer)->GetQuestStatus(10873) == QUEST_STATUS_INCOMPLETE)
            {
                if (rand()%100 < 25)
                {
                    DoSpawnCreature(QUEST_TARGET,0,0,0,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,60000);
                    CAST_PLR(Killer)->KilledMonsterCredit(QUEST_TARGET, 0);
                } else
                DoSpawnCreature(netherwebVictims[rand()%6],0,0,0,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,60000);

                if (rand()%100 < 75)
                    DoSpawnCreature(netherwebVictims[rand()%6],0,0,0,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,60000);
                DoSpawnCreature(netherwebVictims[rand()%6],0,0,0,0,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,60000);
            }
        }
    }
};
CreatureAI* GetAI_mob_netherweb_victim(Creature* pCreature)
{
    return new mob_netherweb_victimAI (pCreature);
}

/*######
## npc_floon
######*/

#define GOSSIP_FLOON1           "You owe Sim'salabim money. Hand them over or die!"
#define GOSSIP_FLOON2           "Hand over the money or die...again!"

enum eFloon
{
    SAY_FLOON_ATTACK        = -1000195,

    FACTION_HOSTILE_FL      = 1738,
    FACTION_FRIENDLY_FL     = 35,

    SPELL_SILENCE           = 6726,
    SPELL_FROSTBOLT         = 9672,
    SPELL_FROST_NOVA        = 11831
};

struct npc_floonAI : public ScriptedAI
{
    npc_floonAI(Creature* c) : ScriptedAI(c) {}

    uint32 Silence_Timer;
    uint32 Frostbolt_Timer;
    uint32 FrostNova_Timer;

    void Reset()
    {
        Silence_Timer = 2000;
        Frostbolt_Timer = 4000;
        FrostNova_Timer = 9000;
        me->setFaction(FACTION_FRIENDLY_FL);
    }

    void EnterCombat(Unit* /*who*/) {}

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Silence_Timer <= diff)
        {
            DoCastVictim(SPELL_SILENCE);
            Silence_Timer = 30000;
        } else Silence_Timer -= diff;

        if (FrostNova_Timer <= diff)
        {
            DoCast(me,SPELL_FROST_NOVA);
            FrostNova_Timer = 20000;
        } else FrostNova_Timer -= diff;

        if (Frostbolt_Timer <= diff)
        {
            DoCastVictim(SPELL_FROSTBOLT);
            Frostbolt_Timer = 5000;
        } else Frostbolt_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_npc_floon(Creature* pCreature)
{
    return new npc_floonAI (pCreature);
}

bool GossipHello_npc_floon(Player *player, Creature* pCreature)
{
    if (player->GetQuestStatus(10009) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(1, GOSSIP_FLOON1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

    player->SEND_GOSSIP_MENU(9442, pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_floon(Player *player, Creature* pCreature, uint32 /*sender*/, uint32 action)
{
    if (action == GOSSIP_ACTION_INFO_DEF)
    {
        player->ADD_GOSSIP_ITEM(1, GOSSIP_FLOON2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        player->SEND_GOSSIP_MENU(9443, pCreature->GetGUID());
    }
    if (action == GOSSIP_ACTION_INFO_DEF+1)
    {
        player->CLOSE_GOSSIP_MENU();
        pCreature->setFaction(FACTION_HOSTILE_FL);
        DoScriptText(SAY_FLOON_ATTACK, pCreature, player);
        ((npc_floonAI*)pCreature->AI())->AttackStart(player);
    }
    return true;
}

/*######
## npc_isla_starmane
######*/

enum eIsla
{
    SAY_PROGRESS_1  = -1000353,
    SAY_PROGRESS_2  = -1000354,
    SAY_PROGRESS_3  = -1000355,
    SAY_PROGRESS_4  = -1000356,

    QUEST_EFTW_H    = 10052,
    QUEST_EFTW_A    = 10051,
    GO_CAGE         = 182794,
    SPELL_CAT       = 32447
};

struct npc_isla_starmaneAI : public npc_escortAI
{
    npc_isla_starmaneAI(Creature* c) : npc_escortAI(c) {}

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch(i)
        {
        case 0:
            {
            GameObject* Cage = me->FindNearestGameObject(GO_CAGE, 10);
            if (Cage)
                Cage->SetGoState(GO_STATE_ACTIVE);
            }
            break;
        case 2: DoScriptText(SAY_PROGRESS_1, me, pPlayer); break;
        case 5: DoScriptText(SAY_PROGRESS_2, me, pPlayer); break;
        case 6: DoScriptText(SAY_PROGRESS_3, me, pPlayer); break;
        case 29:DoScriptText(SAY_PROGRESS_4, me, pPlayer);
            if (pPlayer)
            {
                if (pPlayer->GetTeam() == ALLIANCE)
                    pPlayer->GroupEventHappens(QUEST_EFTW_A, me);
                else if (pPlayer->GetTeam() == HORDE)
                    pPlayer->GroupEventHappens(QUEST_EFTW_H, me);
            }
            me->SetInFront(pPlayer); break;
        case 30: me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE); break;
        case 31: DoCast(me, SPELL_CAT);
            me->RemoveUnitMovementFlag(MOVEFLAG_WALK_MODE); break;
        }
    }

    void Reset()
    {
        me->RestoreFaction();
    }

    void JustDied(Unit* /*killer*/)
    {
        if (Player* pPlayer = GetPlayerForEscort())
        {
            if (pPlayer->GetTeam() == ALLIANCE)
                pPlayer->FailQuest(QUEST_EFTW_A);
            else if (pPlayer->GetTeam() == HORDE)
                pPlayer->FailQuest(QUEST_EFTW_H);
        }
    }
};

bool QuestAccept_npc_isla_starmane(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_EFTW_H || quest->GetQuestId() == QUEST_EFTW_A)
    {
        CAST_AI(npc_escortAI, (pCreature->AI()))->Start(true, false, pPlayer->GetGUID());
        pCreature->setFaction(113);
    }
    return true;
}

CreatureAI* GetAI_npc_isla_starmaneAI(Creature* pCreature)
{
    return  new npc_isla_starmaneAI(pCreature);
}

/*######
## go_skull_pile
######*/
#define GOSSIP_S_DARKSCREECHER_AKKARAI   "Summon Darkscreecher Akkarai"
#define GOSSIP_S_KARROG                  "Summon Karrog"
#define GOSSIP_S_GEZZARAK_THE_HUNTRESS   "Summon Gezzarak the Huntress"
#define GOSSIP_S_VAKKIZ_THE_WINDRAGER    "Summon Vakkiz the Windrager"

bool GossipHello_go_skull_pile(Player *player, GameObject* _GO)
{
    if ((player->GetQuestStatus(11885) == QUEST_STATUS_INCOMPLETE) || player->GetQuestRewardStatus(11885))
    {
        player->ADD_GOSSIP_ITEM(0, GOSSIP_S_DARKSCREECHER_AKKARAI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_S_KARROG, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_S_GEZZARAK_THE_HUNTRESS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->ADD_GOSSIP_ITEM(0, GOSSIP_S_VAKKIZ_THE_WINDRAGER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
    }

    player->SEND_GOSSIP_MENU(_GO->GetGOInfo()->questgiver.gossipID, _GO->GetGUID());
    return true;
}

void SendActionMenu_go_skull_pile(Player *player, GameObject* /*_GO*/, uint32 action)
{
    switch(action)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
              player->CastSpell(player,40642,false);
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
              player->CastSpell(player,40640,false);
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
              player->CastSpell(player,40632,false);
            break;
        case GOSSIP_ACTION_INFO_DEF + 4:
              player->CastSpell(player,40644,false);
            break;
    }
}

bool GossipSelect_go_skull_pile(Player *player, GameObject* _GO, uint32 sender, uint32 action)
{
    switch(sender)
    {
        case GOSSIP_SENDER_MAIN:    SendActionMenu_go_skull_pile(player, _GO, action); break;
    }
    return true;
}

/*######
## npc_skywing
######*/

enum
{
    QUEST_SKYWING         = 10898,

    NPC_LUANGA_IMPRISONER = 18533
};

struct npc_skywingAI : public npc_escortAI
{
    npc_skywingAI(Creature *c) : npc_escortAI(c) {}

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();
        if (!pPlayer)
            return;

        switch(i)
        {
        case 7:
            me->SummonCreature(NPC_LUANGA_IMPRISONER, -3399.274658,4055.948975,18.603474, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
            break;
        case 8:
            pPlayer->AreaExploredOrEventHappens(10898);
            break;
        }
    }

    void EnterCombat(Unit* /*who*/) {}

    void Reset() {}

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(me);
    }

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);
    }
};

CreatureAI* GetAI_npc_skywingAI(Creature* pCreature)
{
    return new npc_skywingAI(pCreature);
}

bool QuestAccept_npc_skywing(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_SKYWING)
    {
        if (npc_skywingAI* pEscortAI = CAST_AI(npc_skywingAI, pCreature->AI()))
            pEscortAI->Start(false, true, pPlayer->GetGUID(), quest);
    }

    return true;
}

/*######
## npc_slim
######*/

enum eSlim
{
    FACTION_CONSORTIUM  = 933
};

bool GossipHello_npc_slim(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isVendor() && pPlayer->GetReputationRank(FACTION_CONSORTIUM) >= REP_FRIENDLY)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
        pPlayer->SEND_GOSSIP_MENU(9896, pCreature->GetGUID());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(9895, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_slim(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetGUID());

    return true;
}

/*########
####npc_akuno
#####*/

enum
{
    SAY_AKU_START           = -1000477,
    SAY_AKU_AMBUSH_A        = -1000478,
    SAY_AKU_AMBUSH_B        = -1000479,
    SAY_AKU_AMBUSH_B_REPLY  = -1000480,
    SAY_AKU_COMPLETE        = -1000481,

    SPELL_CHAIN_LIGHTNING   = 39945,

    QUEST_ESCAPING_TOMB     = 10887,
    NPC_CABAL_SKIRMISHER    = 21661
};

static float m_afAmbushB1[]= {-2895.525879f, 5336.431641f, -11.800f};
static float m_afAmbushB2[]= {-2890.604980f, 5331.938965f, -11.282f};

struct npc_akunoAI : public npc_escortAI
{
    npc_akunoAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    uint32 m_uiChainLightningTimer;

    void Reset()
    {
        m_uiChainLightningTimer = 1000;
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 5:
                DoScriptText(SAY_AKU_AMBUSH_A,me);
               me->SummonCreature(NPC_CABAL_SKIRMISHER, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                break;
            case 14:
                DoScriptText(SAY_AKU_AMBUSH_B,me);

                if (Creature* pTemp =me->SummonCreature(NPC_CABAL_SKIRMISHER, m_afAmbushB1[0], m_afAmbushB1[1], m_afAmbushB1[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000))
                    DoScriptText(SAY_AKU_AMBUSH_B_REPLY, pTemp);

               me->SummonCreature(NPC_CABAL_SKIRMISHER, m_afAmbushB2[0], m_afAmbushB2[1], m_afAmbushB2[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                break;
            case 15:
                SetRun();
                break;
            case 18:
                DoScriptText(SAY_AKU_COMPLETE,me);

                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_ESCAPING_TOMB,me);

                break;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(me);
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
            return;

        if (m_uiChainLightningTimer < uiDiff)
        {
            DoCastVictim( SPELL_CHAIN_LIGHTNING);
            m_uiChainLightningTimer = urand(7000, 14000);
        }
        else
            m_uiChainLightningTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_akuno(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPING_TOMB)
    {
        if (npc_akunoAI* pEscortAI = CAST_AI(npc_akunoAI, pCreature->AI()))
        {
            pCreature->SetStandState(UNIT_STAND_STATE_STAND);
            pCreature->setFaction(FACTION_ESCORT_N_NEUTRAL_ACTIVE);

            DoScriptText(SAY_AKU_START, pCreature);
            pEscortAI->Start(false, false, pPlayer->GetGUID(), pQuest);
        }
    }
    return true;
}

CreatureAI* GetAI_npc_akuno(Creature* pCreature)
{
    return new npc_akunoAI(pCreature);
}

/*######
## npc_skyguard_handler_deesak
######*/

#define GOSSIP_SKYGUARD "Fly me to Ogri'la please"

bool GossipHello_npc_skyguard_handler_deesak(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetReputationRank(1031) >= REP_HONORED)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SKYGUARD, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_skyguard_handler_deesak(Player* pPlayer, Creature* /*pCreature*/, uint32 /*uiSender*/, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->CastSpell(pPlayer,41279,true);               //TaxiPath 705 (Taxi - Skettis to Skyguard Outpost)
    }
    return true;
}

/*######
## npc_letoll
######*/

enum
{
    SAY_LE_START                    = -1000511,
    SAY_LE_KEEP_SAFE                = -1000512,
    SAY_LE_NORTH                    = -1000513,
    SAY_LE_ARRIVE                   = -1000514,
    SAY_LE_BURIED                   = -1000515,
    SAY_LE_ALMOST                   = -1000516,
    SAY_LE_DRUM                     = -1000517,
    SAY_LE_DRUM_REPLY               = -1000518,
    SAY_LE_DISCOVERY                = -1000519,
    SAY_LE_DISCOVERY_REPLY          = -1000520,
    SAY_LE_NO_LEAVE                 = -1000521,
    SAY_LE_NO_LEAVE_REPLY1          = -1000522,
    SAY_LE_NO_LEAVE_REPLY2          = -1000523,
    SAY_LE_NO_LEAVE_REPLY3          = -1000524,
    SAY_LE_NO_LEAVE_REPLY4          = -1000525,
    SAY_LE_SHUT                     = -1000526,
    SAY_LE_REPLY_HEAR               = -1000527,
    SAY_LE_IN_YOUR_FACE             = -1000528,
    SAY_LE_HELP_HIM                 = -1000529,
    EMOTE_LE_PICK_UP                = -1000530,
    SAY_LE_THANKS                   = -1000531,

    QUEST_DIGGING_BONES             = 10922,

    NPC_RESEARCHER                  = 22464,
    NPC_BONE_SIFTER                 = 22466,

    MAX_RESEARCHER                  = 4
};

//Some details still missing from here, and will also have issues if followers evade for any reason.
struct npc_letollAI : public npc_escortAI
{
    npc_letollAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_uiEventTimer = 5000;
        m_uiEventCount = 0;
        Reset();
    }

    std::list<Creature*> m_lResearchersList;

    uint32 m_uiEventTimer;
    uint32 m_uiEventCount;

    void Reset() {}

    //will make them follow, but will only work until they enter combat with any unit
    void SetFormation()
    {
        uint32 uiCount = 0;

        for (std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
        {
            float fAngle = uiCount < MAX_RESEARCHER ? M_PI/MAX_RESEARCHER - (uiCount*2*M_PI/MAX_RESEARCHER) : 0.0f;

            if ((*itr)->isAlive() && !(*itr)->isInCombat())
                (*itr)->GetMotionMaster()->MoveFollow(me, 2.5f, fAngle);

            ++uiCount;
        }
    }

    Creature* GetAvailableResearcher(uint8 uiListNum)
    {
        if (!m_lResearchersList.empty())
        {
            uint8 uiNum = 1;

            for (std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
            {
                if (uiListNum && uiListNum != uiNum)
                {
                    ++uiNum;
                    continue;
                }

                if ((*itr)->isAlive() && (*itr)->IsWithinDistInMap(me, 20.0f))
                    return (*itr);
            }
        }

        return NULL;
    }

    void JustStartedEscort()
    {
        m_uiEventTimer = 5000;
        m_uiEventCount = 0;

        m_lResearchersList.clear();

        me->GetCreatureListWithEntryInGrid(m_lResearchersList, NPC_RESEARCHER, 25.0f);

        if (!m_lResearchersList.empty())
            SetFormation();
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 0:
                JustStartedEscort();
                for (std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
                {
                    (*itr)->SetUnitMovementFlags(MOVEFLAG_WALK_MODE);
                }
                if (Player* pPlayer = GetPlayerForEscort())
                    DoScriptText(SAY_LE_KEEP_SAFE, me, pPlayer);
                break;
            case 1:
                DoScriptText(SAY_LE_NORTH, me);
                break;
            case 10:
                DoScriptText(SAY_LE_ARRIVE, me);
                break;
            case 12:
                DoScriptText(SAY_LE_BURIED, me);
                SetEscortPaused(true);
                break;
            case 13:
                SetRun();
                for (std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
                {
                    (*itr)->SetUnitMovementFlags(MOVEFLAG_NONE);
                }
                break;
        }
    }

    void Aggro(Unit* pWho)
    {
        if (pWho->isInCombat() && pWho->GetTypeId() == TYPEID_UNIT && pWho->GetEntry() == NPC_BONE_SIFTER)
            DoScriptText(SAY_LE_HELP_HIM, me);
    }

    void JustSummoned(Creature* pSummoned)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (pPlayer && pPlayer->isAlive())
            pSummoned->AI()->AttackStart(pPlayer);
        else
            pSummoned->AI()->AttackStart(me);
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (!UpdateVictim())
        {
            if (HasEscortState(STATE_ESCORT_PAUSED))
            {
                if (m_uiEventTimer < uiDiff)
                {
                    m_uiEventTimer = 7000;

                    switch(m_uiEventCount)
                    {
                        case 0:
                            DoScriptText(SAY_LE_ALMOST, me);
                            break;
                        case 1:
                            DoScriptText(SAY_LE_DRUM, me);
                            break;
                        case 2:
                            if (Creature* pResearcher = GetAvailableResearcher(0))
                                DoScriptText(SAY_LE_DRUM_REPLY, pResearcher);
                            break;
                        case 3:
                            DoScriptText(SAY_LE_DISCOVERY, me);
                            break;
                        case 4:
                            if (Creature* pResearcher = GetAvailableResearcher(0))
                                DoScriptText(SAY_LE_DISCOVERY_REPLY, pResearcher);
                            break;
                        case 5:
                            DoScriptText(SAY_LE_NO_LEAVE, me);
                            break;
                        case 6:
                            if (Creature* pResearcher = GetAvailableResearcher(1))
                                DoScriptText(SAY_LE_NO_LEAVE_REPLY1, pResearcher);
                            break;
                        case 7:
                            if (Creature* pResearcher = GetAvailableResearcher(2))
                                DoScriptText(SAY_LE_NO_LEAVE_REPLY2, pResearcher);
                            break;
                        case 8:
                            if (Creature* pResearcher = GetAvailableResearcher(3))
                                DoScriptText(SAY_LE_NO_LEAVE_REPLY3, pResearcher);
                            break;
                        case 9:
                            if (Creature* pResearcher = GetAvailableResearcher(4))
                                DoScriptText(SAY_LE_NO_LEAVE_REPLY4, pResearcher);
                            break;
                        case 10:
                            DoScriptText(SAY_LE_SHUT, me);
                            break;
                        case 11:
                            if (Creature* pResearcher = GetAvailableResearcher(0))
                                DoScriptText(SAY_LE_REPLY_HEAR, pResearcher);
                            break;
                        case 12:
                            DoScriptText(SAY_LE_IN_YOUR_FACE, me);
                            me->SummonCreature(NPC_BONE_SIFTER, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                            break;
                        case 13:
                            DoScriptText(EMOTE_LE_PICK_UP, me);

                            if (Player* pPlayer = GetPlayerForEscort())
                            {
                                DoScriptText(SAY_LE_THANKS, me, pPlayer);
                                pPlayer->GroupEventHappens(QUEST_DIGGING_BONES, me);
                            }

                            SetEscortPaused(false);
                            break;
                    }

                    ++m_uiEventCount;
                }
                else
                    m_uiEventTimer -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_letoll(Creature* pCreature)
{
    return new npc_letollAI(pCreature);
}

bool QuestAccept_npc_letoll(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_DIGGING_BONES)
    {
        if (npc_letollAI* pEscortAI = dynamic_cast<npc_letollAI*>(pCreature->AI()))
        {
            DoScriptText(SAY_LE_START, pCreature);
            pCreature->setFaction(FACTION_ESCORT_N_NEUTRAL_PASSIVE);

            pEscortAI->Start(false, false, pPlayer->GetGUID(), pQuest, true);
        }
    }

    return true;
}

/*######
## npc_mana_bomb_exp_trigger
######*/

enum
{
    SAY_COUNT_1                 = -1000472,
    SAY_COUNT_2                 = -1000473,
    SAY_COUNT_3                 = -1000474,
    SAY_COUNT_4                 = -1000475,
    SAY_COUNT_5                 = -1000476,

    SPELL_MANA_BOMB_LIGHTNING   = 37843,
    SPELL_MANA_BOMB_EXPL        = 35513,

    NPC_MANA_BOMB_EXPL_TRIGGER  = 20767,
    NPC_MANA_BOMB_KILL_TRIGGER  = 21039
};

struct npc_mana_bomb_exp_triggerAI : public ScriptedAI
{
    npc_mana_bomb_exp_triggerAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    GameObject* pManaBomb;

    bool m_bIsActivated;
    uint32 m_uiEventTimer;
    uint32 m_uiEventCounter;

    void Reset()
    {
        pManaBomb = NULL;
        m_bIsActivated = false;
        m_uiEventTimer = 1000;
        m_uiEventCounter = 0;
    }

    void DoTrigger(Player* pPlayer, GameObject* pGo)
    {
        if (m_bIsActivated)
            return;

        m_bIsActivated = true;

        pPlayer->KilledMonsterCredit(NPC_MANA_BOMB_KILL_TRIGGER,0);

        pManaBomb = pGo;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_bIsActivated)
            return;

        if (m_uiEventTimer < uiDiff)
        {
            m_uiEventTimer = 1000;

            if (m_uiEventCounter < 10)
                me->CastSpell(me, SPELL_MANA_BOMB_LIGHTNING, false);

            switch(m_uiEventCounter)
            {
                case 5:
                    if (pManaBomb)
                        pManaBomb->SetGoState(GO_STATE_ACTIVE);

                    DoScriptText(SAY_COUNT_1, me);
                    break;
                case 6:
                    DoScriptText(SAY_COUNT_2, me);
                    break;
                case 7:
                    DoScriptText(SAY_COUNT_3, me);
                    break;
                case 8:
                    DoScriptText(SAY_COUNT_4, me);
                    break;
                case 9:
                    DoScriptText(SAY_COUNT_5, me);
                    break;
                case 10:
                    me->CastSpell(me, SPELL_MANA_BOMB_EXPL, false);
                    break;
                case 30:
                    if (pManaBomb)
                        pManaBomb->SetGoState(GO_STATE_READY);

                    Reset();
                    break;
            }

            ++m_uiEventCounter;
        }
        else
            m_uiEventTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_mana_bomb_exp_trigger(Creature* pCreature)
{
    return new npc_mana_bomb_exp_triggerAI(pCreature);
}

/*######
## go_mana_bomb
######*/

bool GOHello_go_mana_bomb(Player* pPlayer, GameObject* pGo)
{
    if (Creature* pCreature = GetClosestCreatureWithEntry(pGo, NPC_MANA_BOMB_EXPL_TRIGGER, INTERACTION_DISTANCE))
    {
        if (npc_mana_bomb_exp_triggerAI* pBombAI = dynamic_cast<npc_mana_bomb_exp_triggerAI*>(pCreature->AI()))
            pBombAI->DoTrigger(pPlayer, pGo);
    }

    return true;
}


/*#####
## go_veil_skith_cage & npc_captive_child
#####*/

enum
{
    QUEST_MISSING_FRIENDS     = 10852,
    NPC_CAPTIVE_CHILD         = 22314,
    SAY_THANKS_1              = -1000590,
    SAY_THANKS_2              = -1000591,
    SAY_THANKS_3              = -1000592,
    SAY_THANKS_4              = -1000593
};

bool GOHello_veil_skith_cage(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_MISSING_FRIENDS) == QUEST_STATUS_INCOMPLETE)
    {
        if (Creature* pChild = pGo->FindNearestCreature( NPC_CAPTIVE_CHILD, 5, true))
        {
            pPlayer->KilledMonsterCredit(NPC_CAPTIVE_CHILD, 0);

            switch(urand(0,3))
            {
                case 0: DoScriptText(SAY_THANKS_1, pChild); break;
                case 1: DoScriptText(SAY_THANKS_2, pChild); break;
                case 2: DoScriptText(SAY_THANKS_3, pChild); break;
                case 3: DoScriptText(SAY_THANKS_4, pChild); break;
            }

            pChild->GetMotionMaster()->Clear();
            pChild->GetMotionMaster()->MovePoint(0, -2648.049f, 5274.573f, 1.691529f);
        }
    }
    return false;
};

struct npc_captive_child : public ScriptedAI
{
    npc_captive_child(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset() {}

    void WaypointReached(uint32 /*uiPointId*/)
    {
        // we only have one waypoint
        me->ForcedDespawn();
    }
};

CreatureAI* GetAI_npc_captive_child(Creature* pCreature)
{
    return new npc_captive_child(pCreature);
}

/*######
## npc_skyguard_prisoner
######*/

enum SPrisoner
{
    SAY_START                     = -1000716,
    SAY_DONT_LIKE                 = -1000717,
    SAY_COMPLETE                  = -1000718,

    GO_PRISONER_CAGE              = 185952,

    QUEST_ESCAPE_FROM_SKETTIS     = 11085,
};

struct npc_skyguard_prisonerAI : public npc_escortAI
{
    npc_skyguard_prisonerAI(Creature* pCreature) : npc_escortAI(pCreature) {}

    void Reset() {}

    uint32 CalculateWaypointID()
    {
        //@todo we have 2 ways to calculate wich waypoint we need to use

        /*if (abs(me->GetPositionX() + 4108.25) < abs(me->GetPositionX() + 3718.81) &&
            abs(me->GetPositionX() + 4108.25) < abs(me->GetPositionX() + 3671.51))
            return 1;

        if (abs(me->GetPositionX() + 3718.81) < abs(me->GetPositionX() + 4108.25) &&
            abs(me->GetPositionX() + 3718.81) < abs(me->GetPositionX() + 3671.51))
            return 2;

        return 3;*/

        switch (me->GetGUIDLow())
        {
        case 1189307:
            return 1;
            break;
        case 1189309:
            return 2;
            break;
        default:
            return 3;
            break;
        }
    }

    void StartEvent(Player* pPlayer, const Quest* pQuest)
    {
        switch (CalculateWaypointID())
        {
        case 1:
            AddWaypoint(0, -4108.25, 3032.18, 344.799, 3000);
            AddWaypoint(1, -4114.41, 3036.73, 344.039);
            AddWaypoint(2, -4126.41, 3026.07, 344.156);
            AddWaypoint(3, -4145.17, 3029.69, 337.423);
            AddWaypoint(4, -4173.69, 3035.72, 343.346);
            AddWaypoint(5, -4173.70, 3047.37, 343.888);
            AddWaypoint(6, -4183.47, 3060.62, 344.157, 3000);
            AddWaypoint(7, -4179.13, 3090.20, 323.971, 30000);
            Start(false, false, pPlayer->GetGUID(), pQuest);
            break;
        case 2:
            AddWaypoint(0, -3718.81, 3787.24, 302.890, 3000);
            AddWaypoint(1, -3714.44, 3780.35, 302.075);
            AddWaypoint(2, -3698.33, 3788.04, 302.171);
            AddWaypoint(3, -3679.36, 3780.25, 295.077);
            AddWaypoint(4, -3654.82, 3770.43, 301.291);
            AddWaypoint(5, -3656.07, 3757.31, 301.985);
            AddWaypoint(6, -3648.83, 3743.07, 302.173, 3000);
            AddWaypoint(7, -3659.16, 3714.94, 281.576, 30000);
            Start(false, false, pPlayer->GetGUID(), pQuest);
            break;
        case 3:
            AddWaypoint(0, -3671.51, 3385.36, 312.956, 3000);
            AddWaypoint(1, -3677.74, 3379.05, 312.136);
            AddWaypoint(2, -3667.52, 3366.45, 312.233);
            AddWaypoint(3, -3672.87, 3343.52, 304.994);
            AddWaypoint(4, -3679.35, 3319.01, 311.419);
            AddWaypoint(5, -3692.93, 3318.69, 312.081);
            AddWaypoint(6, -3704.08, 3309.56, 312.233, 3000);
            AddWaypoint(7, -3733.99, 3315.77, 292.093, 30000);
            Start(false, false, pPlayer->GetGUID(), pQuest);
            break;
        }
        return;
    }

    void WaypointReached(uint32 uiPointId)
    {
        if (/*Player* pPlayer = */GetPlayerForEscort())
        {
            switch (uiPointId)
            {
            case 0:
                DoScriptText(SAY_START, me);
                break;
            case 6:
                DoScriptText(SAY_DONT_LIKE, me);
                break;
            case 7:
                DoScriptText(SAY_COMPLETE, me);
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->GroupEventHappens(QUEST_ESCAPE_FROM_SKETTIS, me);
                break;
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        npc_escortAI::UpdateAI(uiDiff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_npc_skyguard_prisoner(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ESCAPE_FROM_SKETTIS)
    {
        if (GameObject* pGo = pCreature->FindNearestGameObject(GO_PRISONER_CAGE, 10.0f))
            pGo->UseDoorOrButton();

        if (npc_skyguard_prisonerAI* pEscortAI = CAST_AI(npc_skyguard_prisonerAI, pCreature->AI()))
            pEscortAI->StartEvent(pPlayer, pQuest);
    }
    return true;
}

CreatureAI* GetAI_npc_skyguard_prisoner(Creature* pCreature)
{
    return new npc_skyguard_prisonerAI(pCreature);
}

void AddSC_terokkar_forest()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "mob_unkor_the_ruthless";
    newscript->GetAI = &GetAI_mob_unkor_the_ruthless;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_infested_root_walker";
    newscript->GetAI = &GetAI_mob_infested_root_walker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_rotting_forest_rager";
    newscript->GetAI = &GetAI_mob_rotting_forest_rager;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_netherweb_victim";
    newscript->GetAI = &GetAI_mob_netherweb_victim;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_floon";
    newscript->pGossipHello =  &GossipHello_npc_floon;
    newscript->pGossipSelect = &GossipSelect_npc_floon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_isla_starmane";
    newscript->GetAI = &GetAI_npc_isla_starmaneAI;
    newscript->pQuestAccept = &QuestAccept_npc_isla_starmane;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_skull_pile";
    newscript->pGOHello  = &GossipHello_go_skull_pile;
    newscript->pGOSelect = &GossipSelect_go_skull_pile;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_skywing";
    newscript->GetAI = &GetAI_npc_skywingAI;
    newscript->pQuestAccept = &QuestAccept_npc_skywing;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_slim";
    newscript->pGossipHello =  &GossipHello_npc_slim;
    newscript->pGossipSelect = &GossipSelect_npc_slim;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_akuno";
    newscript->GetAI = &GetAI_npc_akuno;
    newscript->pQuestAccept = &QuestAccept_npc_akuno;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_letoll";
    newscript->GetAI = &GetAI_npc_letoll;
    newscript->pQuestAccept = &QuestAccept_npc_letoll;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_mana_bomb_exp_trigger";
    newscript->GetAI = &GetAI_npc_mana_bomb_exp_trigger;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_mana_bomb";
    newscript->pGOHello = &GOHello_go_mana_bomb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_skyguard_handler_deesak";
    newscript->pGossipHello =  &GossipHello_npc_skyguard_handler_deesak;
    newscript->pGossipSelect = &GossipSelect_npc_skyguard_handler_deesak;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_veil_skith_cage";
    newscript->pGOHello =  &GOHello_veil_skith_cage;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_captive_child";
    newscript->GetAI = &GetAI_npc_captive_child;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_skyguard_prisoner";
    newscript->GetAI = &GetAI_npc_skyguard_prisoner;
    newscript->pQuestAccept = &QuestAccept_npc_skyguard_prisoner;
    newscript->RegisterSelf();
}

