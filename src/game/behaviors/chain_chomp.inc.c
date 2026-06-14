
static struct ObjectHitbox sChainChompHitbox = {
     INTERACT_MR_BLIZZARD,
     0,
     3,
     1,
     0,
     80,
     160,
     80,
     160,
};

void bhv_chain_chomp_chain_part_update(void) {
    if (o->parentObj->oAction == CHAIN_CHOMP_ACT_UNLOAD_CHAIN) {
        obj_mark_for_deletion(o);
    } else if (o->oBhvParams2ndByte != CHAIN_CHOMP_CHAIN_PART_BP_PIVOT) {
        struct ChainSegment *segment = &o->parentObj->oChainChompSegments[o->oBhvParams2ndByte];

        o->oPosX = o->parentObj->parentObj->oPosX + segment->posX;
        o->oPosY = o->parentObj->parentObj->oPosY + segment->posY;
        o->oPosZ = o->parentObj->parentObj->oPosZ + segment->posZ;
    } else if (o->parentObj->oChainChompReleaseStatus != CHAIN_CHOMP_NOT_RELEASED) {
        cur_obj_update_floor_and_walls();
        cur_obj_move_standard(78);
    }
}

static void chain_chomp_act_uninitialized(void) {
    struct ChainSegment *segments;
    s32 i;

    if (o->oDistanceToMario < 3000.0f) {
        segments = mem_pool_alloc(gObjectMemoryPool, 5 * sizeof(struct ChainSegment));
        if (segments != NULL) {

            o->oChainChompSegments = segments;
            for (i = 0; i <= 4; i++) {
                chain_segment_init(&segments[i]);
            }

            cur_obj_set_pos_to_home();

            if ((o->parentObj =
                     spawn_object(o, CHAIN_CHOMP_CHAIN_PART_BP_PIVOT, bhvChainChompChainPart))
                != NULL) {

                for (i = 1; i <= 4; i++) {
                    spawn_object_relative(i, 0, 0, 0, o, MODEL_METALLIC_BALL, bhvChainChompChainPart);
                }

                o->oAction = CHAIN_CHOMP_ACT_MOVE;
                cur_obj_unhide();
            }
        }
    }
}

static void chain_chomp_update_chain_segments(void) {
    struct ChainSegment *prevSegment;
    struct ChainSegment *segment;
    f32 offsetX;
    f32 offsetY;
    f32 offsetZ;
    f32 offset;
    f32 segmentVelY;
    f32 maxTotalOffset;
    s32 i;

    if (o->oVelY < 0.0f) {
        segmentVelY = o->oVelY;
    } else {
        segmentVelY = -20.0f;
    }

    for (i = 1; i <= 4; i++) {
        prevSegment = &o->oChainChompSegments[i - 1];
        segment = &o->oChainChompSegments[i];

        if ((segment->posY += segmentVelY) < 0.0f) {
            segment->posY = 0.0f;
        }

        offsetX = segment->posX - prevSegment->posX;
        offsetY = segment->posY - prevSegment->posY;
        offsetZ = segment->posZ - prevSegment->posZ;
        offset = sqrtf(offsetX * offsetX + offsetY * offsetY + offsetZ * offsetZ);

        if (offset > o->oChainChompMaxDistBetweenChainParts) {
            offset = o->oChainChompMaxDistBetweenChainParts / offset;
            offsetX *= offset;
            offsetY *= offset;
            offsetZ *= offset;
        }

        offsetX += prevSegment->posX;
        offsetY += prevSegment->posY;
        offsetZ += prevSegment->posZ;
        offset = sqrtf(offsetX * offsetX + offsetY * offsetY + offsetZ * offsetZ);

        maxTotalOffset = o->oChainChompMaxDistFromPivotPerChainPart * (5 - i);
        if (offset > maxTotalOffset) {
            offset = maxTotalOffset / offset;
            offsetX *= offset;
            offsetY *= offset;
            offsetZ *= offset;
        }

        segment->posX = offsetX;
        segment->posY = offsetY;
        segment->posZ = offsetZ;
    }
}

static void chain_chomp_restore_normal_chain_lengths(void) {
    approach_f32_ptr(&o->oChainChompMaxDistFromPivotPerChainPart, 750.0f / 5, 4.0f);
    o->oChainChompMaxDistBetweenChainParts = o->oChainChompMaxDistFromPivotPerChainPart;
}

