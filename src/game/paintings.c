#include <PR/ultratypes.h>

#include "sm64.h"
#include "area.h"
#include "engine/graph_node.h"
#include "engine/surface_collision.h"
#include "game_init.h"
#include "geo_misc.h"
#include "levels/castle_inside/header.h"
#include "levels/hmc/header.h"
#include "levels/ttm/header.h"
#include "mario.h"
#include "memory.h"
#include "moving_texture.h"
#include "object_list_processor.h"
#include "paintings.h"
#include "save_file.h"
#include "segment2.h"

#define RIPPLE_LEFT 0x20

#define RIPPLE_MIDDLE 0x10

#define RIPPLE_RIGHT 0x8

#define ENTER_LEFT 0x4

#define ENTER_MIDDLE 0x2

#define ENTER_RIGHT 0x1

#define NEAREST_4TH 30

#define MARIO_X 40

#define MIDDLE_X 50

#define MARIO_Y 60

#define MARIO_Z 70

#define MIDDLE_Y 80

#define DONT_RESET -56

#define RESET_TIMER 100

s16 gPaintingMarioFloorType;

f32 gPaintingMarioXPos;
f32 gPaintingMarioYPos;
f32 gPaintingMarioZPos;

struct PaintingMeshVertex *gPaintingMesh;

Vec3f *gPaintingTriNorms;

struct Painting *gRipplingPainting;

s8 gDDDPaintingStatus;

struct Painting *sHMCPaintings[] = {
    &cotmc_painting,
    NULL,
};

struct Painting *sInsideCastlePaintings[] = {
    &bob_painting, &ccm_painting, &wf_painting,  &jrb_painting,      &lll_painting,
    &ssl_painting, &hmc_painting, &ddd_painting, &wdw_painting,      &thi_tiny_painting,
    &ttm_painting, &ttc_painting, &sl_painting,  &thi_huge_painting, NULL,
};

struct Painting *sTTMPaintings[] = {
    &ttm_slide_painting,
    NULL,
};

struct Painting **sPaintingGroups[] = {
    sHMCPaintings,
    sInsideCastlePaintings,
    sTTMPaintings,
};

s16 gPaintingUpdateCounter = 1;
s16 gLastPaintingUpdateCounter = 0;

void stop_other_paintings(s16 *idptr, struct Painting *paintingGroup[]) {
    s16 index;
    s16 id = *idptr;

    index = 0;
    while (paintingGroup[index] != NULL) {
        struct Painting *painting = segmented_to_virtual(paintingGroup[index]);

        if (painting->id != id) {
            painting->state = 0;
        }
        index++;
    }
}

f32 painting_mario_y(struct Painting *painting) {

    f32 relY = gPaintingMarioYPos - painting->posY + 50.0;

    if (relY < 0.0) {
        relY = 0.0;
    } else if (relY > painting->size) {
        relY = painting->size;
    }
    return relY;
}

f32 painting_mario_z(struct Painting *painting) {
    f32 relZ = painting->posZ - gPaintingMarioZPos;

    if (relZ < 0.0) {
        relZ = 0.0;
    } else if (relZ > painting->size) {
        relZ = painting->size;
    }
    return relZ;
}

f32 painting_ripple_y(struct Painting *painting, s8 ySource) {
    switch (ySource) {
        case MARIO_Y:
            return painting_mario_y(painting);
            break;
        case MARIO_Z:
            return painting_mario_z(painting);
            break;
        case MIDDLE_Y:
            return painting->size / 2.0;
            break;
    }
#ifdef AVOID_UB
    return 0.0f;
#endif
}

f32 painting_nearest_4th(struct Painting *painting) {
    f32 firstQuarter = painting->size / 4.0;
    f32 secondQuarter = painting->size / 2.0;
    f32 thirdQuarter = painting->size * 3.0 / 4.0;

    if (painting->floorEntered & RIPPLE_LEFT) {
        return firstQuarter;
    } else if (painting->floorEntered & RIPPLE_MIDDLE) {
        return secondQuarter;
    } else if (painting->floorEntered & RIPPLE_RIGHT) {
        return thirdQuarter;

    } else if (painting->floorEntered & ENTER_LEFT) {
        return firstQuarter;
    } else if (painting->floorEntered & ENTER_MIDDLE) {
        return secondQuarter;
    } else if (painting->floorEntered & ENTER_RIGHT) {
        return thirdQuarter;
    }
#ifdef AVOID_UB
    return 0.0f;
#endif
}

f32 painting_mario_x(struct Painting *painting) {
    f32 relX = gPaintingMarioXPos - painting->posX;

    if (relX < 0.0) {
        relX = 0.0;
    } else if (relX > painting->size) {
        relX = painting->size;
    }
    return relX;
}

