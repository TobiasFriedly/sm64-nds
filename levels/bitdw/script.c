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
#include "levels/bitdw/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_BITDW_SQUARE_PLATFORM,    -1966, -3154,  3586,  0, 0, 0,   BPARAM2(0x00),  bhvSquarishPathMoving),
    OBJECT( MODEL_BITDW_SQUARE_PLATFORM,    -1352, -3154,  4200,  0, 0, 0,   BPARAM2(0x02),  bhvSquarishPathMoving),
    OBJECT( MODEL_BITDW_SQUARE_PLATFORM,    -2963,  1017, -2464,  0, 0, 0,   BPARAM2(0x00),  bhvSquarishPathMoving),
    OBJECT( MODEL_BITDW_SQUARE_PLATFORM,    -2349,  1017, -1849,  0, 0, 0,   BPARAM2(0x02),  bhvSquarishPathMoving),
    OBJECT( MODEL_BITDW_SQUARE_PLATFORM,    -2349,  1017, -1235,  0, 0, 0,   BPARAM2(0x00),  bhvSquarishPathMoving),
    OBJECT( MODEL_BITDW_SQUARE_PLATFORM,    -1735,  1017,  -621,  0, 0, 0,   BPARAM2(0x02),  bhvSquarishPathMoving),
    OBJECT( MODEL_BITDW_SEESAW_PLATFORM,     1491,  1273,   512,  0, 90, 0,  0,  bhvSeesawPlatform),
    OBJECT( MODEL_BITDW_SEESAW_PLATFORM,     -147,   894,   512,  0, 90, 0,  0,  bhvSeesawPlatform),
    OBJECT( MODEL_BITDW_SLIDING_PLATFORM,   -5728,   819, -2151,  0, 0, 0,   BPARAM1(0x03) | BPARAM2(0xCE),  bhvSlidingPlatform2),
    OBJECT( MODEL_BITDW_FERRIS_WHEEL_AXLE,   -204, -1924,  3381,  0, 0, 0,   BPARAM2(0x01),  bhvFerrisWheelAxle),
    OBJECT( MODEL_BITDW_STAIRCASE,           5279,  1740,    -6,  0, 0, 0,   BPARAM2(0x01),  bhvAnimatesOnFloorSwitchPress),
    OBJECT( MODEL_PURPLE_SWITCH,             3922,  1740,    -7,  0, 0, 0,   0,  bhvFloorSwitchAnimatesObject),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT( MODEL_NONE,  -3092, -2795, 2842,  0, 0, 0,  0,  bhvFlamethrower),
    OBJECT( MODEL_NONE,   2463, -2386, 2844,  0, 0, 0,  0,  bhvFlamethrower),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT( MODEL_NONE,   7180,  3000,    0,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_1),  bhvBowserCourseRedCoinStar),
    RETURN(),
};

const LevelScript level_bitdw_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _bitdw_segment_7SegmentRomStart, _bitdw_segment_7SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _bidw_skybox_mio0SegmentRomStart, _bidw_skybox_mio0SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _sky_mio0SegmentRomStart, _sky_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group11_mio0SegmentRomStart, _group11_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group11_geoSegmentRomStart,  _group11_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x06, _group17_mio0SegmentRomStart, _group17_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0D, _group17_geoSegmentRomStart,  _group17_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_12),
    JUMP_LINK(script_func_global_18),
    JUMP_LINK(script_func_global_1),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03,       geo_bitdw_0003C0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_04,       geo_bitdw_0003D8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_05,       geo_bitdw_0003F0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_06,       geo_bitdw_000408),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_07,       geo_bitdw_000420),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_08,       geo_bitdw_000438),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_09,       geo_bitdw_000450),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0A,       geo_bitdw_000468),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0B,       geo_bitdw_000480),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0C,       geo_bitdw_000498),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0D,       geo_bitdw_0004B0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0E,       geo_bitdw_0004C8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0F,       geo_bitdw_0004E0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_10,       geo_bitdw_0004F8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_11,       geo_bitdw_000510),
    LOAD_MODEL_FROM_GEO(MODEL_BITDW_WARP_PIPE,         warp_pipe_geo),
    LOAD_MODEL_FROM_GEO(MODEL_BITDW_SQUARE_PLATFORM,   geo_bitdw_000558),
    LOAD_MODEL_FROM_GEO(MODEL_BITDW_SEESAW_PLATFORM,   geo_bitdw_000540),
    LOAD_MODEL_FROM_GEO(MODEL_BITDW_SLIDING_PLATFORM,  geo_bitdw_000528),
    LOAD_MODEL_FROM_GEO(MODEL_BITDW_FERRIS_WHEEL_AXLE, geo_bitdw_000570),
    LOAD_MODEL_FROM_GEO(MODEL_BITDW_BLUE_PLATFORM,     geo_bitdw_000588),
    LOAD_MODEL_FROM_GEO(MODEL_BITDW_STAIRCASE_FRAME4,  geo_bitdw_0005A0),
    LOAD_MODEL_FROM_GEO(MODEL_BITDW_STAIRCASE_FRAME3,  geo_bitdw_0005B8),
    LOAD_MODEL_FROM_GEO(MODEL_BITDW_STAIRCASE_FRAME2,  geo_bitdw_0005D0),
    LOAD_MODEL_FROM_GEO(MODEL_BITDW_STAIRCASE_FRAME1,  geo_bitdw_0005E8),
    LOAD_MODEL_FROM_GEO(MODEL_BITDW_STAIRCASE,         geo_bitdw_000600),

    AREA( 1, geo_bitdw_000618),
        OBJECT( MODEL_NONE,             -7443, -2153, 3886,  0, 90, 0,  BPARAM2(WARP_NODE_0A),  bhvAirborneWarp),
        OBJECT( MODEL_BITDW_WARP_PIPE,   6816,  2860,   -7,  0, 0, 0,   BPARAM2(WARP_NODE_0B),  bhvWarpPipe),
        OBJECT( MODEL_NONE,              5910,  3500,   -7,  0, 90, 0,  BPARAM2(WARP_NODE_0C),  bhvDeathWarp),
        WARP_NODE( WARP_NODE_0A,     LEVEL_BITDW,     1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0B,     LEVEL_BOWSER_1,  1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0C,     LEVEL_BITDW,     1,  WARP_NODE_0C,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,  LEVEL_CASTLE,    1,  WARP_NODE_25,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        TERRAIN( bitdw_seg7_collision_level),
        MACRO_OBJECTS( bitdw_seg7_macro_objs),
        SHOW_DIALOG( 0x00, DIALOG_090),
        SET_BACKGROUND_MUSIC( 0x0000,  SEQ_LEVEL_KOOPA_ROAD),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  90,  -7443, -3153, 3886),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
