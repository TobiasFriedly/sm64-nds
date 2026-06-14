#include <ultra64.h>

#include "sm64.h"
#include "game_init.h"
#include "main.h"
#include "engine/math_util.h"
#include "area.h"
#include "level_update.h"
#include "save_file.h"
#include "sound_init.h"
#include "level_table.h"
#include "course_table.h"
#include "rumble_init.h"

#define MENU_DATA_MAGIC 0x4849
#define SAVE_FILE_MAGIC 0x4441

STATIC_ASSERT(sizeof(struct SaveBuffer) == EEPROM_SIZE, "eeprom buffer size must match");

extern struct SaveBuffer gSaveBuffer;

struct WarpCheckpoint gWarpCheckpoint;

s8 gMainMenuDataModified;
s8 gSaveFileModified;

u8 gLastCompletedCourseNum = COURSE_NONE;
u8 gLastCompletedStarNum = 0;
s8 sUnusedGotGlobalCoinHiScore = FALSE;
u8 gGotFileCoinHiScore = FALSE;
u8 gCurrCourseStarFlags = 0;

u8 gSpecialTripleJump = FALSE;

#define STUB_LEVEL(_0, _1, courseenum, _3, _4, _5, _6, _7, _8) courseenum,
#define DEFINE_LEVEL(_0, _1, courseenum, _3, _4, _5, _6, _7, _8, _9, _10) courseenum,

s8 gLevelToCourseNumTable[] = {
    #include "levels/level_defines.h"
};
#undef STUB_LEVEL
#undef DEFINE_LEVEL

STATIC_ASSERT(ARRAY_COUNT(gLevelToCourseNumTable) == LEVEL_COUNT - 1,
              "change this array if you are adding levels");

static void stub_save_file_1(void) {
    UNUSED u8 filler[4];
}

static s32 read_eeprom_data(void *buffer, s32 size) {
    s32 status = 0;

    if (gEepromProbe != 0) {
        s32 triesLeft = 4;
        u32 offset = (u32)((u8 *) buffer - (u8 *) &gSaveBuffer) / 8;

        do {
#if ENABLE_RUMBLE
            block_until_rumble_pak_free();
#endif
            triesLeft--;
            status = osEepromLongRead(&gSIEventMesgQueue, offset, buffer, size);
#if ENABLE_RUMBLE
            release_rumble_pak_control();
#endif
        } while (triesLeft > 0 && status != 0);
    }

    return status;
}

static s32 write_eeprom_data(void *buffer, s32 size) {
    s32 status = 1;

    if (gEepromProbe != 0) {
        s32 triesLeft = 4;
        u32 offset = (u32)((u8 *) buffer - (u8 *) &gSaveBuffer) >> 3;

        do {
#if ENABLE_RUMBLE
            block_until_rumble_pak_free();
#endif
            triesLeft--;
            status = osEepromLongWrite(&gSIEventMesgQueue, offset, buffer, size);
#if ENABLE_RUMBLE
            release_rumble_pak_control();
#endif
        } while (triesLeft > 0 && status != 0);
    }

    return status;
}

static u16 calc_checksum(u8 *data, s32 size) {
    u16 chksum = 0;

    while (size-- > 2) {
        chksum += *data++;
    }
    return chksum;
}

static s32 verify_save_block_signature(void *buffer, s32 size, u16 magic) {
    struct SaveBlockSignature *sig = (struct SaveBlockSignature *) ((size - 4) + (u8 *) buffer);

    if (sig->magic != magic) {
        return FALSE;
    }
    if (sig->chksum != calc_checksum(buffer, size)) {
        return FALSE;
    }
    return TRUE;
}

static void add_save_block_signature(void *buffer, s32 size, u16 magic) {
    struct SaveBlockSignature *sig = (struct SaveBlockSignature *) ((size - 4) + (u8 *) buffer);

    sig->magic = magic;
    sig->chksum = calc_checksum(buffer, size);
}

static void restore_main_menu_data(s32 srcSlot) {
    s32 destSlot = srcSlot ^ 1;

    add_save_block_signature(&gSaveBuffer.menuData[srcSlot], sizeof(gSaveBuffer.menuData[srcSlot]), MENU_DATA_MAGIC);

    bcopy(&gSaveBuffer.menuData[srcSlot], &gSaveBuffer.menuData[destSlot], sizeof(gSaveBuffer.menuData[destSlot]));

    write_eeprom_data(&gSaveBuffer.menuData[destSlot], sizeof(gSaveBuffer.menuData[destSlot]));
}

