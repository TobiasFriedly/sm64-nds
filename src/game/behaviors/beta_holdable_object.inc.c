
void bhv_beta_holdable_object_init(void) {
    o->oGravity = 2.5f;
    o->oFriction = 0.8f;
    o->oBuoyancy = 1.3f;
}

static void beta_holdable_object_drop(void) {

    cur_obj_enable_rendering();

    cur_obj_get_dropped();

    o->oHeldState = HELD_FREE;

    o->oForwardVel = 0.0f;
    o->oVelY = 0.0f;
}

static void beta_holdable_object_throw(void) {

    cur_obj_enable_rendering_2();
    cur_obj_enable_rendering();

    o->oHeldState = HELD_FREE;

    o->oFlags &= ~OBJ_FLAG_SET_FACE_YAW_TO_MOVE_YAW;

    o->oForwardVel = 40.0f;
    o->oVelY = 20.0f;
}

void bhv_beta_holdable_object_loop(void) {
    switch (o->oHeldState) {
        case HELD_FREE:

            object_step();
            break;

        case HELD_HELD:

            cur_obj_disable_rendering();
            break;

        case HELD_THROWN:
            beta_holdable_object_throw();
            break;

        case HELD_DROPPED:
            beta_holdable_object_drop();
            break;
    }
}
