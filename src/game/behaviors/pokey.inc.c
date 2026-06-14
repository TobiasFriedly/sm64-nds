
static struct ObjectHitbox sPokeyBodyPartHitbox = {
     INTERACT_BOUNCE_TOP,
     10,
     2,
     0,
     0,
     40,
     20,
     20,
     20,
};

static u8 sPokeyBodyPartAttackHandlers[] = {
     ATTACK_HANDLER_KNOCKBACK,
     ATTACK_HANDLER_KNOCKBACK,
     ATTACK_HANDLER_SQUISHED,
     ATTACK_HANDLER_SQUISHED,
     ATTACK_HANDLER_KNOCKBACK,
     ATTACK_HANDLER_KNOCKBACK,
};

void bhv_pokey_body_part_update(void) {

    s16 offsetAngle;
    f32 baseHeight;

    if (obj_update_standard_actions(3.0f)) {
        if (o->parentObj->oAction == POKEY_ACT_UNLOAD_PARTS) {
            obj_mark_for_deletion(o);
        } else {
            cur_obj_update_floor_and_walls();
            obj_update_blinking(&o->oPokeyBodyPartBlinkTimer, 30, 60, 4);

            if (o->oBhvParams2ndByte > 1
                && !(o->parentObj->oPokeyAliveBodyPartFlags & (1 << (o->oBhvParams2ndByte - 1)))) {
                o->parentObj->oPokeyAliveBodyPartFlags =
                    o->parentObj->oPokeyAliveBodyPartFlags | 1 << (o->oBhvParams2ndByte - 1);

                o->parentObj->oPokeyAliveBodyPartFlags =
                    o->parentObj->oPokeyAliveBodyPartFlags & ((1 << o->oBhvParams2ndByte) ^ ~0);

                o->oBhvParams2ndByte--;
            }

            else if (o->parentObj->oPokeyBottomBodyPartSize < 1.0f
                     && o->oBhvParams2ndByte + 1 == o->parentObj->oPokeyNumAliveBodyParts) {
                approach_f32_ptr(&o->parentObj->oPokeyBottomBodyPartSize, 1.0f, 0.1f);
                cur_obj_scale(o->parentObj->oPokeyBottomBodyPartSize * 3.0f);
            }

            offsetAngle = o->oBhvParams2ndByte * 0x4000 + gGlobalTimer * 0x800;
            o->oPosX = o->parentObj->oPosX + coss(offsetAngle) * 6.0f;
            o->oPosZ = o->parentObj->oPosZ + sins(offsetAngle) * 6.0f;

            baseHeight = o->parentObj->oPosY
                         + (120 * (o->parentObj->oPokeyNumAliveBodyParts - o->oBhvParams2ndByte) - 240)
                         + 120.0f * o->parentObj->oPokeyBottomBodyPartSize;

            if (o->oPosY < baseHeight) {
                o->oPosY = baseHeight;
                o->oVelY = 0.0f;
            }

            if (o->oBhvParams2ndByte == 0) {
                o->oNumLootCoins = 1;
            } else {
                o->oNumLootCoins = 0;
            }

            if (obj_handle_attacks(&sPokeyBodyPartHitbox, o->oAction, sPokeyBodyPartAttackHandlers)) {
                o->parentObj->oPokeyNumAliveBodyParts--;
                if (o->oBhvParams2ndByte == 0) {
                    o->parentObj->oPokeyHeadWasKilled = TRUE;

                    o->oNumLootCoins = -1;
                }

                o->parentObj->oPokeyAliveBodyPartFlags =
                    o->parentObj->oPokeyAliveBodyPartFlags & ((1 << o->oBhvParams2ndByte) ^ ~0);
            } else if (o->parentObj->oPokeyHeadWasKilled) {
                cur_obj_become_intangible();

                if (--o->oPokeyBodyPartDeathDelayAfterHeadKilled < 0) {
                    o->parentObj->oPokeyNumAliveBodyParts--;
                    obj_die_if_health_non_positive();
                }
            } else {

                o->oPokeyBodyPartDeathDelayAfterHeadKilled = (o->oBhvParams2ndByte << 2) + 20;
            }

            cur_obj_move_standard(-78);
        }
    } else {
        o->oAnimState = 1;
    }

    o->oGraphYOffset = o->header.gfx.scale[1] * 22.0f;
}

