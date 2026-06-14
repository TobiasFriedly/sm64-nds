#ifndef FRAMEBUFFERS_H
#define FRAMEBUFFERS_H

#include <PR/ultratypes.h>

#include "config.h"

#ifdef TARGET_NDS

#define gFramebuffer0 ((u16 *)0)
#define gFramebuffer1 ((u16 *)0)
#define gFramebuffer2 ((u16 *)0)
#else

#ifdef AVOID_UB
extern u16 gFramebuffers[3][SCREEN_WIDTH * SCREEN_HEIGHT];
#define gFramebuffer0 gFramebuffers[0]
#define gFramebuffer1 gFramebuffers[1]
#define gFramebuffer2 gFramebuffers[2]
#else
extern u16 gFramebuffer0[SCREEN_WIDTH * SCREEN_HEIGHT];
extern u16 gFramebuffer1[SCREEN_WIDTH * SCREEN_HEIGHT];
extern u16 gFramebuffer2[SCREEN_WIDTH * SCREEN_HEIGHT];
#endif
#endif

#endif
