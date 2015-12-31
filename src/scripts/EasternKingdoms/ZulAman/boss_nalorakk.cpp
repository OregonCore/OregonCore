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
SDName: Boss_Nalorakk
SD%Complete: 100
SDComment:
SDCategory: Zul'Aman
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "zulaman.h"
#include "GridNotifiers.h"

// Trash Waves
float NalorakkWay[8][3] =
{
    { 18.569f, 1414.512f, 11.42f},// waypoint 1
    { -17.264f, 1419.551f, 12.62f},
    { -52.642f, 1419.357f, 27.31f}, // waypoint 2
    { -69.908f, 1419.721f, 27.31f},
    { -79.929f, 1395.958f, 27.31f},
    { -80.072f, 1374.555f, 40.87f}, // waypoint 3
    { -80.072f, 1314.398f, 40.87f},
    { -80.072f, 1295.775f, 48.60f} // waypoint 4
};

enum // Sounds
{
    SOUND_NALORAKK_WAVE1 = 12066,
    SOUND_NALORAKK_WAVE2 = 12067,
    SOUND_NALORAKK_WAVE3 = 12068,
    SOUND_NALORAKK_WAVE4 = 12069,

    SOUND_YELL_AGGRO     = 12070,
    SOUND_YELL_KILL_ONE  = 12075,
    SOUND_YELL_KILL_TWO  = 12076,
    SOUND_YELL_DEATH     = 12077,
    SOUND_YELL_BERSERK   = 12074,

    SOUND_YELL_TOBEAR    = 12072,
    SOUND_YELL_SURGE     = 12071,
    SOUND_YELL_TOTROLL   = 12073,

    // UnImplemented SoundIDs
    SOUND_NALORAKK_EVENT1 = 12078, // "What could be better than servin' da bear spirit for eternity? Come closer now. Bring your souls to me!"
    SOUND_NALORAKK_EVENT2 = 12079  // "Don't be delayin' your fate. Come to me now. I make your sacrifice quick. "
};

static const char YELL_NALORAKK_WAVE1[] = "Get da move on, guards! It be killin' time!";
static const char YELL_NALORAKK_WAVE2[] = "Guards, go already! Who you more afraid of, dem... or me?";
static const char YELL_NALORAKK_WAVE3[] = "Ride now! Ride out dere and bring me back some heads!";
static const char YELL_NALORAKK_WAVE4[] = "I be losin' me patience! Go on: make dem wish dey was never born!";

static const char YELL_AGGRO[]          = "You be dead soon enough!";
static const char YELL_KILL_ONE[]       = "Mua-ha-ha! Now whatchoo got to say?";
static const char YELL_KILL_TWO[]       = "Da Amani gonna rule again!";
static const char YELL_DEATH[]          = "I... be waitin' on da udda side....";
static const char YELL_BERSERK[]        = "You had your chance, now it be too late!"; //Never seen this being used, so just guessing from what I hear.

static const char YELL_SURGE[]          = "I bring da pain!";
static const char YELL_SHIFTEDTOTROLL[] = "Make way for Nalorakk!";
static const char YELL_SHIFTEDTOBEAR[]  = "You call on da beast, you gonna get more dan you bargain for!";

enum // Spells
{
    SPELL_BERSERK         = 45078,

    // Spells for Troll form
    SPELL_BRUTALSWIPE     = 42384,
    SPELL_MANGLE          = 42389,
    SPELL_MANGLEEFFECT    = 44955,
    SPELL_SURGE           = 42402,
    SPELL_BEARFORM        = 42377,

    // Spells for Bear form
    SPELL_LACERATINGSLASH = 42395,
    SPELL_RENDFLESH       = 42397,
    SPELL_DEAFENINGROAR   = 42398
};

