#ifndef MODEL_IDS_H
#define MODEL_IDS_H

#define ACT_1 (1 << 0)
#define ACT_2 (1 << 1)
#define ACT_3 (1 << 2)
#define ACT_4 (1 << 3)
#define ACT_5 (1 << 4)
#define ACT_6 (1 << 5)

#define ALL_ACTS_MACRO ACT_1 | ACT_2 | ACT_3 | ACT_4 | ACT_5
#define ALL_ACTS       ACT_1 | ACT_2 | ACT_3 | ACT_4 | ACT_5 | ACT_6

#define MODEL_NONE                        0x00

#define MODEL_MARIO                       0x01
#define MODEL_LUIGI                       0x02

#define MODEL_LEVEL_GEOMETRY_03                0x03
#define MODEL_LEVEL_GEOMETRY_04                0x04
#define MODEL_LEVEL_GEOMETRY_05                0x05
#define MODEL_LEVEL_GEOMETRY_06                0x06
#define MODEL_LEVEL_GEOMETRY_07                0x07
#define MODEL_LEVEL_GEOMETRY_08                0x08
#define MODEL_LEVEL_GEOMETRY_09                0x09
#define MODEL_LEVEL_GEOMETRY_0A                0x0A
#define MODEL_LEVEL_GEOMETRY_0B                0x0B
#define MODEL_LEVEL_GEOMETRY_0C                0x0C
#define MODEL_LEVEL_GEOMETRY_0D                0x0D
#define MODEL_LEVEL_GEOMETRY_0E                0x0E
#define MODEL_LEVEL_GEOMETRY_0F                0x0F
#define MODEL_LEVEL_GEOMETRY_10                0x10
#define MODEL_LEVEL_GEOMETRY_11                0x11
#define MODEL_LEVEL_GEOMETRY_12                0x12
#define MODEL_LEVEL_GEOMETRY_13                0x13
#define MODEL_LEVEL_GEOMETRY_14                0x14
#define MODEL_LEVEL_GEOMETRY_15                0x15
#define MODEL_LEVEL_GEOMETRY_16                0x16

#define MODEL_BOB_BUBBLY_TREE                  0x17
#define MODEL_WDW_BUBBLY_TREE                  0x17
#define MODEL_CASTLE_GROUNDS_BUBBLY_TREE       0x17
#define MODEL_WF_BUBBLY_TREE                   0x17
#define MODEL_THI_BUBBLY_TREE                  0x17
#define MODEL_COURTYARD_SPIKY_TREE             0x18
#define MODEL_CCM_SNOW_TREE                    0x19
#define MODEL_SL_SNOW_TREE                     0x19
#define MODEL_UNKNOWN_TREE_1A                  0x1A
#define MODEL_SSL_PALM_TREE                    0x1B
#define MODEL_CASTLE_CASTLE_DOOR_UNUSED        0x1C
#define MODEL_CASTLE_WOODEN_DOOR_UNUSED        0x1D
#define MODEL_BBH_HAUNTED_DOOR                 0x1D
#define MODEL_HMC_WOODEN_DOOR                  0x1D
#define MODEL_UNKNOWN_DOOR_1E                  0x1E
#define MODEL_HMC_METAL_DOOR                   0x1F
#define MODEL_HMC_HAZY_MAZE_DOOR               0x20
#define MODEL_UNKNOWN_DOOR_21                  0x21
#define MODEL_CASTLE_DOOR_0_STARS              0x22
#define MODEL_CASTLE_DOOR_1_STAR               0x23
#define MODEL_CASTLE_DOOR_3_STARS              0x24
#define MODEL_CASTLE_KEY_DOOR                  0x25
#define MODEL_CASTLE_CASTLE_DOOR               0x26
#define MODEL_CASTLE_GROUNDS_CASTLE_DOOR       0x26
#define MODEL_CASTLE_WOODEN_DOOR               0x27
#define MODEL_COURTYARD_WOODEN_DOOR            0x27
#define MODEL_CCM_CABIN_DOOR                   0x27
#define MODEL_UNKNOWN_DOOR_28                  0x28
#define MODEL_CASTLE_METAL_DOOR                0x29
#define MODEL_CASTLE_GROUNDS_METAL_DOOR        0x29
#define MODEL_UNKNOWN_DOOR_2A                  0x2A
#define MODEL_UNKNOWN_DOOR_2B                  0x2B
#define MODEL_WF_TOWER_TRAPEZOID_PLATORM       0x2C
#define MODEL_WF_TOWER_SQUARE_PLATORM          0x2D
#define MODEL_WF_TOWER_SQUARE_PLATORM_UNUSED   0x2E
#define MODEL_WF_TOWER_SQUARE_PLATORM_ELEVATOR 0x2F

