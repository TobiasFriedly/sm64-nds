#include <PR/ultratypes.h>

#include "sfx.h"

static u32 sCurrSfx;
static u32 sPrevSfx;

void gd_reset_sfx(void) {
    sPrevSfx = GD_SFX_NONE;
    sCurrSfx = GD_SFX_NONE;
}

u32 gd_new_sfx_to_play(void) {
    return ~sPrevSfx & sCurrSfx;
}

void gd_sfx_update(void) {
    sPrevSfx = sCurrSfx;
    sCurrSfx = GD_SFX_NONE;
}

void gd_play_sfx(enum GdSfx sfx) {
    sCurrSfx |= sfx;
}
