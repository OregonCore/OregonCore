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

#ifndef OREGON_MAP_H
#define OREGON_MAP_H

#include "Platform/Define.h"
#include "Policies/ThreadingModel.h"
#include "ace/RW_Thread_Mutex.h"
#include "ace/Thread_Mutex.h"

#include "DBCStructure.h"
#include "GridDefines.h"
#include "Cell.h"
#include "Timer.h"
#include "SharedDefines.h"
#include "GameSystem/GridRefManager.h"
#include "MapRefManager.h"

#include "DynamicTree.h"
#include "GameObjectModel.h"

#include <bitset>
#include <list>
#include <set>

class Unit;
class WorldPacket;
class InstanceData;
class Group;
class InstanceSave;
class Object;
class WorldObject;
class TempSummon;
class Player;
class CreatureGroup;
struct ScriptInfo;
struct ScriptAction;
struct Position;
class Battleground;
class InstanceMap;
namespace Oregon { struct ObjectUpdater; }

struct ScriptAction
{
    uint64 sourceGUID;
    uint64 targetGUID;
    uint64 ownerGUID;                                       // owner of source if source is item
    ScriptInfo const* script;                               // pointer to static script data
};

//******************************************
// Map file format defines
//******************************************
static const uint32 MAP_MAGIC         = 0x5350414D; // SPAM
static const uint32 MAP_VERSION_MAGIC = 0x332E3176; // 3.1v
static const uint32 MAP_AREA_MAGIC    = 0x41455241; // AERA
static const uint32 MAP_HEIGHT_MAGIC  = 0x5447484D; // TGHM
static const uint32 MAP_LIQUID_MAGIC  = 0x51494C4D; // QILM

struct map_fileheader
{
    uint32 mapMagic;
    uint32 versionMagic;
    uint32 buildMagic;
    uint32 areaMapOffset;
    uint32 areaMapSize;
    uint32 heightMapOffset;
    uint32 heightMapSize;
    uint32 liquidMapOffset;
    uint32 liquidMapSize;
    uint32 holesOffset;
    uint32 holesSize;
};

#define MAP_AREA_NO_AREA      0x0001

struct map_areaHeader
{
    uint32 fourcc;
    uint16 flags;
    uint16 gridArea;
};

#define MAP_HEIGHT_NO_HEIGHT  0x0001
#define MAP_HEIGHT_AS_INT16   0x0002
#define MAP_HEIGHT_AS_INT8    0x0004

struct map_heightHeader
{
    uint32 fourcc;
    uint32 flags;
    float  gridHeight;
    float  gridMaxHeight;
};

#define MAP_LIQUID_NO_TYPE    0x0001
#define MAP_LIQUID_NO_HEIGHT  0x0002

struct map_liquidHeader
{
    uint32 fourcc;
    uint16 flags;
    uint16 liquidType;
    uint8  offsetX;
    uint8  offsetY;
    uint8  width;
    uint8  height;
    float  liquidLevel;
};

enum ZLiquidStatus
{
    LIQUID_MAP_NO_WATER     = 0x00000000,
    LIQUID_MAP_ABOVE_WATER  = 0x00000001,
    LIQUID_MAP_WATER_WALK   = 0x00000002,
    LIQUID_MAP_IN_WATER     = 0x00000004,
    LIQUID_MAP_UNDER_WATER  = 0x00000008
};

// defined in DBC and left shifted for flag usage
#define MAP_LIQUID_TYPE_NO_WATER    0x00
#define MAP_LIQUID_TYPE_MAGMA       0x01
#define MAP_LIQUID_TYPE_OCEAN       0x02
#define MAP_LIQUID_TYPE_SLIME       0x04
#define MAP_LIQUID_TYPE_WATER       0x08

#define MAP_ALL_LIQUIDS   (MAP_LIQUID_TYPE_WATER | MAP_LIQUID_TYPE_MAGMA | MAP_LIQUID_TYPE_OCEAN | MAP_LIQUID_TYPE_SLIME)

#define MAP_LIQUID_TYPE_DARK_WATER  0x10
#define MAP_LIQUID_TYPE_WMO_WATER   0x20

struct LiquidData
{
    uint32 type_flags;
    uint32 entry;
    float  level;
    float  depth_level;
};