static void pokey_act_uninitialized(void) {
    struct Object *bodyPart;
    s32 i;
    s16 partModel;

    if (o->oDistanceToMario < 2000.0f) {
        partModel = MODEL_POKEY_HEAD;

        for (i = 0; i < 5; i++) {

            bodyPart = spawn_object_relative(i, 0, -i * 120 + 480, 0, o, partModel, bhvPokeyBodyPart);

            if (bodyPart != NULL) {
                obj_scale(bodyPart, 3.0f);
            }

            partModel = MODEL_POKEY_BODY_PART;
        }

        o->oPokeyAliveBodyPartFlags = 0x1F;
        o->oPokeyNumAliveBodyParts = 5;
        o->oPokeyBottomBodyPartSize = 1.0f;
        o->oAction = POKEY_ACT_WANDER;
    }
}

static void pokey_act_wander(void) {
    s32 targetAngleOffset;

    if (o->oPokeyNumAliveBodyParts == 0) {
        obj_mark_for_deletion(o);
    } else if (o->oDistanceToMario > 2500.0f) {
        o->oAction = POKEY_ACT_UNLOAD_PARTS;
        o->oForwardVel = 0.0f;
    } else {
        treat_far_home_as_mario(1000.0f);
        cur_obj_update_floor_and_walls();

        if (o->oPokeyHeadWasKilled) {
            o->oForwardVel = 0.0f;
        } else {
            o->oForwardVel = 5.0f;

            if (o->oPokeyNumAliveBodyParts < 5) {
                if (o->oTimer > 100) {

                    struct Object *bodyPart
                        = spawn_object_relative(o->oPokeyNumAliveBodyParts, 0, 0, 0, o,
                                                MODEL_POKEY_BODY_PART, bhvPokeyBodyPart);

                    if (bodyPart != NULL) {
                        o->oPokeyAliveBodyPartFlags =
                            o->oPokeyAliveBodyPartFlags | (1 << o->oPokeyNumAliveBodyParts);
                        o->oPokeyNumAliveBodyParts++;
                        o->oPokeyBottomBodyPartSize = 0.0f;

                        obj_scale(bodyPart, 0.0f);
                    }

                    o->oTimer = 0;
                }
            } else {
                o->oTimer = 0;
            }

            if (o->oPokeyTurningAwayFromWall) {
                o->oPokeyTurningAwayFromWall =
                    obj_resolve_collisions_and_turn(o->oPokeyTargetYaw, 0x200);
            } else {

                if (o->oDistanceToMario >= 25000.0f) {
                    o->oPokeyTargetYaw = o->oAngleToMario;
                }

                if (!(o->oPokeyTurningAwayFromWall =
                          obj_bounce_off_walls_edges_objects(&o->oPokeyTargetYaw))) {
                    if (o->oPokeyChangeTargetTimer != 0) {
                        o->oPokeyChangeTargetTimer--;
                    } else if (o->oDistanceToMario > 2000.0f) {
                        o->oPokeyTargetYaw = obj_random_fixed_turn(0x2000);
                        o->oPokeyChangeTargetTimer = random_linear_offset(30, 50);
                    } else {

                        targetAngleOffset = (s32)(0x4000 - (o->oDistanceToMario - 200.0f) * 10.0f);
                        if (targetAngleOffset < 0) {
                            targetAngleOffset = 0;
                        } else if (targetAngleOffset > 0x4000) {
                            targetAngleOffset = 0x4000;
                        }

                        if ((s16)(o->oAngleToMario - o->oMoveAngleYaw) > 0) {
                            targetAngleOffset = -targetAngleOffset;
                        }

                        o->oPokeyTargetYaw = o->oAngleToMario + targetAngleOffset;
                    }
                }

                cur_obj_rotate_yaw_toward(o->oPokeyTargetYaw, 0x200);
            }
        }

        cur_obj_move_standard(-78);
    }
}

static void pokey_act_unload_parts(void) {
    o->oAction = POKEY_ACT_UNINITIALIZED;
    cur_obj_set_pos_to_home();
}

void bhv_pokey_update(void) {

    o->oDeathSound = SOUND_OBJ_POKEY_DEATH;

    switch (o->oAction) {
        case POKEY_ACT_UNINITIALIZED:
            pokey_act_uninitialized();
            break;
        case POKEY_ACT_WANDER:
            pokey_act_wander();
            break;
        case POKEY_ACT_UNLOAD_PARTS:
            pokey_act_unload_parts();
            break;
    }
}
