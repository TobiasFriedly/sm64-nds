#ifndef SM64_H
#define SM64_H

#include "types.h"
#include "config.h"
#include "object_fields.h"
#include "object_constants.h"
#include "sounds.h"
#include "model_ids.h"
#include "mario_animation_ids.h"
#include "mario_geo_switch_case_ids.h"
#include "surface_terrains.h"
#include "macros.h"

#ifdef CRASH_SCREEN_INCLUDED
#define DEBUG_ASSERT(exp) do { if (!(exp)) _n64_assert(__FILE__, __LINE__, #exp, 1); } while (0)
#else
#define DEBUG_ASSERT(exp)
#endif

#ifdef AVOID_UB
#define GET_HIGH_U16_OF_32(var) ((u16)((var) >> 16))
#define GET_HIGH_S16_OF_32(var) ((s16)((var) >> 16))
#define GET_LOW_U16_OF_32(var) ((u16)((var) & 0xFFFF))
#define GET_LOW_S16_OF_32(var) ((s16)((var) & 0xFFFF))
#define SET_HIGH_U16_OF_32(var, x) ((var) = ((var) & 0xFFFF) | ((x) << 16))
#define SET_HIGH_S16_OF_32(var, x) ((var) = ((var) & 0xFFFF) | ((x) << 16))
#else
#define GET_HIGH_U16_OF_32(var) (((u16 *)&(var))[0])
#define GET_HIGH_S16_OF_32(var) (((s16 *)&(var))[0])
#define GET_LOW_U16_OF_32(var) (((u16 *)&(var))[1])
#define GET_LOW_S16_OF_32(var) (((s16 *)&(var))[1])
#define SET_HIGH_U16_OF_32(var, x) ((((u16 *)&(var))[0]) = (x))
#define SET_HIGH_S16_OF_32(var, x) ((((s16 *)&(var))[0]) = (x))
#endif

#define LAYER_FORCE             0
#define LAYER_OPAQUE            1
#define LAYER_OPAQUE_DECAL      2
#define LAYER_OPAQUE_INTER      3
#define LAYER_ALPHA             4
#define LAYER_TRANSPARENT       5
#define LAYER_TRANSPARENT_DECAL 6
#define LAYER_TRANSPARENT_INTER 7

#define INPUT_NONZERO_ANALOG         0x0001
#define INPUT_A_PRESSED              0x0002
#define INPUT_OFF_FLOOR              0x0004
#define INPUT_ABOVE_SLIDE            0x0008
#define INPUT_FIRST_PERSON           0x0010
#define INPUT_UNKNOWN_5              0x0020
#define INPUT_SQUISHED               0x0040
#define INPUT_A_DOWN                 0x0080
#define INPUT_IN_POISON_GAS          0x0100
#define INPUT_IN_WATER               0x0200
#define INPUT_STOMPED                0x0400
#define INPUT_INTERACT_OBJ_GRABBABLE 0x0800
#define INPUT_UNKNOWN_12             0x1000
#define INPUT_B_PRESSED              0x2000
#define INPUT_Z_DOWN                 0x4000
#define INPUT_Z_PRESSED              0x8000

#define GROUND_STEP_LEFT_GROUND              0
#define GROUND_STEP_NONE                     1
#define GROUND_STEP_HIT_WALL                 2
#define GROUND_STEP_HIT_WALL_STOP_QSTEPS     2
#define GROUND_STEP_HIT_WALL_CONTINUE_QSTEPS 3

#define AIR_STEP_CHECK_LEDGE_GRAB 0x00000001
#define AIR_STEP_CHECK_HANG       0x00000002

#define AIR_STEP_NONE            0
#define AIR_STEP_LANDED          1
#define AIR_STEP_HIT_WALL        2
#define AIR_STEP_GRABBED_LEDGE   3
#define AIR_STEP_GRABBED_CEILING 4
#define AIR_STEP_HIT_LAVA_WALL   6

#define WATER_STEP_NONE        0
#define WATER_STEP_HIT_FLOOR   1
#define WATER_STEP_HIT_CEILING 2
#define WATER_STEP_CANCELLED   3
#define WATER_STEP_HIT_WALL    4