class GridMap
{
        uint32  m_flags;
        // Area data
        uint16  m_gridArea;
        uint16* m_area_map;
        // Height level data
        float   m_gridHeight;
        float   m_gridIntHeightMultiplier;
        union
        {
            float*  m_V9;
            uint16* m_uint16_V9;
            uint8*  m_uint8_V9;
        };
        union
        {
            float*  m_V8;
            uint16* m_uint16_V8;
            uint8*  m_uint8_V8;
        };
        // Liquid data
        uint16  m_liquidType;
        uint8   m_liquid_offX;
        uint8   m_liquid_offY;
        uint8   m_liquid_width;
        uint8   m_liquid_height;
        float   m_liquidLevel;
        uint16* _liquidEntry;
        uint8* _liquidFlags;
        float*  _liquidMap;

        bool  loadAreaData(FILE* in, uint32 offset, uint32 size);
        bool  loadHeightData(FILE* in, uint32 offset, uint32 size);
        bool  loadLiquidData(FILE* in, uint32 offset, uint32 size);

        // Get height functions and pointers
        typedef float (GridMap::*pGetHeightPtr) (float x, float y) const;
        pGetHeightPtr m_gridGetHeight;
        float  getHeightFromFloat(float x, float y) const;
        float  getHeightFromUint16(float x, float y) const;
        float  getHeightFromUint8(float x, float y) const;
        float  getHeightFromFlat(float x, float y) const;

    public:
        GridMap();
        ~GridMap();
        bool  loadData(const char* filename);
        void  unloadData();

        uint16 getArea(float x, float y);
        inline float getHeight(float x, float y)
        {
            return (this->*m_gridGetHeight)(x, y);
        }
        float  getLiquidLevel(float x, float y);
        uint8  getTerrainType(float x, float y);
        ZLiquidStatus getLiquidStatus(float x, float y, float z, uint8 ReqLiquidType, LiquidData* data = 0);
};

struct CreatureMover
{
    CreatureMover() : x(0), y(0), z(0), ang(0) {}
    CreatureMover(float _x, float _y, float _z, float _ang) : x(_x), y(_y), z(_z), ang(_ang) {}

    float x, y, z, ang;
};

// GCC have alternative #pragma pack(N) syntax and old gcc version not support pack(push,N), also any gcc version not support it at some platform
#if defined(__GNUC__)
#pragma pack(1)
#else
#pragma pack(push,1)
#endif

struct InstanceTemplate
{
    uint32 map;
    uint32 parent;
    uint32 maxPlayers;
    uint32 reset_delay;
    uint32 access_id;
    float startLocX;
    float startLocY;
    float startLocZ;
    float startLocO;
    uint32 script_id;
    bool allowMount;
};

enum LevelRequirementVsMode
{
    LEVELREQUIREMENT_HEROIC = 70
};

#if defined(__GNUC__)
#pragma pack()
#else
#pragma pack(pop)
#endif

typedef UNORDERED_MAP<Creature*, CreatureMover> CreatureMoveList;

#define MAX_HEIGHT            100000.0f                     // can be use for find ground height at surface
#define INVALID_HEIGHT       -100000.0f                     // for check, must be equal to VMAP_INVALID_HEIGHT, real value for unknown height is VMAP_INVALID_HEIGHT_VALUE
#define MAX_FALL_DISTANCE     250000.0f                     // "unlimited fall" to find VMap ground if it is available, just larger than MAX_HEIGHT - INVALID_HEIGHT
#define DEFAULT_HEIGHT_SEARCH     50.0f                     // default search distance to find height at nearby locations
#define MIN_UNLOAD_DELAY      1                             // immediate unload

typedef std::map<uint32/*leaderDBGUID*/, CreatureGroup*>        CreatureGroupHolderType;

class Map : public GridRefManager<NGridType>, public Oregon::ObjectLevelLockable<Map, ACE_Thread_Mutex>
{
        friend class MapReference;
    public:
        Map(uint32 id, time_t, uint32 InstanceId, uint8 SpawnMode, Map* _parent = NULL);
        ~Map() override;

        // currently unused for normal maps
        bool CanUnload(uint32 diff)
        {
            if (!m_unloadTimer) return false;
            if (m_unloadTimer <= diff) return true;
            m_unloadTimer -= diff;
            return false;
        }

