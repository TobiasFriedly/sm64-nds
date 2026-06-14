#include <PR/ultratypes.h>

#include "sm64.h"
#include "area.h"
#include "behavior_data.h"
#include "camera.h"
#include "debug.h"
#include "engine/behavior_script.h"
#include "engine/graph_node.h"
#include "engine/surface_collision.h"
#include "engine/surface_load.h"
#include "interaction.h"
#include "level_update.h"
#include "mario.h"
#include "memory.h"
#include "object_collision.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "platform_displacement.h"
#include "profiler.h"
#include "spawn_object.h"

s32 gDebugInfoFlags;

s32 gNumFindFloorMisses;

UNUSED s32 unused_8033BEF8;

s32 gUnknownWallCount;

u32 gObjectCounter;

struct NumTimesCalled gNumCalls;

s16 gDebugInfo[16][8];
s16 gDebugInfoOverwrite[16][8];

u32 gTimeStopState;

struct Object gObjectPool[OBJECT_POOL_CAPACITY];

struct Object gMacroObjectDefaultParent;

struct ObjectNode *gObjectLists;

struct ObjectNode gFreeObjectList;

struct Object *gMarioObject;

struct Object *gLuigiObject;

struct Object *gCurrentObject;

const BehaviorScript *gCurBhvCommand;

s16 gPrevFrameObjectCount;

s32 gSurfaceNodesAllocated;

s32 gSurfacesAllocated;

s32 gNumStaticSurfaceNodes;

s32 gNumStaticSurfaces;

struct MemoryPool *gObjectMemoryPool;

s16 gCheckingSurfaceCollisionsForCamera;
s16 gFindFloorIncludeSurfaceIntangible;
TerrainData *gEnvironmentRegions;
s32 gEnvironmentLevels[20];
RoomData gDoorAdjacentRooms[60][2];
s16 gMarioCurrentRoom;
s16 D_8035FEE2;
s16 D_8035FEE4;
s16 gTHIWaterDrained;
s16 gTTCSpeedSetting;
s16 gMarioShotFromCannon;
s16 gCCMEnteredSlide;
s16 gNumRoomedObjectsInMarioRoom;
s16 gNumRoomedObjectsNotInMarioRoom;
s16 gWDWWaterLevelChanging;
s16 gMarioOnMerryGoRound;

struct ObjectNode gObjectListArray[16];

s8 sObjectListUpdateOrder[] = { OBJ_LIST_SPAWNER,
                                OBJ_LIST_SURFACE,
                                OBJ_LIST_POLELIKE,
                                OBJ_LIST_PLAYER,
                                OBJ_LIST_PUSHABLE,
                                OBJ_LIST_GENACTOR,
                                OBJ_LIST_DESTRUCTIVE,
                                OBJ_LIST_LEVEL,
                                OBJ_LIST_DEFAULT,
                                OBJ_LIST_UNIMPORTANT,
                                -1 };

struct ParticleProperties {
    u32 particleFlag;
    u32 activeParticleFlag;
    u8 model;
    const BehaviorScript *behavior;
};

struct ParticleProperties sParticleTypes[] = {
    { PARTICLE_DUST,                 ACTIVE_PARTICLE_DUST,                 MODEL_MIST,                 bhvMistParticleSpawner },
    { PARTICLE_VERTICAL_STAR,        ACTIVE_PARTICLE_V_STAR,               MODEL_NONE,                 bhvVertStarParticleSpawner },
    { PARTICLE_HORIZONTAL_STAR,      ACTIVE_PARTICLE_H_STAR,               MODEL_NONE,                 bhvHorStarParticleSpawner },
    { PARTICLE_SPARKLES,             ACTIVE_PARTICLE_SPARKLES,             MODEL_SPARKLES,             bhvSparkleParticleSpawner },
    { PARTICLE_BUBBLE,               ACTIVE_PARTICLE_BUBBLE,               MODEL_BUBBLE,               bhvBubbleParticleSpawner },
    { PARTICLE_WATER_SPLASH,         ACTIVE_PARTICLE_WATER_SPLASH,         MODEL_WATER_SPLASH,         bhvWaterSplash },
    { PARTICLE_IDLE_WATER_WAVE,      ACTIVE_PARTICLE_IDLE_WATER_WAVE,      MODEL_IDLE_WATER_WAVE,      bhvIdleWaterWave },
    { PARTICLE_PLUNGE_BUBBLE,        ACTIVE_PARTICLE_PLUNGE_BUBBLE,        MODEL_WHITE_PARTICLE_SMALL, bhvPlungeBubble },
    { PARTICLE_WAVE_TRAIL,           ACTIVE_PARTICLE_WAVE_TRAIL,           MODEL_WAVE_TRAIL,           bhvWaveTrail },
    { PARTICLE_FIRE,                 ACTIVE_PARTICLE_FIRE,                 MODEL_RED_FLAME,            bhvFireParticleSpawner },
    { PARTICLE_SHALLOW_WATER_WAVE,   ACTIVE_PARTICLE_SHALLOW_WATER_WAVE,   MODEL_NONE,                 bhvShallowWaterWave },
    { PARTICLE_SHALLOW_WATER_SPLASH, ACTIVE_PARTICLE_SHALLOW_WATER_SPLASH, MODEL_NONE,                 bhvShallowWaterSplash },
    { PARTICLE_LEAF,                 ACTIVE_PARTICLE_LEAF,                 MODEL_NONE,                 bhvLeafParticleSpawner },
    { PARTICLE_SNOW,                 ACTIVE_PARTICLE_SNOW,                 MODEL_NONE,                 bhvSnowParticleSpawner },
    { PARTICLE_BREATH,               ACTIVE_PARTICLE_BREATH,               MODEL_NONE,                 bhvBreathParticleSpawner },
    { PARTICLE_DIRT,                 ACTIVE_PARTICLE_DIRT,                 MODEL_NONE,                 bhvDirtParticleSpawner },
    { PARTICLE_MIST_CIRCLE,          ACTIVE_PARTICLE_MIST_CIRCLE,          MODEL_NONE,                 bhvMistCircParticleSpawner },
    { PARTICLE_TRIANGLE,             ACTIVE_PARTICLE_TRIANGLE,             MODEL_NONE,                 bhvTriangleParticleSpawner },
    { 0, 0, MODEL_NONE, NULL },
};

