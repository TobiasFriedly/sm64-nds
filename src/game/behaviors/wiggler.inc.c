
static struct ObjectHitbox sWigglerBodyPartHitbox = {
     INTERACT_BOUNCE_TOP,
     0,
     3,
     99,
     0,
     20,
     20,
     20,
     10,
};

static struct ObjectHitbox sWigglerHitbox = {
     INTERACT_BOUNCE_TOP,
     0,
     3,
     4,
     0,
     60,
     50,
     30,
     40,
};

static u8 sWigglerAttackHandlers[] = {
     ATTACK_HANDLER_KNOCKBACK,
     ATTACK_HANDLER_KNOCKBACK,
     ATTACK_HANDLER_SPECIAL_WIGGLER_JUMPED_ON,
     ATTACK_HANDLER_SPECIAL_WIGGLER_JUMPED_ON,
     ATTACK_HANDLER_KNOCKBACK,
     ATTACK_HANDLER_KNOCKBACK,
};

static f32 sWigglerSpeeds[] = { 2.0f, 40.0f, 30.0f, 16.0f };

void bhv_wiggler_body_part_update(void) {
    f32 dx;
    f32 dy;
    f32 dz;
    f32 dxz;
    struct ChainSegment *segment = &o->parentObj->oWigglerSegments[o->oBhvParams2ndByte];
    f32 posOffset;

    cur_obj_scale(o->parentObj->header.gfx.scale[0]);

    o->oFaceAnglePitch = segment->pitch;
    o->oFaceAngleYaw = segment->yaw;

    posOffset = -37.5f * o->header.gfx.scale[0];
    dy = posOffset * coss(o->oFaceAnglePitch) - posOffset;
    dxz = posOffset * sins(o->oFaceAnglePitch);
    dx = dxz * sins(o->oFaceAngleYaw);
    dz = dxz * coss(o->oFaceAngleYaw);

    o->oPosX = segment->posX + dx;
    o->oPosY = segment->posY + dy;
    o->oPosZ = segment->posZ + dz;

    if (o->oPosY < o->parentObj->oWigglerFallThroughFloorsHeight) {

        o->oPosY += -30.0f;
        cur_obj_update_floor_height();
        if (o->oFloorHeight > o->oPosY) {
            o->oPosY = o->oFloorHeight;
        }
    }

    segment->posY = o->oPosY;

    cur_obj_init_animation_with_accel_and_sound(0, o->parentObj->oWigglerWalkAnimSpeed);
    if (o->parentObj->oWigglerWalkAnimSpeed == 0.0f) {
        cur_obj_reverse_animation();
    }

    if (o->parentObj->oAction == WIGGLER_ACT_SHRINK) {
        cur_obj_become_intangible();
    } else {
        obj_check_attacks(&sWigglerBodyPartHitbox, o->oAction);
    }
}

void wiggler_init_segments(void) {
    s32 i;
    struct ChainSegment *segments = mem_pool_alloc(gObjectMemoryPool, 4 * sizeof(struct ChainSegment));

    if (segments != NULL) {

        o->oWigglerSegments = segments;
        for (i = 0; i <= 3; i++) {
            chain_segment_init(segments + i);

            (segments + i)->posX = o->oPosX;
            (segments + i)->posY = o->oPosY;
            (segments + i)->posZ = o->oPosZ;

            (segments + i)->pitch = o->oFaceAnglePitch;
            (segments + i)->yaw = o->oFaceAngleYaw;
        }

        o->header.gfx.animInfo.animFrame = -1;

        for (i = 1; i <= 3; i++) {
            struct Object *bodyPart =
                spawn_object_relative(i, 0, 0, 0, o, MODEL_WIGGLER_BODY, bhvWigglerBody);
            if (bodyPart != NULL) {
                obj_init_animation_with_sound(bodyPart, wiggler_seg5_anims_0500C874, 0);
                bodyPart->header.gfx.animInfo.animFrame = (23 * i) % 26 - 1;
            }
        }

        o->oAction = WIGGLER_ACT_WALK;
        cur_obj_unhide();
    }

#if defined(VERSION_EU) || defined(AVOID_UB)
    o->oHealth = 4;
#endif
}

 void wiggler_update_segments(void) {
    struct ChainSegment *prevBodyPart;
    struct ChainSegment *bodyPart;
    f32 dx;
    f32 dy;
    f32 dz;
    s16 dpitch;
    s16 dyaw;
    f32 dxz;
    s32 i;
    f32 segmentLength = 35.0f * o->header.gfx.scale[0];

    for (i = 1; i <= 3; i++) {
        prevBodyPart = &o->oWigglerSegments[i - 1];
        bodyPart = &o->oWigglerSegments[i];

        dx = bodyPart->posX - prevBodyPart->posX;
        dy = bodyPart->posY - prevBodyPart->posY;
        dz = bodyPart->posZ - prevBodyPart->posZ;

        dyaw = atan2s(-dz, -dx) - prevBodyPart->yaw;
        clamp_s16(&dyaw, -0x2000, 0x2000);
        bodyPart->yaw = prevBodyPart->yaw + dyaw;

        dxz = sqrtf(dx * dx + dz * dz);
        dpitch = atan2s(dxz, dy) - prevBodyPart->pitch;
        clamp_s16(&dpitch, -0x2000, 0x2000);
        bodyPart->pitch = prevBodyPart->pitch + dpitch;

        bodyPart->posY = segmentLength * sins(bodyPart->pitch) + prevBodyPart->posY;
        dxz = segmentLength * coss(bodyPart->pitch);
        bodyPart->posX = prevBodyPart->posX - dxz * sins(bodyPart->yaw);
        bodyPart->posZ = prevBodyPart->posZ - dxz * coss(bodyPart->yaw);
    }
}

