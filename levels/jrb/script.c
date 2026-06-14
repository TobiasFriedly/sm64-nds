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
#include "levels/jrb/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT_WITH_ACTS( MODEL_JRB_SUNKEN_SHIP,            2385,  3589,  3727,  0,   0, 0,  0,  bhvSunkenShipPart,           ACT_1),
    OBJECT_WITH_ACTS( MODEL_JRB_SUNKEN_SHIP_BACK,       2385,  3589,  3727,  0,   0, 0,  0,  bhvSunkenShipPart,           ACT_1),
    OBJECT_WITH_ACTS( MODEL_JRB_SHIP_LEFT_HALF_PART,    5385, -5520,  2428,  0,   0, 0,  0,  bhvSunkenShipPart2,         ACT_1),
    OBJECT_WITH_ACTS( MODEL_JRB_SHIP_RIGHT_HALF_PART,   5385, -5520,  2428,  0,   0, 0,  0,  bhvSunkenShipPart2,         ACT_1),
    OBJECT_WITH_ACTS( MODEL_NONE,                       5385, -5520,  2428,  0,   0, 0,  0,  bhvInSunkenShip,             ACT_1),
    OBJECT_WITH_ACTS( MODEL_NONE,                       5385, -5520,  2428,  0,   0, 0,  0,  bhvInSunkenShip2,           ACT_1),
    OBJECT_WITH_ACTS( MODEL_JRB_SHIP_LEFT_HALF_PART,    4880,   820,  2375,  0,   0, 0,  0,  bhvShipPart3,                ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_JRB_SHIP_BACK_LEFT_PART,    4880,   820,  2375,  0,   0, 0,  0,  bhvShipPart3,                ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_JRB_SHIP_RIGHT_HALF_PART,   4880,   820,  2375,  0,   0, 0,  0,  bhvShipPart3,                ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_JRB_SHIP_BACK_RIGHT_PART,   4880,   820,  2375,  0,   0, 0,  0,  bhvShipPart3,                ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_NONE,                       4880,   820,  2375,  0,   0, 0,  0,  bhvInSunkenShip3,           ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_JRB_SLIDING_BOX,            4668,  1434,  2916,  0,   0, 0,  0,  bhvJRBSlidingBox,            ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_UNAGI,                      6048, -5381,  1154,  0, 340, 0,  0,  bhvUnagi,                      ACT_1),
    OBJECT_WITH_ACTS( MODEL_UNAGI,                      8270, -3130,  1846,  0, 285, 0,  BPARAM1(STAR_INDEX_ACT_2) | BPARAM2(0x01),  bhvUnagi,                      ACT_2),
    OBJECT_WITH_ACTS( MODEL_UNAGI,                      6048, -5381,  1154,  0, 340, 0,  BPARAM1(0x02) | BPARAM2(0x02),  bhvUnagi,                      ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_NONE,                       4988, -5221,  2473,  0,   0, 0,  0,  bhvJetStream,                 ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT          ( MODEL_NONE,                      -1800, -2812, -2100,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_3),  bhvTreasureChestsJRB),
    OBJECT_WITH_ACTS( MODEL_BOBOMB_BUDDY,              -1956,  1331,  6500,  0,   0, 0,  0,  bhvBobombBuddyOpensCannon,  ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT( MODEL_JRB_ROCK,                   1834, -2556, -7090,  0, 194, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                  -2005, -2556, -3506,  0, 135, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   1578, -2556, -5554,  0,  90, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                     42, -2556, -6578,  0, 135, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   -981, -2556, -5298,  0, 255, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                  -6549,  1536,  4343,  0,  90, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   1322, -2556, -3506,  0, 165, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   6442, -2556, -6322,  0, 135, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   3882, -2556, -5042,  0,   0, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   1497,  1741,  7810,  0,  14, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                  -3978,  1536,   -85,  0,   0, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                  -5228,  1230,  2106,  0, 323, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                  -7481,  1536,   185,  0, 149, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                  -5749,  1536, -1113,  0, 255, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                  -5332,  1434,  1023,  0, 315, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   -815,  -613,  3556,  0, 315, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                  -3429,   819,  4948,  0, 284, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                  -1940,   410,  2377,  0, 194, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                  -1798,  -716,  4330,  0,   0, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   -845,   922,  7668,  0, 315, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   6741, -2886,  3556,  0, 135, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                    255,  -101,  4719,  0,  45, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   1787,  -306,  5133,  0, 315, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   1079,  -613,  2299,  0,  75, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   2931, -1697,   980,  0, 315, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   3036, -4709,  4027,  0, 315, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   4222, -1125,  7083,  0, 104, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   6650,  -613,  4941,  0, 315, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   5764, -4709,  4427,  0, 315, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   3693, -4709,   856,  0, 135, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   7981,   410,  2704,  0, 165, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   2917, -3046,  4818,  0, 241, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_JRB_ROCK,                   5896,  -393,  -123,  0, 315, 0,  0,  bhvRockSolid),
    OBJECT( MODEL_NONE,                         53,  2355,  2724,  0,   0, 0,  BPARAM2(41),  bhvPoleGrabbing),
    OBJECT( MODEL_NONE,                        659,  2560,  3314,  0,   0, 0,  BPARAM2(41),  bhvPoleGrabbing),
    OBJECT( MODEL_NONE,                       1087,  2150,  3798,  0,   0, 0,  BPARAM2(41),  bhvPoleGrabbing),
    OBJECT( MODEL_NONE,                      -2535,  1075,  6113,  0,   0, 0,  BPARAM2(97),  bhvPoleGrabbing),
    OBJECT( MODEL_JRB_FALLING_PILLAR,         2078, -2863, -4696,  0,  90, 0,  0,  bhvFallingPillar),
    OBJECT( MODEL_JRB_FALLING_PILLAR,        -1403, -2863, -4696,  0,  90, 0,  0,  bhvFallingPillar),
    OBJECT( MODEL_JRB_FALLING_PILLAR,        -1096, -2863, -3262,  0,  90, 0,  0,  bhvFallingPillar),
    OBJECT( MODEL_JRB_FALLING_PILLAR,          337, -2863, -5106,  0,  90, 0,  0,  bhvFallingPillar),
    OBJECT( MODEL_JRB_FALLING_PILLAR,         2078, -2863, -6232,  0,  90, 0,  0,  bhvFallingPillar),
    OBJECT( MODEL_JRB_FALLING_PILLAR,         4330, -2863, -5618,  0,  90, 0,  0,  bhvFallingPillar),
    OBJECT( MODEL_JRB_FALLING_PILLAR_BASE,    2078, -2966, -4696,  0,  90, 0,  0,  bhvPillarBase),
    OBJECT( MODEL_JRB_FALLING_PILLAR_BASE,   -1403, -2966, -4696,  0,  90, 0,  0,  bhvPillarBase),
    OBJECT( MODEL_JRB_FALLING_PILLAR_BASE,   -1096, -2966, -3262,  0,  90, 0,  0,  bhvPillarBase),
    OBJECT( MODEL_JRB_FALLING_PILLAR_BASE,     337, -2966, -5106,  0,  90, 0,  0,  bhvPillarBase),
    OBJECT( MODEL_JRB_FALLING_PILLAR_BASE,    2078, -2966, -6232,  0,  90, 0,  0,  bhvPillarBase),
    OBJECT( MODEL_JRB_FALLING_PILLAR_BASE,    4330, -2966, -5618,  0,  90, 0,  0,  bhvPillarBase),
    OBJECT( MODEL_JRB_FLOATING_PLATFORM,     -1059,  1025,  7072,  0, 247, 0,  0,  bhvJRBFloatingPlatform),
    OBJECT( MODEL_NONE,                      -4236, 1044, 2136,    0,   0, 0,  0,  bhvInsideCannon),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT_WITH_ACTS( MODEL_NONE,             4900,  2400,   800,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_4),  bhvHiddenRedCoinStar,  ALL_ACTS),
