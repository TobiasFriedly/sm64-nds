#include <ultra64.h>
#include <string.h>

#include "nds_netplay.h"
#include "nds_net.h"

#include "sm64.h"
#include "game/game_init.h"
#include "game/level_update.h"
#include "game/mario.h"
#include "game/memory.h"
#include "game/object_helpers.h"
#include "game/object_list_processor.h"
#include "game/interaction.h"
#include "game/area.h"
#include "engine/math_util.h"
#include "model_ids.h"
#include "behavior_data.h"
#include "object_constants.h"
#include "object_fields.h"
#include "game/save_file.h"
#include "course_table.h"
#include "level_table.h"

#include "nds_menu.h"

extern struct SaveBuffer gSaveBuffer;
extern struct GraphNode **gLoadedGraphNodes;
extern u8 gNdsModelLoaded[32];

void nds_debug_printf(const char *fmt, ...);

static s32 nds_model_is_loaded(s16 model) {
    if (model < 0 || model >= 256) {
        return 0;
    }
    return (gNdsModelLoaded[model >> 3] >> (model & 7)) & 1;
}

struct NetPlayerState {
    s16 posX, posY, posZ;
    s16 yaw;
    u32 action;
    s16 animID;
    s16 animFrame;
    u8 level;
    u8 area;
    u8 flags;
    u8 appModel;
    u8 appColor;
    s16 hitDirYaw;
    u8 hitForce;
    u8 hitSeq;
} __attribute__((packed));

volatile u8 gNdsNetPlayPuppetActive;
volatile u8 gNdsNetLoopback;
volatile u8 gNdsTestPickFile;

void nds_force_select_file(s32 fileNum);

#define FILE_CHUNK_DATA 13
static u8 sFileSendNum;
static u8 sFileSendGen;
static u16 sFileSendTimer;
static u8 sFileRecvNum;
static u8 sFileRecvGen;
static u8 sFileRecvMask;
static u8 sFileApplied;
static u8 sFileRecvBuf[72];
static u16 sFileForceTimer;
static void file_sync_update(void);

static u8 sStarSend[3];
static u16 sStarSendTimer;
static void star_sync_update(void);
volatile u32 gNdsNetPlayStatesSent;
volatile u32 gNdsNetPlayStatesApplied;

static struct DmaHandlerList sRemoteAnims;
static u8 sNetplayReady;

static struct Object *sPuppet;
static struct NetPlayerState sPeer;
static u8 sPeerValid;
static u32 sPeerAgeFrames;

static u32 sFrame;

static u8 sLocalHitSeq;
static s16 sLocalHitDirYaw;
static u8 sLocalHitForce;
static u8 sLocalHitTtl;
static u8 sWasAttacking;
static u8 sLastAppliedHitSeq;

extern u16 gAreaUpdateCounter;
extern const struct Animation *const goomba_seg8_anims_0801DA4C[];
extern const struct Animation *const bobomb_seg8_anims_0802396C[];
extern const struct Animation *const koopa_seg6_anims_06011364[];

static struct AnimInfo sLocalSkinAnim;

static const struct {
    s16 model;
    const struct Animation *const *table;
    u8 index;
} sSkinAnims[] = {
    { MODEL_GOOMBA,              goomba_seg8_anims_0801DA4C, 0 },
    { MODEL_BLACK_BOBOMB,        bobomb_seg8_anims_0802396C, 0 },
    { MODEL_BOBOMB_BUDDY,        bobomb_seg8_anims_0802396C, 0 },
    { MODEL_KOOPA_WITHOUT_SHELL, koopa_seg6_anims_06011364,  0 },
};

#define NDS_RAM_LO 0x02000000u
#define NDS_RAM_HI 0x02400000u

