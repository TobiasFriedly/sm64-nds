
struct ObjectHitbox sTweesterHitbox = {
     INTERACT_TORNADO,
     0,
     0,
     0,
     0,
     1500,
     4000,
     0,
     0,
};

void tweester_scale_and_move(f32 preScale) {
    s16 dYaw  = 0x2C00;
    f32 scale = preScale * 0.4;

    o->header.gfx.scale[0]
        = (( coss(o->oTweesterScaleTimer) + 1.0) * 0.5 * 0.3 + 1.0) * scale;
    o->header.gfx.scale[1]
        = ((-coss(o->oTweesterScaleTimer) + 1.0) * 0.5 * 0.5 + 0.5) * scale;
    o->header.gfx.scale[2]
        = (( coss(o->oTweesterScaleTimer) + 1.0) * 0.5 * 0.3 + 1.0) * scale;

    o->oTweesterScaleTimer += 0x200;
    o->oForwardVel = 14.0f;
    o->oFaceAngleYaw += dYaw;
}

void tweester_act_idle(void) {
    if (o->oSubAction == TWEESTER_SUB_ACT_WAIT) {
        cur_obj_become_tangible();
        cur_obj_set_pos_to_home();
        cur_obj_scale(0.0f);

        o->oTweesterUnused = 0;

        if (o->oDistanceToMario < 1500.0f) {
            o->oSubAction++;
        }

        o->oTimer = 0;
    } else {
        cur_obj_play_sound_1(SOUND_ENV_WIND1);
        tweester_scale_and_move(o->oTimer / 60.0f);
        if (o->oTimer >= 60) {
            o->oAction = TWEESTER_ACT_CHASE;
        }
    }
}

void tweester_act_chase(void) {
    f32 activationRadius = o->oBhvParams2ndByte * 100;

    o->oAngleToHome = cur_obj_angle_to_home();
    cur_obj_play_sound_1(SOUND_ENV_WIND1);

    if (cur_obj_lateral_dist_from_mario_to_home() < activationRadius
        && o->oSubAction == TWEESTER_SUB_ACT_CHASE) {

        o->oForwardVel = 20.0f;
        cur_obj_rotate_yaw_toward(o->oAngleToMario, 0x200);
        print_debug_top_down_objectinfo("off ", 0);

        if (gMarioStates[0].action == ACT_TWIRLING) {
            o->oSubAction++;
        }
    } else {
        o->oForwardVel = 20.0f;
        cur_obj_rotate_yaw_toward(o->oAngleToHome, 0x200);

        if (cur_obj_lateral_dist_to_home() < 200.0f) {
            o->oAction = TWEESTER_ACT_HIDE;
        }
    }

    if (o->oDistanceToMario > 3000.0f) {
        o->oAction = TWEESTER_ACT_HIDE;
    }

    cur_obj_update_floor_and_walls();
    if (o->oMoveFlags & OBJ_MOVE_HIT_WALL) {
        o->oMoveAngleYaw = o->oWallAngle;
    }

    cur_obj_move_standard(60);
    tweester_scale_and_move(1.0f);
    spawn_object(o, MODEL_SAND_DUST, bhvTweesterSandParticle);
}

void tweester_act_hide(void) {
    f32 shrinkTimer = 60.0f - o->oTimer;

    if (shrinkTimer >= 0.0f) {
        tweester_scale_and_move(shrinkTimer / 60.0f);
    } else {
        cur_obj_become_intangible();
        if (cur_obj_lateral_dist_from_mario_to_home() > 2500.0f) {
            o->oAction = TWEESTER_ACT_IDLE;
        }
        if (o->oTimer > 360) {
            o->oAction = TWEESTER_ACT_IDLE;
        }
    }
}

void (*sTweesterActions[])(void) = {
    tweester_act_idle,
    tweester_act_chase,
    tweester_act_hide,
};

void bhv_tweester_loop(void) {
    obj_set_hitbox(o, &sTweesterHitbox);
    cur_obj_call_action_function(sTweesterActions);
    o->oInteractStatus = 0;
}

void bhv_tweester_sand_particle_loop(void) {
    o->oMoveAngleYaw += 0x3700;
    o->oForwardVel += 15.0f;
    o->oPosY += 22.0f;

    cur_obj_scale(random_float() + 1.0);

    if (o->oTimer == 0) {
        obj_translate_xz_random(o, 100.0f);
        o->oFaceAnglePitch = random_u16();
        o->oFaceAngleYaw = random_u16();
    }

    if (o->oTimer > 15) {
        obj_mark_for_deletion(o);
    }
}
