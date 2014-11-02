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

#include "ScriptPCH.h"
#include <cstring>

//This function is called when the player logs in (every login)
void OnLogin(Player* /*pPlayer*/)
{

}

//This function is called when the player logs out
void OnLogout(Player* /*pPlayer*/)
{

}

//This function is called when the player kills another player
void OnPVPKill(Player* /*killer*/, Player* /*killed*/)
{

}

void AddSC_onevents()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "scripted_on_events";
    newscript->pOnLogin = &OnLogin;
    newscript->pOnLogout = &OnLogout;
    newscript->pOnPVPKill = &OnPVPKill;

    newscript->RegisterSelf();
}
