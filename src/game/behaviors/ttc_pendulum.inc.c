
static f32 sTTCPendulumInitialAccels[] = {
     13.0f,
     22.0f,
     13.0f,
     0.0f,
};

void bhv_ttc_pendulum_init(void) {
    if (gTTCSpeedSetting != TTC_SPEED_STOPPED) {
        o->oTTCPendulumAngleAccel = sTTCPendulumInitialAccels[gTTCSpeedSetting];
        o->oTTCPendulumAngle = 6500.0f;
    } else {
        o->oTTCPendulumAngle = 6371.5557f;
    }
}

void bhv_ttc_pendulum_update(void) {
    if (gTTCSpeedSetting != TTC_SPEED_STOPPED) {
        UNUSED f32 startVel = o->oTTCPendulumAngleVel;

        if (o->oTTCPendulumSoundTimer != 0) {
            if (--o->oTTCPendulumSoundTimer == 0) {
                cur_obj_play_sound_2(SOUND_GENERAL_PENDULUM_SWING);
            }
        }

        if (o->oTTCPendulumDelay != 0) {
            o->oTTCPendulumDelay--;
        } else {

            if (o->oTTCPendulumAngle * o->oTTCPendulumAccelDir > 0.0f) {
                o->oTTCPendulumAccelDir = -o->oTTCPendulumAccelDir;
            }
            o->oTTCPendulumAngleVel += o->oTTCPendulumAngleAccel * o->oTTCPendulumAccelDir;

            if (o->oTTCPendulumAngleVel == 0.0f) {
                if (gTTCSpeedSetting == TTC_SPEED_RANDOM) {

                    if (random_u16() % 3 != 0) {
                        o->oTTCPendulumAngleAccel = 13.0f;
                    } else {
                        o->oTTCPendulumAngleAccel = 42.0f;
                    }

                    if (random_u16() % 2 == 0) {
                        o->oTTCPendulumDelay = random_linear_offset(5, 30);
                    }
                }

                o->oTTCPendulumSoundTimer = o->oTTCPendulumDelay + 15;
            }

            o->oTTCPendulumAngle += o->oTTCPendulumAngleVel;
        }
    } else {
    }

    o->oFaceAngleRoll = (s32) o->oTTCPendulumAngle;

}
