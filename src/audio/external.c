#include <ultra64.h>
#include "sm64.h"
#include "heap.h"
#include "load.h"
#include "data.h"
#include "seqplayer.h"
#include "external.h"
#include "playback.h"
#include "synthesis.h"
#include "game/level_update.h"
#include "game/object_list_processor.h"
#include "game/camera.h"
#include "seq_ids.h"
#include "dialog_ids.h"

#ifdef TARGET_NDS
extern u8 gNdsAudioDisabled;
#define NDS_AUDIO_GUARD() do { if (gNdsAudioDisabled) return; } while (0)
#else
#define NDS_AUDIO_GUARD()
#endif

#ifdef TARGET_NDS
#define NDS_ITCM_CODE __attribute__((section(".itcm")))
#else
#define NDS_ITCM_CODE
#endif

#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
#define EU_FLOAT(x) x##f
#else
#define EU_FLOAT(x) x
#endif

#define MAX_BACKGROUND_MUSIC_QUEUE_SIZE 6
#define MAX_CHANNELS_PER_SOUND_BANK 1

#define SEQUENCE_NONE 0xFF

#define SAMPLES_TO_OVERPRODUCE 0x10
#define EXTRA_BUFFERED_AI_SAMPLES_TARGET 0x40

struct Sound {
    s32 soundBits;
    f32 *position;
};

struct ChannelVolumeScaleFade {
    f32 velocity;
    u8 target;
    f32 current;
    u16 remainingFrames;
};

struct SoundCharacteristics {
    f32 *x;
    f32 *y;
    f32 *z;
    f32 distance;
    u32 priority;
    u32 soundBits;
    u8 soundStatus;
    u8 freshness;
    u8 prev;
    u8 next;
};

#define SOUND_MAX_FRESHNESS 10

struct SequenceQueueItem {
    u8 seqId;
    u8 priority;
};

#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)

s32 gAudioErrorFlags2 = 0;
#else
s32 gAudioErrorFlags = 0;
#endif
s32 sGameLoopTicked = 0;

#define UKIKI 0
#define TUXIE 1
#define BOWS1 2
#define KOOPA 3
#define KBOMB 4
#define BOO 5
#define BOMB 6
#define BOWS2 7
#define GRUNT 8
#define WIGLR 9
#define YOSHI 10
#define _ 0xFF

#ifdef VERSION_JP
#define DIFF KOOPA
#else
#define DIFF TUXIE
#endif

u8 sDialogSpeaker[] = {

     _,     BOMB,  BOMB,  BOMB,  BOMB,  KOOPA, KOOPA, KOOPA, _,     KOOPA,
     _,     _,     _,     _,     _,     _,     _,     KBOMB, _,     _,
     _,     BOWS1, BOWS1, BOWS1, BOWS1, BOWS1, BOWS1, BOWS1, BOWS1, BOWS1,
     _,     _,     _,     _,     _,     _,     _,     DIFF,  _,     _,
     _,     KOOPA, _,     _,     _,     _,     _,     BOMB,  _,     _,
     _,     _,     _,     _,     _,     TUXIE, TUXIE, TUXIE, TUXIE, TUXIE,
     _,     _,     _,     _,     _,     _,     _,     BOWS2, _,     _,
     _,     _,     _,     _,     _,     _,     _,     _,     _,     UKIKI,
     UKIKI, _,     _,     _,     _,     BOO,   _,     _,     _,     _,
     BOWS2, _,     BOWS2, BOWS2, _,     _,     _,     _,     BOO,   BOO,
     UKIKI, UKIKI, _,     _,     _,     BOMB,  BOMB,  BOO,   BOO,   _,
     _,     _,     _,     _,     GRUNT, GRUNT, KBOMB, GRUNT, GRUNT, _,
     _,     _,     _,     _,     _,     _,     _,     _,     KBOMB, _,
     _,     _,     TUXIE, _,     _,     _,     _,     _,     _,     _,
     _,     _,     _,     _,     _,     _,     _,     _,     _,     _,
     WIGLR, WIGLR, WIGLR, _,     _,     _,     _,     _,     _,     _,
     _,     YOSHI, _,     _,     _,     _,     _,     _,     WIGLR, _
};
#undef _
STATIC_ASSERT(ARRAY_COUNT(sDialogSpeaker) == DIALOG_COUNT,
              "change this array if you are adding dialogs");

s32 sDialogSpeakerVoice[] = {
    SOUND_OBJ_UKIKI_CHATTER_LONG,
    SOUND_OBJ_BIG_PENGUIN_YELL,
    SOUND_OBJ_BOWSER_INTRO_LAUGH,
    SOUND_OBJ_KOOPA_TALK,
    SOUND_OBJ_KING_BOBOMB_TALK,
    SOUND_OBJ_BOO_LAUGH_LONG,
    SOUND_OBJ_BOBOMB_BUDDY_TALK,
    SOUND_OBJ_BOWSER_LAUGH,
    SOUND_OBJ2_BOSS_DIALOG_GRUNT,
    SOUND_OBJ_WIGGLER_TALK,
    SOUND_GENERAL_YOSHI_TALK,
#if defined(VERSION_JP) || defined(VERSION_US)
    NO_SOUND,
    NO_SOUND,
    NO_SOUND,
    NO_SOUND,
#endif
};

u8 sNumProcessedSoundRequests = 0;
u8 sSoundRequestCount = 0;

#define MARIO_X_GE 0
#define MARIO_Y_GE 1
#define MARIO_Z_GE 2
#define MARIO_X_LT 3
#define MARIO_Y_LT 4
#define MARIO_Z_LT 5
#define MARIO_IS_IN_AREA 6
#define MARIO_IS_IN_ROOM 7

#define DYN1(cond1, val1, res) (s16)(1 << (15 - cond1) | res), val1
#define DYN2(cond1, val1, cond2, val2, res)                                                            \
    (s16)(1 << (15 - cond1) | 1 << (15 - cond2) | res), val1, val2
#define DYN3(cond1, val1, cond2, val2, cond3, val3, res)                                               \
    (s16)(1 << (15 - cond1) | 1 << (15 - cond2) | 1 << (15 - cond3) | res), val1, val2, val3

s16 sDynBBH[] = {
    SEQ_LEVEL_SPOOKY, DYN1(MARIO_IS_IN_ROOM, BBH_OUTSIDE_ROOM, 6),
    DYN1(MARIO_IS_IN_ROOM, BBH_NEAR_MERRY_GO_ROUND_ROOM, 6), 5,
};
s16 sDynDDD[] = {
    SEQ_LEVEL_WATER,
    DYN2(MARIO_X_LT, -800, MARIO_IS_IN_AREA, AREA_DDD_WHIRLPOOL & 0xf, 0),
    DYN3(MARIO_Y_GE, -2000, MARIO_X_LT, 470, MARIO_IS_IN_AREA, AREA_DDD_WHIRLPOOL & 0xf, 0),
    DYN2(MARIO_Y_GE, 100, MARIO_IS_IN_AREA, AREA_DDD_SUB & 0xf, 2),
    1,
};
s16 sDynJRB[] = {
    SEQ_LEVEL_WATER,
    DYN2(MARIO_Y_GE, 945, MARIO_X_LT, -5260, 0),
    DYN1(MARIO_IS_IN_AREA, AREA_JRB_SHIP & 0xf, 0),
    DYN1(MARIO_Y_GE, 1000, 0),
    DYN2(MARIO_Y_GE, -3100, MARIO_Z_LT, -900, 2),
    1,
    5,
};
s16 sDynWDW[] = {
    SEQ_LEVEL_UNDERGROUND, DYN2(MARIO_Y_LT, -670, MARIO_IS_IN_AREA, AREA_WDW_MAIN & 0xf, 4),
    DYN1(MARIO_IS_IN_AREA, AREA_WDW_TOWN & 0xf, 4), 3,
};
s16 sDynHMC[] = {
    SEQ_LEVEL_UNDERGROUND, DYN2(MARIO_X_GE, 0, MARIO_Y_LT, -203, 4),
    DYN2(MARIO_X_LT, 0, MARIO_Y_LT, -2400, 4), 3,
};
s16 sDynUnk38[] = {
    SEQ_LEVEL_UNDERGROUND,
    DYN1(MARIO_IS_IN_AREA, 1, 3),
    DYN1(MARIO_IS_IN_AREA, 2, 4),
    DYN1(MARIO_IS_IN_AREA, 3, 7),
    0,
};
s16 sDynNone[] = { SEQ_SOUND_PLAYER, 0 };

u8 sCurrentMusicDynamic = 0xff;
u8 sBackgroundMusicForDynamics = SEQUENCE_NONE;

#define STUB_LEVEL(_0, _1, _2, _3, _4, _5, _6, leveldyn, _8) leveldyn,
#define DEFINE_LEVEL(_0, _1, _2, _3, _4, _5, _6, _7, _8, leveldyn, _10) leveldyn,

#define _ sDynNone
s16 *sLevelDynamics[LEVEL_COUNT] = {
    _,
#include "levels/level_defines.h"
};
#undef _
#undef STUB_LEVEL
#undef DEFINE_LEVEL

struct MusicDynamic {
     s16 bits1;
     u16 volScale1;
     s16 dur1;
     s16 bits2;
     u16 volScale2;
     s16 dur2;
};

struct MusicDynamic sMusicDynamics[8] = {
    { 0x0000, 127, 100, 0x0e43, 0, 100 },
    { 0x0003, 127, 100, 0x0e40, 0, 100 },
    { 0x0e43, 127, 200, 0x0000, 0, 200 },
    { 0x02ff, 127, 100, 0x0100, 0, 100 },
    { 0x03f7, 127, 100, 0x0008, 0, 100 },
    { 0x0070, 127, 10, 0x0000, 0, 100 },
    { 0x0000, 127, 100, 0x0070, 0, 10 },
    { 0xffff, 127, 100, 0x0000, 0, 100 },
};

#define STUB_LEVEL(_0, _1, _2, _3, echo1, echo2, echo3, _7, _8) { echo1, echo2, echo3 },
#define DEFINE_LEVEL(_0, _1, _2, _3, _4, _5, echo1, echo2, echo3, _9, _10) { echo1, echo2, echo3 },

u8 sLevelAreaReverbs[LEVEL_COUNT][3] = {
    { 0x00, 0x00, 0x00 },
#include "levels/level_defines.h"
};
#undef STUB_LEVEL
#undef DEFINE_LEVEL

#define STUB_LEVEL(_0, _1, _2, volume, _4, _5, _6, _7, _8) volume,
#define DEFINE_LEVEL(_0, _1, _2, _3, _4, volume, _6, _7, _8, _9, _10) volume,

u16 sLevelAcousticReaches[LEVEL_COUNT] = {
    20000,
#include "levels/level_defines.h"
};

#undef STUB_LEVEL
#undef DEFINE_LEVEL

#define AUDIO_MAX_DISTANCE US_FLOAT(22000.0)

#ifdef VERSION_JP
#define LOW_VOLUME_REVERB 48.0
#else
#define LOW_VOLUME_REVERB 40.0f
#endif

#ifdef VERSION_JP
#define VOLUME_RANGE_UNK1 0.8f
#define VOLUME_RANGE_UNK2 1.0f
#else
#define VOLUME_RANGE_UNK1 0.9f
#define VOLUME_RANGE_UNK2 0.8f
#endif

u8 sBackgroundMusicDefaultVolume[] = {
    127,
    80,
    80,
    75,
    70,
    75,
    75,
    75,
    70,
    65,
    80,
    65,
    85,
    75,
    65,
    70,
    65,
    70,
    70,
    65,
    80,
    70,
    85,
    75,
    75,
    85,
    70,
    80,
    80,
    70,
    75,
    80,
    70,
    65,
    0,
};

STATIC_ASSERT(ARRAY_COUNT(sBackgroundMusicDefaultVolume) == SEQ_COUNT,
              "change this array if you are adding sequences");

