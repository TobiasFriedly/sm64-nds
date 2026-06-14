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
#include "levels/rr/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_RR_SLIDING_PLATFORM,          -3400, -2038,  6564,   0,   0,   0,  BPARAM1(0x02) | BPARAM2(0x56),  bhvSlidingPlatform2),
    OBJECT( MODEL_RR_SLIDING_PLATFORM,          -2684, -1423,   -36,   0,   0,   0,  BPARAM1(0x02) | BPARAM2(0x59),  bhvSlidingPlatform2),
    OBJECT( MODEL_RR_FLYING_CARPET,              4571, -1782,  2036,   0, 180,   0,  0,  bhvPlatformOnTrack),
    OBJECT( MODEL_RR_FLYING_CARPET,               580,  -963, -3659,   0, 180,   0,  BPARAM2(0x01),  bhvPlatformOnTrack),
    OBJECT( MODEL_RR_FLYING_CARPET,              1567,   880,  -184,   0, 180,   0,  BPARAM2(0x07),  bhvPlatformOnTrack),
    OBJECT( MODEL_RR_FLYING_CARPET,               646,   880,  -184,   0,   0,   0,  BPARAM2(0x08),  bhvPlatformOnTrack),
    OBJECT( MODEL_RR_OCTAGONAL_PLATFORM,          644, -1321, -1301,   0, 180,   0,  BPARAM1(0x03) | BPARAM2(0x01),  bhvOctagonalPlatformRotating),
    OBJECT( MODEL_RR_OCTAGONAL_PLATFORM,         1797, -1321,   -56,   0,   0,   0,  BPARAM2(0x01),  bhvOctagonalPlatformRotating),
    OBJECT( MODEL_RR_OCTAGONAL_PLATFORM,          663, -1321,  1179,   0, 180,   0,  BPARAM1(0x03) | BPARAM2(0x01),  bhvOctagonalPlatformRotating),
    OBJECT( MODEL_RR_OCTAGONAL_PLATFORM,         -502, -1321,   -51,   0,   0,   0,  BPARAM2(0x01),  bhvOctagonalPlatformRotating),
    OBJECT( MODEL_RR_ROTATING_BRIDGE_PLATFORM,   5043,  1342,   300,   0,   0,   0,  BPARAM2(0x01),  bhvRRRotatingBridgePlatform),
    OBJECT( MODEL_RR_CRUISER_WING,               3473,  2422, -1821,   0,   0,   0,  0,  bhvRRCruiserWing),
    OBJECT( MODEL_RR_CRUISER_WING,               4084,  2431, -2883,  45, 180, 180,  BPARAM2(0x01),  bhvRRCruiserWing),
    OBJECT( MODEL_RR_CRUISER_WING,               3470,  2420, -2869,  45, 180, 180,  BPARAM2(0x01),  bhvRRCruiserWing),
    OBJECT( MODEL_RR_CRUISER_WING,               2856,  2410, -2855,  45, 180, 180,  BPARAM2(0x01),  bhvRRCruiserWing),
    OBJECT( MODEL_RR_CRUISER_WING,               4101,  2435, -1813,   0,   0,   0,  0,  bhvRRCruiserWing),
    OBJECT( MODEL_RR_CRUISER_WING,               2859,  2411, -1834,   0,   0,   0,  0,  bhvRRCruiserWing),
    OBJECT( MODEL_RR_SEESAW_PLATFORM,           -6013, -2857,  6564,   0, 270,   0,  BPARAM2(0x05),  bhvSeesawPlatform),
    OBJECT( MODEL_RR_SEESAW_PLATFORM,             614, -3574,  6564,   0, 270,   0,  BPARAM2(0x05),  bhvSeesawPlatform),
    OBJECT( MODEL_RR_SWINGING_PLATFORM,         -3557,  -809,  4619,   0,   0,   0,  0,  bhvSwingPlatform),
    OBJECT( MODEL_RR_SWINGING_PLATFORM,         -2213, -2582,  6257,   0,   0,   0,  0,  bhvSwingPlatform),
    OBJECT( MODEL_NONE,                             0,     0,     0,   0,   0,   0,  0,  bhvDonutPlatformSpawner),
    OBJECT( MODEL_RR_ELEVATOR_PLATFORM,         -2684,  1546,   -36,   0,   0,   0,  BPARAM2(0x05),  bhvRRElevatorPlatform),
    OBJECT( MODEL_RR_TRICKY_TRIANGLES,           5862, -1347,  6564,   0,   0,   0,  BPARAM2(0x02),  bhvAnimatesOnFloorSwitchPress),
    OBJECT( MODEL_PURPLE_SWITCH,                 4428, -1936,  6564,   0,   0,   0,  0,  bhvFloorSwitchAnimatesObject),
    OBJECT( MODEL_NONE,                           614, -2857,  3671,   0,   0,   0,  BPARAM2(204),   bhvPoleGrabbing),
    OBJECT( MODEL_NONE,                           621, -4598,  7362,   0,   0,   0,  BPARAM2(117),   bhvPoleGrabbing),
    OBJECT( MODEL_NONE,                          5119,  3819,  3325,   0,   0,   0,  BPARAM2(97),    bhvPoleGrabbing),
    OBJECT( MODEL_NONE,                          3554,  2891, -2327,   0,   0,   0,  BPARAM2(193),   bhvPoleGrabbing),
    OBJECT( MODEL_NONE,                          2680,   214,   295,   0,   0,   0,  BPARAM2(98),    bhvPoleGrabbing),
    OBJECT( MODEL_NONE,                          3811,  1033,   295,   0,   0,   0,  BPARAM2(98),    bhvPoleGrabbing),
    RETURN(),
};

