#include <PR/ultratypes.h>
#include <PR/gbi.h>

#include "audio/external.h"
#include "behavior_data.h"
#include "dialog_ids.h"
#include "engine/behavior_script.h"
#include "engine/graph_node.h"
#include "engine/math_util.h"
#include "file_select.h"
#include "game/area.h"
#include "game/game_init.h"
#include "game/ingame_menu.h"
#include "game/object_helpers.h"
#include "game/object_list_processor.h"
#include "game/print.h"
#include "game/save_file.h"
#include "game/segment2.h"
#include "game/segment7.h"
#include "game/spawn_object.h"
#include "game/rumble_init.h"
#include "sm64.h"
#include "text_strings.h"

#include "eu_translation.h"
#ifdef VERSION_EU
#undef LANGUAGE_FUNCTION
#define LANGUAGE_FUNCTION sLanguageMode
#endif

#ifdef VERSION_CN
#define FILE_SELECT_PRINT_STRING print_generic_string
#define FILE_SELECT_TEXT_DL_BEGIN dl_ia_text_begin
#define FILE_SELECT_TEXT_DL_END dl_ia_text_end
#else
#define FILE_SELECT_PRINT_STRING print_menu_generic_string
#define FILE_SELECT_TEXT_DL_BEGIN dl_menu_ia8_text_begin
#define FILE_SELECT_TEXT_DL_END dl_menu_ia8_text_end
#endif

#ifdef VERSION_US

static s16 sSoundTextX;
#endif

#if defined(VERSION_EU) && !defined(AVOID_UB)
#define NUM_BUTTONS (MENU_BUTTON_OPTION_MAX - 1)
#else
#define NUM_BUTTONS MENU_BUTTON_OPTION_MAX
#endif

static struct Object *sMainMenuButtons[NUM_BUTTONS];

static u8 sYesNoColor[2];

static s8 sSelectedButtonID = MENU_BUTTON_NONE;

#ifdef VERSION_CN
static s8 sScorePage = 0;
#endif

static s8 sCurrentMenuLevel = MENU_LAYER_MAIN;

static u8 sTextBaseAlpha = 0;

static f32 sCursorPos[] = {0, 0};

static s16 sCursorClickingTimer = 0;

static s16 sClickPos[] = {-10000, -10000};

static s8 sSelectedFileIndex = -1;

static s8 sFadeOutText = FALSE;

static s8 sStatusMessageID = 0;

static u8 sTextFadeAlpha = 0;

static s16 sMainMenuTimer = 0;

static s8 sSoundMode = 0;

#ifdef VERSION_EU
static s8 sLanguageMode = LANGUAGE_ENGLISH;
#endif

static s8 sEraseYesNoHoverState = MENU_ERASE_HOVER_NONE;

static s8 sAllFilesExist = FALSE;

static s8 sSelectedFileNum = 0;

static s8 sScoreFileCoinScoreMode = 0;

#ifdef VERSION_EU
static s8 sOpenLangSettings = FALSE;
#endif

#ifndef VERSION_EU
static u8 textReturn[] = { TEXT_RETURN };
#else
static u8 textReturn[][8] = {{ TEXT_RETURN }, { TEXT_RETURN_FR }, { TEXT_RETURN_DE }};
#endif

#ifndef VERSION_EU
static u8 textViewScore[] = { TEXT_CHECK_SCORE };
#else
static u8 textViewScore[][12] = {{ TEXT_CHECK_SCORE }, {TEXT_CHECK_SCORE_FR}, {TEXT_CHECK_SCORE_DE}};
#endif

#ifndef VERSION_EU
static u8 textCopyFileButton[] = { TEXT_COPY_FILE_BUTTON };
#else
static u8 textCopyFileButton[][15] = {{ TEXT_COPY_FILE }, { TEXT_COPY_FILE_FR }, { TEXT_COPY_FILE_DE }};
#endif

#ifndef VERSION_EU
static u8 textEraseFileButton[] = { TEXT_ERASE_FILE_BUTTON };
#else
static u8 textEraseFileButton[][16] = { {TEXT_ERASE_FILE}, {TEXT_ERASE_FILE_FR}, {TEXT_ERASE_FILE_DE} };
#endif

#ifndef VERSION_EU
static u8 textSoundModes[][8] = { { TEXT_STEREO }, { TEXT_MONO }, { TEXT_HEADSET } };
#endif

static u8 textMarioA[] = { TEXT_FILE_MARIO_A };
static u8 textMarioB[] = { TEXT_FILE_MARIO_B };
static u8 textMarioC[] = { TEXT_FILE_MARIO_C };
static u8 textMarioD[] = { TEXT_FILE_MARIO_D };

#ifndef VERSION_EU
static u8 textNew[] = { TEXT_NEW };
static u8 starIcon[] = { GLYPH_STAR, GLYPH_SPACE };
static u8 xIcon[] = { GLYPH_MULTIPLY, GLYPH_SPACE };
#endif

#ifndef VERSION_EU
static u8 textSelectFile[] = { TEXT_SELECT_FILE };
#else
static u8 textSelectFile[][17] = {{ TEXT_SELECT_FILE }, { TEXT_SELECT_FILE_FR }, { TEXT_SELECT_FILE_DE }};
#endif

#ifndef VERSION_EU
static u8 textScore[] = { TEXT_SCORE };
#else
static u8 textScore[][9] = {{ TEXT_SCORE }, { TEXT_SCORE_FR }, { TEXT_SCORE_DE }};
#endif

#ifndef VERSION_EU
static u8 textCopy[] = { TEXT_COPY };
#else
static u8 textCopy[][9] = {{ TEXT_COPY }, { TEXT_COPY_FR }, { TEXT_COPY_DE }};
#endif

#ifndef VERSION_EU
static u8 textErase[] = { TEXT_ERASE };
#else
static u8 textErase[][8] = {{ TEXT_ERASE }, { TEXT_ERASE_FR }, { TEXT_ERASE_DE }};
#endif

#ifdef VERSION_EU
static u8 textOption[][9] = {{ TEXT_OPTION }, { TEXT_OPTION_FR }, { TEXT_OPTION_DE } };
#endif

#ifndef VERSION_EU
static u8 textCheckFile[] = { TEXT_CHECK_FILE };
#else
static u8 textCheckFile[][18] = {{ TEXT_CHECK_FILE }, { TEXT_CHECK_FILE_FR }, { TEXT_CHECK_FILE_DE }};
#endif

#ifndef VERSION_EU
static u8 textNoSavedDataExists[] = { TEXT_NO_SAVED_DATA_EXISTS };
#else
static u8 textNoSavedDataExists[][30] = {{ TEXT_NO_SAVED_DATA_EXISTS }, { TEXT_NO_SAVED_DATA_EXISTS_FR }, { TEXT_NO_SAVED_DATA_EXISTS_DE }};
#endif

#ifndef VERSION_EU
static u8 textCopyFile[] = { TEXT_COPY_FILE };
#else
static u8 textCopyFile[][16] = {{ TEXT_COPY_FILE_BUTTON }, { TEXT_COPY_FILE_BUTTON_FR }, { TEXT_COPY_FILE_BUTTON_DE }};
#endif

#ifndef VERSION_EU
static u8 textCopyItToWhere[] = { TEXT_COPY_IT_TO_WHERE };
#else
static u8 textCopyItToWhere[][18] = {{ TEXT_COPY_IT_TO_WHERE }, { TEXT_COPY_IT_TO_WHERE_FR }, { TEXT_COPY_IT_TO_WHERE_DE }};
#endif

#if !defined(VERSION_EU)
static u8 textNoSavedDataExistsCopy[] = { TEXT_NO_SAVED_DATA_EXISTS };
#endif

#ifndef VERSION_EU
static u8 textCopyCompleted[] = { TEXT_COPYING_COMPLETED };
#else
static u8 textCopyCompleted[][18] = {{ TEXT_COPYING_COMPLETED }, { TEXT_COPYING_COMPLETED_FR }, { TEXT_COPYING_COMPLETED_DE }};
#endif

#ifndef VERSION_EU
static u8 textSavedDataExists[] = { TEXT_SAVED_DATA_EXISTS };
#else
static u8 textSavedDataExists[][20] = {{ TEXT_SAVED_DATA_EXISTS }, { TEXT_SAVED_DATA_EXISTS_FR }, { TEXT_SAVED_DATA_EXISTS_DE }};
#endif

#ifndef VERSION_EU
static u8 textNoFileToCopyFrom[] = { TEXT_NO_FILE_TO_COPY_FROM };
#else
static u8 textNoFileToCopyFrom[][21] = {{ TEXT_NO_FILE_TO_COPY_FROM }, { TEXT_NO_FILE_TO_COPY_FROM_FR }, { TEXT_NO_FILE_TO_COPY_FROM_DE }};
#endif

#ifndef VERSION_EU
static u8 textYes[] = { TEXT_YES };
#else
static u8 textYes[][4] = {{ TEXT_YES }, { TEXT_YES_FR }, { TEXT_YES_DE }};
#endif

#ifndef VERSION_EU
static u8 textNo[] = { TEXT_NO };
#else
static u8 textNo[][5] = {{ TEXT_NO }, { TEXT_NO_FR }, { TEXT_NO_DE }};
#endif

#ifdef VERSION_EU

static u8 textEraseFile[][17] = {
    { TEXT_ERASE_FILE_BUTTON }, { TEXT_ERASE_FILE_BUTTON_FR }, { TEXT_ERASE_FILE_BUTTON_DE }
};
static u8 textSure[][8] = {{ TEXT_SURE }, { TEXT_SURE_FR }, { TEXT_SURE_DE }};
static u8 textMarioAJustErased[][20] = {
    { TEXT_FILE_MARIO_A_JUST_ERASED }, { TEXT_FILE_MARIO_A_JUST_ERASED_FR }, { TEXT_FILE_MARIO_A_JUST_ERASED_DE }
};

static u8 textSoundSelect[][13] = {
    { TEXT_SOUND_SELECT }, { TEXT_SOUND_SELECT_FR }, { TEXT_SOUND_SELECT_DE }
};

static u8 textLanguageSelect[][17] = {
    { TEXT_LANGUAGE_SELECT }, { TEXT_LANGUAGE_SELECT_FR }, { TEXT_LANGUAGE_SELECT_DE }
};

static u8 textSoundModes[][10] = {
    { TEXT_STEREO }, { TEXT_MONO }, { TEXT_HEADSET },
    { TEXT_STEREO_FR }, { TEXT_MONO_FR }, { TEXT_HEADSET_FR },
    { TEXT_STEREO_DE }, { TEXT_MONO_DE }, { TEXT_HEADSET_DE }
};

static u8 textLanguage[][9] = {{ TEXT_ENGLISH }, { TEXT_FRENCH }, { TEXT_GERMAN }};

static u8 textMario[] = { TEXT_MARIO };
static u8 textHiScore[][15] = {{ TEXT_HI_SCORE }, { TEXT_HI_SCORE_FR }, { TEXT_HI_SCORE_DE }};
static u8 textMyScore[][10] = {{ TEXT_MY_SCORE }, { TEXT_MY_SCORE_FR }, { TEXT_MY_SCORE_DE }};

static u8 textNew[][5] = {{ TEXT_NEW }, { TEXT_NEW_FR }, { TEXT_NEW_DE }};
static u8 starIcon[] = { GLYPH_STAR, GLYPH_SPACE };
static u8 xIcon[] = { GLYPH_MULTIPLY, GLYPH_SPACE };
#endif

void beh_yellow_background_menu_init(void) {
    gCurrentObject->oFaceAngleYaw = 0x8000;
    gCurrentObject->oMenuButtonScale = 9.0f;
}

void beh_yellow_background_menu_loop(void) {
    cur_obj_scale(9.0f);
}

s32 check_clicked_button(s16 x, s16 y, f32 depth) {
    f32 a = 52.4213;
    f32 newX = ((f32) x * 160.0) / (a * depth);
    f32 newY = ((f32) y * 120.0) / (a * 3 / 4 * depth);
    s16 maxX = newX + 25.0f;
    s16 minX = newX - 25.0f;
    s16 maxY = newY + 21.0f;
    s16 minY = newY - 21.0f;

    if (sClickPos[0] < maxX && minX < sClickPos[0] && sClickPos[1] < maxY && minY < sClickPos[1]) {
        return TRUE;
    }
    return FALSE;
}

static void bhv_menu_button_growing_from_main_menu(struct Object *button) {
    if (button->oMenuButtonTimer < 16) {
        button->oFaceAngleYaw += 0x800;
    }
    if (button->oMenuButtonTimer < 8) {
        button->oFaceAnglePitch += 0x800;
    }
    if (button->oMenuButtonTimer >= 8 && button->oMenuButtonTimer < 16) {
        button->oFaceAnglePitch -= 0x800;
    }
    button->oParentRelativePosX -= button->oMenuButtonOrigPosX / 16.0;
    button->oParentRelativePosY -= button->oMenuButtonOrigPosY / 16.0;
    if (button->oPosZ < button->oMenuButtonOrigPosZ + 17800.0) {
        button->oParentRelativePosZ += 1112.5;
    }
    button->oMenuButtonTimer++;
    if (button->oMenuButtonTimer == 16) {
        button->oParentRelativePosX = 0.0f;
        button->oParentRelativePosY = 0.0f;
        button->oMenuButtonState = MENU_BUTTON_STATE_FULLSCREEN;
        button->oMenuButtonTimer = 0;
    }
}

static void bhv_menu_button_shrinking_to_main_menu(struct Object *button) {
    if (button->oMenuButtonTimer < 16) {
        button->oFaceAngleYaw -= 0x800;
    }
    if (button->oMenuButtonTimer < 8) {
        button->oFaceAnglePitch -= 0x800;
    }
    if (button->oMenuButtonTimer >= 8 && button->oMenuButtonTimer < 16) {
        button->oFaceAnglePitch += 0x800;
    }
    button->oParentRelativePosX += button->oMenuButtonOrigPosX / 16.0;
    button->oParentRelativePosY += button->oMenuButtonOrigPosY / 16.0;
    if (button->oPosZ > button->oMenuButtonOrigPosZ) {
        button->oParentRelativePosZ -= 1112.5;
    }
    button->oMenuButtonTimer++;
    if (button->oMenuButtonTimer == 16) {
        button->oParentRelativePosX = button->oMenuButtonOrigPosX;
        button->oParentRelativePosY = button->oMenuButtonOrigPosY;
        button->oMenuButtonState = MENU_BUTTON_STATE_DEFAULT;
        button->oMenuButtonTimer = 0;
    }
}

static void bhv_menu_button_growing_from_submenu(struct Object *button) {
    if (button->oMenuButtonTimer < 16) {
        button->oFaceAngleYaw += 0x800;
    }
    if (button->oMenuButtonTimer < 8) {
        button->oFaceAnglePitch += 0x800;
    }
    if (button->oMenuButtonTimer >= 8 && button->oMenuButtonTimer < 16) {
        button->oFaceAnglePitch -= 0x800;
    }
    button->oParentRelativePosX -= button->oMenuButtonOrigPosX / 16.0;
    button->oParentRelativePosY -= button->oMenuButtonOrigPosY / 16.0;
    button->oParentRelativePosZ -= 116.25;
    button->oMenuButtonTimer++;
    if (button->oMenuButtonTimer == 16) {
        button->oParentRelativePosX = 0.0f;
        button->oParentRelativePosY = 0.0f;
        button->oMenuButtonState = MENU_BUTTON_STATE_FULLSCREEN;
        button->oMenuButtonTimer = 0;
    }
}

static void bhv_menu_button_shrinking_to_submenu(struct Object *button) {
    if (button->oMenuButtonTimer < 16) {
        button->oFaceAngleYaw -= 0x800;
    }
    if (button->oMenuButtonTimer < 8) {
        button->oFaceAnglePitch -= 0x800;
    }
    if (button->oMenuButtonTimer >= 8 && button->oMenuButtonTimer < 16) {
        button->oFaceAnglePitch += 0x800;
    }
    button->oParentRelativePosX += button->oMenuButtonOrigPosX / 16.0;
    button->oParentRelativePosY += button->oMenuButtonOrigPosY / 16.0;
    if (button->oPosZ > button->oMenuButtonOrigPosZ) {
        button->oParentRelativePosZ += 116.25;
    }
    button->oMenuButtonTimer++;
    if (button->oMenuButtonTimer == 16) {
        button->oParentRelativePosX = button->oMenuButtonOrigPosX;
        button->oParentRelativePosY = button->oMenuButtonOrigPosY;
        button->oMenuButtonState = MENU_BUTTON_STATE_DEFAULT;
        button->oMenuButtonTimer = 0;
    }
}

