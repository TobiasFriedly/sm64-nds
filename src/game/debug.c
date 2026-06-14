#include <PR/ultratypes.h>

#include "behavior_data.h"
#include "debug.h"
#include "engine/behavior_script.h"
#include "engine/surface_collision.h"
#include "game_init.h"
#include "main.h"
#include "object_constants.h"
#include "object_fields.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "print.h"
#include "sm64.h"
#include "types.h"

#define DEBUG_INFO_NOFLAGS (0 << 0)
#define DEBUG_INFO_FLAG_DPRINT (1 << 0)
#define DEBUG_INFO_FLAG_LSELECT (1 << 1)
#define DEBUG_INFO_FLAG_ALL 0xFF

s16 gDebugPrintState1[6];
s16 gDebugPrintState2[6];

enum DebugPrintStateInfo {
    DEBUG_PSTATE_DISABLED,
    DEBUG_PSTATE_X_CURSOR,
    DEBUG_PSTATE_Y_CURSOR,
    DEBUG_PSTATE_MIN_Y_CURSOR,
    DEBUG_PSTATE_MAX_X_CURSOR,
    DEBUG_PSTATE_LINE_Y_OFFSET
};

const char *sDebugEffectStringInfo[] = {
    "  a0 %d", "  a1 %d", "  a2 %d", "  a3 %d", "  a4 %d", "  a5 %d", "  a6 %d", "  a7 %d",
    "A"
};

const char *sDebugEnemyStringInfo[] = {
    "  b0 %d", "  b1 %d", "  b2 %d", "  b3 %d", "  b4 %d", "  b5 %d", "  b6 %d", "  b7 %d",
    "B"
};

s32 sDebugInfoDPadMask = 0;
s32 sDebugInfoDPadUpdID = 0;
s8 sDebugLvSelectCheckFlag = FALSE;

#define DEBUG_PAGE_MIN DEBUG_PAGE_OBJECTINFO
#define DEBUG_PAGE_MAX DEBUG_PAGE_ENEMYINFO

s8 sDebugPage = DEBUG_PAGE_MIN;
s8 sNoExtraDebug = FALSE;
s8 sDebugStringArrPrinted = FALSE;
s8 sDebugSysCursor = 0;
s8 sDebugInfoButtonSeqID = 0;
s16 sDebugInfoButtonSeq[] = { U_CBUTTONS, L_CBUTTONS, D_CBUTTONS, R_CBUTTONS, -1 };

void stub_debug_1(void) {
}

void stub_debug_2(void) {
}

void stub_debug_3(void) {
}

void stub_debug_4(void) {
}

s64 get_current_clock(void) {
    s64 wtf = 0;

    return wtf;
}

s64 get_clock_difference(UNUSED s64 cycles) {
    s64 wtf = 0;

    return wtf;
}

void set_print_state_info(s16 *printState, s16 xCursor, s16 yCursor, s16 minYCursor, s16 maxXCursor,
                          s16 lineYOffset) {
    printState[DEBUG_PSTATE_DISABLED] = FALSE;
    printState[DEBUG_PSTATE_X_CURSOR] = xCursor;
    printState[DEBUG_PSTATE_Y_CURSOR] = yCursor;
    printState[DEBUG_PSTATE_MIN_Y_CURSOR] = minYCursor;
    printState[DEBUG_PSTATE_MAX_X_CURSOR] = maxXCursor;
    printState[DEBUG_PSTATE_LINE_Y_OFFSET] = lineYOffset;
}

void print_text_array_info(s16 *printState, const char *str, s32 number) {
    if (!printState[DEBUG_PSTATE_DISABLED]) {
        if ((printState[DEBUG_PSTATE_Y_CURSOR] < printState[DEBUG_PSTATE_MIN_Y_CURSOR])
            || (printState[DEBUG_PSTATE_MAX_X_CURSOR] < printState[DEBUG_PSTATE_Y_CURSOR])) {
            print_text(printState[DEBUG_PSTATE_X_CURSOR], printState[DEBUG_PSTATE_Y_CURSOR],
                       "DPRINT OVER");
            printState[DEBUG_PSTATE_DISABLED]++;
        } else {
            print_text_fmt_int(printState[DEBUG_PSTATE_X_CURSOR], printState[DEBUG_PSTATE_Y_CURSOR],
                               str, number);
            printState[DEBUG_PSTATE_Y_CURSOR] += printState[DEBUG_PSTATE_LINE_Y_OFFSET];
        }
    }
}

void set_text_array_x_y(s32 xOffset, s32 yOffset) {
    s16 *printState = gDebugPrintState1;

    printState[DEBUG_PSTATE_X_CURSOR] += xOffset;
    printState[DEBUG_PSTATE_Y_CURSOR] =
        yOffset * printState[DEBUG_PSTATE_LINE_Y_OFFSET] + printState[DEBUG_PSTATE_Y_CURSOR];
}