u8 sCurrentBackgroundMusicSeqId = SEQUENCE_NONE;
u8 sMusicDynamicDelay = 0;
u8 sSoundBankUsedListBack[SOUND_BANK_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
u8 sSoundBankFreeListFront[SOUND_BANK_COUNT] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
u8 sNumSoundsInBank[SOUND_BANK_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
u8 sMaxChannelsForSoundBank[SOUND_BANK_COUNT] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

#ifdef VERSION_JP
#define BANK27_SIZE 0x30
#else
#define BANK27_SIZE 0x40
#endif
u8 sNumSoundsPerBank[SOUND_BANK_COUNT] = {
    0x70, 0x30, BANK27_SIZE, 0x80, 0x20, 0x80, 0x20, BANK27_SIZE, 0x80, 0x80,
};
#undef BANK27_SIZE

#define TARGET_VOLUME_IS_PRESENT_FLAG 0x80
#define TARGET_VOLUME_VALUE_MASK 0x7f
#define TARGET_VOLUME_UNSET 0x00

f32 gGlobalSoundSource[3] = { 0.0f, 0.0f, 0.0f };
f32 sUnusedSoundArgs[3] = { 1.0f, 1.0f, 1.0f };
u8 sSoundBankDisabled[16] = { 0 };
u8 D_80332108 = 0;
u8 sHasStartedFadeOut = FALSE;
u16 sSoundBanksThatLowerBackgroundMusic = 0;
u8 sUnused80332114 = 0;
u16 sUnused80332118 = 0;
u8 sBackgroundMusicMaxTargetVolume = TARGET_VOLUME_UNSET;
u8 D_80332120 = 0;
u8 D_80332124 = 0;

#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
u8 D_EU_80300558 = 0;
#endif

u8 sBackgroundMusicQueueSize = 0;

#ifndef VERSION_JP
u8 sUnused8033323C = 0;
#endif

#if defined(VERSION_JP) || defined(VERSION_US)
s16 *gCurrAiBuffer;
#endif
#if defined(VERSION_SH) || defined(VERSION_CN)
s8 D_SH_80343CD0_pad[0x20];
s32 D_SH_80343CF0;
s8 D_SH_80343CF8_pad[0x8];
struct UnkStruct80343D00 D_SH_80343D00;
s8 D_SH_8034DC8_pad[0x8];
OSPiHandle DriveRomHandle;
s8 D_SH_80343E48_pad[0x8];
#endif

struct Sound sSoundRequests[0x100];

struct ChannelVolumeScaleFade D_80360928[3][CHANNELS_MAX];
u8 sUsedChannelsForSoundBank[SOUND_BANK_COUNT];
u8 sCurrentSound[SOUND_BANK_COUNT][MAX_CHANNELS_PER_SOUND_BANK];

struct SoundCharacteristics sSoundBanks[SOUND_BANK_COUNT][40];

u8 sSoundMovingSpeed[SOUND_BANK_COUNT];
u8 sBackgroundMusicTargetVolume;
static u8 sLowerBackgroundMusicVolume;
struct SequenceQueueItem sBackgroundMusicQueue[MAX_BACKGROUND_MUSIC_QUEUE_SIZE];

#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
s32 unk_sh_8034754C;
#endif

#ifdef VERSION_EU
OSMesgQueue OSMesgQueue0;
OSMesgQueue OSMesgQueue1;
OSMesgQueue OSMesgQueue2;
OSMesgQueue OSMesgQueue3;
extern OSMesgQueue *OSMesgQueues[];

struct EuAudioCmd sAudioCmd[0x100];

OSMesg OSMesg0;
s32 pad1;
OSMesg OSMesg1;
s32 pad2[2];
OSMesg OSMesg2;
OSMesg OSMesg3;
#else
extern OSMesgQueue *D_SH_80350F88;
extern OSMesgQueue *D_SH_80350FA8;
#endif

#ifdef VERSION_JP
typedef u16 FadeT;
#else
typedef s32 FadeT;
#endif

extern void func_802ad728(u32 bits, f32 arg);
extern void func_802ad74c(u32 bits, u32 arg);
extern void func_802ad770(u32 bits, s8 arg);

static void update_background_music_after_sound(u8 bank, u8 soundIndex);
static void fade_channel_volume_scale(u8 player, u8 channelId, u8 targetScale, u16 fadeTimer);
void process_level_music_dynamics(void);
static u8 begin_background_music_fade(u16 fadeDuration);
void func_80320ED8(void);

#ifndef VERSION_JP
void unused_8031E4F0(void) {

    s32 i;

    stubbed_printf("AUTOSEQ ");
    stubbed_printf("%2x %2x <%5x : %5x / %5x>\n", gSeqLoadedPool.temporary.entries[0].id,
                   gSeqLoadedPool.temporary.entries[1].id, gSeqLoadedPool.temporary.entries[0].size,
                   gSeqLoadedPool.temporary.entries[1].size, gSeqLoadedPool.temporary.pool.size);

    stubbed_printf("AUTOBNK ");
    stubbed_printf("%2x %3x <%5x : %5x / %5x>\n", gBankLoadedPool.temporary.entries[0].id,
                   gBankLoadedPool.temporary.entries[1].id, gBankLoadedPool.temporary.entries[0].size,
                   gBankLoadedPool.temporary.entries[1].size, gBankLoadedPool.temporary.pool.size);

    stubbed_printf("STAYSEQ ");
    stubbed_printf("[%2x] <%5x / %5x>\n", gSeqLoadedPool.persistent.numEntries,
                   gSeqLoadedPool.persistent.pool.cur - gSeqLoadedPool.persistent.pool.start,
                   gSeqLoadedPool.persistent.pool.size);
    for (i = 0; (u32) i < gSeqLoadedPool.persistent.numEntries; i++) {
        stubbed_printf("%2x ", gSeqLoadedPool.persistent.entries[i].id);
    }
    stubbed_printf("\n");

    stubbed_printf("STAYBNK ");
    stubbed_printf("[%2x] <%5x / %5x>\n", gBankLoadedPool.persistent.numEntries,
                   gBankLoadedPool.persistent.pool.cur - gBankLoadedPool.persistent.pool.start,
                   gBankLoadedPool.persistent.pool.size);
    for (i = 0; (u32) i < gBankLoadedPool.persistent.numEntries; i++) {
        stubbed_printf("%2x ", gBankLoadedPool.persistent.entries[i].id);
    }
    stubbed_printf("\n\n");

    stubbed_printf("    0123456789ABCDEF0123456789ABCDEF01234567\n");
    stubbed_printf("--------------------------------------------\n");

    stubbed_printf("SEQ ");
    for (i = 0; i < 40; i++) {
        stubbed_printf("%1x", 0);
    }
    stubbed_printf("\n");

    stubbed_printf("BNK ");
    for (i = 0; i < 40; i += 4) {
        stubbed_printf("%1x", 0);
    }
    stubbed_printf("\n");

    stubbed_printf("FIXHEAP ");
    stubbed_printf("%4x / %4x\n", 0, 0);
    stubbed_printf("DRVHEAP ");
    stubbed_printf("%5x / %5x\n", 0, 0);
    stubbed_printf("DMACACHE  %4d Blocks\n", 0);
    stubbed_printf("CHANNELS  %2d / MAX %3d \n", 0, 0);

    stubbed_printf("TEMPOMAX  %d\n", gTempoInternalToExternal / TEMPO_SCALE);
    stubbed_printf("TEMPO G0  %d\n", gSequencePlayers[SEQ_PLAYER_LEVEL].tempo / TEMPO_SCALE);
    stubbed_printf("TEMPO G1  %d\n", gSequencePlayers[SEQ_PLAYER_ENV].tempo / TEMPO_SCALE);
    stubbed_printf("TEMPO G2  %d\n", gSequencePlayers[SEQ_PLAYER_SFX].tempo / TEMPO_SCALE);
    stubbed_printf("DEBUGFLAG  %8x\n", gAudioErrorFlags);
}

void unused_8031E568(void) {
    stubbed_printf("COUNT %8d\n", gAudioFrameCount);
}
#endif

#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
const char unusedErrorStr1[] = "Error : Queue is not empty ( %x ) \n";
const char unusedErrorStr2[] = "specchg error\n";
#endif

#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
void audio_reset_session_eu(s32 presetId) {
    OSMesg mesg;
#if defined(VERSION_SH) || defined(VERSION_CN)
    osRecvMesg(D_SH_80350FA8, &mesg, OS_MESG_NOBLOCK);
    osSendMesg(D_SH_80350F88, (OSMesg) presetId, OS_MESG_NOBLOCK);
    osRecvMesg(D_SH_80350FA8, &mesg, OS_MESG_BLOCK);
    if ((s32) mesg != presetId) {
        osRecvMesg(D_SH_80350FA8, &mesg, OS_MESG_BLOCK);
    }

#else
    osRecvMesg(OSMesgQueues[3], &mesg, OS_MESG_NOBLOCK);
    osSendMesg(OSMesgQueues[2], (OSMesg) presetId, OS_MESG_NOBLOCK);
    osRecvMesg(OSMesgQueues[3], &mesg, OS_MESG_BLOCK);
    if ((s32) mesg != presetId) {
        osRecvMesg(OSMesgQueues[3], &mesg, OS_MESG_BLOCK);
    }
#endif
}
#endif

#if defined(VERSION_JP) || defined(VERSION_US)

static void seq_player_fade_to_zero_volume(s32 player, FadeT fadeDuration) {
    struct SequencePlayer *seqPlayer = &gSequencePlayers[player];

#ifndef VERSION_JP

    if (fadeDuration == 0) {
        fadeDuration++;
    }
#endif

    seqPlayer->fadeVelocity = -(seqPlayer->fadeVolume / fadeDuration);
    seqPlayer->state = SEQUENCE_PLAYER_STATE_FADE_OUT;
    seqPlayer->fadeRemainingFrames = fadeDuration;
}

static void func_8031D690(s32 player, FadeT fadeInTime) {
    struct SequencePlayer *seqPlayer = &gSequencePlayers[player];

    if (fadeInTime == 0 || seqPlayer->state == SEQUENCE_PLAYER_STATE_FADE_OUT) {
        return;
    }

    seqPlayer->state = SEQUENCE_PLAYER_STATE_2;
    seqPlayer->fadeRemainingFrames = fadeInTime;
    seqPlayer->fadeVolume = 0.0f;
    seqPlayer->fadeVelocity = 0.0f;
}
#endif

static void seq_player_fade_to_percentage_of_volume(s32 player, FadeT fadeDuration, u8 percentage) {
    struct SequencePlayer *seqPlayer = &gSequencePlayers[player];
    f32 targetVolume;

#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
    if (seqPlayer->state == 2) {
        return;
    }
#else
    if (seqPlayer->state == SEQUENCE_PLAYER_STATE_FADE_OUT) {
        return;
    }
#endif

    targetVolume = (FLOAT_CAST(percentage) / EU_FLOAT(100.0)) * seqPlayer->fadeVolume;
    seqPlayer->volume = seqPlayer->fadeVolume;

    seqPlayer->fadeRemainingFrames = 0;
    if (fadeDuration == 0) {
        seqPlayer->fadeVolume = targetVolume;
        return;
    }
    seqPlayer->fadeVelocity = (targetVolume - seqPlayer->fadeVolume) / fadeDuration;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
    seqPlayer->state = 0;
#else
    seqPlayer->state = SEQUENCE_PLAYER_STATE_4;
#endif

    seqPlayer->fadeRemainingFrames = fadeDuration;
}

static void seq_player_fade_to_normal_volume(s32 player, FadeT fadeDuration) {
    struct SequencePlayer *seqPlayer = &gSequencePlayers[player];

#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
    if (seqPlayer->state == 2) {
        return;
    }
#else
    if (seqPlayer->state == SEQUENCE_PLAYER_STATE_FADE_OUT) {
        return;
    }
#endif

    seqPlayer->fadeRemainingFrames = 0;
    if (fadeDuration == 0) {
        seqPlayer->fadeVolume = seqPlayer->volume;
        return;
    }
    seqPlayer->fadeVelocity = (seqPlayer->volume - seqPlayer->fadeVolume) / fadeDuration;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
    seqPlayer->state = 0;
#else
    seqPlayer->state = SEQUENCE_PLAYER_STATE_2;
#endif

    seqPlayer->fadeRemainingFrames = fadeDuration;
}

static void seq_player_fade_to_target_volume(s32 player, FadeT fadeDuration, u8 targetVolume) {
    struct SequencePlayer *seqPlayer = &gSequencePlayers[player];

#if defined(VERSION_JP) || defined(VERSION_US)
    if (seqPlayer->state == SEQUENCE_PLAYER_STATE_FADE_OUT) {
        return;
    }
#endif

    seqPlayer->fadeRemainingFrames = 0;
    if (fadeDuration == 0) {
        seqPlayer->fadeVolume = (FLOAT_CAST(targetVolume) / EU_FLOAT(127.0));
        return;
    }

    seqPlayer->fadeVelocity =
        (((f32)(FLOAT_CAST(targetVolume) / EU_FLOAT(127.0)) - seqPlayer->fadeVolume)
         / (f32) fadeDuration);
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
    seqPlayer->state = 0;
#else
    seqPlayer->state = SEQUENCE_PLAYER_STATE_4;
#endif

    seqPlayer->fadeRemainingFrames = fadeDuration;
}

#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
#ifdef VERSION_EU
extern void func_802ad7a0(void);
#else
extern void func_sh_802F64C8(void);
#endif

void maybe_tick_game_sound(void) {
    if (sGameLoopTicked != 0) {
        update_game_sound();
        sGameLoopTicked = 0;
    }
#ifdef VERSION_EU
    func_802ad7a0();
#else
    func_sh_802F64C8();
#endif
}

void func_eu_802e9bec(s32 player, s32 channel, s32 arg2) {

    func_802ad770(0x08000000 | (player & 0xff) << 16 | (channel & 0xff) << 8, (s8) arg2);
}

#else

struct SPTask *create_next_audio_frame_task(void) {
#ifdef TARGET_N64
    u32 samplesRemainingInAI;
    s32 writtenCmds;
    s32 index;
    OSTask_t *task;
    s32 oldDmaCount;
    s32 flags;

    gAudioFrameCount++;
    if (gAudioLoadLock != AUDIO_LOCK_NOT_LOADING) {
        stubbed_printf("DAC:Lost 1 Frame.\n");
        return NULL;
    }

    gAudioTaskIndex ^= 1;
    gCurrAiBufferIndex++;
    gCurrAiBufferIndex %= NUMAIBUFFERS;
    index = (gCurrAiBufferIndex - 2 + NUMAIBUFFERS) % NUMAIBUFFERS;
    samplesRemainingInAI = osAiGetLength() / 4;

    if (gAiBufferLengths[index] != 0) {
        osAiSetNextBuffer(gAiBuffers[index], gAiBufferLengths[index] * 4);
    }

    oldDmaCount = gCurrAudioFrameDmaCount;

    if (oldDmaCount > AUDIO_FRAME_DMA_QUEUE_SIZE) {
        stubbed_printf("DMA: Request queue over.( %d )\n", oldDmaCount);
    }
    gCurrAudioFrameDmaCount = 0;

    gAudioTask = &gAudioTasks[gAudioTaskIndex];
    gAudioCmd = gAudioCmdBuffers[gAudioTaskIndex];

    index = gCurrAiBufferIndex;
    gCurrAiBuffer = gAiBuffers[index];
    gAiBufferLengths[index] =
        ((gSamplesPerFrameTarget - samplesRemainingInAI + EXTRA_BUFFERED_AI_SAMPLES_TARGET) & ~0xf)
        + SAMPLES_TO_OVERPRODUCE;
    if (gAiBufferLengths[index] < gMinAiBufferLength) {
        gAiBufferLengths[index] = gMinAiBufferLength;
    }
    if (gAiBufferLengths[index] > gSamplesPerFrameTarget + SAMPLES_TO_OVERPRODUCE) {
        gAiBufferLengths[index] = gSamplesPerFrameTarget + SAMPLES_TO_OVERPRODUCE;
    }

    if (sGameLoopTicked != 0) {
        update_game_sound();
        sGameLoopTicked = 0;
    }

    flags = 0;
    gAudioCmd = synthesis_execute(gAudioCmd, &writtenCmds, gCurrAiBuffer, gAiBufferLengths[index]);
    gAudioRandom = ((gAudioRandom + gAudioFrameCount) * gAudioFrameCount);

    index = gAudioTaskIndex;
    gAudioTask->msgqueue = NULL;
    gAudioTask->msg = NULL;

    task = &gAudioTask->task.t;
    task->type = M_AUDTASK;
    task->flags = flags;
    task->ucode_boot = rspF3DBootStart;
    task->ucode_boot_size = (u8 *) rspF3DBootEnd - (u8 *) rspF3DBootStart;
    task->ucode = rspAspMainStart;
    task->ucode_size = 0x800;
    task->ucode_data = rspAspMainDataStart;
    task->ucode_data_size = (rspAspMainDataEnd - rspAspMainDataStart) * sizeof(u64);
    task->dram_stack = NULL;
    task->dram_stack_size = 0;
    task->output_buff = NULL;
    task->output_buff_size = NULL;
    task->data_ptr = gAudioCmdBuffers[index];
    task->data_size = writtenCmds * sizeof(u64);

#ifdef VERSION_JP
    task->yield_data_ptr = (u64 *) gAudioSPTaskYieldBuffer;
    task->yield_data_size = OS_YIELD_AUDIO_SIZE;
#else
    task->yield_data_ptr = NULL;
    task->yield_data_size = 0;
#endif

    decrease_sample_dma_ttls();
    return gAudioTask;
#else
    return NULL;
#endif
}
#endif

void play_sound(s32 soundBits, f32 *pos) {
    NDS_AUDIO_GUARD();
    sSoundRequests[sSoundRequestCount].soundBits = soundBits;
    sSoundRequests[sSoundRequestCount].position = pos;
    sSoundRequestCount++;
}

NDS_ITCM_CODE static void process_sound_request(u32 bits, f32 *pos) {
    u8 bank;
    u8 soundIndex;
    u8 counter = 0;
    u8 soundId;
    f32 dist;
    const f32 one = 1.0f;

    bank = (bits & SOUNDARGS_MASK_BANK) >> SOUNDARGS_SHIFT_BANK;
    soundId = (bits & SOUNDARGS_MASK_SOUNDID) >> SOUNDARGS_SHIFT_SOUNDID;

    if (soundId >= sNumSoundsPerBank[bank] || sSoundBankDisabled[bank]) {
        return;
    }

    soundIndex = sSoundBanks[bank][0].next;
    while (soundIndex != 0xff && soundIndex != 0) {

        if (sSoundBanks[bank][soundIndex].x == pos) {

            if ((sSoundBanks[bank][soundIndex].soundBits & SOUNDARGS_MASK_PRIORITY)
                <= (bits & SOUNDARGS_MASK_PRIORITY)) {

                if ((sSoundBanks[bank][soundIndex].soundBits & SOUND_DISCRETE) != 0
                    || (bits & SOUNDARGS_MASK_SOUNDID)
                           != (sSoundBanks[bank][soundIndex].soundBits & SOUNDARGS_MASK_SOUNDID)) {
                    update_background_music_after_sound(bank, soundIndex);
                    sSoundBanks[bank][soundIndex].soundBits = bits;

                    sSoundBanks[bank][soundIndex].soundStatus = bits & SOUNDARGS_MASK_STATUS;
                }

                sSoundBanks[bank][soundIndex].freshness = SOUND_MAX_FRESHNESS;
            }

            soundIndex = 0;
        } else {
            soundIndex = sSoundBanks[bank][soundIndex].next;
        }
        counter++;
    }

    if (counter == 0) {
        sSoundMovingSpeed[bank] = 32;
    }

    if (sSoundBanks[bank][sSoundBankFreeListFront[bank]].next != 0xff && soundIndex != 0) {

        soundIndex = sSoundBankFreeListFront[bank];

        dist = sqrtf(pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2]) * one;
        sSoundBanks[bank][soundIndex].x = &pos[0];
        sSoundBanks[bank][soundIndex].y = &pos[1];
        sSoundBanks[bank][soundIndex].z = &pos[2];
        sSoundBanks[bank][soundIndex].distance = dist;
        sSoundBanks[bank][soundIndex].soundBits = bits;

        sSoundBanks[bank][soundIndex].soundStatus = bits & SOUNDARGS_MASK_STATUS;
        sSoundBanks[bank][soundIndex].freshness = SOUND_MAX_FRESHNESS;

        sSoundBanks[bank][soundIndex].prev = sSoundBankUsedListBack[bank];
        sSoundBanks[bank][sSoundBankUsedListBack[bank]].next = sSoundBankFreeListFront[bank];
        sSoundBankUsedListBack[bank] = sSoundBankFreeListFront[bank];
        sSoundBankFreeListFront[bank] = sSoundBanks[bank][sSoundBankFreeListFront[bank]].next;
        sSoundBanks[bank][sSoundBankFreeListFront[bank]].prev = 0xff;
        sSoundBanks[bank][soundIndex].next = 0xff;
    }
}

NDS_ITCM_CODE static void process_all_sound_requests(void) {
    struct Sound *sound;

    while (sSoundRequestCount != sNumProcessedSoundRequests) {
        sound = &sSoundRequests[sNumProcessedSoundRequests];
        process_sound_request(sound->soundBits, sound->position);
        sNumProcessedSoundRequests++;
    }
}

static void delete_sound_from_bank(u8 bank, u8 soundIndex) {
    if (sSoundBankUsedListBack[bank] == soundIndex) {

        sSoundBankUsedListBack[bank] = sSoundBanks[bank][soundIndex].prev;
    } else {

        sSoundBanks[bank][sSoundBanks[bank][soundIndex].next].prev = sSoundBanks[bank][soundIndex].prev;
    }

    sSoundBanks[bank][sSoundBanks[bank][soundIndex].prev].next = sSoundBanks[bank][soundIndex].next;

    sSoundBanks[bank][soundIndex].next = sSoundBankFreeListFront[bank];
    sSoundBanks[bank][soundIndex].prev = 0xff;
    sSoundBanks[bank][sSoundBankFreeListFront[bank]].prev = soundIndex;
    sSoundBankFreeListFront[bank] = soundIndex;
}

NDS_ITCM_CODE static void update_background_music_after_sound(u8 bank, u8 soundIndex) {
    if (sSoundBanks[bank][soundIndex].soundBits & SOUND_LOWER_BACKGROUND_MUSIC) {
        sSoundBanksThatLowerBackgroundMusic &= (1 << bank) ^ 0xffff;
        begin_background_music_fade(50);
    }
}

NDS_ITCM_CODE static void select_current_sounds(u8 bank) {
    u32 isDiscreteAndStatus;
    u8 latestSoundIndex;
    u8 i;
    u8 j;
    u8 soundIndex;
    u32 liveSoundPriorities[16] = { 0x10000000, 0x10000000, 0x10000000, 0x10000000,
                                    0x10000000, 0x10000000, 0x10000000, 0x10000000,
                                    0x10000000, 0x10000000, 0x10000000, 0x10000000,
                                    0x10000000, 0x10000000, 0x10000000, 0x10000000 };
    u8 liveSoundIndices[16] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    u8 liveSoundStatuses[16] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    u8 numSoundsInBank = 0;
    u8 requestedPriority;

    soundIndex = sSoundBanks[bank][0].next;
    while (soundIndex != 0xff) {
        latestSoundIndex = soundIndex;

        if ((sSoundBanks[bank][soundIndex].soundBits & (SOUND_DISCRETE | SOUNDARGS_MASK_STATUS))
            == (SOUND_DISCRETE | SOUND_STATUS_WAITING)) {
            if (sSoundBanks[bank][soundIndex].freshness-- == 0) {
                sSoundBanks[bank][soundIndex].soundBits = NO_SOUND;
            }
        }

        else if ((sSoundBanks[bank][soundIndex].soundBits & SOUND_DISCRETE) == 0) {
            if (sSoundBanks[bank][soundIndex].freshness-- == SOUND_MAX_FRESHNESS - 2) {
                update_background_music_after_sound(bank, soundIndex);
                sSoundBanks[bank][soundIndex].soundBits = NO_SOUND;
            }
        }

        if (sSoundBanks[bank][soundIndex].soundBits == NO_SOUND
            && sSoundBanks[bank][soundIndex].soundStatus == SOUND_STATUS_WAITING) {

            latestSoundIndex = sSoundBanks[bank][soundIndex].prev;
            sSoundBanks[bank][soundIndex].soundStatus = SOUND_STATUS_STOPPED;
            delete_sound_from_bank(bank, soundIndex);
        }

        if (sSoundBanks[bank][soundIndex].soundStatus != SOUND_STATUS_STOPPED
            && soundIndex == latestSoundIndex) {

            sSoundBanks[bank][soundIndex].distance =
                sqrtf((*sSoundBanks[bank][soundIndex].x * *sSoundBanks[bank][soundIndex].x)
                      + (*sSoundBanks[bank][soundIndex].y * *sSoundBanks[bank][soundIndex].y)
                      + (*sSoundBanks[bank][soundIndex].z * *sSoundBanks[bank][soundIndex].z))
                * 1;

            requestedPriority = (sSoundBanks[bank][soundIndex].soundBits & SOUNDARGS_MASK_PRIORITY)
                                >> SOUNDARGS_SHIFT_PRIORITY;

            if (sSoundBanks[bank][soundIndex].soundBits & SOUND_NO_PRIORITY_LOSS) {
                sSoundBanks[bank][soundIndex].priority = 0x4c * (0xff - requestedPriority);
            } else if (*sSoundBanks[bank][soundIndex].z > 0.0f) {
                sSoundBanks[bank][soundIndex].priority =
                    (u32) sSoundBanks[bank][soundIndex].distance
                    + (u32)(*sSoundBanks[bank][soundIndex].z / US_FLOAT(6.0))
                    + 0x4c * (0xff - requestedPriority);
            } else {
                sSoundBanks[bank][soundIndex].priority =
                    (u32) sSoundBanks[bank][soundIndex].distance + 0x4c * (0xff - requestedPriority);
            }

            for (i = 0; i < sMaxChannelsForSoundBank[bank]; i++) {

                if (liveSoundPriorities[i] >= sSoundBanks[bank][soundIndex].priority) {

                    for (j = sMaxChannelsForSoundBank[bank] - 1; j > i; j--) {
                        liveSoundPriorities[j] = liveSoundPriorities[j - 1];
                        liveSoundIndices[j] = liveSoundIndices[j - 1];
                        liveSoundStatuses[j] = liveSoundStatuses[j - 1];
                    }

                    liveSoundPriorities[i] = sSoundBanks[bank][soundIndex].priority;
                    liveSoundIndices[i] = soundIndex;
                    liveSoundStatuses[i] = sSoundBanks[bank][soundIndex].soundStatus;

                    i = sMaxChannelsForSoundBank[bank];
                }
            }

            numSoundsInBank++;
        }

        soundIndex = sSoundBanks[bank][latestSoundIndex].next;
    }

    sNumSoundsInBank[bank] = numSoundsInBank;
    sUsedChannelsForSoundBank[bank] = sMaxChannelsForSoundBank[bank];

    for (i = 0; i < sUsedChannelsForSoundBank[bank]; i++) {

        for (soundIndex = 0; soundIndex < sUsedChannelsForSoundBank[bank]; soundIndex++) {
            if (liveSoundIndices[soundIndex] != 0xff
                && sCurrentSound[bank][i] == liveSoundIndices[soundIndex]) {

                liveSoundIndices[soundIndex] = 0xff;
                soundIndex = 0xfe;
            }
        }

        if (soundIndex != 0xff) {
            if (sCurrentSound[bank][i] != 0xff) {

                if (sSoundBanks[bank][sCurrentSound[bank][i]].soundBits == NO_SOUND) {
                    if (sSoundBanks[bank][sCurrentSound[bank][i]].soundStatus == SOUND_STATUS_PLAYING) {
                        sSoundBanks[bank][sCurrentSound[bank][i]].soundStatus = SOUND_STATUS_STOPPED;
                        delete_sound_from_bank(bank, sCurrentSound[bank][i]);
                    }
                }

                isDiscreteAndStatus = sSoundBanks[bank][sCurrentSound[bank][i]].soundBits
                                      & (SOUND_DISCRETE | SOUNDARGS_MASK_STATUS);
                if (isDiscreteAndStatus >= (SOUND_DISCRETE | SOUND_STATUS_PLAYING)
                    && sSoundBanks[bank][sCurrentSound[bank][i]].soundStatus != SOUND_STATUS_STOPPED) {

#ifndef VERSION_JP
                    update_background_music_after_sound(bank, sCurrentSound[bank][i]);
#endif

                    sSoundBanks[bank][sCurrentSound[bank][i]].soundBits = NO_SOUND;
                    sSoundBanks[bank][sCurrentSound[bank][i]].soundStatus = SOUND_STATUS_STOPPED;
                    delete_sound_from_bank(bank, sCurrentSound[bank][i]);
                }

                else {
                    if (isDiscreteAndStatus == SOUND_STATUS_PLAYING
                        && sSoundBanks[bank][sCurrentSound[bank][i]].soundStatus
                               != SOUND_STATUS_STOPPED) {
                        sSoundBanks[bank][sCurrentSound[bank][i]].soundStatus = SOUND_STATUS_WAITING;
                    }
                }
            }
            sCurrentSound[bank][i] = 0xff;
        }
    }

    for (soundIndex = 0; soundIndex < sUsedChannelsForSoundBank[bank]; soundIndex++) {
        if (liveSoundIndices[soundIndex] != 0xff) {
            for (i = 0; i < sUsedChannelsForSoundBank[bank]; i++) {
                if (sCurrentSound[bank][i] == 0xff) {
                    sCurrentSound[bank][i] = liveSoundIndices[soundIndex];

                    sSoundBanks[bank][liveSoundIndices[soundIndex]].soundBits =
                        (sSoundBanks[bank][liveSoundIndices[soundIndex]].soundBits
                         & ~SOUNDARGS_MASK_STATUS)
                        + SOUND_STATUS_WAITING;

                    liveSoundIndices[i] = 0xff;
                    i = 0xfe;
                }
            }
        }
    }
}

NDS_ITCM_CODE static f32 get_sound_pan(f32 x, f32 z) {
    f32 absX;
    f32 absZ;
    f32 pan;

    absX = (x < 0 ? -x : x);
    if (absX > AUDIO_MAX_DISTANCE) {
        absX = AUDIO_MAX_DISTANCE;
    }

    absZ = (z < 0 ? -z : z);
    if (absZ > AUDIO_MAX_DISTANCE) {
        absZ = AUDIO_MAX_DISTANCE;
    }

    if (x == US_FLOAT(0.0) && z == US_FLOAT(0.0)) {

        pan = US_FLOAT(0.5);
    } else if (x >= US_FLOAT(0.0) && absX >= absZ) {

        pan = US_FLOAT(1.0) - (2 * AUDIO_MAX_DISTANCE - absX) / (US_FLOAT(3.0) * (2 * AUDIO_MAX_DISTANCE - absZ));
    } else if (x < 0 && absX > absZ) {

        pan = (2 * AUDIO_MAX_DISTANCE - absX) / (US_FLOAT(3.0) * (2 * AUDIO_MAX_DISTANCE - absZ));
    } else {

        pan = 0.5 + x / (US_FLOAT(6.0) * absZ);
    }

    return pan;
}

NDS_ITCM_CODE static f32 get_sound_volume(u8 bank, u8 soundIndex, f32 volumeRange) {
    f32 maxSoundDistance;
    f32 intensity;
#ifndef VERSION_JP
    s32 div = bank < 3 ? 2 : 3;
#endif

    if (!(sSoundBanks[bank][soundIndex].soundBits & SOUND_NO_VOLUME_LOSS)) {
#ifdef VERSION_JP

        maxSoundDistance = sLevelAcousticReaches[gCurrLevelNum];
        if (maxSoundDistance < sSoundBanks[bank][soundIndex].distance) {
            intensity = 0.0f;
        } else {
            intensity = 1.0 - sSoundBanks[bank][soundIndex].distance / maxSoundDistance;
        }
#else

        if (sSoundBanks[bank][soundIndex].distance > AUDIO_MAX_DISTANCE) {
            intensity = 0.0f;
        } else {
            maxSoundDistance = sLevelAcousticReaches[gCurrLevelNum] / div;
            if (maxSoundDistance < sSoundBanks[bank][soundIndex].distance) {
                intensity = ((AUDIO_MAX_DISTANCE - sSoundBanks[bank][soundIndex].distance)
                             / (AUDIO_MAX_DISTANCE - maxSoundDistance))
                            * (1.0f - volumeRange);
            } else {
                intensity =
                    1.0f - sSoundBanks[bank][soundIndex].distance / maxSoundDistance * volumeRange;
            }
        }
#endif

        if (sSoundBanks[bank][soundIndex].soundBits & SOUND_VIBRATO) {
#ifdef VERSION_JP

            if (intensity != 0.0)
#else
            if (intensity >= 0.08f)
#endif
            {
                intensity -= (f32)(gAudioRandom & 0xf) / US_FLOAT(192.0);
            }
        }
    } else {
        intensity = 1.0f;
    }

    return volumeRange * intensity * intensity + 1.0f - volumeRange;
}

NDS_ITCM_CODE static f32 get_sound_freq_scale(u8 bank, u8 item) {
    f32 amount;

    if (!(sSoundBanks[bank][item].soundBits & SOUND_CONSTANT_FREQUENCY)) {
        amount = sSoundBanks[bank][item].distance / AUDIO_MAX_DISTANCE;
        if (sSoundBanks[bank][item].soundBits & SOUND_VIBRATO) {
            amount += (f32)(gAudioRandom & 0xff) / US_FLOAT(64.0);
        }
    } else {
        amount = 0.0f;
    }

    return amount / US_FLOAT(15.0) + US_FLOAT(1.0);
}

NDS_ITCM_CODE static u8 get_sound_reverb(UNUSED u8 bank, UNUSED u8 soundIndex, u8 channelIndex) {
    u8 area;
    u8 level;
    u8 reverb;

#ifndef VERSION_JP

    if (sSoundBanks[bank][soundIndex].soundBits & SOUND_NO_ECHO) {
        level = 0;
        area = 0;
    } else {
#endif
        level = (gCurrLevelNum > LEVEL_MAX ? LEVEL_MAX : gCurrLevelNum);
        area = gCurrAreaIndex - 1;
        if (area > 2) {
            area = 2;
        }
#ifndef VERSION_JP
    }
#endif

    reverb = (u8)((u8) gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->soundScriptIO[5]
                  + sLevelAreaReverbs[level][area]
                  + (US_FLOAT(1.0) - gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->volume)
                        * LOW_VOLUME_REVERB);

    if (reverb > 0x7f) {
        reverb = 0x7f;
    }
    return reverb;
}

static void noop_8031EEC8(void) {
}

void audio_signal_game_loop_tick(void) {
    NDS_AUDIO_GUARD();
    sGameLoopTicked = 1;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
    maybe_tick_game_sound();
#endif
    noop_8031EEC8();
}

NDS_ITCM_CODE void update_game_sound(void) {
    NDS_AUDIO_GUARD();
    u8 soundStatus;
    u8 i;
    u8 soundId;
    u8 bank;
    u8 channelIndex = 0;
    u8 soundIndex;
#if defined(VERSION_JP) || defined(VERSION_US)
    f32 value;
#endif

    process_all_sound_requests();
    process_level_music_dynamics();

    if (gSequencePlayers[SEQ_PLAYER_SFX].channels[0] == &gSequenceChannelNone) {
        return;
    }

    for (bank = 0; bank < SOUND_BANK_COUNT; bank++) {
        select_current_sounds(bank);

        for (i = 0; i < MAX_CHANNELS_PER_SOUND_BANK; i++) {
            soundIndex = sCurrentSound[bank][i];

            if (soundIndex < 0xff
                && sSoundBanks[bank][soundIndex].soundStatus != SOUND_STATUS_STOPPED) {
                soundStatus = sSoundBanks[bank][soundIndex].soundBits & SOUNDARGS_MASK_STATUS;
                soundId = (sSoundBanks[bank][soundIndex].soundBits >> SOUNDARGS_SHIFT_SOUNDID);

                sSoundBanks[bank][soundIndex].soundStatus = soundStatus;

                if (soundStatus == SOUND_STATUS_WAITING) {
                    if (sSoundBanks[bank][soundIndex].soundBits & SOUND_LOWER_BACKGROUND_MUSIC) {
                        sSoundBanksThatLowerBackgroundMusic |= 1 << bank;
                        begin_background_music_fade(50);
                    }

                    sSoundBanks[bank][soundIndex].soundBits++;
                    sSoundBanks[bank][soundIndex].soundStatus = SOUND_STATUS_PLAYING;

                    gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->soundScriptIO[4] = soundId;
                    gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->soundScriptIO[0] = 1;

                    switch (bank) {
                        case SOUND_BANK_MOVING:
                            if (!(sSoundBanks[bank][soundIndex].soundBits & SOUND_CONSTANT_FREQUENCY)) {
                                if (sSoundMovingSpeed[bank] > 8) {
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
                                    func_802ad728(
                                        0x02020000 | ((channelIndex & 0xff) << 8),
                                        get_sound_volume(bank, soundIndex, VOLUME_RANGE_UNK1));
#else
                                    value = get_sound_volume(bank, soundIndex, VOLUME_RANGE_UNK1);
                                    gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->volume =
                                        value;
#endif
                                } else {
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
                                    func_802ad728(0x02020000 | ((channelIndex & 0xff) << 8),
                                                  get_sound_volume(bank, soundIndex, VOLUME_RANGE_UNK1)
                                                      * ((sSoundMovingSpeed[bank] + 8.0f) / 16));
#else
                                    value = get_sound_volume(bank, soundIndex, VOLUME_RANGE_UNK1);
                                    gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->volume =
                                        (sSoundMovingSpeed[bank] + 8.0f) / 16 * value;
#endif
                                }
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
                                func_802ad770(0x03020000 | ((channelIndex & 0xff) << 8),
                                              get_sound_pan(*sSoundBanks[bank][soundIndex].x,
                                                            *sSoundBanks[bank][soundIndex].z));
#else
                                gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->pan =
                                    get_sound_pan(*sSoundBanks[bank][soundIndex].x,
                                                  *sSoundBanks[bank][soundIndex].z);
#endif

                                if ((sSoundBanks[bank][soundIndex].soundBits & SOUNDARGS_MASK_SOUNDID)
                                    == (SOUND_MOVING_FLYING & SOUNDARGS_MASK_SOUNDID)) {
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
                                    func_802ad728(
                                        0x04020000 | ((channelIndex & 0xff) << 8),
                                        get_sound_freq_scale(bank, soundIndex)
                                            + ((f32) sSoundMovingSpeed[bank] / US_FLOAT(80.0)));
#else
                                    value = get_sound_freq_scale(bank, soundIndex);
                                    gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->freqScale =
                                        ((f32) sSoundMovingSpeed[bank] / US_FLOAT(80.0)) + value;
#endif
                                } else {
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
                                    func_802ad728(
                                        0x04020000 | ((channelIndex & 0xff) << 8),
                                        get_sound_freq_scale(bank, soundIndex)
                                            + ((f32) sSoundMovingSpeed[bank] / US_FLOAT(400.0)));
#else
                                    value = get_sound_freq_scale(bank, soundIndex);
                                    gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->freqScale =
                                        ((f32) sSoundMovingSpeed[bank] / US_FLOAT(400.0)) + value;
#endif
                                }
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
                                func_802ad770(0x05020000 | ((channelIndex & 0xff) << 8),
                                              get_sound_reverb(bank, soundIndex, channelIndex));
#else
                                gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->reverbVol =
                                    get_sound_reverb(bank, soundIndex, channelIndex);
#endif

                                break;
                            }

                        case SOUND_BANK_MENU:
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
                            func_802ad728(0x02020000 | ((channelIndex & 0xff) << 8), 1);
                            func_802ad770(0x03020000 | ((channelIndex & 0xff) << 8), 64);
                            func_802ad728(0x04020000 | ((channelIndex & 0xff) << 8),
                                          get_sound_freq_scale(bank, soundIndex));
#else
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->volume = 1.0f;
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->pan = 0.5f;
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->freqScale = 1.0f;
#endif
                            break;
                        case SOUND_BANK_ACTION:
                        case SOUND_BANK_VOICE:
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
                            func_802ad770(0x05020000 | ((channelIndex & 0xff) << 8),
                                          get_sound_reverb(bank, soundIndex, channelIndex));
                            func_802ad728(0x02020000 | ((channelIndex & 0xff) << 8),
                                          get_sound_volume(bank, soundIndex, VOLUME_RANGE_UNK1));
                            func_802ad770(0x03020000 | ((channelIndex & 0xff) << 8),
                                          get_sound_pan(*sSoundBanks[bank][soundIndex].x,
                                                        *sSoundBanks[bank][soundIndex].z)
                                                  * 127.0f
                                              + 0.5f);
                            func_802ad728(0x04020000 | ((channelIndex & 0xff) << 8),
                                          get_sound_freq_scale(bank, soundIndex));
#else
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->volume =
                                get_sound_volume(bank, soundIndex, VOLUME_RANGE_UNK1);
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->pan =
                                get_sound_pan(*sSoundBanks[bank][soundIndex].x,
                                              *sSoundBanks[bank][soundIndex].z);
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->freqScale =
                                get_sound_freq_scale(bank, soundIndex);
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->reverbVol =
                                get_sound_reverb(bank, soundIndex, channelIndex);
#endif
                            break;
                        case SOUND_BANK_GENERAL:
                        case SOUND_BANK_ENV:
                        case SOUND_BANK_OBJ:
                        case SOUND_BANK_AIR:
                        case SOUND_BANK_GENERAL2:
                        case SOUND_BANK_OBJ2:
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
                            func_802ad770(0x05020000 | ((channelIndex & 0xff) << 8),
                                          get_sound_reverb(bank, soundIndex, channelIndex));
                            func_802ad728(0x02020000 | ((channelIndex & 0xff) << 8),
                                          get_sound_volume(bank, soundIndex, VOLUME_RANGE_UNK2));
                            func_802ad770(0x03020000 | ((channelIndex & 0xff) << 8),
                                          get_sound_pan(*sSoundBanks[bank][soundIndex].x,
                                                        *sSoundBanks[bank][soundIndex].z)
                                                  * 127.0f
                                              + 0.5f);
                            func_802ad728(0x04020000 | ((channelIndex & 0xff) << 8),
                                          get_sound_freq_scale(bank, soundIndex));
#else
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->reverbVol =
                                get_sound_reverb(bank, soundIndex, channelIndex);
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->volume =
                                get_sound_volume(bank, soundIndex, VOLUME_RANGE_UNK2);
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->pan =
                                get_sound_pan(*sSoundBanks[bank][soundIndex].x,
                                              *sSoundBanks[bank][soundIndex].z);
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->freqScale =
                                get_sound_freq_scale(bank, soundIndex);
#endif
                            break;
                    }
                }
#ifdef VERSION_JP

                else if (soundStatus == SOUND_STATUS_STOPPED) {
                    update_background_music_after_sound(bank, soundIndex);
                    gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->soundScriptIO[0] = 0;
                    delete_sound_from_bank(bank, soundIndex);
                }
#else
                else if (gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->layers[0] == NULL) {
                    update_background_music_after_sound(bank, soundIndex);
                    sSoundBanks[bank][soundIndex].soundStatus = SOUND_STATUS_STOPPED;
                    delete_sound_from_bank(bank, soundIndex);
                } else if (soundStatus == SOUND_STATUS_STOPPED
                           && gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]
                                      ->layers[0]->finished == FALSE) {
                    update_background_music_after_sound(bank, soundIndex);
                    gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->soundScriptIO[0] = 0;
                    delete_sound_from_bank(bank, soundIndex);
                }
#endif

                else if (gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->layers[0]->enabled
                         == FALSE) {
                    update_background_music_after_sound(bank, soundIndex);
                    sSoundBanks[bank][soundIndex].soundStatus = SOUND_STATUS_STOPPED;
                    delete_sound_from_bank(bank, soundIndex);
                } else {

                    switch (bank) {
                        case SOUND_BANK_MOVING:
                            if (!(sSoundBanks[bank][soundIndex].soundBits & SOUND_CONSTANT_FREQUENCY)) {
                                if (sSoundMovingSpeed[bank] > 8) {
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
                                    func_802ad728(
                                        0x02020000 | ((channelIndex & 0xff) << 8),
                                        get_sound_volume(bank, soundIndex, VOLUME_RANGE_UNK1));
#else
                                    value = get_sound_volume(bank, soundIndex, VOLUME_RANGE_UNK1);
                                    gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->volume =
                                        value;
#endif
                                } else {
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
                                    func_802ad728(0x02020000 | ((channelIndex & 0xff) << 8),
                                                  get_sound_volume(bank, soundIndex, VOLUME_RANGE_UNK1)
                                                      * ((sSoundMovingSpeed[bank] + 8.0f) / 16));
#else
                                    value = get_sound_volume(bank, soundIndex, VOLUME_RANGE_UNK1);
                                    gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->volume =
                                        (sSoundMovingSpeed[bank] + 8.0f) / 16 * value;
#endif
                                }
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
                                func_802ad770(0x03020000 | ((channelIndex & 0xff) << 8),
                                              get_sound_pan(*sSoundBanks[bank][soundIndex].x,
                                                            *sSoundBanks[bank][soundIndex].z));
#else
                                gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->pan =
                                    get_sound_pan(*sSoundBanks[bank][soundIndex].x,
                                                  *sSoundBanks[bank][soundIndex].z);
#endif

                                if ((sSoundBanks[bank][soundIndex].soundBits & SOUNDARGS_MASK_SOUNDID)
                                    == (SOUND_MOVING_FLYING & SOUNDARGS_MASK_SOUNDID)) {
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
                                    func_802ad728(
                                        0x04020000 | ((channelIndex & 0xff) << 8),
                                        get_sound_freq_scale(bank, soundIndex)
                                            + ((f32) sSoundMovingSpeed[bank] / US_FLOAT(80.0)));
#else
                                    value = get_sound_freq_scale(bank, soundIndex);
                                    gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->freqScale =
                                        ((f32) sSoundMovingSpeed[bank] / US_FLOAT(80.0)) + value;
#endif
                                } else {
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
                                    func_802ad728(
                                        0x04020000 | ((channelIndex & 0xff) << 8),
                                        get_sound_freq_scale(bank, soundIndex)
                                            + ((f32) sSoundMovingSpeed[bank] / US_FLOAT(400.0)));
#else
                                    value = get_sound_freq_scale(bank, soundIndex);
                                    gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->freqScale =
                                        ((f32) sSoundMovingSpeed[bank] / US_FLOAT(400.0)) + value;
#endif
                                }
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
                                func_802ad770(0x05020000 | ((channelIndex & 0xff) << 8),
                                              get_sound_reverb(bank, soundIndex, channelIndex));
#else
                                gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->reverbVol =
                                    get_sound_reverb(bank, soundIndex, channelIndex);
#endif

                                break;
                            }

                        case SOUND_BANK_MENU:
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
                            func_802ad728(0x02020000 | ((channelIndex & 0xff) << 8), 1);
                            func_802ad770(0x03020000 | ((channelIndex & 0xff) << 8), 64);
                            func_802ad728(0x04020000 | ((channelIndex & 0xff) << 8),
                                          get_sound_freq_scale(bank, soundIndex));
#else
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->volume = 1.0f;
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->pan = 0.5f;
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->freqScale = 1.0f;
#endif
                            break;
                        case SOUND_BANK_ACTION:
                        case SOUND_BANK_VOICE:
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
                            func_802ad770(0x05020000 | ((channelIndex & 0xff) << 8),
                                          get_sound_reverb(bank, soundIndex, channelIndex));
                            func_802ad728(0x02020000 | ((channelIndex & 0xff) << 8),
                                          get_sound_volume(bank, soundIndex, VOLUME_RANGE_UNK1));
                            func_802ad770(0x03020000 | ((channelIndex & 0xff) << 8),
                                          get_sound_pan(*sSoundBanks[bank][soundIndex].x,
                                                        *sSoundBanks[bank][soundIndex].z)
                                                  * 127.0f
                                              + 0.5f);
                            func_802ad728(0x04020000 | ((channelIndex & 0xff) << 8),
                                          get_sound_freq_scale(bank, soundIndex));
#else
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->volume =
                                get_sound_volume(bank, soundIndex, VOLUME_RANGE_UNK1);
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->pan =
                                get_sound_pan(*sSoundBanks[bank][soundIndex].x,
                                              *sSoundBanks[bank][soundIndex].z);
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->freqScale =
                                get_sound_freq_scale(bank, soundIndex);
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->reverbVol =
                                get_sound_reverb(bank, soundIndex, channelIndex);
#endif
                            break;
                        case SOUND_BANK_GENERAL:
                        case SOUND_BANK_ENV:
                        case SOUND_BANK_OBJ:
                        case SOUND_BANK_AIR:
                        case SOUND_BANK_GENERAL2:
                        case SOUND_BANK_OBJ2:
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
                            func_802ad770(0x05020000 | ((channelIndex & 0xff) << 8),
                                          get_sound_reverb(bank, soundIndex, channelIndex));
                            func_802ad728(0x02020000 | ((channelIndex & 0xff) << 8),
                                          get_sound_volume(bank, soundIndex, VOLUME_RANGE_UNK2));
                            func_802ad770(0x03020000 | ((channelIndex & 0xff) << 8),
                                          get_sound_pan(*sSoundBanks[bank][soundIndex].x,
                                                        *sSoundBanks[bank][soundIndex].z)
                                                  * 127.0f
                                              + 0.5f);
                            func_802ad728(0x04020000 | ((channelIndex & 0xff) << 8),
                                          get_sound_freq_scale(bank, soundIndex));
#else
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->reverbVol =
                                get_sound_reverb(bank, soundIndex, channelIndex);
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->volume =
                                get_sound_volume(bank, soundIndex, VOLUME_RANGE_UNK2);
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->pan =
                                get_sound_pan(*sSoundBanks[bank][soundIndex].x,
                                              *sSoundBanks[bank][soundIndex].z);
                            gSequencePlayers[SEQ_PLAYER_SFX].channels[channelIndex]->freqScale =
                                get_sound_freq_scale(bank, soundIndex);
#endif
                            break;
                    }
                }
            }

            channelIndex++;
        }

        channelIndex += sMaxChannelsForSoundBank[bank] - sUsedChannelsForSoundBank[bank];
    }
}

