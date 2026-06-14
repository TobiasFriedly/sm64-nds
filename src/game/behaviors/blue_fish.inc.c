
void bhv_blue_fish_movement_loop(void) {
    f32 randomSwitch;

    switch (o->oAction) {

        case BLUE_FISH_ACT_DIVE:
            cur_obj_init_animation_with_accel_and_sound(0, 1.0f);

            if (o->oTimer == 0) {
                o->oBlueFishRandomAngle = random_sign() * 0x800;
                o->oBlueFishRandomVel = random_float() * 2;
                o->oBlueFishRandomTime = (s32)(random_float() * 30) & 0xFE;

                randomSwitch = random_float() * 5;
                if (randomSwitch < 2.0f) {
                    o->oAngleVelPitch = random_f32_around_zero(128);
                } else {
                    o->oAngleVelPitch = 0;
                }
            }

            o->oForwardVel = o->oBlueFishRandomVel + 3.0f;
            if (o->oTimer >= o->oBlueFishRandomTime + 60) {
                o->oAction++;
            }

            if (o->oTimer < (o->oBlueFishRandomTime + 60) / 2) {
                o->oFaceAnglePitch += o->oAngleVelPitch;
            } else {
                o->oFaceAnglePitch -= o->oAngleVelPitch;
            }

            o->oVelY = -sins(o->oFaceAnglePitch) * o->oForwardVel;
            break;

        case BLUE_FISH_ACT_TURN:
            cur_obj_init_animation_with_accel_and_sound(0, 2.0f);
            o->oMoveAngleYaw = (s32)(o->oBlueFishRandomAngle + o->oMoveAngleYaw);
            if (o->oTimer == 15) {
                o->oAction++;
            }
            break;

        case BLUE_FISH_ACT_ASCEND:
            cur_obj_init_animation_with_accel_and_sound(0, 1.0f);

            if (o->oTimer >= o->oBlueFishRandomTime + 60) {
                o->oAction++;
            }

            if (o->oTimer < (o->oBlueFishRandomTime + 60) / 2) {
                o->oFaceAnglePitch -= o->oAngleVelPitch;
            } else {
                o->oFaceAnglePitch += o->oAngleVelPitch;
            }
            break;

        case BLUE_FISH_ACT_TURN_BACK:
            cur_obj_init_animation_with_accel_and_sound(0, 2.0f);
            o->oMoveAngleYaw = (s32)(o->oBlueFishRandomAngle + o->oMoveAngleYaw);

            if (o->oTimer == 15) {
                o->oAction = BLUE_FISH_ACT_DIVE;
            }
            break;
    }

    o->oVelY = -sins(o->oFaceAnglePitch) * o->oForwardVel;
    cur_obj_move_using_fvel_and_gravity();

    if (o->parentObj->oAction == BLUE_FISH_ACT_DUPLICATE) {
        obj_mark_for_deletion(o);
    }
}

void bhv_tank_fish_group_loop(void) {
    struct Object *fish;
    s32 i;

    switch (o->oAction) {
        case BLUE_FISH_ACT_SPAWN:
            if (gMarioCurrentRoom == 15 || gMarioCurrentRoom == 7) {

                for (i = 0; i < 15; i++) {
                    fish = spawn_object_relative(0, 300, 0, -200, o, MODEL_FISH, bhvBlueFish);
                    obj_translate_xyz_random(fish, 200.0f);
                }

                o->oAction++;
            }
            break;

        case BLUE_FISH_ACT_ROOM:
            if (gMarioCurrentRoom != 15 && gMarioCurrentRoom != 7) {
                o->oAction++;
            }
            break;

        case BLUE_FISH_ACT_DUPLICATE:
            o->oAction = BLUE_FISH_ACT_SPAWN;
            break;
    }
}
