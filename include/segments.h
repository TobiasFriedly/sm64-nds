#ifndef SEGMENTS_H
#define SEGMENTS_H

#include "config.h"

#define SEG_START         0x8005C000

#define SEG_FRAMEBUFFERS_SIZE (2 * SCREEN_WIDTH * SCREEN_HEIGHT * 3)
#define SEG_GODDARD_POOL_OFFSET 0x52000
#define SEG_GODDARD       (SEG_POOL_END - SEG_GODDARD_POOL_OFFSET)

#ifndef USE_EXT_RAM

#define RDRAM_END         0x80400000

#define SEG_POOL_START    SEG_START
#define SEG_POOL_SIZE     0x165000
#define SEG_POOL_END      (SEG_POOL_START + SEG_POOL_SIZE)

#define SEG_BUFFERS       SEG_POOL_END

#ifdef VERSION_EU
#define SEG_ENGINE        0x8036FF00
#else
#define SEG_ENGINE        0x80378800
#endif

#define SEG_FRAMEBUFFERS  (RDRAM_END - SEG_FRAMEBUFFERS_SIZE)

#else

#ifdef VERSION_CN
#define RDRAM_END         0x807C0000
#else
#define RDRAM_END         0x80800000
#endif

#define SEG_BUFFERS       SEG_START
#define SEG_ENGINE        ((u32) &_engineSegmentStart)
#define SEG_FRAMEBUFFERS  ((u32) &_framebuffersSegmentNoloadStart)
#define SEG_POOL_START    ((u32) &_framebuffersSegmentNoloadEnd)
#define SEG_POOL_END      RDRAM_END
#define SEG_POOL_END_4MB  0x80400000

#endif

#endif
