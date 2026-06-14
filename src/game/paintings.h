#ifndef PAINTINGS_H
#define PAINTINGS_H

#include <PR/ultratypes.h>
#include <PR/gbi.h>

#include "macros.h"
#include "types.h"

#define PAINTING_ID(id, grp) id | (grp << 8)

#define PAINTING_SIZE 614.0

#define PAINTING_ID_DDD 7

#define BOARD_BOWSERS_SUB (1 << 0)

#define BOWSERS_SUB_BEATEN 0x2
#define DDD_BACK 0x1

#define PAINTING_IDLE 0
#define PAINTING_RIPPLE 1
#define PAINTING_ENTERED 2

#define RIPPLE_TRIGGER_PROXIMITY 10
#define RIPPLE_TRIGGER_CONTINUOUS 20

#define PAINTING_IMAGE 0

#define PAINTING_ENV_MAP 1

struct Painting {
    s16 id;

    s8 imageCount;

    s8 textureType;

    s8 lastFloor;

    s8 currFloor;

    s8 floorEntered;

    s8 state;

    f32 pitch;
    f32 yaw;

    f32 posX;
    f32 posY;
    f32 posZ;

    f32 currRippleMag;
    f32 passiveRippleMag;
    f32 entryRippleMag;

    f32 rippleDecay;
    f32 passiveRippleDecay;
    f32 entryRippleDecay;

    f32 currRippleRate;
    f32 passiveRippleRate;
    f32 entryRippleRate;

    f32 dispersionFactor;
    f32 passiveDispersionFactor;
    f32 entryDispersionFactor;

    f32 rippleTimer;

    f32 rippleX;
    f32 rippleY;

    const Gfx *normalDisplayList;

    const s16 *const *textureMaps;

    const Texture *const *textureArray;
    s16 textureWidth;
    s16 textureHeight;

    const Gfx *rippleDisplayList;

    s8 rippleTrigger;

    u8 alpha;

    s8 marioWasUnder;

    s8 marioIsUnder;

    s8 marioWentUnder;

    f32 size;
};

struct PaintingMeshVertex {
     s16 pos[3];
     s8 norm[3];
};

extern s16 gPaintingMarioFloorType;
extern f32 gPaintingMarioXPos;
extern f32 gPaintingMarioYPos;
extern f32 gPaintingMarioZPos;

extern struct PaintingMeshVertex *gPaintingMesh;
extern Vec3f *gPaintingTriNorms;
extern struct Painting *gRipplingPainting;
extern s8 gDDDPaintingStatus;

Gfx *geo_painting_draw(s32 callContext, struct GraphNode *node, UNUSED void *context);
Gfx *geo_painting_update(s32 callContext, UNUSED struct GraphNode *node, UNUSED Mat4 c);

#endif
