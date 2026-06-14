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
#include "levels/lll/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_LEVEL_GEOMETRY_03,   3840,   0, -5631,  0,   0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_04,   4992,   0,  -639,  0,   0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_05,   7168,   0,  1408,  0,   0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_06,      0,   0,  3712,  0,   0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_07,  -3199,   0,  3456,  0,   0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_08,  -5119,   0, -2047,  0,   0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_0A,      0,   0,     0,  0,   0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_0B,      0,   0,  6272,  0,   0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_0C,   5632,   0,  1408,  0, 270, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_0C,   2048,   0,  3456,  0, 180, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_0C,  -4607,   0,  3456,  0, 270, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_0C,  -5119,   0,  -511,  0,   0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_0D,      0,   0, -2047,  0,   0, 0,  0,  bhvStaticObject),

    OBJECT( MODEL_LEVEL_GEOMETRY_0E,  -5115, 300, -3200,  0,  90, 0,  0,  bhvLLLHexagonalMesh),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT( MODEL_LLL_DRAWBRIDGE_PART,                 -1919,  307,  3648,  0,  0, 0,  0,  bhvLLLDrawbridgeSpawner),
    OBJECT( MODEL_LLL_ROTATING_BLOCK_FIRE_BARS,        -5119,  307, -4095,  0,  0, 0,  0,  bhvLLLRotatingBlockWithFireBars),
    OBJECT( MODEL_LLL_ROTATING_HEXAGONAL_RING,             0,    0,     0,  0,  0, 0,  0,  bhvLLLRotatingHexagonalRing),
    OBJECT( MODEL_LLL_SINKING_RECTANGULAR_PLATFORM,     3968,    0,  1408,  0, 90, 0,  0,  bhvLLLSinkingRectangularPlatform),
    OBJECT( MODEL_LLL_SINKING_RECTANGULAR_PLATFORM,    -5759,    0,  3072,  0,  0, 0,  0,  bhvLLLSinkingRectangularPlatform),
    OBJECT( MODEL_LLL_SINKING_RECTANGULAR_PLATFORM,     2816,    0,   512,  0, 90, 0,  0,  bhvLLLSinkingRectangularPlatform),
    OBJECT( MODEL_LLL_SINKING_RECTANGULAR_PLATFORM,    -1791,    0, -4095,  0, 90, 0,  0,  bhvLLLSinkingRectangularPlatform),
    OBJECT( MODEL_LLL_SINKING_SQUARE_PLATFORMS,         3840,    0, -3199,  0,  0, 0,  0,  bhvLLLSinkingSquarePlatforms),
    OBJECT( MODEL_LLL_TILTING_SQUARE_PLATFORM,           922, -153,  2150,  0,  0, 0,  0,  bhvLLLTiltingInvertedPyramid),
    OBJECT( MODEL_LLL_TILTING_SQUARE_PLATFORM,          1741, -153,  1741,  0,  0, 0,  0,  bhvLLLTiltingInvertedPyramid),
    OBJECT( MODEL_LLL_TILTING_SQUARE_PLATFORM,          1741, -153,  2560,  0,  0, 0,  0,  bhvLLLTiltingInvertedPyramid),
    OBJECT( MODEL_LLL_TILTING_SQUARE_PLATFORM,          2099, -153,  -306,  0,  0, 0,  0,  bhvLLLTiltingInvertedPyramid),
    OBJECT( MODEL_NONE,                                -5119,  102,  1024,  0,  0, 0,  0,  bhvLLLBowserPuzzle),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT( MODEL_LLL_MOVING_OCTAGONAL_MESH_PLATFORM,   1124,    0, -4607,  0,  0, 0,  0,  bhvLLLMovingOctagonalMeshPlatform),
    OBJECT( MODEL_LLL_MOVING_OCTAGONAL_MESH_PLATFORM,   7168,    0,  2432,  0,  0, 0,  BPARAM2(0x01),  bhvLLLMovingOctagonalMeshPlatform),
    OBJECT( MODEL_LLL_SINKING_ROCK_BLOCK,               7168,    0,  7296,  0,  0, 0,  0,  bhvLLLSinkingRockBlock),
    OBJECT( MODEL_LLL_ROLLING_LOG,                      6144,  307,  6016,  0, 90, 0,  0,  bhvLLLRollingLog),
    OBJECT( MODEL_LLL_ROTATING_HEXAGONAL_PLATFORM,     -5119,    0, -4095,  0,  0, 0,  0,  bhvLLLRotatingHexagonalPlatform),
    OBJECT( MODEL_NONE,                                -3583,    0, -4095,  0,  0, 0,  0,  bhvLLLFloatingWoodBridge),
    RETURN(),
};

