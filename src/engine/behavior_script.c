#include <ultra64.h>

#include "sm64.h"
#include "behavior_data.h"
#include "behavior_script.h"
#include "game/area.h"
#include "game/behavior_actions.h"
#include "game/game_init.h"
#include "game/mario.h"
#include "game/memory.h"
#include "game/obj_behaviors_2.h"
#include "game/object_helpers.h"
#include "game/object_list_processor.h"
#include "graph_node.h"
#include "surface_collision.h"

#define BHV_CMD_GET_1ST_U8(index)  (u8)((gCurBhvCommand[index] >> 24) & 0xFF)
#define BHV_CMD_GET_2ND_U8(index)  (u8)((gCurBhvCommand[index] >> 16) & 0xFF)
#define BHV_CMD_GET_3RD_U8(index)  (u8)((gCurBhvCommand[index] >> 8) & 0xFF)
#define BHV_CMD_GET_4TH_U8(index)  (u8)((gCurBhvCommand[index]) & 0xFF)

#define BHV_CMD_GET_1ST_S16(index) (s16)(gCurBhvCommand[index] >> 16)
#define BHV_CMD_GET_2ND_S16(index) (s16)(gCurBhvCommand[index] & 0xFFFF)

#define BHV_CMD_GET_U32(index)     (u32)(gCurBhvCommand[index])
#define BHV_CMD_GET_VPTR(index)    (void *)(gCurBhvCommand[index])

#define BHV_CMD_GET_ADDR_OF_CMD(index) (uintptr_t)(&gCurBhvCommand[index])

static u16 gRandomSeed16;

UNUSED static void goto_behavior_unused(const BehaviorScript *bhvAddr) {
    gCurBhvCommand = segmented_to_virtual(bhvAddr);
    gCurrentObject->bhvStackIndex = 0;
}

u16 random_u16(void) {
    u16 temp1, temp2;

    if (gRandomSeed16 == 22026) {
        gRandomSeed16 = 0;
    }

    temp1 = (gRandomSeed16 & 0x00FF) << 8;
    temp1 = temp1 ^ gRandomSeed16;

    gRandomSeed16 = ((temp1 & 0x00FF) << 8) + ((temp1 & 0xFF00) >> 8);

    temp1 = ((temp1 & 0x00FF) << 1) ^ gRandomSeed16;
    temp2 = (temp1 >> 1) ^ 0xFF80;

    if ((temp1 & 1) == 0) {
        if (temp2 == 43605) {
            gRandomSeed16 = 0;
        } else {
            gRandomSeed16 = temp2 ^ 0x1FF4;
        }
    } else {
        gRandomSeed16 = temp2 ^ 0x8180;
    }

    return gRandomSeed16;
}

f32 random_float(void) {
    f32 rnd = random_u16();
    return rnd / (double) 0x10000;
}

s32 random_sign(void) {
    if (random_u16() >= 0x7FFF) {
        return 1;
    } else {
        return -1;
    }
}

void obj_update_gfx_pos_and_angle(struct Object *obj) {
    obj->header.gfx.pos[0] = obj->oPosX;
    obj->header.gfx.pos[1] = obj->oPosY + obj->oGraphYOffset;
    obj->header.gfx.pos[2] = obj->oPosZ;

    obj->header.gfx.angle[0] = obj->oFaceAnglePitch & 0xFFFF;
    obj->header.gfx.angle[1] = obj->oFaceAngleYaw & 0xFFFF;
    obj->header.gfx.angle[2] = obj->oFaceAngleRoll & 0xFFFF;
}

static void cur_obj_bhv_stack_push(uintptr_t bhvAddr) {
    gCurrentObject->bhvStack[gCurrentObject->bhvStackIndex] = bhvAddr;
    gCurrentObject->bhvStackIndex++;
}

static uintptr_t cur_obj_bhv_stack_pop(void) {
    uintptr_t bhvAddr;

    gCurrentObject->bhvStackIndex--;
    bhvAddr = gCurrentObject->bhvStack[gCurrentObject->bhvStackIndex];

    return bhvAddr;
}

UNUSED static void stub_behavior_script_1(void) {
    for (;;) {
        ;
    }
}

