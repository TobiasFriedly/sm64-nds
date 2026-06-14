
static struct ObjectHitbox sGoombaHitbox = {
     INTERACT_BOUNCE_TOP,
     0,
     1,
     0,
     1,
     72,
     50,
     42,
     40,
};

struct GoombaProperties {
    f32 scale;
    u32 deathSound;
    s16 drawDistance;
    s8 damage;
};

static struct GoombaProperties sGoombaProperties[] = {
    { 1.5f, SOUND_OBJ_ENEMY_DEATH_HIGH, 4000, 1 },
    { 3.5f, SOUND_OBJ_ENEMY_DEATH_LOW, 4000, 2 },
    { 0.5f, SOUND_OBJ_ENEMY_DEATH_HIGH, 1500, 0 },
};

static u8 sGoombaAttackHandlers[][6] = {

    {
         ATTACK_HANDLER_KNOCKBACK,
         ATTACK_HANDLER_KNOCKBACK,
         ATTACK_HANDLER_SQUISHED,
         ATTACK_HANDLER_SQUISHED,
         ATTACK_HANDLER_KNOCKBACK,
         ATTACK_HANDLER_KNOCKBACK,
    },

    {
         ATTACK_HANDLER_SPECIAL_HUGE_GOOMBA_WEAKLY_ATTACKED,
         ATTACK_HANDLER_SPECIAL_HUGE_GOOMBA_WEAKLY_ATTACKED,
         ATTACK_HANDLER_SQUISHED,
         ATTACK_HANDLER_SQUISHED_WITH_BLUE_COIN,
         ATTACK_HANDLER_SPECIAL_HUGE_GOOMBA_WEAKLY_ATTACKED,
         ATTACK_HANDLER_SPECIAL_HUGE_GOOMBA_WEAKLY_ATTACKED,
    },
};

void bhv_goomba_triplet_spawner_update(void) {
    UNUSED u8 filler1[4];
    s16 goombaFlag;
    UNUSED u8 filler2[2];
    s32 angle;

    if (o->oAction == GOOMBA_TRIPLET_SPAWNER_ACT_UNLOADED) {
        if (o->oDistanceToMario < 3000.0f) {

            s32 dAngle =
                0x10000
                / (((o->oBhvParams2ndByte & GOOMBA_TRIPLET_SPAWNER_BP_EXTRA_GOOMBAS_MASK) >> 2) + 3);

            for (angle = 0, goombaFlag = 1 << 8; angle < 0xFFFF; angle += dAngle, goombaFlag <<= 1) {

                if (!(o->oBhvParams & goombaFlag)) {
                    s16 dx = 500.0f * coss(angle);
                    s16 dz = 500.0f * sins(angle);

                    spawn_object_relative((o->oBhvParams2ndByte & GOOMBA_BP_SIZE_MASK)
                                           | (goombaFlag >> 6), dx, 0, dz, o, MODEL_GOOMBA, bhvGoomba);
                }
            }

            o->oAction++;
        }
    } else if (o->oDistanceToMario > 4000.0f) {

        o->oAction = GOOMBA_TRIPLET_SPAWNER_ACT_UNLOADED;
    }
}

void bhv_goomba_init(void) {
    o->oGoombaSize = o->oBhvParams2ndByte & GOOMBA_BP_SIZE_MASK;

    o->oGoombaScale = sGoombaProperties[o->oGoombaSize].scale;
    o->oDeathSound = sGoombaProperties[o->oGoombaSize].deathSound;

    obj_set_hitbox(o, &sGoombaHitbox);

    o->oDrawingDistance = sGoombaProperties[o->oGoombaSize].drawDistance;
    o->oDamageOrCoinValue = sGoombaProperties[o->oGoombaSize].damage;

    o->oGravity = -8.0f / 3.0f * o->oGoombaScale;
}

static void goomba_begin_jump(void) {
    cur_obj_play_sound_2(SOUND_OBJ_GOOMBA_ALERT);

    o->oAction = GOOMBA_ACT_JUMP;
    o->oForwardVel = 0.0f;
    o->oVelY = 50.0f / 3.0f * o->oGoombaScale;
}

static void mark_goomba_as_dead(void) {
    if (o->parentObj != o) {
        set_object_respawn_info_bits(
            o->parentObj, (o->oBhvParams2ndByte & GOOMBA_BP_TRIPLET_RESPAWN_FLAG_MASK) >> 2);

        o->parentObj->oBhvParams =
            o->parentObj->oBhvParams | (o->oBhvParams2ndByte & GOOMBA_BP_TRIPLET_RESPAWN_FLAG_MASK) << 6;
    }
}

