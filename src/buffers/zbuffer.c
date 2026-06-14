#include <ultra64.h>

#include "config.h"
#include "zbuffer.h"

#ifndef TARGET_NDS

ALIGNED8 u16 gZBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
#endif