f32 painting_ripple_x(struct Painting *painting, s8 xSource) {
    switch (xSource) {
        case NEAREST_4TH:
            return painting_nearest_4th(painting);
            break;
        case MARIO_X:
            return painting_mario_x(painting);
            break;
        case MIDDLE_X:
            return painting->size / 2.0;
            break;
    }
#ifdef AVOID_UB
    return 0.0f;
#endif
}

void painting_state(s8 state, struct Painting *painting, struct Painting *paintingGroup[],
                    s8 xSource, s8 ySource, s8 resetTimer) {

    stop_other_paintings(&painting->id, paintingGroup);

    switch (state) {
        case PAINTING_RIPPLE:
            painting->currRippleMag    = painting->passiveRippleMag;
            painting->rippleDecay      = painting->passiveRippleDecay;
            painting->currRippleRate   = painting->passiveRippleRate;
            painting->dispersionFactor = painting->passiveDispersionFactor;
            break;

        case PAINTING_ENTERED:
            painting->currRippleMag    = painting->entryRippleMag;
            painting->rippleDecay      = painting->entryRippleDecay;
            painting->currRippleRate   = painting->entryRippleRate;
            painting->dispersionFactor = painting->entryDispersionFactor;
            break;
    }
    painting->state = state;
    painting->rippleX = painting_ripple_x(painting, xSource);
    painting->rippleY = painting_ripple_y(painting, ySource);
    gPaintingMarioYEntry = gPaintingMarioYPos;

    if (resetTimer == RESET_TIMER) {
        painting->rippleTimer = 0.0f;
    }
    gRipplingPainting = painting;
}

void wall_painting_proximity_idle(struct Painting *painting, struct Painting *paintingGroup[]) {

    if (painting->floorEntered & RIPPLE_LEFT) {
        painting_state(PAINTING_RIPPLE, painting, paintingGroup, NEAREST_4TH, MARIO_Y, RESET_TIMER);
    } else if (painting->floorEntered & RIPPLE_MIDDLE) {
        painting_state(PAINTING_RIPPLE, painting, paintingGroup, NEAREST_4TH, MARIO_Y, RESET_TIMER);
    } else if (painting->floorEntered & RIPPLE_RIGHT) {
        painting_state(PAINTING_RIPPLE, painting, paintingGroup, NEAREST_4TH, MARIO_Y, RESET_TIMER);

    } else if (painting->floorEntered & ENTER_LEFT) {
        painting_state(PAINTING_ENTERED, painting, paintingGroup, NEAREST_4TH, MARIO_Y, RESET_TIMER);
    } else if (painting->floorEntered & ENTER_MIDDLE) {
        painting_state(PAINTING_ENTERED, painting, paintingGroup, NEAREST_4TH, MARIO_Y, RESET_TIMER);
    } else if (painting->floorEntered & ENTER_RIGHT) {
        painting_state(PAINTING_ENTERED, painting, paintingGroup, NEAREST_4TH, MARIO_Y, RESET_TIMER);
    }
}

void wall_painting_proximity_rippling(struct Painting *painting, struct Painting *paintingGroup[]) {
    if (painting->floorEntered & ENTER_LEFT) {
        painting_state(PAINTING_ENTERED, painting, paintingGroup, NEAREST_4TH, MARIO_Y, RESET_TIMER);
    } else if (painting->floorEntered & ENTER_MIDDLE) {
        painting_state(PAINTING_ENTERED, painting, paintingGroup, NEAREST_4TH, MARIO_Y, RESET_TIMER);
    } else if (painting->floorEntered & ENTER_RIGHT) {
        painting_state(PAINTING_ENTERED, painting, paintingGroup, NEAREST_4TH, MARIO_Y, RESET_TIMER);
    }
}

void wall_painting_continuous_idle(struct Painting *painting, struct Painting *paintingGroup[]) {

    if (painting->floorEntered & RIPPLE_LEFT) {
        painting_state(PAINTING_RIPPLE, painting, paintingGroup, MIDDLE_X, MIDDLE_Y, RESET_TIMER);
    } else if (painting->floorEntered & RIPPLE_MIDDLE) {
        painting_state(PAINTING_RIPPLE, painting, paintingGroup, MIDDLE_X, MIDDLE_Y, RESET_TIMER);
    } else if (painting->floorEntered & RIPPLE_RIGHT) {
        painting_state(PAINTING_RIPPLE, painting, paintingGroup, MIDDLE_X, MIDDLE_Y, RESET_TIMER);

    } else if (painting->floorEntered & ENTER_LEFT) {
        painting_state(PAINTING_ENTERED, painting, paintingGroup, NEAREST_4TH, MARIO_Y, RESET_TIMER);
    } else if (painting->floorEntered & ENTER_MIDDLE) {
        painting_state(PAINTING_ENTERED, painting, paintingGroup, NEAREST_4TH, MARIO_Y, RESET_TIMER);
    } else if (painting->floorEntered & ENTER_RIGHT) {
        painting_state(PAINTING_ENTERED, painting, paintingGroup, NEAREST_4TH, MARIO_Y, RESET_TIMER);
    }
}

