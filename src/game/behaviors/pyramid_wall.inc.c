
void bhv_ssl_moving_pyramid_wall_init(void) {
    switch (o->oBhvParams2ndByte) {
        case PYRAMID_WALL_BP_POSITION_HIGH:
            break;

        case PYRAMID_WALL_BP_POSITION_MIDDLE:
            o->oPosY -= 256.0f;
            o->oTimer += 50;
            break;

        case PYRAMID_WALL_BP_POSITION_LOW:
            o->oPosY -= 512.0f;
            o->oAction = PYRAMID_WALL_ACT_MOVING_UP;
            break;
    }
}

void bhv_ssl_moving_pyramid_wall_loop(void) {
    switch (o->oAction) {
        case PYRAMID_WALL_ACT_MOVING_DOWN:
            o->oVelY = -5.12f;
            if (o->oTimer == 100) {
                o->oAction = PYRAMID_WALL_ACT_MOVING_UP;
            }
            break;

        case PYRAMID_WALL_ACT_MOVING_UP:
            o->oVelY = 5.12f;
            if (o->oTimer == 100) {
                o->oAction = PYRAMID_WALL_ACT_MOVING_DOWN;
            }
            break;
    }

    o->oPosY += o->oVelY;
}
