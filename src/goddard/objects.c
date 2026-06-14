#include <PR/ultratypes.h>
#include <stdarg.h>
#include <stdio.h>

#include "objects.h"

#include "debug_utils.h"
#include "draw_objects.h"
#include "dynlist_proc.h"
#include "gd_macros.h"
#include "gd_main.h"
#include "gd_math.h"
#include "gd_types.h"
#include "joints.h"
#include "macros.h"
#include "old_menu.h"
#include "particles.h"
#include "renderer.h"
#include "sfx.h"
#include "shape_helper.h"
#include "skin.h"

struct Unk801B9E68 {
     s32 count;
     u8 filler[20];
};

struct Unk8017F3CC {
     u8 filler[32];
     struct GdVec3f unk20;
};

f32 D_801A81C0 = 0.0f;
f32 D_801A81C4 = 0.0f;

struct GdBoundingBox gSomeBoundingBox;
struct ObjCamera *sCurrentMoveCamera;
struct ObjView *sCurrentMoveView;
struct DebugCounters gGdCounter;
Mat4f D_801B9DC8;
struct GdVec3f D_801B9E08;
struct ObjGroup *sCurrentMoveGrp;
struct GdVec3f D_801B9E18;
struct GdVec3f D_801B9E28;
f32 D_801B9E34;
Mat4f *D_801B9E38;
struct ObjParticle *D_801B9E3C;
s32 D_801B9E40;
s32 D_801B9E44;
Mat4f *D_801B9E48;
struct ObjCamera *gGdCameraList;
void *D_801B9E50;
struct ObjGroup *gGdGroupList;
s32 gGdObjCount;
s32 gGdGroupCount;
s32 gGdPlaneCount;
s32 gGdCameraCount;
struct Unk801B9E68 sGdViewInfo;
void *D_801B9E80;
struct ObjJoint *gGdJointList;
struct ObjBone *gGdBoneList;
struct GdObj *gGdObjectList;
struct ObjGroup *gGdViewsGroup;

void reset_bounding_box(void) {
    gSomeBoundingBox.minX = 10000000.0f;
    gSomeBoundingBox.minY = 10000000.0f;
    gSomeBoundingBox.minZ = 10000000.0f;

    gSomeBoundingBox.maxX = -10000000.0f;
    gSomeBoundingBox.maxY = -10000000.0f;
    gSomeBoundingBox.maxZ = -10000000.0f;
}

void add_obj_pos_to_bounding_box(struct GdObj *obj) {
    struct GdVec3f pos;

    set_cur_dynobj(obj);
    d_get_world_pos(&pos);

    if (pos.x < gSomeBoundingBox.minX) {
        gSomeBoundingBox.minX = pos.x;
    }

    if (pos.y < gSomeBoundingBox.minY) {
        gSomeBoundingBox.minY = pos.y;
    }

    if (pos.z < gSomeBoundingBox.minZ) {
        gSomeBoundingBox.minZ = pos.z;
    }

    if (pos.x > gSomeBoundingBox.maxX) {
        gSomeBoundingBox.maxX = pos.x;
    }

    if (pos.y > gSomeBoundingBox.maxY) {
        gSomeBoundingBox.maxY = pos.y;
    }

    if (pos.z > gSomeBoundingBox.maxZ) {
        gSomeBoundingBox.maxZ = pos.z;
    }
}

void get_some_bounding_box(struct GdBoundingBox *a0) {
    a0->minX = gSomeBoundingBox.minX;
    a0->minY = gSomeBoundingBox.minY;
    a0->minZ = gSomeBoundingBox.minZ;

    a0->maxX = gSomeBoundingBox.maxX;
    a0->maxY = gSomeBoundingBox.maxY;
    a0->maxZ = gSomeBoundingBox.maxZ;
}

void stub_objects_1(UNUSED struct ObjGroup *a0, UNUSED struct GdObj *a1) {
    UNUSED u8 filler[8];

    return;
}

static const char *get_obj_name_str(enum ObjTypeFlag objFlag) {
    const char *objName;
    switch (objFlag) {
        case OBJ_TYPE_JOINTS:
            objName = "joints";
            break;
        case OBJ_TYPE_BONES:
            objName = "bones";
            break;
        case OBJ_TYPE_GROUPS:
            objName = "groups";
            break;
        case OBJ_TYPE_PARTICLES:
            objName = "particles";
            break;
        case OBJ_TYPE_SHAPES:
            objName = "shapes";
            break;
        case OBJ_TYPE_NETS:
            objName = "nets";
            break;
        case OBJ_TYPE_PLANES:
            objName = "planes";
            break;
        case OBJ_TYPE_VERTICES:
            objName = "vertices";
            break;
        case OBJ_TYPE_CAMERAS:
            objName = "cameras";
            break;
        case OBJ_TYPE_FACES:
            objName = "faces";
            break;
        case OBJ_TYPE_MATERIALS:
            objName = "materials";
            break;
        case OBJ_TYPE_LIGHTS:
            objName = "lights";
            break;
        case OBJ_TYPE_WEIGHTS:
            objName = "weights";
            break;
        case OBJ_TYPE_GADGETS:
            objName = "gadgets";
            break;
        case OBJ_TYPE_VIEWS:
            objName = "views";
            break;
        case OBJ_TYPE_LABELS:
            objName = "labels";
            break;
        case OBJ_TYPE_ANIMATORS:
            objName = "animators";
            break;
        case OBJ_TYPE_VALPTRS:
            objName = "valptrs";
            break;
        case OBJ_TYPE_ZONES:
            objName = "zones";
            break;
        default:
            objName = "unkown";
            break;
    }
    return objName;
}

struct GdObj *make_object(enum ObjTypeFlag objType) {
    struct GdObj *newObj;
    struct GdObj *objListOldHead;
    s32 objSize;
    s32 i;
    drawmethod_t objDrawFn;
    const char *typeName;
    u8 *newObjBytes;
    s32 objPermanence = 0x10;

    imin("make_object");

    switch (objType) {
        case OBJ_TYPE_JOINTS:
            objSize = sizeof(struct ObjJoint);
            objDrawFn = (drawmethod_t) draw_joint;
            break;
        case OBJ_TYPE_BONES:
            objSize = sizeof(struct ObjBone);
            objDrawFn = (drawmethod_t) draw_bone;
            break;
        case OBJ_TYPE_GROUPS:
            objSize = sizeof(struct ObjGroup);
            objDrawFn = (drawmethod_t) draw_group;
            break;
        case OBJ_TYPE_PARTICLES:
            objSize = sizeof(struct ObjParticle);
            objDrawFn = (drawmethod_t) draw_particle;
            break;
        case OBJ_TYPE_SHAPES:
            objSize = sizeof(struct ObjShape);

            objDrawFn = (drawmethod_t) draw_nothing;
            break;
        case OBJ_TYPE_UNK200000:
            objSize = sizeof(struct ObjUnk200000);
            objDrawFn = (drawmethod_t) draw_nothing;
            break;
        case OBJ_TYPE_NETS:
            objSize = sizeof(struct ObjNet);
            objDrawFn = (drawmethod_t) draw_net;
            break;
        case OBJ_TYPE_PLANES:
            objSize = sizeof(struct ObjPlane);
            objDrawFn = (drawmethod_t) draw_plane;
            break;
        case OBJ_TYPE_VERTICES:
            objSize = sizeof(struct ObjVertex);
            objDrawFn = (drawmethod_t) draw_nothing;
            break;
        case OBJ_TYPE_CAMERAS:
            objSize = sizeof(struct ObjCamera);
            objDrawFn = (drawmethod_t) draw_camera;
            break;
        case OBJ_TYPE_FACES:
            objSize = sizeof(struct ObjFace);
            objDrawFn = (drawmethod_t) draw_face;
            objPermanence = 1;
            break;
        case OBJ_TYPE_MATERIALS:
            objSize = sizeof(struct ObjMaterial);
            objDrawFn = (drawmethod_t) draw_material;
            break;
        case OBJ_TYPE_LIGHTS:
            objSize = sizeof(struct ObjLight);
            objDrawFn = (drawmethod_t) draw_light;
            break;
        case OBJ_TYPE_WEIGHTS:
            objSize = sizeof(struct ObjWeight);
            objDrawFn = (drawmethod_t) draw_nothing;
            break;
        case OBJ_TYPE_GADGETS:
            objSize = sizeof(struct ObjGadget);
            objDrawFn = (drawmethod_t) draw_gadget;
            break;
        case OBJ_TYPE_VIEWS:
            objSize = sizeof(struct ObjView);
            objDrawFn = (drawmethod_t) draw_nothing;
            break;
        case OBJ_TYPE_LABELS:
            objSize = sizeof(struct ObjLabel);
            objDrawFn = (drawmethod_t) draw_label;
            break;
        case OBJ_TYPE_ANIMATORS:
            objSize = sizeof(struct ObjAnimator);
            objDrawFn = (drawmethod_t) draw_nothing;
            break;
        case OBJ_TYPE_VALPTRS:
            objSize = sizeof(struct ObjValPtr);
            objDrawFn = (drawmethod_t) draw_nothing;
            break;
        case OBJ_TYPE_ZONES:
            objSize = sizeof(struct ObjZone);
            objDrawFn = (drawmethod_t) draw_nothing;
            break;
        default:
            fatal_print("make_object() : Unkown object!");
    }

