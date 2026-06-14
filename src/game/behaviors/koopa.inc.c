
static struct ObjectHitbox sKoopaHitbox = {
     INTERACT_KOOPA,
     0,
     0,
     0,
     -1,
     60,
     40,
     40,
     30,
};

static u8 sKoopaUnshelledAttackHandlers[] = {
     ATTACK_HANDLER_KNOCKBACK,
     ATTACK_HANDLER_KNOCKBACK,
     ATTACK_HANDLER_SQUISHED,
     ATTACK_HANDLER_SQUISHED,
     ATTACK_HANDLER_KNOCKBACK,
     ATTACK_HANDLER_KNOCKBACK,
};

static u8 sKoopaShelledAttackHandlers[] = {
     ATTACK_HANDLER_SPECIAL_KOOPA_LOSE_SHELL,
     ATTACK_HANDLER_SPECIAL_KOOPA_LOSE_SHELL,
     ATTACK_HANDLER_SPECIAL_KOOPA_LOSE_SHELL,
     ATTACK_HANDLER_SPECIAL_KOOPA_LOSE_SHELL,
     ATTACK_HANDLER_SPECIAL_KOOPA_LOSE_SHELL,
     ATTACK_HANDLER_SPECIAL_KOOPA_LOSE_SHELL,
};

struct KoopaTheQuickProperties {
    s16 initDialogID;
    s16 winDialogID;
    Trajectory const *path;
    Vec3s starPos;
};

static struct KoopaTheQuickProperties sKoopaTheQuickProperties[] = {
    { DIALOG_005, DIALOG_007, bob_seg7_trajectory_koopa, { 3030, 4500, -4600 } },
    { DIALOG_009, DIALOG_031, thi_seg7_trajectory_koopa, { 7100, -1300, -6000 } },
};

void bhv_koopa_init(void) {
    if ((o->oKoopaMovementType = o->oBhvParams2ndByte) == KOOPA_BP_TINY) {

        o->oKoopaMovementType = KOOPA_BP_NORMAL;
        o->oKoopaAgility = 1.6f / 3.0f;
        o->oDrawingDistance = 1500.0f;
        cur_obj_scale(0.8f);
        o->oGravity = -6.4f / 3.0f;
    } else if (o->oKoopaMovementType >= KOOPA_BP_KOOPA_THE_QUICK_BASE) {

        o->oKoopaTheQuickRaceIndex = o->oKoopaMovementType - KOOPA_BP_KOOPA_THE_QUICK_BASE;
        o->oKoopaAgility = 4.0f;
        cur_obj_scale(3.0f);
    } else {
        o->oKoopaAgility = 1.0f;
    }
}

static void koopa_play_footstep_sound(s8 animFrame1, s8 animFrame2) {
    s32 sound;

    if (o->header.gfx.scale[0] > 1.5f) {
        sound = SOUND_OBJ_KOOPA_THE_QUICK_WALK;
    } else {
        sound = SOUND_OBJ_KOOPA_WALK;
    }

    cur_obj_play_sound_at_anim_range(animFrame1, animFrame2, sound);
}

static s32 koopa_check_run_from_mario(void) {
    if (o->oKoopaDistanceToMario < 300.0f
        && abs_angle_diff(o->oKoopaAngleToMario, o->oMoveAngleYaw) < 0x3000) {
        o->oAction = KOOPA_SHELLED_ACT_RUN_FROM_MARIO;
        return TRUE;
    }

    return FALSE;
}

static void koopa_shelled_act_stopped(void) {
    o->oForwardVel = 0.0f;
    if (cur_obj_init_anim_and_check_if_end(7)) {
        o->oAction = KOOPA_SHELLED_ACT_WALK;
        o->oKoopaTargetYaw = o->oMoveAngleYaw + 0x2000 * (s16) random_sign();
    }
}

static void koopa_walk_start(void) {
    obj_forward_vel_approach(3.0f * o->oKoopaAgility, 0.3f * o->oKoopaAgility);

    if (cur_obj_init_anim_and_check_if_end(11)) {
        o->oSubAction++;
        o->oKoopaCountdown = random_linear_offset(30, 100);
    }
}

static void koopa_walk(void) {
    cur_obj_init_animation_with_sound(9);
    koopa_play_footstep_sound(2, 17);

    if (o->oKoopaCountdown != 0) {
        o->oKoopaCountdown--;
    } else if (cur_obj_check_if_near_animation_end()) {
        o->oSubAction++;
    }
}

