#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "game/area.h"
#include "game/level_update.h"

#include "levels/scripts.h"

#include "actors/common1.h"

#include "make_const_nonconst.h"
#include "levels/ending/header.h"

const LevelScript level_ending_entry[] = {
     INIT_LEVEL(),
     LOAD_MIO0( 0x07, _ending_segment_7SegmentRomStart, _ending_segment_7SegmentRomEnd),
     ALLOC_LEVEL_POOL(),

     AREA( 1, ending_geo_000050),
     END_AREA(),

     FREE_LEVEL_POOL(),
     SLEEP( 60),
     BLACKOUT( FALSE),
     LOAD_AREA( 1),
     TRANSITION( WARP_TRANSITION_FADE_FROM_COLOR,  75,  0x00, 0x00, 0x00),
     SLEEP( 120),
     CALL( 0,  lvl_play_the_end_screen_sound),

     SLEEP( 1),
     JUMP(level_ending_entry + 17),
};