#define MODEL_BBH_STAIRCASE_STEP               0x35
#define MODEL_BBH_TILTING_FLOOR_PLATFORM       0x36
#define MODEL_BBH_TUMBLING_PLATFORM            0x37
#define MODEL_BBH_TUMBLING_PLATFORM_PART       0x38
#define MODEL_BBH_MOVING_BOOKSHELF             0x39
#define MODEL_BBH_MESH_ELEVATOR                0x3A
#define MODEL_BBH_MERRY_GO_ROUND               0x3B
#define MODEL_BBH_WOODEN_TOMB                  0x3C

#define MODEL_CCM_ROPEWAY_LIFT                 0x36
#define MODEL_CCM_SNOWMAN_HEAD                 0x37

#define MODEL_CASTLE_BOWSER_TRAP               0x35
#define MODEL_CASTLE_WATER_LEVEL_PILLAR        0x36
#define MODEL_CASTLE_CLOCK_MINUTE_HAND         0x37
#define MODEL_CASTLE_CLOCK_HOUR_HAND           0x38
#define MODEL_CASTLE_CLOCK_PENDULUM            0x39

#define MODEL_HMC_METAL_PLATFORM               0x36
#define MODEL_HMC_METAL_ARROW_PLATFORM         0x37
#define MODEL_HMC_ELEVATOR_PLATFORM            0x38
#define MODEL_HMC_ROLLING_ROCK                 0x39
#define MODEL_HMC_ROCK_PIECE                   0x3A
#define MODEL_HMC_ROCK_SMALL_PIECE             0x3B
#define MODEL_HMC_RED_GRILLS                   0x3C

#define MODEL_SSL_PYRAMID_TOP                  0x3A
#define MODEL_SSL_GRINDEL                      0x36
#define MODEL_SSL_SPINDEL                      0x37
#define MODEL_SSL_MOVING_PYRAMID_WALL          0x38
#define MODEL_SSL_PYRAMID_ELEVATOR             0x39

#define MODEL_BOB_CHAIN_CHOMP_GATE             0x36
#define MODEL_BOB_SEESAW_PLATFORM              0x37
#define MODEL_BOB_BARS_GRILLS                  0x38

#define MODEL_SL_SNOW_TRIANGLE                 0x36
#define MODEL_SL_CRACKED_ICE                   0x37
#define MODEL_SL_CRACKED_ICE_CHUNK             0x38

#define MODEL_WDW_SQUARE_FLOATING_PLATFORM        0x36
#define MODEL_WDW_ARROW_LIFT                      0x37
#define MODEL_WDW_WATER_LEVEL_DIAMOND             0x38
#define MODEL_WDW_HIDDEN_PLATFORM                 0x39
#define MODEL_WDW_EXPRESS_ELEVATOR                0x3A
#define MODEL_WDW_RECTANGULAR_FLOATING_PLATFORM   0x3B
#define MODEL_WDW_ROTATING_PLATFORM               0x3C

#define MODEL_JRB_SHIP_LEFT_HALF_PART             0x35
#define MODEL_JRB_SHIP_BACK_LEFT_PART             0x36
#define MODEL_JRB_SHIP_RIGHT_HALF_PART            0x37
#define MODEL_JRB_SHIP_BACK_RIGHT_PART            0x38
#define MODEL_JRB_SUNKEN_SHIP                     0x39
#define MODEL_JRB_SUNKEN_SHIP_BACK                0x3A
#define MODEL_JRB_ROCK                            0x3B
#define MODEL_JRB_SLIDING_BOX                     0x3C
#define MODEL_JRB_FALLING_PILLAR                  0x3D
#define MODEL_JRB_FALLING_PILLAR_BASE             0x3E
#define MODEL_JRB_FLOATING_PLATFORM               0x3F

#define MODEL_THI_HUGE_ISLAND_TOP                 0x36
#define MODEL_THI_TINY_ISLAND_TOP                 0x37

