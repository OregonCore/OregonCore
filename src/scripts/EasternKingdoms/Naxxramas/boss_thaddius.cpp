/* Copyright (C) 2006 - 2008 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/* ScriptData
SDName: Boss_Thaddius
SD%Complete: 0
SDComment: Merge Feugen & Stalagg with this script
SDCategory: Naxxramas
EndScriptData */

#include "ScriptPCH.h"

//Stalagg
#define SAY_STAL_AGGRO          -1533023
#define SAY_STAL_SLAY           -1533024
#define SAY_STAL_DEATH          -1533025

//Feugen
#define SAY_FEUG_AGGRO          -1533026
#define SAY_FEUG_SLAY           -1533027
#define SAY_FEUG_DEATH          -1533028

//Thaddus
#define SAY_GREET               -1533029
#define SAY_AGGRO1              -1533030
#define SAY_AGGRO2              -1533031
#define SAY_AGGRO3              -1533032
#define SAY_SLAY                -1533033
#define SAY_ELECT               -1533034
#define SAY_DEATH               -1533035
#define SAY_SCREAM1             -1533036
#define SAY_SCREAM2             -1533037
#define SAY_SCREAM3             -1533038
#define SAY_SCREAM4             -1533039


#define GO_TESLA_COIL1    181477
#define GO_TESLA_COIL2    181478    //those 2 are not spawned

enum eSpells
{
    // Fuegen
    FEUG_TESLA_PASSIVE            = 28109,
    SPELL_MANABURN                = 28135,

    // Stalagg
    STAL_TESLA_PASSIVE            = 28097,
    SPELL_POWERSURGE              = 28134,

    // shared
    SPELL_WARSTOMP                = 28125,
    SPELL_MAGNETIC_PULL           = 28337,

    // Thaddius
    SPELL_SELF_STUN               = 28160,
    SPELL_BALL_LIGHTNING          = 28299,
    SPELL_CHARGE_POSITIVE_DMGBUFF = 29659,
    SPELL_CHARGE_POSITIVE_NEARDMG = 28059,
    SPELL_CHARGE_NEGATIVE_DMGBUFF = 29660,
    SPELL_CHARGE_NEGATIVE_NEARDMG = 28084,
    SPELL_CHAIN_LIGHTNING         = 28167,
    SPELL_BESERK                  = 26662

};

struct boss_fuegenAI : public BossAI
{
    boss_fuegenAI(Creature *c): BossAI(c){}

    void Reset() {}

    void UpdateAI(uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

struct boss_stalaggAI : public BossAI
{
    boss_stalaggAI(Creature *c) : BossAI(c) {}

    void Reset() {}

    void UpdateAI(uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};

struct TRINITY_DLL_DECL boss_thaddiusAI : public BossAI
{
    boss_thaddiusAI(Creature *c) : BossAI(c) {}

    void Reset() {}

    void UpdateAI(uint32 diff)
    {
        if (!UpdateVictim())
            return;

        //CastNextSpellIfAnyAndReady();
        DoMeleeAttackIfReady();
    }
};
