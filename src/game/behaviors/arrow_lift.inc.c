
static s32 arrow_lift_move_away(void) {
    s8 status = ARROW_LIFT_NOT_DONE_MOVING;

    o->oMoveAngleYaw = o->oFaceAngleYaw - 0x4000;
    o->oVelY = 0;
    o->oForwardVel = 12;

    o->oArrowLiftDisplacement += o->oForwardVel;

    if (o->oArrowLiftDisplacement > 384) {
        o->oForwardVel = 0;
        o->oArrowLiftDisplacement = 384;
        status = ARROW_LIFT_DONE_MOVING;
    }

    obj_move_xyz_using_fvel_and_yaw(o);
    return status;
}

static s8 arrow_lift_move_back(void) {
    s8 status = ARROW_LIFT_NOT_DONE_MOVING;

    o->oMoveAngleYaw = o->oFaceAngleYaw + 0x4000;
    o->oVelY = 0;
    o->oForwardVel = 12;
    o->oArrowLiftDisplacement -= o->oForwardVel;

    if (o->oArrowLiftDisplacement < 0) {
        o->oForwardVel = 0;
        o->oArrowLiftDisplacement = 0;
        status = ARROW_LIFT_DONE_MOVING;
    }

    obj_move_xyz_using_fvel_and_yaw(o);
    return status;
}

void bhv_arrow_lift_loop(void) {
    switch (o->oAction) {
        case ARROW_LIFT_ACT_IDLE:

            if (o->oTimer > 60) {
                if (gMarioObject->platform == o) {
                    o->oAction = ARROW_LIFT_ACT_MOVING_AWAY;
                }
            }

            break;

        case ARROW_LIFT_ACT_MOVING_AWAY:
            if (arrow_lift_move_away() == ARROW_LIFT_DONE_MOVING) {
                o->oAction = ARROW_LIFT_ACT_MOVING_BACK;
            }

            break;

        case ARROW_LIFT_ACT_MOVING_BACK:

            if (o->oTimer > 60) {
                if (arrow_lift_move_back() == ARROW_LIFT_DONE_MOVING) {
                    o->oAction = ARROW_LIFT_ACT_IDLE;
                }
            }

            break;
    }
}
