#ifndef CAMERA_H
#define CAMERA_H

#include <PR/ultratypes.h>

#include "types.h"
#include "area.h"
#include "engine/geo_layout.h"
#include "engine/graph_node.h"

#include "level_table.h"

#define ABS(x) ((x) > 0.f ? (x) : -(x))
#define ABS2(x) ((x) >= 0.f ? (x) : -(x))

#define DEGREES(x) ((x) * 0x10000 / 360)

#define LEVEL_AREA_INDEX(levelNum, areaNum) (((levelNum) << 4) + (areaNum))

#define ZOOMOUT_AREA_MASK(level1Area1, level1Area2, level1Area3, level1Area4, \
                          level2Area1, level2Area2, level2Area3, level2Area4) \
    ((level2Area4) << 7 |                                                     \
     (level2Area3) << 6 |                                                     \
     (level2Area2) << 5 |                                                     \
     (level2Area1) << 4 |                                                     \
     (level1Area4) << 3 |                                                     \
     (level1Area3) << 2 |                                                     \
     (level1Area2) << 1 |                                                     \
     (level1Area1) << 0)

#define AREA_BBH                LEVEL_AREA_INDEX(LEVEL_BBH, 1)
#define AREA_CCM_OUTSIDE        LEVEL_AREA_INDEX(LEVEL_CCM, 1)
#define AREA_CCM_SLIDE          LEVEL_AREA_INDEX(LEVEL_CCM, 2)
#define AREA_CASTLE_LOBBY       LEVEL_AREA_INDEX(LEVEL_CASTLE, 1)
#define AREA_CASTLE_TIPPY       LEVEL_AREA_INDEX(LEVEL_CASTLE, 2)
#define AREA_CASTLE_BASEMENT    LEVEL_AREA_INDEX(LEVEL_CASTLE, 3)
#define AREA_HMC                LEVEL_AREA_INDEX(LEVEL_HMC, 1)
#define AREA_SSL_OUTSIDE        LEVEL_AREA_INDEX(LEVEL_SSL, 1)
#define AREA_SSL_PYRAMID        LEVEL_AREA_INDEX(LEVEL_SSL, 2)
#define AREA_SSL_EYEROK         LEVEL_AREA_INDEX(LEVEL_SSL, 3)
#define AREA_BOB                LEVEL_AREA_INDEX(LEVEL_BOB, 1)
#define AREA_SL_OUTSIDE         LEVEL_AREA_INDEX(LEVEL_SL, 1)
#define AREA_SL_IGLOO           LEVEL_AREA_INDEX(LEVEL_SL, 2)
#define AREA_WDW_MAIN           LEVEL_AREA_INDEX(LEVEL_WDW, 1)
#define AREA_WDW_TOWN           LEVEL_AREA_INDEX(LEVEL_WDW, 2)
#define AREA_JRB_MAIN           LEVEL_AREA_INDEX(LEVEL_JRB, 1)
#define AREA_JRB_SHIP           LEVEL_AREA_INDEX(LEVEL_JRB, 2)
#define AREA_THI_HUGE           LEVEL_AREA_INDEX(LEVEL_THI, 1)
#define AREA_THI_TINY           LEVEL_AREA_INDEX(LEVEL_THI, 2)
#define AREA_THI_WIGGLER        LEVEL_AREA_INDEX(LEVEL_THI, 3)
#define AREA_TTC                LEVEL_AREA_INDEX(LEVEL_TTC, 1)
#define AREA_RR                 LEVEL_AREA_INDEX(LEVEL_RR, 1)
#define AREA_CASTLE_GROUNDS     LEVEL_AREA_INDEX(LEVEL_CASTLE_GROUNDS, 1)
#define AREA_BITDW              LEVEL_AREA_INDEX(LEVEL_BITDW, 1)
#define AREA_VCUTM              LEVEL_AREA_INDEX(LEVEL_VCUTM, 1)
#define AREA_BITFS              LEVEL_AREA_INDEX(LEVEL_BITFS, 1)
#define AREA_SA                 LEVEL_AREA_INDEX(LEVEL_SA, 1)
#define AREA_BITS               LEVEL_AREA_INDEX(LEVEL_BITS, 1)
#define AREA_LLL_OUTSIDE        LEVEL_AREA_INDEX(LEVEL_LLL, 1)
#define AREA_LLL_VOLCANO        LEVEL_AREA_INDEX(LEVEL_LLL, 2)
#define AREA_DDD_WHIRLPOOL      LEVEL_AREA_INDEX(LEVEL_DDD, 1)
#define AREA_DDD_SUB            LEVEL_AREA_INDEX(LEVEL_DDD, 2)
#define AREA_WF                 LEVEL_AREA_INDEX(LEVEL_WF, 1)
#define AREA_ENDING             LEVEL_AREA_INDEX(LEVEL_ENDING, 1)
#define AREA_COURTYARD          LEVEL_AREA_INDEX(LEVEL_CASTLE_COURTYARD, 1)
#define AREA_PSS                LEVEL_AREA_INDEX(LEVEL_PSS, 1)
#define AREA_COTMC              LEVEL_AREA_INDEX(LEVEL_COTMC, 1)
#define AREA_TOTWC              LEVEL_AREA_INDEX(LEVEL_TOTWC, 1)
#define AREA_BOWSER_1           LEVEL_AREA_INDEX(LEVEL_BOWSER_1, 1)
#define AREA_WMOTR              LEVEL_AREA_INDEX(LEVEL_WMOTR, 1)
#define AREA_BOWSER_2           LEVEL_AREA_INDEX(LEVEL_BOWSER_2, 1)
#define AREA_BOWSER_3           LEVEL_AREA_INDEX(LEVEL_BOWSER_3, 1)
#define AREA_TTM_OUTSIDE        LEVEL_AREA_INDEX(LEVEL_TTM, 1)

