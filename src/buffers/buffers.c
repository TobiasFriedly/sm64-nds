#include <ultra64.h>

#include "buffers.h"

#ifndef TARGET_NDS
ALIGNED8 u8 gDecompressionHeap[0xD000];
#endif

#if defined(TARGET_NDS)
ALIGNED16 u8 gAudioHeap[0x17000];
#elif defined(VERSION_EU)
ALIGNED16 u8 gAudioHeap[DOUBLE_SIZE_ON_64_BIT(0x31200) - 0x3800];
#elif defined(VERSION_SH)
ALIGNED16 u8 gAudioHeap[DOUBLE_SIZE_ON_64_BIT(0x31200) - 0x4800];
#elif defined(VERSION_CN)
ALIGNED16 u8 gAudioHeap[DOUBLE_SIZE_ON_64_BIT(0x31200) - 0x4C00];
#else
ALIGNED16 u8 gAudioHeap[DOUBLE_SIZE_ON_64_BIT(0x31200)];
#endif

#ifdef VERSION_CN

ALIGNED8 struct SaveBuffer gSaveBuffer;

struct GfxPool gGfxPools[2];
ALIGNED8 u8 gThread4Stack[STACKSIZE];
#if ENABLE_RUMBLE
ALIGNED8 u8 gThread6Stack[STACKSIZE];
#endif
ALIGNED8 u8 gThread5Stack[STACKSIZE];

ALIGNED8 u8 gGfxSPTaskStack[SP_DRAM_STACK_SIZE8];

ALIGNED8 u8 gGfxSPTaskYieldBuffer[OS_YIELD_DATA_SIZE];
ALIGNED8 u8 gThread3Stack[STACKSIZE + 0x400];
ALIGNED8 u8 gIdleThreadStack[IDLE_STACKSIZE];

#else

#ifndef TARGET_NDS
ALIGNED8 u8 gIdleThreadStack[IDLE_STACKSIZE];
ALIGNED8 u8 gThread3Stack[STACKSIZE];
ALIGNED8 u8 gThread4Stack[STACKSIZE];
ALIGNED8 u8 gThread5Stack[STACKSIZE];
#if ENABLE_RUMBLE
ALIGNED8 u8 gThread6Stack[STACKSIZE];
#endif

ALIGNED8 u8 gGfxSPTaskStack[SP_DRAM_STACK_SIZE8];

ALIGNED8 u8 gGfxSPTaskYieldBuffer[OS_YIELD_DATA_SIZE];
#endif

ALIGNED8 struct SaveBuffer gSaveBuffer;

struct GfxPool gGfxPools[2];

#endif

#ifdef VERSION_JP
ALIGNED8 u8 gAudioSPTaskYieldBuffer[OS_YIELD_AUDIO_SIZE];
#endif

#if !defined(F3DEX_GBI_SHARED) && !defined(VERSION_EU)
ALIGNED8 u8 gUnusedThread2Stack[UNUSED_STACKSIZE];
#endif