void wall_painting_continuous_rippling(struct Painting *painting, struct Painting *paintingGroup[]) {
    if (painting->floorEntered & ENTER_LEFT) {
        painting_state(PAINTING_ENTERED, painting, paintingGroup, NEAREST_4TH, MARIO_Y, DONT_RESET);
    } else if (painting->floorEntered & ENTER_MIDDLE) {
        painting_state(PAINTING_ENTERED, painting, paintingGroup, NEAREST_4TH, MARIO_Y, DONT_RESET);
    } else if (painting->floorEntered & ENTER_RIGHT) {
        painting_state(PAINTING_ENTERED, painting, paintingGroup, NEAREST_4TH, MARIO_Y, DONT_RESET);
    }
}

void floor_painting_proximity_idle(struct Painting *painting, struct Painting *paintingGroup[]) {

    if (painting->floorEntered & RIPPLE_LEFT) {
        painting_state(PAINTING_RIPPLE, painting, paintingGroup, MARIO_X, MARIO_Z, RESET_TIMER);
    } else if (painting->floorEntered & RIPPLE_MIDDLE) {
        painting_state(PAINTING_RIPPLE, painting, paintingGroup, MARIO_X, MARIO_Z, RESET_TIMER);
    } else if (painting->floorEntered & RIPPLE_RIGHT) {
        painting_state(PAINTING_RIPPLE, painting, paintingGroup, MARIO_X, MARIO_Z, RESET_TIMER);

    } else if (painting->marioWentUnder) {
        if (painting->currFloor & ENTER_LEFT) {
            painting_state(PAINTING_ENTERED, painting, paintingGroup, MARIO_X, MARIO_Z, RESET_TIMER);
        } else if (painting->currFloor & ENTER_MIDDLE) {
            painting_state(PAINTING_ENTERED, painting, paintingGroup, MARIO_X, MARIO_Z, RESET_TIMER);
        } else if (painting->currFloor & ENTER_RIGHT) {
            painting_state(PAINTING_ENTERED, painting, paintingGroup, MARIO_X, MARIO_Z, RESET_TIMER);
        }
    }
}

void floor_painting_proximity_rippling(struct Painting *painting, struct Painting *paintingGroup[]) {
    if (painting->marioWentUnder) {
        if (painting->currFloor & ENTER_LEFT) {
            painting_state(PAINTING_ENTERED, painting, paintingGroup, MARIO_X, MARIO_Z, RESET_TIMER);
        } else if (painting->currFloor & ENTER_MIDDLE) {
            painting_state(PAINTING_ENTERED, painting, paintingGroup, MARIO_X, MARIO_Z, RESET_TIMER);
        } else if (painting->currFloor & ENTER_RIGHT) {
            painting_state(PAINTING_ENTERED, painting, paintingGroup, MARIO_X, MARIO_Z, RESET_TIMER);
        }
    }
}

void floor_painting_continuous_idle(struct Painting *painting, struct Painting *paintingGroup[]) {

    if (painting->floorEntered & RIPPLE_LEFT) {
        painting_state(PAINTING_RIPPLE, painting, paintingGroup, MIDDLE_X, MIDDLE_Y, RESET_TIMER);
    } else if (painting->floorEntered & RIPPLE_MIDDLE) {
        painting_state(PAINTING_RIPPLE, painting, paintingGroup, MIDDLE_X, MIDDLE_Y, RESET_TIMER);
    } else if (painting->floorEntered & RIPPLE_RIGHT) {
        painting_state(PAINTING_RIPPLE, painting, paintingGroup, MIDDLE_X, MIDDLE_Y, RESET_TIMER);

    } else if (painting->currFloor & ENTER_LEFT) {
        painting_state(PAINTING_ENTERED, painting, paintingGroup, MARIO_X, MARIO_Z, RESET_TIMER);
    } else if (painting->currFloor & ENTER_MIDDLE) {
        painting_state(PAINTING_ENTERED, painting, paintingGroup, MARIO_X, MARIO_Z, RESET_TIMER);
    } else if (painting->currFloor & ENTER_RIGHT) {
        painting_state(PAINTING_ENTERED, painting, paintingGroup, MARIO_X, MARIO_Z, RESET_TIMER);
    }
}

