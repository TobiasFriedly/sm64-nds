#ifndef GD_TYPES_H
#define GD_TYPES_H

#include <ultra64.h>

struct GdVec3f {
    f32 x, y, z;
};

struct GdBoundingBox {
    f32 minX, minY, minZ;
    f32 maxX, maxY, maxZ;
};

struct GdTriangleF {
    struct GdVec3f p0, p1, p2;
};

struct GdAnimTransform {
    struct GdVec3f scale;
    struct GdVec3f rotate;
    struct GdVec3f pos;
};

typedef f32 Mat4f[4][4];

struct GdColour {
    f32 r, g, b;
};

union DynUnion {
    void *ptr;
    char *str;
    s32 word;
};

struct DynList {
    s32 cmd;
    union DynUnion w1;
    union DynUnion w2;
    struct GdVec3f vec;
};

enum ObjTypeFlag {
    OBJ_TYPE_GROUPS    = 0x00000001,
    OBJ_TYPE_BONES     = 0x00000002,
    OBJ_TYPE_JOINTS    = 0x00000004,
    OBJ_TYPE_PARTICLES = 0x00000008,
    OBJ_TYPE_SHAPES    = 0x00000010,
    OBJ_TYPE_NETS      = 0x00000020,
    OBJ_TYPE_PLANES    = 0x00000040,
    OBJ_TYPE_FACES     = 0x00000080,
    OBJ_TYPE_VERTICES  = 0x00000100,
    OBJ_TYPE_CAMERAS   = 0x00000200,

    OBJ_TYPE_MATERIALS = 0x00000800,
    OBJ_TYPE_WEIGHTS   = 0x00001000,
    OBJ_TYPE_GADGETS   = 0x00002000,
    OBJ_TYPE_VIEWS     = 0x00004000,
    OBJ_TYPE_LABELS    = 0x00008000,
    OBJ_TYPE_ANIMATORS = 0x00010000,
    OBJ_TYPE_VALPTRS   = 0x00020000,

    OBJ_TYPE_LIGHTS    = 0x00080000,
    OBJ_TYPE_ZONES     = 0x00100000,
    OBJ_TYPE_UNK200000 = 0x00200000
};

#define OBJ_TYPE_ALL 0x00FFFFFF

typedef void (*drawmethod_t)(void *);

enum ObjDrawingFlags {
    OBJ_DRAW_UNK01   = 0x01,
    OBJ_INVISIBLE    = 0x02,
    OBJ_PICKED       = 0x04,
    OBJ_IS_GRABBABLE = 0x08,
    OBJ_HIGHLIGHTED  = 0x10
};

struct GdObj {
     struct GdObj *prev;
     struct GdObj *next;
     drawmethod_t objDrawFn;
     enum ObjTypeFlag type;
     s16 index;
     u16 drawFlags;

};

struct ListNode {
     struct ListNode *prev;
     struct ListNode *next;
     struct GdObj *obj;
};

struct VtxLink {
    struct VtxLink *prev;
    struct VtxLink *next;
    Vtx *data;
};

struct GdFaceData {
    u32 count;
    s32 type;
    u16 (*data)[4];
};

struct GdVtxData {
    u32 count;
    s32 type;
    s16 (*data)[3];
};

struct ObjGroup {
     struct GdObj header;
     struct ObjGroup *prev;
     struct ObjGroup *next;
     struct ListNode *firstMember;
     struct ListNode *lastMember;
     s32 memberTypes;
     s32 memberCount;
     s32 debugPrint;
     s32 linkType;
     char name[0x40];
     s32 id;
};

struct ObjBone {
     struct GdObj header;
     struct GdVec3f worldPos;
     struct ObjBone *prev;
     struct ObjBone *next;
     struct GdVec3f unk28;
     u8  filler1[12];
     struct GdVec3f unk40;
     u8  filler2[12];
     struct GdVec3f unk58;
     struct GdVec3f unk64;
     Mat4f mat70;
     Mat4f matB0;
     struct ObjShape *shapePtr;
     f32 unkF4;
     f32 unkF8;
     f32 unkFC;
     s32 colourNum;
     s32 unk104;
     s32 id;
     struct ObjGroup *unk10C;
     f32 spring;
     f32 unk114;
     f32 unk118;
     u8  filler3[8];
};

