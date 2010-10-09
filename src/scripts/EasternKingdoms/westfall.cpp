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
SDName: Westfall
SD%Complete: 90
SDComment: Quest support: 155
SDCategory: Westfall
EndScriptData */

/* ContentData
npc_defias_traitor
EndContentData */

#include "ScriptPCH.h"
#include "ScriptedEscortAI.h"

#define SAY_START                   -1000101
#define SAY_PROGRESS                -1000102
#define SAY_END                     -1000103
#define SAY_AGGRO_1                 -1000104
#define SAY_AGGRO_2                 -1000105

#define QUEST_DEFIAS_BROTHERHOOD    155

struct npc_defias_traitorAI : public npc_escortAI
{
    npc_defias_traitorAI(Creature *c) : npc_escortAI(c) { Reset(); }

    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (i)
        {
            case 35:
                SetRun(false);
                break;
            case 36:
                DoScriptText(SAY_PROGRESS, me, pPlayer);
                break;
            case 44:
                DoScriptText(SAY_END, me, pPlayer);
                {
                    if (pPlayer)
                        pPlayer->GroupEventHappens(QUEST_DEFIAS_BROTHERHOOD,me);
                }
                break;
        }
    }
    void EnterCombat(Unit* who)
    {
        DoScriptText(RAND(SAY_AGGRO_1,SAY_AGGRO_2), me, who);
    }

    void Reset() {}
};

bool QuestAccept_npc_defias_traitor(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_DEFIAS_BROTHERHOOD)
    {
        if (npc_escortAI* pEscortAI = CAST_AI(npc_defias_traitorAI, pCreature->AI()))
            pEscortAI->Start(true, true, pPlayer->GetGUID());

        DoScriptText(SAY_START, pCreature, pPlayer);
    }

    return true;
}

CreatureAI* GetAI_npc_defias_traitor(Creature* pCreature)
{
    return new npc_defias_traitorAI(pCreature);
}

void AddSC_westfall()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_defias_traitor";
    newscript->GetAI = &GetAI_npc_defias_traitor;
    newscript->pQuestAccept = &QuestAccept_npc_defias_traitor;
    newscript->RegisterSelf();
}
