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
#include "levels/hmc/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_RED_FLAME,  4936, -357, -4146,  0, 0, 0,  0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,  5018, -460, -5559,  0, 0, 0,  0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,  1997,  666,  -235,  0, 0, 0,  0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,  1762, -460, -2610,  0, 0, 0,  0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,  4178, -255, -3737,  0, 0, 0,  0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,  2233, -460,   256,  0, 0, 0,  0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,  5510, -255, -3429,  0, 0, 0,  0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,  4690, -357,  -767,  0, 0, 0,  0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,  3462, -255, -1125,  0, 0, 0,  0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,  1762,  666,     0,  0, 0, 0,  0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,  1762, -460,   256,  0, 0, 0,  0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,  6482,  461,  3226,  0, 0, 0,  0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,  1075,  461,  6543,  0, 0, 0,  0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,  6912,  461,  6543,  0, 0, 0,  0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,  6912,  461,  3697,  0, 0, 0,  0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,  6482,  461,  7014,  0, 0, 0,  0,  bhvFlame),
    OBJECT( MODEL_RED_FLAME,  3817,  717,  1034,  0, 0, 0,  0,  bhvFlame),
    OBJECT( MODEL_NONE,        799, 1024,  4434,  0, 0, 0,  BPARAM2(184),  bhvPoleGrabbing),
    OBJECT( MODEL_NONE,        889, 1024,  3277,  0, 0, 0,  BPARAM2(184),  bhvPoleGrabbing),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT( MODEL_HMC_METAL_PLATFORM,      1100,   950,  6350,  0,   0, 0,  0,  bhvControllablePlatform),
    OBJECT( MODEL_HMC_ELEVATOR_PLATFORM,  -3243,  1434,  1392,  0,  27, 0,  0,  bhvHMCElevatorPlatform),
    OBJECT( MODEL_HMC_ELEVATOR_PLATFORM,  -2816,  2253, -2509,  0,   0, 0,  BPARAM2(0x01),  bhvHMCElevatorPlatform),
    OBJECT( MODEL_HMC_ELEVATOR_PLATFORM,   -973,  1741, -7347,  0,   0, 0,  BPARAM2(0x02),  bhvHMCElevatorPlatform),
    OBJECT( MODEL_HMC_ELEVATOR_PLATFORM,  -3533,  1741, -7040,  0,   0, 0,  BPARAM2(0x03),  bhvHMCElevatorPlatform),
    OBJECT( MODEL_NONE,                     614, -4690,  2330,  0, 270, 0,  BPARAM2(OPENABLE_GRILL_BP_HMC),  bhvOpenableGrill),
    OBJECT( MODEL_PURPLE_SWITCH,           -307, -4997,  2483,  0, 270, 0,  0,  bhvFloorSwitchGrills),
    OBJECT( MODEL_CHECKERBOARD_PLATFORM,   1270,  2000,  4000,  0, 270, 0,  BPARAM1(0x09) | BPARAM2(0xA4),  bhvPlatformOnTrack),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT( MODEL_DORRIE,  -3533, -4969,  3558,  0, 0, 0,  0,  bhvDorrie),
    OBJECT( MODEL_NONE,    -6093,  3075, -7807,  0, 0, 0,  0,  bhvBigBoulderGenerator),
    OBJECT( MODEL_NONE,     -500,  1600,  3500,  0, 0, 0,  BPARAM2(0x04),  bhvFlamethrower),
    OBJECT( MODEL_NONE,     -500,  1600,  3800,  0, 0, 0,  BPARAM2(0x04),  bhvFlamethrower),
    RETURN(),
};

static const LevelScript script_func_local_4[] = {
    OBJECT_WITH_ACTS( MODEL_STAR,  -3600, -4000,  3600,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_1),  bhvStar,                  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_NONE,   4000,   300,  5000,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_2),  bhvHiddenRedCoinStar,  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,   6200, -4400,  2300,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_3),  bhvStar,                  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,  -2100,  2100, -7550,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_4),  bhvStar,                  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,  -6500,  2700, -1600,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_5),  bhvStar,                  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,  -5000,  3050, -6700,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_6),  bhvStar,                  ALL_ACTS),
    RETURN(),
};

const LevelScript level_hmc_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _hmc_segment_7SegmentRomStart, _hmc_segment_7SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _cave_mio0SegmentRomStart, _cave_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group6_mio0SegmentRomStart, _group6_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group6_geoSegmentRomStart, _group6_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x06, _group17_mio0SegmentRomStart, _group17_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0D, _group17_geoSegmentRomStart, _group17_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_7),
    JUMP_LINK(script_func_global_18),
    LOAD_MODEL_FROM_GEO(MODEL_HMC_WOODEN_DOOR,          wooden_door_geo),
    LOAD_MODEL_FROM_GEO(MODEL_HMC_METAL_DOOR,           metal_door_geo),
    LOAD_MODEL_FROM_GEO(MODEL_HMC_HAZY_MAZE_DOOR,       hazy_maze_door_geo),
    LOAD_MODEL_FROM_GEO(MODEL_HMC_METAL_PLATFORM,       hmc_geo_0005A0),
    LOAD_MODEL_FROM_GEO(MODEL_HMC_METAL_ARROW_PLATFORM, hmc_geo_0005B8),
    LOAD_MODEL_FROM_GEO(MODEL_HMC_ELEVATOR_PLATFORM,    hmc_geo_0005D0),
    LOAD_MODEL_FROM_GEO(MODEL_HMC_ROLLING_ROCK,         hmc_geo_000548),
    LOAD_MODEL_FROM_GEO(MODEL_HMC_ROCK_PIECE,           hmc_geo_000570),
    LOAD_MODEL_FROM_GEO(MODEL_HMC_ROCK_SMALL_PIECE,     hmc_geo_000588),
    LOAD_MODEL_FROM_GEO(MODEL_HMC_RED_GRILLS,           hmc_geo_000530),

    AREA( 1, hmc_geo_000B90),
        OBJECT( MODEL_NONE,  -7152,  3161, 7181,  0, 135, 0,  BPARAM2(WARP_NODE_0A),  bhvSpinAirborneWarp),
        OBJECT( MODEL_NONE,   3351, -4690, 4773,  0,   0, 0,  BPARAM1(52) | BPARAM2(WARP_NODE_0B),  bhvWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_HMC,     1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0B,       LEVEL_COTMC,   1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  3,  WARP_NODE_34,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  3,  WARP_NODE_66,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        JUMP_LINK(script_func_local_4),
        TERRAIN( hmc_seg7_collision_level),
        MACRO_OBJECTS( hmc_seg7_macro_objs),
        ROOMS( hmc_seg7_rooms),
        SET_BACKGROUND_MUSIC( 0x0004,  SEQ_LEVEL_UNDERGROUND),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  135,  -7152, 2161, 7181),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
