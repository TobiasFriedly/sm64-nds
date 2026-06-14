
static s8 sTTCElevatorSpeeds[] = {
     6,
     10,
     6,
     0,
};

void bhv_ttc_elevator_init(void) {

    f32 peakOffset =
        ((o->oBhvParams >> 16) & 0xFFFF) ? 100.0f * ((o->oBhvParams >> 16) & 0xFFFF) : 500.0f;

    o->oTTCElevatorPeakY = o->oPosY + peakOffset;
}

void bhv_ttc_elevator_update(void) {
    o->oVelY = sTTCElevatorSpeeds[gTTCSpeedSetting] * o->oTTCElevatorDir;

    if (gTTCSpeedSetting == TTC_SPEED_RANDOM) {

        if (o->oTimer > o->oTTCElevatorMoveTime) {
            o->oTTCElevatorDir = random_sign();
            o->oTTCElevatorMoveTime = random_mod_offset(30, 30, 6);
            o->oTimer = 0;
        } else if (o->oTimer < 5) {
            o->oVelY = 0.0f;
        }
    }

    cur_obj_move_using_fvel_and_gravity();

    if (clamp_f32(&o->oPosY, o->oHomeY, o->oTTCElevatorPeakY)) {
        o->oTTCElevatorDir = -o->oTTCElevatorDir;
    }
}
