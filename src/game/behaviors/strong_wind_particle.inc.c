
struct ObjectHitbox sStrongWindParticleHitbox = {
     INTERACT_STRONG_WIND,
     0,
     0,
     0,
     0,
     20,
     70,
     20,
     70,
};

void bhv_strong_wind_particle_loop(void) {
    struct Object *penguinObj;
    f32 distanceFromPenguin;
    f32 penguinXDist, penguinZDist;

    obj_set_hitbox(o, &sStrongWindParticleHitbox);

    if (o->oTimer == 0) {
        o->oStrongWindParticlePenguinObj = cur_obj_nearest_object_with_behavior(bhvSLWalkingPenguin);
        obj_translate_xyz_random(o, 100.0f);

        o->oForwardVel = coss(o->oMoveAnglePitch) * 100.0f;
        o->oVelY = sins(o->oMoveAnglePitch) * -100.0f;

        o->oMoveAngleYaw += random_f32_around_zero(o->oBhvParams2ndByte * 500);
        o->oOpacity = 100;
    }

    cur_obj_move_using_fvel_and_gravity();
    if (o->oTimer > 15) {
        obj_mark_for_deletion(o);
    }

    penguinObj = o->oStrongWindParticlePenguinObj;
    if (penguinObj != NULL) {
        penguinXDist = penguinObj->oSLWalkingPenguinWindCollisionXPos - o->oPosX;
        penguinZDist = penguinObj->oSLWalkingPenguinWindCollisionZPos - o->oPosZ;
        distanceFromPenguin = sqrtf(penguinXDist * penguinXDist + penguinZDist * penguinZDist);
        if (distanceFromPenguin < 300.0f) {
            obj_mark_for_deletion(o);
            cur_obj_become_intangible();
        }
    }
}

void cur_obj_spawn_strong_wind_particles(s32 windSpread, f32 scale, f32 relPosX, f32 relPosY, f32 relPosZ) {

    if (gGlobalTimer & 1) {

        spawn_object_relative_with_scale(windSpread, relPosX, relPosY, relPosZ, 0.5f, o, MODEL_WHITE_PARTICLE_DL, bhvTinyStrongWindParticle);
        spawn_object_relative_with_scale(windSpread, relPosX, relPosY, relPosZ, scale, o, MODEL_NONE, bhvStrongWindParticle);
    } else {
        spawn_object_relative_with_scale(windSpread, relPosX, relPosY, relPosZ, scale, o, MODEL_MIST, bhvStrongWindParticle);
    }

    spawn_object_relative_with_scale(windSpread, relPosX, relPosY, relPosZ, scale, o, MODEL_NONE, bhvStrongWindParticle);
}
