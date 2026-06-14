#include <PR/ultratypes.h>

#include "audio/external.h"
#include "engine/geo_layout.h"
#include "engine/graph_node.h"
#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "level_table.h"
#include "object_constants.h"
#include "object_fields.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "spawn_object.h"
#include "types.h"

struct LinkedList {
    struct LinkedList *next;
    struct LinkedList *prev;
};

void unused_init_free_list(struct LinkedList *usedList, struct LinkedList **pFreeList,
                           struct LinkedList *pool, s32 itemSize, s32 poolLength) {
    s32 i;
    struct LinkedList *node = pool;

    usedList->next = usedList;
    usedList->prev = usedList;

    *pFreeList = pool;

    for (i = 0; i < poolLength - 1; i++) {

        node = (struct LinkedList *) ((u8 *) node + itemSize);
        pool->next = node;
        pool = node;
    }

    pool->next = NULL;
}

struct LinkedList *unused_try_allocate(struct LinkedList *destList,
                                       struct LinkedList *freeList) {
    struct LinkedList *node = freeList->next;

    if (node != NULL) {

        freeList->next = node->next;

        node->prev = destList->prev;
        node->next = destList;
        destList->prev->next = node;
        destList->prev = node;
    }

    return node;
}

struct Object *try_allocate_object(struct ObjectNode *destList, struct ObjectNode *freeList) {
    struct ObjectNode *nextObj;

    if ((nextObj = freeList->next) != NULL) {

        freeList->next = nextObj->next;

        nextObj->prev = destList->prev;
        nextObj->next = destList;
        destList->prev->next = nextObj;
        destList->prev = nextObj;
    } else {
        return NULL;
    }

    geo_remove_child(&nextObj->gfx.node);
    geo_add_child(&gObjParentGraphNode, &nextObj->gfx.node);

    return (struct Object *) nextObj;
}

void unused_deallocate(struct LinkedList *freeList, struct LinkedList *node) {

    node->next->prev = node->prev;
    node->prev->next = node->next;

    node->next = freeList->next;
    freeList->next = node;
}

static void deallocate_object(struct ObjectNode *freeList, struct ObjectNode *obj) {

    obj->next->prev = obj->prev;
    obj->prev->next = obj->next;

    obj->next = freeList->next;
    freeList->next = obj;
}

void init_free_object_list(void) {
    s32 i;
    s32 poolLength = OBJECT_POOL_CAPACITY;

    struct Object *obj = &gObjectPool[0];
    gFreeObjectList.next = (struct ObjectNode *) obj;

    for (i = 0; i < poolLength - 1; i++) {
        obj->header.next = &(obj + 1)->header;
        obj++;
    }

    obj->header.next = NULL;
}

void clear_object_lists(struct ObjectNode *objLists) {
    s32 i;

    for (i = 0; i < NUM_OBJ_LISTS; i++) {
        objLists[i].next = &objLists[i];
        objLists[i].prev = &objLists[i];
    }
}

UNUSED static void unused_delete_leaf_nodes(struct Object *obj) {
    struct Object *children;
    struct Object *sibling;
    struct Object *obj0 = obj;

    if ((children = (struct Object *) obj->header.gfx.node.children) != NULL) {
        unused_delete_leaf_nodes(children);
    } else {

        mark_obj_for_deletion(obj);
    }

    while ((sibling = (struct Object *) obj->header.gfx.node.next) == obj0) {
        unused_delete_leaf_nodes(sibling);
        obj = (struct Object *) sibling->header.gfx.node.next;
    }
}

void unload_object(struct Object *obj) {
    obj->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    obj->prevObj = NULL;

    obj->header.gfx.throwMatrix = NULL;
    stop_sounds_from_source(obj->header.gfx.cameraToObject);
    geo_remove_child(&obj->header.gfx.node);
    geo_add_child(&gObjParentGraphNode, &obj->header.gfx.node);

    obj->header.gfx.node.flags &= ~GRAPH_RENDER_BILLBOARD;
    obj->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;

    deallocate_object(&gFreeObjectList, &obj->header);
}

