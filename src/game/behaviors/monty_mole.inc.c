
struct Object *sMontyMoleHoleList;

s32 sMontyMoleKillStreak;

f32 sMontyMoleLastKilledPosX;
f32 sMontyMoleLastKilledPosY;
f32 sMontyMoleLastKilledPosZ;

static struct Object *link_objects_with_behavior(const BehaviorScript *behavior) {
    const BehaviorScript *behaviorAddr = segmented_to_virtual(behavior);
    struct Object *obj;
    struct Object *lastObject = NULL;
    struct ObjectNode *listHead = &gObjectLists[get_object_list_from_behavior(behaviorAddr)];

    obj = (struct Object *) listHead->next;
    while (obj != (struct Object *) listHead) {
        if (obj->behavior == behaviorAddr && obj->activeFlags != ACTIVE_FLAG_DEACTIVATED) {
            obj->parentObj = lastObject;
            lastObject = obj;
        }

        obj = (struct Object *) obj->header.next;
    }

    return lastObject;
}

static struct Object *monty_mole_select_available_hole(f32 minDistToMario) {
    struct Object *hole = sMontyMoleHoleList;
    s32 numAvailableHoles = 0;

    while (hole != NULL) {
        if (hole->oMontyMoleHoleCooldown == 0) {
            if (hole->oDistanceToMario < 1500.0f && hole->oDistanceToMario > minDistToMario) {
                numAvailableHoles++;
            }
        }

        hole = hole->parentObj;
    }

    if (numAvailableHoles != 0) {
        s32 selectedHole = (s32)(random_float() * numAvailableHoles);

        hole = sMontyMoleHoleList;
        numAvailableHoles = 0;

        while (hole != NULL) {
            if (hole->oMontyMoleHoleCooldown == 0) {
                if (hole->oDistanceToMario < 1500.0f && hole->oDistanceToMario > minDistToMario) {
                    if (numAvailableHoles == selectedHole) {
                        return hole;
                    }

                    numAvailableHoles++;
                }
            }

            hole = hole->parentObj;
        }
    }

    return NULL;
}

void bhv_monty_mole_hole_update(void) {

    if (o->parentObj == o) {
        sMontyMoleHoleList = link_objects_with_behavior(bhvMontyMoleHole);
        sMontyMoleKillStreak = 0;
    } else if (o->oMontyMoleHoleCooldown > 0) {
        o->oMontyMoleHoleCooldown--;
    }
}

void monty_mole_spawn_dirt_particles(s8 offsetY, s8 velYBase) {
    static struct SpawnParticlesInfo montyMoleRiseFromGroundParticles = {
         0,
         3,
         MODEL_SAND_DUST,
         0,
         4,
         4,
         10,
         15,
         -4,
         0,
         10.0f,
         7.0f,
    };

    montyMoleRiseFromGroundParticles.offsetY = offsetY;
    montyMoleRiseFromGroundParticles.velYBase = velYBase;

    cur_obj_spawn_particles(&montyMoleRiseFromGroundParticles);
}

void bhv_monty_mole_init(void) {
    o->oMontyMoleCurrentHole = NULL;
}

static void monty_mole_act_select_hole(void) {
    f32 minDistToMario;

    if (o->oBhvParams2ndByte != MONTY_MOLE_BP_NO_ROCK) {
        minDistToMario = 200.0f;
    } else if (gMarioStates[0].forwardVel < 8.0f) {
        minDistToMario = 100.0f;
    } else {
        minDistToMario = 500.0f;
    }

    if ((o->oMontyMoleCurrentHole = monty_mole_select_available_hole(minDistToMario)) != NULL) {
        cur_obj_play_sound_2(SOUND_OBJ2_MONTY_MOLE_APPEAR);

        o->oMontyMoleCurrentHole->oMontyMoleHoleCooldown = -1;

        o->oPosX = o->oMontyMoleCurrentHole->oPosX;
        o->oPosY = o->oFloorHeight = o->oMontyMoleCurrentHole->oPosY;
        o->oPosZ = o->oMontyMoleCurrentHole->oPosZ;

        o->oFaceAnglePitch = 0;
        o->oMoveAngleYaw = o->oMontyMoleCurrentHole->oAngleToMario;

        if (o->oDistanceToMario > 500.0f || minDistToMario > 100.0f || random_sign() < 0) {
            o->oAction = MONTY_MOLE_ACT_RISE_FROM_HOLE;
            o->oVelY = 3.0f;
            o->oGravity = 0.0f;
            monty_mole_spawn_dirt_particles(0, 10);
        } else {
            o->oAction = MONTY_MOLE_ACT_JUMP_OUT_OF_HOLE;
            o->oVelY = 50.0f;
            o->oGravity = -4.0f;
            monty_mole_spawn_dirt_particles(0, 20);
        }

        cur_obj_unhide();
        cur_obj_become_tangible();
    }
}

