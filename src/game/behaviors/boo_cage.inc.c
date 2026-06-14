
static struct ObjectHitbox sBooCageHitbox = {
     INTERACT_BBH_ENTRANCE,
     0,
     0,
     0,
     0,
     120,
     300,
     0,
     0,
};

void bhv_boo_cage_loop(void) {
    UNUSED u8 filler[4];

    obj_set_hitbox(o, &sBooCageHitbox);

    switch (o->oAction) {
        case BOO_CAGE_ACT_IN_BOO:

            cur_obj_become_intangible();

            cur_obj_scale(1.0f);

            if (o->parentObj->oBooDeathStatus != BOO_DEATH_STATUS_ALIVE) {
                o->oAction++;
                o->oVelY = 60.0f;
                play_puzzle_jingle();
            } else {
                obj_copy_pos_and_angle(o, o->parentObj);
            }

            break;

        case BOO_CAGE_ACT_FALLING:

            o->oFaceAnglePitch = 0;
            o->oFaceAngleRoll = 0;

            cur_obj_update_floor_and_walls();
            cur_obj_move_standard(-78);

            spawn_object(o, MODEL_NONE, bhvSparkleSpawn);

            if (o->oMoveFlags & OBJ_MOVE_LANDED) {
                cur_obj_play_sound_2(SOUND_GENERAL_SOFT_LANDING);
            }

            if (o->oMoveFlags
                & (OBJ_MOVE_UNDERWATER_ON_GROUND | OBJ_MOVE_AT_WATER_SURFACE | OBJ_MOVE_ON_GROUND)) {
                o->oAction++;
            }

            break;

        case BOO_CAGE_ACT_ON_GROUND:

            cur_obj_become_tangible();

            cur_obj_scale(1.0f);

            if (obj_check_if_collided_with_object(o, gMarioObject)) {
                o->oAction++;
            }

            break;

        case BOO_CAGE_ACT_MARIO_JUMPING_IN:

            if (o->oTimer > 100) {
                o->oAction++;
            }

            break;

        case BOO_CAGE_ACT_USELESS:
            break;
    }
}