static s32 nds_resolve_skin_anim(s16 model, struct AnimInfo *out) {
    u32 i;
    for (i = 0; i < sizeof(sSkinAnims) / sizeof(sSkinAnims[0]); i++) {
        const struct Animation *const *tbl;
        struct Animation *anim;
        if (sSkinAnims[i].model != model) {
            continue;
        }
        tbl = segmented_to_virtual((const void *) sSkinAnims[i].table);
        if ((uintptr_t) tbl < NDS_RAM_LO || (uintptr_t) tbl >= NDS_RAM_HI) {
            return 0;
        }
        anim = segmented_to_virtual((const void *) tbl[sSkinAnims[i].index]);
        if ((uintptr_t) anim < NDS_RAM_LO || (uintptr_t) anim >= NDS_RAM_HI) {
            return 0;
        }

        if (anim->loopEnd <= 0 || anim->loopEnd > 4000 || anim->loopStart < 0
            || anim->loopStart >= anim->loopEnd
            || (uintptr_t) anim->values < 0x01000000 || (uintptr_t) anim->values >= 0x10000000
            || (uintptr_t) anim->index  < 0x01000000 || (uintptr_t) anim->index  >= 0x10000000) {
            return 0;
        }
        {
            u32 len = (u32) (anim->loopEnd - anim->loopStart);
            out->curAnim = anim;
            out->animID = sSkinAnims[i].index;
            out->animYTrans = anim->animYTransDivisor;
            out->animAccel = 0;
            out->animFrame = (s16) (anim->loopStart + (s16) ((gGlobalTimer >> 1) % len));
            out->animFrameAccelAssist = (s32) out->animFrame << 16;
            out->animTimer = gAreaUpdateCounter;
        }
        return 1;
    }
    return 0;
}

void nds_netplay_init(void) {
    void *mem = main_pool_alloc(0x4000, MEMORY_POOL_LEFT);
    if (mem == NULL) {
        nds_debug_printf("netplay: NO MEM\n");
        return;
    }
    setup_dma_table_list(&sRemoteAnims, gMarioAnims, mem);
    sNetplayReady = 1;
    nds_debug_printf("netplay: ready\n");
}

static s16 clamp_to_s16(f32 v) {
    if (v > 32000.0f) return 32000;
    if (v < -32000.0f) return -32000;
    return (s16) v;
}

static void send_own_state(void) {
    struct NetPlayerState st;
    struct MarioState *m = gMarioState;

    if (m == NULL || m->marioObj == NULL) {
        return;
    }
    st.posX = clamp_to_s16(m->pos[0]);
    st.posY = clamp_to_s16(m->pos[1]);
    st.posZ = clamp_to_s16(m->pos[2]);
    st.yaw = m->faceAngle[1];
    st.action = m->action;
    st.animID = m->marioObj->header.gfx.animInfo.animID;
    st.animFrame = m->marioObj->header.gfx.animInfo.animFrame;
    st.level = (u8) gCurrLevelNum;
    st.area = (u8) gCurrAreaIndex;
    st.flags = 0;

    st.appModel = gNdsLocalAppearanceModel;
    st.appColor = gNdsLocalAppearanceColor;

    if (sLocalHitTtl > 0) {
        sLocalHitTtl--;
        st.hitDirYaw = sLocalHitDirYaw;
        st.hitForce = sLocalHitForce;
    } else {
        st.hitDirYaw = 0;
        st.hitForce = 0;
    }
    st.hitSeq = sLocalHitSeq;

    nds_net_send_state(&st, sizeof(st));
    gNdsNetPlayStatesSent++;
}

static void puppet_despawn(void) {
    if (sPuppet != NULL) {

        if ((sPuppet->activeFlags & ACTIVE_FLAG_ACTIVE)
            && sPuppet->behavior == segmented_to_virtual(bhvNetPlayer)) {
            obj_mark_for_deletion(sPuppet);
        }
        sPuppet = NULL;
        gNdsNetPlayPuppetActive = 0;
    }
}