#if defined(VERSION_JP) || defined(VERSION_SH)
    OBJECT_WITH_ACTS( MODEL_STAR,             1540,  2160,  2130,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_5),  bhvStar,                  ALL_ACTS),
#else
    OBJECT_WITH_ACTS( MODEL_EXCLAMATION_BOX,  1540,  2160,  2130,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_5) | BPARAM2(EXCLAMATION_BOX_BP_STAR_ACT_1),  bhvExclamationBox,       ALL_ACTS),
#endif
    OBJECT_WITH_ACTS( MODEL_STAR,             5000, -4800,  2500,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_6),  bhvStar,                  ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    RETURN(),
};

static const LevelScript script_func_local_4[] = {
    OBJECT( MODEL_NONE,              400,  -350, -2700,  0, 0, 0,  0,  bhvTreasureChestsShip),
    RETURN(),
};

static const LevelScript script_func_local_5[] = {
    RETURN(),
};

const LevelScript level_jrb_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _jrb_segment_7SegmentRomStart, _jrb_segment_7SegmentRomEnd),
    LOAD_MIO0        ( 0x0B, _effect_mio0SegmentRomStart, _effect_mio0SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _water_mio0SegmentRomStart, _water_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _clouds_skybox_mio0SegmentRomStart, _clouds_skybox_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group4_mio0SegmentRomStart, _group4_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group4_geoSegmentRomStart, _group4_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x06, _group13_mio0SegmentRomStart, _group13_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0D, _group13_geoSegmentRomStart,  _group13_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_5),
    JUMP_LINK(script_func_global_14),
    LOAD_MODEL_FROM_GEO(MODEL_JRB_SHIP_LEFT_HALF_PART,  jrb_geo_000978),
    LOAD_MODEL_FROM_GEO(MODEL_JRB_SHIP_BACK_LEFT_PART,  jrb_geo_0009B0),
    LOAD_MODEL_FROM_GEO(MODEL_JRB_SHIP_RIGHT_HALF_PART, jrb_geo_0009E8),
    LOAD_MODEL_FROM_GEO(MODEL_JRB_SHIP_BACK_RIGHT_PART, jrb_geo_000A00),
    LOAD_MODEL_FROM_GEO(MODEL_JRB_SUNKEN_SHIP,          jrb_geo_000990),
    LOAD_MODEL_FROM_GEO(MODEL_JRB_SUNKEN_SHIP_BACK,     jrb_geo_0009C8),
    LOAD_MODEL_FROM_GEO(MODEL_JRB_ROCK,                 jrb_geo_000930),
    LOAD_MODEL_FROM_GEO(MODEL_JRB_SLIDING_BOX,          jrb_geo_000960),
    LOAD_MODEL_FROM_GEO(MODEL_JRB_FALLING_PILLAR,       jrb_geo_000900),
    LOAD_MODEL_FROM_GEO(MODEL_JRB_FALLING_PILLAR_BASE,  jrb_geo_000918),
    LOAD_MODEL_FROM_GEO(MODEL_JRB_FLOATING_PLATFORM,    jrb_geo_000948),

    AREA( 1, jrb_geo_000A18),
        OBJECT( MODEL_NONE,  -6750, 2126, 1482,  0, 90, 0,  BPARAM2(WARP_NODE_0A),  bhvSpinAirborneWarp),
        WARP_NODE( WARP_NODE_0A,          LEVEL_JRB,     1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_WARP_FLOOR,  LEVEL_JRB,     2,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,     LEVEL_CASTLE,  1,  WARP_NODE_35,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,       LEVEL_CASTLE,  1,  WARP_NODE_67,  WARP_NO_CHECKPOINT),
        WHIRLPOOL( 0,  3,  4979, -5222, 2482,  -30),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        TERRAIN( jrb_seg7_area_1_collision),
        MACRO_OBJECTS( jrb_seg7_area_1_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0003,  SEQ_LEVEL_WATER),
        TERRAIN_TYPE( TERRAIN_WATER),
    END_AREA(),

    AREA( 2, jrb_geo_000AFC),
        OBJECT( MODEL_NONE,  928, 1050, -1248,  0, 180, 0,  BPARAM2(WARP_NODE_0A),  bhvSwimmingWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_JRB,     2,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  1,  WARP_NODE_35,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  1,  WARP_NODE_67,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_4),
        JUMP_LINK(script_func_local_5),
        TERRAIN( jrb_seg7_area_2_collision),
        MACRO_OBJECTS( jrb_seg7_area_2_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0003,  SEQ_LEVEL_WATER),
        TERRAIN_TYPE( TERRAIN_WATER),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  90,  -6750, 1126, 1482),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
