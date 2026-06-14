#ifndef AUDIO_HEAP_H
#define AUDIO_HEAP_H

#include <PR/ultratypes.h>

#include "internal.h"

#define SOUND_LOAD_STATUS_NOT_LOADED     0
#define SOUND_LOAD_STATUS_IN_PROGRESS    1
#define SOUND_LOAD_STATUS_COMPLETE       2
#define SOUND_LOAD_STATUS_DISCARDABLE    3
#define SOUND_LOAD_STATUS_4              4
#define SOUND_LOAD_STATUS_5              5

#define IS_BANK_LOAD_COMPLETE(bankId) (gBankLoadStatus[bankId] >= SOUND_LOAD_STATUS_COMPLETE)
#define IS_SEQ_LOAD_COMPLETE(seqId) (gSeqLoadStatus[seqId] >= SOUND_LOAD_STATUS_COMPLETE)

struct SoundAllocPool {
    u8 *start;
    u8 *cur;
    u32 size;
    s32 numAllocatedEntries;
};

struct SeqOrBankEntry {
    u8 *ptr;
    u32 size;
#if defined(VERSION_SH) || defined(VERSION_CN)
    s16 poolIndex;
    s16 id;
#else
    s32 id;
#endif
};

struct PersistentPool {
     u32 numEntries;
     struct SoundAllocPool pool;
     struct SeqOrBankEntry entries[32];
};

struct TemporaryPool {

     u32 nextSide;
     struct SoundAllocPool pool;

     struct SeqOrBankEntry entries[2];

};

struct SoundMultiPool {
     struct PersistentPool persistent;
     struct TemporaryPool temporary;
     u32 pad2[4];
};

struct Unk1Pool {
    struct SoundAllocPool pool;
    struct SeqOrBankEntry entries[32];
};

struct UnkEntry {
    s8 used;
    s8 medium;
    s8 bankId;
    u32 pad;
    u8 *srcAddr;
    u8 *dstAddr;
    u32 size;
};

struct UnkPool {
      struct SoundAllocPool pool;
      struct UnkEntry entries[64];
     s32 numEntries;
     u32 unk514;
};

extern u8 gAudioHeap[];
extern s16 gVolume;
extern s8 gReverbDownsampleRate;
extern struct SoundAllocPool gAudioInitPool;
extern struct SoundAllocPool gNotesAndBuffersPool;
extern struct SoundAllocPool gPersistentCommonPool;
extern struct SoundAllocPool gTemporaryCommonPool;
extern struct SoundMultiPool gSeqLoadedPool;
extern struct SoundMultiPool gBankLoadedPool;
#if defined(VERSION_SH) || defined(VERSION_CN)
extern struct Unk1Pool gUnkPool1;
extern struct UnkPool gUnkPool2;
extern struct UnkPool gUnkPool3;
#endif
extern u8 gBankLoadStatus[64];
extern u8 gSeqLoadStatus[256];
extern volatile u8 gAudioResetStatus;
extern u8 gAudioResetPresetIdToLoad;

#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
extern volatile u8 gAudioResetStatus;
#endif

void *soundAlloc(struct SoundAllocPool *pool, u32 size);
void *sound_alloc_uninitialized(struct SoundAllocPool *pool, u32 size);
void sound_init_main_pools(s32 sizeForAudioInitPool);
void sound_alloc_pool_init(struct SoundAllocPool *pool, void *memAddr, u32 size);
#if defined(VERSION_SH) || defined(VERSION_CN)
void *alloc_bank_or_seq(s32 poolIdx, s32 size, s32 arg3, s32 id);
void *get_bank_or_seq(s32 poolIdx, s32 arg1, s32 id);
#else
void *alloc_bank_or_seq(struct SoundMultiPool *arg0, s32 arg1, s32 size, s32 arg3, s32 id);
void *get_bank_or_seq(struct SoundMultiPool *arg0, s32 arg1, s32 id);
#endif
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
s32 audio_shut_down_and_reset_step(void);
void audio_reset_session(void);
#else
void audio_reset_session(struct AudioSessionSettings *preset);
#endif
void discard_bank(s32 bankId);

#if defined(VERSION_SH) || defined(VERSION_CN)
void fill_filter(s16 filter[8], s32 arg1, s32 arg2);
u8 *func_sh_802f1d40(u32 size, s32 bank, u8 *arg2, s8 medium);
u8 *func_sh_802f1d90(u32 size, s32 bank, u8 *arg2, s8 medium);
void *unk_pool1_lookup(s32 poolIdx, s32 id);
void *unk_pool1_alloc(s32 poolIndex, s32 arg1, u32 size);
#endif

#endif
