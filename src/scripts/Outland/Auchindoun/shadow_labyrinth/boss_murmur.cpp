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
SDName: Boss_Murmur
SD%Complete: 90
SDComment: Timers may be incorrect
SDCategory: Auchindoun, Shadow Labyrinth
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

#define EMOTE_SONIC_BOOM            -1555036

#define SPELL_SONIC_BOOM_CAST       (HeroicMode?38796:33923)
#define SPELL_SONIC_BOOM_EFFECT     (HeroicMode?38795:33666)
#define SPELL_RESONANCE             33657
#define SPELL_MURMURS_TOUCH         (HeroicMode?38794:33711)
#define SPELL_MAGNETIC_PULL         33689
#define SPELL_SONIC_SHOCK           38797
#define SPELL_THUNDERING_STORM      39365

struct boss_murmurAI : public Scripted_NoMovementAI
{
    boss_murmurAI(Creature* c) : Scripted_NoMovementAI(c)
    {
        HeroicMode = me->GetMap()->IsHeroic();
    }

    uint32 SonicBoom_Timer;
    uint32 MurmursTouch_Timer;
    uint32 Resonance_Timer;
    uint32 MagneticPull_Timer;
    uint32 SonicShock_Timer;
    uint32 ThunderingStorm_Timer;
    bool HeroicMode;
    bool SonicBoom;

    void Reset()
    {
        SonicBoom_Timer = 30000;
        MurmursTouch_Timer = 20000;
        Resonance_Timer = 10000;
        MagneticPull_Timer = 20000;
        ThunderingStorm_Timer = 15000;
        SonicShock_Timer = 10000;
        SonicBoom = false;

        //database should have `RegenHealth`=0 to prevent regen
        uint32 hp = (me->GetMaxHealth() * 40) / 100;
        if (hp) me->SetHealth(hp);
        me->SetPlayerDamaged(false);
    }

    void EnterCombat(Unit*) { }

    // Sonic Boom instant damage (needs core fix instead of this)
    void SpellHitTarget(Unit* pTarget, const SpellEntry* spell)
    {
        if (pTarget && pTarget->IsAlive() && spell && spell->Id == SPELL_SONIC_BOOM_EFFECT)
            me->DealDamage(pTarget, (pTarget->GetHealth() * 90) / 100, NULL, SPELL_DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NATURE, spell);
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target or casting
        if (!UpdateVictim() || me->IsNonMeleeSpellCast(false))
            return;

        // Sonic Boom
        if (SonicBoom)
        {
            DoCast(me, SPELL_SONIC_BOOM_EFFECT, true);
            SonicBoom = false;
            Resonance_Timer = 1500;
        }
        if (SonicBoom_Timer <= diff)
        {
            DoScriptText(EMOTE_SONIC_BOOM, me);
            DoCast(me, SPELL_SONIC_BOOM_CAST);
            SonicBoom_Timer = 30000;
            SonicBoom = true;
            return;
        }
        else SonicBoom_Timer -= diff;

        // Murmur's Touch
        if (MurmursTouch_Timer <= diff)
        {
            if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 80, true))
                DoCast(pTarget, SPELL_MURMURS_TOUCH);
            MurmursTouch_Timer = 30000;
        }
        else MurmursTouch_Timer -= diff;

        // Resonance
        if (Resonance_Timer <= diff)
        {
            if (!me->IsWithinMeleeRange(SelectTarget(SELECT_TARGET_NEAREST, 0, 20, true)))
                DoCast(me, SPELL_RESONANCE);
            Resonance_Timer = 5000;
        }
        else Resonance_Timer -= diff;

        // Magnetic Pull
        if (MagneticPull_Timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                if (pTarget->GetTypeId() == TYPEID_PLAYER && pTarget->IsAlive())
                {
                    DoCast(pTarget, SPELL_MAGNETIC_PULL);
                    MagneticPull_Timer = 20000 + rand() % 15000;
                    return;
                }
            MagneticPull_Timer = 500;
        }
        else MagneticPull_Timer -= diff;

        if (HeroicMode)
        {
            // Thundering Storm
            if (ThunderingStorm_Timer <= diff)
            {
                ThreatContainer::StorageType threatlist = me->getThreatManager().getThreatList();
                for (ThreatContainer::StorageType::const_iterator i = threatlist.begin(); i != threatlist.end(); ++i)
                    if (Unit* pTarget = Unit::GetUnit((*me), (*i)->getUnitGuid()))
                        if (pTarget->IsAlive() && me->GetDistance2d(pTarget) > 35)
                            DoCast(pTarget, SPELL_THUNDERING_STORM, true);
                ThunderingStorm_Timer = 15000;
            }
            else ThunderingStorm_Timer -= diff;

            // Sonic Shock
            if (SonicShock_Timer <= diff)
            {
                if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 20, false))
                    if (pTarget->IsAlive())
                        DoCast(pTarget, SPELL_SONIC_SHOCK);
                SonicShock_Timer = 10000 + rand() % 10000;
            }
            else SonicShock_Timer -= diff;
        }

        // Select nearest most aggro target if top aggro too far
        if (!me->isAttackReady())
            return;
        if (!me->IsWithinMeleeRange(me->GetVictim()))
        {
            ThreatContainer::StorageType threatlist = me->getThreatManager().getThreatList();
            for (ThreatContainer::StorageType::const_iterator i = threatlist.begin(); i != threatlist.end(); ++i)
            if (Unit* pTarget = Unit::GetUnit((*me), (*i)->getUnitGuid()))
            if (pTarget->IsAlive() && me->IsWithinMeleeRange(pTarget))
            {
                me->TauntApply(pTarget);
                break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_murmur(Creature* pCreature)
{
    return new boss_murmurAI (pCreature);
}

void AddSC_boss_murmur()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_murmur";
    newscript->GetAI = &GetAI_boss_murmur;
    newscript->RegisterSelf();
}

