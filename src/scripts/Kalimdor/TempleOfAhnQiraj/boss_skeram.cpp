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
SDName: Boss_Skeram
SD%Complete: 75
SDComment: Mind Control buggy.
SDCategory: Temple of Ahn'Qiraj
EndScriptData */

#include "ScriptPCH.h"
#include "temple_of_ahnqiraj.h"
#include "Group.h"

#define SAY_AGGRO1                  -1531000
#define SAY_AGGRO2                  -1531001
#define SAY_AGGRO3                  -1531002
#define SAY_SLAY1                   -1531003
#define SAY_SLAY2                   -1531004
#define SAY_SLAY3                   -1531005
#define SAY_SPLIT                   -1531006
#define SAY_DEATH                   -1531007

#define SPELL_ARCANE_EXPLOSION      25679
#define SPELL_EARTH_SHOCK           26194
#define SPELL_TRUE_FULFILLMENT4     26526
#define SPELL_BLINK                 28391

#define PLACES_CLEANUP delete place1; \
  delete place2;                      \
  delete place3;                      \

class ov_mycoordinates
{
    public:
        float x,y,z,r;
        ov_mycoordinates(float cx, float cy, float cz, float cr)
        {
            x = cx; y = cy; z = cz; r = cr;
        }
};

struct boss_skeramAI : public ScriptedAI
{
    boss_skeramAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
        IsImage = false;
    }

    ScriptedInstance *pInstance;

    uint32 ArcaneExplosion_Timer;
    uint32 EarthShock_Timer;
    uint32 FullFillment_Timer;
    uint32 Blink_Timer;
    uint32 Invisible_Timer;

    Creature *Image1, *Image2;

    bool Images75;
    bool Images50;
    bool Images25;
    bool IsImage;
    bool Invisible;

    void Reset()
    {
        ArcaneExplosion_Timer = 6000 + rand()%6000;
        EarthShock_Timer = 2000;
        FullFillment_Timer = 15000;
        Blink_Timer = 8000 + rand()%12000;
        Invisible_Timer = 500;

        Images75 = false;
        Images50 = false;
        Images25 = false;
        Invisible = false;

        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetVisibility(VISIBILITY_ON);

        if (IsImage)
            me->setDeathState(JUST_DIED);
    }

    void KilledUnit(Unit* /*victim*/)
    {
        switch(rand()%3)
        {
        case 0: DoScriptText(SAY_SLAY1, me); break;
        case 1: DoScriptText(SAY_SLAY2, me); break;
        case 2: DoScriptText(SAY_SLAY3, me); break;
        }
    }

    void JustDied(Unit* /*Killer*/)
    {
        if (!IsImage)
            DoScriptText(SAY_DEATH, me);
    }

    void EnterCombat(Unit* /*who*/)
    {
        if (IsImage || Images75)
            return;
        switch(rand()%3)
        {
        case 0: DoScriptText(SAY_AGGRO1, me); break;
        case 1: DoScriptText(SAY_AGGRO2, me); break;
        case 2: DoScriptText(SAY_AGGRO3, me); break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!UpdateVictim())
            return;

        //ArcaneExplosion_Timer
        if (ArcaneExplosion_Timer <= diff)
        {
            DoCastVictim( SPELL_ARCANE_EXPLOSION);
            ArcaneExplosion_Timer = 8000 + rand()%10000;
        } else ArcaneExplosion_Timer -= diff;

        //If we are within range melee the target
        if (me->IsWithinMeleeRange(me->getVictim()))
        {
            //Make sure our attack is ready and we arn't currently casting
            if (me->isAttackReady() && !me->IsNonMeleeSpellCast(false))
            {
                me->AttackerStateUpdate(me->getVictim());
                me->resetAttackTimer();
            }
        } else
        {
            //EarthShock_Timer
            if (EarthShock_Timer <= diff)
            {
                DoCastVictim(SPELL_EARTH_SHOCK);
                EarthShock_Timer = 1000;
            } else EarthShock_Timer -= diff;
        }

        //Blink_Timer
        if (Blink_Timer <= diff)
        {
            //DoCast(me, SPELL_BLINK);
            switch(rand()%3)
            {
                case 0:
                    me->GetMap()->CreatureRelocation(me, -8340.782227f,2083.814453f,125.648788f,0.0f);
                    DoResetThreat();
                    break;
                case 1:
                    me->GetMap()->CreatureRelocation(me, -8341.546875f,2118.504639f,133.058151f,0.0f);
                    DoResetThreat();
                    break;
                case 2:
                    me->GetMap()->CreatureRelocation(me, -8318.822266f,2058.231201f,133.058151f,0.0f);
                    DoResetThreat();
                    break;
            }
            DoStopAttack();

            Blink_Timer= 20000 + rand()%20000;
        } else Blink_Timer -= diff;

        int procent = (int) (me->GetHealth()*100 / me->GetMaxHealth() +0.5f);

        //Summoning 2 Images and teleporting to a random position on 75% health
        if ((!Images75 && !IsImage) && (procent <= 75 && procent > 70))
            DoSplit(75);

        //Summoning 2 Images and teleporting to a random position on 50% health
        if ((!Images50 && !IsImage) &&
            (procent <= 50 && procent > 45))
            DoSplit(50);

        //Summoning 2 Images and teleporting to a random position on 25% health
        if ((!Images25 && !IsImage) && (procent <= 25 && procent > 20))
            DoSplit(25);

        //Invisible_Timer
        if (Invisible)
        {
            if (Invisible_Timer <= diff)
            {
                //Making Skeram visible after telporting
                me->SetVisibility(VISIBILITY_ON);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                Invisible_Timer = 2500;
                Invisible = false;
            } else Invisible_Timer -= diff;
        }

        DoMeleeAttackIfReady();
    }

    void DoSplit(int atPercent /* 75 50 25 */)
    {
        DoScriptText(SAY_SPLIT, me);

        ov_mycoordinates *place1 = new ov_mycoordinates(-8340.782227f,2083.814453f,125.648788f,0);
        ov_mycoordinates *place2 = new ov_mycoordinates(-8341.546875f,2118.504639f,133.058151f,0);
        ov_mycoordinates *place3 = new ov_mycoordinates(-8318.822266f,2058.231201f,133.058151f,0);

        ov_mycoordinates *bossc=place1, *i1=place2, *i2=place3;

        switch(rand()%3)
        {
            case 0:
                bossc=place1;
                i1=place2;
                i2=place3;
                break;
            case 1:
                bossc=place2;
                i1=place1;
                i2=place3;
                break;
            case 2:
                bossc=place3;
                i1=place1;
                i2=place2;
                break;
        }

        for (int tryi = 0; tryi < 41; tryi ++)
        {
            Unit *targetpl = SelectUnit(SELECT_TARGET_RANDOM, 0);
            if (targetpl->GetTypeId() == TYPEID_PLAYER)
            {
                Group *grp = ((Player *)targetpl)->GetGroup();
                if (grp)
                {
                    for (int ici = 0; ici < TARGETICONCOUNT; ici++)
                    {
                        //if (grp ->m_targetIcons[ici] == me->GetGUID()) -- private member:(
                        grp->SetTargetIcon(ici, 0);
                    }
                }
                break;
            }
        }

        me->RemoveAllAuras();
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetVisibility(VISIBILITY_OFF);
        me->GetMap()->CreatureRelocation(me, bossc->x, bossc->y, bossc->z, bossc->r);
        Invisible = true;
        DoResetThreat();
        DoStopAttack();

        switch (atPercent)
        {
            case 75: Images75 = true; break;
            case 50: Images50 = true; break;
            case 25: Images25 = true; break;
        }

        Unit *pTarget = SelectUnit(SELECT_TARGET_RANDOM,0);

        Image1 = me->SummonCreature(15263, i1->x, i1->y, i1->z, i1->r, TEMPSUMMON_CORPSE_DESPAWN, 30000);
        if (!Image1)
        {
          PLACES_CLEANUP
          return;
        }
        Image1->SetMaxHealth(me->GetMaxHealth() / 5);
        Image1->SetHealth(me->GetHealth() / 5);
        if (pTarget)
            Image1->AI()->AttackStart(pTarget);

        Image2 = me->SummonCreature(15263,i2->x, i2->y, i2->z, i2->r, TEMPSUMMON_CORPSE_DESPAWN, 30000);
        if (!Image2)
        {
          PLACES_CLEANUP
          return;
        }
        Image2->SetMaxHealth(me->GetMaxHealth() / 5);
        Image2->SetHealth(me->GetHealth() / 5);
        if (pTarget)
            Image2->AI()->AttackStart(pTarget);

        ((boss_skeramAI*)Image1->AI())->IsImage = true;
        ((boss_skeramAI*)Image2->AI())->IsImage = true;

        Invisible = true;
    PLACES_CLEANUP
    }

};

CreatureAI* GetAI_boss_skeram(Creature* pCreature)
{
    return new boss_skeramAI (pCreature);
}

void AddSC_boss_skeram()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_skeram";
    newscript->GetAI = &GetAI_boss_skeram;
    newscript->RegisterSelf();
}

