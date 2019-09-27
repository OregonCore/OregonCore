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
SDName: Isle_of_Queldanas
SD%Complete: 100
SDComment: Quest support: 11524, 11525, 11532, 11533, 11542, 11543, 11541
SDCategory: Isle Of Quel'Danas
EndScriptData */

/* ContentData
npc_converted_sentry
npc_greengill_slave
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

/*######
## npc_converted_sentry
######*/

enum eConv
{
    SAY_CONVERTED_1         = -1000188,
    SAY_CONVERTED_2         = -1000189,

    SPELL_CONVERT_CREDIT    = 45009
};

struct npc_converted_sentryAI : public ScriptedAI
{
    npc_converted_sentryAI(Creature* c) : ScriptedAI(c) {}

    bool Credit;
    uint32 Timer;

    void Reset()
    {
        Credit = false;
        Timer = 2500;
    }

    void MoveInLineOfSight(Unit* /*who*/) {}
    void EnterCombat(Unit* /*who*/) {}

    void UpdateAI(const uint32 diff)
    {
        if (!Credit)
        {
            if (Timer <= diff)
            {
                uint32 i = urand(1, 2);
                if (i == 1)
                    DoScriptText(SAY_CONVERTED_1, me);
                else
                    DoScriptText(SAY_CONVERTED_2, me);

                DoCast(me, SPELL_CONVERT_CREDIT);
                if (me->IsPet())
                    CAST_PET(me)->SetDuration(7500);
                Credit = true;
                me->GetMotionMaster()->MoveConfused();
            }
            else Timer -= diff;
        }
    }
};
CreatureAI* GetAI_npc_converted_sentry(Creature* pCreature)
{
    return new npc_converted_sentryAI (pCreature);
}

/*######
## npc_greengill_slave
######*/

#define ENRAGE  45111
#define ORB     45109
#define QUESTG  11541
#define DM      25060

struct npc_greengill_slaveAI : public ScriptedAI
{
    npc_greengill_slaveAI(Creature* c) : ScriptedAI(c) {}

    uint64 PlayerGUID;

    void EnterCombat(Unit* /*who*/) {}

    void Reset()
    {
        PlayerGUID = 0;
    }

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (!caster)
            return;

        if (caster->GetTypeId() == TYPEID_PLAYER && spell->Id == ORB && !me->HasAura(ENRAGE, 0))
        {
            PlayerGUID = caster->GetGUID();
            if (PlayerGUID)
            {
                Player* plr = Unit::GetPlayer(*me, PlayerGUID);
                if (plr && plr->GetQuestStatus(QUESTG) == QUEST_STATUS_INCOMPLETE)
                    plr->KilledMonsterCredit(25086, me->GetGUID());
            }
            DoCast(me, ENRAGE);
            Unit* Myrmidon = me->FindNearestCreature(DM, 70);
            if (Myrmidon)
            {
                me->AddThreat(Myrmidon, 100000.0f);
                AttackStart(Myrmidon);
            }
        }
    }

    void UpdateAI(const uint32 /*diff*/)
    {
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_greengill_slaveAI(Creature* pCreature)
{
    return new npc_greengill_slaveAI(pCreature);
}

void AddSC_isle_of_queldanas()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_converted_sentry";
    newscript->GetAI = &GetAI_npc_converted_sentry;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_greengill_slave";
    newscript->GetAI = &GetAI_npc_greengill_slaveAI;
    newscript->RegisterSelf();
}