#define MODEL_TTC_ROTATING_CUBE                   0x36
#define MODEL_TTC_ROTATING_PRISM                  0x37
#define MODEL_TTC_PENDULUM                        0x38
#define MODEL_TTC_LARGE_TREADMILL                 0x39
#define MODEL_TTC_SMALL_TREADMILL                 0x3A
#define MODEL_TTC_PUSH_BLOCK                      0x3B
#define MODEL_TTC_ROTATING_HEXAGON                0x3C
#define MODEL_TTC_ROTATING_TRIANGLE               0x3D
#define MODEL_TTC_PIT_BLOCK                       0x3E
#define MODEL_TTC_PIT_BLOCK_UNUSED                0x3F
#define MODEL_TTC_ELEVATOR_PLATFORM               0x40
#define MODEL_TTC_CLOCK_HAND                      0x41
#define MODEL_TTC_SPINNER                         0x42
#define MODEL_TTC_SMALL_GEAR                      0x43
#define MODEL_TTC_LARGE_GEAR                      0x44

#define MODEL_RR_SLIDING_PLATFORM                 0x36
#define MODEL_RR_FLYING_CARPET                    0x37
#define MODEL_RR_OCTAGONAL_PLATFORM               0x38
#define MODEL_RR_ROTATING_BRIDGE_PLATFORM         0x39
#define MODEL_RR_TRIANGLE_PLATFORM                0x3A
#define MODEL_RR_CRUISER_WING                     0x3B
#define MODEL_RR_SEESAW_PLATFORM                  0x3C
#define MODEL_RR_L_SHAPED_PLATFORM                0x3D
#define MODEL_RR_SWINGING_PLATFORM                0x3E
#define MODEL_RR_DONUT_PLATFORM                   0x3F
#define MODEL_RR_ELEVATOR_PLATFORM                0x40
#define MODEL_RR_TRICKY_TRIANGLES                 0x41
#define MODEL_RR_TRICKY_TRIANGLES_FRAME1          0x42
#define MODEL_RR_TRICKY_TRIANGLES_FRAME2          0x43
#define MODEL_RR_TRICKY_TRIANGLES_FRAME3          0x44
#define MODEL_RR_TRICKY_TRIANGLES_FRAME4          0x45

#define MODEL_BITDW_SQUARE_PLATFORM               0x36
#define MODEL_BITDW_SEESAW_PLATFORM               0x37
#define MODEL_BITDW_SLIDING_PLATFORM              0x38
#define MODEL_BITDW_FERRIS_WHEEL_AXLE             0x39
#define MODEL_BITDW_BLUE_PLATFORM                 0x3A
#define MODEL_BITDW_STAIRCASE_FRAME4              0x3B
#define MODEL_BITDW_STAIRCASE_FRAME3              0x3C
#define MODEL_BITDW_STAIRCASE_FRAME2              0x3D
#define MODEL_BITDW_STAIRCASE_FRAME1              0x3E
#define MODEL_BITDW_STAIRCASE                     0x3F

#define MODEL_VCUTM_SEESAW_PLATFORM               0x36
#define MODEL_VCUTM_CHECKERBOARD_PLATFORM_SPAWNER 0x37

#define MODEL_BITFS_PLATFORM_ON_TRACK             0x36
#define MODEL_BITFS_TILTING_SQUARE_PLATFORM       0x37
#define MODEL_BITFS_SINKING_PLATFORMS             0x38
#define MODEL_BITFS_BLUE_POLE                     0x39
#define MODEL_BITFS_SINKING_CAGE_PLATFORM         0x3A
#define MODEL_BITFS_ELEVATOR                      0x3B
#define MODEL_BITFS_STRETCHING_PLATFORMS          0x3C
#define MODEL_BITFS_SEESAW_PLATFORM               0x3D
#define MODEL_BITFS_MOVING_SQUARE_PLATFORM        0x3E
#define MODEL_BITFS_SLIDING_PLATFORM              0x3F
#define MODEL_BITFS_TUMBLING_PLATFORM_PART        0x40
#define MODEL_BITFS_TUMBLING_PLATFORM             0x41