void nds_netplay_update(void) {
    struct NetPlayerState in;
    s32 age;

    if (!sNetplayReady) {
        return;
    }
    sFrame++;

    {
        static s16 sLastLevel = -1;
        static s16 sLastArea = -1;
        if (gCurrLevelNum != sLastLevel || gCurrAreaIndex != sLastArea) {
            sLastLevel = gCurrLevelNum;
            sLastArea = gCurrAreaIndex;
            sPuppet = NULL;
            gNdsNetPlayPuppetActive = 0;

            {
                s16 m = nds_skin_model(gNdsLocalAppearanceModel);
                if (m != MODEL_MARIO && !nds_model_is_loaded(m)) {
                    gNdsLocalAppearanceModel = 0;
                }
            }
        }
    }

    {
        static u16 sComboHold;
        if (gNdsNetState != NDS_NET_PAIRED && gPlayer1Controller != NULL
            && (gPlayer1Controller->buttonDown & (Z_TRIG | R_TRIG)) == (Z_TRIG | R_TRIG)) {
            if (++sComboHold == 60) {
                gNdsNetLoopback ^= 1;
                nds_debug_printf("netplay: loopback %d\n", gNdsNetLoopback);
            }
        } else {
            sComboHold = 0;
        }
    }

    if (gNdsTestPickFile >= 1 && gNdsTestPickFile <= 4) {
        s32 f = gNdsTestPickFile;
        gNdsTestPickFile = 0;
        nds_force_select_file(f);
        nds_netplay_host_picked(f);
    }

    if (gNdsNetLoopback && gMarioState != NULL && gMarioState->marioObj != NULL) {
        sPeer.posX = clamp_to_s16(gMarioState->pos[0] + 150.0f);
        sPeer.posY = clamp_to_s16(gMarioState->pos[1]);
        sPeer.posZ = clamp_to_s16(gMarioState->pos[2] + 80.0f);
        sPeer.yaw = gMarioState->faceAngle[1];
        sPeer.action = gMarioState->action;
        sPeer.animID = gMarioState->marioObj->header.gfx.animInfo.animID;
        sPeer.animFrame = gMarioState->marioObj->header.gfx.animInfo.animFrame;
        sPeer.level = (u8) gCurrLevelNum;
        sPeer.area = (u8) gCurrAreaIndex;
        sPeer.appModel = gNdsLocalAppearanceModel;
        sPeer.appColor = gNdsLocalAppearanceColor;
        sPeer.hitForce = 0;
        sPeerValid = 1;
        sPeerAgeFrames = 0;
        gNdsNetPlayStatesApplied++;
    } else if (gNdsNetState != NDS_NET_PAIRED) {
        puppet_despawn();
        sPeerValid = 0;
        return;
    }

    file_sync_update();

    star_sync_update();

    if (gMarioState != NULL && gMarioState->marioObj != NULL) {
        send_own_state();
    }

    if (sFrame % 600 == 0 && !gNdsMenuOpen) {
        nds_debug_printf("np: tx%lu rx%lu age%lu L%d/%d\n",
                         (unsigned long) gNdsNetPlayStatesSent,
                         (unsigned long) gNdsNetRxFrames,
                         (unsigned long) sPeerAgeFrames,
                         sPeerValid ? sPeer.level : -1, gCurrLevelNum);
        if (gNdsNetTxRejected != 0) {
            nds_debug_printf("net: txrej %lu\n", (unsigned long) gNdsNetTxRejected);
        }
    }

    age = nds_net_get_peer_state(&in, sizeof(in));
    if (age >= 0) {
        sPeer = in;
        sPeerValid = 1;
        sPeerAgeFrames = (u32) age;
        gNdsNetPlayStatesApplied++;

        if (!gNdsNetLoopback && sPeer.hitForce != 0 && sPeer.hitSeq != sLastAppliedHitSeq) {
            sLastAppliedHitSeq = sPeer.hitSeq;
            net_knockback_mario(gMarioState, sPeer.hitDirYaw, (f32) sPeer.hitForce);
        }
    } else if (sPeerValid) {
        sPeerAgeFrames++;
    }

    {
        s32 want = sPeerValid && sPeerAgeFrames < 120
                   && sPeer.level == (u8) gCurrLevelNum
                   && sPeer.area == (u8) gCurrAreaIndex
                   && gMarioObject != NULL;

        if (sPuppet != NULL && !(sPuppet->activeFlags & ACTIVE_FLAG_ACTIVE)) {
            sPuppet = NULL;
            gNdsNetPlayPuppetActive = 0;
        }

        if (want && sPuppet == NULL) {
            sPuppet = spawn_object(gMarioObject, MODEL_MARIO, bhvNetPlayer);
            nds_debug_printf("netplay: puppet %p\n", (void *) sPuppet);
            if (sPuppet != NULL) {
                sPuppet->oPosX = sPeer.posX;
                sPuppet->oPosY = sPeer.posY;
                sPuppet->oPosZ = sPeer.posZ;
                gNdsNetPlayPuppetActive = 1;
            }
        } else if (!want) {
            puppet_despawn();
        }
    }
}

