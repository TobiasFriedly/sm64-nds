
static struct ObjectHitbox sWaterBombHitbox = {
     INTERACT_MR_BLIZZARD,
     25,
     1,
     99,
     0,
     80,
     50,
     60,
     50,
};

void bhv_water_bomb_spawner_update(void) {
    f32 latDistToMario;
    f32 spawnerRadius = 50 * (u16)(o->oBhvParams >> 16) + 200.0f;

    latDistToMario = lateral_dist_between_objects(o, gMarioObject);

    if (!o->oWaterBombSpawnerBombActive && latDistToMario < spawnerRadius
        && gMarioObject->oPosY - o->oPosY < 1000.0f) {
        if (o->oWaterBombSpawnerTimeToSpawn != 0) {
            o->oWaterBombSpawnerTimeToSpawn--;
        } else {
            struct Object *waterBomb =
                spawn_object_relative(0, 0, 2000, 0, o, MODEL_WATER_BOMB, bhvWaterBomb);

            if (waterBomb != NULL) {

                f32 waterBombDistToMario = 28.0f * gMarioStates[0].forwardVel + 100.0f;

                waterBomb->oAction = WATER_BOMB_ACT_INIT;

                waterBomb->oPosX =
                    gMarioObject->oPosX + waterBombDistToMario * sins(gMarioObject->oMoveAngleYaw);
                waterBomb->oPosZ =
                    gMarioObject->oPosZ + waterBombDistToMario * coss(gMarioObject->oMoveAngleYaw);

                spawn_object(waterBomb, MODEL_WATER_BOMB_SHADOW, bhvWaterBombShadow);

                o->oWaterBombSpawnerBombActive = TRUE;
                o->oWaterBombSpawnerTimeToSpawn = random_linear_offset(0, 50);
            }
        }
    }
}

void water_bomb_spawn_explode_particles(s8 offsetY, s8 forwardVelRange, s8 velYBase) {
    static struct SpawnParticlesInfo waterBombExplodeParticles = {
         0,
         5,
         MODEL_BUBBLE,
         20,
         20,
         60,
         10,
         10,
         -2,
         0,
         35.0f,
         10.0f,
    };

    waterBombExplodeParticles.offsetY = offsetY;
    waterBombExplodeParticles.forwardVelRange = forwardVelRange;
    waterBombExplodeParticles.velYBase = velYBase;

    cur_obj_spawn_particles(&waterBombExplodeParticles);
}

static void water_bomb_act_init(void) {
    cur_obj_play_sound_2(SOUND_OBJ_SOMETHING_LANDING);

    o->oAction = WATER_BOMB_ACT_DROP;
    o->oMoveFlags = 0;
    o->oVelY = -40.0f;
}

static void water_bomb_act_drop(void) {
    f32 stretch;

    obj_set_hitbox(o, &sWaterBombHitbox);

    if ((o->oInteractStatus & INT_STATUS_INTERACTED) || (o->oMoveFlags & OBJ_MOVE_ENTERED_WATER)) {
        create_sound_spawner(SOUND_OBJ_DIVING_IN_WATER);
        set_camera_shake_from_point(SHAKE_POS_SMALL, o->oPosX, o->oPosY, o->oPosZ);
        o->oAction = WATER_BOMB_ACT_EXPLODE;
    } else if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {

        if (!o->oWaterBombOnGround) {
            o->oWaterBombOnGround = TRUE;

            if ((o->oWaterBombNumBounces += 1.0f) < 3.0f) {
                cur_obj_play_sound_2(SOUND_OBJ_WATER_BOMB_BOUNCING);
            } else {
                create_sound_spawner(SOUND_OBJ_DIVING_IN_WATER);
            }

            set_camera_shake_from_point(SHAKE_POS_SMALL, o->oPosX, o->oPosY, o->oPosZ);

            o->oMoveAngleYaw = o->oAngleToMario;
            o->oForwardVel = 10.0f;
            o->oWaterBombStretchSpeed = -40.0f;
        }

        o->oWaterBombStretchSpeed += 15.0f - o->oWaterBombNumBounces * 2.8f;

        o->oWaterBombVerticalStretch += o->oWaterBombStretchSpeed * 0.01f;

        if (o->oWaterBombVerticalStretch < -0.8f) {
            o->oAction = WATER_BOMB_ACT_EXPLODE;
        } else if (o->oWaterBombVerticalStretch > 0.1f) {

            o->oVelY = 1.8f * o->oWaterBombStretchSpeed;
        }
    } else {
        approach_f32_ptr(&o->oWaterBombVerticalStretch, 0.0f, 0.008f);
        o->oWaterBombOnGround = FALSE;
    }

    o->header.gfx.scale[1] = o->oWaterBombVerticalStretch + 1.0f;

    stretch = o->oWaterBombVerticalStretch;
    if (o->oWaterBombNumBounces == 3.0f) {
        stretch *= 4.0f;
    }
    o->header.gfx.scale[0] = o->header.gfx.scale[2] = 1.0f - stretch;

    cur_obj_move_standard(78);
}

static void water_bomb_act_explode(void) {
    water_bomb_spawn_explode_particles(25, 60, 10);
    o->parentObj->oWaterBombSpawnerBombActive = FALSE;
    obj_mark_for_deletion(o);
}

static void water_bomb_act_shot_from_cannon(void) {
    static struct SpawnParticlesInfo waterBombCannonParticle = {
         0,
         1,
         MODEL_BUBBLE,
         236,
         20,
         5,
         0,
         0,
         -2,
         0,
         20.0f,
         5.0f,
    };

    if (o->oTimer > 100) {
        obj_mark_for_deletion(o);
    } else {
        if (o->oTimer < 7) {
            if (o->oTimer == 1) {
                water_bomb_spawn_explode_particles(-20, 10, 30);
            }
            cur_obj_spawn_particles(&waterBombCannonParticle);
        }

        if (o->header.gfx.scale[1] > 1.2f) {
            o->header.gfx.scale[1] -= 0.1f;
        }

        o->header.gfx.scale[0] = o->header.gfx.scale[2] = 2.0f - o->header.gfx.scale[1];
        cur_obj_set_pos_via_transform();
    }
}

void bhv_water_bomb_update(void) {
    if (o->oAction == WATER_BOMB_ACT_SHOT_FROM_CANNON) {
        water_bomb_act_shot_from_cannon();
    } else {
        o->oGraphYOffset = 40.0f * o->header.gfx.scale[1];
        cur_obj_update_floor_and_walls();

        switch (o->oAction) {
            case WATER_BOMB_ACT_INIT:
                water_bomb_act_init();
                break;
            case WATER_BOMB_ACT_DROP:
                water_bomb_act_drop();
                break;
            case WATER_BOMB_ACT_EXPLODE:
                water_bomb_act_explode();
                break;
        }
    }
}

void bhv_water_bomb_shadow_update(void) {
    if (o->parentObj->oAction == WATER_BOMB_ACT_EXPLODE) {
        obj_mark_for_deletion(o);
    } else {

        f32 bombHeight = o->parentObj->oPosY - o->parentObj->oFloorHeight;
        if (bombHeight > 500.0f) {
            bombHeight = 500.0f;
        }

        obj_copy_pos(o, o->parentObj);
        o->oPosY = o->parentObj->oFloorHeight + bombHeight;
        obj_copy_scale(o, o->parentObj);
    }
}
