
static s8 sCloudPartHeights[] = { 11, 8, 12, 8, 9, 9 };

static void cloud_act_spawn_parts(void) {
    struct Object *cloudPart;
    s32 i;

    for (i = 0; i <= 4; i++) {
        cloudPart = spawn_object_relative(i, 0, 0, 0, o, MODEL_MIST, bhvCloudPart);

        if (cloudPart != NULL) {
            obj_set_billboard(cloudPart);
        }
    }

    if (o->oBhvParams2ndByte == CLOUD_BP_FWOOSH) {

        spawn_object_relative(5, 0, 0, 0, o, MODEL_FWOOSH, bhvCloudPart);

        cur_obj_scale(3.0f);

        o->oCloudCenterX = o->oPosX;
        o->oCloudCenterY = o->oPosY;
    }

    o->oAction = CLOUD_ACT_MAIN;
}

static void cloud_act_fwoosh_hidden(void) {
    if (o->oDistanceToMario < 2000.0f) {
        cur_obj_unhide();
        o->oAction = CLOUD_ACT_SPAWN_PARTS;
    }
}

static void cloud_fwoosh_update(void) {
    if (o->oDistanceToMario > 2500.0f) {
        o->oAction = CLOUD_ACT_UNLOAD;
    } else {
        if (o->oCloudBlowing) {
            o->header.gfx.scale[0] += o->oCloudGrowSpeed;

            if ((o->oCloudGrowSpeed -= 0.005f) < -0.16f) {

                o->oCloudBlowing = o->oTimer = 0;
            } else if (o->oCloudGrowSpeed < -0.1f) {

                cur_obj_play_sound_1(SOUND_AIR_BLOW_WIND);
                cur_obj_spawn_strong_wind_particles(12, 3.0f, 0.0f, -50.0f, 120.0f);
            } else {
                cur_obj_play_sound_1(SOUND_ENV_WIND1);
            }
        } else {

            approach_f32_ptr(&o->header.gfx.scale[0], 3.0f, 0.012f);
            o->oCloudFwooshMovementRadius += 0xC8;

            if (o->oDistanceToMario < 1000.0f) {
                if (o->oTimer > 100) {
                    o->oCloudBlowing = TRUE;
                    o->oCloudGrowSpeed = 0.14f;
                }
            } else {
                o->oTimer = 0;
            }

            o->oCloudCenterX = o->oHomeX + 100.0f * coss(o->oCloudFwooshMovementRadius);
            o->oPosZ = o->oHomeZ + 100.0f * sins(o->oCloudFwooshMovementRadius);
            o->oCloudCenterY = o->oHomeY;
        }

        cur_obj_scale(o->header.gfx.scale[0]);
    }
}

static void cloud_act_main(void) {
    s16 localOffsetPhase = 0x800 * gGlobalTimer;
    f32 localOffset;

    if (o->parentObj != o) {

        if (o->parentObj->activeFlags == ACTIVE_FLAG_DEACTIVATED) {
            o->oAction = CLOUD_ACT_UNLOAD;
        } else {
            o->oCloudCenterX = o->parentObj->oPosX;
            o->oCloudCenterY = o->parentObj->oPosY;
            o->oPosZ = o->parentObj->oPosZ;

            o->oMoveAngleYaw = o->parentObj->oFaceAngleYaw;
        }
    } else if (o->oBhvParams2ndByte != CLOUD_BP_FWOOSH) {

        if (o->oDistanceToMario > 1500.0f) {
            o->oAction = CLOUD_ACT_UNLOAD;
        }
    } else {
        cloud_fwoosh_update();
    }

    localOffset = 2 * coss(localOffsetPhase) * o->header.gfx.scale[0];

    o->oPosX = o->oCloudCenterX + localOffset;
    o->oPosY = o->oCloudCenterY + localOffset + 12.0f * o->header.gfx.scale[0];
}

static void cloud_act_unload(void) {
    if (o->oBhvParams2ndByte != CLOUD_BP_FWOOSH) {
        obj_mark_for_deletion(o);
    } else {
        o->oAction = CLOUD_ACT_FWOOSH_HIDDEN;
        cur_obj_hide();
        cur_obj_set_pos_to_home();
    }
}

void bhv_cloud_update(void) {
    switch (o->oAction) {
        case CLOUD_ACT_SPAWN_PARTS:
            cloud_act_spawn_parts();
            break;
        case CLOUD_ACT_MAIN:
            cloud_act_main();
            break;
        case CLOUD_ACT_UNLOAD:
            cloud_act_unload();
            break;
        case CLOUD_ACT_FWOOSH_HIDDEN:
            cloud_act_fwoosh_hidden();
            break;
    }
}

void bhv_cloud_part_update(void) {
    if (o->parentObj->oAction == CLOUD_ACT_UNLOAD) {
        obj_mark_for_deletion(o);
    } else {
        f32 scale = 2.0f / 3.0f * o->parentObj->header.gfx.scale[0];
        s16 angleFromCenter = o->parentObj->oFaceAngleYaw + 0x10000 / 5 * o->oBhvParams2ndByte;

        s16 localOffsetPhase = 0x800 * gGlobalTimer + 0x4000 * o->oBhvParams2ndByte;
        f32 localOffset;

        f32 cloudRadius;

        cur_obj_scale(scale);

        if (o->oBhvParams2ndByte == 5 && scale > 2.0f) {
            scale = o->header.gfx.scale[1] = 2.0f;
        }

        localOffset = 2 * coss(localOffsetPhase) * scale;

        cloudRadius = 25.0f * scale;

        o->oPosX = o->parentObj->oCloudCenterX + cloudRadius * sins(angleFromCenter) + localOffset;

        o->oPosY =
            o->parentObj->oCloudCenterY + localOffset + scale * sCloudPartHeights[o->oBhvParams2ndByte];

        o->oPosZ = o->parentObj->oPosZ + cloudRadius * coss(angleFromCenter) + localOffset;

        o->oFaceAngleYaw = o->parentObj->oFaceAngleYaw;
    }
}
