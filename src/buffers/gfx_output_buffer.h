#ifndef GFX_OUTPUT_BUFFER_H
#define GFX_OUTPUT_BUFFER_H

#include <PR/ultratypes.h>

#ifdef VERSION_EU

#define GFX_OUTPUT_BUFFER_SIZE 0x2fc0
#else

#define GFX_OUTPUT_BUFFER_SIZE 0x3e00
#endif

#ifdef TARGET_NDS
#define gGfxSPTaskOutputBuffer ((u64 *)0)
#else
extern u64 gGfxSPTaskOutputBuffer[GFX_OUTPUT_BUFFER_SIZE];
#endif

#endif
