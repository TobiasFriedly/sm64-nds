
void bowser_tail_anchor_act_default(void) {
    struct Object *bowser = o->parentObj;
    cur_obj_become_tangible();
    cur_obj_scale(1.0f);

    if (bowser->oAction == BOWSER_ACT_TILT_LAVA_PLATFORM) {

        bowser->oIntangibleTimer = -1;
    } else if (obj_check_if_collided_with_object(o, gMarioObject)) {

        bowser->oIntangibleTimer = 0;
        o->oAction = BOWSER_ACT_TAIL_TOUCHED_MARIO;
    } else {
        bowser->oIntangibleTimer = -1;
    }
}

void bowser_tail_anchor_thrown(void) {
    if (o->oTimer > 30) {
        o->oAction = BOWSER_ACT_TAIL_DEFAULT;
    }
}

void bowser_tail_anchor_act_touched_mario(void) {

    if (o->parentObj->oAction == BOWSER_ACT_TILT_LAVA_PLATFORM) {
        o->parentObj->oIntangibleTimer = -1;
        o->oAction = BOWSER_ACT_TAIL_DEFAULT;
    }
    cur_obj_become_intangible();
}

void (*sBowserTailAnchorActions[])(void) = {
    bowser_tail_anchor_act_default,
    bowser_tail_anchor_thrown,
    bowser_tail_anchor_act_touched_mario,
};

void bhv_bowser_tail_anchor_loop(void) {

    cur_obj_call_action_function(sBowserTailAnchorActions);

    o->oParentRelativePosX = 90.0f;

    if (o->parentObj->oAction == BOWSER_ACT_DEAD) {
        o->parentObj->oIntangibleTimer = -1;
    }

    o->oInteractStatus = 0;
}

void bhv_bowser_flame_spawn_loop(void) {
    struct Object *bowser = o->parentObj;
    s32 animFrame;
    f32 posX;
    f32 posZ;
    f32 cossYaw = coss(bowser->oMoveAngleYaw);
    f32 sinsYaw = sins(bowser->oMoveAngleYaw);
    s16 *data = segmented_to_virtual(dBowserFlamesOrientationValues);

    if (bowser->oSoundStateID == BOWSER_ANIM_BREATH) {

        animFrame = bowser->header.gfx.animInfo.animFrame + 1.0f;
        if (bowser->header.gfx.animInfo.curAnim->loopEnd == animFrame) {
            animFrame = 0;
        }

        if (animFrame > 45 && animFrame < 85) {
            cur_obj_play_sound_1(SOUND_AIR_BOWSER_SPIT_FIRE);
            posX = data[5 * animFrame];
            posZ = data[5 * animFrame + 2];
            o->oPosX = bowser->oPosX + (posZ * sinsYaw + posX * cossYaw);
            o->oPosY = bowser->oPosY + data[5 * animFrame + 1];
            o->oPosZ = bowser->oPosZ + (posZ * cossYaw - posX * sinsYaw);
            o->oMoveAnglePitch = data[5 * animFrame + 4] + 0xC00;
            o->oMoveAngleYaw = data[5 * animFrame + 3] + (s16) bowser->oMoveAngleYaw;

            if (!(animFrame & 1)) {
                spawn_object(o, MODEL_RED_FLAME, bhvFlameMovingForwardGrowing);
            }
        }
    }
}

void bhv_bowser_body_anchor_loop(void) {

    obj_copy_pos_and_angle(o, o->parentObj);

    if (o->parentObj->oAction == BOWSER_ACT_DEAD) {
#if BUGFIX_BOWSER_COLLIDE_BITS_DEAD

        if (o->parentObj->oSubAction == BOWSER_SUB_ACT_DEAD_FINAL_END_OVER) {
            o->oInteractType = 0;
        } else {
            o->oInteractType = INTERACT_TEXT;
        }
#else
        o->oInteractType = INTERACT_TEXT;
#endif
    } else {

        o->oInteractType = INTERACT_DAMAGE;

        if (o->parentObj->oOpacity < 100) {
            cur_obj_become_intangible();
        } else {
            cur_obj_become_tangible();
        }
    }

    if (o->parentObj->oHeldState != HELD_FREE) {
        cur_obj_become_intangible();
    }
    o->oInteractStatus = 0;
}

s32 bowser_spawn_shockwave(void) {
    if (o->oBhvParams2ndByte == BOWSER_BP_BITS) {
        struct Object *wave = spawn_object(o, MODEL_BOWSER_WAVE, bhvBowserShockWave);
        wave->oPosY = o->oFloorHeight;
        return TRUE;
    }
    return FALSE;
}

void bowser_bounce_effects(s32 *timer) {
    if (o->oMoveFlags & OBJ_MOVE_LANDED) {
        (*timer)++;
        if (*timer < 4) {
            cur_obj_start_cam_event(o, CAM_EVENT_BOWSER_THROW_BOUNCE);
            spawn_mist_particles_variable(0, 0, 60.0f);
            cur_obj_play_sound_2(SOUND_OBJ_BOWSER_WALK);
        }
    }
}

s32 bowser_set_anim_look_up_and_walk(void) {
    cur_obj_init_animation_with_sound(BOWSER_ANIM_LOOK_UP_START_WALK);
    if (cur_obj_check_anim_frame(21)) {
        o->oForwardVel = 3.0f;
    }
    if (cur_obj_check_if_near_animation_end()) {
        return TRUE;
    } else {
        return FALSE;
    }
}

s32 bowser_set_anim_slow_gait(void) {
    o->oForwardVel = 3.0f;
    cur_obj_init_animation_with_sound(BOWSER_ANIM_SLOW_GAIT);
    if (cur_obj_check_if_near_animation_end()) {
        return TRUE;
    } else {
        return FALSE;
    }
}

