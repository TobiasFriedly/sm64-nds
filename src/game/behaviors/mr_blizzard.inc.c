
struct ObjectHitbox sMrBlizzardHitbox = {
     INTERACT_MR_BLIZZARD,
     24,
     2,
     99,
     3,
     65,
     170,
     65,
     170,
};

void mr_blizzard_spawn_white_particles(s8 count, s8 offsetY, s8 forwardVelBase, s8 velYBase,
                                       s8 sizeBase) {
    static struct SpawnParticlesInfo D_80331A00 = {
         0,
         6,
         MODEL_WHITE_PARTICLE,
         0,
         5,
         5,
         10,
         10,
         -3,
         0,
         3.0f,
         5.0f,
    };

    D_80331A00.count = count;
    D_80331A00.offsetY = offsetY;
    D_80331A00.forwardVelBase = forwardVelBase;
    D_80331A00.velYBase = velYBase;
    D_80331A00.sizeBase = sizeBase;

    cur_obj_spawn_particles(&D_80331A00);
}

void bhv_mr_blizzard_init(void) {
    if (o->oBhvParams2ndByte == MR_BLIZZARD_STYPE_JUMPING) {

        o->oAction = MR_BLIZZARD_ACT_JUMP;
        o->oMrBlizzardGraphYOffset = 24.0f;
        o->oMrBlizzardTargetMoveYaw = o->oMoveAngleYaw;
    } else {

        if ((o->oBhvParams2ndByte != MR_BLIZZARD_STYPE_GENERIC)
            && (save_file_get_flags() & SAVE_FLAG_CAP_ON_MR_BLIZZARD)) {
            o->oAnimState = 1;
        }

        o->oMrBlizzardGraphYOffset = -200.0f;
        o->oMrBlizzardHeldObj = NULL;
    }
}

static void mr_blizzard_act_spawn_snowball(void) {

    if (o->oMrBlizzardHeldObj == NULL && cur_obj_init_anim_check_frame(0, 5)) {
        o->oMrBlizzardHeldObj =
            spawn_object_relative(0, -70, (s32)(o->oMrBlizzardGraphYOffset + 153.0f), 0, o,
                                  MODEL_WHITE_PARTICLE, bhvMrBlizzardSnowball);
    } else if (cur_obj_check_anim_frame(10)) {
        o->prevObj = o->oMrBlizzardHeldObj;
    } else if (cur_obj_check_if_near_animation_end()) {

        if (o->oMrBlizzardGraphYOffset < 0.0f) {
            o->oAction = MR_BLIZZARD_ACT_HIDE_UNHIDE;
        } else {
            o->oAction = MR_BLIZZARD_ACT_ROTATE;
        }
    }
}

static void mr_blizzard_act_hide_unhide(void) {
    if (o->oDistanceToMario < 1000.0f) {

        cur_obj_play_sound_2(SOUND_OBJ_SNOW_SAND2);
        o->oAction = MR_BLIZZARD_ACT_RISE_FROM_GROUND;
        o->oMoveAngleYaw = o->oAngleToMario;
        o->oMrBlizzardGraphYVel = 42.0f;

        mr_blizzard_spawn_white_particles(8, -10, 15, 20, 10);
        cur_obj_unhide();
        cur_obj_become_tangible();
    } else {

        cur_obj_hide();
    }
}

static void mr_blizzard_act_rise_from_ground(void) {

    if (o->oMrBlizzardTimer != 0) {
        o->oMrBlizzardTimer--;
    } else if ((o->oMrBlizzardGraphYOffset += o->oMrBlizzardGraphYVel) > 24.0f) {

        o->oPosY += o->oMrBlizzardGraphYOffset - 24.0f;
        o->oMrBlizzardGraphYOffset = 24.0f;

        mr_blizzard_spawn_white_particles(8, -20, 20, 15, 10);

        o->oAction = MR_BLIZZARD_ACT_ROTATE;
        o->oVelY = o->oMrBlizzardGraphYVel;
    } else if ((o->oMrBlizzardGraphYVel -= 10.0f) < 0.0f) {

        o->oMrBlizzardGraphYVel = 47.0f;
        o->oMrBlizzardTimer = 5;
    }
}

