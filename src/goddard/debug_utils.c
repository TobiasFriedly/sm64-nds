#include <PR/ultratypes.h>
#include <stdarg.h>

#include "debug_utils.h"
#include "gd_types.h"
#include "macros.h"
#include "renderer.h"
#include "draw_objects.h"

struct UnkBufThing {
     s32 size;
     char name[0x40];
};

static s32 sNumRoutinesInStack = 0;
static s32 sTimerGadgetColours[7] = {
    COLOUR_RED,
    COLOUR_WHITE,
    COLOUR_GREEN,
    COLOUR_BLUE,
    COLOUR_GRAY,
    COLOUR_YELLOW,
    COLOUR_PINK
};
static s32 sNumActiveMemTrackers = 0;
static u32 sPrimarySeed = 0x12345678;
static u32 sSecondarySeed = 0x58374895;

u8 *gGdStreamBuffer;
static const char *sRoutineNames[64];
static s32 sTimingActive;
static struct GdTimer sTimers[GD_NUM_TIMERS];
static struct MemTracker sMemTrackers[GD_NUM_MEM_TRACKERS];
static struct MemTracker *sActiveMemTrackers[16];

struct MemTracker *new_memtracker(const char *name) {
    s32 i;
    struct MemTracker *tracker = NULL;

    for (i = 0; i < ARRAY_COUNT(sMemTrackers); i++) {
        if (sMemTrackers[i].name == NULL) {
            sMemTrackers[i].name = name;
            tracker = &sMemTrackers[i];
            break;
        }
    }

    if (tracker != NULL) {
        tracker->total = 0.0f;
    }

    return tracker;
}

struct MemTracker *get_memtracker(const char *name) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sMemTrackers); i++) {
        if (sMemTrackers[i].name != NULL) {
            if (gd_str_not_equal(sMemTrackers[i].name, name) == FALSE) {
                return &sMemTrackers[i];
            }
        }
    }

    return NULL;
}

struct MemTracker *start_memtracker(const char *name) {
    struct MemTracker *tracker = get_memtracker(name);

    if (tracker == NULL) {
        tracker = new_memtracker(name);
        if (tracker == NULL) {
            fatal_printf("Unable to make memtracker '%s'", name);
        }
    }

    tracker->begin = (f32) get_alloc_mem_amt();
    if (sNumActiveMemTrackers >= ARRAY_COUNT(sActiveMemTrackers)) {
        fatal_printf("too many memtracker calls");
    }

    sActiveMemTrackers[sNumActiveMemTrackers++] = tracker;

    return tracker;
}

void print_most_recent_memtracker_name(void) {
    gd_printf("%s\n", sActiveMemTrackers[sNumActiveMemTrackers - 1]->name);
}

u32 stop_memtracker(const char *name) {
    struct MemTracker *tracker;

    if (sNumActiveMemTrackers-- < 0) {
        fatal_printf("bad mem tracker count");
    }

    tracker = get_memtracker(name);
    if (tracker == NULL) {
        fatal_printf("memtracker '%s' not found", name);
    }

    tracker->end = get_alloc_mem_amt();
    tracker->total += (tracker->end - tracker->begin);

    return (u32) tracker->total;
}

void remove_all_memtrackers(void) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sMemTrackers); i++) {
        sMemTrackers[i].name = NULL;
        sMemTrackers[i].begin = 0.0f;
        sMemTrackers[i].end = 0.0f;
        sMemTrackers[i].total = 0.0f;
    }

#ifdef AVOID_UB
    sNumActiveMemTrackers = 0;
#endif
}

struct MemTracker *get_memtracker_by_index(s32 index) {
    return &sMemTrackers[index];
}

void print_all_memtrackers(void) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sMemTrackers); i++) {
        if (sMemTrackers[i].name != NULL) {
            gd_printf("'%s' = %dk\n", sMemTrackers[i].name, (s32)(sMemTrackers[i].total / 1024.0f));
        }
    }
}