static const LevelScript script_func_local_2[] = {
    OBJECT( MODEL_NONE,  -5809, -1834,  5719,  0, 0, 0,  0,  bhvFlamethrower),
    OBJECT( MODEL_NONE,  -4838, -1015,  4081,  0, 0, 0,  0,  bhvFlamethrower),
    OBJECT( MODEL_NONE,   3301, -1834,  5617,  0, 0, 0,  0,  bhvFlamethrower),
    OBJECT( MODEL_NONE,   6772,  -757,  -606,  0, 0, 0,  0,  bhvFlamethrower),
    OBJECT( MODEL_NONE,  -4187,  3213, -6630,  0, 0, 0,  0,  bhvFlamethrower),
    RETURN(),
};

static const LevelScript script_func_local_3[] = {
    OBJECT_WITH_ACTS( MODEL_STAR,   1450,  3400, -2352,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_1),  bhvStar,                  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,  -4200,  6700, -4450,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_2),  bhvStar,                  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_NONE,  -5150, -1400,     0,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_3),  bhvHiddenRedCoinStar,  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,  -5850,  -700,  4950,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_4),  bhvStar,                  ALL_ACTS),
    OBJECT_WITH_ACTS( MODEL_STAR,   3700,  -400,  6600,  0, 0, 0,  BPARAM1(STAR_INDEX_ACT_5),  bhvStar,                  ALL_ACTS),
    RETURN(),
};