static void bhv_menu_button_zoom_in_out(struct Object *button) {
    if (sCurrentMenuLevel == MENU_LAYER_MAIN) {
        if (button->oMenuButtonTimer < 4) {
            button->oParentRelativePosZ -= 20.0f;
        }
        if (button->oMenuButtonTimer >= 4) {
            button->oParentRelativePosZ += 20.0f;
        }
    } else {
        if (button->oMenuButtonTimer < 4) {
            button->oParentRelativePosZ += 20.0f;
        }
        if (button->oMenuButtonTimer >= 4) {
            button->oParentRelativePosZ -= 20.0f;
        }
    }
    button->oMenuButtonTimer++;
    if (button->oMenuButtonTimer == 8) {
        button->oMenuButtonState = MENU_BUTTON_STATE_DEFAULT;
        button->oMenuButtonTimer = 0;
    }
}

static void bhv_menu_button_zoom_in(struct Object *button) {
    button->oMenuButtonScale += 0.0022;
    button->oMenuButtonTimer++;
    if (button->oMenuButtonTimer == 10) {
        button->oMenuButtonState = MENU_BUTTON_STATE_DEFAULT;
        button->oMenuButtonTimer = 0;
    }
}

static void bhv_menu_button_zoom_out(struct Object *button) {
    button->oMenuButtonScale -= 0.0022;
    button->oMenuButtonTimer++;
    if (button->oMenuButtonTimer == 10) {
        button->oMenuButtonState = MENU_BUTTON_STATE_DEFAULT;
        button->oMenuButtonTimer = 0;
    }
}

void bhv_menu_button_init(void) {
    gCurrentObject->oMenuButtonOrigPosX = gCurrentObject->oParentRelativePosX;
    gCurrentObject->oMenuButtonOrigPosY = gCurrentObject->oParentRelativePosY;
}

void bhv_menu_button_loop(void) {
    switch (gCurrentObject->oMenuButtonState) {
        case MENU_BUTTON_STATE_DEFAULT:
            gCurrentObject->oMenuButtonOrigPosZ = gCurrentObject->oPosZ;
            break;
        case MENU_BUTTON_STATE_GROWING:
            if (sCurrentMenuLevel == MENU_LAYER_MAIN) {
                bhv_menu_button_growing_from_main_menu(gCurrentObject);
            }
            if (sCurrentMenuLevel == MENU_LAYER_SUBMENU) {
                bhv_menu_button_growing_from_submenu(gCurrentObject);
            }
            sTextBaseAlpha = 0;
            sCursorClickingTimer = 4;
            break;
        case MENU_BUTTON_STATE_FULLSCREEN:
            break;
        case MENU_BUTTON_STATE_SHRINKING:
            if (sCurrentMenuLevel == MENU_LAYER_MAIN) {
                bhv_menu_button_shrinking_to_main_menu(gCurrentObject);
            }
            if (sCurrentMenuLevel == MENU_LAYER_SUBMENU) {
                bhv_menu_button_shrinking_to_submenu(gCurrentObject);
            }
            sTextBaseAlpha = 0;
            sCursorClickingTimer = 4;
            break;
        case MENU_BUTTON_STATE_ZOOM_IN_OUT:
            bhv_menu_button_zoom_in_out(gCurrentObject);
            sCursorClickingTimer = 4;
            break;
        case MENU_BUTTON_STATE_ZOOM_IN:
            bhv_menu_button_zoom_in(gCurrentObject);
            sCursorClickingTimer = 4;
            break;
        case MENU_BUTTON_STATE_ZOOM_OUT:
            bhv_menu_button_zoom_out(gCurrentObject);
            sCursorClickingTimer = 4;
            break;
    }
    cur_obj_scale(gCurrentObject->oMenuButtonScale);
}

void exit_score_file_to_score_menu(struct Object *scoreFileButton, s8 scoreButtonID) {

    if (scoreFileButton->oMenuButtonState == MENU_BUTTON_STATE_FULLSCREEN
        && sCursorClickingTimer == 2) {
        play_sound(SOUND_MENU_CAMERA_ZOOM_OUT, gGlobalSoundSource);
#if ENABLE_RUMBLE
        queue_rumble_data(5, 80);
#endif
        scoreFileButton->oMenuButtonState = MENU_BUTTON_STATE_SHRINKING;
    }

    if (scoreFileButton->oMenuButtonState == MENU_BUTTON_STATE_DEFAULT) {
        sSelectedButtonID = scoreButtonID;
        if (sCurrentMenuLevel == MENU_LAYER_SUBMENU) {
            sCurrentMenuLevel = MENU_LAYER_MAIN;
        }
    }
}

void render_score_menu_buttons(struct Object *scoreButton) {

    if (save_file_exists(SAVE_FILE_A) == TRUE) {
        sMainMenuButtons[MENU_BUTTON_SCORE_FILE_A] =
            spawn_object_rel_with_rot(scoreButton, MODEL_MAIN_MENU_MARIO_SAVE_BUTTON, bhvMenuButton,
                                      711, 311, -100, 0, -0x8000, 0);
    } else {
        sMainMenuButtons[MENU_BUTTON_SCORE_FILE_A] =
            spawn_object_rel_with_rot(scoreButton, MODEL_MAIN_MENU_MARIO_NEW_BUTTON, bhvMenuButton, 711,
                                      311, -100, 0, -0x8000, 0);
    }
    sMainMenuButtons[MENU_BUTTON_SCORE_FILE_A]->oMenuButtonScale = 0.11111111f;

    if (save_file_exists(SAVE_FILE_B) == TRUE) {
        sMainMenuButtons[MENU_BUTTON_SCORE_FILE_B] =
            spawn_object_rel_with_rot(scoreButton, MODEL_MAIN_MENU_MARIO_SAVE_BUTTON, bhvMenuButton,
                                      -166, 311, -100, 0, -0x8000, 0);
    } else {
        sMainMenuButtons[MENU_BUTTON_SCORE_FILE_B] =
            spawn_object_rel_with_rot(scoreButton, MODEL_MAIN_MENU_MARIO_NEW_BUTTON, bhvMenuButton,
                                      -166, 311, -100, 0, -0x8000, 0);
    }
    sMainMenuButtons[MENU_BUTTON_SCORE_FILE_B]->oMenuButtonScale = 0.11111111f;

    if (save_file_exists(SAVE_FILE_C) == TRUE) {
        sMainMenuButtons[MENU_BUTTON_SCORE_FILE_C] = spawn_object_rel_with_rot(
            scoreButton, MODEL_MAIN_MENU_MARIO_SAVE_BUTTON, bhvMenuButton, 711, 0, -100, 0, -0x8000, 0);
    } else {
        sMainMenuButtons[MENU_BUTTON_SCORE_FILE_C] = spawn_object_rel_with_rot(
            scoreButton, MODEL_MAIN_MENU_MARIO_NEW_BUTTON, bhvMenuButton, 711, 0, -100, 0, -0x8000, 0);
    }
    sMainMenuButtons[MENU_BUTTON_SCORE_FILE_C]->oMenuButtonScale = 0.11111111f;

    if (save_file_exists(SAVE_FILE_D) == TRUE) {
        sMainMenuButtons[MENU_BUTTON_SCORE_FILE_D] =
            spawn_object_rel_with_rot(scoreButton, MODEL_MAIN_MENU_MARIO_SAVE_BUTTON, bhvMenuButton,
                                      -166, 0, -100, 0, -0x8000, 0);
    } else {
        sMainMenuButtons[MENU_BUTTON_SCORE_FILE_D] = spawn_object_rel_with_rot(
            scoreButton, MODEL_MAIN_MENU_MARIO_NEW_BUTTON, bhvMenuButton, -166, 0, -100, 0, -0x8000, 0);
    }
    sMainMenuButtons[MENU_BUTTON_SCORE_FILE_D]->oMenuButtonScale = 0.11111111f;

    sMainMenuButtons[MENU_BUTTON_SCORE_RETURN] = spawn_object_rel_with_rot(
        scoreButton, MODEL_MAIN_MENU_YELLOW_FILE_BUTTON, bhvMenuButton, 711, -388, -100, 0, -0x8000, 0);
    sMainMenuButtons[MENU_BUTTON_SCORE_RETURN]->oMenuButtonScale = 0.11111111f;

    sMainMenuButtons[MENU_BUTTON_SCORE_COPY_FILE] = spawn_object_rel_with_rot(
        scoreButton, MODEL_MAIN_MENU_BLUE_COPY_BUTTON, bhvMenuButton, 0, -388, -100, 0, -0x8000, 0);
    sMainMenuButtons[MENU_BUTTON_SCORE_COPY_FILE]->oMenuButtonScale = 0.11111111f;

    sMainMenuButtons[MENU_BUTTON_SCORE_ERASE_FILE] = spawn_object_rel_with_rot(
        scoreButton, MODEL_MAIN_MENU_RED_ERASE_BUTTON, bhvMenuButton, -711, -388, -100, 0, -0x8000, 0);
    sMainMenuButtons[MENU_BUTTON_SCORE_ERASE_FILE]->oMenuButtonScale = 0.11111111f;
}

#ifdef VERSION_EU
    #define SCORE_TIMER 46
#else
    #define SCORE_TIMER 31
#endif

void check_score_menu_clicked_buttons(struct Object *scoreButton) {
    if (scoreButton->oMenuButtonState == MENU_BUTTON_STATE_FULLSCREEN) {
        s32 buttonID;

        for (buttonID = MENU_BUTTON_SCORE_MIN; buttonID < MENU_BUTTON_SCORE_MAX; buttonID++) {
            s16 buttonX = sMainMenuButtons[buttonID]->oPosX;
            s16 buttonY = sMainMenuButtons[buttonID]->oPosY;

            if (check_clicked_button(buttonX, buttonY, 22.0f) == TRUE && sMainMenuTimer >= SCORE_TIMER) {

                if (buttonID == MENU_BUTTON_SCORE_RETURN || buttonID == MENU_BUTTON_SCORE_COPY_FILE
                    || buttonID == MENU_BUTTON_SCORE_ERASE_FILE) {
                    play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
#if ENABLE_RUMBLE
                    queue_rumble_data(5, 80);
#endif
                    sMainMenuButtons[buttonID]->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;
                    sSelectedButtonID = buttonID;
                }
                else {
                    if (sMainMenuTimer >= SCORE_TIMER) {

                        if (save_file_exists(buttonID - MENU_BUTTON_SCORE_MIN) == TRUE) {
                            play_sound(SOUND_MENU_CAMERA_ZOOM_IN, gGlobalSoundSource);
#if ENABLE_RUMBLE
                            queue_rumble_data(5, 80);
#endif
                            sMainMenuButtons[buttonID]->oMenuButtonState = MENU_BUTTON_STATE_GROWING;
                            sSelectedButtonID = buttonID;
                        }
                        else {

                            play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
#if ENABLE_RUMBLE
                            queue_rumble_data(5, 80);
#endif
                            sMainMenuButtons[buttonID]->oMenuButtonState =
                                MENU_BUTTON_STATE_ZOOM_IN_OUT;
                            if (sMainMenuTimer >= SCORE_TIMER) {
                                sFadeOutText = TRUE;
                                sMainMenuTimer = 0;
                            }
                        }
                    }
                }
                sCurrentMenuLevel = MENU_LAYER_SUBMENU;
                break;
            }
        }
    }
}

#undef SCORE_TIMER

void render_copy_menu_buttons(struct Object *copyButton) {

    if (save_file_exists(SAVE_FILE_A) == TRUE) {
        sMainMenuButtons[MENU_BUTTON_COPY_FILE_A] =
            spawn_object_rel_with_rot(copyButton, MODEL_MAIN_MENU_MARIO_SAVE_BUTTON, bhvMenuButton, 711,
                                      311, -100, 0, -0x8000, 0);
    } else {
        sMainMenuButtons[MENU_BUTTON_COPY_FILE_A] = spawn_object_rel_with_rot(
            copyButton, MODEL_MAIN_MENU_MARIO_NEW_BUTTON, bhvMenuButton, 711, 311, -100, 0, -0x8000, 0);
    }
    sMainMenuButtons[MENU_BUTTON_COPY_FILE_A]->oMenuButtonScale = 0.11111111f;

    if (save_file_exists(SAVE_FILE_B) == TRUE) {
        sMainMenuButtons[MENU_BUTTON_COPY_FILE_B] =
            spawn_object_rel_with_rot(copyButton, MODEL_MAIN_MENU_MARIO_SAVE_BUTTON, bhvMenuButton,
                                      -166, 311, -100, 0, -0x8000, 0);
    } else {
        sMainMenuButtons[MENU_BUTTON_COPY_FILE_B] =
            spawn_object_rel_with_rot(copyButton, MODEL_MAIN_MENU_MARIO_NEW_BUTTON, bhvMenuButton, -166,
                                      311, -100, 0, -0x8000, 0);
    }
    sMainMenuButtons[MENU_BUTTON_COPY_FILE_B]->oMenuButtonScale = 0.11111111f;

    if (save_file_exists(SAVE_FILE_C) == TRUE) {
        sMainMenuButtons[MENU_BUTTON_COPY_FILE_C] = spawn_object_rel_with_rot(
            copyButton, MODEL_MAIN_MENU_MARIO_SAVE_BUTTON, bhvMenuButton, 711, 0, -100, 0, -0x8000, 0);
    } else {
        sMainMenuButtons[MENU_BUTTON_COPY_FILE_C] = spawn_object_rel_with_rot(
            copyButton, MODEL_MAIN_MENU_MARIO_NEW_BUTTON, bhvMenuButton, 711, 0, -100, 0, -0x8000, 0);
    }
    sMainMenuButtons[MENU_BUTTON_COPY_FILE_C]->oMenuButtonScale = 0.11111111f;

    if (save_file_exists(SAVE_FILE_D) == TRUE) {
        sMainMenuButtons[MENU_BUTTON_COPY_FILE_D] = spawn_object_rel_with_rot(
            copyButton, MODEL_MAIN_MENU_MARIO_SAVE_BUTTON, bhvMenuButton, -166, 0, -100, 0, -0x8000, 0);
    } else {
        sMainMenuButtons[MENU_BUTTON_COPY_FILE_D] = spawn_object_rel_with_rot(
            copyButton, MODEL_MAIN_MENU_MARIO_NEW_BUTTON, bhvMenuButton, -166, 0, -100, 0, -0x8000, 0);
    }
    sMainMenuButtons[MENU_BUTTON_COPY_FILE_D]->oMenuButtonScale = 0.11111111f;

    sMainMenuButtons[MENU_BUTTON_COPY_RETURN] = spawn_object_rel_with_rot(
        copyButton, MODEL_MAIN_MENU_YELLOW_FILE_BUTTON, bhvMenuButton, 711, -388, -100, 0, -0x8000, 0);
    sMainMenuButtons[MENU_BUTTON_COPY_RETURN]->oMenuButtonScale = 0.11111111f;

    sMainMenuButtons[MENU_BUTTON_COPY_CHECK_SCORE] = spawn_object_rel_with_rot(
        copyButton, MODEL_MAIN_MENU_GREEN_SCORE_BUTTON, bhvMenuButton, 0, -388, -100, 0, -0x8000, 0);
    sMainMenuButtons[MENU_BUTTON_COPY_CHECK_SCORE]->oMenuButtonScale = 0.11111111f;

    sMainMenuButtons[MENU_BUTTON_COPY_ERASE_FILE] = spawn_object_rel_with_rot(
        copyButton, MODEL_MAIN_MENU_RED_ERASE_BUTTON, bhvMenuButton, -711, -388, -100, 0, -0x8000, 0);
    sMainMenuButtons[MENU_BUTTON_COPY_ERASE_FILE]->oMenuButtonScale = 0.11111111f;
}

#ifdef VERSION_EU
    #define BUZZ_TIMER 36
#else
    #define BUZZ_TIMER 21
#endif

void copy_action_file_button(struct Object *copyButton, s32 copyFileButtonID) {
    switch (copyButton->oMenuButtonActionPhase) {
        case COPY_PHASE_MAIN:
            if (sAllFilesExist == TRUE) {
                return;
            }
            if (save_file_exists(copyFileButtonID - MENU_BUTTON_COPY_MIN) == TRUE) {

                play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(5, 80);
#endif
                sMainMenuButtons[copyFileButtonID]->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN;
                sSelectedFileIndex = copyFileButtonID - MENU_BUTTON_COPY_MIN;
                copyButton->oMenuButtonActionPhase = COPY_PHASE_COPY_WHERE;
                sFadeOutText = TRUE;
                sMainMenuTimer = 0;
            } else {

                play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(5, 80);
#endif
                sMainMenuButtons[copyFileButtonID]->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;
                if (sMainMenuTimer >= BUZZ_TIMER) {
                    sFadeOutText = TRUE;
                    sMainMenuTimer = 0;
                }
            }
            break;
        case COPY_PHASE_COPY_WHERE:
            sMainMenuButtons[copyFileButtonID]->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;
            if (save_file_exists(copyFileButtonID - MENU_BUTTON_COPY_MIN) == FALSE) {

                play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(5, 80);
#endif
                copyButton->oMenuButtonActionPhase = COPY_PHASE_COPY_COMPLETE;
                sFadeOutText = TRUE;
                sMainMenuTimer = 0;
                save_file_copy(sSelectedFileIndex, copyFileButtonID - MENU_BUTTON_COPY_MIN);
                sMainMenuButtons[copyFileButtonID]->header.gfx.sharedChild =
                    gLoadedGraphNodes[MODEL_MAIN_MENU_MARIO_SAVE_BUTTON_FADE];
                sMainMenuButtons[copyFileButtonID - MENU_BUTTON_COPY_MIN]->header.gfx.sharedChild =
                    gLoadedGraphNodes[MODEL_MAIN_MENU_MARIO_SAVE_BUTTON_FADE];
            } else {

                if (copyFileButtonID == MENU_BUTTON_COPY_FILE_A + sSelectedFileIndex) {
                    play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
#if ENABLE_RUMBLE
                    queue_rumble_data(5, 80);
#endif
                    sMainMenuButtons[MENU_BUTTON_COPY_FILE_A + sSelectedFileIndex]->oMenuButtonState =
                        MENU_BUTTON_STATE_ZOOM_OUT;
                    copyButton->oMenuButtonActionPhase = COPY_PHASE_MAIN;
                    sFadeOutText = TRUE;
                    return;
                }
                if (sMainMenuTimer >= BUZZ_TIMER) {
                    sFadeOutText = TRUE;
                    sMainMenuTimer = 0;
                }
            }
            break;
    }
}

