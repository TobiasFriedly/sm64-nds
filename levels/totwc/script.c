#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "dialog_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "game/level_update.h"

#include "levels/scripts.h"

#include "actors/common1.h"

#include "make_const_nonconst.h"
#include "levels/totwc/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_CAP_SWITCH,    0, -2047, 10,  0, 0, 0,  0,  bhvCapSwitch),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT( MODEL_NONE,        800, -1700,  0,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_1),  bhvHiddenRedCoinStar),
    RETURN(),
};

const LevelScript level_totwc_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _totwc_segment_7SegmentRomStart, _totwc_segment_7SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _cloud_floor_skybox_mio0SegmentRomStart, _cloud_floor_skybox_mio0SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _sky_mio0SegmentRomStart, _sky_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group8_mio0SegmentRomStart, _group8_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group8_geoSegmentRomStart,  _group8_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_9),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03, totwc_geo_000160),

    AREA( 1, totwc_geo_000188),
        OBJECT( MODEL_NONE,  -4095, 2935, 0,  0, 90, 0,  BPARAM2(WARP_NODE_0A),  bhvFlyingWarp),
        WARP_NODE( WARP_NODE_0A,          LEVEL_TOTWC,   1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_WARP_FLOOR,  LEVEL_CASTLE,  1,  WARP_NODE_20,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,     LEVEL_CASTLE,  1,  WARP_NODE_26,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,       LEVEL_CASTLE,  1,  WARP_NODE_23,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_1),
        TERRAIN( totwc_seg7_collision),
        MACRO_OBJECTS( totwc_seg7_macro_objs),
        SHOW_DIALOG( 0x00, DIALOG_131),
        SET_BACKGROUND_MUSIC( 0x0000,  SEQ_LEVEL_SLIDE),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  90,  -4095, 2935, 0),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