void floor_painting_continuous_rippling(struct Painting *painting, struct Painting *paintingGroup[]) {
    if (painting->marioWentUnder) {
        if (painting->currFloor & ENTER_LEFT) {
            painting_state(PAINTING_ENTERED, painting, paintingGroup, MARIO_X, MARIO_Z, DONT_RESET);
        } else if (painting->currFloor & ENTER_MIDDLE) {
            painting_state(PAINTING_ENTERED, painting, paintingGroup, MARIO_X, MARIO_Z, DONT_RESET);
        } else if (painting->currFloor & ENTER_RIGHT) {
            painting_state(PAINTING_ENTERED, painting, paintingGroup, MARIO_X, MARIO_Z, DONT_RESET);
        }
    }
}

void painting_update_floors(struct Painting *painting) {
    s16 paintingId = painting->id;
    s8 rippleLeft = 0;
    s8 rippleMiddle = 0;
    s8 rippleRight = 0;
    s8 enterLeft = 0;
    s8 enterMiddle = 0;
    s8 enterRight = 0;

    if (gPaintingMarioFloorType == paintingId * 3 + SURFACE_PAINTING_WOBBLE_A6) {
        rippleLeft = RIPPLE_LEFT;
    }
    if (gPaintingMarioFloorType == paintingId * 3 + SURFACE_PAINTING_WOBBLE_A7) {
        rippleMiddle = RIPPLE_MIDDLE;
    }
    if (gPaintingMarioFloorType == paintingId * 3 + SURFACE_PAINTING_WOBBLE_A8) {
        rippleRight = RIPPLE_RIGHT;
    }
    if (gPaintingMarioFloorType == paintingId * 3 + SURFACE_PAINTING_WARP_D3) {
        enterLeft = ENTER_LEFT;
    }
    if (gPaintingMarioFloorType == paintingId * 3 + SURFACE_PAINTING_WARP_D4) {
        enterMiddle = ENTER_MIDDLE;
    }
    if (gPaintingMarioFloorType == paintingId * 3 + SURFACE_PAINTING_WARP_D5) {
        enterRight = ENTER_RIGHT;
    }

    painting->lastFloor = painting->currFloor;

    painting->currFloor = rippleLeft + rippleMiddle + rippleRight + enterLeft + enterMiddle + enterRight;

    painting->floorEntered = (painting->lastFloor ^ painting->currFloor) & painting->currFloor;

    painting->marioWasUnder = painting->marioIsUnder;

    if (gPaintingMarioYPos < painting->posY) {
        painting->marioIsUnder = TRUE;
    } else {
        painting->marioIsUnder = FALSE;
    }

    painting->marioWentUnder = (painting->marioWasUnder ^ painting->marioIsUnder) & painting->marioIsUnder;
}

void painting_update_ripple_state(struct Painting *painting) {
    if (gPaintingUpdateCounter != gLastPaintingUpdateCounter) {
        painting->currRippleMag *= painting->rippleDecay;

        painting->rippleTimer += 1.0;
    }
    if (painting->rippleTrigger == RIPPLE_TRIGGER_PROXIMITY) {

        if (painting->currRippleMag <= 1.0) {
            painting->state = PAINTING_IDLE;
            gRipplingPainting = NULL;
        }
    } else if (painting->rippleTrigger == RIPPLE_TRIGGER_CONTINUOUS) {

        if (painting->state == PAINTING_ENTERED && painting->currRippleMag <= painting->passiveRippleMag) {

            painting->state = PAINTING_RIPPLE;
            painting->currRippleMag = painting->passiveRippleMag;
            painting->rippleDecay = painting->passiveRippleDecay;
            painting->currRippleRate = painting->passiveRippleRate;
            painting->dispersionFactor = painting->passiveDispersionFactor;
        }
    }
}

s16 calculate_ripple_at_point(struct Painting *painting, f32 posX, f32 posY) {

    f32 rippleMag = painting->currRippleMag;

    f32 rippleRate = painting->currRippleRate;

    f32 dispersionFactor = painting->dispersionFactor;

    f32 rippleTimer = painting->rippleTimer;

    f32 rippleX = painting->rippleX;
    f32 rippleY = painting->rippleY;

    f32 distanceToOrigin;
    f32 rippleDistance;

    posX *= painting->size / PAINTING_SIZE;
    posY *= painting->size / PAINTING_SIZE;
    distanceToOrigin = sqrtf((posX - rippleX) * (posX - rippleX) + (posY - rippleY) * (posY - rippleY));

    rippleDistance = distanceToOrigin / dispersionFactor;
    if (rippleTimer < rippleDistance) {

        return 0;
    } else {

        f32 rippleZ = rippleMag * cosf(rippleRate * (2 * M_PI) * (rippleTimer - rippleDistance));

        return round_float(rippleZ);
    }
}