void print_all_timers(void) {
    s32 i;

    gd_printf("\nTimers:\n");
    for (i = 0; i < ARRAY_COUNT(sTimers); i++) {
        if (sTimers[i].name != NULL) {
            gd_printf("'%s' = %f (%d)\n", sTimers[i].name, sTimers[i].scaledTotal,
                      sTimers[i].resetCount);
        }
    }
}

void deactivate_timing(void) {
    sTimingActive = FALSE;
}

void activate_timing(void) {
    sTimingActive = TRUE;
}

void remove_all_timers(void) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sTimers); i++) {
        sTimers[i].name = NULL;
        sTimers[i].total = 0;
        sTimers[i].unused = 0.0f;
        sTimers[i].scaledTotal = 0.0f;
        sTimers[i].prevScaledTotal = 0.0f;
        sTimers[i].gadgetColourNum = sTimerGadgetColours[(u32) i % 7];
        sTimers[i].resetCount = 0;
    }
    activate_timing();
}

static struct GdTimer *new_timer(const char *name) {
    s32 i;
    struct GdTimer *timer = NULL;

    for (i = 0; i < ARRAY_COUNT(sTimers); i++) {
        if (sTimers[i].name == NULL) {
            sTimers[i].name = name;
            timer = &sTimers[i];
            break;
        }
    }

    return timer;
}

struct GdTimer *get_timer(const char *timerName) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sTimers); i++) {
        if (sTimers[i].name != NULL) {
            if (gd_str_not_equal(sTimers[i].name, timerName) == FALSE) {
                return &sTimers[i];
            }
        }
    }

    return NULL;
}

static struct GdTimer *get_timer_checked(const char *timerName) {
    struct GdTimer *timer;

    timer = get_timer(timerName);
    if (timer == NULL) {
        fatal_printf("Timer '%s' not found", timerName);
    }

    return timer;
}

struct GdTimer *get_timernum(s32 index) {
    if (index >= ARRAY_COUNT(sTimers)) {
        fatal_printf("get_timernum(): Timer number %d out of range (MAX %d)", index, ARRAY_COUNT(sTimers));
    }

    return &sTimers[index];
}

void split_timer_ptr(struct GdTimer *timer) {
    if (!sTimingActive) {
        return;
    }

    timer->end = gd_get_ostime();
    timer->total += timer->end - timer->start;

    if (timer->total < 0) {
        timer->total = 0;
    }

    timer->scaledTotal = ((f32) timer->total) / get_time_scale();
    timer->start = timer->end;
}

void split_all_timers(void) {
    s32 i;
    struct GdTimer *timer;

    for (i = 0; i < ARRAY_COUNT(sTimers); i++) {
        timer = get_timernum(i);
        if (timer->name != NULL) {
            split_timer_ptr(timer);
        }
    }
}

void start_all_timers(void) {
    s32 i;
    struct GdTimer *timer;

    if (!sTimingActive) {
        return;
    }

    for (i = 0; i < ARRAY_COUNT(sTimers); i++) {
        timer = get_timernum(i);

        if (timer->name != NULL) {
            timer->start = gd_get_ostime();
        }
    }
}

void start_timer(const char *name) {
    struct GdTimer *timer;

    if (!sTimingActive) {
        return;
    }

    timer = get_timer(name);
    if (timer == NULL) {
        timer = new_timer(name);
        if (timer == NULL) {
            fatal_printf("start_timer(): Unable to make timer '%s'", name);
        }
    }

    timer->prevScaledTotal = timer->scaledTotal;
    timer->start = gd_get_ostime();
    timer->total = 0;
    timer->resetCount = 1;
}

void restart_timer(const char *name) {
    struct GdTimer *timer;

    if (!sTimingActive) {
        return;
    }

    timer = get_timer(name);
    if (timer == NULL) {
        timer = new_timer(name);
        if (timer == NULL) {
            fatal_printf("restart_timer(): Unable to make timer '%s'", name);
        }
    }

    timer->start = gd_get_ostime();
    timer->resetCount++;
}

