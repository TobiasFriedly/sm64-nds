#include <PR/ultratypes.h>

#include "types.h"

#include "graph_node.h"

#if IS_64_BIT
static s16 next_s16_in_geo_script(s16 **src) {
    s16 ret;
    if (((uintptr_t)(*src) & 7) == 4) {
         *src += 2;
    }
    ret = *(*src)++;
    if (((uintptr_t)(*src) & 7) == 4) {
         *src += 2;
    }
    return ret;
}
#else
#define next_s16_in_geo_script(src) (*(*src)++)
#endif

s16 *read_vec3s_to_vec3f(Vec3f dst, s16 *src) {
    dst[0] = next_s16_in_geo_script(&src);
    dst[1] = next_s16_in_geo_script(&src);
    dst[2] = next_s16_in_geo_script(&src);
    return src;
}

s16 *read_vec3s(Vec3s dst, s16 *src) {
    dst[0] = next_s16_in_geo_script(&src);
    dst[1] = next_s16_in_geo_script(&src);
    dst[2] = next_s16_in_geo_script(&src);
    return src;
}

s16 *read_vec3s_angle(Vec3s dst, s16 *src) {
    dst[0] = (next_s16_in_geo_script(&src) << 15) / 180;
    dst[1] = (next_s16_in_geo_script(&src) << 15) / 180;
    dst[2] = (next_s16_in_geo_script(&src) << 15) / 180;
    return src;
}

void register_scene_graph_node(struct GraphNode *graphNode) {
    if (graphNode != NULL) {
        gCurGraphNodeList[gCurGraphNodeIndex] = graphNode;

        if (gCurGraphNodeIndex == 0) {
            if (gCurRootGraphNode == NULL) {
                gCurRootGraphNode = graphNode;
            }
        } else {
            if (gCurGraphNodeList[gCurGraphNodeIndex - 1]->type == GRAPH_NODE_TYPE_OBJECT_PARENT) {
                ((struct GraphNodeObjectParent *) gCurGraphNodeList[gCurGraphNodeIndex - 1])
                    ->sharedChild = graphNode;
            } else {
                geo_add_child(gCurGraphNodeList[gCurGraphNodeIndex - 1], graphNode);
            }
        }
    }
}
