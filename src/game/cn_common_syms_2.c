#include <ultra64.h>

#include "macros.h"
#include "types.h"

#ifdef VERSION_CN

struct HudDisplay {
     s16 lives;
     s16 coins;
     s16 stars;
     s16 wedges;
     s16 keys;
     s16 flags;
     u16 timer;
};

typedef struct {
     u32 ramarray[15];
     u32 pifstatus;
} OSPifRam;

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

struct WarpTransition {
     u8 isActive;
     u8 type;
     u8 time;
     u8 pauseRendering;
     struct WarpTransitionData data;
};

typedef struct __OSEventState
{
    OSMesgQueue *messageQueue;
    OSMesg message;
} __OSEventState;

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

FORCE_BSS struct HudDisplay gHudDisplay;
FORCE_BSS OSThread __osThreadSave;
FORCE_BSS OSPifRam __osContPifRam;
FORCE_BSS OSPiHandle __Dom2SpeedParam;
FORCE_BSS struct SpawnInfo gPlayerSpawnInfos[1];
FORCE_BSS struct GraphNode *D_8033A160[0x100];
FORCE_BSS OSPiHandle __CartRomHandle;
FORCE_BSS u8 sBssPad[0x48];
FORCE_BSS OSMesgQueue gOsPiMessageQueue;
FORCE_BSS OSPiHandle __Dom1SpeedParam;
FORCE_BSS OSTimer __osBaseTimer;
FORCE_BSS struct WarpTransition gWarpTransition;
FORCE_BSS OSTimer __osEepromTimer;
FORCE_BSS struct MarioState gMarioStates[1];
FORCE_BSS __OSEventState __osEventStateTab[OS_NUM_EVENTS];
FORCE_BSS OSMesgQueue __osEepromTimerQ;
FORCE_BSS struct Area gAreaData[8];
FORCE_BSS OSMesgQueue gOsSiMessageQueue;
#endif
