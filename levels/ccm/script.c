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
#include "levels/ccm/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_CCM_ROPEWAY_LIFT,  531, -4430, 6426,     0,   0, 0,  BPARAM1(0x07) | BPARAM2(0x12),  bhvPlatformOnTrack),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT          ( MODEL_PENGUIN,            2650, -3735,  3970,  0,   0, 0,  BPARAM2(0x01),  bhvSmallPenguin),
    OBJECT          ( MODEL_PENGUIN,            -555,  3470, -1000,  0,   0, 0,  BPARAM2(0x00),  bhvSmallPenguin),
    OBJECT          ( MODEL_MR_BLIZZARD,       -2376, -1589,  4256,  0, 252, 0,  BPARAM2(MR_BLIZZARD_STYPE_JUMPING),  bhvMrBlizzard),
    OBJECT          ( MODEL_MR_BLIZZARD,        -394, -1589,  4878,  0,  74, 0,  BPARAM2(MR_BLIZZARD_STYPE_JUMPING),  bhvMrBlizzard),
    OBJECT_WITH_ACTS( MODEL_CCM_SNOWMAN_BASE,   2560,  2662, -1122,  0,   0, 0,  0,  bhvSnowmansBottom,  ACT_5),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT_WITH_ACTS( MODEL_NONE,               2665, -4607,  4525,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_1),  bhvCCMTouchedStarSpawn,  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_PENGUIN,            3450, -4700,  4550,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_2),  bhvTuxiesMother,           ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_NONE,               4200,  -927,   400,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_4),  bhvHiddenRedCoinStar,    ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_CCM_SNOWMAN_HEAD,  -4230, -1169,  1813,  0, 270, 0,  BPARAM1(STAR_INDEX_ACT_5),  bhvSnowmansHead,           ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,              -2000, -2200, -3000,  0,   0, 0,  BPARAM1(STAR_INDEX_ACT_6),  bhvStar,                    ALL_ACTS),
    RETURN(),
};

static const LevelScript script_func_local_4[] = {
    OBJECT_WITH_ACTS( MODEL_PENGUIN,  -4952,  6656, -6075,  0, 270, 0,  BPARAM1(STAR_INDEX_ACT_3) | BPARAM2(RACING_PENGUIN_BP_THIN),  bhvRacingPenguin,    ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT          ( MODEL_NONE,     -6500, -5836, -6400,  0,   0, 0,  0,  bhvPenguinRaceFinishLine),
    OBJECT          ( MODEL_NONE,     -6393,  -716,  7503,  0,   0, 0,  0,  bhvPenguinRaceShortcutCheck),
#ifndef VERSION_JP
    OBJECT          ( MODEL_NONE,     -4943,  1321,   667,  0,   0, 0,  0,  bhvPlaysMusicTrackWhenTouched),
#endif
    RETURN(),
};

const LevelScript level_ccm_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _ccm_segment_7SegmentRomStart, _ccm_segment_7SegmentRomEnd),
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
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03, ccm_geo_00042C),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_04, ccm_geo_00045C),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_05, ccm_geo_000494),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_06, ccm_geo_0004BC),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_07, ccm_geo_0004E4),
    LOAD_MODEL_FROM_GEO(MODEL_CCM_CABIN_DOOR,    cabin_door_geo),
    LOAD_MODEL_FROM_GEO(MODEL_CCM_SNOW_TREE,     snow_tree_geo),
    LOAD_MODEL_FROM_GEO(MODEL_CCM_ROPEWAY_LIFT,  ccm_geo_0003D0),
    LOAD_MODEL_FROM_GEO(MODEL_CCM_SNOWMAN_BASE,  ccm_geo_0003F0),
    LOAD_MODEL_FROM_GEO(MODEL_CCM_SNOWMAN_HEAD,  ccm_geo_00040C),

    AREA( 1, ccm_geo_00051C),
        OBJECT( MODEL_NONE,  -1512,  3560, -2305,  0,  140, 0,  BPARAM2(WARP_NODE_0A),  bhvSpinAirborneWarp),
        OBJECT( MODEL_NONE,   -181,  2918, -1486,  0,    0, 0,  BPARAM1(15) | BPARAM2(WARP_NODE_1E),  bhvWarp),
        OBJECT( MODEL_NONE,  -1847,  2815,  -321,  0, -158, 0,  BPARAM2(WARP_NODE_1F),  bhvFadingWarp),
        OBJECT( MODEL_NONE,   3349, -4694,  -183,  0,  -34, 0,  BPARAM2(WARP_NODE_20),  bhvFadingWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_CCM,     1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_14,       LEVEL_CCM,     2,  WARP_NODE_14,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_1E,       LEVEL_CCM,     2,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_1F,       LEVEL_CCM,     1,  WARP_NODE_20,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_20,       LEVEL_CCM,     1,  WARP_NODE_1F,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  1,  WARP_NODE_33,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  1,  WARP_NODE_65,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        TERRAIN( ccm_seg7_area_1_collision),
        MACRO_OBJECTS( ccm_seg7_area_1_macro_objs),
        SHOW_DIALOG( 0x00, DIALOG_048),
        SET_BACKGROUND_MUSIC( 0x0000,  SEQ_LEVEL_SNOW),
        TERRAIN_TYPE( TERRAIN_SNOW),
    END_AREA(),

    AREA( 2, ccm_geo_0005E8),
        OBJECT( MODEL_NONE,  -5836, 7465, -6143,  0, 90, 0,  BPARAM2(WARP_NODE_0A),  bhvAirborneWarp),
        WARP_NODE( WARP_NODE_14,       LEVEL_CCM,     1,  WARP_NODE_14,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0A,       LEVEL_CCM,     2,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  1,  WARP_NODE_33,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  1,  WARP_NODE_65,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_4),
        TERRAIN( ccm_seg7_area_2_collision),
        MACRO_OBJECTS( ccm_seg7_area_2_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0001,  SEQ_LEVEL_SLIDE),
        TERRAIN_TYPE( TERRAIN_SLIDE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  140,  -1512, 2560, -2305),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
