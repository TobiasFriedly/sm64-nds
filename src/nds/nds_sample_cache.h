#ifndef NDS_SAMPLE_CACHE_H
#define NDS_SAMPLE_CACHE_H

#include <ultra64.h>

struct Note;

s32 nds_scache_init(void);

u32 nds_scache_entry_for(s32 bankId, u32 blobRelOffset);

void nds_scache_bank_loaded(s32 bankId);

void nds_scache_note_resolve(struct Note *note);

void nds_scache_retry_pending(void);

void nds_scache_update(void);

void nds_scache_add_region(u32 base, u32 size);

extern volatile u32 gNdsAudioTick;

#endif
