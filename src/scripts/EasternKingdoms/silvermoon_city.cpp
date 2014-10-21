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
SDName: Silvermoon_City
SD%Complete: 100
SDComment: Quest support: 9685
SDCategory: Silvermoon City
EndScriptData */

/* ContentData
npc_blood_knight_stillblade
EndContentData */

#include "ScriptPCH.h"

/*#######
# npc_blood_knight_stillblade
#######*/

enum
{
    SAY_HEAL                        = -1000193,

    QUEST_REDEEMING_THE_DEAD        = 9685,
    SPELL_SHIMMERING_VESSEL         = 31225,
    SPELL_REVIVE_SELF               = 32343
};

struct npc_blood_knight_stillbladeAI : public ScriptedAI
{
    npc_blood_knight_stillbladeAI(Creature* c) : ScriptedAI(c) {}

    uint32 lifeTimer;
    bool spellHit;

    void Reset()
    {
        lifeTimer = 120000;
        me->SetStandState(UNIT_STAND_STATE_DEAD);
        me->SetUInt32Value(UNIT_FIELD_BYTES_1,7);   // lay down
        spellHit = false;
    }

    void EnterCombat(Unit* /*who*/)
    {
    }

    void MoveInLineOfSight(Unit* /*who*/)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (me->IsStandState())
        {
            if (lifeTimer <= diff)
                me->AI()->EnterEvadeMode();
            else
                lifeTimer -= diff;
        }
    }

    void SpellHit(Unit* Hitter, const SpellEntry *Spellkind)
    {
        if ((Spellkind->Id == SPELL_SHIMMERING_VESSEL) && !spellHit &&
            (Hitter->GetTypeId() == TYPEID_PLAYER) && (CAST_PLR(Hitter)->IsActiveQuest(QUEST_REDEEMING_THE_DEAD)))
        {
            CAST_PLR(Hitter)->AreaExploredOrEventHappens(QUEST_REDEEMING_THE_DEAD);
            DoCast(me, SPELL_REVIVE_SELF);
            me->SetStandState(UNIT_STAND_STATE_STAND);
            me->SetUInt32Value(UNIT_DYNAMIC_FLAGS, 0);
            //me->RemoveAllAuras();
            DoScriptText(SAY_HEAL, me, Hitter);
            spellHit = true;
        }
    }
};

CreatureAI* GetAI_npc_blood_knight_stillblade(Creature* pCreature)
{
    return new npc_blood_knight_stillbladeAI (pCreature);
}

void AddSC_silvermoon_city()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "npc_blood_knight_stillblade";
    newscript->GetAI = &GetAI_npc_blood_knight_stillblade;
    newscript->RegisterSelf();
}
