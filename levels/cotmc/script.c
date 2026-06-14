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
#include "levels/cotmc/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_CAP_SWITCH,  0,  363, -6144,  0, 0, 0,  BPARAM2(0x01),  bhvCapSwitch),
    OBJECT( MODEL_NONE,        0,  500, -7373,  0, 0, 0,  0,  bhvWaterfallSoundLoop),
    OBJECT( MODEL_NONE,        0,  500,  3584,  0, 0, 0,  0,  bhvWaterfallSoundLoop),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT( MODEL_NONE,        0, -200, -7000,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_1),  bhvHiddenRedCoinStar),
    RETURN(),
};

const LevelScript level_cotmc_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _cotmc_segment_7SegmentRomStart, _cotmc_segment_7SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _cave_mio0SegmentRomStart, _cave_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group8_mio0SegmentRomStart, _group8_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group8_geoSegmentRomStart,  _group8_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x06, _group17_mio0SegmentRomStart, _group17_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0D, _group17_geoSegmentRomStart,  _group17_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_9),
    JUMP_LINK(script_func_global_18),
    JUMP_LINK(script_func_global_1),

    AREA( 1, cotmc_geo_0001A0),
        OBJECT( MODEL_NONE,  -4185, 1020, -47,  0, 90, 0,  BPARAM2(WARP_NODE_0A),  bhvAirborneWarp),
        WARP_NODE( WARP_NODE_0A,          LEVEL_COTMC,           1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,     LEVEL_CASTLE,          3,  WARP_NODE_34,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,       LEVEL_CASTLE,          3,  WARP_NODE_66,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_WARP_FLOOR,  LEVEL_CASTLE_GROUNDS,  1,  WARP_NODE_14,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_1),
        TERRAIN( cotmc_seg7_collision_level),
        MACRO_OBJECTS( cotmc_seg7_macro_objs),
        SHOW_DIALOG( 0x00, DIALOG_130),
        SET_BACKGROUND_MUSIC( 0x0004,  SEQ_LEVEL_UNDERGROUND),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  90,  -4185, 20, -47),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
