
static struct ObjectHitbox sFallingPillarHitbox = {
     INTERACT_DAMAGE,
     150,
     3,
     0,
     0,
     150,
     300,
     0,
     0,
};

void bhv_falling_pillar_init(void) {
    o->oGravity = 0.5f;
    o->oFriction = 0.91f;
    o->oBuoyancy = 1.3f;
}

void bhv_falling_pillar_spawn_hitboxes(void) {
    s32 i;

    for (i = 0; i < 4; i++) {
        spawn_object_relative(i, 0, i * 400 + 300, 0, o, MODEL_NONE, bhvFallingPillarHitbox);
    }
}

s16 bhv_falling_pillar_calculate_angle_in_front_of_mario(void) {

    f32 targetX = sins(gMarioObject->header.gfx.angle[1]) * 500.0f + gMarioObject->header.gfx.pos[0];
    f32 targetZ = coss(gMarioObject->header.gfx.angle[1]) * 500.0f + gMarioObject->header.gfx.pos[2];

    return atan2s(targetZ - o->oPosZ, targetX - o->oPosX);
}

void bhv_falling_pillar_loop(void) {
    s16 angleInFrontOfMario;

    switch (o->oAction) {
        case FALLING_PILLAR_ACT_IDLE:

            if (is_point_within_radius_of_mario(o->oPosX, o->oPosY, o->oPosZ, 1300)) {

                o->oMoveAngleYaw = o->oAngleToMario;
                o->oForwardVel = 1.0f;

                bhv_falling_pillar_spawn_hitboxes();

                o->oAction = FALLING_PILLAR_ACT_TURNING;

                cur_obj_play_sound_2(SOUND_GENERAL_POUND_ROCK);
            }
            break;

        case FALLING_PILLAR_ACT_TURNING:
            object_step_without_floor_orient();

            angleInFrontOfMario = bhv_falling_pillar_calculate_angle_in_front_of_mario();
            o->oFaceAngleYaw = approach_s16_symmetric(o->oFaceAngleYaw, angleInFrontOfMario, 0x400);

            if (o->oTimer > 10) {
                o->oAction = FALLING_PILLAR_ACT_FALLING;
            }
            break;

        case FALLING_PILLAR_ACT_FALLING:
            object_step_without_floor_orient();

            o->oFallingPillarPitchAcceleration += 4.0f;
            o->oAngleVelPitch += o->oFallingPillarPitchAcceleration;
            o->oFaceAnglePitch += o->oAngleVelPitch;

            if (o->oFaceAnglePitch > 0x3900) {

                o->oPosX += sins(o->oFaceAngleYaw) * 500.0f;
                o->oPosZ += coss(o->oFaceAngleYaw) * 500.0f;

                set_camera_shake_from_point(SHAKE_POS_MEDIUM, o->oPosX, o->oPosY, o->oPosZ);
                spawn_mist_particles_variable(0, 0, 92.0f);

                o->activeFlags = ACTIVE_FLAG_DEACTIVATED;

                create_sound_spawner(SOUND_GENERAL_BIG_POUND);
            }
            break;
    }
}

void bhv_falling_pillar_hitbox_loop(void) {

    s32 pitch = o->parentObj->oFaceAnglePitch;
    s32 yaw = o->parentObj->oFaceAngleYaw;
    f32 x = o->parentObj->oPosX;
    f32 y = o->parentObj->oPosY;
    f32 z = o->parentObj->oPosZ;
    f32 yOffset = o->oBhvParams2ndByte * 400 + 300;

    o->oPosX = sins(pitch) * sins(yaw) * yOffset + x;
    o->oPosY = coss(pitch) * yOffset + y;
    o->oPosZ = sins(pitch) * coss(yaw) * yOffset + z;

    obj_set_hitbox(o, &sFallingPillarHitbox);

    if (o->parentObj->activeFlags == ACTIVE_FLAG_DEACTIVATED) {
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }
}
