
static struct ObjectHitbox sAmpHitbox = {
     INTERACT_SHOCK,
     40,
     1,
     0,
     0,
     40,
     50,
     50,
     60,
};

void bhv_homing_amp_init(void) {
    o->oHomeX = o->oPosX;
    o->oHomeY = o->oPosY;
    o->oHomeZ = o->oPosZ;
    o->oGravity = 0.0f;
    o->oFriction = 1.0f;
    o->oBuoyancy = 1.0f;
    o->oHomingAmpAvgY = o->oHomeY;

    cur_obj_scale(0.1f);

    o->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
}

static void check_amp_attack(void) {

    obj_set_hitbox(o, &sAmpHitbox);

    if (o->oInteractStatus & INT_STATUS_INTERACTED) {

        if (o->oInteractStatus & INT_STATUS_INTERACTED) {

            o->oAction = AMP_ACT_ATTACK_COOLDOWN;
        }

        o->oInteractStatus = 0;
    }
}

static void homing_amp_appear_loop(void) {

    f32 relativeTargetX = gLakituState.goalPos[0] - o->oPosX;
    f32 relativeTargetZ = gLakituState.goalPos[2] - o->oPosZ;
    s16 targetYaw = atan2s(relativeTargetZ, relativeTargetX);

    o->oMoveAngleYaw = approach_s16_symmetric(o->oMoveAngleYaw, targetYaw, 0x1000);

    if (o->oTimer < 30) {
        cur_obj_scale(0.1 + 0.9 * (f32)(o->oTimer / 30.0f));
    } else {
        o->oAnimState = 1;
    }

    if (o->oTimer > 90) {
        cur_obj_scale(1.0f);
        o->oAction = HOMING_AMP_ACT_CHASE;
        o->oAmpYPhase = 0;
    }
}

static void homing_amp_chase_loop(void) {

    if ((o->oAngleToMario - 0x400 < o->oMoveAngleYaw)
        && (o->oMoveAngleYaw < o->oAngleToMario + 0x400)) {
        o->oHomingAmpLockedOn = TRUE;
        o->oTimer = 0;
    }

    if (o->oHomingAmpLockedOn == TRUE) {
        o->oForwardVel = 15.0f;

        if (o->oHomingAmpAvgY > gMarioObject->header.gfx.pos[1] + 150.0f) {
            o->oHomingAmpAvgY -= 10.0f;
        } else {
            o->oHomingAmpAvgY = gMarioObject->header.gfx.pos[1] + 150.0f;
        }

        if (o->oTimer > 30) {
            o->oHomingAmpLockedOn = FALSE;
        }
    } else {

        o->oForwardVel = 10.0f;

        obj_turn_toward_object(o, gMarioObject, 16, 0x400);

        if (o->oHomingAmpAvgY < gMarioObject->header.gfx.pos[1] + 250.0f) {
            o->oHomingAmpAvgY += 10.0f;
        }
    }

    o->oPosY = o->oHomingAmpAvgY + sins(o->oAmpYPhase * 0x400) * 20.0f;

    check_amp_attack();

    if (!is_point_within_radius_of_mario(o->oHomeX, o->oHomeY, o->oHomeZ, 1500)) {
        o->oAction = HOMING_AMP_ACT_GIVE_UP;
    }
}

static void homing_amp_give_up_loop(void) {
    UNUSED u8 filler[8];

    o->oForwardVel = 15.0f;

    if (o->oTimer > 150) {

        o->oPosX = o->oHomeX;
        o->oPosY = o->oHomeY;
        o->oPosZ = o->oHomeZ;
        o->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
        o->oAction = HOMING_AMP_ACT_INACTIVE;
        o->oAnimState = 0;
        o->oForwardVel = 0.0f;
        o->oHomingAmpAvgY = o->oHomeY;
    }
}