static void seq_player_play_sequence(u8 player, u8 seqId, u16 arg2) {
    u8 targetVolume;
    u8 i;

    if (player == SEQ_PLAYER_LEVEL) {
        sCurrentBackgroundMusicSeqId = seqId & SEQ_BASE_ID;
        sBackgroundMusicForDynamics = SEQUENCE_NONE;
        sCurrentMusicDynamic = 0xff;
        sMusicDynamicDelay = 2;
    }

    for (i = 0; i < CHANNELS_MAX; i++) {
        D_80360928[player][i].remainingFrames = 0;
    }

#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
    func_802ad770(0x46000000 | ((u8)(u32) player) << 16, seqId & SEQ_VARIATION);
    func_802ad74c(0x82000000 | ((u8)(u32) player) << 16 | ((u8)(seqId & SEQ_BASE_ID)) << 8, arg2);

    if (player == SEQ_PLAYER_LEVEL) {
        targetVolume = begin_background_music_fade(0);
        if (targetVolume != 0xff) {
            gSequencePlayers[SEQ_PLAYER_LEVEL].fadeVolumeScale = (f32) targetVolume / US_FLOAT(127.0);
        }
    }
#else

    gSequencePlayers[player].seqVariation = seqId & SEQ_VARIATION;
    load_sequence(player, seqId & SEQ_BASE_ID, 0);

    if (player == SEQ_PLAYER_LEVEL) {
        targetVolume = begin_background_music_fade(0);
        if (targetVolume != 0xff) {
            gSequencePlayers[SEQ_PLAYER_LEVEL].state = SEQUENCE_PLAYER_STATE_4;
            gSequencePlayers[SEQ_PLAYER_LEVEL].fadeVolume = (f32) targetVolume / US_FLOAT(127.0);
        }
    } else {
        func_8031D690(player, arg2);
    }
#endif
}

