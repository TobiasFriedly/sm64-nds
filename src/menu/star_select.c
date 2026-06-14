#include <PR/ultratypes.h>

#include "audio/external.h"
#include "behavior_data.h"
#include "engine/behavior_script.h"
#include "engine/graph_node.h"
#include "eu_translation.h"
#include "game/area.h"
#include "game/game_init.h"
#include "game/ingame_menu.h"
#include "game/level_update.h"
#include "game/memory.h"
#include "game/object_helpers.h"
#include "game/object_list_processor.h"
#include "game/save_file.h"
#include "game/segment2.h"
#include "game/segment7.h"
#include "game/rumble_init.h"
#include "sm64.h"
#include "star_select.h"
#include "text_strings.h"

static struct Object *sStarSelectorModels[8];

static s8 sLoadedActNum;

static u8 sObtainedStars;

static s8 sVisibleStars;

static u8 sInitSelectedActNum;

static s8 sSelectedActIndex = 0;

static s8 sSelectableStarIndex = 0;

static s32 sActSelectorMenuTimer = 0;

void bhv_act_selector_star_type_loop(void) {
    switch (gCurrentObject->oStarSelectorType) {

        case STAR_SELECTOR_NOT_SELECTED:
            gCurrentObject->oStarSelectorSize -= 0.1;
            if (gCurrentObject->oStarSelectorSize < 1.0) {
                gCurrentObject->oStarSelectorSize = 1.0;
            }
            gCurrentObject->oFaceAngleYaw = 0;
            break;

        case STAR_SELECTOR_SELECTED:
            gCurrentObject->oStarSelectorSize += 0.1;
            if (gCurrentObject->oStarSelectorSize > 1.3) {
                gCurrentObject->oStarSelectorSize = 1.3;
            }
            gCurrentObject->oFaceAngleYaw += 0x800;
            break;

        case STAR_SELECTOR_100_COINS:
            gCurrentObject->oFaceAngleYaw += 0x800;
            break;
    }

    cur_obj_scale(gCurrentObject->oStarSelectorSize);

    gCurrentObject->oStarSelectorTimer++;
}

void render_100_coin_star(u8 stars) {
    if (stars & (1 << 6)) {

        sStarSelectorModels[6] = spawn_object_abs_with_rot(gCurrentObject, 0, MODEL_STAR,
                                                        bhvActSelectorStarType, 370, 24, -300, 0, 0, 0);
        sStarSelectorModels[6]->oStarSelectorSize = 0.8;
        sStarSelectorModels[6]->oStarSelectorType = STAR_SELECTOR_100_COINS;
    }
}

void bhv_act_selector_init(void) {
    s16 i = 0;
    s32 selectorModelIDs[10];
    u8 stars = save_file_get_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(gCurrCourseNum));

    sVisibleStars = 0;
    while (i != sObtainedStars) {
        if (stars & (1 << sVisibleStars)) {
            selectorModelIDs[sVisibleStars] = MODEL_STAR;
            i++;
        } else {
            selectorModelIDs[sVisibleStars] = MODEL_TRANSPARENT_STAR;

            if (sInitSelectedActNum == 0) {
                sInitSelectedActNum = sVisibleStars + 1;
                sSelectableStarIndex = sVisibleStars;
            }
        }
        sVisibleStars++;
    }

    if (sVisibleStars == sObtainedStars && sVisibleStars != 6) {
        selectorModelIDs[sVisibleStars] = MODEL_TRANSPARENT_STAR;
        sInitSelectedActNum = sVisibleStars + 1;
        sSelectableStarIndex = sVisibleStars;
        sVisibleStars++;
    }

    if (sObtainedStars == 6) {
        sInitSelectedActNum = sVisibleStars;
    }

    if (sObtainedStars == 0) {
        sInitSelectedActNum = 1;
    }

    for (i = 0; i < sVisibleStars; i++) {
        sStarSelectorModels[i] =
            spawn_object_abs_with_rot(gCurrentObject, 0, selectorModelIDs[i], bhvActSelectorStarType,
                                      (sVisibleStars - 1) * -75 + i * 152, 248, -300, 0, 0, 0);

        sStarSelectorModels[i]->oStarSelectorSize = 1.0f;
    }

    render_100_coin_star(stars);
}

