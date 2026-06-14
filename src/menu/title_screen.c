#include <PR/ultratypes.h>

#include "audio/external.h"
#include "engine/math_util.h"
#include "game/area.h"
#include "game/game_init.h"
#include "game/level_update.h"
#include "game/main.h"
#include "game/memory.h"
#include "game/print.h"
#include "game/save_file.h"
#include "game/sound_init.h"
#include "game/rumble_init.h"
#include "level_table.h"
#include "seq_ids.h"
#include "sm64.h"
#include "title_screen.h"

#define STUB_LEVEL(textname, _1, _2, _3, _4, _5, _6, _7, _8) textname,
#define DEFINE_LEVEL(textname, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) textname,

static char sLevelSelectStageNames[64][16] = {
    #include "levels/level_defines.h"
};
#undef STUB_LEVEL
#undef DEFINE_LEVEL

static u16 sDemoCountdown = 0;
#ifndef VERSION_JP
static s16 sPlayMarioGreeting = TRUE;
static s16 sPlayMarioGameOver = TRUE;

#ifdef TARGET_NDS

#include "game/ingame_menu.h"
#include "game/segment2.h"
#include "menu/intro_geo.h"
#include "text_strings.h"

extern volatile unsigned char gNdsNetState;
extern volatile unsigned char gNdsNetIsHost;
extern volatile unsigned char gNdsNetRequest;

static u8 sNdsMpChoice;
static u8 sNdsMpConfirmed;
static u8 sNdsStickNeutral = 1;

static u8 textNdsSingle[]  = { TEXT_NDS_SINGLE_PLAYER };
static u8 textNdsMulti[]   = { TEXT_NDS_MULTIPLAYER };
static u8 textNdsHint[]    = { TEXT_NDS_START_HINT };
static u8 textNdsSearch2[] = { TEXT_NDS_SEARCHING };
static u8 textNdsHost2[]   = { TEXT_NDS_MP_HOST };
static u8 textNdsGuest2[]  = { TEXT_NDS_MP_GUEST };

static void nds_intro_render_menu(void) {

    print_text_centered(160, 120, sNdsMpChoice == 0 ? "+ SINGLE PLAYER +" : "SINGLE PLAYER");
    print_text_centered(160,  95, sNdsMpChoice == 1 ? "+ MULTIPLAYER +"   : "MULTIPLAYER");

    if (sNdsMpConfirmed && sNdsMpChoice == 1) {
        if (gNdsNetState != 2) {
            print_text_centered(160, 62, "SEARCHING");
        } else {
            print_text_centered(160, 62, gNdsNetIsHost ? "HOST  PRESS A" : "GUEST  PRESS A");
        }
    } else {
        print_text_centered(160, 62, "PRESS A TO START");
    }
}

Gfx *geo_nds_intro_menu(UNUSED s32 callContext, UNUSED struct GraphNode *node, UNUSED void *context) {
    return NULL;
}
#endif
#endif

#define PRESS_START_DEMO_TIMER 800

s32 run_level_id_or_demo(s32 level) {
    gCurrDemoInput = NULL;

    if (level == LEVEL_NONE) {
        if (!gPlayer1Controller->buttonDown && !gPlayer1Controller->stickMag) {

            if ((++sDemoCountdown) == PRESS_START_DEMO_TIMER) {

                load_patchable_table(&gDemoInputsBuf, gDemoInputListID);

                if (++gDemoInputListID == gDemoInputsBuf.dmaTable->count) {
                    gDemoInputListID = 0;
                }

                gCurrDemoInput = ((struct DemoInput *) gDemoInputsBuf.bufTarget) + 1;
                level = (s8)((struct DemoInput *) gDemoInputsBuf.bufTarget)->timer;
                gCurrSaveFileNum = 1;
                gCurrActNum = 1;
            }
        } else {
            sDemoCountdown = 0;
        }
    }
    return level;
}

