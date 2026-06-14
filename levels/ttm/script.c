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
#include "levels/ttm/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_TTM_ROLLING_LOG,      4360, -1722,  4001,  0,  48, 0,  0,  bhvTTMRollingLog),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT          ( MODEL_NONE,                -1639,  1146, -1742,  0,   0, 0,  BPARAM2(BBALL_BP_STYPE_TTM),  bhvTTMBowlingBallSpawner),
    OBJECT          ( MODEL_NONE,                 3295, -3692,  2928,  0,   0, 0,  0,  bhvWaterfallSoundLoop),
    OBJECT          ( MODEL_NONE,                 2004, -1580,  1283,  0,   0, 0,  0,  bhvWaterfallSoundLoop),
    OBJECT          ( MODEL_DL_MONTY_MOLE_HOLE,  -2077, -1023, -1969,  0,   0, 0,  0,  bhvMontyMoleHole),
    OBJECT          ( MODEL_DL_MONTY_MOLE_HOLE,  -2500, -1023, -2157,  0,   0, 0,  0,  bhvMontyMoleHole),
    OBJECT          ( MODEL_DL_MONTY_MOLE_HOLE,  -2048, -1023, -2307,  0,   0, 0,  0,  bhvMontyMoleHole),
    OBJECT          ( MODEL_DL_MONTY_MOLE_HOLE,  -2351, -1023, -2416,  0,   0, 0,  0,  bhvMontyMoleHole),
    OBJECT          ( MODEL_DL_MONTY_MOLE_HOLE,  -2400, -2559, -2185,  0,   0, 0,  0,  bhvMontyMoleHole),
    OBJECT          ( MODEL_DL_MONTY_MOLE_HOLE,  -1435, -2559, -3118,  0,   0, 0,  0,  bhvMontyMoleHole),
    OBJECT          ( MODEL_DL_MONTY_MOLE_HOLE,  -1677, -2559, -3507,  0,   0, 0,  0,  bhvMontyMoleHole),
    OBJECT          ( MODEL_DL_MONTY_MOLE_HOLE,  -1869, -2559, -2704,  0,   0, 0,  0,  bhvMontyMoleHole),
    OBJECT          ( MODEL_DL_MONTY_MOLE_HOLE,  -2525, -2559, -2626,  0,   0, 0,  0,  bhvMontyMoleHole),
    OBJECT          ( MODEL_MONTY_MOLE,              0,     0,     0,  0,   0, 0,  BPARAM2(MONTY_MOLE_BP_NO_ROCK),  bhvMontyMole),
    OBJECT          ( MODEL_MONTY_MOLE,              0,     0,     0,  0,   0, 0,  BPARAM2(MONTY_MOLE_BP_GENERIC),  bhvMontyMole),
    OBJECT          ( MODEL_NONE,                 3625,   560,   165,  0, 330, 0,  BPARAM2(CLOUD_BP_FWOOSH),  bhvCloud),
    OBJECT_WITH_ACTS( MODEL_UKIKI,                 729,  2307,   335,  0,   0, 0,  BPARAM2(UKIKI_BP_CAGE),  bhvUkiki,        ACT_2),
    OBJECT_WITH_ACTS( MODEL_UKIKI,                1992, -1548,  2944,  0,   0, 0,  BPARAM2(UKIKI_BP_CAP),  bhvUkiki,        ALL_ACTS),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT_WITH_ACTS( MODEL_STAR,             1200,  2600,   150,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_1),  bhvStar,               ACT_1),
    OBJECT_WITH_ACTS( MODEL_TTM_STAR_CAGE,    2496,  1670,  1492,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_2),  bhvUkikiCage,          ACT_2),
    OBJECT_WITH_ACTS( MODEL_NONE,            -3250, -2500, -3700,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_3),  bhvHiddenRedCoinStar,  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,            -2900, -2700,  3650,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_4),  bhvStar,               ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,             1800,  1200,  1050,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_5),  bhvStar,               ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,             7300, -3100,  1300,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_6),  bhvStar,               ALL_ACTS),
    RETURN(),
};

static const LevelScript script_func_local_4[] = {
    OBJECT( MODEL_TTM_BLUE_SMILEY,     4389,  3620,   624,  0, 0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_TTM_YELLOW_SMILEY,  -1251,  2493,  2224,  0, 0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_TTM_STAR_SMILEY,    -2547,  1365,  -520,  0, 0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_TTM_MOON_SMILEY,     -324,   989, -4090,  0, 0, 0,  0,  bhvStaticObject),
    RETURN(),
};

static const LevelScript script_func_local_5[] = {
    OBJECT( MODEL_TTM_BLUE_SMILEY,     7867,  -959, -6085,  0, 0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_TTM_BLUE_SMILEY,    -5241,  5329,  9466,  0, 0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_TTM_YELLOW_SMILEY,  -1869, -5311,  7358,  0, 0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_TTM_STAR_SMILEY,    -9095,  4262,  5348,  0, 0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_TTM_MOON_SMILEY,    -8477,   730, -7122,  0, 0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_TTM_MOON_SMILEY,     6160, -6076,  7861,  0, 0, 0,  0,  bhvStaticObject),
    RETURN(),
};

static const LevelScript script_func_local_6[] = {
    OBJECT( MODEL_TTM_YELLOW_SMILEY,   5157,  1974, -8292,  0, 0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_TTM_STAR_SMILEY,    11106,  2588,   381,  0, 0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_TTM_MOON_SMILEY,       37,  1974, -1124,  0, 0, 0,  0,  bhvStaticObject),
    RETURN(),
};

static const LevelScript script_func_local_7[] = {
    RETURN(),
};