#define MODEL_BITS_SLIDING_PLATFORM               0x36
#define MODEL_BITS_TWIN_SLIDING_PLATFORMS         0x37
#define MODEL_BITS_OCTAGONAL_PLATFORM             0x39
#define MODEL_BITS_BLUE_PLATFORM                  0x3C
#define MODEL_BITS_FERRIS_WHEEL_AXLE              0x3D
#define MODEL_BITS_ARROW_PLATFORM                 0x3E
#define MODEL_BITS_SEESAW_PLATFORM                0x3F
#define MODEL_BITS_TILTING_W_PLATFORM             0x40
#define MODEL_BITS_STAIRCASE                      0x41
#define MODEL_BITS_STAIRCASE_FRAME1               0x42
#define MODEL_BITS_STAIRCASE_FRAME2               0x43
#define MODEL_BITS_STAIRCASE_FRAME3               0x44
#define MODEL_BITS_STAIRCASE_FRAME4               0x45
#define MODEL_BITS_WARP_PIPE                      0x49

#define MODEL_LLL_DRAWBRIDGE_PART                 0x38
#define MODEL_LLL_ROTATING_BLOCK_FIRE_BARS        0x3A
#define MODEL_LLL_ROTATING_HEXAGONAL_RING         0x3E
#define MODEL_LLL_SINKING_RECTANGULAR_PLATFORM    0x3F
#define MODEL_LLL_SINKING_SQUARE_PLATFORMS        0x40
#define MODEL_LLL_TILTING_SQUARE_PLATFORM         0x41
#define MODEL_LLL_BOWSER_PIECE_1                  0x43
#define MODEL_LLL_BOWSER_PIECE_2                  0x44
#define MODEL_LLL_BOWSER_PIECE_3                  0x45
#define MODEL_LLL_BOWSER_PIECE_4                  0x46
#define MODEL_LLL_BOWSER_PIECE_5                  0x47
#define MODEL_LLL_BOWSER_PIECE_6                  0x48
#define MODEL_LLL_BOWSER_PIECE_7                  0x49
#define MODEL_LLL_BOWSER_PIECE_8                  0x4A
#define MODEL_LLL_BOWSER_PIECE_9                  0x4B
#define MODEL_LLL_BOWSER_PIECE_10                 0x4C
#define MODEL_LLL_BOWSER_PIECE_11                 0x4D
#define MODEL_LLL_BOWSER_PIECE_12                 0x4E
#define MODEL_LLL_BOWSER_PIECE_13                 0x4F
#define MODEL_LLL_BOWSER_PIECE_14                 0x50
#define MODEL_LLL_MOVING_OCTAGONAL_MESH_PLATFORM  0x36
#define MODEL_LLL_SINKING_ROCK_BLOCK              0x37
#define MODEL_LLL_ROLLING_LOG                     0x39
#define MODEL_LLL_WOOD_BRIDGE                     0x35
#define MODEL_LLL_LARGE_WOOD_BRIDGE               0x3B
#define MODEL_LLL_FALLING_PLATFORM                0x3C
#define MODEL_LLL_LARGE_FALLING_PLATFORM          0x3D
#define MODEL_LLL_VOLCANO_FALLING_TRAP            0x53

#define MODEL_DDD_BOWSER_SUB_DOOR                 0x36
#define MODEL_DDD_BOWSER_SUB                      0x37
#define MODEL_DDD_POLE                            0x38

#define MODEL_WF_BREAKABLE_WALL_RIGHT             0x36
#define MODEL_WF_BREAKABLE_WALL_LEFT              0x37
#define MODEL_WF_KICKABLE_BOARD                   0x38
#define MODEL_WF_TOWER_DOOR                       0x39
#define MODEL_WF_KICKABLE_BOARD_FELLED            0x3A

#define MODEL_CASTLE_GROUNDS_VCUTM_GRILL          0x36
#define MODEL_CASTLE_GROUNDS_FLAG                 0x37
#define MODEL_CASTLE_GROUNDS_CANNON_GRILL         0x38

#define MODEL_BOWSER_2_TILTING_ARENA              0x36

#define MODEL_BOWSER_3_FALLING_PLATFORM_1         0x36
#define MODEL_BOWSER_3_FALLING_PLATFORM_2         0x37
#define MODEL_BOWSER_3_FALLING_PLATFORM_3         0x38
#define MODEL_BOWSER_3_FALLING_PLATFORM_4         0x39
#define MODEL_BOWSER_3_FALLING_PLATFORM_5         0x3A
#define MODEL_BOWSER_3_FALLING_PLATFORM_6         0x3B
#define MODEL_BOWSER_3_FALLING_PLATFORM_7         0x3C
#define MODEL_BOWSER_3_FALLING_PLATFORM_8         0x3D
#define MODEL_BOWSER_3_FALLING_PLATFORM_9         0x3E
#define MODEL_BOWSER_3_FALLING_PLATFORM_10        0x3F

