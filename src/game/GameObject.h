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

#ifndef OREGONCORE_GAMEOBJECT_H
#define OREGONCORE_GAMEOBJECT_H

#include "Common.h"
#include "SharedDefines.h"
#include "Unit.h"
#include "Object.h"
#include "LootMgr.h"
#include "Database/DatabaseEnv.h"

// GCC have alternative #pragma pack(N) syntax and old gcc version not support pack(push,N), also any gcc version not support it at some platform
#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

// from `gameobject_template`
struct GameObjectInfo
{
    uint32  id;
    uint32  type;
    uint32  displayId;
    char*   name;
    char*   castBarCaption;
    uint32  faction;
    uint32  flags;
    float   size;
    union                                                   // different GO types have different data field
    {
        //0 GAMEOBJECT_TYPE_DOOR
        struct
        {
            uint32 startOpen;                               //0 used client side to determine GO_ACTIVATED means open/closed
            uint32 lockId;                                  //1 -> Lock.dbc
            uint32 autoCloseTime;                           //2 secs till autoclose = autoCloseTime / 0x10000
            uint32 noDamageImmune;                          //3 break opening whenever you recieve damage?
            uint32 openTextID;                              //4 can be used to replace castBarCaption?
            uint32 closeTextID;                             //5
        } door;
        //1 GAMEOBJECT_TYPE_BUTTON
        struct
        {
            uint32 startOpen;                               //0
            uint32 lockId;                                  //1 -> Lock.dbc
            uint32 autoCloseTime;                           //2 secs till autoclose = autoCloseTime / 0x10000
            uint32 linkedTrapId;                            //3
            uint32 noDamageImmune;                          //4 isBattlegroundObject
            uint32 large;                                   //5
            uint32 openTextID;                              //6 can be used to replace castBarCaption?
            uint32 closeTextID;                             //7
            uint32 losOK;                                   //8
        } button;
        //2 GAMEOBJECT_TYPE_QUESTGIVER
        struct
        {
            uint32 lockId;                                  //0 -> Lock.dbc
            uint32 questList;                               //1
            uint32 pageMaterial;                            //2
            uint32 gossipID;                                //3
            uint32 customAnim;                              //4
            uint32 noDamageImmune;                          //5
            uint32 openTextID;                              //6 can be used to replace castBarCaption?
            uint32 losOK;                                   //7
            uint32 allowMounted;                            //8
            uint32 large;                                   //9
        } questgiver;
        //3 GAMEOBJECT_TYPE_CHEST
        struct
        {
            uint32 lockId;                                  //0 -> Lock.dbc
            uint32 lootId;                                  //1
            uint32 chestRestockTime;                        //2
            uint32 consumable;                              //3
            uint32 minSuccessOpens;                         //4
            uint32 maxSuccessOpens;                         //5
            uint32 eventId;                                 //6 lootedEvent
            uint32 linkedTrapId;                            //7
            uint32 questId;                                 //8 not used currently but store quest required for GO activation for player
            uint32 level;                                   //9
            uint32 losOK;                                   //10
            uint32 leaveLoot;                               //11
            uint32 notInCombat;                             //12
            uint32 logLoot;                                 //13
            uint32 openTextID;                              //14 can be used to replace castBarCaption?
            uint32 groupLootRules;                          //15
        } chest;
        //4 GAMEOBJECT_TYPE_BINDER - empty
        //5 GAMEOBJECT_TYPE_GENERIC
        struct
        {
            uint32 floatingTooltip;                         //0
            uint32 highlight;                               //1
            uint32 serverOnly;                              //2
            uint32 large;                                   //3
            uint32 floatOnWater;                            //4
            uint32 questID;                                 //5
        } _generic;
        //6 GAMEOBJECT_TYPE_TRAP
        struct
        {
            uint32 lockId;                                  //0 -> Lock.dbc
            uint32 level;                                   //1
            uint32 radius;                                  //2 radius for trap activation
            uint32 spellId;                                 //3
            uint32 type;                                    //4 0 trap with no despawn after cast. 1 trap despawns after cast. 2 bomb casts on spawn.
            uint32 cooldown;                                //5 time in secs
            uint32 autoCloseTime;                           //6
            uint32 startDelay;                              //7
            uint32 serverOnly;                              //8
            uint32 stealthed;                               //9
            uint32 large;                                   //10
            uint32 invisible;                               //11
            uint32 openTextID;                              //12 can be used to replace castBarCaption?
            uint32 closeTextID;                             //13
        } trap;
        //7 GAMEOBJECT_TYPE_CHAIR
        struct
        {
            uint32 slots;                                   //0
            uint32 height;                                  //1
            uint32 onlyCreatorUse;                          //2
        } chair;
        //8 GAMEOBJECT_TYPE_SPELL_FOCUS
        struct
        {
            uint32 focusId;                                 //0
            uint32 dist;                                    //1
            uint32 linkedTrapId;                            //2
            uint32 serverOnly;                              //3
            uint32 questID;                                 //4
            uint32 large;                                   //5
        } spellFocus;
        //9 GAMEOBJECT_TYPE_TEXT
        struct
        {
            uint32 pageID;                                  //0
            uint32 language;                                //1
            uint32 pageMaterial;                            //2
            uint32 allowMounted;                            //3
        } text;
        //10 GAMEOBJECT_TYPE_GOOBER
        struct
        {
            uint32 lockId;                                  //0 -> Lock.dbc
            uint32 questId;                                 //1
            uint32 eventId;                                 //2
            uint32 autoCloseTime;                           //3
            uint32 customAnim;                              //4
            uint32 consumable;                              //5
            uint32 cooldown;                                //6
            uint32 pageId;                                  //7
            uint32 language;                                //8
            uint32 pageMaterial;                            //9
            uint32 spellId;                                 //10
            uint32 noDamageImmune;                          //11
            uint32 linkedTrapId;                            //12
            uint32 large;                                   //13
            uint32 openTextID;                              //14 can be used to replace castBarCaption?
            uint32 closeTextID;                             //15
            uint32 losOK;                                   //16 isBattlegroundObject
            uint32 allowMounted;                            //17
            uint32 floatingTooltip;                         //18
            uint32 gossipID;                                //19
        } goober;
        //11 GAMEOBJECT_TYPE_TRANSPORT
        struct
        {
            uint32 pause;                                   //0
            uint32 startOpen;                               //1
            uint32 autoCloseTime;                           //2 secs till autoclose = autoCloseTime / 0x10000
        } transport;
        //12 GAMEOBJECT_TYPE_AREADAMAGE
        struct
        {
            uint32 lockId;                                  //0
            uint32 radius;                                  //1
            uint32 damageMin;                               //2
            uint32 damageMax;                               //3
            uint32 damageSchool;                            //4
            uint32 autoCloseTime;                           //5 secs till autoclose = autoCloseTime / 0x10000
            uint32 openTextID;                              //6
            uint32 closeTextID;                             //7
        } areadamage;
        //13 GAMEOBJECT_TYPE_CAMERA
        struct
        {
            uint32 lockId;                                  //0 -> Lock.dbc
            uint32 cinematicId;                             //1
            uint32 eventID;                                 //2
            uint32 openTextID;                              //3 can be used to replace castBarCaption?
        } camera;
        //14 GAMEOBJECT_TYPE_MAPOBJECT - empty
        //15 GAMEOBJECT_TYPE_MO_TRANSPORT
        struct
        {
            uint32 taxiPathId;                              //0
            uint32 moveSpeed;                               //1
            uint32 accelRate;                               //2
            uint32 startEventID;                            //3
            uint32 stopEventID;                             //4
            uint32 transportPhysics;                        //5
            uint32 mapID;                                   //6
        } moTransport;
        //16 GAMEOBJECT_TYPE_DUELFLAG - empty
        //17 GAMEOBJECT_TYPE_FISHINGNODE
        struct
        {
            uint32 _data0;                                  //0
            uint32 lootId;                                  //1
        } fishnode;
        //18 GAMEOBJECT_TYPE_SUMMONING_RITUAL
        struct
        {
            uint32 reqParticipants;                         //0
            uint32 spellId;                                 //1
            uint32 animSpell;                               //2
            uint32 ritualPersistent;                        //3
            uint32 casterTargetSpell;                       //4
            uint32 casterTargetSpellTargets;                //5
            uint32 castersGrouped;                          //6
            uint32 ritualNoTargetCheck;                     //7
        } summoningRitual;
        //19 GAMEOBJECT_TYPE_MAILBOX - empty
        //20 GAMEOBJECT_TYPE_AUCTIONHOUSE
        struct
        {
            uint32 actionHouseID;                           //0
        } auctionhouse;
        //21 GAMEOBJECT_TYPE_GUARDPOST
        struct
        {
            uint32 creatureID;                              //0
            uint32 charges;                                 //1
        } guardpost;
        //22 GAMEOBJECT_TYPE_SPELLCASTER
        struct
        {
            uint32 spellId;                                 //0
            uint32 charges;                                 //1
            uint32 partyOnly;                               //2
        } spellcaster;
        //23 GAMEOBJECT_TYPE_MEETINGSTONE
        struct
        {
            uint32 minLevel;                                //0
            uint32 maxLevel;                                //1
            uint32 areaID;                                  //2
        } meetingstone;
        //24 GAMEOBJECT_TYPE_FLAGSTAND
        struct
        {
            uint32 lockId;                                  //0
            uint32 pickupSpell;                             //1
            uint32 radius;                                  //2
            uint32 returnAura;                              //3
            uint32 returnSpell;                             //4
            uint32 noDamageImmune;                          //5
            uint32 openTextID;                              //6
            uint32 losOK;                                   //7
        } flagstand;
        //25 GAMEOBJECT_TYPE_FISHINGHOLE                    // not implemented yet
        struct
        {
            uint32 radius;                                  //0 how close bobber must land for sending loot
            uint32 lootId;                                  //1
            uint32 minSuccessOpens;                         //2
            uint32 maxSuccessOpens;                         //3
            uint32 lockId;                                  //4 -> Lock.dbc; possibly 1628 for all?
        } fishinghole;
        //26 GAMEOBJECT_TYPE_FLAGDROP
        struct
        {
            uint32 lockId;                                  //0
            uint32 eventID;                                 //1
            uint32 pickupSpell;                             //2
            uint32 noDamageImmune;                          //3
            uint32 openTextID;                              //4
        } flagdrop;
        //27 GAMEOBJECT_TYPE_MINI_GAME
        struct
        {
            uint32 gameType;                                //0
        } miniGame;
        //29 GAMEOBJECT_TYPE_CAPTURE_POINT
        struct
        {
            uint32 radius;                                  //0
            uint32 spell;                                   //1
            uint32 worldState1;                             //2
            uint32 worldstate2;                             //3
            uint32 winEventID1;                             //4
            uint32 winEventID2;                             //5
            uint32 contestedEventID1;                       //6
            uint32 contestedEventID2;                       //7
            uint32 progressEventID1;                        //8
            uint32 progressEventID2;                        //9
            uint32 neutralEventID1;                         //10
            uint32 neutralEventID2;                         //11
            uint32 neutralPercent;                          //12
            uint32 worldstate3;                             //13
            uint32 minSuperiority;                          //14
            uint32 maxSuperiority;                          //15
            uint32 minTime;                                 //16
            uint32 maxTime;                                 //17
            uint32 large;                                   //18
            uint32 highlight;                               //19
        } capturePoint;
        //30 GAMEOBJECT_TYPE_AURA_GENERATOR
        struct
        {
            uint32 startOpen;                               //0
            uint32 radius;                                  //1
            uint32 auraID1;                                 //2
            uint32 conditionID1;                            //3
            uint32 auraID2;                                 //4
            uint32 conditionID2;                            //5
            uint32 serverOnly;                              //6
        } auraGenerator;
        //31 GAMEOBJECT_TYPE_DUNGEON_DIFFICULTY
        struct
        {
            uint32 mapID;                                   //0
            uint32 difficulty;                              //1
        } dungeonDifficulty;
        //32 GAMEOBJECT_TYPE_DO_NOT_USE_YET
        struct
        {
            uint32 mapID;                                   //0
            uint32 difficulty;                              //1
        } doNotUseYet;
        //33 GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING
        struct
        {
            uint32 dmgPctState1;                            //0
            uint32 dmgPctState2;                            //1
            uint32 state1Name;                              //2
            uint32 state2Name;                              //3
        } destructibleBuilding;
        //34 GAMEOBJECT_TYPE_GUILDBANK - empty

