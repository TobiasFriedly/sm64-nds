#include <PR/ultratypes.h>

#include "debug_utils.h"
#include "gd_main.h"
#include "gd_memory.h"
#include "macros.h"
#include "objects.h"
#include "renderer.h"

s32 gGdMoveScene = TRUE;
UNUSED static s32 sUnref801A8054 = TRUE;
f32 D_801A8058 = -600.0f;
s32 gGdUseVtxNormal = TRUE;
UNUSED static s32 sUnrefScnWidth = 320;
UNUSED static s32 sUnrefScnHeight = 240;

struct GdControl gGdCtrl;
struct GdControl gGdCtrlPrev;

u32 __main__(void) {
    UNUSED u8 filler[4];

    gd_printf("%x, %x\n", (u32) (uintptr_t) &D_801A8058, (u32) (uintptr_t) &gGdMoveScene);
    imin("main");
    gd_init();

    gGdCtrl.unk88 = 0.46799f;
    gGdCtrl.unkA0 = -34.0f;
    gGdCtrl.unkAC = 34.0f;
    gGdCtrl.unk00 = 2;
    gGdCtrl.newStartPress = FALSE;
    gGdCtrl.prevFrame = &gGdCtrlPrev;

    imin("main - make_scene");
    make_scene();
    imout();

    gd_init_controllers();
    print_all_memtrackers();

    start_timer("dlgen");
    stop_timer("dlgen");
    mem_stats();

    while (TRUE) {
        func_801A520C();
    }

    imout();
    return 0;
}
