
static Collision const *sPlatformOnTrackCollisionModels[] = {
     rr_seg7_collision_07029038,
     ccm_seg7_collision_070163F8,
     checkerboard_platform_seg8_collision_0800D710,
     bitfs_seg7_collision_070157E0,
};

static Trajectory const *sPlatformOnTrackPaths[] = {
    rr_seg7_trajectory_0702EC3C,
    rr_seg7_trajectory_0702ECC0,
    ccm_seg7_trajectory_0701669C,
    bitfs_seg7_trajectory_070159AC,
    hmc_seg7_trajectory_0702B86C,
    lll_seg7_trajectory_0702856C,
    lll_seg7_trajectory_07028660,
    rr_seg7_trajectory_0702ED9C,
    rr_seg7_trajectory_0702EEE0,
};

static void platform_on_track_reset(void) {
    o->oAction = PLATFORM_ON_TRACK_ACT_INIT;

    o->oPlatformOnTrackBaseBallIndex += 99;
}

static void platform_on_track_mario_not_on_platform(void) {
    if (!((u16)(o->oBhvParams >> 16) & PLATFORM_ON_TRACK_BP_DONT_DISAPPEAR)) {

        if (cur_obj_wait_then_blink(150, 40)) {
            platform_on_track_reset();
            o->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
        }
    }
}

void bhv_platform_on_track_init(void) {
    if (!(o->activeFlags & ACTIVE_FLAG_IN_DIFFERENT_ROOM)) {
        s16 pathIndex = (u16)(o->oBhvParams >> 16) & PLATFORM_ON_TRACK_BP_MASK_PATH;
        o->oPlatformOnTrackType = ((u16)(o->oBhvParams >> 16) & PLATFORM_ON_TRACK_BP_MASK_TYPE) >> 4;

        o->oPlatformOnTrackIsNotSkiLift = o->oPlatformOnTrackType - PLATFORM_ON_TRACK_TYPE_SKI_LIFT;

        o->collisionData =
            segmented_to_virtual(sPlatformOnTrackCollisionModels[o->oPlatformOnTrackType]);

        o->oPlatformOnTrackStartWaypoint = segmented_to_virtual(sPlatformOnTrackPaths[pathIndex]);

        o->oPlatformOnTrackIsNotHMC = pathIndex - 4;

        o->oBhvParams2ndByte = o->oMoveAngleYaw;

        if (o->oPlatformOnTrackType == PLATFORM_ON_TRACK_TYPE_CHECKERED) {
            o->header.gfx.scale[1] = 2.0f;
        }
    }
}

static void platform_on_track_act_init(void) {
    s32 i;

    o->oPlatformOnTrackPrevWaypoint = o->oPlatformOnTrackStartWaypoint;
    o->oPlatformOnTrackPrevWaypointFlags = 0;
    o->oPlatformOnTrackBaseBallIndex = 0;

    o->oPosX = o->oHomeX = o->oPlatformOnTrackStartWaypoint->pos[0];
    o->oPosY = o->oHomeY = o->oPlatformOnTrackStartWaypoint->pos[1];
    o->oPosZ = o->oHomeZ = o->oPlatformOnTrackStartWaypoint->pos[2];

    o->oFaceAngleYaw = o->oBhvParams2ndByte;
    o->oForwardVel = o->oVelX = o->oVelY = o->oVelZ = o->oPlatformOnTrackDistMovedSinceLastBall = 0.0f;

    o->oPlatformOnTrackWasStoodOn = FALSE;

    if (o->oPlatformOnTrackIsNotSkiLift) {
        o->oFaceAngleRoll = 0;
    }

    for (i = 1; i < 6; i++) {
        platform_on_track_update_pos_or_spawn_ball(i, o->oHomeX, o->oHomeY, o->oHomeZ);
    }

    o->oAction = PLATFORM_ON_TRACK_ACT_WAIT_FOR_MARIO;
}

static void platform_on_track_act_wait_for_mario(void) {
    if (gMarioObject->platform == o) {
        if (o->oTimer > 20) {
            o->oAction = PLATFORM_ON_TRACK_ACT_MOVE_ALONG_TRACK;
        }
    } else {
        if (o->activeFlags & ACTIVE_FLAG_IN_DIFFERENT_ROOM) {
            platform_on_track_reset();
        }

        o->oTimer = 0;
    }
}