static void monty_mole_act_rise_from_hole(void) {
    cur_obj_init_animation_with_sound(1);

    if (o->oMontyMoleHeightRelativeToFloor >= 49.0f) {
        o->oPosY = o->oFloorHeight + 50.0f;
        o->oVelY = 0.0f;

        if (cur_obj_check_if_near_animation_end()) {
            o->oAction = MONTY_MOLE_ACT_SPAWN_ROCK;
        }
    }
}

static void monty_mole_act_spawn_rock(void) {
    struct Object *rock;

    if (cur_obj_init_anim_and_check_if_end(2)) {
        if (o->oBhvParams2ndByte != MONTY_MOLE_BP_NO_ROCK
            && abs_angle_diff(o->oAngleToMario, o->oMoveAngleYaw) < 0x4000
            && (rock = spawn_object(o, MODEL_PEBBLE, bhvMontyMoleRock)) != NULL) {
            o->prevObj = rock;
            o->oAction = MONTY_MOLE_ACT_THROW_ROCK;
        } else {
            o->oAction = MONTY_MOLE_ACT_BEGIN_JUMP_INTO_HOLE;
        }
    }
}

static void monty_mole_act_begin_jump_into_hole(void) {
    if (cur_obj_init_anim_and_check_if_end(3) || obj_is_near_to_and_facing_mario(1000.0f, 0x4000)) {
        o->oAction = MONTY_MOLE_ACT_JUMP_INTO_HOLE;
        o->oVelY = 40.0f;
        o->oGravity = -6.0f;
    }
}

static void monty_mole_act_throw_rock(void) {
    if (cur_obj_init_anim_check_frame(8, 10)) {
        cur_obj_play_sound_2(SOUND_OBJ_MONTY_MOLE_ATTACK);
        o->prevObj = NULL;
    }

    if (cur_obj_check_if_near_animation_end()) {
        o->oAction = MONTY_MOLE_ACT_BEGIN_JUMP_INTO_HOLE;
    }
}

static void monty_mole_act_jump_into_hole(void) {
    cur_obj_init_anim_extend(0);

    o->oFaceAnglePitch = -atan2s(o->oVelY, -4.0f);

    if (o->oVelY < 0.0f && o->oMontyMoleHeightRelativeToFloor < 120.0f) {
        o->oAction = MONTY_MOLE_ACT_HIDE;
        o->oGravity = 0.0f;
        monty_mole_spawn_dirt_particles(-80, 15);
    }
}

static void monty_mole_hide_in_hole(void) {
    o->oMontyMoleCurrentHole->oMontyMoleHoleCooldown = 30;
    o->oAction = MONTY_MOLE_ACT_SELECT_HOLE;
    o->oVelY = 0.0f;

    cur_obj_become_intangible();
}

static void monty_mole_act_hide(void) {
    cur_obj_init_animation_with_sound(1);

    if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {
        cur_obj_hide();
        monty_mole_hide_in_hole();
    } else {
        approach_f32_ptr(&o->oVelY, -4.0f, 0.5f);
    }
}

static void monty_mole_act_jump_out_of_hole(void) {
    if (o->oVelY > 0.0f) {
        cur_obj_init_animation_with_sound(9);
    } else {
        cur_obj_init_anim_extend(4);

        if (o->oMontyMoleHeightRelativeToFloor < 50.0f) {
            o->oPosY = o->oFloorHeight + 50.0f;
            o->oAction = MONTY_MOLE_ACT_BEGIN_JUMP_INTO_HOLE;
            o->oVelY = o->oGravity = 0.0f;
        }
    }
}

static struct ObjectHitbox sMontyMoleHitbox = {
     INTERACT_BOUNCE_TOP,
     0,
     2,
     -1,
     0,
     70,
     50,
     30,
     40,
};