static void koopa_walk_stop(void) {
    obj_forward_vel_approach(0.0f, 1.0f * o->oKoopaAgility);
    if (cur_obj_init_anim_and_check_if_end(10)) {
        o->oAction = KOOPA_SHELLED_ACT_STOPPED;
    }
}

static void koopa_shelled_act_walk(void) {
    if (o->oKoopaTurningAwayFromWall) {
        o->oKoopaTurningAwayFromWall = obj_resolve_collisions_and_turn(o->oKoopaTargetYaw, 0x200);
    } else {

        if (o->oDistanceToMario >= 25000.0f) {
            o->oKoopaTargetYaw = o->oAngleToMario;
        }

        o->oKoopaTurningAwayFromWall = obj_bounce_off_walls_edges_objects(&o->oKoopaTargetYaw);
        cur_obj_rotate_yaw_toward(o->oKoopaTargetYaw, 0x200);
    }

    switch (o->oSubAction) {
        case KOOPA_SHELLED_SUB_ACT_START_WALK:
            koopa_walk_start();
            break;
        case KOOPA_SHELLED_SUB_ACT_WALK:
            koopa_walk();
            break;
        case KOOPA_SHELLED_SUB_ACT_STOP_WALK:
            koopa_walk_stop();
            break;
    }

    koopa_check_run_from_mario();
}

static void koopa_shelled_act_run_from_mario(void) {
    cur_obj_init_animation_with_sound(1);
    koopa_play_footstep_sound(0, 11);

    if (o->oDistanceToMario >= 25000.0f) {
        o->oAngleToMario += 0x8000;
        o->oDistanceToMario = 0.0f;
    }

    if (o->oTimer > 30 && o->oDistanceToMario > 800.0f) {
        if (obj_forward_vel_approach(0.0f, 1.0f)) {
            o->oAction = KOOPA_SHELLED_ACT_STOPPED;
        }
    } else {
        cur_obj_rotate_yaw_toward(o->oAngleToMario + 0x8000, 0x400);
        obj_forward_vel_approach(17.0f, 1.0f);
    }
}

static void koopa_dive_update_speed(f32 decel) {
    if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {
        obj_forward_vel_approach(0.0f, decel);
        if (o->oForwardVel > 5.0f) {
            if (!(o->oTimer % 4)) {
                spawn_object_with_scale(o, MODEL_SMOKE, bhvWhitePuffSmoke2, 1.0f);
            }
        }
    }
}

static void koopa_shelled_act_lying(void) {
    if (o->oForwardVel != 0.0f) {
        if (o->oMoveFlags & OBJ_MOVE_HIT_WALL) {
            o->oMoveAngleYaw = cur_obj_reflect_move_angle_off_wall();
        }

        cur_obj_init_anim_extend(5);
        koopa_dive_update_speed(0.3f);
    } else if (o->oKoopaCountdown != 0) {
        o->oKoopaCountdown--;
        cur_obj_extend_animation_if_at_end();
    } else if (cur_obj_init_anim_and_check_if_end(6)) {
        o->oAction = KOOPA_SHELLED_ACT_STOPPED;
    }
}

void shelled_koopa_attack_handler(s32 attackType) {
    if (o->header.gfx.scale[0] > 0.8f) {
        cur_obj_play_sound_2(SOUND_OBJ_KOOPA_DAMAGE);

        o->oKoopaMovementType = KOOPA_BP_UNSHELLED;
        o->oAction = KOOPA_UNSHELLED_ACT_LYING;
        o->oForwardVel = 20.0f;

        if (attackType != ATTACK_FROM_ABOVE && attackType != ATTACK_GROUND_POUND_OR_TWIRL) {
            o->oMoveAngleYaw = obj_angle_to_object(gMarioObject, o);
        }

        cur_obj_set_model(MODEL_KOOPA_WITHOUT_SHELL);
        spawn_object(o, MODEL_KOOPA_SHELL, bhvKoopaShell);

        cur_obj_become_intangible();
    } else {

        obj_die_if_health_non_positive();
    }
}

