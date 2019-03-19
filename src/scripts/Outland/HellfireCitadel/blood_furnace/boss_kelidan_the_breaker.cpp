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
SDName: Boss_Kelidan_The_Breaker
SD%Complete: 100
SDComment:
SDCategory: Hellfire Citadel, Blood Furnace
EndScriptData */

/* ContentData
boss_kelidan_the_breaker
mob_shadowmoon_channeler
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "blood_furnace.h"
#include "SpellAuras.h"

enum eKelidan
{
    SAY_WAKE                    = -1542000,
    SAY_ADD_AGGRO_1             = -1542001,
    SAY_ADD_AGGRO_2             = -1542002,
    SAY_ADD_AGGRO_3             = -1542003,
    SAY_KILL_1                  = -1542004,
    SAY_KILL_2                  = -1542005,
    SAY_NOVA                    = -1542006,
    SAY_DIE                     = -1542007,

    SPELL_CORRUPTION            = 30938,
    SPELL_EVOCATION             = 30935,

    SPELL_FIRE_NOVA             = 33132,
    H_SPELL_FIRE_NOVA           = 37371,

    SPELL_SHADOW_BOLT_VOLLEY    = 28599,
    H_SPELL_SHADOW_BOLT_VOLLEY  = 40070,

    SPELL_BURNING_NOVA          = 30940,
    SPELL_VORTEX                = 37370,

    ENTRY_KELIDAN               = 17377,
    ENTRY_CHANNELER             = 17653,

    ACTION_ACTIVATE_ADDS        = 92,
	
	SPELL_BUFF					= 30939
};

const float ShadowmoonChannelers[5][4] =
{
    {302.0f, -87.0f, -24.4f, 0.157f},
    {321.0f, -63.5f, -24.6f, 4.887f},
    {346.0f, -74.5f, -24.6f, 3.595f},
    {344.0f, -103.5f, -24.5f, 2.356f},
    {316.0f, -109.0f, -24.6f, 1.257f}
};

class BurningNovaAura : public Aura
{
    public:
        BurningNovaAura(SpellEntry* spell, uint32 eff, Unit* pTarget, Unit* caster) : Aura(spell, eff, NULL, pTarget, caster, NULL) {}
};

struct boss_kelidan_the_breakerAI : public ScriptedAI
{
    boss_kelidan_the_breakerAI(Creature* creature) : ScriptedAI(creature)
    {
        instance = (ScriptedInstance*)creature->GetInstanceData();
        for (uint8 i = 0; i < 5; ++i)
            Channelers[i] = 0;
    }

    ScriptedInstance* instance;

    uint32 ShadowVolley_Timer;
    uint32 BurningNova_Timer;
    uint32 Firenova_Timer;
    uint32 Corruption_Timer;
    uint32 check_Timer;
    bool Firenova;
    bool addYell;
    uint64 Channelers[5];

    void Reset()
    {
        ShadowVolley_Timer = 1000;
        BurningNova_Timer = 15000;
        Corruption_Timer = 5000;
        check_Timer = 0;
        Firenova = false;
        addYell = false;
        SummonChannelers();
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        if (instance)
            instance->SetData(DATA_KELIDANEVENT, NOT_STARTED);

		//warrior bleeds
		me->ApplySpellImmune(0, IMMUNITY_ID, 25208, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11574, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11573, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11572, true);

		me->ApplySpellImmune(0, IMMUNITY_ID, 12834, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 12849, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 12867, true);

		//druid bleeds
		me->ApplySpellImmune(0, IMMUNITY_ID, 33876, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 33878, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 33917, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 33982, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 33983, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 33986, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 33987, true);

		me->ApplySpellImmune(0, IMMUNITY_ID, 27008, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 9492, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 9493, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 9752, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 9894, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 9896, true);

		//rogue bleeds
		me->ApplySpellImmune(0, IMMUNITY_ID, 26867, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 1943, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8639, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8640, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11273, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11274, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11275, true);

		me->ApplySpellImmune(0, IMMUNITY_ID, 26884, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 26839, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11290, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 11289, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8633, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8632, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 8631, true);
		me->ApplySpellImmune(0, IMMUNITY_ID, 703, true);
    }

    void EnterCombat(Unit* who)
    {
        DoScriptText(SAY_WAKE, me);
        if (me->IsNonMeleeSpellCast(false))
            me->InterruptNonMeleeSpells(true);
        DoStartMovement(who);
        if (instance)
            instance->SetData(DATA_KELIDANEVENT, IN_PROGRESS);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        if (rand() % 2)
            return;

        DoScriptText(RAND(SAY_KILL_1, SAY_KILL_2), me);
    }

    void ChannelerEngaged(Unit* who)
    {
        if (who && !addYell)
        {
            addYell = true;
            DoScriptText(RAND(SAY_ADD_AGGRO_1, SAY_ADD_AGGRO_2, SAY_ADD_AGGRO_3), me);
        }
        for (int i = 0; i < 5; ++i)
        {
            Creature* channeler = Unit::GetCreature(*me, Channelers[i]);
            if (who && channeler && !channeler->IsInCombat())
                channeler->AI()->AttackStart(who);
        }
    }

    void ChannelerDied(Unit* killer)
    {
        for (int i = 0; i < 5; ++i)
        {
            Creature* channeler = Unit::GetCreature(*me, Channelers[i]);
            if (channeler && channeler->IsAlive())
                return;
        }

        me->SetReactState(REACT_AGGRESSIVE);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        if (killer)
            me->AI()->AttackStart(killer);
    }

    uint64 GetChanneled(Creature* channeler1)
    {
        SummonChannelers();
        if (!channeler1)
            return 0;

        int i;
        for (i = 0; i < 5; ++i)
        {
            Creature* channeler = Unit::GetCreature(*me, Channelers[i]);
            if (channeler && channeler->GetGUID() == channeler1->GetGUID())
                break;
        }
        return Channelers[(i + 2) % 5];
    }

    void SummonChannelers()
    {
        for (int i = 0; i < 5; ++i)
        {
            Creature* channeler = Unit::GetCreature(*me, Channelers[i]);
            if (!channeler || channeler->isDead())
                channeler = me->SummonCreature(ENTRY_CHANNELER, ShadowmoonChannelers[i][0], ShadowmoonChannelers[i][1], ShadowmoonChannelers[i][2], ShadowmoonChannelers[i][3], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 300000);
            if (channeler)
                Channelers[i] = channeler->GetGUID();
            else
                Channelers[i] = 0;
        }
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DIE, me);
        if (instance)
            instance->SetData(DATA_KELIDANEVENT, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
        {
            if (check_Timer <= diff)
            {
                if (!me->IsNonMeleeSpellCast(false))
                    DoCast(me, SPELL_EVOCATION);
                check_Timer = 5000;
            }
            else
                check_Timer -= diff;

            return;
        }

        if (Firenova)
        {
            if (Firenova_Timer <= diff)
            {
                DoCast(me, HeroicMode ? H_SPELL_FIRE_NOVA : SPELL_FIRE_NOVA, true);
                Firenova = false;
                ShadowVolley_Timer = 2000;
            }
            else
                Firenova_Timer -= diff;

            return;
        }

        if (ShadowVolley_Timer <= diff)
        {
            DoCast(me, HeroicMode ? H_SPELL_SHADOW_BOLT_VOLLEY : SPELL_SHADOW_BOLT_VOLLEY);
            ShadowVolley_Timer = 5000 + rand() % 8000;
        }
        else
            ShadowVolley_Timer -= diff;

        if (Corruption_Timer <= diff)
        {
            DoCast(me, SPELL_CORRUPTION);
            Corruption_Timer = 30000 + rand() % 20000;
        }
        else
            Corruption_Timer -= diff;

        if (BurningNova_Timer <= diff)
        {
            if (me->IsNonMeleeSpellCast(false))
                me->InterruptNonMeleeSpells(true);

            DoScriptText(SAY_NOVA, me);

            if (SpellEntry* nova = GET_SPELL(SPELL_BURNING_NOVA))
            {
                for (uint32 i = 0; i < 3; ++i)
                    if (nova->Effect[i] == SPELL_EFFECT_APPLY_AURA)
                    {
                        Aura* Aur = new BurningNovaAura(nova, i, me, me);
                        me->AddAura(Aur);
                    }
            }

            if (HeroicMode)
                DoTeleportAll(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());

            BurningNova_Timer = 20000 + rand() % 8000;
            Firenova_Timer = 5000;
            Firenova = true;
        }
        else
            BurningNova_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_kelidan_the_breaker(Creature* pCreature)
{
    return new boss_kelidan_the_breakerAI (pCreature);
}

/*######
## mob_shadowmoon_channeler
######*/

