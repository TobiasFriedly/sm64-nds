#include <PR/ultratypes.h>
#include <PR/gbi.h>

#include "config.h"
#include "gfx_dimensions.h"
#include "game_init.h"
#include "memory.h"
#include "print.h"
#include "segment2.h"

struct TextLabel {
    u32 x;
    u32 y;
    s16 length;
    char buffer[50];
};

FORCE_BSS struct TextLabel *sTextLabels[52];
s16 sTextLabelsCount = 0;

s32 int_pow(s32 n, s32 exponent) {
    s32 result = 1;
    s32 i;

    for (i = 0; i < exponent; i++) {
        result = n * result;
    }

    return result;
}

void format_integer(s32 n, s32 base, char *dest, s32 *totalLength, u8 width, s8 zeroPad) {
    u32 powBase;
    s32 numDigits = 0;
    s32 i;
    s32 len = 0;
    s8 digit;
    s8 negative = FALSE;
    char pad;

    if (zeroPad == TRUE) {
        pad = '0';
    } else {
        pad = -1;
    }

    if (n != 0) {

        if (n < 0) {
            n = -n;
            negative = TRUE;
        }

        while (TRUE) {
            powBase = int_pow(base, numDigits);

            if (powBase > (u32) n) {
                break;
            }

            numDigits++;
        }

        if (width > numDigits) {
            for (len = 0; len < width - numDigits; len++) dest[len] = pad;

            if (negative == TRUE) {
                len--;
            }
        }

        if (negative == TRUE) {
            dest[len] = 'M';
            len++;
        }

        for (i = numDigits - 1; i >= 0; i--) {
            powBase = int_pow(base, i);
            digit = n / powBase;

            if (digit < 10) {
                *(dest + len + (numDigits - 1) - i) = digit + '0';
            } else {
                *(dest + len + (numDigits - 1) - i) = digit + '7';
            }

            n -= digit * powBase;
        }
    } else {
        numDigits = 1;
        if (width > numDigits) {
            for (len = 0; len < width - numDigits; len++) dest[len] = pad;
        }
        dest[len] = '0';
    }

    *totalLength += numDigits + len;
}

void parse_width_field(const char *str, s32 *srcIndex, u8 *width, s8 *zeroPad) {
    s8 digits[12];
    s8 digitsLen = 0;
    s16 i;

    if (str[*srcIndex] == '0') {
        *zeroPad = TRUE;
    }

    while (str[*srcIndex] != 'd' && str[*srcIndex] != 'x') {
        digits[digitsLen] = str[*srcIndex] - '0';

        if (digits[digitsLen] < 0 || digits[digitsLen] >= 10) {
            *width = 0;
            return;
        }

        digitsLen++;
        (*srcIndex)++;
    }

    if (digitsLen == 0) {
        return;
    }

    for (i = 0; i < digitsLen - 1; i++) {
        *width = *width + ((digitsLen - i - 1) * 10) * digits[i];
    }

    *width = *width + digits[digitsLen - 1];
}

void print_text_fmt_int(s32 x, s32 y, const char *str, s32 n) {
    char c = 0;
    s8 zeroPad = FALSE;
    u8 width = 0;
    s32 base = 0;
    s32 len = 0;
    s32 srcIndex = 0;

    if ((sTextLabels[sTextLabelsCount] = mem_pool_alloc(gEffectsMemoryPool,
                                                        sizeof(struct TextLabel))) == NULL) {
        return;
    }

    sTextLabels[sTextLabelsCount]->x = x;
    sTextLabels[sTextLabelsCount]->y = y;

    c = str[srcIndex];

    while (c != '\0') {
        if (c == '%') {
            srcIndex++;

            parse_width_field(str, &srcIndex, &width, &zeroPad);

            if (str[srcIndex] != 'd' && str[srcIndex] != 'x') {
                break;
            }
            if (str[srcIndex] == 'd') {
                base = 10;
            }
            if (str[srcIndex] == 'x') {
                base = 16;
            }

            srcIndex++;

            format_integer(n, base, sTextLabels[sTextLabelsCount]->buffer + len, &len, width, zeroPad);
        } else {
            sTextLabels[sTextLabelsCount]->buffer[len] = c;
            len++;
            srcIndex++;
        }
        c = str[srcIndex];
    }

    sTextLabels[sTextLabelsCount]->length = len;
    sTextLabelsCount++;
}