struct ObjJoint {
     struct GdObj header;
     struct GdVec3f worldPos;
     struct ObjShape *shapePtr;
     struct ObjJoint *prevjoint;
     struct ObjJoint *nextjoint;
     void (*updateFunc)(struct ObjJoint*);
     struct GdVec3f unk30;
     struct GdVec3f unk3C;
     struct GdVec3f unk48;
     struct GdVec3f initPos;
     u8  filler1[12];
     struct GdVec3f unk6C;
     struct GdVec3f velocity;
     struct GdVec3f unk84;
     struct GdVec3f unk90;
     struct GdVec3f scale;
     struct GdVec3f unkA8;
     struct GdVec3f unkB4;
     struct GdVec3f shapeOffset;
     struct GdVec3f unkCC;
     u8  filler2[4];
     struct GdVec3f friction;
     Mat4f matE8;
     Mat4f mat128;
     Mat4f mat168;
     struct GdVec3f unk1A8;
     s32 id;
     u8  filler3[4];
     s32 flags;
     s32 unk1C0;
     struct ObjGroup *unk1C4;
     s32 colourNum;
     s32 type;
     struct ObjAnimator *rootAnimator;
     u8  filler4[32];
     struct ObjGroup *weightGrp;
     struct ObjGroup *attachedObjsGrp;
     s32 attachFlags;
     struct GdVec3f attachOffset;
     struct GdObj *attachedToObj;
     u8  filler5[24];
     f32 unk228;
};

struct ObjParticle {
     struct GdObj header;
     u8 filler1[8];
     struct ObjShape *shapePtr;
     struct GdVec3f pos;
     u8 filler2[4];
     f32 unk30;
     u8 filler3[4];
     struct GdVec3f unk38;
     f32 unk44;
     f32 unk48;
     u8 filler4[4];
     s32 id;
     u32 flags;
     s32 colourNum;
     s32 timeout;
     s32 unk60;
     s32 unk64;
     u8 filler5[4];
     struct ObjGroup *subParticlesGrp;
     u8 filler6[4];
     s32 unk74;
     u8 unk78[4];
     struct ObjAnimator *unk7C;
     struct ObjLight *unk80;
     u8 filler7[44];
     s32 unkB0;
     struct ObjGroup *attachedObjsGrp;
     s32 attachFlags;
     struct GdObj *attachedToObj;
};

struct ObjShape {
     struct GdObj header;
     struct ObjShape *prevShape;
     struct ObjShape *nextShape;
     struct ObjGroup *faceGroup;
     struct ObjGroup *vtxGroup;
     struct ObjGroup *scaledVtxGroup;
     u8 filler1[4];
     struct ObjGroup *mtlGroup;
     s32 unk30;
     s32 faceCount;
     s32 vtxCount;
     s32 unk3C;
     u32 id;
     s32 flag;
     s32 dlNums[2];
               s32 unk50;
     u8  filler2[4];
     f32 alpha;
     char name[0x40];
};

struct ObjNet {
     struct GdObj header;
     struct GdVec3f worldPos;
     struct GdVec3f initPos;
     u8  filler1[8];
     s32 flags;
     u32 id;
     s32 unk3C;
     s32 colourNum;
     struct GdVec3f unusedForce;
     struct GdVec3f velocity;
     struct GdVec3f rotation;
     struct GdVec3f unk68;
     struct GdVec3f collDisp;
     struct GdVec3f collTorque;
     struct GdVec3f unusedCollTorqueL;
     struct GdVec3f unusedCollTorqueD;
     struct GdVec3f torque;
     struct GdVec3f centerOfGravity;
     struct GdBoundingBox boundingBox;
     struct GdVec3f unusedCollDispOff;
     f32 unusedCollMaxD;
     f32 maxRadius;
     Mat4f matE8;
     Mat4f mat128;
     Mat4f mat168;
     struct ObjShape *shapePtr;
     struct GdVec3f scale;
     f32 unusedMass;
     s32 numModes;
     struct ObjGroup *unk1C0;
     struct ObjGroup *skinGrp;
     struct ObjGroup *unk1C8;
     struct ObjGroup *unk1CC;
     struct ObjGroup *unk1D0;
     struct ObjGroup *attachedObjsGrp;
     struct GdVec3f attachOffset;
     s32 attachFlags;
     struct GdObj *attachedToObj;
     s32 netType;
     struct ObjNet *unk1F0;
     struct GdVec3f unk1F4;
     struct GdVec3f unk200;
     struct ObjGroup *unk20C;
     s32 ctrlType;
     u8  filler2[8];
     struct ObjGroup *unk21C;
};

