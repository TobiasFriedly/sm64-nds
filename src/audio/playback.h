#ifndef AUDIO_PLAYBACK_H
#define AUDIO_PLAYBACK_H

#include <PR/ultratypes.h>

#include "internal.h"

#define NOTE_ALLOC_LAYER 1
#define NOTE_ALLOC_CHANNEL 2
#define NOTE_ALLOC_SEQ 4
#define NOTE_ALLOC_GLOBAL_FREELIST 8

void process_notes(void);
void seq_channel_layer_note_decay(struct SequenceChannelLayer *seqLayer);
void seq_channel_layer_note_release(struct SequenceChannelLayer *seqLayer);
void init_synthetic_wave(struct Note *note, struct SequenceChannelLayer *seqLayer);
void init_note_lists(struct NotePool *pool);
void init_note_free_list(void);
void note_pool_clear(struct NotePool *pool);
void note_pool_fill(struct NotePool *pool, s32 count);
void audio_list_push_front(struct AudioListItem *list, struct AudioListItem *item);
void audio_list_remove(struct AudioListItem *item);
struct Note *alloc_note(struct SequenceChannelLayer *seqLayer);
void reclaim_notes(void);
void note_init_all(void);

#if defined(VERSION_SH)
void note_set_vel_pan_reverb(struct Note *note, struct ReverbInfo *reverbInfo);
#elif defined(VERSION_EU)
void note_set_vel_pan_reverb(struct Note *note, f32 velocity, u8 pan, u8 reverbVol);
#endif

#if defined(VERSION_EU) || defined(VERSION_SH)
struct AudioBankSound *instrument_get_audio_bank_sound(struct Instrument *instrument, s32 semitone);
struct Instrument *get_instrument_inner(s32 bankId, s32 instId);
struct Drum *get_drum(s32 bankId, s32 drumId);
void note_init_volume(struct Note *note);
void note_set_frequency(struct Note *note, f32 frequency);
void note_enable(struct Note *note);
void note_disable(struct Note *note);
#endif

#endif