#define CAM_MODE_MARIO_ACTIVE           0x01
#define CAM_MODE_LAKITU_WAS_ZOOMED_OUT  0x02
#define CAM_MODE_MARIO_SELECTED         0x04

#define CAM_SELECTION_MARIO 1
#define CAM_SELECTION_FIXED 2

#define CAM_ANGLE_MARIO  1
#define CAM_ANGLE_LAKITU 2

#define CAMERA_MODE_NONE              0x00
#define CAMERA_MODE_RADIAL            0x01
#define CAMERA_MODE_OUTWARD_RADIAL    0x02
#define CAMERA_MODE_BEHIND_MARIO      0x03
#define CAMERA_MODE_CLOSE             0x04
#define CAMERA_MODE_C_UP              0x06
#define CAMERA_MODE_WATER_SURFACE     0x08
#define CAMERA_MODE_SLIDE_HOOT        0x09
#define CAMERA_MODE_INSIDE_CANNON     0x0A
#define CAMERA_MODE_BOSS_FIGHT        0x0B
#define CAMERA_MODE_PARALLEL_TRACKING 0x0C
#define CAMERA_MODE_FIXED             0x0D
#define CAMERA_MODE_8_DIRECTIONS      0x0E
#define CAMERA_MODE_FREE_ROAM         0x10
#define CAMERA_MODE_SPIRAL_STAIRS     0x11

#define CAM_MOVE_RETURN_TO_MIDDLE       0x0001
#define CAM_MOVE_ZOOMED_OUT             0x0002
#define CAM_MOVE_ROTATE_RIGHT           0x0004
#define CAM_MOVE_ROTATE_LEFT            0x0008
#define CAM_MOVE_ENTERED_ROTATE_SURFACE 0x0010
#define CAM_MOVE_METAL_BELOW_WATER      0x0020
#define CAM_MOVE_FIX_IN_PLACE           0x0040
#define CAM_MOVE_UNKNOWN_8              0x0080
#define CAM_MOVING_INTO_MODE            0x0100
#define CAM_MOVE_STARTED_EXITING_C_UP   0x0200
#define CAM_MOVE_UNKNOWN_11             0x0400
#define CAM_MOVE_INIT_CAMERA            0x0800
#define CAM_MOVE_ALREADY_ZOOMED_OUT     0x1000
#define CAM_MOVE_C_UP_MODE              0x2000
#define CAM_MOVE_SUBMERGED              0x4000
#define CAM_MOVE_PAUSE_SCREEN           0x8000

