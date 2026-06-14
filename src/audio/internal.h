#ifndef AUDIO_INTERNAL_H
#define AUDIO_INTERNAL_H

#include <ultra64.h>

#include "types.h"

#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
#define SEQUENCE_PLAYERS 4
#define SEQUENCE_CHANNELS 48
#define SEQUENCE_LAYERS 64
#else
#define SEQUENCE_PLAYERS 3
#define SEQUENCE_CHANNELS 32
#ifdef VERSION_JP
#define SEQUENCE_LAYERS 48
#else
#define SEQUENCE_LAYERS 52
#endif
#endif

#define LAYERS_MAX       4
#define CHANNELS_MAX     16

#define NO_LAYER ((struct SequenceChannelLayer *)(-1))

#define MUTE_BEHAVIOR_STOP_SCRIPT 0x80
#define MUTE_BEHAVIOR_STOP_NOTES 0x40
#define MUTE_BEHAVIOR_SOFTEN 0x20

#define SEQUENCE_PLAYER_STATE_0 0
#define SEQUENCE_PLAYER_STATE_FADE_OUT 1
#define SEQUENCE_PLAYER_STATE_2 2
#define SEQUENCE_PLAYER_STATE_3 3
#define SEQUENCE_PLAYER_STATE_4 4

#define NOTE_PRIORITY_DISABLED 0
#define NOTE_PRIORITY_STOPPING 1
#define NOTE_PRIORITY_MIN 2
#define NOTE_PRIORITY_DEFAULT 3

#define TATUMS_PER_BEAT 48

#define CODEC_ADPCM 0
#define CODEC_S8 1
#define CODEC_SKIP 2

#ifdef VERSION_JP
#define TEMPO_SCALE 1
#else
#define TEMPO_SCALE TATUMS_PER_BEAT
#endif

#ifdef VERSION_JP
#define US_FLOAT(x) x
#else
#define US_FLOAT(x) x ## f
#endif

#ifdef VERSION_JP
#define FLOAT_CAST(x) (f32) (x)
#else
#define FLOAT_CAST(x) (f32) (s32) (x)
#endif

#ifdef __sgi
#define stubbed_printf
#else
#define stubbed_printf(...)
#endif

#ifdef VERSION_EU
#define eu_stubbed_printf_0(msg) stubbed_printf(msg)
#define eu_stubbed_printf_1(msg, a) stubbed_printf(msg, a)
#define eu_stubbed_printf_2(msg, a, b) stubbed_printf(msg, a, b)
#define eu_stubbed_printf_3(msg, a, b, c) stubbed_printf(msg, a, b, c)
#else
#define eu_stubbed_printf_0(msg)
#define eu_stubbed_printf_1(msg, a)
#define eu_stubbed_printf_2(msg, a, b)
#define eu_stubbed_printf_3(msg, a, b, c)
#endif

struct NotePool;

struct AudioListItem {

    struct AudioListItem *prev;
    struct AudioListItem *next;
    union {
        void *value;
        s32 count;
    } u;
    struct NotePool *pool;
};

struct NotePool {
    struct AudioListItem disabled;
    struct AudioListItem decaying;
    struct AudioListItem releasing;
    struct AudioListItem active;
};

struct VibratoState {
     struct SequenceChannel *seqChannel;
     u32 time;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
     s16 *curve;
     f32 extent;
     f32 rate;
     u8 active;
#else
     s8 *curve;
     u8 active;
     u16 rate;
     u16 extent;
#endif
     u16 rateChangeTimer;
     u16 extentChangeTimer;
     u16 delay;
};

struct Portamento {
    u8 mode;
    f32 cur;
    f32 speed;
    f32 extent;
};

struct AdsrEnvelope {
    s16 delay;
    s16 arg;
};

struct AdpcmLoop {
    u32 start;
    u32 end;
    u32 count;
    u32 pad;
    s16 state[16];
};

struct AdpcmBook {
    s32 order;
    s32 npredictors;
    s16 book[1];
};

struct AudioBankSample {
#if defined(VERSION_SH) || defined(VERSION_CN)
     u32 codec : 4;
     u32 medium : 2;
     u32 bit1 : 1;
     u32 isPatched : 1;
     u32 size : 24;
#else
    u8 unused;
    u8 loaded;
#endif
    u8 *sampleAddr;
    struct AdpcmLoop *loop;
    struct AdpcmBook *book;
#if !defined(VERSION_SH) && !defined(VERSION_CN)
    u32 sampleSize;
#endif
};

struct AudioBankSound {
    struct AudioBankSample *sample;
    f32 tuning;
};

