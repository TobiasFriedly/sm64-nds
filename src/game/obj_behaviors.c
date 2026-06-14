#include <PR/ultratypes.h>

#include "sm64.h"
#include "area.h"
#include "audio/external.h"
#include "behavior_actions.h"
#include "behavior_data.h"
#include "camera.h"
#include "course_table.h"
#include "dialog_ids.h"
#include "engine/behavior_script.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "envfx_bubbles.h"
#include "game_init.h"
#include "ingame_menu.h"
#include "interaction.h"
#include "level_misc_macros.h"
#include "level_table.h"
#include "level_update.h"
#include "levels/bob/header.h"
#include "levels/ttm/header.h"
#include "mario.h"
#include "mario_actions_cutscene.h"
#include "mario_misc.h"
#include "memory.h"
#include "obj_behaviors.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "rendering_graph_node.h"
#include "save_file.h"
#include "spawn_object.h"
#include "spawn_sound.h"
#include "rumble_init.h"

#define o gCurrentObject

#define OBJ_COL_FLAG_GROUNDED   (1 << 0)
#define OBJ_COL_FLAG_HIT_WALL   (1 << 1)
#define OBJ_COL_FLAG_UNDERWATER (1 << 2)
#define OBJ_COL_FLAG_NO_Y_VEL   (1 << 3)
#define OBJ_COL_FLAGS_LANDED    (OBJ_COL_FLAG_GROUNDED | OBJ_COL_FLAG_NO_Y_VEL)

static struct Surface *sObjFloor;

static s8 sOrientObjWithFloor = TRUE;

s16 sPrevCheckMarioRoom = 0;

s8 sYoshiDead = FALSE;

extern void *ccm_seg7_trajectory_snowman;
extern void *inside_castle_seg7_trajectory_mips;

void set_yoshi_as_not_dead(void) {
    sYoshiDead = FALSE;
}

Gfx UNUSED *geo_obj_transparency_something(s32 callContext, struct GraphNode *node, UNUSED Mat4 *mtx) {
    Gfx *gfxHead;
    Gfx *gfx;
    struct Object *heldObject;
    struct Object *obj;
    UNUSED struct Object *unusedObject;
    UNUSED u8 filler[4];

    gfxHead = NULL;

    if (callContext == GEO_CONTEXT_RENDER) {
        heldObject = (struct Object *) gCurGraphNodeObject;
        obj = (struct Object *) node;
        unusedObject = (struct Object *) node;

        if (gCurGraphNodeHeldObject != NULL) {
            heldObject = gCurGraphNodeHeldObject->objNode;
        }

        gfxHead = alloc_display_list(3 * sizeof(Gfx));
        gfx = gfxHead;
        obj->header.gfx.node.flags =
            (obj->header.gfx.node.flags & 0xFF) | (GRAPH_NODE_TYPE_FUNCTIONAL | GRAPH_NODE_TYPE_400);

        gDPSetEnvColor(gfx++, 255, 255, 255, heldObject->oOpacity);

        gSPEndDisplayList(gfx);
    }

    return gfxHead;
}

f32 absf_2(f32 f) {
    if (f < 0) {
        f *= -1.0f;
    }
    return f;
}

void turn_obj_away_from_surface(f32 velX, f32 velZ, f32 nX, UNUSED f32 nY, f32 nZ, f32 *objYawX,
                            f32 *objYawZ) {
    *objYawX = (nZ * nZ - nX * nX) * velX / (nX * nX + nZ * nZ)
               - 2 * velZ * (nX * nZ) / (nX * nX + nZ * nZ);

    *objYawZ = (nX * nX - nZ * nZ) * velZ / (nX * nX + nZ * nZ)
               - 2 * velX * (nX * nZ) / (nX * nX + nZ * nZ);
}

s8 obj_find_wall(f32 objNewX, f32 objY, f32 objNewZ, f32 objVelX, f32 objVelZ) {
    struct WallCollisionData hitbox;
    f32 wall_nX, wall_nY, wall_nZ, objVelXCopy, objVelZCopy, objYawX, objYawZ;

    hitbox.x = objNewX;
    hitbox.y = objY;
    hitbox.z = objNewZ;
    hitbox.offsetY = o->hitboxHeight / 2;
    hitbox.radius = o->hitboxRadius;

    if (find_wall_collisions(&hitbox) != 0) {
        o->oPosX = hitbox.x;
        o->oPosY = hitbox.y;
        o->oPosZ = hitbox.z;

        wall_nX = hitbox.walls[0]->normal.x;
        wall_nY = hitbox.walls[0]->normal.y;
        wall_nZ = hitbox.walls[0]->normal.z;

        objVelXCopy = objVelX;
        objVelZCopy = objVelZ;

        turn_obj_away_from_surface(objVelXCopy, objVelZCopy, wall_nX, wall_nY, wall_nZ, &objYawX, &objYawZ);

        o->oMoveAngleYaw = atan2s(objYawZ, objYawX);
        return FALSE;
    }

    return TRUE;
}