void bhv_act_selector_loop(void) {
    s8 i;
    u8 starIndexCounter;
    u8 stars = save_file_get_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(gCurrCourseNum));

    if (sObtainedStars != 6) {

        sSelectedActIndex = 0;
        handle_menu_scrolling(MENU_SCROLL_HORIZONTAL, &sSelectableStarIndex, 0, sObtainedStars);
        starIndexCounter = sSelectableStarIndex;
        for (i = 0; i < sVisibleStars; i++) {

            if ((stars & (1 << i)) || i == sInitSelectedActNum - 1) {
                if (starIndexCounter == 0) {
                    sSelectedActIndex = i;
                    break;
                }
                starIndexCounter--;
            }
        }
    } else {

        handle_menu_scrolling(MENU_SCROLL_HORIZONTAL, &sSelectableStarIndex, 0, sVisibleStars - 1);
        sSelectedActIndex = sSelectableStarIndex;
    }

    for (i = 0; i < sVisibleStars; i++) {
        if (sSelectedActIndex == i) {
            sStarSelectorModels[i]->oStarSelectorType = STAR_SELECTOR_SELECTED;
        } else {
            sStarSelectorModels[i]->oStarSelectorType = STAR_SELECTOR_NOT_SELECTED;
        }
    }
}

#ifdef VERSION_EU
void print_course_number(s16 language) {
#else
void print_course_number(void) {
#endif
    u8 courseNum[4];

    create_dl_translation_matrix(MENU_MTX_PUSH, 158.0f, 81.0f, 0.0f);

    gSPDisplayList(gDisplayListHead++, dl_menu_rgba16_wood_course);

#ifdef VERSION_EU

    switch (language) {
        case LANGUAGE_ENGLISH:
            gSPDisplayList(gDisplayListHead++, dl_menu_texture_course_upper);
            break;
        case LANGUAGE_FRENCH:
            gSPDisplayList(gDisplayListHead++, dl_menu_texture_niveau_upper);
            break;
        case LANGUAGE_GERMAN:
            gSPDisplayList(gDisplayListHead++, dl_menu_texture_kurs_upper);
            break;
    }

    gSPDisplayList(gDisplayListHead++, dl_menu_rgba16_wood_course_end);
#endif

    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);

    int_to_str(gCurrCourseNum, courseNum);

    if (gCurrCourseNum < 10) {
        print_hud_lut_string(HUD_LUT_GLOBAL, 152, 158, courseNum);
    } else {
        print_hud_lut_string(HUD_LUT_GLOBAL, 143, 158, courseNum);
    }

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);
}

#ifdef VERSION_JP
#define ACT_NAME_X 158
#else
#define ACT_NAME_X 163
#endif

void print_act_selector_strings(void) {
#ifdef VERSION_EU
    unsigned char myScore[][10] = { {TEXT_MYSCORE}, {TEXT_MY_SCORE_FR}, {TEXT_MY_SCORE_DE} };
#else
    unsigned char myScore[] = { TEXT_MYSCORE };
#endif

    unsigned char starNumbers[] = { TEXT_ZERO };

#ifdef VERSION_EU
    u8 **levelNameTbl;
    u8 *currLevelName;
    u8 **actNameTbl;
#else
    u8 **levelNameTbl = segmented_to_virtual(seg2_course_name_table);
    u8 *currLevelName = segmented_to_virtual(levelNameTbl[COURSE_NUM_TO_INDEX(gCurrCourseNum)]);
    u8 **actNameTbl = segmented_to_virtual(seg2_act_name_table);
#endif
    u8 *selectedActName;
    s16 lvlNameX;
    s16 actNameX;
    s8 i;
#ifdef VERSION_EU
    s16 language = eu_get_language();
#endif

    create_dl_ortho_matrix();

#ifdef VERSION_EU
    switch (language) {
        case LANGUAGE_ENGLISH:
            actNameTbl = segmented_to_virtual(act_name_table_eu_en);
            levelNameTbl = segmented_to_virtual(course_name_table_eu_en);
            break;
        case LANGUAGE_FRENCH:
            actNameTbl = segmented_to_virtual(act_name_table_eu_fr);
            levelNameTbl = segmented_to_virtual(course_name_table_eu_fr);
            break;
        case LANGUAGE_GERMAN:
            actNameTbl = segmented_to_virtual(act_name_table_eu_de);
            levelNameTbl = segmented_to_virtual(course_name_table_eu_de);
            break;
    }
    currLevelName = segmented_to_virtual(levelNameTbl[COURSE_NUM_TO_INDEX(gCurrCourseNum)]);
#endif

    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    print_hud_my_score_coins(1, gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(gCurrCourseNum), 155, 106);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);

    if (save_file_get_course_coin_score(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(gCurrCourseNum)) != 0) {

#ifdef VERSION_EU
        print_generic_string(95, 118, myScore[language]);
#elif defined(VERSION_CN)
        print_generic_string(89, 118, myScore);
#else
        print_generic_string(102, 118, myScore);
#endif
    }

