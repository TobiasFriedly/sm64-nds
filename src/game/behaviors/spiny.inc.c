
static struct ObjectHitbox sSpinyHitbox = {
     INTERACT_MR_BLIZZARD,
     0,
     2,
     0,
     0,
     80,
     50,
     40,
     40,
};

static u8 sSpinyWalkAttackHandlers[] = {
     ATTACK_HANDLER_KNOCKBACK,
     ATTACK_HANDLER_KNOCKBACK,
     ATTACK_HANDLER_NOP,
     ATTACK_HANDLER_NOP,
     ATTACK_HANDLER_KNOCKBACK,
     ATTACK_HANDLER_KNOCKBACK,
};

static s32 spiny_check_active(void) {
    if (o->parentObj != o) {
        if (o->oDistanceToMario > 2500.0f) {

            o->parentObj->oEnemyLakituNumSpinies--;
            obj_mark_for_deletion(o);
            return FALSE;
        }
    }

    return TRUE;
}

static void spiny_act_walk(void) {
    if (spiny_check_active()) {
        cur_obj_update_floor_and_walls();

        o->oGraphYOffset = -17.0f;
        cur_obj_init_animation_with_sound(0);

        if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {

            if (!(o->oFlags & OBJ_FLAG_SET_FACE_YAW_TO_MOVE_YAW)) {
                if (obj_forward_vel_approach(0.0f, 1.0f)) {
                    o->oFlags |= OBJ_FLAG_SET_FACE_YAW_TO_MOVE_YAW;
                    o->oMoveAngleYaw = o->oFaceAngleYaw;
                }
            } else {
                obj_forward_vel_approach(1.0f, 0.2f);
            }

            if (o->oSpinyTurningAwayFromWall) {
                o->oSpinyTurningAwayFromWall =
                    obj_resolve_collisions_and_turn(o->oSpinyTargetYaw, 0x80);
            } else {
                if (!(o->oSpinyTurningAwayFromWall =
                          obj_bounce_off_walls_edges_objects(&o->oSpinyTargetYaw))) {

                    if (o->oSpinyTimeUntilTurn != 0) {
                        o->oSpinyTimeUntilTurn--;
                    } else {
                        o->oSpinyTargetYaw = o->oMoveAngleYaw + (s16) random_sign() * 0x2000;
                        o->oSpinyTimeUntilTurn = random_linear_offset(100, 100);
                    }
                }

                cur_obj_rotate_yaw_toward(o->oSpinyTargetYaw, 0x80);
            }

        } else if (o->oMoveFlags & OBJ_MOVE_HIT_WALL) {

            o->oMoveAngleYaw = cur_obj_reflect_move_angle_off_wall();
        }

        cur_obj_move_standard(-78);

        if (obj_handle_attacks(&sSpinyHitbox, SPINY_ACT_ATTACKED_MARIO, sSpinyWalkAttackHandlers)) {

            o->oAction = SPINY_ACT_WALK;
            o->oForwardVel *= 0.1f;
            o->oVelY *= 0.7f;

            o->oMoveFlags = 0;

            o->oInteractType = INTERACT_MR_BLIZZARD;
        } else {
            o->oInteractType = INTERACT_UNKNOWN_08;
        }
    }
}

static void spiny_act_held_by_lakitu(void) {
    o->oGraphYOffset = 15.0f;
    cur_obj_init_animation_with_sound(0);

    o->oParentRelativePosX = -50.0f;
    o->oParentRelativePosY = 35.0f;
    o->oParentRelativePosZ = -100.0f;

    if (o->parentObj->prevObj == NULL) {
        o->oAction = SPINY_ACT_THROWN_BY_LAKITU;
        o->oMoveAngleYaw = o->parentObj->oFaceAngleYaw;

        o->oForwardVel =
            o->parentObj->oForwardVel * coss(o->oMoveAngleYaw - o->parentObj->oMoveAngleYaw) + 10.0f;
        o->oVelY = 30.0f;

        o->oMoveFlags = 0;
    }
}

static void spiny_act_thrown_by_lakitu(void) {
    if (spiny_check_active()) {
        cur_obj_update_floor_and_walls();

        o->oGraphYOffset = 15.0f;
        o->oFaceAnglePitch -= 0x2000;

        cur_obj_init_animation_with_sound(0);

        if (o->oMoveFlags & OBJ_MOVE_LANDED) {
            cur_obj_play_sound_2(SOUND_OBJ_SPINY_UNK59);
            cur_obj_set_model(MODEL_SPINY);
            obj_init_animation_with_sound(o, spiny_seg5_anims_05016EAC, 0);
            o->oGraphYOffset = -17.0f;

            o->oFaceAnglePitch = 0;
            o->oAction = SPINY_ACT_WALK;
        } else if (o->oMoveFlags & OBJ_MOVE_HIT_WALL) {
            o->oMoveAngleYaw = cur_obj_reflect_move_angle_off_wall();
        }

        cur_obj_move_standard(-78);

        if (obj_check_attacks(&sSpinyHitbox, o->oAction) != 0 && o->parentObj != o) {
            o->parentObj->oEnemyLakituNumSpinies--;
        }
    }
}

void bhv_spiny_update(void) {

    switch (o->oAction) {
        case SPINY_ACT_WALK:
            spiny_act_walk();
            break;
        case SPINY_ACT_HELD_BY_LAKITU:
            spiny_act_held_by_lakitu();
            break;
        case SPINY_ACT_THROWN_BY_LAKITU:
            spiny_act_thrown_by_lakitu();
            break;
        case SPINY_ACT_ATTACKED_MARIO:
            obj_move_for_one_second(SPINY_ACT_WALK);
            break;
    }
}