static void koopa_shelled_update(void) {
    cur_obj_update_floor_and_walls();
    obj_update_blinking(&o->oKoopaBlinkTimer, 20, 50, 4);

    switch (o->oAction) {
        case KOOPA_SHELLED_ACT_STOPPED:
            koopa_shelled_act_stopped();
            koopa_check_run_from_mario();
            break;

        case KOOPA_SHELLED_ACT_WALK:
            koopa_shelled_act_walk();
            break;

        case KOOPA_SHELLED_ACT_RUN_FROM_MARIO:
            koopa_shelled_act_run_from_mario();
            break;

        case KOOPA_SHELLED_ACT_LYING:
            koopa_shelled_act_lying();
            break;
    }

    if (o->header.gfx.scale[0] > 0.8f) {
        obj_handle_attacks(&sKoopaHitbox, o->oAction, sKoopaShelledAttackHandlers);
    } else {

        obj_handle_attacks(&sKoopaHitbox, KOOPA_SHELLED_ACT_DIE, sKoopaUnshelledAttackHandlers);
        if (o->oAction == KOOPA_SHELLED_ACT_DIE) {
            obj_die_if_health_non_positive();
        }
    }

    cur_obj_move_standard(-78);
}

static void koopa_unshelled_act_run(void) {
    f32 distToShell = 99999.0f;
    struct Object *shell;

    cur_obj_init_animation_with_sound(3);
    koopa_play_footstep_sound(0, 6);

    if (o->oKoopaTurningAwayFromWall) {
        o->oKoopaTurningAwayFromWall = obj_resolve_collisions_and_turn(o->oKoopaTargetYaw, 0x600);
    } else {

        if (o->oDistanceToMario >= 25000.0f) {
            o->oKoopaTargetYaw = o->oAngleToMario;
        }

        shell = cur_obj_find_nearest_object_with_behavior(bhvKoopaShell, &distToShell);
        if (shell != NULL) {

            o->oKoopaTargetYaw = obj_angle_to_object(o, shell);
        } else if (!(o->oKoopaTurningAwayFromWall =
                         obj_bounce_off_walls_edges_objects(&o->oKoopaTargetYaw))) {

            if (o->oKoopaUnshelledTimeUntilTurn != 0) {
                o->oKoopaUnshelledTimeUntilTurn--;
            } else {
                o->oKoopaTargetYaw = obj_random_fixed_turn(0x2000);
            }
        }

        if (o->oDistanceToMario > 800.0f
            || (shell != NULL
                && abs_angle_diff(o->oKoopaTargetYaw, o->oAngleToMario + 0x8000) < 0x2000)) {

            cur_obj_rotate_yaw_toward(o->oKoopaTargetYaw, 0x600);
        } else {

            cur_obj_rotate_yaw_toward(o->oAngleToMario + 0x8000, 0x600);
        }
    }

    if (obj_forward_vel_approach(20.0f, 1.0f) && distToShell < 600.0f
        && abs_angle_diff(o->oKoopaTargetYaw, o->oMoveAngleYaw) < 0xC00) {
        o->oMoveAngleYaw = o->oKoopaTargetYaw;
        o->oAction = KOOPA_UNSHELLED_ACT_DIVE;
        o->oForwardVel *= 1.2f;
        o->oVelY = distToShell / 20.0f;
        o->oKoopaCountdown = 20;
    }
}

static void koopa_unshelled_act_dive(void) {
    struct Object *shell;
    f32 distToShell;

    if (o->oTimer > 10) {
        cur_obj_become_tangible();
    }

    if (o->oTimer > 10) {
        shell = cur_obj_find_nearest_object_with_behavior(bhvKoopaShell, &distToShell);

        if (shell != NULL && dist_between_objects(shell, gMarioObject) > 200.0f
            && distToShell < 50.0f) {
            o->oKoopaMovementType = KOOPA_BP_NORMAL;
            o->oAction = KOOPA_SHELLED_ACT_LYING;
            o->oForwardVel *= 0.5f;

            cur_obj_set_model(MODEL_KOOPA_WITH_SHELL);
            obj_mark_for_deletion(shell);
            goto end;
        }
    }

    if (o->oForwardVel != 0.0f) {
        if (o->oAction == KOOPA_UNSHELLED_ACT_LYING) {
            o->oAnimState = 1;
            cur_obj_init_anim_extend(2);
        } else {
            cur_obj_init_anim_extend(5);
        }
        koopa_dive_update_speed(0.5f);
    } else if (o->oKoopaCountdown != 0) {
        o->oKoopaCountdown--;
        cur_obj_extend_animation_if_at_end();
    } else if (cur_obj_init_anim_and_check_if_end(6)) {
        o->oAction = KOOPA_UNSHELLED_ACT_RUN;
    }

end:;
}

static void koopa_unshelled_act_unused3(void) {
    cur_obj_init_anim_extend(0);
}

