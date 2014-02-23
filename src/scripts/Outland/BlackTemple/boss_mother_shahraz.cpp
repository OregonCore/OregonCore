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
SDName: Boss_Mother_Shahraz
SD%Complete: 80
SDComment: Saber Lash missing, Fatal Attraction slightly incorrect; need to damage only if affected players are within range of each other
SDCategory: Black Temple
EndScriptData */

#include "ScriptPCH.h"
#include "black_temple.h"

//Speech'n'Sounds
#define SAY_TAUNT1                  -1564018
#define SAY_TAUNT2                  -1564019
#define SAY_TAUNT3                  -1564020
#define SAY_AGGRO                   -1564021
#define SAY_SPELL1                  -1564022
#define SAY_SPELL2                  -1564023
#define SAY_SPELL3                  -1564024
#define SAY_SLAY1                   -1564025
#define SAY_SLAY2                   -1564026
#define SAY_ENRAGE                  -1564027
#define SAY_DEATH                   -1564028

//Spells
#define SPELL_BEAM_SINISTER         40859
#define SPELL_BEAM_SINISTER_TRIGGER 40863
#define SPELL_BEAM_VILE             40860
#define SPELL_BEAM_VILE_TRIGGER     40865
#define SPELL_BEAM_WICKED           40861
#define SPELL_BEAM_WICKED_TRIGGER   40866
#define SPELL_BEAM_SINFUL           40827
#define SPELL_BEAM_SINFUL_TRIGGER   40862
#define SPELL_ATTRACTION            40871
#define SPELL_ATTRACTION_VIS        41001
#define SPELL_SILENCING_SHRIEK      40823
#define SPELL_ENRAGE                23537
#define SPELL_SABER_LASH            40810
#define SPELL_SABER_LASH_TRIGGER    40816
#define SPELL_SABER_LASH_IMM        43690
#define SPELL_TELEPORT_VISUAL       40869
#define SPELL_BERSERK               45078

#define SPELL_PRISMATIC_SHIELD      40879

struct Locations
{
    float x,y,z;
};

static Locations TeleportPoint[]=
{
    {959.996f, 212.576f, 193.843f},
    {932.537f, 231.813f, 193.838f},
    {958.675f, 254.767f, 193.822f},
    {946.955f, 201.316f, 192.535f},
    {944.294f, 149.676f, 197.551f},
    {930.548f, 284.888f, 193.367f},
    {965.997f, 278.398f, 195.777f}
};