static void chain_chomp_sub_act_turn(void) {
    o->oGravity = -4.0f;

    chain_chomp_restore_normal_chain_lengths();
    obj_move_pitch_approach(0, 0x100);

    if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {
        cur_obj_rotate_yaw_toward(o->oAngleToMario, 0x400);
        if (abs_angle_diff(o->oAngleToMario, o->oMoveAngleYaw) < 0x800) {
            if (o->oTimer > 30) {
                if (cur_obj_check_anim_frame(0)) {
                    cur_obj_reverse_animation();
                    if (o->oTimer > 40) {

                        cur_obj_play_sound_2(SOUND_GENERAL_CHAIN_CHOMP2);

                        o->oSubAction = CHAIN_CHOMP_SUB_ACT_LUNGE;
                        o->oChainChompMaxDistFromPivotPerChainPart = 900.0f / 5;

                        o->oForwardVel = 140.0f;
                        o->oVelY = 20.0f;
                        o->oGravity = 0.0f;
                        o->oChainChompTargetPitch = obj_get_pitch_from_vel();
                    }
                } else {
                    o->oTimer--;
                }
            } else {
                o->oForwardVel = 0.0f;
            }
        } else {
            cur_obj_play_sound_2(SOUND_GENERAL_CHAIN_CHOMP1);
            o->oForwardVel = 10.0f;
            o->oVelY = 20.0f;
        }
    } else {
        cur_obj_rotate_yaw_toward(o->oAngleToMario, 400);
        o->oTimer = 0;
    }
}

static void chain_chomp_sub_act_lunge(void) {
    obj_face_pitch_approach(o->oChainChompTargetPitch, 0x400);

    if (o->oForwardVel != 0.0f) {
        f32 val04;

        if (o->oChainChompRestrictedByChain == TRUE) {
            o->oForwardVel = o->oVelY = 0.0f;
            o->oChainChompUnk104 = 30.0f;
        }

        if ((val04 = 900.0f - o->oChainChompDistToPivot) > 220.0f) {
            val04 = 220.0f;
        }

        o->oChainChompMaxDistBetweenChainParts =
            val04 / 220.0f * o->oChainChompMaxDistFromPivotPerChainPart;
        o->oTimer = 0;
    } else {

        cur_obj_rotate_yaw_toward(atan2s(o->oChainChompSegments[0].posZ, o->oChainChompSegments[0].posX),
                                  0x1000);

        if (o->oChainChompUnk104 != 0.0f) {
            approach_f32_ptr(&o->oChainChompUnk104, 0.0f, 0.8f);
        } else {
            o->oSubAction = CHAIN_CHOMP_SUB_ACT_TURN;
        }

        o->oChainChompMaxDistBetweenChainParts = o->oChainChompUnk104;
        if (gGlobalTimer % 2 != 0) {
            o->oChainChompMaxDistBetweenChainParts = -o->oChainChompUnk104;
        }
    }

    if (o->oTimer < 30) {
        cur_obj_reverse_animation();
    }
}

static void chain_chomp_released_trigger_cutscene(void) {
    o->oForwardVel = 0.0f;
    o->oGravity = -4.0f;

    if (set_mario_npc_dialog(MARIO_DIALOG_LOOK_UP) == MARIO_DIALOG_STATUS_SPEAK
        && (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) && cutscene_object(CUTSCENE_STAR_SPAWN, o) == 1) {
        o->oChainChompReleaseStatus = CHAIN_CHOMP_RELEASED_LUNGE_AROUND;
        o->oTimer = 0;
    }
}

static void chain_chomp_released_lunge_around(void) {
    chain_chomp_restore_normal_chain_lengths();

    if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {

        if (o->oChainChompNumLunges == 0) {
            if (cur_obj_rotate_yaw_toward(o->oAngleToMario, 800)) {
                if (o->oTimer > 60) {
                    o->oChainChompNumLunges++;

                    o->oWallHitboxRadius = 200.0f;
                }
            } else {
                o->oTimer = 0;
            }
        } else {
            if (++o->oChainChompNumLunges <= 5) {
                cur_obj_play_sound_2(SOUND_GENERAL_CHAIN_CHOMP1);
                o->oMoveAngleYaw = o->oAngleToMario + random_sign() * 0x2000;
                o->oForwardVel = 30.0f;
                o->oVelY = 50.0f;
            } else {
                o->oChainChompReleaseStatus = CHAIN_CHOMP_RELEASED_BREAK_GATE;
                o->oHomeX = 1450.0f;
                o->oHomeZ = 562.0f;
                o->oMoveAngleYaw = cur_obj_angle_to_home();
                o->oForwardVel = cur_obj_lateral_dist_to_home() / 8;
                o->oVelY = 50.0f;
            }
        }
    }
}

