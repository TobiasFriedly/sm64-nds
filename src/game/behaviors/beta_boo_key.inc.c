
void bhv_alpha_boo_key_loop(void) {

    o->oFaceAngleRoll += 0x200;
    o->oFaceAngleYaw += 0x200;

    if (obj_check_if_collided_with_object(o, gMarioObject)) {

        o->parentObj->oBooDeathStatus = BOO_DEATH_STATUS_DYING;

        obj_mark_for_deletion(o);
        spawn_object(o, MODEL_SPARKLES, bhvGoldenCoinSparkles);
    }
}

static void beta_boo_key_dropped_loop(void) {

    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(78);

    if (o->oGraphYOffset < 26.0f) {
        o->oGraphYOffset += 2.0f;
    }

    if (o->oFaceAngleRoll & 0xFFFF) {
        o->oFaceAngleRoll &= 0xF800;
        o->oFaceAngleRoll += 0x800;
    }

    if (o->oMoveFlags & OBJ_MOVE_ON_GROUND) {
        o->oVelX = 0.0f;
        o->oVelZ = 0.0f;
    }

    o->oFaceAngleYaw += 0x800;

    if (o->oTimer > 90 || o->oMoveFlags & OBJ_MOVE_LANDED) {
        cur_obj_become_tangible();

        if (obj_check_if_collided_with_object(o, gMarioObject)) {

            o->parentObj->oInteractStatus = TRUE;

            obj_mark_for_deletion(o);
            spawn_object(o, MODEL_SPARKLES, bhvGoldenCoinSparkles);
        }
    }
}

static void beta_boo_key_drop(void) {
    s16 velocityDirection;
    f32 velocityMagnitude;

    struct Object *parent = o->parentObj;
    obj_copy_pos(o, parent);

    if (o->oTimer == 0) {

        o->parentObj = parent->parentObj;

        o->oAction = BETA_BOO_KEY_ACT_DROPPED;

        velocityDirection = gMarioObject->oMoveAngleYaw;
        velocityMagnitude = 3.0f;

        o->oVelX = sins(velocityDirection) * velocityMagnitude;
        o->oVelZ = coss(velocityDirection) * velocityMagnitude;

        o->oVelY = 40.0f;
    }

    o->oFaceAngleYaw += 0x200;
    o->oFaceAngleRoll += 0x200;
}

static void beta_boo_key_inside_boo_loop(void) {

    struct Object *parent = o->parentObj;
    obj_copy_pos(o, parent);

    o->oPosY += 40.0f;

    if (parent->oBooDeathStatus != BOO_DEATH_STATUS_ALIVE) {
        o->oAction = BETA_BOO_KEY_ACT_DROPPING;
    }

    o->oFaceAngleRoll += 0x200;
    o->oFaceAngleYaw += 0x200;
}

static void (*sBetaBooKeyActions[])(void) = {
    beta_boo_key_inside_boo_loop,
    beta_boo_key_drop,
    beta_boo_key_dropped_loop,
};

void bhv_beta_boo_key_loop(void) {
    cur_obj_call_action_function(sBetaBooKeyActions);
}