        // not use for specific field access (only for output with loop by all filed), also this determinate max union size
        struct
        {
            uint32 data[24];
        } raw;
    };

    char const* AIName;
    uint32 ScriptId;

    // helpers
    bool IsDespawnAtAction() const
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_CHEST: return chest.consumable;
            case GAMEOBJECT_TYPE_GOOBER: return goober.consumable;
            default: return false;
        }
    }

    uint32 GetCharges() const                               // despawn at uses amount
    {
        switch (type)
        {
            //case GAMEOBJECT_TYPE_TRAP:        return trap.charges;
            case GAMEOBJECT_TYPE_GUARDPOST: return guardpost.charges;
            case GAMEOBJECT_TYPE_SPELLCASTER: return spellcaster.charges;
            default: return 0;
        }
    }

    uint32 GetAutoCloseTime() const
    {
        uint32 autoCloseTime = 0;
        switch (type)
        {
            case GAMEOBJECT_TYPE_DOOR:
                autoCloseTime = door.autoCloseTime;
                break;
            case GAMEOBJECT_TYPE_BUTTON:
                autoCloseTime = button.autoCloseTime;
                break;
            case GAMEOBJECT_TYPE_TRAP:
                autoCloseTime = trap.autoCloseTime;
                break;
            case GAMEOBJECT_TYPE_GOOBER:
                autoCloseTime = goober.autoCloseTime;
                break;
            case GAMEOBJECT_TYPE_TRANSPORT:
                autoCloseTime = transport.autoCloseTime;
                break;
            case GAMEOBJECT_TYPE_AREADAMAGE:
                autoCloseTime = areadamage.autoCloseTime;
                break;
            default:
                break;
        }
        return autoCloseTime / 0x10000;
    }

    uint32 GetLootId() const
    {
        switch(type)
        {
            case GAMEOBJECT_TYPE_CHEST:       return chest.lootId;
            case GAMEOBJECT_TYPE_FISHINGHOLE: return fishinghole.lootId;
            default: return 0;
        }
    }

    uint32 GetLockId() const
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_DOOR:
                return door.lockId;
            case GAMEOBJECT_TYPE_BUTTON:
                return button.lockId;
            case GAMEOBJECT_TYPE_QUESTGIVER:
                return questgiver.lockId;
            case GAMEOBJECT_TYPE_CHEST:
                return chest.lockId;
            case GAMEOBJECT_TYPE_TRAP:
                return trap.lockId;
            case GAMEOBJECT_TYPE_GOOBER:
                return goober.lockId;
            case GAMEOBJECT_TYPE_AREADAMAGE:
                return areadamage.lockId;
            case GAMEOBJECT_TYPE_CAMERA:
                return camera.lockId;
            case GAMEOBJECT_TYPE_FLAGSTAND:
                return flagstand.lockId;
            case GAMEOBJECT_TYPE_FISHINGHOLE:
                return fishinghole.lockId;
            case GAMEOBJECT_TYPE_FLAGDROP:
                return flagdrop.lockId;
            default:
                return 0;
        }
    }

    bool GetDespawnPossibility() const                      // despawn at targeting of cast?
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_DOOR:
                return door.noDamageImmune;
            case GAMEOBJECT_TYPE_BUTTON:
                return button.noDamageImmune;
            case GAMEOBJECT_TYPE_QUESTGIVER:
                return questgiver.noDamageImmune;
            case GAMEOBJECT_TYPE_GOOBER:
                return goober.noDamageImmune;
            case GAMEOBJECT_TYPE_FLAGSTAND:
                return flagstand.noDamageImmune;
            case GAMEOBJECT_TYPE_FLAGDROP:
                return flagdrop.noDamageImmune;
            default:
                return true;
        }
    }

    uint32 GetLinkedGameObjectEntry() const
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_CHEST:
                return chest.linkedTrapId;
            case GAMEOBJECT_TYPE_SPELL_FOCUS:
                return spellFocus.linkedTrapId;
            case GAMEOBJECT_TYPE_GOOBER:
                return goober.linkedTrapId;
            default:
                return 0;
        }
    }

    uint32 GetGossipMenuId() const
    {
        switch (type)
        {
            case GAMEOBJECT_TYPE_QUESTGIVER: return questgiver.gossipID;
            case GAMEOBJECT_TYPE_GOOBER: return goober.gossipID;
            default: return 0;
        }
    }

    std::string GetAIName() const
    {
        return AIName;
    }
};