static void save_main_menu_data(void) {
    if (gMainMenuDataModified) {

        add_save_block_signature(&gSaveBuffer.menuData[0], sizeof(gSaveBuffer.menuData[0]), MENU_DATA_MAGIC);

        bcopy(&gSaveBuffer.menuData[0], &gSaveBuffer.menuData[1], sizeof(gSaveBuffer.menuData[1]));

        write_eeprom_data(gSaveBuffer.menuData, sizeof(gSaveBuffer.menuData));

        gMainMenuDataModified = FALSE;
    }
}

static void wipe_main_menu_data(void) {
    bzero(&gSaveBuffer.menuData[0], sizeof(gSaveBuffer.menuData[0]));

    gSaveBuffer.menuData[0].coinScoreAges[0] = 0x3FFFFFFF;
    gSaveBuffer.menuData[0].coinScoreAges[1] = 0x2AAAAAAA;
    gSaveBuffer.menuData[0].coinScoreAges[2] = 0x15555555;

    gMainMenuDataModified = TRUE;
    save_main_menu_data();
}

static s32 get_coin_score_age(s32 fileIndex, s32 courseIndex) {
    return (gSaveBuffer.menuData[0].coinScoreAges[fileIndex] >> (2 * courseIndex)) & 0x3;
}

static void set_coin_score_age(s32 fileIndex, s32 courseIndex, s32 age) {
    s32 mask = 0x3 << (2 * courseIndex);

    gSaveBuffer.menuData[0].coinScoreAges[fileIndex] &= ~mask;
    gSaveBuffer.menuData[0].coinScoreAges[fileIndex] |= age << (2 * courseIndex);
}

static void touch_coin_score_age(s32 fileIndex, s32 courseIndex) {
    s32 i;
    u32 age;
    u32 currentAge = get_coin_score_age(fileIndex, courseIndex);

    if (currentAge != 0) {
        for (i = 0; i < NUM_SAVE_FILES; i++) {
            age = get_coin_score_age(i, courseIndex);
            if (age < currentAge) {
                set_coin_score_age(i, courseIndex, age + 1);
            }
        }

        set_coin_score_age(fileIndex, courseIndex, 0);
        gMainMenuDataModified = TRUE;
    }
}

static void touch_high_score_ages(s32 fileIndex) {
    s32 i;

    for (i = COURSE_NUM_TO_INDEX(COURSE_MIN); i <= COURSE_NUM_TO_INDEX(COURSE_STAGES_MAX); i++) {
        touch_coin_score_age(fileIndex, i);
    }
}

static void restore_save_file_data(s32 fileIndex, s32 srcSlot) {
    s32 destSlot = srcSlot ^ 1;

    add_save_block_signature(&gSaveBuffer.files[fileIndex][srcSlot],
                             sizeof(gSaveBuffer.files[fileIndex][srcSlot]), SAVE_FILE_MAGIC);

    bcopy(&gSaveBuffer.files[fileIndex][srcSlot], &gSaveBuffer.files[fileIndex][destSlot],
          sizeof(gSaveBuffer.files[fileIndex][destSlot]));

    write_eeprom_data(&gSaveBuffer.files[fileIndex][destSlot],
                      sizeof(gSaveBuffer.files[fileIndex][destSlot]));
}

void save_file_do_save(s32 fileIndex) {
    if (gSaveFileModified) {

        add_save_block_signature(&gSaveBuffer.files[fileIndex][0],
                                 sizeof(gSaveBuffer.files[fileIndex][0]), SAVE_FILE_MAGIC);

        bcopy(&gSaveBuffer.files[fileIndex][0], &gSaveBuffer.files[fileIndex][1],
              sizeof(gSaveBuffer.files[fileIndex][1]));

        write_eeprom_data(gSaveBuffer.files[fileIndex], sizeof(gSaveBuffer.files[fileIndex]));

        gSaveFileModified = FALSE;
    }

    save_main_menu_data();
}

void save_file_erase(s32 fileIndex) {
    touch_high_score_ages(fileIndex);
    bzero(&gSaveBuffer.files[fileIndex][0], sizeof(gSaveBuffer.files[fileIndex][0]));

    gSaveFileModified = TRUE;
    save_file_do_save(fileIndex);
}