void copy_mario_state_to_object(void) {
    s32 i = 0;

    if (gCurrentObject != gMarioObject) {
        i++;
    }

    gCurrentObject->oVelX = gMarioStates[i].vel[0];
    gCurrentObject->oVelY = gMarioStates[i].vel[1];
    gCurrentObject->oVelZ = gMarioStates[i].vel[2];

    gCurrentObject->oPosX = gMarioStates[i].pos[0];
    gCurrentObject->oPosY = gMarioStates[i].pos[1];
    gCurrentObject->oPosZ = gMarioStates[i].pos[2];

    gCurrentObject->oMoveAnglePitch = gCurrentObject->header.gfx.angle[0];
    gCurrentObject->oMoveAngleYaw = gCurrentObject->header.gfx.angle[1];
    gCurrentObject->oMoveAngleRoll = gCurrentObject->header.gfx.angle[2];

    gCurrentObject->oFaceAnglePitch = gCurrentObject->header.gfx.angle[0];
    gCurrentObject->oFaceAngleYaw = gCurrentObject->header.gfx.angle[1];
    gCurrentObject->oFaceAngleRoll = gCurrentObject->header.gfx.angle[2];

    gCurrentObject->oAngleVelPitch = gMarioStates[i].angleVel[0];
    gCurrentObject->oAngleVelYaw = gMarioStates[i].angleVel[1];
    gCurrentObject->oAngleVelRoll = gMarioStates[i].angleVel[2];
}

void spawn_particle(u32 activeParticleFlag, s16 model, const BehaviorScript *behavior) {
    if (!(gCurrentObject->oActiveParticleFlags & activeParticleFlag)) {
        struct Object *particle;
        gCurrentObject->oActiveParticleFlags |= activeParticleFlag;
        particle = spawn_object_at_origin(gCurrentObject, 0, model, behavior);
        obj_copy_pos_and_angle(particle, gCurrentObject);
    }
}

void bhv_mario_update(void) {
    u32 particleFlags = 0;
    s32 i;

    particleFlags = execute_mario_action(gCurrentObject);
    gCurrentObject->oMarioParticleFlags = particleFlags;

    copy_mario_state_to_object();

    i = 0;
    while (sParticleTypes[i].particleFlag != 0) {
        if (particleFlags & sParticleTypes[i].particleFlag) {
            spawn_particle(sParticleTypes[i].activeParticleFlag, sParticleTypes[i].model,
                           sParticleTypes[i].behavior);
        }

        i++;
    }
}

s32 update_objects_starting_at(struct ObjectNode *objList, struct ObjectNode *firstObj) {
    s32 count = 0;

    while (objList != firstObj) {
        gCurrentObject = (struct Object *) firstObj;

        gCurrentObject->header.gfx.node.flags |= GRAPH_RENDER_HAS_ANIMATION;
        cur_obj_update();

        firstObj = firstObj->next;
        count++;
    }

    return count;
}

