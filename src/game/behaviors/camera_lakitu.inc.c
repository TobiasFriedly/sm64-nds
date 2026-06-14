
void bhv_camera_lakitu_init(void) {
    if (o->oBhvParams2ndByte != CAMERA_LAKITU_BP_FOLLOW_CAMERA) {

        if (gNeverEnteredCastle != TRUE) {
            obj_mark_for_deletion(o);
        }
    } else {
        spawn_object_relative_with_scale(CLOUD_BP_LAKITU_CLOUD, 0, 0, 0, 2.0f, o, MODEL_MIST, bhvCloud);
    }
}

static void camera_lakitu_intro_act_trigger_cutscene(void) {

    if (gMarioObject->oPosX > -544.0f && gMarioObject->oPosX < 545.0f
        && gMarioObject->oPosY > 800.0f && gMarioObject->oPosZ > -2000.0f
        && gMarioObject->oPosZ < -177.0f && gMarioObject->oPosZ < -177.0f
        && set_mario_npc_dialog(MARIO_DIALOG_LOOK_UP) == MARIO_DIALOG_STATUS_START) {
        o->oAction = CAMERA_LAKITU_INTRO_ACT_SPAWN_CLOUD;
    }
}

static void camera_lakitu_intro_act_spawn_cloud(void) {
    if (set_mario_npc_dialog(MARIO_DIALOG_LOOK_UP) == MARIO_DIALOG_STATUS_SPEAK) {
        o->oAction = CAMERA_LAKITU_INTRO_ACT_UNK2;

        o->oPosX = 1800.0f;
        o->oPosY = 2400.0f;
        o->oPosZ = -2400.0f;

        o->oMoveAnglePitch = 0x4000;
        o->oCameraLakituSpeed = 60.0f;
        o->oCameraLakituCircleRadius = 1000.0f;

        spawn_object_relative_with_scale(CLOUD_BP_LAKITU_CLOUD, 0, 0, 0, 2.0f, o, MODEL_MIST, bhvCloud);
    }
}

static void camera_lakitu_intro_act_show_dialog(void) {
    s16 targetMovePitch;
    s16 targetMoveYaw;
#ifdef AVOID_UB
    targetMovePitch = 0;
    targetMoveYaw = 0;
#endif

    cur_obj_play_sound_1(SOUND_AIR_LAKITU_FLY);

    o->oFaceAnglePitch = obj_turn_pitch_toward_mario(120.0f, 0);
    o->oFaceAngleYaw = o->oAngleToMario;

    if (o->oCameraLakituFinishedDialog) {
        approach_f32_ptr(&o->oCameraLakituSpeed, 60.0f, 3.0f);
        if (o->oDistanceToMario > 6000.0f) {
            obj_mark_for_deletion(o);
        }

        targetMovePitch = -0x3000;
        targetMoveYaw = -0x6000;
    } else {
        if (o->oCameraLakituSpeed != 0.0f) {
            if (o->oDistanceToMario > 5000.0f) {
                targetMovePitch = o->oMoveAnglePitch;
                targetMoveYaw = o->oAngleToMario;
            } else {

                s16 turnAmount = 0x4000
                                 - atan2s(o->oCameraLakituCircleRadius,
                                          o->oDistanceToMario - o->oCameraLakituCircleRadius);
                if ((s16)(o->oMoveAngleYaw - o->oAngleToMario) < 0) {
                    turnAmount = -turnAmount;
                }

                targetMoveYaw = o->oAngleToMario + turnAmount;
                targetMovePitch = o->oFaceAnglePitch;

                approach_f32_ptr(&o->oCameraLakituCircleRadius, 200.0f, 50.0f);
                if (o->oDistanceToMario < 1000.0f) {
#ifndef VERSION_JP
                    if (!o->oCameraLakituUnk104) {
                        play_music(SEQ_PLAYER_LEVEL, SEQUENCE_ARGS(15, SEQ_EVENT_CUTSCENE_LAKITU), 0);
                        o->oCameraLakituUnk104 = TRUE;
                    }
#endif

                    approach_f32_ptr(&o->oCameraLakituSpeed, 20.0f, 1.0f);
                    if (o->oDistanceToMario < 500.0f
                        && abs_angle_diff(gMarioObject->oFaceAngleYaw, o->oFaceAngleYaw) > 0x7000) {

                        approach_f32_ptr(&o->oCameraLakituSpeed, 0.0f, 5.0f);
                    }
                }
            }
        } else if (cur_obj_update_dialog_with_cutscene(MARIO_DIALOG_LOOK_UP,
            DIALOG_FLAG_TURN_TO_MARIO, CUTSCENE_DIALOG, DIALOG_034)) {
            o->oCameraLakituFinishedDialog = TRUE;
        }
    }

    o->oCameraLakituPitchVel = approach_s16_symmetric(o->oCameraLakituPitchVel, 2000, 400);
    obj_move_pitch_approach(targetMovePitch, o->oCameraLakituPitchVel);

    o->oCameraLakituYawVel = approach_s16_symmetric(o->oCameraLakituYawVel, 2000, 100);
    cur_obj_rotate_yaw_toward(targetMoveYaw, o->oCameraLakituYawVel);

    obj_compute_vel_from_move_pitch(o->oCameraLakituSpeed);
    cur_obj_move_using_fvel_and_gravity();
}

void bhv_camera_lakitu_update(void) {
    if (!(o->activeFlags & ACTIVE_FLAG_IN_DIFFERENT_ROOM)) {
        obj_update_blinking(&o->oCameraLakituBlinkTimer, 20, 40, 4);

        if (o->oBhvParams2ndByte != CAMERA_LAKITU_BP_FOLLOW_CAMERA) {
            switch (o->oAction) {
                case CAMERA_LAKITU_INTRO_ACT_TRIGGER_CUTSCENE:
                    camera_lakitu_intro_act_trigger_cutscene();
                    break;
                case CAMERA_LAKITU_INTRO_ACT_SPAWN_CLOUD:
                    camera_lakitu_intro_act_spawn_cloud();
                    break;
                case CAMERA_LAKITU_INTRO_ACT_UNK2:
                    camera_lakitu_intro_act_show_dialog();
                    break;
            }
        } else {
            f32 val0C = 4331.53f - gLakituState.curPos[0];

            if (gLakituState.curPos[0] < 1700.0f || val0C < 0.0f) {
                cur_obj_hide();
            } else {
                cur_obj_unhide();

                o->oPosX = gLakituState.curPos[0];
                o->oPosY = gLakituState.curPos[1];
                o->oPosZ = gLakituState.curPos[2];

                o->oHomeX = gLakituState.curFocus[0];
                o->oHomeZ = gLakituState.curFocus[2];

                o->oFaceAngleYaw = -cur_obj_angle_to_home();
                o->oFaceAnglePitch = atan2s(cur_obj_lateral_dist_to_home(),
                                            o->oPosY - gLakituState.curFocus[1]);

                o->oPosX = 4331.53f + val0C;
            }
        }
    }
}
