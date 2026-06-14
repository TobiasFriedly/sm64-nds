
static Collision const *sTTCCogCollisionModels[] = {
    ttc_seg7_collision_07015584,
    ttc_seg7_collision_07015650,
};

static s8 sTTCCogDirections[] = { 1, -1 };

static s16 sTTCCogNormalSpeeds[] = { 200, 400 };

void bhv_ttc_cog_init(void) {
    o->collisionData = segmented_to_virtual(
        sTTCCogCollisionModels[(o->oBhvParams2ndByte & TTC_COG_BP_SHAPE_MASK) >> 1]);
    o->oTTCCogDir = sTTCCogDirections[o->oBhvParams2ndByte & TTC_COG_BP_DIR_MASK];
}

void bhv_ttc_cog_update(void) {
    switch (gTTCSpeedSetting) {
        case TTC_SPEED_SLOW:
        case TTC_SPEED_FAST:
            o->oTTCCogSpeed = sTTCCogNormalSpeeds[gTTCSpeedSetting];
            break;

        case TTC_SPEED_RANDOM:
            if (approach_f32_ptr(&o->oTTCCogSpeed, o->oTTCCogTargetVel, 50.0f)) {
                o->oTTCCogTargetVel = 200.0f * (random_u16() % 7) * random_sign();
            }

        case TTC_SPEED_STOPPED:
            break;
    }

    o->oAngleVelYaw = (s32)(o->oTTCCogSpeed * o->oTTCCogDir);
    o->oFaceAngleYaw += o->oAngleVelYaw;
}