struct boss_nalorakkAI : public ScriptedAI
{
    boss_nalorakkAI(Creature* c) : ScriptedAI(c)
    {
        MoveEvent = true;
        MovePhase = 0;
        pInstance = (ScriptedInstance*)c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint32 BrutalSwipe_Timer;
    uint32 Mangle_Timer;
    uint32 Surge_Timer;

    uint32 LaceratingSlash_Timer;
    uint32 RendFlesh_Timer;
    uint32 DeafeningRoar_Timer;

    uint32 ShapeShift_Timer;
    uint32 Berserk_Timer;

    bool inBearForm;
    bool MoveEvent;
    bool inMove;
    uint32 MovePhase;
    uint32 waitTimer;

    void Reset()
    {
        if (MoveEvent)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            inMove = false;
            waitTimer = 0;
            me->SetSpeed(MOVE_RUN, 2);
            me->SetWalk(false);
        }
        else
            (*me).GetMotionMaster()->MovePoint(0, NalorakkWay[7][0], NalorakkWay[7][1], NalorakkWay[7][2]);

        if (pInstance)
            pInstance->SetData(ENCOUNTER_NALORAKK, NOT_STARTED);

        Surge_Timer = 15000 + rand() % 5000;
        BrutalSwipe_Timer = 7000 + rand() % 5000;
        Mangle_Timer = 10000 + rand() % 5000;
        ShapeShift_Timer = 45000 + rand() % 5000;
        Berserk_Timer = 600000;

        inBearForm = false;
        me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + 1, 5122);
    }

    void SendAttacker(Unit* pTarget)
    {
        std::list<Creature*> templist;
        float x, y, z;
        me->GetPosition(x, y, z);

        {
            CellCoord pair(Oregon::ComputeCellCoord(x, y));
            Cell cell(pair);
            cell.SetNoCreate();

            Oregon::AllFriendlyCreaturesInGrid check(me);
            Oregon::CreatureListSearcher<Oregon::AllFriendlyCreaturesInGrid> searcher(templist, check);
            TypeContainerVisitor<Oregon::CreatureListSearcher<Oregon::AllFriendlyCreaturesInGrid>, GridTypeMapContainer> cSearcher(searcher);
            cell.Visit(pair, cSearcher, *(me->GetMap()), *me, me->GetGridActivationRange());

        }

        if (!templist.size())
            return;

        for (std::list<Creature*>::const_iterator i = templist.begin(); i != templist.end(); ++i)
        {
            if ((*i) && me->IsWithinDistInMap((*i), 25))
            {
                (*i)->SetNoCallAssistance(true);
                (*i)->AI()->AttackStart(pTarget);
            }
        }
    }

    void AttackStart(Unit* who)
    {
        if (!MoveEvent)
            ScriptedAI::AttackStart(who);
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (!MoveEvent)
            ScriptedAI::MoveInLineOfSight(who);
        else
        {
            if (me->IsHostileTo(who))
            {
                if (!inMove)
                {
                    switch (MovePhase)
                    {
                    case 0:
                        if (me->IsWithinDistInMap(who, 50))
                        {
                            me->MonsterYell(YELL_NALORAKK_WAVE1, LANG_UNIVERSAL, 0);
                            DoPlaySoundToSet(me, SOUND_NALORAKK_WAVE1);

                            (*me).GetMotionMaster()->MovePoint(1, NalorakkWay[1][0], NalorakkWay[1][1], NalorakkWay[1][2]);
                            MovePhase ++;
                            inMove = true;

                            SendAttacker(who);
                        }
                        break;
                    case 2:
                        if (me->IsWithinDistInMap(who, 40))
                        {
                            me->MonsterYell(YELL_NALORAKK_WAVE2, LANG_UNIVERSAL, 0);
                            DoPlaySoundToSet(me, SOUND_NALORAKK_WAVE2);

                            (*me).GetMotionMaster()->MovePoint(3, NalorakkWay[3][0], NalorakkWay[3][1], NalorakkWay[3][2]);
                            MovePhase ++;
                            inMove = true;

                            SendAttacker(who);
                        }
                        break;
                    case 5:
                        if (me->IsWithinDistInMap(who, 40))
                        {
                            me->MonsterYell(YELL_NALORAKK_WAVE3, LANG_UNIVERSAL, 0);
                            DoPlaySoundToSet(me, SOUND_NALORAKK_WAVE3);

                            (*me).GetMotionMaster()->MovePoint(6, NalorakkWay[6][0], NalorakkWay[6][1], NalorakkWay[6][2]);
                            MovePhase ++;
                            inMove = true;

                            SendAttacker(who);
                        }
                        break;
                    case 7:
                        if (me->IsWithinDistInMap(who, 50))
                        {
                            SendAttacker(who);

                            me->MonsterYell(YELL_NALORAKK_WAVE4, LANG_UNIVERSAL, 0);
                            DoPlaySoundToSet(me, SOUND_NALORAKK_WAVE4);

                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                            MoveEvent = false;
                        }
                        break;
                    }
                }
            }
        }
    }

    void EnterCombat(Unit* /*who*/)
    {
        if (pInstance)
            pInstance->SetData(ENCOUNTER_NALORAKK, IN_PROGRESS);

        me->MonsterYell(YELL_AGGRO, LANG_UNIVERSAL, 0);
        DoPlaySoundToSet(me, SOUND_YELL_AGGRO);
        DoZoneInCombat();
    }

    void JustDied(Unit* /*Killer*/)
    {
        if (pInstance)
            pInstance->SetData(ENCOUNTER_NALORAKK, DONE);

        me->MonsterYell(YELL_DEATH, LANG_UNIVERSAL, 0);
        DoPlaySoundToSet(me, SOUND_YELL_DEATH);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        switch (urand(0, 1))
        {
        case 0:
            me->MonsterYell(YELL_KILL_ONE, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, SOUND_YELL_KILL_ONE);
            break;
        case 1:
            me->MonsterYell(YELL_KILL_TWO, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, SOUND_YELL_KILL_TWO);
            break;
        }
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (MoveEvent)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (!inMove)
                return;

            if (MovePhase != id)
                return;

            switch (MovePhase)
            {
            case 2:
                me->SetOrientation(3.1415f * 2);
                inMove = false;
                return;
            case 1:
            case 3:
            case 4:
            case 6:
                MovePhase ++;
                waitTimer = 1;
                inMove = true;
                return;
            case 5:
                me->SetOrientation(3.1415f * 0.5f);
                inMove = false;
                return;
            case 7:
                me->SetOrientation(3.1415f * 0.5f);
                inMove = false;
                return;
            }

        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (waitTimer)
        {
            if (inMove)
            {
                if (waitTimer <= diff)
                {
                    (*me).GetMotionMaster()->MovementExpired();
                    (*me).GetMotionMaster()->MovePoint(MovePhase, NalorakkWay[MovePhase][0], NalorakkWay[MovePhase][1], NalorakkWay[MovePhase][2]);
                    waitTimer = 0;
                }
                else waitTimer -= diff;
            }
        }

        if (!UpdateVictim())
            return;

        if (Berserk_Timer <= diff)
        {
            DoCast(me, SPELL_BERSERK, true);
            me->MonsterYell(YELL_BERSERK, LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me, SOUND_YELL_BERSERK);
            Berserk_Timer = 600000;
        }
        else
            Berserk_Timer -= diff;

        if (ShapeShift_Timer <= diff)
        {
            if (inBearForm)
            {
                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + 1, 5122);
                me->MonsterYell(YELL_SHIFTEDTOTROLL, LANG_UNIVERSAL, 0);
                DoPlaySoundToSet(me, SOUND_YELL_TOTROLL);
                me->RemoveAurasDueToSpell(SPELL_BEARFORM);
                Surge_Timer = 15000 + rand() % 5000;
                BrutalSwipe_Timer = 7000 + rand() % 5000;
                Mangle_Timer = 10000 + rand() % 5000;
                ShapeShift_Timer = 45000 + rand() % 5000;
                inBearForm = false;
            }
            else
            {
                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_DISPLAY + 1, 0);
                me->MonsterYell(YELL_SHIFTEDTOBEAR, LANG_UNIVERSAL, 0);
                DoPlaySoundToSet(me, SOUND_YELL_TOBEAR);
                DoCast(me, SPELL_BEARFORM, true);
                LaceratingSlash_Timer = 2000; // dur 18s
                RendFlesh_Timer = 3000;  // dur 5s
                DeafeningRoar_Timer = 5000 + rand() % 5000; // dur 2s
                ShapeShift_Timer = 20000 + rand() % 5000; // dur 30s
                inBearForm = true;
            }
        }
        else
            ShapeShift_Timer -= diff;

        if (!inBearForm)
        {
            if (BrutalSwipe_Timer <= diff)
            {
                DoCastVictim( SPELL_BRUTALSWIPE);
                BrutalSwipe_Timer = 7000 + rand() % 5000;
            }
            else
                BrutalSwipe_Timer -= diff;

            if (Mangle_Timer <= diff)
            {
                if (me->getVictim() && !me->getVictim()->HasAura(SPELL_MANGLEEFFECT, 0))
                {
                    DoCastVictim( SPELL_MANGLE);
                    Mangle_Timer = 1000;
                }
                else Mangle_Timer = 10000 + rand() % 5000;
            }
            else
                Mangle_Timer -= diff;

            if (Surge_Timer <= diff)
            {
                me->MonsterYell(YELL_SURGE, LANG_UNIVERSAL, 0);
                DoPlaySoundToSet(me, SOUND_YELL_SURGE);
                Unit* pTarget = SelectTarget(SELECT_TARGET_RANDOM, 1, 45, true);
                if (pTarget)
                    DoCast(pTarget, SPELL_SURGE);
                Surge_Timer = 15000 + rand() % 5000;
            }
            else
                Surge_Timer -= diff;
        }
        else
        {
            if (LaceratingSlash_Timer <= diff)
            {
                DoCastVictim( SPELL_LACERATINGSLASH);
                LaceratingSlash_Timer = 18000 + rand() % 5000;
            }
            else
                LaceratingSlash_Timer -= diff;

            if (RendFlesh_Timer <= diff)
            {
                DoCastVictim( SPELL_RENDFLESH);
                RendFlesh_Timer = 5000 + rand() % 5000;
            }
            else
                RendFlesh_Timer -= diff;

            if (DeafeningRoar_Timer <= diff)
            {
                DoCastVictim( SPELL_DEAFENINGROAR);
                DeafeningRoar_Timer = 15000 + rand() % 5000;
            }
            else
                DeafeningRoar_Timer -= diff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_nalorakk(Creature* pCreature)
{
    return new boss_nalorakkAI (pCreature);
}

void AddSC_boss_nalorakk()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "boss_nalorakk";
    newscript->GetAI = &GetAI_boss_nalorakk;
    newscript->RegisterSelf();
}

