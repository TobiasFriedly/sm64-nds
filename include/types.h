#ifndef TYPES_H
#define TYPES_H

#include <ultra64.h>
#include "macros.h"
#include "config.h"

#ifdef AVOID_UB
    #define BAD_RETURN(cmd) void
#else
    #define BAD_RETURN(cmd) cmd
#endif

struct Controller {
   s16 rawStickX;
   s16 rawStickY;
   float stickX;
   float stickY;
   float stickMag;
   u16 buttonDown;
   u16 buttonPressed;
   OSContStatus *statusData;
   OSContPad *controllerData;
#if ENABLE_RUMBLE
   s32 port;
#endif
};

typedef f32 Vec2f[2];
typedef f32 Vec3f[3];
typedef s16 Vec3s[3];
typedef s32 Vec3i[3];
typedef f32 Vec4f[4];
typedef s16 Vec4s[4];

typedef f32 Mat4[4][4];

typedef uintptr_t GeoLayout;
typedef uintptr_t LevelScript;
typedef s16 Movtex;
typedef s16 MacroObject;
typedef s16 Collision;
typedef s16 Trajectory;
typedef s16 PaintingData;
typedef uintptr_t BehaviorScript;
typedef u8 Texture;
typedef s8 RoomData;
typedef Collision TerrainData;
typedef TerrainData Vec3Terrain[3];

enum SpTaskState {
    SPTASK_STATE_NOT_STARTED,
    SPTASK_STATE_RUNNING,
    SPTASK_STATE_INTERRUPTED,
    SPTASK_STATE_FINISHED,
    SPTASK_STATE_FINISHED_DP
};

struct SPTask {
     OSTask task;
     OSMesgQueue *msgqueue;
     OSMesg msg;
     enum SpTaskState state;
};

struct VblankHandler {
    OSMesgQueue *queue;
    OSMesg msg;
};

#define ANIM_FLAG_NOLOOP     (1 << 0)
#define ANIM_FLAG_BACKWARD   (1 << 1)
#define ANIM_FLAG_2          (1 << 2)
#define ANIM_FLAG_HOR_TRANS  (1 << 3)
#define ANIM_FLAG_VERT_TRANS (1 << 4)
#define ANIM_FLAG_5          (1 << 5)
#define ANIM_FLAG_6          (1 << 6)
#define ANIM_FLAG_7          (1 << 7)

struct Animation {
     s16 flags;
     s16 animYTransDivisor;
     s16 startFrame;
     s16 loopStart;
     s16 loopEnd;
     s16 unusedBoneCount;
     const s16 *values;
     const u16 *index;
     u32 length;
};

#define ANIMINDEX_NUMPARTS(animindex) (sizeof(animindex) / sizeof(u16) / 6 - 1)

struct GraphNode {
     s16 type;
     s16 flags;
     struct GraphNode *prev;
     struct GraphNode *next;
     struct GraphNode *parent;
     struct GraphNode *children;
};

struct AnimInfo {
     s16 animID;
     s16 animYTrans;
     struct Animation *curAnim;
     s16 animFrame;
     u16 animTimer;
     s32 animFrameAccelAssist;
     s32 animAccel;
};

struct GraphNodeObject {
     struct GraphNode node;
     struct GraphNode *sharedChild;
     s8 areaIndex;
     s8 activeAreaIndex;
     Vec3s angle;
     Vec3f pos;
     Vec3f scale;
     struct AnimInfo animInfo;
     struct SpawnInfo *unk4C;
     Mat4 *throwMatrix;
     Vec3f cameraToObject;
};

struct ObjectNode {
    struct GraphNodeObject gfx;
    struct ObjectNode *next;
    struct ObjectNode *prev;
};

struct Object {
     struct ObjectNode header;
     struct Object *parentObj;
     struct Object *prevObj;
     u32 collidedObjInteractTypes;
     s16 activeFlags;
     s16 numCollidedObjs;
     struct Object *collidedObjs[4];

    union {