struct Instrument {
     u8 loaded;
     u8 normalRangeLo;
     u8 normalRangeHi;
     u8 releaseRate;
     struct AdsrEnvelope *envelope;
     struct AudioBankSound lowNotesSound;
     struct AudioBankSound normalNotesSound;
     struct AudioBankSound highNotesSound;
};

struct Drum {
    u8 releaseRate;
    u8 pan;
    u8 loaded;
    struct AudioBankSound sound;
    struct AdsrEnvelope *envelope;
};

struct AudioBank {
    struct Drum **drums;
    struct Instrument *instruments[1];
};

struct CtlEntry {
#if !defined(VERSION_SH) && !defined(VERSION_CN)
    u8 unused;
#endif
    u8 numInstruments;
    u8 numDrums;
#if defined(VERSION_SH) || defined(VERSION_CN)
    u8 bankId1;
    u8 bankId2;
#endif
    struct Instrument **instruments;
    struct Drum **drums;
};

struct M64ScriptState {
    u8 *pc;
    u8 *stack[4];
    u8 remLoopIters[4];
    u8 depth;
};

struct SequencePlayer {

#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
     u8 enabled : 1;
#else
     volatile u8 enabled : 1;
#endif
     u8 finished : 1;
     u8 muted : 1;
     u8 seqDmaInProgress : 1;
     u8 bankDmaInProgress : 1;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
     u8 recalculateVolume : 1;
#endif
#if defined(VERSION_SH) || defined(VERSION_CN)
     u8 unkSh: 1;
#endif
#if defined(VERSION_JP) || defined(VERSION_US)
     s8 seqVariation;
#endif
     u8 state;
     u8 noteAllocPolicy;
     u8 muteBehavior;
     u8 seqId;
     u8 defaultBank[1];

     u8 loadingBankId;
#if defined(VERSION_JP) || defined(VERSION_US)
     u8 loadingBankNumInstruments;
     u8 loadingBankNumDrums;
#endif
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
     s8 seqVariationEu[1];
#endif
     u16 tempo;
     u16 tempoAcc;
#if defined(VERSION_JP) || defined(VERSION_US)
     u16 fadeRemainingFrames;
#endif
#if defined(VERSION_SH) || defined(VERSION_CN)
     s16 tempoAdd;
#endif
     s16 transposition;
     u16 delay;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
     u16 fadeRemainingFrames;
     u16 fadeTimerUnkEu;
#endif
     u8 *seqData;
     f32 fadeVolume;
     f32 fadeVelocity;
     f32 volume;
     f32 muteVolumeScale;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
     f32 fadeVolumeScale;
     f32 appliedFadeVolume;
#else
     u8 pad2[4];
#endif
     struct SequenceChannel *channels[CHANNELS_MAX];
     struct M64ScriptState scriptState;
     u8 *shortNoteVelocityTable;
     u8 *shortNoteDurationTable;
     struct NotePool notePool;
     OSMesgQueue seqDmaMesgQueue;
     OSMesg seqDmaMesg;
     OSIoMesg seqDmaIoMesg;
     OSMesgQueue bankDmaMesgQueue;
     OSMesg bankDmaMesg;
     OSIoMesg bankDmaIoMesg;
     u8 *bankDmaCurrMemAddr;
#if defined(VERSION_JP) || defined(VERSION_US)
     struct AudioBank *loadingBank;
#endif
     uintptr_t bankDmaCurrDevAddr;
     ssize_t bankDmaRemaining;
};

struct AdsrSettings {
    u8 releaseRate;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
    u8 sustain;
#else
    u16 sustain;
#endif
    struct AdsrEnvelope *envelope;
};

struct AdsrState {
     u8 action;
     u8 state;
#if defined(VERSION_JP) || defined(VERSION_US)
     s16 initial;
     s16 target;
     s16 current;
#endif
     s16 envIndex;
     s16 delay;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
     f32 sustain;
     f32 velocity;
     f32 fadeOutVel;
     f32 current;
     f32 target;
    s32 pad1C;
#else
     s16 sustain;
     s16 fadeOutVel;
     s32 velocity;
     s32 currentHiRes;
     s16 *volOut;
#endif
     struct AdsrEnvelope *envelope;
};

struct ReverbBitsData {
     u8 bit0 : 1;
     u8 bit1 : 1;
     u8 bit2 : 1;
     u8 usesHeadsetPanEffects : 1;
     u8 stereoHeadsetEffects : 2;
     u8 strongRight : 1;
     u8 strongLeft : 1;
};

