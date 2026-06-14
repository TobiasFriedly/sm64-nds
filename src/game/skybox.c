#include <PR/ultratypes.h>

#include "area.h"
#include "engine/math_util.h"
#include "geo_misc.h"
#include "gfx_dimensions.h"
#include "level_update.h"
#include "memory.h"
#include "save_file.h"
#include "segment2.h"
#include "sm64.h"

struct Skybox {

    u16 yaw;

    s16 pitch;

    s32 scaledX;

    s32 scaledY;

    s32 upperLeftTile;
};

struct Skybox sSkyBoxInfo[2];

typedef const u8 *const SkyboxTexture[80];

extern SkyboxTexture bbh_skybox_ptrlist;
extern SkyboxTexture bidw_skybox_ptrlist;
extern SkyboxTexture bitfs_skybox_ptrlist;
extern SkyboxTexture bits_skybox_ptrlist;
extern SkyboxTexture ccm_skybox_ptrlist;
extern SkyboxTexture cloud_floor_skybox_ptrlist;
extern SkyboxTexture clouds_skybox_ptrlist;
extern SkyboxTexture ssl_skybox_ptrlist;
extern SkyboxTexture water_skybox_ptrlist;
extern SkyboxTexture wdw_skybox_ptrlist;

SkyboxTexture *sSkyboxTextures[10] = {
    &water_skybox_ptrlist,
    &bitfs_skybox_ptrlist,
    &wdw_skybox_ptrlist,
    &cloud_floor_skybox_ptrlist,
    &ccm_skybox_ptrlist,
    &ssl_skybox_ptrlist,
    &bbh_skybox_ptrlist,
    &bidw_skybox_ptrlist,
    &clouds_skybox_ptrlist,
    &bits_skybox_ptrlist,
};

u8 sSkyboxColors[][3] = {
    { 0x50, 0x64, 0x5A },
    { 0xFF, 0xFF, 0xFF },
};

#define SKYBOX_WIDTH (4 * SCREEN_WIDTH)

#define SKYBOX_HEIGHT (4 * SCREEN_HEIGHT)

#define SKYBOX_TILE_WIDTH (SCREEN_WIDTH / 2)

#define SKYBOX_TILE_HEIGHT (SCREEN_HEIGHT / 2)

#define SKYBOX_COLS (10)

#define SKYBOX_ROWS (8)

s32 calculate_skybox_scaled_x(s8 player, f32 fov) {
    f32 yaw = sSkyBoxInfo[player].yaw;

    f32 yawScaled = SCREEN_WIDTH * 360.0 * yaw / (fov * 65536.0);

    s32 scaledX = yawScaled + 0.5;

    if (scaledX > SKYBOX_WIDTH) {
        scaledX -= scaledX / SKYBOX_WIDTH * SKYBOX_WIDTH;
    }
    return SKYBOX_WIDTH - scaledX;
}

s32 calculate_skybox_scaled_y(s8 player, UNUSED f32 fov) {

    f32 pitchInDegrees = (f32) sSkyBoxInfo[player].pitch * 360.0 / 65535.0;

    f32 degreesToScale = 360.0f * pitchInDegrees / 90.0;
    s32 roundedY = round_float(degreesToScale);

    s32 scaledY = roundedY + 5 * SKYBOX_TILE_HEIGHT;

    if (scaledY > SKYBOX_HEIGHT) {
        scaledY = SKYBOX_HEIGHT;
    }
    if (scaledY < SCREEN_HEIGHT) {
        scaledY = SCREEN_HEIGHT;
    }
    return scaledY;
}

static s32 get_top_left_tile_idx(s8 player) {
    s32 tileCol = sSkyBoxInfo[player].scaledX / SKYBOX_TILE_WIDTH;
    s32 tileRow = (SKYBOX_HEIGHT - sSkyBoxInfo[player].scaledY) / SKYBOX_TILE_HEIGHT;

    return tileRow * SKYBOX_COLS + tileCol;
}

Vtx *make_skybox_rect(s32 tileIndex, s8 colorIndex) {
    Vtx *verts = alloc_display_list(4 * sizeof(*verts));
    s16 x = tileIndex % SKYBOX_COLS * SKYBOX_TILE_WIDTH;
    s16 y = SKYBOX_HEIGHT - tileIndex / SKYBOX_COLS * SKYBOX_TILE_HEIGHT;

    if (verts != NULL) {
        make_vertex(verts, 0, x, y, -1, 0, 0, sSkyboxColors[colorIndex][0], sSkyboxColors[colorIndex][1],
                    sSkyboxColors[colorIndex][2], 255);
        make_vertex(verts, 1, x, y - SKYBOX_TILE_HEIGHT, -1, 0, 31 << 5, sSkyboxColors[colorIndex][0], sSkyboxColors[colorIndex][1],
                    sSkyboxColors[colorIndex][2], 255);
        make_vertex(verts, 2, x + SKYBOX_TILE_WIDTH, y - SKYBOX_TILE_HEIGHT, -1, 31 << 5, 31 << 5, sSkyboxColors[colorIndex][0],
                    sSkyboxColors[colorIndex][1], sSkyboxColors[colorIndex][2], 255);
        make_vertex(verts, 3, x + SKYBOX_TILE_WIDTH, y, -1, 31 << 5, 0, sSkyboxColors[colorIndex][0], sSkyboxColors[colorIndex][1],
                    sSkyboxColors[colorIndex][2], 255);
    } else {
    }
    return verts;
}

