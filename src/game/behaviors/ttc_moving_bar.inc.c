
static s16 sTTCMovingBarDelays[] = {
     55,
     30,
     55,
     0,
};

static s8 sTTCMovingBarRandomDelays[] = { 1, 12, 55, 100 };

void bhv_ttc_moving_bar_init(void) {

    if ((o->oTTCMovingBarDelay = sTTCMovingBarDelays[gTTCSpeedSetting]) == 0) {
        o->oTTCMovingBarOffset = 250.0f;
    }

    o->oTTCMovingBarStoppedTimer = 10 * o->oBhvParams2ndByte;

    o->oMoveAngleYaw = 0x4000 - o->oMoveAngleYaw;
}

static void ttc_moving_bar_act_wait(void) {
    if (o->oTTCMovingBarDelay != 0 && o->oTimer > o->oTTCMovingBarDelay) {

        if (o->oTTCMovingBarStoppedTimer != 0) {
            o->oTTCMovingBarStoppedTimer--;
        } else {
            if (gTTCSpeedSetting == TTC_SPEED_RANDOM) {

                o->oTTCMovingBarDelay = sTTCMovingBarRandomDelays[random_u16() & 0x03];

                if (random_u16() % 2 == 0) {
                    o->oTTCMovingBarStoppedTimer = random_linear_offset(20, 100);
                }
            }

            o->oAction = TTC_MOVING_BAR_ACT_PULL_BACK;
            o->oTTCMovingBarSpeed = -8.0f;
        }
    }
}

static void ttc_moving_bar_act_pull_back(void) {

    if ((o->oTTCMovingBarSpeed += 0.73f) > 0.0f) {

        if (o->oTTCMovingBarStoppedTimer != 0) {
            o->oTTCMovingBarStoppedTimer--;
            o->oTTCMovingBarSpeed = 0.0f;
        } else {

            o->oAction = TTC_MOVING_BAR_ACT_EXTEND;
            o->oTTCMovingBarSpeed = 29.0f;
        }
    }
}

static void ttc_moving_bar_reset(void) {
    o->oTTCMovingBarOffset = o->oTTCMovingBarSpeed = 0.0f;
    o->oAction = TTC_MOVING_BAR_ACT_WAIT;
}

static void ttc_moving_bar_act_extend(void) {

    if ((o->oTTCMovingBarOffset == 250.0f
         || (250.0f - o->oTTCMovingBarOffset) * (250.0f - o->oTTCMovingBarStartOffset) < 0.0f)
        && o->oTTCMovingBarSpeed > -8.0f && o->oTTCMovingBarSpeed < 8.0f) {

        o->oAction = TTC_MOVING_BAR_ACT_RETRACT;
        o->oTTCMovingBarSpeed = 0.0f;
    } else {
        f32 accel;

        if (o->oTTCMovingBarOffset < 250.0f) {
            accel = 6.4f;
        } else {
            accel = -6.4f;
        }

        if (o->oTTCMovingBarSpeed * accel < 0.0f) {
            accel *= 2.35f;
        }

        o->oTTCMovingBarSpeed += accel;

        if (gTTCSpeedSetting == TTC_SPEED_RANDOM
            && o->oTTCMovingBarOffset * o->oTTCMovingBarStartOffset < 0.0f && random_u16() % 4 == 0) {
            ttc_moving_bar_reset();
        }
    }
}

static void ttc_moving_bar_act_retract(void) {

    if (o->oTimer > 30) {

        o->oTTCMovingBarSpeed = -5.0f;
        if (o->oTTCMovingBarOffset < 0.0f) {
            ttc_moving_bar_reset();
        }
    }
}

void bhv_ttc_moving_bar_update(void) {
    o->oTTCMovingBarStartOffset = o->oTTCMovingBarOffset;
    obj_perform_position_op(POS_OP_SAVE_POSITION);

    o->oTTCMovingBarOffset += o->oTTCMovingBarSpeed;

    switch (o->oAction) {
        case TTC_MOVING_BAR_ACT_WAIT:
            ttc_moving_bar_act_wait();
            break;
        case TTC_MOVING_BAR_ACT_PULL_BACK:
            ttc_moving_bar_act_pull_back();
            break;
        case TTC_MOVING_BAR_ACT_EXTEND:
            ttc_moving_bar_act_extend();
            break;
        case TTC_MOVING_BAR_ACT_RETRACT:
            ttc_moving_bar_act_retract();
            break;
    }

    obj_set_dist_from_home(o->oTTCMovingBarOffset);
    obj_perform_position_op(POS_OP_COMPUTE_VELOCITY);
}
