
static void bird_act_inactive(void) {

    if (o->oBhvParams2ndByte == BIRD_BP_SPAWNED || o->oDistanceToMario < 2000.0f) {

        if (o->oBhvParams2ndByte != BIRD_BP_SPAWNED) {
            s32 i;

            cur_obj_play_sound_2(SOUND_GENERAL_BIRDS_FLY_AWAY);

            for (i = 0; i < 6; i++) {
                spawn_object(o, MODEL_BIRDS, bhvBird);
            }

            o->oHomeX = -20.0f;
            o->oHomeZ = -3990.0f;
        }

        o->oAction = BIRD_ACT_FLY;

        o->oMoveAnglePitch = 5000 - (s32)(4000.0f * random_float());
        o->oMoveAngleYaw = random_u16();

        o->oBirdSpeed = 40.0f;

        cur_obj_unhide();
    }
}

static void bird_act_fly(void) {
    UNUSED u8 filler[4];
    f32 distance;

    obj_compute_vel_from_move_pitch(o->oBirdSpeed);

    if (o->parentObj->oPosY > 8000.0f) {
        obj_mark_for_deletion(o);
    } else {

        if (o->oBhvParams2ndByte != BIRD_BP_SPAWNED) {
            distance = cur_obj_lateral_dist_to_home();

            o->oBirdTargetPitch = atan2s(distance, o->oPosY - 10000.0f);
            o->oBirdTargetYaw = cur_obj_angle_to_home();
        } else {
            distance = lateral_dist_between_objects(o, o->parentObj);

            o->oBirdTargetPitch = atan2s(distance, o->oPosY - o->parentObj->oPosY);
            o->oBirdTargetYaw = obj_angle_to_object(o, o->parentObj);

            o->oBirdSpeed = 0.04f * dist_between_objects(o, o->parentObj) + 20.0f;
        }

        obj_move_pitch_approach(o->oBirdTargetPitch, 140);
        cur_obj_rotate_yaw_toward(o->oBirdTargetYaw, 800);
        obj_roll_to_match_yaw_turn(o->oBirdTargetYaw, 0x3000, 600);
    }

    cur_obj_move_using_fvel_and_gravity();
}

void bhv_bird_update(void) {
    switch (o->oAction) {
        case BIRD_ACT_INACTIVE:
            bird_act_inactive();
            break;
        case BIRD_ACT_FLY:
            bird_act_fly();
            break;
    }
}