void split_timer(const char *name) {
    struct GdTimer *timer;

    if (!sTimingActive) {
        return;
    }

    timer = get_timer_checked(name);
    split_timer_ptr(timer);
}

void stop_timer(const char *name) {
    struct GdTimer *timer;

    if (!sTimingActive) {
        return;
    }

    timer = get_timer_checked(name);
    timer->end = gd_get_ostime();
    timer->total += timer->end - timer->start;
    if (timer->total < 0) {
        timer->total = 0;
    }

    timer->scaledTotal = ((f32) timer->total) / get_time_scale();
}

f32 get_scaled_timer_total(const char *name) {
    struct GdTimer *timer = get_timer_checked(name);

    return timer->scaledTotal;
}

f32 get_timer_total(const char *name) {
    struct GdTimer *timer = get_timer_checked(name);

    return (f32) timer->total;
}

void fatal_print(const char *str) {
    fatal_printf(str);
}

void print_stack_trace(void) {
    s32 i;

    for (i = 0; i < sNumRoutinesInStack; i++) {
        gd_printf("\tIn: '%s'\n", sRoutineNames[i]);
    }
}

void fatal_printf(const char *fmt, ...) {
    char cur;
    UNUSED u8 filler[4];
    va_list vl;

    va_start(vl, fmt);
    while ((cur = *fmt++)) {
        switch (cur) {
            case '%':
                switch (cur = *fmt++) {
                    case 'd':
                        gd_printf("%d", va_arg(vl, s32));
                        break;
                    case 'f':
                        gd_printf("%f", va_arg(vl, double));
                        break;
                    case 's':
                        gd_printf("%s", va_arg(vl, char *));
                        break;
                    case 'c':
#ifdef AVOID_UB
                        gd_printf("%c", (char)va_arg(vl, int));
#else
                        gd_printf("%c", va_arg(vl, char));
#endif
                        break;
                    case 'x':
                        gd_printf("%x", va_arg(vl, s32));
                        break;
                    default:
                        gd_printf("%c", cur);
                }
                break;
            case '\\':
                gd_printf("\\");
                break;
            case '\n':
                gd_printf("\n");
                break;
            default:
                gd_printf("%c", cur);
        }
    }
    va_end(vl);

    gd_printf("\n");
    print_stack_trace();
    gd_printf("\n");
    gd_exit(-1);
}

void imin(const char *routine) {
    sRoutineNames[sNumRoutinesInStack++] = routine;
    sRoutineNames[sNumRoutinesInStack] = NULL;

    if (sNumRoutinesInStack >= ARRAY_COUNT(sRoutineNames)) {
        fatal_printf("You're in too many routines");
    }
}

void imout(void) {
    s32 i;

    if (--sNumRoutinesInStack < 0) {
        for (i = 0; i < ARRAY_COUNT(sRoutineNames); i++) {
            if (sRoutineNames[i] != NULL) {
                gd_printf(" - %s\n", sRoutineNames[i]);
            } else {
                break;
            }
        }

        fatal_printf("imout() - imout() called too many times");
    }
}

f32 gd_rand_float(void) {
    u32 temp;
    u32 i;
    f32 val;

    for (i = 0; i < 4; i++) {
        if (sPrimarySeed & 0x80000000) {
            sPrimarySeed = sPrimarySeed << 1 | 1;
        } else {
            sPrimarySeed <<= 1;
        }
    }
    sPrimarySeed += 4;

    if ((sPrimarySeed ^= gd_get_ostime()) & 1) {
        temp = sPrimarySeed;
        sPrimarySeed = sSecondarySeed;
        sSecondarySeed = temp;
    }

    val = (sPrimarySeed & 0xFFFF) / 65535.0;

    return val;
}