#define PARTICLE_DUST                  (1 <<  0)
#define PARTICLE_VERTICAL_STAR         (1 <<  1)
#define PARTICLE_2                     (1 <<  2)
#define PARTICLE_SPARKLES              (1 <<  3)
#define PARTICLE_HORIZONTAL_STAR       (1 <<  4)
#define PARTICLE_BUBBLE                (1 <<  5)
#define PARTICLE_WATER_SPLASH          (1 <<  6)
#define PARTICLE_IDLE_WATER_WAVE       (1 <<  7)
#define PARTICLE_SHALLOW_WATER_WAVE    (1 <<  8)
#define PARTICLE_PLUNGE_BUBBLE         (1 <<  9)
#define PARTICLE_WAVE_TRAIL            (1 << 10)
#define PARTICLE_FIRE                  (1 << 11)
#define PARTICLE_SHALLOW_WATER_SPLASH  (1 << 12)
#define PARTICLE_LEAF                  (1 << 13)
#define PARTICLE_SNOW                  (1 << 14)
#define PARTICLE_DIRT                  (1 << 15)
#define PARTICLE_MIST_CIRCLE           (1 << 16)
#define PARTICLE_BREATH                (1 << 17)
#define PARTICLE_TRIANGLE              (1 << 18)
#define PARTICLE_19                    (1 << 19)

#define MODEL_STATE_NOISE_ALPHA 0x180
#define MODEL_STATE_METAL       0x200

#define MARIO_NORMAL_CAP                0x00000001
#define MARIO_VANISH_CAP                0x00000002
#define MARIO_METAL_CAP                 0x00000004
#define MARIO_WING_CAP                  0x00000008
#define MARIO_CAP_ON_HEAD               0x00000010
#define MARIO_CAP_IN_HAND               0x00000020
#define MARIO_METAL_SHOCK               0x00000040
#define MARIO_TELEPORTING               0x00000080
#define MARIO_UNKNOWN_08                0x00000100
#define MARIO_UNKNOWN_13                0x00002000
#define MARIO_ACTION_SOUND_PLAYED       0x00010000
#define MARIO_MARIO_SOUND_PLAYED        0x00020000
#define MARIO_UNKNOWN_18                0x00040000
#define MARIO_PUNCHING                  0x00100000
#define MARIO_KICKING                   0x00200000
#define MARIO_TRIPPING                  0x00400000
#define MARIO_UNKNOWN_25                0x02000000
#define MARIO_UNKNOWN_30                0x40000000
#define MARIO_UNKNOWN_31                0x80000000

#define MARIO_SPECIAL_CAPS (MARIO_VANISH_CAP | MARIO_METAL_CAP | MARIO_WING_CAP)
#define MARIO_CAPS (MARIO_NORMAL_CAP | MARIO_SPECIAL_CAPS)

#define ACT_ID_MASK 0x000001FF

#define ACT_GROUP_MASK       0x000001C0
#define ACT_GROUP_STATIONARY  (0 << 6)
#define ACT_GROUP_MOVING      (1 << 6)
#define ACT_GROUP_AIRBORNE    (2 << 6)
#define ACT_GROUP_SUBMERGED   (3 << 6)
#define ACT_GROUP_CUTSCENE    (4 << 6)
#define ACT_GROUP_AUTOMATIC   (5 << 6)
#define ACT_GROUP_OBJECT      (6 << 6)

