#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "nds_include.h"
#include <fat.h>
#include <filesystem.h>

#include "audio/data.h"
#include "audio/external.h"
#include "audio/load.h"
#include "audio/seqplayer.h"
#include "game/game_init.h"
#include "game/save_file.h"
#include "nds_renderer.h"
#include "nds_overlay.h"
#include "nds_sample_cache.h"
#include "nds_arm7_copy.h"
#include "nds_net.h"
#include "nds_netplay.h"
#include "nds_menu.h"

u8 nds_audio_state;
static u8 audio_step;
static u8 fps;

extern unsigned char *gSoundDataADSR;
extern unsigned char *gSoundDataRaw;
extern unsigned char *gMusicData;

u8 gNdsAudioDisabled = 0;

static PrintConsole *sDebugConsole = NULL;

void nds_debug_printf(const char *fmt, ...) {

    if (sDebugConsole && gNdsMenuOpen) {
        va_list args;
        va_start(args, fmt);
        consoleSelect(sDebugConsole);
        viprintf(fmt, args);
        va_end(args);
    }
}

void nds_debug_console_clear(void) {
    if (sDebugConsole) {
        consoleSelect(sDebugConsole);
        consoleClear();
    }
}

void exec_display_list(struct SPTask *spTask) {
    draw_frame((Gfx*)spTask->task.t.data_ptr);
    fps++;

    if (gNdsMenuOpen) {
        nds_menu_render();
    } else {
        nds_debug_console_clear();
    }

    nds_scache_update();

    nds_net_update();
}

static void update_audio(void) {

    gNdsAudioTick++;

    if (gNdsAudioDisabled) {

        IPC_SendSync(0);
        return;
    }

    if (nds_audio_state == 0) {

        if ((audio_step = (audio_step + 1) & 7) == 0) {
            update_game_sound();
            gAudioFrameCount += 2;
            gAudioRandom = ((gAudioRandom + gAudioFrameCount) * gAudioFrameCount);
        }

        process_sequences(0);

        nds_scache_retry_pending();
    } else if (nds_audio_state == 1) {

        for (int i = 0; i < 16; i++) {
            gNotes[i].enabled = false;
        }
        nds_audio_state = 2;
    }

    if (gNotes != NULL) {
        DC_FlushRange(gNotes, 20 * sizeof(struct Note));
    }

    IPC_SendSync(0);
}

static void update_fps(void) {

    if (!gNdsMenuOpen) {
        consoleClear();
        printf("FPS: %d\n", fps);
    }
    fps = 0;
}

static unsigned char *load_nitro_file(const char *path, unsigned long *outSize) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char *buf = (unsigned char *)malloc(size);
    if (!buf) { fclose(f); return NULL; }

    fread(buf, 1, size, f);
    fclose(f);

    if (outSize) *outSize = size;
    return buf;
}

#define TBL_HEADER_SIZE 4096

static void nds_load_audio_data(void) {
    gSoundDataADSR = load_nitro_file("nitro:/sound/sound_data.ctl", NULL);
    gMusicData     = load_nitro_file("nitro:/sound/sequences.bin", NULL);

    gSoundDataRaw = (unsigned char *)malloc(TBL_HEADER_SIZE);
    if (gSoundDataRaw) {
        FILE *f = fopen("nitro:/sound/sound_data.tbl", "rb");
        if (f) {
            if (fread(gSoundDataRaw, 1, TBL_HEADER_SIZE, f) != TBL_HEADER_SIZE) {
                free(gSoundDataRaw);
                gSoundDataRaw = NULL;
            }
            fclose(f);
        } else {
            free(gSoundDataRaw);
            gSoundDataRaw = NULL;
        }
    }
}

int main(void) {

    const size_t poolSize = 0x158000;
    u8 *pool = (u8 *)malloc(poolSize);

    videoSetModeSub(MODE_0_2D);
    vramSetBankH(VRAM_H_SUB_BG);
    sDebugConsole = consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);
    iprintf("SM64 DS Debug Console\n");
    iprintf("=====================\n");

    iprintf("Pool init...\n");
    if (pool == NULL) {
        iprintf("  POOL ALLOC FAILED (size=0x%lx)\n", (unsigned long)poolSize);
        while (1) swiWaitForVBlank();
    }
    main_pool_init(pool, pool + poolSize);
    gEffectsMemoryPool = mem_pool_init(0x4000, MEMORY_POOL_LEFT);
    iprintf("  pool @ %p, size=0x%lx\n", (void *)pool, (unsigned long)poolSize);

    iprintf("FAT init...\n");
    if (!fatInitDefault()) {
        iprintf("  FAT INIT FAILED!\n");
        while (1) swiWaitForVBlank();
    }
    iprintf("  FAT OK\n");

    iprintf("NitroFS init...\n");
    nds_overlay_init();

    {
        FILE *test = fopen("nitro:/sound/sound_data.ctl", "rb");
        if (test) {
            fseek(test, 0, SEEK_END);
            iprintf("  NitroFS OK (ctl=%ld)\n", ftell(test));
            fclose(test);
        } else {
            iprintf("  NitroFS FAILED!\n");
            while (1) swiWaitForVBlank();
        }
    }

    iprintf("Renderer init...\n");
    renderer_init();
    iprintf("  Renderer OK\n");

    vramSetBankH(VRAM_H_SUB_BG);
    sDebugConsole = consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);

    vramSetBankC(VRAM_C_ARM7_0x06000000);
    vramSetBankD(VRAM_D_ARM7_0x06020000);

    iprintf("Audio data load...\n");

    nds_load_audio_data();
    iprintf("  ADSR=%p RAW=%p SEQ=%p\n",
            (void *)gSoundDataADSR, (void *)gSoundDataRaw, (void *)gMusicData);

    if (!gSoundDataADSR || !gSoundDataRaw || !gMusicData) {
        iprintf("  AUDIO DATA MISSING - audio off\n");
        gNdsAudioDisabled = 1;
    } else if (nds_scache_init() != 0) {
        iprintf("  SAMPLE CACHE FAILED - audio off\n");
        gNdsAudioDisabled = 1;
    } else {

        nds_scache_add_region(NDS_VRAMC_SAMPLE_BASE, NDS_VRAM_BANK_SIZE);
        nds_scache_add_region(NDS_VRAMD_SAMPLE_BASE, NDS_VRAM_BANK_SIZE);
    }

    if (!gNdsAudioDisabled) {
        iprintf("Audio init...\n");
        audio_init();
        iprintf("Sound init...\n");
        sound_init();
    } else {
        iprintf("Audio init SKIPPED\n");
    }

    irqSet(IRQ_IPC_SYNC, update_audio);
    irqEnable(IRQ_IPC_SYNC);

    fifoSendValue32(FIFO_USER_01, (u32)gNotes);

#ifdef ENABLE_FPS

    timerStart(0, ClockDivider_1024, TIMER_FREQ_1024(1), update_fps);
#endif

    nds_netplay_init();

    thread5_game_loop(NULL);

    return 0;
}