#ifdef VERSION_EU
    #define ACTION_TIMER      41
    #define MAIN_RETURN_TIMER 36
#else
    #define ACTION_TIMER      31
    #define MAIN_RETURN_TIMER 31
#endif

void check_copy_menu_clicked_buttons(struct Object *copyButton) {
    if (copyButton->oMenuButtonState == MENU_BUTTON_STATE_FULLSCREEN) {
        s32 buttonID;

        for (buttonID = MENU_BUTTON_COPY_MIN; buttonID < MENU_BUTTON_COPY_MAX; buttonID++) {
            s16 buttonX = sMainMenuButtons[buttonID]->oPosX;
            s16 buttonY = sMainMenuButtons[buttonID]->oPosY;

            if (check_clicked_button(buttonX, buttonY, 22.0f) == TRUE) {

                if (buttonID == MENU_BUTTON_COPY_RETURN || buttonID == MENU_BUTTON_COPY_CHECK_SCORE
                    || buttonID == MENU_BUTTON_COPY_ERASE_FILE) {
                    if (copyButton->oMenuButtonActionPhase == COPY_PHASE_MAIN) {
                        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
#if ENABLE_RUMBLE
                        queue_rumble_data(5, 80);
#endif
                        sMainMenuButtons[buttonID]->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;
                        sSelectedButtonID = buttonID;
                    }
                }
                else {

                    if (sMainMenuButtons[buttonID]->oMenuButtonState == MENU_BUTTON_STATE_DEFAULT
                        && sMainMenuTimer >= ACTION_TIMER) {
                        copy_action_file_button(copyButton, buttonID);
                    }
                }
                sCurrentMenuLevel = MENU_LAYER_SUBMENU;
                break;
            }
        }

        if (copyButton->oMenuButtonActionPhase == COPY_PHASE_COPY_COMPLETE
            && sMainMenuTimer >= MAIN_RETURN_TIMER) {
            copyButton->oMenuButtonActionPhase = COPY_PHASE_MAIN;
            sMainMenuButtons[MENU_BUTTON_COPY_MIN + sSelectedFileIndex]->oMenuButtonState =
                MENU_BUTTON_STATE_ZOOM_OUT;
        }
    }
}

void render_erase_menu_buttons(struct Object *eraseButton) {

    if (save_file_exists(SAVE_FILE_A) == TRUE) {
        sMainMenuButtons[MENU_BUTTON_ERASE_FILE_A] =
            spawn_object_rel_with_rot(eraseButton, MODEL_MAIN_MENU_MARIO_SAVE_BUTTON, bhvMenuButton,
                                      711, 311, -100, 0, -0x8000, 0);
    } else {
        sMainMenuButtons[MENU_BUTTON_ERASE_FILE_A] =
            spawn_object_rel_with_rot(eraseButton, MODEL_MAIN_MENU_MARIO_NEW_BUTTON, bhvMenuButton, 711,
                                      311, -100, 0, -0x8000, 0);
    }
    sMainMenuButtons[MENU_BUTTON_ERASE_FILE_A]->oMenuButtonScale = 0.11111111f;

    if (save_file_exists(SAVE_FILE_B) == TRUE) {
        sMainMenuButtons[MENU_BUTTON_ERASE_FILE_B] =
            spawn_object_rel_with_rot(eraseButton, MODEL_MAIN_MENU_MARIO_SAVE_BUTTON, bhvMenuButton,
                                      -166, 311, -100, 0, -0x8000, 0);
    } else {
        sMainMenuButtons[MENU_BUTTON_ERASE_FILE_B] =
            spawn_object_rel_with_rot(eraseButton, MODEL_MAIN_MENU_MARIO_NEW_BUTTON, bhvMenuButton,
                                      -166, 311, -100, 0, -0x8000, 0);
    }
    sMainMenuButtons[MENU_BUTTON_ERASE_FILE_B]->oMenuButtonScale = 0.11111111f;

    if (save_file_exists(SAVE_FILE_C) == TRUE) {
        sMainMenuButtons[MENU_BUTTON_ERASE_FILE_C] = spawn_object_rel_with_rot(
            eraseButton, MODEL_MAIN_MENU_MARIO_SAVE_BUTTON, bhvMenuButton, 711, 0, -100, 0, -0x8000, 0);
    } else {
        sMainMenuButtons[MENU_BUTTON_ERASE_FILE_C] = spawn_object_rel_with_rot(
            eraseButton, MODEL_MAIN_MENU_MARIO_NEW_BUTTON, bhvMenuButton, 711, 0, -100, 0, -0x8000, 0);
    }
    sMainMenuButtons[MENU_BUTTON_ERASE_FILE_C]->oMenuButtonScale = 0.11111111f;

    if (save_file_exists(SAVE_FILE_D) == TRUE) {
        sMainMenuButtons[MENU_BUTTON_ERASE_FILE_D] =
            spawn_object_rel_with_rot(eraseButton, MODEL_MAIN_MENU_MARIO_SAVE_BUTTON, bhvMenuButton,
                                      -166, 0, -100, 0, -0x8000, 0);
    } else {
        sMainMenuButtons[MENU_BUTTON_ERASE_FILE_D] = spawn_object_rel_with_rot(
            eraseButton, MODEL_MAIN_MENU_MARIO_NEW_BUTTON, bhvMenuButton, -166, 0, -100, 0, -0x8000, 0);
    }
    sMainMenuButtons[MENU_BUTTON_ERASE_FILE_D]->oMenuButtonScale = 0.11111111f;

    sMainMenuButtons[MENU_BUTTON_ERASE_RETURN] = spawn_object_rel_with_rot(
        eraseButton, MODEL_MAIN_MENU_YELLOW_FILE_BUTTON, bhvMenuButton, 711, -388, -100, 0, -0x8000, 0);
    sMainMenuButtons[MENU_BUTTON_ERASE_RETURN]->oMenuButtonScale = 0.11111111f;

    sMainMenuButtons[MENU_BUTTON_ERASE_CHECK_SCORE] = spawn_object_rel_with_rot(
        eraseButton, MODEL_MAIN_MENU_GREEN_SCORE_BUTTON, bhvMenuButton, 0, -388, -100, 0, -0x8000, 0);
    sMainMenuButtons[MENU_BUTTON_ERASE_CHECK_SCORE]->oMenuButtonScale = 0.11111111f;

    sMainMenuButtons[MENU_BUTTON_ERASE_COPY_FILE] = spawn_object_rel_with_rot(
        eraseButton, MODEL_MAIN_MENU_BLUE_COPY_BUTTON, bhvMenuButton, -711, -388, -100, 0, -0x8000, 0);
    sMainMenuButtons[MENU_BUTTON_ERASE_COPY_FILE]->oMenuButtonScale = 0.11111111f;
}

void erase_action_file_button(struct Object *eraseButton, s32 eraseFileButtonID) {
    switch (eraseButton->oMenuButtonActionPhase) {
        case ERASE_PHASE_MAIN:
            if (save_file_exists(eraseFileButtonID - MENU_BUTTON_ERASE_MIN) == TRUE) {

                play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(5, 80);
#endif
                sMainMenuButtons[eraseFileButtonID]->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN;
                sSelectedFileIndex = eraseFileButtonID - MENU_BUTTON_ERASE_MIN;
                eraseButton->oMenuButtonActionPhase = ERASE_PHASE_PROMPT;
                sFadeOutText = TRUE;
                sMainMenuTimer = 0;
            } else {

                play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(5, 80);
#endif
                sMainMenuButtons[eraseFileButtonID]->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;

                if (sMainMenuTimer >= BUZZ_TIMER) {
                    sFadeOutText = TRUE;
                    sMainMenuTimer = 0;
                }
            }
            break;
        case ERASE_PHASE_PROMPT:
            if (eraseFileButtonID == MENU_BUTTON_ERASE_MIN + sSelectedFileIndex) {

                play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(5, 80);
#endif
                sMainMenuButtons[MENU_BUTTON_ERASE_MIN + sSelectedFileIndex]->oMenuButtonState =
                    MENU_BUTTON_STATE_ZOOM_OUT;
                eraseButton->oMenuButtonActionPhase = ERASE_PHASE_MAIN;
                sFadeOutText = TRUE;
            }
            break;
    }
}

#undef BUZZ_TIMER

void check_erase_menu_clicked_buttons(struct Object *eraseButton) {
    if (eraseButton->oMenuButtonState == MENU_BUTTON_STATE_FULLSCREEN) {
        s32 buttonID;

        for (buttonID = MENU_BUTTON_ERASE_MIN; buttonID < MENU_BUTTON_ERASE_MAX; buttonID++) {
            s16 buttonX = sMainMenuButtons[buttonID]->oPosX;
            s16 buttonY = sMainMenuButtons[buttonID]->oPosY;

            if (check_clicked_button(buttonX, buttonY, 22.0f) == TRUE) {

                if (buttonID == MENU_BUTTON_ERASE_RETURN || buttonID == MENU_BUTTON_ERASE_CHECK_SCORE
                    || buttonID == MENU_BUTTON_ERASE_COPY_FILE) {
                    if (eraseButton->oMenuButtonActionPhase == ERASE_PHASE_MAIN) {
                        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
#if ENABLE_RUMBLE
                        queue_rumble_data(5, 80);
#endif
                        sMainMenuButtons[buttonID]->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;
                        sSelectedButtonID = buttonID;
                    }
                }
                else {

                    if (sMainMenuTimer >= ACTION_TIMER) {
                        erase_action_file_button(eraseButton, buttonID);
                    }
                }
                sCurrentMenuLevel = MENU_LAYER_SUBMENU;
                break;
            }
        }

        if (eraseButton->oMenuButtonActionPhase == ERASE_PHASE_MARIO_ERASED
            && sMainMenuTimer >= MAIN_RETURN_TIMER) {
            eraseButton->oMenuButtonActionPhase = ERASE_PHASE_MAIN;
            sMainMenuButtons[MENU_BUTTON_ERASE_MIN + sSelectedFileIndex]->oMenuButtonState =
                MENU_BUTTON_STATE_ZOOM_OUT;
        }
    }
}

#undef ACTION_TIMER
#undef MAIN_RETURN_TIMER

#ifdef VERSION_EU
    #define SOUND_BUTTON_Y 388
#else
    #define SOUND_BUTTON_Y 0
#endif

void render_sound_mode_menu_buttons(struct Object *soundModeButton) {

    sMainMenuButtons[MENU_BUTTON_STEREO] = spawn_object_rel_with_rot(
        soundModeButton, MODEL_MAIN_MENU_GENERIC_BUTTON, bhvMenuButton, 533, SOUND_BUTTON_Y, -100, 0, -0x8000, 0);
    sMainMenuButtons[MENU_BUTTON_STEREO]->oMenuButtonScale = 0.11111111f;

    sMainMenuButtons[MENU_BUTTON_MONO] = spawn_object_rel_with_rot(
        soundModeButton, MODEL_MAIN_MENU_GENERIC_BUTTON, bhvMenuButton, 0, SOUND_BUTTON_Y, -100, 0, -0x8000, 0);
    sMainMenuButtons[MENU_BUTTON_MONO]->oMenuButtonScale = 0.11111111f;

    sMainMenuButtons[MENU_BUTTON_HEADSET] = spawn_object_rel_with_rot(
        soundModeButton, MODEL_MAIN_MENU_GENERIC_BUTTON, bhvMenuButton, -533, SOUND_BUTTON_Y, -100, 0, -0x8000, 0);
    sMainMenuButtons[MENU_BUTTON_HEADSET]->oMenuButtonScale = 0.11111111f;

#ifdef VERSION_EU

    sMainMenuButtons[MENU_BUTTON_LANGUAGE_ENGLISH] = spawn_object_rel_with_rot(
        soundModeButton, MODEL_MAIN_MENU_GENERIC_BUTTON, bhvMenuButton, 533, -111, -100, 0, -0x8000, 0);
    sMainMenuButtons[MENU_BUTTON_LANGUAGE_ENGLISH]->oMenuButtonScale = 0.11111111f;

    sMainMenuButtons[MENU_BUTTON_LANGUAGE_FRENCH] = spawn_object_rel_with_rot(
        soundModeButton, MODEL_MAIN_MENU_GENERIC_BUTTON, bhvMenuButton, 0, -111, -100, 0, -0x8000, 0);
    sMainMenuButtons[MENU_BUTTON_LANGUAGE_FRENCH]->oMenuButtonScale = 0.11111111f;

    sMainMenuButtons[MENU_BUTTON_LANGUAGE_GERMAN] = spawn_object_rel_with_rot(
        soundModeButton, MODEL_MAIN_MENU_GENERIC_BUTTON, bhvMenuButton, -533, -111, -100, 0, -0x8000, 0);
    sMainMenuButtons[MENU_BUTTON_LANGUAGE_GERMAN]->oMenuButtonScale = 0.11111111f;

    sMainMenuButtons[MENU_BUTTON_LANGUAGE_RETURN] = spawn_object_rel_with_rot(
        soundModeButton, MODEL_MAIN_MENU_YELLOW_FILE_BUTTON, bhvMenuButton, 0, -533, -100, 0, -0x8000, 0);
    sMainMenuButtons[MENU_BUTTON_LANGUAGE_RETURN]->oMenuButtonScale = 0.11111111f;
#else

    sMainMenuButtons[MENU_BUTTON_OPTION_MIN + sSoundMode]->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN;
#endif
}

#undef SOUND_BUTTON_Y

void check_sound_mode_menu_clicked_buttons(struct Object *soundModeButton) {
    if (soundModeButton->oMenuButtonState == MENU_BUTTON_STATE_FULLSCREEN) {
        s32 buttonID;

        for (buttonID = MENU_BUTTON_OPTION_MIN; buttonID < MENU_BUTTON_OPTION_MAX; buttonID++) {
            s16 buttonX = sMainMenuButtons[buttonID]->oPosX;
            s16 buttonY = sMainMenuButtons[buttonID]->oPosY;

            if (check_clicked_button(buttonX, buttonY, 22.0f) == TRUE) {

                if (buttonID == MENU_BUTTON_STEREO || buttonID == MENU_BUTTON_MONO
                    || buttonID == MENU_BUTTON_HEADSET) {
                    if (soundModeButton->oMenuButtonActionPhase == SOUND_MODE_PHASE_MAIN) {
                        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
#if ENABLE_RUMBLE
                        queue_rumble_data(5, 80);
#endif
                        sMainMenuButtons[buttonID]->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;
#ifndef VERSION_EU

                        sSelectedButtonID = buttonID;
#endif
                        sSoundMode = buttonID - MENU_BUTTON_OPTION_MIN;
                        save_file_set_sound_mode(sSoundMode);
                    }
                }
#ifdef VERSION_EU

                if (buttonID == MENU_BUTTON_LANGUAGE_ENGLISH || buttonID == MENU_BUTTON_LANGUAGE_FRENCH
                         || buttonID == MENU_BUTTON_LANGUAGE_GERMAN) {
                    if (soundModeButton->oMenuButtonActionPhase == SOUND_MODE_PHASE_MAIN) {
                        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
                        sMainMenuButtons[buttonID]->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;
                        sLanguageMode = buttonID - MENU_BUTTON_LANGUAGE_MIN;
                        eu_set_language(sLanguageMode);
                    }
                }

                if (buttonID == MENU_BUTTON_LANGUAGE_RETURN) {
                    play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
                    sMainMenuButtons[buttonID]->oMenuButtonState = MENU_BUTTON_STATE_ZOOM_IN_OUT;
                    sSelectedButtonID = buttonID;
                }
#endif
                sCurrentMenuLevel = MENU_LAYER_SUBMENU;

                break;
            }
        }
    }
}

#ifdef TARGET_NDS

void nds_force_select_file(s32 fileNum) {
    sSelectedFileNum = fileNum;
}
void nds_netplay_host_picked(s32 fileNum);
s32 nds_netplay_block_local_pick(void);
#endif

