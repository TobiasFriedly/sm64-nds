#ifndef SAVE_FILE_H
#define SAVE_FILE_H

#include <PR/ultratypes.h>

#include "types.h"
#include "area.h"

#include "course_table.h"

#define EEPROM_SIZE 0x200
#define NUM_SAVE_FILES 4

struct SaveBlockSignature {
    u16 magic;
    u16 chksum;
};

struct SaveFile {

    u8 capLevel;
    u8 capArea;
    Vec3s capPos;

    u32 flags;

    u8 courseStars[COURSE_COUNT];

    u8 courseCoinScores[COURSE_STAGES_COUNT];

    struct SaveBlockSignature signature;
};

enum SaveFileIndex {
    SAVE_FILE_A,
    SAVE_FILE_B,
    SAVE_FILE_C,
    SAVE_FILE_D
};

struct MainMenuSaveData {

    u32 coinScoreAges[NUM_SAVE_FILES];
    u16 soundMode;

#ifdef VERSION_EU
    u16 language;
#define SUBTRAHEND 8
#else
#define SUBTRAHEND 6
#endif

    u8 filler[EEPROM_SIZE / 2 - SUBTRAHEND - NUM_SAVE_FILES * (4 + sizeof(struct SaveFile))];

    struct SaveBlockSignature signature;
};

struct SaveBuffer {

    struct SaveFile files[NUM_SAVE_FILES][2];

    struct MainMenuSaveData menuData[2];
};

extern u8 gLastCompletedCourseNum;
extern u8 gLastCompletedStarNum;
extern s8 sUnusedGotGlobalCoinHiScore;
extern u8 gGotFileCoinHiScore;
extern u8 gCurrCourseStarFlags;
extern u8 gSpecialTripleJump;
extern s8 gLevelToCourseNumTable[];

#define SAVE_FLAG_FILE_EXISTS             (1 << 0)
#define SAVE_FLAG_HAVE_WING_CAP           (1 << 1)
#define SAVE_FLAG_HAVE_METAL_CAP          (1 << 2)
#define SAVE_FLAG_HAVE_VANISH_CAP         (1 << 3)
#define SAVE_FLAG_HAVE_KEY_1              (1 << 4)
#define SAVE_FLAG_HAVE_KEY_2              (1 << 5)
#define SAVE_FLAG_UNLOCKED_BASEMENT_DOOR  (1 << 6)
#define SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR  (1 << 7)
#define SAVE_FLAG_DDD_MOVED_BACK          (1 << 8)
#define SAVE_FLAG_MOAT_DRAINED            (1 << 9)
#define SAVE_FLAG_UNLOCKED_PSS_DOOR       (1 << 10)
#define SAVE_FLAG_UNLOCKED_WF_DOOR        (1 << 11)
#define SAVE_FLAG_UNLOCKED_CCM_DOOR       (1 << 12)
#define SAVE_FLAG_UNLOCKED_JRB_DOOR       (1 << 13)
#define SAVE_FLAG_UNLOCKED_BITDW_DOOR     (1 << 14)
#define SAVE_FLAG_UNLOCKED_BITFS_DOOR     (1 << 15)
#define SAVE_FLAG_CAP_ON_GROUND           (1 << 16)
#define SAVE_FLAG_CAP_ON_KLEPTO           (1 << 17)
#define SAVE_FLAG_CAP_ON_UKIKI            (1 << 18)
#define SAVE_FLAG_CAP_ON_MR_BLIZZARD      (1 << 19)
#define SAVE_FLAG_UNLOCKED_50_STAR_DOOR   (1 << 20)
#define SAVE_FLAG_COLLECTED_TOAD_STAR_1   (1 << 24)
#define SAVE_FLAG_COLLECTED_TOAD_STAR_2   (1 << 25)
#define SAVE_FLAG_COLLECTED_TOAD_STAR_3   (1 << 26)
#define SAVE_FLAG_COLLECTED_MIPS_STAR_1   (1 << 27)
#define SAVE_FLAG_COLLECTED_MIPS_STAR_2   (1 << 28)

#define SAVE_FLAG_TO_STAR_FLAG(cmd) (((cmd) >> 24) & 0x7F)
#define STAR_FLAG_TO_SAVE_FLAG(cmd) ((cmd) << 24)

struct WarpCheckpoint {
     u8 actNum;
     u8 courseNum;
     u8 levelID;
     u8 areaNum;
     u8 warpNode;
};

extern struct WarpCheckpoint gWarpCheckpoint;

extern s8 gMainMenuDataModified;
extern s8 gSaveFileModified;

void save_file_do_save(s32 fileIndex);
void save_file_erase(s32 fileIndex);
BAD_RETURN(s32) save_file_copy(s32 srcFileIndex, s32 destFileIndex);
void save_file_load_all(void);
void save_file_reload(void);
void save_file_collect_star_or_key(s16 coinScore, s16 starIndex);
s32 save_file_exists(s32 fileIndex);
u32 save_file_get_max_coin_score(s32 courseIndex);
s32 save_file_get_course_star_count(s32 fileIndex, s32 courseIndex);
s32 save_file_get_total_star_count(s32 fileIndex, s32 minCourse, s32 maxCourse);
void save_file_set_flags(u32 flags);
void save_file_clear_flags(u32 flags);
u32 save_file_get_flags(void);
u32 save_file_get_star_flags(s32 fileIndex, s32 courseIndex);
void save_file_set_star_flags(s32 fileIndex, s32 courseIndex, u32 starFlags);
s32 save_file_get_course_coin_score(s32 fileIndex, s32 courseIndex);
s32 save_file_is_cannon_unlocked(void);
void save_file_set_cannon_unlocked(void);
void save_file_set_cap_pos(s16 x, s16 y, s16 z);
s32 save_file_get_cap_pos(Vec3s capPos);
void save_file_set_sound_mode(u16 mode);
u16 save_file_get_sound_mode(void);
void save_file_move_cap_to_default_location(void);

void disable_warp_checkpoint(void);
void check_if_should_set_warp_checkpoint(struct WarpNode *warpNode);
s32 check_warp_checkpoint(struct WarpNode *warpNode);

#ifdef VERSION_EU
enum EuLanguages {
    LANGUAGE_ENGLISH,
    LANGUAGE_FRENCH,
    LANGUAGE_GERMAN
};

void eu_set_language(u16 language);
u16 eu_get_language(void);
#endif

#endif
