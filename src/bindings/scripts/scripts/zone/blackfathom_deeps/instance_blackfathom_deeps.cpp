/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Blackfathom_Deeps
SD%Complete: 75%
SDComment: Consider it temporary, no instance save yet.
SDCategory: Blackfathom Deeps
EndScriptData */

#include "precompiled.h"
#include "def_blackfathom_deeps.h"

#define ENCOUNTERS 4

class OREGON_DLL_SPEC instance_blackfathom_deeps : public ScriptedInstance
{
public:

    instance_blackfathom_deeps(Map *map) : ScriptedInstance(map) {Initialize();}

    uint32 CandlesLighted;

    uint64 AkumaiDoorGUID;

    void Initialize()
    {

        CandlesLighted = 0;
        AkumaiDoorGUID = 0;

    }

    Player* GetPlayerInMap()
    {
        Map::PlayerList const& players = instance->GetPlayers();

        if (!players.isEmpty())
        {
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                if (Player* plr = itr->getSource())
                    return plr;
            }
        }

        debug_log("TSCR: Instance Black Temple: GetPlayerInMap, but PlayerList is empty!");
        return NULL;
    }

    void OnObjectCreate(GameObject* go)
    {
        switch(go->GetEntry())
        {
            case 21117:
                AkumaiDoorGUID = go->GetGUID();
                break;
        }
    }

    void Update(uint32 diff)
    {

    }

    bool IsEncouterInProgress() const
    {
        return false;
    };

    uint32 GetData(uint32 type)
    {
        if (type == DATA_CANDLES)
            return CandlesLighted;

        return 0;
    }

    uint64 GetData64(uint32 identifier)
    {
        return 0;
    }

    void SetData(uint32 type, uint32 data)
    {
        switch(type)
        {
            case DATA_CANDLES:
                CandlesLighted = data;

                if (CandlesLighted == 4)
                   HandleGameObject(AkumaiDoorGUID,true);

                break;
        }
    }


};

InstanceData* GetInstance_instance_blackfathom_deeps(Map *_Map)
{
    return new instance_blackfathom_deeps(_Map);
}

void AddSC_instance_blackfathom_deeps()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="instance_blackfathom_deeps";
    newscript->GetInstanceData = &GetInstance_instance_blackfathom_deeps;
    newscript->RegisterSelf();
}

