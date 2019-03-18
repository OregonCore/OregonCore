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
 * with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/* ScriptData
SDName: Instance_Sunken_Temple
SD%Complete: 100
SDComment:Place Holder
SDCategory: Sunken Temple
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "sunken_temple.h"

#define GO_ATALAI_STATUE1 148830
#define GO_ATALAI_STATUE2 148831
#define GO_ATALAI_STATUE3 148832
#define GO_ATALAI_STATUE4 148833
#define GO_ATALAI_STATUE5 148834
#define GO_ATALAI_STATUE6 148835
#define GO_ATALAI_IDOL 148836

#define GO_ATALAI_LIGHT1 148883
#define GO_ATALAI_LIGHT2 148937

#define NPC_MALFURION_STORMRAGE 15362

struct instance_sunken_temple : public ScriptedInstance
{
    instance_sunken_temple(Map* pMap) : ScriptedInstance(pMap)
    {
        Initialize();
    };

    uint64 GOAtalaiStatue1;
    uint64 GOAtalaiStatue2;
    uint64 GOAtalaiStatue3;
    uint64 GOAtalaiStatue4;
    uint64 GOAtalaiStatue5;
    uint64 GOAtalaiStatue6;
    uint64 GOAtalaiIdol;

    uint32 State;

    bool s1;
    bool s2;
    bool s3;
    bool s4;
    bool s5;
    bool s6;

    void Initialize()
    {
        GOAtalaiStatue1 = 0;
        GOAtalaiStatue2 = 0;
        GOAtalaiStatue3 = 0;
        GOAtalaiStatue4 = 0;
        GOAtalaiStatue5 = 0;
        GOAtalaiStatue6 = 0;
        GOAtalaiIdol = 0;

        State = 0;

        s1 = false;
        s2 = false;
        s3 = false;
        s4 = false;
        s5 = false;
        s6 = false;
    }

    void OnGameObjectCreate(GameObject* pGo, bool /*add*/)
    {
        switch (pGo->GetEntry())
        {
        case GO_ATALAI_STATUE1:
            GOAtalaiStatue1 = pGo->GetGUID();
            break;
        case GO_ATALAI_STATUE2:
            GOAtalaiStatue2 = pGo->GetGUID();
            break;
        case GO_ATALAI_STATUE3:
            GOAtalaiStatue3 = pGo->GetGUID();
            break;
        case GO_ATALAI_STATUE4:
            GOAtalaiStatue4 = pGo->GetGUID();
            break;
        case GO_ATALAI_STATUE5:
            GOAtalaiStatue5 = pGo->GetGUID();
            break;
        case GO_ATALAI_STATUE6:
            GOAtalaiStatue6 = pGo->GetGUID();
            break;
        case GO_ATALAI_IDOL:
            GOAtalaiIdol = pGo->GetGUID();
            break;
        }
    }

    virtual void Update(uint32 /*diff*/) // correct order goes form 1-6
    {
        switch (State)
        {
        case GO_ATALAI_STATUE1:
            if (!s1 && !s2 && !s3 && !s4 && !s5 && !s6)
            {
                if (GameObject* pAtalaiStatue1 = instance->GetGameObject(GOAtalaiStatue1))
                    UseStatue(pAtalaiStatue1);
                s1 = true;
                State = 0;
            };
            break;
        case GO_ATALAI_STATUE2:
            if (s1 && !s2 && !s3 && !s4 && !s5 && !s6)
            {
                if (GameObject* pAtalaiStatue2 = instance->GetGameObject(GOAtalaiStatue2))
                    UseStatue(pAtalaiStatue2);
                s2 = true;
                State = 0;
            };
            break;
        case GO_ATALAI_STATUE3:
            if (s1 && s2 && !s3 && !s4 && !s5 && !s6)
            {
                if (GameObject* pAtalaiStatue3 = instance->GetGameObject(GOAtalaiStatue3))
                    UseStatue(pAtalaiStatue3);
                s3 = true;
                State = 0;
            };
            break;
        case GO_ATALAI_STATUE4:
            if (s1 && s2 && s3 && !s4 && !s5 && !s6)
            {
                if (GameObject* pAtalaiStatue4 = instance->GetGameObject(GOAtalaiStatue4))
                    UseStatue(pAtalaiStatue4);
                s4 = true;
                State = 0;
            }
            break;
        case GO_ATALAI_STATUE5:
            if (s1 && s2 && s3 && s4 && !s5 && !s6)
            {
                if (GameObject* pAtalaiStatue5 = instance->GetGameObject(GOAtalaiStatue5))
                    UseStatue(pAtalaiStatue5);
                s5 = true;
                State = 0;
            }
            break;
        case GO_ATALAI_STATUE6:
            if (s1 && s2 && s3 && s4 && s5 && !s6)
            {
				if (GameObject* pAtalaiStatue6 = instance->GetGameObject(GOAtalaiStatue6))
				{
					UseStatue(pAtalaiStatue6);
					UseLastStatue();
					s6 = true;
					State = 0;
				}
            }
            break;
        }
    };

    void UseStatue(GameObject* pGo)
    {
        pGo->SummonGameObject(GO_ATALAI_LIGHT1, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), 0, 0, 0, 0, 0, 0);
        pGo->SetUInt32Value(GAMEOBJECT_FLAGS, 4);
    }
   
    void UseLastStatue()
    {	
		if (GameObject* pAtalaiStatue6 = instance->GetGameObject(GOAtalaiStatue6))
		{
			pAtalaiStatue6->SummonGameObject(148838, -488.997f, 96.61f, -189.019f, -1.52f, 0, 0, 0, 0, 100000);
		}
    }
    
    void SetData(uint32 type, uint32 data)
    {
        if (type == EVENT_STATE)
            State = data;
    }

    uint32 GetData(uint32 type)
    {
        if (type == EVENT_STATE)
            return State;
        return 0;
    }
};

InstanceData* GetInstanceData_instance_sunken_temple(Map* pMap)
{
    return new instance_sunken_temple(pMap);
}

void AddSC_instance_sunken_temple()
{
    Script* newscript;
    newscript = new Script;
    newscript->Name = "instance_sunken_temple";
    newscript->GetInstanceData = &GetInstanceData_instance_sunken_temple;
    newscript->RegisterSelf();
}
