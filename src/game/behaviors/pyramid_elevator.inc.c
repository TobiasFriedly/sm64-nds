
void bhv_pyramid_elevator_init(void) {
    s32 i;

    for (i = 0; i < 10; i++) {
        struct Object *ball = spawn_object(o, MODEL_TRAJECTORY_MARKER_BALL,
                                           bhvPyramidElevatorTrajectoryMarkerBall);
        ball->oPosY = 4600 - i * 460;
    }
}

void bhv_pyramid_elevator_loop(void) {
    switch (o->oAction) {

        case PYRAMID_ELEVATOR_ACT_IDLE:
            if (gMarioObject->platform == o) {
                o->oAction = PYRAMID_ELEVATOR_ACT_START_MOVING;
            }
            break;

        case PYRAMID_ELEVATOR_ACT_START_MOVING:
            o->oPosY = o->oHomeY - sins(o->oTimer * 0x1000) * 10.0f;
            if (o->oTimer == 8) {
                o->oAction = PYRAMID_ELEVATOR_ACT_CONSTANT_VELOCITY;
            }
            break;

        case PYRAMID_ELEVATOR_ACT_CONSTANT_VELOCITY:
            o->oVelY = -10.0f;
            o->oPosY += o->oVelY;
            if (o->oPosY < 128.0f) {
                o->oPosY = 128.0f;
                o->oAction = PYRAMID_ELEVATOR_ACT_AT_BOTTOM;
            }
            break;

        case PYRAMID_ELEVATOR_ACT_AT_BOTTOM:
            o->oPosY = sins(o->oTimer * 0x1000) * 10.0f + 128.0f;
            if (o->oTimer >= 8) {
                o->oVelY = 0;
                o->oPosY = 128.0f;
            }
            break;
    }
}

void bhv_pyramid_elevator_trajectory_marker_ball_loop(void) {
    struct Object *elevator;

    cur_obj_scale(0.15f);
    elevator = cur_obj_nearest_object_with_behavior(bhvPyramidElevator);

    if (elevator != NULL) {
        if (elevator->oAction != PYRAMID_ELEVATOR_ACT_IDLE) {
            o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        }
    }
}
