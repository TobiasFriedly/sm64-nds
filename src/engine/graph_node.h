#ifndef GRAPH_NODE_H
#define GRAPH_NODE_H

#include <PR/ultratypes.h>
#include <PR/gbi.h>

#include "types.h"
#include "game/memory.h"

#define GRAPH_RENDER_ACTIVE         (1 << 0)
#define GRAPH_RENDER_CHILDREN_FIRST (1 << 1)
#define GRAPH_RENDER_BILLBOARD      (1 << 2)
#define GRAPH_RENDER_Z_BUFFER       (1 << 3)
#define GRAPH_RENDER_INVISIBLE      (1 << 4)
#define GRAPH_RENDER_HAS_ANIMATION  (1 << 5)

#define GRAPH_NODE_TYPE_FUNCTIONAL            0x100

#define GRAPH_NODE_TYPE_400                   0x400

#define GRAPH_NODE_TYPE_ROOT                  0x001
#define GRAPH_NODE_TYPE_ORTHO_PROJECTION      0x002
#define GRAPH_NODE_TYPE_PERSPECTIVE          (0x003 | GRAPH_NODE_TYPE_FUNCTIONAL)
#define GRAPH_NODE_TYPE_MASTER_LIST           0x004
#define GRAPH_NODE_TYPE_START                 0x00A
#define GRAPH_NODE_TYPE_LEVEL_OF_DETAIL       0x00B
#define GRAPH_NODE_TYPE_SWITCH_CASE          (0x00C | GRAPH_NODE_TYPE_FUNCTIONAL)
#define GRAPH_NODE_TYPE_CAMERA               (0x014 | GRAPH_NODE_TYPE_FUNCTIONAL)
#define GRAPH_NODE_TYPE_TRANSLATION_ROTATION  0x015
#define GRAPH_NODE_TYPE_TRANSLATION           0x016
#define GRAPH_NODE_TYPE_ROTATION              0x017
#define GRAPH_NODE_TYPE_OBJECT                0x018
#define GRAPH_NODE_TYPE_ANIMATED_PART         0x019
#define GRAPH_NODE_TYPE_BILLBOARD             0x01A
#define GRAPH_NODE_TYPE_DISPLAY_LIST          0x01B
#define GRAPH_NODE_TYPE_SCALE                 0x01C
#define GRAPH_NODE_TYPE_SHADOW                0x028
#define GRAPH_NODE_TYPE_OBJECT_PARENT         0x029
#define GRAPH_NODE_TYPE_GENERATED_LIST       (0x02A | GRAPH_NODE_TYPE_FUNCTIONAL)
#define GRAPH_NODE_TYPE_BACKGROUND           (0x02C | GRAPH_NODE_TYPE_FUNCTIONAL)
#define GRAPH_NODE_TYPE_HELD_OBJ             (0x02E | GRAPH_NODE_TYPE_FUNCTIONAL)
#define GRAPH_NODE_TYPE_CULLING_RADIUS        0x02F

#define GFX_NUM_MASTER_LISTS 8

#define GEO_CONTEXT_CREATE        0
#define GEO_CONTEXT_RENDER        1
#define GEO_CONTEXT_AREA_UNLOAD   2
#define GEO_CONTEXT_AREA_LOAD     3
#define GEO_CONTEXT_AREA_INIT     4
#define GEO_CONTEXT_HELD_OBJ      5

typedef Gfx *(*GraphNodeFunc)(s32 callContext, struct GraphNode *node, void *context);

struct FnGraphNode {
     struct GraphNode node;
     GraphNodeFunc func;
};

struct GraphNodeRoot {
     struct GraphNode node;
     u8 areaIndex;
     s8 unk15;
     s16 x;
     s16 y;
     s16 width;
     s16 height;
     s16 numViews;
     struct GraphNode **views;
};

struct GraphNodeOrthoProjection {
     struct GraphNode node;
     f32 scale;
};

struct GraphNodePerspective {
     struct FnGraphNode fnNode;
     s32 unused;
     f32 fov;
     s16 near;
     s16 far;
};

struct DisplayListNode {
    Mtx *transform;
    void *displayList;
    struct DisplayListNode *next;
#if defined(TARGET_NDS) && defined(ARM9)
    u32 tint;
#endif
};

struct GraphNodeMasterList {
     struct GraphNode node;
     struct DisplayListNode *listHeads[GFX_NUM_MASTER_LISTS];
     struct DisplayListNode *listTails[GFX_NUM_MASTER_LISTS];
};

struct GraphNodeStart {
     struct GraphNode node;
};

struct GraphNodeLevelOfDetail {
     struct GraphNode node;
     s16 minDistance;
     s16 maxDistance;
};

