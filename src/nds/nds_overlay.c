#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nds_include.h"
#include <filesystem.h>

#include "nds_overlay.h"
#include "level_table.h"

static const char *sLevelOverlayNames[] = {
    [LEVEL_NONE]             = NULL,
    [LEVEL_UNKNOWN_1]        = NULL,
    [LEVEL_UNKNOWN_2]        = NULL,
    [LEVEL_UNKNOWN_3]        = NULL,
    [LEVEL_BBH]              = "bbh",
    [LEVEL_CCM]              = "ccm",
    [LEVEL_CASTLE]           = "castle_inside",
    [LEVEL_HMC]              = "hmc",
    [LEVEL_SSL]              = "ssl",
    [LEVEL_BOB]              = "bob",
    [LEVEL_SL]               = "sl",
    [LEVEL_WDW]              = "wdw",
    [LEVEL_JRB]              = "jrb",
    [LEVEL_THI]              = "thi",
    [LEVEL_TTC]              = "ttc",
    [LEVEL_RR]               = "rr",
    [LEVEL_CASTLE_GROUNDS]   = "castle_grounds",
    [LEVEL_BITDW]            = "bitdw",
    [LEVEL_VCUTM]            = "vcutm",
    [LEVEL_BITFS]            = "bitfs",
    [LEVEL_SA]               = "sa",
    [LEVEL_BITS]             = "bits",
    [LEVEL_LLL]              = "lll",
    [LEVEL_DDD]              = "ddd",
    [LEVEL_WF]               = "wf",
    [LEVEL_ENDING]           = "ending",
    [LEVEL_CASTLE_COURTYARD] = "castle_courtyard",
    [LEVEL_PSS]              = "pss",
    [LEVEL_COTMC]            = "cotmc",
    [LEVEL_TOTWC]            = "totwc",
    [LEVEL_BOWSER_1]         = "bowser_1",
    [LEVEL_WMOTR]            = "wmotr",
    [LEVEL_UNKNOWN_32]       = NULL,
    [LEVEL_BOWSER_2]         = "bowser_2",
    [LEVEL_BOWSER_3]         = "bowser_3",
    [LEVEL_UNKNOWN_35]       = NULL,
    [LEVEL_TTM]              = "ttm",
};

static s32 sCurrentOverlayLevel = -1;

s32 nds_load_level_overlay(s32 levelId) {
    char path[64];
    FILE *f;
    long size;
    size_t bytesRead;

    if (levelId == sCurrentOverlayLevel) {
        return 1;
    }

    if (levelId < 0 || levelId >= (s32)(sizeof(sLevelOverlayNames) / sizeof(sLevelOverlayNames[0]))) {
        return 0;
    }

    const char *name = sLevelOverlayNames[levelId];
    if (name == NULL) {
        return 0;
    }

    sprintf(path, "nitro:/data/levels/%s/leveldata.bin", name);
    f = fopen(path, "rb");
    if (f == NULL) {
        return 0;
    }

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size > NDS_OVERLAY_SIZE) {
        fclose(f);
        return 0;
    }

    memset(NDS_OVERLAY_BASE, 0, NDS_OVERLAY_SIZE);
    bytesRead = fread(NDS_OVERLAY_BASE, 1, (size_t) size, f);
    fclose(f);
    if (bytesRead != (size_t) size) {
        return 0;
    }

    sCurrentOverlayLevel = levelId;
    return 1;
}

u8 *nds_load_sound_data(const char *filename, u32 *outSize) {
    char path[64];
    FILE *f;
    long size;
    u8 *buf;

    sprintf(path, "nitro:/sound/%s", filename);
    f = fopen(path, "rb");
    if (f == NULL) {
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);

    buf = (u8 *)malloc(size);
    if (buf == NULL) {
        fclose(f);
        return NULL;
    }

    fread(buf, 1, size, f);
    fclose(f);

    if (outSize) {
        *outSize = (u32)size;
    }
    return buf;
}

u8 *nds_load_mario_anims(u32 *outSize) {
    return nds_load_sound_data("mario_anims.bin", outSize);
}