#define CAM_MOVE_ROTATE  (CAM_MOVE_ROTATE_RIGHT | CAM_MOVE_ROTATE_LEFT | CAM_MOVE_RETURN_TO_MIDDLE)

#define CAM_MOVE_RESTRICT  (CAM_MOVE_ENTERED_ROTATE_SURFACE | CAM_MOVE_METAL_BELOW_WATER | CAM_MOVE_FIX_IN_PLACE | CAM_MOVE_UNKNOWN_8)

#define CAM_SOUND_C_UP_PLAYED            0x01
#define CAM_SOUND_MARIO_ACTIVE           0x02
#define CAM_SOUND_NORMAL_ACTIVE          0x04
#define CAM_SOUND_UNUSED_SELECT_MARIO    0x08
#define CAM_SOUND_UNUSED_SELECT_FIXED    0x10
#define CAM_SOUND_FIXED_ACTIVE           0x20

#define CAM_FLAG_SMOOTH_MOVEMENT         0x0001
#define CAM_FLAG_BLOCK_SMOOTH_MOVEMENT   0x0002
#define CAM_FLAG_FRAME_AFTER_CAM_INIT    0x0004
#define CAM_FLAG_CHANGED_PARTRACK_INDEX  0x0008
#define CAM_FLAG_CCM_SLIDE_SHORTCUT      0x0010
#define CAM_FLAG_CAM_NEAR_WALL           0x0020
#define CAM_FLAG_SLEEPING                0x0040
#define CAM_FLAG_UNUSED_7                0x0080
#define CAM_FLAG_UNUSED_8                0x0100
#define CAM_FLAG_COLLIDED_WITH_WALL      0x0200
#define CAM_FLAG_START_TRANSITION        0x0400
#define CAM_FLAG_TRANSITION_OUT_OF_C_UP  0x0800
#define CAM_FLAG_BLOCK_AREA_PROCESSING   0x1000
#define CAM_FLAG_UNUSED_13               0x2000
#define CAM_FLAG_UNUSED_CUTSCENE_ACTIVE  0x4000
#define CAM_FLAG_BEHIND_MARIO_POST_DOOR  0x8000

#define CAM_STATUS_NONE   0
#define CAM_STATUS_MARIO  1 << 0
#define CAM_STATUS_LAKITU 1 << 1
#define CAM_STATUS_FIXED  1 << 2
#define CAM_STATUS_C_DOWN 1 << 3
#define CAM_STATUS_C_UP   1 << 4

#define CAM_STATUS_MODE_GROUP   (CAM_STATUS_MARIO | CAM_STATUS_LAKITU | CAM_STATUS_FIXED)
#define CAM_STATUS_C_MODE_GROUP (CAM_STATUS_C_DOWN | CAM_STATUS_C_UP)

#define SHAKE_ATTACK         1
#define SHAKE_GROUND_POUND   2
#define SHAKE_SMALL_DAMAGE   3
#define SHAKE_MED_DAMAGE     4
#define SHAKE_LARGE_DAMAGE   5
#define SHAKE_HIT_FROM_BELOW 8
#define SHAKE_FALL_DAMAGE    9
#define SHAKE_SHOCK          10

#define SHAKE_ENV_EXPLOSION           1
#define SHAKE_ENV_BOWSER_THROW_BOUNCE 2
#define SHAKE_ENV_BOWSER_JUMP         3
#define SHAKE_ENV_UNUSED_5            5
#define SHAKE_ENV_UNUSED_6            6
#define SHAKE_ENV_UNUSED_7            7
#define SHAKE_ENV_PYRAMID_EXPLODE     8
#define SHAKE_ENV_JRB_SHIP_DRAIN      9
#define SHAKE_ENV_FALLING_BITS_PLAT   10

#define SHAKE_FOV_SMALL     1
#define SHAKE_FOV_UNUSED    2
#define SHAKE_FOV_MEDIUM    3
#define SHAKE_FOV_LARGE     4

#define SHAKE_POS_SMALL         1
#define SHAKE_POS_MEDIUM        2
#define SHAKE_POS_LARGE         3
#define SHAKE_POS_BOWLING_BALL  4