void load_main_menu_save_file(struct Object *fileButton, s32 fileNum) {
    if (fileButton->oMenuButtonState == MENU_BUTTON_STATE_FULLSCREEN) {
#ifdef TARGET_NDS

        if (nds_netplay_block_local_pick()) {
            return;
        }
        nds_netplay_host_picked(fileNum);
#endif
        sSelectedFileNum = fileNum;
    }
}

void return_to_main_menu(s16 prevMenuButtonID, struct Object *sourceButton) {
    s32 buttonID;

    if (sourceButton->oMenuButtonState == MENU_BUTTON_STATE_DEFAULT
        && sMainMenuButtons[prevMenuButtonID]->oMenuButtonState == MENU_BUTTON_STATE_FULLSCREEN) {
        play_sound(SOUND_MENU_CAMERA_ZOOM_OUT, gGlobalSoundSource);
        sMainMenuButtons[prevMenuButtonID]->oMenuButtonState = MENU_BUTTON_STATE_SHRINKING;
        sCurrentMenuLevel = MENU_LAYER_MAIN;
    }

    if (sMainMenuButtons[prevMenuButtonID]->oMenuButtonState == MENU_BUTTON_STATE_DEFAULT) {
        sSelectedButtonID = MENU_BUTTON_NONE;

        if (prevMenuButtonID == MENU_BUTTON_SCORE) {
            for (buttonID = MENU_BUTTON_SCORE_MIN; buttonID < MENU_BUTTON_SCORE_MAX; buttonID++) {
                mark_obj_for_deletion(sMainMenuButtons[buttonID]);
            }
        }
        if (prevMenuButtonID == MENU_BUTTON_COPY) {
            for (buttonID = MENU_BUTTON_COPY_MIN; buttonID < MENU_BUTTON_COPY_MAX; buttonID++) {
                mark_obj_for_deletion(sMainMenuButtons[buttonID]);
            }
        }
        if (prevMenuButtonID == MENU_BUTTON_ERASE) {
            for (buttonID = MENU_BUTTON_ERASE_MIN; buttonID < MENU_BUTTON_ERASE_MAX; buttonID++) {
                mark_obj_for_deletion(sMainMenuButtons[buttonID]);
            }
        }
        if (prevMenuButtonID == MENU_BUTTON_SOUND_MODE) {
            for (buttonID = MENU_BUTTON_OPTION_MIN; buttonID < MENU_BUTTON_OPTION_MAX; buttonID++) {
                mark_obj_for_deletion(sMainMenuButtons[buttonID]);
            }
        }
    }
}

void load_score_menu_from_submenu(s16 prevMenuButtonID, struct Object *sourceButton) {
    s32 buttonID;

    if (sourceButton->oMenuButtonState == MENU_BUTTON_STATE_DEFAULT
        && sMainMenuButtons[prevMenuButtonID]->oMenuButtonState == MENU_BUTTON_STATE_FULLSCREEN) {
        play_sound(SOUND_MENU_CAMERA_ZOOM_OUT, gGlobalSoundSource);
        sMainMenuButtons[prevMenuButtonID]->oMenuButtonState = MENU_BUTTON_STATE_SHRINKING;
        sCurrentMenuLevel = MENU_LAYER_MAIN;
    }

    if (sMainMenuButtons[prevMenuButtonID]->oMenuButtonState == MENU_BUTTON_STATE_DEFAULT) {

        if (prevMenuButtonID == MENU_BUTTON_SCORE) {
            for (buttonID = MENU_BUTTON_SCORE_MIN; buttonID < MENU_BUTTON_SCORE_MAX; buttonID++) {
                mark_obj_for_deletion(sMainMenuButtons[buttonID]);
            }
        }
        if (prevMenuButtonID == MENU_BUTTON_COPY) {
            for (buttonID = MENU_BUTTON_COPY_MIN; buttonID < MENU_BUTTON_COPY_MAX; buttonID++) {
                mark_obj_for_deletion(sMainMenuButtons[buttonID]);
            }
        }
        if (prevMenuButtonID == MENU_BUTTON_ERASE) {
            for (buttonID = MENU_BUTTON_ERASE_MIN; buttonID < MENU_BUTTON_ERASE_MAX; buttonID++) {
                mark_obj_for_deletion(sMainMenuButtons[buttonID]);
            }
        }

        sSelectedButtonID = MENU_BUTTON_SCORE;
        play_sound(SOUND_MENU_CAMERA_ZOOM_IN, gGlobalSoundSource);
        sMainMenuButtons[MENU_BUTTON_SCORE]->oMenuButtonState = MENU_BUTTON_STATE_GROWING;
        render_score_menu_buttons(sMainMenuButtons[MENU_BUTTON_SCORE]);
    }
}

void load_copy_menu_from_submenu(s16 prevMenuButtonID, struct Object *sourceButton) {
    s32 buttonID;

    if (sourceButton->oMenuButtonState == MENU_BUTTON_STATE_DEFAULT
        && sMainMenuButtons[prevMenuButtonID]->oMenuButtonState == MENU_BUTTON_STATE_FULLSCREEN) {
        play_sound(SOUND_MENU_CAMERA_ZOOM_OUT, gGlobalSoundSource);
        sMainMenuButtons[prevMenuButtonID]->oMenuButtonState = MENU_BUTTON_STATE_SHRINKING;
        sCurrentMenuLevel = MENU_LAYER_MAIN;
    }

    if (sMainMenuButtons[prevMenuButtonID]->oMenuButtonState == MENU_BUTTON_STATE_DEFAULT) {

        if (prevMenuButtonID == MENU_BUTTON_SCORE) {
            for (buttonID = MENU_BUTTON_SCORE_MIN; buttonID < MENU_BUTTON_SCORE_MAX; buttonID++) {
                mark_obj_for_deletion(sMainMenuButtons[buttonID]);
            }
        }

        if (prevMenuButtonID == MENU_BUTTON_COPY) {
            for (buttonID = MENU_BUTTON_COPY_MIN; buttonID < MENU_BUTTON_COPY_MAX; buttonID++) {
                mark_obj_for_deletion(sMainMenuButtons[buttonID]);
            }
        }
        if (prevMenuButtonID == MENU_BUTTON_ERASE) {
            for (buttonID = MENU_BUTTON_ERASE_MIN; buttonID < MENU_BUTTON_ERASE_MAX; buttonID++) {
                mark_obj_for_deletion(sMainMenuButtons[buttonID]);
            }
        }

        sSelectedButtonID = MENU_BUTTON_COPY;
        play_sound(SOUND_MENU_CAMERA_ZOOM_IN, gGlobalSoundSource);
        sMainMenuButtons[MENU_BUTTON_COPY]->oMenuButtonState = MENU_BUTTON_STATE_GROWING;
        render_copy_menu_buttons(sMainMenuButtons[MENU_BUTTON_COPY]);
    }
}

void load_erase_menu_from_submenu(s16 prevMenuButtonID, struct Object *sourceButton) {
    s32 buttonID;

    if (sourceButton->oMenuButtonState == MENU_BUTTON_STATE_DEFAULT
        && sMainMenuButtons[prevMenuButtonID]->oMenuButtonState == MENU_BUTTON_STATE_FULLSCREEN) {
        play_sound(SOUND_MENU_CAMERA_ZOOM_OUT, gGlobalSoundSource);
        sMainMenuButtons[prevMenuButtonID]->oMenuButtonState = MENU_BUTTON_STATE_SHRINKING;
        sCurrentMenuLevel = MENU_LAYER_MAIN;
    }

    if (sMainMenuButtons[prevMenuButtonID]->oMenuButtonState == MENU_BUTTON_STATE_DEFAULT) {

        if (prevMenuButtonID == MENU_BUTTON_SCORE) {
            for (buttonID = MENU_BUTTON_SCORE_MIN; buttonID < MENU_BUTTON_SCORE_MAX; buttonID++) {
                mark_obj_for_deletion(sMainMenuButtons[buttonID]);
            }
        }
        if (prevMenuButtonID == MENU_BUTTON_COPY) {
            for (buttonID = MENU_BUTTON_COPY_MIN; buttonID < MENU_BUTTON_COPY_MAX; buttonID++) {
                mark_obj_for_deletion(sMainMenuButtons[buttonID]);
            }
        }

        if (prevMenuButtonID == MENU_BUTTON_ERASE) {
            for (buttonID = MENU_BUTTON_ERASE_MIN; buttonID < MENU_BUTTON_ERASE_MAX; buttonID++) {
                mark_obj_for_deletion(sMainMenuButtons[buttonID]);
            }
        }

        sSelectedButtonID = MENU_BUTTON_ERASE;
        play_sound(SOUND_MENU_CAMERA_ZOOM_IN, gGlobalSoundSource);
        sMainMenuButtons[MENU_BUTTON_ERASE]->oMenuButtonState = MENU_BUTTON_STATE_GROWING;
        render_erase_menu_buttons(sMainMenuButtons[MENU_BUTTON_ERASE]);
    }
}

void bhv_menu_button_manager_init(void) {

    if (save_file_exists(SAVE_FILE_A) == TRUE) {
        sMainMenuButtons[MENU_BUTTON_PLAY_FILE_A] =
            spawn_object_rel_with_rot(gCurrentObject, MODEL_MAIN_MENU_MARIO_SAVE_BUTTON_FADE,
                                      bhvMenuButton, -6400, 2800, 0, 0, 0, 0);
    } else {
        sMainMenuButtons[MENU_BUTTON_PLAY_FILE_A] =
            spawn_object_rel_with_rot(gCurrentObject, MODEL_MAIN_MENU_MARIO_NEW_BUTTON_FADE,
                                      bhvMenuButton, -6400, 2800, 0, 0, 0, 0);
    }
    sMainMenuButtons[MENU_BUTTON_PLAY_FILE_A]->oMenuButtonScale = 1.0f;

    if (save_file_exists(SAVE_FILE_B) == TRUE) {
        sMainMenuButtons[MENU_BUTTON_PLAY_FILE_B] =
            spawn_object_rel_with_rot(gCurrentObject, MODEL_MAIN_MENU_MARIO_SAVE_BUTTON_FADE,
                                      bhvMenuButton, 1500, 2800, 0, 0, 0, 0);
    } else {
        sMainMenuButtons[MENU_BUTTON_PLAY_FILE_B] =
            spawn_object_rel_with_rot(gCurrentObject, MODEL_MAIN_MENU_MARIO_NEW_BUTTON_FADE,
                                      bhvMenuButton, 1500, 2800, 0, 0, 0, 0);
    }
    sMainMenuButtons[MENU_BUTTON_PLAY_FILE_B]->oMenuButtonScale = 1.0f;

    if (save_file_exists(SAVE_FILE_C) == TRUE) {
        sMainMenuButtons[MENU_BUTTON_PLAY_FILE_C] =
            spawn_object_rel_with_rot(gCurrentObject, MODEL_MAIN_MENU_MARIO_SAVE_BUTTON_FADE,
                                      bhvMenuButton, -6400, 0, 0, 0, 0, 0);
    } else {
        sMainMenuButtons[MENU_BUTTON_PLAY_FILE_C] = spawn_object_rel_with_rot(
            gCurrentObject, MODEL_MAIN_MENU_MARIO_NEW_BUTTON_FADE, bhvMenuButton, -6400, 0, 0, 0, 0, 0);
    }
    sMainMenuButtons[MENU_BUTTON_PLAY_FILE_C]->oMenuButtonScale = 1.0f;

    if (save_file_exists(SAVE_FILE_D) == TRUE) {
        sMainMenuButtons[MENU_BUTTON_PLAY_FILE_D] = spawn_object_rel_with_rot(
            gCurrentObject, MODEL_MAIN_MENU_MARIO_SAVE_BUTTON_FADE, bhvMenuButton, 1500, 0, 0, 0, 0, 0);
    } else {
        sMainMenuButtons[MENU_BUTTON_PLAY_FILE_D] = spawn_object_rel_with_rot(
            gCurrentObject, MODEL_MAIN_MENU_MARIO_NEW_BUTTON_FADE, bhvMenuButton, 1500, 0, 0, 0, 0, 0);
    }
    sMainMenuButtons[MENU_BUTTON_PLAY_FILE_D]->oMenuButtonScale = 1.0f;

    sMainMenuButtons[MENU_BUTTON_SCORE] = spawn_object_rel_with_rot(
        gCurrentObject, MODEL_MAIN_MENU_GREEN_SCORE_BUTTON, bhvMenuButton, -6400, -3500, 0, 0, 0, 0);
    sMainMenuButtons[MENU_BUTTON_SCORE]->oMenuButtonScale = 1.0f;

    sMainMenuButtons[MENU_BUTTON_COPY] = spawn_object_rel_with_rot(
        gCurrentObject, MODEL_MAIN_MENU_BLUE_COPY_BUTTON, bhvMenuButton, -2134, -3500, 0, 0, 0, 0);
    sMainMenuButtons[MENU_BUTTON_COPY]->oMenuButtonScale = 1.0f;

    sMainMenuButtons[MENU_BUTTON_ERASE] = spawn_object_rel_with_rot(
        gCurrentObject, MODEL_MAIN_MENU_RED_ERASE_BUTTON, bhvMenuButton, 2134, -3500, 0, 0, 0, 0);
    sMainMenuButtons[MENU_BUTTON_ERASE]->oMenuButtonScale = 1.0f;

    sMainMenuButtons[MENU_BUTTON_SOUND_MODE] = spawn_object_rel_with_rot(
        gCurrentObject, MODEL_MAIN_MENU_PURPLE_SOUND_BUTTON, bhvMenuButton, 6400, -3500, 0, 0, 0, 0);
    sMainMenuButtons[MENU_BUTTON_SOUND_MODE]->oMenuButtonScale = 1.0f;

    sTextBaseAlpha = 0;
}

#ifdef VERSION_JP
    #define SAVE_FILE_SOUND SOUND_MENU_STAR_SOUND
#else
    #define SAVE_FILE_SOUND SOUND_MENU_STAR_SOUND_OKEY_DOKEY
#endif

