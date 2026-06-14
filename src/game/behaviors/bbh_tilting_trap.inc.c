
void bhv_bbh_tilting_trap_platform_loop(void) {
    UNUSED u8 filler[4];

#ifndef VERSION_JP
    if (gMarioObject->platform == o) {
        o->oAction = BBH_TILTING_TRAP_PLATFORM_ACT_MARIO_ON;
    } else {
        o->oAction = BBH_TILTING_TRAP_PLATFORM_ACT_MARIO_OFF;
    }

    if (o->oAction == BBH_TILTING_TRAP_PLATFORM_ACT_MARIO_ON) {
#else
    if (gMarioObject->platform == o) {
#endif
        o->oAngleVelPitch = (s32)(o->oDistanceToMario * coss(o->oAngleToMario));
        o->oFaceAnglePitch += o->oAngleVelPitch;
    } else
#ifndef VERSION_JP

        if ((absi(o->oFaceAnglePitch) < 3000) || (o->oTimer > 15))
#endif
    {

        o->oAngleVelPitch = 0;

        if ((s16) o->oFaceAnglePitch > 0) {
            if (o->oFaceAnglePitch < 200) {
                o->oFaceAnglePitch = 0;
            } else {
                o->oAngleVelPitch = -200;
            }
        } else {
            if (o->oFaceAnglePitch > -200) {
                o->oFaceAnglePitch = 0;
            } else {
                o->oAngleVelPitch = 200;
            }
        }
    }

    o->oFaceAnglePitch += o->oAngleVelPitch;
}