static void koopa_unshelled_update(void) {
    cur_obj_update_floor_and_walls();
    obj_update_blinking(&o->oKoopaBlinkTimer, 10, 15, 3);

    switch (o->oAction) {
        case KOOPA_UNSHELLED_ACT_RUN:
            koopa_unshelled_act_run();
            break;
        case KOOPA_UNSHELLED_ACT_DIVE:
        case KOOPA_UNSHELLED_ACT_LYING:
            koopa_unshelled_act_dive();
            break;
        case KOOPA_UNSHELLED_ACT_UNUSED3:
            koopa_unshelled_act_unused3();
            break;
    }

    obj_handle_attacks(&sKoopaHitbox, o->oAction, sKoopaUnshelledAttackHandlers);
    cur_obj_move_standard(-78);
}

s32 obj_begin_race(s32 noTimer) {
    if (o->oTimer == 50) {
        cur_obj_play_sound_2(SOUND_GENERAL_RACE_GUN_SHOT);

        if (!noTimer) {
            play_music(SEQ_PLAYER_LEVEL, SEQUENCE_ARGS(4, SEQ_LEVEL_SLIDE), 0);

            level_control_timer(TIMER_CONTROL_SHOW);
            level_control_timer(TIMER_CONTROL_START);

            o->parentObj->oKoopaRaceEndpointRaceBegun = TRUE;
        }

        set_mario_npc_dialog(MARIO_DIALOG_STOP);
        disable_time_stop_including_mario();
    } else if (o->oTimer > 50) {
        return TRUE;
    }

    return FALSE;
}

static void koopa_the_quick_act_wait_before_race(void) {
    koopa_shelled_act_stopped();

    if (o->oKoopaTheQuickInitTextboxCooldown != 0) {
        o->oKoopaTheQuickInitTextboxCooldown--;
    } else if (cur_obj_can_mario_activate_textbox_2(400.0f, 400.0f)) {

        o->oAction = KOOPA_THE_QUICK_ACT_SHOW_INIT_TEXT;
        o->oForwardVel = 0.0f;
        cur_obj_init_animation_with_sound(7);
    }
}

static void koopa_the_quick_act_show_init_text(void) {
    s32 response = obj_update_race_proposition_dialog(
        sKoopaTheQuickProperties[o->oKoopaTheQuickRaceIndex].initDialogID);

    if (response == DIALOG_RESPONSE_YES) {
        UNUSED u8 filler[4];

        gMarioShotFromCannon = FALSE;
        o->oAction = KOOPA_THE_QUICK_ACT_RACE;
        o->oForwardVel = 0.0f;

        o->parentObj = cur_obj_nearest_object_with_behavior(bhvKoopaRaceEndpoint);
        o->oPathedStartWaypoint = o->oPathedPrevWaypoint =
            segmented_to_virtual(sKoopaTheQuickProperties[o->oKoopaTheQuickRaceIndex].path);

        o->oKoopaTurningAwayFromWall = FALSE;
        o->oFlags |= OBJ_FLAG_ACTIVE_FROM_AFAR;
    } else if (response == DIALOG_RESPONSE_NO) {
        o->oAction = KOOPA_THE_QUICK_ACT_WAIT_BEFORE_RACE;
        o->oKoopaTheQuickInitTextboxCooldown = 60;
    }
}

static s32 koopa_the_quick_detect_bowling_ball(void) {
    struct Object *ball;
    f32 distToBall;

    ball = cur_obj_find_nearest_object_with_behavior(bhvBowlingBall, &distToBall);

    if (ball != NULL) {
        s16 angleToBall = obj_turn_toward_object(o, ball, O_MOVE_ANGLE_YAW_INDEX, 0);
        f32 ballSpeedInKoopaRunDir = ball->oForwardVel * coss(ball->oMoveAngleYaw - o->oMoveAngleYaw);

        if (abs_angle_diff(o->oMoveAngleYaw, angleToBall) < 0x4000) {

            if (distToBall < 400.0f) {
                if (ballSpeedInKoopaRunDir < o->oForwardVel * 0.7f) {

                    return 1;
                } else {

                    o->oForwardVel -= 2.0f;
                }
            }
        } else if (distToBall < 300.0f && ballSpeedInKoopaRunDir > o->oForwardVel) {

            return -1;
        }
    }

    return 0;
}

static void koopa_the_quick_animate_footsteps(void) {

    cur_obj_init_animation_with_accel_and_sound(9, o->oForwardVel * 0.09f);
    koopa_play_footstep_sound(2, 17);
}

