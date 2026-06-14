#include <stdio.h>
#include <PR/gbi.h>

#include "nds_include.h"
#include <nds/arm9/postest.h>

#include "nds_renderer.h"
#include "c_button.h"
#include "stick.h"
#include "stick_base_1.h"
#include "stick_base_2.h"
#include "game/memory.h"

#define BATCH_SIZE 96
#define SM64_LOGICAL_WIDTH 320
#define SM64_LOGICAL_HEIGHT 240
#define NDS_TOP_WIDTH 256
#define NDS_TOP_HEIGHT 192

static inline s32 clamp_s32(s32 value, s32 min, s32 max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

static inline s32 logical_x_to_ndc(s32 x_q2) {
    return x_q2 * (2 << 12) / (SM64_LOGICAL_WIDTH << 2) - (1 << 12);
}

static inline s32 logical_y_to_ndc(s32 y_q2) {
    return -(y_q2 * (2 << 12) / (SM64_LOGICAL_HEIGHT << 2) - (1 << 12));
}

struct Color {
    uint8_t r, g, b, a;
};

struct Texture {
    uint8_t *address;
    int name;
    uint8_t type;
    uint8_t size_x;
    uint8_t size_y;
};

struct Light {
    int16_t nx, ny, nz;
    int8_t x, y, z;
    uint8_t r, g, b;
};

DTCM_BSS static struct Color fill_color;
DTCM_BSS static struct Color fog_color;
DTCM_BSS static struct Color env_color;

DTCM_BSS static uint8_t tint_r, tint_g, tint_b;
DTCM_BSS static uint8_t tint_mode;
DTCM_BSS static bool tint_active;

DTCM_BSS static Vtx vertex_buffer[16];
static struct Texture texture_map[1024];
DTCM_BSS static struct Light lights[5];

static uint16_t texture_fifo[2048];
static uint16_t texture_fifo_start;
static uint16_t texture_fifo_end;

static uint8_t *texture_address;
DTCM_BSS static uint8_t texture_format;
DTCM_BSS static uint8_t texture_bit_width;
DTCM_BSS static uint16_t texture_row_size;
DTCM_BSS static uint16_t texture_size;
DTCM_BSS static uint16_t texture_scale_s;
DTCM_BSS static uint16_t texture_scale_t;

DTCM_BSS static uint32_t geometry_mode;
DTCM_BSS static uint32_t rdphalf_1;
DTCM_BSS static uint32_t other_mode_l;
DTCM_BSS static uint32_t other_mode_h;
DTCM_BSS static Gwords texrect;

DTCM_BSS static uint8_t *z_buffer;
DTCM_BSS static uint8_t *c_buffer;

DTCM_BSS static bool texture_dirty;
DTCM_BSS static bool lights_dirty;
DTCM_BSS static int num_lights;

DTCM_BSS static int polygon_id;
DTCM_BSS static int poly_fmt;
DTCM_BSS static int tex_params;

DTCM_BSS static bool use_color;
DTCM_BSS static bool use_texture;
DTCM_BSS static bool use_env_color;
DTCM_BSS static bool use_env_alpha;

DTCM_BSS static bool shrunk;
DTCM_BSS static bool background;
DTCM_BSS static int32_t z_depth;

DTCM_BSS static uint8_t fog_status;
DTCM_BSS static uint16_t fog_min;
DTCM_BSS static uint16_t fog_max;

DTCM_BSS static int no_texture;
DTCM_BSS static int frame_count;

DTCM_BSS static Vtx_t *vertex_batch[BATCH_SIZE];
DTCM_BSS static uint8_t batch_count;

u64 rspF3DStart[] = {};
u64 rspF3DBootStart[] = {};
u64 rspF3DBootEnd[] = {};
u64 rspF3DDataStart[] = {};

struct Sprite sprites[MAX_SPRITES];

struct {
    const void *texture;
    gl_texture_data *tex;
} glTexQueue[128];

static uint8_t glTexCount;
static void glTexSync();
static bool renderer_initialized;

static int glTexImage2DAsync(int target, int empty1, GL_TEXTURE_TYPE_ENUM type, int sizeX, int sizeY, int empty2, int param, const void *texture) {
    if (!glGlob->activeTexture)
        return 0;

    uint32_t size = 1 << (sizeX + sizeY + 6);
    uint32_t typeSizes[9] = { 0, 8, 2, 4, 8, 3, 8, 16, 16 };

    if (type == GL_RGBA)
        size <<= 1;
    else if (type == GL_NOTEXTURE)
        size = 0;
    else if (type != GL_RGB8_A5)
        return 0;

    gl_texture_data *tex = (gl_texture_data*)DynamicArrayGet(&glGlob->texturePtrs, glGlob->activeTexture);

    if (tex) {
        uint32_t texType = ((tex->texFormat >> 26) & 0x07);
        if ((tex->texSize != size) || (typeSizes[texType] != typeSizes[type])) {
            if(tex->texIndexExt)
                vramBlock_deallocateBlock(glGlob->vramBlocks[0], tex->texIndexExt);
            if(tex->texIndex)
                vramBlock_deallocateBlock(glGlob->vramBlocks[0], tex->texIndex);
            tex->texIndex = tex->texIndexExt = 0;
            tex->vramAddr = NULL;
        }
    }

    tex->texSize = size;

    if (!tex->texIndex) {
        if (type != GL_NOTEXTURE) {
            tex->texIndex = vramBlock_allocateBlock(glGlob->vramBlocks[0], tex->texSize, 3);
        }
        if (tex->texIndex) {
            tex->vramAddr = vramBlock_getAddr(glGlob->vramBlocks[0], tex->texIndex);
            tex->texFormat = (sizeX << 20) | (sizeY << 23) | (type << 26) | (((uint32_t)tex->vramAddr >> 3) & 0xFFFF);
        } else {
            tex->vramAddr = NULL;
            tex->texFormat = 0;
            return 0;
        }
    } else {
        tex->texFormat = (sizeX << 20) | (sizeY << 23) | (type << 26) | (tex->texFormat & 0xFFFF);
    }

    glTexParameter(target, param);

    if (type != GL_NOTEXTURE && texture) {
        if (glTexCount == 128)
            glTexSync();

        glTexQueue[glTexCount].texture = texture;
        glTexQueue[glTexCount].tex = tex;
        glTexCount++;
    }

    return 1;
}

static void glTexSync() {

    for (size_t i = 0; i < glTexCount; i++) {
        const void *texture = glTexQueue[i].texture;
        gl_texture_data *tex = glTexQueue[i].tex;

        uint32_t vramTemp = VRAM_CR;
        uint16_t *startBank = vramGetBank((uint16_t*)tex->vramAddr);
        uint16_t *endBank = vramGetBank((uint16_t*)((char*)tex->vramAddr + tex->texSize - 1));

        do {
            if (startBank == VRAM_A)
                vramSetBankA(VRAM_A_LCD);
            else if (startBank == VRAM_B)
                vramSetBankB(VRAM_B_LCD);
            else if (startBank == VRAM_C)
                vramSetBankC(VRAM_C_LCD);
            else if (startBank == VRAM_D)
                vramSetBankD(VRAM_D_LCD);
            startBank += 0x10000;
        } while (startBank <= endBank);

        dmaCopyWords(0, texture, tex->vramAddr, tex->texSize);

        vramRestorePrimaryBanks(vramTemp);
    }

    glTexCount = 0;
}

void renderer_invalidate_texture_cache(void) {
    if (renderer_initialized && glTexCount > 0) {
        glTexSync();
    }

    for (int i = 0; i < 1024; i++) {
        if (renderer_initialized && texture_map[i].name != 0) {
            glDeleteTextures(1, &texture_map[i].name);
        }
        texture_map[i].address = NULL;
        texture_map[i].name = 0;
        texture_map[i].type = 0;
        texture_map[i].size_x = 0;
        texture_map[i].size_y = 0;
    }

    texture_fifo_start = 0;
    texture_fifo_end = 0;
}

static void load_texture() {

    uint32_t index = ((uint32_t)texture_address >> 5) & 0x3FF;
    while (texture_map[index].address != texture_address && texture_map[index].address != NULL) {
        index = (index + 1) & 0x3FF;
    }

    struct Texture *cur = &texture_map[index];

    if (cur->address != NULL) {
        if (cur->name) {
            glBindTexture(GL_TEXTURE_2D, cur->name);
            return;
        }

        glGenTextures(1, &cur->name);
        glBindTexture(GL_TEXTURE_2D, cur->name);
        while (!glTexImage2DAsync(GL_TEXTURE_2D, 0, cur->type, cur->size_x, cur->size_y, 0, TEXGEN_TEXCOORD, cur->address)) {
            glDeleteTextures(1, &texture_map[texture_fifo[texture_fifo_end]].name);
            texture_map[texture_fifo[texture_fifo_end]].name = 0;
            texture_fifo_end = (texture_fifo_end + 1) & 0x7FF;
        }
        texture_fifo[texture_fifo_start] = index;
        texture_fifo_start = (texture_fifo_start + 1) & 0x7FF;
        return;
    }

    cur->address = texture_address;

    switch (texture_format) {
        case G_IM_FMT_RGBA: cur->type = GL_RGBA;    break;
        case G_IM_FMT_IA:   cur->type = GL_RGB8_A5; break;

        default:

            glBindTexture(GL_TEXTURE_2D, cur->name = no_texture);
            return;
    }

    const int width = texture_row_size << (4 - texture_bit_width);
    const int height = ((texture_size << 1) >> texture_bit_width) / width;
    for (cur->size_x = 0; (width  - 1) >> (cur->size_x + 3) != 0; cur->size_x++);
    for (cur->size_y = 0; (height - 1) >> (cur->size_y + 3) != 0; cur->size_y++);

    glGenTextures(1, &cur->name);
    glBindTexture(GL_TEXTURE_2D, cur->name);
    while (!glTexImage2DAsync(GL_TEXTURE_2D, 0, cur->type, cur->size_x, cur->size_y, 0, TEXGEN_TEXCOORD, cur->address)) {
        glDeleteTextures(1, &texture_map[texture_fifo[texture_fifo_end]].name);
        texture_map[texture_fifo[texture_fifo_end]].name = 0;
        texture_fifo_end = (texture_fifo_end + 1) & 0x7FF;
    }
    texture_fifo[texture_fifo_start] = index;
    texture_fifo_start = (texture_fifo_start + 1) & 0x7FF;
}

static inline void gl_color_tinted(int r, int g, int b) {
    if (tint_active) {
        if (tint_mode == 2) {
            glColor3b(tint_r, tint_g, tint_b);
        } else {
            glColor3b((r + tint_r) >> 1, (g + tint_g) >> 1, (b + tint_b) >> 1);
        }
    } else {
        glColor3b(r, g, b);
    }
}

ITCM_CODE static void draw_vertices(const Vtx_t **v, int count) {

    const int alpha = ((other_mode_l & (G_BL_A_MEM << 18)) ? 31 : ((use_env_alpha ? env_color.a : v[0]->cn[3]) >> 3));
    if (alpha == 0) return;

    const uint8_t tex_ofs = ((other_mode_h & (3 << G_MDSFT_TEXTFILT)) == G_TF_POINT) ? 0 : (1 << 4);

    if (use_env_color) {
        gl_color_tinted(env_color.r, env_color.g, env_color.b);
    } else if (!use_color) {
        gl_color_tinted(0xFF, 0xFF, 0xFF);
    }

    if (!use_texture) {
        glBindTexture(GL_TEXTURE_2D, no_texture);
        texture_dirty = true;
    } else if (texture_dirty) {
        load_texture();
        glTexParameter(GL_TEXTURE_2D, tex_params);
        texture_dirty = false;
    }

    if (geometry_mode & G_ZBUFFER) {

        int fmt = poly_fmt | POLY_ALPHA(alpha) | POLY_ID(polygon_id);
        if ((geometry_mode & G_FOG) || (((glGetTexParameter() >> 26) & 0x7) == GL_RGB8_A5 && alpha < 31))
            fmt |= POLY_FOG;

        glPolyFmt(fmt);
        glBegin(GL_TRIANGLE);

        if (!shrunk) {
            const m4x4 shrink = {{
                1 << 12, 0, 0, 0,
                0, 1 << 12, 0, 0,
                0, 0, 1 << 12, 0,
                0, 0, 0, 1 <<  0
            }};
            glMatrixMode(GL_MODELVIEW);
            glMultMatrix4x4(&shrink);
            shrunk = true;
        }

        if ((other_mode_l & ZMODE_DEC) == ZMODE_DEC) {
            for (int i = 0; i < count; i++) {

                if (use_color) gl_color_tinted(v[i]->cn[0], v[i]->cn[1], v[i]->cn[2]);
                if (use_texture) glTexCoord2t16(((v[i]->tc[0] * texture_scale_s) >> 17) + tex_ofs, ((v[i]->tc[1] * texture_scale_t) >> 17) + tex_ofs);

                PosTest(v[i]->ob[0], v[i]->ob[1], v[i]->ob[2]);

                glMatrixMode(GL_MODELVIEW);
                glPushMatrix();
                glLoadIdentity();
                glMatrixMode(GL_PROJECTION);
                glPushMatrix();

                const m4x4 vertex = {{
                    PosTestXresult(), 0, 0, 0,
                    0, PosTestYresult(), 0, 0,
                    0, 0, PosTestZresult() - (3 << 4), 0,
                    0, 0, 0, PosTestWresult()
                }};
                glLoadMatrix4x4(&vertex);
                glVertex3v16(1 << 12, 1 << 12, 1 << 12);

                glPopMatrix(1);
                glMatrixMode(GL_MODELVIEW);
                glPopMatrix(1);
            }
        } else {

            if (__builtin_expect((use_color), true)) {
                int lr = -1, lg = -1, lb = -1;
                if (use_texture) {
                    for (int i = 0; i < count; i++) {
                        if (v[i]->cn[0] != lr || v[i]->cn[1] != lg || v[i]->cn[2] != lb) {
                            gl_color_tinted(v[i]->cn[0], v[i]->cn[1], v[i]->cn[2]);
                            lr = v[i]->cn[0]; lg = v[i]->cn[1]; lb = v[i]->cn[2];
                        }
                        glTexCoord2t16(((v[i]->tc[0] * texture_scale_s) >> 17) + tex_ofs, ((v[i]->tc[1] * texture_scale_t) >> 17) + tex_ofs);
                        glVertex3v16(v[i]->ob[0], v[i]->ob[1], v[i]->ob[2]);
                    }
                } else {
                    for (int i = 0; i < count; i++) {
                        if (v[i]->cn[0] != lr || v[i]->cn[1] != lg || v[i]->cn[2] != lb) {
                            gl_color_tinted(v[i]->cn[0], v[i]->cn[1], v[i]->cn[2]);
                            lr = v[i]->cn[0]; lg = v[i]->cn[1]; lb = v[i]->cn[2];
                        }
                        glVertex3v16(v[i]->ob[0], v[i]->ob[1], v[i]->ob[2]);
                    }
                }
            } else {
                if (use_texture) {
                    for (int i = 0; i < count; i++) {
                        glTexCoord2t16(((v[i]->tc[0] * texture_scale_s) >> 17) + tex_ofs, ((v[i]->tc[1] * texture_scale_t) >> 17) + tex_ofs);
                        glVertex3v16(v[i]->ob[0], v[i]->ob[1], v[i]->ob[2]);
                    }
                }
            }
        }

        if (background) {
            z_depth = (128 - 0x1000) * 6;
            background = false;
        }
    } else {

        glPolyFmt(poly_fmt | POLY_ALPHA(alpha) | POLY_ID(polygon_id));
        glBegin(GL_TRIANGLE);

        const m4x4 enlarge = {{
            1 << 24, 0, 0, 0,
            0, 1 << 24, 0, 0,
            0, 0, 1 << 24, 0,
            0, 0, 0, 1 << (shrunk ? 24 : 12)
        }};
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glMultMatrix4x4(&enlarge);

        const int hijack_z = ((--z_depth) / 6) << 4;

        for (int i = 0; i < count; i++) {

            if (use_color) gl_color_tinted(v[i]->cn[0], v[i]->cn[1], v[i]->cn[2]);
            if (use_texture) glTexCoord2t16(((v[i]->tc[0] * texture_scale_s) >> 17) + tex_ofs, ((v[i]->tc[1] * texture_scale_t) >> 17) + tex_ofs);

            PosTest(v[i]->ob[0], v[i]->ob[1], v[i]->ob[2]);

            glPushMatrix();
            glLoadIdentity();
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();

            const m4x4 vertex = {{
                PosTestXresult(), 0, 0, 0,
                0, PosTestYresult(), 0, 0,
                0, 0, hijack_z, 0,
                0, 0, 0, PosTestWresult()
            }};
            glLoadMatrix4x4(&vertex);
            glVertex3v16(1 << 12, 1 << 12, 1 << 12);

            glPopMatrix(1);
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix(1);
        }

        glPopMatrix(1);
    }
}

ITCM_CODE static void g_vtx(Gwords *words) {
    const uint8_t count = ((words->w0 >> 12) & 0xFF);
    const uint8_t index = ((words->w0 >>  0) & 0xFF) >> 1;
    const Vtx *vertices = (const Vtx*)segmented_to_virtual((const void*)words->w1);

    memcpy(&vertex_buffer[index - count], vertices, count * sizeof(Vtx));

    if (geometry_mode & G_LIGHTING) {

        if (lights_dirty) {

            int m[12];
            glGetFixed(GL_GET_MATRIX_VECTOR, m);

            for (int i = 0; i < num_lights; i++) {

                lights[i].nx = (lights[i].x * m[0] + lights[i].y * m[1] + lights[i].z * m[2]) >> 7;
                lights[i].ny = (lights[i].x * m[3] + lights[i].y * m[4] + lights[i].z * m[5]) >> 7;
                lights[i].nz = (lights[i].x * m[6] + lights[i].y * m[7] + lights[i].z * m[8]) >> 7;

                int s = (lights[i].nx * lights[i].nx + lights[i].ny * lights[i].ny + lights[i].nz * lights[i].nz) >> 8;
                if (s > 0) {
                    s = sqrt64((s64)s << 16);
                    lights[i].nx = (lights[i].nx << 16) / s;
                    lights[i].ny = (lights[i].ny << 16) / s;
                    lights[i].nz = (lights[i].nz << 16) / s;
                }
            }

            lights_dirty = false;
        }

        for (int i = index - count; i < index; i++) {
            Vtx_t  *v = &vertex_buffer[i].v;
            Vtx_tn *n = &vertex_buffer[i].n;

            uint32_t r = lights[num_lights].r;
            uint32_t g = lights[num_lights].g;
            uint32_t b = lights[num_lights].b;

            for (int i = 2; i < num_lights; i++) {
                int intensity = (lights[i].nx * n->n[0] + lights[i].ny * n->n[1] + lights[i].nz * n->n[2]) >> 7;
                if (intensity > 0) {
                    r += (intensity * lights[i].r) >> 12;
                    g += (intensity * lights[i].g) >> 12;
                    b += (intensity * lights[i].b) >> 12;
                }
            }

            if (geometry_mode & G_TEXTURE_GEN) {
                v->tc[0] = ((lights[1].nx * n->n[0] + lights[1].ny * n->n[1] + lights[1].nz * n->n[2]) >> 5) + (1 << 14);
                v->tc[1] = ((lights[0].nx * n->n[0] + lights[0].ny * n->n[1] + lights[0].nz * n->n[2]) >> 5) + (1 << 14);
            }

            v->cn[0] = (r > 0xFF) ? 0xFF : r;
            v->cn[1] = (g > 0xFF) ? 0xFF : g;
            v->cn[2] = (b > 0xFF) ? 0xFF : b;
        }
    }
}

ITCM_CODE static void g_tri1(Gwords *words) {

    vertex_batch[batch_count++] = &vertex_buffer[((words->w0 >> 16) & 0xFF) >> 1].v;
    vertex_batch[batch_count++] = &vertex_buffer[((words->w0 >>  8) & 0xFF) >> 1].v;
    vertex_batch[batch_count++] = &vertex_buffer[((words->w0 >>  0) & 0xFF) >> 1].v;
}

ITCM_CODE static void g_tri2(Gwords *words) {

    vertex_batch[batch_count++] = &vertex_buffer[((words->w0 >> 16) & 0xFF) >> 1].v;
    vertex_batch[batch_count++] = &vertex_buffer[((words->w0 >>  8) & 0xFF) >> 1].v;
    vertex_batch[batch_count++] = &vertex_buffer[((words->w0 >>  0) & 0xFF) >> 1].v;
    vertex_batch[batch_count++] = &vertex_buffer[((words->w1 >> 16) & 0xFF) >> 1].v;
    vertex_batch[batch_count++] = &vertex_buffer[((words->w1 >>  8) & 0xFF) >> 1].v;
    vertex_batch[batch_count++] = &vertex_buffer[((words->w1 >>  0) & 0xFF) >> 1].v;
}

static void g_texture(Gwords *words) {

    texture_scale_s = (words->w1 >> 16) & 0xFFFF;
    texture_scale_t = (words->w1 >>  0) & 0xFFFF;
}

static void g_popmtx(Gwords *words) {

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix(words->w1 / 64);
}

static void g_geometrymode(Gwords *words) {

    geometry_mode = (geometry_mode & words->w0) | words->w1;

    poly_fmt |= POLY_CULL_NONE;
    if (geometry_mode & (1 << 9)) {
        poly_fmt &= ~POLY_CULL_BACK;
    }
    if (geometry_mode & (1 << 10)) {
        poly_fmt &= ~POLY_CULL_FRONT;
    }
}

ITCM_CODE static void g_mtx(Gwords *words) {

    m4x4 matrix;
    for (int i = 0; i < 16; i += 2) {
        const uint32_t *data = &((uint32_t*)segmented_to_virtual((const void*)words->w1))[i / 2];
        matrix.m[i + 0] = (int32_t)((data[0] & 0xFFFF0000) | (data[8] >> 16));
        matrix.m[i + 1] = (int32_t)((data[0] << 16) | (data[8] & 0x0000FFFF));
    }

    const uint8_t params = words->w0 ^ G_MTX_PUSH;
    if (params & G_MTX_PROJECTION) {
        glMatrixMode(GL_PROJECTION);

        if (params & G_MTX_LOAD) {
            glLoadMatrix4x4(&matrix);
        } else {

            const m4x4 shrink = {{
                1 << 8, 0, 0, 0,
                0, 1 << 8, 0, 0,
                0, 0, 1 << 8, 0,
                0, 0, 0, 1 << 8
            }};
            glMultMatrix4x4(&shrink);

            glMultMatrix4x4(&matrix);
        }
    } else {
        glMatrixMode(GL_MODELVIEW);

        if (params & G_MTX_PUSH) {
            glPushMatrix();
        }

        for (int i = 0; i < 16; i++) {
            matrix.m[i] >>= 4;
        }

        if (params & G_MTX_LOAD) {
            glLoadMatrix4x4(&matrix);
        } else {

            if (shrunk) {
                const m4x4 enlarge = {{
                    1 << 12, 0, 0, 0,
                    0, 1 << 12, 0, 0,
                    0, 0, 1 << 12, 0,
                    0, 0, 0, 1 << 24
                }};
                glMultMatrix4x4(&enlarge);
            }

            glMultMatrix4x4(&matrix);
        }

        shrunk = false;
        lights_dirty = true;
    }
}

static void g_moveword(Gwords *words) {

    const uint8_t index = (words->w0 >> 16) & 0xFF;
    switch (index) {
        case G_MW_NUMLIGHT:

            num_lights = (words->w1 / 24) + 2;
            break;

        case G_MW_FOG:
            if (fog_status < 2) {

                int16_t mul = words->w1 >> 16;
                int16_t ofs = words->w1 >>  0;
                uint16_t min = 500 - ofs * 500 / mul;
                uint16_t max = 128000 / mul + min;

                if (fog_status == 0 || fog_min != min || fog_max != max)
                {
                    fog_status++;
                    fog_min = min;
                    fog_max = max;
                }
            }
            break;

        case G_MW_CLIP:      break;
        case G_MW_PERSPNORM: break;

        default:

            break;
    }
}

ITCM_CODE static void g_movemem(Gwords *words) {

    const uint8_t index = (words->w0 >> 0) & 0xFF;
    switch (index) {
        case G_MV_VIEWPORT: {

            const Vp_t *vp = (Vp_t*)segmented_to_virtual((const void*)words->w1);
            const s32 vp_left = (vp->vtrans[0] - vp->vscale[0]) >> 2;
            const s32 vp_right = (vp->vtrans[0] + vp->vscale[0]) >> 2;
            const s32 vp_bottom = (vp->vtrans[1] - vp->vscale[1]) >> 2;
            const s32 vp_top = (vp->vtrans[1] + vp->vscale[1]) >> 2;

            s32 x1 = (vp_left * (NDS_TOP_WIDTH - 1)) / SM64_LOGICAL_WIDTH;
            s32 x2 = (vp_right * (NDS_TOP_WIDTH - 1)) / SM64_LOGICAL_WIDTH;
            s32 y1 = (vp_bottom * (NDS_TOP_HEIGHT - 1)) / SM64_LOGICAL_HEIGHT;
            s32 y2 = (vp_top * (NDS_TOP_HEIGHT - 1)) / SM64_LOGICAL_HEIGHT;

            x1 = clamp_s32(x1, 0, NDS_TOP_WIDTH - 1);
            x2 = clamp_s32(x2, 0, NDS_TOP_WIDTH - 1);
            y1 = clamp_s32(y1, 0, NDS_TOP_HEIGHT - 1);
            y2 = clamp_s32(y2, 0, NDS_TOP_HEIGHT - 1);

            if (x2 < x1) {
                s32 tmp = x1;
                x1 = x2;
                x2 = tmp;
            }
            if (y2 < y1) {
                s32 tmp = y1;
                y1 = y2;
                y2 = tmp;
            }

            glViewport((u8)x1, (u8)y1, (u8)x2, (u8)y2);
            break;
        }

        case G_MV_LIGHT: {

            const uint8_t index = ((words->w0 >> 8) & 0xFF) / 3;
            const Light_t *src = (Light_t*)segmented_to_virtual((const void*)words->w1);
            struct Light *dst = &lights[index];
            if (index >= 2) {
                dst->r = src->col[0];
                dst->g = src->col[1];
                dst->b = src->col[2];
            }
            if (index < num_lights &&

                (dst->x != src->dir[0] || dst->y != src->dir[1] || dst->z != src->dir[2])) {
                dst->x = src->dir[0];
                dst->y = src->dir[1];
                dst->z = src->dir[2];
                lights_dirty = true;
            }
            break;
        }

        default:

            break;
    }
}

static void g_rdphalf_1(Gwords *words) {

    rdphalf_1 = words->w1;
}

static void g_setothermode_l(Gwords *words) {

    const uint8_t bits = ((words->w0 >> 0) & 0xFF) + 1;
    const uint8_t shift = 32 - ((words->w0 >> 8) & 0xFF) - bits;
    const uint32_t mask = ((1 << bits) - 1) << shift;
    other_mode_l = (other_mode_l & ~mask) | (words->w1 & mask);
}

static void g_setothermode_h(Gwords *words) {

    const uint8_t bits = ((words->w0 >> 0) & 0xFF) + 1;
    const uint8_t shift = 32 - ((words->w0 >> 8) & 0xFF) - bits;
    const uint32_t mask = ((1 << bits) - 1) << shift;
    other_mode_h = (other_mode_h & ~mask) | (words->w1 & mask);
}

static void g_texrect(Gwords *words) {

    texrect = *words;
}

ITCM_CODE static void g_rdphalf_2(Gwords *words) {

    const int alpha = (use_env_alpha ? (env_color.a >> 3) : 31);
    if (alpha == 0) return;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    if (texture_dirty) {
        load_texture();
        glTexParameter(GL_TEXTURE_2D, tex_params);
        texture_dirty = false;
    }

    glPolyFmt(POLY_CULL_NONE | POLY_ALPHA(alpha));
    glBegin(GL_TRIANGLE);

    const bool copy = ((other_mode_h & (3 << G_MDSFT_CYCLETYPE)) == G_CYC_COPY);

    if (use_env_color && !copy) {
        glColor3b(env_color.r, env_color.g, env_color.b);
    } else {
        glColor3b(0xFF, 0xFF, 0xFF);
    }

    int16_t x1 = ((texrect.w1 >> 12) & 0xFFF);
    int16_t y1 = ((texrect.w1 >>  0) & 0xFFF);
    int16_t x2 = ((texrect.w0 >> 12) & 0xFFF) + (copy ? (1 << 2) : 0);
    int16_t y2 = ((texrect.w0 >>  0) & 0xFFF) + (copy ? (1 << 2) : 0);

    const int16_t s1 = (((rdphalf_1 >> 16) & 0xFFFF) >> 1);
    const int16_t t1 = (((rdphalf_1 >>  0) & 0xFFFF) >> 1);
    const int16_t s2 = s1 + ((((words->w1 >> 16) & 0xFFFF) * (x2 - x1)) >> (copy ? 10 : 8));
    const int16_t t2 = t1 + ((((words->w1 >>  0) & 0xFFFF) * (y2 - y1)) >> 8);

    x1 = logical_x_to_ndc(x1);
    y1 = logical_y_to_ndc(y1);
    x2 = logical_x_to_ndc(x2);
    y2 = logical_y_to_ndc(y2);

    const int rect_z = (--z_depth) / 6;
    glTexCoord2t16(s1, t1);
    glVertex3v16(x1, y1, rect_z);
    glTexCoord2t16(s1, t2);
    glVertex3v16(x1, y2, rect_z);
    glTexCoord2t16(s2, t1);
    glVertex3v16(x2, y1, rect_z);

    glTexCoord2t16(s2, t1);
    glVertex3v16(x2, y1, rect_z);
    glTexCoord2t16(s1, t2);
    glVertex3v16(x1, y2, rect_z);
    glTexCoord2t16(s2, t2);
    glVertex3v16(x2, y2, rect_z);

    glPopMatrix(1);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix(1);
}

static void g_loadblock(Gwords *words) {
    const int tile = (words->w1 >> 24) & 0x07;
    if (tile != G_TX_LOADTILE) return;

    texture_size = (((words->w1 >> 12) & 0xFFF) + 1);
    switch (texture_bit_width) {
        case G_IM_SIZ_4b:  texture_size >>= 1; break;
        case G_IM_SIZ_16b: texture_size <<= 1; break;
    }
}

static void g_settile(Gwords *words) {
    const int tile = (words->w1 >> 24) & 0x07;
    if (tile != G_TX_RENDERTILE) return;

    texture_format    = (words->w0 >> 21) & 0x007;
    texture_bit_width = (words->w0 >> 19) & 0x003;
    texture_row_size  = (words->w0 >>  9) & 0x1FF;
    const uint8_t cms = (words->w1 >>  8) & 0x003;
    const uint8_t cmt = (words->w1 >> 18) & 0x003;

    tex_params = 0;
    if (!(cms & G_TX_CLAMP)) {
        tex_params |= GL_TEXTURE_WRAP_S;
        if (cms & G_TX_MIRROR) {
            tex_params |= GL_TEXTURE_FLIP_S;
        }
    }
    if (!(cmt & G_TX_CLAMP)) {
        tex_params |= GL_TEXTURE_WRAP_T;
        if (cmt & G_TX_MIRROR) {
            tex_params |= GL_TEXTURE_FLIP_T;
        }
    }
}

static void g_fillrect(Gwords *words) {

    const u32 raw = ((u32) fill_color.r << 24) | ((u32) fill_color.g << 16)
                  | ((u32) fill_color.b << 8)  |  (u32) fill_color.a;
    if (raw == (u32) ((GPACK_ZDZ(G_MAXFBZ, 0) << 16) | GPACK_ZDZ(G_MAXFBZ, 0))) return;

    const int alpha = fill_color.a >> 3;
    if (alpha == 0) return;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glBindTexture(GL_TEXTURE_2D, no_texture);
    texture_dirty = true;

    glPolyFmt(POLY_CULL_NONE | POLY_ALPHA(alpha));
    glBegin(GL_TRIANGLE);
    glColor3b(fill_color.r, fill_color.g, fill_color.b);

    const int16_t x1 = logical_x_to_ndc(((words->w1 >> 12) & 0xFFF) + (0 << 2));
    const int16_t y1 = logical_y_to_ndc(((words->w1 >>  0) & 0xFFF) + (0 << 2));
    const int16_t x2 = logical_x_to_ndc(((words->w0 >> 12) & 0xFFF) + (1 << 2));
    const int16_t y2 = logical_y_to_ndc(((words->w0 >>  0) & 0xFFF) + (1 << 2));

    glVertex3v16(x1, y1, (--z_depth) / 6);
    glVertex3v16(x1, y2, (--z_depth) / 6);
    glVertex3v16(x2, y1, (--z_depth) / 6);

    glVertex3v16(x2, y1, (--z_depth) / 6);
    glVertex3v16(x1, y2, (--z_depth) / 6);
    glVertex3v16(x2, y2, (--z_depth) / 6);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix(1);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix(1);
}

static void g_setfillcolor(Gwords *words) {

    fill_color.r = (words->w1 >> 24) & 0xFF;
    fill_color.g = (words->w1 >> 16) & 0xFF;
    fill_color.b = (words->w1 >>  8) & 0xFF;
    fill_color.a = (words->w1 >>  0) & 0xFF;
}

static void g_setfogcolor(Gwords *words) {

    if (fog_status < 2) {
        fog_color.r = (words->w1 >> 24) & 0xFF;
        fog_color.g = (words->w1 >> 16) & 0xFF;
        fog_color.b = (words->w1 >>  8) & 0xFF;
        fog_color.a = (words->w1 >>  0) & 0xFF;
    }
}

static void g_setenvcolor(Gwords *words) {

    env_color.r = (words->w1 >> 24) & 0xFF;
    env_color.g = (words->w1 >> 16) & 0xFF;
    env_color.b = (words->w1 >>  8) & 0xFF;
    env_color.a = (words->w1 >>  0) & 0xFF;
}

static void g_setcombine(Gwords *words) {
    const uint8_t a_color = (words->w0 >> 20) & 0x0F;
    const uint8_t b_color = (words->w1 >> 28) & 0x0F;
    const uint8_t c_color = (words->w0 >> 15) & 0x1F;
    const uint8_t d_color = (words->w1 >> 15) & 0x07;

    const uint8_t c_alpha = (words->w0 >>  9) & 0x07;
    const uint8_t d_alpha = (words->w1 >>  9) & 0x07;

    use_env_color = (c_color == G_CCMUX_ENVIRONMENT || d_color == G_CCMUX_ENVIRONMENT);
    use_env_alpha = (c_alpha == G_CCMUX_ENVIRONMENT || d_alpha == G_CCMUX_ENVIRONMENT);
    use_color = !use_env_color && (a_color == G_CCMUX_SHADE || b_color == G_CCMUX_SHADE || c_color == G_CCMUX_SHADE || d_color == G_CCMUX_SHADE);
    use_texture = (a_color == G_CCMUX_TEXEL0 || b_color == G_CCMUX_TEXEL0 || c_color == G_CCMUX_TEXEL0 || d_color == G_CCMUX_TEXEL0);

    if (b_color == d_color) {
        poly_fmt |= POLY_DECAL;

        if (a_color == G_CCMUX_PRIMITIVE) {
            use_texture = false;
        }
    } else {
        poly_fmt &= ~POLY_DECAL;
    }

    polygon_id = (polygon_id + 1) & 0x3F;
}

static void g_settimg(Gwords *words) {

    texture_address = (uint8_t*)segmented_to_virtual((const void*)words->w1);
    texture_format = (words->w0 >> 21) & 0x07;
    texture_bit_width = (words->w0 >> 19) & 0x03;
    texture_dirty = true;
}

static void g_setzimg(Gwords *words) {

    z_buffer = (uint8_t*)segmented_to_virtual((const void*)words->w1);
}

static void g_setcimg(Gwords *words) {

    c_buffer = (uint8_t*)segmented_to_virtual((const void*)words->w1);
}

ITCM_CODE static bool valid_dl_ptr(const Gfx *cmd) {
    const uintptr_t ptr = (uintptr_t) cmd;
    return ptr >= 0x02000000 && ptr <= 0x023FFFF8;
}

ITCM_CODE static void execute(Gfx* cmd) {

    int cmd_budget = 20000;

    if (!valid_dl_ptr(cmd)) {
        return;
    }

    while (true) {
        if (!valid_dl_ptr(cmd) || --cmd_budget <= 0) {
            return;
        }

        const uint8_t opcode = cmd->words.w0 >> 24;

        if ((opcode != G_TRI1 && opcode != G_TRI2 && batch_count > 0) || batch_count > BATCH_SIZE - 6) {
            draw_vertices(vertex_batch, batch_count);
            batch_count = 0;
        }

        switch (opcode) {
            case G_VTX:            g_vtx(&cmd->words);            break;
            case G_TRI1:           g_tri1(&cmd->words);           break;
            case G_TRI2:           g_tri2(&cmd->words);           break;
            case G_TEXTURE:        g_texture(&cmd->words);        break;
            case G_POPMTX:         g_popmtx(&cmd->words);         break;
            case G_GEOMETRYMODE:   g_geometrymode(&cmd->words);   break;
            case G_MTX:            g_mtx(&cmd->words);            break;
            case G_MOVEWORD:       g_moveword(&cmd->words);       break;
            case G_MOVEMEM:        g_movemem(&cmd->words);        break;
            case G_RDPHALF_1:      g_rdphalf_1(&cmd->words);      break;
            case G_SETOTHERMODE_L: g_setothermode_l(&cmd->words); break;
            case G_SETOTHERMODE_H: g_setothermode_h(&cmd->words); break;
            case G_TEXRECT:        g_texrect(&cmd->words);        break;
            case G_RDPHALF_2:      g_rdphalf_2(&cmd->words);      break;
            case G_LOADBLOCK:      g_loadblock(&cmd->words);      break;
            case G_SETTILE:        g_settile(&cmd->words);        break;
            case G_FILLRECT:       g_fillrect(&cmd->words);       break;
            case G_SETFILLCOLOR:   g_setfillcolor(&cmd->words);   break;
            case G_SETFOGCOLOR:    g_setfogcolor(&cmd->words);    break;
            case G_SETENVCOLOR:    g_setenvcolor(&cmd->words);    break;
            case G_SETCOMBINE:     g_setcombine(&cmd->words);     break;
            case G_SETTIMG:        g_settimg(&cmd->words);        break;
            case G_SETZIMG:        g_setzimg(&cmd->words);        break;
            case G_SETCIMG:        g_setcimg(&cmd->words);        break;

            case G_RDPLOADSYNC: break;
            case G_RDPPIPESYNC: break;
            case G_RDPTILESYNC: break;
            case G_RDPFULLSYNC: break;

            case G_SETSCISSOR:    break;
            case G_SETTILESIZE:   break;
            case G_SETBLENDCOLOR: break;
            case G_SETPRIMCOLOR:  break;

            case G_SPECIAL_1: {
                uint32_t mode = cmd->words.w0 & 0xFF;
                uint32_t c = cmd->words.w1;
                tint_r = (c >> 16) & 0xFF;
                tint_g = (c >>  8) & 0xFF;
                tint_b =  c        & 0xFF;
                tint_mode = mode;
                tint_active = (mode != 0);
                break;
            }

            case G_DL:

                if (cmd->words.w0 & (1 << 16)) {
                    Gfx *next = (Gfx*) segmented_to_virtual((const void*) cmd->words.w1);
                    if (!valid_dl_ptr(next)) {
                        return;
                    }
                    cmd = next;
                    continue;
                } else {
                    Gfx *next = (Gfx*) segmented_to_virtual((const void*) cmd->words.w1);
                    if (valid_dl_ptr(next)) {
                        execute(next);
                    }
                    break;
                }

            case G_ENDDL:

                return;

            default:

                break;
        }

        cmd++;
    }
}

static void end_frame() {

    frame_count++;

    if (glTexCount > 0) glTexSync();
    oamUpdate(&oamSub);
}

static uint16_t *bitmap_init_4bpp(const uint32_t *bitmap, int palRow) {
    const uint16_t *src = (const uint16_t *) bitmap;
    uint16_t *gfx = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);
    static uint8_t tiles[64 * 64 / 2];
    uint16_t pal[16];
    int palCount = 1;

    memset(tiles, 0, sizeof(tiles));
    pal[0] = 0;

    for (int px = 0; px < 64 * 64; px++) {
        uint16_t c = src[px];
        uint8_t idx = 0;
        if (c & BIT(15)) {
            int found = 0;
            for (int i = 1; i < palCount; i++) {
                if (pal[i] == c) {
                    idx = i;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (palCount < 16) {
                    pal[palCount] = c;
                    idx = palCount++;
                } else {

                    int best = 1, bestDist = 0x7FFFFFFF;
                    int r = (c >> 0) & 0x1F, g = (c >> 5) & 0x1F, b = (c >> 10) & 0x1F;
                    for (int i = 1; i < 16; i++) {
                        int dr = r - ((pal[i] >> 0) & 0x1F);
                        int dg = g - ((pal[i] >> 5) & 0x1F);
                        int db = b - ((pal[i] >> 10) & 0x1F);
                        int dist = dr * dr + dg * dg + db * db;
                        if (dist < bestDist) {
                            bestDist = dist;
                            best = i;
                        }
                    }
                    idx = best;
                }
            }
        }
        {
            int x = px & 63, y = px >> 6;
            int tile = (y >> 3) * 8 + (x >> 3);
            int ofs = tile * 32 + (y & 7) * 4 + ((x & 7) >> 1);
            if (x & 1) {
                tiles[ofs] |= idx << 4;
            } else {
                tiles[ofs] |= idx;
            }
        }
    }

    for (int i = 0; i < 16; i++) {
        SPRITE_PALETTE_SUB[palRow * 16 + i] = (i < palCount) ? pal[i] : 0;
    }
    if (gfx) {

        DC_FlushRange(tiles, sizeof(tiles));
        dmaCopy(tiles, gfx, sizeof(tiles));
    }
    return gfx;
}

static void palette_make_pressed(int srcRow, int dstRow) {
    for (int i = 0; i < 16; i++) {
        uint16_t c = SPRITE_PALETTE_SUB[srcRow * 16 + i];
        uint8_t r = ((c >> 10) & 0x1F) * 3 / 4;
        uint8_t g = ((c >> 5) & 0x1F) * 3 / 4;
        uint8_t b = ((c >> 0) & 0x1F) * 3 / 4;
        SPRITE_PALETTE_SUB[dstRow * 16 + i] = (c & BIT(15)) | (r << 10) | (g << 5) | b;
    }
}

void renderer_init() {

    videoSetMode(MODE_0_3D);
    videoSetModeSub(MODE_0_2D);

#ifdef ENABLE_FPS

    consoleDemoInit();
#endif

    glInit();
    glClearColor(0, 0, 0, 31);
    glClearDepth(GL_MAX_DEPTH);
    glEnable(GL_ANTIALIAS);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);

    BG_PALETTE[0x200] = ARGB16(1, 15, 16, 17);
    oamInit(&oamSub, SpriteMapping_1D_64, false);
    oamClear(&oamSub, 0, 0);

    vramSetBankA(VRAM_A_TEXTURE);
    vramSetBankB(VRAM_B_TEXTURE);
    vramSetBankI(VRAM_I_SUB_SPRITE);
    vramSetBankE(VRAM_E_TEX_PALETTE);

    glGenTextures(1, &no_texture);
    glBindTexture(GL_TEXTURE_2D, no_texture);
    glTexImage2DAsync(GL_TEXTURE_2D, 0, GL_NOTEXTURE, 0, 0, 0, TEXGEN_TEXCOORD, NULL);
    renderer_initialized = true;

    uint16_t palette[8];
    for (int x = 0; x < 8; x++) {
        const int i = x * 31 / 7;
        palette[x] = (i << 10) | (i << 5) | i;
    }
    glColorTableEXT(GL_TEXTURE_2D, 0, 8, 0, 0, palette);

    irqSet(IRQ_VBLANK, end_frame);
    irqEnable(IRQ_VBLANK);

    uint16_t *c_gfx = bitmap_init_4bpp(c_buttonBitmap, 0);
    palette_make_pressed(0, 1);
    uint16_t *stick = bitmap_init_4bpp(stickBitmap, 2);
    uint16_t *stick_base_1 = bitmap_init_4bpp(stick_base_1Bitmap, 3);
    uint16_t *stick_base_2 = bitmap_init_4bpp(stick_base_2Bitmap, 4);

    sprites[C_LEFT].gfx = c_gfx;
    sprites[C_LEFT].pal_press = 1;
    sprites[C_LEFT].pal_release = 0;
    sprites[C_LEFT].x = 0;
    sprites[C_LEFT].y = 128;
    sprites[C_LEFT].vflip = false;

    sprites[C_RIGHT].gfx = c_gfx;
    sprites[C_RIGHT].pal_press = 1;
    sprites[C_RIGHT].pal_release = 0;
    sprites[C_RIGHT].x = 192;
    sprites[C_RIGHT].y = 128;
    sprites[C_RIGHT].vflip = true;

    sprites[STICK].gfx = stick;
    sprites[STICK].pal_press = 2;
    sprites[STICK].pal_release = 2;
    sprites[STICK].x = 96;
    sprites[STICK].y = 64;
    sprites[STICK].vflip = false;

    sprites[STICK_BASE_1].gfx = stick_base_1;
    sprites[STICK_BASE_1].pal_press = 3;
    sprites[STICK_BASE_1].pal_release = 3;
    sprites[STICK_BASE_1].x = 64;
    sprites[STICK_BASE_1].y = 32;
    sprites[STICK_BASE_1].vflip = false;

    sprites[STICK_BASE_2].gfx = stick_base_2;
    sprites[STICK_BASE_2].pal_press = 4;
    sprites[STICK_BASE_2].pal_release = 4;
    sprites[STICK_BASE_2].x = 64;
    sprites[STICK_BASE_2].y = 96;
    sprites[STICK_BASE_2].vflip = false;

    sprites[STICK_BASE_3].gfx = stick_base_1;
    sprites[STICK_BASE_3].pal_press = 3;
    sprites[STICK_BASE_3].pal_release = 3;
    sprites[STICK_BASE_3].x = 128;
    sprites[STICK_BASE_3].y = 32;
    sprites[STICK_BASE_3].vflip = true;

    sprites[STICK_BASE_4].gfx = stick_base_2;
    sprites[STICK_BASE_4].pal_press = 4;
    sprites[STICK_BASE_4].pal_release = 4;
    sprites[STICK_BASE_4].x = 128;
    sprites[STICK_BASE_4].y = 96;
    sprites[STICK_BASE_4].vflip = true;
}

