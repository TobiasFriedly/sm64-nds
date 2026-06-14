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
#include "levels/wmotr/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_NONE,   3996, -2739,  5477,  0, 0, 0,  BPARAM2(82),   bhvPoleGrabbing),
    OBJECT( MODEL_NONE,  -2911,  3564, -3967,  0, 0, 0,  BPARAM2(84),   bhvPoleGrabbing),
    OBJECT( MODEL_NONE,  -3258,  3359, -3946,  0, 0, 0,  BPARAM2(105),  bhvPoleGrabbing),
    OBJECT( MODEL_NONE,  -2639,  3154, -4369,  0, 0, 0,  BPARAM2(125),  bhvPoleGrabbing),
    OBJECT( MODEL_NONE,  -2980,  4048, -4248,  0, 0, 0,  BPARAM2(36),   bhvPoleGrabbing),
    OBJECT( MODEL_NONE,  -3290,  3636, -4477,  0, 0, 0,  BPARAM2(77),   bhvPoleGrabbing),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT( MODEL_NONE,  -160, 1950, -470,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_1),  bhvHiddenRedCoinStar),
    RETURN(),
};

const LevelScript level_wmotr_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _wmotr_segment_7SegmentRomStart, _wmotr_segment_7SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _cloud_floor_skybox_mio0SegmentRomStart, _cloud_floor_skybox_mio0SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _sky_mio0SegmentRomStart, _sky_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group2_mio0SegmentRomStart, _group2_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group2_geoSegmentRomStart,  _group2_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x06, _group17_mio0SegmentRomStart, _group17_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0D, _group17_geoSegmentRomStart,  _group17_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_3),
    JUMP_LINK(script_func_global_18),

    AREA( 1, wmotr_geo_0001F0),
        OBJECT( MODEL_NONE,  -67, 2669, -16,  0, 270, 0,  BPARAM2(WARP_NODE_0A),  bhvAirborneWarp),
        WARP_NODE( WARP_NODE_0A,          LEVEL_WMOTR,           1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,     LEVEL_CASTLE,          2,  WARP_NODE_38,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,       LEVEL_CASTLE,          2,  WARP_NODE_6D,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_WARP_FLOOR,  LEVEL_CASTLE_GROUNDS,  1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        TERRAIN( wmotr_seg7_collision),
        MACRO_OBJECTS( wmotr_seg7_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0000,  SEQ_LEVEL_SLIDE),
        TERRAIN_TYPE( TERRAIN_SNOW),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  270,  -67, 1669, -16),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
