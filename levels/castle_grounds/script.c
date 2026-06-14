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
#include "levels/castle_grounds/header.h"

static const LevelScript script_func_local_1[] = {
    WARP_NODE( WARP_NODE_00,  LEVEL_CASTLE,          1,  WARP_NODE_00,  WARP_NO_CHECKPOINT),
    WARP_NODE( WARP_NODE_01,  LEVEL_CASTLE,          1,  WARP_NODE_01,  WARP_NO_CHECKPOINT),
    WARP_NODE( WARP_NODE_02,  LEVEL_CASTLE,          3,  WARP_NODE_02,  WARP_NO_CHECKPOINT),
    OBJECT( MODEL_NONE,      0,   900, -1710,  0, 180, 0,  BPARAM2(WARP_NODE_03),  bhvDeathWarp),
    WARP_NODE( WARP_NODE_03,  LEVEL_CASTLE_GROUNDS,  1,  WARP_NODE_03,  WARP_NO_CHECKPOINT),
    OBJECT( MODEL_NONE,  -1328,   260,  4664,  0, 180, 0,  BPARAM2(WARP_NODE_04),  bhvSpinAirborneCircleWarp),
    WARP_NODE( WARP_NODE_04,  LEVEL_CASTLE_GROUNDS,  1,  WARP_NODE_04,  WARP_NO_CHECKPOINT),
    OBJECT( MODEL_NONE,  -3379,  -815, -2025,  0,   0, 0,  BPARAM1(60) | BPARAM2(WARP_NODE_05),  bhvWarp),
    OBJECT( MODEL_NONE,  -3379,  -500, -2025,  0, 180, 0,  BPARAM2(WARP_NODE_06),  bhvLaunchDeathWarp),
    OBJECT( MODEL_NONE,  -3799, -1199, -5816,  0,   0, 0,  BPARAM2(WARP_NODE_07),  bhvSwimmingWarp),
    OBJECT( MODEL_NONE,  -3379,  -500, -2025,  0, 180, 0,  BPARAM2(WARP_NODE_08),  bhvLaunchStarCollectWarp),
    WARP_NODE( WARP_NODE_05,  LEVEL_VCUTM,           1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
    WARP_NODE( WARP_NODE_06,  LEVEL_CASTLE_GROUNDS,  1,  WARP_NODE_06,  WARP_NO_CHECKPOINT),
    WARP_NODE( WARP_NODE_07,  LEVEL_CASTLE_GROUNDS,  1,  WARP_NODE_07,  WARP_NO_CHECKPOINT),
    WARP_NODE( WARP_NODE_08,  LEVEL_CASTLE_GROUNDS,  1,  WARP_NODE_08,  WARP_NO_CHECKPOINT),
    OBJECT( MODEL_NONE,   5408,  4500,  3637,  0, 225, 0,  BPARAM2(WARP_NODE_0A),  bhvAirborneWarp),
    WARP_NODE( WARP_NODE_0A,  LEVEL_CASTLE_GROUNDS,  1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
    OBJECT( MODEL_NONE,  -6901,  2376, -6509,  0, 230, 0,  BPARAM2(WARP_NODE_14),  bhvAirborneWarp),
    WARP_NODE( WARP_NODE_14,  LEVEL_CASTLE_GROUNDS,  1,  WARP_NODE_14,  WARP_NO_CHECKPOINT),
    OBJECT( MODEL_NONE,   4997, -1250,  2258,  0, 210, 0,  BPARAM2(WARP_NODE_1E),  bhvSwimmingWarp),
    WARP_NODE( WARP_NODE_1E,  LEVEL_CASTLE_GROUNDS,  1,  WARP_NODE_1E,  WARP_NO_CHECKPOINT),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT( MODEL_NONE,                         -5812,  100, -5937,  0,   0, 0,  0,  bhvWaterfallSoundLoop),
    OBJECT( MODEL_NONE,                         -7430, 1500,   873,  0,   0, 0,  BPARAM2(0x00),  bhvBirdsSoundLoop),
    OBJECT( MODEL_NONE,                           -80, 1500,  5004,  0,   0, 0,  BPARAM2(0x01),  bhvBirdsSoundLoop),
    OBJECT( MODEL_NONE,                          7131, 1500, -2989,  0,   0, 0,  BPARAM2(0x02),  bhvBirdsSoundLoop),
    OBJECT( MODEL_NONE,                         -7430, 1500, -5937,  0,   0, 0,  0,  bhvAmbientSounds),
    OBJECT( MODEL_CASTLE_GROUNDS_VCUTM_GRILL,       0,    0,     0,  0,   0, 0,  0,  bhvMoatGrills),
    OBJECT( MODEL_NONE,                             0,    0,     0,  0,   0, 0,  0,  bhvInvisibleObjectsUnderBridge),
    OBJECT( MODEL_MIST,                         -4878, -787, -5690,  0,   0, 0,  0,  bhvWaterMist2),
    OBJECT( MODEL_MIST,                         -4996, -787, -5548,  0,   0, 0,  BPARAM2(0x01),  bhvWaterMist2),
    OBJECT( MODEL_MIST,                         -5114, -787, -5406,  0,   0, 0,  BPARAM2(0x02),  bhvWaterMist2),
    OBJECT( MODEL_MIST,                         -5212, -787, -5219,  0,   0, 0,  BPARAM2(0x03),  bhvWaterMist2),
    OBJECT( MODEL_MIST,                         -5311, -787, -5033,  0,   0, 0,  BPARAM2(0x04),  bhvWaterMist2),
    OBJECT( MODEL_MIST,                         -5419, -787, -4895,  0,   0, 0,  BPARAM2(0x05),  bhvWaterMist2),
    OBJECT( MODEL_MIST,                         -5527, -787, -4757,  0,   0, 0,  BPARAM2(0x06),  bhvWaterMist2),
    OBJECT( MODEL_MIST,                         -5686, -787, -4733,  0,   0, 0,  BPARAM2(0x07),  bhvWaterMist2),
    OBJECT( MODEL_MIST,                         -5845, -787, -4710,  0,   0, 0,  BPARAM2(0x08),  bhvWaterMist2),
    OBJECT( MODEL_NONE,                          5223, -975,  1667,  0,   0, 0,  0,  bhvManyBlueFishSpawner),
    OBJECT( MODEL_BIRDS,                        -5069,  850,  3221,  0,   0, 0,  BPARAM2(BIRD_BP_SPAWNER),  bhvBird),
    OBJECT( MODEL_BIRDS,                        -4711,  742,   433,  0,   0, 0,  BPARAM2(BIRD_BP_SPAWNER),  bhvBird),
    OBJECT( MODEL_BIRDS,                         5774,  913, -1114,  0,   0, 0,  BPARAM2(BIRD_BP_SPAWNER),  bhvBird),
    OBJECT( MODEL_NONE,                         -1328,  260,  4664,  0, 180, 0,  BPARAM2(0x28),  bhvIntroScene),
    OBJECT( MODEL_CASTLE_GROUNDS_CANNON_GRILL,      0,    0,     0,  0,   0, 0,  0,  bhvHiddenAt120Stars),
    OBJECT( MODEL_LAKITU,                          11,  803, -3015,  0,   0, 0,  BPARAM2(CAMERA_LAKITU_BP_INTRO),  bhvCameraLakitu),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT( MODEL_CASTLE_GROUNDS_FLAG,  -3213, 3348, -3011,  0, 0, 0,  0,  bhvCastleFlagWaving),
    OBJECT( MODEL_CASTLE_GROUNDS_FLAG,   3213, 3348, -3011,  0, 0, 0,  0,  bhvCastleFlagWaving),
    OBJECT( MODEL_CASTLE_GROUNDS_FLAG,  -3835, 3348, -6647,  0, 0, 0,  0,  bhvCastleFlagWaving),
    OBJECT( MODEL_CASTLE_GROUNDS_FLAG,   3835, 3348, -6647,  0, 0, 0,  0,  bhvCastleFlagWaving),
    RETURN(),
};

static const LevelScript script_func_local_4[] = {
    OBJECT( MODEL_BUTTERFLY,  -4508,  406,  4400,  0, 0, 0,  0,  bhvButterfly),
    OBJECT( MODEL_BUTTERFLY,  -4408,  406,  4500,  0, 0, 0,  0,  bhvButterfly),
    OBJECT( MODEL_BUTTERFLY,  -4708,  406,  4100,  0, 0, 0,  0,  bhvButterfly),
    OBJECT( MODEL_BUTTERFLY,  -6003,  473, -2621,  0, 0, 0,  0,  bhvButterfly),
    OBJECT( MODEL_BUTTERFLY,  -6003,  473, -2321,  0, 0, 0,  0,  bhvButterfly),
    OBJECT( MODEL_BUTTERFLY,   6543,  461,  -617,  0, 0, 0,  0,  bhvButterfly),
    OBJECT( MODEL_BUTTERFLY,   6143,  461,  -617,  0, 0, 0,  0,  bhvButterfly),
    OBJECT( MODEL_BUTTERFLY,   5773,  775, -5722,  0, 0, 0,  0,  bhvButterfly),
    OBJECT( MODEL_BUTTERFLY,   5873,  775, -5622,  0, 0, 0,  0,  bhvButterfly),
    OBJECT( MODEL_BUTTERFLY,   5473,  775, -5322,  0, 0, 0,  0,  bhvButterfly),
    OBJECT( MODEL_BUTTERFLY,  -1504,  326,  3196,  0, 0, 0,  0,  bhvButterfly),
    OBJECT( MODEL_BUTTERFLY,  -1204,  326,  3296,  0, 0, 0,  0,  bhvButterfly),
    OBJECT( MODEL_YOSHI,          0, 3174, -5625,  0, 0, 0,  0,  bhvYoshi),
    RETURN(),
};

const LevelScript level_castle_grounds_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _castle_grounds_segment_7SegmentRomStart, _castle_grounds_segment_7SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _water_skybox_mio0SegmentRomStart, _water_skybox_mio0SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _outside_mio0SegmentRomStart, _outside_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group10_mio0SegmentRomStart, _group10_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group10_geoSegmentRomStart,  _group10_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x06, _group15_mio0SegmentRomStart, _group15_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0D, _group15_geoSegmentRomStart,  _group15_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_11),
    JUMP_LINK(script_func_global_16),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03,           castle_grounds_geo_0006F4),
    LOAD_MODEL_FROM_GEO(MODEL_CASTLE_GROUNDS_BUBBLY_TREE,  bubbly_tree_geo),
    LOAD_MODEL_FROM_GEO(MODEL_CASTLE_GROUNDS_WARP_PIPE,    warp_pipe_geo),
    LOAD_MODEL_FROM_GEO(MODEL_CASTLE_GROUNDS_CASTLE_DOOR,  castle_door_geo),
    LOAD_MODEL_FROM_GEO(MODEL_CASTLE_GROUNDS_METAL_DOOR,   metal_door_geo),
    LOAD_MODEL_FROM_GEO(MODEL_CASTLE_GROUNDS_VCUTM_GRILL,  castle_grounds_geo_00070C),
    LOAD_MODEL_FROM_GEO(MODEL_CASTLE_GROUNDS_FLAG,         castle_grounds_geo_000660),
    LOAD_MODEL_FROM_GEO(MODEL_CASTLE_GROUNDS_CANNON_GRILL, castle_grounds_geo_000724),

    AREA( 1, castle_grounds_geo_00073C),
        WARP_NODE( WARP_NODE_DEATH,  LEVEL_CASTLE_GROUNDS,  1,  WARP_NODE_03,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        JUMP_LINK(script_func_local_4),
        TERRAIN( castle_grounds_seg7_collision_level),
        MACRO_OBJECTS( castle_grounds_seg7_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0000,  SEQ_SOUND_PLAYER),
        TERRAIN_TYPE( TERRAIN_GRASS),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  180,  -1328, 260, 4664),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