    typeName = get_obj_name_str(objType);

    start_memtracker(typeName);
    newObj = gd_malloc(objSize, objPermanence);
    if (newObj == NULL) {
        fatal_printf("Cant allocate object '%s' memory!", typeName);
    }
    stop_memtracker(typeName);

    newObjBytes = (u8 *) newObj;
    for (i = 0; i < objSize; i++) {
        newObjBytes[i] = 0;
    }

    gGdObjCount++;
    objListOldHead = gGdObjectList;
    gGdObjectList = newObj;
    newObj->prev = NULL;
    if (objListOldHead != NULL) {
        newObj->next = objListOldHead;
        objListOldHead->prev = newObj;
    }

    newObj->index = gGdObjCount;
    newObj->type = objType;
    newObj->objDrawFn = objDrawFn;
    newObj->drawFlags = 0;

    imout();
    return newObj;
}

struct ObjZone *make_zone(struct ObjGroup *a0, struct GdBoundingBox *bbox, struct ObjGroup *a2) {
    struct ObjZone *newZone = (struct ObjZone *) make_object(OBJ_TYPE_ZONES);

    newZone->boundingBox.minX = bbox->minX;
    newZone->boundingBox.minY = bbox->minY;
    newZone->boundingBox.minZ = bbox->minZ;
    newZone->boundingBox.maxX = bbox->maxX;
    newZone->boundingBox.maxY = bbox->maxY;
    newZone->boundingBox.maxZ = bbox->maxZ;

    newZone->unk2C = a2;
    newZone->unk30 = a0;

#ifdef AVOID_UB
    return newZone;
#endif
}

struct ObjUnk200000 *func_8017C7A0(struct ObjVertex *a0, struct ObjFace *a1) {
    struct ObjUnk200000 *unk = (struct ObjUnk200000 *) make_object(OBJ_TYPE_UNK200000);

    unk->unk30 = a0;
    unk->unk34 = a1;

    return unk;
}

struct ListNode *make_link_to_obj(struct ListNode *prevNode, struct GdObj *obj) {
    struct ListNode *newNode;

    start_memtracker("links");
    newNode = gd_malloc_perm(sizeof(struct ListNode));
    if (newNode == NULL) {
        fatal_print("Cant allocate link memory!");
    }
    stop_memtracker("links");

    if (prevNode != NULL) {
        prevNode->next = newNode;
    }

    newNode->prev = prevNode;
    newNode->next = NULL;
    newNode->obj = obj;

    return newNode;
}

struct VtxLink *make_vtx_link(struct VtxLink *prevNode, Vtx *data) {
    struct VtxLink *newNode;

    newNode = gd_malloc_perm(sizeof(struct VtxLink));
    if (newNode == NULL) {
        fatal_print("Cant allocate link memory!");
    }

    if (prevNode != NULL) {
        prevNode->next = newNode;
    }

    newNode->prev = prevNode;
    newNode->next = NULL;
    newNode->data = data;

    if (((uintptr_t)(newNode)) == 0x3F800000) {
        fatal_printf("bad3\n");
    }

    return newNode;
}

struct ObjValPtr *make_valptr(struct GdObj *obj, s32 flag, enum ValPtrType type, size_t offset) {
    struct ObjValPtr *sp1C = (struct ObjValPtr *) make_object(OBJ_TYPE_VALPTRS);

    sp1C->obj = obj;
    sp1C->flag = flag;
    sp1C->offset = offset;
    sp1C->datatype = type;

    return sp1C;
}

void reset_plane(struct ObjPlane *plane) {
    struct ObjFace *sp4C;
    f32 sp48;
    f32 sp44;
    UNUSED u8 filler[12];
    s32 i;
    s32 sp30;
    register f32 sp28;

    imin("reset_plane");

    sp4C = plane->unk40;
    calc_face_normal(sp4C);
    plane->unk1C = gd_dot_vec3f(&sp4C->vertices[0]->pos, &sp4C->normal);
    sp48 = 0.0f;

    sp28 = sp4C->normal.x < 0.0f ? -sp4C->normal.x : sp4C->normal.x;
    sp44 = sp28;
    if (sp44 > sp48) {
        sp30 = 0;
        sp48 = sp44;
    }

    sp28 = sp4C->normal.y < 0.0f ? -sp4C->normal.y : sp4C->normal.y;
    sp44 = sp28;
    if (sp44 > sp48) {
        sp30 = 1;
        sp48 = sp44;
    }

    sp28 = sp4C->normal.z < 0.0f ? -sp4C->normal.z : sp4C->normal.z;
    sp44 = sp28;
    if (sp44 > sp48) {
        sp30 = 2;
    }

    switch (sp30) {
        case 0:
            plane->unk20 = 1;
            plane->unk24 = 2;
            break;
        case 1:
            plane->unk20 = 0;
            plane->unk24 = 2;
            break;
        case 2:
            plane->unk20 = 0;
            plane->unk24 = 1;
            break;
    }

    reset_bounding_box();

    for (i = 0; i < sp4C->vtxCount; i++) {
        add_obj_pos_to_bounding_box(&sp4C->vertices[i]->header);
    }

    plane->boundingBox.minX = gSomeBoundingBox.minX;
    plane->boundingBox.minY = gSomeBoundingBox.minY;
    plane->boundingBox.minZ = gSomeBoundingBox.minZ;
    plane->boundingBox.maxX = gSomeBoundingBox.maxX;
    plane->boundingBox.maxY = gSomeBoundingBox.maxY;
    plane->boundingBox.maxZ = gSomeBoundingBox.maxZ;

    if (plane->boundingBox.maxX - plane->boundingBox.minX < 100.0f) {
        plane->boundingBox.maxX += 50.0f;
        plane->boundingBox.minX -= 50.0f;
    }

    plane->boundingBox.maxY += 200.0f;
    plane->boundingBox.minY -= 200.0f;

    if (plane->boundingBox.maxZ - plane->boundingBox.minZ < 100.0f) {
        plane->boundingBox.maxZ += 50.0f;
        plane->boundingBox.minZ -= 50.0f;
    }
    imout();
}

struct ObjPlane *make_plane(s32 inZone, struct ObjFace *a1) {
    UNUSED u8 filler[4];
    struct ObjPlane *newPlane = (struct ObjPlane *) make_object(OBJ_TYPE_PLANES);

    gGdPlaneCount++;
    newPlane->id = gGdPlaneCount;
    newPlane->unk18 = inZone;
    newPlane->unk40 = a1;
    reset_plane(newPlane);

    return newPlane;
}

struct ObjCamera *make_camera(s32 flags, struct GdObj *a1) {
    struct ObjCamera *newCam;
    struct ObjCamera *oldCameraHead;

    newCam = (struct ObjCamera *) make_object(OBJ_TYPE_CAMERAS);

    gGdCameraCount++;
    newCam->id = gGdCameraCount;

    oldCameraHead = gGdCameraList;
    gGdCameraList = newCam;

    if (oldCameraHead != NULL) {
        newCam->next = oldCameraHead;
        oldCameraHead->prev = newCam;
    }

    newCam->flags = flags | 0x10;
    newCam->unk30 = a1;
    gd_set_identity_mat4(&newCam->unk64);
    gd_set_identity_mat4(&newCam->unkA8);