static void wiggler_act_walk(void) {
    s16 yawTurnSpeed;

    o->oWigglerWalkAnimSpeed = 0.06f * o->oForwardVel;

    if (o->oWigglerTextStatus < WIGGLER_TEXT_STATUS_COMPLETED_DIALOG) {
        if (o->oWigglerTextStatus == WIGGLER_TEXT_STATUS_AWAIT_DIALOG) {
            seq_player_lower_volume(SEQ_PLAYER_LEVEL, 60, 40);
            o->oWigglerTextStatus = WIGGLER_TEXT_STATUS_SHOWING_DIALOG;
        }

        if (gMarioObject->oPosY < o->oPosY || cur_obj_update_dialog_with_cutscene(
            MARIO_DIALOG_LOOK_UP, DIALOG_FLAG_NONE, CUTSCENE_DIALOG, DIALOG_150)) {
            o->oWigglerTextStatus = WIGGLER_TEXT_STATUS_COMPLETED_DIALOG;
        }
    } else {

        obj_forward_vel_approach(sWigglerSpeeds[o->oHealth - 1], 1.0f);

        if (o->oWigglerWalkAwayFromWallTimer != 0) {
            o->oWigglerWalkAwayFromWallTimer--;
        } else {
            if (o->oDistanceToMario >= 25000.0f) {

                o->oWigglerTargetYaw = o->oAngleToMario;
            }

            if (obj_bounce_off_walls_edges_objects(&o->oWigglerTargetYaw)) {

                o->oWigglerWalkAwayFromWallTimer = random_linear_offset(30, 30);
            } else {
                if (o->oHealth < 4) {
                    o->oWigglerTargetYaw = o->oAngleToMario;
                } else if (o->oWigglerTimeUntilRandomTurn != 0) {
                    o->oWigglerTimeUntilRandomTurn--;
                } else {
                    o->oWigglerTargetYaw = o->oMoveAngleYaw + 0x4000 * (s16) random_sign();
                    o->oWigglerTimeUntilRandomTurn = random_linear_offset(30, 50);
                }
            }
        }

        yawTurnSpeed = (s16)(30.0f * o->oForwardVel);
        cur_obj_rotate_yaw_toward(o->oWigglerTargetYaw, yawTurnSpeed);
        obj_face_yaw_approach(o->oMoveAngleYaw, 2 * yawTurnSpeed);

        obj_face_pitch_approach(0, 0x320);

        if (o->oTimer < 60) {
            obj_check_attacks(&sWigglerHitbox, o->oAction);
        } else if (obj_handle_attacks(&sWigglerHitbox, o->oAction, sWigglerAttackHandlers)) {
            if (o->oAction != WIGGLER_ACT_JUMPED_ON) {
                o->oAction = WIGGLER_ACT_KNOCKBACK;
            }

            o->oWigglerWalkAwayFromWallTimer = 0;
            o->oWigglerWalkAnimSpeed = 0.0f;
        }
    }
}

