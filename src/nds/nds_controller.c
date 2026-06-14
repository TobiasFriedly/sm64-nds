#include <PR/gbi.h>

#include "nds_include.h"
#include "nds_renderer.h"
#include "engine/math_util.h"
#include "lib/src/osContInternal.h"
#include "nds_net.h"
#include "nds_netplay.h"
#include "nds_menu.h"

#define STICK_RADIUS 28

extern u8 nds_audio_state;

s32 osContInit(UNUSED OSMesgQueue *mq, u8 *controllerBits, UNUSED OSContStatus *status) {
    *controllerBits = 1;
    return 0;
}

s32 osContStartReadData(UNUSED OSMesgQueue *mesg) {
    return 0;
}

void osContGetReadData(OSContPad *pad) {

    pad->button = 0;
    pad->stick_x = 0;
    pad->stick_y = 0;

    sprites[C_LEFT].pressed = false;
    sprites[C_RIGHT].pressed = false;

    scanKeys();
    const u32 down = keysDown();
    const u32 keys = keysHeld();

    if (down & KEY_SELECT) {
        gNdsMenuOpen = !gNdsMenuOpen;
    }

    if (gNdsMenuOpen) {
        nds_menu_feed_keys(down & ~KEY_SELECT);
        return;
    }

    if (keys & KEY_A) pad->button |= A_BUTTON;
    if (keys & KEY_B) pad->button |= B_BUTTON;
    if (keys & KEY_X) pad->button |= U_CBUTTONS;
    if (keys & KEY_Y) pad->button |= D_CBUTTONS;
    if (keys & KEY_START) pad->button |= START_BUTTON;
    if (keys & KEY_L) pad->button |= R_TRIG;
    if (keys & KEY_R) pad->button |= Z_TRIG;

    if (keys & KEY_UP) pad->stick_y = 80;
    if (keys & KEY_DOWN) pad->stick_y = -80;
    if (keys & KEY_LEFT) pad->stick_x = -80;
    if (keys & KEY_RIGHT) pad->stick_x = 80;

    if (keys & KEY_TOUCH) {

        touchPosition pos;
        touchRead(&pos);

        if (sprites[STICK].pressed || (pos.px > 64 && pos.px < 192 && pos.py > 32 && pos.py < 160)) {

            int x = pos.px - 128;
            int y = pos.py - 96;
            int d = x * x + y * y;

            if (d > STICK_RADIUS * STICK_RADIUS)
            {
                int scale = (STICK_RADIUS << 8) / sqrt32(d);
                x = (x * scale) >> 8;
                y = (y * scale) >> 8;
            }

            pad->stick_x = x * 80 / STICK_RADIUS;
            pad->stick_y = -y * 80 / STICK_RADIUS;
            sprites[STICK].x = x + 96;
            sprites[STICK].y = y + 64;
            sprites[STICK].pressed = true;
        } else if (pos.px < 64 && pos.py > 128) {

            pad->button |= L_CBUTTONS;
            sprites[C_LEFT].pressed = true;
        } else if (pos.px > 192 && pos.py > 128) {

            pad->button |= R_CBUTTONS;
            sprites[C_RIGHT].pressed = true;
        }
    } else {

        sprites[STICK].x = 96;
        sprites[STICK].y = 64;
        sprites[STICK].pressed = false;
    }
}