union ReverbBits {
     struct ReverbBitsData s;
     u8 asByte;
};
struct ReverbInfo {
    u8 reverbVol;
    u8 synthesisVolume;
    u8 pan;
    union ReverbBits reverbBits;
    f32 freqScale;
    f32 velocity;
    s32 unused;
    s16 *filter;
};

struct NoteAttributes {
    u8 reverbVol;
#if defined(VERSION_SH) || defined(VERSION_CN)
    u8 synthesisVolume;
#endif
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
    u8 pan;
#endif
#if defined(VERSION_SH) || defined(VERSION_CN)
    union ReverbBits reverbBits;
#endif
    f32 freqScale;
    f32 velocity;
#if defined(VERSION_JP) || defined(VERSION_US)
    f32 pan;
#endif
#if defined(VERSION_SH) || defined(VERSION_CN)
    s16 *filter;
#endif
};

struct SequenceChannel {

     u8 enabled : 1;
     u8 finished : 1;
     u8 stopScript : 1;
     u8 stopSomething2 : 1;
     u8 hasInstrument : 1;
     u8 stereoHeadsetEffects : 1;
     u8 largeNotes : 1;
     u8 unused : 1;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
     union {
        struct {
            u8 freqScale : 1;
            u8 volume : 1;
            u8 pan : 1;
        } as_bitfields;
        u8 as_u8;
    } changes;
#endif
     u8 noteAllocPolicy;
     u8 muteBehavior;
     u8 reverbVol;
     u8 notePriority;
#if defined(VERSION_SH) || defined(VERSION_CN)
                   u8 unkSH06;
#endif
     u8 bankId;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
     u8 reverbIndex;
     u8 bookOffset;
     u8 newPan;
     u8 panChannelWeight;
#else
     u8 updatesPerFrameUnused;
#endif
#if defined(VERSION_SH) || defined(VERSION_CN)
     u8 synthesisVolume;
#endif
     u16 vibratoRateStart;
     u16 vibratoExtentStart;
     u16 vibratoRateTarget;
     u16 vibratoExtentTarget;
     u16 vibratoRateChangeDelay;
     u16 vibratoExtentChangeDelay;
     u16 vibratoDelay;
     u16 delay;
     s16 instOrWave;

     s16 transposition;
     f32 volumeScale;
     f32 volume;
#if defined(VERSION_JP) || defined(VERSION_US)
     f32 pan;
     f32 panChannelWeight;
#else
     s32 pan;
     f32 appliedVolume;
#endif
     f32 freqScale;
     u8 (*dynTable)[][2];
     struct Note *noteUnused;
     struct SequenceChannelLayer *layerUnused;
     struct Instrument *instrument;
     struct SequencePlayer *seqPlayer;
     struct SequenceChannelLayer *layers[LAYERS_MAX];
#if !defined(VERSION_SH) && !defined(VERSION_CN)
     s8 soundScriptIO[8];

#endif
     struct M64ScriptState scriptState;
     struct AdsrSettings adsr;
     struct NotePool notePool;
#if defined(VERSION_SH) || defined(VERSION_CN)
     s8 soundScriptIO[8];

     u16 unkC8;
     s16 *filter;
#endif
};

struct SequenceChannelLayer {

     u8 enabled : 1;
     u8 finished : 1;
     u8 stopSomething : 1;
     u8 continuousNotes : 1;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
     u8 unusedEu0b8 : 1;
     u8 notePropertiesNeedInit : 1;
     u8 ignoreDrumPan : 1;
#if defined(VERSION_SH) || defined(VERSION_CN)
     union ReverbBits reverbBits;
#endif
     u8 instOrWave;
#endif
     u8 status;
     u8 noteDuration;
     u8 portamentoTargetNote;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
     u8 pan;
     u8 notePan;
#endif
     struct Portamento portamento;
     struct AdsrSettings adsr;
     u16 portamentoTime;
     s16 transposition;

     f32 freqScale;
#if defined(VERSION_SH) || defined(VERSION_CN)
     f32 freqScaleMultiplier;
#endif
     f32 velocitySquare;
#if defined(VERSION_JP) || defined(VERSION_US)
     f32 pan;
#endif
     f32 noteVelocity;
#if defined(VERSION_JP) || defined(VERSION_US)
     f32 notePan;
#endif
     f32 noteFreqScale;
     s16 shortNoteDefaultPlayPercentage;
     s16 playPercentage;
     s16 delay;
     s16 duration;
     s16 delayUnused;
     struct Note *note;
     struct Instrument *instrument;
     struct AudioBankSound *sound;
     struct SequenceChannel *seqChannel;
     struct M64ScriptState scriptState;
     struct AudioListItem listItem;
#if defined(VERSION_EU)
    u8 pad2[4];
#endif
};

