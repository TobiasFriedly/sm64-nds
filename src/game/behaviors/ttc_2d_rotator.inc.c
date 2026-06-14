
static s16 sTTC2DRotatorSpeeds[] = {
    -0x444,
    -0xCCC,
};

static s16 sTTC2DRotatorTimeBetweenTurns[][4] = {
    {
         40,
         10,
         10,
         0,
    },
    {
         20,
         5,
         5,
         0,
    },
};

void bhv_ttc_2d_rotator_init(void) {
    o->oTTC2DRotatorMinTimeUntilNextTurn =
        sTTC2DRotatorTimeBetweenTurns[o->oBhvParams2ndByte][gTTCSpeedSetting];
    o->oTTC2DRotatorIncrement = o->oTTC2DRotatorSpeed = sTTC2DRotatorSpeeds[o->oBhvParams2ndByte];
}

void bhv_ttc_2d_rotator_update(void) {
    s32 startYaw = o->oFaceAngleYaw;

    if (o->oTTC2DRotatorRandomDirTimer != 0) {
        o->oTTC2DRotatorRandomDirTimer--;
    }

    if (o->oTTC2DRotatorMinTimeUntilNextTurn != 0
        && obj_face_yaw_approach(o->oTTC2DRotatorTargetYaw, 0xC8)) {

        if (o->oTimer > o->oTTC2DRotatorMinTimeUntilNextTurn) {

            o->oTTC2DRotatorTargetYaw += o->oTTC2DRotatorIncrement;
            o->oTimer = 0;

            if (gTTCSpeedSetting == TTC_SPEED_RANDOM) {

                if (o->oTTC2DRotatorRandomDirTimer == 0) {
                    if (random_u16() & 0x03) {
                        o->oTTC2DRotatorIncrement = o->oTTC2DRotatorSpeed;
                        o->oTTC2DRotatorRandomDirTimer = random_mod_offset(90, 60, 4);
                    } else {
                        o->oTTC2DRotatorIncrement = -o->oTTC2DRotatorSpeed;
                        o->oTTC2DRotatorRandomDirTimer = random_mod_offset(30, 30, 3);
                    }
                }

                o->oTTC2DRotatorMinTimeUntilNextTurn = random_mod_offset(10, 20, 3);
            }
        }
    }

    o->oAngleVelYaw = o->oFaceAngleYaw - startYaw;
    if (o->oBhvParams2ndByte == TTC_2D_ROTATOR_BP_HAND) {
        load_object_collision_model();
    }
}
