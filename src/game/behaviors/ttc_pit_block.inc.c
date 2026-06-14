
static Collision const *sTTCPitBlockCollisionModels[] = {
    ttc_seg7_collision_07015754,
    ttc_seg7_collision_070157D8,
};

struct TTCPitBlockProperties {
    s16 speed;
    s16 waitTime;
};

static struct TTCPitBlockProperties sTTCPitBlockProperties[][2] = {
     { { 11, 20 }, { -9, 30 } },
     { { 18, 15 }, { -11, 15 } },
     { { 11, 20 }, { -9, -1 } },
     { { 0, 0 }, { 0, 0 } },
};

void bhv_ttc_pit_block_init(void) {
    o->collisionData = segmented_to_virtual(sTTCPitBlockCollisionModels[o->oBhvParams2ndByte]);

    o->oTTCPitBlockPeakY = o->oPosY + 330.0f;

    if (gTTCSpeedSetting == TTC_SPEED_STOPPED) {
        o->oPosY += 330.0f;
    }
}

void bhv_ttc_pit_block_update(void) {
    if (o->oTimer > o->oTTCPitBlockWaitTime) {

        cur_obj_move_using_fvel_and_gravity();

        if (clamp_f32(&o->oPosY, o->oHomeY, o->oTTCPitBlockPeakY)) {
            o->oTTCPitBlockDir = o->oTTCPitBlockDir ^ 0x01;

            if ((o->oTTCPitBlockWaitTime =
                     sTTCPitBlockProperties[gTTCSpeedSetting][o->oTTCPitBlockDir & 0x01].waitTime)
                < 0) {
                o->oTTCPitBlockWaitTime = random_mod_offset(10, 20, 6);
            }

            o->oVelY = sTTCPitBlockProperties[gTTCSpeedSetting][o->oTTCPitBlockDir].speed;
            o->oTimer = 0;
        }
    }
}
