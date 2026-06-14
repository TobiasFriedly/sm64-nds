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
#include "levels/ttc/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_NONE,    -1080,  -840,  1573,  0,   0, 0,  BPARAM2(86),  bhvPoleGrabbing),
    OBJECT( MODEL_THWOMP,   1919,  6191,  1919,  0, 225, 0,  BPARAM2(0),   bhvThwomp),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT_WITH_ACTS( MODEL_STAR,    -1450, -1130, -1050,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_1),  bhvStar,                  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,    -1850,   300,  -950,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_2),  bhvStar,                  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,    -1300, -2250, -1300,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_3),  bhvStar,                  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,     2200,  7300,  2210,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_4),  bhvStar,                  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,    -1050,  2400,  -790,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_5),  bhvStar,                  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_NONE,     1815, -3200,   800,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_6),  bhvHiddenRedCoinStar,  ALL_ACTS),
    RETURN(),
};

const LevelScript level_ttc_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _ttc_segment_7SegmentRomStart, _ttc_segment_7SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _machine_mio0SegmentRomStart, _machine_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group1_mio0SegmentRomStart, _group1_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group1_geoSegmentRomStart,  _group1_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_2),
    LOAD_MODEL_FROM_GEO(MODEL_TTC_ROTATING_CUBE,     ttc_geo_000240),
    LOAD_MODEL_FROM_GEO(MODEL_TTC_ROTATING_PRISM,    ttc_geo_000258),
    LOAD_MODEL_FROM_GEO(MODEL_TTC_PENDULUM,          ttc_geo_000270),
    LOAD_MODEL_FROM_GEO(MODEL_TTC_LARGE_TREADMILL,   ttc_geo_000288),
    LOAD_MODEL_FROM_GEO(MODEL_TTC_SMALL_TREADMILL,   ttc_geo_0002A8),
    LOAD_MODEL_FROM_GEO(MODEL_TTC_PUSH_BLOCK,        ttc_geo_0002C8),
    LOAD_MODEL_FROM_GEO(MODEL_TTC_ROTATING_HEXAGON,  ttc_geo_0002E0),
    LOAD_MODEL_FROM_GEO(MODEL_TTC_ROTATING_TRIANGLE, ttc_geo_0002F8),
    LOAD_MODEL_FROM_GEO(MODEL_TTC_PIT_BLOCK,         ttc_geo_000310),
    LOAD_MODEL_FROM_GEO(MODEL_TTC_PIT_BLOCK_UNUSED,  ttc_geo_000328),
    LOAD_MODEL_FROM_GEO(MODEL_TTC_ELEVATOR_PLATFORM, ttc_geo_000340),
    LOAD_MODEL_FROM_GEO(MODEL_TTC_CLOCK_HAND,        ttc_geo_000358),
    LOAD_MODEL_FROM_GEO(MODEL_TTC_SPINNER,           ttc_geo_000370),
    LOAD_MODEL_FROM_GEO(MODEL_TTC_SMALL_GEAR,        ttc_geo_000388),
    LOAD_MODEL_FROM_GEO(MODEL_TTC_LARGE_GEAR,        ttc_geo_0003A0),

    AREA( 1, ttc_geo_0003B8),
        OBJECT( MODEL_NONE,  1417, -3822, -548,  0, 316, 0,  BPARAM2(WARP_NODE_0A),  bhvSpinAirborneWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_TTC,     1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  2,  WARP_NODE_35,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  2,  WARP_NODE_67,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        TERRAIN( ttc_seg7_collision_level),
        MACRO_OBJECTS( ttc_seg7_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0001,  SEQ_LEVEL_SLIDE),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  316,  1417, -4822, -548),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
