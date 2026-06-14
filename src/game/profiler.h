#ifndef PROFILER_H
#define PROFILER_H

#include <PR/ultratypes.h>
#include <PR/os_time.h>

#include "types.h"

extern u64 osClockRate;

struct ProfilerFrameData {
     s16 numSoundTimes;
     s16 numVblankTimes;

     OSTime gameTimes[5];

     OSTime gfxTimes[3];
     OSTime soundTimes[8];
     OSTime vblankTimes[8];
};

enum ProfilerGameEvent {
    THREAD5_START,
    LEVEL_SCRIPT_EXECUTE,
    BEFORE_DISPLAY_LISTS,
    AFTER_DISPLAY_LISTS,
    THREAD5_END
};

enum ProfilerGfxEvent {
    TASKS_QUEUED,
    RSP_COMPLETE,
    RDP_COMPLETE
};

void profiler_log_thread5_time(enum ProfilerGameEvent eventID);
void profiler_log_thread4_time(void);
void profiler_log_gfx_time(enum ProfilerGfxEvent eventID);
void profiler_log_vblank_time(void);
void draw_profiler(void);

#endif
