
static Collision const *sSeesawPlatformCollisionModels[] = {
    bitdw_seg7_collision_0700F70C,
    bits_seg7_collision_0701ADD8,
    bits_seg7_collision_0701AE5C,
    bob_seg7_collision_bridge,
    bitfs_seg7_collision_07015928,
    rr_seg7_collision_07029750,
    rr_seg7_collision_07029858,
    vcutm_seg7_collision_0700AC44,
};

void bhv_seesaw_platform_init(void) {
    o->collisionData = segmented_to_virtual(sSeesawPlatformCollisionModels[o->oBhvParams2ndByte]);

    if (o->oBhvParams2ndByte == 2) {
        o->oCollisionDistance = 2000.0f;
    }
}

void bhv_seesaw_platform_update(void) {
    UNUSED s32 startPitch = o->oFaceAnglePitch;
    o->oFaceAnglePitch += (s32) o->oSeesawPlatformPitchVel;

    if (absf(o->oSeesawPlatformPitchVel) > 10.0f) {
        cur_obj_play_sound_1(SOUND_ENV_BOAT_ROCKING1);
    }

    if (gMarioObject->platform == o) {

        f32 rotation = o->oDistanceToMario * coss(o->oAngleToMario - o->oMoveAngleYaw);
        UNUSED u8 filler[4];

        if (o->oSeesawPlatformPitchVel * rotation < 0) {
            rotation *= 0.04f;
        } else {
            rotation *= 0.02f;
        }

        o->oSeesawPlatformPitchVel += rotation;
        clamp_f32(&o->oSeesawPlatformPitchVel, -50.0f, 50.0f);
    } else {

        oscillate_toward(
             &o->oFaceAnglePitch,
             &o->oSeesawPlatformPitchVel,
             0.0f,
             6.0f,
             3.0f,
             3.0f);
    }
}
