#include "ScriptMgr.h"
#include "ScriptedCreature.h"

enum GateWatcherIronHand
{
    SAY_AGGRO_1 = -1554006,
    SAY_HAMMER_1 = -1554007,
    SAY_HAMMER_2 = -1554008,
    SAY_SLAY_1 = -1554009,
    SAY_SLAY_2 = -1554010,
    SAY_DEATH_1 = -1554011,
    EMOTE_HAMMER = -1554012,

    SPELL_SHADOW_POWER = 35322,
    H_SPELL_SHADOW_POWER = 39193,
    SPELL_HAMMER_PUNCH = 35326,
    SPELL_JACKHAMMER = 35327,
    H_SPELL_JACKHAMMER = 39194,
    SPELL_STREAM_OF_MACHINE_FLUID = 35311,

    EVENT_SHADOW_POWER = 1,
    EVENT_STREAM_OF_MACHINE_FLUID = 2,
    EVENT_JACKHAMMER = 3
};

struct boss_gatewatcher_iron_handAI : public ScriptedAI
{
    boss_gatewatcher_iron_handAI(Creature* c) : ScriptedAI(c)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;
    EventMap events;

    void Reset()
    {
        events.Reset();
    }
    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(SAY_AGGRO_1, me);
        events.ScheduleEvent(EVENT_STREAM_OF_MACHINE_FLUID, 15000);
        events.ScheduleEvent(EVENT_JACKHAMMER, 35000);
        events.ScheduleEvent(EVENT_SHADOW_POWER, 25000);
    }

    void KilledUnit(Unit* victim)
    {
        if (victim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
    }

    void JustDied(Unit* /*Killer*/)
    {
        DoScriptText(SAY_DEATH_1, me);
    }

    void UpdateAI(const uint32 diff)
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
            events.ScheduleEvent(EVENT_STREAM_OF_MACHINE_FLUID, 20000);
            break;
        case EVENT_JACKHAMMER:
            DoScriptText(EMOTE_HAMMER, me);
            DoScriptText(RAND(SAY_HAMMER_1, SAY_HAMMER_2), me);
            me->CastSpell(me, SPELL_JACKHAMMER, false);
            events.ScheduleEvent(EVENT_JACKHAMMER, 40000);
            break;
        case EVENT_SHADOW_POWER:
            me->CastSpell(me, SPELL_SHADOW_POWER, false);
            events.ScheduleEvent(EVENT_SHADOW_POWER, 25000);
            break;
        }

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_gatewatcher_iron_hand(Creature* pCreature)
{
    return GetInstanceAI<boss_gatewatcher_iron_handAI>(pCreature);
}

void AddSC_boss_gatewatcher_iron_hand()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_gatewatcher_iron_hand";
    newscript->GetAI = &GetAI_boss_gatewatcher_iron_hand;
    newscript->RegisterSelf();
}

