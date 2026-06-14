#ifndef ENVFX_BUBBLES_H
#define ENVFX_BUBBLES_H

#include <PR/ultratypes.h>
#include <PR/gbi.h>

#define ENVFX_STATE_UNUSED          0
#define ENVFX_STATE_SRC_X           1
#define ENVFX_STATE_SRC_Y           2
#define ENVFX_STATE_SRC_Z           3
#define ENVFX_STATE_DEST_X          4
#define ENVFX_STATE_DEST_Y          5
#define ENVFX_STATE_DEST_Z          6
#define ENVFX_STATE_PARTICLECOUNT   7
#define ENVFX_STATE_PITCH           8
#define ENVFX_STATE_YAW             9

extern s16 gEnvFxBubbleConfig[10];
Gfx *envfx_update_bubbles(s32 mode, Vec3s marioPos, Vec3s camTo, Vec3s camFrom);

#endif