void check_main_menu_clicked_buttons(void) {
#ifdef VERSION_EU
    if (sMainMenuTimer >= 5) {
#endif

        if (check_clicked_button(sMainMenuButtons[MENU_BUTTON_SOUND_MODE]->oPosX,
                                sMainMenuButtons[MENU_BUTTON_SOUND_MODE]->oPosY, 200.0f) == TRUE) {
            sMainMenuButtons[MENU_BUTTON_SOUND_MODE]->oMenuButtonState = MENU_BUTTON_STATE_GROWING;
            sSelectedButtonID = MENU_BUTTON_SOUND_MODE;
        } else {

            s8 buttonID;

            for (buttonID = MENU_BUTTON_MAIN_MIN; buttonID < MENU_BUTTON_MAIN_MAX; buttonID++) {
                s16 buttonX = sMainMenuButtons[buttonID]->oPosX;
                s16 buttonY = sMainMenuButtons[buttonID]->oPosY;

                if (check_clicked_button(buttonX, buttonY, 200.0f) == TRUE) {

                    sMainMenuButtons[buttonID]->oMenuButtonState = MENU_BUTTON_STATE_GROWING;
                    sSelectedButtonID = buttonID;
                    break;
                }
            }
        }
#ifdef VERSION_EU

        if (sOpenLangSettings == TRUE) {
            sMainMenuButtons[MENU_BUTTON_SOUND_MODE]->oMenuButtonState = MENU_BUTTON_STATE_GROWING;
            sSelectedButtonID = MENU_BUTTON_SOUND_MODE;
            sOpenLangSettings = FALSE;
        }
#endif

        switch (sSelectedButtonID) {
            case MENU_BUTTON_PLAY_FILE_A:
                play_sound(SAVE_FILE_SOUND, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(60, 70);
                func_sh_8024C89C(1);
#endif
                break;
            case MENU_BUTTON_PLAY_FILE_B:
                play_sound(SAVE_FILE_SOUND, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(60, 70);
                func_sh_8024C89C(1);
#endif
                break;
            case MENU_BUTTON_PLAY_FILE_C:
                play_sound(SAVE_FILE_SOUND, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(60, 70);
                func_sh_8024C89C(1);
#endif
                break;
            case MENU_BUTTON_PLAY_FILE_D:
                play_sound(SAVE_FILE_SOUND, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(60, 70);
                func_sh_8024C89C(1);
#endif
                break;

            case MENU_BUTTON_SCORE:
                play_sound(SOUND_MENU_CAMERA_ZOOM_IN, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(5, 80);
#endif
                render_score_menu_buttons(sMainMenuButtons[MENU_BUTTON_SCORE]);
                break;
            case MENU_BUTTON_COPY:
                play_sound(SOUND_MENU_CAMERA_ZOOM_IN, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(5, 80);
#endif
                render_copy_menu_buttons(sMainMenuButtons[MENU_BUTTON_COPY]);
                break;
            case MENU_BUTTON_ERASE:
                play_sound(SOUND_MENU_CAMERA_ZOOM_IN, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(5, 80);
#endif
                render_erase_menu_buttons(sMainMenuButtons[MENU_BUTTON_ERASE]);
                break;
            case MENU_BUTTON_SOUND_MODE:
                play_sound(SOUND_MENU_CAMERA_ZOOM_IN, gGlobalSoundSource);
#if ENABLE_RUMBLE
                queue_rumble_data(5, 80);
#endif
                render_sound_mode_menu_buttons(sMainMenuButtons[MENU_BUTTON_SOUND_MODE]);
                break;
        }
#ifdef VERSION_EU
    }
#endif
}

#undef SAVE_FILE_SOUND

#ifdef TARGET_NDS

extern volatile unsigned char gNdsNetState;
extern volatile unsigned char gNdsNetIsHost;
extern volatile unsigned char gNdsNetRequest;
void nds_make_120_star_file(s32 fileIndex);

static u8 textNdsMpPrompt[]  = { TEXT_NDS_MP_PROMPT };
static u8 textNdsSearching[] = { TEXT_NDS_SEARCHING };
static u8 textNdsHostPick[]  = { TEXT_NDS_HOST_PICK };
static u8 textNdsGuestWait[] = { TEXT_NDS_GUEST_WAIT };
static u8 textNds120Pick[]   = { TEXT_NDS_120_PICK };
static u8 textNds120Done[]   = { TEXT_NDS_120_DONE };
static u8 textNds120New[]    = { TEXT_NDS_120_NEW };
static u8 textNds120Ok[]     = { TEXT_NDS_120_OK };
static u8 textNds120No[]     = { TEXT_NDS_120_NO };
static u8 textNdsGuestBig[]  = { TEXT_NDS_GUEST_BIG };

static u8 sNdsMpMode;
static u16 sNds120Msg;
static u16 sNds120Bad;

static s32 nds_cursor_file_num(void) {
    s32 col = (sCursorPos[0] < -35.0f) ? 0 : 1;
    s32 row = (sCursorPos[1] >  25.0f) ? 0 : 1;
    return 1 + row * 2 + col;
}

static void nds_home_menu_update(void) {
    u16 pressed = gPlayer1Controller->buttonPressed | gPlayer3Controller->buttonPressed;

    if ((pressed & U_CBUTTONS) && !sNdsMpMode) {
        sNdsMpMode = 1;
        gNdsNetRequest = 1;
        play_sound(SOUND_MENU_CAMERA_ZOOM_IN, gGlobalSoundSource);
    }

    if (pressed & D_CBUTTONS) {
        s32 file = nds_cursor_file_num() - 1;
        if (!save_file_exists(file)) {
            nds_make_120_star_file(file);
            sNds120Msg = 120;
            sNds120Bad = 0;
            play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
        } else {
            sNds120Bad = 90;
            sNds120Msg = 0;
            play_sound(SOUND_MENU_CAMERA_BUZZ, gGlobalSoundSource);
        }
    }

    if (sNds120Bad > 0) {
        sNds120Bad--;
    }
    if (sNds120Msg > 0) {
        sNds120Msg--;
    }
}

#define NDS_BIG_Y 218

static void nds_home_menu_render(void) {
    const u8 *big;
    s16 bx;

    if (sNds120Msg > 0) {
        big = textNds120Ok;    bx = 100;
    } else if (sNds120Bad > 0) {
        big = textNds120No;    bx = 58;
    } else if (gNdsNetState == 2 && !gNdsNetIsHost) {
        big = textNdsGuestBig; bx = 42;
    } else {
        big = textNds120New;   bx = 34;
    }
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    print_hud_lut_string(HUD_LUT_GLOBAL, bx, NDS_BIG_Y, big);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
}
#endif

void bhv_menu_button_manager_loop(void) {
    switch (sSelectedButtonID) {
        case MENU_BUTTON_NONE:
#ifdef TARGET_NDS
            nds_home_menu_update();
#endif
            check_main_menu_clicked_buttons();
            break;
        case MENU_BUTTON_PLAY_FILE_A:
            load_main_menu_save_file(sMainMenuButtons[MENU_BUTTON_PLAY_FILE_A], 1);
            break;
        case MENU_BUTTON_PLAY_FILE_B:
            load_main_menu_save_file(sMainMenuButtons[MENU_BUTTON_PLAY_FILE_B], 2);
            break;
        case MENU_BUTTON_PLAY_FILE_C:
            load_main_menu_save_file(sMainMenuButtons[MENU_BUTTON_PLAY_FILE_C], 3);
            break;
        case MENU_BUTTON_PLAY_FILE_D:
            load_main_menu_save_file(sMainMenuButtons[MENU_BUTTON_PLAY_FILE_D], 4);
            break;
        case MENU_BUTTON_SCORE:
            check_score_menu_clicked_buttons(sMainMenuButtons[MENU_BUTTON_SCORE]);
            break;
        case MENU_BUTTON_COPY:
            check_copy_menu_clicked_buttons(sMainMenuButtons[MENU_BUTTON_COPY]);
            break;
        case MENU_BUTTON_ERASE:
            check_erase_menu_clicked_buttons(sMainMenuButtons[MENU_BUTTON_ERASE]);
            break;

        case MENU_BUTTON_SCORE_FILE_A:
            exit_score_file_to_score_menu(sMainMenuButtons[MENU_BUTTON_SCORE_FILE_A], MENU_BUTTON_SCORE);
            break;
        case MENU_BUTTON_SCORE_FILE_B:
            exit_score_file_to_score_menu(sMainMenuButtons[MENU_BUTTON_SCORE_FILE_B], MENU_BUTTON_SCORE);
            break;
        case MENU_BUTTON_SCORE_FILE_C:
            exit_score_file_to_score_menu(sMainMenuButtons[MENU_BUTTON_SCORE_FILE_C], MENU_BUTTON_SCORE);
            break;
        case MENU_BUTTON_SCORE_FILE_D:
            exit_score_file_to_score_menu(sMainMenuButtons[MENU_BUTTON_SCORE_FILE_D], MENU_BUTTON_SCORE);
            break;
        case MENU_BUTTON_SCORE_RETURN:
            return_to_main_menu(MENU_BUTTON_SCORE, sMainMenuButtons[MENU_BUTTON_SCORE_RETURN]);
            break;
        case MENU_BUTTON_SCORE_COPY_FILE:
            load_copy_menu_from_submenu(MENU_BUTTON_SCORE,
                                        sMainMenuButtons[MENU_BUTTON_SCORE_COPY_FILE]);
            break;
        case MENU_BUTTON_SCORE_ERASE_FILE:
            load_erase_menu_from_submenu(MENU_BUTTON_SCORE,
                                         sMainMenuButtons[MENU_BUTTON_SCORE_ERASE_FILE]);
            break;

        case MENU_BUTTON_COPY_FILE_A:
            break;
        case MENU_BUTTON_COPY_FILE_B:
            break;
        case MENU_BUTTON_COPY_FILE_C:
            break;
        case MENU_BUTTON_COPY_FILE_D:
            break;
        case MENU_BUTTON_COPY_RETURN:
            return_to_main_menu(MENU_BUTTON_COPY, sMainMenuButtons[MENU_BUTTON_COPY_RETURN]);
            break;
        case MENU_BUTTON_COPY_CHECK_SCORE:
            load_score_menu_from_submenu(MENU_BUTTON_COPY,
                                         sMainMenuButtons[MENU_BUTTON_COPY_CHECK_SCORE]);
            break;
        case MENU_BUTTON_COPY_ERASE_FILE:
            load_erase_menu_from_submenu(MENU_BUTTON_COPY,
                                         sMainMenuButtons[MENU_BUTTON_COPY_ERASE_FILE]);
            break;

        case MENU_BUTTON_ERASE_FILE_A:
            break;
        case MENU_BUTTON_ERASE_FILE_B:
            break;
        case MENU_BUTTON_ERASE_FILE_C:
            break;
        case MENU_BUTTON_ERASE_FILE_D:
            break;
        case MENU_BUTTON_ERASE_RETURN:
            return_to_main_menu(MENU_BUTTON_ERASE, sMainMenuButtons[MENU_BUTTON_ERASE_RETURN]);
            break;
        case MENU_BUTTON_ERASE_CHECK_SCORE:
            load_score_menu_from_submenu(MENU_BUTTON_ERASE,
                                         sMainMenuButtons[MENU_BUTTON_ERASE_CHECK_SCORE]);
            break;
        case MENU_BUTTON_ERASE_COPY_FILE:
            load_copy_menu_from_submenu(MENU_BUTTON_ERASE,
                                        sMainMenuButtons[MENU_BUTTON_ERASE_COPY_FILE]);
            break;

        case MENU_BUTTON_SOUND_MODE:
            check_sound_mode_menu_clicked_buttons(sMainMenuButtons[MENU_BUTTON_SOUND_MODE]);
            break;

#ifdef VERSION_EU
        case MENU_BUTTON_LANGUAGE_RETURN:
            return_to_main_menu(MENU_BUTTON_SOUND_MODE, sMainMenuButtons[MENU_BUTTON_LANGUAGE_RETURN]);
            break;
#else
        case MENU_BUTTON_STEREO:
            return_to_main_menu(MENU_BUTTON_SOUND_MODE, sMainMenuButtons[MENU_BUTTON_STEREO]);
            break;
        case MENU_BUTTON_MONO:
            return_to_main_menu(MENU_BUTTON_SOUND_MODE, sMainMenuButtons[MENU_BUTTON_MONO]);
            break;
        case MENU_BUTTON_HEADSET:
            return_to_main_menu(MENU_BUTTON_SOUND_MODE, sMainMenuButtons[MENU_BUTTON_HEADSET]);
            break;
#endif
    }

    sClickPos[0] = -10000;
    sClickPos[1] = -10000;
}

void handle_cursor_button_input(void) {

    if (sSelectedButtonID == MENU_BUTTON_SCORE_FILE_A || sSelectedButtonID == MENU_BUTTON_SCORE_FILE_B
        || sSelectedButtonID == MENU_BUTTON_SCORE_FILE_C
        || sSelectedButtonID == MENU_BUTTON_SCORE_FILE_D) {
        if (gPlayer3Controller->buttonPressed
#ifdef VERSION_EU
            & (B_BUTTON | START_BUTTON | Z_TRIG)
#else
            & (B_BUTTON | START_BUTTON)
#endif
        ) {
            sClickPos[0] = sCursorPos[0];
            sClickPos[1] = sCursorPos[1];
            sCursorClickingTimer = 1;
        } else if (gPlayer3Controller->buttonPressed & A_BUTTON) {
            sScoreFileCoinScoreMode = 1 - sScoreFileCoinScoreMode;
            play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
#ifdef VERSION_CN
        } else if ((gPlayer3Controller->buttonPressed & L_TRIG) || (gPlayer3Controller->buttonPressed & R_TRIG)) {
            sScorePage = 1 - sScorePage;
            play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
#endif
        }
    } else {
        if (gPlayer3Controller->buttonPressed
#ifdef VERSION_EU
            & (A_BUTTON | B_BUTTON | START_BUTTON | Z_TRIG)) {
#else
            & (A_BUTTON | B_BUTTON | START_BUTTON)) {
#endif
            sClickPos[0] = sCursorPos[0];
            sClickPos[1] = sCursorPos[1];
            sCursorClickingTimer = 1;
        }
    }
}

void handle_controller_cursor_input(void) {
    s16 rawStickX = gPlayer3Controller->rawStickX;
    s16 rawStickY = gPlayer3Controller->rawStickY;

    if (rawStickY > -2 && rawStickY < 2) {
        rawStickY = 0;
    }
    if (rawStickX > -2 && rawStickX < 2) {
        rawStickX = 0;
    }

    sCursorPos[0] += rawStickX / 8;
    sCursorPos[1] += rawStickY / 8;

    if (sCursorPos[0] > 132.0f) {
        sCursorPos[0] = 132.0f;
    }
    if (sCursorPos[0] < -132.0f) {
        sCursorPos[0] = -132.0f;
    }

    if (sCursorPos[1] > 90.0f) {
        sCursorPos[1] = 90.0f;
    }
    if (sCursorPos[1] < -90.0f) {
        sCursorPos[1] = -90.0f;
    }

    if (sCursorClickingTimer == 0) {
        handle_cursor_button_input();
    }
}

void print_menu_cursor(void) {
    handle_controller_cursor_input();
    create_dl_translation_matrix(MENU_MTX_PUSH, sCursorPos[0] + 160.0f - 5.0, sCursorPos[1] + 120.0f - 25.0, 0.0f);

    if (sCursorClickingTimer == 0) {

        gSPDisplayList(gDisplayListHead++, dl_menu_idle_hand);
    }
    if (sCursorClickingTimer != 0) {

        gSPDisplayList(gDisplayListHead++, dl_menu_grabbing_hand);
    }
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    if (sCursorClickingTimer != 0) {
        sCursorClickingTimer++;

        if (sCursorClickingTimer == 5) {
            sCursorClickingTimer = 0;
        }
    }
}

void print_hud_lut_string_fade(s8 hudLUT, s16 x, s16 y, const u8 *text) {
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha - sTextFadeAlpha);
    print_hud_lut_string(hudLUT, x, y, text);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
}

void print_generic_string_fade(s16 x, s16 y, const u8 *text) {
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha - sTextFadeAlpha);
    print_generic_string(x, y, text);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

s32 update_text_fade_out(void) {
    if (sFadeOutText == TRUE) {
        sTextFadeAlpha += 50;
        if (sTextFadeAlpha == 250) {
            sFadeOutText = FALSE;
            return TRUE;
        }
    } else {
        if (sTextFadeAlpha > 0) {
            sTextFadeAlpha -= 50;
        }
    }
    return FALSE;
}

void print_save_file_star_count(s8 fileIndex, s16 x, s16 y) {
    u8 starCountText[4];
    s8 offset = 0;
    s16 starCount;

    if (save_file_exists(fileIndex) == TRUE) {
        starCount = save_file_get_total_star_count(fileIndex, COURSE_MIN - 1, COURSE_MAX - 1);

        print_hud_lut_string(HUD_LUT_GLOBAL, x, y, starIcon);

        if (starCount < 100) {
            print_hud_lut_string(HUD_LUT_GLOBAL, x + 16, y, xIcon);
            offset = 16;
        }

        int_to_str(starCount, starCountText);
        print_hud_lut_string(HUD_LUT_GLOBAL, x + (offset + 16), y, starCountText);
    } else {

#ifdef VERSION_CN
        print_hud_lut_string(HUD_LUT_GLOBAL, x - 2, y - 5, LANGUAGE_ARRAY(textNew));
#else
        print_hud_lut_string(HUD_LUT_GLOBAL, x, y, LANGUAGE_ARRAY(textNew));
#endif
    }
}

#if defined(VERSION_JP) || defined(VERSION_SH)
    #define SELECT_FILE_X 96
    #define SELECT_FILE_Y 35
    #define SCORE_X 50
    #define COPY_X 115
    #define ERASE_X 180
#ifdef VERSION_JP
    #define SOUNDMODE_X1 235
#else
    #define SOUNDMODE_X1 sSoundTextX
#endif
    #define SAVEFILE_X1 92
    #define SAVEFILE_X2 209
    #define MARIOTEXT_X1 92
    #define MARIOTEXT_X2 207
    #define MARIOTEXT_Y1 65
    #define MARIOTEXT_Y2 105
#elif defined(VERSION_US)
    #define SELECT_FILE_X 93
    #define SELECT_FILE_Y 35
    #define SCORE_X 52
    #define COPY_X 117
    #define ERASE_X 177
    #define SOUNDMODE_X1 sSoundTextX
    #define SAVEFILE_X1 92
    #define SAVEFILE_X2 209
    #define MARIOTEXT_X1 92
    #define MARIOTEXT_X2 207
    #define MARIOTEXT_Y1 65
    #define MARIOTEXT_Y2 105
#elif defined(VERSION_EU)
    #define SAVEFILE_X1 97
    #define SAVEFILE_X2 204
    #define MARIOTEXT_X1 97
    #define MARIOTEXT_X2 204
    #define MARIOTEXT_Y1 65
    #define MARIOTEXT_Y2 105
#elif defined(VERSION_CN)
    #define SELECT_FILE_X 106
    #define SELECT_FILE_Y 25
    #define SCORE_X 52
    #define COPY_X 113
    #define ERASE_X 177
    #define SOUNDMODE_X1 sSoundTextX
    #define SAVEFILE_X1 92
    #define SAVEFILE_X2 209
    #define MARIOTEXT_X1 92
    #define MARIOTEXT_X2 207
    #define MARIOTEXT_Y1 164
    #define MARIOTEXT_Y2 124
#endif

void print_main_menu_strings(void) {
#if defined(VERSION_SH) || defined(VERSION_CN)

    static s16 sSoundTextX;
#endif

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
#ifndef VERSION_EU
    print_hud_lut_string(HUD_LUT_DIFF2, SELECT_FILE_X, SELECT_FILE_Y, textSelectFile);
#endif

    print_save_file_star_count(SAVE_FILE_A, SAVEFILE_X1, 78);
    print_save_file_star_count(SAVE_FILE_B, SAVEFILE_X2, 78);
    print_save_file_star_count(SAVE_FILE_C, SAVEFILE_X1, 118);
    print_save_file_star_count(SAVE_FILE_D, SAVEFILE_X2, 118);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
#ifndef VERSION_EU

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    print_generic_string(SCORE_X, 39, textScore);
    print_generic_string(COPY_X, 39, textCopy);
    print_generic_string(ERASE_X, 39, textErase);
#ifndef VERSION_JP
    sSoundTextX = get_str_x_pos_from_center(254, textSoundModes[sSoundMode], 10.0f);
#endif
    print_generic_string(SOUNDMODE_X1, 39, textSoundModes[sSoundMode]);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
#endif

    gSPDisplayList(gDisplayListHead++, FILE_SELECT_TEXT_DL_BEGIN);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    FILE_SELECT_PRINT_STRING(MARIOTEXT_X1, MARIOTEXT_Y1, textMarioA);
    FILE_SELECT_PRINT_STRING(MARIOTEXT_X2, MARIOTEXT_Y1, textMarioB);
    FILE_SELECT_PRINT_STRING(MARIOTEXT_X1, MARIOTEXT_Y2, textMarioC);
    FILE_SELECT_PRINT_STRING(MARIOTEXT_X2, MARIOTEXT_Y2, textMarioD);
    gSPDisplayList(gDisplayListHead++, FILE_SELECT_TEXT_DL_END);

#ifdef TARGET_NDS
    nds_home_menu_render();
#endif
}

#ifdef VERSION_EU

void print_main_lang_strings(void) {
    static s16 centeredX;

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    centeredX = get_str_x_pos_from_center_scale(160, textSelectFile[sLanguageMode], 12.0f);
    print_hud_lut_string(HUD_LUT_GLOBAL, centeredX, 35, textSelectFile[sLanguageMode]);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    centeredX = get_str_x_pos_from_center(76, textScore[sLanguageMode], 10.0f);
    print_generic_string(centeredX, 39, textScore[sLanguageMode]);
    centeredX = get_str_x_pos_from_center(131, textCopy[sLanguageMode], 10.0f);
    print_generic_string(centeredX, 39, textCopy[sLanguageMode]);
    centeredX = get_str_x_pos_from_center(189, textErase[sLanguageMode], 10.0f);
    print_generic_string(centeredX, 39, textErase[sLanguageMode]);
    centeredX = get_str_x_pos_from_center(245, textOption[sLanguageMode], 10.0f);
    print_generic_string(centeredX, 39, textOption[sLanguageMode]);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);

    print_main_menu_strings();
}
#endif

#if defined(VERSION_JP) || defined(VERSION_SH)
    #define CHECK_FILE_X 90
    #define CHECK_FILE_Y 35
    #define NOSAVE_DATA_X1 90
#elif defined(VERSION_US)
    #define CHECK_FILE_X 95
    #define CHECK_FILE_Y 35
    #define NOSAVE_DATA_X1 99
#elif defined(VERSION_EU)
    #define CHECK_FILE_X checkFileX
    #define CHECK_FILE_Y 35
    #define NOSAVE_DATA_X1 noSaveDataX
#elif defined(VERSION_CN)
    #define CHECK_FILE_X 106
    #define CHECK_FILE_Y 25
    #define NOSAVE_DATA_X1 99
#endif

void score_menu_display_message(s8 messageID) {
#ifdef VERSION_EU
    s16 checkFileX, noSaveDataX;
#endif

    switch (messageID) {
        case SCORE_MSG_CHECK_FILE:
#ifdef VERSION_EU
            checkFileX = get_str_x_pos_from_center_scale(160, LANGUAGE_ARRAY(textCheckFile), 12.0f);
#endif
            print_hud_lut_string_fade(HUD_LUT_DIFF, CHECK_FILE_X, CHECK_FILE_Y, LANGUAGE_ARRAY(textCheckFile));
            break;
        case SCORE_MSG_NOSAVE_DATA:
#ifdef VERSION_EU
            noSaveDataX = get_str_x_pos_from_center(160, LANGUAGE_ARRAY(textNoSavedDataExists), 10.0f);
#endif
            print_generic_string_fade(NOSAVE_DATA_X1, 190, LANGUAGE_ARRAY(textNoSavedDataExists));
            break;
    }
}

#if defined(VERSION_JP) || defined(VERSION_SH)
    #define RETURN_X     45
    #define COPYFILE_X1  128
    #define ERASEFILE_X1 228
    #define SCORE_FILE_Y1 62
    #define SCORE_FILE_Y2 105
#elif defined(VERSION_US)
    #define RETURN_X     44
    #define COPYFILE_X1  135
    #define ERASEFILE_X1 231
    #define SCORE_FILE_Y1 62
    #define SCORE_FILE_Y2 105
#elif defined(VERSION_EU)
    #define RETURN_X     centeredX
    #define COPYFILE_X1  centeredX
    #define ERASEFILE_X1 centeredX
#elif defined(VERSION_CN)
    #define RETURN_X     44
    #define COPYFILE_X1  135
    #define ERASEFILE_X1 231
    #define SCORE_FILE_Y1 164
    #define SCORE_FILE_Y2 121
#endif

#ifdef VERSION_CN
    #define RETURN_X_OLD 45
#else
    #define RETURN_X_OLD RETURN_X
#endif

#ifdef VERSION_EU
    #define FADEOUT_TIMER 35
#else
    #define FADEOUT_TIMER 20
#endif

void print_score_menu_strings(void) {
#ifdef VERSION_EU
    s16 centeredX;
#endif

    if (sMainMenuTimer == FADEOUT_TIMER) {
        sFadeOutText = TRUE;
    }
    if (update_text_fade_out() == TRUE) {
        if (sStatusMessageID == SCORE_MSG_CHECK_FILE) {
            sStatusMessageID = SCORE_MSG_NOSAVE_DATA;
        } else {
            sStatusMessageID = SCORE_MSG_CHECK_FILE;
        }
    }

    score_menu_display_message(sStatusMessageID);

#ifndef VERSION_EU

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    print_save_file_star_count(SAVE_FILE_A, 90, 76);
    print_save_file_star_count(SAVE_FILE_B, 211, 76);
    print_save_file_star_count(SAVE_FILE_C, 90, 119);
    print_save_file_star_count(SAVE_FILE_D, 211, 119);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
#endif

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
#ifdef VERSION_EU
    centeredX = get_str_x_pos_from_center(69, textReturn[sLanguageMode], 10.0f);
#endif
    print_generic_string(RETURN_X, 35, LANGUAGE_ARRAY(textReturn));
#ifdef VERSION_EU
    centeredX = get_str_x_pos_from_center(159, textCopyFileButton[sLanguageMode], 10.0f);
#endif
    print_generic_string(COPYFILE_X1, 35, LANGUAGE_ARRAY(textCopyFileButton));
#ifdef VERSION_EU
    centeredX = get_str_x_pos_from_center(249, textEraseFileButton[sLanguageMode], 10.0f);
#endif
    print_generic_string(ERASEFILE_X1, 35, LANGUAGE_ARRAY(textEraseFileButton));
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);

#ifdef VERSION_EU
    print_main_menu_strings();
#else
    gSPDisplayList(gDisplayListHead++, FILE_SELECT_TEXT_DL_BEGIN);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    FILE_SELECT_PRINT_STRING(89, SCORE_FILE_Y1, textMarioA);
    FILE_SELECT_PRINT_STRING(211, SCORE_FILE_Y1, textMarioB);
    FILE_SELECT_PRINT_STRING(89, SCORE_FILE_Y2, textMarioC);
    FILE_SELECT_PRINT_STRING(211, SCORE_FILE_Y2, textMarioD);
    gSPDisplayList(gDisplayListHead++, FILE_SELECT_TEXT_DL_END);
#endif
}

#if defined(VERSION_JP) || defined(VERSION_SH)
    #define NOFILE_COPY_X  90
    #define COPY_FILE_X    90
    #define COPYIT_WHERE_X 90
    #define NOSAVE_DATA_X2 90
    #define COPYCOMPLETE_X 90
    #define SAVE_EXISTS_X1 90
    #define COPY_FILE_Y    35
#elif defined(VERSION_US)
    #define NOFILE_COPY_X  119
    #define COPY_FILE_X    104
    #define COPYIT_WHERE_X 109
    #define NOSAVE_DATA_X2 101
    #define COPYCOMPLETE_X 110
    #define SAVE_EXISTS_X1 110
    #define COPY_FILE_Y    35
#elif defined(VERSION_EU)
    #define NOFILE_COPY_X  centeredX
    #define COPY_FILE_X    centeredX
    #define COPYIT_WHERE_X centeredX
    #define NOSAVE_DATA_X2 centeredX
    #define COPYCOMPLETE_X centeredX
    #define SAVE_EXISTS_X1 centeredX
    #define COPY_FILE_Y    35
#elif defined(VERSION_CN)
    #define NOFILE_COPY_X  119
    #define COPY_FILE_X    104
    #define COPYIT_WHERE_X 109
    #define NOSAVE_DATA_X2 101
    #define COPYCOMPLETE_X 110
    #define SAVE_EXISTS_X1 110
    #define COPY_FILE_Y    25
#endif

void copy_menu_display_message(s8 messageID) {
#ifdef VERSION_EU
    s16 centeredX;
#endif

    switch (messageID) {
        case COPY_MSG_MAIN_TEXT:
            if (sAllFilesExist == TRUE) {
#ifdef VERSION_EU
                centeredX = get_str_x_pos_from_center(160, textNoFileToCopyFrom[sLanguageMode], 10.0f);
#endif
                print_generic_string_fade(NOFILE_COPY_X, 190, LANGUAGE_ARRAY(textNoFileToCopyFrom));
            } else {
#ifdef VERSION_EU
                centeredX = get_str_x_pos_from_center_scale(160, textCopyFile[sLanguageMode], 12.0f);
#endif
                print_hud_lut_string_fade(HUD_LUT_DIFF, COPY_FILE_X, COPY_FILE_Y, LANGUAGE_ARRAY(textCopyFile));
            }
            break;
        case COPY_MSG_COPY_WHERE:
#ifdef VERSION_EU
            centeredX = get_str_x_pos_from_center(160, textCopyItToWhere[sLanguageMode], 10.0f);
#endif
            print_generic_string_fade(COPYIT_WHERE_X, 190, LANGUAGE_ARRAY(textCopyItToWhere));
            break;
        case COPY_MSG_NOSAVE_EXISTS:
#ifdef VERSION_EU
            centeredX = get_str_x_pos_from_center(160, textNoSavedDataExists[sLanguageMode], 10.0f);
            print_generic_string_fade(NOSAVE_DATA_X2, 190, textNoSavedDataExists[sLanguageMode]);
#else
            print_generic_string_fade(NOSAVE_DATA_X2, 190, textNoSavedDataExistsCopy);
#endif
            break;
        case COPY_MSG_COPY_COMPLETE:
#ifdef VERSION_EU
            centeredX = get_str_x_pos_from_center(160, textCopyCompleted[sLanguageMode], 10.0f);
#endif
            print_generic_string_fade(COPYCOMPLETE_X, 190, LANGUAGE_ARRAY(textCopyCompleted));
            break;
        case COPY_MSG_SAVE_EXISTS:
#ifdef VERSION_EU
            centeredX = get_str_x_pos_from_center(160, textSavedDataExists[sLanguageMode], 10.0f);
#endif
            print_generic_string_fade(SAVE_EXISTS_X1, 190, LANGUAGE_ARRAY(textSavedDataExists));
            break;
    }
}

void copy_menu_update_message(void) {
    switch (sMainMenuButtons[MENU_BUTTON_COPY]->oMenuButtonActionPhase) {
        case COPY_PHASE_MAIN:
            if (sMainMenuTimer == FADEOUT_TIMER) {
                sFadeOutText = TRUE;
            }
            if (update_text_fade_out() == TRUE) {
                if (sStatusMessageID == COPY_MSG_MAIN_TEXT) {
                    sStatusMessageID = COPY_MSG_NOSAVE_EXISTS;
                } else {
                    sStatusMessageID = COPY_MSG_MAIN_TEXT;
                }
            }
            break;
        case COPY_PHASE_COPY_WHERE:
            if (sMainMenuTimer == FADEOUT_TIMER
                && sStatusMessageID == COPY_MSG_SAVE_EXISTS) {
                sFadeOutText = TRUE;
            }
            if (update_text_fade_out() == TRUE) {
                if (sStatusMessageID != COPY_MSG_COPY_WHERE) {
                    sStatusMessageID = COPY_MSG_COPY_WHERE;
                } else {
                    sStatusMessageID = COPY_MSG_SAVE_EXISTS;
                }
            }
            break;
        case COPY_PHASE_COPY_COMPLETE:
            if (sMainMenuTimer == FADEOUT_TIMER) {
                sFadeOutText = TRUE;
            }
            if (update_text_fade_out() == TRUE) {
                if (sStatusMessageID != COPY_MSG_COPY_COMPLETE) {
                    sStatusMessageID = COPY_MSG_COPY_COMPLETE;
                } else {
                    sStatusMessageID = COPY_MSG_MAIN_TEXT;
                }
            }
            break;
    }
}

#if defined(VERSION_JP)
    #define VIEWSCORE_X1 133
    #define ERASEFILE_X2 220
    #define COPY_FILE_Y1 62
    #define COPY_FILE_Y2 105
#elif defined(VERSION_US)
    #define VIEWSCORE_X1 128
    #define ERASEFILE_X2 230
    #define COPY_FILE_Y1 62
    #define COPY_FILE_Y2 105
#elif defined(VERSION_EU)
    #define VIEWSCORE_X1 centeredX
    #define ERASEFILE_X2 centeredX
#elif defined(VERSION_SH)
    #define VIEWSCORE_X1 133
    #define ERASEFILE_X2 230
    #define COPY_FILE_Y1 62
    #define COPY_FILE_Y2 105
#elif defined(VERSION_CN)
    #define VIEWSCORE_X1 128
    #define ERASEFILE_X2 230
    #define COPY_FILE_Y1 164
    #define COPY_FILE_Y2 121
#endif

void print_copy_menu_strings(void) {
#ifdef VERSION_EU
    s16 centeredX;
#endif

    copy_menu_update_message();

    copy_menu_display_message(sStatusMessageID);
#ifndef VERSION_EU

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    print_save_file_star_count(SAVE_FILE_A, 90, 76);
    print_save_file_star_count(SAVE_FILE_B, 211, 76);
    print_save_file_star_count(SAVE_FILE_C, 90, 119);
    print_save_file_star_count(SAVE_FILE_D, 211, 119);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
#endif

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
#ifdef VERSION_EU
    centeredX = get_str_x_pos_from_center(69, textReturn[sLanguageMode], 10.0f);
#endif
    print_generic_string(RETURN_X, 35, LANGUAGE_ARRAY(textReturn));
#ifdef VERSION_EU
    centeredX = get_str_x_pos_from_center(159, textViewScore[sLanguageMode], 10.0f);
#endif
    print_generic_string(VIEWSCORE_X1, 35, LANGUAGE_ARRAY(textViewScore));
#ifdef VERSION_EU
    centeredX = get_str_x_pos_from_center(249, textEraseFileButton[sLanguageMode], 10.0f);
#endif
    print_generic_string(ERASEFILE_X2, 35, LANGUAGE_ARRAY(textEraseFileButton));
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);

#ifdef VERSION_EU
    print_main_menu_strings();
#else
    gSPDisplayList(gDisplayListHead++, FILE_SELECT_TEXT_DL_BEGIN);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    FILE_SELECT_PRINT_STRING(89, COPY_FILE_Y1, textMarioA);
    FILE_SELECT_PRINT_STRING(211, COPY_FILE_Y1, textMarioB);
    FILE_SELECT_PRINT_STRING(89, COPY_FILE_Y2, textMarioC);
    FILE_SELECT_PRINT_STRING(211, COPY_FILE_Y2, textMarioD);
    gSPDisplayList(gDisplayListHead++, FILE_SELECT_TEXT_DL_END);
#endif
}

#if defined(VERSION_JP) || defined(VERSION_SH)
#ifdef VERSION_JP
    #define CURSOR_X 160.0f
#else
    #define CURSOR_X (x + 70)
#endif
    #define MENU_ERASE_YES_MIN_X 145
    #define MENU_ERASE_YES_MAX_X 164
#elif defined(VERSION_CN)
    #define CURSOR_X (x + 70)
    #define MENU_ERASE_YES_MIN_X 144
    #define MENU_ERASE_YES_MAX_X 173
#else
    #define CURSOR_X (x + 70)
    #define MENU_ERASE_YES_MIN_X 140
    #define MENU_ERASE_YES_MAX_X 169
#endif

#ifdef VERSION_CN
    #define MENU_ERASE_YES_NO_MIN_Y 191
    #define MENU_ERASE_YES_NO_MAX_Y 210
    #define MENU_ERASE_NO_MIN_X 189
    #define MENU_ERASE_NO_MAX_X 218
    #define MENU_ERASE_YES_X_OFFSET 60
#elif defined(VERSION_SH)
    #define MENU_ERASE_YES_NO_MIN_Y 191
    #define MENU_ERASE_YES_NO_MAX_Y 210
    #define MENU_ERASE_NO_MIN_X 194
    #define MENU_ERASE_NO_MAX_X 213
    #define MENU_ERASE_YES_X_OFFSET 56
#else
    #define MENU_ERASE_YES_NO_MIN_Y 191
    #define MENU_ERASE_YES_NO_MAX_Y 210
    #define MENU_ERASE_NO_MIN_X 189
    #define MENU_ERASE_NO_MAX_X 218
    #define MENU_ERASE_YES_X_OFFSET 56
#endif

void print_erase_menu_prompt(s16 x, s16 y) {
    s16 colorTransTimer = gGlobalTimer * (1 << 12);

    s16 cursorX = sCursorPos[0] + CURSOR_X;
    s16 cursorY = sCursorPos[1] + 120.0f;

    if (cursorX < MENU_ERASE_YES_MAX_X && cursorX >= MENU_ERASE_YES_MIN_X &&
#ifdef VERSION_CN
        (u16) (cursorY - MENU_ERASE_YES_NO_MIN_Y) < MENU_ERASE_YES_NO_MAX_Y - MENU_ERASE_YES_NO_MIN_Y
#else
        cursorY < MENU_ERASE_YES_NO_MAX_Y && cursorY >= MENU_ERASE_YES_NO_MIN_Y
#endif
    ) {

        sYesNoColor[0] = sins(colorTransTimer) * 50.0f + 205.0f;
        sYesNoColor[1] = 150;
        sEraseYesNoHoverState = MENU_ERASE_HOVER_YES;
    } else if (cursorX < MENU_ERASE_NO_MAX_X && cursorX >= MENU_ERASE_NO_MIN_X &&
#ifdef VERSION_CN
        (u16) (cursorY - MENU_ERASE_YES_NO_MIN_Y) < MENU_ERASE_YES_NO_MAX_Y - MENU_ERASE_YES_NO_MIN_Y
#else
        cursorY < MENU_ERASE_YES_NO_MAX_Y && cursorY >= MENU_ERASE_YES_NO_MIN_Y
#endif
    ) {

        sYesNoColor[0] = 150;
        sYesNoColor[1] = sins(colorTransTimer) * 50.0f + 205.0f;
        sEraseYesNoHoverState = MENU_ERASE_HOVER_NO;
    } else {

        sYesNoColor[0] = 150;
        sYesNoColor[1] = 150;
        sEraseYesNoHoverState = MENU_ERASE_HOVER_NONE;
    }

    if (sCursorClickingTimer == 2) {

        if (sEraseYesNoHoverState == MENU_ERASE_HOVER_YES) {
            play_sound(SOUND_MARIO_WAAAOOOW, gGlobalSoundSource);
#if ENABLE_RUMBLE
            queue_rumble_data(5, 80);
#endif
            sMainMenuButtons[MENU_BUTTON_ERASE]->oMenuButtonActionPhase = ERASE_PHASE_MARIO_ERASED;
            sFadeOutText = TRUE;
            sMainMenuTimer = 0;
            save_file_erase(sSelectedFileIndex);
            sMainMenuButtons[MENU_BUTTON_ERASE_MIN + sSelectedFileIndex]->header.gfx.sharedChild =
                gLoadedGraphNodes[MODEL_MAIN_MENU_MARIO_NEW_BUTTON_FADE];
            sMainMenuButtons[sSelectedFileIndex]->header.gfx.sharedChild =
                gLoadedGraphNodes[MODEL_MAIN_MENU_MARIO_NEW_BUTTON_FADE];
            sEraseYesNoHoverState = MENU_ERASE_HOVER_NONE;

        } else if (sEraseYesNoHoverState == MENU_ERASE_HOVER_NO) {
            play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
#if ENABLE_RUMBLE
            queue_rumble_data(5, 80);
#endif
            sMainMenuButtons[MENU_BUTTON_ERASE_MIN + sSelectedFileIndex]->oMenuButtonState =
                MENU_BUTTON_STATE_ZOOM_OUT;
            sMainMenuButtons[MENU_BUTTON_ERASE]->oMenuButtonActionPhase = ERASE_PHASE_MAIN;
            sFadeOutText = TRUE;
            sMainMenuTimer = 0;
            sEraseYesNoHoverState = MENU_ERASE_HOVER_NONE;
        }
    }

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, sYesNoColor[0], sYesNoColor[0], sYesNoColor[0], sTextBaseAlpha);
    print_generic_string(x + MENU_ERASE_YES_X_OFFSET, y, LANGUAGE_ARRAY(textYes));
    gDPSetEnvColor(gDisplayListHead++, sYesNoColor[1], sYesNoColor[1], sYesNoColor[1], sTextBaseAlpha);
    print_generic_string(x + 98, y, LANGUAGE_ARRAY(textNo));
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

#if defined(VERSION_JP) || defined(VERSION_SH)
#ifdef VERSION_JP
    #define ERASE_FILE_X     96
#else
    #define ERASE_FILE_X     111
#endif
    #define ERASE_FILE_Y     35
    #define NOSAVE_DATA_X3   90
    #define MARIO_ERASED_VAR 3
    #define MARIO_ERASED_X   90
    #define SAVE_EXISTS_X2   90
#elif defined(VERSION_US)
    #define ERASE_FILE_X     98
    #define ERASE_FILE_Y     35
    #define NOSAVE_DATA_X3   100
    #define MARIO_ERASED_VAR 6
    #define MARIO_ERASED_X   100
    #define SAVE_EXISTS_X2   100
#elif defined(VERSION_EU)
    #define ERASE_FILE_X     centeredX
    #define ERASE_FILE_Y     35
    #define NOSAVE_DATA_X3   centeredX
    #define MARIO_ERASED_VAR 6
    #define MARIO_ERASED_X   centeredX
    #define SAVE_EXISTS_X2   centeredX
#elif defined(VERSION_CN)
    #define ERASE_FILE_X     106
    #define ERASE_FILE_Y     25
    #define NOSAVE_DATA_X3   100
    #define MARIO_ERASED_VAR 15
    #define MARIO_ERASED_X   100
    #define SAVE_EXISTS_X2   100
#endif

void erase_menu_display_message(s8 messageID) {
#ifdef VERSION_EU
    s16 centeredX;
#endif

#ifndef VERSION_EU
    u8 textEraseFile[] = { TEXT_ERASE_FILE };
    u8 textSure[] = { TEXT_SURE };
    u8 textNoSavedDataExists[] = { TEXT_NO_SAVED_DATA_EXISTS };
    u8 textMarioAJustErased[] = { TEXT_FILE_MARIO_A_JUST_ERASED };
    u8 textSavedDataExists[] = { TEXT_SAVED_DATA_EXISTS };
#endif

    switch (messageID) {
        case ERASE_MSG_MAIN_TEXT:
#ifdef VERSION_EU
            centeredX = get_str_x_pos_from_center_scale(160, textEraseFile[sLanguageMode], 12.0f);
#endif
            print_hud_lut_string_fade(HUD_LUT_DIFF, ERASE_FILE_X, ERASE_FILE_Y, LANGUAGE_ARRAY(textEraseFile));
            break;
        case ERASE_MSG_PROMPT:
            print_generic_string_fade(90, 190, LANGUAGE_ARRAY(textSure));
            print_erase_menu_prompt(90, 190);
            break;
        case ERASE_MSG_NOSAVE_EXISTS:
#ifdef VERSION_EU
            centeredX = get_str_x_pos_from_center(160, textNoSavedDataExists[sLanguageMode], 10.0f);
#endif
            print_generic_string_fade(NOSAVE_DATA_X3, 190, LANGUAGE_ARRAY(textNoSavedDataExists));
            break;
        case ERASE_MSG_MARIO_ERASED:
            LANGUAGE_ARRAY(textMarioAJustErased)[MARIO_ERASED_VAR] = sSelectedFileIndex + 10;
#ifdef VERSION_EU
            centeredX = get_str_x_pos_from_center(160, textMarioAJustErased[sLanguageMode], 10.0f);
#endif
            print_generic_string_fade(MARIO_ERASED_X, 190, LANGUAGE_ARRAY(textMarioAJustErased));
            break;
        case ERASE_MSG_SAVE_EXISTS:
#ifdef VERSION_EU
            centeredX = get_str_x_pos_from_center(160, textSavedDataExists[sLanguageMode], 10.0f);
#endif
            print_generic_string_fade(SAVE_EXISTS_X2, 190, LANGUAGE_ARRAY(textSavedDataExists));
            break;
    }
}

void erase_menu_update_message(void) {
    switch (sMainMenuButtons[MENU_BUTTON_ERASE]->oMenuButtonActionPhase) {
        case ERASE_PHASE_MAIN:
            if (sMainMenuTimer == FADEOUT_TIMER
                && sStatusMessageID == ERASE_MSG_NOSAVE_EXISTS) {
                sFadeOutText = TRUE;
            }
            if (update_text_fade_out() == TRUE) {
                if (sStatusMessageID == ERASE_MSG_MAIN_TEXT) {
                    sStatusMessageID = ERASE_MSG_NOSAVE_EXISTS;
                } else {
                    sStatusMessageID = ERASE_MSG_MAIN_TEXT;
                }
            }
            break;
        case ERASE_PHASE_PROMPT:
            if (update_text_fade_out() == TRUE) {
                if (sStatusMessageID != ERASE_MSG_PROMPT) {
                    sStatusMessageID = ERASE_MSG_PROMPT;
                }
                sCursorPos[0] = 43.0f;
                sCursorPos[1] = 80.0f;
            }
            break;
        case ERASE_PHASE_MARIO_ERASED:
            if (sMainMenuTimer == FADEOUT_TIMER) {
                sFadeOutText = TRUE;
            }
            if (update_text_fade_out() == TRUE) {
                if (sStatusMessageID != ERASE_MSG_MARIO_ERASED) {
                    sStatusMessageID = ERASE_MSG_MARIO_ERASED;
                } else {
                    sStatusMessageID = ERASE_MSG_MAIN_TEXT;
                }
            }
            break;
    }
}

#if defined(VERSION_JP) || defined(VERSION_SH)
    #define VIEWSCORE_X2 133
    #define COPYFILE_X2 223
    #define ERASE_FILE_Y1 62
    #define ERASE_FILE_Y2 105
#elif defined(VERSION_CN)
    #define VIEWSCORE_X2 129
    #define COPYFILE_X2 228
    #define ERASE_FILE_Y1 164
    #define ERASE_FILE_Y2 121
#else
    #define VIEWSCORE_X2 127
    #define COPYFILE_X2 233
    #define ERASE_FILE_Y1 62
    #define ERASE_FILE_Y2 105
#endif

void print_erase_menu_strings(void) {
#ifdef VERSION_EU
    s16 centeredX;
#endif

    erase_menu_update_message();

    erase_menu_display_message(sStatusMessageID);

#ifndef VERSION_EU

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    print_save_file_star_count(SAVE_FILE_A, 90, 76);
    print_save_file_star_count(SAVE_FILE_B, 211, 76);
    print_save_file_star_count(SAVE_FILE_C, 90, 119);
    print_save_file_star_count(SAVE_FILE_D, 211, 119);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
#endif

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);

#ifdef VERSION_EU
    centeredX = get_str_x_pos_from_center(69, textReturn[sLanguageMode], 10.0f);
    print_generic_string(centeredX, 35, textReturn[sLanguageMode]);
    centeredX = get_str_x_pos_from_center(159, textViewScore[sLanguageMode], 10.0f);
    print_generic_string(centeredX, 35, textViewScore[sLanguageMode]);
    centeredX = get_str_x_pos_from_center(249, textCopyFileButton[sLanguageMode], 10.0f);
    print_generic_string(centeredX, 35, textCopyFileButton[sLanguageMode]);
#else
    print_generic_string(RETURN_X_OLD, 35, textReturn);
    print_generic_string(VIEWSCORE_X2, 35, textViewScore);
    print_generic_string(COPYFILE_X2, 35, textCopyFileButton);
#endif
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);

#ifdef VERSION_EU
    print_main_menu_strings();
#else
    gSPDisplayList(gDisplayListHead++, FILE_SELECT_TEXT_DL_BEGIN);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    FILE_SELECT_PRINT_STRING(89, ERASE_FILE_Y1, textMarioA);
    FILE_SELECT_PRINT_STRING(211, ERASE_FILE_Y1, textMarioB);
    FILE_SELECT_PRINT_STRING(89, ERASE_FILE_Y2, textMarioC);
    FILE_SELECT_PRINT_STRING(211, ERASE_FILE_Y2, textMarioD);
    gSPDisplayList(gDisplayListHead++, FILE_SELECT_TEXT_DL_END);
#endif
}

#if defined(VERSION_JP) || defined(VERSION_SH)
    #define SOUND_HUD_X 96
    #define SOUND_HUD_Y 35
#elif defined(VERSION_US)
    #define SOUND_HUD_X 88
    #define SOUND_HUD_Y 35
#elif defined(VERSION_CN)
    #define SOUND_HUD_X 106
    #define SOUND_HUD_Y 55
#endif

void print_sound_mode_menu_strings(void) {
    s32 mode;

#if defined(VERSION_US) || defined(VERSION_SH) || defined(VERSION_CN)
    s16 textX;
#elif defined(VERSION_EU)
    s32 textX;
#endif

#ifndef VERSION_EU
    u8 textSoundSelect[] = { TEXT_SOUND_SELECT };
#endif

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);

#ifdef VERSION_EU
    print_hud_lut_string(HUD_LUT_DIFF, 47, 32, textSoundSelect[sLanguageMode]);
    print_hud_lut_string(HUD_LUT_DIFF, 47, 101, textLanguageSelect[sLanguageMode]);
#else
    print_hud_lut_string(HUD_LUT_DIFF, SOUND_HUD_X, SOUND_HUD_Y, textSoundSelect);
#endif

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);