        virtual bool AddPlayerToMap(Player*);
        virtual void RemovePlayerFromMap(Player*, bool);

        template<class T> bool AddToMap(T*);
        template<class T> void RemoveFromMap(T*, bool);

        void VisitNearbyCellsOf(WorldObject* obj, TypeContainerVisitor<Oregon::ObjectUpdater, GridTypeMapContainer> &gridVisitor, TypeContainerVisitor<Oregon::ObjectUpdater, WorldTypeMapContainer> &worldVisitor);
        virtual void Update(const uint32&);

        float GetVisibilityRange() const { return m_VisibleDistance; }

        //function for setting up visibility distance for maps on per-type/per-Id basis
        virtual void InitVisibilityDistance();

        void PlayerRelocation(Player*, float x, float y, float z, float orientation);
        void CreatureRelocation(Creature* creature, float x, float y, float z, float ang, bool respawnRelocationOnFail = true);

        template<class T, class CONTAINER> void Visit(const Cell& cell, TypeContainerVisitor<T, CONTAINER>& visitor);

        bool IsRemovalGrid(float x, float y) const
        {
            GridCoord p = Oregon::ComputeGridCoord(x, y);
            return !getNGrid(p.x_coord, p.y_coord) || getNGrid(p.x_coord, p.y_coord)->GetGridState() == GRID_STATE_REMOVAL;
        }

        bool IsGridLoaded(float x, float y) const
        {
            return IsGridLoaded(Oregon::ComputeGridCoord(x, y));
        }

        bool GetUnloadLock(const GridCoord& p) const
        {
            return getNGrid(p.x_coord, p.y_coord)->getUnloadLock();
        }
        void SetUnloadLock(const GridCoord& p, bool on)
        {
            getNGrid(p.x_coord, p.y_coord)->setUnloadExplicitLock(on);
        }
        void LoadGrid(float x, float y);
        bool UnloadGrid(NGridType& ngrid, bool pForce);
        virtual void UnloadAll();

        void ResetGridExpiry(NGridType& grid, float factor = 1) const
        {
            grid.ResetTimeTracker(time_t(float(i_gridExpiry)*factor));
        }

        time_t GetGridExpiry(void) const
        {
            return i_gridExpiry;
        }
        uint32 GetId(void) const
        {
            return i_mapEntry->MapID;
        }

        static bool ExistMap(uint32 mapid, int gx, int gy);
        static bool ExistVMap(uint32 mapid, int gx, int gy);

        static void InitStateMachine();
        static void DeleteStateMachine();

        Map const* GetParent() const
        {
            return m_parentMap;
        }

        InstanceMap* ToInstanceMap() { if (IsDungeon()) return reinterpret_cast<InstanceMap*>(this); else return NULL;  }
        InstanceMap const* ToInstanceMap() const { if (IsDungeon()) return reinterpret_cast<InstanceMap const*>(this); return NULL; }

        float GetWaterOrGroundLevel(float x, float y, float z, float* ground = NULL, bool swim = false) const;
        float GetHeight(float x, float y, float z, bool checkVMap = true, float maxSearchDist = DEFAULT_HEIGHT_SEARCH) const;

        ZLiquidStatus getLiquidStatus(float x, float y, float z, uint8 ReqLiquidType, LiquidData* data = 0) const;

        uint16 GetAreaFlag(float x, float y, float z, bool* isOutdoors = 0) const;
        bool GetAreaInfo(float x, float y, float z, uint32& mogpflags, int32& adtId, int32& rootId, int32& groupId) const;

        bool IsOutdoors(float x, float y, float z) const;

        uint8 GetTerrainType(float x, float y) const;
        float GetWaterLevel(float x, float y) const;
        bool IsInWater(float x, float y, float z, LiquidData* data = nullptr) const;
        bool IsUnderWater(float x, float y, float z) const;
        bool IsSwimmable(float x, float y, float z, LiquidData* data = nullptr) const;

        static uint32 GetAreaId(uint16 areaflag, uint32 map_id);
        static uint32 GetZoneId(uint16 areaflag, uint32 map_id);