s32 gd_atoi(const char *str) {
    char cur;
    const char *origstr = str;
    s32 curval;
    s32 out = 0;
    s32 isNegative = FALSE;

    while (TRUE) {
        cur = *str++;

        if ((cur < '0' || cur > '9') && (cur != '-'))
            fatal_printf("gd_atoi() bad number '%s'", origstr);

        if (cur == '-') {
            isNegative = TRUE;
        } else {
            curval = cur - '0';
            out += curval & 0xFF;

            if (*str == '\0' || *str == '.' || *str < '0' || *str > '9') {
                break;
            }

            out *= 10;
        }
    }

    if (isNegative) {
        out = -out;
    }

    return out;
}

f64 gd_lazy_atof(const char *str, UNUSED u32 *unk) {
    return gd_atoi(str);
}

static char sHexNumerals[] = {"0123456789ABCDEF"};

char *format_number_hex(char *str, s32 val) {
    s32 shift;

    for (shift = 28; shift > -4; shift -= 4) {
        *str++ = sHexNumerals[(val >> shift) & 0xF];
    }

    *str = '\0';

    return str;
}

static s32 sPadNumPrint = 0;

char *format_number_decimal(char *str, s32 val, s32 padnum) {
    s32 i;

    if (val == 0) {
        *str++ = '0';
        *str = '\0';
        return str;
    }

    if (val < 0) {
        val = -val;
        *str++ = '-';
    }

    while (padnum > 0) {
        if (padnum <= val) {
            sPadNumPrint = TRUE;

            for (i = 0; i < 9; i++) {
                val -= padnum;
                if (val < 0) {
                    val += padnum;
                    break;
                }
            }

            *str++ = i + '0';
        } else {
            if (sPadNumPrint) {
                *str++ = '0';
            }
        }

        padnum /= 10;
    }

    *str = '\0';

    return str;
}

static s32 int_sci_notation(s32 base, s32 significand) {
    s32 i;

    for (i = 1; i < significand; i++) {
        base *= 10;
    }

    return base;
}

char *sprint_val_withspecifiers(char *str, union PrintVal val, char *specifiers) {
    s32 fracPart;
    s32 intPart;
    s32 intPrec;
    s32 fracPrec;
    UNUSED u8 filler[4];
    char cur;

    fracPrec = 6;
    intPrec = 6;

    while ((cur = *specifiers++)) {
        if (cur == 'd') {
            sPadNumPrint = FALSE;
            str = format_number_decimal(str, val.i, 1000000000);
        } else if (cur == 'x') {
            sPadNumPrint = TRUE;
            str = format_number_hex(str, val.i);
        } else if (cur == 'f') {
            intPart = (s32) val.f;
            fracPart = (s32)((val.f - (f32) intPart) * (f32) int_sci_notation(10, fracPrec));
            sPadNumPrint = FALSE;
            str = format_number_decimal(str, intPart, int_sci_notation(10, intPrec));
            *str++ = '.';
            sPadNumPrint = TRUE;
            str = format_number_decimal(str, fracPart, int_sci_notation(10, fracPrec - 1));
        } else if (cur >= '0' && cur <= '9') {
            cur = cur - '0';
            intPrec = cur;
            if (*specifiers++) {
                fracPrec = (*specifiers++) - '0';
            }
        } else {
            gd_strcpy(str, "<BAD TYPE>");
            str += 10;
        }
    }

    return str;
}

void gd_strcpy(char *dst, const char *src) {
    while ((*dst++ = *src++)) {
        ;
    }
}

void ascii_to_uppercase(char *str) {
    char c;

    while ((c = *str)) {
        if (c >= 'a' && c <= 'z') {
            *str = c & 0xDF;
        }
        str++;
    }
}

char *gd_strdup(const char *src) {
    char *dst;

    dst = gd_malloc_perm((gd_strlen(src) + 1) * sizeof(char));

    if (dst == NULL) {
        fatal_printf("gd_strdup(): out of memory");
    }
    gd_strcpy(dst, src);

    return dst;
}