static const LevelScript script_func_local_4[] = {
    OBJECT          ( MODEL_NONE,             -3199,  307,  3456,  0,   0, 0,  0,  bhvMrI),
    OBJECT          ( MODEL_BULLY_BOSS,           0,  307, -4385,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_1),  bhvBigBully),
    OBJECT          ( MODEL_BULLY_BOSS,        4046, 2234, -5521,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_2),  bhvBigBullyWithMinions),
    OBJECT          ( MODEL_BULLY,            -5119,  307, -2482,  0,   0, 0,  0,  bhvSmallBully),
    OBJECT          ( MODEL_BULLY,                0,  307,  3712,  0,   0, 0,  0,  bhvSmallBully),
    OBJECT          ( MODEL_BULLY,             6813,  307,  1613,  0,   0, 0,  0,  bhvSmallBully),
    OBJECT          ( MODEL_BULLY,             7168,  307,   998,  0,   0, 0,  0,  bhvSmallBully),
    OBJECT          ( MODEL_BULLY,            -5130,  285, -1663,  0,   0, 0,  0,  bhvSmallBully),
    OBJECT          ( MODEL_NONE,                 0,  200, -2048,  0,   0, 0,  0,  bhvFlamethrower),
    OBJECT          ( MODEL_NONE,               500,    2,  5000,  0, 270, 0,  0,  bhvBouncingFireball),
    OBJECT          ( MODEL_NONE,              -700,    2,  4500,  0,  90, 0,  0,  bhvBouncingFireball),
    OBJECT          ( MODEL_NONE,             -6300,    2,  2625,  0,  90, 0,  0,  bhvBouncingFireball),
    OBJECT          ( MODEL_NONE,             -3280,    2, -4854,  0,   0, 0,  0,  bhvBouncingFireball),
    OBJECT          ( MODEL_NONE,              5996,    2,  -390,  0, 315, 0,  0,  bhvBouncingFireball),
    OBJECT          ( MODEL_NONE,              5423,    2, -1991,  0, 315, 0,  0,  bhvBouncingFireball),
    OBJECT          ( MODEL_NONE,              4921,    2, -1504,  0,  90, 0,  0,  bhvBouncingFireball),
    OBJECT_WITH_ACTS( MODEL_EXCLAMATION_BOX,   1050,  550,  6200,  0,   0, 0,  BPARAM2(EXCLAMATION_BOX_BP_KOOPA_SHELL),  bhvExclamationBox,  ACT_5 | ACT_6),
    RETURN(),
};

static const LevelScript script_func_local_5[] = {
    OBJECT_WITH_ACTS( MODEL_NONE,  -4400, 350,  250,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_3),  bhvHiddenRedCoinStar,  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,   3100, 400, 7900,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_4),  bhvStar,                  ALL_ACTS),
    RETURN(),
};