        uint32 GetAreaId(float x, float y, float z) const
        {
            return GetAreaId(GetAreaFlag(x, y, z), GetId());
        }

        uint32 GetZoneId(float x, float y, float z) const
        {
            return GetZoneId(GetAreaFlag(x, y, z), GetId());
        }

        void MoveAllCreaturesInMoveList();
        void RemoveAllObjectsInRemoveList();
        virtual void RemoveAllPlayers();

        bool CreatureRespawnRelocation(Creature* c);        // used only in MoveAllCreaturesInMoveList and ObjectGridUnloader

        // assert print helper
        bool CheckGridIntegrity(Creature* c, bool moved) const;

        uint32 GetInstanceId() const
        {
            return i_InstanceId;
        }
        uint8 GetSpawnMode() const
        {
            return (i_spawnMode);
        }

        enum EnterState
        {
            CAN_ENTER = 0,
            CANNOT_ENTER_ALREADY_IN_MAP = 1, // Player is already in the map
            CANNOT_ENTER_NO_ENTRY, // No map entry was found for the target map ID
            CANNOT_ENTER_UNINSTANCED_DUNGEON, // No instance template was found for dungeon map
            CANNOT_ENTER_DIFFICULTY_UNAVAILABLE, // Requested instance difficulty is not available for target map
            CANNOT_ENTER_NOT_IN_RAID, // Target instance is a raid instance and the player is not in a raid group
            CANNOT_ENTER_CORPSE_IN_DIFFERENT_INSTANCE, // Player is dead and their corpse is not in target instance
            CANNOT_ENTER_INSTANCE_BIND_MISMATCH, // Player's permanent instance save is not compatible with their group's current instance bind
            CANNOT_ENTER_TOO_MANY_INSTANCES, // Player has entered too many instances recently
            CANNOT_ENTER_MAX_PLAYERS, // Target map already has the maximum number of players allowed
            CANNOT_ENTER_ZONE_IN_COMBAT, // A boss encounter is currently in progress on the target map
            CANNOT_ENTER_UNSPECIFIED_REASON
        };
        virtual EnterState CannotEnter(Player* /*player*/) { return CAN_ENTER; }

        const char* GetMapName() const;

        // have meaning only for instanced map (that have set real difficulty)
        DungeonDifficulty GetDifficulty() const { return DungeonDifficulty(GetSpawnMode()); }
        bool IsRegularDifficulty() const { return GetDifficulty() == DIFFICULTY_NORMAL; }

        bool Instanceable() const
        {
            return i_mapEntry && i_mapEntry->Instanceable();
        }
        // NOTE: this duplicate of Instanceable(), but Instanceable() can be changed when BG also will be instanceable
        bool IsDungeon() const
        {
            return i_mapEntry && i_mapEntry->IsDungeon();
        }
        bool IsRaid() const
        {
            return i_mapEntry && i_mapEntry->IsRaid();
        }
        bool IsHeroic() const
        {
            return i_spawnMode == DIFFICULTY_HEROIC;
        }
        bool IsBattleground() const
        {
            return i_mapEntry && i_mapEntry->IsBattleground();
        }
        bool IsBattleArena() const
        {
            return i_mapEntry && i_mapEntry->IsBattleArena();
        }
        bool IsBattlegroundOrArena() const
        {
            return i_mapEntry && i_mapEntry->IsBattlegroundOrArena();
        }

        void AddObjectToRemoveList(WorldObject* obj);
        void AddObjectToSwitchList(WorldObject* obj, bool on);
        virtual void DelayedUpdate(const uint32 diff);

        void UpdateObjectVisibility(WorldObject* obj, Cell cell, CellCoord cellpair);
        void UpdateObjectsVisibilityFor(Player* player, Cell cell, CellCoord cellpair);

        void resetMarkedCells()
        {
            marked_cells.reset();
        }
        bool isCellMarked(uint32 pCellId)
        {
            return marked_cells.test(pCellId);
        }
        void markCell(uint32 pCellId)
        {
            marked_cells.set(pCellId);
        }

        bool HavePlayers() const
        {
            return !m_mapRefManager.isEmpty();
        }
        uint32 GetPlayersCountExceptGMs() const;
        bool ActiveObjectsNearGrid(NGridType const& ngrid) const;

