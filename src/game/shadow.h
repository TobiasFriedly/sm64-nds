#ifndef SHADOW_H
#define SHADOW_H

#include <PR/ultratypes.h>
#include <PR/gbi.h>

enum ShadowType {
    SHADOW_CIRCLE_9_VERTS = 0,
    SHADOW_CIRCLE_4_VERTS = 1,
    SHADOW_CIRCLE_4_VERTS_FLAT_UNUSED = 2,
    SHADOW_SQUARE_PERMANENT = 10,
    SHADOW_SQUARE_SCALABLE = 11,
    SHADOW_SQUARE_TOGGLABLE = 12,

    SHADOW_RECTANGLE_HARDCODED_OFFSET = 50,
    SHADOW_CIRCLE_PLAYER = 99
};

extern s8 sMarioOnFlyingCarpet;

extern s16 sSurfaceTypeBelowShadow;

extern s8 gShadowAboveWaterOrLava;

extern s8 gMarioOnIceOrCarpet;

Gfx *create_shadow_below_xyz(f32 xPos, f32 yPos, f32 zPos, s16 shadowScale, u8 shadowSolidity, s8 shadowType);

#endif
