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
#include "levels/wdw/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_WDW_SQUARE_FLOATING_PLATFORM,        3390,    0,   384,  0, 180, 0,  0,  bhvWDWSquareFloatingPlatform),
    OBJECT( MODEL_WDW_SQUARE_FLOATING_PLATFORM,        -767,  384,  3584,  0,   0, 0,  0,  bhvWDWSquareFloatingPlatform),
    OBJECT( MODEL_WDW_SQUARE_FLOATING_PLATFORM,        -767,  384,  1536,  0,   0, 0,  0,  bhvWDWSquareFloatingPlatform),
    OBJECT( MODEL_WDW_SQUARE_FLOATING_PLATFORM,        -767, 2304, -1279,  0,   0, 0,  0,  bhvWDWSquareFloatingPlatform),
    OBJECT( MODEL_WDW_ARROW_LIFT,                      -578, 2177,  3009,  0,   0, 0,  0,  bhvArrowLift),
    OBJECT( MODEL_WDW_ARROW_LIFT,                     -1474, 2177,  3393,  0, 270, 0,  0,  bhvArrowLift),
    OBJECT( MODEL_WDW_ARROW_LIFT,                     -1602, 2177,  3009,  0,   0, 0,  0,  bhvArrowLift),
    OBJECT( MODEL_WDW_ARROW_LIFT,                     -1090, 2177,  3521,  0,   0, 0,  0,  bhvArrowLift),
    OBJECT( MODEL_WDW_ARROW_LIFT,                      -962, 2177,  3137,  0,  90, 0,  0,  bhvArrowLift),
    OBJECT( MODEL_NONE,                                   0,    0,     0,  0,   0, 0,  0,  bhvInitializeChangingWaterLevel),
    OBJECT( MODEL_WDW_WATER_LEVEL_DIAMOND,             1920, 2560, -3583,  0,   0, 0,  0,  bhvWaterLevelDiamond),
    OBJECT( MODEL_WDW_WATER_LEVEL_DIAMOND,             3328,  256,  2918,  0,   0, 0,  0,  bhvWaterLevelDiamond),
    OBJECT( MODEL_WDW_WATER_LEVEL_DIAMOND,             2048, 1792,  2176,  0,   0, 0,  0,  bhvWaterLevelDiamond),
    OBJECT( MODEL_WDW_WATER_LEVEL_DIAMOND,              640, 1024,  3712,  0,   0, 0,  0,  bhvWaterLevelDiamond),
    OBJECT( MODEL_WDW_WATER_LEVEL_DIAMOND,             1810,   40, -3118,  0,   0, 0,  0,  bhvWaterLevelDiamond),
    OBJECT( MODEL_PURPLE_SWITCH,                       3360, 1280,  3420,  0,   0, 0,  0,  bhvFloorSwitchHiddenObjects),
    OBJECT( MODEL_WDW_HIDDEN_PLATFORM,                 2239, 1126,  3391,  0,   0, 0,  BPARAM2(HIDDEN_OBJECT_BP_WDW_PLATFORM),  bhvHiddenObject),
    OBJECT( MODEL_WDW_HIDDEN_PLATFORM,                 1215, 1357,  2751,  0,   0, 0,  BPARAM2(HIDDEN_OBJECT_BP_WDW_PLATFORM),  bhvHiddenObject),
    OBJECT( MODEL_WDW_HIDDEN_PLATFORM,                 1215, 1229,  3391,  0,   0, 0,  BPARAM2(HIDDEN_OBJECT_BP_WDW_PLATFORM),  bhvHiddenObject),
    OBJECT( MODEL_WDW_HIDDEN_PLATFORM,                 1599, 1101,  3391,  0,   0, 0,  BPARAM2(HIDDEN_OBJECT_BP_WDW_PLATFORM),  bhvHiddenObject),
    OBJECT( MODEL_WDW_HIDDEN_PLATFORM,                 2879, 1152,  3391,  0,   0, 0,  BPARAM2(HIDDEN_OBJECT_BP_WDW_PLATFORM),  bhvHiddenObject),
    OBJECT( MODEL_WDW_EXPRESS_ELEVATOR,                1024, 3277, -2112,  0,   0, 0,  BPARAM2(0x32),  bhvWDWExpressElevatorPlatform),
    OBJECT( MODEL_WDW_EXPRESS_ELEVATOR,                1024, 3277, -1663,  0,   0, 0,  BPARAM2(0x32),  bhvWDWExpressElevator),
    OBJECT( MODEL_WDW_RECTANGULAR_FLOATING_PLATFORM,   -767, 1152,   128,  0,   0, 0,  0,  bhvWDWRectangularFloatingPlatform),
    OBJECT( MODEL_WDW_RECTANGULAR_FLOATING_PLATFORM,   -767, 2304, -2687,  0,   0, 0,  0,  bhvWDWRectangularFloatingPlatform),
    OBJECT( MODEL_WDW_ROTATING_PLATFORM,                734, 3840,    84,  0,   0, 0,  BPARAM1(0x46) | BPARAM2(ROTATING_PLATFORM_BP_WDW),  bhvRotatingPlatform),
    OBJECT( MODEL_SKEETER,                             2956,  288,  -468,  0,   0, 0,  0,  bhvSkeeter),
    OBJECT( MODEL_SKEETER,                              184,  384,   621,  0,   0, 0,  0,  bhvSkeeter),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT( MODEL_NONE,                         0,     0,     0,  0, 0, 0,  0,  bhvInitializeChangingWaterLevel),
    OBJECT( MODEL_WDW_WATER_LEVEL_DIAMOND,  -3583, -2508, -2047,  0, 0, 0,  0,  bhvWaterLevelDiamond),
    OBJECT( MODEL_WDW_WATER_LEVEL_DIAMOND,   -767,  -127,  1792,  0, 0, 0,  0,  bhvWaterLevelDiamond),
    OBJECT( MODEL_NONE,                      -768,  -665,  3584,  0, 0, 0,  BPARAM2(92),  bhvPoleGrabbing),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT_WITH_ACTS( MODEL_NONE,  3360,  1580,  2660,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_3),  bhvHiddenStar,           ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,   890,  3400, -2040,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_4),  bhvStar,                  ALL_ACTS),
    RETURN(),
};

