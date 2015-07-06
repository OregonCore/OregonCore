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
Name: Boss_Vazruden_the_Herald
%Complete: 99
Comment:
Category: Hellfire Citadel, Hellfire Ramparts
EndScriptData */

#include "ScriptPCH.h"
#include "hellfire_ramparts.h"

#define SPELL_FIREBALL              (HeroicMode?36920:34653)
#define SPELL_CONE_OF_FIRE          (HeroicMode?36921:30926)
#define SPELL_SUMMON_LIQUID_FIRE    (HeroicMode?30928:23971)
#define SPELL_BELLOWING_ROAR        39427
#define SPELL_REVENGE               (HeroicMode?40392:19130)
#define SPELL_KIDNEY_SHOT           30621
#define SPELL_FIRE_NOVA_VISUAL      19823

#define ENTRY_HELLFIRE_SENTRY       17517
#define ENTRY_VAZRUDEN_HERALD       17307
#define ENTRY_VAZRUDEN              17537
#define ENTRY_NAZAN                 17536
#define ENTRY_LIQUID_FIRE           22515
#define ENTRY_REINFORCED_FEL_IRON_CHEST (HeroicMode?185169:185168)

#define SAY_INTRO               -1543017
#define SAY_WIPE                -1543018
#define SAY_AGGRO_1             -1543019
#define SAY_AGGRO_2             -1543020
#define SAY_AGGRO_3             -1543021
#define SAY_KILL_1              -1543022
#define SAY_KILL_2              -1543023
#define SAY_DIE                 -1543024
#define EMOTE                   -1543025

#define PATH_ENTRY              2081

const float VazrudenMiddle[3] = { -1406.5f, 1746.5f, 81.2f};
const float VazrudenRing[2][3] =
{
    { -1430, 1705, 112},
    { -1377, 1760, 112}
};