s16 ripple_if_movable(struct Painting *painting, s16 movable, s16 posX, s16 posY) {
    s16 rippleZ = 0;

    if (movable) {
        rippleZ = calculate_ripple_at_point(painting, posX, posY);
    }
    return rippleZ;
}

void painting_generate_mesh(struct Painting *painting, s16 *mesh, s16 numTris) {
    s16 i;

    gPaintingMesh = mem_pool_alloc(gEffectsMemoryPool, numTris * sizeof(struct PaintingMeshVertex));
    if (gPaintingMesh == NULL) {
    }

    for (i = 0; i < numTris; i++) {
        gPaintingMesh[i].pos[0] = mesh[i * 3 + 1];
        gPaintingMesh[i].pos[1] = mesh[i * 3 + 2];

        gPaintingMesh[i].pos[2] = ripple_if_movable(painting, mesh[i * 3 + 3],
                                                    gPaintingMesh[i].pos[0], gPaintingMesh[i].pos[1]);
    }
}

void painting_calculate_triangle_normals(s16 *mesh, s16 numVtx, s16 numTris) {
    s16 i;

    gPaintingTriNorms = mem_pool_alloc(gEffectsMemoryPool, numTris * sizeof(Vec3f));
    if (gPaintingTriNorms == NULL) {
    }
    for (i = 0; i < numTris; i++) {
        s16 tri = numVtx * 3 + i * 3 + 2;
        s16 v0 = mesh[tri];
        s16 v1 = mesh[tri + 1];
        s16 v2 = mesh[tri + 2];

        f32 x0 = gPaintingMesh[v0].pos[0];
        f32 y0 = gPaintingMesh[v0].pos[1];
        f32 z0 = gPaintingMesh[v0].pos[2];

        f32 x1 = gPaintingMesh[v1].pos[0];
        f32 y1 = gPaintingMesh[v1].pos[1];
        f32 z1 = gPaintingMesh[v1].pos[2];

        f32 x2 = gPaintingMesh[v2].pos[0];
        f32 y2 = gPaintingMesh[v2].pos[1];
        f32 z2 = gPaintingMesh[v2].pos[2];

        gPaintingTriNorms[i][0] = (y1 - y0) * (z2 - z1) - (z1 - z0) * (y2 - y1);
        gPaintingTriNorms[i][1] = (z1 - z0) * (x2 - x1) - (x1 - x0) * (z2 - z1);
        gPaintingTriNorms[i][2] = (x1 - x0) * (y2 - y1) - (y1 - y0) * (x2 - x1);
    }
}

s8 normalize_component(f32 comp) {
    s8 rounded;

    if (comp > 0.0) {
        rounded = comp * 127.0 + 0.5;
    } else if (comp < 0.0) {
        rounded = comp * 128.0 - 0.5;
    } else {
        rounded = 0;
    }
    return rounded;
}

void painting_average_vertex_normals(s16 *neighborTris, s16 numVtx) {
    UNUSED s16 unused;
    s16 tri;
    s16 i;
    s16 j;
    s16 neighbors;
    s16 entry = 0;

    for (i = 0; i < numVtx; i++) {
        f32 nx = 0.0f;
        f32 ny = 0.0f;
        f32 nz = 0.0f;
        f32 nlen;

        neighbors = neighborTris[entry];
        for (j = 0; j < neighbors; j++) {
            tri = neighborTris[entry + j + 1];
            nx += gPaintingTriNorms[tri][0];
            ny += gPaintingTriNorms[tri][1];
            nz += gPaintingTriNorms[tri][2];
        }

        entry += neighbors + 1;

        nx /= neighbors;
        ny /= neighbors;
        nz /= neighbors;
        nlen = sqrtf(nx * nx + ny * ny + nz * nz);

        if (nlen == 0.0) {
            gPaintingMesh[i].norm[0] = 0;
            gPaintingMesh[i].norm[1] = 0;
            gPaintingMesh[i].norm[2] = 0;
        } else {
            gPaintingMesh[i].norm[0] = normalize_component(nx / nlen);
            gPaintingMesh[i].norm[1] = normalize_component(ny / nlen);
            gPaintingMesh[i].norm[2] = normalize_component(nz / nlen);
        }
    }
}