struct GraphNodeSwitchCase {
     struct FnGraphNode fnNode;
     s32 unused;
     s16 numCases;
     s16 selectedCase;
};

struct GraphNodeCamera {
     struct FnGraphNode fnNode;
     union {

        s32 mode;
        struct Camera *camera;
    } config;
     Vec3f pos;
     Vec3f focus;
     Mat4 *matrixPtr;
     s16 roll;
     s16 rollScreen;
};

struct GraphNodeTranslationRotation {
     struct GraphNode node;
     void *displayList;
     Vec3s translation;
     Vec3s rotation;
};

struct GraphNodeTranslation {
     struct GraphNode node;
     void *displayList;
     Vec3s translation;
    u8 filler[2];
};

struct GraphNodeRotation {
     struct GraphNode node;
     void *displayList;
     Vec3s rotation;
    u8 filler[2];
};

struct GraphNodeAnimatedPart {
     struct GraphNode node;
     void *displayList;
     Vec3s translation;
};

struct GraphNodeBillboard {
     struct GraphNode node;
     void *displayList;
     Vec3s translation;
};

struct GraphNodeDisplayList {
     struct GraphNode node;
     void *displayList;
};

struct GraphNodeScale {
     struct GraphNode node;
     void *displayList;
     f32 scale;
};

struct GraphNodeShadow {
     struct GraphNode node;
     s16 shadowScale;
     u8 shadowSolidity;
     u8 shadowType;
};

struct GraphNodeObjectParent {
     struct GraphNode node;
     struct GraphNode *sharedChild;
};

struct GraphNodeGenerated {
     struct FnGraphNode fnNode;
     u32 parameter;
};

struct GraphNodeBackground {
     struct FnGraphNode fnNode;
     s32 unused;
     s32 background;
};

struct GraphNodeHeldObject {
     struct FnGraphNode fnNode;
     s32 playerIndex;
     struct Object *objNode;
     Vec3s translation;
};

struct GraphNodeCullingRadius {
     struct GraphNode node;
     s16 cullingRadius;
    u8 filler[2];
};

extern struct GraphNodeMasterList *gCurGraphNodeMasterList;
extern struct GraphNodePerspective *gCurGraphNodeCamFrustum;
extern struct GraphNodeCamera *gCurGraphNodeCamera;
extern struct GraphNodeHeldObject *gCurGraphNodeHeldObject;
extern u16 gAreaUpdateCounter;

extern struct GraphNode *gCurRootGraphNode;
extern struct GraphNode *gCurGraphNodeList[];

extern s16 gCurGraphNodeIndex;

extern Vec3f gVec3fZero;
extern Vec3s gVec3sZero;
extern Vec3f gVec3fOne;
extern Vec3s gVec3sOne;

void init_scene_graph_node_links(struct GraphNode *graphNode, s32 type);

struct GraphNodeRoot *init_graph_node_root(struct AllocOnlyPool *pool, struct GraphNodeRoot *graphNode,
                                           s16 areaIndex, s16 x, s16 y, s16 width, s16 height);
struct GraphNodeOrthoProjection *init_graph_node_ortho_projection(struct AllocOnlyPool *pool, struct GraphNodeOrthoProjection *graphNode, f32 scale);
struct GraphNodePerspective *init_graph_node_perspective(struct AllocOnlyPool *pool, struct GraphNodePerspective *graphNode,
                                                         f32 fov, s16 near, s16 far, GraphNodeFunc nodeFunc, s32 unused);
struct GraphNodeStart *init_graph_node_start(struct AllocOnlyPool *pool, struct GraphNodeStart *graphNode);
struct GraphNodeMasterList *init_graph_node_master_list(struct AllocOnlyPool *pool, struct GraphNodeMasterList *graphNode, s16 on);
struct GraphNodeLevelOfDetail *init_graph_node_render_range(struct AllocOnlyPool *pool, struct GraphNodeLevelOfDetail *graphNode,
                                                            s16 minDistance, s16 maxDistance);
struct GraphNodeSwitchCase *init_graph_node_switch_case(struct AllocOnlyPool *pool, struct GraphNodeSwitchCase *graphNode,
                                                        s16 numCases, s16 selectedCase, GraphNodeFunc nodeFunc, s32 unused);
struct GraphNodeCamera *init_graph_node_camera(struct AllocOnlyPool *pool, struct GraphNodeCamera *graphNode,
                                               f32 *pos, f32 *focus, GraphNodeFunc func, s32 mode);