BAD_RETURN(s32) save_file_copy(s32 srcFileIndex, s32 destFileIndex) {
    UNUSED u8 filler[4];

    touch_high_score_ages(destFileIndex);
    bcopy(&gSaveBuffer.files[srcFileIndex][0], &gSaveBuffer.files[destFileIndex][0],
          sizeof(gSaveBuffer.files[destFileIndex][0]));

    gSaveFileModified = TRUE;
    save_file_do_save(destFileIndex);
}

void save_file_load_all(void) {
    s32 file;
    s32 validSlots;

    gMainMenuDataModified = FALSE;
    gSaveFileModified = FALSE;

    bzero(&gSaveBuffer, sizeof(gSaveBuffer));
    read_eeprom_data(&gSaveBuffer, sizeof(gSaveBuffer));

    validSlots = verify_save_block_signature(&gSaveBuffer.menuData[0], sizeof(gSaveBuffer.menuData[0]), MENU_DATA_MAGIC);
    validSlots |= verify_save_block_signature(&gSaveBuffer.menuData[1], sizeof(gSaveBuffer.menuData[1]),MENU_DATA_MAGIC) << 1;
    switch (validSlots) {
        case 0:
            wipe_main_menu_data();
            break;
        case 1:
            restore_main_menu_data(0);
            break;
        case 2:
            restore_main_menu_data(1);
            break;
    }

    for (file = 0; file < NUM_SAVE_FILES; file++) {

        validSlots = verify_save_block_signature(&gSaveBuffer.files[file][0], sizeof(gSaveBuffer.files[file][0]), SAVE_FILE_MAGIC);
        validSlots |= verify_save_block_signature(&gSaveBuffer.files[file][1], sizeof(gSaveBuffer.files[file][1]), SAVE_FILE_MAGIC) << 1;
        switch (validSlots) {
            case 0:
                save_file_erase(file);
                break;
            case 1:
                restore_save_file_data(file, 0);
                break;
            case 2:
                restore_save_file_data(file, 1);
                break;
        }
    }

    stub_save_file_1();
}

void save_file_reload(void) {

    bcopy(&gSaveBuffer.files[gCurrSaveFileNum - 1][1], &gSaveBuffer.files[gCurrSaveFileNum - 1][0],
          sizeof(gSaveBuffer.files[gCurrSaveFileNum - 1][0]));

    bcopy(&gSaveBuffer.menuData[1], &gSaveBuffer.menuData[0], sizeof(gSaveBuffer.menuData[0]));

    gMainMenuDataModified = FALSE;
    gSaveFileModified = FALSE;
}

void save_file_collect_star_or_key(s16 coinScore, s16 starIndex) {
    s32 fileIndex = gCurrSaveFileNum - 1;
    s32 courseIndex = COURSE_NUM_TO_INDEX(gCurrCourseNum);

    s32 starFlag = 1 << starIndex;
    UNUSED s32 flags = save_file_get_flags();

    gLastCompletedCourseNum = courseIndex + 1;
    gLastCompletedStarNum = starIndex + 1;
    sUnusedGotGlobalCoinHiScore = FALSE;
    gGotFileCoinHiScore = FALSE;

    if (courseIndex >= COURSE_NUM_TO_INDEX(COURSE_MIN)
        && courseIndex <= COURSE_NUM_TO_INDEX(COURSE_STAGES_MAX)) {

        if (coinScore > ((u16) save_file_get_max_coin_score(courseIndex) & 0xFFFF)) {
            sUnusedGotGlobalCoinHiScore = TRUE;
        }

        if (coinScore > save_file_get_course_coin_score(fileIndex, courseIndex)) {
            gSaveBuffer.files[fileIndex][0].courseCoinScores[courseIndex] = coinScore;
            touch_coin_score_age(fileIndex, courseIndex);

            gGotFileCoinHiScore = TRUE;
            gSaveFileModified = TRUE;
        }
    }

    switch (gCurrLevelNum) {
        case LEVEL_BOWSER_1:
            if (!(save_file_get_flags() & (SAVE_FLAG_HAVE_KEY_1 | SAVE_FLAG_UNLOCKED_BASEMENT_DOOR))) {
                save_file_set_flags(SAVE_FLAG_HAVE_KEY_1);
            }
            break;

        case LEVEL_BOWSER_2:
            if (!(save_file_get_flags() & (SAVE_FLAG_HAVE_KEY_2 | SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR))) {
                save_file_set_flags(SAVE_FLAG_HAVE_KEY_2);
            }
            break;

        case LEVEL_BOWSER_3:
            break;

        default:
            if (!(save_file_get_star_flags(fileIndex, courseIndex) & starFlag)) {
                save_file_set_star_flags(fileIndex, courseIndex, starFlag);
            }
            break;
    }

#ifdef TARGET_NDS

    {
        extern void nds_netplay_on_star_collected(s32 courseNum, s32 starIndex, s32 levelNum);
        nds_netplay_on_star_collected(gCurrCourseNum, starIndex, gCurrLevelNum);
    }
#endif
}

