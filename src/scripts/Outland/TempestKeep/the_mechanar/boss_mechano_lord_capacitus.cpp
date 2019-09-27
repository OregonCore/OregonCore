#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "mechanar.h"
#include "SpellAuras.h"

enum spells
{
    SPELL_HEAD_CRACK = 35161,
    SPELL_NETHERBOM = 35057,
    SPELL_REFLECTIVE_DAMAGE_SHIELD = 35159,
    SPELL_REFLECTIVE_MAGIC_SHIELD = 35158,
    SPELL_POLARITY_SHIFT = 39096,
    SPELL_ENRAGE = 26662
};

enum creatures
{
    NPC_NETHER_CHARGE = 20405
};

enum says
{
    YELL_AGGRO = -1921900,
    YELL_REFLECTIVE_TAUNT1 = -1921910,
    YELL_REFLECTIVE_TAUNT2 = -1921920,
    YELL_KILL1 = -1921930,
    YELL_KILL2 = -1921940,
    YELL_DEATH = -1921950
};

enum events
{
    EVENT_HEAD_CRACK     = 1,
    EVENT_NETHERBOMB     = 2,
    EVENT_REFLECTIVE     = 3,
    EVENT_POLARITY_SHIFT = 4,
    EVENT_ENRAGE         = 5
};


struct boss_mechano_lord_capacitusAI : public ScriptedAI
{
    boss_mechano_lord_capacitusAI(Creature* c) : ScriptedAI(c), summons(me)
    {
        pInstance = (ScriptedInstance*)c->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();
    }

    SummonList summons;
    ScriptedInstance* pInstance;
    bool HeroicMode;
    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(YELL_AGGRO, me);
        events.ScheduleEvent(EVENT_HEAD_CRACK, 6000);
        events.ScheduleEvent(EVENT_NETHERBOMB, 10000);
        events.ScheduleEvent(EVENT_ENRAGE, 180000);
        events.ScheduleEvent(HeroicMode ? EVENT_POLARITY_SHIFT : EVENT_REFLECTIVE, 15000);
    }

    void JustDied(Unit* /*victim*/)
    {
        DoScriptText(YELL_DEATH, me, nullptr);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        DoScriptText(RAND(YELL_KILL1, YELL_KILL2), me);
    }

    void JustSummoned(Creature* summon)
    {
        summons.Summon(summon);
        summon->GetMotionMaster()->MoveRandom(30.0f);
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
        case EVENT_HEAD_CRACK:
            me->CastSpell(me->GetVictim(), SPELL_HEAD_CRACK, false);
            events.Repeat(20000);
            break;
        case EVENT_NETHERBOMB:
        {
            Position pos = me->GetRandomNearPosition(8.0f);
            me->SummonCreature(NPC_NETHER_CHARGE, pos, TEMPSUMMON_CORPSE_DESPAWN, 18000);
            events.Repeat(5000);
            break;
        }
        case EVENT_REFLECTIVE:
            me->CastSpell(me, RAND(SPELL_REFLECTIVE_DAMAGE_SHIELD, SPELL_REFLECTIVE_MAGIC_SHIELD), false);
            DoScriptText(RAND(YELL_REFLECTIVE_TAUNT1, YELL_REFLECTIVE_TAUNT2), me);
            events.Repeat(EVENT_REFLECTIVE, 20000);
            break;
        case EVENT_POLARITY_SHIFT: // Should only happen in heroic mode.
            me->CastSpell(me, SPELL_POLARITY_SHIFT, true);
            events.Repeat(30000);
            break;
        case EVENT_ENRAGE: // After 3 minutes he should enrage
            me->CastSpell(me, SPELL_ENRAGE, false);
            break;
        }
        DoMeleeAttackIfReady();
    }

};


CreatureAI* GetAI_boss_mechano_lord_capacitus(Creature* pCreature)
{
    return new boss_mechano_lord_capacitusAI(pCreature);
}

void AddSC_boss_mechano_lord_capacitus()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_mechano_lord_capacitus";
    newscript->GetAI = &GetAI_boss_mechano_lord_capacitus;
    newscript->RegisterSelf();
}