#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "dialog_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "game/level_update.h"

#include "levels/scripts.h"

#include "actors/common1.h"

#include "make_const_nonconst.h"
#include "levels/bbh/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_RED_FLAME,                    2089,  1331, -1125,  0, 270, 0,  0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,                    1331,  1075, -1330,  0, 90, 0,   0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,                    2089,  1331,  -511,  0, 270, 0,  0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,                    -511,   358, -1330,  0, 90, 0,   0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,                    1126,   358,  2212,  0, 0, 0,    0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,                     205,   358,  2212,  0, 0, 0,    0,  bhvFlame),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT( MODEL_BBH_TILTING_FLOOR_PLATFORM,   2866,   820,  1897,  0, 0, 0,    0,  bhvBBHTiltingTrapPlatform),
    OBJECT( MODEL_BBH_TUMBLING_PLATFORM,        2961,     0,  -768,  0, 0, 0,    0,  bhvBBHTumblingBridge),
    OBJECT( MODEL_BBH_MOVING_BOOKSHELF,        -1994,   819,   213,  0, 0, 0,    0,  bhvHauntedBookshelf),
    OBJECT( MODEL_BBH_MESH_ELEVATOR,           -2985,  -205,  5400,  0, -45, 0,  0,  bhvMeshElevator),
    OBJECT( MODEL_BBH_MERRY_GO_ROUND,           -205, -2560,   205,  0, 0, 0,    0,  bhvMerryGoRound),
    OBJECT( MODEL_NONE,                         2200,   819,  -800,  0, 0, 0,    0,  bhvCoffinSpawner),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT_WITH_ACTS( MODEL_BOO,                          1000,    50,  1000,  0, 0, 0,    BPARAM1(STAR_INDEX_ACT_1) | BPARAM2(BIG_BOO_BP_GHOST_HUNT),  bhvGhostHuntBigBoo,        ACT_1),
    OBJECT_WITH_ACTS( MODEL_BOO,                            20,   100,  -908,  0, 0, 0,    BPARAM2(BOO_BP_GHOST_HUNT),  bhvGhostHuntBoo,                  ACT_1),
    OBJECT_WITH_ACTS( MODEL_BOO,                          3150,   100,   398,  0, 0, 0,    BPARAM2(BOO_BP_GHOST_HUNT),  bhvGhostHuntBoo,                  ACT_1),
    OBJECT_WITH_ACTS( MODEL_BOO,                         -2000,   150,  -800,  0, 0, 0,    BPARAM2(BOO_BP_GHOST_HUNT),  bhvGhostHuntBoo,                  ACT_1),
    OBJECT_WITH_ACTS( MODEL_BOO,                          2851,   100,  2289,  0, 0, 0,    BPARAM2(BOO_BP_GHOST_HUNT),  bhvGhostHuntBoo,                  ACT_1),
    OBJECT_WITH_ACTS( MODEL_BOO,                         -1551,   100, -1018,  0, 0, 0,    BPARAM2(BOO_BP_GHOST_HUNT),  bhvGhostHuntBoo,                  ACT_1),
    OBJECT_WITH_ACTS( MODEL_BBH_STAIRCASE_STEP,            973,     0,   517,  0, 0, 0,    0,  bhvHiddenStaircaseStep,  ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_BBH_STAIRCASE_STEP,            973,  -206,   717,  0, 0, 0,    0,  bhvHiddenStaircaseStep,  ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_BBH_STAIRCASE_STEP,            973,  -412,   917,  0, 0, 0,    0,  bhvHiddenStaircaseStep,  ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_BOO,                            20,   100,  -908,  0, 0, 0,    0,  bhvBoo,                  ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_BOO,                          3150,   100,   398,  0, 0, 0,    0,  bhvBoo,                  ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_BOO,                         -2000,   150,  -800,  0, 0, 0,    0,  bhvBoo,                  ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_BOO,                          2851,   100,  2289,  0, 0, 0,    0,  bhvBoo,                  ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_BOO,                         -1551,   100, -1018,  0, 0, 0,    0,  bhvBoo,                  ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_NONE,                          990, -2146,  -908,  0, -45, 0,  BPARAM2(0x03),  bhvFlamethrower,           ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_NONE,                        -1100, -2372,  1100,  0, 135, 0,  BPARAM1(STAR_INDEX_ACT_2),  bhvMerryGoRoundBooManager,          ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_BOO,                          1030,  1922,  2546,  0, -90, 0,  BPARAM1(STAR_INDEX_ACT_5),  bhvBalconyBigBoo,         ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_BOO,                           581,  1850,  -206,  0, -90, 0,  0,  bhvBoo,                  ALL_ACTS),
    OBJECT          ( MODEL_MAD_PIANO,                   -1300,     0,  2310,  0, 243, 0,  0,  bhvMadPiano),
    OBJECT          ( MODEL_HAUNTED_CHAIR,               -1530,     0,  2200,  0, 66, 0,   0,  bhvHauntedChair),
    OBJECT          ( MODEL_NONE,                        -1330,   890,   200,  0, 90, 0,   0,  bhvBookendSpawn),
    OBJECT          ( MODEL_NONE,                         -818,   890,  -200,  0, 270, 0,  0,  bhvBookendSpawn),
    OBJECT          ( MODEL_NONE,                        -1330,   890,  -622,  0, 90, 0,   0,  bhvBookendSpawn),
    OBJECT          ( MODEL_NONE,                         -818,   890,  -686,  0, 270, 0,  0,  bhvBookendSpawn),
    OBJECT          ( MODEL_NONE,                        -1950,   880,     8,  0, 180, 0,  0,  bhvHauntedBookshelfManager),
    OBJECT          ( MODEL_BOOKEND,                      2680,  1045,   876,  0, 166, 0,  0,  bhvFlyingBookend),
    OBJECT          ( MODEL_BOOKEND,                      3075,  1045,   995,  0, 166, 0,  0,  bhvFlyingBookend),
    OBJECT          ( MODEL_BOOKEND,                     -1411,   218,   922,  0, 180, 0,  0,  bhvFlyingBookend),
    RETURN(),
};

