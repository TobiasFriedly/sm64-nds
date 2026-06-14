#include <ultra64.h>

#include "nds_include.h"
#include "nds_menu.h"
#include "macros.h"
#include "model_ids.h"

void nds_debug_printf(const char *fmt, ...);
void nds_debug_console_clear(void);

extern u8 gNdsModelLoaded[32];

volatile u8 gNdsMenuOpen;
volatile u8 gNdsLocalAppearanceModel;
volatile u8 gNdsLocalAppearanceColor;
volatile u8 gNdsExperimentalSkins;

static const struct {
    const char *name;
    s16 model;
} sSkins[] = {
    { "Mario",   MODEL_MARIO },
    { "Goomba",  MODEL_GOOMBA },
    { "Koopa",   MODEL_KOOPA_WITHOUT_SHELL },
    { "Bob-omb", MODEL_BLACK_BOBOMB },
    { "Buddy",   MODEL_BOBOMB_BUDDY },
};

#define COLOR_BLEND   1
#define COLOR_REPLACE 2
static const struct {
    const char *name;
    u8 rgb[3];
    u8 mode;
} sColors[] = {
    { "None",   { 0xFF, 0xFF, 0xFF }, COLOR_BLEND   },
    { "Red",    { 0xFF, 0x40, 0x40 }, COLOR_BLEND   },
    { "Green",  { 0x40, 0xFF, 0x40 }, COLOR_BLEND   },
    { "Blue",   { 0x60, 0x80, 0xFF }, COLOR_BLEND   },
    { "Purple", { 0xB0, 0x50, 0xFF }, COLOR_BLEND   },
    { "Yellow", { 0xFF, 0xD0, 0x30 }, COLOR_BLEND   },
    { "Pink",   { 0xFF, 0x90, 0xC0 }, COLOR_BLEND   },
    { "Dark",   { 0x70, 0x70, 0x70 }, COLOR_BLEND   },
    { "Black",  { 0x00, 0x00, 0x00 }, COLOR_REPLACE },
};

s16 nds_skin_model(u8 idx) {
    return (idx < ARRAY_COUNT(sSkins)) ? sSkins[idx].model : MODEL_MARIO;
}

u32 nds_skin_color_tint(u8 idx) {
    if (idx == 0 || idx >= ARRAY_COUNT(sColors)) {
        return 0;
    }
    return ((u32) sColors[idx].mode << 24)
         | ((u32) sColors[idx].rgb[0] << 16)
         | ((u32) sColors[idx].rgb[1] << 8)
         |  (u32) sColors[idx].rgb[2];
}

u8 nds_skin_count(void) {
    return (u8) ARRAY_COUNT(sSkins);
}

u8 nds_color_count(void) {
    return (u8) ARRAY_COUNT(sColors);
}

static s32 skin_available(u8 idx) {
    s16 m = sSkins[idx].model;
    if (m == MODEL_MARIO) {
        return 1;
    }
    if (!gNdsExperimentalSkins || m < 0 || m >= 256) {
        return 0;
    }
    return (gNdsModelLoaded[m >> 3] >> (m & 7)) & 1;
}

static u8 next_available_skin(u8 from, s32 dir) {
    u8 nSkins = nds_skin_count();
    u8 i = from;
    do {
        i = (u8) ((i + (dir > 0 ? 1 : nSkins - 1)) % nSkins);
    } while (!skin_available(i) && i != from);
    return i;
}

void nds_menu_feed_keys(u32 down) {
    u8 nColors = nds_color_count();

    if (down & KEY_X) {
        gNdsExperimentalSkins ^= 1;
        if (!gNdsExperimentalSkins) {
            gNdsLocalAppearanceModel = 0;
        }
    }

    if (!skin_available(gNdsLocalAppearanceModel)) {
        gNdsLocalAppearanceModel = next_available_skin(gNdsLocalAppearanceModel, 1);
    }

    if (down & KEY_UP) {
        gNdsLocalAppearanceModel = next_available_skin(gNdsLocalAppearanceModel, -1);
    }
    if (down & KEY_DOWN) {
        gNdsLocalAppearanceModel = next_available_skin(gNdsLocalAppearanceModel, 1);
    }
    if (down & KEY_LEFT) {
        gNdsLocalAppearanceColor = (gNdsLocalAppearanceColor + nColors - 1) % nColors;
    }
    if (down & KEY_RIGHT) {
        gNdsLocalAppearanceColor = (gNdsLocalAppearanceColor + 1) % nColors;
    }

    if (down & (KEY_A | KEY_B)) {
        gNdsMenuOpen = 0;
    }
}

void nds_menu_render(void) {
    static u8 sWasOpen = 0;
    u8 i;
    if (!gNdsMenuOpen) {

        if (sWasOpen) {
            nds_debug_console_clear();
            sWasOpen = 0;
        }
        return;
    }
    sWasOpen = 1;
    nds_debug_console_clear();
    nds_debug_printf("== CHARACTER ==\n");
    nds_debug_printf("UP/DOWN: skin\n");
    nds_debug_printf("LEFT/RIGHT: color\n");
    nds_debug_printf("X: experimental [%s]\n", gNdsExperimentalSkins ? "ON" : "OFF");
    nds_debug_printf("A: confirm\n\n");

    for (i = 0; i < nds_skin_count(); i++) {
        if (!skin_available(i)) {
            continue;
        }
        nds_debug_printf("%c %s\n",
                         (i == gNdsLocalAppearanceModel) ? '>' : ' ',
                         sSkins[i].name);
    }
    nds_debug_printf("\nColor: %s\n", sColors[gNdsLocalAppearanceColor].name);
}