s32 bowser_set_anim_look_down_stop_walk(void) {
    cur_obj_init_animation_with_sound(BOWSER_ANIM_LOOK_DOWN_STOP_WALK);
    if (cur_obj_check_anim_frame(20)) {
        o->oForwardVel = 0.0f;
    }
    if (cur_obj_check_if_near_animation_end()) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void bowser_init_camera_actions(void) {
    if (o->oBowserCamAct == BOWSER_CAM_ACT_IDLE) {
        o->oAction = BOWSER_ACT_WAIT;
    } else if (o->oBowserCamAct == BOWSER_CAM_ACT_WALK) {
        o->oAction = BOWSER_ACT_INTRO_WALK;

    } else if (o->oBhvParams2ndByte == BOWSER_BP_BITFS) {
        o->oAction = BOWSER_ACT_BIG_JUMP;
    } else {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

void bowser_act_wait(void) {
    o->oForwardVel = 0.0f;
    cur_obj_init_animation_with_sound(BOWSER_ANIM_IDLE);
    bowser_init_camera_actions();
}

void bowser_act_intro_walk(void) {

    if (o->oSubAction == 0) {
        if (bowser_set_anim_look_up_and_walk()) {
            o->oSubAction++;
        }

    } else if (o->oSubAction == 1) {
        if (bowser_set_anim_slow_gait()) {
            o->oSubAction++;
        }

    } else if (bowser_set_anim_look_down_stop_walk()) {
        if (o->oBowserCamAct == BOWSER_CAM_ACT_WALK) {
            o->oBowserCamAct = BOWSER_CAM_ACT_IDLE;
        }
        bowser_init_camera_actions();
    }
}

s8 sBowserDebugActions[] = {
    BOWSER_ACT_CHARGE_MARIO,
    BOWSER_ACT_SPIT_FIRE_INTO_SKY,
    BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR,
    BOWSER_ACT_HIT_MINE,
    BOWSER_ACT_BIG_JUMP,
    BOWSER_ACT_WALK_TO_MARIO,
    BOWSER_ACT_BREATH_FIRE,
    BOWSER_ACT_DEAD,
    BOWSER_ACT_DANCE,
    BOWSER_ACT_TELEPORT,
    BOWSER_ACT_QUICK_JUMP,
    BOWSER_ACT_TILT_LAVA_PLATFORM,
    BOWSER_ACT_DANCE,
    BOWSER_ACT_DANCE,
    BOWSER_ACT_DANCE,
    BOWSER_ACT_DANCE,
};

UNUSED static void bowser_debug_actions(void) {
    if (gDebugInfo[DEBUG_PAGE_ENEMYINFO][1] != 0) {
        o->oAction = sBowserDebugActions[gDebugInfo[DEBUG_PAGE_ENEMYINFO][2] & 0x0F];
        gDebugInfo[DEBUG_PAGE_ENEMYINFO][1] = 0;
    }
}

void bowser_bitdw_actions(void) {

    f32 rand = random_float();

    if (!o->oBowserIsReacting) {
        if (o->oBowserStatus & BOWSER_STATUS_ANGLE_MARIO) {
            if (o->oDistanceToMario < 1500.0f) {
                o->oAction = BOWSER_ACT_BREATH_FIRE;
            } else {
                o->oAction = BOWSER_ACT_QUICK_JUMP;
            }
        } else {

            o->oAction = BOWSER_ACT_WALK_TO_MARIO;
        }
        o->oBowserIsReacting++;
    } else {
        o->oBowserIsReacting = FALSE;

#ifndef VERSION_JP
        if (gCurrDemoInput == NULL) {
#endif
            if (rand < 0.1) {
                o->oAction = BOWSER_ACT_DANCE;
            } else {
                o->oAction = BOWSER_ACT_WALK_TO_MARIO;
            }
#ifndef VERSION_JP
        } else {
            o->oAction = BOWSER_ACT_WALK_TO_MARIO;
        }
#endif
    }
}

void bowser_bitfs_actions(void) {

    f32 rand = random_float();

    if (!o->oBowserIsReacting) {
        if (o->oBowserStatus & BOWSER_STATUS_ANGLE_MARIO) {
            if (o->oDistanceToMario < 1300.0f) {
                if (rand < 0.5) {
                    o->oAction = BOWSER_ACT_TELEPORT;
                } else {
                    o->oAction = BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR;
                }
            } else {
                o->oAction = BOWSER_ACT_CHARGE_MARIO;
                if (500.0f < o->oBowserDistToCenter && o->oBowserDistToCenter < 1500.0f
                    && rand < 0.5) {
                    o->oAction = BOWSER_ACT_BIG_JUMP;
                }
            }
        } else {

            o->oAction = BOWSER_ACT_WALK_TO_MARIO;
        }
        o->oBowserIsReacting++;
    } else {

        o->oBowserIsReacting = FALSE;
        o->oAction = BOWSER_ACT_WALK_TO_MARIO;
    }
}

void bowser_bits_action_list(void) {
    f32 rand = random_float();
    if (o->oBowserStatus & BOWSER_STATUS_ANGLE_MARIO) {
        if (o->oDistanceToMario < 1000.0f) {
            if (rand < 0.4) {
                o->oAction = BOWSER_ACT_SPIT_FIRE_ONTO_FLOOR;
            } else if (rand < 0.8) {
                o->oAction = BOWSER_ACT_SPIT_FIRE_INTO_SKY;
            } else {
                o->oAction = BOWSER_ACT_BREATH_FIRE;
            }
        } else {
            if (rand < 0.5) {
                o->oAction = BOWSER_ACT_BIG_JUMP;
            } else {
                o->oAction = BOWSER_ACT_CHARGE_MARIO;
            }
        }
    } else {

        o->oAction = BOWSER_ACT_WALK_TO_MARIO;
    }
}

void bowser_set_act_big_jump(void) {
    o->oAction = BOWSER_ACT_BIG_JUMP;
}

void bowser_bits_actions(void) {
    switch (o->oBowserIsReacting) {
        case FALSE:

            if (o->oBowserBitSJustJump == FALSE) {
                bowser_bits_action_list();
            } else {
                bowser_set_act_big_jump();
            }
            o->oBowserIsReacting = TRUE;
            break;

        case TRUE:
            o->oBowserIsReacting = FALSE;
            o->oAction = BOWSER_ACT_WALK_TO_MARIO;
            break;
    }
}

#if BUGFIX_BOWSER_FALLEN_OFF_STAGE
void bowser_reset_fallen_off_stage(void) {
    if (o->oVelY < 0.0f && o->oPosY < (o->oHomeY - 300.0f)) {
        o->oPosX = o->oPosZ = 0.0f;
        o->oPosY = o->oHomeY + 2000.0f;
        o->oVelY = 0.0f;
        o->oForwardVel = 0.0f;
    }
}
#endif

void bowser_act_idle(void) {
    if (cur_obj_init_animation_and_check_if_near_end(BOWSER_ANIM_IDLE)) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

void bowser_act_default(void) {

    o->oBowserEyesShut = FALSE;

    cur_obj_init_animation_with_sound(BOWSER_ANIM_IDLE);

    o->oAngleVelYaw = 0;
    o->oForwardVel = 0.0f;
    o->oVelY = 0.0f;

    if (o->oBhvParams2ndByte == BOWSER_BP_BITDW) {
        bowser_bitdw_actions();
    } else if (o->oBhvParams2ndByte == BOWSER_BP_BITFS) {
        bowser_bitfs_actions();
    } else {
        bowser_bits_actions();
    }
}

void bowser_act_breath_fire(void) {
    o->oForwardVel = 0.0f;
    if (o->oTimer == 0) {
        cur_obj_play_sound_2(SOUND_OBJ_BOWSER_INHALING);
    }

    if (cur_obj_init_animation_and_check_if_near_end(BOWSER_ANIM_BREATH)) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

void bowser_act_walk_to_mario(void) {
    UNUSED s32 facing;
    s16 turnSpeed;
    s16 angleFromMario = abs_angle_diff(o->oMoveAngleYaw, o->oAngleToMario);

    if (o->oBhvParams2ndByte == BOWSER_BP_BITFS) {
        turnSpeed = 0x400;
    } else {
        if (o->oHealth >= 3) {
            turnSpeed = 0x400;
        } else if (o->oHealth == 2) {
            turnSpeed = 0x300;
        } else {
            turnSpeed = 0x200;
        }
    }

    facing = cur_obj_rotate_yaw_toward(o->oAngleToMario, turnSpeed);

    if (o->oSubAction == 0) {
        o->oBowserTimer = 0;

        if (bowser_set_anim_look_up_and_walk()) {
            o->oSubAction++;
        }
    } else if (o->oSubAction == 1) {

        if (bowser_set_anim_slow_gait()) {
            o->oBowserTimer++;

            if (o->oBowserStatus & BOWSER_STATUS_FIRE_SKY) {
                if (o->oBowserTimer > 4) {
                    o->oBowserStatus &= ~BOWSER_STATUS_FIRE_SKY;
                }

            } else if (angleFromMario < 0x2000) {
                o->oSubAction++;
            }
        }

    } else if (bowser_set_anim_look_down_stop_walk()) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

void bowser_act_teleport(void) {
    switch (o->oSubAction) {

        case BOWSER_SUB_ACT_TELEPORT_START:
            cur_obj_become_intangible();
            o->oBowserTargetOpacity = 0;
            o->oBowserTimer = 30;

            if (o->oTimer == 0) {
                cur_obj_play_sound_2(SOUND_OBJ2_BOWSER_TELEPORT);
            }

            if (o->oOpacity == 0) {
                o->oSubAction++;
                o->oMoveAngleYaw = o->oAngleToMario;
            }
            break;

        case BOWSER_SUB_ACT_TELEPORT_MOVE:

            if (o->oBowserTimer--) {
                o->oForwardVel = 100.0f;
            } else {
                o->oSubAction = BOWSER_SUB_ACT_TELEPORT_STOP;
                o->oMoveAngleYaw = o->oAngleToMario;
            }

            if (abs_angle_diff(o->oMoveAngleYaw, o->oAngleToMario) > 0x4000) {
                if (o->oDistanceToMario > 500.0f) {
                    o->oSubAction = BOWSER_SUB_ACT_TELEPORT_STOP;
                    o->oMoveAngleYaw = o->oAngleToMario;
                    cur_obj_play_sound_2(SOUND_OBJ2_BOWSER_TELEPORT);
                }
            }
            break;

        case BOWSER_SUB_ACT_TELEPORT_STOP:
            o->oForwardVel = 0.0f;
            o->oBowserTargetOpacity = 255;

            if (o->oOpacity == 255) {
                o->oAction = BOWSER_ACT_DEFAULT;
            }
            cur_obj_become_tangible();
            break;
    }
}

void bowser_act_spit_fire_into_sky(void) {
    s32 animFrame;

    cur_obj_init_animation_with_sound(BOWSER_ANIM_BREATH_UP);

    animFrame = o->header.gfx.animInfo.animFrame;

    if (animFrame > 24 && animFrame < 36) {
        cur_obj_play_sound_1(SOUND_AIR_BOWSER_SPIT_FIRE);
        if (animFrame == 35) {
            spawn_object_relative(1, 0, 400, 100, o, MODEL_RED_FLAME, bhvBlueBowserFlame);
        } else {
            spawn_object_relative(0, 0, 400, 100, o, MODEL_RED_FLAME, bhvBlueBowserFlame);
        }
    }

    if (cur_obj_check_if_near_animation_end()) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }

    o->oBowserStatus |= BOWSER_STATUS_FIRE_SKY;
}

void bowser_act_hit_mine(void) {

    if (o->oTimer == 0) {
        o->oForwardVel = -400.0f;
        o->oVelY = 100.0f;
        o->oMoveAngleYaw = o->oBowserAngleToCenter + 0x8000;
        o->oBowserEyesShut = TRUE;
    }

    if (o->oSubAction == BOWSER_SUB_ACT_HIT_MINE_START) {
        cur_obj_init_animation_with_sound(BOWSER_ANIM_FLIP);
        o->oSubAction++;
        o->oBowserTimer = 0;

    } else if (o->oSubAction == BOWSER_SUB_ACT_HIT_MINE_FALL) {
        cur_obj_init_animation_with_sound(BOWSER_ANIM_FLIP);
        cur_obj_extend_animation_if_at_end();
        bowser_bounce_effects(&o->oBowserTimer);

        if (o->oBowserTimer > 2) {
            cur_obj_init_animation_with_sound(BOWSER_ANIM_STAND_UP_FROM_FLIP);
            o->oVelY = 0.0f;
            o->oForwardVel = 0.0f;
            o->oSubAction++;
        }

    } else if (o->oSubAction == BOWSER_SUB_ACT_HIT_MINE_STOP) {
        if (cur_obj_check_if_near_animation_end()) {

            if (o->oHealth == 1) {
                o->oAction = BOWSER_ACT_DANCE;
            } else {
                o->oAction = BOWSER_ACT_DEFAULT;
            }
            o->oBowserEyesShut = FALSE;
        }
    } else {
    }
}

s32 bowser_set_anim_jump(void) {
    cur_obj_init_animation_with_sound(BOWSER_ANIM_JUMP_START);
    if (cur_obj_check_anim_frame(11)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

s32 bowser_land(void) {
    if (o->oMoveFlags & OBJ_MOVE_LANDED) {
        o->oForwardVel = 0.0f;
        o->oVelY = 0.0f;
        spawn_mist_particles_variable(0, 0, 60.0f);
        cur_obj_init_animation_with_sound(BOWSER_ANIM_JUMP_STOP);
        o->header.gfx.animInfo.animFrame = 0;
        cur_obj_start_cam_event(o, CAM_EVENT_BOWSER_JUMP);

        if (o->oBhvParams2ndByte == BOWSER_BP_BITDW) {
            if (o->oDistanceToMario < 850.0f) {
                gMarioObject->oInteractStatus |= INT_STATUS_MARIO_KNOCKBACK_DMG;
            } else {
                gMarioObject->oInteractStatus |= INT_STATUS_MARIO_STUNNED;
            }
        }
        return TRUE;
    } else {
        return FALSE;
    }
}

void bowser_short_second_hop(void) {
    if (o->oBhvParams2ndByte == BOWSER_BP_BITS && o->oBowserStatus & BOWSER_STATUS_BIG_JUMP) {
        if (o->oBowserDistToCenter > 1000.0f) {
            o->oForwardVel = 60.0f;
        }
    }
}

void bowser_act_big_jump(void) {
    UNUSED u8 filler[4];

    if (o->oSubAction == 0) {

        if (bowser_set_anim_jump()) {

            if (o->oBhvParams2ndByte == BOWSER_BP_BITS && o->oBowserStatus & BOWSER_STATUS_BIG_JUMP) {
                o->oVelY = 70.0f;
            } else {
                o->oVelY = 80.0f;
            }
            o->oBowserTimer = 0;
            bowser_short_second_hop();
            o->oSubAction++;
        }
    } else if (o->oSubAction == 1) {
#if BUGFIX_BOWSER_FALLEN_OFF_STAGE

        if (o->oBhvParams2ndByte == BOWSER_BP_BITS && o->oBowserStatus & BOWSER_STATUS_BIG_JUMP) {
            bowser_reset_fallen_off_stage();
        }
#endif

        if (bowser_land()) {
            o->oBowserStatus &= ~BOWSER_STATUS_BIG_JUMP;
            o->oForwardVel = 0.0f;
            o->oSubAction++;

            bowser_spawn_shockwave();

            if (o->oBhvParams2ndByte == BOWSER_BP_BITFS) {
                o->oAction = BOWSER_ACT_TILT_LAVA_PLATFORM;
            }
        } else {
        }

    } else if (cur_obj_check_if_near_animation_end()) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

s16 sBowserVelYAir[] = { 60 };
s16 sBowserFVelAir[] = { 50 };

void bowser_act_quick_jump(void) {
    f32 velY = sBowserVelYAir[0];
    f32 fVel = sBowserFVelAir[0];

    if (o->oSubAction == 0) {

        if (bowser_set_anim_jump()) {
            o->oVelY = velY;
            o->oForwardVel = fVel;
            o->oBowserTimer = 0;
            o->oSubAction++;
        }

    } else if (o->oSubAction == 1) {
        if (bowser_land()) {
            o->oSubAction++;
        }
    } else if (cur_obj_check_if_near_animation_end()) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

void bowser_act_hit_edge(void) {

    o->oForwardVel = 0.0f;
    if (o->oTimer == 0) {
        o->oBowserTimer = 0;
    }

    switch (o->oSubAction) {
        case 0:

            cur_obj_init_animation_with_sound(BOWSER_ANIM_EDGE_MOVE);
            if (cur_obj_check_if_near_animation_end()) {
                o->oBowserTimer++;
            }
            if (o->oBowserTimer > 0) {
                o->oSubAction++;
            }
            break;

        case 1:

            cur_obj_init_animation_with_sound(BOWSER_ANIM_EDGE_STOP);

            if (cur_obj_check_if_near_animation_end()) {
                o->oAction = BOWSER_ACT_TURN_FROM_EDGE;
            }
            break;
    }
}

void bowser_act_spit_fire_onto_floor(void) {

    if (gHudDisplay.wedges < 4) {
        o->oBowserRandSplitFloor = 3;
    } else {
        o->oBowserRandSplitFloor = random_float() * 3.0f + 1.0f;
    }

    cur_obj_init_animation_with_sound(BOWSER_ANIM_BREATH_QUICK);
    if (cur_obj_check_anim_frame(5)) {
        obj_spit_fire(0, 200, 180, 7.0f, MODEL_RED_FLAME, 30.0f, 10.0f, 0x1000);
    }

    if (cur_obj_check_if_near_animation_end()) {
        o->oSubAction++;
    }

    if (o->oSubAction >= o->oBowserRandSplitFloor) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

s32 bowser_turn_on_timer(s32 time, s16 yaw) {
    if (o->oSubAction == 0) {
        if (cur_obj_init_animation_and_check_if_near_end(BOWSER_ANIM_LOOK_UP_START_WALK)) {
            o->oSubAction++;
        }
    } else if (o->oSubAction == 1) {
        if (cur_obj_init_animation_and_check_if_near_end(BOWSER_ANIM_LOOK_DOWN_STOP_WALK)) {
            o->oSubAction++;
        }
    } else {
        cur_obj_init_animation_with_sound(BOWSER_ANIM_IDLE);
    }

    o->oForwardVel = 0.0f;
    o->oMoveAngleYaw += yaw;

    if (o->oTimer >= time) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void bowser_act_turn_from_edge(void) {
    if (bowser_turn_on_timer(63, 0x200)) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

void bowser_act_charge_mario(void) {
    s32 time;

    if (o->oTimer == 0) {
        o->oForwardVel = 0.0f;
    }

    switch (o->oSubAction) {
        case BOWSER_SUB_ACT_CHARGE_START:

            o->oBowserTimer = 0;
            if (cur_obj_init_animation_and_check_if_near_end(BOWSER_ANIM_RUN_START)) {
                o->oSubAction = BOWSER_SUB_ACT_CHARGE_RUN;
            }
            break;

        case BOWSER_SUB_ACT_CHARGE_RUN:

            o->oForwardVel = 50.0f;
            if (cur_obj_init_animation_and_check_if_near_end(BOWSER_ANIM_RUN)) {
                o->oBowserTimer++;

                if (o->oBowserTimer >= 6) {
                    o->oSubAction = BOWSER_SUB_ACT_CHARGE_SLIP;
                }

                if (o->oBowserTimer >= 2) {
                    if (abs_angle_diff(o->oAngleToMario, o->oMoveAngleYaw) > 0x2000) {
                        o->oSubAction = BOWSER_SUB_ACT_CHARGE_SLIP;
                    }
                }
            }

            cur_obj_rotate_yaw_toward(o->oAngleToMario, 0x200);
            break;

        case BOWSER_SUB_ACT_CHARGE_SLIP:

            o->oBowserTimer = 0;
            cur_obj_init_animation_with_sound(BOWSER_ANIM_RUN_SLIP);
            spawn_object_relative_with_scale(0, 100, -50, 0, 3.0f, o, MODEL_SMOKE, bhvWhitePuffSmoke2);
            spawn_object_relative_with_scale(0, -100, -50, 0, 3.0f, o, MODEL_SMOKE,
                                             bhvWhitePuffSmoke2);

            if (approach_f32_signed(&o->oForwardVel, 0, -1.0f)) {
                o->oSubAction = BOWSER_SUB_ACT_CHARGE_END;
            }
            cur_obj_extend_animation_if_at_end();
            break;

        case BOWSER_SUB_ACT_CHARGE_END:

            o->oForwardVel = 0.0f;
            cur_obj_init_animation_with_sound(BOWSER_ANIM_RUN_STOP);
            if (cur_obj_check_if_near_animation_end()) {

                if (o->oBhvParams2ndByte == BOWSER_BP_BITS) {
                    time = 10;
                } else {
                    time = 30;
                }
                if (o->oBowserTimer > time) {
                    o->oAction = BOWSER_ACT_DEFAULT;
                }
                o->oBowserTimer++;
            }
            cur_obj_extend_animation_if_at_end();
            break;
    }

    if (o->oMoveFlags & OBJ_MOVE_HIT_EDGE) {
        o->oAction = BOWSER_ACT_HIT_EDGE;
    }
}

s32 bowser_check_hit_mine(void) {
    struct Object *mine;
    f32 dist;

    mine = cur_obj_find_nearest_object_with_behavior(bhvBowserBomb, &dist);
    if (mine != NULL && dist < 800.0f) {
        mine->oInteractStatus |= INT_STATUS_HIT_MINE;
        return TRUE;
    }

    return FALSE;
}

void bowser_act_thrown(void) {
    UNUSED u8 filler[4];

    if (o->oTimer < 2) {
        o->oBowserTimer = 0;
    }
    if (o->oSubAction == 0) {

        cur_obj_init_animation_with_sound(BOWSER_ANIM_SHAKING);
        bowser_bounce_effects(&o->oBowserTimer);

        if (o->oMoveFlags & OBJ_MOVE_ON_GROUND) {
            o->oForwardVel = 0.0f;
            o->oSubAction++;
        }

    } else if (cur_obj_init_animation_and_check_if_near_end(BOWSER_ANIM_STAND_UP)) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }

    if (bowser_check_hit_mine()) {
        o->oHealth--;
        if (o->oHealth <= 0) {
            o->oAction = BOWSER_ACT_DEAD;
        } else {
            o->oAction = BOWSER_ACT_HIT_MINE;
        }
    }
}

void bowser_set_goal_invisible(void) {
    o->oBowserTargetOpacity = 0;
    if (o->oOpacity == 0) {
        o->oForwardVel = 0.0f;
        o->oVelY = 0.0f;
        o->oPosY = o->oHomeY - 1000.0f;
    }
}

void bowser_act_jump_onto_stage(void) {
    s32 onDynamicFloor;
    UNUSED u8 filler[4];
    struct Surface *floor = o->oFloor;

    if (floor != NULL && floor->flags & SURFACE_FLAG_DYNAMIC) {
        onDynamicFloor = TRUE;
    } else {
        onDynamicFloor = FALSE;
    }

    o->oBowserStatus |= BOWSER_STATUS_BIG_JUMP;

    switch (o->oSubAction) {

        case BOWSER_SUB_ACT_JUMP_ON_STAGE_IDLE:
            if (o->oTimer == 0) {
                o->oFaceAnglePitch = 0;
                o->oFaceAngleRoll = 0;
            }
            o->oFaceAnglePitch += 0x800;
            o->oFaceAngleRoll += 0x800;
            if (!(o->oFaceAnglePitch & 0xFFFF)) {
                o->oSubAction++;
            }
            bowser_set_goal_invisible();
            break;

        case BOWSER_SUB_ACT_JUMP_ON_STAGE_START:
            cur_obj_init_animation_with_sound(BOWSER_ANIM_JUMP_START);
            if (cur_obj_check_anim_frame(11)) {
                o->oMoveAngleYaw = o->oBowserAngleToCenter;
                o->oVelY = 150.0f;
                o->oBowserTargetOpacity = 255;
                o->oBowserTimer = 0;
                o->oSubAction++;
            } else {
                bowser_set_goal_invisible();
            }
            break;

        case BOWSER_SUB_ACT_JUMP_ON_STAGE_LAND:
            if (o->oPosY > o->oHomeY) {
                o->oDragStrength = 0.0f;
                if (o->oBowserDistToCenter < 2500.0f) {
                    if (absf(o->oFloorHeight - o->oHomeY) < 100.0f) {
                        approach_f32_signed(&o->oForwardVel, 0, -5.0f);
                    } else {
                        cur_obj_forward_vel_approach_upward(150.0f, 2.0f);
                    }
                } else {
                    cur_obj_forward_vel_approach_upward(150.0f, 2.0f);
                }
            }

            if (bowser_land()) {
                o->oDragStrength = 10.0f;
                o->oSubAction++;

                if (onDynamicFloor == FALSE) {
                    bowser_spawn_shockwave();

                } else if (o->oBhvParams2ndByte == BOWSER_BP_BITS) {
                    o->oAction = BOWSER_ACT_BIG_JUMP;
                }

                if (o->oBhvParams2ndByte == BOWSER_BP_BITFS) {
                    o->oAction = BOWSER_ACT_TILT_LAVA_PLATFORM;
                }
            }

#if BUGFIX_BOWSER_FALLEN_OFF_STAGE
            bowser_reset_fallen_off_stage();
#else
            if (o->oVelY < 0.0f && o->oPosY < o->oHomeY - 300.0f) {
                o->oPosZ = 0.0f, o->oPosX = o->oPosZ;
                o->oPosY = o->oHomeY + 2000.0f;
                o->oVelY = 0.0f;
            }
#endif
            break;

        case BOWSER_SUB_ACT_JUMP_ON_STAGE_STOP:
            if (cur_obj_check_if_near_animation_end()) {
                o->oAction = BOWSER_ACT_DEFAULT;
                o->oBowserStatus &= ~BOWSER_STATUS_BIG_JUMP;
                cur_obj_extend_animation_if_at_end();
            }
            break;
    }

    print_debug_bottom_up("sp %d", o->oForwardVel);
}

s8 sBowserDanceStepNoises[] = { 24, 42, 60, -1 };

void bowser_act_dance(void) {

    if (is_item_in_array(o->oTimer, sBowserDanceStepNoises)) {
        cur_obj_play_sound_2(SOUND_OBJ_BOWSER_WALK);
    }

    if (cur_obj_init_animation_and_check_if_near_end(BOWSER_ANIM_DANCE)) {
        o->oAction = BOWSER_ACT_DEFAULT;
    }
}

void bowser_spawn_collectable(void) {
    if (o->oBhvParams2ndByte == BOWSER_BP_BITS) {
        gSecondCameraFocus = spawn_object(o, MODEL_STAR, bhvGrandStar);
    } else {
        gSecondCameraFocus = spawn_object(o, MODEL_BOWSER_KEY, bhvBowserKey);
        cur_obj_play_sound_2(SOUND_GENERAL2_BOWSER_KEY);
    }
    gSecondCameraFocus->oAngleVelYaw = o->oAngleVelYaw;
}

void bowser_fly_back_dead(void) {
    cur_obj_init_animation_with_sound(BOWSER_ANIM_FLIP_DOWN);

    if (o->oBhvParams2ndByte == BOWSER_BP_BITS) {
        o->oForwardVel = -400.0f;
    } else {
        o->oForwardVel = -200.0f;
    }
    o->oVelY = 100.0f;
    o->oMoveAngleYaw = o->oBowserAngleToCenter + 0x8000;
    o->oBowserTimer = 0;
    o->oSubAction++;
}

void bowser_dead_bounce(void) {
    o->oBowserEyesShut = TRUE;
    bowser_bounce_effects(&o->oBowserTimer);
    if (o->oMoveFlags & OBJ_MOVE_LANDED) {
        cur_obj_play_sound_2(SOUND_OBJ_BOWSER_WALK);
    }
    if (o->oMoveFlags & OBJ_MOVE_ON_GROUND) {
        o->oForwardVel = 0.0f;
        o->oSubAction++;
    }
}

s32 bowser_dead_wait_for_mario(void) {
    s32 ret = FALSE;
    cur_obj_become_intangible();
    if (cur_obj_init_animation_and_check_if_near_end(BOWSER_ANIM_LAY_DOWN) && o->oDistanceToMario < 700.0f
        && abs_angle_diff(gMarioObject->oMoveAngleYaw, o->oAngleToMario) > 0x6000) {
        ret = TRUE;
    }
    cur_obj_extend_animation_if_at_end();
    o->oBowserTimer = 0;
    return ret;
}

s32 bowser_dead_twirl_up(void) {
    s32 ret = FALSE;

    if (o->header.gfx.scale[0] < 0.8) {
        o->oAngleVelYaw += 0x80;
    }

    if (o->header.gfx.scale[0] > 0.2) {
        o->header.gfx.scale[0] = o->header.gfx.scale[0] - 0.02;
        o->header.gfx.scale[2] = o->header.gfx.scale[2] - 0.02;
    } else {

        o->header.gfx.scale[1] = o->header.gfx.scale[1] - 0.01;
        o->oVelY = 20.0f;
        o->oGravity = 0.0f;
    }

    if (o->header.gfx.scale[1] < 0.5) {
        ret = TRUE;
    }

    o->oMoveAngleYaw += o->oAngleVelYaw;

    if (o->oOpacity >= 3) {
        o->oOpacity -= 2;
    }

    return ret;
}

void bowser_dead_hide(void) {
    cur_obj_scale(0);
    o->oForwardVel = 0;
    o->oVelY = 0;
    o->oGravity = 0;
}

s16 sBowserDefeatedDialogText[3] = { DIALOG_119, DIALOG_120, DIALOG_121 };

s32 bowser_dead_default_stage_ending(void) {
    s32 ret = FALSE;

    if (o->oBowserTimer < 2) {

        if (o->oBowserTimer == 0) {
            seq_player_lower_volume(SEQ_PLAYER_LEVEL, 60, 40);
            o->oBowserTimer++;
        }

        if (cur_obj_update_dialog(MARIO_DIALOG_LOOK_UP,
            (DIALOG_FLAG_TEXT_DEFAULT | DIALOG_FLAG_TIME_STOP_ENABLED),
            sBowserDefeatedDialogText[o->oBhvParams2ndByte], 0)) {

            o->oBowserTimer++;
            cur_obj_play_sound_2(SOUND_GENERAL2_BOWSER_EXPLODE);
            seq_player_unlower_volume(SEQ_PLAYER_LEVEL, 60);
            seq_player_fade_out(SEQ_PLAYER_LEVEL, 1);
        }

    } else if (bowser_dead_twirl_up()) {
        bowser_dead_hide();
        spawn_triangle_break_particles(20, MODEL_YELLOW_COIN, 1.0f, 0);
        bowser_spawn_collectable();
        set_mario_npc_dialog(MARIO_DIALOG_STOP);
        ret = TRUE;
    }

    return ret;
}

s32 bowser_dead_final_stage_ending(void) {
    UNUSED u8 filler[4];
    s32 ret = FALSE;
    s32 dialogID;

    if (o->oBowserTimer < 2) {

        if (gHudDisplay.stars < 120) {
            dialogID = DIALOG_121;
        } else {
            dialogID = DIALOG_163;
        }

        if (o->oBowserTimer == 0) {
            seq_player_lower_volume(SEQ_PLAYER_LEVEL, 60, 40);
            o->oBowserTimer++;
        }

        if (cur_obj_update_dialog(MARIO_DIALOG_LOOK_UP,
            (DIALOG_FLAG_TEXT_DEFAULT | DIALOG_FLAG_TIME_STOP_ENABLED), dialogID, 0)) {

            cur_obj_set_model(MODEL_BOWSER_NO_SHADOW);
            seq_player_unlower_volume(SEQ_PLAYER_LEVEL, 60);
            seq_player_fade_out(SEQ_PLAYER_LEVEL, 1);
            bowser_spawn_collectable();
            o->oBowserTimer++;
        }

    } else if (o->oOpacity > 4) {
        o->oOpacity -= 4;
    } else {

        bowser_dead_hide();
        ret = TRUE;
    }

    return ret;
}

void bowser_act_dead(void) {
    switch (o->oSubAction) {
        case BOWSER_SUB_ACT_DEAD_FLY_BACK:
            bowser_fly_back_dead();
            break;

        case BOWSER_SUB_ACT_DEAD_BOUNCE:
            bowser_dead_bounce();
            break;

        case BOWSER_SUB_ACT_DEAD_WAIT:

            if (bowser_dead_wait_for_mario()) {
                o->oBowserTimer = 0;

                if (o->oBhvParams2ndByte == BOWSER_BP_BITS) {
                    o->oSubAction = BOWSER_SUB_ACT_DEAD_FINAL_END;
                } else {
                    o->activeFlags |= ACTIVE_FLAG_DITHERED_ALPHA;
                    o->oSubAction++;
                }
            }
            break;

        case BOWSER_SUB_ACT_DEAD_DEFAULT_END:
            if (bowser_dead_default_stage_ending()) {
                o->oSubAction++;
            }
            break;

        case BOWSER_SUB_ACT_DEAD_DEFAULT_END_OVER:
            break;

        case BOWSER_SUB_ACT_DEAD_FINAL_END:
            if (bowser_dead_final_stage_ending()) {
                o->oSubAction++;
            }
            break;

        case BOWSER_SUB_ACT_DEAD_FINAL_END_OVER:
            break;
    }
}

void bowser_tilt_platform(struct Object *platform, s16 angSpeed) {
    s16 angle = o->oBowserAngleToCenter + 0x8000;
    platform->oAngleVelPitch = coss(angle) * angSpeed;
    platform->oAngleVelRoll = -sins(angle) * angSpeed;
}

struct BowserTiltPlatformInfo {

    s16 flag;

    s16 angSpeed;

    s16 time;
};

struct BowserTiltPlatformInfo sBowsertiltPlatformData[] = {
    {  1,   10,  40 },
    {  0,    0,  74 },
    { -1,  -10, 114 },
    {  1,  -20, 134 },
    { -1,   20, 154 },
    {  1,   40, 164 },
    { -1,  -40, 174 },
    {  1,  -80, 179 },
    { -1,   80, 184 },
    {  1,  160, 186 },
    { -1, -160, 186 },
    {  1,    0,   0 },
};

void bowser_act_tilt_lava_platform(void) {

    struct Object *platform = cur_obj_nearest_object_with_behavior(bhvTiltingBowserLavaPlatform);
    UNUSED s16 angle = o->oBowserAngleToCenter + 0x8000;
    s16 angSpeed;
    UNUSED u8 filler[4];

    if (platform == NULL) {
        o->oAction = BOWSER_ACT_DEFAULT;
    } else {
        s32 i = 0;
        s32 isNotTilting = TRUE;

        while (sBowsertiltPlatformData[i].time != 0) {

            if (o->oTimer < sBowsertiltPlatformData[i].time) {

                angSpeed = sBowsertiltPlatformData[i].angSpeed;

                if (sBowsertiltPlatformData[i].flag > 0) {
                    angSpeed = (sBowsertiltPlatformData[i].time - o->oTimer - 1) * angSpeed;
                } else {
                    angSpeed = (o->oTimer - sBowsertiltPlatformData[i - 1].time) * angSpeed;
                }

                bowser_tilt_platform(platform, angSpeed);

                if (angSpeed != 0) {
                    play_sound(SOUND_ENV_MOVING_BIG_PLATFORM, platform->header.gfx.cameraToObject);
                }
                isNotTilting = FALSE;
                break;
            }
            i++;
        }

        if (isNotTilting) {
            o->oAction = BOWSER_ACT_DEFAULT;
            platform->oAngleVelPitch = 0;
            platform->oAngleVelRoll = 0;
            platform->oFaceAnglePitch = 0;
            platform->oFaceAngleRoll = 0;
        }
    }

    cur_obj_extend_animation_if_at_end();
}

s32 bowser_check_fallen_off_stage(void) {
    if (o->oAction != BOWSER_ACT_JUMP_ONTO_STAGE && o->oAction != BOWSER_ACT_TILT_LAVA_PLATFORM) {
        if (o->oPosY < o->oHomeY - 1000.0f) {
            return TRUE;
        }
        if (o->oMoveFlags & OBJ_MOVE_LANDED) {

            if (o->oFloorType == SURFACE_BURNING) {
                return TRUE;
            }

            if (o->oFloorType == SURFACE_DEATH_PLANE) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

void (*sBowserActions[])(void) = {
    bowser_act_default,
    bowser_act_thrown,
    bowser_act_jump_onto_stage,
    bowser_act_dance,
    bowser_act_dead,
    bowser_act_wait,
    bowser_act_intro_walk,
    bowser_act_charge_mario,
    bowser_act_spit_fire_into_sky,
    bowser_act_spit_fire_onto_floor,
    bowser_act_hit_edge,
    bowser_act_turn_from_edge,
    bowser_act_hit_mine,
    bowser_act_big_jump,
    bowser_act_walk_to_mario,
    bowser_act_breath_fire,
    bowser_act_teleport,
    bowser_act_quick_jump,
    bowser_act_idle,
    bowser_act_tilt_lava_platform,
};

struct SoundState sBowserSoundStates[] = {
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 1, 0, -1, SOUND_OBJ_BOWSER_WALK },
    { 1, 0, -1, SOUND_OBJ2_BOWSER_ROAR },
    { 1, 0, -1, SOUND_OBJ2_BOWSER_ROAR },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 1, 20, 40, SOUND_OBJ_BOWSER_WALK },
    { 1, 20, -1, SOUND_OBJ_BOWSER_WALK },
    { 1, 20, 40, SOUND_OBJ_BOWSER_WALK },
    { 1, 0, -1, SOUND_OBJ_BOWSER_TAIL_PICKUP },
    { 1, 0, -1, SOUND_OBJ_BOWSER_DEFEATED },
    { 1, 8, -1, SOUND_OBJ_BOWSER_WALK },
    { 1, 8, 17, SOUND_OBJ_BOWSER_WALK },
    { 1, 8, -10, SOUND_OBJ_BOWSER_WALK },
    { 0, 0, 0, NO_SOUND },
    { 1, 5, -1, SOUND_OBJ_FLAME_BLOWN },
    { 0, 0, 0, NO_SOUND },
    { 0, 0, 0, NO_SOUND },
    { 1, 0, -1, SOUND_OBJ_BOWSER_TAIL_PICKUP },
    { 1, 0, -1, SOUND_OBJ2_BOWSER_ROAR },
};

s8 sBowserRainbowLight[] = { FALSE, FALSE, TRUE };

s8 sBowserHealth[] = { 1, 1, 3 };

void bowser_free_update(void) {
    struct Surface *floor;
    struct Object *platform;
    UNUSED f32 floorHeight;

    if ((platform = o->platform) != NULL) {
        apply_platform_displacement(FALSE, platform);
    }

    o->oBowserGrabbedStatus = BOWSER_GRAB_STATUS_NONE;

    cur_obj_update_floor_and_walls();
    cur_obj_call_action_function(sBowserActions);
    cur_obj_move_standard(-78);

    if (bowser_check_fallen_off_stage()) {
        o->oAction = BOWSER_ACT_JUMP_ONTO_STAGE;
    }

    floorHeight = find_floor(o->oPosX, o->oPosY, o->oPosZ, &floor);
    if ((floor != NULL) && (floor->object != NULL)) {
        o->platform = floor->object;
    } else {
        o->platform = NULL;
    }

    exec_anim_sound_state(sBowserSoundStates);
}

void bowser_held_update(void) {

    o->oBowserStatus &= ~BOWSER_STATUS_FIRE_SKY;
    cur_obj_become_intangible();

    switch (o->oBowserGrabbedStatus) {

        case BOWSER_GRAB_STATUS_NONE:
            cur_obj_play_sound_2(SOUND_OBJ_BOWSER_TAIL_PICKUP);
            cur_obj_unrender_set_action_and_anim(BOWSER_ANIM_GRABBED, BOWSER_ACT_THROWN);
            o->oBowserGrabbedStatus++;
            break;

        case BOWSER_GRAB_STATUS_GRABBED:
            if (cur_obj_check_if_near_animation_end()) {
                cur_obj_init_animation_with_sound(BOWSER_ANIM_SHAKING);
                o->oBowserGrabbedStatus++;
            }
            break;
        case BOWSER_GRAB_STATUS_HOLDING:
            break;
    }

    o->oMoveFlags = 0;

    o->oBowserHeldAnglePitch = gMarioObject->oMoveAnglePitch;
    o->oBowserHeldAngleVelYaw = gMarioObject->oAngleVelYaw;
    o->oMoveAngleYaw = gMarioObject->oMoveAngleYaw;
}

void bowser_thrown_dropped_update(void) {
    f32 swingSpd;

    o->oBowserGrabbedStatus = BOWSER_GRAB_STATUS_NONE;

    cur_obj_get_thrown_or_placed(1.0f, 1.0f, BOWSER_ACT_THROWN);

    swingSpd = o->oBowserHeldAngleVelYaw / 3000.0 * 70.0f;

    if (swingSpd < 0.0f) {
        swingSpd = -swingSpd;
    }

    if (swingSpd > 90.0f) {
        swingSpd *= 2.5;
    }

    o->oForwardVel = coss(o->oBowserHeldAnglePitch) * swingSpd;
    o->oVelY = -sins(o->oBowserHeldAnglePitch) * swingSpd;
    cur_obj_become_intangible();

    o->prevObj->oAction = BOWSER_ACT_TAIL_THROWN;
    o->prevObj->oTimer = 0;
    o->prevObj->oSubAction = 0;

    o->oTimer = 0;
    o->oSubAction = 0;
}

void bhv_bowser_loop(void) {
    s16 angleToMario;
    s16 angleToCenter;

    o->oBowserDistToCenter = sqrtf(o->oPosX * o->oPosX + o->oPosZ * o->oPosZ);
    o->oBowserAngleToCenter = atan2s(0.0f - o->oPosZ, 0.0f - o->oPosX);
    angleToMario = abs_angle_diff(o->oMoveAngleYaw, o->oAngleToMario);
    angleToCenter = abs_angle_diff(o->oMoveAngleYaw, o->oBowserAngleToCenter);

    o->oBowserStatus &= ~0xFF;

    if (angleToMario < 0x2000) {
        o->oBowserStatus |= BOWSER_STATUS_ANGLE_MARIO;
    }
    if (angleToCenter < 0x3800) {
        o->oBowserStatus |= BOWSER_STATUS_ANGLE_CENTER;
    }
    if (o->oBowserDistToCenter < 1000.0f) {
        o->oBowserStatus |= BOWSER_STATUS_DIST_CENTER;
    }
    if (o->oDistanceToMario < 850.0f) {
        o->oBowserStatus |= BOWSER_STATUS_DIST_MARIO;
    }

    switch (o->oHeldState) {
        case HELD_FREE:
            bowser_free_update();
            break;
        case HELD_HELD:
            bowser_held_update();
            break;
        case HELD_THROWN:
            bowser_thrown_dropped_update();
            break;
        case HELD_DROPPED:
            bowser_thrown_dropped_update();
            break;
    }

    cur_obj_align_gfx_with_floor();

    if (o->oAction != BOWSER_ACT_DEAD) {
        if (o->oBowserTargetOpacity != o->oOpacity) {

            if (o->oBowserTargetOpacity > o->oOpacity) {
                o->oOpacity += 20;
                if (o->oOpacity > 255) {
                    o->oOpacity = 255;
                }

            } else {
                o->oOpacity -= 20;
                if (o->oOpacity < 0) {
                    o->oOpacity = 0;
                }
            }
        }
    }
}

void bhv_bowser_init(void) {
    s32 level;

    o->oBowserIsReacting = TRUE;

    o->oOpacity = 255;
    o->oBowserTargetOpacity = 255;

    if (gCurrLevelNum == LEVEL_BOWSER_2) {
        level = BOWSER_BP_BITFS;
    } else if (gCurrLevelNum == LEVEL_BOWSER_3) {
        level = BOWSER_BP_BITS;
    } else {
        level = BOWSER_BP_BITDW;
    }
    o->oBhvParams2ndByte = level;

    o->oBowserRainbowLight = sBowserRainbowLight[level];
    o->oHealth = sBowserHealth[level];

    cur_obj_start_cam_event(o, CAM_EVENT_BOWSER_INIT);
    o->oAction = BOWSER_ACT_WAIT;

    o->oBowserEyesTimer = 0;
    o->oBowserEyesShut = FALSE;
}

Gfx *geo_update_body_rot_from_parent(s32 callContext, UNUSED struct GraphNode *node, Mat4 mtx) {
    if (callContext == GEO_CONTEXT_RENDER) {
        Mat4 mtx2;
        struct Object *obj = (struct Object *) gCurGraphNodeObject;
        if (obj->prevObj != NULL) {
            create_transformation_from_matrices(mtx2, mtx, *gCurGraphNodeCamera->matrixPtr);
            obj_update_pos_from_parent_transformation(mtx2, obj->prevObj);
            obj_set_gfx_pos_from_pos(obj->prevObj);
        }
    }

    return NULL;
}

enum BowserEyesGSCId {
     BOWSER_EYES_OPEN,
     BOWSER_EYES_HALF_CLOSED,
     BOWSER_EYES_CLOSED,
     BOWSER_EYES_LEFT,
     BOWSER_EYES_FAR_LEFT,
     BOWSER_EYES_RIGHT,
     BOWSER_EYES_FAR_RIGHT,
     BOWSER_EYES_DERP,
     BOWSER_EYES_CROSS,
     BOWSER_EYES_RESET
};

void bowser_open_eye_switch(struct Object *obj, struct GraphNodeSwitchCase *switchCase) {
    s32 eyeCase;
    s16 angleFromMario;

    angleFromMario = abs_angle_diff(obj->oMoveAngleYaw, obj->oAngleToMario);
    eyeCase = switchCase->selectedCase;

    switch (eyeCase) {
        case BOWSER_EYES_OPEN:

            if (angleFromMario > 0x2000) {
                if (obj->oAngleVelYaw > 0) {
                    switchCase->selectedCase = BOWSER_EYES_RIGHT;
                }
                if (obj->oAngleVelYaw < 0) {
                    switchCase->selectedCase = BOWSER_EYES_LEFT;
                }
            }

            if (obj->oBowserEyesTimer > 50) {
                switchCase->selectedCase = BOWSER_EYES_HALF_CLOSED;
            }
            break;

        case BOWSER_EYES_HALF_CLOSED:

            if (obj->oBowserEyesTimer > 2) {
                switchCase->selectedCase = BOWSER_EYES_CLOSED;
            }
            break;

        case BOWSER_EYES_CLOSED:

            if (obj->oBowserEyesTimer > 2) {
                switchCase->selectedCase = BOWSER_EYES_RESET;
            }
            break;

        case BOWSER_EYES_RESET:

            if (obj->oBowserEyesTimer > 2) {
                switchCase->selectedCase = BOWSER_EYES_OPEN;
            }
            break;

        case BOWSER_EYES_RIGHT:

            if (obj->oBowserEyesTimer > 2) {
                switchCase->selectedCase = BOWSER_EYES_FAR_RIGHT;
                if (obj->oAngleVelYaw <= 0) {
                    switchCase->selectedCase = BOWSER_EYES_OPEN;
                }
            }
            break;

        case BOWSER_EYES_FAR_RIGHT:

            if (obj->oAngleVelYaw <= 0) {
                switchCase->selectedCase = BOWSER_EYES_RIGHT;
            }
            break;

        case BOWSER_EYES_LEFT:

            if (obj->oBowserEyesTimer > 2) {
                switchCase->selectedCase = BOWSER_EYES_FAR_LEFT;
                if (obj->oAngleVelYaw >= 0) {
                    switchCase->selectedCase = BOWSER_EYES_OPEN;
                }
            }
            break;

        case BOWSER_EYES_FAR_LEFT:

            if (obj->oAngleVelYaw >= 0) {
                switchCase->selectedCase = BOWSER_EYES_LEFT;
            }
            break;

        default:
            switchCase->selectedCase = BOWSER_EYES_OPEN;
    }

    if (switchCase->selectedCase != eyeCase) {
        obj->oBowserEyesTimer = -1;
    }
}

Gfx *geo_switch_bowser_eyes(s32 callContext, struct GraphNode *node, UNUSED Mat4 *mtx) {
    UNUSED s16 eyeShut;
    UNUSED u8 filler[4];
    struct Object *obj = (struct Object *) gCurGraphNodeObject;
    struct GraphNodeSwitchCase *switchCase = (struct GraphNodeSwitchCase *) node;

    if (callContext == GEO_CONTEXT_RENDER) {
        if (gCurGraphNodeHeldObject != NULL) {
            obj = gCurGraphNodeHeldObject->objNode;
        }

        switch (eyeShut = obj->oBowserEyesShut) {
            case FALSE:
                bowser_open_eye_switch(obj, switchCase);
                break;
            case TRUE:
                switchCase->selectedCase = BOWSER_EYES_CLOSED;
                break;
        }

        obj->oBowserEyesTimer++;
    }

    return NULL;
}

Gfx *geo_bits_bowser_coloring(s32 callContext, struct GraphNode *node, UNUSED s32 context) {
    Gfx *gfxHead = NULL;
    Gfx *gfx;

    if (callContext == GEO_CONTEXT_RENDER) {
        struct Object *obj = (struct Object *) gCurGraphNodeObject;
        struct GraphNodeGenerated *graphNode = (struct GraphNodeGenerated *) node;

        if (gCurGraphNodeHeldObject != NULL) {
            obj = gCurGraphNodeHeldObject->objNode;
        }

        if (obj->oOpacity == 255) {
            graphNode->fnNode.node.flags = (graphNode->fnNode.node.flags & 0xFF) | (LAYER_OPAQUE << 8);
        } else {
            graphNode->fnNode.node.flags = (graphNode->fnNode.node.flags & 0xFF) | (LAYER_TRANSPARENT << 8);
        }

        gfx = gfxHead = alloc_display_list(2 * sizeof(Gfx));

        if (obj->oBowserRainbowLight) {
            gSPClearGeometryMode(gfx++, G_LIGHTING);
        }

        gSPEndDisplayList(gfx);
    }

    return gfxHead;
}
