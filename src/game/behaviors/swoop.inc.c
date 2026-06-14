
static struct ObjectHitbox sSwoopHitbox = {
     INTERACT_HIT_FROM_BELOW,
     0,
     1,
     0,
     1,
     100,
     80,
     70,
     70,
};

static void swoop_act_idle(void) {
    cur_obj_init_animation_with_sound(1);

    if (approach_f32_ptr(&o->header.gfx.scale[0], 1.0f, 0.05f) && o->oDistanceToMario < 1500.0f) {
        if (cur_obj_rotate_yaw_toward(o->oAngleToMario, 800)) {
            cur_obj_play_sound_2(SOUND_OBJ2_SWOOP);
            o->oAction = SWOOP_ACT_MOVE;
            o->oVelY = -12.0f;
        }
    }

    o->oFaceAngleRoll = 0x8000;
}

static void swoop_act_move(void) {
    cur_obj_init_animation_with_accel_and_sound(0, 2.0f);
    if (cur_obj_check_if_near_animation_end()) {
        cur_obj_play_sound_2(SOUND_OBJ_UNKNOWN6);
    }

    if (o->oForwardVel == 0.0f) {

        if (obj_face_roll_approach(0, 2500)) {
            o->oForwardVel = 10.0f;
            o->oVelY = -10.0f;
        }
    } else if (cur_obj_mario_far_away()) {

        o->oAction = SWOOP_ACT_IDLE;
        cur_obj_set_pos_to_home();
        o->header.gfx.scale[0] = o->oForwardVel = o->oVelY = 0.0f;
        o->oFaceAngleRoll = 0;
    } else {
        if (o->oSwoopBonkCountdown != 0) {
            o->oSwoopBonkCountdown--;
        } else if (o->oVelY != 0.0f) {

            o->oSwoopTargetYaw = o->oAngleToMario;
            if (o->oPosY < gMarioObject->oPosY + 200.0f) {
                if (obj_y_vel_approach(0.0f, 0.5f)) {
                    o->oForwardVel *= 2.0f;
                }
            } else {
                obj_y_vel_approach(-10.0f, 0.5f);
            }
        } else if (o->oMoveFlags & OBJ_MOVE_HIT_WALL) {

            o->oSwoopTargetYaw = cur_obj_reflect_move_angle_off_wall();
            o->oSwoopBonkCountdown = 30;
        }

        if ((o->oSwoopTargetPitch = obj_get_pitch_from_vel()) == 0) {
            o->oSwoopTargetPitch += o->oForwardVel * 500;
        }
        obj_move_pitch_approach(o->oSwoopTargetPitch, 140);

        cur_obj_rotate_yaw_toward(o->oSwoopTargetYaw + (s32)(3000 * coss(4000 * gGlobalTimer)), 1200);
        obj_roll_to_match_yaw_turn(o->oSwoopTargetYaw, 0x3000, 500);

        o->oFaceAngleRoll += (s32)(1000 * coss(20000 * gGlobalTimer));
    }
}

void bhv_swoop_update(void) {

    if (!(o->activeFlags & ACTIVE_FLAG_IN_DIFFERENT_ROOM)) {
        o->oDeathSound = SOUND_OBJ_SWOOP_DEATH;

        cur_obj_update_floor_and_walls();

        switch (o->oAction) {
            case SWOOP_ACT_IDLE:
                swoop_act_idle();
                break;
            case SWOOP_ACT_MOVE:
                swoop_act_move();
                break;
        }

        cur_obj_scale(o->header.gfx.scale[0]);
        cur_obj_move_standard(78);

        obj_check_attacks(&sSwoopHitbox, o->oAction);
    }
}
