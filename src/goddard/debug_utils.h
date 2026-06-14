#ifndef GD_DEBUGGING_UTILS_H
#define GD_DEBUGGING_UTILS_H

#include <PR/ultratypes.h>

#include "gd_types.h"
#include "macros.h"

#define GD_NUM_MEM_TRACKERS 32
#define GD_NUM_TIMERS 32

struct MemTracker {
     const char *name;
     f32 begin;
     f32 end;
     f32 total;
};

struct GdTimer {
     s32 start;
     s32 end;
     s32 total;
     f32 unused;
     f32 scaledTotal;
     f32 prevScaledTotal;
     const char *name;
     s32 gadgetColourNum;
     s32 resetCount;
};

union PrintVal {
    f32 f;
    s32 i;
    s64 pad;
};

struct GdFile {
     u8  filler1[4];
     u32 pos;
     s8 *stream;

     u32 flags;
     u8  filler2[64];
     u32 size;
};

extern u8 *gGdStreamBuffer;

extern struct MemTracker *start_memtracker(const char *);
extern u32 stop_memtracker(const char *);
extern void remove_all_memtrackers(void);
extern struct MemTracker *get_memtracker_by_index(s32);
extern void print_all_memtrackers(void);
extern void print_all_timers(void);
extern void deactivate_timing(void);
extern void activate_timing(void);
extern void remove_all_timers(void);
extern struct GdTimer *get_timer(const char *);
extern struct GdTimer *get_timernum(s32);
extern void start_timer(const char *);
extern void restart_timer(const char *);
extern void split_timer(const char *);
extern void stop_timer(const char *);
extern f32 get_scaled_timer_total(const char *);
extern void fatal_print(const char *) NORETURN;
extern void fatal_printf(const char *, ...) NORETURN;
extern void imin(const char *);
extern void imout(void);
extern f32 gd_rand_float(void);
extern s32 gd_atoi(const char *);
extern f64 gd_lazy_atof(const char *, u32 *);
extern char *sprint_val_withspecifiers(char *, union PrintVal, char *);
extern void gd_strcpy(char *, const char *);
extern char *gd_strdup(const char *);
extern u32 gd_strlen(const char *);
extern char *gd_strcat(char *, const char *);
extern s32 gd_str_not_equal(const char *, const char *);
extern s32 gd_str_contains(const char *, const char *);
extern s32 gd_feof(struct GdFile *);
extern struct GdFile *gd_fopen(const char *, const char *);
extern s32 gd_fread(s8 *, s32, s32, struct GdFile *);
extern void gd_fclose(struct GdFile *);
extern u32 gd_get_file_size(struct GdFile *);
extern s32 gd_fread_line(char *, u32, struct GdFile *);

#endif