        void AddWorldObject(WorldObject* obj)
        {
            i_worldObjects.insert(obj);
        }
        void RemoveWorldObject(WorldObject* obj)
        {
            i_worldObjects.erase(obj);
        }

        void SendToPlayers(WorldPacket const* data) const;

        typedef MapRefManager PlayerList;
        PlayerList const& GetPlayers() const
        {
            return m_mapRefManager;
        }

        //per-map script storage
        void ScriptsStart(std::map<uint32, std::multimap<uint32, ScriptInfo> > const& scripts, uint32 id, Object* source, Object* target);
        void ScriptCommandStart(ScriptInfo const& script, uint32 delay, Object* source, Object* target);

        // must called with AddToWorld
        template<class T>
        void AddToActive(T* obj)
        {
            AddToActiveHelper(obj);
        }

        void AddToActive(Creature* obj);

        // must called with RemoveFromWorld
        template<class T>
        void RemoveFromActive(T* obj)
        {
            RemoveFromActiveHelper(obj);
        }

        void RemoveFromActive(Creature* obj);

        void SwitchGridContainers(Creature* creature, bool toWorldContainer);
        template<class NOTIFIER> void VisitAll(const float& x, const float& y, float radius, NOTIFIER& notifier);
        template<class NOTIFIER> void VisitWorld(const float& x, const float& y, float radius, NOTIFIER& notifier);
        template<class NOTIFIER> void VisitGrid(const float& x, const float& y, float radius, NOTIFIER& notifier);
        CreatureGroupHolderType CreatureGroupHolder;

        void UpdateIteratorBack(Player* player);

        TempSummon* SummonCreature(uint32 entry, const Position& pos, SummonPropertiesEntry const* properties = NULL, uint32 duration = 0, Unit* summoner = NULL, uint32 spellId = 0, TempSummonType spwType = TEMPSUMMON_MANUAL_DESPAWN);
        Creature* GetCreature(uint64 guid);
        GameObject* GetGameObject(uint64 guid);
        DynamicObject* GetDynamicObject(uint64 guid);

        bool isInLineOfSight(float x1, float y1, float z1, float x2, float y2, float z2) const;
        void Balance() { m_dyn_tree.balance(); }
        void Remove(const GameObjectModel& mdl) { m_dyn_tree.remove(mdl); }
        void Insert(const GameObjectModel& mdl) { m_dyn_tree.insert(mdl); }
        bool Contains(const GameObjectModel& mdl) const { return m_dyn_tree.contains(mdl);}
        bool getObjectHitPos(float x1, float y1, float z1, float x2, float y2, float z2, float& rx, float &ry, float& rz, float modifyDist);
    private:
        void LoadMapAndVMap(int gx, int gy);
        void LoadVMap(int gx, int gy);
        void LoadMMap(int gx, int gy);
        void LoadMap(int gx, int gy, bool reload = false);
        GridMap* GetGrid(float x, float y);

        void SetTimer(uint32 t)
        {
            i_gridExpiry = t < MIN_GRID_DELAY ? MIN_GRID_DELAY : t;
        }

        void SendInitSelf(Player* player);

        void SendInitTransports(Player* player);
        void SendRemoveTransports(Player* player);

        bool CreatureCellRelocation(Creature* creature, Cell new_cell);

        void AddCreatureToMoveList(Creature* c, float x, float y, float z, float ang);
        CreatureMoveList i_creaturesToMove;

        bool IsGridLoaded(const GridCoord &) const;
        void EnsureGridCreated(const GridCoord&);
        bool EnsureGridLoaded(Cell const&);
        void EnsureGridLoadedForActiveObject(Cell const&, WorldObject* object);

        void buildNGridLinkage(NGridType* pNGridType)
        {
            pNGridType->link(this);
        }

        template<class T> void AddType(T* obj);
        template<class T> void RemoveType(T* obj, bool);

        NGridType* getNGrid(uint32 x, uint32 y) const
        {
            ASSERT(x < MAX_NUMBER_OF_GRIDS);
            ASSERT(y < MAX_NUMBER_OF_GRIDS);
            return i_grids[x][y];
        }