void print_debug_bottom_up(const char *str, s32 number) {
    if (gDebugInfoFlags & DEBUG_INFO_FLAG_DPRINT) {
        print_text_array_info(gDebugPrintState2, str, number);
    }
}

void print_debug_top_down_objectinfo(const char *str, s32 number) {
    if ((gDebugInfoFlags & DEBUG_INFO_FLAG_DPRINT) && sDebugPage == DEBUG_PAGE_OBJECTINFO) {
        print_text_array_info(gDebugPrintState1, str, number);
    }
}

void print_debug_top_down_mapinfo(const char *str, s32 number) {
    if (sNoExtraDebug) {
        return;
    }

    if (gDebugInfoFlags & DEBUG_INFO_FLAG_DPRINT) {
        print_text_array_info(gDebugPrintState1, str, number);
    }
}

void print_debug_top_down_normal(const char *str, s32 number) {
    if (gDebugInfoFlags & DEBUG_INFO_FLAG_DPRINT) {
        print_text_array_info(gDebugPrintState1, str, number);
    }
}

void print_mapinfo(void) {

    struct Surface *pfloor;
    UNUSED f32 bgY;
    UNUSED f32 water;
    UNUSED s32 area;
    UNUSED s32 angY;

    angY = gCurrentObject->oMoveAngleYaw / 182.044000;
    area = ((s32) gCurrentObject->oPosX + 0x2000) / 1024
           + ((s32) gCurrentObject->oPosZ + 0x2000) / 1024 * 16;

    bgY = find_floor(gCurrentObject->oPosX, gCurrentObject->oPosY, gCurrentObject->oPosZ, &pfloor);
    water = find_water_level(gCurrentObject->oPosX, gCurrentObject->oPosZ);

    print_debug_top_down_normal("mapinfo", 0);
#ifndef VERSION_EU
    print_debug_top_down_mapinfo("area %x", area);
    print_debug_top_down_mapinfo("wx   %d", gCurrentObject->oPosX);

    print_debug_top_down_mapinfo("wy\t  %d", gCurrentObject->oPosY);
    print_debug_top_down_mapinfo("wz   %d", gCurrentObject->oPosZ);
    print_debug_top_down_mapinfo("bgY  %d", bgY);
    print_debug_top_down_mapinfo("angY %d", angY);

    if (pfloor != NULL) {
        print_debug_top_down_mapinfo("bgcode   %d", pfloor->type);
        print_debug_top_down_mapinfo("bgstatus %d", pfloor->flags);
        print_debug_top_down_mapinfo("bgarea   %d", pfloor->room);
    }

    if (gCurrentObject->oPosY < water) {
        print_debug_top_down_mapinfo("water %d", water);
    }
#endif
}

void print_checkinfo(void) {
    print_debug_top_down_normal("checkinfo", 0);
}

void print_surfaceinfo(void) {
    debug_surface_list_info(gMarioObject->oPosX, gMarioObject->oPosZ);
}

void print_stageinfo(void) {
    print_debug_top_down_normal("stageinfo", 0);
    print_debug_top_down_normal("stage param %d", gTTCSpeedSetting);
}

void print_string_array_info(const char **strArr) {
    s32 i;

    if (!sDebugStringArrPrinted) {
        sDebugStringArrPrinted++;
        for (i = 0; i < 8; i++) {

            print_debug_top_down_mapinfo(strArr[i], gDebugInfo[sDebugPage][i]);
        }

        set_text_array_x_y(0, -1 - (u32)(7 - sDebugSysCursor));
        print_debug_top_down_mapinfo(strArr[8], 0);
        set_text_array_x_y(0, 7 - sDebugSysCursor);
    }
}

void print_effectinfo(void) {
    print_debug_top_down_normal("effectinfo", 0);
    print_string_array_info(sDebugEffectStringInfo);
}

void print_enemyinfo(void) {
    print_debug_top_down_normal("enemyinfo", 0);
    print_string_array_info(sDebugEnemyStringInfo);
}

void update_debug_dpadmask(void) {
    s32 dPadMask = gPlayer1Controller->buttonDown & (U_JPAD | D_JPAD | L_JPAD | R_JPAD);

    if (!dPadMask) {
        sDebugInfoDPadUpdID = 0;
        sDebugInfoDPadMask = 0;
    } else {

        if (sDebugInfoDPadUpdID == 0) {
            sDebugInfoDPadMask = dPadMask;
        } else if (sDebugInfoDPadUpdID == 6) {
            sDebugInfoDPadMask = dPadMask;
        } else {
            sDebugInfoDPadMask = 0;
        }
        sDebugInfoDPadUpdID++;
        if (sDebugInfoDPadUpdID >= 8) {
            sDebugInfoDPadUpdID = 6;
        }
    }
}