#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
struct NoteSynthesisState {
     u8 restart;
     u8 sampleDmaIndex;
     u8 prevHeadsetPanRight;
     u8 prevHeadsetPanLeft;
#if defined(VERSION_SH) || defined(VERSION_CN)
     u8 reverbVol;
     u8 unk5;
#endif
     u16 samplePosFrac;
     s32 samplePosInt;
     struct NoteSynthesisBuffers *synthesisBuffers;
     s16 curVolLeft;
     s16 curVolRight;
};
struct NotePlaybackState {

     u8 priority;
     u8 waveId;
     u8 sampleCountIndex;
#if defined(VERSION_SH) || defined(VERSION_CN)
     u8 bankId;
     u8 unkSH34;
#endif
     s16 adsrVolScale;
     f32 portamentoFreqScale;
     f32 vibratoFreqScale;
     struct SequenceChannelLayer *prevParentLayer;
     struct SequenceChannelLayer *parentLayer;
     struct SequenceChannelLayer *wantedParentLayer;
     struct NoteAttributes attributes;
     struct AdsrState adsr;
     struct Portamento portamento;
     struct VibratoState vibratoState;
};
struct NoteSubEu {
     volatile u8 enabled : 1;
     u8 needsInit : 1;
     u8 finished : 1;
     u8 envMixerNeedsInit : 1;
     u8 stereoStrongRight : 1;
     u8 stereoStrongLeft : 1;
     u8 stereoHeadsetEffects : 1;
     u8 usesHeadsetPanEffects : 1;
     u8 reverbIndex : 3;
     u8 bookOffset : 3;
     u8 isSyntheticWave : 1;
     u8 hasTwoAdpcmParts : 1;
#ifdef VERSION_EU
     u8 bankId;
#else
     u8 synthesisVolume;
#endif
     u8 headsetPanRight;
     u8 headsetPanLeft;
     u8 reverbVol;
     u16 targetVolLeft;
     u16 targetVolRight;
     u16 resamplingRateFixedPoint;
     union {
        s16 *samples;
        struct AudioBankSound *audioBankSound;
    } sound;
#if defined(VERSION_SH) || defined(VERSION_CN)
     s16 *filter;
#endif
};
struct Note {

     struct AudioListItem listItem;
     struct NoteSynthesisState synthesisState;

#ifdef TARGET_N64
    u8 pad0[12];
#endif

     u8 priority;
     u8 waveId;
     u8 sampleCountIndex;
#if defined(VERSION_SH) || defined(VERSION_CN)
     u8 bankId;
     u8 unkSH34;
#endif
     s16 adsrVolScale;
     f32 portamentoFreqScale;
     f32 vibratoFreqScale;
     struct SequenceChannelLayer *prevParentLayer;
     struct SequenceChannelLayer *parentLayer;
     struct SequenceChannelLayer *wantedParentLayer;
     struct NoteAttributes attributes;
     struct AdsrState adsr;
     struct Portamento portamento;
     struct VibratoState vibratoState;
    u8 pad3[8];
     struct NoteSubEu noteSubEu;
};
#else

struct vNote {

     volatile u8 enabled : 1;
    long long int force_structure_alignment;
};
struct Note {

     u8 enabled : 1;
     u8 needsInit : 1;
     u8 restart : 1;
     u8 finished : 1;
     u8 envMixerNeedsInit : 1;
     u8 stereoStrongRight : 1;
     u8 stereoStrongLeft : 1;
     u8 stereoHeadsetEffects : 1;
     u8 usesHeadsetPanEffects;
     u8 unk2;
     u8 sampleDmaIndex;
     u8 priority;
     u8 sampleCount;
     u8 instOrWave;
     u8 bankId;
     s16 adsrVolScale;
     u8 pad1[2];
     u16 headsetPanRight;
     u16 headsetPanLeft;
     u16 prevHeadsetPanRight;
     u16 prevHeadsetPanLeft;
     s32 samplePosInt;
     f32 portamentoFreqScale;
     f32 vibratoFreqScale;
     u16 samplePosFrac;
     struct AudioBankSound *sound;
     struct SequenceChannelLayer *prevParentLayer;
     struct SequenceChannelLayer *parentLayer;
     struct SequenceChannelLayer *wantedParentLayer;
     struct NoteSynthesisBuffers *synthesisBuffers;
     f32 frequency;
     u16 targetVolLeft;
     u16 targetVolRight;
     u8 reverbVol;
     u8 unused1;
     struct NoteAttributes attributes;
     struct AdsrState adsr;
     struct Portamento portamento;
     struct VibratoState vibratoState;
     s16 curVolLeft;
     s16 curVolRight;
     s16 reverbVolShifted;
     s16 unused2;
     struct AudioListItem listItem;
     u8 pad2[0xc];
#ifdef TARGET_NDS