static const LevelScript script_func_local_6[] = {
    OBJECT( MODEL_NONE,                            728, 2606, -2754,  0,   0, 0,  BPARAM2(56),    bhvPoleGrabbing),
    OBJECT( MODEL_NONE,                           1043, 2972, -2679,  0,   0, 0,  BPARAM2(78),    bhvPoleGrabbing),
    OBJECT( MODEL_NONE,                           1078, 3078, -2269,  0,   0, 0,  BPARAM2(102),   bhvPoleGrabbing),
    OBJECT( MODEL_NONE,                           1413, 3222, -2190,  0,   0, 0,  BPARAM2(82),    bhvPoleGrabbing),
    OBJECT( MODEL_NONE,                            783, 1126,   -47,  0,   0, 0,  BPARAM2(102),   bhvPoleGrabbing),
    OBJECT( MODEL_NONE,                            662, 2150,   708,  0,   0, 0,  BPARAM2(102),   bhvPoleGrabbing),
    OBJECT( MODEL_NONE,                           2943,  476,    10,  0, 270, 0,  BPARAM2(0x02),  bhvFlamethrower),
    OBJECT( MODEL_NONE,                          -2759, 2350, -1108,  0,  60, 0,  BPARAM2(0x02),  bhvFlamethrower),
    OBJECT( MODEL_NONE,                          -2472, 2350, -1605,  0,  60, 0,  BPARAM2(0x02),  bhvFlamethrower),
    OBJECT( MODEL_LLL_VOLCANO_FALLING_TRAP,       -485, 1203,  2933,  0,   0, 0,  0,  bhvLLLVolcanoFallingTrap),
    OBJECT( MODEL_LLL_ROTATING_BLOCK_FIRE_BARS,    417, 2150,   583,  0,   0, 0,  0,  bhvLLLRotatingBlockWithFireBars),
    OBJECT( MODEL_CHECKERBOARD_PLATFORM,          -764,    0,  1664,  0, 180, 0,  BPARAM1(0x08) | BPARAM2(0xA5),  bhvPlatformOnTrack),
    OBJECT( MODEL_CHECKERBOARD_PLATFORM,           184,  980, -1366,  0, 180, 0,  BPARAM1(0x08) | BPARAM2(0xA6),  bhvPlatformOnTrack),
    OBJECT( MODEL_NONE,                            -26,  103, -2649,  0,   0, 0,  0,  bhvVolcanoSoundLoop),
    RETURN(),
};

static const LevelScript script_func_local_7[] = {
    OBJECT_WITH_ACTS( MODEL_STAR,  2523, 3850, -901,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_5),  bhvStar,  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,  1800, 3400, 1450,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_6),  bhvStar,  ALL_ACTS),
    RETURN(),
};