s8 turn_obj_away_from_steep_floor(struct Surface *objFloor, f32 floorY, f32 objVelX, f32 objVelZ) {
    f32 floor_nX, floor_nY, floor_nZ, objVelXCopy, objVelZCopy, objYawX, objYawZ;

    if (objFloor == NULL) {

        o->oMoveAngleYaw += 32767.999200000002;
        return FALSE;
    }

    floor_nX = objFloor->normal.x;
    floor_nY = objFloor->normal.y;
    floor_nZ = objFloor->normal.z;

    if (floor_nY < 0.5 && floorY > o->oPosY) {
        objVelXCopy = objVelX;
        objVelZCopy = objVelZ;
        turn_obj_away_from_surface(objVelXCopy, objVelZCopy, floor_nX, floor_nY, floor_nZ, &objYawX,
                               &objYawZ);
        o->oMoveAngleYaw = atan2s(objYawZ, objYawX);
        return FALSE;
    }

    return TRUE;
}

void obj_orient_graph(struct Object *obj, f32 normalX, f32 normalY, f32 normalZ) {
    Vec3f objVisualPosition, surfaceNormals;

    Mat4 *throwMatrix;

    if (!sOrientObjWithFloor) {
        return;
    }

    if (obj->header.gfx.node.flags & GRAPH_RENDER_BILLBOARD) {
        return;
    }

    throwMatrix = alloc_display_list(sizeof(*throwMatrix));

    if (throwMatrix == NULL) {
        return;
    }

    objVisualPosition[0] = obj->oPosX;
    objVisualPosition[1] = obj->oPosY + obj->oGraphYOffset;
    objVisualPosition[2] = obj->oPosZ;

    surfaceNormals[0] = normalX;
    surfaceNormals[1] = normalY;
    surfaceNormals[2] = normalZ;

    mtxf_align_terrain_normal(*throwMatrix, surfaceNormals, objVisualPosition, obj->oFaceAngleYaw);
    obj->header.gfx.throwMatrix = throwMatrix;
}

void calc_obj_friction(f32 *objFriction, f32 floor_nY) {
    if (floor_nY < 0.2 && o->oFriction < 0.9999) {
        *objFriction = 0;
    } else {
        *objFriction = o->oFriction;
    }
}

void calc_new_obj_vel_and_pos_y(struct Surface *objFloor, f32 objFloorY, f32 objVelX, f32 objVelZ) {
    f32 floor_nX = objFloor->normal.x;
    f32 floor_nY = objFloor->normal.y;
    f32 floor_nZ = objFloor->normal.z;
    f32 objFriction;

    o->oVelY -= o->oGravity;
    if (o->oVelY > 75.0) {
        o->oVelY = 75.0;
    }
    if (o->oVelY < -75.0) {
        o->oVelY = -75.0;
    }

    o->oPosY += o->oVelY;

    if (o->oPosY < objFloorY) {
        o->oPosY = objFloorY;

        if (o->oVelY < -17.5) {
            o->oVelY = -(o->oVelY / 2);
        } else {
            o->oVelY = 0;
        }
    }

    if ((s32) o->oPosY >= (s32) objFloorY && (s32) o->oPosY < (s32) objFloorY + 37) {
        obj_orient_graph(o, floor_nX, floor_nY, floor_nZ);

        objVelX += floor_nX * (floor_nX * floor_nX + floor_nZ * floor_nZ)
                   / (floor_nX * floor_nX + floor_nY * floor_nY + floor_nZ * floor_nZ) * o->oGravity
                   * 2;
        objVelZ += floor_nZ * (floor_nX * floor_nX + floor_nZ * floor_nZ)
                   / (floor_nX * floor_nX + floor_nY * floor_nY + floor_nZ * floor_nZ) * o->oGravity
                   * 2;

        if (objVelX < 0.000001 && objVelX > -0.000001) {
            objVelX = 0;
        }
        if (objVelZ < 0.000001 && objVelZ > -0.000001) {
            objVelZ = 0;
        }

        if (objVelX != 0 || objVelZ != 0) {
            o->oMoveAngleYaw = atan2s(objVelZ, objVelX);
        }

        calc_obj_friction(&objFriction, floor_nY);
        o->oForwardVel = sqrtf(objVelX * objVelX + objVelZ * objVelZ) * objFriction;
    }
}

