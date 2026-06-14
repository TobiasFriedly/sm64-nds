#include <PR/ultratypes.h>
#include <PR/os_time.h>
#include <PR/gbi.h>

#include "sm64.h"
#include "profiler.h"
#include "game_init.h"

s16 gProfilerMode = 0;

s16 gCurrentFrameIndex1 = 1;
s16 gCurrentFrameIndex2 = 0;

struct ProfilerFrameData gProfilerFrameData[2];

void profiler_log_thread5_time(enum ProfilerGameEvent eventID) {
    gProfilerFrameData[gCurrentFrameIndex1].gameTimes[eventID] = osGetTime();

    if (eventID == THREAD5_END) {
        gCurrentFrameIndex1 ^= 1;
        gProfilerFrameData[gCurrentFrameIndex1].numSoundTimes = 0;
    }
}

void profiler_log_thread4_time(void) {
    struct ProfilerFrameData *profiler = &gProfilerFrameData[gCurrentFrameIndex1];

    if (profiler->numSoundTimes < ARRAY_COUNT(profiler->soundTimes)) {
        profiler->soundTimes[profiler->numSoundTimes++] = osGetTime();
    }
}

void profiler_log_gfx_time(enum ProfilerGfxEvent eventID) {
    if (eventID == TASKS_QUEUED) {
        gCurrentFrameIndex2 ^= 1;
        gProfilerFrameData[gCurrentFrameIndex2].numVblankTimes = 0;
    }

    gProfilerFrameData[gCurrentFrameIndex2].gfxTimes[eventID] = osGetTime();
}

void profiler_log_vblank_time(void) {
    struct ProfilerFrameData *profiler = &gProfilerFrameData[gCurrentFrameIndex2];

    if (profiler->numVblankTimes < ARRAY_COUNT(profiler->vblankTimes)) {
        profiler->vblankTimes[profiler->numVblankTimes++] = osGetTime();
    }
}

void draw_profiler_bar(OSTime clockBase, OSTime clockStart, OSTime clockEnd, s16 posY, u16 color) {
    s64 durationStart, durationEnd;
    s32 rectX1, rectX2;

    if ((durationStart = clockStart - clockBase) < 0) {
        durationStart = 0;
    }

    if ((durationEnd = clockEnd - clockBase) < 0) {
        durationEnd = 0;
    }

    rectX1 = ((((durationStart * 1000000) / osClockRate * 3) / 1000) + 30);
    rectX2 = ((((durationEnd * 1000000) / osClockRate * 3) / 1000) + 30);

    if (rectX1 > 319) {
        clockStart = 319;
    }
    if (rectX2 > 319) {
        clockEnd = 319;
    }

    if (rectX1 < rectX2) {
        gDPPipeSync(gDisplayListHead++);
        gDPSetFillColor(gDisplayListHead++, color << 16 | color);
        gDPFillRectangle(gDisplayListHead++, rectX1, posY, rectX2, posY + 2);
    }
}

void draw_reference_profiler_bars(void) {

    gDPPipeSync(gDisplayListHead++);
    gDPSetFillColor(gDisplayListHead++,
                    GPACK_RGBA5551(40, 80, 255, 1) << 16 | GPACK_RGBA5551(40, 80, 255, 1));
    gDPFillRectangle(gDisplayListHead++, 30, 220, 79, 222);

    gDPPipeSync(gDisplayListHead++);
    gDPSetFillColor(gDisplayListHead++,
                    GPACK_RGBA5551(255, 255, 40, 1) << 16 | GPACK_RGBA5551(255, 255, 40, 1));
    gDPFillRectangle(gDisplayListHead++, 79, 220, 128, 222);

    gDPPipeSync(gDisplayListHead++);
    gDPSetFillColor(gDisplayListHead++,
                    GPACK_RGBA5551(255, 120, 40, 1) << 16 | GPACK_RGBA5551(255, 120, 40, 1));
    gDPFillRectangle(gDisplayListHead++, 128, 220, 177, 222);

    gDPPipeSync(gDisplayListHead++);
    gDPSetFillColor(gDisplayListHead++,
                    GPACK_RGBA5551(255, 40, 40, 1) << 16 | GPACK_RGBA5551(255, 40, 40, 1));
    gDPFillRectangle(gDisplayListHead++, 177, 220, 226, 222);
}

