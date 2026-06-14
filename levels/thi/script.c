#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "game/level_update.h"

#include "levels/scripts.h"

#include "actors/common1.h"

#include "make_const_nonconst.h"
#include "levels/thi/header.h"

static const LevelScript script_func_local_1[] = {
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT_WITH_ACTS( MODEL_NONE,         0, -700, -4500,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_4),  bhvHiddenStar,           ALL_ACTS),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT_WITH_ACTS( MODEL_NONE,              -1800,   800, -1500,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_5),  bhvHiddenRedCoinStar,  ALL_ACTS),
    OBJECT          ( MODEL_WIGGLER_HEAD,         17,  1843,   -62,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_6),  bhvWigglerHead),
    RETURN(),
};

static const LevelScript script_func_local_4[] = {
    OBJECT_WITH_ACTS( MODEL_KOOPA_WITH_SHELL,  -1900,  -511,  2400,  0, -30, 0,  BPARAM1(STAR_INDEX_ACT_3) | BPARAM2(KOOPA_BP_KOOPA_THE_QUICK_THI),  bhvKoopa,              ACT_3),
    OBJECT_WITH_ACTS( MODEL_NONE,               7400, -1537, -6300,  0,   0, 0,  0,  bhvKoopaRaceEndpoint,  ACT_3),
    OBJECT          ( MODEL_NONE,              -6556, -2969,  6565,  0,   0, 0,  BPARAM2(GOOMBA_TRIPLET_SPAWNER_BP_EXTRA_GOOMBAS(0) | GOOMBA_SIZE_HUGE),  bhvGoombaTripletSpawner),
    OBJECT          ( MODEL_GOOMBA,             6517, -2559,  4327,  0,   0, 0,  BPARAM2(GOOMBA_SIZE_HUGE),  bhvGoomba),
    OBJECT          ( MODEL_PIRANHA_PLANT,     -6336, -2047, -3861,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_1) | BPARAM2(0x01),  bhvFirePiranhaPlant),
    OBJECT          ( MODEL_PIRANHA_PLANT,     -5740, -2047, -6578,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_1) | BPARAM2(0x01),  bhvFirePiranhaPlant),
    OBJECT          ( MODEL_PIRANHA_PLANT,     -6481, -2047, -5998,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_1) | BPARAM2(0x01),  bhvFirePiranhaPlant),
    OBJECT          ( MODEL_PIRANHA_PLANT,     -5577, -2047, -4961,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_1) | BPARAM2(0x01),  bhvFirePiranhaPlant),
    OBJECT          ( MODEL_PIRANHA_PLANT,     -6865, -2047, -4568,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_1) | BPARAM2(0x01),  bhvFirePiranhaPlant),
    OBJECT          ( MODEL_NONE,              -4413,   204, -2140,  0,   0, 0,  BPARAM2(BBALL_BP_STYPE_THI_LARGE),  bhvTHIBowlingBallSpawner),
    OBJECT          ( MODEL_BUBBA,             -6241, -3300,  -716,  0,   0, 0,  0,  bhvBubba),
    OBJECT          ( MODEL_BUBBA,              1624, -3300,  8142,  0,   0, 0,  0,  bhvBubba),
    RETURN(),
};

static const LevelScript script_func_local_5[] = {
    OBJECT( MODEL_THI_HUGE_ISLAND_TOP,      0, 3891, -1533,  0, 0, 0,  0,  bhvTHIHugeIslandTop),
    RETURN(),
};

static const LevelScript script_func_local_6[] = {
    OBJECT( MODEL_THI_TINY_ISLAND_TOP,      0, 1167,  -460,  0, 0, 0,  0,  bhvTHITinyIslandTop),
    OBJECT( MODEL_NONE,                 -1382,   80,  -649,  0, 0, 0,  BPARAM2(BBALL_BP_STYPE_THI_SMALL),  bhvTHIBowlingBallSpawner),
    RETURN(),
};

static const LevelScript script_func_local_7[] = {
    OBJECT( MODEL_THI_WARP_PIPE,   6656, -1536, -5632,  0, 0, 0,  BPARAM2(WARP_NODE_32),  bhvWarpPipe),
    OBJECT( MODEL_THI_WARP_PIPE,  -5888, -2048, -5888,  0, 0, 0,  BPARAM2(WARP_NODE_33),  bhvWarpPipe),
    OBJECT( MODEL_THI_WARP_PIPE,  -3072,   512, -3840,  0, 0, 0,  BPARAM2(WARP_NODE_34),  bhvWarpPipe),
    WARP_NODE( WARP_NODE_32,  LEVEL_THI,  2,  WARP_NODE_32,  WARP_NO_CHECKPOINT),
    WARP_NODE( WARP_NODE_33,  LEVEL_THI,  2,  WARP_NODE_33,  WARP_NO_CHECKPOINT),
    WARP_NODE( WARP_NODE_34,  LEVEL_THI,  2,  WARP_NODE_34,  WARP_NO_CHECKPOINT),
    RETURN(),
};