#ifdef VERSION_EU

    for (mode = 0, textX = 90; mode < 3; textX += 70, mode++) {
        if (mode == sSoundMode) {
            gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
        } else {
            gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, sTextBaseAlpha);
        }
        print_generic_string(
            get_str_x_pos_from_center(textX, textSoundModes[sLanguageMode * 3 + mode], 10.0f),
            141, textSoundModes[sLanguageMode * 3 + mode]);
    }

    for (mode = 0, textX = 90; mode < 3; textX += 70, mode++) {
        if (mode == sLanguageMode) {
            gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
        } else {
            gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, sTextBaseAlpha);
        }
        print_generic_string(
            get_str_x_pos_from_center(textX, textLanguage[mode], 10.0f),
            72, textLanguage[mode]);
    }
#else

    for (mode = 0; mode < 3; mode++) {
        if (sSoundMode == mode) {
            gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
        } else {
            gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, sTextBaseAlpha);
        }
        #ifndef VERSION_JP

            textX = get_str_x_pos_from_center(mode * 74 + 87, textSoundModes[mode], 10.0f);
            print_generic_string(textX, 87, textSoundModes[mode]);
        #else
            print_generic_string(mode * 74 + 67, 87, textSoundModes[mode]);
        #endif
    }
#endif

#ifdef VERSION_EU
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    print_generic_string(182, 29, textReturn[sLanguageMode]);
#endif

    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

