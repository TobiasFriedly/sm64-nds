
void bhv_hidden_blue_coin_loop(void) {
    struct Object *blueCoinSwitch;

    switch (o->oAction) {
        case HIDDEN_BLUE_COIN_ACT_INACTIVE:

            cur_obj_disable_rendering();
            cur_obj_become_intangible();

            o->oHiddenBlueCoinSwitch = cur_obj_nearest_object_with_behavior(bhvBlueCoinSwitch);

            if (o->oHiddenBlueCoinSwitch != NULL) {
                o->oAction++;
            }

            break;

        case HIDDEN_BLUE_COIN_ACT_WAITING:

            blueCoinSwitch = o->oHiddenBlueCoinSwitch;

            if (blueCoinSwitch->oAction == BLUE_COIN_SWITCH_ACT_TICKING) {
                o->oAction++;
            }

            break;

        case HIDDEN_BLUE_COIN_ACT_ACTIVE:

            cur_obj_enable_rendering();
            cur_obj_become_tangible();

            if (o->oInteractStatus & INT_STATUS_INTERACTED) {
                spawn_object(o, MODEL_SPARKLES, bhvGoldenCoinSparkles);
                obj_mark_for_deletion(o);
            }

            if (cur_obj_wait_then_blink(200, 20)) {
                obj_mark_for_deletion(o);
            }

            break;
    }

    o->oInteractStatus = 0;
}

void bhv_blue_coin_switch_loop(void) {

    cur_obj_scale(3.0f);

    switch (o->oAction) {
        case BLUE_COIN_SWITCH_ACT_IDLE:

            if (gMarioObject->platform == o) {
                if (gMarioStates[0].action == ACT_GROUND_POUND_LAND) {

                    o->oAction++;

                    o->oVelY = -20.0f;

                    o->oGravity = 0.0f;

                    cur_obj_play_sound_2(SOUND_GENERAL_SWITCH_DOOR_OPEN);
                }
            }

            load_object_collision_model();

            break;

        case BLUE_COIN_SWITCH_ACT_RECEDING:

            if (o->oTimer > 5) {
                cur_obj_hide();

                o->oAction++;

                o->oPosY = gMarioObject->oPosY - 40.0f;

                spawn_mist_particles_variable(0, 0, 46.0f);
            } else {

                load_object_collision_model();

                cur_obj_move_using_fvel_and_gravity();
            }

            break;

        case BLUE_COIN_SWITCH_ACT_TICKING:

            if (o->oTimer < 200) {
                play_sound(SOUND_GENERAL2_SWITCH_TICK_FAST, gGlobalSoundSource);
            } else {
                play_sound(SOUND_GENERAL2_SWITCH_TICK_SLOW, gGlobalSoundSource);
            }

            if (cur_obj_nearest_object_with_behavior(bhvHiddenBlueCoin) == NULL || o->oTimer > 240) {
                obj_mark_for_deletion(o);
            }

            break;
    }
}
