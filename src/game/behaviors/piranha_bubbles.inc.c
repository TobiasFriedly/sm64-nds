
void bhv_piranha_plant_waking_bubbles_loop(void) {
    if (o->oTimer == 0) {
        o->oVelY = random_float() * 10.0f + 5.0f;
        o->oForwardVel = random_float() * 10.0f + 5.0f;
        o->oMoveAngleYaw = random_u16();
    }
    cur_obj_move_using_fvel_and_gravity();
}

void bhv_piranha_plant_bubble_loop(void) {
    struct Object *parent = o->parentObj;
    f32 scale = 0;
    s32 i;
    s32 animFrame = parent->header.gfx.animInfo.animFrame;

    s32 lastFrame = parent->header.gfx.animInfo.curAnim->loopEnd - 2;
    UNUSED u8 filler[4];

    cur_obj_set_pos_relative(parent, 0, 72.0f, 180.0f);

    switch (o->oAction) {
        case PIRANHA_PLANT_BUBBLE_ACT_IDLE:
            cur_obj_disable_rendering();
            scale = 0;

            if (parent->oAction == PIRANHA_PLANT_ACT_SLEEPING) {
                o->oAction++;
            }
            break;

        case PIRANHA_PLANT_BUBBLE_ACT_GROW_SHRINK_LOOP:
            if (parent->oDistanceToMario < parent->oDrawingDistance) {
                cur_obj_enable_rendering();

                if (parent->oAction == PIRANHA_PLANT_ACT_SLEEPING) {

                    f32 doneShrinkingFrame = lastFrame / 2.0f - 4.0f;

                    f32 beginGrowingFrame = lastFrame / 2.0f + 4.0f;

                    if (animFrame < doneShrinkingFrame) {

                        scale = coss(animFrame / doneShrinkingFrame * 0x4000) * 4.0f + 1.0;
                    } else if (animFrame > beginGrowingFrame) {

                        scale = sins((

                                    (animFrame - (lastFrame / 2.0f + 4.0f)) / beginGrowingFrame
                                    ) * 0x4000) * 4.0f + 1.0;
                    } else {

                        scale = 1.0f;
                    }
                } else {

                    o->oAction++;
                }
            } else {
                cur_obj_disable_rendering();
            }
            break;

        case PIRANHA_PLANT_BUBBLE_ACT_BURST:
            cur_obj_disable_rendering();
            scale = 0.0f;

            for (i = 0; i < 15; i++) {
                try_to_spawn_object(0, 1.0f, o, MODEL_BUBBLE, bhvPiranhaPlantWakingBubbles);
            }

            o->oAction = PIRANHA_PLANT_BUBBLE_ACT_IDLE;
            scale = 1.0f;
            break;
    }

    cur_obj_scale(scale);
}
