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
#include "levels/bob/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_BOB_CHAIN_CHOMP_GATE,    1456,   768,   446,  0, 326, 0,   0,  bhvChainChompGate),
    OBJECT( MODEL_BOB_SEESAW_PLATFORM,    -2303,   717,  1024,  0, 45, 0,    BPARAM2(0x03),  bhvSeesawPlatform),
    OBJECT( MODEL_NONE,                   -2050,     0, -3069,  0, 25, 0,    BPARAM2(OPENABLE_GRILL_BP_BOB),  bhvOpenableGrill),
    OBJECT( MODEL_PURPLE_SWITCH,          -2283,     0, -3682,  0, 27, 0,    0,  bhvFloorSwitchGrills),
    OBJECT( MODEL_CHECKERBOARD_PLATFORM,   1612,   300,  4611,  0, 0, 0,     BPARAM1(0) | BPARAM2(40),  bhvCheckerboardElevatorGroup),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT_WITH_ACTS( MODEL_NONE,                    1535, 3840, -5561,  0, 0, 0,    BPARAM2(BBALL_BP_STYPE_BOB_UPPER),  bhvBoBBowlingBallSpawner,   ACT_1 | ACT_2),
    OBJECT_WITH_ACTS( MODEL_NONE,                    1535, 3840, -5561,  0, 0, 0,    BPARAM2(BBALL_BP_STYPE_BOB_UPPER),  bhvTTMBowlingBallSpawner,   ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_NONE,                     524, 2825, -5400,  0, 0, 0,    BPARAM2(BBALL_BP_STYPE_BOB_LOWER),  bhvBoBBowlingBallSpawner,   ACT_1 | ACT_2),
    OBJECT_WITH_ACTS( MODEL_NONE,                     524, 2825, -5400,  0, 0, 0,    BPARAM2(BBALL_BP_STYPE_BOB_LOWER),  bhvTTMBowlingBallSpawner,   ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT          ( MODEL_BOWLING_BALL,            -993,  886, -3565,  0, 0, 0,    0,  bhvPitBowlingBall),
    OBJECT          ( MODEL_BOWLING_BALL,            -785,  886, -4301,  0, 0, 0,    0,  bhvPitBowlingBall),
    OBJECT_WITH_ACTS( MODEL_BOWLING_BALL,             -93,  886, -3414,  0, 0, 0,    0,  bhvPitBowlingBall,          ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_BOBOMB_BUDDY,           -5723,  140,  6017,  0, 0, 0,    BPARAM2(DIALOG_002),  bhvBobombBuddy,             ACT_1),
    OBJECT_WITH_ACTS( MODEL_BOBOMB_BUDDY,           -6250,    0,  6680,  0, 0, 0,    BPARAM2(DIALOG_001),  bhvBobombBuddy,             ACT_1),
    OBJECT_WITH_ACTS( MODEL_BOBOMB_BUDDY,           -5723,  140,  6017,  0, 0, 0,    0,        bhvBobombBuddyOpensCannon,  ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_BOBOMB_BUDDY,           -6250,    0,  6680,  0, 0, 0,    BPARAM2(DIALOG_003),  bhvBobombBuddy,             ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_CANNON_BASE,            -5694,  128,  5600,  0, 135, 0,  BPARAM2(0x01),  bhvWaterBombCannon,         ACT_1),
    OBJECT_WITH_ACTS( MODEL_DL_CANNON_LID,          -5694,  128,  5600,  0, 180, 0,  BPARAM2(0x00),  bhvCannonClosed,            ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_NONE,                    3304, 4242, -4603,  0, 0, 0,    0,  bhvKoopaRaceEndpoint,       ACT_2),
    OBJECT_WITH_ACTS( MODEL_KOOPA_WITH_SHELL,        3400,  770,  6500,  0, 0, 0,    BPARAM2(KOOPA_BP_NORMAL),  bhvKoopa,                   ACT_3 | ACT_4 | ACT_5 | ACT_6),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT_WITH_ACTS( MODEL_KING_BOBOMB,             1636, 4242, -5567,  0, -147, 0,  BPARAM1(STAR_INDEX_ACT_1),  bhvKingBobomb,             ACT_1),
    OBJECT_WITH_ACTS( MODEL_KOOPA_WITH_SHELL,       -4004,    0,  5221,  0, 0, 0,     BPARAM1(STAR_INDEX_ACT_2) | BPARAM2(KOOPA_BP_KOOPA_THE_QUICK_BOB),  bhvKoopa,                  ACT_2),
    OBJECT_WITH_ACTS( MODEL_NONE,                   -6000, 1000,  2400,  0, 0, 0,     BPARAM1(STAR_INDEX_ACT_4),  bhvHiddenRedCoinStar,      ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_NONE,                   -6600, 1000,  1250,  0, 0, 0,     BPARAM1(STAR_INDEX_ACT_5) | BPARAM2(0x04),  bhvHiddenStar,             ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,                    1550, 1200,   300,  0, 0, 0,     BPARAM1(STAR_INDEX_ACT_6),  bhvStar,                   ALL_ACTS),
    RETURN(),
};