u32 gd_strlen(const char *str) {
    u32 len = 0;

    while (*str++) {
        len++;
    }

    return len;
}

char *gd_strcat(char *dst, const char *src) {
    while (*dst++) {
        ;
    }

    if (*src) {
        dst--;
        while ((*dst++ = *src++)) {
            ;
        }
    }

    return --dst;
}

s32 gd_str_not_equal(const char *str1, const char *str2) {
    while (*str1 && *str2) {
        if (*str1++ != *str2++) {
            return TRUE;
        }
    }

    return *str1 != '\0' || *str2 != '\0';
}

s32 gd_str_contains(const char *str1, const char *str2) {
    const char *startsub = str2;

    while (*str1 && *str2) {
        if (*str1++ != *str2++) {
            str2 = startsub;
        }
    }

    return !*str2;
}

s32 gd_feof(struct GdFile *f) {
    return f->flags & 0x4;
}

void gd_set_feof(struct GdFile *f) {
    f->flags |= 0x4;
}

struct GdFile *gd_fopen(const char *filename, const char *mode) {
    struct GdFile *f;
    char *loadedname;
    u32 i;
    UNUSED u8 filler[4];
    struct UnkBufThing buf;
    u8 *bufbytes;
    u8 *fileposptr;
    s32 filecsr;

    filecsr = 0;

    while (TRUE) {
        bufbytes = (u8 *) &buf;
        for (i = 0; i < sizeof(struct UnkBufThing); i++) {
            *bufbytes++ = gGdStreamBuffer[filecsr++];
        }
        stub_renderer_13(&buf);
        fileposptr = &gGdStreamBuffer[filecsr];
        filecsr += buf.size;

        loadedname = buf.name;

        if (buf.size == 0) {
            break;
        }
        if (!gd_str_not_equal(filename, loadedname)) {
            break;
        }
    }

    if (buf.size == 0) {
        fatal_printf("gd_fopen() File not found '%s'", filename);
        return NULL;
    }

    f = gd_malloc_perm(sizeof(struct GdFile));
    if (f == NULL) {
        fatal_printf("gd_fopen() Out of memory loading '%s'", filename);
        return NULL;
    }

    f->stream = (s8 *) fileposptr;
    f->size = buf.size;
    f->pos = f->flags = 0;
    if (gd_str_contains(mode, "w")) {
        f->flags |= 0x1;
    }
    if (gd_str_contains(mode, "b")) {
        f->flags |= 0x2;
    }

    return f;
}

s32 gd_fread(s8 *buf, s32 bytes, UNUSED s32 count, struct GdFile *f) {
    s32 bytesToRead = bytes;
    s32 bytesread;

    if (f->pos + bytesToRead > f->size) {
        bytesToRead = f->size - f->pos;
    }

    if (bytesToRead == 0) {
        gd_set_feof(f);
        return -1;
    }

    bytesread = bytesToRead;
    while (bytesread--) {
        *buf++ = f->stream[f->pos++];
    }

    return bytesToRead;
}

void gd_fclose(UNUSED struct GdFile *f) {
    return;
}

u32 gd_get_file_size(struct GdFile *f) {
    return f->size;
}

s32 is_newline(char c) {
    return c == '\r' || c == '\n';
}

s32 gd_fread_line(char *buf, u32 size, struct GdFile *f) {
    signed char c;
    u32 pos = 0;
    UNUSED u8 filler[4];

    do {
        if (gd_fread(&c, 1, 1, f) == -1) {
            break;
        }
    } while (is_newline(c));

    while (!is_newline(c)) {
        if (c == -1) {
            break;
        }
        if (pos > size) {
            break;
        }
        buf[pos++] = c;
        if (gd_fread(&c, 1, 1, f) == -1) {
            break;
        }
    }
    buf[pos++] = '\0';

    return pos;
}
