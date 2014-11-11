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
SDName: Boss_Broggok
SD%Complete: 99
SDComment:
SDCategory: Hellfire Citadel, Blood Furnace
EndScriptData */

#include "ScriptPCH.h"
#include "blood_furnace.h"

enum Texts
{
    SAY_AGGRO              = -1542008
};

enum Spells
{
    SPELL_SLIME_SPRAY            = 30913,
    SPELL_SLIME_SPRAY_H          = 38458,
    SPELL_POISON_CLOUD           = 30916,
    SPELL_POISON_BOLT            = 30917,
    SPELL_POISON_BOLT_H          = 38459,

    SPELL_POISON_CLOUD_PASSIVE   = 30914,
    SPELL_POISON_CLOUD_PASSIVE_H = 38462
};

struct boss_broggokAI : public ScriptedAI
{
    boss_broggokAI(Creature* creature) : ScriptedAI(creature)
    {
        instance = creature->GetInstanceData();
    }

    ScriptedInstance* instance;

    uint32 AcidSpray_Timer;
    uint32 PoisonSpawn_Timer;
    uint32 PoisonBolt_Timer;

    void Reset()
    {
        me->SetUnitMovementFlags(MOVEFLAG_NONE);
        AcidSpray_Timer = 10000;
        PoisonSpawn_Timer = 5000;
        PoisonBolt_Timer = 7000;

        if (instance)
            instance->SetData(DATA_BROGGOKEVENT, NOT_STARTED);
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoScriptText(SAY_AGGRO, me);

        if (instance)
            instance->SetData(DATA_BROGGOKEVENT, IN_PROGRESS);
    }

    void JustDied(Unit* /*Killer*/)
    {
        if (instance)
            instance->SetData(DATA_BROGGOKEVENT, DONE);
    }

    void EnterEvadeMode()
    {
        me->RemoveAllAuras();
        me->DeleteThreatList();
        me->CombatStop(true);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE | UNIT_FLAG_PASSIVE);
        Reset();

        if (!me->IsAlive())
            return;

        if (instance)
        {
            instance->SetData(DATA_BROGGOKEVENT, FAIL);
            float fRespX, fRespY, fRespZ;
            me->GetRespawnCoord(fRespX, fRespY, fRespZ);
            me->GetMotionMaster()->MovePoint(0, fRespX, fRespY, fRespZ);
        }
        else
            me->GetMotionMaster()->MoveTargetedHome();
    }

    void MovementInform(uint32 uiMotionType, uint32 /*uiPointId*/)
    {
        if (uiMotionType == POINT_MOTION_TYPE)
        {
            if (GameObject* pFrontDoor = me->FindNearestGameObject(181819, 60.0f))
                me->SetFacingToObject(pFrontDoor);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (AcidSpray_Timer <= diff)
        {
            DoCastVictim(HeroicMode ? SPELL_SLIME_SPRAY_H : SPELL_SLIME_SPRAY);
            AcidSpray_Timer = 4000 + rand() % 8000;
        }
        else AcidSpray_Timer -= diff;

        if (PoisonBolt_Timer <= diff)
        {
            DoCastVictim(HeroicMode ? SPELL_POISON_BOLT_H : SPELL_POISON_BOLT);
            PoisonBolt_Timer = 4000 + rand() % 8000;
        }
        else PoisonBolt_Timer -= diff;

        if (PoisonSpawn_Timer <= diff)
        {
            DoCast(me, SPELL_POISON_CLOUD);
            PoisonSpawn_Timer = 20000;
        }
        else PoisonSpawn_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_broggokAI(Creature* pCreature)
{
    return new boss_broggokAI (pCreature);
}

/*######
## mob_nascent_orc
######*/

enum OrcSpells
{
    SPELL_BLOW    = 22427,
    SPELL_STOMP   = 31900
};

struct mob_nascent_orcAI : public ScriptedAI
{
    mob_nascent_orcAI(Creature* creature) : ScriptedAI(creature)
    {
        instance = creature->GetInstanceData();
    }

    ScriptedInstance* instance;

    uint32 Blow_Timer;
    uint32 Stomp_Timer;

    void Reset()
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetUnitMovementFlags(MOVEFLAG_NONE);
        Blow_Timer = 4000 + rand() % 4000;
        Stomp_Timer = 5000 + rand() % 4000;
    }

    void MovementInform(uint32 uiMotionType, uint32 /*uiPointId*/)
    {
        if (uiMotionType == POINT_MOTION_TYPE)
        {
            if (Unit* pTarget = me->SelectNearestTarget(99.0f))
                me->AI()->AttackStart(pTarget);
        }
    }

    void EnterEvadeMode()
    {
        if (instance)
            instance->SetData(DATA_BROGGOKEVENT, FAIL);

        me->DeleteThreatList();
        me->CombatStop(true);
        me->GetMotionMaster()->MoveTargetedHome();
        Reset();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (Blow_Timer <= diff)
        {
            DoCastVictim(SPELL_BLOW);
            Blow_Timer = 10000 + rand() % 4000;
        }
        else Blow_Timer -= diff;

        if (Stomp_Timer <= diff)
        {
            DoCastVictim(SPELL_STOMP);
            Stomp_Timer = 15000 + rand() % 4000;
        }
        else Stomp_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_nascent_orc(Creature* pCreature)
{
    return new mob_nascent_orcAI (pCreature);
}

/*######
## mob_broggok_poisoncloud
######*/

struct mob_broggok_poisoncloudAI : public ScriptedAI
{
    mob_broggok_poisoncloudAI(Creature* creature) : ScriptedAI(creature)
    {
        instance = creature->GetInstanceData();
    }

    ScriptedInstance* instance;

    void Reset()
    {
        DoCast(me, HeroicMode ? SPELL_POISON_CLOUD_PASSIVE_H : SPELL_POISON_CLOUD_PASSIVE);
    }

    void AttackedBy(Unit* /*who*/) {}
    void AttackStart(Unit* /*who*/) {}
};

CreatureAI* GetAI_mob_broggok_poisoncloud(Creature* pCreature)
{
    return new mob_broggok_poisoncloudAI (pCreature);
}

void AddSC_boss_broggok()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_broggok";
    newscript->GetAI = &GetAI_boss_broggokAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_nascent_orc";
    newscript->GetAI = &GetAI_mob_nascent_orc;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_broggok_poisoncloud";
    newscript->GetAI = &GetAI_mob_broggok_poisoncloud;
    newscript->RegisterSelf();
}