void seq_player_fade_out(u8 player, u16 fadeDuration) {
    NDS_AUDIO_GUARD();
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
#ifdef VERSION_EU
    u32 fd = fadeDuration;
#else
    s32 fd = fadeDuration;
#endif
    if (!player) {
        sCurrentBackgroundMusicSeqId = SEQUENCE_NONE;
    }
    func_802ad74c(0x83000000 | (player & 0xff) << 16, fd);
#else
    if (player == SEQ_PLAYER_LEVEL) {
        sCurrentBackgroundMusicSeqId = SEQUENCE_NONE;
    }
    seq_player_fade_to_zero_volume(player, fadeDuration);
#endif
}

void fade_volume_scale(u8 player, u8 targetScale, u16 fadeDuration) {
    NDS_AUDIO_GUARD();
    u8 i;
    for (i = 0; i < CHANNELS_MAX; i++) {
        fade_channel_volume_scale(player, i, targetScale, fadeDuration);
    }
}

static void fade_channel_volume_scale(u8 player, u8 channelIndex, u8 targetScale, u16 fadeDuration) {
    struct ChannelVolumeScaleFade *temp;

    if (gSequencePlayers[player].channels[channelIndex] != &gSequenceChannelNone) {
        temp = &D_80360928[player][channelIndex];
        temp->remainingFrames = fadeDuration;
        temp->velocity = ((f32)(targetScale / US_FLOAT(127.0))
                          - gSequencePlayers[player].channels[channelIndex]->volumeScale)
                         / fadeDuration;
        temp->target = targetScale;
        temp->current = gSequencePlayers[player].channels[channelIndex]->volumeScale;
    }
}