// GCC have alternative #pragma pack() syntax and old gcc version not support pack(pop), also any gcc version not support it at some platform
#if defined( __GNUC__ )
#pragma pack()
#else
#pragma pack(pop)
#endif

struct GameObjectLocale
{
    std::vector<std::string> Name;
    std::vector<std::string> CastBarCaption;
};

// client side GO show states
enum GOState
{
    GO_STATE_ACTIVE             = 0,                        // show in world as used and not reset (closed door open)
    GO_STATE_READY              = 1,                        // show in world as ready (closed door close)
    GO_STATE_ACTIVE_ALTERNATIVE = 2                         // show in world as used in alt way and not reset (closed door open by cannon fire)
};

#define MAX_GO_STATE              3

// from `gameobject`
struct GameObjectData
{
    explicit GameObjectData() : dbData(true) {}
    uint32 id;                                              // entry in gamobject_template
    uint32 mapid;
    float posX;
    float posY;
    float posZ;
    float orientation;
    float rotation0;
    float rotation1;
    float rotation2;
    float rotation3;
    int32  spawntimesecs;
    uint32 animprogress;
    GOState go_state;
    uint8 spawnMask;
    uint32 artKit;
    bool dbData;
};

// For containers:  [GO_NOT_READY]->GO_READY (close)->GO_ACTIVATED (open) ->GO_JUST_DEACTIVATED->GO_READY        -> ...
// For bobber:      GO_NOT_READY  ->GO_READY (close)->GO_ACTIVATED (open) ->GO_JUST_DEACTIVATED-><deleted>
// For door(closed):[GO_NOT_READY]->GO_READY (close)->GO_ACTIVATED (open) ->GO_JUST_DEACTIVATED->GO_READY(close) -> ...
// For door(open):  [GO_NOT_READY]->GO_READY (open) ->GO_ACTIVATED (close)->GO_JUST_DEACTIVATED->GO_READY(open)  -> ...
enum LootState
{
    GO_NOT_READY = 0,
    GO_READY,                                               // can be ready but despawned, and then not possible activate until spawn
    GO_ACTIVATED,
    GO_JUST_DEACTIVATED
};