#define ACT_FLAG_STATIONARY                   (1 <<  9)
#define ACT_FLAG_MOVING                       (1 << 10)
#define ACT_FLAG_AIR                          (1 << 11)
#define ACT_FLAG_INTANGIBLE                   (1 << 12)
#define ACT_FLAG_SWIMMING                     (1 << 13)
#define ACT_FLAG_METAL_WATER                  (1 << 14)
#define ACT_FLAG_SHORT_HITBOX                 (1 << 15)
#define ACT_FLAG_RIDING_SHELL                 (1 << 16)
#define ACT_FLAG_INVULNERABLE                 (1 << 17)
#define ACT_FLAG_BUTT_OR_STOMACH_SLIDE        (1 << 18)
#define ACT_FLAG_DIVING                       (1 << 19)
#define ACT_FLAG_ON_POLE                      (1 << 20)
#define ACT_FLAG_HANGING                      (1 << 21)
#define ACT_FLAG_IDLE                         (1 << 22)
#define ACT_FLAG_ATTACKING                    (1 << 23)
#define ACT_FLAG_ALLOW_VERTICAL_WIND_ACTION   (1 << 24)
#define ACT_FLAG_CONTROL_JUMP_HEIGHT          (1 << 25)
#define ACT_FLAG_ALLOW_FIRST_PERSON           (1 << 26)
#define ACT_FLAG_PAUSE_EXIT                   (1 << 27)
#define ACT_FLAG_SWIMMING_OR_FLYING           (1 << 28)
#define ACT_FLAG_WATER_OR_TEXT                (1 << 29)
#define ACT_FLAG_THROWING                     (1 << 31)

#define ACT_UNINITIALIZED              0x00000000

#define ACT_IDLE                       0x0C400201
#define ACT_START_SLEEPING             0x0C400202
#define ACT_SLEEPING                   0x0C000203
#define ACT_WAKING_UP                  0x0C000204
#define ACT_PANTING                    0x0C400205
#define ACT_HOLD_PANTING_UNUSED        0x08000206
#define ACT_HOLD_IDLE                  0x08000207
#define ACT_HOLD_HEAVY_IDLE            0x08000208
#define ACT_STANDING_AGAINST_WALL      0x0C400209
#define ACT_COUGHING                   0x0C40020A
#define ACT_SHIVERING                  0x0C40020B
#define ACT_IN_QUICKSAND               0x0002020D
#define ACT_UNKNOWN_0002020E           0x0002020E
#define ACT_CROUCHING                  0x0C008220
#define ACT_START_CROUCHING            0x0C008221
#define ACT_STOP_CROUCHING             0x0C008222
#define ACT_START_CRAWLING             0x0C008223
#define ACT_STOP_CRAWLING              0x0C008224
#define ACT_SLIDE_KICK_SLIDE_STOP      0x08000225
#define ACT_SHOCKWAVE_BOUNCE           0x00020226
#define ACT_FIRST_PERSON               0x0C000227
#define ACT_BACKFLIP_LAND_STOP         0x0800022F
#define ACT_JUMP_LAND_STOP             0x0C000230
#define ACT_DOUBLE_JUMP_LAND_STOP      0x0C000231
#define ACT_FREEFALL_LAND_STOP         0x0C000232
#define ACT_SIDE_FLIP_LAND_STOP        0x0C000233
#define ACT_HOLD_JUMP_LAND_STOP        0x08000234
#define ACT_HOLD_FREEFALL_LAND_STOP    0x08000235
#define ACT_AIR_THROW_LAND             0x80000A36
#define ACT_TWIRL_LAND                 0x18800238
#define ACT_LAVA_BOOST_LAND            0x08000239
#define ACT_TRIPLE_JUMP_LAND_STOP      0x0800023A
#define ACT_LONG_JUMP_LAND_STOP        0x0800023B
#define ACT_GROUND_POUND_LAND          0x0080023C
#define ACT_BRAKING_STOP               0x0C00023D
#define ACT_BUTT_SLIDE_STOP            0x0C00023E
#define ACT_HOLD_BUTT_SLIDE_STOP       0x0800043F