#ifndef VERSION_CN
u8 textStarX[] = { TEXT_STAR_X };
#endif

void print_score_file_castle_secret_stars(s8 fileIndex, s16 x, s16 y) {
#ifdef VERSION_CN
    u8 secretStarsText[36];
    u8 textStarX[] = { TEXT_STAR_X };
#else
    u8 secretStarsText[20];
#endif

    FILE_SELECT_PRINT_STRING(x, y, textStarX);

    INT_TO_STR_DIFF(save_file_get_total_star_count(fileIndex, COURSE_BONUS_STAGES - 1, COURSE_MAX - 1),
               secretStarsText);

#ifdef VERSION_EU
    FILE_SELECT_PRINT_STRING(x + 20, y, secretStarsText);
#else
    FILE_SELECT_PRINT_STRING(x + 16, y, secretStarsText);
#endif
}

#if defined(VERSION_JP) || defined(VERSION_SH)
    #define HISCORE_COIN_ICON_X  0
    #define HISCORE_COIN_TEXT_X  16
    #define HISCORE_COIN_NAMES_X 45
#else
    #define HISCORE_COIN_ICON_X  18
    #define HISCORE_COIN_TEXT_X  34
    #define HISCORE_COIN_NAMES_X 60
#endif

void print_score_file_course_coin_score(s8 fileIndex, s16 courseIndex, s16 x, s16 y) {
#ifdef VERSION_CN
    u8 coinScoreText[36];
#else
    u8 coinScoreText[20];
#endif

    u8 stars = save_file_get_star_flags(fileIndex, courseIndex);
    u8 textCoinX[] = { TEXT_COIN_X };
    u8 textStar[] = { TEXT_STAR };

#if defined(VERSION_JP) || defined(VERSION_SH)
    #define LENGTH 5
#elif defined(VERSION_CN)
    #define LENGTH 16
#else
    #define LENGTH 8
#endif
    u8 fileNames[][LENGTH] = {
        { TEXT_4DASHES },
        { TEXT_SCORE_MARIO_A }, { TEXT_SCORE_MARIO_B }, { TEXT_SCORE_MARIO_C }, { TEXT_SCORE_MARIO_D },
    };
#undef LENGTH

    if (sScoreFileCoinScoreMode == 0) {

        FILE_SELECT_PRINT_STRING(x + 25, y, textCoinX);

        INT_TO_STR_DIFF(save_file_get_course_coin_score(fileIndex, courseIndex), coinScoreText);
        FILE_SELECT_PRINT_STRING(x + 41, y, coinScoreText);

        if (stars & (1 << 6)) {
            FILE_SELECT_PRINT_STRING(x + 70, y, textStar);
        }
    }

    else {

        FILE_SELECT_PRINT_STRING(x + HISCORE_COIN_ICON_X, y, textCoinX);

        INT_TO_STR_DIFF((u16) save_file_get_max_coin_score(courseIndex) & 0xFFFF, coinScoreText);
        FILE_SELECT_PRINT_STRING(x + HISCORE_COIN_TEXT_X, y, coinScoreText);

        FILE_SELECT_PRINT_STRING(x + HISCORE_COIN_NAMES_X, y,
                         fileNames[(save_file_get_max_coin_score(courseIndex) >> 16) & 0xFFFF]);
    }
}