const LevelScript level_ttm_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _ttm_segment_7SegmentRomStart, _ttm_segment_7SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _mountain_mio0SegmentRomStart, _mountain_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _water_skybox_mio0SegmentRomStart, _water_skybox_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group6_mio0SegmentRomStart, _group6_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group6_geoSegmentRomStart,  _group6_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_7),
    LOAD_MODEL_FROM_GEO(MODEL_TTM_SLIDE_EXIT_PODIUM, ttm_geo_000DF4),
    LOAD_MODEL_FROM_GEO(MODEL_TTM_ROLLING_LOG,       ttm_geo_000730),
    LOAD_MODEL_FROM_GEO(MODEL_TTM_STAR_CAGE,         ttm_geo_000710),
    LOAD_MODEL_FROM_GEO(MODEL_TTM_BLUE_SMILEY,       ttm_geo_000D14),
    LOAD_MODEL_FROM_GEO(MODEL_TTM_YELLOW_SMILEY,     ttm_geo_000D4C),
    LOAD_MODEL_FROM_GEO(MODEL_TTM_STAR_SMILEY,       ttm_geo_000D84),
    LOAD_MODEL_FROM_GEO(MODEL_TTM_MOON_SMILEY,       ttm_geo_000DBC),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03,     ttm_geo_000748),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_04,     ttm_geo_000778),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_05,     ttm_geo_0007A8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_06,     ttm_geo_0007D8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_07,     ttm_geo_000808),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_08,     ttm_geo_000830),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_09,     ttm_geo_000858),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0A,     ttm_geo_000880),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0B,     ttm_geo_0008A8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0C,     ttm_geo_0008D0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0D,     ttm_geo_0008F8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0F,     ttm_geo_000920),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_10,     ttm_geo_000948),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_11,     ttm_geo_000970),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_12,     ttm_geo_000990),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_13,     ttm_geo_0009C0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_14,     ttm_geo_0009F0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_15,     ttm_geo_000A18),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_16,     ttm_geo_000A40),

    AREA( 1, ttm_geo_000A70),
        OBJECT( MODEL_NONE,    102, -3332,  5734,  0,   45, 0,  BPARAM2(WARP_NODE_0A),  bhvSpinAirborneWarp),
        OBJECT( MODEL_NONE,  -2447, -2457,  3952,  0, -105, 0,  BPARAM2(WARP_NODE_14),  bhvAirborneWarp),
        OBJECT( MODEL_NONE,   2267, -3006, -3788,  0,  148, 0,  BPARAM2(WARP_NODE_15),  bhvFadingWarp),
        OBJECT( MODEL_NONE,   -557, -3448, -4146,  0, -168, 0,  BPARAM2(WARP_NODE_16),  bhvFadingWarp),
        WARP_NODE( WARP_NODE_0A,  LEVEL_TTM,  1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_14,  LEVEL_TTM,  1,  WARP_NODE_14,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_15,  LEVEL_TTM,  1,  WARP_NODE_16,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_16,  LEVEL_TTM,  1,  WARP_NODE_15,  WARP_NO_CHECKPOINT),
        PAINTING_WARP_NODE( WARP_NODE_00,  LEVEL_TTM,  2,  WARP_NODE_0A,  WARP_CHECKPOINT),
        PAINTING_WARP_NODE( WARP_NODE_01,  LEVEL_TTM,  2,  WARP_NODE_0A,  WARP_CHECKPOINT),
        PAINTING_WARP_NODE( WARP_NODE_02,  LEVEL_TTM,  2,  WARP_NODE_0A,  WARP_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,     LEVEL_CASTLE,  2,  WARP_NODE_34,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,       LEVEL_CASTLE,  2,  WARP_NODE_66,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        TERRAIN( ttm_seg7_area_1_collision),
        MACRO_OBJECTS( ttm_seg7_area_1_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0000,  SEQ_LEVEL_GRASS),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    AREA( 2, ttm_geo_000B5C),
        OBJECT( MODEL_NONE,  7000, 5381, 6750,  0, 225, 0,  BPARAM2(WARP_NODE_0A),  bhvAirborneWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_TTM,     2,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  2,  WARP_NODE_34,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  2,  WARP_NODE_66,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_4),
        TERRAIN( ttm_seg7_area_2_collision),
        MACRO_OBJECTS( ttm_seg7_area_2_macro_objs),
        INSTANT_WARP( 2,  3,  10240, 7168, 10240),
        SET_BACKGROUND_MUSIC( 0x0001,  SEQ_LEVEL_SLIDE),
        TERRAIN_TYPE( TERRAIN_SLIDE),
    END_AREA(),

    AREA( 3, ttm_geo_000BEC),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  2,  WARP_NODE_34,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  2,  WARP_NODE_66,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_5),
        TERRAIN( ttm_seg7_area_3_collision),
        MACRO_OBJECTS( ttm_seg7_area_3_macro_objs),
        INSTANT_WARP( 3,  4,  -11264, 13312, 3072),
        SET_BACKGROUND_MUSIC( 0x0001,  SEQ_LEVEL_SLIDE),
        TERRAIN_TYPE( TERRAIN_SLIDE),
    END_AREA(),

    AREA( 4, ttm_geo_000C84),
        OBJECT( MODEL_TTM_SLIDE_EXIT_PODIUM,  -7285, -1866, -4812,  0, 0, 0,  BPARAM2(WARP_NODE_0A),  bhvExitPodiumWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_TTM,     1,  WARP_NODE_14,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  2,  WARP_NODE_34,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  2,  WARP_NODE_66,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_6),
        JUMP_LINK(script_func_local_7),
        TERRAIN( ttm_seg7_area_4_collision),
        MACRO_OBJECTS( ttm_seg7_area_4_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0001,  SEQ_LEVEL_SLIDE),
        TERRAIN_TYPE( TERRAIN_SLIDE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  45,  102, -4332, 5734),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