    newCam->unk180.x = 1.0f;
    newCam->unk180.y = 0.1f;
    newCam->unk180.z = 1.0f;

    newCam->unk134.x = 4.0f;
    newCam->unk134.y = 4.0f;
    newCam->unk134.z = 4.0f;

    newCam->unk178 = 0.0f;
    newCam->unk17C = 0.25f;

    newCam->zoomLevel = 0;
    newCam->maxZoomLevel = -1;

    newCam->unkA4 = 0.0f;

    newCam->lookAt.x = newCam->lookAt.y = newCam->lookAt.z = 0.0f;
    newCam->worldPos.x = newCam->worldPos.y = newCam->worldPos.z = 0.0f;

    return newCam;
}

struct ObjMaterial *make_material(UNUSED s32 a0, char *name, s32 id) {
    struct ObjMaterial *newMtl;

    newMtl = (struct ObjMaterial *) make_object(OBJ_TYPE_MATERIALS);

    if (name != NULL) {
        gd_strcpy(newMtl->name, name);
    } else {
        gd_strcpy(newMtl->name, "NoName");
    }

    newMtl->id = id;
    newMtl->gddlNumber = 0;
    newMtl->type = 16;

    return newMtl;
}

struct ObjLight *make_light(s32 flags, char *name, s32 id) {
    struct ObjLight *newLight;

    newLight = (struct ObjLight *) make_object(OBJ_TYPE_LIGHTS);

    if (name != NULL) {
        gd_strcpy(newLight->name, name);
    } else {
        gd_strcpy(newLight->name, "NoName");
    }

    newLight->id = id;
    newLight->unk30 = 1.0f;
    newLight->unk4C = 0;
    newLight->flags = flags | LIGHT_NEW_UNCOUNTED;
    newLight->unk98 = 0;
    newLight->unk40 = 0;

    newLight->unk68.x = newLight->unk68.y = newLight->unk68.z = 0;

    return newLight;
}

struct ObjView *make_view(const char *name, s32 flags, s32 projectionType, s32 ulx, s32 uly, s32 lrx, s32 lry,
                          struct ObjGroup *parts) {
    struct ObjView *newView = (struct ObjView *) make_object(OBJ_TYPE_VIEWS);

    if (gGdViewsGroup == NULL) {
        gGdViewsGroup = make_group(0);
    }

    addto_group(gGdViewsGroup, &newView->header);

    newView->flags = flags | VIEW_UPDATE | VIEW_LIGHT;
    newView->id = sGdViewInfo.count++;

    if ((newView->components = parts) != NULL) {
        reset_nets_and_gadgets(parts);
    }

    newView->unk78 = 0;
    newView->projectionType = projectionType;

    newView->clipping.x = 30.0f;
    newView->clipping.y = 5000.0f;
    newView->clipping.z = 45.0f;

    newView->upperLeft.x = (f32) ulx;
    newView->upperLeft.y = (f32) uly;

    newView->lowerRight.x = (f32) lrx;
    newView->lowerRight.y = (f32) lry;

    newView->unk48 = 1.0f;
    newView->unk4C = 1.0f;

    newView->colour.r = newView->id * 0.1;
    newView->colour.g = 0.06f;
    newView->colour.b = 1.0f;

    newView->proc = NULL;
    newView->unk9C = 0;

    if (name != NULL) {
        newView->unk1C = setup_view_buffers(name, newView, ulx, uly, lrx, lry);
    }

    newView->namePtr = name;
    newView->lights = NULL;

    return newView;
}

struct ObjAnimator *make_animator(void) {
    struct ObjAnimator *newAnim = (struct ObjAnimator *) make_object(OBJ_TYPE_ANIMATORS);
    newAnim->unk24 = 1.0f;
    newAnim->frame = 1.0f;

    newAnim->controlFunc = NULL;
    newAnim->state = 0;

    return newAnim;
}

struct ObjWeight *make_weight(UNUSED s32 a0, s32 vtxId, struct ObjVertex *vtx , f32 weight) {
    struct ObjWeight *newWeight = (struct ObjWeight *) make_object(OBJ_TYPE_WEIGHTS);

    newWeight->vtxId = vtxId;
    newWeight->weightVal = weight;
    newWeight->vtx = vtx;

    return newWeight;
}

struct ObjGroup *make_group_of_type(enum ObjTypeFlag type, struct GdObj *fromObj, struct GdObj *toObj) {
    struct ObjGroup *newGroup;
    struct GdObj *curObj;

    newGroup = make_group(0);
    curObj = fromObj;

    while (curObj != NULL) {
        if (curObj->type & type) {
            addto_group(newGroup, curObj);
        }

        if (curObj == toObj) {
            break;
        }

        curObj = curObj->prev;
    }

    return newGroup;
}

void format_object_id(char *str, struct GdObj *obj) {
    enum ObjTypeFlag type = obj->type;

    switch (type) {
        case OBJ_TYPE_BONES:
            sprintf(str, "b%d ", ((struct ObjBone *) obj)->id);
            break;
        case OBJ_TYPE_JOINTS:
            sprintf(str, "j%d ", ((struct ObjJoint *) obj)->id);
            break;
        case OBJ_TYPE_GROUPS:
            sprintf(str, "g%d ", ((struct ObjGroup *) obj)->id);
            break;
        case OBJ_TYPE_PARTICLES:
            sprintf(str, "p%d ", ((struct ObjParticle *) obj)->id);
            break;
        case OBJ_TYPE_NETS:
            sprintf(str, "net(no id) ");
            break;
        case OBJ_TYPE_CAMERAS:
            sprintf(str, "c%d ", ((struct ObjCamera *) obj)->id);
            break;
        case OBJ_TYPE_VERTICES:
            sprintf(str, "v(no id) ");
            break;
        case OBJ_TYPE_PLANES:
            sprintf(str, "pl%d ", ((struct ObjPlane *) obj)->id);
            break;
        default:
            sprintf(str, "?%x ", type);
            break;
    }
}

struct ObjGroup *make_group(s32 count, ...) {
    va_list args;
    s32 i;
    UNUSED u8 filler1[4];
    struct GdObj *curObj;
    UNUSED u8 filler2[12];
    struct ObjGroup *newGroup;
    struct ObjGroup *oldGroupListHead;
    struct GdObj *vargObj;
    char idStrBuf[0x20];
    struct ListNode *curLink;

    newGroup = (struct ObjGroup *) make_object(OBJ_TYPE_GROUPS);
    newGroup->id = ++gGdGroupCount;
    newGroup->memberCount = 0;
    newGroup->firstMember = newGroup->lastMember = NULL;

    printf("Made group no.%d\n", newGroup->id);

    oldGroupListHead = gGdGroupList;
    gGdGroupList = newGroup;
    if (oldGroupListHead != NULL) {
        newGroup->next = oldGroupListHead;
        oldGroupListHead->prev = newGroup;
    }

    if (count == 0) {
        return newGroup;
    }

    va_start(args, count);
    curLink = NULL;

    for (i = 0; i < count; i++) {

        vargObj = va_arg(args, struct GdObj *);

        if (vargObj == NULL) {
            fatal_printf("make_group() NULL group ptr");
        }

        curObj = vargObj;
        newGroup->memberTypes |= curObj->type;
        addto_group(newGroup, vargObj);
    }
    va_end(args);

    curLink = newGroup->firstMember;
    printf("Made group no.%d from: ", newGroup->id);
    while (curLink != NULL) {
        curObj = curLink->obj;
        format_object_id(idStrBuf, curObj);
        printf("%s", idStrBuf);
        printf("\n");
        curLink = curLink->next;
    }

    return newGroup;
}

void addto_group(struct ObjGroup *group, struct GdObj *obj) {
    char strbuf[0x20];
    UNUSED u8 filler[8];

    imin("addto_group");

    if (group->firstMember == NULL) {
        group->firstMember = make_link_to_obj(NULL, obj);
        group->lastMember = group->firstMember;
    } else {
        group->lastMember = make_link_to_obj(group->lastMember, obj);
    }

    group->memberTypes |= obj->type;
    group->memberCount++;

    printf("Added ");
    format_object_id(strbuf, obj);
    printf("%s", strbuf);
    printf(" to ");
    format_object_id(strbuf, &group->header);
    printf("%s", strbuf);
    printf("\n");

    imout();
}