class GameObjectAI;
class Unit;
class GameObjectModel;
class Transport;

// 5 sec for bobber catch
#define FISHING_BOBBER_READY_TIME 5

class GameObject : public WorldObject, public GridObject<GameObject>
{
    public:
        explicit GameObject();
        ~GameObject() override;

        void AddToWorld() override;
        void RemoveFromWorld() override;
        void CleanupsBeforeDelete() override;

        void RemoveFromOwner();

        bool Create(uint32 guidlow, uint32 name_id, Map* map, float x, float y, float z, float ang, float rotation0, float rotation1, float rotation2, float rotation3, uint32 animprogress, GOState go_state, uint32 ArtKit = 0);
        void Update(uint32 diff) override;
        static GameObject* GetGameObject(WorldObject& object, uint64 guid);
        GameObjectInfo const* GetGOInfo() const
        {
            return m_goInfo;
        }
        GameObjectData const* GetGOData() const
        {
            return m_goData;
        }

        void SetGoState(GOState state);
        void EnableCollision(bool enable);

        bool IsTransport() const;

        uint32 GetDBTableGUIDLow() const
        {
            return m_DBTableGuid;
        }

        void UpdateRotationFields(float rotation2 = 0.0f, float rotation3 = 0.0f);

        void Say(const char* text, uint32 language, uint64 TargetGuid)
        {
            MonsterSay(text, language, TargetGuid);
        }
        void Yell(const char* text, uint32 language, uint64 TargetGuid)
        {
            MonsterYell(text, language, TargetGuid);
        }
        void TextEmote(const char* text, uint64 TargetGuid)
        {
            MonsterTextEmote(text, TargetGuid);
        }
        void Whisper(const char* text, uint64 receiver)
        {
            MonsterWhisper(text, receiver);
        }
        void Say(int32 textId, uint32 language, uint64 TargetGuid)
        {
            MonsterSay(textId, language, TargetGuid);
        }
        void Yell(int32 textId, uint32 language, uint64 TargetGuid)
        {
            MonsterYell(textId, language, TargetGuid);
        }
        void TextEmote(int32 textId, uint64 TargetGuid)
        {
            MonsterTextEmote(textId, TargetGuid);
        }
        void Whisper(int32 textId, uint64 receiver)
        {
            MonsterWhisper(textId, receiver);
        }

