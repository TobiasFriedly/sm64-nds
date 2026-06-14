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
#include "levels/ddd/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_SUSHI,         -3071,  -270,   0,  0, 0, 0,  0,  bhvSushiShark),
    OBJECT( MODEL_SUSHI,         -3071, -4270,   0,  0, 0, 0,  0,  bhvSushiShark),
    OBJECT( MODEL_NONE,          -3071,  -130,   0,  0, 0, 0,  0,  bhvFewBlueFishSpawner),
    OBJECT( MODEL_NONE,          -3071, -4270,   0,  0, 0, 0,  0,  bhvManyBlueFishSpawner),
    OBJECT( MODEL_NONE,          -3071, -2000,   0,  0, 0, 0,  0,  bhvChirpChirp),
    OBJECT( MODEL_NONE,          -3071, -3000,   0,  0, 0, 0,  0,  bhvChirpChirp),
    OBJECT( MODEL_DL_WHIRLPOOL,  -3174, -4915, 102,  0, 0, 0,  0,  bhvWhirlpool),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT_WITH_ACTS( MODEL_NONE,       -2400, -4607, 125,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_2),  bhvTreasureChestsDDD,  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_MANTA_RAY,  -4640, -1380,  40,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_5),  bhvMantaRay,           ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT( MODEL_DDD_BOWSER_SUB_DOOR,     0,    0,     0,  0,   0, 0,  0,  bhvBowserSubDoor),
    OBJECT( MODEL_DDD_BOWSER_SUB,          0,    0,     0,  0,   0, 0,  0,  bhvBowsersSub),
    OBJECT( MODEL_DDD_POLE,             5120, 1005,  3584,  0, 180, 0,  BPARAM2(0x1E),  bhvDDDPole),
    OBJECT( MODEL_DDD_POLE,             5605, 1005,  3380,  0, 270, 0,  BPARAM2(0x15),  bhvDDDPole),
    OBJECT( MODEL_DDD_POLE,             1800, 1005,  1275,  0,   0, 0,  BPARAM2(0x0B),  bhvDDDPole),
    OBJECT( MODEL_DDD_POLE,             4000, 1005,  1075,  0, 180, 0,  BPARAM2(0x0B),  bhvDDDPole),
    OBJECT( MODEL_DDD_POLE,             1830, 1005,   520,  0, 270, 0,  BPARAM2(0x14),  bhvDDDPole),
    OBJECT( MODEL_DDD_POLE,             4000, 1005,  1275,  0,   0, 0,  BPARAM2(0x0B),  bhvDDDPole),
    OBJECT( MODEL_DDD_POLE,             5760, 1005,   360,  0, 270, 0,  BPARAM2(0x17),  bhvDDDPole),
    OBJECT( MODEL_DDD_POLE,             3310, 1005, -1945,  0,   0, 0,  BPARAM2(0x17),  bhvDDDPole),
    OBJECT( MODEL_DDD_POLE,             3550, 1005, -2250,  0,   0, 0,  BPARAM2(0x0D),  bhvDDDPole),
    RETURN(),
};

static const LevelScript script_func_local_4[] = {
    OBJECT( MODEL_NONE,  3404, -3319, -489,  0, 0, 0,  0,  bhvJetStream),
    RETURN(),
};

static const LevelScript script_func_local_5[] = {
    OBJECT_WITH_ACTS( MODEL_STAR,  3900,   850,  -600,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_1),  bhvStar,                     ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_NONE,  5513,  1200,   900,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_3),  bhvHiddenRedCoinStar,     ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_NONE,  3404, -3319,  -489,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_4),  bhvJetStreamRingSpawner,  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,  2030, -3700, -2780,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_6),  bhvStar,                     ALL_ACTS),
    RETURN(),
};

const LevelScript level_ddd_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _ddd_segment_7SegmentRomStart, _ddd_segment_7SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _water_mio0SegmentRomStart, _water_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x0B, _effect_mio0SegmentRomStart, _effect_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _water_skybox_mio0SegmentRomStart, _water_skybox_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group4_mio0SegmentRomStart, _group4_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group4_geoSegmentRomStart,  _group4_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x06, _group13_mio0SegmentRomStart, _group13_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0D, _group13_geoSegmentRomStart,  _group13_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_5),
    JUMP_LINK(script_func_global_14),
    LOAD_MODEL_FROM_GEO(MODEL_DDD_BOWSER_SUB_DOOR, ddd_geo_000478),
    LOAD_MODEL_FROM_GEO(MODEL_DDD_BOWSER_SUB,      ddd_geo_0004A0),
    LOAD_MODEL_FROM_GEO(MODEL_DDD_POLE,            ddd_geo_000450),

    AREA( 1, ddd_geo_0004C0),
        OBJECT( MODEL_NONE,  -3071, 3000, 0,  0, 7, 0,  BPARAM2(WARP_NODE_0A),  bhvSpinAirborneWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_DDD,     1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  3,  WARP_NODE_35,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  3,  WARP_NODE_67,  WARP_NO_CHECKPOINT),
        WHIRLPOOL( 0,  0,  -3174, -4915, 102,  20),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        INSTANT_WARP( 3,  2,  -8192, 0, 0),
        TERRAIN( ddd_seg7_area_1_collision),
        MACRO_OBJECTS( ddd_seg7_area_1_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0003,  SEQ_LEVEL_WATER),
        TERRAIN_TYPE( TERRAIN_WATER),
    END_AREA(),

    AREA( 2, ddd_geo_000570),
        WHIRLPOOL( 0,  0,  3355, -3575, -515,  -30),
        WHIRLPOOL( 1,  2,  3917, -2040, -6041,  50),
        WARP_NODE( WARP_NODE_SUCCESS,     LEVEL_CASTLE,          3,  WARP_NODE_35,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,       LEVEL_CASTLE,          3,  WARP_NODE_67,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_WARP_FLOOR,  LEVEL_CASTLE_GROUNDS,  1,  WARP_NODE_1E,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_3),
        JUMP_LINK(script_func_local_4),
        JUMP_LINK(script_func_local_5),
        INSTANT_WARP( 2,  1,  8192, 0, 0),
        TERRAIN( ddd_seg7_area_2_collision),
        MACRO_OBJECTS( ddd_seg7_area_2_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0003,  SEQ_LEVEL_WATER),
        TERRAIN_TYPE( TERRAIN_WATER),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  180,  -3071, 3000, 500),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
