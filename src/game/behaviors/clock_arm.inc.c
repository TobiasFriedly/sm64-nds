
void bhv_rotating_clock_arm_loop(void) {
    struct Surface *marioSurface;
    u16 rollAngle = o->oFaceAngleRoll;

    o->oFloorHeight =
        find_floor(gMarioObject->oPosX, gMarioObject->oPosY, gMarioObject->oPosZ, &marioSurface);

    if (o->oAction == 0) {
        if (marioSurface->type == SURFACE_DEFAULT && o->oTimer >= 4) {
            o->oAction++;
        }
    } else if (o->oAction == 1) {

        if (marioSurface != NULL
            && (marioSurface->type == SURFACE_TTC_PAINTING_1
                || marioSurface->type == SURFACE_TTC_PAINTING_2
                || marioSurface->type == SURFACE_TTC_PAINTING_3)) {

            if (cur_obj_has_behavior(bhvClockMinuteHand)) {

                if (rollAngle < 0xAAA) {
                    gTTCSpeedSetting = TTC_SPEED_STOPPED;
                } else if (rollAngle < 0x6AA4) {
                    gTTCSpeedSetting = TTC_SPEED_FAST;
                } else if (rollAngle < 0x954C) {
                    gTTCSpeedSetting = TTC_SPEED_RANDOM;
                } else if (rollAngle < 0xF546) {
                    gTTCSpeedSetting = TTC_SPEED_SLOW;
                } else {
                    gTTCSpeedSetting = TTC_SPEED_STOPPED;
                }
            }

            o->oAction++;
        } else {
        }
    }

    if (o->oAction < 2) {
        cur_obj_rotate_face_angle_using_vel();
    }
}