struct boss_nazanAI : public ScriptedAI
{
    boss_nazanAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();
        VazrudenGUID = 0;
        flight = true;
    }

    ScriptedInstance* pInstance;

    uint32 Fireball_Timer;
    uint32 ConeOfFire_Timer;
    uint32 BellowingRoar_Timer;
    uint32 Fly_Timer;
    uint32 Turn_Timer;
    uint32 UnsummonCheck;
    bool flight;
    uint64 VazrudenGUID;
    bool HeroicMode;
    SpellEntry* liquid_fire;

    void Reset()
    {
        Fireball_Timer = 4000;
        Fly_Timer = 45000;
        Turn_Timer = 0;
        UnsummonCheck = 5000;
    }

    void EnterCombat(Unit* /*who*/) {}

    void JustSummoned(Creature* summoned)
    {
        if (summoned && summoned->GetEntry() == ENTRY_LIQUID_FIRE)
        {
            summoned->SetLevel(me->getLevel());
            summoned->setFaction(me->getFaction());
            summoned->CastSpell(summoned, SPELL_SUMMON_LIQUID_FIRE, true);
            summoned->CastSpell(summoned, SPELL_FIRE_NOVA_VISUAL, true);
        }
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* entry)
    {
        if (pTarget && entry->Id == SPELL_FIREBALL)
            me->SummonCreature(ENTRY_LIQUID_FIRE, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), pTarget->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 30000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
        {
            if (UnsummonCheck <= diff && me->IsAlive())
            {
                me->SetLootRecipient(NULL);
                me->SetVisibility(VISIBILITY_OFF);
                me->DealDamage(me, me->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                me->RemoveCorpse();
            }
            else UnsummonCheck -= diff;
            return;
        }

        if (Fireball_Timer <= diff)
        {
            if (Unit* victim = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(victim, SPELL_FIREBALL, true);
            Fireball_Timer = 4000 + rand() % 3000;
        }
        else Fireball_Timer -= diff;

        if (flight) // phase 1 - the flight
        {
            Creature* Vazruden = Unit::GetCreature(*me, VazrudenGUID);
            if (Fly_Timer <= diff || !(Vazruden && Vazruden->IsAlive() && (Vazruden->GetHealth() * 5 > Vazruden->GetMaxHealth())))
            {
                flight = false;
                BellowingRoar_Timer = 6000;
                ConeOfFire_Timer = 12000;
                me->SetLevitate(false);
                me->SetWalk(true);
                me->GetMotionMaster()->Clear();
                if (Unit* victim = SelectUnit(SELECT_TARGET_NEAREST, 0))
                    me->AI()->AttackStart(victim);
                DoStartMovement(me->getVictim());
                DoScriptText(EMOTE, me);
                return;
            }
            else Fly_Timer -= diff;

            if (Turn_Timer <= diff)
            {
                uint32 waypoint = (Fly_Timer / 10000) % 2;
                if (me->GetDistance(VazrudenRing[waypoint][0], VazrudenRing[waypoint][1], VazrudenRing[waypoint][2]) > 5)
                    me->GetMotionMaster()->MovePoint(0, VazrudenRing[waypoint][0], VazrudenRing[waypoint][1], VazrudenRing[waypoint][2]);
                Turn_Timer = 10000;
            }
            else Turn_Timer -= diff;
        }
        else // phase 2 - land fight
        {
            if (ConeOfFire_Timer <= diff)
            {
                DoCast(me, SPELL_CONE_OF_FIRE);
                ConeOfFire_Timer = 12000;
                Fireball_Timer = 4000;
            }
            else ConeOfFire_Timer -= diff;

            if (HeroicMode && BellowingRoar_Timer <= diff)
            {
                DoCast(me, SPELL_BELLOWING_ROAR);
                BellowingRoar_Timer = 45000;
            }
            else BellowingRoar_Timer -= diff;

            DoMeleeAttackIfReady();
        }
    }
};

struct boss_vazrudenAI : public ScriptedAI
{
    boss_vazrudenAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();
    }

    ScriptedInstance* pInstance;

    uint32 Revenge_Timer;
    bool HeroicMode;
    bool WipeSaid;
    uint32 UnsummonCheck;

    void Reset()
    {
        Revenge_Timer = 4000;
        UnsummonCheck = 2000;
        WipeSaid = false;
    }

    void EnterCombat(Unit* /*who*/)
    {
        switch (rand() % 3)
        {
        case 0:
            DoScriptText(SAY_AGGRO_1, me);
            break;
        case 1:
            DoScriptText(SAY_AGGRO_2, me);
            break;
        default:
            DoScriptText(SAY_AGGRO_3, me);
            break;
        }
    }

    void KilledUnit(Unit* who)
    {
        if (who && who->GetEntry() != ENTRY_VAZRUDEN)
            switch (rand() % 2)
            {
            case 0:
                DoScriptText(SAY_KILL_1, me);
                break;
            default:
                DoScriptText(SAY_KILL_2, me);
                break;
            }
    }

    void JustDied(Unit* who)
    {
        if (who && who != me)
            DoScriptText(SAY_DIE, me);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
        {
            if (UnsummonCheck <= diff && me->IsAlive())
            {
                if (!WipeSaid)
                {
                    DoScriptText(SAY_WIPE, me);
                    WipeSaid = true;
                }
                me->SetLootRecipient(NULL);
                me->SetVisibility(VISIBILITY_OFF);
                me->DealDamage(me, me->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                me->RemoveCorpse();
            }
            else UnsummonCheck -= diff;
            return;
        }

        if (Revenge_Timer <= diff)
        {
            if (Unit* victim = me->getVictim())
                DoCast(victim, SPELL_REVENGE);
            Revenge_Timer = 5000;
        }
        else Revenge_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct boss_vazruden_the_heraldAI : public ScriptedAI
{
    boss_vazruden_the_heraldAI(Creature* c) : ScriptedAI(c)
    {
        summoned = false;
        sentryDown = false;
        NazanGUID = 0;
        VazrudenGUID = 0;
        pInstance = c->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();
    }

    ScriptedInstance* pInstance;

    std::vector<uint64> OrcGUID;
    std::list<Creature*> orcs;

    uint32 phase;
    uint32 waypoint;
    uint32 check;
    bool sentryDown;
    uint64 NazanGUID;
    uint64 VazrudenGUID;
    bool summoned;
    bool HeroicMode;

    void Reset()
    {
        me->GetCreatureListWithEntryInGrid(orcs, 17517, 70.0f);
        OrcGUID.clear();
        for (std::list<Creature*>::iterator itr = orcs.begin(); itr != orcs.end(); itr++)
            OrcGUID.push_back((*itr)->GetGUID());

        phase = 0;
        waypoint = 0;
        check = 0;
        UnsummonAdds();
        me->GetMotionMaster()->MovePath(PATH_ENTRY, true);

        if (pInstance)
            pInstance->SetData(DATA_VAZRUDEN, NOT_STARTED);
    }

    void UnsummonAdds()
    {
        if (summoned)
        {
            Creature* Nazan = Unit::GetCreature(*me, NazanGUID);
            if (!Nazan)
                Nazan = me->FindNearestCreature(ENTRY_NAZAN, 5000);
            if (Nazan)
            {
                Nazan->DisappearAndDie();
                NazanGUID = 0;
            }

            Creature* Vazruden = Unit::GetCreature(*me, VazrudenGUID);
            if (!Vazruden)
                Vazruden = me->FindNearestCreature(ENTRY_VAZRUDEN, 5000);
            if (Vazruden)
            {
                Vazruden->DisappearAndDie();
                VazrudenGUID = 0;
            }
            summoned = false;
            me->ClearUnitState(UNIT_STATE_ROOT);
            me->SetVisibility(VISIBILITY_ON);

            for (std::vector<uint64>::const_iterator itr = OrcGUID.begin(); itr != OrcGUID.end(); ++itr)
            {
                if (Creature* pOrc = Creature::GetCreature(*me, *itr))
                {
                    if (!pOrc->IsAlive())
                    {
                        pOrc->ForcedDespawn();
                        pOrc->Respawn();
                    }
                }
            }
        }
    }

    void SummonAdds()
    {
        if (!summoned)
        {
            Creature* Vazruden = me->SummonCreature(ENTRY_VAZRUDEN, VazrudenMiddle[0], VazrudenMiddle[1], VazrudenMiddle[2], 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 6000000);
            if (Vazruden)
                VazrudenGUID = Vazruden->GetGUID();
            Creature* Nazan = me->SummonCreature(ENTRY_NAZAN, VazrudenMiddle[0], VazrudenMiddle[1], VazrudenMiddle[2], 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 6000000);
            if (Nazan)
                NazanGUID = Nazan->GetGUID();
            summoned = true;
            me->SetVisibility(VISIBILITY_OFF);
            me->AddUnitState(UNIT_STATE_ROOT);
        }
    }

    void EnterCombat(Unit* /*who*/)
    {
        if (phase == 0)
        {
            phase = 1;
            check = 0;
            DoScriptText(SAY_INTRO, me);

            if (pInstance)
                pInstance->SetData(DATA_VAZRUDEN, IN_PROGRESS);
        }
    }

    void JustSummoned(Creature* summoned)
    {
        if (!summoned) return;
        Unit* victim = me->getVictim();
        if (summoned->GetEntry() == ENTRY_NAZAN)
        {
            ((boss_nazanAI*)summoned->AI())->VazrudenGUID = VazrudenGUID;
            summoned->SetLevitate(true);
            summoned->SetSpeed(MOVE_FLIGHT, 2.5f);
            if (victim)
                summoned->AI()->AttackStart(victim);
        }
        else if (victim)
            summoned->AI()->AttackStart(victim);
    }

    void SentryDownBy(Unit* killer)
    {
        if (sentryDown)
        {
            AttackStartNoMove(killer);
            sentryDown = false;
        }
        else
            sentryDown = true;
    }

    void UpdateAI(const uint32 diff)
    {
        switch (phase)
        {
        case 0: // circle around the platform
            return;
            break;
        case 1: // go to the middle and begin the fight
            if (check <= diff)
            {
                if (me->GetDistance(VazrudenMiddle[0], VazrudenMiddle[1], VazrudenMiddle[2]) > 5)
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(0, VazrudenMiddle[0], VazrudenMiddle[1], VazrudenMiddle[2]);
                    check = 1000;
                }
                else
                {
                    SummonAdds();
                    phase = 2;
                    return;
                }
            }
            else check -= diff;
            break;
        default: // adds do the job now
            if (check <= diff)
            {
                Creature* Nazan = Unit::GetCreature(*me, NazanGUID);
                Creature* Vazruden = Unit::GetCreature(*me, VazrudenGUID);
                if ((Nazan && Nazan->IsAlive()) || (Vazruden && Vazruden->IsAlive()))
                {
                    if ((Nazan && Nazan->getVictim()) || (Vazruden && Vazruden->getVictim()))
                        return;
                    else
                    {
                        Reset();
                        EnterEvadeMode();
                        return;
                    }
                }
                else
                {
                    me->SummonGameObject(ENTRY_REINFORCED_FEL_IRON_CHEST, VazrudenMiddle[0], VazrudenMiddle[1], VazrudenMiddle[2], 0, 0, 0, 0, 0, 0);
                    me->SetLootRecipient(NULL);
                    me->DealDamage(me, me->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    if (pInstance)
                        pInstance->SetData(DATA_VAZRUDEN, DONE);
                }
                check = 2000;
            }
            else check -= diff;
            break;
        }
    }
};

struct mob_hellfire_sentryAI : public ScriptedAI
{
    mob_hellfire_sentryAI(Creature* c) : ScriptedAI(c) {}

    uint32 KidneyShot_Timer;

    void Reset()
    {
        KidneyShot_Timer = 3000 + rand() % 4000;
    }

    void EnterCombat(Unit* /*who*/) {}

    void JustDied(Unit* who)
    {
        if (Creature* herald = me->FindNearestCreature(ENTRY_VAZRUDEN_HERALD, 150))
            CAST_AI(boss_vazruden_the_heraldAI, herald->AI())->SentryDownBy(who);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (KidneyShot_Timer <= diff)
        {
            if (Unit* victim = me->getVictim())
                DoCast(victim, SPELL_KIDNEY_SHOT);
            KidneyShot_Timer = 20000;
        }
        else KidneyShot_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_vazruden_the_herald(Creature* pCreature)
{
    return new boss_vazruden_the_heraldAI (pCreature);
}

CreatureAI* GetAI_boss_vazruden(Creature* pCreature)
{
    return new boss_vazrudenAI (pCreature);
}


CreatureAI* GetAI_boss_nazan(Creature* pCreature)
{
    return new boss_nazanAI (pCreature);
}

CreatureAI* GetAI_mob_hellfire_sentry(Creature* pCreature)
{
    return new mob_hellfire_sentryAI (pCreature);
}

void AddSC_boss_vazruden_the_herald()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_vazruden_the_herald";
    newscript->GetAI = &GetAI_boss_vazruden_the_herald;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_vazruden";
    newscript->GetAI = &GetAI_boss_vazruden;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_nazan";
    newscript->GetAI = &GetAI_boss_nazan;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_hellfire_sentry";
    newscript->GetAI = &GetAI_mob_hellfire_sentry;
    newscript->RegisterSelf();
}

