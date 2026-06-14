#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "game/area.h"
#include "game/level_update.h"
#include "menu/title_screen.h"

#include "levels/scripts.h"
#include "levels/menu/header.h"

#include "actors/common1.h"

#include "make_const_nonconst.h"
#include "levels/intro/header.h"

const LevelScript level_intro_splash_screen[] = {
    INIT_LEVEL(),
    FIXED_LOAD( _goddardSegmentStart,  _goddardSegmentRomStart,  _goddardSegmentRomEnd),
    LOAD_RAW ( 0x13, _behaviorSegmentRomStart, _behaviorSegmentRomEnd),
    LOAD_MIO0( 0x07, _intro_segment_7SegmentRomStart, _intro_segment_7SegmentRomEnd),

    ALLOC_LEVEL_POOL(),
    AREA( 1, intro_geo_0002D0),
    END_AREA(),
    FREE_LEVEL_POOL(),

    LOAD_AREA( 1),

    CALL( LVL_INTRO_PLAY_ITS_A_ME_MARIO,  lvl_intro_update),
    SLEEP( 75),
    TRANSITION( WARP_TRANSITION_FADE_INTO_COLOR,  16,  0x00, 0x00, 0x00),
    SLEEP( 16),
    CMD2A( 1),
    CLEAR_LEVEL(),
    SLEEP( 2),

    SET_REG( 16),
    EXIT_AND_EXECUTE( 0x14, _menuSegmentRomStart, _menuSegmentRomEnd, level_main_menu_entry_1),
};

const LevelScript level_intro_mario_head_regular[] = {
    INIT_LEVEL(),
    BLACKOUT( TRUE),
    FIXED_LOAD( _goddardSegmentStart,  _goddardSegmentRomStart,  _goddardSegmentRomEnd),
    LOAD_MARIO_HEAD( REGULAR_FACE),
    LOAD_RAW         ( 0x13, _behaviorSegmentRomStart, _behaviorSegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x0A, _title_screen_bg_mio0SegmentRomStart, _title_screen_bg_mio0SegmentRomEnd),

    ALLOC_LEVEL_POOL(),
    AREA( 1, intro_geo_mario_head_regular),
    END_AREA(),
    FREE_LEVEL_POOL(),

    SLEEP( 2),
    BLACKOUT( FALSE),
    LOAD_AREA( 1),
    SET_MENU_MUSIC( SEQ_MENU_TITLE_SCREEN),
    TRANSITION( WARP_TRANSITION_FADE_FROM_STAR,  20,  0x00, 0x00, 0x00),
    SLEEP( 20),
    CALL_LOOP( LVL_INTRO_REGULAR,  lvl_intro_update),

    JUMP_IF( OP_EQ,  100, script_intro_to_splash),
    JUMP_IF( OP_EQ,  101, script_intro_L2),
    JUMP(script_intro_L4),
};

const LevelScript level_intro_mario_head_dizzy[] = {
    INIT_LEVEL(),
    BLACKOUT( TRUE),
    FIXED_LOAD( _goddardSegmentStart,  _goddardSegmentRomStart,  _goddardSegmentRomEnd),
    LOAD_MARIO_HEAD( DIZZY_FACE),
    LOAD_RAW         ( 0x13, _behaviorSegmentRomStart, _behaviorSegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x0A, _title_screen_bg_mio0SegmentRomStart, _title_screen_bg_mio0SegmentRomEnd),
    ALLOC_LEVEL_POOL(),

    AREA( 1, intro_geo_mario_head_dizzy),
    END_AREA(),

    FREE_LEVEL_POOL(),
    SLEEP( 2),
    BLACKOUT( FALSE),
    LOAD_AREA( 1),
    SET_MENU_MUSIC( SEQ_MENU_GAME_OVER),
    TRANSITION( WARP_TRANSITION_FADE_FROM_STAR,  20,  0x00, 0x00, 0x00),
    SLEEP( 20),
    CALL_LOOP( LVL_INTRO_GAME_OVER,  lvl_intro_update),
    JUMP_IF( OP_EQ,  100, script_intro_L1),
    JUMP_IF( OP_EQ,  101, script_intro_L2),
    JUMP(script_intro_L4),
};

