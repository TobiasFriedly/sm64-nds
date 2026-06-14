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
#include "actors/group12.h"

#include "make_const_nonconst.h"
#include "levels/bowser_1/header.h"

const LevelScript level_bowser_1_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0( 0x07, _bowser_1_segment_7SegmentRomStart, _bowser_1_segment_7SegmentRomEnd),
    LOAD_MIO0( 0x0A, _bidw_skybox_mio0SegmentRomStart, _bidw_skybox_mio0SegmentRomEnd),
    LOAD_MIO0( 0x06, _group12_mio0SegmentRomStart, _group12_mio0SegmentRomEnd),
    LOAD_RAW ( 0x0D, _group12_geoSegmentRomStart, _group12_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_13),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03, bowser_1_yellow_sphere_geo),

    AREA( 1, bowser_1_geo_0000D0),
        OBJECT( MODEL_NONE,  0, 1307, 0,  0, 180, 0,  BPARAM2(WARP_NODE_0A),  bhvSpinAirborneCircleWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_BOWSER_1,  1,  WARP_NODE_0A,   WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,    1,  WARP_NODE_24,   WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_BITDW,     1,  WARP_NODE_0C,   WARP_NO_CHECKPOINT),
        TERRAIN( bowser_1_seg7_collision_level),
        SET_BACKGROUND_MUSIC( 0x0002,  SEQ_LEVEL_BOSS_KOOPA),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  180,  0, 307, 0),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
