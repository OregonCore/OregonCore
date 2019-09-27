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
SDName: Boss_Moroes
SD%Complete: 95
SDComment:
SDCategory: Karazhan
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "karazhan.h"

#define SAY_AGGRO           -1532011
#define SAY_SPECIAL_1       -1532012
#define SAY_SPECIAL_2       -1532013
#define SAY_KILL_1          -1532014
#define SAY_KILL_2          -1532015
#define SAY_KILL_3          -1532016
#define SAY_DEATH           -1532017

#define SPELL_VANISH        29448
#define SPELL_GARROTE       37066
#define SPELL_BLIND         34694
#define SPELL_GOUGE         29425
#define SPELL_FRENZY        37023

#define POS_Z               81.73f

float Locations[4][3] =
{
    { -10987.086f, -1884.002f, 1.352331f},
    { -10989.889f, -1882.592f, 0.932928f},
    { -10979.159f, -1885.326f, 2.017563f},
    { -10976.789f, -1883.946f, 2.328581f}
};

const uint32 Adds[6] =
{
    17007,
    19872,
    19873,
    19874,
    19875,
    19876,
};

struct boss_moroesAI : public ScriptedAI
{
    boss_moroesAI(Creature* c) : ScriptedAI(c)
    {
        for (uint8 i = 0; i < 4; ++i)
            AddId[i] = 0;
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint64 AddGUID[4];

    uint32 Vanish_Timer;
    uint32 Blind_Timer;
    uint32 Gouge_Timer;
    uint32 Wait_Timer;
    uint32 CheckAdds_Timer;
    uint32 AddId[4];

    bool InVanish;
    bool Enrage;

    void Reset()
    {
        Vanish_Timer = 35000;
        Blind_Timer = 40000;
        Gouge_Timer = 35000;
        Wait_Timer = 0;
        CheckAdds_Timer = 5000;

        Enrage = false;
        InVanish = false;
        if (me->GetHealth() > 0)
        {
            DeSpawnAdds();
            SpawnAdds();
        }

        if (pInstance)
            pInstance->SetData(TYPE_MOROES, NOT_STARTED);
    }

    void StartEvent()
    {
        if (pInstance)
            pInstance->SetData(TYPE_MOROES, IN_PROGRESS);

        DoZoneInCombat();
    }

    void EnterCombat(Unit* /*who*/)
    {
        StartEvent();

        DoScriptText(SAY_AGGRO, me);
        AddsAttack();
        DoZoneInCombat();
    }

    void KilledUnit(Unit* /*victim*/)
    {
        DoScriptText(RAND(SAY_KILL_1, SAY_KILL_2, SAY_KILL_3), me);
    }

    void JustDied(Unit* /*victim*/)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance)
            pInstance->SetData(TYPE_MOROES, DONE);

        DeSpawnAdds();

