#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum GateWatcher
{
    SAY_AGGRO           = - 1554000,
    SAY_SAW_ATTACK1     = - 1554001,
    SAY_SAW_ATTACK2     = - 1554002,
    SAY_SLAY1           = - 1554003,
    SAY_SLAY2           = - 1554004,
    SAY_DEATH           = - 1554005,

    SPELL_STREAM_OF_MACHINE_FLUID  = 35311,
    SPELL_SAW_BLADE                = 35318,
    H_SPELL_SAW_BLADE              = 39192,
    SPELL_SHADOW_POWER             = 35322,
    H_SPELL_SHADOW_POWER           = 39193,

    EVENT_STREAM_OF_MACHINE_FLUID   = 1,
    EVENT_SAW_BLADE                 = 2,
    EVENT_SHADOW_POWER              = 3
};

struct boss_gatewatcter_gryokillAI : public ScriptedAI
{
    boss_gatewatcter_gryokillAI(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    void JustDied(Unit* /*killer*/)
    {
        DoScriptText(SAY_DEATH, me);
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(SAY_AGGRO, me);
        events.ScheduleEvent(EVENT_STREAM_OF_MACHINE_FLUID, 10000);
        events.ScheduleEvent(EVENT_SAW_BLADE, 20000);
        events.ScheduleEvent(EVENT_SHADOW_POWER, 30000);
    }

    void KilledUnit(Unit* victim)
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2), me);
    }

    void UpdateAI(uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);
        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        switch (events.ExecuteEvent())
        {
        case EVENT_STREAM_OF_MACHINE_FLUID:
            me->CastSpell(me->GetVictim(), SPELL_STREAM_OF_MACHINE_FLUID, false);
            events.ScheduleEvent(EVENT_STREAM_OF_MACHINE_FLUID, urand(12000, 14000));
            break;
        case EVENT_SAW_BLADE:
            if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 50.0f))
                me->CastSpell(target, HeroicMode ? H_SPELL_SAW_BLADE : SPELL_SAW_BLADE, false);
            DoScriptText(RAND(SAY_SAW_ATTACK1, SAY_SAW_ATTACK2), me);
            events.ScheduleEvent(EVENT_SAW_BLADE, 25000);
            break;
        case EVENT_SHADOW_POWER:
            me->CastSpell(me, HeroicMode? H_SPELL_SHADOW_POWER : SPELL_SHADOW_POWER, false);
            events.ScheduleEvent(EVENT_SAW_BLADE, 25000);
            break;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_GateKeeperGryo(Creature* pCreature)
{
    return GetInstanceAI<boss_gatewatcter_gryokillAI>(pCreature);
}

void AddSC_boss_gatewatcher_gryo()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_gatewatcher_gryo";
    newscript->GetAI = &GetAI_boss_GateKeeperGryo;
    newscript->RegisterSelf();
}