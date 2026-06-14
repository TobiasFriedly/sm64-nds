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
#include "levels/pss/header.h"

const LevelScript level_pss_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _pss_segment_7SegmentRomStart, _pss_segment_7SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _mountain_mio0SegmentRomStart, _mountain_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group8_mio0SegmentRomStart, _group8_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group8_geoSegmentRomStart,  _group8_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_9),

    AREA( 1, pss_geo_000100),
        OBJECT( MODEL_NONE,  5632, 6751, -5631,  0, 270, 0,  BPARAM2(WARP_NODE_0A),  bhvAirborneWarp),
        WARP_NODE( WARP_NODE_0A,          LEVEL_PSS,     1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_WARP_FLOOR,  LEVEL_CASTLE,  1,  WARP_NODE_20,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,     LEVEL_CASTLE,  1,  WARP_NODE_26,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,       LEVEL_CASTLE,  1,  WARP_NODE_23,  WARP_NO_CHECKPOINT),
        TERRAIN( pss_seg7_collision),
        MACRO_OBJECTS( pss_seg7_macro_objs),
        TERRAIN_TYPE( TERRAIN_SLIDE),
        SET_BACKGROUND_MUSIC( 0x0001,  SEQ_LEVEL_SLIDE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  270,  5632, 6451, -5631),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