struct boss_shahrazAI : public ScriptedAI
{
    boss_shahrazAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = c->GetInstanceData();
    }

    ScriptedInstance* pInstance;

    uint64 TargetGUID[3];
    uint32 BeamTimer;
    uint32 BeamCount;
    uint32 CurrentBeam;
    uint32 FatalAttractionTimer;
    uint32 FatalAttractionExplodeTimer;
    uint32 ShriekTimer;
    uint32 SaberTimer;
    uint32 RandomYellTimer;
    uint32 EnrageTimer;

    bool Enraged;

    void Reset()
    {
        if (pInstance)
            pInstance->SetData(DATA_MOTHERSHAHRAZEVENT, NOT_STARTED);

        for (uint8 i = 0; i<3; ++i)
            TargetGUID[i] = 0;

        BeamTimer = 5000; // Timers may be incorrect
        //BeamCount = 1;
        CurrentBeam = rand()%4;                                    // 0 - Sinister, 1 - Vile, 2 - Wicked, 3 - Sinful
        FatalAttractionTimer = 60000;
        FatalAttractionExplodeTimer = 70000;
        ShriekTimer = 20000;
        SaberTimer = 5000;
        RandomYellTimer = 70000 + rand()%41 * 1000;
        EnrageTimer = 600000;

        Enraged = false;
    }

    void EnterCombat(Unit * /*who*/)
    {
        if (pInstance)
            pInstance->SetData(DATA_MOTHERSHAHRAZEVENT, IN_PROGRESS);

        DoZoneInCombat();
        DoScriptText(SAY_AGGRO, me);
        DoCast(me,SPELL_PRISMATIC_SHIELD,true);
        DoCast(me,SPELL_SABER_LASH_TRIGGER,true);
    }

    void KilledUnit(Unit * /*victim*/)
    {
        DoScriptText(RAND(SAY_SLAY1,SAY_SLAY2), me);
    }

    void JustDied(Unit * /*victim*/)
    {
        if (pInstance)
            pInstance->SetData(DATA_MOTHERSHAHRAZEVENT, DONE);

        DoScriptText(SAY_DEATH, me);
    }

    void TeleportPlayers()
    {
        uint32 random = rand()%7;
        float X = TeleportPoint[random].x;
        float Y = TeleportPoint[random].y;
        float Z = TeleportPoint[random].z;
        for (uint8 i = 0; i < 3; ++i)
        {
            Unit* pUnit = SelectUnit(SELECT_TARGET_RANDOM, 1);
            if (pUnit && pUnit->isAlive() && (pUnit->GetTypeId() == TYPEID_PLAYER) && !pUnit->HasAura(SPELL_SABER_LASH_IMM,0))
            {
                TargetGUID[i] = pUnit->GetGUID();
                pUnit->CastSpell(pUnit, SPELL_TELEPORT_VISUAL, true);
                DoTeleportPlayer(pUnit, X, Y, Z, pUnit->GetOrientation());
            }
        }
    }

    bool TryDoCast(Unit *victim, uint32 spellId, bool triggered = false)
    {
        if (me->IsNonMeleeSpellCasted(false)) return false;

        DoCast(victim,spellId,triggered);
        return true;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        if (((me->GetHealth()*100 / me->GetMaxHealth()) < 10) && !Enraged)
        {
            Enraged = true;
            DoCast(me, SPELL_ENRAGE, true);
            DoScriptText(SAY_ENRAGE, me);
        }

        if (BeamTimer <= diff)
        {
            if (!me->IsNonMeleeSpellCasted(false))
            {
                switch(CurrentBeam)
                {
                    case 0:
                        DoCast(me, SPELL_BEAM_SINISTER_TRIGGER);
                        break;
                    case 1:
                        DoCast(me, SPELL_BEAM_VILE_TRIGGER);
                        break;
                    case 2:
                        DoCast(me, SPELL_BEAM_WICKED_TRIGGER);
                        break;
                    case 3:
                        DoCast(me, SPELL_BEAM_SINFUL_TRIGGER);
                        break;
                }
                uint32 Beam = CurrentBeam;
                while (CurrentBeam == Beam)
                    CurrentBeam = rand()%4;

                BeamTimer = 30000;
            }
        } else BeamTimer -= diff;

        // Select 3 random targets (can select same target more than once), teleport to a random location then make them cast explosions until they get away from each other.
        if (FatalAttractionTimer <= diff)
        {
            TeleportPlayers();

            DoScriptText(RAND(SAY_SPELL2,SAY_SPELL3), me);
            FatalAttractionExplodeTimer = 2000;
            FatalAttractionTimer = 30000;
        } else FatalAttractionTimer -= diff;

        if (FatalAttractionExplodeTimer <= diff)
        {
            Player* targets[3];
            for (uint8 i = 0; i < 3; ++i)
            {
                if (TargetGUID[i])
                    targets[i] = Player::GetPlayer(*me, TargetGUID[i]);
                else
                    targets[i] = NULL;
            }

            if (targets[0] && targets[0]->isAlive())
            {
                bool isNear = false;
                if (targets[1] && targets[1]->isAlive() && targets[0]->GetDistance2d(targets[1]) < 25)
                    isNear = true;

                if (!isNear)
                    if (targets[2] && targets[2]->isAlive() && targets[0]->GetDistance2d(targets[2]) < 25)
                        isNear = true;

                if (isNear)
                    targets[0]->CastSpell(targets[0],SPELL_ATTRACTION,true);
                else
                {
                    targets[0]->RemoveAurasDueToSpell(SPELL_ATTRACTION_VIS);
                    TargetGUID[0] = 0;
                    targets[0] = NULL;
                }
            }


            if (targets[1] && targets[1]->isAlive())
            {
                bool isNear = false;
                if (targets[0] && targets[0]->isAlive() && targets[1]->GetDistance2d(targets[0]) < 25)
                    isNear = true;

                if (!isNear)
                    if (targets[2] && targets[2]->isAlive() && targets[1]->GetDistance2d(targets[2]) < 25)
                        isNear = true;

                if (isNear)
                    targets[1]->CastSpell(targets[1],SPELL_ATTRACTION,true);
                else
                {
                    targets[1]->RemoveAurasDueToSpell(SPELL_ATTRACTION_VIS);
                    TargetGUID[1] = 0;
                    targets[1] = NULL;
                }
            }

            if (targets[2] && targets[2]->isAlive())
            {
                bool isNear = false;
                if (targets[0] && targets[0]->isAlive() && targets[2]->GetDistance2d(targets[0]) < 25)
                    isNear = true;

                if (!isNear)
                    if (targets[1] && targets[1]->isAlive() && targets[2]->GetDistance2d(targets[1]) < 25)
                        isNear = true;

                if (isNear)
                    targets[2]->CastSpell(targets[1],SPELL_ATTRACTION,true);
                else
                {
                    targets[2]->RemoveAurasDueToSpell(SPELL_ATTRACTION_VIS);
                    TargetGUID[2] = 0;
                    targets[2] = NULL;
                }
            }

            bool allClear = true;
            for (uint8 i = 0; i < 3; i++)
            {
                if (TargetGUID[i] != 0)
                    allClear = false;
            }

            if (allClear)
                FatalAttractionExplodeTimer = 60000;
            else
                FatalAttractionExplodeTimer = 1000;

        } else FatalAttractionExplodeTimer -= diff;

        if (ShriekTimer <= diff)
        {
            if (TryDoCast(me->getVictim(), SPELL_SILENCING_SHRIEK))
                ShriekTimer = 20000;
        } else ShriekTimer -= diff;

        //Enrage
        if (!me->HasAura(SPELL_BERSERK, 0))
        {
            if (EnrageTimer <= diff)
            {
                DoCast(me, SPELL_BERSERK);
                DoScriptText(SAY_ENRAGE, me);
            } else EnrageTimer -= diff;
        }
        //Random taunts
        if (RandomYellTimer <= diff)
        {
            DoScriptText(RAND(SAY_TAUNT1,SAY_TAUNT2,SAY_TAUNT3), me);
            RandomYellTimer = 60000 + rand()%91 * 1000;
        } else RandomYellTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_shahraz(Creature* pCreature)
{
    return new boss_shahrazAI (pCreature);
}

void AddSC_boss_mother_shahraz()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_mother_shahraz";
    newscript->GetAI = &GetAI_boss_shahraz;
    newscript->RegisterSelf();
}