Gfx *render_painting(u8 *img, s16 tWidth, s16 tHeight, s16 *textureMap, s16 mapVerts, s16 mapTris, u8 alpha) {
    s16 group;
    s16 map;
    s16 triGroup;
    s16 mapping;
    s16 meshVtx;
    s16 tx;
    s16 ty;

    s16 triGroups = mapTris / 5;
    s16 remGroupTris = mapTris % 5;
    s16 numVtx = mapTris * 3;

    s16 commands = triGroups * 2 + remGroupTris + 7;
    Vtx *verts = alloc_display_list(numVtx * sizeof(Vtx));
    Gfx *dlist = alloc_display_list(commands * sizeof(Gfx));
    Gfx *gfx = dlist;

    if (verts == NULL || dlist == NULL) {
    }

    gLoadBlockTexture(gfx++, tWidth, tHeight, G_IM_FMT_RGBA, img);

    for (group = 0; group < triGroups; group++) {

        triGroup = mapVerts * 3 + group * 15 + 2;
        for (map = 0; map < 15; map++) {

            mapping = textureMap[triGroup + map];

            meshVtx = textureMap[mapping * 3 + 1];

            tx = textureMap[mapping * 3 + 2];
            ty = textureMap[mapping * 3 + 3];

            make_vertex(verts, group * 15 + map, gPaintingMesh[meshVtx].pos[0], gPaintingMesh[meshVtx].pos[1],
                        gPaintingMesh[meshVtx].pos[2], tx, ty, gPaintingMesh[meshVtx].norm[0],
                        gPaintingMesh[meshVtx].norm[1], gPaintingMesh[meshVtx].norm[2], alpha);
        }

        gSPVertex(gfx++, VIRTUAL_TO_PHYSICAL(verts + group * 15), 15, 0);
        gSPDisplayList(gfx++, dl_paintings_draw_ripples);
    }

    triGroup = mapVerts * 3 + triGroups * 15 + 2;

    for (map = 0; map < remGroupTris * 3; map++) {
        mapping = textureMap[triGroup + map];
        meshVtx = textureMap[mapping * 3 + 1];
        tx = textureMap[mapping * 3 + 2];
        ty = textureMap[mapping * 3 + 3];
        make_vertex(verts, triGroups * 15 + map, gPaintingMesh[meshVtx].pos[0], gPaintingMesh[meshVtx].pos[1],
                    gPaintingMesh[meshVtx].pos[2], tx, ty, gPaintingMesh[meshVtx].norm[0],
                    gPaintingMesh[meshVtx].norm[1], gPaintingMesh[meshVtx].norm[2], alpha);
    }

    gSPVertex(gfx++, VIRTUAL_TO_PHYSICAL(verts + triGroups * 15), remGroupTris * 3, 0);
    for (group = 0; group < remGroupTris; group++) {
        gSP1Triangle(gfx++, group * 3, group * 3 + 1, group * 3 + 2, 0);
    }

    gSPEndDisplayList(gfx);
    return dlist;
}

