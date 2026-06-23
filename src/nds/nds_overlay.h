#ifndef NDS_OVERLAY_H
#define NDS_OVERLAY_H

#include <PR/ultratypes.h>

#define NDS_OVERLAY_BASE    ((void *)0x02280000)
#define NDS_OVERLAY_SIZE    0x00100000

s32 nds_load_level_overlay(s32 levelId);

u8 *nds_load_sound_data(const char *filename, u32 *outSize);

u8 *nds_load_mario_anims(u32 *outSize);

#endif
