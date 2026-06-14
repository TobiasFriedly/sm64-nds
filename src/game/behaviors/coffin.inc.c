
struct LateralPosition {
    s16 x;
    s16 z;
};

struct LateralPosition coffinRelativePos[] = {
    { 412, -150 }, { 762, -150 }, { 1112, -150 },
    { 412,  150 }, { 762,  150 }, { 1112,  150 },
};

void bhv_coffin_spawner_loop(void) {
    struct Object *coffin;
    s32 i;
    s16 relativeZ;

    if (o->oAction == COFFIN_SPAWNER_ACT_COFFINS_UNLOADED) {
        if (!(o->activeFlags & ACTIVE_FLAG_IN_DIFFERENT_ROOM)) {
            for (i = 0; i < 6; i++) {
                relativeZ = coffinRelativePos[i].z;

                coffin = spawn_object_relative(i & 1, coffinRelativePos[i].x, 0, relativeZ, o,
                                               MODEL_BBH_WOODEN_TOMB, bhvCoffin);

                if (coffin != NULL) {

                    if (relativeZ > 0) {
                        coffin->oFaceAngleYaw = 0x8000;
                    }
                }
            }

            o->oAction++;
        }
    } else if (o->activeFlags & ACTIVE_FLAG_IN_DIFFERENT_ROOM) {
        o->oAction = COFFIN_SPAWNER_ACT_COFFINS_UNLOADED;
    }
}

void coffin_act_idle(void) {
    if (o->oBhvParams2ndByte != COFFIN_BP_STATIC) {

        if (o->oFaceAnglePitch != 0) {
            o->oAngleVelPitch = approach_s16_symmetric(o->oAngleVelPitch, -2000, 200);

            if (obj_face_pitch_approach(0, -o->oAngleVelPitch)) {
                cur_obj_play_sound_2(SOUND_GENERAL_ELEVATOR_MOVE_2);

                obj_perform_position_op(POS_OP_SAVE_POSITION);
                o->oMoveAngleYaw = o->oFaceAngleYaw - 0x4000;
                obj_set_dist_from_home(200.0f);
                spawn_mist_from_global();
                obj_perform_position_op(POS_OP_RESTORE_POSITION);
            }

            o->oTimer = 0;
        } else {

            f32 yawCos = coss(o->oFaceAngleYaw);
            f32 yawSin = sins(o->oFaceAngleYaw);

            f32 dx = gMarioObject->oPosX - o->oPosX;
            f32 dz = gMarioObject->oPosZ - o->oPosZ;

            f32 distForwards = dx * yawCos + dz * yawSin;
            f32 distSideways = dz * yawCos - dx * yawSin;

            if (o->oTimer > 60
                && (o->oDistanceToMario > 100.0f || gMarioState->action == ACT_SQUISHED)
                && gMarioObject->oPosY - o->oPosY < 200.0f && absf(distForwards) < 140.0f
                && distSideways < 150.0f && distSideways > -450.0f) {
                cur_obj_play_sound_2(SOUND_GENERAL_BUTTON_PRESS_2_LOWPRIO);
                o->oAction = COFFIN_ACT_STAND_UP;
            }

            o->oAngleVelPitch = 0;
        }
    }
}

void coffin_act_stand_up(void) {

    if (o->oFaceAnglePitch != 0x4000) {
        o->oAngleVelPitch = approach_s16_symmetric(o->oAngleVelPitch, 1000, 200);
        obj_face_pitch_approach(0x4000, o->oAngleVelPitch);
    } else {

        if (o->oTimer > 60) {
            o->oAction = COFFIN_ACT_IDLE;
            o->oFaceAngleRoll = 0;
        } else if (o->oTimer > 30) {
            if (gGlobalTimer % 4 == 0) {
                cur_obj_play_sound_2(SOUND_GENERAL_ELEVATOR_MOVE_2);
            }

            o->oFaceAngleRoll = 400 * (gGlobalTimer % 2) - 200;
        }

        o->oAngleVelPitch = 0;
    }
}

void bhv_coffin_loop(void) {

    if (o->parentObj->oAction == COFFIN_SPAWNER_ACT_COFFINS_UNLOADED) {
        obj_mark_for_deletion(o);
    } else {

        o->header.gfx.scale[1] = 1.1f;

        switch (o->oAction) {
            case COFFIN_ACT_IDLE:
                coffin_act_idle();
                break;
            case COFFIN_ACT_STAND_UP:
                coffin_act_stand_up();
                break;
        }

        load_object_collision_model();
    }
}