static void amp_attack_cooldown_loop(void) {

    o->header.gfx.animInfo.animFrame += 2;
    o->oForwardVel = 0.0f;

    cur_obj_become_intangible();

    if (o->oTimer > 30) {
        o->oAnimState = 0;
    }

    if (o->oTimer > 90) {
        o->oAnimState = 1;
        cur_obj_become_tangible();
        o->oAction = HOMING_AMP_ACT_CHASE;
    }
}

void bhv_homing_amp_loop(void) {
    switch (o->oAction) {
        case HOMING_AMP_ACT_INACTIVE:
            if (is_point_within_radius_of_mario(o->oHomeX, o->oHomeY, o->oHomeZ, 800) == TRUE) {

                o->oAction = HOMING_AMP_ACT_APPEAR;
                o->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
            }
            break;

        case HOMING_AMP_ACT_APPEAR:
            homing_amp_appear_loop();
            break;

        case HOMING_AMP_ACT_CHASE:
            homing_amp_chase_loop();
            cur_obj_play_sound_1(SOUND_AIR_AMP_BUZZ);
            break;

        case HOMING_AMP_ACT_GIVE_UP:
            homing_amp_give_up_loop();
            break;

        case HOMING_AMP_ACT_ATTACK_COOLDOWN:
            amp_attack_cooldown_loop();
            break;
    }

    object_step();

    o->oAmpYPhase++;
}

void bhv_circling_amp_init(void) {
    o->oHomeX = o->oPosX;
    o->oHomeY = o->oPosY;
    o->oHomeZ = o->oPosZ;
    o->oAnimState = 1;

    switch (o->oBhvParams2ndByte) {
        case AMP_BP_ROT_RADIUS_200:
            o->oAmpRadiusOfRotation = 200.0f;
            break;

        case AMP_BP_ROT_RADIUS_300:
            o->oAmpRadiusOfRotation = 300.0f;
            break;

        case AMP_BP_ROT_RADIUS_400:
            o->oAmpRadiusOfRotation = 400.0f;
            break;

        case AMP_BP_ROT_RADIUS_0:
            break;
    }

    o->oMoveAngleYaw = random_u16();

    o->oAction = AMP_ACT_IDLE;
}

static void fixed_circling_amp_idle_loop(void) {

    f32 xToMario = gMarioObject->header.gfx.pos[0] - o->oPosX;
    f32 yToMario = gMarioObject->header.gfx.pos[1] + 120.0f - o->oPosY;
    f32 zToMario = gMarioObject->header.gfx.pos[2] - o->oPosZ;
    s16 vAngleToMario = atan2s(sqrtf(xToMario * xToMario + zToMario * zToMario), -yToMario);

    obj_turn_toward_object(o, gMarioObject, 19, 0x1000);
    o->oFaceAnglePitch = approach_s16_symmetric(o->oFaceAnglePitch, vAngleToMario, 0x1000);

    o->oPosY = o->oHomeY + coss(o->oAmpYPhase * 0x458) * 20.0f;

    check_amp_attack();

    o->oAmpYPhase++;

}

static void circling_amp_idle_loop(void) {

    o->oPosX = o->oHomeX + sins(o->oMoveAngleYaw) * o->oAmpRadiusOfRotation;
    o->oPosZ = o->oHomeZ + coss(o->oMoveAngleYaw) * o->oAmpRadiusOfRotation;
    o->oPosY = o->oHomeY + coss(o->oAmpYPhase * 0x8B0) * 30.0f;
    o->oMoveAngleYaw += 0x400;
    o->oFaceAngleYaw = o->oMoveAngleYaw + 0x4000;

    check_amp_attack();

    o->oAmpYPhase++;

    cur_obj_play_sound_1(SOUND_AIR_AMP_BUZZ);
}

void bhv_circling_amp_loop(void) {
    switch (o->oAction) {
        case AMP_ACT_IDLE:
            if (o->oBhvParams2ndByte == AMP_BP_ROT_RADIUS_0) {
                fixed_circling_amp_idle_loop();
            } else {
                circling_amp_idle_loop();
            }
            break;

        case AMP_ACT_ATTACK_COOLDOWN:
            amp_attack_cooldown_loop();
            break;
    }
}
