#include <ultra64.h>
#include "sm64.h"

#include "geo_layout.h"
#include "math_util.h"
#include "game/memory.h"
#include "graph_node.h"

typedef void (*GeoLayoutCommandProc)(void);

GeoLayoutCommandProc GeoLayoutJumpTable[] = {
    geo_layout_cmd_branch_and_link,
    geo_layout_cmd_end,
    geo_layout_cmd_branch,
    geo_layout_cmd_return,
    geo_layout_cmd_open_node,
    geo_layout_cmd_close_node,
    geo_layout_cmd_assign_as_view,
    geo_layout_cmd_update_node_flags,
    geo_layout_cmd_node_root,
    geo_layout_cmd_node_ortho_projection,
    geo_layout_cmd_node_perspective,
    geo_layout_cmd_node_start,
    geo_layout_cmd_node_master_list,
    geo_layout_cmd_node_level_of_detail,
    geo_layout_cmd_node_switch_case,
    geo_layout_cmd_node_camera,
    geo_layout_cmd_node_translation_rotation,
    geo_layout_cmd_node_translation,
    geo_layout_cmd_node_rotation,
    geo_layout_cmd_node_animated_part,
    geo_layout_cmd_node_billboard,
    geo_layout_cmd_node_display_list,
    geo_layout_cmd_node_shadow,
    geo_layout_cmd_node_object_parent,
    geo_layout_cmd_node_generated,
    geo_layout_cmd_node_background,
    geo_layout_cmd_nop,
    geo_layout_cmd_copy_view,
    geo_layout_cmd_node_held_obj,
    geo_layout_cmd_node_scale,
    geo_layout_cmd_nop2,
    geo_layout_cmd_nop3,
    geo_layout_cmd_node_culling_radius,
};

struct GraphNode gObjParentGraphNode;
struct AllocOnlyPool *gGraphNodePool;
struct GraphNode *gCurRootGraphNode;

UNUSED s32 D_8038BCA8;

struct GraphNode **gGeoViews;
u16 gGeoNumViews;

uintptr_t gGeoLayoutStack[16];
struct GraphNode *gCurGraphNodeList[32];
s16 gCurGraphNodeIndex;
s16 gGeoLayoutStackIndex;
UNUSED s16 D_8038BD7C;
s16 gGeoLayoutReturnIndex;
u8 *gGeoLayoutCommand;

u32 unused_8038B894[3] = { 0 };

void geo_layout_cmd_branch_and_link(void) {
    gGeoLayoutStack[gGeoLayoutStackIndex++] = (uintptr_t) (gGeoLayoutCommand + CMD_PROCESS_OFFSET(8));
    gGeoLayoutStack[gGeoLayoutStackIndex++] = (gCurGraphNodeIndex << 16) + gGeoLayoutReturnIndex;
    gGeoLayoutReturnIndex = gGeoLayoutStackIndex;
    gGeoLayoutCommand = segmented_to_virtual(cur_geo_cmd_ptr(0x04));
}

void geo_layout_cmd_end(void) {
    gGeoLayoutStackIndex = gGeoLayoutReturnIndex;
    gGeoLayoutReturnIndex = gGeoLayoutStack[--gGeoLayoutStackIndex] & 0xFFFF;
    gCurGraphNodeIndex = gGeoLayoutStack[gGeoLayoutStackIndex] >> 16;
    gGeoLayoutCommand = (u8 *) gGeoLayoutStack[--gGeoLayoutStackIndex];
}

void geo_layout_cmd_branch(void) {
    if (cur_geo_cmd_u8(0x01) == 1) {
        gGeoLayoutStack[gGeoLayoutStackIndex++] = (uintptr_t) (gGeoLayoutCommand + CMD_PROCESS_OFFSET(8));
    }

    gGeoLayoutCommand = segmented_to_virtual(cur_geo_cmd_ptr(0x04));
}