#define MODEL_TTM_ROLLING_LOG                     0x35
#define MODEL_TTM_STAR_CAGE                       0x36
#define MODEL_TTM_BLUE_SMILEY                     0x37
#define MODEL_TTM_YELLOW_SMILEY                   0x38
#define MODEL_TTM_STAR_SMILEY                     0x39
#define MODEL_TTM_MOON_SMILEY                     0x3A

#define MODEL_BULLET_BILL                 0x54
#define MODEL_YELLOW_SPHERE               0x55
#define MODEL_HOOT                        0x56
#define MODEL_YOSHI_EGG                   0x57
#define MODEL_THWOMP                      0x58
#define MODEL_HEAVE_HO                    0x59

#define MODEL_BLARGG                      0x54
#define MODEL_BULLY                       0x56
#define MODEL_BULLY_BOSS                  0x57

#define MODEL_WATER_BOMB                  0x54
#define MODEL_WATER_BOMB_SHADOW           0x55
#define MODEL_KING_BOBOMB                 0x56

#define MODEL_MANTA_RAY                   0x54
#define MODEL_UNAGI                       0x55
#define MODEL_SUSHI                       0x56
#define MODEL_DL_WHIRLPOOL                0x57
#define MODEL_CLAM_SHELL                  0x58

#define MODEL_POKEY_HEAD                  0x54
#define MODEL_POKEY_BODY_PART             0x55
#define MODEL_TWEESTER                    0x56
#define MODEL_KLEPTO                      0x57
#define MODEL_EYEROK_LEFT_HAND            0x58
#define MODEL_EYEROK_RIGHT_HAND           0x59

#define MODEL_DL_MONTY_MOLE_HOLE          0x54
#define MODEL_MONTY_MOLE                  0x55
#define MODEL_UKIKI                       0x56
#define MODEL_FWOOSH                      0x57

#define MODEL_SPINDRIFT                   0x54
#define MODEL_MR_BLIZZARD_HIDDEN          0x55
#define MODEL_MR_BLIZZARD                 0x56
#define MODEL_PENGUIN                     0x57

#define MODEL_CAP_SWITCH_EXCLAMATION      0x54
#define MODEL_CAP_SWITCH                  0x55
#define MODEL_CAP_SWITCH_BASE             0x56

#define MODEL_BOO                         0x54
#define MODEL_BETA_BOO_KEY                0x55
#define MODEL_HAUNTED_CHAIR               0x56
#define MODEL_MAD_PIANO                   0x57
#define MODEL_BOOKEND_PART                0x58
#define MODEL_BOOKEND                     0x59
#define MODEL_HAUNTED_CAGE                0x5A

#define MODEL_BIRDS                       0x54
#define MODEL_YOSHI                       0x55

#define MODEL_ENEMY_LAKITU                0x54
#define MODEL_SPINY_BALL                  0x55
#define MODEL_SPINY                       0x56
#define MODEL_WIGGLER_HEAD                0x57
#define MODEL_WIGGLER_BODY                0x58
#define MODEL_BUBBA                       0x59

#define MODEL_UNKNOWN_54                  0x54
#define MODEL_UNKNOWN_58                  0x58

#define MODEL_BOWSER                      0x64
#define MODEL_BOWSER_BOMB_CHILD_OBJ       0x65
#define MODEL_BOWSER_SMOKE                0x66
#define MODEL_BOWSER_FLAMES               0x67
#define MODEL_BOWSER_WAVE                 0x68
#define MODEL_BOWSER_NO_SHADOW            0x69

#define MODEL_BUB                         0x64
#define MODEL_TREASURE_CHEST_BASE         0x65
#define MODEL_TREASURE_CHEST_LID          0x66
#define MODEL_CYAN_FISH                   0x67
#define MODEL_WATER_RING                  0x68
#define MODEL_SKEETER                     0x69