void draw_profiler_mode_1(void) {
    s32 i;
    struct ProfilerFrameData *profiler;
    OSTime clockBase;

    profiler = &gProfilerFrameData[gCurrentFrameIndex1 ^ 1];

    clockBase = profiler->soundTimes[0] - (16433 * osClockRate / 1000000);

    draw_profiler_bar(clockBase, profiler->gameTimes[0], profiler->gameTimes[1], 212,
                      GPACK_RGBA5551(255, 255, 40, 1));

    draw_profiler_bar(clockBase, profiler->gameTimes[1], profiler->gameTimes[2], 212,
                      GPACK_RGBA5551(255, 120, 40, 1));

    draw_profiler_bar(clockBase, profiler->gameTimes[2], profiler->gameTimes[3], 212,
                      GPACK_RGBA5551(40, 192, 230, 1));

    profiler->numSoundTimes &= 0xFFFE;

    for (i = 0; i < profiler->numSoundTimes; i += 2) {
        draw_profiler_bar(clockBase, profiler->soundTimes[i], profiler->soundTimes[i + 1], 212,
                          GPACK_RGBA5551(255, 40, 40, 1));
    }

    draw_profiler_bar(clockBase, profiler->gfxTimes[0], profiler->gfxTimes[1], 216,
                      GPACK_RGBA5551(255, 255, 40, 1));

    draw_profiler_bar(clockBase, profiler->gfxTimes[1], profiler->gfxTimes[2], 216,
                      GPACK_RGBA5551(255, 120, 40, 1));

    profiler->numVblankTimes &= 0xFFFE;

    for (i = 0; i < profiler->numVblankTimes; i += 2) {
        draw_profiler_bar(clockBase, profiler->vblankTimes[i], profiler->vblankTimes[i + 1], 216,
                          GPACK_RGBA5551(255, 40, 40, 1));
    }

    draw_reference_profiler_bars();
}

void draw_profiler_mode_0(void) {
    s32 i;
    struct ProfilerFrameData *profiler;

    u64 clockStart;
    u64 levelScriptDuration;
    u64 renderDuration;
    u64 taskStart;
    u64 rspDuration;
    u64 rdpDuration;
    u64 vblank;
    u64 soundDuration;

    profiler = &gProfilerFrameData[gCurrentFrameIndex1 ^ 1];

    clockStart = profiler->gameTimes[0] <= profiler->soundTimes[0] ? profiler->gameTimes[0]
                                                                    : profiler->soundTimes[0];

    levelScriptDuration = profiler->gameTimes[1] - clockStart;
    renderDuration = profiler->gameTimes[2] - profiler->gameTimes[1];
    taskStart = 0;
    rspDuration = profiler->gfxTimes[1] - profiler->gfxTimes[0];
    rdpDuration = profiler->gfxTimes[2] - profiler->gfxTimes[0];
    vblank = 0;

    profiler->numSoundTimes &= 0xFFFE;

    for (i = 0; i < profiler->numSoundTimes; i += 2) {

        soundDuration = profiler->soundTimes[i + 1] - profiler->soundTimes[i];
        taskStart += soundDuration;

        if (profiler->soundTimes[i] < profiler->gameTimes[1]) {

            levelScriptDuration -= soundDuration;
        } else if (profiler->soundTimes[i] < profiler->gameTimes[2]) {

            renderDuration -= soundDuration;
        }
    }

    profiler->numSoundTimes &= 0xFFFE;

    for (i = 0; i < profiler->numSoundTimes; i += 2) {
        vblank += (profiler->vblankTimes[i + 1] - profiler->vblankTimes[i]);
    }

    clockStart = 0;
    draw_profiler_bar(0, clockStart, clockStart + taskStart, 212, GPACK_RGBA5551(255, 40, 40, 1));

    clockStart += taskStart;
    draw_profiler_bar(0, clockStart, clockStart + levelScriptDuration, 212,
                      GPACK_RGBA5551(255, 255, 40, 1));

    clockStart += levelScriptDuration;
    draw_profiler_bar(0, clockStart, clockStart + renderDuration, 212,
                      GPACK_RGBA5551(255, 120, 40, 1));

    draw_profiler_bar(0, 0, rdpDuration, 216, GPACK_RGBA5551(255, 120, 40, 1));

    draw_profiler_bar(0, 0, rspDuration, 216, GPACK_RGBA5551(255, 255, 40, 1));

    draw_profiler_bar(0, 0, vblank, 216, GPACK_RGBA5551(255, 40, 40, 1));

    draw_reference_profiler_bars();
}

void draw_profiler(void) {
    if (gPlayer1Controller->buttonPressed & L_TRIG) {
        gProfilerMode ^= 1;
    }

    if (gProfilerMode == 0) {
        draw_profiler_mode_0();
    } else {
        draw_profiler_mode_1();
    }
}
