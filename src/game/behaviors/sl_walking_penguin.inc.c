
struct SLWalkingPenguinStep {
    s32 stepLength;
    s32 anim;
    f32 speed;
    f32 animSpeed;
};

struct SLWalkingPenguinStep sSLWalkingPenguinErraticSteps[] = {
    { 60, PENGUIN_ANIM_WALK, 6.0f,  1.0f },
    { 30, PENGUIN_ANIM_IDLE, 0.0f,  1.0f },
    { 30, PENGUIN_ANIM_WALK, 12.0f, 2.0f },
    { 30, PENGUIN_ANIM_IDLE, 0.0f,  1.0f },
    { 30, PENGUIN_ANIM_WALK, -6.0f, 1.0f },
    { 30, PENGUIN_ANIM_IDLE, 0.0f,  1.0f },
    { -1, 0, 0.0f,  0.0f },
};

static s32 sl_walking_penguin_turn(void) {

    o->oForwardVel = 0.0f;
    cur_obj_init_animation_with_accel_and_sound(PENGUIN_ANIM_WALK, 1.0f);

    o->oAngleVelYaw = 0x400;
    o->oMoveAngleYaw += o->oAngleVelYaw;

    if (o->oTimer == 31) {
        return TRUE;
    } else {
        return FALSE;
    }
}

void bhv_sl_walking_penguin_loop(void) {
    f32 adjustedXPos, adjustedZPos;
    f32 perpendicularOffset = 100.0f;

    o->oAngleVelYaw = 0;
    cur_obj_update_floor_and_walls();

    switch (o->oAction) {

        case SL_WALKING_PENGUIN_ACT_MOVING_FORWARDS:
            if (o->oTimer == 0) {

                o->oSLWalkingPenguinCurStep = 0;
                o->oSLWalkingPenguinCurStepTimer = 0;
            }

            if (o->oSLWalkingPenguinCurStepTimer <
                sSLWalkingPenguinErraticSteps[o->oSLWalkingPenguinCurStep].stepLength) {
                o->oSLWalkingPenguinCurStepTimer++;
            } else {

                o->oSLWalkingPenguinCurStepTimer = 0;
                o->oSLWalkingPenguinCurStep++;
                if (sSLWalkingPenguinErraticSteps[o->oSLWalkingPenguinCurStep].stepLength < 0) {

                    o->oSLWalkingPenguinCurStep = 0;
                }
            }

            if (o->oPosX < 300.0f) {
                o->oAction++;
            } else {

                o->oForwardVel = sSLWalkingPenguinErraticSteps[o->oSLWalkingPenguinCurStep].speed;

                cur_obj_init_animation_with_accel_and_sound(
                    sSLWalkingPenguinErraticSteps[o->oSLWalkingPenguinCurStep].anim,
                    sSLWalkingPenguinErraticSteps[o->oSLWalkingPenguinCurStep].animSpeed
                );
            }
            break;

        case SL_WALKING_PENGUIN_ACT_TURNING_BACK:
            if (sl_walking_penguin_turn()) {
                o->oAction++;
            }
            break;

        case SL_WALKING_PENGUIN_ACT_RETURNING:

            o->oForwardVel = 12.0f;
            cur_obj_init_animation_with_accel_and_sound(PENGUIN_ANIM_WALK, 2.0f);

            if (o->oPosX > 1700.0f) {
                o->oAction++;
            }
            break;

        case SL_WALKING_PENGUIN_ACT_TURNING_FORWARDS:
            if (sl_walking_penguin_turn()) {
                o->oAction = SL_WALKING_PENGUIN_ACT_MOVING_FORWARDS;
            }
            break;
    }

    cur_obj_move_standard(-78);
    if (!cur_obj_hide_if_mario_far_away_y(1000.0f)) {
        play_penguin_walking_sound(PENGUIN_WALK_BIG);
    }

    adjustedXPos = o->oPosX + sins(0xDBB0) * 60.0f;
    adjustedZPos = o->oPosZ + coss(0xDBB0) * 60.0f;
    adjustedXPos += perpendicularOffset * sins(0x1BB0);
    adjustedZPos += perpendicularOffset * coss(0x1BB0);
    o->oSLWalkingPenguinWindCollisionXPos = adjustedXPos;
    o->oSLWalkingPenguinWindCollisionZPos = adjustedZPos;

    print_debug_bottom_up("x %d", o->oPosX);
    print_debug_bottom_up("z %d", o->oPosZ);
}
