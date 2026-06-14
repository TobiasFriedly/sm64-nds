#ifndef SURFACE_TERRAINS_H
#define SURFACE_TERRAINS_H

#define SURFACE_DEFAULT                      0x0000
#define SURFACE_BURNING                      0x0001
#define SURFACE_0004                         0x0004
#define SURFACE_HANGABLE                     0x0005
#define SURFACE_SLOW                         0x0009
#define SURFACE_DEATH_PLANE                  0x000A
#define SURFACE_CLOSE_CAMERA                 0x000B
#define SURFACE_WATER                        0x000D
#define SURFACE_FLOWING_WATER                0x000E
#define SURFACE_INTANGIBLE                   0x0012
#define SURFACE_VERY_SLIPPERY                0x0013
#define SURFACE_SLIPPERY                     0x0014
#define SURFACE_NOT_SLIPPERY                 0x0015
#define SURFACE_TTM_VINES                    0x0016
#define SURFACE_MGR_MUSIC                    0x001A
#define SURFACE_INSTANT_WARP_1B              0x001B
#define SURFACE_INSTANT_WARP_1C              0x001C
#define SURFACE_INSTANT_WARP_1D              0x001D
#define SURFACE_INSTANT_WARP_1E              0x001E
#define SURFACE_SHALLOW_QUICKSAND            0x0021
#define SURFACE_DEEP_QUICKSAND               0x0022
#define SURFACE_INSTANT_QUICKSAND            0x0023
#define SURFACE_DEEP_MOVING_QUICKSAND        0x0024
#define SURFACE_SHALLOW_MOVING_QUICKSAND     0x0025
#define SURFACE_QUICKSAND                    0x0026
#define SURFACE_MOVING_QUICKSAND             0x0027
#define SURFACE_WALL_MISC                    0x0028
#define SURFACE_NOISE_DEFAULT                0x0029
#define SURFACE_NOISE_SLIPPERY               0x002A
#define SURFACE_HORIZONTAL_WIND              0x002C
#define SURFACE_INSTANT_MOVING_QUICKSAND     0x002D
#define SURFACE_ICE                          0x002E
#define SURFACE_LOOK_UP_WARP                 0x002F
#define SURFACE_HARD                         0x0030
#define SURFACE_WARP                         0x0032
#define SURFACE_TIMER_START                  0x0033
#define SURFACE_TIMER_END                    0x0034
#define SURFACE_HARD_SLIPPERY                0x0035
#define SURFACE_HARD_VERY_SLIPPERY           0x0036
#define SURFACE_HARD_NOT_SLIPPERY            0x0037
#define SURFACE_VERTICAL_WIND                0x0038
#define SURFACE_BOSS_FIGHT_CAMERA            0x0065
#define SURFACE_CAMERA_FREE_ROAM             0x0066
#define SURFACE_THI3_WALLKICK                0x0068
#define SURFACE_CAMERA_8_DIR                 0x0069
#define SURFACE_CAMERA_MIDDLE                0x006E
#define SURFACE_CAMERA_ROTATE_RIGHT          0x006F
#define SURFACE_CAMERA_ROTATE_LEFT           0x0070
#define SURFACE_CAMERA_BOUNDARY              0x0072
#define SURFACE_NOISE_VERY_SLIPPERY_73       0x0073
#define SURFACE_NOISE_VERY_SLIPPERY_74       0x0074
#define SURFACE_NOISE_VERY_SLIPPERY          0x0075
#define SURFACE_NO_CAM_COLLISION             0x0076
#define SURFACE_NO_CAM_COLLISION_77          0x0077
#define SURFACE_NO_CAM_COL_VERY_SLIPPERY     0x0078
#define SURFACE_NO_CAM_COL_SLIPPERY          0x0079
#define SURFACE_SWITCH                       0x007A
#define SURFACE_VANISH_CAP_WALLS             0x007B
#define SURFACE_PAINTING_WOBBLE_A6           0x00A6
#define SURFACE_PAINTING_WOBBLE_A7           0x00A7
#define SURFACE_PAINTING_WOBBLE_A8           0x00A8
#define SURFACE_PAINTING_WOBBLE_A9           0x00A9
#define SURFACE_PAINTING_WOBBLE_AA           0x00AA
#define SURFACE_PAINTING_WOBBLE_AB           0x00AB
#define SURFACE_PAINTING_WOBBLE_AC           0x00AC
#define SURFACE_PAINTING_WOBBLE_AD           0x00AD
#define SURFACE_PAINTING_WOBBLE_AE           0x00AE
#define SURFACE_PAINTING_WOBBLE_AF           0x00AF
#define SURFACE_PAINTING_WOBBLE_B0           0x00B0
#define SURFACE_PAINTING_WOBBLE_B1           0x00B1
#define SURFACE_PAINTING_WOBBLE_B2           0x00B2
#define SURFACE_PAINTING_WOBBLE_B3           0x00B3
#define SURFACE_PAINTING_WOBBLE_B4           0x00B4
#define SURFACE_PAINTING_WOBBLE_B5           0x00B5
#define SURFACE_PAINTING_WOBBLE_B6           0x00B6
#define SURFACE_PAINTING_WOBBLE_B7           0x00B7
#define SURFACE_PAINTING_WOBBLE_B8           0x00B8
#define SURFACE_PAINTING_WOBBLE_B9           0x00B9
#define SURFACE_PAINTING_WOBBLE_BA           0x00BA
#define SURFACE_PAINTING_WOBBLE_BB           0x00BB
#define SURFACE_PAINTING_WOBBLE_BC           0x00BC
#define SURFACE_PAINTING_WOBBLE_BD           0x00BD
#define SURFACE_PAINTING_WOBBLE_BE           0x00BE
#define SURFACE_PAINTING_WOBBLE_BF           0x00BF
#define SURFACE_PAINTING_WOBBLE_C0           0x00C0
#define SURFACE_PAINTING_WOBBLE_C1           0x00C1
#define SURFACE_PAINTING_WOBBLE_C2           0x00C2
#define SURFACE_PAINTING_WOBBLE_C3           0x00C3
#define SURFACE_PAINTING_WOBBLE_C4           0x00C4
#define SURFACE_PAINTING_WOBBLE_C5           0x00C5
#define SURFACE_PAINTING_WOBBLE_C6           0x00C6
#define SURFACE_PAINTING_WOBBLE_C7           0x00C7
#define SURFACE_PAINTING_WOBBLE_C8           0x00C8
#define SURFACE_PAINTING_WOBBLE_C9           0x00C9
#define SURFACE_PAINTING_WOBBLE_CA           0x00CA
#define SURFACE_PAINTING_WOBBLE_CB           0x00CB
#define SURFACE_PAINTING_WOBBLE_CC           0x00CC
#define SURFACE_PAINTING_WOBBLE_CD           0x00CD
#define SURFACE_PAINTING_WOBBLE_CE           0x00CE
#define SURFACE_PAINTING_WOBBLE_CF           0x00CF
#define SURFACE_PAINTING_WOBBLE_D0           0x00D0
#define SURFACE_PAINTING_WOBBLE_D1           0x00D1
#define SURFACE_PAINTING_WOBBLE_D2           0x00D2
#define SURFACE_PAINTING_WARP_D3             0x00D3
#define SURFACE_PAINTING_WARP_D4             0x00D4
#define SURFACE_PAINTING_WARP_D5             0x00D5
#define SURFACE_PAINTING_WARP_D6             0x00D6
#define SURFACE_PAINTING_WARP_D7             0x00D7
#define SURFACE_PAINTING_WARP_D8             0x00D8
#define SURFACE_PAINTING_WARP_D9             0x00D9
#define SURFACE_PAINTING_WARP_DA             0x00DA
#define SURFACE_PAINTING_WARP_DB             0x00DB
#define SURFACE_PAINTING_WARP_DC             0x00DC
#define SURFACE_PAINTING_WARP_DD             0x00DD
#define SURFACE_PAINTING_WARP_DE             0x00DE
#define SURFACE_PAINTING_WARP_DF             0x00DF
#define SURFACE_PAINTING_WARP_E0             0x00E0
#define SURFACE_PAINTING_WARP_E1             0x00E1
#define SURFACE_PAINTING_WARP_E2             0x00E2
#define SURFACE_PAINTING_WARP_E3             0x00E3
#define SURFACE_PAINTING_WARP_E4             0x00E4
#define SURFACE_PAINTING_WARP_E5             0x00E5
#define SURFACE_PAINTING_WARP_E6             0x00E6
#define SURFACE_PAINTING_WARP_E7             0x00E7
#define SURFACE_PAINTING_WARP_E8             0x00E8
#define SURFACE_PAINTING_WARP_E9             0x00E9
#define SURFACE_PAINTING_WARP_EA             0x00EA
#define SURFACE_PAINTING_WARP_EB             0x00EB
#define SURFACE_PAINTING_WARP_EC             0x00EC
#define SURFACE_PAINTING_WARP_ED             0x00ED
#define SURFACE_PAINTING_WARP_EE             0x00EE
#define SURFACE_PAINTING_WARP_EF             0x00EF
#define SURFACE_PAINTING_WARP_F0             0x00F0
#define SURFACE_PAINTING_WARP_F1             0x00F1
#define SURFACE_PAINTING_WARP_F2             0x00F2
#define SURFACE_PAINTING_WARP_F3             0x00F3
#define SURFACE_TTC_PAINTING_1               0x00F4
#define SURFACE_TTC_PAINTING_2               0x00F5
#define SURFACE_TTC_PAINTING_3               0x00F6
#define SURFACE_PAINTING_WARP_F7             0x00F7
#define SURFACE_PAINTING_WARP_F8             0x00F8
#define SURFACE_PAINTING_WARP_F9             0x00F9
#define SURFACE_PAINTING_WARP_FA             0x00FA
#define SURFACE_PAINTING_WARP_FB             0x00FB
#define SURFACE_PAINTING_WARP_FC             0x00FC
#define SURFACE_WOBBLING_WARP                0x00FD
#define SURFACE_TRAPDOOR                     0x00FF

