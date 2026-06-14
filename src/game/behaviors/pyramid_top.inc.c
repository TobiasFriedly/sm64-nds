
void bhv_pyramid_top_init(void) {
    spawn_object_abs_with_rot(o, 0, MODEL_NONE, bhvPyramidPillarTouchDetector,
                              1789, 1024, 764, 0, 0, 0);
    spawn_object_abs_with_rot(o, 0, MODEL_NONE, bhvPyramidPillarTouchDetector,
                              1789, 896, -2579, 0, 0, 0);
    spawn_object_abs_with_rot(o, 0, MODEL_NONE, bhvPyramidPillarTouchDetector,
                              -5883, 1024, -2579, 0, 0, 0);
    spawn_object_abs_with_rot(o, 0, MODEL_NONE, bhvPyramidPillarTouchDetector,
                              -5883, 1024, 764, 0, 0, 0);
}

void bhv_pyramid_top_spinning(void) {

    o->oPosX = o->oHomeX + sins(o->oTimer * 0x4000) * 40.0f;

    if (o->oTimer < 60) {
        o->oPosY = o->oHomeY + absf_2(sins(o->oTimer * 0x2000) * 10.0f);
    } else {

        o->oAngleVelYaw += 0x100;
        if (o->oAngleVelYaw > 0x1800) {
            o->oAngleVelYaw = 0x1800;
            o->oVelY = 5.0f;
        }

        o->oFaceAngleYaw += o->oAngleVelYaw;
        o->oPosY += o->oVelY;
    }

    if (o->oTimer < 90) {
        struct Object *pyramidFragment = spawn_object(o, MODEL_DIRT_ANIMATION, bhvPyramidTopFragment);
        pyramidFragment->oForwardVel = random_float() * 10.0f + 20.0f;
        pyramidFragment->oMoveAngleYaw = random_u16();
        pyramidFragment->oPyramidTopFragmentsScale = 0.8f;
        pyramidFragment->oGravity = random_float() + 2.0f;
    }

    if (o->oTimer == 150) {
        o->oAction = PYRAMID_TOP_ACT_EXPLODE;
    }
}

void bhv_pyramid_top_explode(void) {
    struct Object *pyramidFragment;
    s16 i;

    spawn_mist_particles_variable(0, 0, 690.0f);

    for (i = 0; i < 30; i++) {
        pyramidFragment = spawn_object(o, MODEL_DIRT_ANIMATION, bhvPyramidTopFragment);
        pyramidFragment->oForwardVel = random_float() * 50 + 80;
        pyramidFragment->oVelY = random_float() * 80 + 20;
        pyramidFragment->oMoveAngleYaw = random_u16();
        pyramidFragment->oPyramidTopFragmentsScale = 3;
        pyramidFragment->oGravity = random_float() * 2 + 5;
    }

    o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
}

void bhv_pyramid_top_loop(void) {
    switch (o->oAction) {
        case PYRAMID_TOP_ACT_CHECK_IF_SOLVED:
            if (o->oPyramidTopPillarsTouched == 4) {
                play_puzzle_jingle();
                o->oAction = PYRAMID_TOP_ACT_SPINNING;
            }
            break;

        case PYRAMID_TOP_ACT_SPINNING:
            if (o->oTimer == 0) {
                cur_obj_play_sound_2(SOUND_GENERAL2_PYRAMID_TOP_SPIN);
            }

            bhv_pyramid_top_spinning();
            break;

        case PYRAMID_TOP_ACT_EXPLODE:
            if (o->oTimer == 0) {
                create_sound_spawner(SOUND_GENERAL2_PYRAMID_TOP_EXPLOSION);
            }

            bhv_pyramid_top_explode();
            break;
    }
}

void bhv_pyramid_top_fragment_init(void) {
    o->oFriction = 0.999f;
    o->oBuoyancy = 2.0f;
    o->oAnimState = 3;
    cur_obj_scale(o->oPyramidTopFragmentsScale);
}

void bhv_pyramid_top_fragment_loop(void) {
    object_step();
    o->oFaceAngleYaw += 0x1000;
    o->oFaceAnglePitch += 0x1000;

    if (o->oTimer == 60) {
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }
}

void bhv_pyramid_pillar_touch_detector_loop(void) {
    cur_obj_become_tangible();
    if (obj_check_if_collided_with_object(o, gMarioObject) == TRUE) {

        o->parentObj->oPyramidTopPillarsTouched++;
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }
}
