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

#ifndef SC_SCRIPTMGR_H
#define SC_SCRIPTMGR_H

#include "Common.h"
#include "Platform/CompilerDefs.h"
#include "DBCStructure.h"

class Player;
class Creature;
class CreatureAI;
class InstanceData;
class Quest;
class Item;
class GameObject;
class SpellCastTargets;
class Map;
class Unit;
class WorldObject;
struct ItemPrototype;

#define MAX_SCRIPTS         5000                            //72 bytes each (approx 351kb)
#define VISIBLE_RANGE       (166.0f)                        //MAX visible range (size of grid)
#define DEFAULT_TEXT        "<Oregon Script Text Entry Missing!>"

struct Script
{
    Script() :
        pOnLogin(NULL), pOnLogout(NULL), pOnPVPKill(NULL),
        pGossipHello(NULL), pQuestAccept(NULL), pGossipSelect(NULL), pGossipSelectWithCode(NULL),
        pQuestSelect(NULL), pQuestComplete(NULL), pNPCDialogStatus(NULL), pGODialogStatus(NULL),
        pChooseReward(NULL), pItemHello(NULL), pGOHello(NULL), pAreaTrigger(NULL), pItemQuestAccept(NULL),
        pGOQuestAccept(NULL), pGOChooseReward(NULL), pItemUse(NULL), pEffectDummyCreature(NULL),
        GetAI(NULL), GetInstanceData(NULL)
    {}

    std::string Name;

    //Methods to be scripted
    void (*pOnLogin             )(Player*);
    void (*pOnLogout            )(Player*);
    void (*pOnPVPKill           )(Player*, Player*);
    bool (*pGossipHello         )(Player*, Creature*);
    bool (*pQuestAccept         )(Player*, Creature*, Quest const*);
    bool (*pGossipSelect        )(Player*, Creature*, uint32 , uint32);
    bool (*pGossipSelectWithCode)(Player*, Creature*, uint32 , uint32 , const char*);
    bool (*pGOSelect            )(Player*, GameObject*, uint32 , uint32);
    bool (*pGOSelectWithCode    )(Player*, GameObject*, uint32 , uint32 , const char*);
    bool (*pQuestSelect         )(Player*, Creature*, Quest const*);
    bool (*pQuestComplete       )(Player*, Creature*, Quest const*);
    uint32 (*pNPCDialogStatus   )(Player*, Creature*);
    uint32 (*pGODialogStatus    )(Player*, GameObject* _GO);
    bool (*pChooseReward        )(Player*, Creature*, Quest const*, uint32);
    bool (*pItemHello           )(Player*, Item*, Quest const*);
    bool (*pGOHello             )(Player*, GameObject*);
    bool (*pAreaTrigger         )(Player*, AreaTriggerEntry const*);
    bool (*pItemQuestAccept     )(Player*, Item* , Quest const*);
    bool (*pGOQuestAccept       )(Player*, GameObject*, Quest const*);
    bool (*pGOChooseReward      )(Player*, GameObject*, Quest const*, uint32);
    bool (*pItemUse             )(Player*, Item*, SpellCastTargets const&);
    bool (*pEffectDummyCreature )(Unit*, uint32, uint32, Creature*);

    CreatureAI* (*GetAI)(Creature*);
    InstanceData* (*GetInstanceData)(Map*);

    void RegisterSelf();
};

class ScriptMgr
{
    public:
        ScriptMgr();
        ~ScriptMgr();

        void ScriptsInit();
        void LoadDatabase();
        char const* ScriptsVersion();

    //event handlers
        void OnLogin(Player* pPlayer);
        void OnLogout(Player* pPlayer);
        void OnPVPKill(Player* killer, Player* killed);
        bool GossipHello (Player* pPlayer, Creature* pCreature);
        bool GossipSelect(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction);
        bool GossipSelectWithCode(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction, const char* sCode);
        bool GOSelect(Player* pPlayer, GameObject* pGO, uint32 uiSender, uint32 uiAction);
        bool GOSelectWithCode(Player* pPlayer, GameObject* pGO, uint32 uiSender, uint32 uiAction, const char* sCode);
        bool QuestAccept(Player* pPlayer, Creature* pCreature, Quest const* pQuest);
        bool QuestSelect(Player* pPlayer, Creature* pCreature, Quest const* pQuest);
        bool QuestComplete(Player* pPlayer, Creature* pCreature, Quest const* pQuest);
        bool ChooseReward(Player* pPlayer, Creature* pCreature, Quest const* pQuest, uint32 opt);
        uint32 NPCDialogStatus(Player* pPlayer, Creature* pCreature);
        uint32 GODialogStatus(Player* pPlayer, GameObject* pGO);
        bool ItemHello(Player* pPlayer, Item* pItem, Quest const* pQuest);
        bool ItemQuestAccept(Player* pPlayer, Item* pItem, Quest const* pQuest);
        bool GOHello(Player* pPlayer, GameObject* pGO);
        bool GOQuestAccept(Player* pPlayer, GameObject* pGO, Quest const* pQuest);
        bool GOChooseReward(Player* pPlayer, GameObject* pGO, Quest const* pQuest, uint32 opt);
        bool AreaTrigger(Player* pPlayer,AreaTriggerEntry const* atEntry);
        CreatureAI* GetAI(Creature* pCreature);
        bool ItemUse(Player* pPlayer, Item* pItem, SpellCastTargets const& targets);
        bool EffectDummyCreature(Unit* caster, uint32 spellId, uint32 effIndex, Creature* crTarget);
        InstanceData* CreateInstanceData(Map *map);
};

//Generic scripting text function
void DoScriptText(int32 textEntry, WorldObject* pSource, Unit* pTarget = NULL);

#if COMPILER == COMPILER_GNU
#define FUNC_PTR(name,callconvention,returntype,parameters)    typedef returntype(*name)parameters __attribute__ ((callconvention));
#else
#define FUNC_PTR(name, callconvention, returntype, parameters)    typedef returntype(callconvention *name)parameters;
#endif

#define sScriptMgr Oregon::Singleton<ScriptMgr>::Instance()
#endif

