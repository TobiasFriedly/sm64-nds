#ifndef INGAME_MENU_H
#define INGAME_MENU_H

#include <PR/ultratypes.h>

#define ASCII_TO_DIALOG(asc)                                       \
    (((asc) >= '0' && (asc) <= '9') ? ((asc) - '0') :              \
     ((asc) >= 'A' && (asc) <= 'Z') ? ((asc) - 'A' + 0x0A) :       \
     ((asc) >= 'a' && (asc) <= 'z') ? ((asc) - 'a' + 0x24) : 0x00)

#define MENU_MTX_PUSH   1
#define MENU_MTX_NOPUSH 2

#define MENU_SCROLL_VERTICAL   1
#define MENU_SCROLL_HORIZONTAL 2

#define HUD_LUT_JPMENU 1
#define HUD_LUT_GLOBAL 2

#if defined(VERSION_JP) || defined(VERSION_SH)
#define HUD_LUT_DIFF HUD_LUT_JPMENU
#else
#define HUD_LUT_DIFF HUD_LUT_GLOBAL
#endif

#ifdef VERSION_CN
#define HUD_LUT_DIFF2 HUD_LUT_JPMENU
#else
#define HUD_LUT_DIFF2 HUD_LUT_DIFF
#endif

enum MenuMode {
    MENU_MODE_NONE = -1,
    MENU_MODE_UNUSED_0,
    MENU_MODE_RENDER_PAUSE_SCREEN,
    MENU_MODE_RENDER_COURSE_COMPLETE_SCREEN,
    MENU_MODE_UNUSED_3
};

extern s8 gDialogCourseActNum;
extern s8 gHudFlash;

struct DialogEntry {
     u32 unused;
     s8 linesPerBox;
     s16 leftOffset;
     s16 width;
     const u8 *str;
};

enum HudSpecialCharsEU {
    HUD_CHAR_A_UMLAUT = 0x3A,
    HUD_CHAR_O_UMLAUT = 0x3B,
    HUD_CHAR_U_UMLAUT = 0x3C
};

enum SpecialFontChars {
    GLOBAL_CHAR_SPACE = 0x9E,
#ifdef VERSION_CN
    GLOBAL_CHAR_NEWLINE = 0xFE,
#endif
    GLOBAL_CHAR_TERMINATOR = 0xFF
};

enum DialogMark {
    DIALOG_MARK_NONE,
    DIALOG_MARK_DAKUTEN,
    DIALOG_MARK_HANDAKUTEN
};

enum DialogSpecialChars {
#ifdef VERSION_EU
    DIALOG_CHAR_LOWER_A_GRAVE = 0x60,
    DIALOG_CHAR_LOWER_A_CIRCUMFLEX = 0x61,
    DIALOG_CHAR_LOWER_A_UMLAUT = 0x62,
    DIALOG_CHAR_UPPER_A_GRAVE = 0x64,
    DIALOG_CHAR_UPPER_A_CIRCUMFLEX = 0x65,
    DIALOG_CHAR_UPPER_A_UMLAUT = 0x66,
    DIALOG_CHAR_LOWER_E_GRAVE = 0x70,
    DIALOG_CHAR_LOWER_E_CIRCUMFLEX = 0x71,
    DIALOG_CHAR_LOWER_E_UMLAUT = 0x72,
    DIALOG_CHAR_LOWER_E_ACUTE = 0x73,
    DIALOG_CHAR_UPPER_E_GRAVE = 0x74,
    DIALOG_CHAR_UPPER_E_CIRCUMFLEX = 0x75,
    DIALOG_CHAR_UPPER_E_UMLAUT = 0x76,
    DIALOG_CHAR_UPPER_E_ACUTE = 0x77,
    DIALOG_CHAR_LOWER_U_GRAVE = 0x80,
    DIALOG_CHAR_LOWER_U_CIRCUMFLEX = 0x81,
    DIALOG_CHAR_LOWER_U_UMLAUT = 0x82,
    DIALOG_CHAR_UPPER_U_GRAVE = 0x84,
    DIALOG_CHAR_UPPER_U_CIRCUMFLEX = 0x85,
    DIALOG_CHAR_UPPER_U_UMLAUT = 0x86,
    DIALOG_CHAR_LOWER_O_CIRCUMFLEX = 0x91,
    DIALOG_CHAR_LOWER_O_UMLAUT = 0x92,
    DIALOG_CHAR_UPPER_O_CIRCUMFLEX = 0x95,
    DIALOG_CHAR_UPPER_O_UMLAUT = 0x96,
    DIALOG_CHAR_LOWER_I_CIRCUMFLEX = 0xA1,
    DIALOG_CHAR_LOWER_I_UMLAUT = 0xA2,
    DIALOG_CHAR_I_NO_DIA = 0xEB,
    DIALOG_CHAR_DOUBLE_LOW_QUOTE = 0xF0,
#endif
#if defined(VERSION_US) || defined(VERSION_EU) || defined(VERSION_CN)
    DIALOG_CHAR_SLASH = 0xD0,
    DIALOG_CHAR_MULTI_THE = 0xD1,
    DIALOG_CHAR_MULTI_YOU = 0xD2,
#endif
    DIALOG_CHAR_PERIOD = 0x6E,
    DIALOG_CHAR_COMMA = 0x6F,
    DIALOG_CHAR_SPACE = 0x9E,
    DIALOG_CHAR_STAR_COUNT = 0xE0,
    DIALOG_CHAR_UMLAUT = 0xE9,
    DIALOG_CHAR_MARK_START = 0xEF,
    DIALOG_CHAR_DAKUTEN = DIALOG_CHAR_MARK_START + DIALOG_MARK_DAKUTEN,
    DIALOG_CHAR_PERIOD_OR_HANDAKUTEN = DIALOG_CHAR_MARK_START + DIALOG_MARK_HANDAKUTEN,
    DIALOG_CHAR_STAR_FILLED = 0xFA,
    DIALOG_CHAR_STAR_OPEN = 0xFD,
    DIALOG_CHAR_NEWLINE = 0xFE,
    DIALOG_CHAR_TERMINATOR = 0xFF
};

