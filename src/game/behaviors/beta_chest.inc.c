
void bhv_beta_chest_bottom_init(void) {

    cur_obj_set_model(MODEL_TREASURE_CHEST_BASE);

    o->oMoveAngleYaw = random_u16();
    o->oMoveAngleYaw = 0;

    spawn_object_relative(0, 0, 97, -77, o, MODEL_TREASURE_CHEST_LID, bhvBetaChestLid);
}

void bhv_beta_chest_bottom_loop(void) {
    cur_obj_push_mario_away_from_cylinder(200.0f, 200.0f);
}

void bhv_beta_chest_lid_loop(void) {
    switch (o->oAction) {
        case BETA_CHEST_ACT_IDLE_CLOSED:
            if (dist_between_objects(o->parentObj, gMarioObject) < 300.0f) {
                o->oAction++;
            }

            break;

        case BETA_CHEST_ACT_OPENING:
            if (o->oTimer == 0) {

                spawn_object_relative(0, 0, -80, 120, o, MODEL_BUBBLE, bhvWaterAirBubble);
                play_sound(SOUND_GENERAL_CLAM_SHELL1, o->header.gfx.cameraToObject);
            }

            o->oFaceAnglePitch -= 0x400;
            if (o->oFaceAnglePitch < -0x4000) {
                o->oAction++;
            }

        case BETA_CHEST_ACT_IDLE_OPEN:
            break;
    }
}