void calc_new_obj_vel_and_pos_y_underwater(struct Surface *objFloor, f32 floorY, f32 objVelX, f32 objVelZ,
                                    f32 waterY) {
    f32 floor_nX = objFloor->normal.x;
    f32 floor_nY = objFloor->normal.y;
    f32 floor_nZ = objFloor->normal.z;

    f32 netYAccel = (1.0f - o->oBuoyancy) * (-1.0f * o->oGravity);
    o->oVelY -= netYAccel;

    if (o->oVelY > 75.0) {
        o->oVelY = 75.0;
    }
    if (o->oVelY < -75.0) {
        o->oVelY = -75.0;
    }

    o->oPosY += o->oVelY;

    if (o->oPosY < floorY) {
        o->oPosY = floorY;

        if (o->oVelY < -17.5) {
            o->oVelY = -(o->oVelY / 2);
        } else {
            o->oVelY = 0;
        }
    }

    if (o->oForwardVel > 12.5 && (waterY + 30.0f) > o->oPosY && (waterY - 30.0f) < o->oPosY) {
        o->oVelY = -o->oVelY;
    }

    if ((s32) o->oPosY >= (s32) floorY && (s32) o->oPosY < (s32) floorY + 37) {
        obj_orient_graph(o, floor_nX, floor_nY, floor_nZ);

        objVelX += floor_nX * (floor_nX * floor_nX + floor_nZ * floor_nZ)
                   / (floor_nX * floor_nX + floor_nY * floor_nY + floor_nZ * floor_nZ) * netYAccel * 2;
        objVelZ += floor_nZ * (floor_nX * floor_nX + floor_nZ * floor_nZ)
                   / (floor_nX * floor_nX + floor_nY * floor_nY + floor_nZ * floor_nZ) * netYAccel * 2;
    }

    if (objVelX < 0.000001 && objVelX > -0.000001) {
        objVelX = 0;
    }
    if (objVelZ < 0.000001 && objVelZ > -0.000001) {
        objVelZ = 0;
    }

    if (o->oVelY < 0.000001 && o->oVelY > -0.000001) {
        o->oVelY = 0;
    }

    if (objVelX != 0 || objVelZ != 0) {
        o->oMoveAngleYaw = atan2s(objVelZ, objVelX);
    }

    o->oForwardVel = sqrtf(objVelX * objVelX + objVelZ * objVelZ) * 0.8;
    o->oVelY *= 0.8;
}

void obj_update_pos_vel_xz(void) {
    f32 xVel = o->oForwardVel * sins(o->oMoveAngleYaw);
    f32 zVel = o->oForwardVel * coss(o->oMoveAngleYaw);

    o->oPosX += xVel;
    o->oPosZ += zVel;
}

void obj_splash(s32 waterY, s32 objY) {
    u32 globalTimer = gGlobalTimer;

    if ((f32)(waterY + 30) > o->oPosY && o->oPosY > (f32)(waterY - 30)) {
        spawn_object(o, MODEL_IDLE_WATER_WAVE, bhvObjectWaterWave);

        if (o->oVelY < -20.0f) {
            cur_obj_play_sound_2(SOUND_OBJ_DIVING_INTO_WATER);
        }
    }

    if ((objY + 50) < waterY && !(globalTimer & 31)) {
        spawn_object(o, MODEL_WHITE_PARTICLE_SMALL, bhvObjectBubble);
    }
}