static const LevelScript script_func_local_4[] = {
    OBJECT_WITH_ACTS( MODEL_STAR,  -2030, 1350,  1940,  0, 0, 0,   BPARAM1(STAR_INDEX_ACT_3),  bhvStar,                     ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_NONE,   -204, 1100,  1576,  0, 0, 0,   BPARAM1(STAR_INDEX_ACT_4),  bhvHiddenRedCoinStar,     ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_NONE,    923, 1741,  -332,  0, 18, 0,  BPARAM1(STAR_INDEX_ACT_6) | BPARAM2(0x01),  bhvMrI,                     ALL_ACTS),
    RETURN(),
};

const LevelScript level_bbh_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _bbh_segment_7SegmentRomStart, _bbh_segment_7SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _bbh_skybox_mio0SegmentRomStart, _bbh_skybox_mio0SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _spooky_mio0SegmentRomStart, _spooky_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group9_mio0SegmentRomStart, _group9_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group9_geoSegmentRomStart,  _group9_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x06, _group17_mio0SegmentRomStart, _group17_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0D, _group17_geoSegmentRomStart, _group17_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_10),
    JUMP_LINK(script_func_global_18),
    LOAD_MODEL_FROM_GEO(MODEL_BBH_HAUNTED_DOOR,           haunted_door_geo),
    LOAD_MODEL_FROM_GEO(MODEL_BBH_STAIRCASE_STEP,         geo_bbh_0005B0),
    LOAD_MODEL_FROM_GEO(MODEL_BBH_TILTING_FLOOR_PLATFORM, geo_bbh_0005C8),
    LOAD_MODEL_FROM_GEO(MODEL_BBH_TUMBLING_PLATFORM,      geo_bbh_0005E0),
    LOAD_MODEL_FROM_GEO(MODEL_BBH_TUMBLING_PLATFORM_PART, geo_bbh_0005F8),
    LOAD_MODEL_FROM_GEO(MODEL_BBH_MOVING_BOOKSHELF,       geo_bbh_000610),
    LOAD_MODEL_FROM_GEO(MODEL_BBH_MESH_ELEVATOR,          geo_bbh_000628),
    LOAD_MODEL_FROM_GEO(MODEL_BBH_MERRY_GO_ROUND,         geo_bbh_000640),
    LOAD_MODEL_FROM_GEO(MODEL_BBH_WOODEN_TOMB,            geo_bbh_000658),

    AREA( 1, geo_bbh_000F00),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        JUMP_LINK(script_func_local_4),
        OBJECT( MODEL_NONE,  666, 796, 5350,  0, 180, 0,  BPARAM2(WARP_NODE_0A),  bhvSpinAirborneWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_BBH,               1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE_COURTYARD,  1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE_COURTYARD,  1,  WARP_NODE_0B,  WARP_NO_CHECKPOINT),
        TERRAIN( bbh_seg7_collision_level),
        MACRO_OBJECTS( bbh_seg7_macro_objs),
        ROOMS( bbh_seg7_rooms),
        SHOW_DIALOG( 0x00, DIALOG_098),
        SET_BACKGROUND_MUSIC( 0x0006,  SEQ_LEVEL_SPOOKY),
        TERRAIN_TYPE( TERRAIN_SPOOKY),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  180,  666, -204, 5350),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
