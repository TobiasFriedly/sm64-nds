#ifndef AREA_H
#define AREA_H

#include <PR/ultratypes.h>

#include "types.h"
#include "camera.h"
#include "engine/graph_node.h"

struct WarpNode {
     u8 id;
     u8 destLevel;
     u8 destArea;
     u8 destNode;
};

struct ObjectWarpNode {
     struct WarpNode node;
     struct Object *object;
     struct ObjectWarpNode *next;
};

#define INSTANT_WARP_INDEX_START  0x00
#define INSTANT_WARP_INDEX_STOP   0x04

struct InstantWarp {
     u8 id;
     u8 area;
     Vec3s displacement;
};

struct SpawnInfo {
     Vec3s startPos;
     Vec3s startAngle;
     s8 areaIndex;
     s8 activeAreaIndex;
     u32 behaviorArg;
     void *behaviorScript;
     struct GraphNode *model;
     struct SpawnInfo *next;
};

struct UnusedArea28 {
     s16 unk00;
     s16 unk02;
     s16 unk04;
     s16 unk06;
     s16 unk08;
};

struct Whirlpool {
     Vec3s pos;
     s16 strength;
};

struct Area {
     s8 index;
     s8 flags;
     u16 terrainType;
     struct GraphNodeRoot *unk04;
     TerrainData *terrainData;
     RoomData *surfaceRooms;
     s16 *macroObjects;
     struct ObjectWarpNode *warpNodes;
     struct WarpNode *paintingWarpNodes;
     struct InstantWarp *instantWarps;
     struct SpawnInfo *objectSpawnInfos;
     struct Camera *camera;
     struct UnusedArea28 *unused;
     struct Whirlpool *whirlpools[2];
     u8 dialog[2];
     u16 musicParam;
     u16 musicParam2;
};

struct WarpTransitionData {
     u8 red;
     u8 green;
     u8 blue;

     s16 startTexRadius;
     s16 endTexRadius;
     s16 startTexX;
     s16 startTexY;
     s16 endTexX;
     s16 endTexY;

     s16 texTimer;
};

#define WARP_TRANSITION_FADE_FROM_COLOR  0x00
#define WARP_TRANSITION_FADE_INTO_COLOR  0x01
#define WARP_TRANSITION_FADE_FROM_STAR   0x08
#define WARP_TRANSITION_FADE_INTO_STAR   0x09
#define WARP_TRANSITION_FADE_FROM_CIRCLE 0x0A
#define WARP_TRANSITION_FADE_INTO_CIRCLE 0x0B
#define WARP_TRANSITION_FADE_FROM_MARIO  0x10
#define WARP_TRANSITION_FADE_INTO_MARIO  0x11
#define WARP_TRANSITION_FADE_FROM_BOWSER 0x12
#define WARP_TRANSITION_FADE_INTO_BOWSER 0x13

struct WarpTransition {
     u8 isActive;
     u8 type;
     u8 time;
     u8 pauseRendering;
     struct WarpTransitionData data;
};

enum MenuOption {
    MENU_OPT_NONE,
    MENU_OPT_1,
    MENU_OPT_2,
    MENU_OPT_3,
    MENU_OPT_DEFAULT = MENU_OPT_1,

    MENU_OPT_CONTINUE = MENU_OPT_1,
    MENU_OPT_EXIT_COURSE = MENU_OPT_2,
    MENU_OPT_CAMERA_ANGLE_R = MENU_OPT_3,

    MENU_OPT_SAVE_AND_CONTINUE = MENU_OPT_1,
    MENU_OPT_SAVE_AND_QUIT = MENU_OPT_2,
    MENU_OPT_CONTINUE_DONT_SAVE = MENU_OPT_3
};

extern struct GraphNode **gLoadedGraphNodes;
extern struct SpawnInfo gPlayerSpawnInfos[];
extern struct GraphNode *D_8033A160[];
extern struct Area gAreaData[];
extern struct WarpTransition gWarpTransition;
extern s16 gCurrCourseNum;
extern s16 gCurrActNum;
extern s16 gCurrAreaIndex;
extern s16 gSavedCourseNum;
extern s16 gMenuOptSelectIndex;
extern s16 gSaveOptSelectIndex;

extern struct SpawnInfo *gMarioSpawnInfo;

extern struct Area *gAreas;
extern struct Area *gCurrentArea;

extern s16 gCurrSaveFileNum;
extern s16 gCurrLevelNum;

void override_viewport_and_clip(Vp *a, Vp *b, u8 c, u8 d, u8 e);
void print_intro_text(void);
u32 get_mario_spawn_type(struct Object *o);
struct ObjectWarpNode *area_get_warp_node(u8 id);
void clear_areas(void);
void clear_area_graph_nodes(void);
void load_area(s32 index);
void unload_area(void);
void load_mario_area(void);
void unload_mario_area(void);
void change_area(s32 index);
void area_update_objects(void);
void play_transition(s16 transType, s16 time, u8 red, u8 green, u8 blue);
void play_transition_after_delay(s16 transType, s16 time, u8 red, u8 green, u8 blue, s16 delay);
void render_game(void);

#endif