static void net_player_set_anim(struct Object *o, s32 animID, s16 frame) {
    struct Animation *targetAnim = sRemoteAnims.bufTarget;

    if (animID < 0 || animID >= 209) {
        return;
    }
    if (load_patchable_table(&sRemoteAnims, animID)) {
        targetAnim->values = (void *) VIRTUAL_TO_PHYSICAL((u8 *) targetAnim + (uintptr_t) targetAnim->values);
        targetAnim->index = (void *) VIRTUAL_TO_PHYSICAL((u8 *) targetAnim + (uintptr_t) targetAnim->index);
    }
    if (o->header.gfx.animInfo.animID != animID) {
        o->header.gfx.animInfo.animID = animID;
        o->header.gfx.animInfo.curAnim = targetAnim;
        o->header.gfx.animInfo.animAccel = 0;
        o->header.gfx.animInfo.animYTrans = 0xBD;
    }
    if (frame >= 0 && frame < targetAnim->loopEnd) {
        o->header.gfx.animInfo.animFrame = frame;
    }
}

static struct ObjectHitbox sNetPlayerHitbox = {
     INTERACT_BOUNCE_TOP,
     0,
     0,
     0,
     0,
     60,
     110,
     50,
     90,
};

void bhv_net_player_loop(void) {
    struct Object *o = gCurrentObject;
    f32 dx, dy, dz;

    if (!sPeerValid) {
        return;
    }

    obj_set_hitbox(o, &sNetPlayerHitbox);
    o->oInteractStatus = 0;
    o->oIntangibleTimer = 0;

    dx = (f32) sPeer.posX - o->oPosX;
    dy = (f32) sPeer.posY - o->oPosY;
    dz = (f32) sPeer.posZ - o->oPosZ;

    if (dx * dx + dy * dy + dz * dz > 600.0f * 600.0f) {

        o->oPosX = sPeer.posX;
        o->oPosY = sPeer.posY;
        o->oPosZ = sPeer.posZ;
    } else {

        o->oPosX += dx * 0.34f;
        o->oPosY += dy * 0.34f;
        o->oPosZ += dz * 0.34f;
    }

    o->oFaceAngleYaw = sPeer.yaw;
    o->oMoveAngleYaw = sPeer.yaw;

    {
        s16 wantModel = nds_skin_model(sPeer.appModel);
        if (wantModel != MODEL_MARIO && !nds_model_is_loaded(wantModel)) {
            wantModel = MODEL_MARIO;
        }
        if (gLoadedGraphNodes[wantModel] != NULL
            && o->header.gfx.sharedChild != gLoadedGraphNodes[wantModel]) {
            o->header.gfx.sharedChild = gLoadedGraphNodes[wantModel];
        }
        if (wantModel == MODEL_MARIO) {
            net_player_set_anim(o, sPeer.animID, sPeer.animFrame);
        } else {

            o->oAnimState = 0;
            if (!nds_resolve_skin_anim(wantModel, &o->header.gfx.animInfo)) {
                o->header.gfx.animInfo.curAnim = NULL;
                o->header.gfx.animInfo.animID = -1;
            }
        }
    }

    if (gMarioState != NULL && gMarioState->marioObj != NULL) {
        s32 attack = 0;
        f32 mdx = gMarioState->pos[0] - o->oPosX;
        f32 mdy = gMarioState->pos[1] - o->oPosY;
        f32 mdz = gMarioState->pos[2] - o->oPosZ;
        if (mdx * mdx + mdz * mdz < 170.0f * 170.0f && mdy > -160.0f && mdy < 220.0f) {
            attack = net_classify_attack(gMarioState, o);
        }
        if (attack && !sWasAttacking) {
            sLocalHitDirYaw = atan2s(o->oPosZ - gMarioState->pos[2],
                                     o->oPosX - gMarioState->pos[0]);
            sLocalHitForce = (attack == 2) ? 36 : 24;
            sLocalHitSeq++;
            sLocalHitTtl = 6;
        }
        sWasAttacking = (u8) (attack != 0);
    }
}

s32 nds_object_render_appearance(struct Object *o, struct GraphNode **outGeo, u32 *outTint,
                                 struct AnimInfo **outAnim) {
    s32 flags = 0;

    if (o == gMarioObject) {
        s16 model = nds_skin_model(gNdsLocalAppearanceModel);
        if (model != MODEL_MARIO && nds_model_is_loaded(model)
            && gLoadedGraphNodes[model] != NULL) {
            *outGeo = gLoadedGraphNodes[model];
            flags |= 1;

            o->oAnimState = 0;
            if (nds_resolve_skin_anim(model, &sLocalSkinAnim)) {
                *outAnim = &sLocalSkinAnim;
                flags |= 8;
            } else {
                flags |= 2;
            }
        }
        if (gNdsLocalAppearanceColor != 0) {
            *outTint = nds_skin_color_tint(gNdsLocalAppearanceColor);
            flags |= 4;
        }
    } else if (o == sPuppet && sPeerValid) {
        if (sPeer.appColor != 0) {
            *outTint = nds_skin_color_tint(sPeer.appColor);
            flags |= 4;
        }
    }

    return flags;
}