struct GraphNodeTranslationRotation *init_graph_node_translation_rotation(struct AllocOnlyPool *pool, struct GraphNodeTranslationRotation *graphNode,
                                                                          s32 drawingLayer, void *displayList, Vec3s translation, Vec3s rotation);
struct GraphNodeTranslation *init_graph_node_translation(struct AllocOnlyPool *pool, struct GraphNodeTranslation *graphNode,
                                                         s32 drawingLayer, void *displayList, Vec3s translation);
struct GraphNodeRotation *init_graph_node_rotation(struct AllocOnlyPool *pool, struct GraphNodeRotation *graphNode,
                                                   s32 drawingLayer, void *displayList, Vec3s rotation);
struct GraphNodeScale *init_graph_node_scale(struct AllocOnlyPool *pool, struct GraphNodeScale *graphNode,
                                             s32 drawingLayer, void *displayList, f32 scale);
struct GraphNodeObject *init_graph_node_object(struct AllocOnlyPool *pool, struct GraphNodeObject *graphNode,
                                               struct GraphNode *sharedChild, Vec3f pos, Vec3s angle, Vec3f scale);
struct GraphNodeCullingRadius *init_graph_node_culling_radius(struct AllocOnlyPool *pool, struct GraphNodeCullingRadius *graphNode, s16 radius);
struct GraphNodeAnimatedPart *init_graph_node_animated_part(struct AllocOnlyPool *pool, struct GraphNodeAnimatedPart *graphNode,
                                                            s32 drawingLayer, void *displayList, Vec3s translation);
struct GraphNodeBillboard *init_graph_node_billboard(struct AllocOnlyPool *pool, struct GraphNodeBillboard *graphNode,
                                                     s32 drawingLayer, void *displayList, Vec3s translation);
struct GraphNodeDisplayList *init_graph_node_display_list(struct AllocOnlyPool *pool, struct GraphNodeDisplayList *graphNode,
                                                          s32 drawingLayer, void *displayList);
struct GraphNodeShadow *init_graph_node_shadow(struct AllocOnlyPool *pool, struct GraphNodeShadow *graphNode,
                                               s16 shadowScale, u8 shadowSolidity, u8 shadowType);
struct GraphNodeObjectParent *init_graph_node_object_parent(struct AllocOnlyPool *pool, struct GraphNodeObjectParent *sp1c,
                                                            struct GraphNode *sharedChild);
struct GraphNodeGenerated *init_graph_node_generated(struct AllocOnlyPool *pool, struct GraphNodeGenerated *sp1c,
                                                     GraphNodeFunc gfxFunc, s32 parameter);
struct GraphNodeBackground *init_graph_node_background(struct AllocOnlyPool *pool, struct GraphNodeBackground *sp1c,
                                                       u16 background, GraphNodeFunc backgroundFunc, s32 zero);
struct GraphNodeHeldObject *init_graph_node_held_object(struct AllocOnlyPool *pool, struct GraphNodeHeldObject *sp1c,
                                                        struct Object *objNode, Vec3s translation,
                                                        GraphNodeFunc nodeFunc, s32 playerIndex);
struct GraphNode *geo_add_child(struct GraphNode *parent, struct GraphNode *childNode);
struct GraphNode *geo_remove_child(struct GraphNode *graphNode);
struct GraphNode *geo_make_first_child(struct GraphNode *newFirstChild);

void geo_call_global_function_nodes_helper(struct GraphNode *graphNode, s32 callContext);
void geo_call_global_function_nodes(struct GraphNode *graphNode, s32 callContext);

void geo_reset_object_node(struct GraphNodeObject *graphNode);
void geo_obj_init(struct GraphNodeObject *graphNode, void *sharedChild, Vec3f pos, Vec3s angle);
void geo_obj_init_spawninfo(struct GraphNodeObject *graphNode, struct SpawnInfo *spawn);
void geo_obj_init_animation(struct GraphNodeObject *graphNode, struct Animation **animPtrAddr);
void geo_obj_init_animation_accel(struct GraphNodeObject *graphNode, struct Animation **animPtrAddr, u32 animAccel);

s32 retrieve_animation_index(s32 frame, u16 **attributes);

s16 geo_update_animation_frame(struct AnimInfo *obj, s32 *accelAssist);
void geo_retreive_animation_translation(struct GraphNodeObject *obj, Vec3f position);

struct GraphNodeRoot *geo_find_root(struct GraphNode *graphNode);

s16 *read_vec3s_to_vec3f(Vec3f, s16 *src);
s16 *read_vec3s(Vec3s dst, s16 *src);
s16 *read_vec3s_angle(Vec3s dst, s16 *src);
void register_scene_graph_node(struct GraphNode *graphNode);

#endif
