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
#include "levels/castle_courtyard/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_NONE,      0, 200, -1652,  0, 0, 0,  0,  bhvAmbientSounds),
    OBJECT( MODEL_NONE,  -2700,   0, -1652,  0, 0, 0,  BPARAM2(0x00),  bhvBirdsSoundLoop),
    OBJECT( MODEL_NONE,   2700,   0, -1652,  0, 0, 0,  BPARAM2(0x01),  bhvBirdsSoundLoop),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT( MODEL_BOO,  -3217, 100,  -101,  0, 0, 0,  0,  bhvCourtyardBooTriplet),
    OBJECT( MODEL_BOO,   3317, 100, -1701,  0, 0, 0,  0,  bhvCourtyardBooTriplet),
    OBJECT( MODEL_BOO,    -71,   1, -1387,  0, 0, 0,  0,  bhvCourtyardBooTriplet),
    RETURN(),
};

const LevelScript level_castle_courtyard_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _castle_courtyard_segment_7SegmentRomStart, _castle_courtyard_segment_7SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _water_skybox_mio0SegmentRomStart, _water_skybox_mio0SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _outside_mio0SegmentRomStart, _outside_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group9_mio0SegmentRomStart, _group9_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group9_geoSegmentRomStart,  _group9_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_10),
    LOAD_MODEL_FROM_GEO(MODEL_COURTYARD_SPIKY_TREE,  spiky_tree_geo),
    LOAD_MODEL_FROM_GEO(MODEL_COURTYARD_WOODEN_DOOR, wooden_door_geo),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03,     castle_courtyard_geo_000200),

    AREA( 1, castle_courtyard_geo_000218),
        OBJECT( MODEL_BOO,   -2360, -100, -2712,  0,   0, 0,  BPARAM1(1) | BPARAM2(WARP_NODE_05),  bhvBooWithCage),
        OBJECT( MODEL_NONE,      0,   51, -1000,  0, 180, 0,  BPARAM2(WARP_NODE_0A),  bhvLaunchStarCollectWarp),
        OBJECT( MODEL_NONE,      0,   51, -1000,  0, 180, 0,  BPARAM2(WARP_NODE_0B),  bhvLaunchDeathWarp),
        WARP_NODE( WARP_NODE_05,     LEVEL_BBH,               1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0A,     LEVEL_CASTLE_COURTYARD,  1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0B,     LEVEL_CASTLE_COURTYARD,  1,  WARP_NODE_0B,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_01,     LEVEL_CASTLE,            1,  WARP_NODE_02,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,  LEVEL_CASTLE_GROUNDS,    1,  WARP_NODE_03,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        TERRAIN( castle_courtyard_seg7_collision),
        MACRO_OBJECTS( castle_courtyard_seg7_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0000,  SEQ_SOUND_PLAYER),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  0,  -14, 0, -201),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