static void koopa_the_quick_act_race(void) {
    if (obj_begin_race(FALSE)) {

        cur_obj_push_mario_away_from_cylinder(180.0f, 300.0f);

        if (cur_obj_follow_path(0) == PATH_REACHED_END) {
            o->oAction = KOOPA_THE_QUICK_ACT_DECELERATE;
        } else {
            f32 downhillSteepness;
            s32 bowlingBallStatus;

            downhillSteepness = 1.0f + sins((s16)(f32) o->oPathedTargetPitch);
            cur_obj_rotate_yaw_toward(o->oPathedTargetYaw, (s32)(o->oKoopaAgility * 150.0f));

            switch (o->oSubAction) {
                case KOOPA_THE_QUICK_SUB_ACT_START_RUN:
                    koopa_walk_start();
                    break;

                case KOOPA_THE_QUICK_SUB_ACT_RUN:
                    koopa_the_quick_animate_footsteps();

                    if (o->parentObj->oKoopaRaceEndpointRaceStatus != 0 && o->oDistanceToMario > 1500.0f
                        && (o->oPathedPrevWaypointFlags & WAYPOINT_MASK_00FF) < 28) {

                        o->oKoopaAgility = 8.0f;
                    } else if (o->oKoopaTheQuickRaceIndex != KOOPA_THE_QUICK_BOB_INDEX) {
                        o->oKoopaAgility = 6.0f;
                    } else {
                        o->oKoopaAgility = 4.0f;
                    }

                    obj_forward_vel_approach(o->oKoopaAgility * 6.0f * downhillSteepness,
                                             o->oKoopaAgility * 0.1f);

                    if (o->oMoveFlags & OBJ_MOVE_HIT_WALL) {
                        o->oVelY = 20.0f;
                    }

                    bowlingBallStatus = koopa_the_quick_detect_bowling_ball();
                    if (bowlingBallStatus != 0 || (o->oMoveFlags & OBJ_MOVE_HIT_EDGE)) {

                        if (bowlingBallStatus < 0) {
                            o->oForwardVel = 0.0f;
                        }

                        if (bowlingBallStatus != 0
                            || (o->oPathedPrevWaypointFlags & WAYPOINT_MASK_00FF) >= 8) {
                            o->oVelY = 80.0f;
                        } else {
                            o->oVelY = 40.0f;
                        }

                        o->oGravity = -6.0f;
                        o->oSubAction = 2;
                        o->oMoveFlags = 0;

                        cur_obj_init_animation_with_sound(12);
                    }
                    break;

                case KOOPA_THE_QUICK_SUB_ACT_JUMP:

                    if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {
                        if (cur_obj_init_anim_and_check_if_end(13)) {
                            o->oSubAction--;
                        }

                        koopa_the_quick_detect_bowling_ball();
                    }
            }
        }
    }
}

static void koopa_the_quick_act_decelerate(void) {
    obj_forward_vel_approach(3.0f, 1.0f);
    cur_obj_init_animation_with_accel_and_sound(9, 0.99f);

    if (cur_obj_check_if_near_animation_end()) {
        o->oAction = KOOPA_THE_QUICK_ACT_STOP;
        o->oForwardVel = 3.0f;
    }
}

static void koopa_the_quick_act_stop(void) {
    koopa_walk_stop();

    if (o->oAction == KOOPA_SHELLED_ACT_STOPPED) {
        o->oAction = KOOPA_THE_QUICK_ACT_AFTER_RACE;
    }
}

