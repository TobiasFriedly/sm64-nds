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
#include "levels/bitfs/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_BITFS_PLATFORM_ON_TRACK,        -5733, -3071,    0,  0, 0, 0,    BPARAM1(0x07) | BPARAM2(0x33),  bhvPlatformOnTrack),
    OBJECT( MODEL_BITFS_TILTING_SQUARE_PLATFORM,  -1945, -3225, -715,  0, 0, 0,    0,  bhvBitFSTiltingInvertedPyramid),
    OBJECT( MODEL_BITFS_TILTING_SQUARE_PLATFORM,  -2866, -3225, -715,  0, 0, 0,    0,  bhvBitFSTiltingInvertedPyramid),
    OBJECT( MODEL_BITFS_SINKING_PLATFORMS,        -1381,  3487,   96,  0, 0, 0,    0,  bhvBitFSSinkingPlatforms),
    OBJECT( MODEL_BITFS_SINKING_PLATFORMS,         1126, -3065,  307,  0, 0, 0,    0,  bhvBitFSSinkingPlatforms),
    OBJECT( MODEL_BITFS_SINKING_PLATFORMS,        -3225,  3487,   96,  0, 0, 0,    0,  bhvBitFSSinkingPlatforms),
    OBJECT( MODEL_BITFS_SINKING_CAGE_PLATFORM,     6605, -3071,  266,  0, 0, 0,    BPARAM2(0x00),  bhvBitFSSinkingCagePlatform),
    OBJECT( MODEL_BITFS_SINKING_CAGE_PLATFORM,     1843,  3584,   96,  0, 0, 0,    BPARAM2(0x01),  bhvBitFSSinkingCagePlatform),
    OBJECT( MODEL_BITFS_SINKING_CAGE_PLATFORM,      614,  3584,   96,  0, 0, 0,    BPARAM2(0x01),  bhvBitFSSinkingCagePlatform),
    OBJECT( MODEL_BITFS_SINKING_CAGE_PLATFORM,     3072,  3584,   96,  0, 0, 0,    BPARAM2(0x01),  bhvBitFSSinkingCagePlatform),
    OBJECT( MODEL_BITFS_ELEVATOR,                  2867, -1279,  307,  0, 0, 0,    BPARAM1(ACTIVATED_BF_PLAT_TYPE_BITFS_ELEVATOR) | BPARAM2(159),  bhvActivatedBackAndForthPlatform),
    OBJECT( MODEL_BITFS_STRETCHING_PLATFORMS,     -5836,   410,  300,  0, 0, 0,    0,  bhvSquishablePlatform),
    OBJECT( MODEL_BITFS_SEESAW_PLATFORM,           4454, -2226,  266,  0, 0, 0,    BPARAM2(0x04),  bhvSeesawPlatform),
    OBJECT( MODEL_BITFS_SEESAW_PLATFORM,           5786, -2380,  266,  0, 0, 0,    BPARAM2(0x04),  bhvSeesawPlatform),
    OBJECT( MODEL_BITFS_MOVING_SQUARE_PLATFORM,   -3890,   102,  617,  0, 90, 0,   BPARAM1(0x01) | BPARAM2(0x0C),  bhvSlidingPlatform2),
    OBJECT( MODEL_BITFS_MOVING_SQUARE_PLATFORM,   -3276,   102,    2,  0, 270, 0,  BPARAM1(0x01) | BPARAM2(0x0C),  bhvSlidingPlatform2),
    OBJECT( MODEL_BITFS_SLIDING_PLATFORM,          2103,   198,  312,  0, 0, 0,    BPARAM1(0x01) | BPARAM2(0x9F),  bhvSlidingPlatform2),
    OBJECT( MODEL_BITFS_TUMBLING_PLATFORM,         4979,  4250,   96,  0, 0, 0,    BPARAM2(TUMBLING_BRIDGE_BP_BITFS),  bhvTumblingBridge),
    OBJECT( MODEL_NONE,                            3890, -2043,  266,  0, 0, 0,    BPARAM2(82),    bhvPoleGrabbing),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT( MODEL_NONE,  -3226, 3584, -822,  0, 0, 0,  0,  bhvFlamethrower),
    OBJECT( MODEL_NONE,  -1382, 3584, -822,  0, 0, 0,  0,  bhvFlamethrower),
    OBJECT( MODEL_NONE,   1229,  307, -412,  0, 0, 0,  0,  bhvFlamethrower),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT( MODEL_NONE,   1200, 5700,  160,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_1),  bhvBowserCourseRedCoinStar),
    RETURN(),
};

