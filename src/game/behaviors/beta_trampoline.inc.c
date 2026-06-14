
void bhv_beta_trampoline_spring_loop(void) {
    f32 yScale;
    f32 yDisplacement;

    obj_copy_pos_and_angle(o, o->parentObj);
    obj_copy_graph_y_offset(o, o->parentObj);
    o->oPosY -= 75.0f;

    if ((yDisplacement = o->oPosY - o->oHomeY) >= 0) {
        yScale = yDisplacement / 10.0 + 1.0;
    } else {

        yDisplacement = -yDisplacement;
        yScale = 1.0 - yDisplacement / 500.0;
    }

    obj_scale_xyz(o, 1.0f, yScale, 1.0f);
}

void bhv_beta_trampoline_top_loop(void) {
    cur_obj_set_model(MODEL_TRAMPOLINE);

    if (o->oTimer == 0) {
        struct Object *trampolinePart;

        trampolinePart = spawn_object(o, MODEL_TRAMPOLINE_CENTER, bhvBetaTrampolineSpring);
        trampolinePart->oPosY -= 75.0f;

        trampolinePart = spawn_object(o, MODEL_TRAMPOLINE_BASE, bhvStaticObject);
        trampolinePart->oPosY -= 150.0f;
    }

    if (gMarioObject->platform == o) {
        o->oBetaTrampolineMarioOnTrampoline = TRUE;
    } else {
        o->oBetaTrampolineMarioOnTrampoline = FALSE;
        o->oPosY = o->oHomeY;
    }

    stub_mario_step_2();
}
