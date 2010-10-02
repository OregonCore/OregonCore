/* Copyright (C) 2008 OREGON <http://www.oregoncore.com/>
 *
 * Thanks to the original authors: ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef SC_SCRIPTMGR_H
#define SC_SCRIPTMGR_H

#include "Common.h"
#include "Platform/CompilerDefs.h"
#include "DBCStructure.h"

#ifndef _OREGON_SCRIPT_CONFIG
# define _OREGON_SCRIPT_CONFIG  "oregoncore.conf"
#endif _OREGON_SCRIPT_CONFIG

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
        pGOQuestAccept(NULL), pGOChooseReward(NULL),pReceiveEmote(NULL),pItemUse(NULL),
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
    uint32 (*pGODialogStatus    )(Player*, GameObject * _GO);
    bool (*pChooseReward        )(Player*, Creature*, Quest const*, uint32);
    bool (*pItemHello           )(Player*, Item*, Quest const*);
    bool (*pGOHello             )(Player*, GameObject*);
    bool (*pAreaTrigger         )(Player*, AreaTriggerEntry const*);
    bool (*pItemQuestAccept     )(Player*, Item *, Quest const*);
    bool (*pGOQuestAccept       )(Player*, GameObject*, Quest const*);
    bool (*pGOChooseReward      )(Player*, GameObject*, Quest const*, uint32);
    bool (*pReceiveEmote        )(Player*, Creature*, uint32);
    bool (*pItemUse             )(Player*, Item*, SpellCastTargets const&);

    CreatureAI* (*GetAI)(Creature*);
    InstanceData* (*GetInstanceData)(Map*);

    void RegisterSelf();
};

class ScriptMgr
{
    public:
        ScriptMgr();
        ~ScriptMgr();
        
        void ScriptsInit(char const* cfg_file = _OREGON_SCRIPT_CONFIG);
        void LoadDatabase();
        char const* ScriptsVersion();    

        std::string GetConfigValueStr(char const* option);
        int32 GetConfigValueInt32(char const* option);
        float GetConfigValueFloat(char const* option);

    //event handlers
        void OnLogin(Player *pPlayer);
        void OnLogout(Player *pPlayer);
        void OnPVPKill(Player *killer, Player *killed);
        bool GossipHello (Player * pPlayer, Creature* pCreature);
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
        bool ReceiveEmote(Player* pPlayer, Creature* pCreature, uint32 emote);
        InstanceData* CreateInstanceData(Map *map);
};

//Generic scripting text function
void DoScriptText(int32 textEntry, WorldObject* pSource, Unit *pTarget = NULL);

#if COMPILER == COMPILER_GNU
#define FUNC_PTR(name,callconvention,returntype,parameters)    typedef returntype(*name)parameters __attribute__ ((callconvention));
#else
#define FUNC_PTR(name, callconvention, returntype, parameters)    typedef returntype(callconvention *name)parameters;
#endif

#define sScriptMgr Oregon::Singleton<ScriptMgr>::Instance()
#endif