s32 save_file_exists(s32 fileIndex) {
    return (gSaveBuffer.files[fileIndex][0].flags & SAVE_FLAG_FILE_EXISTS) != 0;
}

u32 save_file_get_max_coin_score(s32 courseIndex) {
    s32 fileIndex;
    s32 maxCoinScore = -1;
    s32 maxScoreAge = -1;
    s32 maxScoreFileNum = 0;

    for (fileIndex = 0; fileIndex < NUM_SAVE_FILES; fileIndex++) {
        if (save_file_get_star_flags(fileIndex, courseIndex) != 0) {
            s32 coinScore = save_file_get_course_coin_score(fileIndex, courseIndex);
            s32 scoreAge = get_coin_score_age(fileIndex, courseIndex);

            if (coinScore > maxCoinScore || (coinScore == maxCoinScore && scoreAge > maxScoreAge)) {
                maxCoinScore = coinScore;
                maxScoreAge = scoreAge;
                maxScoreFileNum = fileIndex + 1;
            }
        }
    }
    return (maxScoreFileNum << 16) + max(maxCoinScore, 0);
}

s32 save_file_get_course_star_count(s32 fileIndex, s32 courseIndex) {
    s32 i;
    s32 count = 0;
    u8 flag = 1;
    u8 starFlags = save_file_get_star_flags(fileIndex, courseIndex);

    for (i = 0; i < 7; i++, flag <<= 1) {
        if (starFlags & flag) {
            count++;
        }
    }
    return count;
}

s32 save_file_get_total_star_count(s32 fileIndex, s32 minCourse, s32 maxCourse) {
    s32 count = 0;

    for (; minCourse <= maxCourse; minCourse++) {
        count += save_file_get_course_star_count(fileIndex, minCourse);
    }

    return save_file_get_course_star_count(fileIndex, COURSE_NUM_TO_INDEX(COURSE_NONE)) + count;
}

void save_file_set_flags(u32 flags) {
    gSaveBuffer.files[gCurrSaveFileNum - 1][0].flags |= (flags | SAVE_FLAG_FILE_EXISTS);
    gSaveFileModified = TRUE;
}

void save_file_clear_flags(u32 flags) {
    gSaveBuffer.files[gCurrSaveFileNum - 1][0].flags &= ~flags;
    gSaveBuffer.files[gCurrSaveFileNum - 1][0].flags |= SAVE_FLAG_FILE_EXISTS;
    gSaveFileModified = TRUE;
}

u32 save_file_get_flags(void) {
    if (gCurrCreditsEntry != NULL || gCurrDemoInput != NULL) {
        return 0;
    }
    return gSaveBuffer.files[gCurrSaveFileNum - 1][0].flags;
}

u32 save_file_get_star_flags(s32 fileIndex, s32 courseIndex) {
    u32 starFlags;

    if (courseIndex == COURSE_NUM_TO_INDEX(COURSE_NONE)) {
        starFlags = SAVE_FLAG_TO_STAR_FLAG(gSaveBuffer.files[fileIndex][0].flags);
    } else {
        starFlags = gSaveBuffer.files[fileIndex][0].courseStars[courseIndex] & 0x7F;
    }

    return starFlags;
}

void save_file_set_star_flags(s32 fileIndex, s32 courseIndex, u32 starFlags) {
    if (courseIndex == COURSE_NUM_TO_INDEX(COURSE_NONE)) {
        gSaveBuffer.files[fileIndex][0].flags |= STAR_FLAG_TO_SAVE_FLAG(starFlags);
    } else {
        gSaveBuffer.files[fileIndex][0].courseStars[courseIndex] |= starFlags;
    }

    gSaveBuffer.files[fileIndex][0].flags |= SAVE_FLAG_FILE_EXISTS;
    gSaveFileModified = TRUE;
}