#define CUTSCENE_DOOR_PULL            130
#define CUTSCENE_DOOR_PUSH            131
#define CUTSCENE_ENTER_CANNON         133
#define CUTSCENE_ENTER_PAINTING       134
#define CUTSCENE_DEATH_EXIT           135
#define CUTSCENE_DOOR_WARP            139
#define CUTSCENE_DOOR_PULL_MODE       140
#define CUTSCENE_DOOR_PUSH_MODE       141
#define CUTSCENE_INTRO_PEACH          142
#define CUTSCENE_DANCE_ROTATE         143
#define CUTSCENE_ENTER_BOWSER_ARENA   144
#define CUTSCENE_0F_UNUSED            145
#define CUTSCENE_UNUSED_EXIT          147
#define CUTSCENE_SLIDING_DOORS_OPEN   149
#define CUTSCENE_PREPARE_CANNON       150
#define CUTSCENE_UNLOCK_KEY_DOOR      151
#define CUTSCENE_STANDING_DEATH       152
#define CUTSCENE_DEATH_ON_STOMACH     153
#define CUTSCENE_DEATH_ON_BACK        154
#define CUTSCENE_QUICKSAND_DEATH      155
#define CUTSCENE_SUFFOCATION_DEATH    156
#define CUTSCENE_EXIT_BOWSER_SUCC     157
#define CUTSCENE_EXIT_BOWSER_DEATH    158
#define CUTSCENE_WATER_DEATH          159
#define CUTSCENE_EXIT_PAINTING_SUCC   160
#define CUTSCENE_CAP_SWITCH_PRESS     161
#define CUTSCENE_DIALOG               162
#define CUTSCENE_RACE_DIALOG          163
#define CUTSCENE_ENTER_PYRAMID_TOP    164
#define CUTSCENE_DANCE_FLY_AWAY       165
#define CUTSCENE_DANCE_CLOSEUP        166
#define CUTSCENE_KEY_DANCE            167
#define CUTSCENE_SSL_PYRAMID_EXPLODE  168
#define CUTSCENE_EXIT_SPECIAL_SUCC    169
#define CUTSCENE_NONPAINTING_DEATH    170
#define CUTSCENE_READ_MESSAGE         171
#define CUTSCENE_ENDING               172
#define CUTSCENE_STAR_SPAWN           173
#define CUTSCENE_GRAND_STAR           174
#define CUTSCENE_DANCE_DEFAULT        175
#define CUTSCENE_RED_COIN_STAR_SPAWN  176
#define CUTSCENE_END_WAVING           177
#define CUTSCENE_CREDITS              178
#define CUTSCENE_EXIT_WATERFALL       179
#define CUTSCENE_EXIT_FALL_WMOTR      180
#define CUTSCENE_ENTER_POOL           181

#define CUTSCENE_STOP         0x8000

#define CUTSCENE_LOOP         0x7FFF

#define HAND_CAM_SHAKE_OFF                  0
#define HAND_CAM_SHAKE_CUTSCENE             1
#define HAND_CAM_SHAKE_UNUSED               2
#define HAND_CAM_SHAKE_HANG_OWL             3
#define HAND_CAM_SHAKE_HIGH                 4
#define HAND_CAM_SHAKE_STAR_DANCE           5
#define HAND_CAM_SHAKE_LOW                  6

#define DOOR_DEFAULT         0
#define DOOR_LEAVING_SPECIAL 1
#define DOOR_ENTER_LOBBY     2

#define CAM_FOV_SET_45      1
#define CAM_FOV_DEFAULT     2
#define CAM_FOV_APP_45      4
#define CAM_FOV_SET_30      5
#define CAM_FOV_APP_20      6
#define CAM_FOV_BBH         7
#define CAM_FOV_APP_80      9
#define CAM_FOV_APP_30      10
#define CAM_FOV_APP_60      11
#define CAM_FOV_ZOOM_30     12
#define CAM_FOV_SET_29      13

