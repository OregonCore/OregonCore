/*
 * Copyright (C) 2010-2014 OregonCore <http://www.oregoncore.com/>
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2012 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Boss_Captain_Skarloc
SD%Complete: 99
SDComment:
SDCategory: Caverns of Time, Old Hillsbrad Foothills
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "old_hillsbrad.h"

#define SAY_ENTER                   -1560000
#define SAY_TAUNT1                  -1560001
#define SAY_TAUNT2                  -1560002
#define SAY_SLAY1                   -1560003
#define SAY_SLAY2                   -1560004
#define SAY_DEATH                   -1560005

#define SPELL_HOLY_LIGHT            29427
#define SPELL_CLEANSE               29380
#define SPELL_HAMMER_OF_JUSTICE     13005
#define SPELL_HOLY_SHIELD           31904
#define SPELL_DEVOTION_AURA         8258
#define SPELL_CONSECRATION          38385

#define NPC_THRALL                  17876
#define C_WARDEN                    17833
#define C_VETERAN                   17860
#define SKARLOC_MOUNT               18798
#define SKARLOC_MOUNT_MODEL         18223

struct boss_captain_skarlocAI : public ScriptedAI
{
    boss_captain_skarlocAI(Creature *creature) : ScriptedAI(creature)
    {
        pInstance = (ScriptedInstance*)creature->GetInstanceData();
        HeroicMode = me->GetMap()->IsHeroic();
    }

    ScriptedInstance *pInstance;

    bool HeroicMode;
    bool Intro;

    uint8 Next;
    uint32 IntroTimer;
    uint32 Holy_Light_Timer;
    uint32 Cleanse_Timer;
    uint32 HammerOfJustice_Timer;
    uint32 HolyShield_Timer;
    uint32 DevotionAura_Timer;
    uint32 Consecration_Timer;
    uint64 Add1GUID;
    uint64 Add2GUID;
    uint64 ThrallinGUID;

    void Reset()
    {
        Intro = true;
        Next = 0;
        Add1GUID = 0;
        Add2GUID = 0;
        IntroTimer = 20000;
        Holy_Light_Timer = 30000;
        Cleanse_Timer = 10000;
        HammerOfJustice_Timer = 60000;
        HolyShield_Timer = 240000;
        DevotionAura_Timer = 3000;
        Consecration_Timer = 8000;
        me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->Mount(SKARLOC_MOUNT_MODEL);
        SummonGuards();
        me->GetMotionMaster()->MovePoint(0, 2047.90f, 254.85f, 62.822f);

        if (Creature *Thrall = (Creature*)(Unit::GetUnit((*me), pInstance->GetData64(DATA_THRALL))))
            ThrallinGUID = Thrall->GetGUID();
        else if (Creature* Thrall = me->FindNearestCreature(17876, 100.0f, true)) 
            ThrallinGUID = Thrall->GetGUID();

    }

    void SummonGuards()
    {
        if (Creature *tAdd1 = DoSpawnCreature( C_WARDEN, -2, -2, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000))
        {
            Add1GUID = tAdd1->GetGUID();
            tAdd1->SetReactState(REACT_PASSIVE);
            tAdd1->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            tAdd1->GetMotionMaster()->MovePoint(0, 2044.12f, 253.47f, 62.748f);
        }

        if (Creature *tAdd2 = DoSpawnCreature( C_VETERAN, 2, 2, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000))
        {
            Add2GUID = tAdd2->GetGUID();
            tAdd2->SetReactState(REACT_PASSIVE);
            tAdd2->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            tAdd2->GetMotionMaster()->MovePoint(0, 2049.22f, 258.16f, 62.754f);
        }
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type == POINT_MOTION_TYPE)
        {
            switch (Next)
            {
                case 0:
                    me->RemoveAurasByType(SPELL_AURA_MOUNTED);

                    if (Creature* Thrall = me->GetMap()->GetCreature(ThrallinGUID))
                        Thrall->SummonCreature(SKARLOC_MOUNT,2049.90f, 256.85f, 62.822f, me->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 5000);
                            
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(0, 2056.80f, 240.81f, 63.538f);

                    if (Creature* tAdd1 = me->GetMap()->GetCreature(Add1GUID))
                    {
                        tAdd1->SetWalk(true);
                        tAdd1->GetMotionMaster()->MovePoint(0, 2052.94f, 239.37f, 63.219f);
                    }

                    if (Creature* tAdd2 = me->GetMap()->GetCreature(Add2GUID))
                    {
                        tAdd2->SetWalk(true);
                        tAdd2->GetMotionMaster()->MovePoint(0, 2058.66f, 243.99f, 63.59f);
                    }

                    ++Next;
                    break;
                case 1:
                    DoScriptText(SAY_ENTER, me);
                    ++Next;
                    break;
            }
        }
    }

    void IntroEnd()
    {
        me->SetReactState(REACT_AGGRESSIVE);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->Dismount();
        me->SetWalk(false);

        if (Creature* Thrall = me->GetMap()->GetCreature(ThrallinGUID))
        {
            me->AI()->AttackStart(Thrall);

            if (Creature* tAdd1 = me->GetMap()->GetCreature(Add1GUID))
            {
                tAdd1->SetReactState(REACT_AGGRESSIVE);
                tAdd1->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                tAdd1->SetWalk(false);
                tAdd1->AI()->AttackStart(Thrall);
            }

            if (Creature* tAdd2 = me->GetMap()->GetCreature(Add2GUID))
            {
                tAdd2->SetReactState(REACT_AGGRESSIVE);
                tAdd2->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                tAdd2->SetWalk(false);
                tAdd2->AI()->AttackStart(Thrall);
            }
        }
    }

    void EnterCombat(Unit *who)
    {

        DoScriptText(SAY_TAUNT1, me);
        DoScriptText(SAY_TAUNT2, me);
    }

    void KilledUnit(Unit *victim)
    {
        DoScriptText(RAND(SAY_SLAY1, SAY_SLAY2), me);
    }

    void EnterEvadeMode()
    {
        me->InterruptNonMeleeSpells(true);
        me->RemoveAllAuras();
        me->DeleteThreatList();
        me->CombatStop(true);

        Map* tmpMap = me->GetMap();

        if (!tmpMap)
            return;

        if (Creature *mount = (Creature*)(Unit::GetUnit((*me), pInstance->GetData64(DATA_SKARLOC_MOUNT))))
            mount->ForcedDespawn();
    }

    void JustDied(Unit *victim)
    {
        DoScriptText(SAY_DEATH, me);

        if (pInstance->GetData(TYPE_THRALL_EVENT) == IN_PROGRESS)
            pInstance->SetData(TYPE_THRALL_PART1, DONE);

        if (pInstance->GetData(DATA_SKARLOC_DEATH) == DONE)
            me->SetLootRecipient(NULL);
        else
            pInstance->SetData(DATA_SKARLOC_DEATH, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (Intro)
        {
            if (IntroTimer < diff)
            {
                IntroEnd();
                Intro = false;
            }
            else
                IntroTimer -= diff;
        }

        //Return since we have no target
        if (!UpdateVictim() )
            return;

        //Holy_Light
        if (Holy_Light_Timer < diff)
        {
            DoCast(me, SPELL_HOLY_LIGHT);
            Holy_Light_Timer = 30000;
        }
        else
            Holy_Light_Timer -= diff;

        //Cleanse
        if(Cleanse_Timer  < diff)
        {
            DoCast(me, SPELL_CLEANSE);
            Cleanse_Timer = 10000;
        }
        else
            Cleanse_Timer -= diff;

        //Hammer of Justice
        if (HammerOfJustice_Timer < diff)
        {
            DoCast(me->getVictim(), SPELL_HAMMER_OF_JUSTICE);
            HammerOfJustice_Timer = 60000;
        }
        else
            HammerOfJustice_Timer -= diff;

        //Holy Shield
        if(HolyShield_Timer < diff)
        {
            DoCast(me, SPELL_HOLY_SHIELD);
            HolyShield_Timer = 240000;
        }
        else
            HolyShield_Timer -= diff;

        //Devotion_Aura
        if (DevotionAura_Timer < diff)
        {
            DoCast(me, SPELL_DEVOTION_AURA);
            DevotionAura_Timer = 60000;
        }
        else
            DevotionAura_Timer -= diff;

        if(HeroicMode)
        if(Consecration_Timer < diff)
        {
            DoCast(me, SPELL_CONSECRATION);
            Consecration_Timer = 8000;
        }
        else
            Consecration_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_captain_skarloc(Creature *creature)
{
    return new boss_captain_skarlocAI (creature);
}

void AddSC_boss_captain_skarloc()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name="boss_captain_skarloc";
    newscript->GetAI = &GetAI_boss_captain_skarloc;
    newscript->RegisterSelf();
}