static const LevelScript script_func_local_8[] = {
    OBJECT( MODEL_THI_WARP_PIPE,   1997, -461, -1690,  0, 0, 0,  BPARAM2(WARP_NODE_32),  bhvWarpPipe),
    OBJECT( MODEL_THI_WARP_PIPE,  -1766, -614, -1766,  0, 0, 0,  BPARAM2(WARP_NODE_33),  bhvWarpPipe),
    OBJECT( MODEL_THI_WARP_PIPE,   -922,  154, -1152,  0, 0, 0,  BPARAM2(WARP_NODE_34),  bhvWarpPipe),
    WARP_NODE( WARP_NODE_32,  LEVEL_THI,  1,  WARP_NODE_32,  WARP_NO_CHECKPOINT),
    WARP_NODE( WARP_NODE_33,  LEVEL_THI,  1,  WARP_NODE_33,  WARP_NO_CHECKPOINT),
    WARP_NODE( WARP_NODE_34,  LEVEL_THI,  1,  WARP_NODE_34,  WARP_NO_CHECKPOINT),
    RETURN(),
};

const LevelScript level_thi_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _thi_segment_7SegmentRomStart, _thi_segment_7SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _grass_mio0SegmentRomStart, _grass_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _water_skybox_mio0SegmentRomStart, _water_skybox_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group11_mio0SegmentRomStart, _group11_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group11_geoSegmentRomStart,  _group11_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x06, _group14_mio0SegmentRomStart, _group14_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0D, _group14_geoSegmentRomStart,  _group14_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_12),
    JUMP_LINK(script_func_global_15),
    LOAD_MODEL_FROM_GEO(MODEL_THI_BUBBLY_TREE,     bubbly_tree_geo),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03,   thi_geo_0005F0),
    LOAD_MODEL_FROM_GEO(MODEL_THI_WARP_PIPE,       warp_pipe_geo),
    LOAD_MODEL_FROM_GEO(MODEL_THI_HUGE_ISLAND_TOP, thi_geo_0005B0),
    LOAD_MODEL_FROM_GEO(MODEL_THI_TINY_ISLAND_TOP, thi_geo_0005C8),

    AREA( 1, thi_geo_000608),
        OBJECT( MODEL_NONE,  -7372, -1969,  7373,  0, 149, 0,  BPARAM2(WARP_NODE_0A),  bhvSpinAirborneWarp),
        OBJECT( MODEL_NONE,    410,  -512,   922,  0,   0, 0,  BPARAM2(WARP_NODE_0B),  bhvInstantActiveWarp),
        OBJECT( MODEL_NONE,    410,  -512,   717,  0,   0, 0,  BPARAM1(5)  | BPARAM2(WARP_NODE_0C),  bhvWarp),
        OBJECT( MODEL_NONE,      0,  3170, -1570,  0,   0, 0,  BPARAM1(10) | BPARAM2(WARP_NODE_0D),  bhvWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_THI,     1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0B,       LEVEL_THI,     1,  WARP_NODE_0B,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0C,       LEVEL_THI,     3,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0D,       LEVEL_THI,     3,  WARP_NODE_0B,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  2,  WARP_NODE_37,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  2,  WARP_NODE_69,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_7),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_5),
        JUMP_LINK(script_func_local_4),
        TERRAIN( thi_seg7_area_1_collision),
        MACRO_OBJECTS( thi_seg7_area_1_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0000,  SEQ_LEVEL_GRASS),
        TERRAIN_TYPE( TERRAIN_GRASS),
    END_AREA(),

    AREA( 2, thi_geo_0006D4),
        OBJECT( MODEL_NONE,  -2211,  110,  2212,  0,  149, 0,  BPARAM2(WARP_NODE_0A),  bhvSpinAirborneWarp),
        OBJECT( MODEL_NONE,    280, -767, -4180,  0,    0, 0,  BPARAM2(WARP_NODE_0B),  bhvFadingWarp),
        OBJECT( MODEL_NONE,  -1638,    0, -1988,  0, -126, 0,  BPARAM2(WARP_NODE_0C),  bhvFadingWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_THI,     2,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0B,       LEVEL_THI,     2,  WARP_NODE_0C,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0C,       LEVEL_THI,     2,  WARP_NODE_0B,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  2,  WARP_NODE_33,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  2,  WARP_NODE_65,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_8),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_6),
        TERRAIN( thi_seg7_area_2_collision),
        MACRO_OBJECTS( thi_seg7_area_2_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0000,  SEQ_LEVEL_GRASS),
        TERRAIN_TYPE( TERRAIN_GRASS),
    END_AREA(),

    AREA( 3, thi_geo_00079C),
        OBJECT( MODEL_NONE,  512, 1024, 2150,  0, 180, 0,  BPARAM2(WARP_NODE_0A),  bhvInstantActiveWarp),
        OBJECT( MODEL_NONE,    0, 3277,    0,  0,   0, 0,  BPARAM2(WARP_NODE_0B),  bhvAirborneWarp),
        OBJECT( MODEL_NONE,  512, 1024, 2355,  0,   0, 0,  BPARAM1(5) | BPARAM2(WARP_NODE_0C),  bhvWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_THI,     3,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0B,       LEVEL_THI,     3,  WARP_NODE_0B,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0C,       LEVEL_THI,     1,  WARP_NODE_0B,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  2,  WARP_NODE_37,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  2,  WARP_NODE_69,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_3),
        TERRAIN( thi_seg7_area_3_collision),
        MACRO_OBJECTS( thi_seg7_area_3_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0004,  SEQ_LEVEL_UNDERGROUND),
        TERRAIN_TYPE( TERRAIN_GRASS),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  149,  -7372, -2969, 7373),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
