#include <ultra64.h>
#ifdef NO_SEGMENTED_MEMORY
#include <string.h>
#endif
#ifdef TARGET_NDS
extern void nds_debug_printf(const char *fmt, ...);
#define DBG(...)
#else
#define DBG(...)
#endif

#include "sm64.h"
#include "audio/external.h"
#include "buffers/framebuffers.h"
#include "buffers/zbuffer.h"
#include "game/area.h"
#include "game/game_init.h"
#include "game/mario.h"
#include "game/memory.h"
#include "game/object_helpers.h"
#include "game/object_list_processor.h"
#include "game/profiler.h"
#include "game/save_file.h"
#include "game/sound_init.h"
#include "goddard/renderer.h"
#include "geo_layout.h"
#include "graph_node.h"
#include "level_script.h"
#include "level_misc_macros.h"
#include "math_util.h"
#include "surface_collision.h"
#include "surface_load.h"
#ifdef TARGET_NDS
#include "nds/nds_overlay.h"
#endif

#define CMD_GET(type, offset) (*(type *) (CMD_PROCESS_OFFSET(offset) + (u8 *) sCurrentCmd))

#define CMD_NEXT ((struct LevelCommand *) ((u8 *) sCurrentCmd + (sCurrentCmd->size << CMD_SIZE_SHIFT)))
#define NEXT_CMD ((struct LevelCommand *) ((sCurrentCmd->size << CMD_SIZE_SHIFT) + (u8 *) sCurrentCmd))

struct LevelCommand {
     u8 type;
     u8 size;

};

enum ScriptStatus { SCRIPT_RUNNING = 1, SCRIPT_PAUSED = 0, SCRIPT_PAUSED2 = -1 };

static uintptr_t sStack[32];

static struct AllocOnlyPool *sLevelPool = NULL;

u8 gNdsModelLoaded[32];
#define NDS_MARK_MODEL_LOADED(id) \
    do { if ((u32) (id) < 256) gNdsModelLoaded[(id) >> 3] |= (u8) (1 << ((id) & 7)); } while (0)

static u16 sDelayFrames = 0;
static u16 sDelayFrames2 = 0;

static s16 sCurrAreaIndex = -1;

static uintptr_t *sStackTop = sStack;
static uintptr_t *sStackBase = NULL;

static s16 sScriptStatus;
static s32 sRegister;
static struct LevelCommand *sCurrentCmd;
static int sLevelScriptDbgCount = 0;

static s32 eval_script_op(s8 op, s32 arg) {
    s32 result = 0;

    switch (op) {
        case 0:
            result = sRegister & arg;
            break;
        case 1:
            result = !(sRegister & arg);
            break;
        case 2:
            result = sRegister == arg;
            break;
        case 3:
            result = sRegister != arg;
            break;
        case 4:
            result = sRegister < arg;
            break;
        case 5:
            result = sRegister <= arg;
            break;
        case 6:
            result = sRegister > arg;
            break;
        case 7:
            result = sRegister >= arg;
            break;
    }

    return result;
}

static void level_cmd_load_and_execute(void) {
    s16 seg = CMD_GET(s16, 2);
    void *entry = CMD_GET(void *, 12);
    DBG("EXECUTE seg=0x%x entry=%p\n", seg, entry);
    main_pool_push_state();
    load_segment(seg, CMD_GET(void *, 4), CMD_GET(void *, 8), MEMORY_POOL_LEFT);

    *sStackTop++ = (uintptr_t) NEXT_CMD;
    *sStackTop++ = (uintptr_t) sStackBase;
    sStackBase = sStackTop;

    sCurrentCmd = segmented_to_virtual(entry);
    DBG("  -> cmd=%p\n", (void *)sCurrentCmd);
}

static void level_cmd_exit_and_execute(void) {
    void *targetAddr = CMD_GET(void *, 12);
    s16 seg = CMD_GET(s16, 2);
    DBG("EXIT_EXEC seg=0x%x entry=%p\n", seg, targetAddr);

    main_pool_pop_state();
    main_pool_push_state();

    load_segment(seg, CMD_GET(void *, 4), CMD_GET(void *, 8),
            MEMORY_POOL_LEFT);

    sStackTop = sStackBase;
    sCurrentCmd = segmented_to_virtual(targetAddr);
    DBG("  -> cmd=%p\n", (void *)sCurrentCmd);
}