static void func_8031F96C(u8 player) {
    u8 i;

    for (i = 0; i < CHANNELS_MAX; i++) {
        if (gSequencePlayers[player].channels[i] != &gSequenceChannelNone
            && D_80360928[player][i].remainingFrames != 0) {
            D_80360928[player][i].current += D_80360928[player][i].velocity;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
            func_802ad728(0x01000000 | (player & 0xff) << 16 | (i & 0xff) << 8,
                          D_80360928[player][i].current);
#else
            gSequencePlayers[player].channels[i]->volumeScale = D_80360928[player][i].current;
#endif
            D_80360928[player][i].remainingFrames--;
            if (D_80360928[player][i].remainingFrames == 0) {
#if defined(VERSION_EU)
                func_802ad728(0x01000000 | (player & 0xff) << 16 | (i & 0xff) << 8,
                              FLOAT_CAST(D_80360928[player][i].target) / 127.0);
#elif defined(VERSION_SH) || defined(VERSION_CN)
                func_802ad728(0x01000000 | (player & 0xff) << 16 | (i & 0xff) << 8,
                              FLOAT_CAST(D_80360928[player][i].target) / 127.0f);
#else
                gSequencePlayers[player].channels[i]->volumeScale =
                    D_80360928[player][i].target / 127.0f;
#endif
            }
        }
    }
}

