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
SDName: Boss_Exarch_Maladaar
SD%Complete: 95
SDComment: Most of event implemented, some adjustments to timers remain and possibly make some better code for switching his dark side in to better "images" of player.
SDCategory: Auchindoun, Auchenai Crypts
EndScriptData */

/* ContentData
mob_stolen_soul
boss_exarch_maladaar
mob_avatar_of_martyred
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum ExarchMaladaar
{
    SAY_INTRO                   = -1558000,
    SAY_SUMMON                  = -1558001,
    SAY_AGGRO_1                 = -1558002,
    SAY_AGGRO_2                 = -1558003,
    SAY_AGGRO_3                 = -1558004,
    SAY_ROAR                    = -1558005,
    SAY_SOUL_CLEAVE             = -1558006,
    SAY_SLAY_1                  = -1558007,
    SAY_SLAY_2                  = -1558008,
    SAY_DEATH                   = -1558009,

    SPELL_RIBBON_OF_SOULS       = 32422,
    SPELL_SOUL_SCREAM           = 32421,
    SPELL_STOLEN_SOUL           = 32346,
    SPELL_STOLEN_SOUL_VISUAL    = 32395,
    SPELL_SUMMON_AVATAR         = 32424,

    ENTRY_STOLEN_SOUL           = 18441
};

struct boss_exarch_maladaarAI : public ScriptedAI
{
    boss_exarch_maladaarAI(Creature* c) : ScriptedAI(c)
    {
        HasTaunted = false;
    }

    uint32 soulmodel;
    uint64 soulholder;
    uint8 soulclass;

    uint32 Fear_timer;
    uint32 Ribbon_of_Souls_timer;
    uint32 StolenSoul_Timer;

    bool HasTaunted;
    bool Avatar_summoned;

    void Reset()
    {
        soulmodel = 0;
        soulholder = 0;
        soulclass = 0;

        Fear_timer = 15000 + rand() % 5000;
        Ribbon_of_Souls_timer = 5000;
        StolenSoul_Timer = 25000 + rand() % 10000;

        Avatar_summoned = false;
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (!HasTaunted && me->IsWithinDistInMap(who, 150.0f))
        {
            DoScriptText(SAY_INTRO, me);
            HasTaunted = true;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }


    void EnterCombat(Unit*)
    {
        switch (rand() % 3)
        {
        case 0:
            DoScriptText(SAY_AGGRO_1, me);
            break;
        case 1:
            DoScriptText(SAY_AGGRO_2, me);
            break;
        case 2:
            DoScriptText(SAY_AGGRO_3, me);
            break;
        }
    }

    void JustSummoned(Creature* summoned)
    {
        if (summoned->GetEntry() == ENTRY_STOLEN_SOUL)
        {
            //SPELL_STOLEN_SOUL_VISUAL has shapeshift effect, but not implemented feature in OREGON for this spell.
            summoned->CastSpell(summoned, SPELL_STOLEN_SOUL_VISUAL, false);
            summoned->SetDisplayId(soulmodel);
            summoned->SetFaction(me->GetFaction());

            if (Unit* pTarget = Unit::GetUnit(*me, soulholder))
            {

                //((mob_stolen_soulAI*)summoned->AI())->SetMyClass(soulclass);
                summoned->AI()->AttackStart(pTarget);
            }
        }
    }

    void KilledUnit(Unit*)
    {
        if (rand() % 2)
            return;

        switch (rand() % 2)
        {
        case 0:
            DoScriptText(SAY_SLAY_1, me);
            break;
        case 1:
            DoScriptText(SAY_SLAY_2, me);
            break;
        }
    }

    void JustDied(Unit*)
    {
        DoScriptText(SAY_DEATH, me);
        //When Exarch Maladar is defeated D'ore appear.
        DoSpawnCreature(19412, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 600000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (!Avatar_summoned && ((me->GetHealth() * 100) / me->GetMaxHealth() < 25))
        {
            if (me->IsNonMeleeSpellCast(false))
                me->InterruptNonMeleeSpells(true);

            DoScriptText(SAY_SUMMON, me);

            DoCast(me, SPELL_SUMMON_AVATAR);
            Avatar_summoned = true;
            StolenSoul_Timer = 15000 + rand() % 15000;
        }

        if (StolenSoul_Timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
            {
                if (pTarget->GetTypeId() == TYPEID_PLAYER)
                {
                    if (me->IsNonMeleeSpellCast(false))
                        me->InterruptNonMeleeSpells(true);

                    uint32 i = urand(1, 2);
                    if (i == 1)
                        DoScriptText(SAY_ROAR, me);
                    else
                        DoScriptText(SAY_SOUL_CLEAVE, me);

                    soulmodel = pTarget->GetDisplayId();
                    soulholder = pTarget->GetGUID();
                    soulclass = pTarget->getClass();

                    DoCast(pTarget, SPELL_STOLEN_SOUL);
                    DoSpawnCreature(ENTRY_STOLEN_SOUL, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);

                    StolenSoul_Timer = 20000 + rand() % 10000;
                }
                else StolenSoul_Timer = 1000;
            }
        }
        else StolenSoul_Timer -= diff;

        if (Ribbon_of_Souls_timer <= diff)
        {
            if (Unit* pTarget = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_RIBBON_OF_SOULS);

            Ribbon_of_Souls_timer = 5000 + (rand() % 20 * 1000);
        }
        else Ribbon_of_Souls_timer -= diff;

        if (Fear_timer <= diff)
        {
            DoCast(me, SPELL_SOUL_SCREAM);
            Fear_timer = 15000 + rand() % 15000;
        }
        else Fear_timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_exarch_maladaar(Creature* pCreature)
{
    return new boss_exarch_maladaarAI (pCreature);
}

#define SPELL_AV_MORTAL_STRIKE          16856
#define SPELL_AV_MORTAL_STRIKE          16856

struct mob_avatar_of_martyredAI : public ScriptedAI
{
    mob_avatar_of_martyredAI(Creature* c) : ScriptedAI(c) {}

    uint32 Mortal_Strike_timer;

    void Reset()
    {
        Mortal_Strike_timer = 10000;
    }

    void EnterCombat(Unit*)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Mortal_Strike_timer <= diff)
        {
            DoCastVictim( SPELL_AV_MORTAL_STRIKE);
            Mortal_Strike_timer = 10000 + rand() % 20 * 1000;
        }
        else Mortal_Strike_timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_avatar_of_martyred(Creature* pCreature)
{
    return new mob_avatar_of_martyredAI (pCreature);
}

enum stolenSoul
{
	SPELL_MOONFIRE				= 37328,
	SPELL_FIREBALL				= 37329,
	SPELL_MIND_FLAY				= 37330,
	SPELL_HEMORRHAGE			= 37331,
	SPELL_FROSTSHOCK			= 37332,
	SPELL_CURSE_OF_AGONY		= 37334,
	SPELL_MORTAL_STRIKE			= 37335,
	SPELL_FREEZING_TRAP			= 37368,
	SPELL_HAMMER_OF_JUSTICE		= 37369
};

struct mob_stolen_soulAI : public ScriptedAI
{
    mob_stolen_soulAI(Creature* c) : ScriptedAI(c) {}

    uint8 myClass;
    uint32 Class_Timer;

    void Reset()
    {
        myClass = CLASS_WARRIOR;
        Class_Timer = 1000;
    }

    void EnterCombat(Unit*) { }

    void SetMyClass(uint8 myclass)
    {
        myClass = myclass;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Class_Timer <= diff)
        {
            switch (myClass)
            {
                case CLASS_WARRIOR:
                    DoCastVictim( SPELL_MORTAL_STRIKE);
                    Class_Timer = 6000;
                    break;
                case CLASS_PALADIN:
                    DoCastVictim( SPELL_HAMMER_OF_JUSTICE);
                    Class_Timer = 6000;
                    break;
                case CLASS_HUNTER:
                    DoCastVictim( SPELL_FREEZING_TRAP);
                    Class_Timer = 20000;
                    break;
                case CLASS_ROGUE:
                    DoCastVictim( SPELL_HEMORRHAGE);
                    Class_Timer = 10000;
                    break;
                case CLASS_PRIEST:
                    DoCastVictim( SPELL_MIND_FLAY);
                    Class_Timer = 5000;
                    break;
                case CLASS_SHAMAN:
                    DoCastVictim( SPELL_FROSTSHOCK);
                    Class_Timer = 8000;
                    break;
                case CLASS_MAGE:
                    DoCastVictim( SPELL_FIREBALL);
                    Class_Timer = 5000;
                    break;
                case CLASS_WARLOCK:
                    DoCastVictim( SPELL_CURSE_OF_AGONY);
                    Class_Timer = 20000;
                    break;
                case CLASS_DRUID:
                    DoCastVictim( SPELL_MOONFIRE);
                    Class_Timer = 10000;
                    break;
            }
        }
        else Class_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_stolen_soul(Creature* creature)
{
    return new mob_stolen_soulAI (creature);
}

void AddSC_boss_exarch_maladaar()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_exarch_maladaar";
    newscript->GetAI = &GetAI_boss_exarch_maladaar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_avatar_of_martyred";
    newscript->GetAI = &GetAI_mob_avatar_of_martyred;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_stolen_soul";
    newscript->GetAI = &GetAI_mob_stolen_soul;
    newscript->RegisterSelf();
}