void draw_frame(Gfx *display_list) {

    background = true;
    z_depth = 0x1000 * 6;
    fog_status = 0;
    tint_active = false;

    execute(display_list);
    glFlush(GL_TRANS_MANUALSORT);

    if (fog_status) {

        int shift = 0;
        for (int i = 500; i >= fog_max - fog_min; i >>= 1)
            shift++;

        int density = 0;
        int inc = ((((128 * 1000) << 1) / ((fog_max - fog_min) * 32)) + 1) >> (shift + 1);

        for (int i = 0; i < 32; i++) {
            glFogDensity(i, density);
            if ((density += inc) > 127)
                density = 127;
        }

        glFogShift(shift);
        glFogOffset((fog_min * 0x7FFF / 1000) - (0x400 >> shift));
        glFogColor(fog_color.r >> 3, fog_color.g >> 3, fog_color.b >> 3, fog_color.a >> 3);
        glEnable(GL_FOG);
    } else {
        glDisable(GL_FOG);
    }

    for (int i = 0; i < MAX_SPRITES; i++)
        oamSet(&oamSub, i, sprites[i].x, sprites[i].y, 1, sprites[i].pressed
            ? sprites[i].pal_press : sprites[i].pal_release, SpriteSize_64x64, SpriteColorFormat_16Color,
            sprites[i].gfx, -1, false, false, sprites[i].vflip, false, false);

    for (int i = frame_count; i < 2; i++)
        swiWaitForVBlank();

    frame_count = 0;
}