s16 object_step(void) {
    f32 objX = o->oPosX;
    f32 objY = o->oPosY;
    f32 objZ = o->oPosZ;

    f32 floorY;
    f32 waterY = FLOOR_LOWER_LIMIT_MISC;

    f32 objVelX = o->oForwardVel * sins(o->oMoveAngleYaw);
    f32 objVelZ = o->oForwardVel * coss(o->oMoveAngleYaw);

    s16 collisionFlags = 0;

    if (obj_find_wall(objX + objVelX, objY, objZ + objVelZ, objVelX, objVelZ) == 0) {
        collisionFlags += OBJ_COL_FLAG_HIT_WALL;
    }

    floorY = find_floor(objX + objVelX, objY, objZ + objVelZ, &sObjFloor);
    if (turn_obj_away_from_steep_floor(sObjFloor, floorY, objVelX, objVelZ) == 1) {
        waterY = find_water_level(objX + objVelX, objZ + objVelZ);
        if (waterY > objY) {
            calc_new_obj_vel_and_pos_y_underwater(sObjFloor, floorY, objVelX, objVelZ, waterY);
            collisionFlags += OBJ_COL_FLAG_UNDERWATER;
        } else {
            calc_new_obj_vel_and_pos_y(sObjFloor, floorY, objVelX, objVelZ);
        }
    } else {

        collisionFlags +=
            ((collisionFlags & OBJ_COL_FLAG_HIT_WALL) ^ OBJ_COL_FLAG_HIT_WALL);
    }

    obj_update_pos_vel_xz();
    if ((s32) o->oPosY == (s32) floorY) {
        collisionFlags += OBJ_COL_FLAG_GROUNDED;
    }

    if ((s32) o->oVelY == 0) {
        collisionFlags += OBJ_COL_FLAG_NO_Y_VEL;
    }

    obj_splash((s32) waterY, (s32) o->oPosY);
    return collisionFlags;
}

s16 object_step_without_floor_orient(void) {
    s16 collisionFlags = 0;
    sOrientObjWithFloor = FALSE;
    collisionFlags = object_step();
    sOrientObjWithFloor = TRUE;

    return collisionFlags;
}

void obj_move_xyz_using_fvel_and_yaw(struct Object *obj) {
    o->oVelX = obj->oForwardVel * sins(obj->oMoveAngleYaw);
    o->oVelZ = obj->oForwardVel * coss(obj->oMoveAngleYaw);

    obj->oPosX += o->oVelX;
    obj->oPosY += obj->oVelY;
    obj->oPosZ += o->oVelZ;
}

s8 is_point_within_radius_of_mario(f32 x, f32 y, f32 z, s32 dist) {
    f32 mGfxX = gMarioObject->header.gfx.pos[0];
    f32 mGfxY = gMarioObject->header.gfx.pos[1];
    f32 mGfxZ = gMarioObject->header.gfx.pos[2];

    if ((x - mGfxX) * (x - mGfxX) + (y - mGfxY) * (y - mGfxY) + (z - mGfxZ) * (z - mGfxZ)
        < (f32)(dist * dist)) {
        return TRUE;
    }

    return FALSE;
}

s8 is_point_close_to_object(struct Object *obj, f32 x, f32 y, f32 z, s32 dist) {
    f32 objX = obj->oPosX;
    f32 objY = obj->oPosY;
    f32 objZ = obj->oPosZ;

    if ((x - objX) * (x - objX) + (y - objY) * (y - objY) + (z - objZ) * (z - objZ)
        < (f32)(dist * dist)) {
        return TRUE;
    }

    return FALSE;
}

void set_object_visibility(struct Object *obj, s32 dist) {
    f32 objX = obj->oPosX;
    f32 objY = obj->oPosY;
    f32 objZ = obj->oPosZ;

    if (is_point_within_radius_of_mario(objX, objY, objZ, dist) == TRUE) {
        obj->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
    } else {
        obj->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
    }
}

s8 obj_return_home_if_safe(struct Object *obj, f32 homeX, f32 y, f32 homeZ, s32 dist) {
    f32 homeDistX = homeX - obj->oPosX;
    f32 homeDistZ = homeZ - obj->oPosZ;
    s16 angleTowardsHome = atan2s(homeDistZ, homeDistX);

    if (is_point_within_radius_of_mario(homeX, y, homeZ, dist) == TRUE) {
        return TRUE;
    } else {
        obj->oMoveAngleYaw = approach_s16_symmetric(obj->oMoveAngleYaw, angleTowardsHome, 320);
    }

    return FALSE;
}

void obj_return_and_displace_home(struct Object *obj, f32 homeX, UNUSED f32 homeY, f32 homeZ, s32 baseDisp) {
    s16 angleToNewHome;
    f32 homeDistX, homeDistZ;

    if ((s32)(random_float() * 50.0f) == 0) {
        obj->oHomeX = (f32)(baseDisp * 2) * random_float() - (f32) baseDisp + homeX;
        obj->oHomeZ = (f32)(baseDisp * 2) * random_float() - (f32) baseDisp + homeZ;
    }

    homeDistX = obj->oHomeX - obj->oPosX;
    homeDistZ = obj->oHomeZ - obj->oPosZ;
    angleToNewHome = atan2s(homeDistZ, homeDistX);
    obj->oMoveAngleYaw = approach_s16_symmetric(obj->oMoveAngleYaw, angleToNewHome, 320);
}