void nds_make_120_star_file(s32 fileIndex) {
    struct SaveFile *file = &gSaveBuffer.files[fileIndex][0];
    s32 i;

    for (i = 0; i < COURSE_COUNT; i++) {
        file->courseStars[i] = 0;
    }
    for (i = 0; i < COURSE_STAGES_COUNT; i++) {
        file->courseCoinScores[i] = 0;
    }

    for (i = 0; i < COURSE_STAGES_COUNT; i++) {
        file->courseStars[i] |= 0x7F;
        file->courseCoinScores[i] = 100;
    }

    for (i = 0; i < COURSE_STAGES_COUNT; i++) {
        file->courseStars[i + 1] |= 0x80;
    }

    for (i = COURSE_BITDW - 1; i <= COURSE_SA - 1; i++) {
        file->courseStars[i] |= (i == COURSE_PSS - 1) ? 0x03 : 0x01;
    }

    file->flags = SAVE_FLAG_FILE_EXISTS
                 | SAVE_FLAG_HAVE_WING_CAP | SAVE_FLAG_HAVE_METAL_CAP | SAVE_FLAG_HAVE_VANISH_CAP
                 | SAVE_FLAG_HAVE_KEY_1 | SAVE_FLAG_HAVE_KEY_2
                 | SAVE_FLAG_UNLOCKED_BASEMENT_DOOR | SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR
                 | SAVE_FLAG_DDD_MOVED_BACK | SAVE_FLAG_MOAT_DRAINED
                 | SAVE_FLAG_UNLOCKED_PSS_DOOR | SAVE_FLAG_UNLOCKED_WF_DOOR
                 | SAVE_FLAG_UNLOCKED_CCM_DOOR | SAVE_FLAG_UNLOCKED_JRB_DOOR
                 | SAVE_FLAG_UNLOCKED_BITDW_DOOR | SAVE_FLAG_UNLOCKED_BITFS_DOOR
                 | SAVE_FLAG_UNLOCKED_50_STAR_DOOR
                 | SAVE_FLAG_COLLECTED_TOAD_STAR_1 | SAVE_FLAG_COLLECTED_TOAD_STAR_2
                 | SAVE_FLAG_COLLECTED_TOAD_STAR_3
                 | SAVE_FLAG_COLLECTED_MIPS_STAR_1 | SAVE_FLAG_COLLECTED_MIPS_STAR_2;

    gSaveFileModified = TRUE;
    gMainMenuDataModified = TRUE;
    save_file_do_save(fileIndex);
    nds_debug_printf("netplay: file %c = 120 stars\n", 'A' + fileIndex);
}

void nds_netplay_host_picked(s32 fileNum) {
    if (gNdsNetState == NDS_NET_PAIRED && gNdsNetIsHost) {
        sFileSendNum = (u8) fileNum;
        sFileSendGen++;
        sFileSendTimer = 3600;
        gNdsNetFileAckNum = 0;
        gNdsNetFileAckGen = 0;
        gNdsNetFileAckMask = 0;
        nds_debug_printf("net: sending file %c...\n", 'A' + fileNum - 1);
    }
}

s32 nds_netplay_block_local_pick(void) {
    return gNdsNetState == NDS_NET_PAIRED && !gNdsNetIsHost && !sFileApplied;
}

void nds_netplay_on_star_collected(s32 courseNum, s32 starIndex, s32 levelNum) {
    if (gNdsNetState != NDS_NET_PAIRED) {
        return;
    }
    sStarSend[0] = (u8) courseNum;
    sStarSend[1] = (u8) starIndex;
    sStarSend[2] = (u8) levelNum;
    sStarSendTimer = 90;
}