#define CAM_EVENT_CANNON              1
#define CAM_EVENT_SHOT_FROM_CANNON    2
#define CAM_EVENT_UNUSED_3            3
#define CAM_EVENT_BOWSER_INIT         4
#define CAM_EVENT_DOOR_WARP           5
#define CAM_EVENT_DOOR                6
#define CAM_EVENT_BOWSER_JUMP         7
#define CAM_EVENT_BOWSER_THROW_BOUNCE 8
#define CAM_EVENT_START_INTRO         9
#define CAM_EVENT_START_GRAND_STAR    10
#define CAM_EVENT_START_ENDING        11
#define CAM_EVENT_START_END_WAVING    12
#define CAM_EVENT_START_CREDITS       13

struct PlayerCameraState {

     u32 action;
     Vec3f pos;
     Vec3s faceAngle;
     Vec3s headRotation;
     s16 unused;

     s16 cameraEvent;
     struct Object *usedObj;
};

struct TransitionInfo {
     s16 posPitch;
     s16 posYaw;
     f32 posDist;
     s16 focPitch;
     s16 focYaw;
     f32 focDist;
     s32 framesLeft;
     Vec3f marioPos;
     u8 unused;
};

struct HandheldShakePoint {
     s8 index;
     u32 unused;
     Vec3s point;
};

typedef BAD_RETURN(s32) (*CameraEvent)(struct Camera *c);

typedef CameraEvent CutsceneShot;

struct CameraTrigger {

    s8 area;

    CameraEvent event;

    s16 centerX;
    s16 centerY;
    s16 centerZ;

    s16 boundsX;
    s16 boundsY;
    s16 boundsZ;

    s16 boundsYaw;
};

struct Cutscene {

    CutsceneShot shot;

    s16 duration;
};

struct CameraFOVStatus {

     u8 fovFunc;

     f32 fov;

     f32 fovOffset;

     u32 unusedIsSleeping;

     f32 shakeAmplitude;

     s16 shakePhase;

     s16 shakeSpeed;

     s16 decay;
};

struct CutsceneSplinePoint {

    s8 index;

    u8 speed;
    Vec3s point;
};

struct PlayerGeometry {
     struct Surface *currFloor;
     f32 currFloorHeight;
     s16 currFloorType;
     struct Surface *currCeil;
     s16 currCeilType;
     f32 currCeilHeight;
     struct Surface *prevFloor;
     f32 prevFloorHeight;
     s16 prevFloorType;
     struct Surface *prevCeil;
     f32 prevCeilHeight;
     s16 prevCeilType;

     f32 waterHeight;
};

struct LinearTransitionPoint {
    Vec3f focus;
    Vec3f pos;
    f32 dist;
    s16 pitch;
    s16 yaw;
};

struct ModeTransitionInfo {
    s16 newMode;
    s16 lastMode;
    s16 max;
    s16 frame;
    struct LinearTransitionPoint transitionStart;
    struct LinearTransitionPoint transitionEnd;
};

struct ParallelTrackingPoint {

    s16 startOfPath;

    Vec3f pos;

    f32 distThresh;

    f32 zoom;
};

struct CameraStoredInfo {
     Vec3f pos;
     Vec3f focus;
     f32 panDist;
     f32 cannonYOffset;
};

struct CutsceneVariable {

    s32 unused1;
    Vec3f point;
    Vec3f unusedPoint;
    Vec3s angle;

    s16 unused2;
};

struct Camera {
     u8 mode;
     u8 defMode;

     s16 yaw;
     Vec3f focus;
     Vec3f pos;
     Vec3f unusedVec1;

     f32 areaCenX;

     f32 areaCenZ;
     u8 cutscene;
     u8 filler1[8];
     s16 nextYaw;
     u8 filler2[40];
     u8 doorStatus;

     f32 areaCenY;
};

struct LakituState {

     Vec3f curFocus;

     Vec3f curPos;

     Vec3f goalFocus;

     Vec3f goalPos;

     u8 filler1[12];

     u8 mode;

     u8 defMode;

     u8 filler2[10];

     f32 focusDistance;
     s16 oldPitch;
     s16 oldYaw;
     s16 oldRoll;

     Vec3s shakeMagnitude;

     s16 shakePitchPhase;
     s16 shakePitchVel;
     s16 shakePitchDecay;

     Vec3f unusedVec1;
     Vec3s unusedVec2;
     u8 filler3[8];