void print_score_file_star_score(s8 fileIndex, s16 courseIndex, s16 x, s16 y) {
    s16 i = 0;

#ifdef VERSION_CN
    u8 starScoreText[36];
#else
    u8 starScoreText[19];
#endif

    u8 stars = save_file_get_star_flags(fileIndex, courseIndex);
    s8 starCount = save_file_get_course_star_count(fileIndex, courseIndex);

    if (stars & (1 << 6)) {
        starCount--;
    }

    for (i = 0; i < starCount; i++) {
#ifdef VERSION_CN
        starScoreText[i * 2] = 0x00;
        starScoreText[i * 2 + 1] = DIALOG_CHAR_STAR_FILLED;
#else
        starScoreText[i] = DIALOG_CHAR_STAR_FILLED;
#endif
    }

#ifdef VERSION_CN
    starScoreText[i * 2] = DIALOG_CHAR_TERMINATOR;
    starScoreText[i * 2 + 1] = DIALOG_CHAR_TERMINATOR;
#else
    starScoreText[i] = DIALOG_CHAR_TERMINATOR;
#endif

    FILE_SELECT_PRINT_STRING(x, y, starScoreText);
}

#if defined(VERSION_JP) || defined(VERSION_SH)
    #define MARIO_X 28
    #define MARIO_Y 15
    #define FILE_LETTER_X 86
#ifdef VERSION_JP
    #define LEVEL_NUM_PAD 0
    #define SECRET_STARS_PAD 0
#else
    #define LEVEL_NUM_PAD 5
    #define SECRET_STARS_PAD 10
#endif
    #define LEVEL_NAME_X 23
    #define STAR_SCORE_X 152
    #define MYSCORE_X 237
    #define HISCORE_X 237
    #define MYSCORE_Y 24
    #define HISCORE_Y 24
#elif defined(VERSION_CN)
    #define MARIO_X 25
    #define MARIO_Y 9
    #define FILE_LETTER_X 95
    #define SECRET_STARS_PAD 6
    #define LEVEL_NAME_X 26
    #define STAR_SCORE_X 171
    #define MYSCORE_X 238
    #define HISCORE_X 231
    #define MYSCORE_Y 200
    #define HISCORE_Y 200
#else
    #define MARIO_X 25
    #define MARIO_Y 15
    #define FILE_LETTER_X 95
    #define LEVEL_NUM_PAD 3
    #define SECRET_STARS_PAD 6
    #define LEVEL_NAME_X 23
    #define STAR_SCORE_X 171
#ifdef VERSION_EU
    #define MYSCORE_X get_str_x_pos_from_center(257, textMyScore[sLanguageMode], 10.0f)
    #define HISCORE_X get_str_x_pos_from_center(257, textHiScore[sLanguageMode], 10.0f)
#else
    #define MYSCORE_X 238
    #define HISCORE_X 231
#endif
    #define MYSCORE_Y 24
    #define HISCORE_Y 24
#endif

#ifdef VERSION_EU
#include "game/segment7.h"
#endif

#define PRINT_COURSE_NAME_CN(courseIndex, shift) \
    FILE_SELECT_PRINT_STRING(LEVEL_NAME_X, 14 + 21 * (9 - courseIndex) + shift, \
                             segmented_to_virtual(levelNameTable[courseIndex - 1]));

#define PRINT_COURSE_SCORES_CN(courseIndex, shift) \
    print_score_file_star_score(fileIndex, courseIndex - 1, STAR_SCORE_X, 14 + 21 * (9 - courseIndex) + shift); \
    print_score_file_course_coin_score(fileIndex, courseIndex - 1, 213, 14 + 21 * (9 - courseIndex) + shift);

#define PRINT_COURSE_NAME_AND_SCORES(courseIndex, pad) \
    FILE_SELECT_PRINT_STRING(LEVEL_NAME_X + (pad * LEVEL_NUM_PAD), 23 + 12 * courseIndex, \
                             segmented_to_virtual(levelNameTable[courseIndex - 1])); \
    print_score_file_star_score(fileIndex, courseIndex - 1, STAR_SCORE_X, 23 + 12 * courseIndex); \
    print_score_file_course_coin_score(fileIndex, courseIndex - 1, 213, 23 + 12 * courseIndex);

void print_save_file_scores(s8 fileIndex) {
#ifndef VERSION_EU

    u8 textMario[] = { TEXT_MARIO };
#ifdef VERSION_JP
    u8 textFileLetter[] = { TEXT_ZERO };
    void **levelNameTable = segmented_to_virtual(seg2_course_name_table);
#endif
    u8 textHiScore[] = { TEXT_HI_SCORE };
    u8 textMyScore[] = { TEXT_MY_SCORE };
#ifdef VERSION_CN
    u8 textArrowL[] = { TEXT_ARROW_L };
    u8 textRArrow[] = { TEXT_R_ARROW };
#endif
#ifndef VERSION_JP
    u8 textFileLetter[] = { TEXT_ZERO };
    void **levelNameTable = segmented_to_virtual(seg2_course_name_table);
#endif

#else
    u8 textFileLetter[] = { TEXT_ZERO };
    void **levelNameTable;

    switch (sLanguageMode) {
        case LANGUAGE_ENGLISH:
            levelNameTable = segmented_to_virtual(eu_course_strings_en_table);
            break;
        case LANGUAGE_FRENCH:
            levelNameTable = segmented_to_virtual(eu_course_strings_fr_table);
            break;
        case LANGUAGE_GERMAN:
            levelNameTable = segmented_to_virtual(eu_course_strings_de_table);
            break;
    }
#endif

    textFileLetter[0] = fileIndex + ASCII_TO_DIALOG('A');

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);
    print_hud_lut_string(HUD_LUT_DIFF, MARIO_X, MARIO_Y, textMario);
    print_hud_lut_string(HUD_LUT_GLOBAL, FILE_LETTER_X, 15, textFileLetter);

    print_save_file_star_count(fileIndex, 124, 15);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);

    gSPDisplayList(gDisplayListHead++, FILE_SELECT_TEXT_DL_BEGIN);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, sTextBaseAlpha);

#ifdef VERSION_CN
    if (sScorePage == 0) {
        PRINT_COURSE_NAME_CN(COURSE_SSL, 0)
        PRINT_COURSE_NAME_CN(COURSE_LLL, 0)
        PRINT_COURSE_NAME_CN(COURSE_HMC, 0)
        PRINT_COURSE_NAME_CN(COURSE_BBH, 0)
        PRINT_COURSE_NAME_CN(COURSE_CCM, 0)
        PRINT_COURSE_NAME_CN(COURSE_JRB, 0)
        PRINT_COURSE_NAME_CN(COURSE_WF, 0)
        PRINT_COURSE_NAME_CN(COURSE_BOB, 0)

        PRINT_COURSE_SCORES_CN(COURSE_SSL, 0)
        PRINT_COURSE_SCORES_CN(COURSE_LLL, 0)
        PRINT_COURSE_SCORES_CN(COURSE_HMC, 0)
        PRINT_COURSE_SCORES_CN(COURSE_BBH, 0)
        PRINT_COURSE_SCORES_CN(COURSE_CCM, 0)
        PRINT_COURSE_SCORES_CN(COURSE_JRB, 0)
        PRINT_COURSE_SCORES_CN(COURSE_WF, 0)
        PRINT_COURSE_SCORES_CN(COURSE_BOB, 0)
    } else if (sScorePage == 1) {

        print_generic_string(LEVEL_NAME_X, 23 + 12 * 1,
                                  segmented_to_virtual(levelNameTable[25]));

        PRINT_COURSE_NAME_CN(COURSE_RR, 168)
        PRINT_COURSE_NAME_CN(COURSE_TTC, 168)
        PRINT_COURSE_NAME_CN(COURSE_THI, 168)
        PRINT_COURSE_NAME_CN(COURSE_TTM, 168)
        PRINT_COURSE_NAME_CN(COURSE_WDW, 168)
        PRINT_COURSE_NAME_CN(COURSE_SL, 168)
        PRINT_COURSE_NAME_CN(COURSE_DDD, 168)

        print_score_file_castle_secret_stars(fileIndex, STAR_SCORE_X, 23 + 12 * 1);

        PRINT_COURSE_SCORES_CN(COURSE_RR, 168)
        PRINT_COURSE_SCORES_CN(COURSE_TTC, 168)
        PRINT_COURSE_SCORES_CN(COURSE_THI, 168)
        PRINT_COURSE_SCORES_CN(COURSE_TTM, 168)
        PRINT_COURSE_SCORES_CN(COURSE_WDW, 168)
        PRINT_COURSE_SCORES_CN(COURSE_SL, 168)
        PRINT_COURSE_SCORES_CN(COURSE_DDD, 168)
    }
#else

    PRINT_COURSE_NAME_AND_SCORES(COURSE_BOB, 1)
    PRINT_COURSE_NAME_AND_SCORES(COURSE_WF,  1)
    PRINT_COURSE_NAME_AND_SCORES(COURSE_JRB, 1)
    PRINT_COURSE_NAME_AND_SCORES(COURSE_CCM, 1)
    PRINT_COURSE_NAME_AND_SCORES(COURSE_BBH, 1)
    PRINT_COURSE_NAME_AND_SCORES(COURSE_HMC, 1)
    PRINT_COURSE_NAME_AND_SCORES(COURSE_LLL, 1)
    PRINT_COURSE_NAME_AND_SCORES(COURSE_SSL, 1)
    PRINT_COURSE_NAME_AND_SCORES(COURSE_DDD, 1)
    PRINT_COURSE_NAME_AND_SCORES(COURSE_SL,  0)
    PRINT_COURSE_NAME_AND_SCORES(COURSE_WDW, 0)
    PRINT_COURSE_NAME_AND_SCORES(COURSE_TTM, 0)
    PRINT_COURSE_NAME_AND_SCORES(COURSE_THI, 0)
    PRINT_COURSE_NAME_AND_SCORES(COURSE_TTC, 0)
    PRINT_COURSE_NAME_AND_SCORES(COURSE_RR,  0)

    FILE_SELECT_PRINT_STRING(LEVEL_NAME_X + SECRET_STARS_PAD, 23 + 12 * 16,
                              segmented_to_virtual(levelNameTable[25]));

    print_score_file_castle_secret_stars(fileIndex, STAR_SCORE_X, 23 + 12 * 16);
#endif

    if (sScoreFileCoinScoreMode == 0) {
        FILE_SELECT_PRINT_STRING(MYSCORE_X, MYSCORE_Y, LANGUAGE_ARRAY(textMyScore));
    } else {
        FILE_SELECT_PRINT_STRING(HISCORE_X, HISCORE_Y, LANGUAGE_ARRAY(textHiScore));
    }

#ifdef VERSION_CN

    FILE_SELECT_PRINT_STRING(30, 17, textArrowL);
    FILE_SELECT_PRINT_STRING(270, 17, textRArrow);
#endif

    gSPDisplayList(gDisplayListHead++, FILE_SELECT_TEXT_DL_END);
}

#undef PRINT_COURSE_NAME_CN
#undef PRINT_COURSE_SCORES_CN
#undef PRINT_COURSE_NAME_AND_SCORES

static void print_file_select_strings(void) {
#ifndef VERSION_CN
    UNUSED u8 filler[8];
#endif

    create_dl_ortho_matrix();
    switch (sSelectedButtonID) {
        case MENU_BUTTON_NONE:
#ifdef VERSION_EU

            print_main_lang_strings();
#else
            print_main_menu_strings();
#endif
            break;
        case MENU_BUTTON_SCORE:
            print_score_menu_strings();
            sScoreFileCoinScoreMode = 0;
            break;
        case MENU_BUTTON_COPY:
            print_copy_menu_strings();
            break;
        case MENU_BUTTON_ERASE:
            print_erase_menu_strings();
            break;
        case MENU_BUTTON_SCORE_FILE_A:
            print_save_file_scores(SAVE_FILE_A);
            break;
        case MENU_BUTTON_SCORE_FILE_B:
            print_save_file_scores(SAVE_FILE_B);
            break;
        case MENU_BUTTON_SCORE_FILE_C:
            print_save_file_scores(SAVE_FILE_C);
            break;
        case MENU_BUTTON_SCORE_FILE_D:
            print_save_file_scores(SAVE_FILE_D);
            break;
        case MENU_BUTTON_SOUND_MODE:
            print_sound_mode_menu_strings();
            break;
    }

    if (save_file_exists(SAVE_FILE_A) == TRUE && save_file_exists(SAVE_FILE_B) == TRUE &&
        save_file_exists(SAVE_FILE_C) == TRUE && save_file_exists(SAVE_FILE_D) == TRUE) {
        sAllFilesExist = TRUE;
    } else {
        sAllFilesExist = FALSE;
    }

    if (sTextBaseAlpha < 250) {
        sTextBaseAlpha += 10;
    }
    if (sMainMenuTimer < 1000) {
        sMainMenuTimer++;
    }
}

Gfx *geo_file_select_strings_and_menu_cursor(s32 callContext, UNUSED struct GraphNode *node, UNUSED Mat4 mtx) {
    if (callContext == GEO_CONTEXT_RENDER) {
        print_file_select_strings();
        print_menu_cursor();
    }
    return NULL;
}

s32 lvl_init_menu_values_and_cursor_pos(UNUSED s32 arg, UNUSED s32 unused) {
#ifdef VERSION_EU
    s8 fileIndex;
#endif
    sSelectedButtonID = MENU_BUTTON_NONE;
    sCurrentMenuLevel = MENU_LAYER_MAIN;
    sTextBaseAlpha = 0;

    switch (gCurrSaveFileNum) {
        case 1:
            sCursorPos[0] = -94.0f;
            sCursorPos[1] = 46.0f;
            break;
        case 2:
            sCursorPos[0] = 24.0f;
            sCursorPos[1] = 46.0f;
            break;
        case 3:
            sCursorPos[0] = -94.0f;
            sCursorPos[1] = 5.0f;
            break;
        case 4:
            sCursorPos[0] = 24.0f;
            sCursorPos[1] = 5.0f;
            break;
    }
    sClickPos[0] = -10000;
    sClickPos[1] = -10000;
    sCursorClickingTimer = 0;
    sSelectedFileNum = 0;
    sSelectedFileIndex = MENU_BUTTON_NONE;
    sFadeOutText = FALSE;
    sStatusMessageID = 0;
    sTextFadeAlpha = 0;
    sMainMenuTimer = 0;
    sEraseYesNoHoverState = MENU_ERASE_HOVER_NONE;
    sSoundMode = save_file_get_sound_mode();
#ifdef VERSION_EU
    sLanguageMode = eu_get_language();

    for (fileIndex = 0; fileIndex <= 3; fileIndex++) {
        if (save_file_exists(fileIndex) == TRUE) {
            sOpenLangSettings = FALSE;
            break;
        } else {
            sOpenLangSettings = TRUE;
        }
    }
#endif

#ifdef AVOID_UB
    return 0;
#endif
}

s32 lvl_update_obj_and_load_file_selected(UNUSED s32 arg, UNUSED s32 unused) {
    area_update_objects();
    return sSelectedFileNum;
}

#undef FILE_SELECT_PRINT_STRING
#undef FILE_SELECT_TEXT_DL_BEGIN
#undef FILE_SELECT_TEXT_DL_END