s8 obj_check_if_facing_toward_angle(u32 base, u32 goal, s16 range) {
    s16 dAngle = (u16) goal - (u16) base;

    if (((f32) sins(-range) < (f32) sins(dAngle)) && ((f32) sins(dAngle) < (f32) sins(range))
        && (coss(dAngle) > 0)) {
        return TRUE;
    }

    return FALSE;
}

s8 obj_find_wall_displacement(Vec3f dist, f32 x, f32 y, f32 z, f32 radius) {
    struct WallCollisionData hitbox;
    UNUSED u8 filler[32];

    hitbox.x = x;
    hitbox.y = y;
    hitbox.z = z;
    hitbox.offsetY = 10.0f;
    hitbox.radius = radius;

    if (find_wall_collisions(&hitbox) != 0) {
        dist[0] = hitbox.x - x;
        dist[1] = hitbox.y - y;
        dist[2] = hitbox.z - z;
        return TRUE;
    } else {
        return FALSE;
    }
}

void obj_spawn_yellow_coins(struct Object *obj, s8 nCoins) {
    struct Object *coin;
    s8 count;

    for (count = 0; count < nCoins; count++) {
        coin = spawn_object(obj, MODEL_YELLOW_COIN, bhvMovingYellowCoin);
        coin->oForwardVel = random_float() * 20;
        coin->oVelY = random_float() * 40 + 20;
        coin->oMoveAngleYaw = random_u16();
    }
}

s8 obj_flicker_and_disappear(struct Object *obj, s16 lifeSpan) {
    if (obj->oTimer < lifeSpan) {
        return FALSE;
    }

    if (obj->oTimer < lifeSpan + 40) {
        if (obj->oTimer % 2 != 0) {
            obj->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
        } else {
            obj->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
        }
    } else {
        obj->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        return TRUE;
    }

    return FALSE;
}

s8 current_mario_room_check(s16 room) {
    s16 result;

    if (gMarioCurrentRoom == 0) {
        if (room == sPrevCheckMarioRoom) {
            return TRUE;
        } else {
            return FALSE;
        }
    } else {
        if (room == gMarioCurrentRoom) {
            result = TRUE;
        } else {
            result = FALSE;
        }

        sPrevCheckMarioRoom = gMarioCurrentRoom;
    }

    return result;
}

s16 trigger_obj_dialog_when_facing(s32 *inDialog, s16 dialogID, f32 dist, s32 actionArg) {
    if ((is_point_within_radius_of_mario(o->oPosX, o->oPosY, o->oPosZ, (s32) dist) == TRUE
         && obj_check_if_facing_toward_angle(o->oFaceAngleYaw, gMarioObject->header.gfx.angle[1] + 0x8000, 0x1000) == TRUE
         && obj_check_if_facing_toward_angle(o->oMoveAngleYaw, o->oAngleToMario, 0x1000) == TRUE)
        || (*inDialog == TRUE)) {
        *inDialog = TRUE;

        if (set_mario_npc_dialog(actionArg) == MARIO_DIALOG_STATUS_SPEAK) {
            s16 dialogResponse = cutscene_object_with_dialog(CUTSCENE_DIALOG, o, dialogID);
            if (dialogResponse != DIALOG_RESPONSE_NONE) {
                set_mario_npc_dialog(MARIO_DIALOG_STOP);
                *inDialog = FALSE;
                return dialogResponse;
            }
            return 0;
        }
    }

    return 0;
}

void obj_check_floor_death(s16 collisionFlags, struct Surface *floor) {
    if (floor == NULL) {
        return;
    }

    if ((collisionFlags & OBJ_COL_FLAG_GROUNDED) == OBJ_COL_FLAG_GROUNDED) {
        switch (floor->type) {
            case SURFACE_BURNING:
                o->oAction = OBJ_ACT_LAVA_DEATH;
                break;

            case SURFACE_DEATH_PLANE:
                o->oAction = OBJ_ACT_DEATH_PLANE_DEATH;
                break;
            default:
                break;
        }
    }
}

