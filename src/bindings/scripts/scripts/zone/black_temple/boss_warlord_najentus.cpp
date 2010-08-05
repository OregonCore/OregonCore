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
SDName: Boss_Warlord_Najentus
SD%Complete: 95
SDComment:
SDCategory: Black Temple
EndScriptData */

#include "precompiled.h"
#include "def_black_temple.h"

#define SAY_AGGRO                       -1564000
#define SAY_NEEDLE1                     -1564001
#define SAY_NEEDLE2                     -1564002
#define SAY_SLAY1                       -1564003
#define SAY_SLAY2                       -1564004
#define SAY_SPECIAL1                    -1564005
#define SAY_SPECIAL2                    -1564006
#define SAY_ENRAGE1                     -1564007            //is this text actually in use?
#define SAY_ENRAGE2                     -1564008
#define SAY_DEATH                       -1564009

//Spells
#define SPELL_NEEDLE_SPINE              39992
#define SPELL_NEEDLE_SPINE_DMG          39835
#define SPELL_NEEDLE_SPINE_AOE          39968
#define SPELL_TIDAL_BURST               39878
#define SPELL_TIDAL_SHIELD              39872
#define SPELL_IMPALING_SPINE            39837
#define SPELL_CREATE_NAJENTUS_SPINE     39956
#define SPELL_HURL_SPINE                39948
#define SPELL_BERSERK                   45078

#define GOBJECT_SPINE                  185584

struct OREGON_DLL_DECL boss_najentusAI : public ScriptedAI
{
    boss_najentusAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 NeedleSpineTimer;
    uint32 EnrageTimer;
    uint32 SpecialYellTimer;
    uint32 TidalShieldTimer;
    uint32 ImpalingSpineTimer;

    uint64 SpineTargetGUID;

    void Reset()
    {
        EnrageTimer = 480000;
        SpecialYellTimer = 45000 + (rand()%76)*1000;
        TidalShieldTimer = 60000;

        ResetTimer();

        SpineTargetGUID = 0;

        if (pInstance)
            pInstance->SetData(DATA_HIGHWARLORDNAJENTUSEVENT, NOT_STARTED);

        DeleteSpine();
    }

    void KilledUnit(Unit *victim)
    {
        switch(rand()%2)
        {
        case 0: DoScriptText(SAY_SLAY1, me); break;
        case 1: DoScriptText(SAY_SLAY2, me); break;
        }
    }

    void JustDied(Unit *victim)
    {
        if (pInstance)
            pInstance->SetData(DATA_HIGHWARLORDNAJENTUSEVENT, DONE);

        DoScriptText(SAY_DEATH, me);
        DeleteSpine();
    }