struct ObjPlane {
     struct GdObj header;
     u32 id;
     s32 unk18;
     f32 unk1C;
     s32 unk20;
     s32 unk24;
     struct GdBoundingBox boundingBox;
     struct ObjFace* unk40;
};

struct ObjVertex {
     struct GdObj header;
     struct GdVec3f initPos;
     struct GdVec3f pos;
     struct GdVec3f normal;
     s16 id;
     f32 scaleFactor;
     f32 alpha;
     struct VtxLink *gbiVerts;
};

struct ObjFace {
     struct GdObj header;
     struct GdColour colour;
     s32 colourNum;
     struct GdVec3f normal;
     s32 vtxCount;
     struct ObjVertex *vertices[4];
     s32 mtlId;
     struct ObjMaterial *mtl;
};

#define CAMERA_FLAG_CONTROLLABLE 0x4

struct ObjCamera {
     struct GdObj header;
     struct GdVec3f worldPos;
     struct ObjCamera* prev;
     struct ObjCamera* next;
     s32 id;
     s32 flags;
     struct GdObj* unk30;
     struct GdVec3f lookAt;
     struct GdVec3f unk40;
     struct GdVec3f unk4C;
     f32 unk58;
     u8  filler[4];
     f32 unk60;
     Mat4f unk64;
     f32 unkA4;
     Mat4f unkA8;
     Mat4f unkE8;
     struct GdVec3f unk128;
     struct GdVec3f unk134;
     struct GdVec3f zoomPositions[4];
     s32 maxZoomLevel;
     s32 zoomLevel;
     f32 unk178;
     f32 unk17C;
     struct GdVec3f unk180;
     struct ObjView *unk18C;
};

enum GdMtlTypes {
    GD_MTL_STUB_DL = 0x01,
    GD_MTL_BREAK = 0x04,
    GD_MTL_SHINE_DL = 0x10,
    GD_MTL_TEX_OFF = 0x20,
    GD_MTL_LIGHTS = 0x40
};

struct ObjMaterial {
     struct GdObj header;
     u8  filler1[8];
     s32 id;
     char name[8];
     s32 type;
     u8  filler2[4];
     struct GdColour Ka;
     struct GdColour Kd;
     u8  filler3[16];
     void *texture;
     s32 gddlNumber;
};

struct ObjWeight {
     struct GdObj header;
     u8  filler1[8];
     s32 vtxId;
     struct GdVec3f vec20;
     u8  filler2[12];
     f32 weightVal;
     struct ObjVertex* vtx;
};

union ObjVarVal {
    s32 i;
    f32 f;
    u64 l;
};

struct ObjGadget {
     struct GdObj header;
     struct GdVec3f worldPos;
     s32 unk20;
     s32 type;
     f32 sliderPos;
     u8 filler1[4];
     union ObjVarVal varval;
     f32 rangeMin;
     f32 rangeMax;
     struct GdVec3f size;
     struct ObjGroup *valueGrp;
     struct ObjShape *shapePtr;
     struct ObjGroup *unk54;
     u8 filler2[4];
     s32 colourNum;
};

enum GdViewFlags {
    VIEW_2_COL_BUF      = 0x000008,
    VIEW_ALLOC_ZBUF     = 0x000010,
    VIEW_SAVE_TO_GLOBAL = 0x000040,
    VIEW_DEFAULT_PARENT = 0x000100,
    VIEW_BORDERED       = 0x000400,
    VIEW_UPDATE         = 0x000800,
    VIEW_UNK_1000       = 0x001000,
    VIEW_UNK_2000       = 0x002000,
    VIEW_UNK_4000       = 0x004000,
    VIEW_COLOUR_BUF     = 0x008000,
    VIEW_Z_BUF          = 0x010000,
    VIEW_1_CYCLE        = 0x020000,
    VIEW_MOVEMENT       = 0x040000,
    VIEW_DRAW           = 0x080000,
    VIEW_WAS_UPDATED    = 0x100000,
    VIEW_LIGHT          = 0x200000
};