enum eShadowmoon
{
    SPELL_SHADOW_BOLT       = 12739,
    H_SPELL_SHADOW_BOLT     = 15472,

    SPELL_MARK_OF_SHADOW    = 30937,
    SPELL_CHANNELING        = 39123
};

struct mob_shadowmoon_channelerAI : public ScriptedAI
{
    mob_shadowmoon_channelerAI(Creature* creature) : ScriptedAI(creature)
    {
        instance = (ScriptedInstance*)creature->GetInstanceData();
    }

    ScriptedInstance* instance;

    uint32 ShadowBolt_Timer;
    uint32 MarkOfShadow_Timer;
    uint32 check_Timer;

    void Reset()
    {
        ShadowBolt_Timer = 1000 + rand() % 1000;
        MarkOfShadow_Timer = 5000 + rand() % 2000;
        check_Timer = 0;
        if (me->IsNonMeleeSpellCast(false))
            me->InterruptNonMeleeSpells(true);
    }

    void EnterCombat(Unit* who)
    {
        if (Creature* Kelidan = me->FindNearestCreature(ENTRY_KELIDAN, 100))
            CAST_AI(boss_kelidan_the_breakerAI, Kelidan->AI())->ChannelerEngaged(who);
        if (me->IsNonMeleeSpellCast(false))
            me->InterruptNonMeleeSpells(true);
        DoStartMovement(who);
    }