        bool isGridObjectDataLoaded(uint32 x, uint32 y) const
        {
            return getNGrid(x, y)->isGridObjectDataLoaded();
        }
        void setGridObjectDataLoaded(bool pLoaded, uint32 x, uint32 y)
        {
            getNGrid(x, y)->setGridObjectDataLoaded(pLoaded);
        }

        void setNGrid(NGridType* grid, uint32 x, uint32 y);
        void ScriptsProcess();

    protected:
        void SetUnloadReferenceLock(const GridCoord& p, bool on)
        {
            getNGrid(p.x_coord, p.y_coord)->setUnloadReferenceLock(on);
        }

        typedef Oregon::ObjectLevelLockable<Map, ACE_Thread_Mutex>::Lock Guard;

        MapEntry const* i_mapEntry;
        uint8 i_spawnMode;
        uint32 i_InstanceId;
        uint32 m_unloadTimer;
        float m_VisibleDistance;

        MapRefManager m_mapRefManager;
        MapRefManager::iterator m_mapRefIter;

        int32 m_VisibilityNotifyPeriod;

        typedef std::set<WorldObject*> ActiveNonPlayers;
        ActiveNonPlayers m_activeNonPlayers;
        ActiveNonPlayers::iterator m_activeNonPlayersIter;

    private:
        Player* _GetScriptPlayerSourceOrTarget(Object* source, Object* target, const ScriptInfo* scriptInfo) const;
        Creature* _GetScriptCreatureSourceOrTarget(Object* source, Object* target, const ScriptInfo* scriptInfo, bool bReverse = false) const;
        Unit* _GetScriptUnit(Object* obj, bool isSource, const ScriptInfo* scriptInfo) const;
        Player* _GetScriptPlayer(Object* obj, bool isSource, const ScriptInfo* scriptInfo) const;
        Creature* _GetScriptCreature(Object* obj, bool isSource, const ScriptInfo* scriptInfo) const;
        WorldObject* _GetScriptWorldObject(Object* obj, bool isSource, const ScriptInfo* scriptInfo) const;
        void _ScriptProcessDoor(Object* source, Object* target, const ScriptInfo* scriptInfo) const;
        GameObject* _FindGameObject(WorldObject* pWorldObject, uint32 guid) const;

        DynamicMapTree m_dyn_tree;
        time_t i_gridExpiry;

        //used for fast base_map (e.g. MapInstanced class object) search for
        //InstanceMaps and BattlegroundMaps...
        Map* m_parentMap;

        NGridType* i_grids[MAX_NUMBER_OF_GRIDS][MAX_NUMBER_OF_GRIDS];
        GridMap* GridMaps[MAX_NUMBER_OF_GRIDS][MAX_NUMBER_OF_GRIDS];
        std::bitset<TOTAL_NUMBER_OF_CELLS_PER_MAP* TOTAL_NUMBER_OF_CELLS_PER_MAP> marked_cells;

        //these functions used to process player/mob aggro reactions and
        //visibility calculations. Highly optimized for massive calculations
        void ProcessRelocationNotifies(const uint32& diff);

        bool i_scriptLock;
        std::set<WorldObject*> i_objectsToRemove;
        std::map<WorldObject*, bool> i_objectsToSwitch;
        std::set<WorldObject*> i_worldObjects;
        std::multimap<time_t, ScriptAction> m_scriptSchedule;

        // Type specific code for add/remove to/from grid
        template<class T>
            void AddToGrid(T* object, Cell const& cell);

        template<class T>
            void DeleteFromWorld(T*);

        template<class T>
        void AddToActiveHelper(T* obj)
        {
            m_activeNonPlayers.insert(obj);
        }

        template<class T>
        void RemoveFromActiveHelper(T* obj)
        {
            // Map::Update for active object in proccess
            if (m_activeNonPlayersIter != m_activeNonPlayers.end())
            {
                ActiveNonPlayers::iterator itr = m_activeNonPlayers.find(obj);
                if (itr == m_activeNonPlayers.end())
                    return;
                if (itr == m_activeNonPlayersIter)
                    ++m_activeNonPlayersIter;
                m_activeNonPlayers.erase(itr);
            }
            else
                m_activeNonPlayers.erase(obj);
        }
};