        //remove aura from spell Garrote when Moroes dies
        Map* map = me->GetMap();
        if (map->IsDungeon())
        {
            Map::PlayerList const& PlayerList = map->GetPlayers();

            if (PlayerList.isEmpty())
                return;

            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                if (i->GetSource()->IsAlive() && i->GetSource()->HasAura(SPELL_GARROTE, 0))
                    i->GetSource()->RemoveAurasDueToSpell(SPELL_GARROTE);
            }
        }
    }

    void SpawnAdds()
    {
        DeSpawnAdds();
        if (isAddlistEmpty())
        {
            Creature* pCreature = NULL;
            std::vector<uint32> AddList;

            for (uint8 i = 0; i < 6; ++i)
                AddList.push_back(Adds[i]);

            while (AddList.size() > 4)
                AddList.erase((AddList.begin()) + (rand() % AddList.size()));

            uint8 i = 0;
            for (std::vector<uint32>::const_iterator itr = AddList.begin(); itr != AddList.end(); ++itr)
            {
                uint32 entry = *itr;

                pCreature = me->SummonCreature(entry, Locations[i][0], Locations[i][1], POS_Z, Locations[i][2], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                if (pCreature)
                {
                    AddGUID[i] = pCreature->GetGUID();
                    AddId[i] = entry;
                }
                ++i;
            }
        }
        else
        {
            for (uint8 i = 0; i < 4; ++i)
            {
                Creature* pCreature = me->SummonCreature(AddId[i], Locations[i][0], Locations[i][1], POS_Z, Locations[i][2], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                if (pCreature)
                    AddGUID[i] = pCreature->GetGUID();
            }
        }
    }

    bool isAddlistEmpty()
    {
        for (uint8 i = 0; i < 4; ++i)
        {
            if (AddId[i] == 0)
                return true;
        }
        return false;
    }

    void DeSpawnAdds()
    {
        for (uint8 i = 0; i < 4 ; ++i)
        {
            Creature* Temp = NULL;
            if (AddGUID[i])
            {
                Temp = Creature::GetCreature((*me), AddGUID[i]);
                if (Temp && Temp->IsAlive())
                    Temp->DisappearAndDie(false);
            }
        }
    }

    void AddsAttack()
    {
        for (uint8 i = 0; i < 4; ++i)
        {
            Creature* Temp = NULL;
            if (AddGUID[i])
            {
                Temp = Creature::GetCreature((*me), AddGUID[i]);
                if (Temp && Temp->IsAlive())
                {
                    Temp->AI()->AttackStart(me->GetVictim());
                    DoZoneInCombat(Temp);
                }
                else
                    EnterEvadeMode();
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (pInstance && !pInstance->GetData(TYPE_MOROES))
        {
            EnterEvadeMode();
            return;
        }

        if (!Enrage && HealthBelowPct(30))
        {
            DoCast(me, SPELL_FRENZY);
            Enrage = true;
        }

        if (CheckAdds_Timer <= diff)
        {
            for (uint8 i = 0; i < 4; ++i)
            {
                Creature* Temp = NULL;
                if (AddGUID[i])
                {
                    Temp = Unit::GetCreature((*me), AddGUID[i]);
                    if (Temp && Temp->IsAlive())
                        if (!Temp->GetVictim())
                            Temp->AI()->AttackStart(me->GetVictim());
                }
            }
            CheckAdds_Timer = 5000;
        }
        else CheckAdds_Timer -= diff;

        if (!Enrage)
        {
            //Cast Vanish, then Garrote random victim
            if (Vanish_Timer <= diff)
            {
                DoCast(me, SPELL_VANISH);
                InVanish = true;
                Vanish_Timer = 35000;
                Wait_Timer = 12000;
                return;
            }
            else Vanish_Timer -= diff;

            //Blind highest aggro, and attack second highest
            if (Gouge_Timer <= diff)
            {
                DoCastVictim( SPELL_GOUGE);
                Gouge_Timer = 40000;
            }
            else Gouge_Timer -= diff;

            if (Blind_Timer <= diff)
            {
                Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 25, true);
                if (pTarget && me->IsWithinMeleeRange(pTarget))
                    DoCast(pTarget, SPELL_BLIND);

                Blind_Timer = 40000;
            }
            else Blind_Timer -= diff;
        }

        if (InVanish)
        {
            if (Wait_Timer <= diff)
            {
                DoScriptText(RAND(SAY_SPECIAL_1, SAY_SPECIAL_2), me);

                if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                    pTarget->CastSpell(pTarget, SPELL_GARROTE, true);

                InVanish = false;
            }
            else Wait_Timer -= diff;
        }

        if (!InVanish)
            DoMeleeAttackIfReady();
    }
};

struct boss_moroes_guestAI : public ScriptedAI
{
    ScriptedInstance* pInstance;

    uint64 GuestGUID[4];

    boss_moroes_guestAI(Creature* c) : ScriptedAI(c)
    {
        for (uint8 i = 0; i < 4; ++i)
            GuestGUID[i] = 0;

        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    void Reset()
    {
        if (pInstance)
            pInstance->SetData(TYPE_MOROES, NOT_STARTED);
    }

    void AcquireGUID()
    {
        if (!pInstance)
            return;

        uint64 MoroesGUID = pInstance->GetData64(DATA_MOROES);
        Creature* Moroes = (Unit::GetCreature((*me), MoroesGUID));
        if (Moroes)
        {
            for (uint8 i = 0; i < 4; ++i)
            {
                uint64 GUID = CAST_AI(boss_moroesAI, Moroes->AI())->AddGUID[i];
                if (GUID)
                    GuestGUID[i] = GUID;
            }
        }
    }

    Unit* SelectGuestTarget()
    {
        uint64 TempGUID = GuestGUID[rand() % 4];
        if (TempGUID)
        {
            Unit* pUnit = Unit::GetUnit((*me), TempGUID);
            if (pUnit && pUnit->IsAlive())
                return pUnit;
        }

        return me;
    }

    void UpdateAI(const uint32 /*diff*/)
    {
        if (pInstance && !pInstance->GetData(TYPE_MOROES))
            EnterEvadeMode();

        DoMeleeAttackIfReady();
    }
};

#define SPELL_MANABURN       29405
#define SPELL_MINDFLY        29570
#define SPELL_SWPAIN         34441
#define SPELL_SHADOWFORM     29406

struct boss_baroness_dorothea_millstipeAI : public boss_moroes_guestAI
{
    //Shadow Priest
    boss_baroness_dorothea_millstipeAI(Creature* c) : boss_moroes_guestAI(c) {}

    uint32 ManaBurn_Timer;
    uint32 MindFlay_Timer;
    uint32 ShadowWordPain_Timer;

    void Reset()
    {
        ManaBurn_Timer = 7000;
        MindFlay_Timer = 1000;
        ShadowWordPain_Timer = 6000;

        DoCast(me, SPELL_SHADOWFORM, true);

        boss_moroes_guestAI::Reset();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        boss_moroes_guestAI::UpdateAI(diff);

        if (MindFlay_Timer <= diff)
        {
            DoCastVictim( SPELL_MINDFLY);
            MindFlay_Timer = 12000;                         // 3 sec channeled
        }
        else MindFlay_Timer -= diff;

        if (ManaBurn_Timer <= diff)
        {
            if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
                if (pTarget->getPowerType() == POWER_MANA)
                    DoCast(pTarget, SPELL_MANABURN);
            ManaBurn_Timer = 5000;                          // 3 sec cast
        }
        else ManaBurn_Timer -= diff;

        if (ShadowWordPain_Timer <= diff)
        {
            if (Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true))
            {
                DoCast(pTarget, SPELL_SWPAIN);
                ShadowWordPain_Timer = 7000;
            }
        }
        else ShadowWordPain_Timer -= diff;
    }
};

#define SPELL_HAMMEROFJUSTICE       13005
#define SPELL_JUDGEMENTOFCOMMAND    29386
#define SPELL_SEALOFCOMMAND         29385

struct boss_baron_rafe_dreugerAI : public boss_moroes_guestAI
{
    //Retr Pally
    boss_baron_rafe_dreugerAI(Creature* c) : boss_moroes_guestAI(c) {}

    uint32 HammerOfJustice_Timer;
    uint32 SealOfCommand_Timer;
    uint32 JudgementOfCommand_Timer;

    void Reset()
    {
        HammerOfJustice_Timer = 1000;
        SealOfCommand_Timer = 7000;
        JudgementOfCommand_Timer = SealOfCommand_Timer + 29000;

        boss_moroes_guestAI::Reset();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        boss_moroes_guestAI::UpdateAI(diff);

        if (SealOfCommand_Timer <= diff)
        {
            DoCast(me, SPELL_SEALOFCOMMAND);
            SealOfCommand_Timer = 32000;
            JudgementOfCommand_Timer = 29000;
        }
        else SealOfCommand_Timer -= diff;

        if (JudgementOfCommand_Timer <= diff)
        {
            DoCastVictim( SPELL_JUDGEMENTOFCOMMAND);
            JudgementOfCommand_Timer = SealOfCommand_Timer + 29000;
        }
        else JudgementOfCommand_Timer -= diff;

        if (HammerOfJustice_Timer <= diff)
        {
            DoCastVictim( SPELL_HAMMEROFJUSTICE);
            HammerOfJustice_Timer = 12000;
        }
        else HammerOfJustice_Timer -= diff;
    }
};

#define SPELL_DISPELMAGIC           15090                   //Self or other guest+Moroes
#define SPELL_GREATERHEAL           29564                   //Self or other guest+Moroes
#define SPELL_HOLYFIRE              29563
#define SPELL_PWSHIELD              29408

struct boss_lady_catriona_von_indiAI : public boss_moroes_guestAI
{
    //Holy Priest
    boss_lady_catriona_von_indiAI(Creature* c) : boss_moroes_guestAI(c) {}

    uint32 DispelMagic_Timer;
    uint32 GreaterHeal_Timer;
    uint32 HolyFire_Timer;
    uint32 PowerWordShield_Timer;

    void Reset()
    {
        DispelMagic_Timer = 11000;
        GreaterHeal_Timer = 1500;
        HolyFire_Timer = 5000;
        PowerWordShield_Timer = 1000;

        AcquireGUID();

        boss_moroes_guestAI::Reset();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        boss_moroes_guestAI::UpdateAI(diff);

        if (PowerWordShield_Timer <= diff)
        {
            DoCast(me, SPELL_PWSHIELD);
            PowerWordShield_Timer = 15000;
        }
        else PowerWordShield_Timer -= diff;

        if (GreaterHeal_Timer <= diff)
        {
            Unit* pTarget = SelectGuestTarget();

            DoCast(pTarget, SPELL_GREATERHEAL);
            GreaterHeal_Timer = 17000;
        }
        else GreaterHeal_Timer -= diff;

        if (HolyFire_Timer <= diff)
        {
            DoCastVictim( SPELL_HOLYFIRE);
            HolyFire_Timer = 22000;
        }
        else HolyFire_Timer -= diff;

        if (DispelMagic_Timer <= diff)
        {
            if (Unit* pTarget = RAND(SelectGuestTarget(), SelectTarget(SELECT_TARGET_RANDOM, 0, 100, true)))
                DoCast(pTarget, SPELL_DISPELMAGIC);

            DispelMagic_Timer = 25000;
        }
        else DispelMagic_Timer -= diff;
    }
};

#define SPELL_CLEANSE               29380                   //Self or other guest+Moroes
#define SPELL_GREATERBLESSOFMIGHT   29381                   //Self or other guest+Moroes
#define SPELL_HOLYLIGHT             29562                   //Self or other guest+Moroes
#define SPELL_DIVINESHIELD          41367

struct boss_lady_keira_berrybuckAI : public boss_moroes_guestAI
{
    //Holy Pally
    boss_lady_keira_berrybuckAI(Creature* c) : boss_moroes_guestAI(c)  {}

    uint32 Cleanse_Timer;
    uint32 GreaterBless_Timer;
    uint32 HolyLight_Timer;
    uint32 DivineShield_Timer;

    void Reset()
    {
        Cleanse_Timer = 13000;
        GreaterBless_Timer = 1000;
        HolyLight_Timer = 7000;
        DivineShield_Timer = 31000;

        AcquireGUID();

        boss_moroes_guestAI::Reset();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        boss_moroes_guestAI::UpdateAI(diff);

        if (DivineShield_Timer <= diff)
        {
            DoCast(me, SPELL_DIVINESHIELD);
            DivineShield_Timer = 31000;
        }
        else DivineShield_Timer -= diff;

        if (HolyLight_Timer <= diff)
        {
            Unit* pTarget = SelectGuestTarget();

            DoCast(pTarget, SPELL_HOLYLIGHT);
            HolyLight_Timer = 10000;
        }
        else HolyLight_Timer -= diff;

        if (GreaterBless_Timer <= diff)
        {
            Unit* pTarget = SelectGuestTarget();

            DoCast(pTarget, SPELL_GREATERBLESSOFMIGHT);

            GreaterBless_Timer = 50000;
        }
        else GreaterBless_Timer -= diff;

        if (Cleanse_Timer <= diff)
        {
            Unit* pTarget = SelectGuestTarget();

            DoCast(pTarget, SPELL_CLEANSE);

            Cleanse_Timer = 10000;
        }
        else Cleanse_Timer -= diff;
    }
};

#define SPELL_HAMSTRING         9080
#define SPELL_MORTALSTRIKE      29572
#define SPELL_WHIRLWIND         29573

struct boss_lord_robin_darisAI : public boss_moroes_guestAI
{
    //Arms Warr
    boss_lord_robin_darisAI(Creature* c) : boss_moroes_guestAI(c) {}

    uint32 Hamstring_Timer;
    uint32 MortalStrike_Timer;
    uint32 WhirlWind_Timer;

    void Reset()
    {
        Hamstring_Timer = 7000;
        MortalStrike_Timer = 10000;
        WhirlWind_Timer = 21000;

        boss_moroes_guestAI::Reset();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        boss_moroes_guestAI::UpdateAI(diff);

        if (Hamstring_Timer <= diff)
        {
            DoCastVictim( SPELL_HAMSTRING);
            Hamstring_Timer = 12000;
        }
        else Hamstring_Timer -= diff;

        if (MortalStrike_Timer <= diff)
        {
            DoCastVictim( SPELL_MORTALSTRIKE);
            MortalStrike_Timer = 18000;
        }
        else MortalStrike_Timer -= diff;

        if (WhirlWind_Timer <= diff)
        {
            DoCast(me, SPELL_WHIRLWIND);
            WhirlWind_Timer = 21000;
        }
        else WhirlWind_Timer -= diff;
    }
};

#define SPELL_DISARM            8379
#define SPELL_HEROICSTRIKE      29567
#define SPELL_SHIELDBASH        11972
#define SPELL_SHIELDWALL        29390

struct boss_lord_crispin_ferenceAI : public boss_moroes_guestAI
{
    //Arms Warr
    boss_lord_crispin_ferenceAI(Creature* c) : boss_moroes_guestAI(c) {}

    uint32 Disarm_Timer;
    uint32 HeroicStrike_Timer;
    uint32 ShieldBash_Timer;
    uint32 ShieldWall_Timer;

    void Reset()
    {
        Disarm_Timer = 6000;
        HeroicStrike_Timer = 10000;
        ShieldBash_Timer = 8000;
        ShieldWall_Timer = 4000;

        boss_moroes_guestAI::Reset();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        boss_moroes_guestAI::UpdateAI(diff);

        if (Disarm_Timer <= diff)
        {
            DoCastVictim( SPELL_DISARM);
            Disarm_Timer = 12000;
        }
        else Disarm_Timer -= diff;

        if (HeroicStrike_Timer <= diff)
        {
            DoCastVictim( SPELL_HEROICSTRIKE);
            HeroicStrike_Timer = 10000;
        }
        else HeroicStrike_Timer -= diff;

        if (ShieldBash_Timer <= diff)
        {
            DoCastVictim( SPELL_SHIELDBASH);
            ShieldBash_Timer = 13000;
        }
        else ShieldBash_Timer -= diff;

        if (ShieldWall_Timer <= diff)
        {
            DoCast(me, SPELL_SHIELDWALL);
            ShieldWall_Timer = 21000;
        }
        else ShieldWall_Timer -= diff;
    }
};

CreatureAI* GetAI_boss_moroes(Creature* pCreature)
{
    return GetInstanceAI<boss_moroesAI>(pCreature);
}

CreatureAI* GetAI_baroness_dorothea_millstipe(Creature* pCreature)
{
    return GetInstanceAI<boss_baroness_dorothea_millstipeAI>(pCreature);
}

CreatureAI* GetAI_baron_rafe_dreuger(Creature* pCreature)
{
    return GetInstanceAI<boss_baron_rafe_dreugerAI>(pCreature);
}

CreatureAI* GetAI_lady_catriona_von_indi(Creature* pCreature)
{
    return GetInstanceAI<boss_lady_catriona_von_indiAI>(pCreature);
}

CreatureAI* GetAI_lady_keira_berrybuck(Creature* pCreature)
{
    return GetInstanceAI<boss_lady_keira_berrybuckAI>(pCreature);
}

CreatureAI* GetAI_lord_robin_daris(Creature* pCreature)
{
    return GetInstanceAI<boss_lord_robin_darisAI>(pCreature);
}

CreatureAI* GetAI_lord_crispin_ference(Creature* pCreature)
{
    return GetInstanceAI<boss_lord_crispin_ferenceAI>(pCreature);
}

void AddSC_boss_moroes()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_moroes";
    newscript->GetAI = &GetAI_boss_moroes;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_baroness_dorothea_millstipe";
    newscript->GetAI = &GetAI_baroness_dorothea_millstipe;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_baron_rafe_dreuger";
    newscript->GetAI = &GetAI_baron_rafe_dreuger;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_lady_catriona_von_indi";
    newscript->GetAI = &GetAI_lady_catriona_von_indi;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_lady_keira_berrybuck";
    newscript->GetAI = &GetAI_lady_keira_berrybuck;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_lord_robin_daris";
    newscript->GetAI = &GetAI_lord_robin_daris;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_lord_crispin_ference";
    newscript->GetAI = &GetAI_lord_crispin_ference;
    newscript->RegisterSelf();
}