static void level_cmd_exit(void) {
    main_pool_pop_state();

    sStackTop = sStackBase;
    sStackBase = (uintptr_t *) *(--sStackTop);
    sCurrentCmd = (struct LevelCommand *) *(--sStackTop);
}

static void level_cmd_sleep(void) {
    sScriptStatus = SCRIPT_PAUSED;

    if (sDelayFrames == 0) {
        sDelayFrames = CMD_GET(s16, 2);
    } else if (--sDelayFrames == 0) {
        sCurrentCmd = CMD_NEXT;
        sScriptStatus = SCRIPT_RUNNING;
    }
}

static void level_cmd_sleep2(void) {
    sScriptStatus = SCRIPT_PAUSED2;

    if (sDelayFrames2 == 0) {
        sDelayFrames2 = CMD_GET(s16, 2);
    } else if (--sDelayFrames2 == 0) {
        sCurrentCmd = CMD_NEXT;
        sScriptStatus = SCRIPT_RUNNING;
    }
}

static void level_cmd_jump(void) {
    sCurrentCmd = segmented_to_virtual(CMD_GET(void *, 4));
}

static void level_cmd_jump_and_link(void) {
    *sStackTop++ = (uintptr_t) NEXT_CMD;
    sCurrentCmd = segmented_to_virtual(CMD_GET(void *, 4));
}

static void level_cmd_return(void) {
    sCurrentCmd = (struct LevelCommand *) *(--sStackTop);
}

