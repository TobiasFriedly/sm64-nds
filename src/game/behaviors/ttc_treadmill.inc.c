
static Collision const *sTTCTreadmillCollisionModels[] = {
    ttc_seg7_collision_070152B4,
    ttc_seg7_collision_070153E0,
};

static s16 sTTCTreadmillSpeeds[] = {
     50,
     100,
     0,
     0,
};

extern s16 ttc_movtex_tris_big_surface_treadmill[];
extern s16 ttc_movtex_tris_small_surface_treadmill[];

void bhv_ttc_treadmill_init(void) {
    o->collisionData = segmented_to_virtual(
        sTTCTreadmillCollisionModels[o->oBhvParams2ndByte & TTC_TREADMILL_BP_FLAG_MASK]);

    o->oTTCTreadmillBigSurface = segmented_to_virtual(ttc_movtex_tris_big_surface_treadmill);
    o->oTTCTreadmillSmallSurface = segmented_to_virtual(ttc_movtex_tris_small_surface_treadmill);

    *o->oTTCTreadmillBigSurface = *o->oTTCTreadmillSmallSurface = sTTCTreadmillSpeeds[gTTCSpeedSetting];

    sMasterTreadmill = NULL;
}

void bhv_ttc_treadmill_update(void) {
    if (sMasterTreadmill == o || sMasterTreadmill == NULL) {
        sMasterTreadmill = o;

        cur_obj_play_sound_1(SOUND_ENV_ELEVATOR2);

        if (gTTCSpeedSetting == TTC_SPEED_RANDOM) {

            if (o->oTimer > o->oTTCTreadmillTimeUntilSwitch) {

                if (approach_f32_ptr(&o->oTTCTreadmillSpeed, 0.0f, 10.0f)) {
                    o->oTTCTreadmillTimeUntilSwitch = random_mod_offset(10, 20, 7);
                    o->oTTCTreadmillTargetSpeed = random_sign() * 50.0f;
                    o->oTimer = 0;
                }
            } else if (o->oTimer > 5) {
                approach_f32_ptr(&o->oTTCTreadmillSpeed, o->oTTCTreadmillTargetSpeed, 10.0f);
            }

            *o->oTTCTreadmillBigSurface = *o->oTTCTreadmillSmallSurface = o->oTTCTreadmillSpeed;
        }
    }

    o->oForwardVel = 0.084f * *o->oTTCTreadmillBigSurface;
}
