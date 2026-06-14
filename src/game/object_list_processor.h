#ifndef OBJECT_LIST_PROCESSOR_H
#define OBJECT_LIST_PROCESSOR_H

#include <PR/ultratypes.h>

#include "area.h"
#include "macros.h"
#include "types.h"

#define TIME_STOP_UNKNOWN_0         (1 << 0)
#define TIME_STOP_ENABLED           (1 << 1)
#define TIME_STOP_DIALOG            (1 << 2)
#define TIME_STOP_MARIO_AND_DOORS   (1 << 3)
#define TIME_STOP_ALL_OBJECTS       (1 << 4)
#define TIME_STOP_MARIO_OPENED_DOOR (1 << 5)
#define TIME_STOP_ACTIVE            (1 << 6)

#define OBJECT_POOL_CAPACITY 240

enum ObjectList {
    OBJ_LIST_PLAYER,
    OBJ_LIST_UNUSED_1,
    OBJ_LIST_DESTRUCTIVE,

    OBJ_LIST_UNUSED_3,
    OBJ_LIST_GENACTOR,

    OBJ_LIST_PUSHABLE,

    OBJ_LIST_LEVEL,
    OBJ_LIST_UNUSED_7,
    OBJ_LIST_DEFAULT,

    OBJ_LIST_SURFACE,

    OBJ_LIST_POLELIKE,

    OBJ_LIST_SPAWNER,
    OBJ_LIST_UNIMPORTANT,

    NUM_OBJ_LISTS
};

extern struct ObjectNode gObjectListArray[];

extern s32 gDebugInfoFlags;
extern s32 gNumFindFloorMisses;
extern UNUSED s32 unused_8033BEF8;
extern s32 gUnknownWallCount;
extern u32 gObjectCounter;

struct NumTimesCalled {
     s16 floor;
     s16 ceil;
     s16 wall;
};

extern struct NumTimesCalled gNumCalls;

extern s16 gDebugInfo[][8];
extern s16 gDebugInfoOverwrite[][8];

extern u32 gTimeStopState;
extern struct Object gObjectPool[];
extern struct Object gMacroObjectDefaultParent;
extern struct ObjectNode *gObjectLists;
extern struct ObjectNode gFreeObjectList;

extern struct Object *gMarioObject;
extern struct Object *gLuigiObject;
extern struct Object *gCurrentObject;

extern const BehaviorScript *gCurBhvCommand;
extern s16 gPrevFrameObjectCount;

extern s32 gSurfaceNodesAllocated;
extern s32 gSurfacesAllocated;
extern s32 gNumStaticSurfaceNodes;
extern s32 gNumStaticSurfaces;

extern struct MemoryPool *gObjectMemoryPool;

extern s16 gCheckingSurfaceCollisionsForCamera;
extern s16 gFindFloorIncludeSurfaceIntangible;
extern TerrainData *gEnvironmentRegions;
extern s32 gEnvironmentLevels[20];
extern RoomData gDoorAdjacentRooms[60][2];
extern s16 gMarioCurrentRoom;
extern s16 D_8035FEE2;
extern s16 D_8035FEE4;
extern s16 gTHIWaterDrained;
extern s16 gTTCSpeedSetting;
extern s16 gMarioShotFromCannon;
extern s16 gCCMEnteredSlide;
extern s16 gNumRoomedObjectsInMarioRoom;
extern s16 gNumRoomedObjectsNotInMarioRoom;
extern s16 gWDWWaterLevelChanging;
extern s16 gMarioOnMerryGoRound;

void bhv_mario_update(void);
void set_object_respawn_info_bits(struct Object *obj, u8 bits);
void unload_objects_from_area(UNUSED s32 unused, s32 areaIndex);
void spawn_objects_from_info(UNUSED s32 unused, struct SpawnInfo *spawnInfo);
void clear_objects(void);
void update_objects(UNUSED s32 unused);

#endif
