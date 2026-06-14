#ifndef INTERACTION_H
#define INTERACTION_H

#include <PR/ultratypes.h>

#include "types.h"

#define INTERACT_HOOT            (1 <<  0)
#define INTERACT_GRABBABLE       (1 <<  1)
#define INTERACT_DOOR            (1 <<  2)
#define INTERACT_DAMAGE          (1 <<  3)
#define INTERACT_COIN            (1 <<  4)
#define INTERACT_CAP             (1 <<  5)
#define INTERACT_POLE            (1 <<  6)
#define INTERACT_KOOPA           (1 <<  7)
#define INTERACT_UNKNOWN_08      (1 <<  8)
#define INTERACT_BREAKABLE       (1 <<  9)
#define INTERACT_STRONG_WIND     (1 << 10)
#define INTERACT_WARP_DOOR       (1 << 11)
#define INTERACT_STAR_OR_KEY     (1 << 12)
#define INTERACT_WARP            (1 << 13)
#define INTERACT_CANNON_BASE     (1 << 14)
#define INTERACT_BOUNCE_TOP      (1 << 15)
#define INTERACT_WATER_RING      (1 << 16)
#define INTERACT_BULLY           (1 << 17)
#define INTERACT_FLAME           (1 << 18)
#define INTERACT_KOOPA_SHELL     (1 << 19)
#define INTERACT_BOUNCE_TOP2     (1 << 20)
#define INTERACT_MR_BLIZZARD     (1 << 21)
#define INTERACT_HIT_FROM_BELOW  (1 << 22)
#define INTERACT_TEXT            (1 << 23)
#define INTERACT_TORNADO         (1 << 24)
#define INTERACT_WHIRLPOOL       (1 << 25)
#define INTERACT_CLAM_OR_BUBBA   (1 << 26)
#define INTERACT_BBH_ENTRANCE    (1 << 27)
#define INTERACT_SNUFIT_BULLET   (1 << 28)
#define INTERACT_SHOCK           (1 << 29)
#define INTERACT_IGLOO_BARRIER   (1 << 30)
#define INTERACT_UNKNOWN_31      (1 << 31)

#define INT_SUBTYPE_FADING_WARP 0x00000001

#define INT_SUBTYPE_DELAY_INVINCIBILITY 0x00000002
#define INT_SUBTYPE_BIG_KNOCKBACK 0x00000008

#define INT_SUBTYPE_GRABS_MARIO 0x00000004
#define INT_SUBTYPE_HOLDABLE_NPC 0x00000010
#define INT_SUBTYPE_DROP_IMMEDIATELY 0x00000040
#define INT_SUBTYPE_KICKABLE 0x00000100
#define INT_SUBTYPE_NOT_GRABBABLE 0x00000200

#define INT_SUBTYPE_STAR_DOOR 0x00000020

#define INT_SUBTYPE_TWIRL_BOUNCE 0x00000080

#define INT_SUBTYPE_NO_EXIT 0x00000400
#define INT_SUBTYPE_GRAND_STAR 0x00000800

#define INT_SUBTYPE_SIGN 0x00001000
#define INT_SUBTYPE_NPC 0x00004000

#define INT_SUBTYPE_EATS_MARIO 0x00002000

#define ATTACK_PUNCH                 1
#define ATTACK_KICK_OR_TRIP          2
#define ATTACK_FROM_ABOVE            3
#define ATTACK_GROUND_POUND_OR_TWIRL 4
#define ATTACK_FAST_ATTACK           5
#define ATTACK_FROM_BELOW            6

#define INT_STATUS_ATTACK_MASK 0x000000FF

#define INT_STATUS_MARIO_STUNNED         (1 <<  0)
#define INT_STATUS_MARIO_KNOCKBACK_DMG   (1 <<  1)
#define INT_STATUS_MARIO_UNK2            (1 <<  2)
#define INT_STATUS_MARIO_DROP_OBJECT     (1 <<  3)
#define INT_STATUS_MARIO_SHOCKWAVE       (1 <<  4)
#define INT_STATUS_MARIO_UNK5            (1 <<  5)
#define INT_STATUS_MARIO_UNK6            (1 <<  6)
#define INT_STATUS_MARIO_UNK7            (1 <<  7)

#define INT_STATUS_GRABBED_MARIO         (1 << 11)
#define INT_STATUS_ATTACKED_MARIO        (1 << 13)
#define INT_STATUS_WAS_ATTACKED          (1 << 14)
#define INT_STATUS_INTERACTED            (1 << 15)
#define INT_STATUS_UNK16                 (1 << 16)
#define INT_STATUS_UNK17                 (1 << 17)
#define INT_STATUS_UNK18                 (1 << 18)
#define INT_STATUS_UNK19                 (1 << 19)
#define INT_STATUS_TRAP_TURN             (1 << 20)
#define INT_STATUS_HIT_MINE              (1 << 21)
#define INT_STATUS_STOP_RIDING           (1 << 22)
#define INT_STATUS_TOUCHED_BOB_OMB       (1 << 23)

s16 mario_obj_angle_to_object(struct MarioState *m, struct Object *o);
void mario_stop_riding_object(struct MarioState *m);
void mario_grab_used_object(struct MarioState *m);
void mario_drop_held_object(struct MarioState *m);
void mario_throw_held_object(struct MarioState *m);
void mario_stop_riding_and_holding(struct MarioState *m);
u32 does_mario_have_normal_cap_on_head(struct MarioState *m);
void mario_blow_off_cap(struct MarioState *m, f32 capSpeed);
u32 mario_lose_cap_to_enemy(u32 arg);
void mario_retrieve_cap(void);
struct Object *mario_get_collided_object(struct MarioState *m, u32 interactType);
u32 mario_check_object_grab(struct MarioState *m);
u32 get_door_save_file_flag(struct Object *door);
void mario_process_interactions(struct MarioState *m);
void mario_handle_special_floors(struct MarioState *m);

s32 net_classify_attack(struct MarioState *m, struct Object *o);
void net_knockback_mario(struct MarioState *m, s16 pushYaw, f32 force);

#endif