        // overwrite WorldObject function for proper name localization
        const char* GetNameForLocaleIdx(int32 locale_idx) const override;

        void SaveToDB();
        void SaveToDB(uint32 mapid, uint8 spawnMask);
        bool LoadFromDB(uint32 guid, Map* map) { return LoadGameObjectFromDB(guid, map, false); }
        bool LoadGameObjectFromDB(uint32 guid, Map* map, bool addToMap = true);
        void DeleteFromDB();

        void SetOwnerGUID(uint64 owner)
        {
            // Owner already found and different than expected owner - remove object from old owner
            if (owner && GetOwnerGUID() && GetOwnerGUID() != owner)
                assert(false);
            m_spawnedByDefault = false;                     // all object with owner is despawned after delay
            SetUInt64Value(OBJECT_FIELD_CREATED_BY, owner);
        }
        uint64 GetOwnerGUID() const
        {
            return GetUInt64Value(OBJECT_FIELD_CREATED_BY);
        }
        Unit* GetOwner() const;

        void SetSpellId(uint32 id)
        {
            m_spawnedByDefault = false;                     // all summoned object is despawned after delay
            m_spellId = id;
        }
        uint32 GetSpellId() const
        {
            return m_spellId;
        }

        time_t GetRespawnTime() const
        {
            return m_respawnTime;
        }
        time_t GetRespawnTimeEx() const
        {
            time_t now = time(NULL);
            if (m_respawnTime > now)
                return m_respawnTime;
            else
                return now;
        }