enum InstanceResetMethod
{
    INSTANCE_RESET_ALL,
    INSTANCE_RESET_CHANGE_DIFFICULTY,
    INSTANCE_RESET_GLOBAL,
    INSTANCE_RESET_GROUP_DISBAND,
    INSTANCE_RESET_GROUP_JOIN,
    INSTANCE_RESET_RESPAWN_DELAY
};

class InstanceMap : public Map
{
    public:
        InstanceMap(uint32 id, time_t, uint32 InstanceId, uint8 SpawnMode, Map* _parent);
        ~InstanceMap() override;
        bool AddPlayerToMap(Player*) override;
        void RemovePlayerFromMap(Player*, bool) override;
        void Update(const uint32&) override;
        void CreateInstanceData(bool load);
        bool Reset(uint8 method);
        uint32 GetScriptId()
        {
            return i_script_id;
        }
        InstanceData* GetInstanceData()
        {
            return i_data;
        }
        void PermBindAllPlayers(Player* player);
        time_t GetResetTime();
        void UnloadAll() override;
        EnterState CannotEnter(Player* player) override;
        void SendResetWarnings(uint32 timeLeft) const;
        void SetResetSchedule(bool on);

        void InitVisibilityDistance() override;
    private:
        bool m_resetAfterUnload;
        bool m_unloadWhenEmpty;
        InstanceData* i_data;
        uint32 i_script_id;
};

class BattlegroundMap : public Map
{
    public:
        BattlegroundMap(uint32 id, time_t, uint32 InstanceId, Map* _parent);
        ~BattlegroundMap() override;

        bool AddPlayerToMap(Player*) override;
        void RemovePlayerFromMap(Player*, bool) override;
        EnterState CannotEnter(Player* player) override;
        void SetUnload();
        void RemoveAllPlayers() override;

        void InitVisibilityDistance() override;
        Battleground* GetBG()
        {
            return m_bg;
        }
        void SetBG(Battleground* bg)
        {
            m_bg = bg;
        }
    private:
        Battleground* m_bg;
};

/*inline
uint64
Map::CalculateGridMask(const uint32 &y) const
{
    uint64 mask = 1;
    mask <<= y;
    return mask;
}
*/

template<class T, class CONTAINER>
inline void Map::Visit(Cell const& cell, TypeContainerVisitor<T, CONTAINER>& visitor)
{
    const uint32 x = cell.GridX();
    const uint32 y = cell.GridY();
    const uint32 cell_x = cell.CellX();
    const uint32 cell_y = cell.CellY();

    if (!cell.NoCreate() || IsGridLoaded(GridCoord(x, y)))
    {
        EnsureGridLoaded(cell);
        getNGrid(x, y)->VisitGrid(cell_x, cell_y, visitor);
    }
}

template<class NOTIFIER>
inline void Map::VisitAll(float const& x, float const& y, float radius, NOTIFIER& notifier)
{
    CellCoord p(Oregon::ComputeCellCoord(x, y));
    Cell cell(p);
    cell.SetNoCreate();

    TypeContainerVisitor<NOTIFIER, WorldTypeMapContainer> world_object_notifier(notifier);
    cell.Visit(p, world_object_notifier, *this, radius, x, y);
    TypeContainerVisitor<NOTIFIER, GridTypeMapContainer >  grid_object_notifier(notifier);
    cell.Visit(p, grid_object_notifier, *this, radius, x, y);
}

template<class NOTIFIER>
inline void Map::VisitWorld(const float &x, const float &y, float radius, NOTIFIER &notifier)
{
    CellCoord p(Oregon::ComputeCellCoord(x, y));
    Cell cell(p);
    cell.SetNoCreate();

    TypeContainerVisitor<NOTIFIER, WorldTypeMapContainer> world_object_notifier(notifier);
    cell.Visit(p, world_object_notifier, *this, radius, x, y);
}

template<class NOTIFIER>
inline void Map::VisitGrid(const float &x, const float &y, float radius, NOTIFIER &notifier)
{
    CellCoord p(Oregon::ComputeCellCoord(x, y));
    Cell cell(p);
    cell.SetNoCreate();

    TypeContainerVisitor<NOTIFIER, GridTypeMapContainer >  grid_object_notifier(notifier);
    cell.Visit(p, grid_object_notifier, *this, radius, x, y);
}
#endif

