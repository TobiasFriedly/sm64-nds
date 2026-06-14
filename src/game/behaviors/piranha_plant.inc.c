
void piranha_plant_act_idle(void) {
    cur_obj_become_intangible();
    cur_obj_init_animation_with_sound(8);

#if BUGFIX_PIRANHA_PLANT_STATE_RESET

    cur_obj_scale(1.0f);
#endif

    if (o->oDistanceToMario < 1200.0f) {
        o->oAction = PIRANHA_PLANT_ACT_SLEEPING;
    }
}

s32 piranha_plant_check_interactions(void) {
    s32 i;
    s32 interacted = TRUE;

    if (o->oInteractStatus & INT_STATUS_INTERACTED) {
        func_80321080(50);
        if (o->oInteractStatus & INT_STATUS_WAS_ATTACKED) {
            cur_obj_play_sound_2(SOUND_OBJ2_PIRANHA_PLANT_DYING);

            for (i = 0; i < 20; i++) {
                spawn_object(o, MODEL_PURPLE_MARBLE, bhvPurpleParticle);
            }
            o->oAction = PIRANHA_PLANT_ACT_ATTACKED;
        } else {
            o->oAction = PIRANHA_PLANT_ACT_WOKEN_UP;
        }
        o->oInteractStatus = 0;
    } else {
        interacted = FALSE;
    }

    return interacted;
}

void piranha_plant_act_sleeping(void) {
    cur_obj_become_tangible();
    o->oInteractType = INTERACT_BOUNCE_TOP;

    cur_obj_init_animation_with_sound(8);

    cur_obj_set_hitbox_radius_and_height(250.0f, 200.0f);
    cur_obj_set_hurtbox_radius_and_height(150.0f, 100.0f);

#if BUGFIX_PIRANHA_PLANT_SLEEP_DAMAGE

    o->oDamageOrCoinValue = 0;
#elif defined(VERSION_EU)

    o->oDamageOrCoinValue = 3;
#endif

    if (o->oDistanceToMario < 400.0f) {
        if (mario_moving_fast_enough_to_make_piranha_plant_bite()) {
            o->oAction = PIRANHA_PLANT_ACT_WOKEN_UP;
        }
    } else if (o->oDistanceToMario < 1000.0f) {
        play_secondary_music(SEQ_EVENT_PIRANHA_PLANT, 0, 255, 1000);
        o->oPiranhaPlantSleepMusicState = PIRANHA_PLANT_SLEEP_MUSIC_PLAYING;
    } else if (o->oPiranhaPlantSleepMusicState == PIRANHA_PLANT_SLEEP_MUSIC_PLAYING) {
        o->oPiranhaPlantSleepMusicState++;
        func_80321080(50);
    }
    piranha_plant_check_interactions();
}

void piranha_plant_act_woken_up(void) {
#if BUGFIX_PIRANHA_PLANT_SLEEP_DAMAGE || defined(VERSION_EU)

    o->oDamageOrCoinValue = 3;
#endif
    if (o->oTimer == 0) {
        func_80321080(50);
    }

    if (!piranha_plant_check_interactions() && o->oTimer > 10) {
        o->oAction = PIRANHA_PLANT_ACT_BITING;
    }
}

#if BUGFIX_PIRANHA_PLANT_STATE_RESET

void piranha_plant_reset_when_far(void) {
    if (o->activeFlags & ACTIVE_FLAG_FAR_AWAY) {
        o->oAction = PIRANHA_PLANT_ACT_IDLE;
    }
}
#endif

void piranha_plant_attacked(void) {
    cur_obj_become_intangible();
    cur_obj_init_animation_with_sound(2);
    o->oInteractStatus = 0;
    if (cur_obj_check_if_near_animation_end()) {
        o->oAction = PIRANHA_PLANT_ACT_SHRINK_AND_DIE;
    }
#if BUGFIX_PIRANHA_PLANT_STATE_RESET
    piranha_plant_reset_when_far();
#endif
}