static void mr_blizzard_act_rotate(void) {

    if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {
        s16 angleDiff;
        cur_obj_rotate_yaw_toward(o->oAngleToMario, 0x600);

        angleDiff = o->oAngleToMario - o->oMoveAngleYaw;
        if (angleDiff != 0) {
            if (angleDiff < 0) {
                o->oMrBlizzardChangeInDizziness -= 8.0f;
            } else {
                o->oMrBlizzardChangeInDizziness += 8.0f;
            }

            o->oMrBlizzardDizziness += o->oMrBlizzardChangeInDizziness;
        } else if (o->oMrBlizzardDizziness != 0.0f) {
            f32 prevDizziness = o->oMrBlizzardDizziness;

            if (o->oMrBlizzardDizziness < 0.0f) {
                approach_f32_ptr(&o->oMrBlizzardChangeInDizziness, 1000.0f, 80.0f);
            } else {
                approach_f32_ptr(&o->oMrBlizzardChangeInDizziness, -1000.0f, 80.0f);
            }

            o->oMrBlizzardDizziness += o->oMrBlizzardChangeInDizziness;

            if (prevDizziness * o->oMrBlizzardDizziness < 0.0f) {
                o->oMrBlizzardDizziness = o->oMrBlizzardChangeInDizziness = 0.0f;
            }
        }

        if (o->oMrBlizzardDizziness != 0.0f) {
            if (absi(o->oFaceAngleRoll) > 0x3000) {
                o->oAction = MR_BLIZZARD_ACT_DEATH;
                o->prevObj = o->oMrBlizzardHeldObj = NULL;
                cur_obj_become_intangible();
            }
        }

        else if (o->oDistanceToMario > 1500.0f) {
            o->oAction = MR_BLIZZARD_ACT_BURROW;
            o->oMrBlizzardChangeInDizziness = 300.0f;
            o->prevObj = o->oMrBlizzardHeldObj = NULL;
        }

        else if (o->oTimer > 60 && abs_angle_diff(o->oAngleToMario, o->oMoveAngleYaw) < 0x800) {
            o->oAction = MR_BLIZZARD_ACT_THROW_SNOWBALL;
        }
    }
}

static void mr_blizzard_act_death(void) {
    if (clamp_f32(&o->oMrBlizzardDizziness, -0x4000, 0x4000)) {
        if (o->oMrBlizzardChangeInDizziness != 0.0f) {
            cur_obj_play_sound_2(SOUND_OBJ_SNOW_SAND1);

            if (o->oAnimState != 0) {
                struct Object *cap;
                save_file_clear_flags(SAVE_FLAG_CAP_ON_MR_BLIZZARD);

                cap = spawn_object_relative(0, 5, 105, 0, o, MODEL_MARIOS_CAP, bhvNormalCap);
                if (cap != NULL) {
                    cap->oMoveAngleYaw = o->oFaceAngleYaw + (o->oFaceAngleRoll < 0 ? 0x4000 : -0x4000);
                    cap->oForwardVel = 10.0f;
                }

                o->oAnimState = 0;
            }

            o->oMrBlizzardChangeInDizziness = 0.0f;
        }
    } else {
        if (o->oMrBlizzardDizziness < 0) {
            o->oMrBlizzardChangeInDizziness -= 40.0f;
        } else {
            o->oMrBlizzardChangeInDizziness += 40.0f;
        }

        o->oMrBlizzardDizziness += o->oMrBlizzardChangeInDizziness;
    }

    if (o->oTimer >= 30) {
        if (o->oTimer == 30) {
            cur_obj_play_sound_2(SOUND_OBJ_ENEMY_DEFEAT_SHRINK);
        }

        if (o->oMrBlizzardScale != 0.0f) {
            if ((o->oMrBlizzardScale -= 0.03f) <= 0.0f) {
                o->oMrBlizzardScale = 0.0f;
                if (!(o->oBhvParams & 0x0000FF00)) {
                    obj_spawn_loot_yellow_coins(o, o->oNumLootCoins, 20.0f);
                    set_object_respawn_info_bits(o, 1);
                }
            }
        }

        else if (o->oDistanceToMario > 1000.0f) {
            cur_obj_init_animation_with_sound(1);

            o->oAction = MR_BLIZZARD_ACT_SPAWN_SNOWBALL;
            o->oMrBlizzardScale = 1.0f;
            o->oMrBlizzardGraphYOffset = -200.0f;
            o->oFaceAngleRoll = 0;
            o->oMrBlizzardDizziness = o->oMrBlizzardChangeInDizziness = 0.0f;
        }
    }
}

static void mr_blizzard_act_throw_snowball(void) {

    if (cur_obj_init_anim_check_frame(1, 7)) {
        cur_obj_play_sound_2(SOUND_OBJ2_SCUTTLEBUG_ALERT);
        o->prevObj = o->oMrBlizzardHeldObj = NULL;
    } else if (cur_obj_check_if_near_animation_end()) {
        o->oAction = MR_BLIZZARD_ACT_SPAWN_SNOWBALL;
    }
}

static void mr_blizzard_act_burrow(void) {

    o->oMrBlizzardDizziness += o->oMrBlizzardChangeInDizziness;

    if (o->oMrBlizzardDizziness < 0.0f) {
        o->oMrBlizzardChangeInDizziness += 150.0f;
    } else {
        o->oMrBlizzardChangeInDizziness -= 150.0f;
    }

    if (approach_f32_ptr(&o->oMrBlizzardGraphYOffset, -200.0f, 4.0f)) {
        o->oAction = MR_BLIZZARD_ACT_SPAWN_SNOWBALL;
        cur_obj_init_animation_with_sound(1);
    }
}

