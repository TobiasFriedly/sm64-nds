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
#include "levels/wf/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_LEVEL_GEOMETRY_03,   2305, 2432,  -255,  0, 0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_04,   3405, 1664, -1791,  0, 0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_05,   3840,    0, -2303,  0, 0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_05,   3840,    0, -1279,  0, 0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_06,      0,    0,     0,  0, 0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_07,   1757, 3519, -3151,  0, 0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_0A,   3840,  794,  2688,  0, 0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_LEVEL_GEOMETRY_0C,   1408, 2522,  2431,  0, 0, 0,  0,  bhvStaticObject),
    OBJECT( MODEL_WF_GIANT_POLE,      -2560, 2560,  -256,  0, 0, 0,  0,  bhvGiantPole),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT          ( MODEL_WF_SMALL_BOMP,                 3300, 1070,     1,  0,  90, 0,  0,  bhvSmallBomp),
    OBJECT          ( MODEL_WF_SMALL_BOMP,                 3300, 1070,  1281,  0,  90, 0,  0,  bhvSmallBomp),
    OBJECT          ( MODEL_WF_LARGE_BOMP,                 3300, 1070,   641,  0,   0, 0,  0,  bhvLargeBomp),
    OBJECT          ( MODEL_WF_ROTATING_WOODEN_PLATFORM,   -255, 2560,  2304,  0,   0, 0,  0,  bhvWFRotatingWoodenPlatform),
    OBJECT          ( MODEL_WF_SLIDING_PLATFORM,           3328, 1075, -1791,  0,  90, 0,  BPARAM2(WF_SLID_BRICK_PTFM_BP_MOV_VEL_15),  bhvWFSlidingPlatform),
    OBJECT          ( MODEL_WF_SLIDING_PLATFORM,           3328, 1075,  -767,  0,  90, 0,  BPARAM2(WF_SLID_BRICK_PTFM_BP_MOV_VEL_10),  bhvWFSlidingPlatform),
    OBJECT          ( MODEL_WF_SLIDING_PLATFORM,           3328, 1075, -2815,  0,  90, 0,  BPARAM2(WF_SLID_BRICK_PTFM_BP_MOV_VEL_20),  bhvWFSlidingPlatform),
    OBJECT          ( MODEL_WF_TUMBLING_BRIDGE,            1792, 2496,  1600,  0,   0, 0,  BPARAM2(TUMBLING_BRIDGE_BP_WF),  bhvTumblingBridge),
    OBJECT          ( MODEL_WF_BREAKABLE_WALL_RIGHT,        512, 2176,  2944,  0,   0, 0,  0,  bhvWFBreakableWallRight),
    OBJECT          ( MODEL_WF_BREAKABLE_WALL_LEFT,       -1023, 2176,  2944,  0,   0, 0,  0,  bhvWFBreakableWallLeft),
    OBJECT_WITH_ACTS( MODEL_WF_KICKABLE_BOARD,               13, 3584, -1407,  0, 315, 0,  0,  bhvKickableBoard,  ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_1UP,                           -384, 3584,     6,  0,   0, 0,  BPARAM2(ONE_UP_BP_GENERIC),  bhv1Up,             ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT          ( MODEL_WF_ROTATING_PLATFORM,          2304, 3584, -2303,  0,   0, 0,  BPARAM1(0x08) | BPARAM2(ROTATING_PLATFORM_BP_WF),  bhvRotatingPlatform),
    OBJECT          ( MODEL_WF_ROTATING_PLATFORM,          3200, 3328, -1791,  0,   0, 0,  BPARAM1(0x08) | BPARAM2(ROTATING_PLATFORM_BP_WF),  bhvRotatingPlatform),
    OBJECT          ( MODEL_WF_ROTATING_PLATFORM,          2688, 3584,  -895,  0,   0, 0,  BPARAM1(0x08) | BPARAM2(ROTATING_PLATFORM_BP_WF),  bhvRotatingPlatform),
    OBJECT          ( MODEL_NONE,                         -2495, 1331,  -256,  0,   0, 0,  BPARAM2(61),  bhvPoleGrabbing),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT          ( MODEL_THWOMP,              3462, 1939, -1545,  0,  180, 0,  BPARAM2(0),  bhvThwomp2),
    OBJECT          ( MODEL_THWOMP,              3462, 1075, -3314,  0,   90, 0,  BPARAM2(0),  bhvThwomp),
    OBJECT          ( MODEL_NONE,                -856,  922,  3819,  0,    0, 0,  0,  bhvBetaFishSplashSpawner),
    OBJECT          ( MODEL_PIRANHA_PLANT,       1822, 2560,  -101,  0,   90, 0,  0,  bhvPiranhaPlant),
    OBJECT          ( MODEL_PIRANHA_PLANT,       4625,  256,  5017,  0,  -90, 0,  0,  bhvPiranhaPlant),
    OBJECT          ( MODEL_PIRANHA_PLANT,        689, 2560,  1845,  0,   90, 0,  0,  bhvPiranhaPlant),
    OBJECT          ( MODEL_WHOMP,              -1545, 2560,  -286,  0,    0, 0,  BPARAM2(WHOMP_BP_SMALL),  bhvSmallWhomp),
    OBJECT          ( MODEL_WHOMP,                189, 2560, -1857,  0, -135, 0,  BPARAM2(WHOMP_BP_SMALL),  bhvSmallWhomp),
    OBJECT          ( MODEL_BUTTERFLY,           4736,  256,  4992,  0,    0, 0,  0,  bhvButterfly),
    OBJECT          ( MODEL_BUTTERFLY,           4608,  256,  5120,  0,    0, 0,  0,  bhvButterfly),
    OBJECT          ( MODEL_BUTTERFLY,           4608,  256,  4992,  0,    0, 0,  0,  bhvButterfly),
    OBJECT          ( MODEL_BUTTERFLY,           4608,  256,  4864,  0,    0, 0,  0,  bhvButterfly),
    OBJECT          ( MODEL_BUTTERFLY,           4480,  256,  4992,  0,    0, 0,  0,  bhvButterfly),
    OBJECT          ( MODEL_BUTTERFLY,           4608,  256,   256,  0,    0, 0,  0,  bhvButterfly),
    OBJECT          ( MODEL_BUTTERFLY,           4736,  256,   128,  0,    0, 0,  0,  bhvButterfly),
    OBJECT          ( MODEL_BUTTERFLY,           4480,  256,   128,  0,    0, 0,  0,  bhvButterfly),
    OBJECT          ( MODEL_BUTTERFLY,           4608,  256,     0,  0,    0, 0,  0,  bhvButterfly),
    OBJECT          ( MODEL_BUTTERFLY,           4608,  256,   128,  0,    0, 0,  0,  bhvButterfly),
    OBJECT          ( MODEL_NONE,                1035, 2880,  -900,  0,   45, 0,  BPARAM1(0) | BPARAM2(0),  bhvCheckerboardElevatorGroup),
    OBJECT_WITH_ACTS( MODEL_BULLET_BILL,         1280, 3712,   968,  0,  180, 0,  0,  bhvBulletBill,                ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_LEVEL_GEOMETRY_08,      0, 3584,     0,  0,    0, 0,  0,  bhvTower,                      ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_LEVEL_GEOMETRY_09,   1280, 3584,   896,  0,    0, 0,  0,  bhvBulletBillCannon,         ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_NONE,                   0, 3483,     0,  0,    0, 0,  0,  bhvTowerPlatformGroup,       ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_WF_TOWER_DOOR,       -511, 3584,     0,  0,    0, 0,  0,  bhvTowerDoor,                 ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_BOBOMB_BUDDY,       -1700, 1140,  3500,  0,    0, 0,  0,  bhvBobombBuddyOpensCannon,  ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_HOOT,                2560,  700,  4608,  0,    0, 0,  0,  bhvHoot,                       ACT_3 | ACT_4 | ACT_5 | ACT_6),
    RETURN(),
};

