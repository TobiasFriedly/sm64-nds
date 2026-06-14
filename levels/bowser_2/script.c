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
#include "levels/bowser_2/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_BOWSER_2_TILTING_ARENA,      0,    0,     0,  0, 90, 0,  0,  bhvTiltingBowserLavaPlatform),
    OBJECT( MODEL_BOWSER_BOMB,                 4, 1329,  3598,  0, 90, 0,  0,  bhvBowserBomb),
    OBJECT( MODEL_BOWSER_BOMB,              3584, 1329,     0,  0, 90, 0,  0,  bhvBowserBomb),
    OBJECT( MODEL_BOWSER_BOMB,                 0, 1329, -3583,  0, 90, 0,  0,  bhvBowserBomb),
    OBJECT( MODEL_BOWSER_BOMB,             -3583, 1329,     0,  0, 90, 0,  0,  bhvBowserBomb),
    RETURN(),
};

const LevelScript level_bowser_2_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x0A, _bitfs_skybox_mio0SegmentRomStart, _bitfs_skybox_mio0SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _fire_mio0SegmentRomStart, _fire_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x0B, _effect_mio0SegmentRomStart, _effect_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x07, _bowser_2_segment_7SegmentRomStart, _bowser_2_segment_7SegmentRomEnd),
    LOAD_MIO0        ( 0x06, _group12_mio0SegmentRomStart, _group12_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0D, _group12_geoSegmentRomStart, _group12_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_13),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_2_TILTING_ARENA, bowser_2_geo_000170),

    AREA( 1, bowser_2_geo_000188),
        OBJECT( MODEL_NONE,  0, 2229, 0,  0, 180, 0,  BPARAM2(WARP_NODE_0A),  bhvSpinAirborneCircleWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_BOWSER_2,  1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,    3,  WARP_NODE_36,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_BITFS,     1,  WARP_NODE_0C,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        TERRAIN( bowser_2_seg7_collision_lava),
        SET_BACKGROUND_MUSIC( 0x0002,  SEQ_LEVEL_BOSS_KOOPA),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  180,  0, 1229, 0),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
