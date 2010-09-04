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
SDName: Silverpine_Forest
SD%Complete: 100
SDComment: Quest support: 1886, 435, 452
SDCategory: Silverpine Forest
EndScriptData */

/* ContentData
npc_astor_hadren
npc_deathstalker_erland
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_astor_hadren
######*/

#define GOSSIP_HAH "You're Astor Hadren, right?"
#define GOSSIP_SAH "You've got something I need, Astor. And I'll be taking it now."
struct OREGON_DLL_DECL npc_astor_hadrenAI : public ScriptedAI
{
    npc_astor_hadrenAI(Creature *c) : ScriptedAI(c) {}

    void Reset()
    {
        me->setFaction(68);
    }

    void EnterCombat(Unit* who)
    {
    }

    void JustDied(Unit *who)
    {
        me->setFaction(68);
    }
};

CreatureAI* GetAI_npc_astor_hadren(Creature* pCreature)
{
    return new npc_astor_hadrenAI(pCreature);
}

bool GossipHello_npc_astor_hadren(Player *player, Creature* pCreature)
{
    if (player->GetQuestStatus(1886) == QUEST_STATUS_INCOMPLETE)
        player->ADD_GOSSIP_ITEM(0, GOSSIP_HAH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    player->SEND_GOSSIP_MENU(623, pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_astor_hadren(Player *player, Creature* pCreature, uint32 sender, uint32 action)
{
    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            player->ADD_GOSSIP_ITEM(0, GOSSIP_SAH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->SEND_GOSSIP_MENU(624, pCreature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            player->CLOSE_GOSSIP_MENU();
            pCreature->setFaction(21);
            if (player)
                ((npc_astor_hadrenAI*)pCreature->AI())->AttackStart(player);
            break;
    }
    return true;
}

/*######
## npc_deathstalker_erland
######*/

enum eErland
{
    SAY_QUESTACCEPT     = -1000335,
    SAY_START           = -1000336,
    SAY_AGGRO_1         = -1000337,
    SAY_AGGRO_2         = -1000338,
    SAY_LAST            = -1000339,

    SAY_THANKS          = -1000340,
    SAY_RANE            = -1000341,
    SAY_ANSWER          = -1000342,
    SAY_MOVE_QUINN      = -1000343,

    SAY_GREETINGS       = -1000344,
    SAY_QUINN           = -1000345,
    SAY_ON_BYE          = -1000346,

    QUEST_ESCORTING     = 435,
    NPC_RANE            = 1950,
    NPC_QUINN           = 1951
};

struct OREGON_DLL_DECL npc_deathstalker_erlandAI : public npc_escortAI
{
    npc_deathstalker_erlandAI(Creature *c) : npc_escortAI(c) {}

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch(i)
        {
        case 1: DoScriptText(SAY_START, me, pPlayer);break;
        case 13:
            DoScriptText(SAY_LAST, me, pPlayer);
            pPlayer->GroupEventHappens(QUEST_ESCORTING, me); break;
        case 14: DoScriptText(SAY_THANKS, me, pPlayer); break;
        case 15: {
                Unit* Rane = FindCreature(NPC_RANE, 20, me);
                if (Rane)
                    DoScriptText(SAY_RANE, Rane);
                break;}
        case 16: DoScriptText(SAY_ANSWER, me); break;
        case 17: DoScriptText(SAY_MOVE_QUINN, me); break;
        case 24: DoScriptText(SAY_GREETINGS, me); break;
        case 25: {
                Unit* Quinn = FindCreature(NPC_QUINN, 20, me);
                if (Quinn)
                    DoScriptText(SAY_QUINN, Quinn);
                break;}
        case 26: DoScriptText(SAY_ON_BYE, me, NULL); break;

        }
    }

    void Reset() {}

    void EnterCombat(Unit* who)
    {
        DoScriptText(RAND(SAY_AGGRO_1,SAY_AGGRO_2), me, who);
    }
};

bool QuestAccept_npc_deathstalker_erland(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_ESCORTING)
    {
        DoScriptText(SAY_QUESTACCEPT, pCreature, pPlayer);

        if (npc_escortAI* pEscortAI = CAST_AI(npc_deathstalker_erlandAI, pCreature->AI()))
            pEscortAI->Start(true, false, pPlayer->GetGUID());
    }

    return true;
}

CreatureAI* GetAI_npc_deathstalker_erlandAI(Creature* pCreature)
{
    npc_deathstalker_erlandAI* deathstalker_erlandAI = new npc_deathstalker_erlandAI(pCreature);

    deathstalker_erlandAI->AddWaypoint(0, 1406.32, 1083.10, 52.55);
    deathstalker_erlandAI->AddWaypoint(1, 1400.49, 1080.42, 52.50); //first say
    deathstalker_erlandAI->AddWaypoint(2, 1388.48, 1083.10, 52.52);
    deathstalker_erlandAI->AddWaypoint(3, 1370.16, 1084.02, 52.30);
    deathstalker_erlandAI->AddWaypoint(4, 1359.02, 1080.85, 52.46);
    deathstalker_erlandAI->AddWaypoint(5, 1341.43, 1087.39, 52.69);
    deathstalker_erlandAI->AddWaypoint(6, 1321.93, 1090.51, 50.66);
    deathstalker_erlandAI->AddWaypoint(7, 1312.98, 1095.91, 47.49);
    deathstalker_erlandAI->AddWaypoint(8, 1301.09, 1102.94, 47.76);
    deathstalker_erlandAI->AddWaypoint(9, 1297.73, 1106.35, 50.18);
    deathstalker_erlandAI->AddWaypoint(10, 1295.49, 1124.32, 50.49);
    deathstalker_erlandAI->AddWaypoint(11, 1294.84, 1137.25, 51.75);
    deathstalker_erlandAI->AddWaypoint(12, 1292.89, 1158.99, 52.65);
    deathstalker_erlandAI->AddWaypoint(13, 1290.75, 1168.67, 52.56, 1000); //complete quest and say last
    deathstalker_erlandAI->AddWaypoint(14, 1287.12, 1203.49, 52.66, 5000);
    deathstalker_erlandAI->AddWaypoint(15, 1287.12, 1203.49, 52.66, 4000);
    deathstalker_erlandAI->AddWaypoint(16, 1287.12, 1203.49, 52.66, 5000);
    deathstalker_erlandAI->AddWaypoint(17, 1287.12, 1203.49, 52.66, 4000);
    deathstalker_erlandAI->AddWaypoint(18, 1290.72, 1207.44, 52.69);
    deathstalker_erlandAI->AddWaypoint(19, 1297.50, 1207.18, 53.74);
    deathstalker_erlandAI->AddWaypoint(20, 1301.32, 1220.90, 53.74);
    deathstalker_erlandAI->AddWaypoint(21, 1298.55, 1220.43, 53.74);
    deathstalker_erlandAI->AddWaypoint(22, 1297.59, 1211.23, 58.47);
    deathstalker_erlandAI->AddWaypoint(23, 1305.01, 1206.10, 58.51);
    deathstalker_erlandAI->AddWaypoint(24, 1310.51, 1207.36, 58.51, 5000);
    deathstalker_erlandAI->AddWaypoint(25, 1310.51, 1207.36, 58.51, 5000);
    deathstalker_erlandAI->AddWaypoint(26, 1310.51, 1207.36, 58.51, 2000);

    return (CreatureAI*)deathstalker_erlandAI;
}

/*######
## pyrewood_ambush
######*/

#define QUEST_PYREWOOD_AMBUSH 452

#define NPCSAY_INIT "Get ready, they'll be arriving any minute..." //no blizzlike
#define NPCSAY_END "Thanks for your help!" //no blizzlike

static float SpawnPoints[3][4] =
{
    //pos_x   pos_y     pos_z    orien
    //door
    {-396.17, 1505.86, 19.77, 0},
    {-396.91, 1505.77, 19.77, 0},
    {-397.94, 1504.74, 19.77, 0},
};

#define WAIT_SECS 6000

struct OREGON_DLL_DECL pyrewood_ambushAI : public ScriptedAI
{

    pyrewood_ambushAI(Creature *c) : ScriptedAI(c), Summons(me)
    {
       QuestInProgress = false;
    }


    uint32 Phase;
    int KillCount;
    uint32 WaitTimer;
    uint64 PlayerGUID;
    SummonList Summons;

    bool QuestInProgress;

    void Reset()
    {
        WaitTimer = WAIT_SECS;

        if (!QuestInProgress) //fix reset values (see UpdateVictim)
        {
            Phase = 0;
            KillCount = 0;
            PlayerGUID = 0;
            Summons.DespawnAll();
        }
    }

    void EnterCombat(Unit *who){}

    void JustSummoned(Creature *pSummoned)
    {
        Summons.Summon(pSummoned);
        ++KillCount;
    }

    void SummonedCreatureDespawn(Creature* pSummoned)
    {
        Summons.Despawn(pSummoned);
        --KillCount;
    }

    void SummonCreatureWithRandomTarget(uint32 creatureId, int position)
    {
        Creature *pSummoned = me->SummonCreature(creatureId, SpawnPoints[position][0], SpawnPoints[position][1], SpawnPoints[position][2], SpawnPoints[position][3], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000);
        if (pSummoned)
        {
            Player* pPlayer = NULL;
            Unit* pTarget = NULL;
            if (PlayerGUID)
            {
                pPlayer = Unit::GetPlayer(*me, PlayerGUID);
                switch(rand()%2)
                {
                    case 0: pTarget = me; break;
                    case 1: pTarget = pPlayer; break;
                }
            } else
                pTarget = me;

            pSummoned->setFaction(168);
            pSummoned->AddThreat(pTarget, 32.0f);
            pSummoned->AI()->AttackStart(pTarget);
        }
    }

    void JustDied(Unit* pKiller)
    {
        if (PlayerGUID)
        {
            Player* pPlayer = Unit::GetPlayer(*me, PlayerGUID);
            if (pPlayer && pPlayer->GetQuestStatus(QUEST_PYREWOOD_AMBUSH) == QUEST_STATUS_INCOMPLETE)
                pPlayer->FailQuest(QUEST_PYREWOOD_AMBUSH);
        }

    }

    void UpdateAI(const uint32 diff)
    {

        //sLog.outString("DEBUG: p(%i) k(%i) d(%u) W(%i)", Phase, KillCount, diff, WaitTimer);

        if (!QuestInProgress)
            return;

        if (KillCount && (Phase < 6))
        {
            if (!UpdateVictim()) //reset() on target Despawn...
                return;

            DoMeleeAttackIfReady();
            return;
        }


        switch(Phase){
            case 0:
                if (WaitTimer == WAIT_SECS)
                    me->Say(NPCSAY_INIT, LANG_UNIVERSAL, 0); //no blizzlike

                if (WaitTimer <= diff)
                {
                    WaitTimer -= diff;
                    return;
                }
                break;
            case 1:
                SummonCreatureWithRandomTarget(2060, 1);
                break;
            case 2:
                SummonCreatureWithRandomTarget(2061, 2);
                SummonCreatureWithRandomTarget(2062, 0);
                break;
            case 3:
                SummonCreatureWithRandomTarget(2063, 1);
                SummonCreatureWithRandomTarget(2064, 2);
                SummonCreatureWithRandomTarget(2065, 0);
                break;
            case 4:
                SummonCreatureWithRandomTarget(2066, 1);
                SummonCreatureWithRandomTarget(2067, 0);
                SummonCreatureWithRandomTarget(2068, 2);
                break;
            case 5: //end
                if (PlayerGUID)
                {
                    if (Player *pPlayer = Unit::GetPlayer(*me, PlayerGUID))
                    {
                        me->Say(NPCSAY_END, LANG_UNIVERSAL, 0); //no blizzlike
                        pPlayer->GroupEventHappens(QUEST_PYREWOOD_AMBUSH, me);
                    }
                }
                QuestInProgress = false;
                Reset();
                break;
         }

         Phase++; //prepare next phase

    }
};

CreatureAI* GetAI_pyrewood_ambush(Creature *pCreature)
{
    return new pyrewood_ambushAI (pCreature);
}

bool QuestAccept_pyrewood_ambush(Player *pPlayer, Creature *pCreature, const Quest *pQuest)
{
    if ((pQuest->GetQuestId() == QUEST_PYREWOOD_AMBUSH) && (!((pyrewood_ambushAI*)(pCreature->AI()))->QuestInProgress))
    {
        ((pyrewood_ambushAI*)(pCreature->AI()))->QuestInProgress = true;
        ((pyrewood_ambushAI*)(pCreature->AI()))->Phase = 0;
        ((pyrewood_ambushAI*)(pCreature->AI()))->KillCount = 0;
        ((pyrewood_ambushAI*)(pCreature->AI()))->PlayerGUID = pPlayer->GetGUID();
    }

    return true;
}

/*######
## AddSC
######*/

void AddSC_silverpine_forest()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_astor_hadren";
    newscript->pGossipHello =  &GossipHello_npc_astor_hadren;
    newscript->pGossipSelect = &GossipSelect_npc_astor_hadren;
    newscript->GetAI = &GetAI_npc_astor_hadren;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_deathstalker_erland";
    newscript->GetAI = &GetAI_npc_deathstalker_erlandAI;
    newscript->pQuestAccept = &QuestAccept_npc_deathstalker_erland;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "pyrewood_ambush";
    newscript->GetAI = &GetAI_pyrewood_ambush;
    newscript->pQuestAccept = &QuestAccept_pyrewood_ambush;
    newscript->RegisterSelf();
}