static s32 bhv_cmd_hide(void) {
    cur_obj_hide();

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_disable_rendering(void) {
    gCurrentObject->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_billboard(void) {
    gCurrentObject->header.gfx.node.flags |= GRAPH_RENDER_BILLBOARD;

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_set_model(void) {
    s32 modelID = BHV_CMD_GET_2ND_S16(0);

    gCurrentObject->header.gfx.sharedChild = gLoadedGraphNodes[modelID];

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_spawn_child(void) {
    u32 model = BHV_CMD_GET_U32(1);
    const BehaviorScript *behavior = BHV_CMD_GET_VPTR(2);

    struct Object *child = spawn_object_at_origin(gCurrentObject, 0, model, behavior);
    obj_copy_pos_and_angle(child, gCurrentObject);

    gCurBhvCommand += 3;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_spawn_obj(void) {
    u32 model = BHV_CMD_GET_U32(1);
    const BehaviorScript *behavior = BHV_CMD_GET_VPTR(2);

    struct Object *object = spawn_object_at_origin(gCurrentObject, 0, model, behavior);
    obj_copy_pos_and_angle(object, gCurrentObject);

    gCurrentObject->prevObj = object;

    gCurBhvCommand += 3;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_spawn_child_with_param(void) {
    u32 bhvParam = BHV_CMD_GET_2ND_S16(0);
    u32 modelID = BHV_CMD_GET_U32(1);
    const BehaviorScript *behavior = BHV_CMD_GET_VPTR(2);

    struct Object *child = spawn_object_at_origin(gCurrentObject, 0, modelID, behavior);
    obj_copy_pos_and_angle(child, gCurrentObject);
    child->oBhvParams2ndByte = bhvParam;

    gCurBhvCommand += 3;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_deactivate(void) {
    gCurrentObject->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    return BHV_PROC_BREAK;
}

static s32 bhv_cmd_break(void) {
    return BHV_PROC_BREAK;
}

static s32 bhv_cmd_break_unused(void) {
    return BHV_PROC_BREAK;
}

static s32 bhv_cmd_call(void) {
    const BehaviorScript *jumpAddress;
    gCurBhvCommand++;

    cur_obj_bhv_stack_push(BHV_CMD_GET_ADDR_OF_CMD(1));
    jumpAddress = segmented_to_virtual(BHV_CMD_GET_VPTR(0));
    gCurBhvCommand = jumpAddress;

    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_return(void) {
    gCurBhvCommand = (const BehaviorScript *) cur_obj_bhv_stack_pop();
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_delay(void) {
    s16 num = BHV_CMD_GET_2ND_S16(0);

    if (gCurrentObject->bhvDelayTimer < num - 1) {
        gCurrentObject->bhvDelayTimer++;
    } else {
        gCurrentObject->bhvDelayTimer = 0;
        gCurBhvCommand++;
    }

    return BHV_PROC_BREAK;
}

static s32 bhv_cmd_delay_var(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s32 num = cur_obj_get_int(field);

    if (gCurrentObject->bhvDelayTimer < num - 1) {
        gCurrentObject->bhvDelayTimer++;
    } else {
        gCurrentObject->bhvDelayTimer = 0;
        gCurBhvCommand++;
    }

    return BHV_PROC_BREAK;
}

static s32 bhv_cmd_goto(void) {
    gCurBhvCommand++;
    gCurBhvCommand = segmented_to_virtual(BHV_CMD_GET_VPTR(0));
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_begin_repeat_unused(void) {
    s32 count = BHV_CMD_GET_2ND_U8(0);

    cur_obj_bhv_stack_push(BHV_CMD_GET_ADDR_OF_CMD(1));
    cur_obj_bhv_stack_push(count);

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_begin_repeat(void) {
    s32 count = BHV_CMD_GET_2ND_S16(0);

    cur_obj_bhv_stack_push(BHV_CMD_GET_ADDR_OF_CMD(1));
    cur_obj_bhv_stack_push(count);

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_end_repeat(void) {
    u32 count = cur_obj_bhv_stack_pop();
    count--;

    if (count != 0) {
        gCurBhvCommand = (const BehaviorScript *) cur_obj_bhv_stack_pop();

        cur_obj_bhv_stack_push(BHV_CMD_GET_ADDR_OF_CMD(0));
        cur_obj_bhv_stack_push(count);
    } else {
        cur_obj_bhv_stack_pop();
        gCurBhvCommand++;
    }

    return BHV_PROC_BREAK;
}

static s32 bhv_cmd_end_repeat_continue(void) {
    u32 count = cur_obj_bhv_stack_pop();
    count--;

    if (count != 0) {
        gCurBhvCommand = (const BehaviorScript *) cur_obj_bhv_stack_pop();

        cur_obj_bhv_stack_push(BHV_CMD_GET_ADDR_OF_CMD(0));
        cur_obj_bhv_stack_push(count);
    } else {
        cur_obj_bhv_stack_pop();
        gCurBhvCommand++;
    }

    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_begin_loop(void) {
    cur_obj_bhv_stack_push(BHV_CMD_GET_ADDR_OF_CMD(1));

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_end_loop(void) {
    gCurBhvCommand = (const BehaviorScript *) cur_obj_bhv_stack_pop();
    cur_obj_bhv_stack_push(BHV_CMD_GET_ADDR_OF_CMD(0));

    return BHV_PROC_BREAK;
}

typedef void (*NativeBhvFunc)(void);
static s32 bhv_cmd_call_native(void) {
    NativeBhvFunc behaviorFunc = BHV_CMD_GET_VPTR(1);

    behaviorFunc();

    gCurBhvCommand += 2;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_set_float(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    f32 value = BHV_CMD_GET_2ND_S16(0);

    cur_obj_set_float(field, value);

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_set_int(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s16 value = BHV_CMD_GET_2ND_S16(0);

    cur_obj_set_int(field, value);

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_set_int_unused(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s32 value = BHV_CMD_GET_2ND_S16(1);

    cur_obj_set_int(field, value);

    gCurBhvCommand += 2;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_set_random_float(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    f32 min = BHV_CMD_GET_2ND_S16(0);
    f32 range = BHV_CMD_GET_1ST_S16(1);

    cur_obj_set_float(field, (range * random_float()) + min);

    gCurBhvCommand += 2;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_set_random_int(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s32 min = BHV_CMD_GET_2ND_S16(0);
    s32 range = BHV_CMD_GET_1ST_S16(1);

    cur_obj_set_int(field, (s32)(range * random_float()) + min);

    gCurBhvCommand += 2;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_set_int_rand_rshift(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s32 min = BHV_CMD_GET_2ND_S16(0);
    s32 rshift = BHV_CMD_GET_1ST_S16(1);

    cur_obj_set_int(field, (random_u16() >> rshift) + min);

    gCurBhvCommand += 2;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_add_random_float(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    f32 min = BHV_CMD_GET_2ND_S16(0);
    f32 range = BHV_CMD_GET_1ST_S16(1);

    cur_obj_set_float(field, cur_obj_get_float(field) + min + (range * random_float()));

    gCurBhvCommand += 2;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_add_int_rand_rshift(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s32 min = BHV_CMD_GET_2ND_S16(0);
    s32 rshift = BHV_CMD_GET_1ST_S16(1);
    s32 rnd = random_u16();

    cur_obj_set_int(field, (cur_obj_get_int(field) + min) + (rnd >> rshift));

    gCurBhvCommand += 2;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_add_float(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    f32 value = BHV_CMD_GET_2ND_S16(0);

    cur_obj_add_float(field, value);

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_add_int(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s16 value = BHV_CMD_GET_2ND_S16(0);

    cur_obj_add_int(field, value);

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_or_int(void) {
    u8 objectOffset = BHV_CMD_GET_2ND_U8(0);
    s32 value = BHV_CMD_GET_2ND_S16(0);

    value &= 0xFFFF;
    cur_obj_or_int(objectOffset, value);

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_bit_clear(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s32 value = BHV_CMD_GET_2ND_S16(0);

    value = (value & 0xFFFF) ^ 0xFFFF;
    cur_obj_and_int(field, value);

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_load_animations(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);

    cur_obj_set_vptr(field, BHV_CMD_GET_VPTR(1));

    gCurBhvCommand += 2;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_animate(void) {
    s32 animIndex = BHV_CMD_GET_2ND_U8(0);
    struct Animation **animations = gCurrentObject->oAnimations;

    geo_obj_init_animation(&gCurrentObject->header.gfx, &animations[animIndex]);

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_drop_to_floor(void) {
    f32 x = gCurrentObject->oPosX;
    f32 y = gCurrentObject->oPosY;
    f32 z = gCurrentObject->oPosZ;

    f32 floor = find_floor_height(x, y + 200.0f, z);
    gCurrentObject->oPosY = floor;
    gCurrentObject->oMoveFlags |= OBJ_MOVE_ON_GROUND;

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_nop_1(void) {
    UNUSED u8 field = BHV_CMD_GET_2ND_U8(0);

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_nop_3(void) {
    UNUSED u8 field = BHV_CMD_GET_2ND_U8(0);

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_nop_2(void) {
    UNUSED u8 field = BHV_CMD_GET_2ND_U8(0);

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_sum_float(void) {
    u32 fieldDst = BHV_CMD_GET_2ND_U8(0);
    u32 fieldSrc1 = BHV_CMD_GET_3RD_U8(0);
    u32 fieldSrc2 = BHV_CMD_GET_4TH_U8(0);

    cur_obj_set_float(fieldDst, cur_obj_get_float(fieldSrc1) + cur_obj_get_float(fieldSrc2));

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_sum_int(void) {
    u32 fieldDst = BHV_CMD_GET_2ND_U8(0);
    u32 fieldSrc1 = BHV_CMD_GET_3RD_U8(0);
    u32 fieldSrc2 = BHV_CMD_GET_4TH_U8(0);

    cur_obj_set_int(fieldDst, cur_obj_get_int(fieldSrc1) + cur_obj_get_int(fieldSrc2));

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_set_hitbox(void) {
    s16 radius = BHV_CMD_GET_1ST_S16(1);
    s16 height = BHV_CMD_GET_2ND_S16(1);

    gCurrentObject->hitboxRadius = radius;
    gCurrentObject->hitboxHeight = height;

    gCurBhvCommand += 2;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_set_hurtbox(void) {
    s16 radius = BHV_CMD_GET_1ST_S16(1);
    s16 height = BHV_CMD_GET_2ND_S16(1);

    gCurrentObject->hurtboxRadius = radius;
    gCurrentObject->hurtboxHeight = height;

    gCurBhvCommand += 2;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_set_hitbox_with_offset(void) {
    s16 radius = BHV_CMD_GET_1ST_S16(1);
    s16 height = BHV_CMD_GET_2ND_S16(1);
    s16 downOffset = BHV_CMD_GET_1ST_S16(2);

    gCurrentObject->hitboxRadius = radius;
    gCurrentObject->hitboxHeight = height;
    gCurrentObject->hitboxDownOffset = downOffset;

    gCurBhvCommand += 3;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_nop_4(void) {
    UNUSED s16 field = BHV_CMD_GET_2ND_U8(0);
    UNUSED s16 value = BHV_CMD_GET_2ND_S16(0);

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_begin(void) {

    if (cur_obj_has_behavior(bhvHauntedChair)) {
        bhv_init_room();
    }
    if (cur_obj_has_behavior(bhvMadPiano)) {
        bhv_init_room();
    }

    if (cur_obj_has_behavior(bhvMessagePanel)) {
        gCurrentObject->oCollisionDistance = 150.0f;
    }
    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

UNUSED static void bhv_cmd_set_int_random_from_table(s32 tableSize) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s32 table[16];
    s32 i;

    for (i = 0; i <= tableSize / 2; i += 2) {
        table[i] = BHV_CMD_GET_1ST_S16(i + 1);
        table[i + 1] = BHV_CMD_GET_2ND_S16(i + 1);
    }

    cur_obj_set_int(field, table[(s32)(tableSize * random_float())]);

}

static s32 bhv_cmd_load_collision_data(void) {
    u32 *collisionData = segmented_to_virtual(BHV_CMD_GET_VPTR(1));

    gCurrentObject->collisionData = collisionData;

    gCurBhvCommand += 2;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_set_home(void) {
    gCurrentObject->oHomeX = gCurrentObject->oPosX;
    gCurrentObject->oHomeY = gCurrentObject->oPosY;
    gCurrentObject->oHomeZ = gCurrentObject->oPosZ;

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_set_interact_type(void) {
    gCurrentObject->oInteractType = BHV_CMD_GET_U32(1);

    gCurBhvCommand += 2;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_set_interact_subtype(void) {
    gCurrentObject->oInteractionSubtype = BHV_CMD_GET_U32(1);

    gCurBhvCommand += 2;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_scale(void) {
    UNUSED u8 unusedField = BHV_CMD_GET_2ND_U8(0);
    s16 percent = BHV_CMD_GET_2ND_S16(0);

    cur_obj_scale(percent / 100.0f);

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_set_obj_physics(void) {
    UNUSED f32 unused1, unused2;

    gCurrentObject->oWallHitboxRadius = BHV_CMD_GET_1ST_S16(1);
    gCurrentObject->oGravity = BHV_CMD_GET_2ND_S16(1) / 100.0f;
    gCurrentObject->oBounciness = BHV_CMD_GET_1ST_S16(2) / 100.0f;
    gCurrentObject->oDragStrength = BHV_CMD_GET_2ND_S16(2) / 100.0f;
    gCurrentObject->oFriction = BHV_CMD_GET_1ST_S16(3) / 100.0f;
    gCurrentObject->oBuoyancy = BHV_CMD_GET_2ND_S16(3) / 100.0f;

    unused1 = BHV_CMD_GET_1ST_S16(4) / 100.0f;
    unused2 = BHV_CMD_GET_2ND_S16(4) / 100.0f;

    gCurBhvCommand += 5;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_parent_bit_clear(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s32 value = BHV_CMD_GET_U32(1);

    value = value ^ 0xFFFFFFFF;
    obj_and_int(gCurrentObject->parentObj, field, value);

    gCurBhvCommand += 2;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_spawn_water_droplet(void) {
    struct WaterDropletParams *dropletParams = BHV_CMD_GET_VPTR(1);

    spawn_water_droplet(gCurrentObject, dropletParams);

    gCurBhvCommand += 2;
    return BHV_PROC_CONTINUE;
}

static s32 bhv_cmd_animate_texture(void) {
    u8 field = BHV_CMD_GET_2ND_U8(0);
    s16 rate = BHV_CMD_GET_2ND_S16(0);

    if ((gGlobalTimer % rate) == 0) {
        cur_obj_add_int(field, 1);
    }

    gCurBhvCommand++;
    return BHV_PROC_CONTINUE;
}

void stub_behavior_script_2(void) {
}

typedef s32 (*BhvCommandProc)(void);
static BhvCommandProc BehaviorCmdTable[] = {
    bhv_cmd_begin,
    bhv_cmd_delay,
    bhv_cmd_call,
    bhv_cmd_return,
    bhv_cmd_goto,
    bhv_cmd_begin_repeat,
    bhv_cmd_end_repeat,
    bhv_cmd_end_repeat_continue,
    bhv_cmd_begin_loop,
    bhv_cmd_end_loop,
    bhv_cmd_break,
    bhv_cmd_break_unused,
    bhv_cmd_call_native,
    bhv_cmd_add_float,
    bhv_cmd_set_float,
    bhv_cmd_add_int,
    bhv_cmd_set_int,
    bhv_cmd_or_int,
    bhv_cmd_bit_clear,
    bhv_cmd_set_int_rand_rshift,
    bhv_cmd_set_random_float,
    bhv_cmd_set_random_int,
    bhv_cmd_add_random_float,
    bhv_cmd_add_int_rand_rshift,
    bhv_cmd_nop_1,
    bhv_cmd_nop_2,
    bhv_cmd_nop_3,
    bhv_cmd_set_model,
    bhv_cmd_spawn_child,
    bhv_cmd_deactivate,
    bhv_cmd_drop_to_floor,
    bhv_cmd_sum_float,
    bhv_cmd_sum_int,
    bhv_cmd_billboard,
    bhv_cmd_hide,
    bhv_cmd_set_hitbox,
    bhv_cmd_nop_4,
    bhv_cmd_delay_var,
    bhv_cmd_begin_repeat_unused,
    bhv_cmd_load_animations,
    bhv_cmd_animate,
    bhv_cmd_spawn_child_with_param,
    bhv_cmd_load_collision_data,
    bhv_cmd_set_hitbox_with_offset,
    bhv_cmd_spawn_obj,
    bhv_cmd_set_home,
    bhv_cmd_set_hurtbox,
    bhv_cmd_set_interact_type,
    bhv_cmd_set_obj_physics,
    bhv_cmd_set_interact_subtype,
    bhv_cmd_scale,
    bhv_cmd_parent_bit_clear,
    bhv_cmd_animate_texture,
    bhv_cmd_disable_rendering,
    bhv_cmd_set_int_unused,
    bhv_cmd_spawn_water_droplet,
};

void cur_obj_update(void) {
    UNUSED u8 filler[4];

    s16 objFlags = gCurrentObject->oFlags;
    f32 distanceFromMario;
    BhvCommandProc bhvCmdProc;
    s32 bhvProcResult;

    if (objFlags & OBJ_FLAG_COMPUTE_DIST_TO_MARIO) {
        gCurrentObject->oDistanceToMario = dist_between_objects(gCurrentObject, gMarioObject);
        distanceFromMario = gCurrentObject->oDistanceToMario;
    } else {
        distanceFromMario = 0.0f;
    }

    if (objFlags & OBJ_FLAG_COMPUTE_ANGLE_TO_MARIO) {
        gCurrentObject->oAngleToMario = obj_angle_to_object(gCurrentObject, gMarioObject);
    }

    if (gCurrentObject->oAction != gCurrentObject->oPrevAction) {
        (void) (gCurrentObject->oTimer = 0, gCurrentObject->oSubAction = 0,
                gCurrentObject->oPrevAction = gCurrentObject->oAction);
    }

    gCurBhvCommand = gCurrentObject->curBhvCommand;

    do {
        bhvCmdProc = BehaviorCmdTable[*gCurBhvCommand >> 24];
        bhvProcResult = bhvCmdProc();
    } while (bhvProcResult == BHV_PROC_CONTINUE);

    gCurrentObject->curBhvCommand = gCurBhvCommand;

    if (gCurrentObject->oTimer < 0x3FFFFFFF) {
        gCurrentObject->oTimer++;
    }

    if (gCurrentObject->oAction != gCurrentObject->oPrevAction) {
        (void) (gCurrentObject->oTimer = 0, gCurrentObject->oSubAction = 0,
                gCurrentObject->oPrevAction = gCurrentObject->oAction);
    }

    objFlags = (s16) gCurrentObject->oFlags;

    if (objFlags & OBJ_FLAG_SET_FACE_ANGLE_TO_MOVE_ANGLE) {
        obj_set_face_angle_to_move_angle(gCurrentObject);
    }

    if (objFlags & OBJ_FLAG_SET_FACE_YAW_TO_MOVE_YAW) {
        gCurrentObject->oFaceAngleYaw = gCurrentObject->oMoveAngleYaw;
    }

    if (objFlags & OBJ_FLAG_MOVE_XZ_USING_FVEL) {
        cur_obj_move_xz_using_fvel_and_yaw();
    }

    if (objFlags & OBJ_FLAG_MOVE_Y_WITH_TERMINAL_VEL) {
        cur_obj_move_y_with_terminal_vel();
    }

    if (objFlags & OBJ_FLAG_TRANSFORM_RELATIVE_TO_PARENT) {
        obj_build_transform_relative_to_parent(gCurrentObject);
    }

    if (objFlags & OBJ_FLAG_SET_THROW_MATRIX_FROM_TRANSFORM) {
        obj_set_throw_matrix_from_transform(gCurrentObject);
    }

    if (objFlags & OBJ_FLAG_UPDATE_GFX_POS_AND_ANGLE) {
        obj_update_gfx_pos_and_angle(gCurrentObject);
    }

    if (gCurrentObject->oRoom != -1) {

        cur_obj_enable_rendering_if_mario_in_room();
    } else if ((objFlags & OBJ_FLAG_COMPUTE_DIST_TO_MARIO) && gCurrentObject->collisionData == NULL) {
        if (!(objFlags & OBJ_FLAG_ACTIVE_FROM_AFAR)) {

            if (distanceFromMario > gCurrentObject->oDrawingDistance) {

                gCurrentObject->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
                gCurrentObject->activeFlags |= ACTIVE_FLAG_FAR_AWAY;
            } else if (gCurrentObject->oHeldState == HELD_FREE) {

                gCurrentObject->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
                gCurrentObject->activeFlags &= ~ACTIVE_FLAG_FAR_AWAY;
            }
        }
    }
}