Gfx *painting_model_view_transform(struct Painting *painting) {
    f32 sizeRatio = painting->size / PAINTING_SIZE;
    Mtx *rotX = alloc_display_list(sizeof(Mtx));
    Mtx *rotY = alloc_display_list(sizeof(Mtx));
    Mtx *translate = alloc_display_list(sizeof(Mtx));
    Mtx *scale = alloc_display_list(sizeof(Mtx));
    Gfx *dlist = alloc_display_list(5 * sizeof(Gfx));
    Gfx *gfx = dlist;

    if (rotX == NULL || rotY == NULL || translate == NULL || dlist == NULL) {
    }

    guTranslate(translate, painting->posX, painting->posY, painting->posZ);
    guRotate(rotX, painting->pitch, 1.0f, 0.0f, 0.0f);
    guRotate(rotY, painting->yaw, 0.0f, 1.0f, 0.0f);
    guScale(scale, sizeRatio, sizeRatio, sizeRatio);

    gSPMatrix(gfx++, translate, G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPMatrix(gfx++, rotX,      G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    gSPMatrix(gfx++, rotY,      G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    gSPMatrix(gfx++, scale,     G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    gSPEndDisplayList(gfx);

    return dlist;
}

Gfx *painting_ripple_image(struct Painting *painting) {
    s16 meshVerts;
    s16 meshTris;
    s16 i;
    s16 *textureMap;
    s16 imageCount = painting->imageCount;
    s16 tWidth = painting->textureWidth;
    s16 tHeight = painting->textureHeight;
    s16 **textureMaps = segmented_to_virtual(painting->textureMaps);
    u8 **textures = segmented_to_virtual(painting->textureArray);
    Gfx *dlist = alloc_display_list((imageCount + 6) * sizeof(Gfx));
    Gfx *gfx = dlist;

    if (dlist == NULL) {
        return dlist;
    }

    gSPDisplayList(gfx++, painting_model_view_transform(painting));
    gSPDisplayList(gfx++, dl_paintings_rippling_begin);
    gSPDisplayList(gfx++, painting->rippleDisplayList);

    for (i = 0; i < imageCount; i++) {
        textureMap = segmented_to_virtual(textureMaps[i]);
        meshVerts = textureMap[0];
        meshTris = textureMap[meshVerts * 3 + 1];
        gSPDisplayList(gfx++, render_painting(textures[i], tWidth, tHeight, textureMap, meshVerts, meshTris, painting->alpha));
    }

    painting_update_ripple_state(painting);

    gSPPopMatrix(gfx++, G_MTX_MODELVIEW);
    gSPDisplayList(gfx++, dl_paintings_rippling_end);
    gSPEndDisplayList(gfx);
    return dlist;
}

Gfx *painting_ripple_env_mapped(struct Painting *painting) {
    s16 meshVerts;
    s16 meshTris;
    s16 *textureMap;
    s16 tWidth = painting->textureWidth;
    s16 tHeight = painting->textureHeight;
    s16 **textureMaps = segmented_to_virtual(painting->textureMaps);
    u8 **tArray = segmented_to_virtual(painting->textureArray);
    Gfx *dlist = alloc_display_list(7 * sizeof(Gfx));
    Gfx *gfx = dlist;

    if (dlist == NULL) {
        return dlist;
    }

    gSPDisplayList(gfx++, painting_model_view_transform(painting));
    gSPDisplayList(gfx++, dl_paintings_env_mapped_begin);
    gSPDisplayList(gfx++, painting->rippleDisplayList);

    textureMap = segmented_to_virtual(textureMaps[0]);
    meshVerts = textureMap[0];
    meshTris = textureMap[meshVerts * 3 + 1];
    gSPDisplayList(gfx++, render_painting(tArray[0], tWidth, tHeight, textureMap, meshVerts, meshTris, painting->alpha));

    painting_update_ripple_state(painting);

    gSPPopMatrix(gfx++, G_MTX_MODELVIEW);
    gSPDisplayList(gfx++, dl_paintings_env_mapped_end);
    gSPEndDisplayList(gfx);
    return dlist;
}

Gfx *display_painting_rippling(struct Painting *painting) {
    s16 *mesh = segmented_to_virtual(seg2_painting_triangle_mesh);
    s16 *neighborTris = segmented_to_virtual(seg2_painting_mesh_neighbor_tris);
    s16 numVtx = mesh[0];
    s16 numTris = mesh[numVtx * 3 + 1];
    Gfx *dlist;

    painting_generate_mesh(painting, mesh, numVtx);
    painting_calculate_triangle_normals(mesh, numVtx, numTris);
    painting_average_vertex_normals(neighborTris, numVtx);

    switch (painting->textureType) {
        case PAINTING_IMAGE:
            dlist = painting_ripple_image(painting);
            break;
        case PAINTING_ENV_MAP:
            dlist = painting_ripple_env_mapped(painting);
            break;
    }

    mem_pool_free(gEffectsMemoryPool, gPaintingMesh);
    mem_pool_free(gEffectsMemoryPool, gPaintingTriNorms);
    return dlist;
}

Gfx *display_painting_not_rippling(struct Painting *painting) {
    Gfx *dlist = alloc_display_list(4 * sizeof(Gfx));
    Gfx *gfx = dlist;

    if (dlist == NULL) {
        return dlist;
    }
    gSPDisplayList(gfx++, painting_model_view_transform(painting));
    gSPDisplayList(gfx++, painting->normalDisplayList);
    gSPPopMatrix(gfx++, G_MTX_MODELVIEW);
    gSPEndDisplayList(gfx);
    return dlist;
}

void reset_painting(struct Painting *painting) {
    painting->lastFloor = 0;
    painting->currFloor = 0;
    painting->floorEntered = 0;
    painting->marioWasUnder = 0;
    painting->marioIsUnder = 0;
    painting->marioWentUnder = 0;

    gRipplingPainting = NULL;

#ifdef NO_SEGMENTED_MEMORY

    painting->state = PAINTING_IDLE;
    painting->currRippleMag = 0.0f;
    painting->rippleDecay = 1.0f;
    painting->currRippleRate = 0.0f;
    painting->dispersionFactor = 0.0f;
    painting->rippleTimer = 0.0f;
    painting->rippleX = 0.0f;
    painting->rippleY = 0.0f;
    if (painting == &ddd_painting) {

        painting->posX = 3456.0f;
    }
#endif
}

void move_ddd_painting(struct Painting *painting, f32 frontPos, f32 backPos, f32 speed) {

    u32 dddFlags = save_file_get_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(COURSE_DDD));

    u32 saveFileFlags = save_file_get_flags();

    u32 bowsersSubBeaten = dddFlags & BOARD_BOWSERS_SUB;

    u32 dddBack = saveFileFlags & SAVE_FLAG_DDD_MOVED_BACK;

    if (!bowsersSubBeaten && !dddBack) {

        painting->posX = frontPos;
        gDDDPaintingStatus = 0;
    } else if (bowsersSubBeaten && !dddBack) {

        painting->posX += speed;
        gDDDPaintingStatus = BOWSERS_SUB_BEATEN;
        if (painting->posX >= backPos) {
            painting->posX = backPos;

            save_file_set_flags(SAVE_FLAG_DDD_MOVED_BACK);
        }
    } else if (bowsersSubBeaten && dddBack) {

        painting->posX = backPos;
        gDDDPaintingStatus = BOWSERS_SUB_BEATEN | DDD_BACK;
    }
}

void set_painting_layer(struct GraphNodeGenerated *gen, struct Painting *painting) {
    switch (painting->alpha) {
        case 0xFF:
            gen->fnNode.node.flags = (gen->fnNode.node.flags & 0xFF) | (LAYER_OPAQUE << 8);
            break;
        default:
            gen->fnNode.node.flags = (gen->fnNode.node.flags & 0xFF) | (LAYER_TRANSPARENT << 8);
            break;
    }
}

Gfx *display_painting(struct Painting *painting) {
    switch (painting->state) {
        case PAINTING_IDLE:
            return display_painting_not_rippling(painting);
            break;
        default:
            return display_painting_rippling(painting);
            break;
    }
}

void wall_painting_update(struct Painting *painting, struct Painting *paintingGroup[]) {
    if (painting->rippleTrigger == RIPPLE_TRIGGER_PROXIMITY) {
        switch (painting->state) {
            case PAINTING_IDLE:
                wall_painting_proximity_idle(painting, paintingGroup);
                break;
            case PAINTING_RIPPLE:
                wall_painting_proximity_rippling(painting, paintingGroup);
                break;
        }
    } else if (painting->rippleTrigger == RIPPLE_TRIGGER_CONTINUOUS) {
        switch (painting->state) {
            case PAINTING_IDLE:
                wall_painting_continuous_idle(painting, paintingGroup);
                break;
            case PAINTING_RIPPLE:
                wall_painting_continuous_rippling(painting, paintingGroup);
                break;
        }
    }
}

void floor_painting_update(struct Painting *painting, struct Painting *paintingGroup[]) {
    if (painting->rippleTrigger == RIPPLE_TRIGGER_PROXIMITY) {
        switch (painting->state) {
            case PAINTING_IDLE:
                floor_painting_proximity_idle(painting, paintingGroup);
                break;
            case PAINTING_RIPPLE:
                floor_painting_proximity_rippling(painting, paintingGroup);
                break;
        }
    } else if (painting->rippleTrigger == RIPPLE_TRIGGER_CONTINUOUS) {
        switch (painting->state) {
            case PAINTING_IDLE:
                floor_painting_continuous_idle(painting, paintingGroup);
                break;
            case PAINTING_RIPPLE:
                floor_painting_continuous_rippling(painting, paintingGroup);
                break;
        }
    }
}

Gfx *geo_painting_draw(s32 callContext, struct GraphNode *node, UNUSED void *context) {
    struct GraphNodeGenerated *gen = (struct GraphNodeGenerated *) node;
    s32 group = (gen->parameter >> 8) & 0xFF;
    s32 id = gen->parameter & 0xFF;
    Gfx *paintingDlist = NULL;
    struct Painting **paintingGroup = sPaintingGroups[group];
    struct Painting *painting = segmented_to_virtual(paintingGroup[id]);

    if (callContext != GEO_CONTEXT_RENDER) {
        reset_painting(painting);
    } else if (callContext == GEO_CONTEXT_RENDER) {

        if (group == 1 && id == PAINTING_ID_DDD) {
            move_ddd_painting(painting, 3456.0f, 5529.6f, 20.0f);
        }

        set_painting_layer(gen, painting);

        paintingDlist = display_painting(painting);

        painting_update_floors(painting);
        switch ((s16) painting->pitch) {

            case 0:
                wall_painting_update(painting, paintingGroup);
                break;
            default:
                floor_painting_update(painting, paintingGroup);
                break;
        }
    }
    return paintingDlist;
}

Gfx *geo_painting_update(s32 callContext, UNUSED struct GraphNode *node, UNUSED Mat4 c) {
    struct Surface *surface;

    if (callContext != GEO_CONTEXT_RENDER) {
        gLastPaintingUpdateCounter = gAreaUpdateCounter - 1;
        gPaintingUpdateCounter = gAreaUpdateCounter;
    } else {
        gLastPaintingUpdateCounter = gPaintingUpdateCounter;
        gPaintingUpdateCounter = gAreaUpdateCounter;

        find_floor(gMarioObject->oPosX, gMarioObject->oPosY, gMarioObject->oPosZ, &surface);
        gPaintingMarioFloorType = surface->type;
        gPaintingMarioXPos = gMarioObject->oPosX;
        gPaintingMarioYPos = gMarioObject->oPosY;
        gPaintingMarioZPos = gMarioObject->oPosZ;
    }
    return NULL;
}