void piranha_plant_act_shrink_and_die(void) {
    if (o->oTimer == 0) {
        cur_obj_play_sound_2(SOUND_OBJ_ENEMY_DEFEAT_SHRINK);
        o->oPiranhaPlantScale = 1.0f;
    }

    if (o->oPiranhaPlantScale > 0.0f) {

        o->oPiranhaPlantScale = o->oPiranhaPlantScale - 0.04;
    } else {
        o->oPiranhaPlantScale = 0.0f;
        cur_obj_spawn_loot_blue_coin();
        o->oAction = PIRANHA_PLANT_ACT_WAIT_TO_RESPAWN;
    }

    cur_obj_scale(o->oPiranhaPlantScale);

#if BUGFIX_PIRANHA_PLANT_STATE_RESET
    piranha_plant_reset_when_far();
#endif
}

void piranha_plant_act_wait_to_respawn(void) {
    if (o->oDistanceToMario > 1200.0f) {
        o->oAction = PIRANHA_PLANT_ACT_RESPAWN;
    }
}

void piranha_plant_act_respawn(void) {
    cur_obj_init_animation_with_sound(8);
    if (o->oTimer == 0) {
        o->oPiranhaPlantScale = 0.3f;
    }

    if (o->oPiranhaPlantScale < 1.0) {

        o->oPiranhaPlantScale += 0.02;
    } else {
        o->oPiranhaPlantScale = 1.0f;
        o->oAction = PIRANHA_PLANT_ACT_IDLE;
    }
    cur_obj_scale(o->oPiranhaPlantScale);
}

static s8 sPiranhaPlantBiteSoundFrames[] = { 12, 28, 50, 64, -1 };

void piranha_plant_act_biting(void) {
    s32 animFrame = o->header.gfx.animInfo.animFrame;

    cur_obj_become_tangible();

    o->oInteractType = INTERACT_DAMAGE;

    cur_obj_init_animation_with_sound(0);

    cur_obj_set_hitbox_radius_and_height(150.0f, 100.0f);
    cur_obj_set_hurtbox_radius_and_height(150.0f, 100.0f);

    if (is_item_in_array(animFrame, sPiranhaPlantBiteSoundFrames)) {
        cur_obj_play_sound_2(SOUND_OBJ2_PIRANHA_PLANT_BITE);
    }

    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, o->oAngleToMario, 0x400);

    if (o->oDistanceToMario > 500.0f && cur_obj_check_if_near_animation_end()) {
        o->oAction = PIRANHA_PLANT_ACT_STOPPED_BITING;
    }

    if ((o->oInteractStatus & INT_STATUS_INTERACTED) && (gMarioState->flags & MARIO_METAL_CAP)) {
        o->oAction = PIRANHA_PLANT_ACT_ATTACKED;
    }
}

s32 mario_moving_fast_enough_to_make_piranha_plant_bite(void) {
    if (gMarioStates[0].vel[1] > 10.0f) {
        return TRUE;
    }
    if (gMarioStates[0].forwardVel > 10.0f) {
        return TRUE;
    }
    return FALSE;
}

void piranha_plant_act_stopped_biting(void) {
    cur_obj_become_intangible();
    cur_obj_init_animation_with_sound(6);

    if (cur_obj_check_if_near_animation_end()) {
        o->oAction = PIRANHA_PLANT_ACT_SLEEPING;
    }

    if (o->oDistanceToMario < 400.0f && mario_moving_fast_enough_to_make_piranha_plant_bite()) {
        o->oAction = PIRANHA_PLANT_ACT_BITING;
    }
}

void (*TablePiranhaPlantActions[])(void) = {
    piranha_plant_act_idle,
    piranha_plant_act_sleeping,
    piranha_plant_act_biting,
    piranha_plant_act_woken_up,
    piranha_plant_act_stopped_biting,
    piranha_plant_attacked,
    piranha_plant_act_shrink_and_die,
    piranha_plant_act_wait_to_respawn,
    piranha_plant_act_respawn,
};

void bhv_piranha_plant_loop(void) {
    cur_obj_call_action_function(TablePiranhaPlantActions);

    if (gCurrLevelNum == LEVEL_WF) {
        if (gMarioObject->oPosY > 3400.0f) {
            cur_obj_hide();
        } else {
            cur_obj_unhide();
        }
    }

    o->oInteractStatus = 0;
}
