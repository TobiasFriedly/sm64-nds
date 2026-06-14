
static struct ObjectHitbox sFlyGuyHitbox = {
     INTERACT_BOUNCE_TOP,
     0,
     2,
     0,
     2,
     70,
     60,
     40,
     50,
};

static s16 sFlyGuyJitterAmounts[] = { 0x1000, -0x2000, 0x2000 };

static void fly_guy_act_idle(void) {
    o->oForwardVel = 0.0f;

    if (approach_f32_ptr(&o->header.gfx.scale[0], 1.5f, 0.02f)) {

        if (o->oDistanceToMario >= 25000.0f || o->oDistanceToMario < 2000.0f) {

            obj_face_yaw_approach(o->oAngleToMario, 0x300);

            if (cur_obj_rotate_yaw_toward(o->oAngleToMario, 0x300)) {
                o->oAction = FLY_GUY_ACT_APPROACH_MARIO;
            }
        } else {

            if (o->oFlyGuyIdleTimer >= 3 || o->oFlyGuyIdleTimer == (random_u16() & 1) + 2) {
                o->oFlyGuyIdleTimer = 0;
                o->oAction = FLY_GUY_ACT_APPROACH_MARIO;
            } else {
                o->oFlyGuyUnusedJitter = o->oMoveAngleYaw + sFlyGuyJitterAmounts[o->oFlyGuyIdleTimer];
                o->oFlyGuyIdleTimer++;
            }
        }
    }
}

static void fly_guy_act_approach_mario(void) {

    if (o->oDistanceToMario >= 25000.0f || o->oDistanceToMario < 2000.0f) {
        obj_forward_vel_approach(10.0f, 0.5f);

        obj_face_yaw_approach(o->oAngleToMario, 0x400);
        cur_obj_rotate_yaw_toward(o->oAngleToMario, 0x200);

        if (abs_angle_diff(o->oAngleToMario, o->oFaceAngleYaw) < 0x2000
            && (o->oPosY - gMarioObject->oPosY > 400.0f || o->oDistanceToMario < 400.0f)) {

            if (o->oBhvParams2ndByte != FLY_GUY_BP_GENERIC && random_u16() % 2) {
                o->oAction = FLY_GUY_ACT_SHOOT_FIRE;
                o->oFlyGuyScaleVel = 0.06f;
            } else {
                o->oAction = FLY_GUY_ACT_LUNGE;
                o->oFlyGuyLungeTargetPitch = obj_turn_pitch_toward_mario(-200.0f, 0);

                o->oForwardVel = 25.0f * coss(o->oFlyGuyLungeTargetPitch);
                o->oVelY = 25.0f * -sins(o->oFlyGuyLungeTargetPitch);
                o->oFlyGuyLungeYDecel = -o->oVelY / 30.0f;
            }
        }
    } else if (obj_forward_vel_approach(0.0f, 0.2f)) {
        o->oAction = FLY_GUY_ACT_IDLE;
    }
}

static void fly_guy_act_lunge(void) {
    if (o->oVelY < 0.0f) {

        o->oVelY += o->oFlyGuyLungeYDecel;

        cur_obj_rotate_yaw_toward(o->oFaceAngleYaw, 0x800);
        obj_face_pitch_approach(o->oFlyGuyLungeTargetPitch, 0x400);

        o->oFlyGuyTargetRoll = 0x1000 * (s16)(random_float() * 3.0f) - 0x1000;
        o->oTimer = 0;
    } else {

        obj_face_pitch_approach(0, 0x100);
        obj_face_roll_approach(o->oFlyGuyTargetRoll, 300);

        o->oMoveAngleYaw -= o->oFaceAngleRoll / 4;
        obj_face_yaw_approach(o->oMoveAngleYaw, 0x800);

        if (o->oPosY < gMarioObject->oPosY + 200.0f) {
            obj_y_vel_approach(20.0f, 0.5f);
        } else if (obj_y_vel_approach(0.0f, 0.5f)) {

            if (o->oFaceAngleRoll == 0) {
                o->oAction = FLY_GUY_ACT_APPROACH_MARIO;
            }

            o->oFlyGuyTargetRoll = 0;
        }
    }
}

static void fly_guy_act_shoot_fire(void) {
    o->oForwardVel = 0.0f;

    if (obj_face_yaw_approach(o->oAngleToMario, 0x800)) {
        s32 scaleStatus;

        o->oMoveAngleYaw = o->oFaceAngleYaw;

        scaleStatus = obj_grow_then_shrink(&o->oFlyGuyScaleVel, 1.2f, 1.1f);

        if (scaleStatus != 0) {
            if (scaleStatus < 0) {

                o->oAction = FLY_GUY_ACT_IDLE;
            } else {

                s16 fireMovePitch = obj_turn_pitch_toward_mario(0.0f, 0);

                cur_obj_play_sound_2(SOUND_OBJ_FLAME_BLOWN);
                clamp_s16(&fireMovePitch, 0x800, 0x3000);

                obj_spit_fire(
                     0, 38, 20,
                     2.5f,
                     MODEL_RED_FLAME_SHADOW,
                     25.0f,
                     20.0f,
                     fireMovePitch);
            }
        }
    } else {

        o->oTimer = 0;
    }
}

void bhv_fly_guy_update(void) {

    if (!(o->activeFlags & ACTIVE_FLAG_IN_DIFFERENT_ROOM)) {
        o->oDeathSound = SOUND_OBJ_KOOPA_FLYGUY_DEATH;

        cur_obj_scale(o->header.gfx.scale[0]);
        treat_far_home_as_mario(2000.0f);
        cur_obj_update_floor_and_walls();

        if (o->oMoveFlags & OBJ_MOVE_HIT_WALL) {
            o->oMoveAngleYaw = cur_obj_reflect_move_angle_off_wall();
        } else if (o->oMoveFlags & OBJ_MOVE_MASK_IN_WATER) {
            o->oVelY = 6.0f;
        }

        o->oFlyGuyOscTimer++;
        o->oPosY += coss(0x400 * o->oFlyGuyOscTimer) * 1.5f;

        switch (o->oAction) {
            case FLY_GUY_ACT_IDLE:
                fly_guy_act_idle();
                break;
            case FLY_GUY_ACT_APPROACH_MARIO:
                fly_guy_act_approach_mario();
                break;
            case FLY_GUY_ACT_LUNGE:
                fly_guy_act_lunge();
                break;
            case FLY_GUY_ACT_SHOOT_FIRE:
                fly_guy_act_shoot_fire();
                break;
        }

        cur_obj_move_standard(78);
        obj_check_attacks(&sFlyGuyHitbox, o->oAction);
    }
}
