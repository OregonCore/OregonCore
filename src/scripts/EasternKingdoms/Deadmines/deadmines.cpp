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
SDName: Deadmines
SD%Complete: 0
SDComment: Placeholder
SDCategory: Deadmines
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "deadmines.h"
#include "Spell.h"

/*#####
# item_Defias_Gunpowder
#####*/

bool ItemUse_item_defias_gunpowder(Player* pPlayer, Item* pItem, SpellCastTargets const& targets)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pPlayer->GetInstanceData();

    if (!pInstance)
    {
        pPlayer->GetSession()->SendNotification("Instance script not initialized");
        return true;
    }
    if (pInstance->GetData(EVENT_CANNON) != CANNON_NOT_USED)
        return false;
    if (targets.getGOTarget() && targets.getGOTarget()->GetTypeId() == TYPEID_GAMEOBJECT && targets.getGOTarget()->GetEntry() == GO_DEFIAS_CANNON)
        pInstance->SetData(EVENT_CANNON, CANNON_GUNPOWDER_USED);

    pPlayer->DestroyItemCount(pItem->GetEntry(), 1, true);
    return true;
}

void AddSC_deadmines()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "item_defias_gunpowder";
    newscript->pItemUse = &ItemUse_item_defias_gunpowder;
    newscript->RegisterSelf();
}