static void wiggler_act_jumped_on(void) {

    s32 attackText[3] = { DIALOG_152, DIALOG_168, DIALOG_151 };

    if (approach_f32_ptr(&o->oWigglerSquishSpeed, 0.0f, 0.05f)) {

        approach_f32_ptr(&o->header.gfx.scale[1], 4.0f, 0.2f);
    } else {
        o->header.gfx.scale[1] -= o->oWigglerSquishSpeed;
    }

    if (o->header.gfx.scale[1] >= 4.0f) {
        if (o->oTimer > 30) {
            if (cur_obj_update_dialog_with_cutscene(MARIO_DIALOG_LOOK_UP,
                DIALOG_FLAG_NONE, CUTSCENE_DIALOG, attackText[o->oHealth - 2])) {

                if (--o->oHealth == 1) {
                    o->oAction = WIGGLER_ACT_SHRINK;
                    cur_obj_become_intangible();
                } else {
                    o->oAction = WIGGLER_ACT_WALK;
                    o->oMoveAngleYaw = o->oFaceAngleYaw;

                    if (o->oHealth == 2) {
                        cur_obj_play_sound_2(SOUND_OBJ_WIGGLER_JUMP);
                        o->oForwardVel = 10.0f;
                        o->oVelY = 70.0f;
                    }
                }
            }
        }
    } else {
        o->oTimer = 0;
    }

    obj_check_attacks(&sWigglerHitbox, o->oAction);
}

static void wiggler_act_knockback(void) {
    if (o->oVelY > 0.0f) {
        o->oFaceAnglePitch -= o->oVelY * 30.0f;
    } else {
        obj_face_pitch_approach(0, 0x190);
    }

    if (obj_forward_vel_approach(0.0f, 1.0f) && o->oFaceAnglePitch == 0) {
        o->oAction = WIGGLER_ACT_WALK;
        o->oMoveAngleYaw = o->oFaceAngleYaw;
    }

    obj_check_attacks(&sWigglerHitbox, o->oAction);
}

static void wiggler_act_shrink(void) {
    if (o->oTimer >= 20) {
        if (o->oTimer == 20) {
            cur_obj_play_sound_2(SOUND_OBJ_ENEMY_DEFEAT_SHRINK);
        }

        if (approach_f32_ptr(&o->header.gfx.scale[0], 1.0f, 0.1f)) {
            spawn_default_star(0.0f, 2048.0f, 0.0f);
            o->oAction = WIGGLER_ACT_FALL_THROUGH_FLOOR;
        }

        cur_obj_scale(o->header.gfx.scale[0]);
    }
}

static void wiggler_act_fall_through_floor(void) {
    if (o->oTimer == 60) {
        stop_background_music(SEQUENCE_ARGS(4, SEQ_EVENT_BOSS));
        o->oWigglerFallThroughFloorsHeight = 1700.0f;
    } else if (o->oTimer > 60) {
        if (o->oPosY < o->oWigglerFallThroughFloorsHeight) {
            o->oAction = WIGGLER_ACT_WALK;
        } else {
            o->oFaceAnglePitch = obj_get_pitch_from_vel();
        }

        cur_obj_move_using_fvel_and_gravity();
    }
}

void wiggler_jumped_on_attack_handler(void) {
    cur_obj_play_sound_2(SOUND_OBJ_WIGGLER_ATTACKED);
    o->oAction = WIGGLER_ACT_JUMPED_ON;
    o->oForwardVel = o->oVelY = 0.0f;
    o->oWigglerSquishSpeed = 0.4f;
}

void bhv_wiggler_update(void) {

    if (o->oAction == WIGGLER_ACT_UNINITIALIZED) {
        wiggler_init_segments();
    } else {
        if (o->oAction == WIGGLER_ACT_FALL_THROUGH_FLOOR) {
            wiggler_act_fall_through_floor();
        } else {
            treat_far_home_as_mario(1200.0f);

            cur_obj_init_animation_with_accel_and_sound(0, o->oWigglerWalkAnimSpeed);
            if (o->oWigglerWalkAnimSpeed != 0.0f) {
                cur_obj_play_sound_at_anim_range(0, 13,
                              o->oHealth >= 4 ? SOUND_OBJ_WIGGLER_LOW_PITCH : SOUND_OBJ_WIGGLER_HIGH_PITCH);
            } else {
                cur_obj_reverse_animation();
            }

            cur_obj_update_floor_and_walls();
            switch (o->oAction) {
                case WIGGLER_ACT_WALK:
                    wiggler_act_walk();
                    break;
                case WIGGLER_ACT_KNOCKBACK:
                    wiggler_act_knockback();
                    break;
                case WIGGLER_ACT_JUMPED_ON:
                    wiggler_act_jumped_on();
                    break;
                case WIGGLER_ACT_SHRINK:
                    wiggler_act_shrink();
                    break;
                case WIGGLER_ACT_FALL_THROUGH_FLOOR:
                    wiggler_act_fall_through_floor();
                    break;
            }

            cur_obj_move_standard(-78);
        }

        o->oWigglerSegments[0].posX = o->oPosX;
        o->oWigglerSegments[0].posY = o->oPosY;
        o->oWigglerSegments[0].posZ = o->oPosZ;
        o->oWigglerSegments[0].pitch = o->oFaceAnglePitch;
        o->oWigglerSegments[0].yaw = o->oFaceAngleYaw;

        wiggler_update_segments();
    }
}