static void koopa_the_quick_act_after_race(void) {
    cur_obj_init_animation_with_sound(7);

    if (o->parentObj->oKoopaRaceEndpointDialog == 0) {
        if (cur_obj_can_mario_activate_textbox_2(400.0f, 400.0f)) {
            stop_background_music(SEQUENCE_ARGS(4, SEQ_LEVEL_SLIDE));

            if (o->parentObj->oKoopaRaceEndpointRaceStatus != 0) {
                if (o->parentObj->oKoopaRaceEndpointRaceStatus < 0) {

                    o->parentObj->oKoopaRaceEndpointRaceStatus = 0;
                    o->parentObj->oKoopaRaceEndpointDialog = DIALOG_006;
                } else {

                    o->parentObj->oKoopaRaceEndpointDialog =
                        sKoopaTheQuickProperties[o->oKoopaTheQuickRaceIndex].winDialogID;
                }
            } else {

                o->parentObj->oKoopaRaceEndpointDialog = DIALOG_041;
            }

            o->oFlags &= ~OBJ_FLAG_ACTIVE_FROM_AFAR;
        }
    } else if (o->parentObj->oKoopaRaceEndpointDialog > 0) {
        s32 dialogResponse = cur_obj_update_dialog_with_cutscene(MARIO_DIALOG_LOOK_UP,
            DIALOG_FLAG_TURN_TO_MARIO, CUTSCENE_DIALOG, o->parentObj->oKoopaRaceEndpointDialog);
        if (dialogResponse != 0) {
            o->parentObj->oKoopaRaceEndpointDialog = DIALOG_NONE;
            o->oTimer = 0;
        }
    } else if (o->parentObj->oKoopaRaceEndpointRaceStatus != 0) {
        spawn_default_star(sKoopaTheQuickProperties[o->oKoopaTheQuickRaceIndex].starPos[0],
                           sKoopaTheQuickProperties[o->oKoopaTheQuickRaceIndex].starPos[1],
                           sKoopaTheQuickProperties[o->oKoopaTheQuickRaceIndex].starPos[2]);

        o->parentObj->oKoopaRaceEndpointRaceStatus = 0;
    }
}

static void koopa_the_quick_update(void) {
    cur_obj_update_floor_and_walls();
    obj_update_blinking(&o->oKoopaBlinkTimer, 10, 15, 3);

    switch (o->oAction) {
        case KOOPA_THE_QUICK_ACT_WAIT_BEFORE_RACE:
        case KOOPA_THE_QUICK_ACT_UNUSED1:
            koopa_the_quick_act_wait_before_race();
            break;
        case KOOPA_THE_QUICK_ACT_SHOW_INIT_TEXT:
            koopa_the_quick_act_show_init_text();
            break;
        case KOOPA_THE_QUICK_ACT_RACE:
            koopa_the_quick_act_race();
            break;
        case KOOPA_THE_QUICK_ACT_DECELERATE:
            koopa_the_quick_act_decelerate();
            break;
        case KOOPA_THE_QUICK_ACT_STOP:
            koopa_the_quick_act_stop();
            break;
        case KOOPA_THE_QUICK_ACT_AFTER_RACE:
            koopa_the_quick_act_after_race();
            break;
    }

    if (o->parentObj != o) {
        if (dist_between_objects(o, o->parentObj) < 400.0f) {
            o->parentObj->oKoopaRaceEndpointKoopaFinished = TRUE;
        }
    }

    cur_obj_push_mario_away_from_cylinder(140.0f, 300.0f);
    cur_obj_move_standard(-78);
}

void bhv_koopa_update(void) {

    o->oDeathSound = SOUND_OBJ_KOOPA_FLYGUY_DEATH;

    if (o->oKoopaMovementType >= KOOPA_BP_KOOPA_THE_QUICK_BASE) {
        koopa_the_quick_update();
    } else if (obj_update_standard_actions(o->oKoopaAgility * 1.5f)) {
        o->oKoopaDistanceToMario = o->oDistanceToMario;
        o->oKoopaAngleToMario = o->oAngleToMario;
        treat_far_home_as_mario(1000.0f);

        switch (o->oKoopaMovementType) {
            case KOOPA_BP_UNSHELLED:
                koopa_unshelled_update();
                break;
            case KOOPA_BP_NORMAL:
                koopa_shelled_update();
                break;
            case KOOPA_BP_KOOPA_THE_QUICK_BOB:
            case KOOPA_BP_KOOPA_THE_QUICK_THI:
                koopa_the_quick_update();
                break;
        }
    } else {
        o->oAnimState = 1;
    }

    obj_face_yaw_approach(o->oMoveAngleYaw, 0x600);
}

void bhv_koopa_race_endpoint_update(void) {
    if (o->oKoopaRaceEndpointRaceBegun && !o->oKoopaRaceEndpointRaceEnded) {
        if (o->oKoopaRaceEndpointKoopaFinished || o->oDistanceToMario < 400.0f) {
            o->oKoopaRaceEndpointRaceEnded = TRUE;
            level_control_timer(TIMER_CONTROL_STOP);

            if (!o->oKoopaRaceEndpointKoopaFinished) {
                play_race_fanfare();
                if (gMarioShotFromCannon) {
                    o->oKoopaRaceEndpointRaceStatus = -1;
                } else {
                    o->oKoopaRaceEndpointRaceStatus = 1;
                }
            }
        }
    }
}
