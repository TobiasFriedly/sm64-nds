
void bhv_ukiki_cage_star_loop(void) {
    switch (o->oAction) {
        case UKIKI_CAGE_STAR_ACT_IN_CAGE:

            if (o->oTimer == 0) {
                if (bit_shift_left(1)
                    & save_file_get_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(gCurrCourseNum))) {
                    cur_obj_set_model(MODEL_TRANSPARENT_STAR);
                }
            }

            obj_copy_pos(o, o->parentObj);
            obj_copy_behavior_params(o, o->parentObj);

            if (o->parentObj->oAction == UKIKI_CAGE_ACT_HIDE) {
                o->oAction++;
            }
            break;

        case UKIKI_CAGE_STAR_ACT_SPAWN_STAR:
            obj_mark_for_deletion(o);
            spawn_mist_particles();
            spawn_triangle_break_particles(20, MODEL_DIRT_ANIMATION, 0.7f, 3);
            spawn_default_star(2500.0f, -1200.0f, 1300.0f);
            break;
    }

    o->oFaceAngleYaw += 0x400;
}

void ukiki_cage_act_wait_for_ukiki(void) {
    if (o->oUkikiCageNextAction != UKIKI_CAGE_ACT_WAIT_FOR_UKIKI) {
        o->oAction = UKIKI_CAGE_ACT_SPIN;
    }

    load_object_collision_model();
}

void ukiki_cage_act_spin(void) {
    if (o->oUkikiCageNextAction != UKIKI_CAGE_ACT_SPIN) {
        o->oAction = UKIKI_CAGE_ACT_FALL;
    }

    o->oMoveAngleYaw += 0x800;
    load_object_collision_model();
}

void ukiki_cage_act_fall(void) {

    cur_obj_update_floor_and_walls();
    cur_obj_move_standard(78);
    if (o->oMoveFlags & (OBJ_MOVE_LANDED | OBJ_MOVE_ENTERED_WATER)) {
        o->oAction = UKIKI_CAGE_ACT_HIDE;
    }
}

void ukiki_cage_act_hide(void) {
    cur_obj_hide();
}

void (*sUkikiCageActions[])(void) = {
    ukiki_cage_act_wait_for_ukiki,
    ukiki_cage_act_spin,
    ukiki_cage_act_fall,
    ukiki_cage_act_hide,
};

void bhv_ukiki_cage_loop(void) {
    cur_obj_call_action_function(sUkikiCageActions);
}