void geo_layout_cmd_return(void) {
    gGeoLayoutCommand = (u8 *) gGeoLayoutStack[--gGeoLayoutStackIndex];
}

void geo_layout_cmd_open_node(void) {
    gCurGraphNodeList[gCurGraphNodeIndex + 1] = gCurGraphNodeList[gCurGraphNodeIndex];
    gCurGraphNodeIndex++;
    gGeoLayoutCommand += 0x04 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_close_node(void) {
    gCurGraphNodeIndex--;
    gGeoLayoutCommand += 0x04 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_assign_as_view(void) {
    u16 index = cur_geo_cmd_s16(0x02);

    if (index < gGeoNumViews) {
        gGeoViews[index] = gCurGraphNodeList[gCurGraphNodeIndex];
    }

    gGeoLayoutCommand += 0x04 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_update_node_flags(void) {
    u16 operation = cur_geo_cmd_u8(0x01);
    u16 flagBits = cur_geo_cmd_s16(0x02);

    switch (operation) {
        case GEO_CMD_FLAGS_RESET:
            gCurGraphNodeList[gCurGraphNodeIndex]->flags = flagBits;
            break;
        case GEO_CMD_FLAGS_SET:
            gCurGraphNodeList[gCurGraphNodeIndex]->flags |= flagBits;
            break;
        case GEO_CMD_FLAGS_CLEAR:
            gCurGraphNodeList[gCurGraphNodeIndex]->flags &= ~flagBits;
            break;
    }

    gGeoLayoutCommand += 0x04 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_node_root(void) {
    s32 i;
    struct GraphNodeRoot *graphNode;

    s16 x = cur_geo_cmd_s16(0x04);
    s16 y = cur_geo_cmd_s16(0x06);
    s16 width = cur_geo_cmd_s16(0x08);
    s16 height = cur_geo_cmd_s16(0x0A);

    gGeoNumViews = cur_geo_cmd_s16(0x02) + 2;

    graphNode = init_graph_node_root(gGraphNodePool, NULL, 0, x, y, width, height);

    gGeoViews = alloc_only_pool_alloc(gGraphNodePool, gGeoNumViews * sizeof(struct GraphNode *));

    graphNode->views = gGeoViews;
    graphNode->numViews = gGeoNumViews;

    for (i = 0; i < gGeoNumViews; i++) {
        gGeoViews[i] = NULL;
    }

    register_scene_graph_node(&graphNode->node);

    gGeoLayoutCommand += 0x0C << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_node_ortho_projection(void) {
    struct GraphNodeOrthoProjection *graphNode;
    f32 scale = (f32) cur_geo_cmd_s16(0x02) / 100.0f;

    graphNode = init_graph_node_ortho_projection(gGraphNodePool, NULL, scale);

    register_scene_graph_node(&graphNode->node);

    gGeoLayoutCommand += 0x04 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_node_perspective(void) {
    struct GraphNodePerspective *graphNode;
    GraphNodeFunc frustumFunc = NULL;
    s16 fov = cur_geo_cmd_s16(0x02);
    s16 near = cur_geo_cmd_s16(0x04);
    s16 far = cur_geo_cmd_s16(0x06);

    if (cur_geo_cmd_u8(0x01) != 0) {

        frustumFunc = (GraphNodeFunc) cur_geo_cmd_ptr(0x08);
        gGeoLayoutCommand += 4 << CMD_SIZE_SHIFT;
    }

    graphNode = init_graph_node_perspective(gGraphNodePool, NULL, (f32) fov, near, far, frustumFunc, 0);

    register_scene_graph_node(&graphNode->fnNode.node);

    gGeoLayoutCommand += 0x08 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_node_start(void) {
    struct GraphNodeStart *graphNode;

    graphNode = init_graph_node_start(gGraphNodePool, NULL);

    register_scene_graph_node(&graphNode->node);

    gGeoLayoutCommand += 0x04 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_nop3(void) {
    gGeoLayoutCommand += 0x10 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_node_master_list(void) {
    struct GraphNodeMasterList *graphNode;

    graphNode = init_graph_node_master_list(gGraphNodePool, NULL, cur_geo_cmd_u8(0x01));

    register_scene_graph_node(&graphNode->node);

    gGeoLayoutCommand += 0x04 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_node_level_of_detail(void) {
    struct GraphNodeLevelOfDetail *graphNode;
    s16 minDistance = cur_geo_cmd_s16(0x04);
    s16 maxDistance = cur_geo_cmd_s16(0x06);

    graphNode = init_graph_node_render_range(gGraphNodePool, NULL, minDistance, maxDistance);

    register_scene_graph_node(&graphNode->node);

    gGeoLayoutCommand += 0x08 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_node_switch_case(void) {
    struct GraphNodeSwitchCase *graphNode;

    graphNode =
        init_graph_node_switch_case(gGraphNodePool, NULL,
                                    cur_geo_cmd_s16(0x02),
                                    0,
                                    (GraphNodeFunc) cur_geo_cmd_ptr(0x04),
                                    0);

    register_scene_graph_node(&graphNode->fnNode.node);

    gGeoLayoutCommand += 0x08 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_node_camera(void) {
    struct GraphNodeCamera *graphNode;
    s16 *cmdPos = (s16 *) &gGeoLayoutCommand[4];

    Vec3f pos, focus;

    cmdPos = read_vec3s_to_vec3f(pos, cmdPos);
    cmdPos = read_vec3s_to_vec3f(focus, cmdPos);

    graphNode = init_graph_node_camera(gGraphNodePool, NULL, pos, focus,
                                       (GraphNodeFunc) cur_geo_cmd_ptr(0x10), cur_geo_cmd_s16(0x02));

    register_scene_graph_node(&graphNode->fnNode.node);

    gGeoViews[0] = &graphNode->fnNode.node;

    gGeoLayoutCommand += 0x14 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_node_translation_rotation(void) {
    struct GraphNodeTranslationRotation *graphNode;

    Vec3s translation, rotation;

    void *displayList = NULL;
    s16 drawingLayer = 0;

    s16 params = cur_geo_cmd_u8(0x01);
    s16 *cmdPos = (s16 *) gGeoLayoutCommand;

    switch ((params & 0x70) >> 4) {
        case 0:
            cmdPos = read_vec3s(translation, &cmdPos[2]);
            cmdPos = read_vec3s_angle(rotation, cmdPos);
            break;
        case 1:
            cmdPos = read_vec3s(translation, &cmdPos[1]);
            vec3s_copy(rotation, gVec3sZero);
            break;
        case 2:
            cmdPos = read_vec3s_angle(rotation, &cmdPos[1]);
            vec3s_copy(translation, gVec3sZero);
            break;
        case 3:
            vec3s_copy(translation, gVec3sZero);
            vec3s_set(rotation, 0, (cmdPos[1] << 15) / 180, 0);
            cmdPos += 2 << CMD_SIZE_SHIFT;
            break;
    }

    if (params & 0x80) {
        displayList = *(void **) &cmdPos[0];
        drawingLayer = params & 0x0F;
        cmdPos += 2 << CMD_SIZE_SHIFT;
    }

    graphNode = init_graph_node_translation_rotation(gGraphNodePool, NULL, drawingLayer, displayList,
                                                     translation, rotation);
    register_scene_graph_node(&graphNode->node);

    gGeoLayoutCommand = (u8 *) cmdPos;
}

void geo_layout_cmd_node_translation(void) {
    struct GraphNodeTranslation *graphNode;

    Vec3s translation;

    s16 drawingLayer = 0;
    s16 params = cur_geo_cmd_u8(0x01);
    s16 *cmdPos = (s16 *) gGeoLayoutCommand;
    void *displayList = NULL;

    cmdPos = read_vec3s(translation, &cmdPos[1]);

    if (params & 0x80) {
        displayList = *(void **) &cmdPos[0];
        drawingLayer = params & 0x0F;
        cmdPos += 2 << CMD_SIZE_SHIFT;
    }

    graphNode =
        init_graph_node_translation(gGraphNodePool, NULL, drawingLayer, displayList, translation);

    register_scene_graph_node(&graphNode->node);

    gGeoLayoutCommand = (u8 *) cmdPos;
}

void geo_layout_cmd_node_rotation(void) {
    struct GraphNodeRotation *graphNode;

    Vec3s sp2c;

    s16 drawingLayer = 0;
    s16 params = cur_geo_cmd_u8(0x01);
    s16 *cmdPos = (s16 *) gGeoLayoutCommand;
    void *displayList = NULL;

    cmdPos = read_vec3s_angle(sp2c, &cmdPos[1]);

    if (params & 0x80) {
        displayList = *(void **) &cmdPos[0];
        drawingLayer = params & 0x0F;
        cmdPos += 2 << CMD_SIZE_SHIFT;
    }

    graphNode = init_graph_node_rotation(gGraphNodePool, NULL, drawingLayer, displayList, sp2c);

    register_scene_graph_node(&graphNode->node);

    gGeoLayoutCommand = (u8 *) cmdPos;
}

void geo_layout_cmd_node_scale(void) {
    struct GraphNodeScale *graphNode;

    s16 drawingLayer = 0;
    s16 params = cur_geo_cmd_u8(0x01);
    f32 scale = cur_geo_cmd_u32(0x04) / 65536.0f;
    void *displayList = NULL;

    if (params & 0x80) {
        displayList = cur_geo_cmd_ptr(0x08);
        drawingLayer = params & 0x0F;
        gGeoLayoutCommand += 4 << CMD_SIZE_SHIFT;
    }

    graphNode = init_graph_node_scale(gGraphNodePool, NULL, drawingLayer, displayList, scale);

    register_scene_graph_node(&graphNode->node);

    gGeoLayoutCommand += 0x08 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_nop2(void) {
    gGeoLayoutCommand += 0x08 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_node_animated_part(void) {
    struct GraphNodeAnimatedPart *graphNode;
    Vec3s translation;
    s32 drawingLayer = cur_geo_cmd_u8(0x01);
    void *displayList = cur_geo_cmd_ptr(0x08);
    s16 *cmdPos = (s16 *) gGeoLayoutCommand;

    read_vec3s(translation, &cmdPos[1]);

    graphNode =
        init_graph_node_animated_part(gGraphNodePool, NULL, drawingLayer, displayList, translation);

    register_scene_graph_node(&graphNode->node);

    gGeoLayoutCommand += 0x0C << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_node_billboard(void) {
    struct GraphNodeBillboard *graphNode;
    Vec3s translation;
    s16 drawingLayer = 0;
    s16 params = cur_geo_cmd_u8(0x01);
    s16 *cmdPos = (s16 *) gGeoLayoutCommand;
    void *displayList = NULL;

    cmdPos = read_vec3s(translation, &cmdPos[1]);

    if (params & 0x80) {
        displayList = *(void **) &cmdPos[0];
        drawingLayer = params & 0x0F;
        cmdPos += 2 << CMD_SIZE_SHIFT;
    }

    graphNode = init_graph_node_billboard(gGraphNodePool, NULL, drawingLayer, displayList, translation);

    register_scene_graph_node(&graphNode->node);

    gGeoLayoutCommand = (u8 *) cmdPos;
}

void geo_layout_cmd_node_display_list(void) {
    struct GraphNodeDisplayList *graphNode;
    s32 drawingLayer = cur_geo_cmd_u8(0x01);
    void *displayList = cur_geo_cmd_ptr(0x04);

    graphNode = init_graph_node_display_list(gGraphNodePool, NULL, drawingLayer, displayList);

    register_scene_graph_node(&graphNode->node);

    gGeoLayoutCommand += 0x08 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_node_shadow(void) {
    struct GraphNodeShadow *graphNode;
    u8 shadowType = cur_geo_cmd_s16(0x02);
    u8 shadowSolidity = cur_geo_cmd_s16(0x04);
    s16 shadowScale = cur_geo_cmd_s16(0x06);

    graphNode = init_graph_node_shadow(gGraphNodePool, NULL, shadowScale, shadowSolidity, shadowType);

    register_scene_graph_node(&graphNode->node);

    gGeoLayoutCommand += 0x08 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_node_object_parent(void) {
    struct GraphNodeObjectParent *graphNode;

    graphNode = init_graph_node_object_parent(gGraphNodePool, NULL, &gObjParentGraphNode);

    register_scene_graph_node(&graphNode->node);

    gGeoLayoutCommand += 0x04 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_node_generated(void) {
    struct GraphNodeGenerated *graphNode;

    graphNode = init_graph_node_generated(gGraphNodePool, NULL,
                                          (GraphNodeFunc) cur_geo_cmd_ptr(0x04),
                                          cur_geo_cmd_s16(0x02));

    register_scene_graph_node(&graphNode->fnNode.node);

    gGeoLayoutCommand += 0x08 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_node_background(void) {
    struct GraphNodeBackground *graphNode;

    graphNode = init_graph_node_background(
        gGraphNodePool, NULL,
        cur_geo_cmd_s16(0x02),
        (GraphNodeFunc) cur_geo_cmd_ptr(0x04),
        0);

    register_scene_graph_node(&graphNode->fnNode.node);

    gGeoLayoutCommand += 0x08 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_nop(void) {
    gGeoLayoutCommand += 0x08 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_copy_view(void) {
    struct GraphNodeObjectParent *graphNode;
    struct GraphNode *node = NULL;
    s16 index = cur_geo_cmd_s16(0x02);

    if (index >= 0) {
        node = gGeoViews[index];

        if (node->type == GRAPH_NODE_TYPE_OBJECT_PARENT) {
            node = ((struct GraphNodeObjectParent *) node)->sharedChild;
        } else {
            node = NULL;
        }
    }

    graphNode = init_graph_node_object_parent(gGraphNodePool, NULL, node);

    register_scene_graph_node(&graphNode->node);

    gGeoLayoutCommand += 0x04 << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_node_held_obj(void) {
    struct GraphNodeHeldObject *graphNode;
    Vec3s offset;

    read_vec3s(offset, (s16 *) &gGeoLayoutCommand[0x02]);

    graphNode = init_graph_node_held_object(
        gGraphNodePool, NULL, NULL, offset, (GraphNodeFunc) cur_geo_cmd_ptr(0x08), cur_geo_cmd_u8(0x01));

    register_scene_graph_node(&graphNode->fnNode.node);

    gGeoLayoutCommand += 0x0C << CMD_SIZE_SHIFT;
}

void geo_layout_cmd_node_culling_radius(void) {
    struct GraphNodeCullingRadius *graphNode;
    graphNode = init_graph_node_culling_radius(gGraphNodePool, NULL, cur_geo_cmd_s16(0x02));
    register_scene_graph_node(&graphNode->node);
    gGeoLayoutCommand += 0x04 << CMD_SIZE_SHIFT;
}

struct GraphNode *process_geo_layout(struct AllocOnlyPool *pool, void *segptr) {

    gCurRootGraphNode = NULL;

    gGeoNumViews = 0;

    gCurGraphNodeList[0] = 0;
    gCurGraphNodeIndex = 0;

    gGeoLayoutStackIndex = 2;
    gGeoLayoutReturnIndex = 2;

    gGeoLayoutCommand = segmented_to_virtual(segptr);

    gGraphNodePool = pool;

    gGeoLayoutStack[0] = 0;
    gGeoLayoutStack[1] = 0;

    while (gGeoLayoutCommand != NULL) {
        GeoLayoutJumpTable[gGeoLayoutCommand[0x00]]();
    }

    return gCurRootGraphNode;
}
