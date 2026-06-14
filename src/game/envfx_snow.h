#ifndef ENVFX_SNOW_H
#define ENVFX_SNOW_H

#include <PR/ultratypes.h>
#include "types.h"

#define ENVFX_MODE_NONE     0
#define ENVFX_SNOW_NORMAL   1
#define ENVFX_SNOW_WATER    2
#define ENVFX_SNOW_BLIZZARD 3

#define ENVFX_BUBBLE_START      10

#define ENVFX_FLOWERS           11
#define ENVFX_LAVA_BUBBLES      12
#define ENVFX_WHIRLPOOL_BUBBLES 13
#define ENVFX_JETSTREAM_BUBBLES 14

struct EnvFxParticle {
    s8 isAlive;
    s16 animFrame;
    s32 xPos;
    s32 yPos;
    s32 zPos;
    s32 angleAndDist[2];
    s32 unusedBubbleVar;
    s32 bubbleY;
    u8 filler[24];
};

extern s8 gEnvFxMode;
extern UNUSED s32 D_80330644;

extern struct EnvFxParticle *gEnvFxBuffer;
extern Vec3i gSnowCylinderLastPos;
extern s16 gSnowParticleCount;

Gfx *envfx_update_particles(s32 mode, Vec3s marioPos, Vec3s camTo, Vec3s camFrom);
void orbit_from_positions(Vec3s from, Vec3s to, s16 *radius, s16 *pitch, s16 *yaw);
void rotate_triangle_vertices(Vec3s vertex1, Vec3s vertex2, Vec3s vertex3, s16 pitch, s16 yaw);

#endif
