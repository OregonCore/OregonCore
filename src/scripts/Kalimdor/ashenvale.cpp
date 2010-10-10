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
SDName: Ashenvale
SD%Complete: 70
SDComment: Quest support: 6544, 6482
SDCategory: Ashenvale Forest
EndScriptData */

/* ContentData
npc_torek
npc_ruul_snowhoof
EndContentData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"

/*####
# npc_torek
####*/

#define SAY_READY                   -1000106
#define SAY_MOVE                    -1000107
#define SAY_PREPARE                 -1000108
#define SAY_WIN                     -1000109
#define SAY_END                     -1000110

#define SPELL_REND                  11977
#define SPELL_THUNDERCLAP           8078

#define QUEST_TOREK_ASSULT          6544

#define ENTRY_SPLINTERTREE_RAIDER   12859
#define ENTRY_DURIEL                12860
#define ENTRY_SILVERWING_SENTINEL   12896
#define ENTRY_SILVERWING_WARRIOR    12897

struct npc_torekAI : public npc_escortAI
{
    npc_torekAI(Creature *c) : npc_escortAI(c) {}

    uint32 Rend_Timer;
    uint32 Thunderclap_Timer;
    bool Completed;

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {
        case 1:
            DoScriptText(SAY_MOVE, me, pPlayer);
            break;
        case 8:
            DoScriptText(SAY_PREPARE, me, pPlayer);
            break;
        case 19:
            //TODO: verify location and creatures amount.
            me->SummonCreature(ENTRY_DURIEL,1776.73f,-2049.06f,109.83f,1.54f,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,25000);
            me->SummonCreature(ENTRY_SILVERWING_SENTINEL,1774.64f,-2049.41f,109.83f,1.40f,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,25000);
            me->SummonCreature(ENTRY_SILVERWING_WARRIOR,1778.73f,-2049.50f,109.83f,1.67f,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,25000);
            break;
        case 20:
            DoScriptText(SAY_WIN, me, pPlayer);
            Completed = true;
            if (pPlayer)
                pPlayer->GroupEventHappens(QUEST_TOREK_ASSULT, me);
            break;
        case 21:
            DoScriptText(SAY_END, me, pPlayer);
            break;
        }
    }

    void Reset()
    {
        Rend_Timer = 5000;
        Thunderclap_Timer = 8000;
        Completed = false;
    }

    void EnterCombat(Unit* who) {}

    void JustSummoned(Creature* summoned)
    {
        summoned->AI()->AttackStart(me);
    }

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);

        if (!UpdateVictim())
            return;

        if (Rend_Timer <= diff)
        {
            DoCast(me->getVictim(),SPELL_REND);
            Rend_Timer = 20000;
        } else Rend_Timer -= diff;

        if (Thunderclap_Timer <= diff)
        {
            DoCast(me,SPELL_THUNDERCLAP);
            Thunderclap_Timer = 30000;
        } else Thunderclap_Timer -= diff;
    }
};

bool QuestAccept_npc_torek(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_TOREK_ASSULT)
    {
        //TODO: find companions, make them follow Torek, at any time (possibly done by mangos/database in future?)
        DoScriptText(SAY_READY, pCreature, pPlayer);
        pCreature->setFaction(113);

        if (npc_escortAI* pEscortAI = CAST_AI(npc_torekAI, pCreature->AI()))
            pEscortAI->Start(true, true, pPlayer->GetGUID());
    }

    return true;
}

CreatureAI* GetAI_npc_torek(Creature* pCreature)
{
    return new npc_torekAI(pCreature);
}

/*####
# npc_ruul_snowhoof
####*/

#define QUEST_FREEDOM_TO_RUUL    6482
#define GO_CAGE                  178147

struct npc_ruul_snowhoofAI : public npc_escortAI
{
    npc_ruul_snowhoofAI(Creature *c) : npc_escortAI(c) {}

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch(i)
        {
        case 0:    {
                me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                GameObject* Cage = FindGameObject(GO_CAGE, 20, me);
                if (Cage)
                    Cage->SetGoState(GO_STATE_ACTIVE);
                break;}
        case 13:
                me->SummonCreature(3922, 3449.218018f, -587.825073f, 174.978867f, 4.714445f, TEMPSUMMON_DEAD_DESPAWN, 60000);
                me->SummonCreature(3921, 3446.384521f, -587.830872f, 175.186279f, 4.714445f, TEMPSUMMON_DEAD_DESPAWN, 60000);
                me->SummonCreature(3926, 3444.218994f, -587.835327f, 175.380600f, 4.714445f, TEMPSUMMON_DEAD_DESPAWN, 60000);
                break;
        case 19:
                me->SummonCreature(3922, 3508.344482f, -492.024261f, 186.929031f, 4.145029f, TEMPSUMMON_DEAD_DESPAWN, 60000);
                me->SummonCreature(3921, 3506.265625f, -490.531006f, 186.740128f, 4.239277f, TEMPSUMMON_DEAD_DESPAWN, 60000);
                me->SummonCreature(3926, 3503.682373f, -489.393799f, 186.629684f, 4.349232f, TEMPSUMMON_DEAD_DESPAWN, 60000);
                break;

        case 21:{
                if (pPlayer)
                    pPlayer->GroupEventHappens(QUEST_FREEDOM_TO_RUUL, me);

                break;  }
        }
    }

    void EnterCombat(Unit* who) {}

    void Reset()
    {
        GameObject* Cage = FindGameObject(GO_CAGE, 20, me);
        if (Cage)
            Cage->SetGoState(GO_STATE_READY);
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->AI()->AttackStart(me);
    }

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);
    }
};

bool QuestAccept_npc_ruul_snowhoof(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_FREEDOM_TO_RUUL)
    {
        pCreature->setFaction(113);

        if (npc_escortAI* pEscortAI = CAST_AI(npc_ruul_snowhoofAI, (pCreature->AI())))
            pEscortAI->Start(true, false, pPlayer->GetGUID());
    }
    return true;
}

CreatureAI* GetAI_npc_ruul_snowhoofAI(Creature* pCreature)
{
    return new npc_ruul_snowhoofAI(pCreature);
}

void AddSC_ashenvale()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_torek";
    newscript->GetAI = &GetAI_npc_torek;
    newscript->pQuestAccept = &QuestAccept_npc_torek;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_ruul_snowhoof";
    newscript->GetAI = &GetAI_npc_ruul_snowhoofAI;
    newscript->pQuestAccept = &QuestAccept_npc_ruul_snowhoof;
    newscript->RegisterSelf();
}
