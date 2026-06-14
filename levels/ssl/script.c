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
#include "levels/ssl/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_SSL_PYRAMID_TOP,  -2047, 1536, -1023,  0, 0, 0,  0,  bhvPyramidTop),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT          ( MODEL_SSL_TOX_BOX,      -1284,    0, -5895,  0, 0, 0,  BPARAM2(TOX_BOX_BP_MOVEMENT_PATTERN_1),  bhvToxBox),
    OBJECT          ( MODEL_SSL_TOX_BOX,       1283,    0, -4865,  0, 0, 0,  BPARAM2(TOX_BOX_BP_MOVEMENT_PATTERN_2),  bhvToxBox),
    OBJECT          ( MODEL_SSL_TOX_BOX,       4873,    0, -3335,  0, 0, 0,  BPARAM2(TOX_BOX_BP_MOVEMENT_PATTERN_3),  bhvToxBox),
    OBJECT          ( MODEL_TWEESTER,         -3600, -200,  2940,  0, 0, 0,  BPARAM2(0x12),  bhvTweester),
    OBJECT_WITH_ACTS( MODEL_TWEESTER,          1017, -200,  3832,  0, 0, 0,  BPARAM2(0x19),  bhvTweester,  ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_TWEESTER,          3066, -200,   400,  0, 0, 0,  BPARAM2(0x19),  bhvTweester,  ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_KLEPTO,            2200, 1174, -2820,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_1) | BPARAM2(0x01),  bhvKlepto,    ACT_1),
    OBJECT_WITH_ACTS( MODEL_KLEPTO,           -5963,  573, -4784,  0, 0, 0,  0,  bhvKlepto,    ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT_WITH_ACTS( MODEL_STAR,  -2050, 1200, -580,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_2),  bhvStar,                  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_NONE,   6000,  800, 3500,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_5),  bhvHiddenRedCoinStar,  ALL_ACTS),
    RETURN(),
};

static const LevelScript script_func_local_4[] = {
    OBJECT( MODEL_NONE,                      2867,  640,  2867,  0,   0, 0,  BPARAM2(77),  bhvPoleGrabbing),
    OBJECT( MODEL_NONE,                         0, 3200,  1331,  0,   0, 0,  BPARAM2(92),  bhvPoleGrabbing),
    OBJECT( MODEL_SSL_GRINDEL,               3297,    0,    95,  0,   0, 0,  BPARAM2(28),  bhvGrindel),
    OBJECT( MODEL_SSL_GRINDEL,               -870, 3840,   105,  0, 180, 0,  0,  bhvHorizontalGrindel),
    OBJECT( MODEL_SSL_GRINDEL,              -3362,    0, -1385,  0,   0, 0,  0,  bhvHorizontalGrindel),
    OBJECT( MODEL_SSL_SPINDEL,              -2458, 2109, -1430,  0,   0, 0,  0,  bhvSpindel),
    OBJECT( MODEL_SSL_MOVING_PYRAMID_WALL,    858, 1927, -2307,  0,   0, 0,  BPARAM2(PYRAMID_WALL_BP_POSITION_HIGH),    bhvSSLMovingPyramidWall),
    OBJECT( MODEL_SSL_MOVING_PYRAMID_WALL,    730, 1927, -2307,  0,   0, 0,  BPARAM2(PYRAMID_WALL_BP_POSITION_MIDDLE),  bhvSSLMovingPyramidWall),
    OBJECT( MODEL_SSL_MOVING_PYRAMID_WALL,   1473, 2567, -2307,  0,   0, 0,  BPARAM2(PYRAMID_WALL_BP_POSITION_MIDDLE),  bhvSSLMovingPyramidWall),
    OBJECT( MODEL_SSL_MOVING_PYRAMID_WALL,   1345, 2567, -2307,  0,   0, 0,  BPARAM2(PYRAMID_WALL_BP_POSITION_LOW),     bhvSSLMovingPyramidWall),
    OBJECT( MODEL_SSL_PYRAMID_ELEVATOR,         0, 4966,   256,  0,   0, 0,  0,  bhvPyramidElevator),
    OBJECT( MODEL_NONE,                      1198, -133,  2396,  0,   0, 0,  0,  bhvSandSoundLoop),
    OBJECT( MODEL_NONE,                         7, 1229,  -708,  0,   0, 0,  0,  bhvSandSoundLoop),
    OBJECT( MODEL_NONE,                         7, 4317,  -708,  0,   0, 0,  0,  bhvSandSoundLoop),
    RETURN(),
};

static const LevelScript script_func_local_5[] = {
    OBJECT_WITH_ACTS( MODEL_STAR,  500, 5050, -500,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_3),  bhvStar,         ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_NONE,  900, 1400, 2350,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_6) | BPARAM2(0x04),  bhvHiddenStar,  ALL_ACTS),
    RETURN(),
};

static const LevelScript script_func_local_6[] = {
    OBJECT( MODEL_NONE,  0, -1534, -3693,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_4),  bhvEyerokBoss),
    RETURN(),
};

