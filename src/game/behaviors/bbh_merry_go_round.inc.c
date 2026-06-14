
static void handle_merry_go_round_music(void) {

    if (o->oMerryGoRoundMusicShouldPlay == FALSE) {
        if (gMarioCurrentRoom == BBH_NEAR_MERRY_GO_ROUND_ROOM) {

            play_secondary_music(SEQ_EVENT_MERRY_GO_ROUND, 45, 20, 200);

            o->oMerryGoRoundMusicShouldPlay++;
        }
    } else {

        struct Surface *marioFloor;
        u16 marioFloorType;

        find_floor(gMarioObject->oPosX, gMarioObject->oPosY, gMarioObject->oPosZ, &marioFloor);

        if (marioFloor == NULL) {
            marioFloorType = 0;
        } else {
            marioFloorType = marioFloor->type;
        }

        if (cur_obj_is_mario_on_platform() || marioFloorType == SURFACE_MGR_MUSIC) {

            play_secondary_music(SEQ_EVENT_MERRY_GO_ROUND, 0, 78, 50);
            gMarioOnMerryGoRound = TRUE;
        } else {

            play_secondary_music(SEQ_EVENT_MERRY_GO_ROUND, 45, 20, 200);
            gMarioOnMerryGoRound = FALSE;
        }

        if (

            gMarioCurrentRoom != BBH_DYNAMIC_SURFACE_ROOM
            && gMarioCurrentRoom != BBH_NEAR_MERRY_GO_ROUND_ROOM) {
            func_80321080(300);
            o->oMerryGoRoundMusicShouldPlay = FALSE;
        } else {
            cur_obj_play_sound_1(SOUND_ENV_MERRY_GO_ROUND_CREAKING);
        }
    }
}

void bhv_merry_go_round_loop(void) {

    if (!o->oMerryGoRoundMarioIsOutside) {
        if (gMarioCurrentRoom == BBH_OUTSIDE_ROOM) {

            o->oMerryGoRoundMarioIsOutside++;
        }
    } else {
        play_sound(SOUND_AIR_HOWLING_WIND, gGlobalSoundSource);

        if (

            gMarioCurrentRoom != BBH_OUTSIDE_ROOM && gMarioCurrentRoom != BBH_DYNAMIC_SURFACE_ROOM) {
            o->oMerryGoRoundMarioIsOutside = FALSE;
        }
    }

    if (!o->oMerryGoRoundStopped) {
        o->oAngleVelYaw = 0x80;
        o->oMoveAngleYaw += o->oAngleVelYaw;
        o->oFaceAngleYaw += o->oAngleVelYaw;
        handle_merry_go_round_music();
    } else {
        o->oAngleVelYaw = 0;
        func_80321080(300);
    }
}