s32 save_file_get_course_coin_score(s32 fileIndex, s32 courseIndex) {
    return gSaveBuffer.files[fileIndex][0].courseCoinScores[courseIndex];
}

s32 save_file_is_cannon_unlocked(void) {
    return (gSaveBuffer.files[gCurrSaveFileNum - 1][0].courseStars[gCurrCourseNum] & (1 << 7)) != 0;
}

void save_file_set_cannon_unlocked(void) {
    gSaveBuffer.files[gCurrSaveFileNum - 1][0].courseStars[gCurrCourseNum] |= (1 << 7);
    gSaveBuffer.files[gCurrSaveFileNum - 1][0].flags |= SAVE_FLAG_FILE_EXISTS;
    gSaveFileModified = TRUE;
}

void save_file_set_cap_pos(s16 x, s16 y, s16 z) {
    struct SaveFile *saveFile = &gSaveBuffer.files[gCurrSaveFileNum - 1][0];

    saveFile->capLevel = gCurrLevelNum;
    saveFile->capArea = gCurrAreaIndex;
    vec3s_set(saveFile->capPos, x, y, z);
    save_file_set_flags(SAVE_FLAG_CAP_ON_GROUND);
}

s32 save_file_get_cap_pos(Vec3s capPos) {
    struct SaveFile *saveFile = &gSaveBuffer.files[gCurrSaveFileNum - 1][0];
    s32 flags = save_file_get_flags();

    if (saveFile->capLevel == gCurrLevelNum && saveFile->capArea == gCurrAreaIndex
        && (flags & SAVE_FLAG_CAP_ON_GROUND)) {
        vec3s_copy(capPos, saveFile->capPos);
        return TRUE;
    }
    return FALSE;
}

void save_file_set_sound_mode(u16 mode) {
    set_sound_mode(mode);
    gSaveBuffer.menuData[0].soundMode = mode;

    gMainMenuDataModified = TRUE;
    save_main_menu_data();
}

u16 save_file_get_sound_mode(void) {
    return gSaveBuffer.menuData[0].soundMode;
}

void save_file_move_cap_to_default_location(void) {
    if (save_file_get_flags() & SAVE_FLAG_CAP_ON_GROUND) {
        switch (gSaveBuffer.files[gCurrSaveFileNum - 1][0].capLevel) {
            case LEVEL_SSL:
                save_file_set_flags(SAVE_FLAG_CAP_ON_KLEPTO);
                break;
            case LEVEL_SL:
                save_file_set_flags(SAVE_FLAG_CAP_ON_MR_BLIZZARD);
                break;
            case LEVEL_TTM:
                save_file_set_flags(SAVE_FLAG_CAP_ON_UKIKI);
                break;
        }
        save_file_clear_flags(SAVE_FLAG_CAP_ON_GROUND);
    }
}

#ifdef VERSION_EU
void eu_set_language(u16 language) {
    gSaveBuffer.menuData[0].language = language;
    gMainMenuDataModified = TRUE;
    save_main_menu_data();
}

u16 eu_get_language(void) {
    return gSaveBuffer.menuData[0].language;
}
#endif

void disable_warp_checkpoint(void) {

    gWarpCheckpoint.courseNum = COURSE_NONE;
}

void check_if_should_set_warp_checkpoint(struct WarpNode *warpNode) {
    if (warpNode->destLevel & 0x80) {

        gWarpCheckpoint.actNum = gCurrActNum;
        gWarpCheckpoint.courseNum = gCurrCourseNum;
        gWarpCheckpoint.levelID = warpNode->destLevel & 0x7F;
        gWarpCheckpoint.areaNum = warpNode->destArea;
        gWarpCheckpoint.warpNode = warpNode->destNode;
    }
}

s32 check_warp_checkpoint(struct WarpNode *warpNode) {
    s16 warpCheckpointActive = FALSE;
    s16 currCourseNum = gLevelToCourseNumTable[(warpNode->destLevel & 0x7F) - 1];

    if (gWarpCheckpoint.courseNum != COURSE_NONE && gSavedCourseNum == currCourseNum
        && gWarpCheckpoint.actNum == gCurrActNum) {
        warpNode->destLevel = gWarpCheckpoint.levelID;
        warpNode->destArea = gWarpCheckpoint.areaNum;
        warpNode->destNode = gWarpCheckpoint.warpNode;
        warpCheckpointActive = TRUE;
    } else {

        gWarpCheckpoint.courseNum = COURSE_NONE;
    }

    return warpCheckpointActive;
}