const LevelScript level_rr_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0        ( 0x07, _rr_segment_7SegmentRomStart, _rr_segment_7SegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x09, _sky_mio0SegmentRomStart, _sky_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x0A, _cloud_floor_skybox_mio0SegmentRomStart, _cloud_floor_skybox_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x05, _group11_mio0SegmentRomStart, _group11_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0C, _group11_geoSegmentRomStart,  _group11_geoSegmentRomEnd),
    LOAD_MIO0        ( 0x08, _common0_mio0SegmentRomStart, _common0_mio0SegmentRomEnd),
    LOAD_RAW         ( 0x0F, _common0_geoSegmentRomStart, _common0_geoSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_12),
    JUMP_LINK(script_func_global_1),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03,           rr_geo_000660),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_04,           rr_geo_000678),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_05,           rr_geo_000690),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_06,           rr_geo_0006A8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_07,           rr_geo_0006C0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_08,           rr_geo_0006D8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_09,           rr_geo_0006F0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0A,           rr_geo_000708),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0B,           rr_geo_000720),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0C,           rr_geo_000738),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0D,           rr_geo_000758),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0E,           rr_geo_000770),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_0F,           rr_geo_000788),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_10,           rr_geo_0007A0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_11,           rr_geo_0007B8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_12,           rr_geo_0007D0),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_13,           rr_geo_0007E8),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_14,           rr_geo_000800),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_15,           rr_geo_000818),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_16,           rr_geo_000830),
    LOAD_MODEL_FROM_GEO(MODEL_RR_SLIDING_PLATFORM,         rr_geo_0008C0),
    LOAD_MODEL_FROM_GEO(MODEL_RR_FLYING_CARPET,            rr_geo_000848),
    LOAD_MODEL_FROM_GEO(MODEL_RR_OCTAGONAL_PLATFORM,       rr_geo_0008A8),
    LOAD_MODEL_FROM_GEO(MODEL_RR_ROTATING_BRIDGE_PLATFORM, rr_geo_000878),
    LOAD_MODEL_FROM_GEO(MODEL_RR_TRIANGLE_PLATFORM,        rr_geo_0008D8),
    LOAD_MODEL_FROM_GEO(MODEL_RR_CRUISER_WING,             rr_geo_000890),
    LOAD_MODEL_FROM_GEO(MODEL_RR_SEESAW_PLATFORM,          rr_geo_000908),
    LOAD_MODEL_FROM_GEO(MODEL_RR_L_SHAPED_PLATFORM,        rr_geo_000940),
    LOAD_MODEL_FROM_GEO(MODEL_RR_SWINGING_PLATFORM,        rr_geo_000860),
    LOAD_MODEL_FROM_GEO(MODEL_RR_DONUT_PLATFORM,           rr_geo_000920),
    LOAD_MODEL_FROM_GEO(MODEL_RR_ELEVATOR_PLATFORM,        rr_geo_0008F0),
    LOAD_MODEL_FROM_GEO(MODEL_RR_TRICKY_TRIANGLES,         rr_geo_000958),
    LOAD_MODEL_FROM_GEO(MODEL_RR_TRICKY_TRIANGLES_FRAME1,  rr_geo_000970),
    LOAD_MODEL_FROM_GEO(MODEL_RR_TRICKY_TRIANGLES_FRAME2,  rr_geo_000988),
    LOAD_MODEL_FROM_GEO(MODEL_RR_TRICKY_TRIANGLES_FRAME3,  rr_geo_0009A0),
    LOAD_MODEL_FROM_GEO(MODEL_RR_TRICKY_TRIANGLES_FRAME4,  rr_geo_0009B8),

    AREA( 1, rr_geo_0009D0),
        OBJECT( MODEL_NONE,  2599, -833, 2071,  0, 90, 0,  BPARAM2(WARP_NODE_0A),  bhvSpinAirborneWarp),
        OBJECT( MODEL_NONE,  -7092, 2364, -63,  0, 90, 0,  BPARAM2(WARP_NODE_0B),  bhvFadingWarp),
        OBJECT( MODEL_NONE,  -4213, 3379, -2815,  0, 180, 0,  BPARAM2(WARP_NODE_0C),  bhvFadingWarp),
        WARP_NODE( WARP_NODE_0A,       LEVEL_RR,      1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0B,       LEVEL_RR,      1,  WARP_NODE_0C,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_0C,       LEVEL_RR,      1,  WARP_NODE_0B,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_SUCCESS,  LEVEL_CASTLE,  2,  WARP_NODE_3A,  WARP_NO_CHECKPOINT),
        WARP_NODE( WARP_NODE_DEATH,    LEVEL_CASTLE,  2,  WARP_NODE_6C,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        JUMP_LINK(script_func_local_2),
        JUMP_LINK(script_func_local_3),
        TERRAIN( rr_seg7_collision_level),
        MACRO_OBJECTS( rr_seg7_macro_objs),
        SET_BACKGROUND_MUSIC( 0x0000,  SEQ_LEVEL_SLIDE),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  90,  2599, -1833, 2071),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