void debug_unknown_level_select_check(void) {
    if (!sDebugLvSelectCheckFlag) {
        sDebugLvSelectCheckFlag++;

        if (!gDebugLevelSelect) {
            gDebugInfoFlags = DEBUG_INFO_NOFLAGS;
        } else {
            gDebugInfoFlags = DEBUG_INFO_FLAG_LSELECT;
        }

        gNumCalls.floor = 0;
        gNumCalls.ceil = 0;
        gNumCalls.wall = 0;
    }
}

void reset_debug_objectinfo(void) {
    gNumFindFloorMisses = 0;
    gUnknownWallCount = 0;
    gObjectCounter = 0;
    sDebugStringArrPrinted = FALSE;
    D_8035FEE2 = 0;
    D_8035FEE4 = 0;

    set_print_state_info(gDebugPrintState1, 20, 185, 40, 200, -15);
    set_print_state_info(gDebugPrintState2, 180, 30, 0, 150, 15);
    update_debug_dpadmask();
}

UNUSED static void check_debug_button_seq(void) {
    s16 *buttonArr = sDebugInfoButtonSeq;
    s16 cButtonMask;

    if (!(gPlayer1Controller->buttonDown & L_TRIG)) {
        sDebugInfoButtonSeqID = 0;
    } else {
        if ((s16)(cButtonMask = (gPlayer1Controller->buttonPressed & C_BUTTONS))) {
            if (buttonArr[sDebugInfoButtonSeqID] == cButtonMask) {
                sDebugInfoButtonSeqID++;
                if (buttonArr[sDebugInfoButtonSeqID] == -1) {
                    if (gDebugInfoFlags == DEBUG_INFO_FLAG_ALL) {
                        gDebugInfoFlags = DEBUG_INFO_FLAG_LSELECT;
                    } else {
                        gDebugInfoFlags = DEBUG_INFO_FLAG_ALL;
                    }
                }
            } else {
                sDebugInfoButtonSeqID = 0;
            }
        }
    }
}

UNUSED static void try_change_debug_page(void) {
    if (gDebugInfoFlags & DEBUG_INFO_FLAG_DPRINT) {
        if ((gPlayer1Controller->buttonPressed & L_JPAD)
            && (gPlayer1Controller->buttonDown & (L_TRIG | R_TRIG))) {
            sDebugPage++;
        }
        if ((gPlayer1Controller->buttonPressed & R_JPAD)
            && (gPlayer1Controller->buttonDown & (L_TRIG | R_TRIG))) {
            sDebugPage--;
        }
        if (sDebugPage >= (DEBUG_PAGE_MAX + 1)) {
            sDebugPage = DEBUG_PAGE_MIN;
        }
        if (sDebugPage < DEBUG_PAGE_MIN) {
            sDebugPage = DEBUG_PAGE_MAX;
        }
    }
}

#ifdef VERSION_EU
UNUSED static
#endif
void try_modify_debug_controls(void) {
    s32 sp4;

    if (gPlayer1Controller->buttonPressed & Z_TRIG) {
        sNoExtraDebug ^= 1;
    }
    if (!(gPlayer1Controller->buttonDown & (L_TRIG | R_TRIG)) && !sNoExtraDebug) {
        sp4 = 1;
        if (gPlayer1Controller->buttonDown & B_BUTTON) {
            sp4 = 100;
        }

        if (sDebugInfoDPadMask & U_JPAD) {
            sDebugSysCursor--;
            if (sDebugSysCursor < 0) {
                sDebugSysCursor = 0;
            }
        }

        if (sDebugInfoDPadMask & D_JPAD) {
            sDebugSysCursor++;
            if (sDebugSysCursor >= 8) {
                sDebugSysCursor = 7;
            }
        }

        if (sDebugInfoDPadMask & L_JPAD) {

            if (gPlayer1Controller->buttonDown & A_BUTTON) {
                gDebugInfo[sDebugPage][sDebugSysCursor] =
                    gDebugInfoOverwrite[sDebugPage][sDebugSysCursor];
            } else {
                gDebugInfo[sDebugPage][sDebugSysCursor] = gDebugInfo[sDebugPage][sDebugSysCursor] - sp4;
            }
        }

        if (sDebugInfoDPadMask & R_JPAD) {
            gDebugInfo[sDebugPage][sDebugSysCursor] = gDebugInfo[sDebugPage][sDebugSysCursor] + sp4;
        }
    }
}

void stub_debug_5(void) {
}