static const LevelScript script_func_local_4[] = {
    OBJECT_WITH_ACTS( MODEL_NONE,  -770, -1600,  3600,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_5),  bhvHiddenRedCoinStar,  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,  2180,  -840,  3720,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_6),  bhvStar,                  ALL_ACTS),
    RETURN(),
};

const LevelScript level_wdw_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _wdw_segment_7SegmentRomStart, _wdw_segment_7SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _grass_mio0SegmentRomStart, _grass_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _wdw_skybox_mio0SegmentRomStart, _wdw_skybox_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group1_mio0SegmentRomStart, _group1_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group1_geoSegmentRomStart,  _group1_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x06, _group13_mio0SegmentRomStart, _group13_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0D, _group13_geoSegmentRomStart,  _group13_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_2),
    JUMP_LINK(script_func_global_14),
    LOAD_MODEL_FROM_GEO(MODEL_WDW_BUBBLY_TREE,                   bubbly_tree_geo),
    LOAD_MODEL_FROM_GEO(MODEL_WDW_SQUARE_FLOATING_PLATFORM,      wdw_geo_000580),
    LOAD_MODEL_FROM_GEO(MODEL_WDW_ARROW_LIFT,                    wdw_geo_000598),
    LOAD_MODEL_FROM_GEO(MODEL_WDW_WATER_LEVEL_DIAMOND,           wdw_geo_0005C0),
    LOAD_MODEL_FROM_GEO(MODEL_WDW_HIDDEN_PLATFORM,               wdw_geo_0005E8),
    LOAD_MODEL_FROM_GEO(MODEL_WDW_EXPRESS_ELEVATOR,              wdw_geo_000610),
    LOAD_MODEL_FROM_GEO(MODEL_WDW_RECTANGULAR_FLOATING_PLATFORM, wdw_geo_000628),
    LOAD_MODEL_FROM_GEO(MODEL_WDW_ROTATING_PLATFORM,             wdw_geo_000640),

    AREA( 1, wdw_geo_000658),
        OBJECT( MODEL_NONE,   3395, 3580,  384,  0, 180, 0,  BPARAM2(WARP_NODE_0A),  bhvSpinAirborneWarp),
        OBJECT( MODEL_NONE,    818,    0, 3634,  0,  45, 0,  BPARAM2(WARP_NODE_0B),  bhvFadingWarp),
        OBJECT( MODEL_NONE,  -2865, 3328, 3065,  0,   0, 0,  BPARAM2(WARP_NODE_0C),  bhvFadingWarp),
        WARP_NODE( WARP_NODE_0A,  LEVEL_WDW,  1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0B,  LEVEL_WDW,  1,  WARP_NODE_0C,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0C,  LEVEL_WDW,  1,  WARP_NODE_0B,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_3),
        JUMP_LINK(script_func_local_1),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  2,  WARP_NODE_32,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  2,  WARP_NODE_64,  WARP_NO_CHECKPOINT),
        INSTANT_WARP( 1,  2,  0, 0, 0),
        TERRAIN( wdw_seg7_area_1_collision),
        MACRO_OBJECTS( wdw_seg7_area_1_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0003,  SEQ_LEVEL_UNDERGROUND),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    AREA( 2, wdw_geo_000724),
        JUMP_LINK(script_func_local_4),
        JUMP_LINK(script_func_local_2),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  2,  WARP_NODE_32,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  2,  WARP_NODE_64,  WARP_NO_CHECKPOINT),
        INSTANT_WARP( 0,  1,  0, 0, 0),
        TERRAIN( wdw_seg7_area_2_collision),
        MACRO_OBJECTS( wdw_seg7_area_2_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0003,  SEQ_LEVEL_UNDERGROUND),
        TERRAIN_TYPE( TERRAIN_WATER),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  180,  3395, 2580, 384),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