NDS_ITCM_CODE void process_level_music_dynamics(void) {
    NDS_AUDIO_GUARD();
    u32 conditionBits;
    u16 tempBits;
    UNUSED u16 pad;
    u8 musicDynIndex;
    u8 condIndex;
    u8 i;
    u8 j;
    s16 conditionValues[8];
    u8 conditionTypes[8];
    s16 dur1;
    s16 dur2;
    u16 bit;

    func_8031F96C(0);
    func_8031F96C(2);
    func_80320ED8();
    if (sMusicDynamicDelay != 0) {
        sMusicDynamicDelay--;
    } else {
        sBackgroundMusicForDynamics = sCurrentBackgroundMusicSeqId;
    }

    if (sBackgroundMusicForDynamics != sLevelDynamics[gCurrLevelNum][0]) {
        return;
    }

    conditionBits = sLevelDynamics[gCurrLevelNum][1] & 0xff00;
    musicDynIndex = (u8) sLevelDynamics[gCurrLevelNum][1] & 0xff;
    i = 2;
    while (conditionBits & 0xff00) {
        j = 0;
        condIndex = 0;
        bit = 0x8000;
        while (j < 8) {
            if (conditionBits & bit) {
                conditionValues[condIndex] = sLevelDynamics[gCurrLevelNum][i++];
                conditionTypes[condIndex] = j;
                condIndex++;
            }

            j++;
            bit = bit >> 1;
        }

        for (j = 0; j < condIndex; j++) {
            switch (conditionTypes[j]) {
                case MARIO_X_GE: {
                    if (((s16) gMarioStates[0].pos[0]) < conditionValues[j]) {
                        j = condIndex + 1;
                    }
                    break;
                }
                case MARIO_Y_GE: {
                    if (((s16) gMarioStates[0].pos[1]) < conditionValues[j]) {
                        j = condIndex + 1;
                    }
                    break;
                }
                case MARIO_Z_GE: {
                    if (((s16) gMarioStates[0].pos[2]) < conditionValues[j]) {
                        j = condIndex + 1;
                    }
                    break;
                }
                case MARIO_X_LT: {
                    if (((s16) gMarioStates[0].pos[0]) >= conditionValues[j]) {
                        j = condIndex + 1;
                    }
                    break;
                }
                case MARIO_Y_LT: {
                    if (((s16) gMarioStates[0].pos[1]) >= conditionValues[j]) {
                        j = condIndex + 1;
                    }
                    break;
                }
                case MARIO_Z_LT: {
                    if (((s16) gMarioStates[0].pos[2]) >= conditionValues[j]) {
                        j = condIndex + 1;
                    }
                    break;
                }
                case MARIO_IS_IN_AREA: {
                    if (gCurrAreaIndex != conditionValues[j]) {
                        j = condIndex + 1;
                    }
                    break;
                }
                case MARIO_IS_IN_ROOM: {
                    if (gMarioCurrentRoom != conditionValues[j]) {
                        j = condIndex + 1;
                    }
                    break;
                }
            }
        }

        if (j == condIndex) {

            tempBits = 0;
        } else {
            tempBits      = sLevelDynamics[gCurrLevelNum][i] & 0xff00;
            musicDynIndex = sLevelDynamics[gCurrLevelNum][i] & 0xff;
            i++;
        }

        conditionBits = tempBits;
    }

    if (sCurrentMusicDynamic != musicDynIndex) {
        tempBits = 1;
        if (sCurrentMusicDynamic == 0xff) {
            dur1 = 1;
            dur2 = 1;
        } else {
            dur1 = sMusicDynamics[musicDynIndex].dur1;
            dur2 = sMusicDynamics[musicDynIndex].dur2;
        }

        for (i = 0; i < CHANNELS_MAX; i++) {
            conditionBits = tempBits;
            tempBits = 0;
            if (sMusicDynamics[musicDynIndex].bits1 & conditionBits) {
                fade_channel_volume_scale(SEQ_PLAYER_LEVEL, i, sMusicDynamics[musicDynIndex].volScale1,
                                          dur1);
            }
            if (sMusicDynamics[musicDynIndex].bits2 & conditionBits) {
                fade_channel_volume_scale(SEQ_PLAYER_LEVEL, i, sMusicDynamics[musicDynIndex].volScale2,
                                          dur2);
            }
            tempBits = conditionBits << 1;
        }

        sCurrentMusicDynamic = musicDynIndex;
    }
}