static void chain_chomp_released_break_gate(void) {
    if (!o->oChainChompHitGate) {

        if (o->oMoveFlags & OBJ_MOVE_HIT_WALL) {
            o->oChainChompHitGate = TRUE;
            o->oMoveAngleYaw = cur_obj_reflect_move_angle_off_wall();
            o->oForwardVel *= 0.4f;
        }
    } else if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {
        o->oChainChompReleaseStatus = CHAIN_CHOMP_RELEASED_JUMP_AWAY;
        o->oHomeX = 3288.0f;
        o->oHomeZ = -1770.0f;
        o->oMoveAngleYaw = cur_obj_angle_to_home();
        o->oForwardVel = cur_obj_lateral_dist_to_home() / 50.0f;
        o->oVelY = 120.0f;
    }
}

static void chain_chomp_released_jump_away(void) {
    if (o->oMoveFlags & OBJ_MOVE_MASK_ON_GROUND) {
        gObjCutsceneDone = TRUE;
        o->oChainChompReleaseStatus = CHAIN_CHOMP_RELEASED_END_CUTSCENE;
    }
}

static void chain_chomp_released_end_cutscene(void) {
    if (cutscene_object(CUTSCENE_STAR_SPAWN, o) == -1) {
        set_mario_npc_dialog(MARIO_DIALOG_STOP);
        o->oAction = CHAIN_CHOMP_ACT_UNLOAD_CHAIN;
    }
}

static void chain_chomp_act_move(void) {
    f32 maxDistToPivot;

    if (o->oChainChompReleaseStatus == CHAIN_CHOMP_NOT_RELEASED && o->oDistanceToMario > 4000.0f) {
        o->oAction = CHAIN_CHOMP_ACT_UNLOAD_CHAIN;
        o->oForwardVel = o->oVelY = 0.0f;
    } else {
        cur_obj_update_floor_and_walls();

        switch (o->oChainChompReleaseStatus) {
            case CHAIN_CHOMP_NOT_RELEASED:
                switch (o->oSubAction) {
                    case CHAIN_CHOMP_SUB_ACT_TURN:
                        chain_chomp_sub_act_turn();
                        break;
                    case CHAIN_CHOMP_SUB_ACT_LUNGE:
                        chain_chomp_sub_act_lunge();
                        break;
                }
                break;
            case CHAIN_CHOMP_RELEASED_TRIGGER_CUTSCENE:
                chain_chomp_released_trigger_cutscene();
                break;
            case CHAIN_CHOMP_RELEASED_LUNGE_AROUND:
                chain_chomp_released_lunge_around();
                break;
            case CHAIN_CHOMP_RELEASED_BREAK_GATE:
                chain_chomp_released_break_gate();
                break;
            case CHAIN_CHOMP_RELEASED_JUMP_AWAY:
                chain_chomp_released_jump_away();
                break;
            case CHAIN_CHOMP_RELEASED_END_CUTSCENE:
                chain_chomp_released_end_cutscene();
                break;
        }

        cur_obj_move_standard(78);

        o->oChainChompSegments[0].posX = o->oPosX - o->parentObj->oPosX;
        o->oChainChompSegments[0].posY = o->oPosY - o->parentObj->oPosY;
        o->oChainChompSegments[0].posZ = o->oPosZ - o->parentObj->oPosZ;

        o->oChainChompDistToPivot =
            sqrtf(o->oChainChompSegments[0].posX * o->oChainChompSegments[0].posX
                  + o->oChainChompSegments[0].posY * o->oChainChompSegments[0].posY
                  + o->oChainChompSegments[0].posZ * o->oChainChompSegments[0].posZ);

        maxDistToPivot = o->oChainChompMaxDistFromPivotPerChainPart * 5;
        if (o->oChainChompDistToPivot > maxDistToPivot) {
            f32 ratio = maxDistToPivot / o->oChainChompDistToPivot;
            o->oChainChompDistToPivot = maxDistToPivot;

            o->oChainChompSegments[0].posX *= ratio;
            o->oChainChompSegments[0].posY *= ratio;
            o->oChainChompSegments[0].posZ *= ratio;

            if (o->oChainChompReleaseStatus == CHAIN_CHOMP_NOT_RELEASED) {

                o->oPosX = o->parentObj->oPosX + o->oChainChompSegments[0].posX;
                o->oPosY = o->parentObj->oPosY + o->oChainChompSegments[0].posY;
                o->oPosZ = o->parentObj->oPosZ + o->oChainChompSegments[0].posZ;

                o->oChainChompRestrictedByChain = TRUE;
            } else {

                f32 oldPivotY = o->parentObj->oPosY;

                o->parentObj->oPosX = o->oPosX - o->oChainChompSegments[0].posX;
                o->parentObj->oPosY = o->oPosY - o->oChainChompSegments[0].posY;
                o->parentObj->oVelY = o->parentObj->oPosY - oldPivotY;
                o->parentObj->oPosZ = o->oPosZ - o->oChainChompSegments[0].posZ;
            }
        } else {
            o->oChainChompRestrictedByChain = FALSE;
        }

        chain_chomp_update_chain_segments();

        if (obj_check_attacks(&sChainChompHitbox, o->oAction) != 0) {
            o->oSubAction = CHAIN_CHOMP_SUB_ACT_LUNGE;
            o->oChainChompMaxDistFromPivotPerChainPart = 900.0f / 5;
            o->oForwardVel = 0.0f;
            o->oVelY = 300.0f;
            o->oGravity = -4.0f;
            o->oChainChompTargetPitch = -0x3000;
        }
    }
}