const LevelScript level_ssl_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _ssl_segment_7SegmentRomStart, _ssl_segment_7SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _ssl_skybox_mio0SegmentRomStart, _ssl_skybox_mio0SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _generic_mio0SegmentRomStart, _generic_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group5_mio0SegmentRomStart, _group5_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group5_geoSegmentRomStart,  _group5_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_6),
    LOAD_MODEL_FROM_GEO(MODEL_SSL_PALM_TREE,           palm_tree_geo),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03,       ssl_geo_0005C0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_04,       ssl_geo_0005D8),
    LOAD_MODEL_FROM_GEO(MODEL_SSL_PYRAMID_TOP,         ssl_geo_000618),
    LOAD_MODEL_FROM_GEO(MODEL_SSL_GRINDEL,             ssl_geo_000734),
    LOAD_MODEL_FROM_GEO(MODEL_SSL_SPINDEL,             ssl_geo_000764),
    LOAD_MODEL_FROM_GEO(MODEL_SSL_MOVING_PYRAMID_WALL, ssl_geo_000794),
    LOAD_MODEL_FROM_GEO(MODEL_SSL_PYRAMID_ELEVATOR,    ssl_geo_0007AC),
    LOAD_MODEL_FROM_GEO(MODEL_SSL_TOX_BOX,             ssl_geo_000630),

    AREA( 1, ssl_geo_000648),
        OBJECT( MODEL_NONE,    653, 1038,  6566,  0,  90, 0,  BPARAM2(WARP_NODE_0A),  bhvSpinAirborneWarp),
        OBJECT( MODEL_NONE,  -2048,    0,    56,  0,   0, 0,  BPARAM2(WARP_NODE_14),  bhvWarp),
        OBJECT( MODEL_NONE,  -2048,  768, -1024,  0,   0, 0,  BPARAM1(15) | BPARAM2(WARP_NODE_1E),  bhvWarp),
        OBJECT( MODEL_NONE,   6930,    0, -4871,  0, 159, 0,  BPARAM2(WARP_NODE_1F),  bhvFadingWarp),
        OBJECT( MODEL_NONE,  -5943,    0, -4903,  0,  49, 0,  BPARAM2(WARP_NODE_20),  bhvFadingWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_SSL,     1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_14,       LEVEL_SSL,     2,  WARP_NODE_0A,  WARP_CHECKPOINT),
        WARP_NODE( WARP_NODE_1E,       LEVEL_SSL,     2,  WARP_NODE_14,  WARP_CHECKPOINT),
        WARP_NODE( WARP_NODE_1F,       LEVEL_SSL,     1,  WARP_NODE_20,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_20,       LEVEL_SSL,     1,  WARP_NODE_1F,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  3,  WARP_NODE_33,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  3,  WARP_NODE_65,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        TERRAIN( ssl_seg7_area_1_collision),
        MACRO_OBJECTS( ssl_seg7_area_1_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0000,  SEQ_LEVEL_HOT),
        TERRAIN_TYPE( TERRAIN_SAND),
    END_AREA(),

    AREA( 2, ssl_geo_0007CC),
        OBJECT( MODEL_NONE,     0,  300,  6451,  0, 180, 0,  BPARAM2(WARP_NODE_0A),  bhvAirborneWarp),
        OBJECT( MODEL_NONE,     0, 5500,   256,  0, 180, 0,  BPARAM2(WARP_NODE_14),  bhvAirborneWarp),
        OBJECT( MODEL_NONE,  3070, 1280,  2900,  0, 180, 0,  BPARAM2(WARP_NODE_15),  bhvFadingWarp),
        OBJECT( MODEL_NONE,  2546, 1150, -2647,  0,  78, 0,  BPARAM2(WARP_NODE_16),  bhvFadingWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_SSL,     2,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_14,       LEVEL_SSL,     2,  WARP_NODE_14,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_15,       LEVEL_SSL,     2,  WARP_NODE_16,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_16,       LEVEL_SSL,     2,  WARP_NODE_15,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  3,  WARP_NODE_33,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  3,  WARP_NODE_65,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_4),
        JUMP_LINK(script_func_local_5),
        INSTANT_WARP( 3,  3,  0, 0, 0),
        TERRAIN( ssl_seg7_area_2_collision),
        MACRO_OBJECTS( ssl_seg7_area_2_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0004,  SEQ_LEVEL_UNDERGROUND),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    AREA( 3, ssl_geo_00088C),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  3,  WARP_NODE_33,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  3,  WARP_NODE_65,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_6),
        TERRAIN( ssl_seg7_area_3_collision),
        MACRO_OBJECTS( ssl_seg7_area_3_macro_objs),
        INSTANT_WARP( 2,  2,  0, 0, 0),
        SET_BACKGROUND_MUSIC( 0x0004,  SEQ_LEVEL_UNDERGROUND),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  88,  653, 38, 6566),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