#ifdef VERSION_CN
    lvlNameX = get_str_x_pos_from_center(160, currLevelName + 6, 16.0f);
    print_generic_string(lvlNameX, 30, currLevelName + 6);
#else
    lvlNameX = get_str_x_pos_from_center(160, currLevelName + 3, 10.0f);
    print_generic_string(lvlNameX, 33, currLevelName + 3);
#endif

    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);

#ifdef VERSION_EU
    print_course_number(language);
#else
    print_course_number();
#endif

#ifdef VERSION_CN
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
#else
    gSPDisplayList(gDisplayListHead++, dl_menu_ia8_text_begin);
#endif
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);

    if (sVisibleStars != 0) {
        selectedActName = segmented_to_virtual(actNameTbl[COURSE_NUM_TO_INDEX(gCurrCourseNum) * 6 + sSelectedActIndex]);

#ifdef VERSION_CN
        actNameX = get_str_x_pos_from_center(ACT_NAME_X, selectedActName, 16.0f);
        print_generic_string(actNameX, 141, selectedActName);
#else
        actNameX = get_str_x_pos_from_center(ACT_NAME_X, selectedActName, 8.0f);
        print_menu_generic_string(actNameX, 81, selectedActName);
#endif
    }

#ifdef VERSION_CN
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);

    gSPDisplayList(gDisplayListHead++, dl_menu_ia8_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);
#endif

    for (i = 1; i <= sVisibleStars; i++) {
        starNumbers[0] = i;
#ifdef VERSION_EU
        print_menu_generic_string(128 - (sVisibleStars - 1) * 15 + i * 30, 38, starNumbers);
#else
        print_menu_generic_string(122 - (sVisibleStars - 1) * 17 + i * 34, 38, starNumbers);
#endif
    }

    gSPDisplayList(gDisplayListHead++, dl_menu_ia8_text_end);
}

#ifdef AVOID_UB
Gfx *geo_act_selector_strings(s16 callContext, UNUSED struct GraphNode *node, UNUSED void *context)
#else
Gfx *geo_act_selector_strings(s16 callContext, UNUSED struct GraphNode *node)
#endif
{
    if (callContext == GEO_CONTEXT_RENDER) {
        print_act_selector_strings();
    }
    return NULL;
}

s32 lvl_init_act_selector_values_and_stars(UNUSED s32 arg, UNUSED s32 unused) {
    u8 stars = save_file_get_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(gCurrCourseNum));

    sLoadedActNum = 0;
    sInitSelectedActNum = 0;
    sVisibleStars = 0;
    sActSelectorMenuTimer = 0;
    sObtainedStars =
        save_file_get_course_star_count(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(gCurrCourseNum));

    if (stars & (1 << 6)) {
        sObtainedStars--;
    }

#ifdef AVOID_UB
    return 0;
#endif
}

s32 lvl_update_obj_and_load_act_button_actions(UNUSED s32 arg, UNUSED s32 unused) {
    if (sActSelectorMenuTimer > 10) {

#ifndef VERSION_EU
        if ((gPlayer3Controller->buttonPressed & A_BUTTON)
         || (gPlayer3Controller->buttonPressed & START_BUTTON)
         || (gPlayer3Controller->buttonPressed & B_BUTTON))
#else
        if (gPlayer3Controller->buttonPressed & (A_BUTTON | START_BUTTON | B_BUTTON | Z_TRIG))
#endif
        {
#ifdef VERSION_JP
            play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
#else
            play_sound(SOUND_MENU_STAR_SOUND_LETS_A_GO, gGlobalSoundSource);
#endif
#if ENABLE_RUMBLE
            queue_rumble_data(60, 70);
            func_sh_8024C89C(1);
#endif
            if (sInitSelectedActNum >= sSelectedActIndex + 1) {
                sLoadedActNum = sSelectedActIndex + 1;
            } else {
                sLoadedActNum = sInitSelectedActNum;
            }
            gDialogCourseActNum = sSelectedActIndex + 1;
        }
    }

    area_update_objects();
    sActSelectorMenuTimer++;
    return sLoadedActNum;
}