struct ObjView {
     struct GdObj header;
     u8  filler1[8];
     s32 unk1C;
     s32 id;
     struct ObjCamera *activeCam;
     struct ObjGroup *components;
     struct ObjGroup *lights;
     struct GdObj *pickedObj;
     enum GdViewFlags flags;
     s32 projectionType;
     struct GdVec3f upperLeft;
     f32 unk48;
     f32 unk4C;
     u8  filler2[4];
     struct GdVec3f lowerRight;
     struct GdVec3f clipping;
     const char *namePtr;
     s32 gdDlNum;
     s32 unk74;
     s32 unk78;
     struct GdColour colour;
     struct ObjView *parent;
     void *zbuf;
     void *colourBufs[2];
     void (*proc)(struct ObjView *);
     s32 unk9C;
};

typedef union ObjVarVal * (*valptrproc_t)(union ObjVarVal *, union ObjVarVal);

struct ObjLabel {
     struct GdObj header;
     struct GdVec3f position;
     char *fmtstr;
     s32 unk24;
     struct ObjValPtr *valptr;
     valptrproc_t valfn;
     s32 unk30;
};

struct ObjAnimator {
     struct GdObj header;
     struct ObjGroup* animatedPartsGrp;
     struct ObjGroup* animdataGrp;
     u8  filler1[4];
     s32 animSeqNum;
     f32 unk24;
     f32 frame;
     u8  filler2[4];
     struct ObjGroup *attachedObjsGrp;
     s32 attachFlags;
     u8  filler3[12];
     struct GdObj* attachedToObj;
     void (*controlFunc) (struct ObjAnimator *);
     s32 state;
     s32 nods;
     s32 stillTimer;
};

enum GdAnimations {
    GD_ANIM_EMPTY                 = 0,
    GD_ANIM_MTX4x4                = 1,
    GD_ANIM_SCALE3F_ROT3F_POS3F   = 2,
    GD_ANIM_SCALE3S_POS3S_ROT3S   = 3,
    GD_ANIM_SCALE3F_ROT3F_POS3F_2 = 4,
    GD_ANIM_STUB                  = 5,
    GD_ANIM_ROT3S                 = 6,
    GD_ANIM_POS3S                 = 7,
    GD_ANIM_ROT3S_POS3S           = 8,
    GD_ANIM_MTX4x4F_SCALE3F       = 9,
    GD_ANIM_CAMERA_EYE3S_LOOKAT3S = 11
};

struct AnimDataInfo {
    s32 count;
    enum GdAnimations type;
    void* data;
};

struct AnimMtxVec {
     Mat4f matrix;
     struct GdVec3f vec;
};

enum ValPtrType {
    OBJ_VALUE_INT   = 1,
    OBJ_VALUE_FLOAT = 2
};

struct ObjValPtr {
     struct GdObj header;
     struct GdObj *obj;
     uintptr_t offset;
     enum ValPtrType datatype;
     s32 flag;
};

enum GdLightFlags {
    LIGHT_UNK02 = 0x02,
    LIGHT_NEW_UNCOUNTED = 0x10,
    LIGHT_UNK20 = 0x20
};

struct ObjLight {
     struct GdObj header;
     u8  filler1[8];
     s32 id;
     char name[8];
     u8  filler2[4];
     s32 flags;
     f32 unk30;
     u8  filler3[4];
     f32 unk38;
     s32 unk3C;
     s32 unk40;
     u8  filler4[8];
     s32 unk4C;
     struct GdColour diffuse;
     struct GdColour colour;
     struct GdVec3f unk68;
     struct GdVec3f position;
     struct GdVec3f unk80;
     struct GdVec3f unk8C;
     s32 unk98;
     struct ObjShape *unk9C;
};

struct ObjZone {
     struct GdObj header;
     struct GdBoundingBox boundingBox;
     struct ObjGroup *unk2C;
     struct ObjGroup *unk30;
     u8  filler[4];
};

struct ObjUnk200000 {
     struct GdObj header;
     u8  filler[28];
     struct ObjVertex *unk30;
     struct ObjFace *unk34;
};

#endif