const LevelScript level_intro_entry_4[] = {
    INIT_LEVEL(),
    LOAD_RAW         ( 0x13, _behaviorSegmentRomStart, _behaviorSegmentRomEnd),
    LOAD_MIO0_TEXTURE( 0x0A, _title_screen_bg_mio0SegmentRomStart, _title_screen_bg_mio0SegmentRomEnd),
    LOAD_MIO0        ( 0x07, _debug_level_select_mio0SegmentRomStart, _debug_level_select_mio0SegmentRomEnd),
    FIXED_LOAD( _goddardSegmentStart,  _goddardSegmentRomStart,  _goddardSegmentRomEnd),
    ALLOC_LEVEL_POOL(),

    AREA( 1, intro_geo_000414),
    END_AREA(),

    FREE_LEVEL_POOL(),
    LOAD_AREA( 1),
    SET_MENU_MUSIC( SEQ_MENU_TITLE_SCREEN),
    TRANSITION( WARP_TRANSITION_FADE_FROM_COLOR,  16,  0xFF, 0xFF, 0xFF),
    SLEEP( 16),
    CALL_LOOP( LVL_INTRO_LEVEL_SELECT,  lvl_intro_update),
    JUMP_IF( OP_EQ,  -1, script_intro_L5),
    JUMP(script_intro_L3),
};

const LevelScript script_intro_L1[] = {
    STOP_MUSIC( 0x00BE),
    TRANSITION( WARP_TRANSITION_FADE_INTO_COLOR,  16,  0xFF, 0xFF, 0xFF),
    SLEEP( 16),
    CLEAR_LEVEL(),
    SLEEP( 2),
    SET_REG( 16),
    EXIT_AND_EXECUTE( 0x14, _menuSegmentRomStart, _menuSegmentRomEnd, level_main_menu_entry_1),
};

const LevelScript script_intro_L2[] = {
    TRANSITION( WARP_TRANSITION_FADE_INTO_COLOR,  16,  0xFF, 0xFF, 0xFF),
    SLEEP( 16),
    CLEAR_LEVEL(),
    SLEEP( 2),
    EXIT_AND_EXECUTE( 0x14, _introSegmentRomStart, _introSegmentRomEnd, level_intro_entry_4),
};

const LevelScript script_intro_L3[] = {
    STOP_MUSIC( 0x00BE),
    TRANSITION( WARP_TRANSITION_FADE_INTO_COLOR,  16,  0xFF, 0xFF, 0xFF),
    SLEEP( 16),
    CLEAR_LEVEL(),
    SLEEP( 2),
    EXIT_AND_EXECUTE( 0x15, _scriptsSegmentRomStart, _scriptsSegmentRomEnd, level_main_scripts_entry),
};

const LevelScript script_intro_L4[] = {
    TRANSITION( WARP_TRANSITION_FADE_INTO_COLOR,  16,  0xFF, 0xFF, 0xFF),
    SLEEP( 16),
    CLEAR_LEVEL(),
    SLEEP( 2),
    EXIT_AND_EXECUTE( 0x15, _scriptsSegmentRomStart, _scriptsSegmentRomEnd, level_main_scripts_entry),
};

const LevelScript script_intro_L5[] = {
    STOP_MUSIC( 0x00BE),
    TRANSITION( WARP_TRANSITION_FADE_INTO_COLOR,  16,  0x00, 0x00, 0x00),
    SLEEP( 16),
    CLEAR_LEVEL(),
    SLEEP( 2),
    EXIT_AND_EXECUTE( 0x14, _introSegmentRomStart, _introSegmentRomEnd, level_intro_splash_screen),
};

const LevelScript script_intro_to_splash[] = {
    STOP_MUSIC( 0x00BE),
    TRANSITION( WARP_TRANSITION_FADE_INTO_COLOR,  16,  0x00, 0x00, 0x00),
    SLEEP( 16),
    CLEAR_LEVEL(),
    SLEEP( 2),
    EXIT_AND_EXECUTE( 0x14, _introSegmentRomStart, _introSegmentRomEnd, level_intro_splash_screen),
};
