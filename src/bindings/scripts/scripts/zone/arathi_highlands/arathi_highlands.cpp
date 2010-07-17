/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Arathi Highlands
SD%Complete: 100
SDComment: Quest support: 665
SDCategory: Arathi Highlands
EndScriptData */

/* ContentData
npc_professor_phizzlethorpe
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_professor_phizzlethorpe
######*/

enum eEnums
{
    SAY_PROGRESS_1      = -1000235,
    SAY_PROGRESS_2      = -1000236,
    SAY_PROGRESS_3      = -1000237,
    EMOTE_PROGRESS_4    = -1000238,
    SAY_AGGRO           = -1000239,
    SAY_PROGRESS_5      = -1000240,
    SAY_PROGRESS_6      = -1000241,
    SAY_PROGRESS_7      = -1000242,
    EMOTE_PROGRESS_8    = -1000243,
    SAY_PROGRESS_9      = -1000244,

    QUEST_SUNKEN_TREASURE   = 665,
    MOB_VENGEFUL_SURGE      = 2776
};

struct OREGON_DLL_DECL npc_professor_phizzlethorpeAI : public npc_escortAI
{
    npc_professor_phizzlethorpeAI(Creature *c) : npc_escortAI(c) {}

    void WaypointReached(uint32 uiPointId)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch(uiPointId)
        {
        case 4:DoScriptText(SAY_PROGRESS_2, me, pPlayer);break;
        case 5:DoScriptText(SAY_PROGRESS_3, me, pPlayer);break;
        case 8:DoScriptText(EMOTE_PROGRESS_4, me);break;
        case 9:
            {
            me->SummonCreature(MOB_VENGEFUL_SURGE, -2052.96, -2142.49, 20.15, 1.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
            me->SummonCreature(MOB_VENGEFUL_SURGE, -2052.96, -2142.49, 20.15, 1.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
            break;
            }
        case 10:DoScriptText(SAY_PROGRESS_5, me, pPlayer);break;
        case 11:
            DoScriptText(SAY_PROGRESS_6, me, pPlayer);
            SetRun();
            break;
        case 19:DoScriptText(SAY_PROGRESS_7, me, pPlayer); break;
        case 20:
            DoScriptText(EMOTE_PROGRESS_8, me);
            DoScriptText(SAY_PROGRESS_9, me, pPlayer);
            if (pPlayer)
                CAST_PLR(pPlayer)->GroupEventHappens(QUEST_SUNKEN_TREASURE, me);
            break;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(me);
    }

    void EnterCombat(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, me);
    }

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);
    }
};

bool QuestAccept_npc_professor_phizzlethorpe(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_SUNKEN_TREASURE)
    {
        DoScriptText(SAY_PROGRESS_1, pCreature, pPlayer);
        if (npc_escortAI* pEscortAI = CAST_AI(npc_professor_phizzlethorpeAI, (pCreature->AI())))
            pEscortAI->Start(false, false, pPlayer->GetGUID(), pQuest);

        pCreature->setFaction(113);
    }
    return true;
}

CreatureAI* GetAI_npc_professor_phizzlethorpeAI(Creature *_Creature)
{
    npc_professor_phizzlethorpeAI* professor_phizzlethorpeAI = new npc_professor_phizzlethorpeAI(_Creature);

    professor_phizzlethorpeAI->AddWaypoint(0, -2066.45, -2085.96, 9.08);
    professor_phizzlethorpeAI->AddWaypoint(1, -2077.99, -2105.33, 13.24);
    professor_phizzlethorpeAI->AddWaypoint(2, -2074.60, -2109.67, 14.24);
    professor_phizzlethorpeAI->AddWaypoint(3, -2076.60, -2117.46, 16.67);
    professor_phizzlethorpeAI->AddWaypoint(4, -2073.51, -2123.46, 18.42, 2000);
    professor_phizzlethorpeAI->AddWaypoint(5, -2073.51, -2123.46, 18.42, 4000);
    professor_phizzlethorpeAI->AddWaypoint(6, -2066.60, -2131.85, 21.56);
    professor_phizzlethorpeAI->AddWaypoint(7, -2053.85, -2143.19, 20.31);
    professor_phizzlethorpeAI->AddWaypoint(8, -2043.49, -2153.73, 20.20, 12000);
    professor_phizzlethorpeAI->AddWaypoint(9, -2043.49, -2153.73, 20.20, 14000);
    professor_phizzlethorpeAI->AddWaypoint(10, -2043.49, -2153.73, 20.20, 10000);
    professor_phizzlethorpeAI->AddWaypoint(11, -2043.49, -2153.73, 20.20, 2000);
    professor_phizzlethorpeAI->AddWaypoint(12, -2053.85, -2143.19, 20.31);
    professor_phizzlethorpeAI->AddWaypoint(13, -2066.60, -2131.85, 21.56);
    professor_phizzlethorpeAI->AddWaypoint(14, -2073.51, -2123.46, 18.42);
    professor_phizzlethorpeAI->AddWaypoint(15, -2076.60, -2117.46, 16.67);
    professor_phizzlethorpeAI->AddWaypoint(16, -2074.60, -2109.67, 14.24);
    professor_phizzlethorpeAI->AddWaypoint(17, -2077.99, -2105.33, 13.24);
    professor_phizzlethorpeAI->AddWaypoint(18, -2066.45, -2085.96, 9.08);
    professor_phizzlethorpeAI->AddWaypoint(19, -2066.41, -2086.21, 8.97, 6000);
    professor_phizzlethorpeAI->AddWaypoint(20, -2066.41, -2086.21, 8.97, 2000);

    return (CreatureAI*)professor_phizzlethorpeAI;
}

void AddSC_arathi_highlands()
{
    Script * newscript;

    newscript = new Script;
    newscript->Name = "npc_professor_phizzlethorpe";
    newscript->GetAI = &GetAI_npc_professor_phizzlethorpeAI;
    newscript->pQuestAccept = &QuestAccept_npc_professor_phizzlethorpe;
    newscript->RegisterSelf();
}