        u32 asU32[0x50];
        s32 asS32[0x50];
        s16 asS16[0x50][2];
        f32 asF32[0x50];
#if !IS_64_BIT
        s16 *asS16P[0x50];
        s32 *asS32P[0x50];
        struct Animation **asAnims[0x50];
        struct Waypoint *asWaypoint[0x50];
        struct ChainSegment *asChainSegment[0x50];
        struct Object *asObject[0x50];
        struct Surface *asSurface[0x50];
        void *asVoidPtr[0x50];
        const void *asConstVoidPtr[0x50];
#endif
    } rawData;
#if IS_64_BIT
    union {
        s16 *asS16P[0x50];
        s32 *asS32P[0x50];
        struct Animation **asAnims[0x50];
        struct Waypoint *asWaypoint[0x50];
        struct ChainSegment *asChainSegment[0x50];
        struct Object *asObject[0x50];
        struct Surface *asSurface[0x50];
        void *asVoidPtr[0x50];
        const void *asConstVoidPtr[0x50];
    } ptrData;
#endif
     u32 unused1;
     const BehaviorScript *curBhvCommand;
     u32 bhvStackIndex;
     uintptr_t bhvStack[8];
     s16 bhvDelayTimer;
     s16 respawnInfoType;
     f32 hitboxRadius;
     f32 hitboxHeight;
     f32 hurtboxRadius;
     f32 hurtboxHeight;
     f32 hitboxDownOffset;
     const BehaviorScript *behavior;
     u32 unused2;
     struct Object *platform;
     void *collisionData;
     Mat4 transform;
     void *respawnInfo;
};

struct ObjectHitbox {
     u32 interactType;
     u8 downOffset;
     s8 damageOrCoinValue;
     s8 health;
     s8 numLootCoins;
     s16 radius;
     s16 height;
     s16 hurtboxRadius;
     s16 hurtboxHeight;
};

struct Waypoint {
    s16 flags;
    Vec3s pos;
};

struct Surface {
     TerrainData type;
     TerrainData force;
     s8 flags;
     RoomData room;
     TerrainData lowerY;
     TerrainData upperY;
     Vec3Terrain vertex1;
     Vec3Terrain vertex2;
     Vec3Terrain vertex3;
     struct {
        f32 x;
        f32 y;
        f32 z;
    } normal;
     f32 originOffset;
     struct Object *object;
};

struct MarioBodyState {
     u32 action;
     s8 capState;
     s8 eyeState;
     s8 handState;
     s8 wingFlutter;
     s16 modelState;
     s8 grabPos;
     u8 punchState;
     Vec3s torsoAngle;
     Vec3s headAngle;
     Vec3f heldObjLastPosition;
    u8 filler[4];
};

struct MarioState {
     u16 unk00;
     u16 input;
     u32 flags;
     u32 particleFlags;
     u32 action;
     u32 prevAction;
     u32 terrainSoundAddend;
     u16 actionState;
     u16 actionTimer;
     u32 actionArg;
     f32 intendedMag;
     s16 intendedYaw;
     s16 invincTimer;
     u8 framesSinceA;
     u8 framesSinceB;
     u8 wallKickTimer;
     u8 doubleJumpTimer;
     Vec3s faceAngle;
     Vec3s angleVel;
     s16 slideYaw;
     s16 twirlYaw;
     Vec3f pos;
     Vec3f vel;
     f32 forwardVel;
     f32 slideVelX;
     f32 slideVelZ;
     struct Surface *wall;
     struct Surface *ceil;
     struct Surface *floor;
     f32 ceilHeight;
     f32 floorHeight;
     s16 floorAngle;
     s16 waterLevel;
     struct Object *interactObj;
     struct Object *heldObj;
     struct Object *usedObj;
     struct Object *riddenObj;
     struct Object *marioObj;
     struct SpawnInfo *spawnInfo;
     struct Area *area;
     struct PlayerCameraState *statusForCamera;
     struct MarioBodyState *marioBodyState;
     struct Controller *controller;
     struct DmaHandlerList *animList;
     u32 collidedObjInteractTypes;
     s16 numCoins;
     s16 numStars;
     s8 numKeys;
     s8 numLives;
     s16 health;
     s16 unkB0;
     u8 hurtCounter;
     u8 healCounter;
     u8 squishTimer;
     u8 fadeWarpOpacity;
     u16 capTimer;
     s16 prevNumStarsForDialog;
     f32 peakHeight;
     f32 quicksandDepth;
     f32 gettingBlownGravity;
};

#endif
