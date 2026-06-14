
void bhv_pole_init(void) {

    s32 tenthHitboxHeight = (o->oBhvParams >> 16) & 0xFF;
    o->hitboxHeight = tenthHitboxHeight * 10;
}

void bhv_giant_pole_loop(void) {
    if (o->oTimer == 0) {
        struct Object *topBall = spawn_object(o, MODEL_YELLOW_SPHERE, bhvYellowBall);
        topBall->oPosY += o->hitboxHeight + 50.0f;
    }
    bhv_pole_base_loop();
}
