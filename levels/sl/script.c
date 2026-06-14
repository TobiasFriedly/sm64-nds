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
#include "levels/sl/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT_WITH_ACTS( MODEL_STAR,   700, 4500,  690,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_1),  bhvStar,                  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,  4350, 1350, 4350,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_3),  bhvStar,                  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_NONE,  5000, 1200,    0,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_5),  bhvHiddenRedCoinStar,  ALL_ACTS),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT( MODEL_NONE,   977, 1024, 2075,  0, 0, 0,  0,  bhvSnowMoundSpawn),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT( MODEL_PENGUIN,             1715, 3328,   518,  0, -51, 0,  0,  bhvSLWalkingPenguin),
    OBJECT( MODEL_NONE,                 700, 3428,   700,  0,  30, 0,  0,  bhvSLSnowmanWind),
    OBJECT( MODEL_NONE,                 480, 2300,  1370,  0,   0, 0,  0,  bhvIgloo),
    OBJECT( MODEL_BIG_CHILL_BULLY,      315, 1331, -4852,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_2),  bhvBigChillBully),
    OBJECT( MODEL_MR_BLIZZARD_HIDDEN,  2954,  970,   750,  0,   0, 0,  BPARAM2(MR_BLIZZARD_STYPE_CAP),  bhvMrBlizzard),
    RETURN(),
};

static const LevelScript script_func_local_4[] = {
    OBJECT_WITH_ACTS( MODEL_STAR,  0, 500, 1000,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_6),  bhvStar,  ALL_ACTS),
    RETURN(),
};

const LevelScript level_sl_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _sl_segment_7SegmentRomStart, _sl_segment_7SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _snow_mio0SegmentRomStart, _snow_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x0B, _effect_mio0SegmentRomStart, _effect_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _ccm_skybox_mio0SegmentRomStart, _ccm_skybox_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group7_mio0SegmentRomStart, _group7_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group7_geoSegmentRomStart,  _group7_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x06, _group16_mio0SegmentRomStart, _group16_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0D, _group16_geoSegmentRomStart,  _group16_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_8),
    JUMP_LINK(script_func_global_17),
    LOAD_MODEL_FROM_GEO(MODEL_SL_SNOW_TRIANGLE,     sl_geo_000390),
    LOAD_MODEL_FROM_GEO(MODEL_SL_CRACKED_ICE,       sl_geo_000360),
    LOAD_MODEL_FROM_GEO(MODEL_SL_CRACKED_ICE_CHUNK, sl_geo_000378),
    LOAD_MODEL_FROM_GEO(MODEL_SL_SNOW_TREE,         snow_tree_geo),

    AREA( 1, sl_geo_0003A8),
        OBJECT( MODEL_NONE,   5541, 2024,   443,  0, 270, 0,  BPARAM2(WARP_NODE_0A),  bhvSpinAirborneWarp),
        OBJECT( MODEL_NONE,    257, 2150,  1399,  0, 290, 0,  BPARAM2(WARP_NODE_0B),  bhvInstantActiveWarp),
        OBJECT( MODEL_NONE,    569, 2150,  1336,  0,   0, 0,  BPARAM1(6) | BPARAM2(WARP_NODE_0C),  bhvWarp),
        OBJECT( MODEL_NONE,   5468, 1056, -5400,  0, -20, 0,  BPARAM2(WARP_NODE_0D),  bhvFadingWarp),
        OBJECT( MODEL_NONE,  -3698, 1024, -1237,  0,   6, 0,  BPARAM2(WARP_NODE_0E),  bhvFadingWarp),
        WARP_NODE( WARP_NODE_0A,  LEVEL_SL,  1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0B,  LEVEL_SL,  1,  WARP_NODE_0B,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0C,  LEVEL_SL,  2,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0D,  LEVEL_SL,  1,  WARP_NODE_0E,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0E,  LEVEL_SL,  1,  WARP_NODE_0D,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        WARP_NODE( WARP_NODE_SUCCESS,     LEVEL_CASTLE,  2,  WARP_NODE_36,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,  LEVEL_CASTLE,  2,  WARP_NODE_68,  WARP_NO_CHECKPOINT),
        TERRAIN( sl_seg7_area_1_collision),
        MACRO_OBJECTS( sl_seg7_area_1_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0000,  SEQ_LEVEL_SNOW),
        TERRAIN_TYPE( TERRAIN_SNOW),
    END_AREA(),

    AREA( 2, sl_geo_000484),
        OBJECT( MODEL_NONE,  0, 0, 2867,  0, 180, 0,  BPARAM2(WARP_NODE_0A),  bhvInstantActiveWarp),
        OBJECT( MODEL_NONE,  0, 0, 3277,  0,   0, 0,  BPARAM1(20) | BPARAM2(WARP_NODE_0B),  bhvWarp),
        WARP_NODE( WARP_NODE_0A,  LEVEL_SL,  2,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0B,  LEVEL_SL,  1,  WARP_NODE_0B,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_4),
        WARP_NODE( WARP_NODE_SUCCESS,     LEVEL_CASTLE,  2,  WARP_NODE_36,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,  LEVEL_CASTLE,  2,  WARP_NODE_68,  WARP_NO_CHECKPOINT),
        TERRAIN( sl_seg7_area_2_collision),
        MACRO_OBJECTS( sl_seg7_area_2_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0004,  SEQ_LEVEL_UNDERGROUND),
        TERRAIN_TYPE( TERRAIN_SNOW),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  270,  5541, 1024, 443),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