void try_print_debug_mario_object_info(void) {
    if (gMarioObject != NULL) {
        switch (sDebugPage) {
            case DEBUG_PAGE_CHECKSURFACEINFO:
                print_surfaceinfo();
                break;
            case DEBUG_PAGE_EFFECTINFO:
                print_effectinfo();
                break;
            case DEBUG_PAGE_ENEMYINFO:
                print_enemyinfo();
                break;
            default:
                break;
        }
    }

    print_debug_top_down_mapinfo("obj  %d", gObjectCounter);

    if (gNumFindFloorMisses != 0) {
        print_debug_bottom_up("NULLBG %d", gNumFindFloorMisses);
    }

    if (gUnknownWallCount != 0) {
        print_debug_bottom_up("WALL   %d", gUnknownWallCount);
    }
}

void try_print_debug_mario_level_info(void) {
    switch (sDebugPage) {
        case DEBUG_PAGE_OBJECTINFO:
            break;
        case DEBUG_PAGE_CHECKSURFACEINFO:
            print_checkinfo();
            break;
        case DEBUG_PAGE_MAPINFO:
            print_mapinfo();
            break;
        case DEBUG_PAGE_STAGEINFO:
            print_stageinfo();
            break;
        default:
            break;
    }
}

void try_do_mario_debug_object_spawn(void) {
    UNUSED u8 filler[4];

    if (sDebugPage == DEBUG_PAGE_STAGEINFO && gDebugInfo[DEBUG_PAGE_ENEMYINFO][7] == 1) {
        if (gPlayer1Controller->buttonPressed & R_JPAD) {
            spawn_object_relative(0, 0, 100, 200, gCurrentObject, MODEL_KOOPA_SHELL, bhvKoopaShell);
        }
        if (gPlayer1Controller->buttonPressed & L_JPAD) {
            spawn_object_relative(0, 0, 100, 200, gCurrentObject, MODEL_BREAKABLE_BOX_SMALL,
                                  bhvJumpingBox);
        }
        if (gPlayer1Controller->buttonPressed & D_JPAD) {
            spawn_object_relative(0, 0, 100, 200, gCurrentObject, MODEL_KOOPA_SHELL,
                                  bhvKoopaShellUnderwater);
        }
    }
}

void debug_print_obj_move_flags(void) {
#ifndef VERSION_EU
    if (gCurrentObject->oMoveFlags & OBJ_MOVE_LANDED) {
        print_debug_top_down_objectinfo("BOUND   %x", gCurrentObject->oMoveFlags);
    }
    if (gCurrentObject->oMoveFlags & OBJ_MOVE_ON_GROUND) {
        print_debug_top_down_objectinfo("TOUCH   %x", gCurrentObject->oMoveFlags);
    }
    if (gCurrentObject->oMoveFlags & OBJ_MOVE_LEFT_GROUND) {
        print_debug_top_down_objectinfo("TAKEOFF %x", gCurrentObject->oMoveFlags);
    }
    if (gCurrentObject->oMoveFlags & OBJ_MOVE_ENTERED_WATER) {
        print_debug_top_down_objectinfo("DIVE    %x", gCurrentObject->oMoveFlags);
    }
    if (gCurrentObject->oMoveFlags & OBJ_MOVE_AT_WATER_SURFACE) {
        print_debug_top_down_objectinfo("S WATER %x", gCurrentObject->oMoveFlags);
    }
    if (gCurrentObject->oMoveFlags & OBJ_MOVE_UNDERWATER_OFF_GROUND) {
        print_debug_top_down_objectinfo("U WATER %x", gCurrentObject->oMoveFlags);
    }
    if (gCurrentObject->oMoveFlags & OBJ_MOVE_UNDERWATER_ON_GROUND) {
        print_debug_top_down_objectinfo("B WATER %x", gCurrentObject->oMoveFlags);
    }
    if (gCurrentObject->oMoveFlags & OBJ_MOVE_IN_AIR) {
        print_debug_top_down_objectinfo("SKY     %x", gCurrentObject->oMoveFlags);
    }
    if (gCurrentObject->oMoveFlags & OBJ_MOVE_OUT_SCOPE) {
        print_debug_top_down_objectinfo("OUT SCOPE %x", gCurrentObject->oMoveFlags);
    }
#endif
}

void debug_enemy_unknown(s16 *enemyArr) {

    enemyArr[4] = gDebugInfo[DEBUG_PAGE_ENEMYINFO][1];
    enemyArr[5] = gDebugInfo[DEBUG_PAGE_ENEMYINFO][2];
    enemyArr[6] = gDebugInfo[DEBUG_PAGE_ENEMYINFO][3];
    enemyArr[7] = gDebugInfo[DEBUG_PAGE_ENEMYINFO][4];
}
