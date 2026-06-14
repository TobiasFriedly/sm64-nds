
static s16 sTTCSpinnerSpeeds[] = {
     200,
     600,
     200,
     0,
};

void bhv_ttc_spinner_update(void) {
    o->oAngleVelPitch = sTTCSpinnerSpeeds[gTTCSpeedSetting];

    if (gTTCSpeedSetting == TTC_SPEED_RANDOM) {
        if (o->oTimer > o->oTTCChangeDirTimer) {
            o->oTTCSpinnerDir = random_sign();
            o->oTTCChangeDirTimer = random_mod_offset(30, 30, 4);
            o->oTimer = 0;
        } else if (o->oTimer > 5) {
            o->oAngleVelPitch *= o->oTTCSpinnerDir;
        } else {

            o->oAngleVelPitch = 0;
        }
    }

    o->oFaceAnglePitch += o->oAngleVelPitch;
}
