#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

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

static bool nds_assets_readable(void) {
    FILE *f = fopen("nitro:/sound/sound_data.ctl", "rb");
    if (f == NULL) {
        return false;
    }
    unsigned char probe[16];
    size_t got = fread(probe, 1, sizeof(probe), f);
    fclose(f);
    return got == sizeof(probe);
}

static char sNdsRomPath[300];
static char *sNdsArgvPtr[1];

static bool nds_nitrofs_file_mode(const char *path) {
    struct __argv *av = __system_argv;

    if (path == NULL || path[0] == '\0') {
        return false;
    }
    if (strncmp(path, "fat", 3) == 0 || strncmp(path, "sd", 2) == 0) {
        strncpy(sNdsRomPath, path, sizeof(sNdsRomPath) - 1);
        sNdsRomPath[sizeof(sNdsRomPath) - 1] = '\0';
    } else {
        const char *slash = strchr(path, '/');
        strcpy(sNdsRomPath, "fat:");
        if (slash != NULL) {
            strncat(sNdsRomPath, slash, sizeof(sNdsRomPath) - strlen(sNdsRomPath) - 1);
        } else {
            strncat(sNdsRomPath, "/", sizeof(sNdsRomPath) - strlen(sNdsRomPath) - 1);
            strncat(sNdsRomPath, path, sizeof(sNdsRomPath) - strlen(sNdsRomPath) - 1);
        }
    }

    sNdsArgvPtr[0] = sNdsRomPath;
    av->argvMagic = ARGV_MAGIC;
    av->commandLine = sNdsRomPath;
    av->length = strlen(sNdsRomPath) + 1;
    av->argc = 1;
    av->argv = sNdsArgvPtr;

    iprintf("[file] %s\n", sNdsRomPath);
    nitroFSInit(NULL);
    iprintf("[file] read\n");
    return nds_assets_readable();
}

static bool nds_find_rom_and_mount(void) {
    const u8 *me = (const u8 *) __NDSHeader;
    static const char *dirs[] = { "/roms/nds", "/nds", "/roms", "/", NULL };
    char path[300];
    u8 cand[0x170];

    for (int i = 0; dirs[i] != NULL; i++) {
        DIR *dir = opendir(dirs[i]);
        if (dir == NULL) {
            continue;
        }
        struct dirent *ent;
        while ((ent = readdir(dir)) != NULL) {
            size_t n = strlen(ent->d_name);
            if (n < 4) {
                continue;
            }
            const char *x = ent->d_name + n - 4;
            if (x[0] != '.' || (x[1] | 0x20) != 'n' || (x[2] | 0x20) != 'd' || (x[3] | 0x20) != 's') {
                continue;
            }
            strcpy(path, dirs[i]);
            size_t pl = strlen(path);
            if (pl == 0 || path[pl - 1] != '/') {
                strcat(path, "/");
            }
            strncat(path, ent->d_name, sizeof(path) - strlen(path) - 1);

            FILE *f = fopen(path, "rb");
            if (f == NULL) {
                continue;
            }
            size_t got = fread(cand, 1, sizeof(cand), f);
            fclose(f);
            if (got >= 0x160 && memcmp(cand, me, 16) == 0
                && memcmp(cand + 0x15E, me + 0x15E, 2) == 0) {
                closedir(dir);
                iprintf("[scan] %s\n", path);
                return nds_nitrofs_file_mode(path);
            }
        }
        closedir(dir);
    }
    return false;
}

static bool nds_storage_init(void) {
    struct __argv *av = __system_argv;
    int argvOk = (av->argvMagic == ARGV_MAGIC && av->argc >= 1
                  && av->argv != NULL && av->argv[0] != NULL && av->argv[0][0] != '\0');
    const char *p0 = argvOk ? av->argv[0] : NULL;

    iprintf("Mode: %s\n", isDSiMode() ? "DSi" : "DS");
    iprintf("argv: magic=%d argc=%d\n", (av->argvMagic == ARGV_MAGIC), av->argc);
    iprintf("p0: %s\n", p0 ? p0 : "(none)");

    bool fatOk = fatInitDefault();
    iprintf("FAT: %s\n", fatOk ? "ok" : "no");

    if (p0 != NULL && nds_nitrofs_file_mode(p0)) {
        iprintf("Assets: file ok\n");
        return true;
    }
    if (p0 != NULL) {
        iprintf("[file] fail\n");
    }

    if (fatOk) {
        iprintf("[scan]\n");
        if (nds_find_rom_and_mount()) {
            iprintf("Assets: scan ok\n");
            return true;
        }
        iprintf("[scan] fail\n");
    }

    iprintf("[card] init\n");
    nitroFSInit(NULL);
    iprintf("[card] read\n");
    if (nds_assets_readable()) {
        iprintf("Assets: card ok\n");
        return true;
    }
    iprintf("[card] fail\n");

    return false;
}

static void nds_fatal_screen(const char *what) {
    videoSetModeSub(MODE_0_2D);
    vramSetBankH(VRAM_H_SUB_BG);
    PrintConsole *con = consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);
    consoleSelect(con);
    consoleClear();
    iprintf("  SUPER MARIO 64 DS\n");
    iprintf("  -----------------\n\n");
    iprintf("  STARTUP FAILED\n\n");
    iprintf("  %s\n\n", what);
    iprintf("This game streams its data from\n");
    iprintf("the ROM filesystem (NitroFS).\n\n");
    iprintf("- Real DS: use a flashcard with\n");
    iprintf("  a working DLDI driver.\n");
    iprintf("- DSi/3DS: run via TWiLight\n");
    iprintf("  Menu++ in DS (nds-bootstrap)\n");
    iprintf("  mode.\n");
    iprintf("- Emulator: enable DLDI / SD\n");
    iprintf("  access so the ROM filesystem\n");
    iprintf("  can be read.\n");
    while (1) {
        swiWaitForVBlank();
    }
}

int main(void) {

    videoSetModeSub(MODE_0_2D);
    vramSetBankH(VRAM_H_SUB_BG);
    sDebugConsole = consoleInit(NULL, 0, BgType_Text4bpp, BgSize_T_256x256, 15, 0, false, true);
    iprintf("SM64 DS Debug Console\n");
    iprintf("=====================\n");

    const size_t poolSize = 0x158000;
    u8 *pool = (u8 *)malloc(poolSize);

    iprintf("Pool init...\n");
    if (pool == NULL) {
        nds_fatal_screen("Out of memory (main pool).");
    }
    main_pool_init(pool, pool + poolSize);
    gEffectsMemoryPool = mem_pool_init(0x4000, MEMORY_POOL_LEFT);
    iprintf("  pool @ %p, size=0x%lx\n", (void *)pool, (unsigned long)poolSize);

    iprintf("Storage init...\n");
    if (!nds_storage_init()) {
        nds_fatal_screen("Could not read game assets.");
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