static void level_cmd_jump_and_link_push_arg(void) {
    *sStackTop++ = (uintptr_t) NEXT_CMD;
    *sStackTop++ = CMD_GET(s16, 2);
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_jump_repeat(void) {
    s32 val = *(sStackTop - 1);

    if (val == 0) {
        sCurrentCmd = (struct LevelCommand *) *(sStackTop - 2);
    } else if (--val != 0) {
        *(sStackTop - 1) = val;
        sCurrentCmd = (struct LevelCommand *) *(sStackTop - 2);
    } else {
        sCurrentCmd = CMD_NEXT;
        sStackTop -= 2;
    }
}

static void level_cmd_loop_begin(void) {
    *sStackTop++ = (uintptr_t) NEXT_CMD;
    *sStackTop++ = 0;
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_loop_until(void) {
    if (eval_script_op(CMD_GET(u8, 2), CMD_GET(s32, 4)) != 0) {
        sCurrentCmd = CMD_NEXT;
        sStackTop -= 2;
    } else {
        sCurrentCmd = (struct LevelCommand *) *(sStackTop - 2);
    }
}

static void level_cmd_jump_if(void) {
    if (eval_script_op(CMD_GET(u8, 2), CMD_GET(s32, 4)) != 0) {
        sCurrentCmd = segmented_to_virtual(CMD_GET(void *, 8));
    } else {
        sCurrentCmd = CMD_NEXT;
    }
}

static void level_cmd_jump_and_link_if(void) {
    if (eval_script_op(CMD_GET(u8, 2), CMD_GET(s32, 4)) != 0) {
        *sStackTop++ = (uintptr_t) NEXT_CMD;
        sCurrentCmd = segmented_to_virtual(CMD_GET(void *, 8));
    } else {
        sCurrentCmd = CMD_NEXT;
    }
}

static void level_cmd_skip_if(void) {
    if (eval_script_op(CMD_GET(u8, 2), CMD_GET(s32, 4)) == 0) {
        do {
            sCurrentCmd = CMD_NEXT;
        } while (sCurrentCmd->type == 0x0F || sCurrentCmd->type == 0x10);
    }

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_skip(void) {
    do {
        sCurrentCmd = CMD_NEXT;
    } while (sCurrentCmd->type == 0x10);

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_skippable_nop(void) {
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_call(void) {
    typedef s32 (*Func)(s16, s32);
    Func func = CMD_GET(Func, 4);
    sRegister = func(CMD_GET(s16, 2), sRegister);
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_call_loop(void) {
    typedef s32 (*Func)(s16, s32);
    Func func = CMD_GET(Func, 4);
    sRegister = func(CMD_GET(s16, 2), sRegister);

    if (sRegister == 0) {
        sScriptStatus = SCRIPT_PAUSED;
    } else {
        sScriptStatus = SCRIPT_RUNNING;
        sCurrentCmd = CMD_NEXT;
    }
}

static void level_cmd_set_register(void) {
    sRegister = CMD_GET(s16, 2);
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_push_pool_state(void) {
    main_pool_push_state();
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_pop_pool_state(void) {
    main_pool_pop_state();
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_load_to_fixed_address(void) {
    load_to_fixed_pool_addr(CMD_GET(void *, 4), CMD_GET(void *, 8), CMD_GET(void *, 12));
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_load_raw(void) {
    load_segment(CMD_GET(s16, 2), CMD_GET(void *, 4), CMD_GET(void *, 8),
            MEMORY_POOL_LEFT);
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_load_mio0(void) {
    load_segment_decompress(CMD_GET(s16, 2), CMD_GET(void *, 4), CMD_GET(void *, 8));
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_load_mario_head(void) {
#ifndef TARGET_NDS

    void *addr = main_pool_alloc(DOUBLE_SIZE_ON_64_BIT(0xE1000), MEMORY_POOL_LEFT);
    if (addr != NULL) {
        gdm_init(addr, DOUBLE_SIZE_ON_64_BIT(0xE1000));
        gd_add_to_heap(gZBuffer, sizeof(gZBuffer));
        gd_add_to_heap(gFramebuffer0, 3 * sizeof(gFramebuffer0));
        gdm_setup();
        gdm_maketestdl(CMD_GET(s16, 2));
    } else {
        CN_DEBUG_PRINTF(("face anime memory overflow\n"));
    }
#endif

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_load_mio0_texture(void) {
    load_segment_decompress_heap(CMD_GET(s16, 2), CMD_GET(void *, 4), CMD_GET(void *, 8));
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_init_level(void) {
    DBG("INIT_LEVEL\n");
    sLevelScriptDbgCount = 0;
    init_graph_node_start(NULL, (struct GraphNodeStart *) &gObjParentGraphNode);
    clear_objects();
    clear_areas();
    main_pool_push_state();

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_clear_level(void) {
    clear_objects();
    clear_area_graph_nodes();
    clear_areas();
    main_pool_pop_state();

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_alloc_level_pool(void) {
    s32 i;
    if (sLevelPool == NULL) {
        DBG("ALLOC_LVLPOOL avail=%u\n", (unsigned)main_pool_available());
        sLevelPool = alloc_only_pool_init(main_pool_available() - sizeof(struct AllocOnlyPool),
                                          MEMORY_POOL_LEFT);
    }

    for (i = 0; i < 32; i++) {
        gNdsModelLoaded[i] = 0;
    }

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_free_level_pool(void) {
    s32 i;
    DBG("FREE_LVLPOOL used=%u\n", (unsigned)sLevelPool->usedSpace);

    alloc_only_pool_resize(sLevelPool, sLevelPool->usedSpace);
    sLevelPool = NULL;

    for (i = 0; i < 8; i++) {
        if (gAreaData[i].terrainData != NULL) {
            DBG("alloc_surf_pools pool=%u\n", (unsigned)main_pool_available());
            alloc_surface_pools();
            DBG("  done pool=%u\n", (unsigned)main_pool_available());
            break;
        }
    }

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_begin_area(void) {
    u8 areaIndex = CMD_GET(u8, 2);
    void *geoLayoutAddr = CMD_GET(void *, 4);
    DBG("AREA(%d) geo=%p\n", areaIndex, geoLayoutAddr);

    if (areaIndex < 8) {
        struct GraphNodeRoot *screenArea =
            (struct GraphNodeRoot *) process_geo_layout(sLevelPool, geoLayoutAddr);
        DBG("  screenArea=%p\n", (void *)screenArea);
        struct GraphNodeCamera *node = (struct GraphNodeCamera *) screenArea->views[0];

        sCurrAreaIndex = areaIndex;
        screenArea->areaIndex = areaIndex;
        gAreas[areaIndex].unk04 = screenArea;

        if (node != NULL) {
            gAreas[areaIndex].camera = (struct Camera *) node->config.camera;
        } else {
            gAreas[areaIndex].camera = NULL;
        }
    }

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_end_area(void) {
    sCurrAreaIndex = -1;
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_load_model_from_dl(void) {
    s16 val1 = CMD_GET(s16, 2) & 0x0FFF;
    s16 val2 = ((u16)CMD_GET(s16, 2)) >> 12;
    void *val3 = CMD_GET(void *, 4);

    if (val1 < 256) {
        gLoadedGraphNodes[val1] =
            (struct GraphNode *) init_graph_node_display_list(sLevelPool, 0, val2, val3);
        NDS_MARK_MODEL_LOADED(val1);
    }

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_load_model_from_geo(void) {
    s16 arg0 = CMD_GET(s16, 2);
    void *arg1 = CMD_GET(void *, 4);

    if (arg0 < 256) {
        gLoadedGraphNodes[arg0] = process_geo_layout(sLevelPool, arg1);
        NDS_MARK_MODEL_LOADED(arg0);
    }

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_23(void) {
    union {
        s32 i;
        f32 f;
    } arg2;

    s16 model = CMD_GET(s16, 2) & 0x0FFF;
    s16 arg0H = ((u16)CMD_GET(s16, 2)) >> 12;
    void *arg1 = CMD_GET(void *, 4);

    arg2.i = CMD_GET(s32, 8);

    if (model < 256) {

        gLoadedGraphNodes[model] =
            (struct GraphNode *) init_graph_node_scale(sLevelPool, 0, arg0H, arg1, arg2.f);
        NDS_MARK_MODEL_LOADED(model);
    }

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_init_mario(void) {
    DBG("INIT_MARIO\n");
    vec3s_set(gMarioSpawnInfo->startPos, 0, 0, 0);
    vec3s_set(gMarioSpawnInfo->startAngle, 0, 0, 0);

    gMarioSpawnInfo->activeAreaIndex = -1;
    gMarioSpawnInfo->areaIndex = 0;
    gMarioSpawnInfo->behaviorArg = CMD_GET(u32, 4);
    gMarioSpawnInfo->behaviorScript = CMD_GET(void *, 8);
    gMarioSpawnInfo->model = gLoadedGraphNodes[CMD_GET(u8, 3)];
    gMarioSpawnInfo->next = NULL;

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_place_object(void) {
    u8 val7 = 1 << (gCurrActNum - 1);
    u16 model;
    struct SpawnInfo *spawnInfo;

    if (sCurrAreaIndex != -1 && ((CMD_GET(u8, 2) & val7) || CMD_GET(u8, 2) == 0x1F)) {
        model = CMD_GET(u8, 3);
        spawnInfo = alloc_only_pool_alloc(sLevelPool, sizeof(struct SpawnInfo));

        spawnInfo->startPos[0] = CMD_GET(s16, 4);
        spawnInfo->startPos[1] = CMD_GET(s16, 6);
        spawnInfo->startPos[2] = CMD_GET(s16, 8);

        spawnInfo->startAngle[0] = CMD_GET(s16, 10) * 0x8000 / 180;
        spawnInfo->startAngle[1] = CMD_GET(s16, 12) * 0x8000 / 180;
        spawnInfo->startAngle[2] = CMD_GET(s16, 14) * 0x8000 / 180;

        spawnInfo->areaIndex = sCurrAreaIndex;
        spawnInfo->activeAreaIndex = sCurrAreaIndex;

        spawnInfo->behaviorArg = CMD_GET(u32, 16);
        spawnInfo->behaviorScript = CMD_GET(void *, 20);
        spawnInfo->model = gLoadedGraphNodes[model];
        spawnInfo->next = gAreas[sCurrAreaIndex].objectSpawnInfos;

        gAreas[sCurrAreaIndex].objectSpawnInfos = spawnInfo;
    }

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_create_warp_node(void) {
    if (sCurrAreaIndex != -1) {
        struct ObjectWarpNode *warpNode =
            alloc_only_pool_alloc(sLevelPool, sizeof(struct ObjectWarpNode));

        warpNode->node.id = CMD_GET(u8, 2);
        warpNode->node.destLevel = CMD_GET(u8, 3) + CMD_GET(u8, 6);
        warpNode->node.destArea = CMD_GET(u8, 4);
        warpNode->node.destNode = CMD_GET(u8, 5);

        warpNode->object = NULL;

        warpNode->next = gAreas[sCurrAreaIndex].warpNodes;
        gAreas[sCurrAreaIndex].warpNodes = warpNode;
    }

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_create_instant_warp(void) {
    s32 i;
    struct InstantWarp *warp;

    if (sCurrAreaIndex != -1) {
        if (gAreas[sCurrAreaIndex].instantWarps == NULL) {
            gAreas[sCurrAreaIndex].instantWarps =
                alloc_only_pool_alloc(sLevelPool, 4 * sizeof(struct InstantWarp));

            for (i = INSTANT_WARP_INDEX_START; i < INSTANT_WARP_INDEX_STOP; i++) {
                gAreas[sCurrAreaIndex].instantWarps[i].id = 0;
            }
        }

        warp = gAreas[sCurrAreaIndex].instantWarps + CMD_GET(u8, 2);

        warp[0].id = 1;
        warp[0].area = CMD_GET(u8, 3);

        warp[0].displacement[0] = CMD_GET(s16, 4);
        warp[0].displacement[1] = CMD_GET(s16, 6);
        warp[0].displacement[2] = CMD_GET(s16, 8);
    }

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_set_terrain_type(void) {
    if (sCurrAreaIndex != -1) {
        gAreas[sCurrAreaIndex].terrainType |= CMD_GET(s16, 2);
    }

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_create_painting_warp_node(void) {
    s32 i;
    struct WarpNode *node;

    if (sCurrAreaIndex != -1) {
        if (gAreas[sCurrAreaIndex].paintingWarpNodes == NULL) {
            gAreas[sCurrAreaIndex].paintingWarpNodes =
                alloc_only_pool_alloc(sLevelPool, 45 * sizeof(struct WarpNode));

            for (i = 0; i < 45; i++) {
                gAreas[sCurrAreaIndex].paintingWarpNodes[i].id = 0;
            }
        }

        node = &gAreas[sCurrAreaIndex].paintingWarpNodes[CMD_GET(u8, 2)];

        node->id = 1;
        node->destLevel = CMD_GET(u8, 3) + CMD_GET(u8, 6);
        node->destArea = CMD_GET(u8, 4);
        node->destNode = CMD_GET(u8, 5);
    }

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_3A(void) {
    struct UnusedArea28 *val4;

    if (sCurrAreaIndex != -1) {
        if ((val4 = gAreas[sCurrAreaIndex].unused) == NULL) {
            val4 = gAreas[sCurrAreaIndex].unused =
                alloc_only_pool_alloc(sLevelPool, sizeof(struct UnusedArea28));
        }

        val4->unk00 = CMD_GET(s16, 2);
        val4->unk02 = CMD_GET(s16, 4);
        val4->unk04 = CMD_GET(s16, 6);
        val4->unk06 = CMD_GET(s16, 8);
        val4->unk08 = CMD_GET(s16, 10);
    }

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_create_whirlpool(void) {
    struct Whirlpool *whirlpool;
    s32 index = CMD_GET(u8, 2);
    s32 beatBowser2 =
        (save_file_get_flags() & (SAVE_FLAG_HAVE_KEY_2 | SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR)) != 0;

    if (CMD_GET(u8, 3) == 0 || (CMD_GET(u8, 3) == 1 && !beatBowser2)
        || (CMD_GET(u8, 3) == 2 && beatBowser2) || (CMD_GET(u8, 3) == 3 && gCurrActNum >= 2)) {
        if (sCurrAreaIndex != -1 && index < 2) {
            if ((whirlpool = gAreas[sCurrAreaIndex].whirlpools[index]) == NULL) {
                whirlpool = alloc_only_pool_alloc(sLevelPool, sizeof(struct Whirlpool));
                gAreas[sCurrAreaIndex].whirlpools[index] = whirlpool;
            }

            vec3s_set(whirlpool->pos, CMD_GET(s16, 4), CMD_GET(s16, 6), CMD_GET(s16, 8));
            whirlpool->strength = CMD_GET(s16, 10);
        }
    }

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_set_blackout(void) {
    osViBlack(CMD_GET(u8, 2));
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_set_gamma(void) {
    osViSetSpecialFeatures(CMD_GET(u8, 2) == 0 ? OS_VI_GAMMA_OFF : OS_VI_GAMMA_ON);
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_set_terrain_data(void) {
    DBG("SET_TERRAIN area=%d\n", sCurrAreaIndex);
    if (sCurrAreaIndex != -1) {
#ifndef NO_SEGMENTED_MEMORY
        gAreas[sCurrAreaIndex].terrainData = segmented_to_virtual(CMD_GET(void *, 4));
#else
        Collision *data;
        u32 size;

        data = segmented_to_virtual(CMD_GET(void *, 4));
        size = get_area_terrain_size(data) * sizeof(Collision);
        gAreas[sCurrAreaIndex].terrainData = alloc_only_pool_alloc(sLevelPool, size);
        memcpy(gAreas[sCurrAreaIndex].terrainData, data, size);
#endif
    }
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_set_rooms(void) {
    if (sCurrAreaIndex != -1) {
        gAreas[sCurrAreaIndex].surfaceRooms = segmented_to_virtual(CMD_GET(void *, 4));
    }
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_set_macro_objects(void) {
    DBG("SET_MACROS area=%d\n", sCurrAreaIndex);
    if (sCurrAreaIndex != -1) {
#ifndef NO_SEGMENTED_MEMORY
        gAreas[sCurrAreaIndex].macroObjects = segmented_to_virtual(CMD_GET(void *, 4));
#else

        MacroObject *data = segmented_to_virtual(CMD_GET(void *, 4));
        s32 len = 0;
        while (data[len++] != MACRO_OBJECT_END()) {
            len += 4;
        }
        gAreas[sCurrAreaIndex].macroObjects = alloc_only_pool_alloc(sLevelPool, len * sizeof(MacroObject));
        memcpy(gAreas[sCurrAreaIndex].macroObjects, data, len * sizeof(MacroObject));
#endif
    }
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_load_area(void) {
    s16 areaIndex = CMD_GET(u8, 2);
    UNUSED void *unused = (u8 *) sCurrentCmd + 4;
    DBG("LOAD_AREA(%d)\n", areaIndex);

    stop_sounds_in_continuous_banks();
    load_area(areaIndex);

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_unload_area(void) {
    unload_area();
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_set_mario_start_pos(void) {
    gMarioSpawnInfo->areaIndex = CMD_GET(u8, 2);

#if IS_64_BIT
    vec3s_set(gMarioSpawnInfo->startPos, CMD_GET(s16, 6), CMD_GET(s16, 8), CMD_GET(s16, 10));
#else
    vec3s_copy(gMarioSpawnInfo->startPos, CMD_GET(Vec3s, 6));
#endif
    vec3s_set(gMarioSpawnInfo->startAngle, 0, CMD_GET(s16, 4) * 0x8000 / 180, 0);

    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_2C(void) {
    unload_mario_area();
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_2D(void) {
    area_update_objects();
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_set_transition(void) {
    if (gCurrentArea != NULL) {
        play_transition(CMD_GET(u8, 2), CMD_GET(u8, 3), CMD_GET(u8, 4), CMD_GET(u8, 5), CMD_GET(u8, 6));
    }
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_nop(void) {
    CN_DEBUG_PRINTF(("BAD: seqBlankColor\n"));
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_show_dialog(void) {
    if (sCurrAreaIndex != -1) {
        if (CMD_GET(u8, 2) < 2) {
            gAreas[sCurrAreaIndex].dialog[CMD_GET(u8, 2)] = CMD_GET(u8, 3);
        }
    }
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_set_music(void) {
    if (sCurrAreaIndex != -1) {
        gAreas[sCurrAreaIndex].musicParam = CMD_GET(s16, 2);
        gAreas[sCurrAreaIndex].musicParam2 = CMD_GET(s16, 4);
    }
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_set_menu_music(void) {
    set_background_music(0, CMD_GET(s16, 2), 0);
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_38(void) {
    fadeout_music(CMD_GET(s16, 2));
    sCurrentCmd = CMD_NEXT;
}

static void level_cmd_get_or_set_var(void) {
    if (CMD_GET(u8, 2) == 0) {
        switch (CMD_GET(u8, 3)) {
            case 0:
                gCurrSaveFileNum = sRegister;
                break;
            case 1:
                gCurrCourseNum = sRegister;
                break;
            case 2:
                gCurrActNum = sRegister;
                break;
            case 3:
                gCurrLevelNum = sRegister;
                break;
            case 4:
                gCurrAreaIndex = sRegister;
                break;
        }
    } else {
        switch (CMD_GET(u8, 3)) {
            case 0:
                sRegister = gCurrSaveFileNum;
                break;
            case 1:
                sRegister = gCurrCourseNum;
                break;
            case 2:
                sRegister = gCurrActNum;
                break;
            case 3:
                sRegister = gCurrLevelNum;
                break;
            case 4:
                sRegister = gCurrAreaIndex;
                break;
        }
    }

    sCurrentCmd = CMD_NEXT;
}

#ifdef TARGET_NDS
static void level_cmd_execute_overlay(void) {
    s16 levelId = CMD_GET(s16, 2);

    if (!nds_load_level_overlay(levelId)) {
        DBG("EXECUTE_OVERLAY load fail level=%d\n", levelId);
        sCurrentCmd = CMD_NEXT;
        return;
    }

    main_pool_push_state();
    *sStackTop++ = (uintptr_t) NEXT_CMD;
    *sStackTop++ = (uintptr_t) sStackBase;
    sStackBase = sStackTop;

    sCurrentCmd = (struct LevelCommand *) NDS_OVERLAY_BASE;
}
#endif

static void (*LevelScriptJumpTable[])(void) = {
     level_cmd_load_and_execute,
     level_cmd_exit_and_execute,
     level_cmd_exit,
     level_cmd_sleep,
     level_cmd_sleep2,
     level_cmd_jump,
     level_cmd_jump_and_link,
     level_cmd_return,
     level_cmd_jump_and_link_push_arg,
     level_cmd_jump_repeat,
     level_cmd_loop_begin,
     level_cmd_loop_until,
     level_cmd_jump_if,
     level_cmd_jump_and_link_if,
     level_cmd_skip_if,
     level_cmd_skip,
     level_cmd_skippable_nop,
     level_cmd_call,
     level_cmd_call_loop,
     level_cmd_set_register,
     level_cmd_push_pool_state,
     level_cmd_pop_pool_state,
     level_cmd_load_to_fixed_address,
     level_cmd_load_raw,
     level_cmd_load_mio0,
     level_cmd_load_mario_head,
     level_cmd_load_mio0_texture,
     level_cmd_init_level,
     level_cmd_clear_level,
     level_cmd_alloc_level_pool,
     level_cmd_free_level_pool,
     level_cmd_begin_area,
     level_cmd_end_area,
     level_cmd_load_model_from_dl,
     level_cmd_load_model_from_geo,
     level_cmd_23,
     level_cmd_place_object,
     level_cmd_init_mario,
     level_cmd_create_warp_node,
     level_cmd_create_painting_warp_node,
     level_cmd_create_instant_warp,
     level_cmd_load_area,
     level_cmd_unload_area,
     level_cmd_set_mario_start_pos,
     level_cmd_2C,
     level_cmd_2D,
     level_cmd_set_terrain_data,
     level_cmd_set_rooms,
     level_cmd_show_dialog,
     level_cmd_set_terrain_type,
     level_cmd_nop,
     level_cmd_set_transition,
     level_cmd_set_blackout,
     level_cmd_set_gamma,
     level_cmd_set_music,
     level_cmd_set_menu_music,
     level_cmd_38,
     level_cmd_set_macro_objects,
     level_cmd_3A,
     level_cmd_create_whirlpool,
     level_cmd_get_or_set_var,
#ifdef TARGET_NDS
     level_cmd_execute_overlay,
#endif
};

struct LevelCommand *level_script_execute(struct LevelCommand *cmd) {
    sScriptStatus = SCRIPT_RUNNING;
    sCurrentCmd = cmd;

    while (sScriptStatus == SCRIPT_RUNNING) {
        CN_DEBUG_PRINTF(("%08X: ", sCurrentCmd));
        CN_DEBUG_PRINTF(("%02d\n", sCurrentCmd->type));

        if (sLevelScriptDbgCount < 80) {
            DBG("LS[%d] t=%d\n", sLevelScriptDbgCount, sCurrentCmd->type);
        }
        sLevelScriptDbgCount++;

        LevelScriptJumpTable[sCurrentCmd->type]();
    }

    profiler_log_thread5_time(LEVEL_SCRIPT_EXECUTE);
    init_rcp();
    render_game();
    end_master_display_list();
    alloc_display_list(0);

    return sCurrentCmd;
}