     s16 roll;

     s16 yaw;

     s16 nextYaw;

     Vec3f focus;

     Vec3f pos;

     s16 shakeRollPhase;
     s16 shakeRollVel;
     s16 shakeRollDecay;
     s16 shakeYawPhase;
     s16 shakeYawVel;
     s16 shakeYawDecay;

     f32 focHSpeed;

     f32 focVSpeed;

     f32 posHSpeed;
     f32 posVSpeed;

     s16 keyDanceRoll;

     u32 lastFrameAction;
     s16 unused;
};

#ifndef INCLUDED_FROM_CAMERA_C

extern s16 sSelectionFlags;
extern s16 sCameraSoundFlags;
extern u16 sCButtonsPressed;
extern struct PlayerCameraState gPlayerCameraState[2];
extern struct LakituState gLakituState;
extern s16 gCameraMovementFlags;
extern s32 gObjCutsceneDone;
extern struct Camera *gCamera;
#endif

extern struct Object *gCutsceneFocus;
extern struct Object *gSecondCameraFocus;
extern u8 gRecentCutscene;

void set_camera_shake_from_hit(s16 shake);
void set_environmental_camera_shake(s16 shake);
void set_camera_shake_from_point(s16 shake, f32 posX, f32 posY, f32 posZ);
void move_mario_head_c_up(UNUSED struct Camera *c);
void transition_next_state(UNUSED struct Camera *c, s16 frames);
void set_camera_mode(struct Camera *c, s16 mode, s16 frames);
void update_camera(struct Camera *c);
void reset_camera(struct Camera *c);
void init_camera(struct Camera *c);
void select_mario_cam_mode(void);
Gfx *geo_camera_main(s32 callContext, struct GraphNode *g, void *context);
void stub_camera_2(UNUSED struct Camera *c);
void stub_camera_3(UNUSED struct Camera *c);
void vec3f_sub(Vec3f dst, Vec3f src);
void object_pos_to_vec3f(Vec3f dst, struct Object *o);
void vec3f_to_object_pos(struct Object *o, Vec3f src);
s32 move_point_along_spline(Vec3f p, struct CutsceneSplinePoint spline[], s16 *splineSegment, f32 *progress);
s32 cam_select_alt_mode(s32 angle);
s32 set_cam_angle(s32 mode);
void set_handheld_shake(u8 mode);
void shake_camera_handheld(Vec3f pos, Vec3f focus);
s32 find_c_buttons_pressed(u16 currentState, u16 buttonsPressed, u16 buttonsDown);
s32 update_camera_hud_status(struct Camera *c);
s32 collide_with_walls(Vec3f pos, f32 offsetY, f32 radius);
s32 clamp_pitch(Vec3f from, Vec3f to, s16 maxPitch, s16 minPitch);
s32 is_within_100_units_of_mario(f32 posX, f32 posY, f32 posZ);
s32 set_or_approach_f32_asymptotic(f32 *dst, f32 goal, f32 scale);
s32 approach_f32_asymptotic_bool(f32 *current, f32 target, f32 multiplier);
f32 approach_f32_asymptotic(f32 current, f32 target, f32 multiplier);
s32 approach_s16_asymptotic_bool(s16 *current, s16 target, s16 divisor);
s32 approach_s16_asymptotic(s16 current, s16 target, s16 divisor);
void approach_vec3f_asymptotic(Vec3f current, Vec3f target, f32 xMul, f32 yMul, f32 zMul);
void set_or_approach_vec3f_asymptotic(Vec3f dst, Vec3f goal, f32 xMul, f32 yMul, f32 zMul);
s32 camera_approach_s16_symmetric_bool(s16 *current, s16 target, s16 increment);
s32 set_or_approach_s16_symmetric(s16 *current, s16 target, s16 increment);
s32 camera_approach_f32_symmetric_bool(f32 *current, f32 target, f32 increment);
f32 camera_approach_f32_symmetric(f32 value, f32 target, f32 increment);
void random_vec3s(Vec3s dst, s16 xRange, s16 yRange, s16 zRange);
s32 clamp_positions_and_find_yaw(Vec3f pos, Vec3f origin, f32 xMax, f32 xMin, f32 zMax, f32 zMin);
s32 is_range_behind_surface(Vec3f from, Vec3f to, struct Surface *surf, s16 range, s16 surfType);
void scale_along_line(Vec3f dest, Vec3f from, Vec3f to, f32 scale);
s16 calculate_pitch(Vec3f from, Vec3f to);
s16 calculate_yaw(Vec3f from, Vec3f to);
void calculate_angles(Vec3f from, Vec3f to, s16 *pitch, s16 *yaw);
f32 calc_abs_dist(Vec3f a, Vec3f b);
f32 calc_hor_dist(Vec3f a, Vec3f b);
void rotate_in_xz(Vec3f dst, Vec3f src, s16 yaw);
void rotate_in_yz(Vec3f dst, Vec3f src, s16 pitch);
void set_camera_pitch_shake(s16 mag, s16 decay, s16 inc);
void set_camera_yaw_shake(s16 mag, s16 decay, s16 inc);
void set_camera_roll_shake(s16 mag, s16 decay, s16 inc);
void set_pitch_shake_from_point(s16 mag, s16 decay, s16 inc, f32 maxDist, f32 posX, f32 posY, f32 posZ);
void shake_camera_pitch(Vec3f pos, Vec3f focus);
void shake_camera_yaw(Vec3f pos, Vec3f focus);
void shake_camera_roll(s16 *roll);
s32 offset_yaw_outward_radial(struct Camera *c, s16 areaYaw);
void play_camera_buzz_if_cdown(void);
void play_camera_buzz_if_cbutton(void);
void play_camera_buzz_if_c_sideways(void);
void play_sound_cbutton_up(void);
void play_sound_cbutton_down(void);
void play_sound_cbutton_side(void);
void play_sound_button_change_blocked(void);
void play_sound_rbutton_changed(void);
void play_sound_if_cam_switched_to_lakitu_or_mario(void);
s32 radial_camera_input(struct Camera *c, UNUSED f32 unused);
s32 trigger_cutscene_dialog(s32 trigger);
void handle_c_button_movement(struct Camera *c);
void start_cutscene(struct Camera *c, u8 cutscene);
u8 get_cutscene_from_mario_status(struct Camera *c);
void warp_camera(f32 displacementX, f32 displacementY, f32 displacementZ);
void approach_camera_height(struct Camera *c, f32 goal, f32 inc);
void offset_rotated(Vec3f dst, Vec3f from, Vec3f to, Vec3s rotation);
s16 next_lakitu_state(Vec3f newPos, Vec3f newFoc, Vec3f curPos, Vec3f curFoc, Vec3f oldPos, Vec3f oldFoc, s16 yaw);
void set_fixed_cam_axis_sa_lobby(UNUSED s16 preset);
s16 camera_course_processing(struct Camera *c);
void resolve_geometry_collisions(Vec3f pos, UNUSED Vec3f lastGood);
s32 rotate_camera_around_walls(struct Camera *c, Vec3f cPos, s16 *avoidYaw, s16 yawRange);
void find_mario_floor_and_ceil(struct PlayerGeometry *pg);
u8 start_object_cutscene_without_focus(u8 cutscene);
s16 cutscene_object_with_dialog(u8 cutscene, struct Object *o, s16 dialogID);
s16 cutscene_object_without_dialog(u8 cutscene, struct Object *o);
s16 cutscene_object(u8 cutscene, struct Object *o);
void play_cutscene(struct Camera *c);
s32 cutscene_event(CameraEvent event, struct Camera * c, s16 start, s16 end);
s32 cutscene_spawn_obj(u32 obj, s16 frame);
void set_fov_shake(s16 amplitude, s16 decay, s16 shakeSpeed);

void set_fov_function(u8 func);
void cutscene_set_fov_shake_preset(u8 preset);
void set_fov_shake_from_point_preset(u8 preset, f32 posX, f32 posY, f32 posZ);
void obj_rotate_towards_point(struct Object *o, Vec3f point, s16 pitchOff, s16 yawOff, s16 pitchDiv, s16 yawDiv);

Gfx *geo_camera_fov(s32 callContext, struct GraphNode *g, UNUSED void *context);

#endif
