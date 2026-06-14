
f32 sBowserShockwaveHitPoints[] = { 1.9f, 2.4f, 4.0f, 4.8f };

void bhv_bowser_shock_wave_loop(void) {
    f32 distMin1, distMax1, distMin2, distMax2;
    s16 fadeFrames = 70;

    o->oBowserShockWaveScale = o->oTimer * 10;
    cur_obj_scale(o->oBowserShockWaveScale);

    if (gGlobalTimer % 3 != 0) {
        o->oOpacity--;
    }

    if (o->oTimer > fadeFrames) {
        o->oOpacity -= 5;
    }

    if (o->oOpacity <= 0) {
        obj_mark_for_deletion(o);
    }

    if (o->oTimer < fadeFrames && !mario_is_in_air_action()) {

        distMin1 = o->oBowserShockWaveScale * sBowserShockwaveHitPoints[0];
        distMax1 = o->oBowserShockWaveScale * sBowserShockwaveHitPoints[1];
        distMin2 = o->oBowserShockWaveScale * sBowserShockwaveHitPoints[2];
        distMax2 = o->oBowserShockWaveScale * sBowserShockwaveHitPoints[3];

        if ((distMin1 < o->oDistanceToMario && o->oDistanceToMario < distMax1)
            || (distMin2 < o->oDistanceToMario && o->oDistanceToMario < distMax2)) {
            gMarioObject->oInteractStatus |= INT_STATUS_MARIO_SHOCKWAVE;
        }
    }
}