        void SetRespawnTime(int32 respawn)
        {
            m_respawnTime = respawn > 0 ? time(NULL) + respawn : 0;
            m_respawnDelayTime = respawn > 0 ? respawn : 0;
        }
        void Respawn();
        bool isSpawned() const
        {
            return m_respawnDelayTime == 0 ||
                   (m_respawnTime > 0 && !m_spawnedByDefault) ||
                   (m_respawnTime == 0 && m_spawnedByDefault);
        }
        bool isSpawnedByDefault() const
        {
            return m_spawnedByDefault;
        }
        void SetSpawnedByDefault(bool b)
        {
            m_spawnedByDefault = b;
        }
        uint32 GetRespawnDelay() const
        {
            return m_respawnDelayTime;
        }
        void Refresh();
        void Delete();
        void getFishLoot(Loot* loot, Player* loot_owner);
        GameobjectTypes GetGoType() const
        {
            return GameobjectTypes(GetUInt32Value(GAMEOBJECT_TYPE_ID));
        }
        void SetGoType(GameobjectTypes type)
        {
            SetUInt32Value(GAMEOBJECT_TYPE_ID, type);
        }
        GOState GetGoState() const
        {
            return GOState(GetUInt32Value(GAMEOBJECT_STATE));
        }
        uint32 GetGoArtKit() const
        {
            return GetUInt32Value(GAMEOBJECT_ARTKIT);
        }
        void SetGoArtKit(uint32 artkit);
        uint32 GetGoAnimProgress() const
        {
            return GetUInt32Value(GAMEOBJECT_ANIMPROGRESS);
        }
        void SetGoAnimProgress(uint32 animprogress)
        {
            SetUInt32Value(GAMEOBJECT_ANIMPROGRESS, animprogress);
        }

        void Use(Unit* user);

        LootState getLootState() const
        {
            return m_lootState;
        }
        void SetLootState(LootState s, Unit* unit = NULL);

