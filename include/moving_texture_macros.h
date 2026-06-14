#ifndef MOVING_TEXTURE_MACROS_H
#define MOVING_TEXTURE_MACROS_H

#include "game/moving_texture.h"

#define TEXTURE_WATER            0
#define TEXTURE_MIST             1
#define TEXTURE_JRB_WATER        2
#define TEXTURE_UNK_WATER        3
#define TEXTURE_LAVA             4
#define TEX_QUICKSAND_SSL        5
#define TEX_PYRAMID_SAND_SSL     6
#define TEX_YELLOW_TRI_TTC       7

#define ROTATE_CLOCKWISE         0
#define ROTATE_COUNTER_CLOCKWISE 1

#define MOV_TEX_INIT_LOAD(amount) \
    amount, 0

#define MOV_TEX_4_BOX_TRIS(x, z) \
    x, z

#define MOV_TEX_DEFINE(text) \
    text

#define MOV_TEX_SPD(speed) \
    speed

#define MOV_TEX_ROT_SPEED(rotspeed) \
    rotspeed

#define MOV_TEX_ROT_SCALE(rotscale) \
    rotscale

#define MOV_TEX_ROT(rot) \
    rot

#define MOV_TEX_ALPHA(alpha) \
    alpha

#define MOV_TEX_TRIS(x, y, z, param1, param2) \
    x, y, z, param1, param2

#define MOV_TEX_ROT_TRIS(x, y, z, rotx, roty, rotz, param1, param2) \
    x, y, z, rotx, roty, rotz, param1, param2

#define MOV_TEX_LIGHT_TRIS(x, y, z, light, param1, param2) \
    x, y, z, 0, light, 0, param1, param2

#define MOV_TEX_END() \
    0

#define MOV_TEX_ROT_END() \
    0, 0

#endif
