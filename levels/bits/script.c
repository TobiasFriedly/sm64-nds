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
#include "levels/bits/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_BITS_SLIDING_PLATFORM,        -2370, -4525,     0,  0, 0, 0,    BPARAM2(0x10),  bhvSlidingPlatform2),
    OBJECT( MODEL_BITS_TWIN_SLIDING_PLATFORMS,  -2611,  3544,  -904,  0, 0, 0,    BPARAM2(0xCF),  bhvSlidingPlatform2),
    OBJECT( MODEL_BITS_TWIN_SLIDING_PLATFORMS,  -4700,  3544,  -904,  0, 180, 0,  BPARAM2(0x8F),  bhvSlidingPlatform2),
    OBJECT( MODEL_BITS_OCTAGONAL_PLATFORM,       4139, -1740, -1831,  0, 0, 0,    BPARAM1(0x02),  bhvOctagonalPlatformRotating),
    OBJECT( MODEL_BITS_OCTAGONAL_PLATFORM,      -6459,  1732,  -904,  0, 0, 0,    0,  bhvOctagonalPlatformRotating),
    OBJECT( MODEL_BITS_OCTAGONAL_PLATFORM,      -4770,  1732,  -904,  0, 0, 0,    BPARAM1(0x02),  bhvOctagonalPlatformRotating),
    OBJECT( MODEL_BITS_FERRIS_WHEEL_AXLE,       -1748, -1330, -1094,  0, 0, 0,    0,  bhvFerrisWheelAxle),
    OBJECT( MODEL_BITS_FERRIS_WHEEL_AXLE,        2275,  5628, -1315,  0, 0, 0,    0,  bhvFerrisWheelAxle),
    OBJECT( MODEL_BITS_FERRIS_WHEEL_AXLE,        3114,  4701, -1320,  0, 0, 0,    0,  bhvFerrisWheelAxle),
    OBJECT( MODEL_BITS_ARROW_PLATFORM,           2793,  2325,  -904,  0, 0, 0,    BPARAM1(ACTIVATED_BF_PLAT_TYPE_BITS_ARROW_PLAT) | BPARAM2(97),  bhvActivatedBackAndForthPlatform),
    OBJECT( MODEL_BITS_SEESAW_PLATFORM,            27, -1555,  -713,  0, 90, 0,   BPARAM2(0x01),  bhvSeesawPlatform),
    OBJECT( MODEL_BITS_TILTING_W_PLATFORM,       -306, -4300,     0,  0, 0, 0,    BPARAM2(0x02),  bhvSeesawPlatform),
    OBJECT( MODEL_BITS_STAIRCASE,                1769,  -234,  -899,  0, 0, 0,    0,  bhvAnimatesOnFloorSwitchPress),
    OBJECT( MODEL_PURPLE_SWITCH,                 -279,  -234,  -900,  0, 0, 0,    0,  bhvFloorSwitchAnimatesObject),
    OBJECT( MODEL_NONE,                         -6460,  2039,  -905,  0, 0, 0,    BPARAM2(207),   bhvPoleGrabbing),
    OBJECT( MODEL_NONE,                         -3326,  3227,  -905,  0, 0, 0,    BPARAM2(77),    bhvPoleGrabbing),
    OBJECT( MODEL_NONE,                          5518,  3184, -4019,  0, 0, 0,    0,  bhvFlamethrower),
    OBJECT( MODEL_NONE,                          6465,  3731, -1915,  0, 0, 0,    0,  bhvFlamethrower),
    OBJECT( MODEL_NONE,                          5915,  3718, -4019,  0, 0, 0,    0,  bhvFlamethrower),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT( MODEL_NONE,  350, 6800, -6800,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_1),  bhvBowserCourseRedCoinStar),
    RETURN(),
};

