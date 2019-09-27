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
SDName: Boss_Heigan
SD%Complete: 0
SDComment: Place Holder
SDCategory: Naxxramas
EndScriptData */

//Lotheb or Heigan?
//8825 aggro1 - You are mine now!
//8826 aggro2 - I see you!
//8827 aggro3 - You...are next!
//8828 death -
//8829 slay - close your eyes... sleep
//8830 taunt1 - The races of the world will perish. It is only a matter of time.
//8831 taunt2 - I see endless suffering, I see torment, I see rage. I see... everything!
//8832 taunt3 - Soon... the world will tremble!
//8833 taunt4 - The end is upon you.
//8834 taunt5 - Hungry worms will feast on your rotten flesh!

#include "ScriptMgr.h"
#include "ScriptedCreature.h"

//Spell used by floor peices to cause damage to players
#define SPELL_ERUPTION      29371

//Spells by boss
#define SPELL_WILT          23772
#define SPELL_FEAVER        29998

//Spell by eye stalks
#define SPELL_MIND_FLAY     26143