void addto_groupfirst(struct ObjGroup *group, struct GdObj *obj) {
    imin("addto_groupfirst");

    if (group->firstMember == NULL) {
        group->firstMember = make_link_to_obj(NULL, obj);
        group->lastMember = group->firstMember;
    } else {
        struct ListNode *newNode = make_link_to_obj(NULL, obj);
        group->firstMember->prev = newNode;
        newNode->next = group->firstMember;
        group->firstMember = newNode;
    }

    group->memberTypes |= obj->type;
    group->memberCount++;

    imout();
}

s32 group_contains_obj(struct ObjGroup *group, struct GdObj *obj) {
    struct ListNode *node = group->firstMember;

    while (node != NULL) {
        if (node->obj->index == obj->index) {
            return TRUE;
        }
        node = node->next;
    }

    return FALSE;
}

void show_details(enum ObjTypeFlag type) {
    enum ObjTypeFlag curObjType;
    struct ListNode *curGroupLink;
    struct ObjGroup *curSubGroup;
    struct GdObj *curObj;
    char idStrBuf[0x24];
    s32 curGroupTypes;

    gd_printf("\nDetails about: ");
    switch (type) {
        case OBJ_TYPE_GROUPS:
            gd_printf("Groups\n");
            break;
        case OBJ_TYPE_BONES:
            gd_printf("Bones\n");
            break;
        case OBJ_TYPE_JOINTS:
            gd_printf("Joints\n");
            break;
        case OBJ_TYPE_PARTICLES:
            gd_printf("Particles\n");
            break;
        default:
            gd_printf("Everything?\n");
            break;
    }

    curObj = gGdObjectList;
    while (curObj != NULL) {
        curObjType = curObj->type;
        if (curObjType == type) {
            format_object_id(idStrBuf, curObj);
            switch (curObjType) {
                case OBJ_TYPE_GROUPS:
                    gd_printf("Group %s: ", idStrBuf);
                    curGroupTypes = ((struct ObjGroup *) curObj)->memberTypes;

                    if (curGroupTypes & OBJ_TYPE_GROUPS) {
                        gd_printf("groups ");
                    }
                    if (curGroupTypes & OBJ_TYPE_BONES) {
                        gd_printf("bones ");
                    }
                    if (curGroupTypes & OBJ_TYPE_JOINTS) {
                        gd_printf("joints ");
                    }
                    if (curGroupTypes & OBJ_TYPE_PARTICLES) {
                        gd_printf("particles ");
                    }
                    if (curGroupTypes & OBJ_TYPE_CAMERAS) {
                        gd_printf("cameras ");
                    }
                    if (curGroupTypes & OBJ_TYPE_NETS) {
                        gd_printf("nets ");
                    }
                    if (curGroupTypes & OBJ_TYPE_GADGETS) {
                        gd_printf("gadgets ");
                    }
                    if (curGroupTypes & OBJ_TYPE_LABELS) {
                        gd_printf("labels ");
                    }
                    if (curGroupTypes & OBJ_TYPE_FACES) {
                        gd_printf("face ");
                    }
                    if (curGroupTypes & OBJ_TYPE_VERTICES) {
                        gd_printf("vertex ");
                    }

                    curGroupLink = ((struct ObjGroup *) curObj)->firstMember;
                    while (curGroupLink != NULL) {
                        format_object_id(idStrBuf, curGroupLink->obj);
                        gd_printf("%s", idStrBuf);
                        curGroupLink = curGroupLink->next;
                    }
                    gd_printf("\n");
                    break;
                case OBJ_TYPE_BONES:
                    gd_printf("Bone %s: ", idStrBuf);
                    curSubGroup = ((struct ObjBone *) curObj)->unk10C;
                    curGroupLink = curSubGroup->firstMember;
                    while (curGroupLink != NULL) {
                        format_object_id(idStrBuf, curGroupLink->obj);
                        gd_printf("%s", idStrBuf);
                        curGroupLink = curGroupLink->next;
                    }
                    gd_printf("\n");
                    break;
                case OBJ_TYPE_JOINTS:
                    gd_printf("Joint %s: ", idStrBuf);
                    curSubGroup = ((struct ObjJoint *) curObj)->unk1C4;
                    curGroupLink = curSubGroup->firstMember;
                    while (curGroupLink != NULL) {
                        format_object_id(idStrBuf, curGroupLink->obj);
                        gd_printf("%s", idStrBuf);
                        curGroupLink = curGroupLink->next;
                    }
                    gd_printf("\n");
                    break;
                default:;
            }
        }
        curObj = curObj->next;
    }
}

s32 stub_objects_2(void) {
    s32 sp4 = 0;
    return sp4;
}

s32 make_scene(void) {
    s32 sp4 = 0;
    return sp4;
}

static void reset_joint_or_net(struct GdObj *obj) {
    struct GdObj *localObjPtr = obj;

    switch (obj->type) {
        case OBJ_TYPE_JOINTS:
            reset_joint((struct ObjJoint *) localObjPtr);
            break;
        case OBJ_TYPE_NETS:
            reset_net((struct ObjNet *) localObjPtr);
            break;
        default:;
    }
}

void menu_cb_reset_positions(void) {
    apply_to_obj_types_in_group(OBJ_TYPE_NETS, (applyproc_t) reset_joint_or_net, sCurrentMoveGrp);
}

struct GdObj *func_8017E2F0(struct GdObj *obj, enum ObjTypeFlag type) {
    UNUSED u8 filler[4];
    enum ObjTypeFlag curObjType;
    struct ListNode *node;

    curObjType = obj->type;

    switch (curObjType) {
        case OBJ_TYPE_GROUPS:
            node = ((struct ObjGroup *) obj)->firstMember;
            while (node != NULL) {
                func_8017E2F0(node->obj, type);
                node = node->next;
            }
            break;
        case OBJ_TYPE_BONES:
            break;
        default:;
    }

    if (curObjType == type) {
        return obj;
    }

#ifdef AVOID_UB
    return NULL;
#endif
}

s32 apply_to_obj_types_in_group(s32 types, applyproc_t func, struct ObjGroup *group) {
    struct ListNode *curLink;
    struct ListNode *nextLink;
    struct GdObj *linkedObj;
    enum ObjTypeFlag linkedObjType;
    applyproc_t objFn;
    UNUSED u8 filler[32];
    s32 fnAppliedCount;

    fnAppliedCount = 0;

    if (group == NULL) {
#ifdef AVOID_UB
        return fnAppliedCount;
#else
        return;
#endif
    }

    if (group->linkType & 1) {
        return fnAppliedCount;
    }

    if (!((group->memberTypes & OBJ_TYPE_GROUPS) | (group->memberTypes & types))) {
        return fnAppliedCount;
    }

    objFn = func;
    curLink = group->firstMember;

    while (curLink != NULL) {
        linkedObj = curLink->obj;
        linkedObjType = linkedObj->type;
        nextLink = curLink->next;

        if (linkedObjType == OBJ_TYPE_GROUPS) {
            fnAppliedCount += apply_to_obj_types_in_group(types, func, (struct ObjGroup *) linkedObj);
        }

        if (linkedObjType & types) {
            (*objFn)(linkedObj);
            fnAppliedCount++;
        }

        curLink = nextLink;
    }
    return fnAppliedCount;
}

void func_8017E584(struct ObjNet *a0, struct GdVec3f *a1, struct GdVec3f *a2) {
    struct GdVec3f sp94;
    struct GdVec3f sp88;
    struct GdVec3f sp7C;
    struct GdVec3f sp70;
    UNUSED u8 filler1[64];
    f32 sp2C;
    UNUSED u8 filler2[4];
    struct GdVec3f sp1C;

    sp70.x = a2->x;
    sp70.y = a2->y;
    sp70.z = a2->z;

    gd_normalize_vec3f(&sp70);

    sp7C.x = a1->x;
    sp7C.y = a1->y;
    sp7C.z = a1->z;

    sp1C.x = a0->centerOfGravity.x;
    sp1C.y = a0->centerOfGravity.y;
    sp1C.z = a0->centerOfGravity.z;

    gd_rotate_and_translate_vec3f(&sp1C, &a0->mat128);

    sp7C.x -= sp1C.x;
    sp7C.y -= sp1C.y;
    sp7C.z -= sp1C.z;

    if (gd_normalize_vec3f(&sp7C) == FALSE) {
        sp7C.x = -sp70.x;
        sp7C.y = -sp70.y;
        sp7C.z = -sp70.z;
    }

    gd_cross_vec3f(&sp70, a1, &sp94);
    sp2C = (f32) gd_sqrt_d((sp94.x * sp94.x) + (sp94.z * sp94.z));

    if (sp2C > 1000.0) {
        sp2C = 1000.0f;
    }

    sp2C /= 1000.0;
    sp2C = 1.0 - sp2C;

    sp88.x = a2->x * sp2C;
    sp88.y = a2->y * sp2C;
    sp88.z = a2->z * sp2C;

    a0->collDisp.x += sp88.x;
    a0->collDisp.y += sp88.y;
    a0->collDisp.z += sp88.z;
}