const LevelScript level_bob_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _bob_segment_7SegmentRomStart, _bob_segment_7SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _generic_mio0SegmentRomStart, _generic_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _water_skybox_mio0SegmentRomStart, _water_skybox_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group3_mio0SegmentRomStart, _group3_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group3_geoSegmentRomStart,  _group3_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x06, _group14_mio0SegmentRomStart, _group14_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0D, _group14_geoSegmentRomStart,  _group14_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_4),
    JUMP_LINK(script_func_global_15),
    LOAD_MODEL_FROM_GEO(MODEL_BOB_BUBBLY_TREE,      bubbly_tree_geo),
    LOAD_MODEL_FROM_GEO(MODEL_BOB_CHAIN_CHOMP_GATE, bob_geo_000440),
    LOAD_MODEL_FROM_GEO(MODEL_BOB_SEESAW_PLATFORM,  bob_geo_000458),
    LOAD_MODEL_FROM_GEO(MODEL_BOB_BARS_GRILLS,      bob_geo_000470),

    AREA( 1, bob_geo_000488),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        OBJECT( MODEL_NONE,  -6558,  1000,  6464,  0, 135, 0,   BPARAM2(WARP_NODE_0A),  bhvSpinAirborneWarp),
        OBJECT( MODEL_NONE,    583,  2683, -5387,  0, -154, 0,  BPARAM2(WARP_NODE_0B),  bhvFadingWarp),
        OBJECT( MODEL_NONE,   1680,  3835, -5523,  0, -153, 0,  BPARAM2(WARP_NODE_0C),  bhvFadingWarp),
        OBJECT( MODEL_NONE,  -6612,  1024, -3351,  0, 107, 0,   BPARAM2(WARP_NODE_0D),  bhvFadingWarp),
        OBJECT( MODEL_NONE,   1980,   768,  6618,  0, -151, 0,  BPARAM2(WARP_NODE_0E),  bhvFadingWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_BOB,     1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0B,       LEVEL_BOB,     1,  WARP_NODE_0C,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0C,       LEVEL_BOB,     1,  WARP_NODE_0B,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0D,       LEVEL_BOB,     1,  WARP_NODE_0E,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0E,       LEVEL_BOB,     1,  WARP_NODE_0D,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  1,  WARP_NODE_32,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  1,  WARP_NODE_64,  WARP_NO_CHECKPOINT),
        TERRAIN( bob_seg7_collision_level),
        MACRO_OBJECTS( bob_seg7_macro_objs),
        SHOW_DIALOG( 0x00, DIALOG_000),
        SET_BACKGROUND_MUSIC( 0x0000,  SEQ_LEVEL_GRASS),
        TERRAIN_TYPE( TERRAIN_GRASS),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  135,  -6558, 0, 6464),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
