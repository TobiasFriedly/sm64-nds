#ifndef ZBUFFER_H
#define ZBUFFER_H

#include <PR/ultratypes.h>

#include "config.h"
#include "macros.h"

#ifdef TARGET_NDS
#define gZBuffer ((u16 *)0)
#else
extern u16 gZBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
#endif
extern s32 gZBufferEnd;

#endif
