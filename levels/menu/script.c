#include <ultra64.h>
#include "sm64.h"
#include "behavior_data.h"
#include "model_ids.h"
#include "seq_ids.h"
#include "segment_symbols.h"
#include "level_commands.h"

#include "game/area.h"
#include "game/level_update.h"
#include "menu/file_select.h"
#include "menu/star_select.h"

#include "levels/scripts.h"

#include "actors/common1.h"

#include "make_const_nonconst.h"
#include "levels/menu/header.h"

const LevelScript level_main_menu_entry_1[] = {
    INIT_LEVEL(),
    FIXED_LOAD( _goddardSegmentStart,  _goddardSegmentRomStart,  _goddardSegmentRomEnd),
    LOAD_MIO0( 0x07, _menu_segment_7SegmentRomStart, _menu_segment_7SegmentRomEnd),
    LOAD_RAW ( 0x13, _behaviorSegmentRomStart, _behaviorSegmentRomEnd),
    ALLOC_LEVEL_POOL(),
    LOAD_MODEL_FROM_GEO(MODEL_MAIN_MENU_MARIO_SAVE_BUTTON,      geo_menu_mario_save_button),
    LOAD_MODEL_FROM_GEO(MODEL_MAIN_MENU_RED_ERASE_BUTTON,       geo_menu_erase_button),
    LOAD_MODEL_FROM_GEO(MODEL_MAIN_MENU_BLUE_COPY_BUTTON,       geo_menu_copy_button),
    LOAD_MODEL_FROM_GEO(MODEL_MAIN_MENU_YELLOW_FILE_BUTTON,     geo_menu_file_button),
    LOAD_MODEL_FROM_GEO(MODEL_MAIN_MENU_GREEN_SCORE_BUTTON,     geo_menu_score_button),
    LOAD_MODEL_FROM_GEO(MODEL_MAIN_MENU_MARIO_SAVE_BUTTON_FADE, geo_menu_mario_save_button_fade),
    LOAD_MODEL_FROM_GEO(MODEL_MAIN_MENU_MARIO_NEW_BUTTON,       geo_menu_mario_new_button),
    LOAD_MODEL_FROM_GEO(MODEL_MAIN_MENU_MARIO_NEW_BUTTON_FADE,  geo_menu_mario_new_button_fade),
    LOAD_MODEL_FROM_GEO(MODEL_MAIN_MENU_PURPLE_SOUND_BUTTON,    geo_menu_sound_button),
    LOAD_MODEL_FROM_GEO(MODEL_MAIN_MENU_GENERIC_BUTTON,         geo_menu_generic_button),

    AREA( 1, geo_menu_file_select_strings_and_menu_cursor),
        OBJECT( MODEL_NONE,          0, 0, -19000,  0, 0, 0,  BPARAM1(0x04),  bhvMenuButtonManager),
        OBJECT( MODEL_MAIN_MENU_YELLOW_FILE_BUTTON,  0, 0, -19000,  0, 0, 0,  BPARAM1(0x04),  bhvYellowBackgroundInMenu),
        TERRAIN( main_menu_seg7_collision),
    END_AREA(),

    FREE_LEVEL_POOL(),
    LOAD_AREA( 1),
    SET_MENU_MUSIC( SEQ_MENU_FILE_SELECT),
    TRANSITION( WARP_TRANSITION_FADE_FROM_COLOR,  16,  0xFF, 0xFF, 0xFF),
    CALL( 0,  lvl_init_menu_values_and_cursor_pos),
    CALL_LOOP( 0,  lvl_update_obj_and_load_file_selected),
    GET_OR_SET( OP_SET,  VAR_CURR_SAVE_FILE_NUM),
    STOP_MUSIC( 0x00BE),
    TRANSITION( WARP_TRANSITION_FADE_INTO_COLOR,  16,  0xFF, 0xFF, 0xFF),
    SLEEP( 16),
    CLEAR_LEVEL(),
    SLEEP_BEFORE_EXIT( 1),
    SET_REG( LEVEL_CASTLE_GROUNDS),
    EXIT_AND_EXECUTE( 0x15, _scriptsSegmentRomStart, _scriptsSegmentRomEnd, level_main_scripts_entry),
};

const LevelScript level_main_menu_entry_2[] = {
     CALL( 0,  lvl_set_current_level),
     JUMP_IF( OP_EQ,  0, level_main_menu_entry_2 + 42),
     INIT_LEVEL(),
     FIXED_LOAD( _goddardSegmentStart,  _goddardSegmentRomStart,  _goddardSegmentRomEnd),
     LOAD_MIO0( 0x07, _menu_segment_7SegmentRomStart, _menu_segment_7SegmentRomEnd),
     ALLOC_LEVEL_POOL(),

     AREA( 2, geo_menu_act_selector_strings),
         OBJECT( MODEL_NONE,  0, -100, 0,  0, 0, 0,  BPARAM1(0x04),  bhvActSelector),
         TERRAIN( main_menu_seg7_collision),
     END_AREA(),

     FREE_LEVEL_POOL(),
     LOAD_AREA( 2),
#ifdef NO_SEGMENTED_MEMORY

           CALL( 0,  lvl_init_act_selector_values_and_stars),
#endif
     TRANSITION( WARP_TRANSITION_FADE_FROM_COLOR,  16,  0xFF, 0xFF, 0xFF),
     SLEEP( 16),
     SET_MENU_MUSIC( 0x000D),
#ifndef NO_SEGMENTED_MEMORY
     CALL( 0,  lvl_init_act_selector_values_and_stars),
#endif
     CALL_LOOP( 0,  lvl_update_obj_and_load_act_button_actions),
     GET_OR_SET( OP_SET,  VAR_CURR_ACT_NUM),
     STOP_MUSIC( 0x00BE),
     TRANSITION( WARP_TRANSITION_FADE_INTO_COLOR,  16,  0xFF, 0xFF, 0xFF),
     SLEEP( 16),
     CLEAR_LEVEL(),
     SLEEP_BEFORE_EXIT( 1),

     EXIT(),
};