const LevelScript level_lll_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _lll_segment_7SegmentRomStart, _lll_segment_7SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _fire_mio0SegmentRomStart, _fire_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _bitfs_skybox_mio0SegmentRomStart, _bitfs_skybox_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x0B, _effect_mio0SegmentRomStart, _effect_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group2_mio0SegmentRomStart, _group2_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group2_geoSegmentRomStart,  _group2_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x06, _group17_mio0SegmentRomStart, _group17_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0D, _group17_geoSegmentRomStart,  _group17_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_3),
    JUMP_LINK(script_func_global_18),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03,                  lll_geo_0009E0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_04,                  lll_geo_0009F8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_05,                  lll_geo_000A10),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_06,                  lll_geo_000A28),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_07,                  lll_geo_000A40),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_08,                  lll_geo_000A60),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0A,                  lll_geo_000A90),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0B,                  lll_geo_000AA8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0C,                  lll_geo_000AC0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0D,                  lll_geo_000AD8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0E,                  lll_geo_000AF0),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_DRAWBRIDGE_PART,                lll_geo_000B20),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_ROTATING_BLOCK_FIRE_BARS,       lll_geo_000B38),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_ROTATING_HEXAGONAL_RING,        lll_geo_000BB0),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_SINKING_RECTANGULAR_PLATFORM,   lll_geo_000BC8),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_SINKING_SQUARE_PLATFORMS,       lll_geo_000BE0),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_TILTING_SQUARE_PLATFORM,        lll_geo_000BF8),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_BOWSER_PIECE_1,                 lll_geo_000C10),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_BOWSER_PIECE_2,                 lll_geo_000C30),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_BOWSER_PIECE_3,                 lll_geo_000C50),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_BOWSER_PIECE_4,                 lll_geo_000C70),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_BOWSER_PIECE_5,                 lll_geo_000C90),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_BOWSER_PIECE_6,                 lll_geo_000CB0),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_BOWSER_PIECE_7,                 lll_geo_000CD0),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_BOWSER_PIECE_8,                 lll_geo_000CF0),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_BOWSER_PIECE_9,                 lll_geo_000D10),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_BOWSER_PIECE_10,                lll_geo_000D30),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_BOWSER_PIECE_11,                lll_geo_000D50),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_BOWSER_PIECE_12,                lll_geo_000D70),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_BOWSER_PIECE_13,                lll_geo_000D90),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_BOWSER_PIECE_14,                lll_geo_000DB0),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_MOVING_OCTAGONAL_MESH_PLATFORM, lll_geo_000B08),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_SINKING_ROCK_BLOCK,             lll_geo_000DD0),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_ROLLING_LOG,                    lll_geo_000DE8),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_ROTATING_HEXAGONAL_PLATFORM,    lll_geo_000A78),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_WOOD_BRIDGE,                    lll_geo_000B50),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_LARGE_WOOD_BRIDGE,              lll_geo_000B68),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_FALLING_PLATFORM,               lll_geo_000B80),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_LARGE_FALLING_PLATFORM,         lll_geo_000B98),
    LOAD_MODEL_FROM_GEO(MODEL_LLL_VOLCANO_FALLING_TRAP,           lll_geo_000EA8),

    AREA( 1, lll_geo_000E00),
        OBJECT( MODEL_NONE,  -3839, 1154, 6272,  0,   90, 0,  BPARAM2(WARP_NODE_0A),  bhvSpinAirborneWarp),
        OBJECT( MODEL_NONE,      0,  105,    0,  0,    0, 0,  BPARAM1(25) | BPARAM2(WARP_NODE_0B),  bhvWarp),
        OBJECT( MODEL_NONE,  -3200,   11, 3456,  0, -100, 0,  BPARAM2(WARP_NODE_0C),  bhvFadingWarp),
        OBJECT( MODEL_NONE,  -5888,  154, 6656,  0,  100, 0,  BPARAM2(WARP_NODE_0D),  bhvFadingWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_LLL,     1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0B,       LEVEL_LLL,     2,  WARP_NODE_0A,  WARP_CHECKPOINT),
        WARP_NODE( WARP_NODE_0C,       LEVEL_LLL,     1,  WARP_NODE_0D,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0D,       LEVEL_LLL,     1,  WARP_NODE_0C,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  3,  WARP_NODE_32,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  3,  WARP_NODE_64,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        JUMP_LINK(script_func_local_4),
        JUMP_LINK(script_func_local_5),
        TERRAIN( lll_seg7_area_1_collision),
        MACRO_OBJECTS( lll_seg7_area_1_macro_objs),
        SHOW_DIALOG( 0x00, DIALOG_097),
        SET_BACKGROUND_MUSIC( 0x0000,  SEQ_LEVEL_HOT),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    AREA( 2, lll_geo_000EC0),
        OBJECT( MODEL_NONE,  -955, 1103, -1029,  0, 118, 0,  BPARAM2(WARP_NODE_0A),  bhvAirborneWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_LLL,     2,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  3,  WARP_NODE_32,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  3,  WARP_NODE_64,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_6),
        JUMP_LINK(script_func_local_7),
        TERRAIN( lll_seg7_area_2_collision),
        MACRO_OBJECTS( lll_seg7_area_2_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0004,  SEQ_LEVEL_HOT),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  90,  -3839, 154, 6272),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