s32 update_objects_during_time_stop(struct ObjectNode *objList, struct ObjectNode *firstObj) {
    s32 count = 0;
    s32 unfrozen;

    while (objList != firstObj) {
        gCurrentObject = (struct Object *) firstObj;

        unfrozen = FALSE;

        if (!(gTimeStopState & TIME_STOP_ALL_OBJECTS)) {
            if (gCurrentObject == gMarioObject && !(gTimeStopState & TIME_STOP_MARIO_AND_DOORS)) {
                unfrozen = TRUE;
            }

            if ((gCurrentObject->oInteractType & (INTERACT_DOOR | INTERACT_WARP_DOOR))
                && !(gTimeStopState & TIME_STOP_MARIO_AND_DOORS)) {
                unfrozen = TRUE;
            }

            if (gCurrentObject->activeFlags
                & (ACTIVE_FLAG_UNIMPORTANT | ACTIVE_FLAG_INITIATED_TIME_STOP)) {
                unfrozen = TRUE;
            }
        }

        if (unfrozen) {
            gCurrentObject->header.gfx.node.flags |= GRAPH_RENDER_HAS_ANIMATION;
            cur_obj_update();
        } else {
            gCurrentObject->header.gfx.node.flags &= ~GRAPH_RENDER_HAS_ANIMATION;
        }

        firstObj = firstObj->next;
        count++;
    }

    return count;
}

s32 update_objects_in_list(struct ObjectNode *objList) {
    s32 count;
    struct ObjectNode *firstObj = objList->next;

    if (!(gTimeStopState & TIME_STOP_ACTIVE)) {
        count = update_objects_starting_at(objList, firstObj);
    } else {
        count = update_objects_during_time_stop(objList, firstObj);
    }

    return count;
}

s32 unload_deactivated_objects_in_list(struct ObjectNode *objList) {
    struct ObjectNode *obj = objList->next;

    while (objList != obj) {
        gCurrentObject = (struct Object *) obj;

        obj = obj->next;

        if ((gCurrentObject->activeFlags & ACTIVE_FLAG_ACTIVE) != ACTIVE_FLAG_ACTIVE) {

            if (!(gCurrentObject->oFlags & OBJ_FLAG_PERSISTENT_RESPAWN)) {
                set_object_respawn_info_bits(gCurrentObject, RESPAWN_INFO_DONT_RESPAWN);
            }

            unload_object(gCurrentObject);
        }
    }

    return 0;
}

void set_object_respawn_info_bits(struct Object *obj, u8 bits) {
    u32 *info32;
    u16 *info16;

    switch (obj->respawnInfoType) {
        case RESPAWN_INFO_TYPE_32:
            info32 = (u32 *) obj->respawnInfo;
            *info32 |= bits << 8;
            break;

        case RESPAWN_INFO_TYPE_16:
            info16 = (u16 *) obj->respawnInfo;
            *info16 |= bits << 8;
            break;
    }
}

void unload_objects_from_area(UNUSED s32 unused, s32 areaIndex) {
    struct Object *obj;
    struct ObjectNode *node;
    struct ObjectNode *list;
    s32 i;
    gObjectLists = gObjectListArray;

    for (i = 0; i < NUM_OBJ_LISTS; i++) {
        list = gObjectLists + i;
        node = list->next;

        while (node != list) {
            obj = (struct Object *) node;
            node = node->next;

            if (obj->header.gfx.activeAreaIndex == areaIndex) {
                unload_object(obj);
            }
        }
    }
}

void spawn_objects_from_info(UNUSED s32 unused, struct SpawnInfo *spawnInfo) {
    gObjectLists = gObjectListArray;
    gTimeStopState = 0;

    gWDWWaterLevelChanging = FALSE;
    gMarioOnMerryGoRound = FALSE;

#ifndef VERSION_JP
    clear_mario_platform();
#endif

    if (gCurrAreaIndex == 2) {
        gCCMEnteredSlide |= 1;
    }

    while (spawnInfo != NULL) {
        struct Object *object;
        UNUSED u8 filler[4];
        const BehaviorScript *script;
        UNUSED s16 arg16 = (s16)(spawnInfo->behaviorArg & 0xFFFF);

        script = segmented_to_virtual(spawnInfo->behaviorScript);

        if ((spawnInfo->behaviorArg & (RESPAWN_INFO_DONT_RESPAWN << 8))
            != (RESPAWN_INFO_DONT_RESPAWN << 8)) {
            object = create_object(script);

            object->oBhvParams = spawnInfo->behaviorArg;

            object->oBhvParams2ndByte = ((spawnInfo->behaviorArg) >> 16) & 0xFF;

            object->behavior = script;
            object->unused1 = 0;

            object->respawnInfoType = RESPAWN_INFO_TYPE_32;
            object->respawnInfo = &spawnInfo->behaviorArg;

            if (spawnInfo->behaviorArg & 0x01) {
                gMarioObject = object;
                geo_make_first_child(&object->header.gfx.node);
            }

            geo_obj_init_spawninfo(&object->header.gfx, spawnInfo);

            object->oPosX = spawnInfo->startPos[0];
            object->oPosY = spawnInfo->startPos[1];
            object->oPosZ = spawnInfo->startPos[2];

            object->oFaceAnglePitch = spawnInfo->startAngle[0];
            object->oFaceAngleYaw = spawnInfo->startAngle[1];
            object->oFaceAngleRoll = spawnInfo->startAngle[2];

            object->oMoveAnglePitch = spawnInfo->startAngle[0];
            object->oMoveAngleYaw = spawnInfo->startAngle[1];
            object->oMoveAngleRoll = spawnInfo->startAngle[2];
        }

        spawnInfo = spawnInfo->next;
    }
}