static const LevelScript script_func_local_4[] = {
    OBJECT_WITH_ACTS( MODEL_WHOMP,      0, 3584,    0,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_1),  bhvWhompKingBoss,       ACT_1),
    OBJECT_WITH_ACTS( MODEL_STAR,     300, 5550,    0,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_2),  bhvStar,                  ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6),
    OBJECT_WITH_ACTS( MODEL_STAR,   -2500, 1500, -750,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_3),  bhvStar,                  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_NONE,    4600,  550, 2500,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_4),  bhvHiddenRedCoinStar,  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,    2880, 4300,  190,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_5),  bhvStar,                  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,     590, 2450, 2650,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_6),  bhvStar,                  ALL_ACTS),
    RETURN(),
};

const LevelScript level_wf_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _wf_segment_7SegmentRomStart, _wf_segment_7SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _cloud_floor_skybox_mio0SegmentRomStart, _cloud_floor_skybox_mio0SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _grass_mio0SegmentRomStart, _grass_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group1_mio0SegmentRomStart, _group1_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group1_geoSegmentRomStart,  _group1_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x06, _group14_mio0SegmentRomStart, _group14_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0D, _group14_geoSegmentRomStart,  _group14_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart,  _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_1),
    JUMP_LINK(script_func_global_2),
    JUMP_LINK(script_func_global_15),
    LOAD_MODEL_FROM_GEO(MODEL_WF_BUBBLY_TREE,                   bubbly_tree_geo),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03,                wf_geo_0007E0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_04,                wf_geo_000820),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_05,                wf_geo_000860),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_06,                wf_geo_000878),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_07,                wf_geo_000890),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_08,                wf_geo_0008A8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_09,                wf_geo_0008E8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0A,                wf_geo_000900),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0C,                wf_geo_000940),
    LOAD_MODEL_FROM_GEO(MODEL_WF_GIANT_POLE,                    wf_geo_000AE0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0E,                wf_geo_000958),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0F,                wf_geo_0009A0),
    LOAD_MODEL_FROM_GEO(MODEL_WF_ROTATING_PLATFORM,             wf_geo_0009B8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_11,                wf_geo_0009D0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_12,                wf_geo_0009E8),
    LOAD_MODEL_FROM_GEO(MODEL_WF_SMALL_BOMP,                    wf_geo_000A00),
    LOAD_MODEL_FROM_GEO(MODEL_WF_LARGE_BOMP,                    wf_geo_000A40),
    LOAD_MODEL_FROM_GEO(MODEL_WF_ROTATING_WOODEN_PLATFORM,      wf_geo_000A58),
    LOAD_MODEL_FROM_GEO(MODEL_WF_SLIDING_PLATFORM,              wf_geo_000A98),
    LOAD_MODEL_FROM_GEO(MODEL_WF_TUMBLING_BRIDGE_PART,          wf_geo_000AB0),
    LOAD_MODEL_FROM_GEO(MODEL_WF_TUMBLING_BRIDGE,               wf_geo_000AC8),
    LOAD_MODEL_FROM_GEO(MODEL_WF_TOWER_TRAPEZOID_PLATORM,       wf_geo_000AF8),
    LOAD_MODEL_FROM_GEO(MODEL_WF_TOWER_SQUARE_PLATORM,          wf_geo_000B10),
    LOAD_MODEL_FROM_GEO(MODEL_WF_TOWER_SQUARE_PLATORM_UNUSED,   wf_geo_000B38),
    LOAD_MODEL_FROM_GEO(MODEL_WF_TOWER_SQUARE_PLATORM_ELEVATOR, wf_geo_000B60),
    LOAD_MODEL_FROM_GEO(MODEL_WF_BREAKABLE_WALL_RIGHT,          wf_geo_000B78),
    LOAD_MODEL_FROM_GEO(MODEL_WF_BREAKABLE_WALL_LEFT,           wf_geo_000B90),
    LOAD_MODEL_FROM_GEO(MODEL_WF_KICKABLE_BOARD,                wf_geo_000BA8),
    LOAD_MODEL_FROM_GEO(MODEL_WF_TOWER_DOOR,                    wf_geo_000BE0),
    LOAD_MODEL_FROM_GEO(MODEL_WF_KICKABLE_BOARD_FELLED,         wf_geo_000BC8),

    AREA( 1, wf_geo_000BF8),
        OBJECT( MODEL_NONE,   2600, 1256,  5120,  0, 90, 0,  BPARAM2(WARP_NODE_0A),  bhvSpinAirborneWarp),
        OBJECT( MODEL_NONE,  -2925, 2560,  -947,  0, 19, 0,  BPARAM2(WARP_NODE_0B),  bhvFadingWarp),
        OBJECT( MODEL_NONE,   2548, 1075, -3962,  0, 51, 0,  BPARAM2(WARP_NODE_0C),  bhvFadingWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_WF,      1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0B,       LEVEL_WF,      1,  WARP_NODE_0C,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0C,       LEVEL_WF,      1,  WARP_NODE_0B,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  1,  WARP_NODE_34,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  1,  WARP_NODE_66,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        JUMP_LINK(script_func_local_4),
        TERRAIN( wf_seg7_collision_070102D8),
        MACRO_OBJECTS( wf_seg7_macro_objs),
        SHOW_DIALOG( 0x00, DIALOG_030),
        SET_BACKGROUND_MUSIC( 0x0005,  SEQ_LEVEL_GRASS),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  90,  2600, 256, 5120),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
