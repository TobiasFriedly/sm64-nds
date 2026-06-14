
static Collision const *sTTCRotatingSolidCollisionModels[] = {
    ttc_seg7_collision_07014F70,
    ttc_seg7_collision_07015008,
};

static u8 sTTCRotatingSolidInitialDelays[] = {
     120,
     40,
     0,
     0,
};

void bhv_ttc_rotating_solid_init(void) {
    o->collisionData = segmented_to_virtual(sTTCRotatingSolidCollisionModels[o->oBhvParams2ndByte]);

    o->oTTCRotatingSolidNumSides = o->oBhvParams2ndByte == TTC_ROTATING_SOLID_BP_CUBE ? 4 : 3;

    o->oTTCRotatingSolidRotationDelay = sTTCRotatingSolidInitialDelays[gTTCSpeedSetting];
}

void bhv_ttc_rotating_solid_update(void) {

    if (gTTCSpeedSetting != TTC_SPEED_STOPPED && o->oTimer > o->oTTCRotatingSolidRotationDelay) {
        if (o->oTTCRotatingSolidSoundTimer != 0) {

            if (--o->oTTCRotatingSolidSoundTimer == 0) {
                cur_obj_play_sound_2(SOUND_GENERAL2_ROTATING_BLOCK_ALERT);
            }
        } else if (o->oTTCRotatingSolidVelY > 0.0f && o->oPosY >= o->oHomeY) {

            s32 targetRoll =
                (s32)((f32) o->oTTCRotatingSolidNumTurns / o->oTTCRotatingSolidNumSides * 0x10000);
            s32 startRoll = o->oFaceAngleRoll;

            obj_face_roll_approach(targetRoll, 1200);

            o->oAngleVelRoll = o->oFaceAngleRoll - startRoll;
            if (o->oAngleVelRoll == 0) {
                cur_obj_play_sound_2(SOUND_GENERAL2_ROTATING_BLOCK_CLICK);

                o->oTTCRotatingSolidNumTurns =
                    (o->oTTCRotatingSolidNumTurns + 1) % o->oTTCRotatingSolidNumSides;

                o->oTimer = 0;
                if (gTTCSpeedSetting == TTC_SPEED_RANDOM) {
                    o->oTTCRotatingSolidRotationDelay = random_mod_offset(5, 20, 7);
                }
            }
        } else {

            o->oTTCRotatingSolidVelY += 0.5f;
            if ((o->oPosY += o->oTTCRotatingSolidVelY) >= o->oHomeY) {
                o->oPosY = o->oHomeY;
                o->oTTCRotatingSolidSoundTimer = 6;
            }
        }
    } else {
        o->oTTCRotatingSolidVelY = -5.0f;
    }
}
