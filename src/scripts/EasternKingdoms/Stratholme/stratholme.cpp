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
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/* ScriptData
SDName: Stratholme
SD%Complete: 100
SDComment: Misc mobs for instance. GO-script to apply aura and start event for quest 8945
SDCategory: Stratholme
EndScriptData */

/* ContentData
go_gauntlet_gate
mob_freed_soul
mob_restless_soul
mobs_spectral_ghostly_citizen
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "stratholme.h"

/*######
## go_gauntlet_gate (this is the _first_ of the gauntlet gates, two exist)
######*/

bool GOHello_go_gauntlet_gate(Player* player, GameObject* pGo)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();

    if (!pInstance)
        return false;

    if (pInstance->GetData(TYPE_BARON_RUN) != NOT_STARTED)
        return false;

    if (Group* pGroup = player->GetGroup())
    {
        for (GroupReference* itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* pGroupie = itr->GetSource();
            if (!pGroupie || !pGroupie->IsInMap(player))
                continue;

            if (pGroupie->GetQuestStatus(QUEST_DEAD_MAN_PLEA) == QUEST_STATUS_INCOMPLETE &&
                !pGroupie->HasAura(SPELL_BARON_ULTIMATUM, 0) &&
                pGroupie->GetMap() == pGo->GetMap())
                pGroupie->CastSpell(pGroupie, SPELL_BARON_ULTIMATUM, true);
        }
    }
    else if (player->GetQuestStatus(QUEST_DEAD_MAN_PLEA) == QUEST_STATUS_INCOMPLETE &&
             !player->HasAura(SPELL_BARON_ULTIMATUM, 0) &&
        player->GetMap() == pGo->GetMap())
        player->CastSpell(player, SPELL_BARON_ULTIMATUM, true);

    pInstance->SetData(TYPE_BARON_RUN, IN_PROGRESS);
    return false;
}

/*######
## mob_freed_soul
######*/

//Possibly more of these quotes around.
#define SAY_ZAPPED0 -1329000
#define SAY_ZAPPED1 -1329001
#define SAY_ZAPPED2 -1329002
#define SAY_ZAPPED3 -1329003

struct mob_freed_soulAI : public ScriptedAI
{
    mob_freed_soulAI(Creature* c) : ScriptedAI(c) {}

    void Reset()
    {
        DoScriptText(RAND(SAY_ZAPPED0, SAY_ZAPPED1, SAY_ZAPPED2, SAY_ZAPPED3), me);
    }

    void EnterCombat(Unit* /*who*/) {}
};

CreatureAI* GetAI_mob_freed_soul(Creature* pCreature)
{
    return new mob_freed_soulAI (pCreature);
}

/*######
## mob_restless_soul
######*/

#define SPELL_EGAN_BLASTER  17368
#define SPELL_SOUL_FREED    17370
#define QUEST_RESTLESS_SOUL 5282
#define ENTRY_RESTLESS      11122
#define ENTRY_FREED         11136

struct mob_restless_soulAI : public ScriptedAI
{
    mob_restless_soulAI(Creature* c) : ScriptedAI(c) {}

    uint64 Tagger;
    uint32 Die_Timer;
    bool Tagged;

    void Reset()
    {
        Tagger = 0;
        Die_Timer = 5000;
        Tagged = false;
    }

    void EnterCombat(Unit* /*who*/) {}

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (caster->GetTypeId() == TYPEID_PLAYER)
        {
            if (!Tagged && spell->Id == SPELL_EGAN_BLASTER && CAST_PLR(caster)->GetQuestStatus(QUEST_RESTLESS_SOUL) == QUEST_STATUS_INCOMPLETE)
            {
                Tagged = true;
                Tagger = caster->GetGUID();
            }
        }
    }

    void JustSummoned(Creature* summoned)
    {
        summoned->CastSpell(summoned, SPELL_SOUL_FREED, false);
    }

    void JustDied(Unit* /*Killer*/)
    {
        if (Tagged)
            me->SummonCreature(ENTRY_FREED, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 300000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (Tagged)
        {
            if (Die_Timer <= diff)
            {
                if (Unit* pTemp = Unit::GetUnit(*me, Tagger))
                {
                    CAST_PLR(pTemp)->KilledMonsterCredit(ENTRY_RESTLESS, me->GetGUID());
                    me->Kill(me);
                }
            }
            else Die_Timer -= diff;
        }
    }
};

CreatureAI* GetAI_mob_restless_soul(Creature* pCreature)
{
    return new mob_restless_soulAI (pCreature);
}

/*######
## mobs_spectral_ghostly_citizen
######*/

enum eGhostlyCitizenSpells
{
    SPELL_HAUNTING_PHANTOM  = 16336,
    SPELL_SLAP              = 6754
};

struct mobs_spectral_ghostly_citizenAI : public ScriptedAI
{
    mobs_spectral_ghostly_citizenAI(Creature* c) : ScriptedAI(c) {}

    uint32 Die_Timer;
    bool Tagged;

    void Reset()
    {
        Die_Timer = 5000;
        Tagged = false;
    }

    void EnterCombat(Unit* /*who*/) {}

    void SpellHit(Unit* /*caster*/, const SpellEntry* spell)
    {
        if (!Tagged && spell->Id == SPELL_EGAN_BLASTER)
            Tagged = true;
    }

    void JustDied(Unit* /*Killer*/)
    {
        if (Tagged)
        {
            for (uint32 i = 1; i <= 4; ++i)
            {
                //100%, 50%, 33%, 25% chance to spawn
                if (urand(1, i) == 1)
                    DoSummon(ENTRY_RESTLESS, me, 20.0f, 600000);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (Tagged)
        {
            if (Die_Timer <= diff)
                me->Kill(me);
            else Die_Timer -= diff;
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

    void ReceiveEmote(Player* pPlayer, uint32 emote)
    {
        switch (emote)
        {
        case TEXT_EMOTE_DANCE:
            EnterEvadeMode();
            break;
        case TEXT_EMOTE_RUDE:
            if (me->IsWithinDistInMap(pPlayer, 5))
                DoCast(pPlayer, SPELL_SLAP, false);
            else
                me->HandleEmoteCommand(EMOTE_ONESHOT_RUDE);
            break;
        case TEXT_EMOTE_WAVE:
            me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
            break;
        case TEXT_EMOTE_BOW:
            me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
            break;
        case TEXT_EMOTE_KISS:
            me->HandleEmoteCommand(EMOTE_ONESHOT_FLEX);
            break;
        }
    }
};

CreatureAI* GetAI_mobs_spectral_ghostly_citizen(Creature* pCreature)
{
    return new mobs_spectral_ghostly_citizenAI (pCreature);
}

void AddSC_stratholme()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "go_gauntlet_gate";
    newscript->pGOHello = &GOHello_go_gauntlet_gate;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_freed_soul";
    newscript->GetAI = &GetAI_mob_freed_soul;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_restless_soul";
    newscript->GetAI = &GetAI_mob_restless_soul;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mobs_spectral_ghostly_citizen";
    newscript->GetAI = &GetAI_mobs_spectral_ghostly_citizen;
    newscript->RegisterSelf();
}