#define ACT_WALKING                    0x04000440
#define ACT_HOLD_WALKING               0x00000442
#define ACT_TURNING_AROUND             0x00000443
#define ACT_FINISH_TURNING_AROUND      0x00000444
#define ACT_BRAKING                    0x04000445
#define ACT_RIDING_SHELL_GROUND        0x20810446
#define ACT_HOLD_HEAVY_WALKING         0x00000447
#define ACT_CRAWLING                   0x04008448
#define ACT_BURNING_GROUND             0x00020449
#define ACT_DECELERATING               0x0400044A
#define ACT_HOLD_DECELERATING          0x0000044B
#define ACT_BEGIN_SLIDING              0x00000050
#define ACT_HOLD_BEGIN_SLIDING         0x00000051
#define ACT_BUTT_SLIDE                 0x00840452
#define ACT_STOMACH_SLIDE              0x008C0453
#define ACT_HOLD_BUTT_SLIDE            0x00840454
#define ACT_HOLD_STOMACH_SLIDE         0x008C0455
#define ACT_DIVE_SLIDE                 0x00880456
#define ACT_MOVE_PUNCHING              0x00800457
#define ACT_CROUCH_SLIDE               0x04808459
#define ACT_SLIDE_KICK_SLIDE           0x0080045A
#define ACT_HARD_BACKWARD_GROUND_KB    0x00020460
#define ACT_HARD_FORWARD_GROUND_KB     0x00020461
#define ACT_BACKWARD_GROUND_KB         0x00020462
#define ACT_FORWARD_GROUND_KB          0x00020463
#define ACT_SOFT_BACKWARD_GROUND_KB    0x00020464
#define ACT_SOFT_FORWARD_GROUND_KB     0x00020465
#define ACT_GROUND_BONK                0x00020466
#define ACT_DEATH_EXIT_LAND            0x00020467
#define ACT_JUMP_LAND                  0x04000470
#define ACT_FREEFALL_LAND              0x04000471
#define ACT_DOUBLE_JUMP_LAND           0x04000472
#define ACT_SIDE_FLIP_LAND             0x04000473
#define ACT_HOLD_JUMP_LAND             0x00000474
#define ACT_HOLD_FREEFALL_LAND         0x00000475
#define ACT_QUICKSAND_JUMP_LAND        0x00000476
#define ACT_HOLD_QUICKSAND_JUMP_LAND   0x00000477
#define ACT_TRIPLE_JUMP_LAND           0x04000478
#define ACT_LONG_JUMP_LAND             0x00000479
#define ACT_BACKFLIP_LAND              0x0400047A

#define ACT_JUMP                       0x03000880
#define ACT_DOUBLE_JUMP                0x03000881
#define ACT_TRIPLE_JUMP                0x01000882
#define ACT_BACKFLIP                   0x01000883
#define ACT_STEEP_JUMP                 0x03000885
#define ACT_WALL_KICK_AIR              0x03000886
#define ACT_SIDE_FLIP                  0x01000887
#define ACT_LONG_JUMP                  0x03000888
#define ACT_WATER_JUMP                 0x01000889
#define ACT_DIVE                       0x0188088A
#define ACT_FREEFALL                   0x0100088C
#define ACT_TOP_OF_POLE_JUMP           0x0300088D
#define ACT_BUTT_SLIDE_AIR             0x0300088E
#define ACT_FLYING_TRIPLE_JUMP         0x03000894
#define ACT_SHOT_FROM_CANNON           0x00880898
#define ACT_FLYING                     0x10880899
#define ACT_RIDING_SHELL_JUMP          0x0281089A
#define ACT_RIDING_SHELL_FALL          0x0081089B
#define ACT_VERTICAL_WIND              0x1008089C
#define ACT_HOLD_JUMP                  0x030008A0
#define ACT_HOLD_FREEFALL              0x010008A1
#define ACT_HOLD_BUTT_SLIDE_AIR        0x010008A2
#define ACT_HOLD_WATER_JUMP            0x010008A3
#define ACT_TWIRLING                   0x108008A4
#define ACT_FORWARD_ROLLOUT            0x010008A6
#define ACT_AIR_HIT_WALL               0x000008A7
#define ACT_RIDING_HOOT                0x000004A8
#define ACT_GROUND_POUND               0x008008A9
#define ACT_SLIDE_KICK                 0x018008AA
#define ACT_AIR_THROW                  0x830008AB
#define ACT_JUMP_KICK                  0x018008AC
#define ACT_BACKWARD_ROLLOUT           0x010008AD
#define ACT_CRAZY_BOX_BOUNCE           0x000008AE
#define ACT_SPECIAL_TRIPLE_JUMP        0x030008AF
#define ACT_BACKWARD_AIR_KB            0x010208B0
#define ACT_FORWARD_AIR_KB             0x010208B1
#define ACT_HARD_FORWARD_AIR_KB        0x010208B2
#define ACT_HARD_BACKWARD_AIR_KB       0x010208B3
#define ACT_BURNING_JUMP               0x010208B4
#define ACT_BURNING_FALL               0x010208B5
#define ACT_SOFT_BONK                  0x010208B6
#define ACT_LAVA_BOOST                 0x010208B7
#define ACT_GETTING_BLOWN              0x010208B8
#define ACT_THROWN_FORWARD             0x010208BD
#define ACT_THROWN_BACKWARD            0x010208BE