#ifdef VERSION_CN
#define DIALOG_CHAR_SPECIAL_MODIFIER 0xFF
#define SPECIAL_CHAR(x) (DIALOG_CHAR_SPECIAL_MODIFIER << 8 | (x))
#else
#define SPECIAL_CHAR(x) (x)
#endif

enum DialogResponseDefines {
    DIALOG_RESPONSE_NONE,
    DIALOG_RESPONSE_YES,
    DIALOG_RESPONSE_NO,
    DIALOG_RESPONSE_NOT_DEFINED
};

extern s32 gDialogResponse;
extern u16 gMenuTextColorTransTimer;
extern s8 gLastDialogLineNum;
extern s32 gDialogVariable;
extern u16 gMenuTextAlpha;
extern s16 gCutsceneMsgXOffset;
extern s16 gCutsceneMsgYOffset;
extern s8 gRedCoinsCollected;

void create_dl_identity_matrix(void);
void create_dl_translation_matrix(s8 pushOp, f32 x, f32 y, f32 z);
void create_dl_ortho_matrix(void);

void print_generic_string(s16 x, s16 y, const u8 *str);
void print_hud_lut_string(s8 hudLUT, s16 x, s16 y, const u8 *str);
void print_menu_generic_string(s16 x, s16 y, const u8 *str);

void handle_menu_scrolling(s8 scrollDirection, s8 *currentIndex, s8 minIndex, s8 maxIndex);
#if defined(VERSION_US) || defined(VERSION_EU) || defined(VERSION_CN)
s16 get_str_x_pos_from_center(s16 centerPos, u8 *str, f32 scale);
#endif
#if defined(VERSION_JP) || defined(VERSION_SH)
#define get_str_x_pos_from_center get_str_x_pos_from_center_scale
#endif
#if defined(VERSION_JP) || defined(VERSION_EU) || defined(VERSION_SH)
s16 get_str_x_pos_from_center_scale(s16 centerPos, u8 *str, f32 scale);
#endif
void print_hud_my_score_coins(s32 useCourseCoinScore, s8 fileIndex, s8 courseIndex, s16 x, s16 y);

void int_to_str(s32 num, u8 *dst);

#ifdef VERSION_CN
void int_to_str_2(s32 num, u8 *dst);
#endif

#ifdef VERSION_CN
#define INT_TO_STR_DIFF int_to_str_2
#else
#define INT_TO_STR_DIFF int_to_str
#endif

s16 get_dialog_id(void);
void create_dialog_box(s16 dialog);
void create_dialog_box_with_var(s16 dialog, s32 dialogVar);
void create_dialog_inverted_box(s16 dialog);
void create_dialog_box_with_response(s16 dialog);
void reset_dialog_render_state(void);
void set_menu_mode(s16 mode);
void reset_cutscene_msg_fade(void);
void dl_rgba16_begin_cutscene_msg_fade(void);
void dl_rgba16_stop_cutscene_msg_fade(void);
void print_credits_str_ascii(s16 x, s16 y, const char *str);
void set_cutscene_message(s16 xOffset, s16 yOffset, s16 msgIndex, s16 msgDuration);
void do_cutscene_handler(void);
void render_hud_cannon_reticle(void);
void reset_red_coins_collected(void);
s16 render_menus_and_dialogs(void);

#endif