static void goomba_act_walk(void) {
    treat_far_home_as_mario(1000.0f);

    obj_forward_vel_approach(o->oGoombaRelativeSpeed * o->oGoombaScale, 0.4f);

    if (o->oGoombaRelativeSpeed > 4.0f / 3.0f) {
        cur_obj_play_sound_at_anim_range(2, 17, SOUND_OBJ_GOOMBA_WALK);
    }

    if (o->oGoombaTurningAwayFromWall) {
        o->oGoombaTurningAwayFromWall = obj_resolve_collisions_and_turn(o->oGoombaTargetYaw, 0x200);
    } else {

        if (o->oDistanceToMario >= 25000.0f) {
            o->oGoombaTargetYaw = o->oAngleToMario;
            o->oGoombaWalkTimer = random_linear_offset(20, 30);
        }

        if (!(o->oGoombaTurningAwayFromWall =
                  obj_bounce_off_walls_edges_objects(&o->oGoombaTargetYaw))) {
            if (o->oDistanceToMario < 500.0f) {

                if (o->oGoombaRelativeSpeed <= 2.0f) {
                    goomba_begin_jump();
                }

                o->oGoombaTargetYaw = o->oAngleToMario;
                o->oGoombaRelativeSpeed = 20.0f;
            } else {

                o->oGoombaRelativeSpeed = 4.0f / 3.0f;

                if (o->oGoombaWalkTimer != 0) {
                    o->oGoombaWalkTimer--;
                } else {
                    if (random_u16() & 3) {
                        o->oGoombaTargetYaw = obj_random_fixed_turn(0x2000);
                        o->oGoombaWalkTimer = random_linear_offset(100, 100);
                    } else {
                        goomba_begin_jump();
                        o->oGoombaTargetYaw = obj_random_fixed_turn(0x6000);
                    }
                }
            }
        }

        cur_obj_rotate_yaw_toward(o->oGoombaTargetYaw, 0x200);
    }
}

static void goomba_act_attacked_mario(void) {
    if (o->oGoombaSize == GOOMBA_SIZE_TINY) {
        mark_goomba_as_dead();
        o->oNumLootCoins = 0;
        obj_die_if_health_non_positive();
    } else {

        goomba_begin_jump();
        o->oGoombaTargetYaw = o->oAngleToMario;
        o->oGoombaTurningAwayFromWall = FALSE;
    }
}

static void goomba_act_jump(void) {
    obj_resolve_object_collisions(NULL);

    if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {
        o->oAction = GOOMBA_ACT_WALK;
    } else {
        cur_obj_rotate_yaw_toward(o->oGoombaTargetYaw, 0x800);
    }
}

void huge_goomba_weakly_attacked(void) {
    o->oAction = GOOMBA_ACT_ATTACKED_MARIO;
}

void bhv_goomba_update(void) {

    f32 animSpeed;

    if (obj_update_standard_actions(o->oGoombaScale)) {

        if (o->parentObj != o) {
            if (o->parentObj->oAction == GOOMBA_TRIPLET_SPAWNER_ACT_UNLOADED) {
                obj_mark_for_deletion(o);
            }
        }

        cur_obj_scale(o->oGoombaScale);
        obj_update_blinking(&o->oGoombaBlinkTimer, 30, 50, 5);
        cur_obj_update_floor_and_walls();

        if ((animSpeed = o->oForwardVel / o->oGoombaScale * 0.4f) < 1.0f) {
            animSpeed = 1.0f;
        }

        cur_obj_init_animation_with_accel_and_sound(0, animSpeed);

        switch (o->oAction) {
            case GOOMBA_ACT_WALK:
                goomba_act_walk();
                break;
            case GOOMBA_ACT_ATTACKED_MARIO:
                goomba_act_attacked_mario();
                break;
            case GOOMBA_ACT_JUMP:
                goomba_act_jump();
                break;
        }

        if (obj_handle_attacks(&sGoombaHitbox, GOOMBA_ACT_ATTACKED_MARIO,
                               sGoombaAttackHandlers[o->oGoombaSize & 1])) {
            mark_goomba_as_dead();
        }

        cur_obj_move_standard(-78);
    } else {
        o->oAnimState = 1;
    }
}