#define ACT_WATER_IDLE                 0x380022C0
#define ACT_HOLD_WATER_IDLE            0x380022C1
#define ACT_WATER_ACTION_END           0x300022C2
#define ACT_HOLD_WATER_ACTION_END      0x300022C3
#define ACT_DROWNING                   0x300032C4
#define ACT_BACKWARD_WATER_KB          0x300222C5
#define ACT_FORWARD_WATER_KB           0x300222C6
#define ACT_WATER_DEATH                0x300032C7
#define ACT_WATER_SHOCKED              0x300222C8
#define ACT_BREASTSTROKE               0x300024D0
#define ACT_SWIMMING_END               0x300024D1
#define ACT_FLUTTER_KICK               0x300024D2
#define ACT_HOLD_BREASTSTROKE          0x300024D3
#define ACT_HOLD_SWIMMING_END          0x300024D4
#define ACT_HOLD_FLUTTER_KICK          0x300024D5
#define ACT_WATER_SHELL_SWIMMING       0x300024D6
#define ACT_WATER_THROW                0x300024E0
#define ACT_WATER_PUNCH                0x300024E1
#define ACT_WATER_PLUNGE               0x300022E2
#define ACT_CAUGHT_IN_WHIRLPOOL        0x300222E3
#define ACT_METAL_WATER_STANDING       0x080042F0
#define ACT_HOLD_METAL_WATER_STANDING  0x080042F1
#define ACT_METAL_WATER_WALKING        0x000044F2
#define ACT_HOLD_METAL_WATER_WALKING   0x000044F3
#define ACT_METAL_WATER_FALLING        0x000042F4
#define ACT_HOLD_METAL_WATER_FALLING   0x000042F5
#define ACT_METAL_WATER_FALL_LAND      0x000042F6
#define ACT_HOLD_METAL_WATER_FALL_LAND 0x000042F7
#define ACT_METAL_WATER_JUMP           0x000044F8
#define ACT_HOLD_METAL_WATER_JUMP      0x000044F9
#define ACT_METAL_WATER_JUMP_LAND      0x000044FA
#define ACT_HOLD_METAL_WATER_JUMP_LAND 0x000044FB