static void chain_chomp_act_unload_chain(void) {
    cur_obj_hide();
    mem_pool_free(gObjectMemoryPool, o->oChainChompSegments);

    o->oAction = CHAIN_CHOMP_ACT_UNINITIALIZED;

    if (o->oChainChompReleaseStatus != CHAIN_CHOMP_NOT_RELEASED) {
        obj_mark_for_deletion(o);
    }
}

void bhv_chain_chomp_update(void) {
    switch (o->oAction) {
        case CHAIN_CHOMP_ACT_UNINITIALIZED:
            chain_chomp_act_uninitialized();
            break;
        case CHAIN_CHOMP_ACT_MOVE:
            chain_chomp_act_move();
            break;
        case CHAIN_CHOMP_ACT_UNLOAD_CHAIN:
            chain_chomp_act_unload_chain();
            break;
    }
}

void bhv_wooden_post_update(void) {

    if (!o->oWoodenPostMarioPounding) {
        if ((o->oWoodenPostMarioPounding = cur_obj_is_mario_ground_pounding_platform())) {
            cur_obj_play_sound_2(SOUND_GENERAL_POUND_WOOD_POST);
            o->oWoodenPostSpeedY = -70.0f;
        }
    } else if (approach_f32_ptr(&o->oWoodenPostSpeedY, 0.0f, 25.0f)) {

        o->oWoodenPostMarioPounding = cur_obj_is_mario_ground_pounding_platform();
    } else if ((o->oWoodenPostOffsetY += o->oWoodenPostSpeedY) < -190.0f) {

        o->oWoodenPostOffsetY = -190.0f;
        if (o->parentObj != o) {
            play_puzzle_jingle();
            o->parentObj->oChainChompReleaseStatus = CHAIN_CHOMP_RELEASED_TRIGGER_CUTSCENE;
            o->parentObj = o;
        }
    }

    if (o->oWoodenPostOffsetY != 0.0f) {
        o->oPosY = o->oHomeY + o->oWoodenPostOffsetY;
    } else if (!(o->oBhvParams & WOODEN_POST_BP_NO_COINS_MASK)) {

        if (o->oDistanceToMario > 400.0f) {
            o->oTimer = o->oWoodenPostTotalMarioAngle = 0;
        } else {

            o->oWoodenPostTotalMarioAngle += (s16)(o->oAngleToMario - o->oWoodenPostPrevAngleToMario);
            if (absi(o->oWoodenPostTotalMarioAngle) > 0x30000 && o->oTimer < 200) {
                obj_spawn_loot_yellow_coins(o, 5, 20.0f);
                set_object_respawn_info_bits(o, 1);
            }
        }

        o->oWoodenPostPrevAngleToMario = o->oAngleToMario;
    }
}

void bhv_chain_chomp_gate_init(void) {
    o->parentObj = cur_obj_nearest_object_with_behavior(bhvChainChomp);
}

void bhv_chain_chomp_gate_update(void) {
    if (o->parentObj->oChainChompHitGate) {
        spawn_mist_particles_with_sound(SOUND_GENERAL_WALL_EXPLOSION);
        set_camera_shake_from_point(SHAKE_POS_SMALL, o->oPosX, o->oPosY, o->oPosZ);
        spawn_mist_particles_variable(0, 127, 200.0f);
        spawn_triangle_break_particles(30, MODEL_DIRT_ANIMATION, 3.0f, 4);
        obj_mark_for_deletion(o);
    }
}