void draw_skybox_tile_grid(Gfx **dlist, s8 background, s8 player, s8 colorIndex) {
    s32 row;
    s32 col;

    for (row = 0; row < 3; row++) {
        for (col = 0; col < 3; col++) {
            s32 tileIndex = sSkyBoxInfo[player].upperLeftTile + row * SKYBOX_COLS + col;
            const u8 *const texture =
                (*(SkyboxTexture *) segmented_to_virtual(sSkyboxTextures[background]))[tileIndex];
            Vtx *vertices = make_skybox_rect(tileIndex, colorIndex);

            gLoadBlockTexture((*dlist)++, 32, 32, G_IM_FMT_RGBA, texture);
            gSPVertex((*dlist)++, VIRTUAL_TO_PHYSICAL(vertices), 4, 0);
            gSPDisplayList((*dlist)++, dl_draw_quad_verts_0123);
        }
    }
}

void *create_skybox_ortho_matrix(s8 player) {
    f32 left = sSkyBoxInfo[player].scaledX;
    f32 right = sSkyBoxInfo[player].scaledX + SCREEN_WIDTH;
    f32 bottom = sSkyBoxInfo[player].scaledY - SCREEN_HEIGHT;
    f32 top = sSkyBoxInfo[player].scaledY;
    Mtx *mtx = alloc_display_list(sizeof(*mtx));

#ifdef WIDESCREEN
    f32 half_width = (4.0f / 3.0f) / GFX_DIMENSIONS_ASPECT_RATIO * SCREEN_WIDTH / 2;
    f32 center = (sSkyBoxInfo[player].scaledX + SCREEN_WIDTH / 2);
    if (half_width < SCREEN_WIDTH / 2) {

        left = center - half_width;
        right = center + half_width;
    }
#endif

    if (mtx != NULL) {
        guOrtho(mtx, left, right, bottom, top, 0.0f, 3.0f, 1.0f);
    } else {
    }

    return mtx;
}

Gfx *init_skybox_display_list(s8 player, s8 background, s8 colorIndex) {
    s32 dlCommandCount = 5 + (3 * 3) * 7;
    void *skybox = alloc_display_list(dlCommandCount * sizeof(Gfx));
    Gfx *dlist = skybox;

    if (skybox == NULL) {
        return NULL;
    } else {
        Mtx *ortho = create_skybox_ortho_matrix(player);

        gSPDisplayList(dlist++, dl_skybox_begin);
        gSPMatrix(dlist++, VIRTUAL_TO_PHYSICAL(ortho), G_MTX_PROJECTION | G_MTX_MUL | G_MTX_NOPUSH);
        gSPDisplayList(dlist++, dl_skybox_tile_tex_settings);
        draw_skybox_tile_grid(&dlist, background, player, colorIndex);
        gSPDisplayList(dlist++, dl_skybox_end);
        gSPEndDisplayList(dlist);
    }
    return skybox;
}

Gfx *create_skybox_facing_camera(s8 player, s8 background, f32 fov,
                                    f32 posX, f32 posY, f32 posZ,
                                    f32 focX, f32 focY, f32 focZ) {
    f32 cameraFaceX = focX - posX;
    f32 cameraFaceY = focY - posY;
    f32 cameraFaceZ = focZ - posZ;
    s8 colorIndex = 1;

    if (background == 8
        && !(save_file_get_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(COURSE_JRB)) & (1 << 0))) {
        colorIndex = 0;
    }

    fov = 90.0f;
    sSkyBoxInfo[player].yaw = atan2s(cameraFaceZ, cameraFaceX);
    sSkyBoxInfo[player].pitch = atan2s(sqrtf(cameraFaceX * cameraFaceX + cameraFaceZ * cameraFaceZ), cameraFaceY);
    sSkyBoxInfo[player].scaledX = calculate_skybox_scaled_x(player, fov);
    sSkyBoxInfo[player].scaledY = calculate_skybox_scaled_y(player, fov);
    sSkyBoxInfo[player].upperLeftTile = get_top_left_tile_idx(player);

    return init_skybox_display_list(player, background, colorIndex);
}