const LevelScript level_bitfs_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _bitfs_segment_7SegmentRomStart, _bitfs_segment_7SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _sky_mio0SegmentRomStart, _sky_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _bitfs_skybox_mio0SegmentRomStart, _bitfs_skybox_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x0B, _effect_mio0SegmentRomStart, _effect_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group2_mio0SegmentRomStart, _group2_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group2_geoSegmentRomStart, _group2_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x06, _group17_mio0SegmentRomStart, _group17_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0D, _group17_geoSegmentRomStart, _group17_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart, _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_3),
    JUMP_LINK(script_func_global_18),
    JUMP_LINK(script_func_global_1),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03,             bitfs_geo_0004B0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_04,             bitfs_geo_0004C8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_05,             bitfs_geo_0004E0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_06,             bitfs_geo_0004F8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_07,             bitfs_geo_000510),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_08,             bitfs_geo_000528),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_09,             bitfs_geo_000540),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0A,             bitfs_geo_000558),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0B,             bitfs_geo_000570),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0C,             bitfs_geo_000588),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0D,             bitfs_geo_0005A0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0E,             bitfs_geo_0005B8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0F,             bitfs_geo_0005D0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_10,             bitfs_geo_0005E8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_11,             bitfs_geo_000600),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_12,             bitfs_geo_000618),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_13,             bitfs_geo_000630),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_14,             bitfs_geo_000648),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_15,             bitfs_geo_000660),
    LOAD_MODEL_FROM_GEO(MODEL_BITFS_PLATFORM_ON_TRACK,       bitfs_geo_000758),
    LOAD_MODEL_FROM_GEO(MODEL_BITFS_TILTING_SQUARE_PLATFORM, bitfs_geo_0006C0),
    LOAD_MODEL_FROM_GEO(MODEL_BITFS_SINKING_PLATFORMS,       bitfs_geo_000770),
    LOAD_MODEL_FROM_GEO(MODEL_BITFS_BLUE_POLE,               bitfs_geo_0006A8),
    LOAD_MODEL_FROM_GEO(MODEL_BITFS_SINKING_CAGE_PLATFORM,   bitfs_geo_000690),
    LOAD_MODEL_FROM_GEO(MODEL_BITFS_ELEVATOR,                bitfs_geo_000678),
    LOAD_MODEL_FROM_GEO(MODEL_BITFS_STRETCHING_PLATFORMS,    bitfs_geo_000708),
    LOAD_MODEL_FROM_GEO(MODEL_BITFS_SEESAW_PLATFORM,         bitfs_geo_000788),
    LOAD_MODEL_FROM_GEO(MODEL_BITFS_MOVING_SQUARE_PLATFORM,  bitfs_geo_000728),
    LOAD_MODEL_FROM_GEO(MODEL_BITFS_SLIDING_PLATFORM,        bitfs_geo_000740),
    LOAD_MODEL_FROM_GEO(MODEL_BITFS_TUMBLING_PLATFORM_PART,  bitfs_geo_0006D8),
    LOAD_MODEL_FROM_GEO(MODEL_BITFS_TUMBLING_PLATFORM,       bitfs_geo_0006F0),

    AREA( 1, bitfs_geo_0007A0),
        OBJECT( MODEL_NONE,  -7577, -1764,  0,  0, 90, 0,  BPARAM2(WARP_NODE_0A),  bhvAirborneWarp),
        OBJECT( MODEL_NONE,   6735,  3681, 99,  0, 0, 0,   BPARAM1(20) | BPARAM2(WARP_NODE_0B),  bhvWarp),
        OBJECT( MODEL_NONE,   5886,  5000, 99,  0, 90, 0,  BPARAM2(WARP_NODE_0C),  bhvDeathWarp),
        WARP_NODE( WARP_NODE_0A,     LEVEL_BITFS,     1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0B,     LEVEL_BOWSER_2,  1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0C,     LEVEL_BITFS,     1,  WARP_NODE_0C,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,  LEVEL_CASTLE,    3,  WARP_NODE_68,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        TERRAIN( bitfs_seg7_collision_level),
        MACRO_OBJECTS( bitfs_seg7_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0000,  SEQ_LEVEL_KOOPA_ROAD),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  90,  -7577, -2764, 0),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