s16 intro_level_select(void) {
    s32 stageChanged = FALSE;

    if (gPlayer1Controller->buttonPressed & A_BUTTON) {
        ++gCurrLevelNum, stageChanged = TRUE;
    }
    if (gPlayer1Controller->buttonPressed & B_BUTTON) {
        --gCurrLevelNum, stageChanged = TRUE;
    }
    if (gPlayer1Controller->buttonPressed & U_JPAD) {
        --gCurrLevelNum, stageChanged = TRUE;
    }
    if (gPlayer1Controller->buttonPressed & D_JPAD) {
        ++gCurrLevelNum, stageChanged = TRUE;
    }
    if (gPlayer1Controller->buttonPressed & L_JPAD) {
        gCurrLevelNum -= 10, stageChanged = TRUE;
    }
    if (gPlayer1Controller->buttonPressed & R_JPAD) {
        gCurrLevelNum += 10, stageChanged = TRUE;
    }

    if (stageChanged) {
        play_sound(SOUND_GENERAL_LEVEL_SELECT_CHANGE, gGlobalSoundSource);
    }

    if (gCurrLevelNum > LEVEL_MAX) {
        gCurrLevelNum = LEVEL_MIN;
    }

    if (gCurrLevelNum < LEVEL_MIN) {
        gCurrLevelNum = LEVEL_MAX;
    }

    gCurrSaveFileNum = 4;
    gCurrActNum = 6;

    print_text_centered(160, 80, "SELECT STAGE");
    print_text_centered(160, 30, "PRESS START BUTTON");
    print_text_fmt_int(40, 60, "%2d", gCurrLevelNum);
    print_text(80, 60, sLevelSelectStageNames[gCurrLevelNum - 1]);

#define QUIT_LEVEL_SELECT_COMBO (Z_TRIG | START_BUTTON | L_CBUTTONS | R_CBUTTONS)

    if (gPlayer1Controller->buttonPressed & START_BUTTON) {

        if (gPlayer1Controller->buttonDown == QUIT_LEVEL_SELECT_COMBO) {
            gDebugLevelSelect = FALSE;
            return -1;
        }
        play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
        return gCurrLevelNum;
    }
    return 0;
}

s32 intro_regular(void) {
    s32 level = LEVEL_NONE;

#ifndef VERSION_JP

    if (sPlayMarioGreeting == TRUE) {
        if (gGlobalTimer < 129) {
            play_sound(SOUND_MARIO_HELLO, gGlobalSoundSource);
        } else {
            play_sound(SOUND_MARIO_PRESS_START_TO_PLAY, gGlobalSoundSource);
        }
        sPlayMarioGreeting = FALSE;
#ifdef TARGET_NDS
        sNdsMpChoice = 0;
        sNdsMpConfirmed = 0;
#endif
    }
#endif

#ifdef TARGET_NDS
    {
        u16 pressed = gPlayer1Controller->buttonPressed;
        s16 sy = gPlayer1Controller->stickY;

        sDemoCountdown = 0;

        if (!sNdsMpConfirmed) {

            if (sy > 30 || sy < -30) {
                if (sNdsStickNeutral) {
                    sNdsMpChoice ^= 1;
                    play_sound(SOUND_MENU_CHANGE_SELECT, gGlobalSoundSource);
                    sNdsStickNeutral = 0;
                }
            } else {
                sNdsStickNeutral = 1;
            }
            if (pressed & (A_BUTTON | START_BUTTON)) {
                sNdsMpConfirmed = 1;
                play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
                if (sNdsMpChoice == 1) {
                    gNdsNetRequest = 1;
                }
            }
        } else {

            if (sNdsMpChoice == 0 || (pressed & (A_BUTTON | START_BUTTON))) {
                level = 100 + gDebugLevelSelect;
                sPlayMarioGreeting = TRUE;
            }
        }
        nds_intro_render_menu();
    }
#else
    print_intro_text();

    if (gPlayer1Controller->buttonPressed & START_BUTTON) {
        play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
#if ENABLE_RUMBLE
        queue_rumble_data(60, 70);
        func_sh_8024C89C(1);
#endif

        level = 100 + gDebugLevelSelect;
#ifndef VERSION_JP
        sPlayMarioGreeting = TRUE;
#endif
    }
#endif
    return run_level_id_or_demo(level);
}

s32 intro_game_over(void) {
    s32 level = LEVEL_NONE;

#ifndef VERSION_JP
    if (sPlayMarioGameOver == TRUE) {
        play_sound(SOUND_MARIO_GAME_OVER, gGlobalSoundSource);
        sPlayMarioGameOver = FALSE;
    }
#endif

    print_intro_text();

    if (gPlayer1Controller->buttonPressed & START_BUTTON) {
        play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
#if ENABLE_RUMBLE
        queue_rumble_data(60, 70);
        func_sh_8024C89C(1);
#endif

        level = 100 + gDebugLevelSelect;
#ifndef VERSION_JP
        sPlayMarioGameOver = TRUE;
#endif
    }
    return run_level_id_or_demo(level);
}

s32 intro_play_its_a_me_mario(void) {
    set_background_music(0, SEQ_SOUND_PLAYER, 0);
    play_sound(SOUND_MENU_COIN_ITS_A_ME_MARIO, gGlobalSoundSource);
    return 1;
}

s32 lvl_intro_update(s16 arg, UNUSED s32 unusedArg) {
    s32 retVar;

    switch (arg) {
        case LVL_INTRO_PLAY_ITS_A_ME_MARIO:
            retVar = intro_play_its_a_me_mario();
            break;
        case LVL_INTRO_REGULAR:
            retVar = intro_regular();
            break;
        case LVL_INTRO_GAME_OVER:
            retVar = intro_game_over();
            break;
        case LVL_INTRO_LEVEL_SELECT:
            retVar = intro_level_select();
            break;
    }
    return retVar;
}