        void AddToSkillupList(uint32 PlayerGuidLow)
        {
            m_SkillupList.push_back(PlayerGuidLow);
        }
        bool IsInSkillupList(uint32 PlayerGuidLow) const
        {
            for (std::list<uint32>::const_iterator i = m_SkillupList.begin(); i != m_SkillupList.end(); ++i)
                if (*i == PlayerGuidLow) return true;
            return false;
        }
        void ClearSkillupList()
        {
            m_SkillupList.clear();
        }

        void AddUniqueUse(Player* player);
        void AddUse()
        {
            ++m_usetimes;
        }

        uint32 GetUseCount() const
        {
            return m_usetimes;
        }
        uint32 GetUniqueUseCount() const
        {
            return m_unique_users.size();
        }

        bool IsUsedBy(uint64 guid)
        {
            return m_unique_users.find(guid) != m_unique_users.end();
        }

        void SaveRespawnTime() override;

        Loot        loot;

        bool hasQuest(uint32 quest_id) const override;
        bool hasInvolvedQuest(uint32 quest_id) const override;
        bool ActivateToQuest(Player* pTarget) const;
        void UseDoorOrButton(uint32 time_to_restore = 0, bool alternative = false, Unit* user = NULL);
        // 0 = use `gameobject`.`spawntimesecs`
        void ResetDoorOrButton();

        void Animate(uint32 anim = 0);

        uint32 GetCooldown() const                              // Cooldown preventing goober and traps to cast spell
        {
            switch (GetGoType())
            {
            case GAMEOBJECT_TYPE_TRAP:
                return GetGOInfo()->trap.cooldown;
            case GAMEOBJECT_TYPE_GOOBER:
                return GetGOInfo()->goober.cooldown;
            default:
                return 0;
            }
        }

        void TriggeringLinkedGameObject(uint32 trapEntry, Unit* target);

        bool IsNeverVisible() const override;

        bool IsAlwaysVisibleFor(WorldObject const* seer) const override;
        bool IsInvisibleDueToDespawn() const override;

        uint8 getLevelForTarget(WorldObject const* target) const override
        {
            if (Unit* owner = GetOwner())
                return owner->getLevelForTarget(target);

            return 1;
        }

        Transport* ToTransport() { if (GetGOInfo()->type == GAMEOBJECT_TYPE_TRANSPORT) return reinterpret_cast<Transport*>(this); else return NULL; }
        Transport const* ToTransport() const { if (GetGOInfo()->type == GAMEOBJECT_TYPE_TRANSPORT) return reinterpret_cast<Transport const*>(this); else return NULL; }

        float GetInteractionDistance() const;

        void UpdateModelPosition(); 
        GameObject* LookupFishingHoleAround(float range);

        void CastSpell(Unit* target, uint32 spellId, bool triggered = true);

        GameObjectAI* AI() const { return m_AI; }

        std::string GetAIName() const;
    protected:
        bool AIM_Initialize();
        uint32      m_spellId;
        time_t      m_respawnTime;                          // (secs) time of next respawn (or despawn if GO have owner()),
        uint32      m_respawnDelayTime;                     // (secs) if 0 then current GO state no dependent from timer
        LootState   m_lootState;
        bool        m_spawnedByDefault;
        time_t      m_cooldownTime;                         // used as internal reaction delay time store (not state change reaction).
        // For traps this: spell casting cooldown, for doors/buttons: reset time.
        std::list<uint32> m_SkillupList;

        uint64 m_ritualOwnerGUID;                           // used for GAMEOBJECT_TYPE_SUMMONING_RITUAL where GO is not summoned (no owner)
        std::set<uint64> m_unique_users;
        uint32 m_usetimes;

        uint32 m_DBTableGuid;                               // For new or temporary gameobjects is 0 for saved it is lowguid
        GameObjectInfo const* m_goInfo;
        GameObjectData const* m_goData;
        void SetDisplayId(uint32 displayid);

        GameObjectModel * m_model;
    private:
        void SwitchDoorOrButton(bool activate, bool alternative = false);

        GameObjectAI* m_AI;

        void UpdateModel();                                 // updates model in case displayId were changed
};
#endif