#define MODEL_PIRANHA_PLANT               0x64
#define MODEL_WHOMP                       0x67
#define MODEL_KOOPA_WITH_SHELL            0x68
#define MODEL_METALLIC_BALL               0x65
#define MODEL_CHAIN_CHOMP                 0x66
#define MODEL_KOOPA_FLAG                  0x6A
#define MODEL_WOODEN_POST                 0x6B

#define MODEL_MIPS                        0x64
#define MODEL_BOO_CASTLE                  0x65
#define MODEL_LAKITU                      0x66

#define MODEL_CHILL_BULLY                 0x64
#define MODEL_BIG_CHILL_BULLY             0x65
#define MODEL_MONEYBAG                    0x66

#define MODEL_SWOOP                       0x64
#define MODEL_SCUTTLEBUG                  0x65
#define MODEL_MR_I_IRIS                   0x66
#define MODEL_MR_I                        0x67
#define MODEL_DORRIE                      0x68

#define MODEL_YELLOW_COIN                 0x74
#define MODEL_YELLOW_COIN_NO_SHADOW       0x75
#define MODEL_BLUE_COIN                   0x76
#define MODEL_BLUE_COIN_NO_SHADOW         0x77
#define MODEL_HEART                       0x78
#define MODEL_TRANSPARENT_STAR            0x79
#define MODEL_STAR                        0x7A
#define MODEL_TTM_SLIDE_EXIT_PODIUM       0x7B
#define MODEL_WOODEN_SIGNPOST             0x7C
#define MODEL_UNKNOWN_7D                  0x7D

#define MODEL_CANNON_BARREL               0x7F
#define MODEL_CANNON_BASE                 0x80
#define MODEL_BREAKABLE_BOX               0x81
#define MODEL_BREAKABLE_BOX_SMALL         0x82
#define MODEL_EXCLAMATION_BOX_OUTLINE     0x83
#define MODEL_EXCLAMATION_POINT           0x84
#define MODEL_MARIOS_WINGED_METAL_CAP     0x85
#define MODEL_MARIOS_METAL_CAP            0x86
#define MODEL_MARIOS_WING_CAP             0x87
#define MODEL_MARIOS_CAP                  0x88
#define MODEL_EXCLAMATION_BOX             0x89
#define MODEL_DIRT_ANIMATION              0x8A
#define MODEL_CARTOON_STAR                0x8B
#define MODEL_BLUE_COIN_SWITCH            0x8C

#define MODEL_MIST                        0x8E
#define MODEL_SPARKLES_ANIMATION          0x8F
#define MODEL_RED_FLAME                   0x90
#define MODEL_BLUE_FLAME                  0x91

#define MODEL_BURN_SMOKE                  0x94
#define MODEL_SPARKLES                    0x95
#define MODEL_SMOKE                       0x96

#define MODEL_BURN_SMOKE_UNUSED           0x9C

#define MODEL_WHITE_PARTICLE_DL           0x9E
#define MODEL_SAND_DUST                   0x9F
#define MODEL_WHITE_PARTICLE              0xA0
#define MODEL_PEBBLE                      0xA1
#define MODEL_LEAVES                      0xA2
#define MODEL_WAVE_TRAIL                  0xA3
#define MODEL_WHITE_PARTICLE_SMALL        0xA4
#define MODEL_SMALL_WATER_SPLASH          0xA5
#define MODEL_IDLE_WATER_WAVE             0xA6
#define MODEL_WATER_SPLASH                0xA7
#define MODEL_BUBBLE                      0xA8

#define MODEL_PURPLE_MARBLE               0xAA

#define MODEL_UNKNOWN_AC                  0xAC
#define MODEL_WF_SLIDING_PLATFORM         0xAD
#define MODEL_WF_SMALL_BOMP               0xAE
#define MODEL_WF_ROTATING_WOODEN_PLATFORM 0xAF
#define MODEL_WF_TUMBLING_BRIDGE_PART     0xB0
#define MODEL_WF_LARGE_BOMP               0xB1
#define MODEL_WF_TUMBLING_BRIDGE          0xB2
#define MODEL_BOWSER_BOMB                 0xB3
#define MODEL_WATER_MINE                  0xB3
#define MODEL_BOWLING_BALL                0xB4
#define MODEL_TRAMPOLINE                  0xB5
#define MODEL_TRAMPOLINE_CENTER           0xB6
#define MODEL_TRAMPOLINE_BASE             0xB7
#define MODEL_UNKNOWN_B8                  0xB8
#define MODEL_FISH                        0xB9
#define MODEL_FISH_SHADOW                 0xBA
#define MODEL_BUTTERFLY                   0xBB
#define MODEL_BLACK_BOBOMB                0xBC

