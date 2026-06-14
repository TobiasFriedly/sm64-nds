#ifndef AUDIO_SYNTHESIS_H
#define AUDIO_SYNTHESIS_H

#include "internal.h"

#if defined(VERSION_SH) || defined(VERSION_CN)
#define DEFAULT_LEN_1CH 0x180
#define DEFAULT_LEN_2CH 0x300
#else
#define DEFAULT_LEN_1CH 0x140
#define DEFAULT_LEN_2CH 0x280
#endif

#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
#define MAX_UPDATES_PER_FRAME 5
#else
#define MAX_UPDATES_PER_FRAME 4
#endif

struct ReverbRingBufferItem {
    s16 numSamplesAfterDownsampling;
    s16 chunkLen;
    s16 *toDownsampleLeft;
    s16 *toDownsampleRight;
    s32 startPos;
    s16 lengthA;
    s16 lengthB;
};

struct SynthesisReverb {
     u8 resampleFlags;
     u8 useReverb;
     u8 framesLeftToIgnore;
     u8 curFrame;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
     u8 downsampleRate;
#if defined(VERSION_SH) || defined(VERSION_CN)
     s8 unk5;
#endif
     u16 windowSize;
#endif
#if defined(VERSION_SH) || defined(VERSION_CN)
     u16 unk08;
#endif
     u16 reverbGain;
     u16 resampleRate;
#if defined(VERSION_SH) || defined(VERSION_CN)
     u16 panRight;
     u16 panLeft;
#endif
     s32 nextRingBufferPos;
     s32 unkC;
     s32 bufSizePerChannel;
    struct {
        s16 *left;
        s16 *right;
    } ringBuffer;
     s16 *resampleStateLeft;
     s16 *resampleStateRight;
     s16 *unk24;
     s16 *unk28;
     struct ReverbRingBufferItem items[2][MAX_UPDATES_PER_FRAME];
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)

     s16 *unk100;
     s16 *unk104;
     s16 *unk108;
     s16 *unk10C;
#endif
};
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
extern struct SynthesisReverb gSynthesisReverbs[4];
extern s8 gNumSynthesisReverbs;
extern struct NoteSubEu *gNoteSubsEu;
extern f32 gLeftVolRampings[3][1024];
extern f32 gRightVolRampings[3][1024];
extern f32 *gCurrentLeftVolRamping;
extern f32 *gCurrentRightVolRamping;
#else
extern struct SynthesisReverb gSynthesisReverb;
#endif

#if defined(VERSION_SH) || defined(VERSION_CN)
extern s16 D_SH_803479B4;
#endif

u64 *synthesis_execute(u64 *cmdBuf, s32 *writtenCmds, s16 *aiBuf, s32 bufLen);
#if defined(VERSION_JP) || defined(VERSION_US)
void note_init_volume(struct Note *note);
void note_set_vel_pan_reverb(struct Note *note, f32 velocity, f32 pan, u8 reverbVol);
void note_set_frequency(struct Note *note, f32 frequency);
void note_enable(struct Note *note);
void note_disable(struct Note *note);
#endif

#endif