void func_8017E838(struct ObjNet *a0, struct GdVec3f *a1, struct GdVec3f *a2) {
    UNUSED u8 filler1[12];
    struct GdVec3f sp70;
    struct GdVec3f sp64;
    UNUSED u8 filler2[64];
    struct GdVec3f sp18;

    sp64.x = a1->x;
    sp64.y = a1->y;
    sp64.z = a1->z;

    sp18.x = a0->centerOfGravity.x;
    sp18.y = a0->centerOfGravity.y;
    sp18.z = a0->centerOfGravity.z;

    gd_rotate_and_translate_vec3f(&sp18, &a0->mat128);

    sp64.x -= sp18.x;
    sp64.y -= sp18.y;
    sp64.z -= sp18.z;

    sp64.x *= 0.01;
    sp64.y *= 0.01;
    sp64.z *= 0.01;

    gd_cross_vec3f(a2, &sp64, &sp70);
    gd_clamp_vec3f(&sp70, 5.0f);

    a0->collTorque.x += sp70.x;
    a0->collTorque.y += sp70.y;
    a0->collTorque.z += sp70.z;
}

void func_8017E9EC(struct ObjNet *net) {
    struct GdVec3f sp5C;
    Mat4f sp1C;
    f32 sp18;

    sp5C.x = net->torque.x;
    sp5C.y = net->torque.y;
    sp5C.z = net->torque.z;

    gd_normalize_vec3f(&sp5C);
    sp18 = gd_vec3f_magnitude(&net->torque);
    gd_create_rot_mat_angular(&sp1C, &sp5C, -sp18);
    gd_mult_mat4f(&D_801B9DC8, &sp1C, &D_801B9DC8);
}

s32 func_8017EA94(struct GdVec3f *vec, Mat4f matrix) {
    if (vec->x >= matrix[2][2] && vec->x <= matrix[3][1] && vec->z >= matrix[3][0]
        && vec->z <= matrix[3][3]) {
        return 1;
    }

    return 0;
}