void unused_8031FED0(u8 player, u32 bits, s8 arg2) {
    u8 i;

    if (arg2 < 0) {
        arg2 = -arg2;
    }

    for (i = 0; i < CHANNELS_MAX; i++) {
        if (gSequencePlayers[player].channels[i] != &gSequenceChannelNone) {
            if ((bits & 3) == 0) {
                gSequencePlayers[player].channels[i]->volumeScale = 1.0f;
            } else if ((bits & 1) != 0) {
                gSequencePlayers[player].channels[i]->volumeScale = (f32) arg2 / US_FLOAT(127.0);
            } else {
                gSequencePlayers[player].channels[i]->volumeScale =
                    US_FLOAT(1.0) - (f32) arg2 / US_FLOAT(127.0);
            }
        }
        bits >>= 2;
    }
}

void seq_player_lower_volume(u8 player, u16 fadeDuration, u8 percentage) {
    if (player == SEQ_PLAYER_LEVEL) {
        sLowerBackgroundMusicVolume = TRUE;
        begin_background_music_fade(fadeDuration);
    } else if (gSequencePlayers[player].enabled == TRUE) {
        seq_player_fade_to_percentage_of_volume(player, fadeDuration, percentage);
    }
}

void seq_player_unlower_volume(u8 player, u16 fadeDuration) {
    sLowerBackgroundMusicVolume = FALSE;
    if (player == SEQ_PLAYER_LEVEL) {
        if (gSequencePlayers[player].state != SEQUENCE_PLAYER_STATE_FADE_OUT) {
            begin_background_music_fade(fadeDuration);
        }
    } else {
        if (gSequencePlayers[player].enabled == TRUE) {
            seq_player_fade_to_normal_volume(player, fadeDuration);
        }
    }
}

NDS_ITCM_CODE static u8 begin_background_music_fade(u16 fadeDuration) {
    u8 targetVolume = 0xff;

    if (sCurrentBackgroundMusicSeqId == SEQUENCE_NONE
        || sCurrentBackgroundMusicSeqId == SEQ_EVENT_CUTSCENE_CREDITS) {
        return 0xff;
    }

    if (gSequencePlayers[SEQ_PLAYER_LEVEL].volume == 0.0f && fadeDuration) {
        gSequencePlayers[SEQ_PLAYER_LEVEL].volume = gSequencePlayers[SEQ_PLAYER_LEVEL].fadeVolume;
    }

    if (sBackgroundMusicTargetVolume != TARGET_VOLUME_UNSET) {
        targetVolume = (sBackgroundMusicTargetVolume & TARGET_VOLUME_VALUE_MASK);
    }

    if (sBackgroundMusicMaxTargetVolume != TARGET_VOLUME_UNSET) {
        u8 maxTargetVolume = (sBackgroundMusicMaxTargetVolume & TARGET_VOLUME_VALUE_MASK);
        if (targetVolume > maxTargetVolume) {
            targetVolume = maxTargetVolume;
        }
    }

    if (sLowerBackgroundMusicVolume && targetVolume > 40) {
        targetVolume = 40;
    }

    if (sSoundBanksThatLowerBackgroundMusic != 0 && targetVolume > 20) {
        targetVolume = 20;
    }

    if (gSequencePlayers[SEQ_PLAYER_LEVEL].enabled == TRUE) {
        if (targetVolume != 0xff) {
            seq_player_fade_to_target_volume(SEQ_PLAYER_LEVEL, fadeDuration, targetVolume);
        } else {
#if defined(VERSION_JP) || defined(VERSION_US)
            gSequencePlayers[SEQ_PLAYER_LEVEL].volume =
                sBackgroundMusicDefaultVolume[sCurrentBackgroundMusicSeqId] / 127.0f;
#endif
            seq_player_fade_to_normal_volume(SEQ_PLAYER_LEVEL, fadeDuration);
        }
    }

    return targetVolume;
}

void set_audio_muted(u8 muted) {
    NDS_AUDIO_GUARD();
    u8 i;

    for (i = 0; i < SEQUENCE_PLAYERS; i++) {
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
        if (muted) {
            func_802ad74c(0xf1000000, 0);
        } else {
            func_802ad74c(0xf2000000, 0);
        }
#else
        gSequencePlayers[i].muted = muted;
#endif
    }
}

void sound_init(void) {
    u8 i;
    u8 j;

    for (i = 0; i < SOUND_BANK_COUNT; i++) {

        for (j = 0; j < 40; j++) {
            sSoundBanks[i][j].soundStatus = SOUND_STATUS_STOPPED;
        }

        for (j = 0; j < MAX_CHANNELS_PER_SOUND_BANK; j++) {
            sCurrentSound[i][j] = 0xff;
        }

        sSoundBankUsedListBack[i] = 0;
        sSoundBankFreeListFront[i] = 1;
        sNumSoundsInBank[i] = 0;
    }

    for (i = 0; i < SOUND_BANK_COUNT; i++) {

        sSoundBanks[i][0].prev = 0xff;
        sSoundBanks[i][0].next = 0xff;

        for (j = 1; j < 40 - 1; j++) {
            sSoundBanks[i][j].prev = j - 1;
            sSoundBanks[i][j].next = j + 1;
        }
        sSoundBanks[i][j].prev = j - 1;
        sSoundBanks[i][j].next = 0xff;
    }

    for (j = 0; j < 3; j++) {
        for (i = 0; i < CHANNELS_MAX; i++) {
            D_80360928[j][i].remainingFrames = 0;
        }
    }

    for (i = 0; i < MAX_BACKGROUND_MUSIC_QUEUE_SIZE; i++) {
        sBackgroundMusicQueue[i].priority = 0;
    }

    sound_banks_enable(SEQ_PLAYER_SFX, SOUND_BANKS_ALL_BITS);

    sUnused80332118 = 0;
    sBackgroundMusicTargetVolume = TARGET_VOLUME_UNSET;
    sLowerBackgroundMusicVolume = FALSE;
    sSoundBanksThatLowerBackgroundMusic = 0;
    sUnused80332114 = 0;
    sCurrentBackgroundMusicSeqId = 0xff;
    gSoundMode = SOUND_MODE_STEREO;
    sBackgroundMusicQueueSize = 0;
    sBackgroundMusicMaxTargetVolume = TARGET_VOLUME_UNSET;
    D_80332120 = 0;
    D_80332124 = 0;
    sNumProcessedSoundRequests = 0;
    sSoundRequestCount = 0;
}

void get_currently_playing_sound(u8 bank, u8 *numPlayingSounds, u8 *numSoundsInBank, u8 *soundId) {
    u8 i;
    u8 count = 0;

    for (i = 0; i < sMaxChannelsForSoundBank[bank]; i++) {
        if (sCurrentSound[bank][i] != 0xff) {
            count++;
        }
    }
    *numPlayingSounds = count;

    *numSoundsInBank = sNumSoundsInBank[bank];

    if (sCurrentSound[bank][0] != 0xff) {
        *soundId = (u8)(sSoundBanks[bank][sCurrentSound[bank][0]].soundBits >> SOUNDARGS_SHIFT_SOUNDID);
    } else {
        *soundId = 0xff;
    }
}

void stop_sound(u32 soundBits, f32 *pos) {
    NDS_AUDIO_GUARD();
    u8 bank = (soundBits & SOUNDARGS_MASK_BANK) >> SOUNDARGS_SHIFT_BANK;
    u8 soundIndex = sSoundBanks[bank][0].next;

    while (soundIndex != 0xff) {

        if ((u16)(soundBits >> SOUNDARGS_SHIFT_SOUNDID)
                == (u16)(sSoundBanks[bank][soundIndex].soundBits >> SOUNDARGS_SHIFT_SOUNDID)
            && sSoundBanks[bank][soundIndex].x == pos) {

            update_background_music_after_sound(bank, soundIndex);
            sSoundBanks[bank][soundIndex].soundBits = NO_SOUND;
            soundIndex = 0xff;
        } else {
            soundIndex = sSoundBanks[bank][soundIndex].next;
        }
    }
}

void stop_sounds_from_source(f32 *pos) {
    NDS_AUDIO_GUARD();
    u8 bank;
    u8 soundIndex;

    for (bank = 0; bank < SOUND_BANK_COUNT; bank++) {
        soundIndex = sSoundBanks[bank][0].next;
        while (soundIndex != 0xff) {
            if (sSoundBanks[bank][soundIndex].x == pos) {
                update_background_music_after_sound(bank, soundIndex);
                sSoundBanks[bank][soundIndex].soundBits = NO_SOUND;
            }
            soundIndex = sSoundBanks[bank][soundIndex].next;
        }
    }
}

static void stop_sounds_in_bank(u8 bank) {
    u8 soundIndex = sSoundBanks[bank][0].next;

    while (soundIndex != 0xff) {
        update_background_music_after_sound(bank, soundIndex);
        sSoundBanks[bank][soundIndex].soundBits = NO_SOUND;
        soundIndex = sSoundBanks[bank][soundIndex].next;
    }
}

void stop_sounds_in_continuous_banks(void) {
    NDS_AUDIO_GUARD();
    stop_sounds_in_bank(SOUND_BANK_MOVING);
    stop_sounds_in_bank(SOUND_BANK_ENV);
    stop_sounds_in_bank(SOUND_BANK_AIR);
}

void sound_banks_disable(UNUSED u8 player, u16 bankMask) {
    NDS_AUDIO_GUARD();
    u8 i;

    for (i = 0; i < SOUND_BANK_COUNT; i++) {
        if (bankMask & 1) {
            sSoundBankDisabled[i] = TRUE;
        }
        bankMask = bankMask >> 1;
    }
}

static void disable_all_sequence_players(void) {
    u8 i;

    for (i = 0; i < SEQUENCE_PLAYERS; i++) {
        sequence_player_disable(&gSequencePlayers[i]);
    }
}

void sound_banks_enable(UNUSED u8 player, u16 bankMask) {
    NDS_AUDIO_GUARD();
    u8 i;

    for (i = 0; i < SOUND_BANK_COUNT; i++) {
        if (bankMask & 1) {
            sSoundBankDisabled[i] = FALSE;
        }
        bankMask = bankMask >> 1;
    }
}

u8 unused_803209D8(u8 player, u8 channelIndex, u8 arg2) {
    u8 ret = 0;
    if (gSequencePlayers[player].channels[channelIndex] != &gSequenceChannelNone) {
        gSequencePlayers[player].channels[channelIndex]->stopSomething2 = arg2;
        ret = arg2;
    }
    return ret;
}

void set_sound_moving_speed(u8 bank, u8 speed) {
    NDS_AUDIO_GUARD();
    sSoundMovingSpeed[bank] = speed;
}

void play_dialog_sound(u8 dialogID) {
    NDS_AUDIO_GUARD();
    u8 speaker;

    if (dialogID >= DIALOG_COUNT) {
        dialogID = 0;
    }

    speaker = sDialogSpeaker[dialogID];
    if (speaker != 0xff) {
        play_sound(sDialogSpeakerVoice[speaker], gGlobalSoundSource);

        if (speaker == BOWS1) {
            seq_player_play_sequence(SEQ_PLAYER_ENV, SEQ_EVENT_KOOPA_MESSAGE, 0);
        }
    }

#ifndef VERSION_JP

    if (dialogID == DIALOG_010 || dialogID == DIALOG_011 || dialogID == DIALOG_012) {
        play_puzzle_jingle();
    }
#endif
}