     u32 ndsSourceFull;
     u32 ndsSourceHalf;
     u16 ndsRepeatFull;
     u16 ndsLengthFull;
     u16 ndsRepeatHalf;
     u16 ndsLengthHalf;
     u16 ndsEntry;
     u8 ndsLoop;
     u8 ndsSeq;

#endif
};
#endif

struct NoteSynthesisBuffers {
    s16 adpcmdecState[0x10];
    s16 finalResampleState[0x10];
#if defined(VERSION_SH) || defined(VERSION_CN)
    s16 unk[0x10];
    s16 filterBuffer[0x20];
    s16 panSamplesBuffer[0x20];
#else
    s16 mixEnvelopeState[0x28];
    s16 panResampleState[0x10];
    s16 panSamplesBuffer[0x20];
    s16 dummyResampleState[0x10];
#if defined(VERSION_JP) || defined(VERSION_US)
    s16 samples[0x40];
#endif
#endif
};

#ifdef VERSION_EU
struct ReverbSettingsEU {
    u8 downsampleRate;
    u8 windowSize;
    u16 gain;
};
#else
struct ReverbSettingsEU {
    u8 downsampleRate;
    u8 windowSize;
    u16 gain;
    u16 unk4;
    u16 unk6;
    s8 unk8;
    u16 unkA;
    s16 unkC;
    s16 unkE;
};
#endif

struct AudioSessionSettingsEU {
     u32 frequency;
     u8 unk1;
     u8 maxSimultaneousNotes;
     u8 numReverbs;
     u8 unk2;
     struct ReverbSettingsEU *reverbSettings;
     u16 volume;
     u16 unk3;
     u32 persistentSeqMem;
     u32 persistentBankMem;
#if defined(VERSION_SH) || defined(VERSION_CN)
     u32 unk18;
#endif
     u32 temporarySeqMem;
     u32 temporaryBankMem;
#if defined(VERSION_SH) || defined(VERSION_CN)
     u32 unk24;
     u32 unkMem28;
     u32 unkMem2C;
#endif
};

struct AudioSessionSettings {
     u32 frequency;
     u8 maxSimultaneousNotes;
     u8 reverbDownsampleRate;
     u16 reverbWindowSize;
     u16 reverbGain;
     u16 volume;
     u32 persistentSeqMem;
     u32 persistentBankMem;
     u32 temporarySeqMem;
     u32 temporaryBankMem;
};

struct AudioBufferParametersEU {
     s16 presetUnk4;
     u16 frequency;
     u16 aiFrequency;
     s16 samplesPerFrameTarget;
     s16 maxAiBufferLength;
     s16 minAiBufferLength;
     s16 updatesPerFrame;
     s16 samplesPerUpdate;
     s16 samplesPerUpdateMax;
     s16 samplesPerUpdateMin;
     f32 resampleRate;
     f32 updatesPerFrameInv;
     f32 unkUpdatesPerFrameScaled;
};

struct EuAudioCmd {
    union {
#if IS_BIG_ENDIAN
        struct {
            u8 op;
            u8 arg1;
            u8 arg2;
            u8 arg3;
        } s;
#else
        struct {
            u8 arg3;
            u8 arg2;
            u8 arg1;
            u8 op;
        } s;
#endif
        s32 first;
    } u;
    union {
        s32 as_s32;
        u32 as_u32;
        f32 as_f32;
#if IS_BIG_ENDIAN
        u8 as_u8;
        s8 as_s8;
#else
        struct {
            u8 pad0[3];
            u8 as_u8;
        };
        struct {
            u8 pad1[3];
            s8 as_s8;
        };
#endif
    } u2;
};

#if defined(VERSION_SH) || defined(VERSION_CN)
struct PendingDmaSample {
    u8 medium;
    u8 bankId;
    u8 idx;
    uintptr_t devAddr;
    void *vAddr;
    u8 *resultSampleAddr;
    s32 state;
    s32 remaining;
    s8 *io;
     struct AudioBankSample sample;
     OSMesgQueue queue;
     OSMesg mesgs[1];
     OSIoMesg ioMesg;
};

struct UnkStruct80343D00 {
    u32 someIndex;
    struct PendingDmaSample arr[2];
};

extern s32 D_SH_80343CF0;
extern struct UnkStruct80343D00 D_SH_80343D00;
#endif

#endif
