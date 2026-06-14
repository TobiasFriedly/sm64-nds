
void bhv_beta_bowser_anchor_loop(void) {

    cur_obj_set_pos_relative(gMarioObject, 0, 30.0f, 300.0f);

    o->hitboxRadius = gDebugInfo[DEBUG_PAGE_EFFECTINFO][0] + 100;
    o->hitboxHeight = gDebugInfo[DEBUG_PAGE_EFFECTINFO][1] + 300;

    obj_attack_collided_from_other_object(o);
}