#define ACT_DISAPPEARED                0x00001300
#define ACT_INTRO_CUTSCENE             0x04001301
#define ACT_STAR_DANCE_EXIT            0x00001302
#define ACT_STAR_DANCE_WATER           0x00001303
#define ACT_FALL_AFTER_STAR_GRAB       0x00001904
#define ACT_READING_AUTOMATIC_DIALOG   0x20001305
#define ACT_READING_NPC_DIALOG         0x20001306
#define ACT_STAR_DANCE_NO_EXIT         0x00001307
#define ACT_READING_SIGN               0x00001308
#define ACT_JUMBO_STAR_CUTSCENE        0x00001909
#define ACT_WAITING_FOR_DIALOG         0x0000130A
#define ACT_DEBUG_FREE_MOVE            0x0000130F
#define ACT_STANDING_DEATH             0x00021311
#define ACT_QUICKSAND_DEATH            0x00021312
#define ACT_ELECTROCUTION              0x00021313
#define ACT_SUFFOCATION                0x00021314
#define ACT_DEATH_ON_STOMACH           0x00021315
#define ACT_DEATH_ON_BACK              0x00021316
#define ACT_EATEN_BY_BUBBA             0x00021317
#define ACT_END_PEACH_CUTSCENE         0x00001918
#define ACT_CREDITS_CUTSCENE           0x00001319
#define ACT_END_WAVING_CUTSCENE        0x0000131A
#define ACT_PULLING_DOOR               0x00001320
#define ACT_PUSHING_DOOR               0x00001321
#define ACT_WARP_DOOR_SPAWN            0x00001322
#define ACT_EMERGE_FROM_PIPE           0x00001923
#define ACT_SPAWN_SPIN_AIRBORNE        0x00001924
#define ACT_SPAWN_SPIN_LANDING         0x00001325
#define ACT_EXIT_AIRBORNE              0x00001926
#define ACT_EXIT_LAND_SAVE_DIALOG      0x00001327
#define ACT_DEATH_EXIT                 0x00001928
#define ACT_UNUSED_DEATH_EXIT          0x00001929
#define ACT_FALLING_DEATH_EXIT         0x0000192A
#define ACT_SPECIAL_EXIT_AIRBORNE      0x0000192B
#define ACT_SPECIAL_DEATH_EXIT         0x0000192C
#define ACT_FALLING_EXIT_AIRBORNE      0x0000192D
#define ACT_UNLOCKING_KEY_DOOR         0x0000132E
#define ACT_UNLOCKING_STAR_DOOR        0x0000132F
#define ACT_ENTERING_STAR_DOOR         0x00001331
#define ACT_SPAWN_NO_SPIN_AIRBORNE     0x00001932
#define ACT_SPAWN_NO_SPIN_LANDING      0x00001333
#define ACT_BBH_ENTER_JUMP             0x00001934
#define ACT_BBH_ENTER_SPIN             0x00001535
#define ACT_TELEPORT_FADE_OUT          0x00001336
#define ACT_TELEPORT_FADE_IN           0x00001337
#define ACT_SHOCKED                    0x00020338
#define ACT_SQUISHED                   0x00020339
#define ACT_HEAD_STUCK_IN_GROUND       0x0002033A
#define ACT_BUTT_STUCK_IN_GROUND       0x0002033B
#define ACT_FEET_STUCK_IN_GROUND       0x0002033C
#define ACT_PUTTING_ON_CAP             0x0000133D

#define ACT_HOLDING_POLE               0x08100340
#define ACT_GRAB_POLE_SLOW             0x00100341
#define ACT_GRAB_POLE_FAST             0x00100342
#define ACT_CLIMBING_POLE              0x00100343
#define ACT_TOP_OF_POLE_TRANSITION     0x00100344
#define ACT_TOP_OF_POLE                0x00100345
#define ACT_START_HANGING              0x08200348
#define ACT_HANGING                    0x00200349
#define ACT_HANG_MOVING                0x0020054A
#define ACT_LEDGE_GRAB                 0x0800034B
#define ACT_LEDGE_CLIMB_SLOW_1         0x0000054C
#define ACT_LEDGE_CLIMB_SLOW_2         0x0000054D
#define ACT_LEDGE_CLIMB_DOWN           0x0000054E
#define ACT_LEDGE_CLIMB_FAST           0x0000054F
#define ACT_GRABBED                    0x00020370
#define ACT_IN_CANNON                  0x00001371
#define ACT_TORNADO_TWIRLING           0x10020372

#define ACT_PUNCHING                   0x00800380
#define ACT_PICKING_UP                 0x00000383
#define ACT_DIVE_PICKING_UP            0x00000385
#define ACT_STOMACH_SLIDE_STOP         0x00000386
#define ACT_PLACING_DOWN               0x00000387
#define ACT_THROWING                   0x80000588
#define ACT_HEAVY_THROW                0x80000589
#define ACT_PICKING_UP_BOWSER          0x00000390
#define ACT_HOLDING_BOWSER             0x00000391
#define ACT_RELEASING_BOWSER           0x00000392

#define END_DEMO       (1 << 7)

#define VALID_BUTTONS (A_BUTTON   | B_BUTTON   | Z_TRIG     | START_BUTTON | \
                       U_JPAD     | D_JPAD     | L_JPAD     | R_JPAD       | \
                       L_TRIG     | R_TRIG     |                             \
                       U_CBUTTONS | D_CBUTTONS | L_CBUTTONS | R_CBUTTONS   )

#define C_BUTTONS     (U_CBUTTONS | D_CBUTTONS | L_CBUTTONS | R_CBUTTONS   )

#endif