    void JustDied(Unit* Killer)
    {
        if (Creature* Kelidan = me->FindNearestCreature(ENTRY_KELIDAN, 100))
            CAST_AI(boss_kelidan_the_breakerAI, Kelidan->AI())->ChannelerDied(Killer);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
        {
            if (check_Timer <= diff)
            {
                if (!me->IsNonMeleeSpellCast(false))
                    if (Creature* Kelidan = me->FindNearestCreature(ENTRY_KELIDAN, 100))
                    {
                        uint64 channeler = CAST_AI(boss_kelidan_the_breakerAI, Kelidan->AI())->GetChanneled(me);
                        if (Unit* channeled = Unit::GetUnit(*me, channeler))
                            DoCast(channeled, SPELL_CHANNELING);
                    }
                check_Timer = 5000;
            }
            else
                check_Timer -= diff;

            return;
        }

        if (MarkOfShadow_Timer <= diff)
        {
            if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(target, SPELL_MARK_OF_SHADOW);
            MarkOfShadow_Timer = 15000 + rand() % 5000;
        }
        else
            MarkOfShadow_Timer -= diff;

        if (ShadowBolt_Timer <= diff)
        {
            DoCastVictim(HeroicMode ? H_SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT);
            ShadowBolt_Timer = 5000 + rand() % 1000;
        }
        else
            ShadowBolt_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_shadowmoon_channeler(Creature* pCreature)
{
    return new mob_shadowmoon_channelerAI (pCreature);
}

struct npc_kelridan_triggerAI : public ScriptedAI
{
	ScriptedInstance* pInstance;

	uint32 buffTimer;

	npc_kelridan_triggerAI(Creature* c) : ScriptedAI(c)
	{
		pInstance = (ScriptedInstance*)c->GetInstanceData();
	}

	void Reset()
	{		
		buffTimer = 5000;
	}

	void UpdateAI(const uint32 diff)
	{
		if (!UpdateVictim())
		{
			if (buffTimer <= diff)
			{
				DoCast(me, SPELL_BUFF);
				buffTimer = 5000;
			}
			else buffTimer -= diff;
		}
	}
};

CreatureAI* GetAI_npc_kelridan_trigger(Creature* pCreature)
{
	return new npc_kelridan_triggerAI(pCreature);
}



void AddSC_boss_kelidan_the_breaker()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_kelidan_the_breaker";
    newscript->GetAI = &GetAI_boss_kelidan_the_breaker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shadowmoon_channeler";
    newscript->GetAI = &GetAI_mob_shadowmoon_channeler;
    newscript->RegisterSelf();

	newscript = new Script;
	newscript->Name = "npc_kelridan_trigger";
	newscript->GetAI = &GetAI_npc_kelridan_trigger;
    newscript->RegisterSelf();
}