struct Object *allocate_object(struct ObjectNode *objList) {
    s32 i;
    struct Object *obj = try_allocate_object(objList, &gFreeObjectList);

    if (obj == NULL) {

        struct Object *unimportantObj = find_unimportant_object();

        if (unimportantObj == NULL) {

            while (TRUE) {
            }
        } else {

            unload_object(unimportantObj);
            obj = try_allocate_object(objList, &gFreeObjectList);
            if (gCurrentObject == obj) {

            }
        }
    }

    obj->activeFlags = ACTIVE_FLAG_ACTIVE | ACTIVE_FLAG_UNK8;
    obj->parentObj = obj;
    obj->prevObj = NULL;
    obj->collidedObjInteractTypes = 0;
    obj->numCollidedObjs = 0;

#if IS_64_BIT
    for (i = 0; i < 0x50; i++) {
        obj->rawData.asS32[i] = 0;
        obj->ptrData.asVoidPtr[i] = NULL;
    }
#else

    for (i = 0; i < 0x50; i++) obj->rawData.asS32[i] = 0;
#endif

    obj->unused1 = 0;
    obj->bhvStackIndex = 0;
    obj->bhvDelayTimer = 0;

    obj->hitboxRadius = 50.0f;
    obj->hitboxHeight = 100.0f;
    obj->hurtboxRadius = 0.0f;
    obj->hurtboxHeight = 0.0f;
    obj->hitboxDownOffset = 0.0f;
    obj->unused2 = 0;

    obj->platform = NULL;
    obj->collisionData = NULL;
    obj->oIntangibleTimer = -1;
    obj->oDamageOrCoinValue = 0;
    obj->oHealth = 2048;

    obj->oCollisionDistance = 1000.0f;
    if (gCurrLevelNum == LEVEL_TTC) {
        obj->oDrawingDistance = 2000.0f;
    } else {
        obj->oDrawingDistance = 4000.0f;
    }

    mtxf_identity(obj->transform);

    obj->respawnInfoType = RESPAWN_INFO_TYPE_NULL;
    obj->respawnInfo = NULL;

    obj->oDistanceToMario = 19000.0f;
    obj->oRoom = -1;

    obj->header.gfx.node.flags &= ~GRAPH_RENDER_INVISIBLE;
    obj->header.gfx.pos[0] = -10000.0f;
    obj->header.gfx.pos[1] = -10000.0f;
    obj->header.gfx.pos[2] = -10000.0f;
    obj->header.gfx.throwMatrix = NULL;

    return obj;
}

static void snap_object_to_floor(struct Object *obj) {
    struct Surface *surface;

    obj->oFloorHeight = find_floor(obj->oPosX, obj->oPosY, obj->oPosZ, &surface);

    if (obj->oFloorHeight + 2.0f > obj->oPosY && obj->oPosY > obj->oFloorHeight - 10.0f) {
        obj->oPosY = obj->oFloorHeight;
        obj->oMoveFlags |= OBJ_MOVE_ON_GROUND;
    }
}

struct Object *create_object(const BehaviorScript *bhvScript) {
    s32 objListIndex;
    struct Object *obj;
    struct ObjectNode *objList;
    const BehaviorScript *behavior = bhvScript;

    if ((bhvScript[0] >> 24) == 0) {
        objListIndex = (bhvScript[0] >> 16) & 0xFFFF;
    } else {
        objListIndex = OBJ_LIST_DEFAULT;
    }

    objList = &gObjectLists[objListIndex];
    obj = allocate_object(objList);

    obj->curBhvCommand = bhvScript;
    obj->behavior = behavior;

    if (objListIndex == OBJ_LIST_UNIMPORTANT) {
        obj->activeFlags |= ACTIVE_FLAG_UNIMPORTANT;
    }

    switch (objListIndex) {
        case OBJ_LIST_GENACTOR:
        case OBJ_LIST_PUSHABLE:
        case OBJ_LIST_POLELIKE:
            snap_object_to_floor(obj);
            break;
        default:
            break;
    }

    return obj;
}

void mark_obj_for_deletion(struct Object *obj) {

    obj->activeFlags = ACTIVE_FLAG_DEACTIVATED;
}