s32 func_8017EB24(struct GdObj *obj1, struct GdObj *obj2) {
    struct GdVec3f pos1;
    struct GdVec3f pos2;
    struct GdBoundingBox *bbox1;
    struct GdBoundingBox *bbox2;
    struct GdBoundingBox sp18;

    set_cur_dynobj(obj1);
    d_get_world_pos(&pos1);
    bbox1 = d_get_bounding_box();

    set_cur_dynobj(obj2);
    d_get_world_pos(&pos2);
    bbox2 = d_get_bounding_box();

    sp18.minX = bbox1->minX + bbox2->minX;
    sp18.minY = bbox1->minY + bbox2->minY;
    sp18.minZ = bbox1->minZ + bbox2->minZ;
    sp18.maxX = bbox1->maxX + bbox2->maxX;
    sp18.maxY = bbox1->maxY + bbox2->maxY;
    sp18.maxZ = bbox1->maxZ + bbox2->maxZ;

    D_801B9E08.x = pos2.x - pos1.x;
    D_801B9E08.y = pos2.y - pos1.y;
    D_801B9E08.z = pos2.z - pos1.z;

    if (D_801B9E08.x >= sp18.minX) {
        if (D_801B9E08.x <= sp18.maxX) {
            if (D_801B9E08.z >= sp18.minZ) {
                if (D_801B9E08.z <= sp18.maxZ) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

s32 is_obj_xz_in_bounding_box(struct GdObj *obj, struct GdBoundingBox *bbox) {
    struct GdVec3f pos;

    set_cur_dynobj(obj);
    d_get_world_pos(&pos);

    if (pos.x >= bbox->minX) {
        if (pos.x <= bbox->maxX) {
            if (pos.z >= bbox->minZ) {
                if (pos.z <= bbox->maxZ) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

s32 is_point_xz_in_bounding_box(struct GdVec3f *point, struct GdBoundingBox *bbox) {
    if (point->x >= bbox->minX) {
        if (point->x <= bbox->maxX) {
            if (point->z >= bbox->minZ) {
                if (point->z <= bbox->maxZ) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

s32 gd_plane_point_within(struct GdBoundingBox *box1, struct GdBoundingBox *box2) {

    if (box1->minX >= box2->minX) {
        if (box1->minX <= box2->maxX) {
            if (box1->minZ >= box2->minZ) {
                if (box1->minZ <= box2->maxZ) {
                    return TRUE;
                }
            }
        }
    }

    if (box1->maxX >= box2->minX) {
        if (box1->maxX <= box2->maxX) {
            if (box1->minZ >= box2->minZ) {
                if (box1->minZ <= box2->maxZ) {
                    return TRUE;
                }
            }
        }
    }

    if (box1->maxX >= box2->minX) {
        if (box1->maxX <= box2->maxX) {
            if (box1->maxZ >= box2->minZ) {
                if (box1->maxZ <= box2->maxZ) {
                    return TRUE;
                }
            }
        }
    }

    if (box1->minX >= box2->minX) {
        if (box1->minX <= box2->maxX) {
            if (box1->maxZ >= box2->minZ) {
                if (box1->maxZ <= box2->maxZ) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

s32 transform_child_objects_recursive(struct GdObj *obj, struct GdObj *parentObj) {
    struct ListNode *curLink;
    struct ObjGroup *curGroup;
    UNUSED u8 filler1[4];
    Mat4f *parentUnkMtx;
    Mat4f *iMtx;
    Mat4f *unkMtx;
    Mat4f *rotMtx;
    Mat4f *rotMtx2;
    UNUSED u8 filler2[24];
    struct GdVec3f scale;

    if (parentObj != NULL) {
        set_cur_dynobj(parentObj);
        parentUnkMtx = d_get_matrix_ptr();
        rotMtx = (Mat4f *) d_get_rot_mtx_ptr();

        set_cur_dynobj(obj);
        iMtx = d_get_i_mtx_ptr();
        rotMtx2 = (Mat4f *) d_get_rot_mtx_ptr();

        d_get_scale(&scale);

        unkMtx = d_get_matrix_ptr();
        gd_mult_mat4f(iMtx, parentUnkMtx, unkMtx);

        gd_mult_mat4f(iMtx, rotMtx, rotMtx2);
        gd_scale_mat4f_by_vec3f(rotMtx2, &scale);
    } else {
        set_cur_dynobj(obj);
        unkMtx = d_get_matrix_ptr();
        iMtx = d_get_i_mtx_ptr();
        rotMtx = (Mat4f *) d_get_rot_mtx_ptr();

        d_get_scale(&scale);
        gd_set_identity_mat4(unkMtx);
        gd_copy_mat4f(iMtx, rotMtx);
        gd_scale_mat4f_by_vec3f(rotMtx, &scale);
    }

    set_cur_dynobj(obj);
    curGroup = d_get_att_objgroup();
    if (curGroup != NULL) {
        curLink = curGroup->firstMember;
        while (curLink != NULL) {
            transform_child_objects_recursive(curLink->obj, obj);
            curLink = curLink->next;
        }
    }
    return 0;
}

s32 func_8017F210(struct GdObj *a0, struct GdObj *a1) {
    struct ListNode *sp6C;
    struct ObjGroup *sp68;
    UNUSED u8 filler1[4];
    UNUSED Mat4f *sp60;
    Mat4f *sp5C;
    UNUSED Mat4f *sp58;
    Mat4f *sp54;
    Mat4f *sp50;
    UNUSED u8 filler2[24];
    struct GdVec3f sp2C;
    s32 count = 0;

    count++;

    if (a1 != NULL) {
        set_cur_dynobj(a1);
        sp60 = d_get_matrix_ptr();
        sp54 = (Mat4f *) d_get_rot_mtx_ptr();

        set_cur_dynobj(a0);
        sp5C = d_get_i_mtx_ptr();
        sp50 = (Mat4f *) d_get_rot_mtx_ptr();

        d_get_scale(&sp2C);
        gd_mult_mat4f(sp5C, sp54, sp50);
        gd_scale_mat4f_by_vec3f(sp50, &sp2C);
    } else {
        set_cur_dynobj(a0);
        sp58 = d_get_matrix_ptr();
        sp5C = d_get_i_mtx_ptr();
        sp54 = (Mat4f *) d_get_rot_mtx_ptr();

        d_get_scale(&sp2C);
        gd_copy_mat4f(sp5C, sp54);
        gd_scale_mat4f_by_vec3f(sp54, &sp2C);
    }

    set_cur_dynobj(a0);
    sp68 = d_get_att_objgroup();

    if (sp68 != NULL) {
        sp6C = sp68->firstMember;
        while (sp6C != NULL) {
            count += func_8017F210(sp6C->obj, a0);
            sp6C = sp6C->next;
        }
    }
    return count;
}

void func_8017F3CC(struct Unk8017F3CC *a0) {
    gd_rotate_and_translate_vec3f(&a0->unk20, D_801B9E48);
}

void stub_objects_3(UNUSED f32 a0, UNUSED struct GdObj *a1, UNUSED struct GdObj *a2) {
    UNUSED u8 filler[48];
}

void interpolate_animation_transform(struct GdAnimTransform *t1, struct GdAnimTransform *t2, f32 dt) {
    Mat4f mtx;

    gd_set_identity_mat4(&mtx);

    if (dt != 0.0f) {
        struct GdAnimTransform transform;

        transform.rotate.x = t1->rotate.x + (t2->rotate.x - t1->rotate.x) * dt;
        transform.rotate.y = t1->rotate.y + (t2->rotate.y - t1->rotate.y) * dt;
        transform.rotate.z = t1->rotate.z + (t2->rotate.z - t1->rotate.z) * dt;

        transform.pos.x = t1->pos.x + (t2->pos.x - t1->pos.x) * dt;
        transform.pos.y = t1->pos.y + (t2->pos.y - t1->pos.y) * dt;
        transform.pos.z = t1->pos.z + (t2->pos.z - t1->pos.z) * dt;

        gd_scale_mat4f_by_vec3f(&mtx, &t1->scale);
        gd_rot_mat_about_vec(&mtx, &transform.rotate);
        gd_add_vec3f_to_mat4f_offset(&mtx, &transform.pos);
    } else {
        d_set_scale(t1->scale.x, t1->scale.y, t1->scale.z);
        gd_rot_mat_about_vec(&mtx, &t1->rotate);
        gd_add_vec3f_to_mat4f_offset(&mtx, &t1->pos);
    }
    d_set_i_matrix(&mtx);
}

void move_animator(struct ObjAnimator *animObj) {
    struct AnimDataInfo *animData;
    Mat4f *mtxArr;
    Mat4f localMtx;
    struct GdAnimTransform *triPtr;
    struct GdAnimTransform currTransform;
    struct GdAnimTransform nextTransform;
    s16(*animData9s16)[9];
    s16(*animData3s16)[3];
    s16(*animData6s16)[6];
    s16(*animDataCam)[6];
    struct GdObj *stubObj1 = NULL;
    struct GdObj *stubObj2 = NULL;
    UNUSED u8 filler[12];
    UNUSED struct GdVec3f unusedVec;
    s32 currKeyFrame;
    s32 nextKeyFrame;
    f32 dt;
    f32 scale = 0.1f;
    struct AnimMtxVec *sp28;
    register struct ListNode *link;
    struct GdObj *linkedObj;

    if (animObj->controlFunc != NULL) {
        animObj->controlFunc(animObj);
    }

    if (animObj->animatedPartsGrp == NULL) {
        return;
    }

    animData = (struct AnimDataInfo *) animObj->animdataGrp->firstMember->obj;

    if (animObj->attachedToObj != NULL) {
        animObj->frame = ((struct ObjAnimator *) animObj->attachedToObj)->frame
                         / ((struct ObjAnimator *) animObj->attachedToObj)->unk24;
        animData += ((struct ObjAnimator *) animObj->attachedToObj)->animSeqNum;
    }

    if (animData->type == 0) {
        return;
    }

    unusedVec.x = 4.0f;
    unusedVec.y = 1.0f;
    unusedVec.z = 1.0f;

    if (animObj->frame > (f32) animData->count) {
        animObj->frame = 1.0f;
    } else if (animObj->frame < 0.0f) {
        animObj->frame = (f32) animData->count;
    }

    currKeyFrame = (s32) animObj->frame;
    dt = animObj->frame - (f32) currKeyFrame;
    nextKeyFrame = currKeyFrame + 1;

    if (nextKeyFrame > animData->count) {
        nextKeyFrame = 1;
    }

    currKeyFrame--;
    nextKeyFrame--;

    link = animObj->animatedPartsGrp->firstMember;
    while (link != NULL) {
        linkedObj = link->obj;
        set_cur_dynobj(linkedObj);
        switch (animData->type) {
            case GD_ANIM_MTX4x4:
                mtxArr = (Mat4f *) animData->data;

                d_set_i_matrix(mtxArr + (s32) animObj->frame);
                break;
            case GD_ANIM_ROT3S:
                animData3s16 = (s16(*)[3]) animData->data;

                d_get_scale(&currTransform.scale);
                nextTransform.scale.x = currTransform.scale.x;
                nextTransform.scale.y = currTransform.scale.y;
                nextTransform.scale.z = currTransform.scale.z;

                d_get_init_pos(&currTransform.pos);
                nextTransform.pos.x = currTransform.pos.x;
                nextTransform.pos.y = currTransform.pos.y;
                nextTransform.pos.z = currTransform.pos.z;

                currTransform.rotate.x = (f32) animData3s16[currKeyFrame][0] * scale;
                currTransform.rotate.y = (f32) animData3s16[currKeyFrame][1] * scale;
                currTransform.rotate.z = (f32) animData3s16[currKeyFrame][2] * scale;

                nextTransform.rotate.x = (f32) animData3s16[nextKeyFrame][0] * scale;
                nextTransform.rotate.y = (f32) animData3s16[nextKeyFrame][1] * scale;
                nextTransform.rotate.z = (f32) animData3s16[nextKeyFrame][2] * scale;

                interpolate_animation_transform(&currTransform, &nextTransform, dt);
                break;
            case GD_ANIM_POS3S:
                animData3s16 = (s16(*)[3]) animData->data;

                d_get_scale(&currTransform.scale);
                nextTransform.scale.x = currTransform.scale.x;
                nextTransform.scale.y = currTransform.scale.y;
                nextTransform.scale.z = currTransform.scale.z;

                d_get_init_rot(&currTransform.rotate);
                nextTransform.rotate.x = currTransform.rotate.x;
                nextTransform.rotate.y = currTransform.rotate.y;
                nextTransform.rotate.z = currTransform.rotate.z;

                currTransform.pos.x = (f32) animData3s16[currKeyFrame][0];
                currTransform.pos.y = (f32) animData3s16[currKeyFrame][1];
                currTransform.pos.z = (f32) animData3s16[currKeyFrame][2];

                nextTransform.pos.x = (f32) animData3s16[nextKeyFrame][0];
                nextTransform.pos.y = (f32) animData3s16[nextKeyFrame][1];
                nextTransform.pos.z = (f32) animData3s16[nextKeyFrame][2];

                interpolate_animation_transform(&currTransform, &nextTransform, dt);
                break;
            case GD_ANIM_ROT3S_POS3S:
                animData6s16 = (s16(*)[6]) animData->data;

                d_get_scale(&currTransform.scale);
                nextTransform.scale.x  = currTransform.scale.x;
                nextTransform.scale.y  = currTransform.scale.y;
                nextTransform.scale.z  = currTransform.scale.z;

                currTransform.rotate.x = (f32) animData6s16[currKeyFrame][0] * scale;
                currTransform.rotate.y = (f32) animData6s16[currKeyFrame][1] * scale;
                currTransform.rotate.z = (f32) animData6s16[currKeyFrame][2] * scale;

                nextTransform.rotate.x = (f32) animData6s16[nextKeyFrame][0] * scale;
                nextTransform.rotate.y = (f32) animData6s16[nextKeyFrame][1] * scale;
                nextTransform.rotate.z = (f32) animData6s16[nextKeyFrame][2] * scale;

                currTransform.pos.x  = (f32) animData6s16[currKeyFrame][3];
                currTransform.pos.y  = (f32) animData6s16[currKeyFrame][4];
                currTransform.pos.z  = (f32) animData6s16[currKeyFrame][5];

                nextTransform.pos.x  = (f32) animData6s16[nextKeyFrame][3];
                nextTransform.pos.y  = (f32) animData6s16[nextKeyFrame][4];
                nextTransform.pos.z  = (f32) animData6s16[nextKeyFrame][5];

                interpolate_animation_transform(&currTransform, &nextTransform, dt);
                break;
            case GD_ANIM_SCALE3S_POS3S_ROT3S:
                animData9s16 = (s16(*)[9]) animData->data;

                currTransform.scale.x  = (f32) animData9s16[currKeyFrame][0] * scale;
                currTransform.scale.y  = (f32) animData9s16[currKeyFrame][1] * scale;
                currTransform.scale.z  = (f32) animData9s16[currKeyFrame][2] * scale;

                currTransform.rotate.x = (f32) animData9s16[currKeyFrame][3] * scale;
                currTransform.rotate.y = (f32) animData9s16[currKeyFrame][4] * scale;
                currTransform.rotate.z = (f32) animData9s16[currKeyFrame][5] * scale;

                currTransform.pos.x  = (f32) animData9s16[currKeyFrame][6];
                currTransform.pos.y  = (f32) animData9s16[currKeyFrame][7];
                currTransform.pos.z  = (f32) animData9s16[currKeyFrame][8];

                nextTransform.scale.x  = (f32) animData9s16[nextKeyFrame][0] * scale;
                nextTransform.scale.y  = (f32) animData9s16[nextKeyFrame][1] * scale;
                nextTransform.scale.z  = (f32) animData9s16[nextKeyFrame][2] * scale;

                nextTransform.rotate.x = (f32) animData9s16[nextKeyFrame][3] * scale;
                nextTransform.rotate.y = (f32) animData9s16[nextKeyFrame][4] * scale;
                nextTransform.rotate.z = (f32) animData9s16[nextKeyFrame][5] * scale;

                nextTransform.pos.x  = (f32) animData9s16[nextKeyFrame][6];
                nextTransform.pos.y  = (f32) animData9s16[nextKeyFrame][7];
                nextTransform.pos.z  = (f32) animData9s16[nextKeyFrame][8];

                interpolate_animation_transform(&currTransform, &nextTransform, dt);
                break;
            case GD_ANIM_CAMERA_EYE3S_LOOKAT3S:
                if (linkedObj->type == OBJ_TYPE_CAMERAS) {
                    animDataCam = animData->data;

                    currTransform.pos.x = (f32) animDataCam[currKeyFrame][0];
                    currTransform.pos.y = (f32) animDataCam[currKeyFrame][1];
                    currTransform.pos.z = (f32) animDataCam[currKeyFrame][2];

                    nextTransform.pos.x = (f32) animDataCam[currKeyFrame][3];
                    nextTransform.pos.y = (f32) animDataCam[currKeyFrame][4];
                    nextTransform.pos.z = (f32) animDataCam[currKeyFrame][5];

                    ((struct ObjCamera *) linkedObj)->worldPos.x = currTransform.pos.x;
                    ((struct ObjCamera *) linkedObj)->worldPos.y = currTransform.pos.y;
                    ((struct ObjCamera *) linkedObj)->worldPos.z = currTransform.pos.z;

                    ((struct ObjCamera *) linkedObj)->lookAt.x = nextTransform.pos.x;
                    ((struct ObjCamera *) linkedObj)->lookAt.y = nextTransform.pos.y;
                    ((struct ObjCamera *) linkedObj)->lookAt.z = nextTransform.pos.z;
                }
                break;
            case GD_ANIM_SCALE3F_ROT3F_POS3F:
                triPtr = (struct GdAnimTransform *) animData->data;
                interpolate_animation_transform(&triPtr[currKeyFrame], &triPtr[nextKeyFrame], dt);
                break;
            case GD_ANIM_MTX4x4F_SCALE3F:
                sp28 = &((struct AnimMtxVec *) animData->data)[currKeyFrame];
                d_set_i_matrix(&sp28->matrix);
                d_set_scale(sp28->vec.x, sp28->vec.y, sp28->vec.z);
                break;
            case GD_ANIM_SCALE3F_ROT3F_POS3F_2:
                triPtr = (struct GdAnimTransform *) animData->data;
                gd_set_identity_mat4(&localMtx);
                gd_scale_mat4f_by_vec3f(&localMtx, &triPtr->scale);
                gd_rot_mat_about_vec(&localMtx, &triPtr->rotate);
                gd_add_vec3f_to_mat4f_offset(&localMtx, &triPtr->pos);
                d_set_i_matrix(&localMtx);
                break;
            case GD_ANIM_STUB:
                if (stubObj1 == NULL) {
                    stubObj1 = linkedObj;
                } else {
                    if (stubObj2 == NULL) {
                        stubObj2 = linkedObj;
                        stub_objects_3(animObj->frame, stubObj1, stubObj2);
                    } else {
                        fatal_printf("Too many objects to morph");
                    }
                }
                break;
            default:
                fatal_printf("move_animator(): Unkown animation data type");
        }
        link = link->next;
    }
}

void drag_picked_object(struct GdObj *inputObj) {
    UNUSED u8 filler1[12];
    struct GdVec3f displacement;
    struct GdVec3f spC4;
    struct GdControl *ctrl;
    Mat4f sp80;
    Mat4f sp40;
    UNUSED u8 filler2[12];
    struct GdObj *obj;
    UNUSED u8 filler3[4];
    f32 dispMag;

    ctrl = &gGdCtrl;

    if (gViewUpdateCamera == NULL) {
        return;
    }

    dispMag = gd_vec3f_magnitude(&gViewUpdateCamera->unk40);
    dispMag /= 1000.0f;

    displacement.x = ((f32)(ctrl->csrX - ctrl->dragStartX)) * dispMag;
    displacement.y = ((f32) - (ctrl->csrY - ctrl->dragStartY)) * dispMag;
    displacement.z = 0.0f;

    gd_inverse_mat4f(&gViewUpdateCamera->unkE8, &sp40);
    gd_mat4f_mult_vec3f(&displacement, &sp40);

    obj = inputObj;
    if ((inputObj->drawFlags & OBJ_PICKED) && gGdCtrl.dragging) {
        gd_play_sfx(GD_SFX_PINCH_FACE);

        if (ABS(ctrl->stickDeltaX) + ABS(ctrl->stickDeltaY) >= 11) {
            gd_play_sfx(GD_SFX_PINCH_FACE_2);
        }

        switch (inputObj->type) {
            case OBJ_TYPE_JOINTS:
                ((struct ObjJoint *) obj)->mat128[3][0] += displacement.x;
                ((struct ObjJoint *) obj)->mat128[3][1] += displacement.y;
                ((struct ObjJoint *) obj)->mat128[3][2] += displacement.z;
                break;
            case OBJ_TYPE_GADGETS:
                break;
            case OBJ_TYPE_NETS:
                gd_inverse_mat4f(&((struct ObjNet *) obj)->mat128, &sp80);
                spC4.x = displacement.x;
                spC4.y = displacement.y;
                spC4.z = displacement.z;

                gd_mat4f_mult_vec3f(&spC4, &sp80);
                ((struct ObjNet *) obj)->matE8[3][0] += displacement.x;
                ((struct ObjNet *) obj)->matE8[3][1] += displacement.y;
                ((struct ObjNet *) obj)->matE8[3][2] += displacement.z;
                break;
            case OBJ_TYPE_PARTICLES:
                break;
            default:;
        }
    }
}

void move_animators(struct ObjGroup *group) {
    restart_timer("move_animators");
    apply_to_obj_types_in_group(OBJ_TYPE_ANIMATORS, (applyproc_t) move_animator, group);
    split_timer("move_animators");
}

void find_and_drag_picked_object(struct ObjGroup *group) {
    apply_to_obj_types_in_group(OBJ_TYPE_ALL, (applyproc_t) drag_picked_object, group);
}

void move_camera(struct ObjCamera *cam) {
    struct GdObj *spEC;
    struct GdVec3f spE0;
    struct GdVec3f spD4;
    struct GdVec3f spC8;
    UNUSED u8 filler1[12];
    struct GdVec3f spB0;
    Mat4f sp70;
    UNUSED u8 filler2[64];
    Mat4f *sp2C;
    struct GdControl *ctrl;

    ctrl = &gGdCtrl;
    if (!(cam->flags & 0x10)) {
        return;
    }

    spE0.x = spE0.y = spE0.z = 0.0f;
    spB0.x = spB0.y = spB0.z = 0.0f;

    if ((spEC = cam->unk30) != NULL) {
        set_cur_dynobj(spEC);
        d_get_world_pos(&spE0);
        d_get_matrix(&sp70);

        spC8.x = sp70[2][0] - cam->unk58;
        spC8.z = sp70[2][2] - cam->unk60;

        cam->unk58 += spC8.x * cam->unk180.y;
        cam->unk60 += spC8.z * cam->unk180.y;

        cam->unkA8[2][0] = cam->unk58;
        cam->unkA8[2][1] = 0.0f;
        cam->unkA8[2][2] = cam->unk60;

        cam->unkA8[0][0] = cam->unkA8[2][2];
        cam->unkA8[0][1] = 0.0f;
        cam->unkA8[0][2] = -cam->unkA8[2][0];

        cam->unkA8[1][0] = 0.0f;
        cam->unkA8[1][1] = 1.0f;
        cam->unkA8[1][2] = 0.0f;

        gd_set_identity_mat4(&cam->unkA8);
    } else {
        gd_set_identity_mat4(&cam->unkA8);
    }

    sp2C = &cam->unk64;
    if ((cam->flags & CAMERA_FLAG_CONTROLLABLE) != 0) {
        if (ctrl->btnB != FALSE && ctrl->prevFrame->btnB == FALSE) {
            cam->zoomLevel++;
            if (cam->zoomLevel > cam->maxZoomLevel) {
                cam->zoomLevel = 0;
            }

            switch (cam->zoomLevel) {
                case 0:
                    gd_play_sfx(GD_SFX_CAM_ZOOM_IN);
                    break;
                case 1:
                case 2:
                    gd_play_sfx(GD_SFX_CAM_ZOOM_OUT);
                    break;
            }
        }

        if (ctrl->cleft) {
            cam->unk128.y += cam->unk134.y;
        }

        if (ctrl->cright) {
            cam->unk128.y -= cam->unk134.y;
        }

        if (ctrl->cup) {
            cam->unk128.x += cam->unk134.x;
        }

        if (ctrl->cdown) {
            cam->unk128.x -= cam->unk134.x;
        }

        cam->unk128.x = gd_clamp_f32(cam->unk128.x, 80.0f);

        cam->unk4C.x = cam->zoomPositions[cam->zoomLevel].x;
        cam->unk4C.y = cam->zoomPositions[cam->zoomLevel].y;
        cam->unk4C.z = cam->zoomPositions[cam->zoomLevel].z;

        gd_rot_2d_vec(cam->unk128.x, &cam->unk4C.y, &cam->unk4C.z);
        gd_rot_2d_vec(-cam->unk128.y, &cam->unk4C.x, &cam->unk4C.z);

        cam->unk40.x += (cam->unk4C.x - cam->unk40.x) * cam->unk17C;
        cam->unk40.y += (cam->unk4C.y - cam->unk40.y) * cam->unk17C;
        cam->unk40.z += (cam->unk4C.z - cam->unk40.z) * cam->unk17C;
    } else {
        gd_set_identity_mat4(sp2C);
    }

    spD4.x = cam->unk40.x;
    spD4.y = cam->unk40.y;
    spD4.z = cam->unk40.z;

    spD4.x += spB0.x;
    spD4.y += spB0.y;
    spD4.z += spB0.z;

    gd_mult_mat4f(sp2C, &cam->unkA8, &cam->unkA8);
    gd_mat4f_mult_vec3f(&spD4, &cam->unkA8);

    cam->worldPos.x = spD4.x;
    cam->worldPos.y = spD4.y;
    cam->worldPos.z = spD4.z;

    cam->worldPos.x += spE0.x;
    cam->worldPos.y += spE0.y;
    cam->worldPos.z += spE0.z;
}

void move_cameras_in_grp(struct ObjGroup *group) {
    apply_to_obj_types_in_group(OBJ_TYPE_CAMERAS, (applyproc_t) move_camera, group);
}

void func_8018100C(struct ObjLight *light) {
    Mat4f mtx;
    UNUSED u8 filler[12];

    if (light->unk40 == 3) {
        if (light->unk30 > 0.0) {
            light->unk30 -= 0.2;
        }

        if (light->unk30 < 0.0f) {
            light->unk30 = 0.0f;
        }

        if ((light->unk3C & 0x1) != 0) {
            light->unk30 = 1.0f;
        }

        light->unk3C &= ~1;
    }

    return;

    light->position.x += light->unk80.x;
    light->position.y += light->unk80.y;
    light->position.z += light->unk80.z;

    if (light->position.x > 500.0f || light->position.y < -500.0f) {
        light->unk80.x = -light->unk80.x;
    }

    if (light->position.y > 500.0f || light->position.y < -500.0f) {
        light->unk80.y = -light->unk80.y;
    }

    if (light->position.z > 500.0f || light->position.z < -500.0f) {
        light->unk80.z = -light->unk80.z;
    }

    return;

    D_801A81C0 += 1.0;
    D_801A81C4 += 0.6;

    gd_set_identity_mat4(&mtx);
    gd_absrot_mat4(&mtx, GD_Y_AXIS, light->unk68.y);
    gd_absrot_mat4(&mtx, GD_X_AXIS, light->unk68.x);
    gd_absrot_mat4(&mtx, GD_Z_AXIS, light->unk68.z);
    gd_mat4f_mult_vec3f(&light->unk8C, &mtx);

    light->position.x = light->unk8C.x;
    light->position.y = light->unk8C.y;
    light->position.z = light->unk8C.z;
    return;

    gd_mat4f_mult_vec3f(&light->unk80, &mtx);
    imout();
}

void move_lights_in_grp(struct ObjGroup *group) {
    apply_to_obj_types_in_group(OBJ_TYPE_LIGHTS, (applyproc_t) func_8018100C, group);
}

void move_group_members(void) {
    s32 i;

    if (gGdMoveScene != 0) {
        reset_gadgets_in_grp(sCurrentMoveGrp);
        move_lights_in_grp(sCurrentMoveGrp);
        move_particles_in_grp(sCurrentMoveGrp);
        move_animators(sCurrentMoveGrp);

        for (i = 0; i <= 0; i++) {
            move_nets(sCurrentMoveGrp);
        }

        move_cameras_in_grp(sCurrentMoveGrp);
    }
}

void proc_view_movement(struct ObjView *view) {
    imin("movement");
    sCurrentMoveCamera = view->activeCam;
    sCurrentMoveView = view;
    if ((sCurrentMoveGrp = view->components) != NULL) {
        move_group_members();
    }
    if ((sCurrentMoveGrp = view->lights) != NULL) {
        move_group_members();
    }
    imout();
}

void reset_nets_and_gadgets(struct ObjGroup *group) {
    func_80193848(group);
    apply_to_obj_types_in_group(OBJ_TYPE_GADGETS, (applyproc_t) reset_gadget, group);
}

void null_obj_lists(void) {
    D_801B9E44 = 0;
    gGdObjCount = 0;
    gGdGroupCount = 0;
    gGdPlaneCount = 0;
    gGdCameraCount = 0;
    sGdViewInfo.count = 0;

    gGdCameraList = NULL;
    D_801B9E50 = NULL;
    gGdBoneList = NULL;
    gGdJointList = NULL;
    gGdGroupList = NULL;
    D_801B9E80 = NULL;
    gGdObjectList = NULL;
    gGdViewsGroup = NULL;

    reset_net_count();
    reset_joint_counts();
}