#define SURFACE_IS_QUICKSAND(cmd)     (cmd >= 0x21 && cmd < 0x28)
#define SURFACE_IS_NOT_HARD(cmd)      (cmd != SURFACE_HARD && \
                                     !(cmd >= 0x35 && cmd <= 0x37))
#define SURFACE_IS_PAINTING_WARP(cmd) (cmd >= 0xD3 && cmd < 0xFD)

#define SURFACE_CLASS_DEFAULT       0x0000
#define SURFACE_CLASS_VERY_SLIPPERY 0x0013
#define SURFACE_CLASS_SLIPPERY      0x0014
#define SURFACE_CLASS_NOT_SLIPPERY  0x0015

#define SURFACE_FLAG_DYNAMIC          (1 << 0)
#define SURFACE_FLAG_NO_CAM_COLLISION (1 << 1)
#define SURFACE_FLAG_X_PROJECTION     (1 << 3)

#define TERRAIN_LOAD_VERTICES    0x0040
#define TERRAIN_LOAD_CONTINUE    0x0041
#define TERRAIN_LOAD_END         0x0042
#define TERRAIN_LOAD_OBJECTS     0x0043
#define TERRAIN_LOAD_ENVIRONMENT 0x0044

#define TERRAIN_LOAD_IS_SURFACE_TYPE_LOW(cmd)  (cmd < 0x40)
#define TERRAIN_LOAD_IS_SURFACE_TYPE_HIGH(cmd) (cmd >= 0x65)

#define TERRAIN_GRASS  0x0000
#define TERRAIN_STONE  0x0001
#define TERRAIN_SNOW   0x0002
#define TERRAIN_SAND   0x0003
#define TERRAIN_SPOOKY 0x0004
#define TERRAIN_WATER  0x0005
#define TERRAIN_SLIDE  0x0006
#define TERRAIN_MASK   0x0007

#define COL_INIT() TERRAIN_LOAD_VERTICES

#define COL_VERTEX_INIT(vtxNum) vtxNum

#define COL_VERTEX(x, y, z) x, y, z

#define COL_TRI_INIT(surfType, triNum) surfType, triNum

#define COL_TRI(v1, v2, v3) v1, v2, v3

#define COL_TRI_SPECIAL(v1, v2, v3, param) v1, v2, v3, param

#define COL_TRI_STOP() TERRAIN_LOAD_CONTINUE

#define COL_END() TERRAIN_LOAD_END

#define COL_SPECIAL_INIT(num) TERRAIN_LOAD_OBJECTS, num

#define COL_WATER_BOX_INIT(num) TERRAIN_LOAD_ENVIRONMENT, num

#define COL_WATER_BOX(id, x1, z1, x2, z2, y) id, x1, z1, x2, z2, y

#endif