void play_music(u8 player, u16 seqArgs, u16 fadeTimer) {
    NDS_AUDIO_GUARD();
    u8 seqId = seqArgs & 0xff;
    u8 priority = seqArgs >> 8;
    u8 i;
    u8 foundIndex = 0;

    if (player != SEQ_PLAYER_LEVEL) {
        seq_player_play_sequence(player, seqId, fadeTimer);
        return;
    }

    if (sBackgroundMusicQueueSize == MAX_BACKGROUND_MUSIC_QUEUE_SIZE) {
        return;
    }

    for (i = 0; i < sBackgroundMusicQueueSize; i++) {
        if (sBackgroundMusicQueue[i].seqId == seqId) {
            if (i == 0) {
                seq_player_play_sequence(SEQ_PLAYER_LEVEL, seqId, fadeTimer);
            } else if (!gSequencePlayers[SEQ_PLAYER_LEVEL].enabled) {
                stop_background_music(sBackgroundMusicQueue[0].seqId);
            }
            return;
        }
    }

    for (i = 0; i < sBackgroundMusicQueueSize; i++) {
        if (sBackgroundMusicQueue[i].priority <= priority) {
            foundIndex = i;
            i = sBackgroundMusicQueueSize;
        }
    }

    if (foundIndex == 0) {
        seq_player_play_sequence(SEQ_PLAYER_LEVEL, seqId, fadeTimer);
        sBackgroundMusicQueueSize++;
    }

    for (i = sBackgroundMusicQueueSize - 1; i > foundIndex; i--) {
        sBackgroundMusicQueue[i].priority = sBackgroundMusicQueue[i - 1].priority;
        sBackgroundMusicQueue[i].seqId = sBackgroundMusicQueue[i - 1].seqId;
    }

    sBackgroundMusicQueue[foundIndex].priority = priority;
    sBackgroundMusicQueue[foundIndex].seqId = seqId;
}

void stop_background_music(u16 seqId) {
    NDS_AUDIO_GUARD();
    u8 foundIndex;
    u8 i;

    if (sBackgroundMusicQueueSize == 0) {
        return;
    }

    foundIndex = sBackgroundMusicQueueSize;

    for (i = 0; i < sBackgroundMusicQueueSize; i++) {
        if (sBackgroundMusicQueue[i].seqId == (u8)(seqId & 0xff)) {

            sBackgroundMusicQueueSize--;
            if (i == 0) {
                if (sBackgroundMusicQueueSize != 0) {
                    seq_player_play_sequence(SEQ_PLAYER_LEVEL, sBackgroundMusicQueue[1].seqId, 0);
                } else {
                    seq_player_fade_out(SEQ_PLAYER_LEVEL, 20);
                }
            }
            foundIndex = i;
            i = sBackgroundMusicQueueSize;
        }
    }

    for (i = foundIndex; i < sBackgroundMusicQueueSize; i++) {
        sBackgroundMusicQueue[i].priority = sBackgroundMusicQueue[i + 1].priority;
        sBackgroundMusicQueue[i].seqId = sBackgroundMusicQueue[i + 1].seqId;
    }

    sBackgroundMusicQueue[i].priority = 0;
}

void fadeout_background_music(u16 seqId, u16 fadeOut) {
    NDS_AUDIO_GUARD();
    if (sBackgroundMusicQueueSize != 0 && sBackgroundMusicQueue[0].seqId == (u8)(seqId & 0xff)) {
        seq_player_fade_out(SEQ_PLAYER_LEVEL, fadeOut);
    }
}

void drop_queued_background_music(void) {
    NDS_AUDIO_GUARD();
    if (sBackgroundMusicQueueSize != 0) {
        sBackgroundMusicQueueSize = 1;
    }
}

u16 get_current_background_music(void) {
    if (sBackgroundMusicQueueSize != 0) {
        return (sBackgroundMusicQueue[0].priority << 8) + sBackgroundMusicQueue[0].seqId;
    }
    return -1;
}

void func_80320ED8(void) {
    NDS_AUDIO_GUARD();
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
    if (D_EU_80300558 != 0) {
        D_EU_80300558--;
    }

    if (gSequencePlayers[SEQ_PLAYER_ENV].enabled
        || sBackgroundMusicMaxTargetVolume == TARGET_VOLUME_UNSET || D_EU_80300558 != 0) {
#else
    if (gSequencePlayers[SEQ_PLAYER_ENV].enabled
        || sBackgroundMusicMaxTargetVolume == TARGET_VOLUME_UNSET) {
#endif
        return;
    }

    sBackgroundMusicMaxTargetVolume = TARGET_VOLUME_UNSET;
    begin_background_music_fade(50);

    if (sBackgroundMusicTargetVolume != TARGET_VOLUME_UNSET
        && (D_80332120 == SEQ_EVENT_MERRY_GO_ROUND || D_80332120 == SEQ_EVENT_PIRANHA_PLANT)) {
        seq_player_play_sequence(SEQ_PLAYER_ENV, D_80332120, 1);
        if (D_80332124 != 0xff) {
            seq_player_fade_to_target_volume(SEQ_PLAYER_ENV, 1, D_80332124);
        }
    }
}

void play_secondary_music(u8 seqId, u8 bgMusicVolume, u8 volume, u16 fadeTimer) {
    NDS_AUDIO_GUARD();
    UNUSED u32 dummy;

    sUnused80332118 = 0;
    if (sCurrentBackgroundMusicSeqId == 0xff || sCurrentBackgroundMusicSeqId == SEQ_MENU_TITLE_SCREEN) {
        return;
    }

    if (sBackgroundMusicTargetVolume == TARGET_VOLUME_UNSET) {
        sBackgroundMusicTargetVolume = bgMusicVolume + TARGET_VOLUME_IS_PRESENT_FLAG;
        begin_background_music_fade(fadeTimer);
        seq_player_play_sequence(SEQ_PLAYER_ENV, seqId, fadeTimer >> 1);
        if (volume < 0x80) {
            seq_player_fade_to_target_volume(SEQ_PLAYER_ENV, fadeTimer, volume);
        }
        D_80332124 = volume;
        D_80332120 = seqId;
    } else if (volume != 0xff) {
        sBackgroundMusicTargetVolume = bgMusicVolume + TARGET_VOLUME_IS_PRESENT_FLAG;
        begin_background_music_fade(fadeTimer);
        seq_player_fade_to_target_volume(SEQ_PLAYER_ENV, fadeTimer, volume);
        D_80332124 = volume;
    }
}

void func_80321080(u16 fadeTimer) {
    NDS_AUDIO_GUARD();
    if (sBackgroundMusicTargetVolume != TARGET_VOLUME_UNSET) {
        sBackgroundMusicTargetVolume = TARGET_VOLUME_UNSET;
        D_80332120 = 0;
        D_80332124 = 0;
        begin_background_music_fade(fadeTimer);
        seq_player_fade_out(SEQ_PLAYER_ENV, fadeTimer);
    }
}

void func_803210D4(u16 fadeDuration) {
    NDS_AUDIO_GUARD();
    u8 i;

    if (sHasStartedFadeOut) {
        return;
    }

    if (gSequencePlayers[SEQ_PLAYER_LEVEL].enabled == TRUE) {
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
        func_802ad74c(0x83000000, fadeDuration);
#else
        seq_player_fade_to_zero_volume(SEQ_PLAYER_LEVEL, fadeDuration);
#endif
    }

    if (gSequencePlayers[SEQ_PLAYER_ENV].enabled == TRUE) {
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
        func_802ad74c(0x83010000, fadeDuration);
#else
        seq_player_fade_to_zero_volume(SEQ_PLAYER_ENV, fadeDuration);
#endif
    }

    for (i = 0; i < SOUND_BANK_COUNT; i++) {
        if (i != SOUND_BANK_MENU) {
            fade_channel_volume_scale(SEQ_PLAYER_SFX, i, 0, fadeDuration / 16);
        }
    }

    sHasStartedFadeOut = TRUE;
}

void play_course_clear(void) {
    NDS_AUDIO_GUARD();
    seq_player_play_sequence(SEQ_PLAYER_ENV, SEQ_EVENT_CUTSCENE_COLLECT_STAR, 0);
    sBackgroundMusicMaxTargetVolume = TARGET_VOLUME_IS_PRESENT_FLAG | 0;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
    D_EU_80300558 = 2;
#endif
    begin_background_music_fade(50);
}

void play_peachs_jingle(void) {
    NDS_AUDIO_GUARD();
    seq_player_play_sequence(SEQ_PLAYER_ENV, SEQ_EVENT_PEACH_MESSAGE, 0);
    sBackgroundMusicMaxTargetVolume = TARGET_VOLUME_IS_PRESENT_FLAG | 0;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
    D_EU_80300558 = 2;
#endif
    begin_background_music_fade(50);
}

void play_puzzle_jingle(void) {
    NDS_AUDIO_GUARD();
    seq_player_play_sequence(SEQ_PLAYER_ENV, SEQ_EVENT_SOLVE_PUZZLE, 0);
    sBackgroundMusicMaxTargetVolume = TARGET_VOLUME_IS_PRESENT_FLAG | 20;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
    D_EU_80300558 = 2;
#endif
    begin_background_music_fade(50);
}

void play_star_fanfare(void) {
    NDS_AUDIO_GUARD();
    seq_player_play_sequence(SEQ_PLAYER_ENV, SEQ_EVENT_HIGH_SCORE, 0);
    sBackgroundMusicMaxTargetVolume = TARGET_VOLUME_IS_PRESENT_FLAG | 20;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
    D_EU_80300558 = 2;
#endif
    begin_background_music_fade(50);
}

void play_power_star_jingle(u8 arg0) {
    NDS_AUDIO_GUARD();
    if (!arg0) {
        sBackgroundMusicTargetVolume = 0;
    }
    seq_player_play_sequence(SEQ_PLAYER_ENV, SEQ_EVENT_CUTSCENE_STAR_SPAWN, 0);
    sBackgroundMusicMaxTargetVolume = TARGET_VOLUME_IS_PRESENT_FLAG | 20;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
    D_EU_80300558 = 2;
#endif
    begin_background_music_fade(50);
}

void play_race_fanfare(void) {
    NDS_AUDIO_GUARD();
    seq_player_play_sequence(SEQ_PLAYER_ENV, SEQ_EVENT_RACE, 0);
    sBackgroundMusicMaxTargetVolume = TARGET_VOLUME_IS_PRESENT_FLAG | 20;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
    D_EU_80300558 = 2;
#endif
    begin_background_music_fade(50);
}

void play_toads_jingle(void) {
    NDS_AUDIO_GUARD();
    seq_player_play_sequence(SEQ_PLAYER_ENV, SEQ_EVENT_TOAD_MESSAGE, 0);
    sBackgroundMusicMaxTargetVolume = TARGET_VOLUME_IS_PRESENT_FLAG | 20;
#if defined(VERSION_EU) || defined(VERSION_SH) || defined(VERSION_CN)
    D_EU_80300558 = 2;
#endif
    begin_background_music_fade(50);
}

void sound_reset(u8 presetId) {
    NDS_AUDIO_GUARD();
#ifndef VERSION_JP
    if (presetId >= 8) {
        presetId = 0;
        sUnused8033323C = 0;
    }
#endif
    sGameLoopTicked = 0;
    disable_all_sequence_players();
    sound_init();
#if defined(VERSION_SH) || defined(VERSION_CN)
    func_802ad74c(0xF2000000, 0);
#endif
#if defined(VERSION_JP) || defined(VERSION_US)
    audio_reset_session(&gAudioSessionPresets[presetId]);
#else
    audio_reset_session_eu(presetId);
#endif
    osWritebackDCacheAll();
    if (presetId != 7) {
        preload_sequence(SEQ_EVENT_SOLVE_PUZZLE, PRELOAD_BANKS | PRELOAD_SEQUENCE);
        preload_sequence(SEQ_EVENT_PEACH_MESSAGE, PRELOAD_BANKS | PRELOAD_SEQUENCE);
        preload_sequence(SEQ_EVENT_CUTSCENE_STAR_SPAWN, PRELOAD_BANKS | PRELOAD_SEQUENCE);
    }
    seq_player_play_sequence(SEQ_PLAYER_SFX, SEQ_SOUND_PLAYER, 0);
    D_80332108 = (D_80332108 & 0xf0) + presetId;
    gSoundMode = D_80332108 >> 4;
    sHasStartedFadeOut = FALSE;
}

void audio_set_sound_mode(u8 soundMode) {
    NDS_AUDIO_GUARD();
    D_80332108 = (D_80332108 & 0xf) + (soundMode << 4);
    gSoundMode = soundMode;
}

#if defined(VERSION_JP) || defined(VERSION_US)
void unused_80321460(UNUSED s32 arg0, UNUSED s32 arg1, UNUSED s32 arg2, UNUSED s32 arg3) {
}

void unused_80321474(UNUSED s32 arg0) {
}
#endif
