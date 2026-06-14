#ifndef MACROS_H
#define MACROS_H

#include "platform_info.h"

#ifndef __sgi
#define GLOBAL_ASM(...)
#endif

#if !defined(__sgi) && (!defined(NON_MATCHING) || !defined(AVOID_UB))

#error Matching build is only possible on IDO; please build with NON_MATCHING=1.
#endif

#define ARRAY_COUNT(arr) (s32)(sizeof(arr) / sizeof(arr[0]))

#define GLUE(a, b) a ## b
#define GLUE2(a, b) GLUE(a, b)

#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

#ifdef VERSION_CN
#define UNUSED_CN UNUSED
#else
#define UNUSED_CN
#endif

#ifdef __GNUC__
#define NORETURN __attribute__((noreturn))
#else
#define NORETURN
#endif

#ifdef __GNUC__
#define STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
#else
#define STATIC_ASSERT(cond, msg) typedef char GLUE2(static_assertion_failed, __LINE__)[(cond) ? 1 : -1]
#endif

#ifdef __GNUC__
#define ALIGNED8 __attribute__((aligned(8)))
#else
#define ALIGNED8
#endif

#ifdef __GNUC__
#define ALIGNED16 __attribute__((aligned(16)))
#else
#define ALIGNED16
#endif

#if defined(TARGET_NDS) || defined(NO_SEGMENTED_MEMORY)

#define VIRTUAL_TO_PHYSICAL(addr)   ((uintptr_t)(addr))
#define PHYSICAL_TO_VIRTUAL(addr)   ((uintptr_t)(addr))
#define VIRTUAL_TO_PHYSICAL2(addr)  ((void *)(addr))
#else

#define VIRTUAL_TO_PHYSICAL(addr)   ((uintptr_t)(addr) & 0x1FFFFFFF)
#define PHYSICAL_TO_VIRTUAL(addr)   ((uintptr_t)(addr) | 0x80000000)
#define VIRTUAL_TO_PHYSICAL2(addr)  ((u8 *)(addr) - 0x80000000U)
#endif

#ifdef VERSION_CN
#define CN_DEBUG_PRINTF(args) osSyncPrintf args
#else
#define CN_DEBUG_PRINTF(args)
#endif

#ifdef VERSION_CN
#define FORCE_BSS __attribute__((nocommon)) __attribute__((section (".bss_cn")))
#else
#define FORCE_BSS
#endif

#endif