void stub_obj_list_processor_1(void) {
}

void clear_objects(void) {
    s32 i;

    gTHIWaterDrained = 0;
    gTimeStopState = 0;
    gMarioObject = NULL;
    gMarioCurrentRoom = 0;

    for (i = 0; i < 60; i++) {
        gDoorAdjacentRooms[i][0] = 0;
        gDoorAdjacentRooms[i][1] = 0;
    }

    debug_unknown_level_select_check();

    init_free_object_list();
    clear_object_lists(gObjectListArray);

    stub_behavior_script_2();
    stub_obj_list_processor_1();

    for (i = 0; i < OBJECT_POOL_CAPACITY; i++) {
        gObjectPool[i].activeFlags = ACTIVE_FLAG_DEACTIVATED;
        geo_reset_object_node(&gObjectPool[i].header.gfx);
    }

    gObjectMemoryPool = mem_pool_init(0x800, MEMORY_POOL_LEFT);
    gObjectLists = gObjectListArray;

    clear_dynamic_surfaces();
}

void update_terrain_objects(void) {
    gObjectCounter = update_objects_in_list(&gObjectLists[OBJ_LIST_SPAWNER]);

    gObjectCounter = update_objects_in_list(&gObjectLists[OBJ_LIST_SURFACE]);
}

void update_non_terrain_objects(void) {
    UNUSED u8 filler[4];
    s32 listIndex;

    s32 i = 2;
    while ((listIndex = sObjectListUpdateOrder[i]) != -1) {
        gObjectCounter += update_objects_in_list(&gObjectLists[listIndex]);
        i++;
    }
}

void unload_deactivated_objects(void) {
    UNUSED u8 filler[4];
    s32 listIndex;

    s32 i = 0;
    while ((listIndex = sObjectListUpdateOrder[i]) != -1) {
        unload_deactivated_objects_in_list(&gObjectLists[listIndex]);
        i++;
    }

    gTimeStopState &= ~TIME_STOP_UNKNOWN_0;
}

UNUSED static u16 unused_get_elapsed_time(u64 *cycleCounts, s32 index) {
    u16 time;
    f64 cycles;

    cycles = cycleCounts[index] - cycleCounts[index - 1];
    if (cycles < 0) {
        cycles = 0;
    }

    time = (u16)(((u64) cycles * 1000000 / osClockRate) / 16667.0 * 1000.0);
    if (time > 999) {
        time = 999;
    }

    return time;
}

void update_objects(UNUSED s32 unused) {
    s64 cycleCounts[30];

    cycleCounts[0] = get_current_clock();

    gTimeStopState &= ~TIME_STOP_MARIO_OPENED_DOOR;

    gNumRoomedObjectsInMarioRoom = 0;
    gNumRoomedObjectsNotInMarioRoom = 0;
    gCheckingSurfaceCollisionsForCamera = FALSE;

    reset_debug_objectinfo();
    stub_debug_5();

    gObjectLists = gObjectListArray;

    cycleCounts[1] = get_clock_difference(cycleCounts[0]);
    clear_dynamic_surfaces();

    cycleCounts[2] = get_clock_difference(cycleCounts[0]);
    update_terrain_objects();

    apply_mario_platform_displacement();

    cycleCounts[3] = get_clock_difference(cycleCounts[0]);
    detect_object_collisions();

    cycleCounts[4] = get_clock_difference(cycleCounts[0]);
    update_non_terrain_objects();

    cycleCounts[5] = get_clock_difference(cycleCounts[0]);
    unload_deactivated_objects();

    cycleCounts[6] = get_clock_difference(cycleCounts[0]);
    update_mario_platform();

    cycleCounts[7] = get_clock_difference(cycleCounts[0]);

    cycleCounts[0] = 0;
    try_print_debug_mario_object_info();

    if (gTimeStopState & TIME_STOP_ENABLED) {
        gTimeStopState |= TIME_STOP_ACTIVE;
    } else {
        gTimeStopState &= ~TIME_STOP_ACTIVE;
    }

    gPrevFrameObjectCount = gObjectCounter;
}
