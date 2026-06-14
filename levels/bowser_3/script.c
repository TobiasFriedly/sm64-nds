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
#include "levels/bowser_3/header.h"

static const LevelScript script_func_local_1[] = {
    OBJECT( MODEL_BOWSER_3_FALLING_PLATFORM_1,       0,   0,     0,  0, 0, 0,  BPARAM2(1),  bhvFallingBowserPlatform),
    OBJECT( MODEL_BOWSER_3_FALLING_PLATFORM_2,       0,   0,     0,  0, 0, 0,  BPARAM2(2),  bhvFallingBowserPlatform),
    OBJECT( MODEL_BOWSER_3_FALLING_PLATFORM_3,       0,   0,     0,  0, 0, 0,  BPARAM2(3),  bhvFallingBowserPlatform),
    OBJECT( MODEL_BOWSER_3_FALLING_PLATFORM_4,       0,   0,     0,  0, 0, 0,  BPARAM2(4),  bhvFallingBowserPlatform),
    OBJECT( MODEL_BOWSER_3_FALLING_PLATFORM_5,       0,   0,     0,  0, 0, 0,  BPARAM2(5),  bhvFallingBowserPlatform),
    OBJECT( MODEL_BOWSER_3_FALLING_PLATFORM_6,       0,   0,     0,  0, 0, 0,  BPARAM2(6),  bhvFallingBowserPlatform),
    OBJECT( MODEL_BOWSER_3_FALLING_PLATFORM_7,       0,   0,     0,  0, 0, 0,  BPARAM2(7),  bhvFallingBowserPlatform),
    OBJECT( MODEL_BOWSER_3_FALLING_PLATFORM_8,       0,   0,     0,  0, 0, 0,  BPARAM2(8),  bhvFallingBowserPlatform),
    OBJECT( MODEL_BOWSER_3_FALLING_PLATFORM_9,       0,   0,     0,  0, 0, 0,  BPARAM2(9),  bhvFallingBowserPlatform),
    OBJECT( MODEL_BOWSER_3_FALLING_PLATFORM_10,      0,   0,     0,  0, 0, 0,  BPARAM2(10),  bhvFallingBowserPlatform),
    OBJECT( MODEL_BOWSER_BOMB,                   -2122, 512, -2912,  0, 0, 0,  0,  bhvBowserBomb),
    OBJECT( MODEL_BOWSER_BOMB,                   -3362, 512,  1121,  0, 0, 0,  0,  bhvBowserBomb),
    OBJECT( MODEL_BOWSER_BOMB,                       0, 512,  3584,  0, 0, 0,  0,  bhvBowserBomb),
    OBJECT( MODEL_BOWSER_BOMB,                    3363, 512,  1121,  0, 0, 0,  0,  bhvBowserBomb),
    OBJECT( MODEL_BOWSER_BOMB,                    2123, 512, -2912,  0, 0, 0,  0,  bhvBowserBomb),
    RETURN(),
};

const LevelScript level_bowser_3_entry[] = {
    INIT_LEVEL(),
    LOAD_MIO0( 0x07, _bowser_3_segment_7SegmentRomStart, _bowser_3_segment_7SegmentRomEnd),
    LOAD_MIO0( 0x06, _group12_mio0SegmentRomStart, _group12_mio0SegmentRomEnd),
    LOAD_RAW ( 0x0D, _group12_geoSegmentRomStart,  _group12_geoSegmentRomEnd),
    LOAD_MIO0( 0x0A, _bits_skybox_mio0SegmentRomStart, _bits_skybox_mio0SegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    MARIO( MODEL_MARIO,  BPARAM4(0x01),  bhvMario),
    JUMP_LINK(script_func_global_13),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_1,  bowser_3_geo_000290),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_2,  bowser_3_geo_0002A8),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_3,  bowser_3_geo_0002C0),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_4,  bowser_3_geo_0002D8),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_5,  bowser_3_geo_0002F0),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_6,  bowser_3_geo_000308),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_7,  bowser_3_geo_000320),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_8,  bowser_3_geo_000338),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_9,  bowser_3_geo_000350),
    LOAD_MODEL_FROM_GEO(MODEL_BOWSER_3_FALLING_PLATFORM_10, bowser_3_geo_000368),
    LOAD_MODEL_FROM_GEO(MODEL_LEVEL_GEOMETRY_03,            bowser_3_geo_000380),

    AREA( 1, bowser_3_geo_000398),
        OBJECT( MODEL_NONE,  0, 1307, 0,  0, 183, 0,  BPARAM2(WARP_NODE_0A),  bhvSpinAirborneCircleWarp),
        WARP_NODE( WARP_NODE_0A,     LEVEL_BOWSER_3,  1,  WARP_NODE_0A,  WARP_NO_CHECKPOINT),
        JUMP_LINK(script_func_local_1),
        WARP_NODE( WARP_NODE_DEATH,  LEVEL_BITS,      1,  WARP_NODE_0C,  WARP_NO_CHECKPOINT),
        TERRAIN( bowser_3_seg7_collision_level),
        SET_BACKGROUND_MUSIC( 0x0002,  SEQ_LEVEL_BOSS_KOOPA_FINAL),
        TERRAIN_TYPE( TERRAIN_STONE),
    END_AREA(),

    FREE_LEVEL_POOL(),
    MARIO_POS( 1,  183,  0, 307, 0),
    CALL( 0,  lvl_init_or_update),
    CALL_LOOP( 1,  lvl_init_or_update),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    EXIT(),
};