#define MODEL_KOOPA_SHELL                 0xBE
#define MODEL_KOOPA_WITHOUT_SHELL         0xBF
#define MODEL_GOOMBA                      0xC0
#define MODEL_SEAWEED                     0xC1
#define MODEL_AMP                         0xC2
#define MODEL_BOBOMB_BUDDY                0xC3

#define MODEL_SSL_TOX_BOX                 0xC7
#define MODEL_BOWSER_KEY_CUTSCENE         0xC8
#define MODEL_DL_CANNON_LID               0xC9
#define MODEL_CHECKERBOARD_PLATFORM       0xCA
#define MODEL_RED_FLAME_SHADOW            0xCB
#define MODEL_BOWSER_KEY                  0xCC
#define MODEL_EXPLOSION                   0xCD
#define MODEL_SNUFIT                      0xCE
#define MODEL_PURPLE_SWITCH               0xCF
#define MODEL_CASTLE_STAR_DOOR_30_STARS   0xD0
#define MODEL_CASTLE_STAR_DOOR_50_STARS   0xD1
#define MODEL_CCM_SNOWMAN_BASE            0xD2

#define MODEL_1UP                         0xD4
#define MODEL_CASTLE_STAR_DOOR_8_STARS    0xD5
#define MODEL_CASTLE_STAR_DOOR_70_STARS   0xD6
#define MODEL_RED_COIN                    0xD7
#define MODEL_RED_COIN_NO_SHADOW          0xD8
#define MODEL_METAL_BOX                   0xD9
#define MODEL_METAL_BOX_DL                0xDA
#define MODEL_NUMBER                      0xDB
#define MODEL_FLYGUY                      0xDC
#define MODEL_TOAD                        0xDD
#define MODEL_PEACH                       0xDE
#define MODEL_CHUCKYA                     0xDF
#define MODEL_WHITE_PUFF                  0xE0
#define MODEL_TRAJECTORY_MARKER_BALL      0xE1

#define MODEL_MAIN_MENU_MARIO_SAVE_BUTTON         MODEL_LEVEL_GEOMETRY_03
#define MODEL_MAIN_MENU_RED_ERASE_BUTTON          MODEL_LEVEL_GEOMETRY_04
#define MODEL_MAIN_MENU_BLUE_COPY_BUTTON          MODEL_LEVEL_GEOMETRY_05
#define MODEL_MAIN_MENU_YELLOW_FILE_BUTTON        MODEL_LEVEL_GEOMETRY_06
#define MODEL_MAIN_MENU_GREEN_SCORE_BUTTON        MODEL_LEVEL_GEOMETRY_07
#define MODEL_MAIN_MENU_MARIO_SAVE_BUTTON_FADE    MODEL_LEVEL_GEOMETRY_08
#define MODEL_MAIN_MENU_MARIO_NEW_BUTTON          MODEL_LEVEL_GEOMETRY_09
#define MODEL_MAIN_MENU_MARIO_NEW_BUTTON_FADE     MODEL_LEVEL_GEOMETRY_0A
#define MODEL_MAIN_MENU_PURPLE_SOUND_BUTTON       MODEL_LEVEL_GEOMETRY_0B
#define MODEL_MAIN_MENU_GENERIC_BUTTON            MODEL_LEVEL_GEOMETRY_0C

#define MODEL_LLL_ROTATING_HEXAGONAL_PLATFORM     MODEL_LEVEL_GEOMETRY_09
#define MODEL_WF_GIANT_POLE                       MODEL_LEVEL_GEOMETRY_0D
#define MODEL_WF_ROTATING_PLATFORM                MODEL_LEVEL_GEOMETRY_10
#define MODEL_BITDW_WARP_PIPE                     MODEL_LEVEL_GEOMETRY_12
#define MODEL_THI_WARP_PIPE                       MODEL_LEVEL_GEOMETRY_16
#define MODEL_VCUTM_WARP_PIPE                     MODEL_LEVEL_GEOMETRY_16
#define MODEL_CASTLE_GROUNDS_WARP_PIPE            MODEL_LEVEL_GEOMETRY_16

#endif