static void platform_on_track_act_move_along_track(void) {
    s16 initialAngle;

    if (!o->oPlatformOnTrackIsNotSkiLift) {
        cur_obj_play_sound_1(SOUND_ENV_ELEVATOR3);
    } else if (!o->oPlatformOnTrackIsNotHMC) {
        cur_obj_play_sound_1(SOUND_ENV_ELEVATOR1);
    }

    if (o->oPlatformOnTrackPrevWaypointFlags == WAYPOINT_FLAGS_END
        && !((u16)(o->oBhvParams >> 16) & PLATFORM_ON_TRACK_BP_RETURN_TO_START)) {
        o->oAction = PLATFORM_ON_TRACK_ACT_FALL;
    } else {

        if (o->oPlatformOnTrackPrevWaypointFlags != 0 && !o->oPlatformOnTrackIsNotSkiLift) {
            if (o->oPlatformOnTrackPrevWaypointFlags == WAYPOINT_FLAGS_END
                || o->oPlatformOnTrackPrevWaypointFlags == WAYPOINT_FLAGS_PLATFORM_ON_TRACK_PAUSE) {
                cur_obj_play_sound_2(SOUND_GENERAL_UNKNOWN4_LOWPRIO);

                o->oForwardVel = 0.0f;
                if (o->oPlatformOnTrackPrevWaypointFlags == WAYPOINT_FLAGS_END) {
                    o->oAction = PLATFORM_ON_TRACK_ACT_INIT;
                } else {
                    o->oAction = PLATFORM_ON_TRACK_ACT_PAUSE_BRIEFLY;
                }
            }
        } else {

            if (!o->oPlatformOnTrackIsNotSkiLift) {
                obj_forward_vel_approach(10.0, 0.1f);
            } else {
                o->oForwardVel = 10.0f;
            }

            if (approach_f32_ptr(&o->oPlatformOnTrackDistMovedSinceLastBall, 300.0f, o->oForwardVel)) {
                o->oPlatformOnTrackDistMovedSinceLastBall -= 300.0f;

                o->oHomeX = o->oPosX;
                o->oHomeY = o->oPosY;
                o->oHomeZ = o->oPosZ;
                o->oPlatformOnTrackBaseBallIndex = (u16)(o->oPlatformOnTrackBaseBallIndex + 1);

                platform_on_track_update_pos_or_spawn_ball(5, o->oHomeX, o->oHomeY, o->oHomeZ);
            }
        }

        platform_on_track_update_pos_or_spawn_ball(0, o->oPosX, o->oPosY, o->oPosZ);

        o->oMoveAnglePitch = o->oPlatformOnTrackPitch;
        o->oMoveAngleYaw = o->oPlatformOnTrackYaw;

        if (!((u16)(o->oBhvParams >> 16) & PLATFORM_ON_TRACK_BP_DONT_TURN_YAW)) {
            s16 targetFaceYaw = o->oMoveAngleYaw + 0x4000;
            s16 yawSpeed = abs_angle_diff(targetFaceYaw, o->oFaceAngleYaw) / 20;

            initialAngle = o->oFaceAngleYaw;
            clamp_s16(&yawSpeed, 100, 500);
            obj_face_yaw_approach(targetFaceYaw, yawSpeed);
            o->oAngleVelYaw = (s16) o->oFaceAngleYaw - initialAngle;
        }

        if (((u16)(o->oBhvParams >> 16) & PLATFORM_ON_TRACK_BP_DONT_TURN_ROLL)) {
            s16 rollSpeed = abs_angle_diff(o->oMoveAnglePitch, o->oFaceAngleRoll) / 20;

            initialAngle = o->oFaceAngleRoll;
            clamp_s16(&rollSpeed, 100, 500);

            obj_face_roll_approach(o->oMoveAnglePitch, rollSpeed);
            o->oAngleVelRoll = (s16) o->oFaceAngleRoll - initialAngle;
        }
    }

    if (gMarioObject->platform != o) {
        platform_on_track_mario_not_on_platform();
    } else {
        o->oTimer = 0;
        o->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
    }
}

static void platform_on_track_act_pause_briefly(void) {
    if (o->oTimer > 20) {
        o->oAction = PLATFORM_ON_TRACK_ACT_MOVE_ALONG_TRACK;
    }
}

static void platform_on_track_act_fall(void) {
    cur_obj_move_using_vel_and_gravity();

    if (gMarioObject->platform != o) {
        platform_on_track_mario_not_on_platform();
    } else {
        o->oTimer = 0;

    }
}

static void platform_on_track_rock_ski_lift(void) {
    s32 targetRoll = 0;
    UNUSED s32 initialRoll = o->oFaceAngleRoll;

    o->oFaceAngleRoll += (s32) o->oPlatformOnTrackSkiLiftRollVel;

    if (gMarioObject->platform == o) {
        targetRoll = o->oForwardVel * sins(o->oMoveAngleYaw) * -50.0f
                     + (s32)(o->oDistanceToMario * sins(o->oAngleToMario - o->oFaceAngleYaw) * -4.0f);
    }

    oscillate_toward(
         &o->oFaceAngleRoll,
         &o->oPlatformOnTrackSkiLiftRollVel,
         targetRoll,
         5.0f,
         6.0f,
         1.5f);
    clamp_f32(&o->oPlatformOnTrackSkiLiftRollVel, -100.0f, 100.0f);
}

void bhv_platform_on_track_update(void) {
    switch (o->oAction) {
        case PLATFORM_ON_TRACK_ACT_INIT:
            platform_on_track_act_init();
            break;
        case PLATFORM_ON_TRACK_ACT_WAIT_FOR_MARIO:
            platform_on_track_act_wait_for_mario();
            break;
        case PLATFORM_ON_TRACK_ACT_MOVE_ALONG_TRACK:
            platform_on_track_act_move_along_track();
            break;
        case PLATFORM_ON_TRACK_ACT_PAUSE_BRIEFLY:
            platform_on_track_act_pause_briefly();
            break;
        case PLATFORM_ON_TRACK_ACT_FALL:
            platform_on_track_act_fall();
            break;
    }

    if (!o->oPlatformOnTrackIsNotSkiLift) {
        platform_on_track_rock_ski_lift();
    } else if (o->oPlatformOnTrackType == PLATFORM_ON_TRACK_TYPE_CARPET) {
        if (!o->oPlatformOnTrackWasStoodOn && gMarioObject->platform == o) {
            o->oPlatformOnTrackOffsetY = -8.0f;
            o->oPlatformOnTrackWasStoodOn = TRUE;
        }

        approach_f32_ptr(&o->oPlatformOnTrackOffsetY, 0.0f, 0.5f);
        o->oPosY += o->oPlatformOnTrackOffsetY;
    }
}

void bhv_track_ball_update(void) {

    s16 relativeIndex =
        (s16) o->oBhvParams2ndByte - (s16) o->parentObj->oPlatformOnTrackBaseBallIndex - 1;
    if (relativeIndex < 1 || relativeIndex > 5) {
        obj_mark_for_deletion(o);
    }
}