const LevelScript level_bits_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _bits_segment_7SegmentRomStart, _bits_segment_7SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _bits_skybox_mio0SegmentRomStart, _bits_skybox_mio0SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _sky_mio0SegmentRomStart, _sky_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x06, _group14_mio0SegmentRomStart, _group14_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0D, _group14_geoSegmentRomStart,  _group14_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_15),
    JUMP_LINK(script_func_global_1),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03,           bits_geo_000430),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_04,           bits_geo_000448),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_05,           bits_geo_000460),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_06,           bits_geo_000478),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_07,           bits_geo_000490),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_08,           bits_geo_0004A8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_09,           bits_geo_0004C0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0A,           bits_geo_0004D8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0B,           bits_geo_0004F0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0C,           bits_geo_000508),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0D,           bits_geo_000520),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0E,           bits_geo_000538),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0F,           bits_geo_000550),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_10,           bits_geo_000568),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_11,           bits_geo_000580),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_12,           bits_geo_000598),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_13,           bits_geo_0005B0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_14,           bits_geo_0005C8),
    LOAD_MODEL_FROM_GEO(MODEL_BITS_SLIDING_PLATFORM,       bits_geo_0005E0),
    LOAD_MODEL_FROM_GEO(MODEL_BITS_TWIN_SLIDING_PLATFORMS, bits_geo_0005F8),
    LOAD_MODEL_FROM_GEO(MODEL_BITS_OCTAGONAL_PLATFORM,     bits_geo_000610),
    LOAD_MODEL_FROM_GEO(MODEL_BITS_BLUE_PLATFORM,          bits_geo_000628),
    LOAD_MODEL_FROM_GEO(MODEL_BITS_FERRIS_WHEEL_AXLE,      bits_geo_000640),
    LOAD_MODEL_FROM_GEO(MODEL_BITS_ARROW_PLATFORM,         bits_geo_000658),
    LOAD_MODEL_FROM_GEO(MODEL_BITS_SEESAW_PLATFORM,        bits_geo_000670),
    LOAD_MODEL_FROM_GEO(MODEL_BITS_TILTING_W_PLATFORM,     bits_geo_000688),
    LOAD_MODEL_FROM_GEO(MODEL_BITS_STAIRCASE,              bits_geo_0006A0),
    LOAD_MODEL_FROM_GEO(MODEL_BITS_STAIRCASE_FRAME1,       bits_geo_0006B8),
    LOAD_MODEL_FROM_GEO(MODEL_BITS_STAIRCASE_FRAME2,       bits_geo_0006D0),
    LOAD_MODEL_FROM_GEO(MODEL_BITS_STAIRCASE_FRAME3,       bits_geo_0006E8),
    LOAD_MODEL_FROM_GEO(MODEL_BITS_STAIRCASE_FRAME4,       bits_geo_000700),
    LOAD_MODEL_FROM_GEO(MODEL_BITS_WARP_PIPE,              warp_pipe_geo),

    AREA( 1, bits_geo_000718),
        OBJECT( MODEL_NONE,            -7039, -3812,     4,  0, 90, 0,   BPARAM2(WARP_NODE_0A),  bhvAirborneWarp),
        OBJECT( MODEL_BITS_WARP_PIPE,    351,  6652, -6030,  0, 0, 0,    BPARAM2(WARP_NODE_0B),  bhvWarpPipe),
        OBJECT( MODEL_NONE,              351,  6800, -3900,  0, 180, 0,  BPARAM2(WARP_NODE_0C),  bhvDeathWarp),
        WARP_NODE( WARP_NODE_0A,     LEVEL_BITS,      1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0B,     LEVEL_BOWSER_3,  1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0C,     LEVEL_BITS,      1,  WARP_NODE_0C,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,  LEVEL_CASTLE,    2,  WARP_NODE_6B,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        TERRAIN( bits_seg7_collision_level),
        MACRO_OBJECTS( bits_seg7_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0000,  SEQ_LEVEL_KOOPA_ROAD),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  90,  -7039, -4812, 4),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