static void star_sync_update(void) {

    if (sStarSendTimer > 0) {
        sStarSendTimer--;
        if (sStarSendTimer % 6 == 0) {
            nds_net_send_star(sStarSend[0], sStarSend[1], sStarSend[2]);
        }
    }

    {
        u8 s[3];
        while (nds_net_get_star(s)) {
            u8 course = s[0], star = s[1], level = s[2];
            s32 fileIndex = gCurrSaveFileNum - 1;
            if (fileIndex < 0 || fileIndex > 3) {
                continue;
            }
            if (level == LEVEL_BOWSER_1) {
                save_file_set_flags(SAVE_FLAG_HAVE_KEY_1);
            } else if (level == LEVEL_BOWSER_2) {
                save_file_set_flags(SAVE_FLAG_HAVE_KEY_2);
            } else if (level != LEVEL_BOWSER_3) {
                save_file_set_star_flags(fileIndex, (s8) course - 1, 1 << star);
            }
            nds_debug_printf("net: peer star C%d S%d\n", course, star + 1);
        }
    }
}

static void file_sync_update(void) {
    u32 fileSize = sizeof(struct SaveFile);
    u32 chunks = (fileSize + FILE_CHUNK_DATA - 1) / FILE_CHUNK_DATA;
    u8 fullMask = (u8) ((1 << chunks) - 1);

    if (sFileSendNum >= 1 && sFileSendTimer > 0) {
        u8 acked = (gNdsNetFileAckNum == sFileSendNum && gNdsNetFileAckGen == sFileSendGen)
                       ? gNdsNetFileAckMask : 0;
        if ((acked & fullMask) == fullMask) {
            nds_debug_printf("net: file %c synced\n", 'A' + sFileSendNum - 1);
            sFileSendNum = 0;
            sFileSendTimer = 0;
        } else {
            sFileSendTimer--;
            if (sFileSendTimer % 4 == 0) {
                u8 payload[3 + FILE_CHUNK_DATA];
                const u8 *src = (const u8 *) &gSaveBuffer.files[sFileSendNum - 1][0];
                u32 c;
                for (c = 0; c < chunks; c++) {
                    u32 off = c * FILE_CHUNK_DATA;
                    u32 n = fileSize - off;
                    if (acked & (1 << c)) {
                        continue;
                    }
                    if (n > FILE_CHUNK_DATA) {
                        n = FILE_CHUNK_DATA;
                    }
                    payload[0] = sFileSendNum;
                    payload[1] = sFileSendGen;
                    payload[2] = (u8) c;
                    memcpy(&payload[3], src + off, n);
                    nds_net_send_file_chunk(payload, 3 + n);
                }
            }
        }
    }

    {
        u8 buf[3 + FILE_CHUNK_DATA];
        s32 len;
        s32 got = 0;
        while ((len = nds_net_get_file_chunk(buf, sizeof(buf))) > 3) {
            u8 fileNum = buf[0];
            u8 gen = buf[1];
            u8 c = buf[2];
            if (fileNum >= 1 && fileNum <= 4 && c < chunks) {
                u32 n = fileSize - (u32) c * FILE_CHUNK_DATA;
                if (n > FILE_CHUNK_DATA) {
                    n = FILE_CHUNK_DATA;
                }
                if (n > (u32) len - 3) {
                    n = (u32) len - 3;
                }
                if (fileNum != sFileRecvNum || gen != sFileRecvGen) {
                    sFileRecvNum = fileNum;
                    sFileRecvGen = gen;
                    sFileRecvMask = 0;
                    sFileApplied = 0;
                }
                memcpy(&sFileRecvBuf[c * FILE_CHUNK_DATA], &buf[3], n);
                sFileRecvMask |= (1 << c);
                got = 1;
                if (sFileRecvMask == fullMask && !gNdsNetIsHost && !sFileApplied) {

                    sFileApplied = 1;
                    memcpy(&gSaveBuffer.files[fileNum - 1][0], sFileRecvBuf, fileSize);
                    memcpy(&gSaveBuffer.files[fileNum - 1][1], sFileRecvBuf, fileSize);
                    sFileForceTimer = 3600;

                    nds_debug_printf("net: host picked file %c\n", 'A' + fileNum - 1);
                }
            }
        }
        if (got && !gNdsNetIsHost) {
            nds_net_send_file_ack(sFileRecvNum, sFileRecvGen, sFileRecvMask);
        }
    }

    if (sFileForceTimer > 0 && sFileRecvNum >= 1) {
        sFileForceTimer--;
        nds_force_select_file(sFileRecvNum);
    }
}