void print_text(s32 x, s32 y, const char *str) {
    char c = 0;
    s32 length = 0;
    s32 srcIndex = 0;

    if ((sTextLabels[sTextLabelsCount] = mem_pool_alloc(gEffectsMemoryPool,
                                                        sizeof(struct TextLabel))) == NULL) {
        return;
    }

    sTextLabels[sTextLabelsCount]->x = x;
    sTextLabels[sTextLabelsCount]->y = y;

    c = str[srcIndex];

    while (c != '\0') {
        sTextLabels[sTextLabelsCount]->buffer[length] = c;
        length++;
        srcIndex++;
        c = str[srcIndex];
    }

    sTextLabels[sTextLabelsCount]->length = length;
    sTextLabelsCount++;
}

void print_text_centered(s32 x, s32 y, const char *str) {
    char c = 0;
    UNUSED s8 unused1 = 0;
    UNUSED s32 unused2 = 0;
    s32 length = 0;
    s32 srcIndex = 0;
#ifdef VERSION_CN
    s32 width = 0;
#endif

    if ((sTextLabels[sTextLabelsCount] = mem_pool_alloc(gEffectsMemoryPool,
                                                        sizeof(struct TextLabel))) == NULL) {
        return;
    }

    c = str[srcIndex];

    while (c != '\0') {
#ifdef VERSION_CN
        if ((u8) c == 0xB0 || (u8) c == 0xC0) {
            width = 16;
        } else {
            width = 12;
        }
#endif
        sTextLabels[sTextLabelsCount]->buffer[length] = c;
        length++;
        srcIndex++;
        c = str[srcIndex];
    }

    sTextLabels[sTextLabelsCount]->length = length;
#ifdef VERSION_CN
    sTextLabels[sTextLabelsCount]->x = x - width * length / 2;
#else
    sTextLabels[sTextLabelsCount]->x = x - 12 * length / 2;
#endif
    sTextLabels[sTextLabelsCount]->y = y;
    sTextLabelsCount++;
}

s8 char_to_glyph_index(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 55;
    }

    if (c >= 'a' && c <= 'z') {
        return c - 87;
    }

    if (c >= '0' && c <= '9') {
        return c - 48;
    }

    if (c == ' ') {
        return GLYPH_SPACE;
    }

    if (c == '!') {
        return GLYPH_EXCLAMATION_PNT;
    }

    if (c == '#') {
        return GLYPH_TWO_EXCLAMATION;
    }

    if (c == '?') {
        return GLYPH_QUESTION_MARK;
    }

    if (c == '&') {
        return GLYPH_AMPERSAND;
    }

    if (c == '%') {
        return GLYPH_PERCENT;
    }

    if (c == '*') {
        return GLYPH_MULTIPLY;
    }

    if (c == '+') {
        return GLYPH_COIN;
    }

    if (c == ',') {
        return GLYPH_MARIO_HEAD;
    }

    if (c == '-') {
        return GLYPH_STAR;
    }

    if (c == '.') {
        return GLYPH_PERIOD;
    }

    if (c == '/') {
        return GLYPH_BETA_KEY;
    }

    return GLYPH_SPACE;
}

void add_glyph_texture(s8 glyphIndex) {
    const u8 *const *glyphs = segmented_to_virtual(main_hud_lut);

    gDPPipeSync(gDisplayListHead++);
#ifdef VERSION_CN
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, glyphs[(u8) glyphIndex]);
#else
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, glyphs[glyphIndex]);
#endif
    gSPDisplayList(gDisplayListHead++, dl_hud_img_load_tex_block);
}

#ifndef WIDESCREEN

void clip_to_bounds(s32 *x, s32 *y) {
    if (*x < TEXRECT_MIN_X) {
        *x = TEXRECT_MIN_X;
    }

    if (*x > TEXRECT_MAX_X) {
        *x = TEXRECT_MAX_X;
    }

    if (*y < TEXRECT_MIN_Y) {
        *y = TEXRECT_MIN_Y;
    }

    if (*y > TEXRECT_MAX_Y) {
        *y = TEXRECT_MAX_Y;
    }
}
#endif