    bool TryDoCast(Unit *victim, uint32 spellId, bool triggered = false)
    {
        if (me->IsNonMeleeSpellCasted(false)) return false;

        DoCast(victim,spellId,triggered);
        return true;
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_HURL_SPINE && me->HasAura(SPELL_TIDAL_SHIELD, 0))
        {
            me->RemoveAurasDueToSpell(SPELL_TIDAL_SHIELD);
            me->CastSpell(me, SPELL_TIDAL_BURST, true);
            ResetTimer();
        }
    }

    void SpellHitTarget(Unit *pTarget, const SpellEntry *spell)
    {
        switch(spell->Id)
        {
        case SPELL_NEEDLE_SPINE:
            me->CastSpell(pTarget,SPELL_NEEDLE_SPINE_DMG,true);
            break;
        }
    }

    void EnterCombat(Unit *who)
    {
        if (pInstance)
            pInstance->SetData(DATA_HIGHWARLORDNAJENTUSEVENT, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, me);
        DoZoneInCombat();
    }

    bool RemoveImpalingSpine()
    {
        if (!SpineTargetGUID) return false;
        Unit *pTarget = Unit::GetUnit(*me, SpineTargetGUID);
        if (pTarget && pTarget->HasAura(SPELL_IMPALING_SPINE, 1))
            pTarget->RemoveAurasDueToSpell(SPELL_IMPALING_SPINE);
        SpineTargetGUID=0;
        return true;
    }

    void ResetTimer(uint32 inc = 0)
    {
        NeedleSpineTimer = 10000 + inc;
        ImpalingSpineTimer = 20000 + inc;
    }

    void DeleteSpine()
    {
        InstanceMap::PlayerList const &playerliste = ((InstanceMap*)me->GetMap())->GetPlayers();
        InstanceMap::PlayerList::const_iterator it;

        Map::PlayerList const &PlayerList = ((InstanceMap*)me->GetMap())->GetPlayers();
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            Player* i_pl = i->getSource();
            if (i_pl)
                i_pl->DestroyItemCount(32408, i_pl->GetItemCount(32408), true);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (TidalShieldTimer <= diff)
        {
            if (TryDoCast(me, SPELL_TIDAL_SHIELD, true))
            {
                ResetTimer(45000);
                TidalShieldTimer = 60000;
            }
        } else TidalShieldTimer -= diff;

        if (!me->HasAura(SPELL_BERSERK,0))
        {
            if (EnrageTimer <= diff)
            {
                DoScriptText(SAY_ENRAGE2, me);
                DoCast(me, SPELL_BERSERK, true);
            } else EnrageTimer -= diff;
        }

        if (SpecialYellTimer <= diff)
        {
            switch(rand()%2)
            {
            case 0: DoScriptText(SAY_SPECIAL1, me); break;
            case 1: DoScriptText(SAY_SPECIAL2, me); break;
            }
            SpecialYellTimer = 25000 + (rand()%76)*1000;
        } else SpecialYellTimer -= diff;

        if (ImpalingSpineTimer <= diff)
        {
            if (!me->IsNonMeleeSpellCasted(false))
            {
                Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM, 1, 80,true);
                if (!pTarget) pTarget = me->getVictim();
                if (pTarget)
                {
                    DoCast(pTarget, SPELL_IMPALING_SPINE);
                    SpineTargetGUID = pTarget->GetGUID();
                    //must let target summon, otherwise you cannot click the spine
                    pTarget->SummonGameObject(GOBJECT_SPINE, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), me->GetOrientation(), 0, 0, 0, 0, 0);

                    switch(rand()%2)
                    {
                    case 0: DoScriptText(SAY_NEEDLE1, me); break;
                    case 1: DoScriptText(SAY_NEEDLE2, me); break;
                    }
                    ImpalingSpineTimer = 21000;
                }
            }
        } else ImpalingSpineTimer -= diff;

        if (NeedleSpineTimer <= diff)
        {
            if (TryDoCast(me, SPELL_NEEDLE_SPINE, true))
            {
                //std::list<Unit*> target;
                //SelectUnitList(target, 3, SELECT_TARGET_RANDOM, 100, true);
                //for (std::list<Unit*>::iterator i = target.begin(); i != target.end(); ++i)
                //    me->CastSpell(*i, 39835, true);
                NeedleSpineTimer = 3000;
            }
        } else NeedleSpineTimer -= diff;


        DoMeleeAttackIfReady();
    }
};

bool GOHello_go_najentus_spine(Player *player, GameObject* _GO)
{
    if (ScriptedInstance* pInstance = _GO->GetInstanceData())
        if (Creature* Najentus = Unit::GetCreature(*_GO, pInstance->GetData64(DATA_HIGHWARLORDNAJENTUS)))
            if (((boss_najentusAI*)Najentus->AI())->RemoveImpalingSpine())
            {
                player->CastSpell(player, SPELL_CREATE_NAJENTUS_SPINE, true);
                //_GO->SetLootState(GO_NOT_READY);
                //_GO->SetRespawnTime(0);
                _GO->AddObjectToRemoveList();
            }
    return true;
}

CreatureAI* GetAI_boss_najentus(Creature* pCreature)
{
    return new boss_najentusAI (pCreature);
}

void AddSC_boss_najentus()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_najentus";
    newscript->GetAI = &GetAI_boss_najentus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_najentus_spine";
    newscript->pGOHello = &GOHello_go_najentus_spine;
    newscript->RegisterSelf();
}