s8 obj_lava_death(void) {
    struct Object *deathSmoke;

    if (o->oTimer > 30) {
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
        return TRUE;
    } else {

        o->oPosY -= 10.0f;
    }

    if ((o->oTimer % 8) == 0) {
        cur_obj_play_sound_2(SOUND_OBJ_BULLY_EXPLODE_2);
        deathSmoke = spawn_object(o, MODEL_SMOKE, bhvBobombBullyDeathSmoke);
        deathSmoke->oPosX += random_float() * 20.0f;
        deathSmoke->oPosY += random_float() * 20.0f;
        deathSmoke->oPosZ += random_float() * 20.0f;
        deathSmoke->oForwardVel = random_float() * 10.0f;
    }

    return FALSE;
}

void spawn_orange_number(s8 bhvParam, s16 relX, s16 relY, s16 relZ) {
    struct Object *orangeNumber;

    if (bhvParam >= 10) {
        return;
    }

    orangeNumber = spawn_object_relative(bhvParam, relX, relY, relZ, o, MODEL_NUMBER, bhvOrangeNumber);
    orangeNumber->oPosY += 25.0f;
}

s8 sDebugSequenceTracker = 0;
s8 sDebugTimer = 0;

UNUSED s8 debug_sequence_tracker(s16 debugInputSequence[]) {

    if (debugInputSequence[sDebugSequenceTracker] == 0) {
        sDebugSequenceTracker = 0;
        return TRUE;
    }

    if (debugInputSequence[sDebugSequenceTracker] & gPlayer3Controller->buttonPressed) {
        sDebugSequenceTracker++;
        sDebugTimer = 0;

    } else if (sDebugTimer == 10 || gPlayer3Controller->buttonPressed != 0) {
        sDebugSequenceTracker = 0;
        sDebugTimer = 0;
        return FALSE;
    }
    sDebugTimer++;

    return FALSE;
}

#include "behaviors/moving_coin.inc.c"
#include "behaviors/seaweed.inc.c"
#include "behaviors/bobomb.inc.c"
#include "behaviors/cannon_door.inc.c"
#include "behaviors/whirlpool.inc.c"
#include "behaviors/amp.inc.c"
#include "behaviors/butterfly.inc.c"
#include "behaviors/hoot.inc.c"
#include "behaviors/beta_holdable_object.inc.c"
#include "behaviors/bubble.inc.c"
#include "behaviors/water_wave.inc.c"
#include "behaviors/explosion.inc.c"
#include "behaviors/corkbox.inc.c"
#include "behaviors/bully.inc.c"
#include "behaviors/water_ring.inc.c"
#include "behaviors/bowser_bomb.inc.c"
#include "behaviors/celebration_star.inc.c"
#include "behaviors/drawbridge.inc.c"
#include "behaviors/bomp.inc.c"
#include "behaviors/sliding_platform.inc.c"
#include "behaviors/moneybag.inc.c"
#include "behaviors/bowling_ball.inc.c"
#include "behaviors/cruiser.inc.c"
#include "behaviors/spindel.inc.c"
#include "behaviors/pyramid_wall.inc.c"
#include "behaviors/pyramid_elevator.inc.c"
#include "behaviors/pyramid_top.inc.c"
#include "behaviors/sound_waterfall.inc.c"
#include "behaviors/sound_volcano.inc.c"
#include "behaviors/castle_flag.inc.c"
#include "behaviors/sound_birds.inc.c"
#include "behaviors/sound_ambient.inc.c"
#include "behaviors/sound_sand.inc.c"
#include "behaviors/castle_cannon_grate.inc.c"
#include "behaviors/snowman.inc.c"
#include "behaviors/boulder.inc.c"
#include "behaviors/cap.inc.c"
#include "behaviors/spawn_star.inc.c"
#include "behaviors/red_coin.inc.c"
#include "behaviors/hidden_star.inc.c"
#include "behaviors/rolling_log.inc.c"
#include "behaviors/mushroom_1up.inc.c"
#include "behaviors/controllable_platform.inc.c"
#include "behaviors/breakable_box_small.inc.c"
#include "behaviors/snow_mound.inc.c"
#include "behaviors/floating_platform.inc.c"
#include "behaviors/arrow_lift.inc.c"
#include "behaviors/orange_number.inc.c"
#include "behaviors/manta_ray.inc.c"
#include "behaviors/falling_pillar.inc.c"
#include "behaviors/floating_box.inc.c"
#include "behaviors/decorative_pendulum.inc.c"
#include "behaviors/treasure_chest.inc.c"
#include "behaviors/mips.inc.c"
#include "behaviors/yoshi.inc.c"