#ifdef VERSION_CN
void render_textrect(s32 x, s32 y, s32 pos, s32 width) {
    s32 rectBaseX = x + pos * width;
#else
void render_textrect(s32 x, s32 y, s32 pos) {
    s32 rectBaseX = x + pos * 12;
#endif
    s32 rectBaseY = 224 - y;
    s32 rectX;
    s32 rectY;

#ifndef WIDESCREEN

    clip_to_bounds(&rectBaseX, &rectBaseY);
#endif
    rectX = rectBaseX;
    rectY = rectBaseY;
    gSPTextureRectangle(gDisplayListHead++, rectX << 2, rectY << 2, (rectX + 15) << 2,
                        (rectY + 15) << 2, G_TX_RENDERTILE, 0, 0, 4 << 10, 1 << 10);
}

void render_text_labels(void) {
    s32 i;
    s32 j;
    s8 glyphIndex;
    Mtx *mtx;

    if (sTextLabelsCount == 0) {
        return;
    }

    mtx = alloc_display_list(sizeof(*mtx));

    if (mtx == NULL) {
        sTextLabelsCount = 0;
        return;
    }

    guOrtho(mtx, 0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, -10.0f, 10.0f, 1.0f);
    gSPPerspNormalize((Gfx *) (gDisplayListHead++), 0xFFFF);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPDisplayList(gDisplayListHead++, dl_hud_img_begin);

    for (i = 0; i < sTextLabelsCount; i++) {
        for (j = 0; j < sTextLabels[i]->length; j++) {
#ifdef VERSION_CN
            if ((u8) sTextLabels[i]->buffer[j] < 0xA0) {
                glyphIndex = char_to_glyph_index(sTextLabels[i]->buffer[j]);
            } else if ((u8) sTextLabels[i]->buffer[j] == 0xB0) {
                glyphIndex = 0xB0;
            } else if ((u8) sTextLabels[i]->buffer[j] == 0xC0) {
                glyphIndex = 0xC0;
            } else {
                glyphIndex = GLYPH_SPACE;
            }
#else
            glyphIndex = char_to_glyph_index(sTextLabels[i]->buffer[j]);
#endif

            if (glyphIndex != GLYPH_SPACE) {
#ifdef VERSION_EU

                if (glyphIndex == GLYPH_BETA_KEY) {
                    add_glyph_texture(GLYPH_U);
                    render_textrect(sTextLabels[i]->x, sTextLabels[i]->y, j);

                    add_glyph_texture(GLYPH_UMLAUT);
                    render_textrect(sTextLabels[i]->x, sTextLabels[i]->y + 3, j);
                } else {
                    add_glyph_texture(glyphIndex);
                    render_textrect(sTextLabels[i]->x, sTextLabels[i]->y, j);
                }
#elif defined(VERSION_CN)
                if ((u8) glyphIndex == 0xB0) {
                    add_glyph_texture(0x92);
                    render_textrect(45, 50, 0, 16);
                    add_glyph_texture(0x93);
                    render_textrect(45, 50, 1, 16);
                    add_glyph_texture(0x94);
                    render_textrect(45, 34, 0, 16);
                    add_glyph_texture(0x95);
                    render_textrect(45, 34, 1, 16);
                } else if ((u8) glyphIndex == 0xC0) {
                    add_glyph_texture(0xAE);
                    render_textrect(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(150), 193, 0, 16);
                    add_glyph_texture(0xAF);
                    render_textrect(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(150), 193, 1, 16);
                    add_glyph_texture(0xB0);
                    render_textrect(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(150), 177, 0, 16);
                    add_glyph_texture(0xB1);
                    render_textrect(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(150), 177, 1, 16);
                    add_glyph_texture(0xB2);
                    render_textrect(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(150), 193, 2, 16);
                    add_glyph_texture(0xB3);
                    render_textrect(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(150), 193, 3, 16);
                    add_glyph_texture(0xB4);
                    render_textrect(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(150), 177, 2, 16);
                    add_glyph_texture(0xB5);
                    render_textrect(GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(150), 177, 3, 16);
                } else {
                    add_glyph_texture(glyphIndex);
                    render_textrect(sTextLabels[i]->x, sTextLabels[i]->y, j, 12);
                }
#else
                add_glyph_texture(glyphIndex);
                render_textrect(sTextLabels[i]->x, sTextLabels[i]->y, j);
#endif
            }
        }

        mem_pool_free(gEffectsMemoryPool, sTextLabels[i]);
    }

    gSPDisplayList(gDisplayListHead++, dl_hud_img_end);

    sTextLabelsCount = 0;
}