void bhv_monty_mole_update(void) {

    o->oDeathSound = SOUND_OBJ_DYING_ENEMY1;
    cur_obj_update_floor_and_walls();

    o->oMontyMoleHeightRelativeToFloor = o->oPosY - o->oFloorHeight;

    switch (o->oAction) {
        case MONTY_MOLE_ACT_SELECT_HOLE:
            monty_mole_act_select_hole();
            break;
        case MONTY_MOLE_ACT_RISE_FROM_HOLE:
            monty_mole_act_rise_from_hole();
            break;
        case MONTY_MOLE_ACT_SPAWN_ROCK:
            monty_mole_act_spawn_rock();
            break;
        case MONTY_MOLE_ACT_BEGIN_JUMP_INTO_HOLE:
            monty_mole_act_begin_jump_into_hole();
            break;
        case MONTY_MOLE_ACT_THROW_ROCK:
            monty_mole_act_throw_rock();
            break;
        case MONTY_MOLE_ACT_JUMP_INTO_HOLE:
            monty_mole_act_jump_into_hole();
            break;
        case MONTY_MOLE_ACT_HIDE:
            monty_mole_act_hide();
            break;
        case MONTY_MOLE_ACT_JUMP_OUT_OF_HOLE:
            monty_mole_act_jump_out_of_hole();
            break;
    }

    if (obj_check_attacks(&sMontyMoleHitbox, o->oAction) != 0) {
        if (sMontyMoleKillStreak != 0) {
            f32 dx = o->oPosX - sMontyMoleLastKilledPosX;
            f32 dy = o->oPosY - sMontyMoleLastKilledPosY;
            f32 dz = o->oPosZ - sMontyMoleLastKilledPosZ;

            f32 distToLastKill = sqrtf(dx * dx + dy * dy + dz * dz);

            if (distToLastKill < 1500.0f) {
                if (sMontyMoleKillStreak == 7) {
                    play_puzzle_jingle();
                    spawn_object(o, MODEL_1UP, bhv1UpWalking);
                }
            } else {
                sMontyMoleKillStreak = 0;
            }
        }

        sMontyMoleKillStreak++;

        sMontyMoleLastKilledPosX = o->oPosX;
        sMontyMoleLastKilledPosY = o->oPosY;
        sMontyMoleLastKilledPosZ = o->oPosZ;

        monty_mole_hide_in_hole();

        o->prevObj = NULL;
    }

    cur_obj_move_standard(78);
}

static void monty_mole_rock_act_held(void) {

    o->oParentRelativePosX = 80.0f;
    o->oParentRelativePosY = -50.0f;
    o->oParentRelativePosZ = 0.0f;

    if (o->parentObj->prevObj == NULL) {
        f32 distToMario = o->oDistanceToMario;
        if (distToMario > 600.0f) {
            distToMario = 600.0f;
        }

        o->oAction = MONTY_MOLE_ROCK_ACT_MOVE;

        o->oMoveAngleYaw = (s32)(o->parentObj->oMoveAngleYaw + 500 - distToMario * 0.1f);

        o->oForwardVel = 40.0f;
        o->oVelY = distToMario * 0.08f + 8.0f;

        o->oMoveFlags = 0;
    }
}

static struct ObjectHitbox sMontyMoleRockHitbox = {
     INTERACT_MR_BLIZZARD,
     15,
     1,
     99,
     0,
     30,
     15,
     30,
     15,
};

static struct SpawnParticlesInfo sMontyMoleRockBreakParticles = {
     0,
     2,
     MODEL_PEBBLE,
     10,
     4,
     4,
     10,
     15,
     -4,
     0,
     8.0f,
     4.0f,
};

static void monty_mole_rock_act_move(void) {
    cur_obj_update_floor_and_walls();

    if (o->oMoveFlags & (OBJ_MOVE_MASK_ON_GROUND | OBJ_MOVE_ENTERED_WATER)) {
        cur_obj_spawn_particles(&sMontyMoleRockBreakParticles);
        obj_mark_for_deletion(o);
    }

    cur_obj_move_standard(78);
}

void bhv_monty_mole_rock_update(void) {

    obj_check_attacks(&sMontyMoleRockHitbox, o->oAction);

    switch (o->oAction) {
        case MONTY_MOLE_ROCK_ACT_HELD:
            monty_mole_rock_act_held();
            break;
        case MONTY_MOLE_ROCK_ACT_MOVE:
            monty_mole_rock_act_move();
            break;
    }
}