static void mr_blizzard_act_jump(void) {
    if (o->oMrBlizzardTimer != 0) {
        cur_obj_rotate_yaw_toward(o->oMrBlizzardTargetMoveYaw, 3400);

        if (--o->oMrBlizzardTimer == 0) {
            cur_obj_play_sound_2(SOUND_OBJ_MR_BLIZZARD_ALERT);

            if (o->oMrBlizzardDistFromHome > 700) {
                o->oMrBlizzardTargetMoveYaw += 0x8000;
                o->oVelY = 25.0f;
                o->oMrBlizzardTimer = 30;
                o->oMrBlizzardDistFromHome = 0;
            }

            else {
                o->oForwardVel = 10.0f;
                o->oVelY = 50.0f;
                o->oMoveFlags = 0;
            }
        }
    } else if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {

        cur_obj_play_sound_2(SOUND_OBJ_SNOW_SAND1);
        if (o->oMrBlizzardDistFromHome != 0) {
            o->oMrBlizzardDistFromHome = (s32) cur_obj_lateral_dist_to_home();
        } else {
            o->oMrBlizzardDistFromHome = 700;
        }

        o->oForwardVel = 0.0f;
        o->oMrBlizzardTimer = 15;
    }
}

void bhv_mr_blizzard_update(void) {
    cur_obj_update_floor_and_walls();

    switch (o->oAction) {
        case MR_BLIZZARD_ACT_SPAWN_SNOWBALL:
            mr_blizzard_act_spawn_snowball();
            break;
        case MR_BLIZZARD_ACT_HIDE_UNHIDE:
            mr_blizzard_act_hide_unhide();
            break;
        case MR_BLIZZARD_ACT_RISE_FROM_GROUND:
            mr_blizzard_act_rise_from_ground();
            break;
        case MR_BLIZZARD_ACT_ROTATE:
            mr_blizzard_act_rotate();
            break;
        case MR_BLIZZARD_ACT_THROW_SNOWBALL:
            mr_blizzard_act_throw_snowball();
            break;
        case MR_BLIZZARD_ACT_BURROW:
            mr_blizzard_act_burrow();
            break;
        case MR_BLIZZARD_ACT_DEATH:
            mr_blizzard_act_death();
            break;
        case MR_BLIZZARD_ACT_JUMP:
            mr_blizzard_act_jump();
            break;
    }

    o->oFaceAngleRoll = o->oMrBlizzardDizziness;

    o->oGraphYOffset = o->oMrBlizzardGraphYOffset + absf(20.0f * sins(o->oFaceAngleRoll))
                       - 40.0f * (1.0f - o->oMrBlizzardScale);

    cur_obj_scale(o->oMrBlizzardScale);
    cur_obj_move_standard(78);
    obj_check_attacks(&sMrBlizzardHitbox, o->oAction);
}

static void mr_blizzard_snowball_act_0(void) {
    cur_obj_move_using_fvel_and_gravity();
    if (o->parentObj->prevObj == o) {
        o->oAction = 1;
        o->oParentRelativePosX = 190.0f;
        o->oParentRelativePosY = o->oParentRelativePosZ = -38.0f;
    }
}

static void mr_blizzard_snowball_act_1(void) {
    if (o->parentObj->prevObj == NULL) {
        if (o->parentObj->oAction == MR_BLIZZARD_ACT_THROW_SNOWBALL) {
            f32 marioDist = o->oDistanceToMario;
            if (marioDist > 800.0f) {
                marioDist = 800.0f;
            }

            o->oMoveAngleYaw = (s32)(o->parentObj->oMoveAngleYaw + 4000 - marioDist * 4.0f);
            o->oForwardVel = 40.0f;
            o->oVelY = -20.0f + marioDist * 0.075f;
        }

        o->oAction = 2;
        o->oMoveFlags = 0;
    }
}

struct ObjectHitbox sMrBlizzardSnowballHitbox = {
     INTERACT_MR_BLIZZARD,
     12,
     1,
     99,
     0,
     30,
     30,
     25,
     25,
};

static void mr_blizzard_snowball_act_2(void) {

    cur_obj_update_floor_and_walls();
    obj_check_attacks(&sMrBlizzardSnowballHitbox, -1);

    if (o->oAction == -1 || o->oMoveFlags & (OBJ_MOVE_MASK_ON_GROUND | OBJ_MOVE_ENTERED_WATER)) {
        mr_blizzard_spawn_white_particles(6, 0, 5, 10, 3);
        create_sound_spawner(SOUND_GENERAL_MOVING_IN_SAND);
        obj_mark_for_deletion(o);
    }

    cur_obj_move_standard(78);
}

void bhv_mr_blizzard_snowball(void) {
    switch (o->oAction) {
        case 0:
            mr_blizzard_snowball_act_0();
            break;
        case 1:
            mr_blizzard_snowball_act_1();
            break;
        case 2:
            mr_blizzard_snowball_act_2();
            break;
    }
}
