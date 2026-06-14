#include <PR/ultratypes.h>
#include <stdio.h>

#include "bad_declarations.h"
#include "debug_utils.h"
#include "draw_objects.h"
#include "dynlist_proc.h"
#include "gd_main.h"
#include "gd_math.h"
#include "gd_types.h"
#include "joints.h"
#include "macros.h"
#include "objects.h"
#include "old_menu.h"
#include "particles.h"
#include "renderer.h"
#include "shape_helper.h"
#include "skin.h"

#define DYNOBJ_NAME_SIZE 8

#define DYNOBJ_LIST_SIZE 3000

#define VTX_BUF_SIZE 3000

struct DynObjInfo {
    char name[DYNOBJ_NAME_SIZE];
    struct GdObj *obj;
    s32 num;
    s32 unk;
};

#define Dyn1AsInt(dyn) ((dyn)->w1.word)
#define Dyn1AsPtr(dyn) ((dyn)->w1.ptr)
#define Dyn1AsStr(dyn) ((dyn)->w1.str)
#define Dyn1AsName(dyn) ((DynObjName)((dyn)->w1.ptr))

#define Dyn2AsInt(dyn) ((dyn)->w2.word)
#define Dyn2AsPtr(dyn) ((dyn)->w2.ptr)
#define Dyn2AsStr(dyn) ((dyn)->w2.str)
#define Dyn2AsName(dyn) ((DynObjName)((dyn)->w2.ptr))

#define DynVec(dyn) (&(dyn)->vec)
#define DynVecX(dyn) ((dyn)->vec.x)
#define DynVecY(dyn) ((dyn)->vec.y)
#define DynVecZ(dyn) ((dyn)->vec.z)

static struct DynObjInfo *sGdDynObjList = NULL;
static struct GdObj *sDynListCurObj = NULL;
static struct GdBoundingBox sNullBoundingBox = {
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0
};
static s32 sUseIntegerNames = FALSE;

static char sIntToStringBuf[DYNOBJ_NAME_SIZE];

static struct DynObjInfo sNullDynObjInfo;
static char sDynNameSuffix[DYNOBJ_NAME_SIZE];
static s32
    sUnnamedObjCount;
static s32 sLoadedDynObjs;
static struct DynObjInfo *sDynListCurInfo;
static struct DynObjInfo *sParentObjInfo;
static struct DynObjInfo *sStashedDynObjInfo;
static struct GdObj *sStashedDynObj;
static s32 sDynNetCount;
static char sDynNetNameSuffix[0x20];
static char sStashedDynNameSuffix[0x100];

void d_add_net_with_subgroup(s32, DynObjName);
void d_end_net_with_subgroup(DynObjName);
void d_attach_joint_to_net(s32, DynObjName);
void d_addto_group(DynObjName);
void d_link_with(DynObjName);
void d_link_with_ptr(void *);
void d_set_normal(f32, f32, f32);
void d_make_vertex(struct GdVec3f *);
void d_set_rotation(f32, f32, f32);
void d_center_of_gravity(f32, f32, f32);
void d_set_shape_offset(f32, f32, f32);
void d_clear_flags(s32);
void d_attach(DynObjName);
void d_attach_to(s32, struct GdObj *);
void d_attachto_dynid(s32, DynObjName);
void d_set_att_offset(const struct GdVec3f *);
void d_set_nodegroup(DynObjName);
void d_set_matgroup(DynObjName);
void d_set_skinshape(DynObjName);
void d_set_planegroup(DynObjName);
void d_set_shapeptr(DynObjName);
void d_friction(f32, f32, f32);
void d_set_spring(f32);
void d_set_ambient(f32, f32, f32);
void d_set_control_type(s32);
void d_set_skin_weight(s32, f32);
void d_set_id(s32);
void d_set_material(void *, s32);
void d_map_materials(DynObjName);
void d_map_vertices(DynObjName);
void d_set_texture_st(f32, f32);
void d_use_texture(void *);
void d_make_netfromshapeid(DynObjName);
void d_make_netfromshape_ptrptr(struct ObjShape **);
void add_to_dynobj_list(struct GdObj *, DynObjName);

void d_stash_dynobj(void) {
    sStashedDynObjInfo = sDynListCurInfo;
    sStashedDynObj = sDynListCurObj;
}

void d_unstash_dynobj(void) {
    sDynListCurObj = sStashedDynObj;
    sDynListCurInfo = sStashedDynObjInfo;
}

void reset_dynlist(void) {
    sUnnamedObjCount = 0;
    sLoadedDynObjs = 0;
    sDynNameSuffix[0] = '\0';
    sGdDynObjList = NULL;
    sDynListCurObj = NULL;
    sDynNetCount = 0;
    sUseIntegerNames = FALSE;
    gd_strcpy(sNullDynObjInfo.name, "NullObj");
}

struct GdObj *proc_dynlist(struct DynList *dylist) {
    UNUSED u8 filler[8];

    if (dylist++->cmd != 0xD1D4) {
        fatal_printf("proc_dynlist() not a valid dyn list");
    }

    while (dylist->cmd != 58) {
        switch (dylist->cmd) {
            case 43:
                d_set_name_suffix(Dyn1AsStr(dylist));
                break;
            case 15:
                d_makeobj(Dyn2AsInt(dylist), Dyn1AsName(dylist));
                break;
            case 46:
                d_add_net_with_subgroup(Dyn2AsInt(dylist), Dyn1AsName(dylist));
                break;
            case 48:
                d_end_net_with_subgroup(Dyn1AsName(dylist));
                break;
            case 47:
                d_attach_joint_to_net(Dyn2AsInt(dylist), Dyn1AsName(dylist));
                break;
            case 16:
                d_start_group(Dyn1AsName(dylist));
                break;
            case 17:
                d_end_group(Dyn1AsName(dylist));
                break;
            case 18:
                d_addto_group(Dyn1AsName(dylist));
                break;
            case 30:
                d_use_obj(Dyn1AsName(dylist));
                break;
            case 28:
                d_link_with(Dyn1AsName(dylist));
                break;
            case 50:
                d_add_valptr(Dyn1AsName(dylist), (u32) DynVecY(dylist), Dyn2AsInt(dylist),
                             (size_t) DynVecX(dylist));
                break;
            case 29:
                d_link_with_ptr(Dyn1AsPtr(dylist));
                break;
            case 12:
                proc_dynlist(Dyn1AsPtr(dylist));
                break;
            case 0:
                d_use_integer_names(Dyn2AsInt(dylist));
                break;
            case 1:
                d_set_init_pos(DynVecX(dylist), DynVecY(dylist), DynVecZ(dylist));
                break;
            case 2:
                d_set_rel_pos(DynVecX(dylist), DynVecY(dylist), DynVecZ(dylist));
                break;
            case 3:
                d_set_world_pos(DynVecX(dylist), DynVecY(dylist), DynVecZ(dylist));
                break;
            case 4:
                d_set_normal(DynVecX(dylist), DynVecY(dylist), DynVecZ(dylist));
                break;
            case 5:
                d_set_scale(DynVecX(dylist), DynVecY(dylist), DynVecZ(dylist));
                break;
            case 49:
                d_make_vertex(DynVec(dylist));
                break;
            case 6:
                d_set_rotation(DynVecX(dylist), DynVecY(dylist), DynVecZ(dylist));
                break;
            case 27:
                d_center_of_gravity(DynVecX(dylist), DynVecY(dylist), DynVecZ(dylist));
                break;
            case 26:
                d_set_shape_offset(DynVecX(dylist), DynVecY(dylist), DynVecZ(dylist));
                break;
            case 44:
                d_set_parm_f(Dyn2AsInt(dylist), DynVecX(dylist));
                break;
            case 45:
                d_set_parm_ptr(Dyn2AsInt(dylist), Dyn1AsPtr(dylist));
                break;
            case 8:
                d_set_flags(Dyn2AsInt(dylist));
                break;
            case 9:
                d_clear_flags(Dyn2AsInt(dylist));
                break;
            case 7:
                d_set_obj_draw_flag(Dyn2AsInt(dylist));
                break;
            case 39:
                d_attach(Dyn1AsName(dylist));
                break;
            case 40:
                d_attachto_dynid(Dyn2AsInt(dylist), Dyn1AsName(dylist));
                break;
            case 41:
                d_set_att_offset(DynVec(dylist));
                break;
            case 21:
                d_set_nodegroup(Dyn1AsName(dylist));
                break;
            case 20:
                d_set_matgroup(Dyn1AsName(dylist));
                break;
            case 22:
                d_set_skinshape(Dyn1AsName(dylist));
                break;
            case 23:
                d_set_planegroup(Dyn1AsName(dylist));
                break;
            case 24:
                d_set_shapeptrptr(Dyn1AsPtr(dylist));
                break;
            case 25:
                d_set_shapeptr(Dyn1AsName(dylist));
                break;
            case 19:
                d_set_type(Dyn2AsInt(dylist));
                break;
            case 13:
                d_set_colour_num(Dyn2AsInt(dylist));
                break;
            case 10:
                d_friction(DynVecX(dylist), DynVecY(dylist), DynVecZ(dylist));
                break;
            case 11:
                d_set_spring(DynVecX(dylist));
                break;
            case 33:
                d_set_ambient(DynVecX(dylist), DynVecY(dylist), DynVecZ(dylist));
                break;
            case 34:
                d_set_diffuse(DynVecX(dylist), DynVecY(dylist), DynVecZ(dylist));
                break;
            case 31:
                d_set_control_type(Dyn2AsInt(dylist));
                break;
            case 32:
                d_set_skin_weight(Dyn2AsInt(dylist), DynVecX(dylist));
                break;
            case 35:
                d_set_id(Dyn2AsInt(dylist));
                break;
            case 36:
                d_set_material(Dyn1AsPtr(dylist), Dyn2AsInt(dylist));
                break;
            case 37:
                d_map_materials(Dyn1AsName(dylist));
                break;
            case 38:
                d_map_vertices(Dyn1AsName(dylist));
                break;
            case 53:
                d_set_texture_st(DynVecX(dylist), DynVecY(dylist));
                break;
            case 52:
                d_use_texture(Dyn2AsPtr(dylist));
                break;
            case 54:
                d_make_netfromshapeid(Dyn1AsName(dylist));
                break;
            case 55:
                d_make_netfromshape_ptrptr(Dyn1AsPtr(dylist));
                break;
            default:
                fatal_printf("proc_dynlist(): unkown command");
        }
        dylist++;
    }

    return sDynListCurObj;
}

void d_set_name_suffix(char *str) {
    if (str != NULL) {
        if (str[0] == '\0') {
            sprintf(sDynNameSuffix, "__%d", ++sUnnamedObjCount);
        } else {
            gd_strcpy(sDynNameSuffix, str);
        }
    } else {
        sDynNameSuffix[0] = '\0';
    }
}

void d_append_to_name_suffix(char *str) {
    char buf[0xff + 1];

    if (str != NULL) {
        if (str[0] == '\0') {
            sprintf(buf, "__%d", ++sUnnamedObjCount);
        } else {
            gd_strcpy(buf, str);
        }
    } else {
        buf[0] = '\0';
    }

    gd_strcat(sDynNameSuffix, buf);
}

static void stash_name_suffix(void) {
    gd_strcpy(sStashedDynNameSuffix, sDynNameSuffix);
}

static void unstash_name_suffix(void) {
    gd_strcpy(sDynNameSuffix, sStashedDynNameSuffix);
}

static struct DynObjInfo *get_dynobj_info(DynObjName name) {
    struct DynObjInfo *foundDynobj;
    char buf[0x100];
    s32 i;

    if (sLoadedDynObjs == 0) {
        return NULL;
    }

    if (sUseIntegerNames) {
        sprintf(buf, "N%d", DynNameAsInt(name));
    } else {
        gd_strcpy(buf, DynNameAsStr(name));
    }

    gd_strcat(buf, sDynNameSuffix);
    foundDynobj = NULL;
    for (i = 0; i < sLoadedDynObjs; i++) {
        if (gd_str_not_equal(sGdDynObjList[i].name, buf) == 0) {
            foundDynobj = &sGdDynObjList[i];
            break;
        }
    }

    return foundDynobj;
}

void reset_dynamic_objs(void) {
    UNUSED u8 filler[4];

    if (sLoadedDynObjs == 0) {
        return;
    }

    gd_free(sGdDynObjList);
    sLoadedDynObjs = 0;
    sGdDynObjList = NULL;
}

void d_add_net_with_subgroup(UNUSED s32 a0, DynObjName name) {
    d_makeobj(D_NET, name);
    d_set_obj_draw_flag(OBJ_INVISIBLE);

    sprintf(sDynNetNameSuffix, "c%d", ++sDynNetCount);
    d_set_type(4);
    stash_name_suffix();
    d_set_name_suffix(sDynNetNameSuffix);
    d_start_group(name);
    unstash_name_suffix();
    d_use_obj(name);
    sParentObjInfo = sDynListCurInfo;
}

void d_end_net_with_subgroup(DynObjName name) {
    d_use_obj(name);
    stash_name_suffix();
    d_set_name_suffix(sDynNetNameSuffix);
    d_end_group(name);
    d_set_nodegroup(name);
    unstash_name_suffix();
    sParentObjInfo = NULL;
}

void d_attach_joint_to_net(UNUSED s32 arg0, DynObjName name) {
    UNUSED struct DynObjInfo *curInfo = sDynListCurInfo;
    UNUSED u8 filler[8];

    d_makeobj(D_JOINT, name);
    d_set_type(3);
    d_set_shapeptrptr(NULL);
    d_attach_to(0xD, sParentObjInfo->obj);
    sParentObjInfo = sDynListCurInfo;
}

void d_make_netfromshapeid(DynObjName name) {
    struct DynObjInfo *dyninfo = get_dynobj_info(name);
    struct ObjNet *net;

    if (dyninfo == NULL) {
        fatal_printf("dMakeNetFromShape(\"%s\"): Undefined object", DynNameAsStr(name));
    }

    net = make_netfromshape((struct ObjShape *) dyninfo->obj);
    add_to_dynobj_list(&net->header, NULL);
}

void d_make_netfromshape_ptrptr(struct ObjShape **shapePtr) {
    UNUSED u8 filler[4];
    struct ObjNet *net = make_netfromshape(*shapePtr);

    printf("dMakeNetFromShapePtrPtr\n");

    add_to_dynobj_list(&net->header, NULL);
}

void add_to_dynobj_list(struct GdObj *newobj, DynObjName name) {
    UNUSED u8 filler[4];
    char idbuf[0x100];

    start_memtracker("dynlist");

    if (sGdDynObjList == NULL) {
        sGdDynObjList = gd_malloc_temp(DYNOBJ_LIST_SIZE * sizeof(struct DynObjInfo));
        if (sGdDynObjList == NULL) {
            fatal_printf("dMakeObj(): Cant allocate dynlist memory");
        }
    }

    stop_memtracker("dynlist");

    if (sUseIntegerNames) {
        sprintf(idbuf, "N%d", DynNameAsInt(name));
        name = NULL;
    } else {
        sprintf(idbuf, "U%d", ((u32) sLoadedDynObjs) + 1);
    }

    if (DynNameAsStr(name) != NULL) {
        if (get_dynobj_info(name) != NULL) {
            fatal_printf("dMakeObj(\"%s\"): Object with same name already exists", DynNameAsStr(name));
        }
        gd_strcpy(sGdDynObjList[sLoadedDynObjs].name, DynNameAsStr(name));
    } else {
        gd_strcpy(sGdDynObjList[sLoadedDynObjs].name, idbuf);
    }

    gd_strcat(sGdDynObjList[sLoadedDynObjs].name, sDynNameSuffix);

    if (gd_strlen(sGdDynObjList[sLoadedDynObjs].name) > (DYNOBJ_NAME_SIZE - 1)) {
        fatal_printf("dyn list obj name too long '%s'", sGdDynObjList[sLoadedDynObjs].name);
    }

    sGdDynObjList[sLoadedDynObjs].num = sLoadedDynObjs;
    sDynListCurInfo = &sGdDynObjList[sLoadedDynObjs];
    sGdDynObjList[sLoadedDynObjs++].obj = newobj;

    if (sLoadedDynObjs >= DYNOBJ_LIST_SIZE) {
        fatal_printf("dMakeObj(): Too many dynlist objects");
    }

    sDynListCurObj = newobj;
}

static char *integer_name_to_string(DynObjName name) {
    if (DynNameAsInt(name) != 0 && sUseIntegerNames) {
        sprintf(sIntToStringBuf, "N%d", DynNameAsInt(name));
        return sIntToStringBuf;
    }

    return NULL;
}

struct GdObj *d_makeobj(enum DObjTypes type, DynObjName name) {
    struct GdObj *dobj;
    UNUSED struct ObjGroup *dgroup;

    switch (type) {
        case D_CAR_DYNAMICS:
            fatal_printf("dmakeobj() Car dynamics are missing!");
            break;
        case D_JOINT:
            dobj = &make_joint(0, 0.0f, 0.0f, 0.0f)->header;
            break;
        case D_ANOTHER_JOINT:
            dobj = &make_joint(0, 0.0f, 0.0f, 0.0f)->header;
            break;
        case D_NET:
            dobj = &make_net(0, NULL, NULL, NULL, NULL)->header;
            break;
        case D_GROUP:
            dobj = &make_group(0)->header;
            dgroup = (struct ObjGroup *) dobj;
            break;
        case D_DATA_GRP:
            d_makeobj(D_GROUP, name);
            ((struct ObjGroup *) sDynListCurObj)->linkType = 1;

#ifdef AVOID_UB
            return NULL;
#else
            return;
#endif
        case D_CAMERA:
            dobj = &make_camera(0, NULL)->header;
            break;
        case D_BONE:
            dobj = &make_bone(0, NULL, NULL, 0)->header;
            break;
        case D_PARTICLE:
            dobj = &make_particle(0, 0, 0.0f, 0.0f, 0.0f)->header;
            break;
        case D_VERTEX:
            dobj = &gd_make_vertex(0.0f, 0.0f, 0.0f)->header;
            break;
        case D_FACE:
            dobj = &make_face_with_colour(1.0, 1.0, 1.0)->header;
            break;
        case D_PLANE:
            dobj = &make_plane(FALSE, NULL)->header;
            break;
        case D_MATERIAL:
            dobj = &make_material(0, NULL, 0)->header;
            break;
        case D_SHAPE:
            dobj = &make_shape(0, integer_name_to_string(name))->header;
            break;
        case D_GADGET:
            dobj = &make_gadget(0, 0)->header;
            break;
        case D_LABEL:

            dobj = &make_label(NULL, NULL, 8, 0, 0, 0)->header;
            break;
        case D_VIEW:
            dobj = &make_view(NULL,
                              (VIEW_2_COL_BUF | VIEW_ALLOC_ZBUF | VIEW_UNK_2000 | VIEW_UNK_4000
                               | VIEW_1_CYCLE | VIEW_MOVEMENT | VIEW_DRAW),
                              2, 0, 0, 0, 0, NULL)
                        ->header;
            break;
        case D_ANIMATOR:
            dobj = &make_animator()->header;
            break;
        case D_LIGHT:
            dobj = &make_light(0, NULL, 0)->header;
            addto_group(gGdLightGroup, dobj);
            break;
        default:
            fatal_printf("dMakeObj(): Unkown object type");
    }

    add_to_dynobj_list(dobj, name);
    return dobj;
}

void d_attach(DynObjName name) {
    struct DynObjInfo *info;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    info = get_dynobj_info(name);
    if (info == NULL) {
        fatal_printf("dAttach(\"%s\"): Undefined object", DynNameAsStr(name));
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dAttach()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_attach_to(s32 flag, struct GdObj *obj) {
    UNUSED u8 filler1[4];
    struct ObjGroup *attgrp;
    UNUSED u8 filler2[8];
    UNUSED struct DynObjInfo *curInfo = sDynListCurInfo;
    struct GdVec3f currObjPos;
    struct GdVec3f objPos;

    d_stash_dynobj();

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (obj->type) {
        case OBJ_TYPE_JOINTS:
            if ((attgrp = ((struct ObjJoint *) obj)->attachedObjsGrp) == NULL) {
                attgrp = ((struct ObjJoint *) obj)->attachedObjsGrp = make_group(0);
            }
            break;
        case OBJ_TYPE_NETS:
            if ((attgrp = ((struct ObjNet *) obj)->attachedObjsGrp) == NULL) {
                attgrp = ((struct ObjNet *) obj)->attachedObjsGrp = make_group(0);
            }
            break;
        case OBJ_TYPE_PARTICLES:
            if ((attgrp = ((struct ObjParticle *) obj)->attachedObjsGrp) == NULL) {
                attgrp = ((struct ObjParticle *) obj)->attachedObjsGrp = make_group(0);
            }
            break;
        case OBJ_TYPE_ANIMATORS:
            if ((attgrp = ((struct ObjAnimator *) obj)->attachedObjsGrp) == NULL) {
                attgrp = ((struct ObjAnimator *) obj)->attachedObjsGrp = make_group(0);
            }
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dAttachTo()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }

    if (group_contains_obj(attgrp, sDynListCurObj)) {
        return;
    }

    addto_group(attgrp, sDynListCurObj);

    if (flag & 9) {
        d_get_world_pos(&currObjPos);
        set_cur_dynobj(obj);
        d_get_world_pos(&objPos);

        currObjPos.x -= objPos.x;
        currObjPos.y -= objPos.y;
        currObjPos.z -= objPos.z;
    }

    d_unstash_dynobj();
    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            ((struct ObjJoint *) sDynListCurObj)->attachFlags = flag;
            ((struct ObjJoint *) sDynListCurObj)->attachedToObj = obj;
            break;
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) sDynListCurObj)->attachFlags = flag;
            ((struct ObjNet *) sDynListCurObj)->attachedToObj = obj;
            break;
        case OBJ_TYPE_PARTICLES:
            ((struct ObjParticle *) sDynListCurObj)->attachFlags = flag;
            ((struct ObjParticle *) sDynListCurObj)->attachedToObj = obj;
            break;
        case OBJ_TYPE_ANIMATORS:
            ((struct ObjAnimator *) sDynListCurObj)->attachFlags = flag;
            ((struct ObjAnimator *) sDynListCurObj)->attachedToObj = obj;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dAttachTo()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }

    if (flag & 9) {
        d_set_att_offset(&currObjPos);
    }
}

void d_attachto_dynid(s32 flag, DynObjName name) {
    struct DynObjInfo *info;

    if (name == NULL) {
        return;
    }
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    info = get_dynobj_info(name);
    if (info == NULL) {
        fatal_printf("dAttachTo(\"%s\"): Undefined object", DynNameAsStr(name));
    }

    d_attach_to(flag, info->obj);
}

void copy_bytes(u8 *src, u8 *dst, s32 num) {
    if (num == 0) {
        return;
    }
    while (num--) {
        *dst++ = *src++;
    }
}

void alloc_animdata(struct ObjAnimator *animator) {
    UNUSED u8 filler1[4];

    struct GdTriangleF tri;
    s16(*halfarr)[9];
    struct AnimDataInfo *curAnimSrc;
    struct AnimDataInfo *animDst;
    struct AnimDataInfo *animDataArr;
    struct ObjGroup *animgrp;
    s32 datasize;
    s32 dataIdx;
    s32 animCnt;
    s32 i;
    void *allocSpace;
    f32 allocMtxScale = 0.1f;
    struct AnimMtxVec *curMtxVec;
    UNUSED u8 filler2[4];

    start_memtracker("animdata");

    if ((animgrp = animator->animdataGrp) == NULL) {
        fatal_printf("no anim group");
    }

    if ((curAnimSrc = (struct AnimDataInfo *) animgrp->firstMember->obj) == NULL) {
        fatal_printf("no animation data");
    }

    animDst = curAnimSrc;
    animCnt = 0;
    while (animDst++->count >= 0) {
        animCnt++;
    }

    animDst = gd_malloc_perm(animCnt * sizeof(struct AnimDataInfo));
    if ((animDataArr = animDst) == NULL) {
        fatal_printf("cant allocate animation data");
    }

    for (i = 0; i < animCnt; i++) {
        allocSpace = NULL;
        if (curAnimSrc->type != 0) {
            switch (curAnimSrc->type) {
                case GD_ANIM_CAMERA_EYE3S_LOOKAT3S:
                    datasize = sizeof(s16[6]);
                    break;
                case GD_ANIM_ROT3S:
                    datasize = sizeof(s16[3]);
                    break;
                case GD_ANIM_POS3S:
                    datasize = sizeof(s16[3]);
                    break;
                case GD_ANIM_ROT3S_POS3S:
                    datasize = sizeof(s16[6]);
                    break;
                case GD_ANIM_SCALE3F_ROT3F_POS3F:
                    datasize = sizeof(f32[3][3]);
                    break;

                case GD_ANIM_SCALE3S_POS3S_ROT3S:
                    datasize = sizeof(struct AnimMtxVec);
                    break;
                case GD_ANIM_MTX4x4:
                    datasize = sizeof(Mat4f);
                    break;
                default:
                    fatal_printf("unknown anim type for allocation");
                    break;
            }

            allocSpace = gd_malloc_perm(curAnimSrc->count * datasize);
            if (allocSpace == NULL) {
                fatal_printf("cant allocate animation data");
            }

            if (curAnimSrc->type == GD_ANIM_SCALE3S_POS3S_ROT3S) {
                for (dataIdx = 0; dataIdx < curAnimSrc->count; dataIdx++) {
                    halfarr = &((s16(*)[9]) curAnimSrc->data)[dataIdx];
                    curMtxVec = &((struct AnimMtxVec *) allocSpace)[dataIdx];

                    tri.p0.x = (f32)(*halfarr)[0] * allocMtxScale;
                    tri.p0.y = (f32)(*halfarr)[1] * allocMtxScale;
                    tri.p0.z = (f32)(*halfarr)[2] * allocMtxScale;
                    tri.p1.x = (f32)(*halfarr)[3] * allocMtxScale;
                    tri.p1.y = (f32)(*halfarr)[4] * allocMtxScale;
                    tri.p1.z = (f32)(*halfarr)[5] * allocMtxScale;
                    tri.p2.x = (f32)(*halfarr)[6];
                    tri.p2.y = (f32)(*halfarr)[7];
                    tri.p2.z = (f32)(*halfarr)[8];

                    gd_set_identity_mat4(&curMtxVec->matrix);
                    gd_rot_mat_about_vec(&curMtxVec->matrix, &tri.p1);
                    gd_add_vec3f_to_mat4f_offset(&curMtxVec->matrix, &tri.p2);

                    ((struct AnimMtxVec *) allocSpace)[dataIdx].vec.x = tri.p0.x;
                    ((struct AnimMtxVec *) allocSpace)[dataIdx].vec.y = tri.p0.y;
                    ((struct AnimMtxVec *) allocSpace)[dataIdx].vec.z = tri.p0.z;
                }
                curAnimSrc->type = GD_ANIM_MTX4x4F_SCALE3F;
            } else {
                copy_bytes(curAnimSrc->data, allocSpace, curAnimSrc->count * datasize);
            }
        }

        animDst[i].type = curAnimSrc->type;
        animDst[i].count = curAnimSrc->count;
        animDst[i].data = allocSpace;

        curAnimSrc++;
    }

    animgrp->firstMember->obj = (void *) animDataArr;
    stop_memtracker("animdata");
}

void chk_shapegen(struct ObjShape *shape) {
    struct ObjFace *face;
    struct ObjVertex *vtx;
    struct ObjVertex **vtxbuf;
    struct ObjGroup *shapeMtls;
    struct ObjGroup *shapeFaces;
    struct ObjGroup *shapeVtx;
    UNUSED u8 filler[4];
    struct ObjGroup *madeFaces;
    struct ObjGroup *madeVtx;
    u32 i;
    struct GdVtxData *vtxdata;
    struct GdFaceData *facedata;
    struct GdObj *oldObjHead;

    start_memtracker("chk_shapegen");
    imin("chk_shapegen");
    shapeMtls = shape->mtlGroup;
    shapeFaces = shape->faceGroup;
    shapeVtx = shape->vtxGroup;

    if (shapeVtx != NULL && shapeFaces != NULL) {
        if ((shapeVtx->linkType & 1) && (shapeFaces->linkType & 1)) {

            vtxdata = (struct GdVtxData *) shapeVtx->firstMember->obj;
            facedata = (struct GdFaceData *) shapeFaces->firstMember->obj;
            if (facedata->type != 1) {
                fatal_printf("unsupported poly type");
            }

            if (vtxdata->type != 1) {
                fatal_printf("unsupported vertex type");
            }

            if (vtxdata->count >= VTX_BUF_SIZE) {
                fatal_printf("shapegen() too many vertices");
            }

            vtxbuf = gd_malloc_temp(VTX_BUF_SIZE * sizeof(struct ObjVertex *));
            oldObjHead = gGdObjectList;

            for (i = 0; i < vtxdata->count; i++) {
                vtx = gd_make_vertex(vtxdata->data[i][0], vtxdata->data[i][1], vtxdata->data[i][2]);
                vtx->normal.x = vtx->normal.y = vtx->normal.z = 0.0f;
                vtxbuf[i] = vtx;
            }

            madeVtx = make_group_of_type(OBJ_TYPE_VERTICES, oldObjHead, NULL);

            oldObjHead = gGdObjectList;
            for (i = 0; i < facedata->count; i++) {

                face = make_face_with_colour(1.0, 1.0, 1.0);
                face->mtlId = (s32) facedata->data[i][0];
                add_3_vtx_to_face(face, vtxbuf[facedata->data[i][1]], vtxbuf[facedata->data[i][2]],
                                  vtxbuf[facedata->data[i][3]]);
                vtxbuf[facedata->data[i][1]]->normal.x += face->normal.x;
                vtxbuf[facedata->data[i][1]]->normal.y += face->normal.y;
                vtxbuf[facedata->data[i][1]]->normal.z += face->normal.z;

                vtxbuf[facedata->data[i][2]]->normal.x += face->normal.x;
                vtxbuf[facedata->data[i][2]]->normal.y += face->normal.y;
                vtxbuf[facedata->data[i][2]]->normal.z += face->normal.z;

                vtxbuf[facedata->data[i][3]]->normal.x += face->normal.x;
                vtxbuf[facedata->data[i][3]]->normal.y += face->normal.y;
                vtxbuf[facedata->data[i][3]]->normal.z += face->normal.z;
            }

            if (shape->flag & 0x10) {
                for (i = 0; i < vtxdata->count; i++) {
                    vtxbuf[i]->normal.x = vtxbuf[i]->pos.x;
                    vtxbuf[i]->normal.y = vtxbuf[i]->pos.y;
                    vtxbuf[i]->normal.z = vtxbuf[i]->pos.z;
                    gd_normalize_vec3f(&vtxbuf[i]->normal);
                }
            } else {
                for (i = 0; i < vtxdata->count; i++) {
                    gd_normalize_vec3f(&vtxbuf[i]->normal);
                }
            }

            gd_free(vtxbuf);
            madeFaces = make_group_of_type(OBJ_TYPE_FACES, oldObjHead, NULL);
            shape->faceGroup = madeFaces;
            shape->vtxGroup = madeVtx;
        }
    }

    if (shapeMtls != NULL) {
        if (shape->faceGroup) {
            map_face_materials(shape->faceGroup, shapeMtls);
        } else {
            fatal_printf("chk_shapegen() please set face group before mats");
        }
    }

    imout();
    stop_memtracker("chk_shapegen");
}

void d_set_nodegroup(DynObjName name) {
    struct DynObjInfo *info;
    UNUSED u8 filler[8];

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    info = get_dynobj_info(name);
    if (info == NULL) {
        fatal_printf("dSetNodeGroup(\"%s\"): Undefined group", DynNameAsStr(name));
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) sDynListCurObj)->unk1C8 = (struct ObjGroup *) info->obj;
            ((struct ObjNet *) sDynListCurObj)->unk1D0 = (struct ObjGroup *) info->obj;
            break;
        case OBJ_TYPE_SHAPES:
            ((struct ObjShape *) sDynListCurObj)->vtxGroup = (struct ObjGroup *) info->obj;
            chk_shapegen((struct ObjShape *) sDynListCurObj);
            break;
        case OBJ_TYPE_GADGETS:
            ((struct ObjGadget *) sDynListCurObj)->unk54 = (struct ObjGroup *) info->obj;
            break;
        case OBJ_TYPE_ANIMATORS:
            ((struct ObjAnimator *) sDynListCurObj)->animdataGrp = (struct ObjGroup *) info->obj;
            alloc_animdata((struct ObjAnimator *) sDynListCurObj);
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetNodeGroup()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_matgroup(DynObjName name) {
    struct DynObjInfo *info;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    info = get_dynobj_info(name);
    if (info == NULL) {
        fatal_printf("dSetMatGroup(\"%s\"): Undefined group", DynNameAsStr(name));
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_SHAPES:
            ((struct ObjShape *) sDynListCurObj)->mtlGroup = (struct ObjGroup *) info->obj;
            chk_shapegen((struct ObjShape *) sDynListCurObj);
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetMatGroup()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_texture_st(UNUSED f32 s, UNUSED f32 t) {
    UNUSED u8 filler[8];

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_VERTICES:
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetTextureST()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_use_texture(void *texture) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_MATERIALS:
            ((struct ObjMaterial *) sDynListCurObj)->texture = texture;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dUseTexture()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_skinshape(DynObjName name) {
    struct DynObjInfo *info;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    info = get_dynobj_info(name);
    if (info == NULL) {
        fatal_printf("dSetSkinShape(\"%s\"): Undefined object", DynNameAsStr(name));
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) sDynListCurObj)->skinGrp = ((struct ObjShape *) info->obj)->vtxGroup;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetSkinShape()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_map_materials(DynObjName name) {
    struct DynObjInfo *info;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    info = get_dynobj_info(name);
    if (info == NULL) {
        fatal_printf("dMapMaterials(\"%s\"): Undefined group", DynNameAsStr(name));
    }

    map_face_materials((struct ObjGroup *) sDynListCurObj, (struct ObjGroup *) info->obj);
}

void d_map_vertices(DynObjName name) {
    struct DynObjInfo *info;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    info = get_dynobj_info(name);
    if (info == NULL) {
        fatal_printf("dMapVertices(\"%s\"): Undefined group", DynNameAsStr(name));
    }

    map_vertices((struct ObjGroup *) sDynListCurObj, (struct ObjGroup *) info->obj);
}

void d_set_planegroup(DynObjName name) {
    struct DynObjInfo *info;
    UNUSED u8 filler[8];

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    info = get_dynobj_info(name);
    if (info == NULL) {
        fatal_printf("dSetPlaneGroup(\"%s\"): Undefined group", DynNameAsStr(name));
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) sDynListCurObj)->unk1CC = (struct ObjGroup *) info->obj;
            break;
        case OBJ_TYPE_SHAPES:
            ((struct ObjShape *) sDynListCurObj)->faceGroup = (struct ObjGroup *) info->obj;
            chk_shapegen((struct ObjShape *) sDynListCurObj);
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetPlaneGroup()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_shapeptrptr(struct ObjShape **shpPtrptr) {
    struct ObjShape *defaultptr = NULL;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    if (shpPtrptr == NULL) {
        shpPtrptr = &defaultptr;
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            ((struct ObjJoint *) sDynListCurObj)->shapePtr = *shpPtrptr;
            ((struct ObjJoint *) sDynListCurObj)->colourNum = 0;
            break;
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) sDynListCurObj)->shapePtr = *shpPtrptr;
            break;
        case OBJ_TYPE_BONES:
            ((struct ObjBone *) sDynListCurObj)->shapePtr = *shpPtrptr;
            break;
        case OBJ_TYPE_GADGETS:
            ((struct ObjGadget *) sDynListCurObj)->shapePtr = *shpPtrptr;
            break;
        case OBJ_TYPE_PARTICLES:
            ((struct ObjParticle *) sDynListCurObj)->shapePtr = *shpPtrptr;
            break;
        case OBJ_TYPE_LIGHTS:
            ((struct ObjLight *) sDynListCurObj)->unk9C = *shpPtrptr;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetShapePtrPtr()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_shapeptr(DynObjName name) {
    struct DynObjInfo *info;
    if (name == NULL) {
        return;
    }

    info = get_dynobj_info(name);
    if (info == NULL) {
        fatal_printf("dSetShapePtr(\"%s\"): Undefined object", DynNameAsStr(name));
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            ((struct ObjJoint *) sDynListCurObj)->shapePtr = (struct ObjShape *) info->obj;
            ((struct ObjJoint *) sDynListCurObj)->colourNum = 0;
            break;
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) sDynListCurObj)->shapePtr = (struct ObjShape *) info->obj;
            break;
        case OBJ_TYPE_BONES:
            ((struct ObjBone *) sDynListCurObj)->shapePtr = (struct ObjShape *) info->obj;
            break;
        case OBJ_TYPE_GADGETS:
            ((struct ObjGadget *) sDynListCurObj)->shapePtr = (struct ObjShape *) info->obj;
            break;
        case OBJ_TYPE_PARTICLES:
            ((struct ObjParticle *) sDynListCurObj)->shapePtr = (struct ObjShape *) info->obj;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetShapePtr()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

struct GdObj *d_use_obj(DynObjName name) {
    struct DynObjInfo *info = get_dynobj_info(name);
    if (info == NULL) {
        fatal_printf("dUseObj(\"%s\"): Undefined object", DynNameAsStr(name));
    }

    sDynListCurObj = info->obj;
    sDynListCurInfo = info;

    return info->obj;
}

void set_cur_dynobj(struct GdObj *obj) {
    sDynListCurObj = obj;
    sDynListCurInfo = &sNullDynObjInfo;
}

void d_start_group(DynObjName name) {
    d_makeobj(D_GROUP, name);
}

void d_end_group(DynObjName name) {
    UNUSED u8 filler[4];
    struct DynObjInfo *info = get_dynobj_info(name);
    struct ObjGroup *dynGrp;
    s32 i;

    if (info == NULL) {
        fatal_printf("dEndGroup(\"%s\"): Undefined group", DynNameAsStr(name));
    }

    dynGrp = (struct ObjGroup *) info->obj;
    for (i = info->num + 1; i < sLoadedDynObjs; i++) {
        if (sGdDynObjList[i].obj->type != OBJ_TYPE_GROUPS) {
            addto_group(dynGrp, sGdDynObjList[i].obj);
        }
    }
}

void d_addto_group(DynObjName name) {
    UNUSED u8 filler[4];
    struct DynObjInfo *info = get_dynobj_info(name);
    struct ObjGroup *targetGrp;

    if (info == NULL) {
        fatal_printf("dAddToGroup(\"%s\"): Undefined group", DynNameAsStr(name));
    }

    targetGrp = (struct ObjGroup *) info->obj;
    addto_group(targetGrp, sDynListCurObj);
}

void d_use_integer_names(s32 isIntBool) {
    sUseIntegerNames = isIntBool;
}

void d_set_init_pos(f32 x, f32 y, f32 z) {
    UNUSED u8 filler1[12];
    struct GdObj *dynobj = sDynListCurObj;
    UNUSED u8 filler2[4];

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            ((struct ObjJoint *) dynobj)->worldPos.x = x;
            ((struct ObjJoint *) dynobj)->worldPos.y = y;
            ((struct ObjJoint *) dynobj)->worldPos.z = z;

            ((struct ObjJoint *) dynobj)->unk3C.x = x;
            ((struct ObjJoint *) dynobj)->unk3C.y = y;
            ((struct ObjJoint *) dynobj)->unk3C.z = z;

            ((struct ObjJoint *) dynobj)->initPos.x = x;
            ((struct ObjJoint *) dynobj)->initPos.y = y;
            ((struct ObjJoint *) dynobj)->initPos.z = z;
            break;
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) dynobj)->worldPos.x = x;
            ((struct ObjNet *) dynobj)->worldPos.y = y;
            ((struct ObjNet *) dynobj)->worldPos.z = z;

            ((struct ObjNet *) dynobj)->initPos.x = x;
            ((struct ObjNet *) dynobj)->initPos.y = y;
            ((struct ObjNet *) dynobj)->initPos.z = z;
            break;
        case OBJ_TYPE_PARTICLES:
            ((struct ObjParticle *) dynobj)->pos.x = x;
            ((struct ObjParticle *) dynobj)->pos.y = y;
            ((struct ObjParticle *) dynobj)->pos.z = z;
            break;
        case OBJ_TYPE_CAMERAS:
            ((struct ObjCamera *) dynobj)->worldPos.x = x;
            ((struct ObjCamera *) dynobj)->worldPos.y = y;
            ((struct ObjCamera *) dynobj)->worldPos.z = z;
            break;
        case OBJ_TYPE_VERTICES:
            d_set_rel_pos(x, y, z);

            ((struct ObjVertex *) dynobj)->initPos.x = x;
            ((struct ObjVertex *) dynobj)->initPos.y = y;
            ((struct ObjVertex *) dynobj)->initPos.z = z;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetInitPos()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_velocity(const struct GdVec3f *vel) {
    struct GdObj *dynobj = sDynListCurObj;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            ((struct ObjJoint *) dynobj)->velocity.x = vel->x;
            ((struct ObjJoint *) dynobj)->velocity.y = vel->y;
            ((struct ObjJoint *) dynobj)->velocity.z = vel->z;
            break;
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) dynobj)->velocity.x = vel->x;
            ((struct ObjNet *) dynobj)->velocity.y = vel->y;
            ((struct ObjNet *) dynobj)->velocity.z = vel->z;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetVelocity()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_get_velocity(struct GdVec3f *dst) {
    struct GdObj *dynobj = sDynListCurObj;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            dst->x = ((struct ObjJoint *) dynobj)->velocity.x;
            dst->y = ((struct ObjJoint *) dynobj)->velocity.y;
            dst->z = ((struct ObjJoint *) dynobj)->velocity.z;
            break;
        case OBJ_TYPE_NETS:
            dst->x = ((struct ObjNet *) dynobj)->velocity.x;
            dst->y = ((struct ObjNet *) dynobj)->velocity.y;
            dst->z = ((struct ObjNet *) dynobj)->velocity.z;
            break;
        default:
            dst->x = dst->y = dst->z = 0.0f;
            break;
    }
}

void d_set_torque(const struct GdVec3f *src) {
    struct GdObj *dynobj = sDynListCurObj;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) dynobj)->torque.x = src->x;
            ((struct ObjNet *) dynobj)->torque.y = src->y;
            ((struct ObjNet *) dynobj)->torque.z = src->z;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetTorque()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_get_init_pos(struct GdVec3f *dst) {
    struct GdObj *dynobj = sDynListCurObj;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            dst->x = ((struct ObjJoint *) dynobj)->initPos.x;
            dst->y = ((struct ObjJoint *) dynobj)->initPos.y;
            dst->z = ((struct ObjJoint *) dynobj)->initPos.z;
            break;
        case OBJ_TYPE_NETS:
            dst->x = ((struct ObjNet *) dynobj)->initPos.x;
            dst->y = ((struct ObjNet *) dynobj)->initPos.y;
            dst->z = ((struct ObjNet *) dynobj)->initPos.z;
            break;
        case OBJ_TYPE_VERTICES:
            dst->x = ((struct ObjVertex *) dynobj)->initPos.x;
            dst->y = ((struct ObjVertex *) dynobj)->initPos.y;
            dst->z = ((struct ObjVertex *) dynobj)->initPos.z;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dGetInitPos()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_get_init_rot(struct GdVec3f *dst) {
    struct GdObj *dynobj = sDynListCurObj;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            dst->x = ((struct ObjJoint *) dynobj)->unk6C.x;
            dst->y = ((struct ObjJoint *) dynobj)->unk6C.y;
            dst->z = ((struct ObjJoint *) dynobj)->unk6C.z;
            break;
        case OBJ_TYPE_NETS:
            dst->x = ((struct ObjNet *) dynobj)->unk68.x;
            dst->y = ((struct ObjNet *) dynobj)->unk68.y;
            dst->z = ((struct ObjNet *) dynobj)->unk68.z;
            break;
        case OBJ_TYPE_LIGHTS:
            dst->x = dst->y = dst->z = 0.0f;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dGetInitRot()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_rel_pos(f32 x, f32 y, f32 z) {
    struct GdObj *dynobj = sDynListCurObj;
    UNUSED struct GdVec3f unusedVec;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            ((struct ObjJoint *) dynobj)->unk3C.x = x;
            ((struct ObjJoint *) dynobj)->unk3C.y = y;
            ((struct ObjJoint *) dynobj)->unk3C.z = z;
            break;
        case OBJ_TYPE_CAMERAS:
            unusedVec.x = x;
            unusedVec.y = y;
            unusedVec.z = z;

            ((struct ObjCamera *) dynobj)->unk40.x = x;
            ((struct ObjCamera *) dynobj)->unk40.y = y;
            ((struct ObjCamera *) dynobj)->unk40.z = z;

            ((struct ObjCamera *) dynobj)->zoomPositions[0].x = x;
            ((struct ObjCamera *) dynobj)->zoomPositions[0].y = y;
            ((struct ObjCamera *) dynobj)->zoomPositions[0].z = z;

            ((struct ObjCamera *) dynobj)->zoomPositions[1].x = x * 1.5;
            ((struct ObjCamera *) dynobj)->zoomPositions[1].y = y * 1.5;
            ((struct ObjCamera *) dynobj)->zoomPositions[1].z = z * 1.5;

            ((struct ObjCamera *) dynobj)->zoomPositions[2].x = x * 2.0f;
            ((struct ObjCamera *) dynobj)->zoomPositions[2].y = y * 2.0f;
            ((struct ObjCamera *) dynobj)->zoomPositions[2].z = z * 2.0f;

            ((struct ObjCamera *) dynobj)->maxZoomLevel = 2;
            break;
        case OBJ_TYPE_VERTICES:
            ((struct ObjVertex *) dynobj)->pos.x = x;
            ((struct ObjVertex *) dynobj)->pos.y = y;
            ((struct ObjVertex *) dynobj)->pos.z = z;
            break;
        case OBJ_TYPE_LABELS:
            ((struct ObjLabel *) dynobj)->position.x = x;
            ((struct ObjLabel *) dynobj)->position.y = y;
            ((struct ObjLabel *) dynobj)->position.z = z;
            break;
        case OBJ_TYPE_PARTICLES:
            ((struct ObjParticle *) dynobj)->pos.x = x;
            ((struct ObjParticle *) dynobj)->pos.y = y;
            ((struct ObjParticle *) dynobj)->pos.z = z;
            break;
        case OBJ_TYPE_NETS:
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetRelPos()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_addto_rel_pos(struct GdVec3f *src) {
    struct GdObj *dynobj = sDynListCurObj;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_VERTICES:
            ((struct ObjVertex *) dynobj)->pos.x += src->x;
            ((struct ObjVertex *) dynobj)->pos.y += src->y;
            ((struct ObjVertex *) dynobj)->pos.z += src->z;
            break;
        case OBJ_TYPE_JOINTS:
            ((struct ObjJoint *) dynobj)->unk3C.x += src->x;
            ((struct ObjJoint *) dynobj)->unk3C.y += src->y;
            ((struct ObjJoint *) dynobj)->unk3C.z += src->z;
            break;
        case OBJ_TYPE_PARTICLES:
            ((struct ObjParticle *) dynobj)->pos.x += src->x;
            ((struct ObjParticle *) dynobj)->pos.y += src->y;
            ((struct ObjParticle *) dynobj)->pos.z += src->z;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dAddToRelPos()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_get_rel_pos(struct GdVec3f *dst) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_VERTICES:
            dst->x = ((struct ObjVertex *) sDynListCurObj)->pos.x;
            dst->y = ((struct ObjVertex *) sDynListCurObj)->pos.y;
            dst->z = ((struct ObjVertex *) sDynListCurObj)->pos.z;
            break;
        case OBJ_TYPE_JOINTS:
            dst->x = ((struct ObjJoint *) sDynListCurObj)->unk3C.x;
            dst->y = ((struct ObjJoint *) sDynListCurObj)->unk3C.y;
            dst->z = ((struct ObjJoint *) sDynListCurObj)->unk3C.z;
            break;
        case OBJ_TYPE_CAMERAS:
            dst->x = ((struct ObjCamera *) sDynListCurObj)->unk40.x;
            dst->y = ((struct ObjCamera *) sDynListCurObj)->unk40.y;
            dst->z = ((struct ObjCamera *) sDynListCurObj)->unk40.z;
            break;
        case OBJ_TYPE_PARTICLES:
            dst->x = ((struct ObjParticle *) sDynListCurObj)->pos.x;
            dst->y = ((struct ObjParticle *) sDynListCurObj)->pos.y;
            dst->z = ((struct ObjParticle *) sDynListCurObj)->pos.z;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dGetRelPos()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

struct ObjGroup *d_get_att_objgroup(void) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            return ((struct ObjJoint *) sDynListCurObj)->attachedObjsGrp;
            break;
        case OBJ_TYPE_NETS:
            return ((struct ObjNet *) sDynListCurObj)->attachedObjsGrp;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dGetAttObjGroup()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }

}

struct GdObj *d_get_att_to_obj(void) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            return ((struct ObjJoint *) sDynListCurObj)->attachedToObj;
            break;
        case OBJ_TYPE_NETS:
            return ((struct ObjNet *) sDynListCurObj)->attachedToObj;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dGetAttToObj()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }

}

void d_get_scale(struct GdVec3f *dst) {
    struct GdObj *dynobj;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    dynobj = sDynListCurObj;
    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            dst->x = ((struct ObjJoint *) dynobj)->scale.x;
            dst->y = ((struct ObjJoint *) dynobj)->scale.y;
            dst->z = ((struct ObjJoint *) dynobj)->scale.z;
            break;
        case OBJ_TYPE_NETS:
            dst->x = ((struct ObjNet *) dynobj)->scale.x;
            dst->y = ((struct ObjNet *) dynobj)->scale.y;
            dst->z = ((struct ObjNet *) dynobj)->scale.z;
            break;
        case OBJ_TYPE_LIGHTS:
            dst->x = 1.0f;
            dst->y = 1.0f;
            dst->z = 1.0f;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dGetScale()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_att_offset(const struct GdVec3f *off) {
    struct GdObj *dynobj;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    dynobj = sDynListCurObj;
    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            ((struct ObjJoint *) dynobj)->attachOffset.x = off->x;
            ((struct ObjJoint *) dynobj)->attachOffset.y = off->y;
            ((struct ObjJoint *) dynobj)->attachOffset.z = off->z;

            ((struct ObjJoint *) dynobj)->initPos.x = off->x;
            ((struct ObjJoint *) dynobj)->initPos.y = off->y;
            ((struct ObjJoint *) dynobj)->initPos.z = off->z;
            break;
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) dynobj)->attachOffset.x = off->x;
            ((struct ObjNet *) dynobj)->attachOffset.y = off->y;
            ((struct ObjNet *) dynobj)->attachOffset.z = off->z;

            ((struct ObjNet *) dynobj)->initPos.x = off->x;
            ((struct ObjNet *) dynobj)->initPos.y = off->y;
            ((struct ObjNet *) dynobj)->initPos.z = off->z;
            break;
        case OBJ_TYPE_PARTICLES:
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetAttOffset()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_att_to_offset(UNUSED u32 a) {
    struct GdObj *dynobj;
    UNUSED u8 filler[24];

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    dynobj = sDynListCurObj;
    d_stash_dynobj();
    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            set_cur_dynobj(((struct ObjJoint *) dynobj)->attachedToObj);
            break;
        case OBJ_TYPE_NETS:
            set_cur_dynobj(((struct ObjNet *) dynobj)->attachedToObj);
            break;
        case OBJ_TYPE_PARTICLES:
            set_cur_dynobj(((struct ObjParticle *) dynobj)->attachedToObj);
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetAttToOffset()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }

    if (sDynListCurObj == NULL) {
        fatal_printf("dSetAttOffset(): Object '%s' isnt attached to anything",
                     sStashedDynObjInfo->name);
    }
    d_set_att_to_offset(a);
    d_unstash_dynobj();
}

void d_get_att_offset(struct GdVec3f *dst) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            dst->x = ((struct ObjJoint *) sDynListCurObj)->attachOffset.x;
            dst->y = ((struct ObjJoint *) sDynListCurObj)->attachOffset.y;
            dst->z = ((struct ObjJoint *) sDynListCurObj)->attachOffset.z;
            break;
        case OBJ_TYPE_NETS:
            dst->x = ((struct ObjNet *) sDynListCurObj)->attachOffset.x;
            dst->y = ((struct ObjNet *) sDynListCurObj)->attachOffset.y;
            dst->z = ((struct ObjNet *) sDynListCurObj)->attachOffset.z;
            break;
        case OBJ_TYPE_PARTICLES:
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dGetAttOffset()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

s32 d_get_att_flags(void) {
    s32 attflag;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            attflag = ((struct ObjJoint *) sDynListCurObj)->attachFlags;
            break;
        case OBJ_TYPE_NETS:
            attflag = ((struct ObjNet *) sDynListCurObj)->attachFlags;
            break;
        case OBJ_TYPE_PARTICLES:
            attflag = ((struct ObjParticle *) sDynListCurObj)->attachFlags;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dGetAttFlags()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }

    return attflag;
}

void d_set_world_pos(f32 x, f32 y, f32 z) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_CAMERAS:
            ((struct ObjCamera *) sDynListCurObj)->worldPos.x = x;
            ((struct ObjCamera *) sDynListCurObj)->worldPos.y = y;
            ((struct ObjCamera *) sDynListCurObj)->worldPos.z = z;
            break;
        case OBJ_TYPE_JOINTS:
            ((struct ObjJoint *) sDynListCurObj)->worldPos.x = x;
            ((struct ObjJoint *) sDynListCurObj)->worldPos.y = y;
            ((struct ObjJoint *) sDynListCurObj)->worldPos.z = z;
            break;
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) sDynListCurObj)->worldPos.x = x;
            ((struct ObjNet *) sDynListCurObj)->worldPos.y = y;
            ((struct ObjNet *) sDynListCurObj)->worldPos.z = z;
            break;
        case OBJ_TYPE_GADGETS:
            ((struct ObjGadget *) sDynListCurObj)->worldPos.x = x;
            ((struct ObjGadget *) sDynListCurObj)->worldPos.y = y;
            ((struct ObjGadget *) sDynListCurObj)->worldPos.z = z;
            break;
        case OBJ_TYPE_VIEWS:
            ((struct ObjView *) sDynListCurObj)->upperLeft.x = x;
            ((struct ObjView *) sDynListCurObj)->upperLeft.y = y;
            ((struct ObjView *) sDynListCurObj)->upperLeft.z = z;
            break;
        case OBJ_TYPE_VERTICES:
            ((struct ObjVertex *) sDynListCurObj)->pos.x = x;
            ((struct ObjVertex *) sDynListCurObj)->pos.y = y;
            ((struct ObjVertex *) sDynListCurObj)->pos.z = z;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetWorldPos()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_normal(f32 x, f32 y, f32 z) {
    struct GdVec3f normal;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    normal.x = x;
    normal.y = y;
    normal.z = z;
    gd_normalize_vec3f(&normal);

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_VERTICES:
            ((struct ObjVertex *) sDynListCurObj)->normal.x = normal.x;
            ((struct ObjVertex *) sDynListCurObj)->normal.y = normal.y;
            ((struct ObjVertex *) sDynListCurObj)->normal.z = normal.z;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetNormal()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

struct GdVec3f *d_get_world_pos_ptr(void) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_VERTICES:
            return &((struct ObjVertex *) sDynListCurObj)->pos;
            break;
        case OBJ_TYPE_PARTICLES:
            return &((struct ObjParticle *) sDynListCurObj)->pos;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dGetWorldPosPtr()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }

}

void d_get_world_pos(struct GdVec3f *dst) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_VERTICES:
            dst->x = ((struct ObjVertex *) sDynListCurObj)->pos.x;
            dst->y = ((struct ObjVertex *) sDynListCurObj)->pos.y;
            dst->z = ((struct ObjVertex *) sDynListCurObj)->pos.z;
            break;
        case OBJ_TYPE_JOINTS:
            dst->x = ((struct ObjJoint *) sDynListCurObj)->worldPos.x;
            dst->y = ((struct ObjJoint *) sDynListCurObj)->worldPos.y;
            dst->z = ((struct ObjJoint *) sDynListCurObj)->worldPos.z;
            break;
        case OBJ_TYPE_NETS:
            dst->x = ((struct ObjNet *) sDynListCurObj)->worldPos.x;
            dst->y = ((struct ObjNet *) sDynListCurObj)->worldPos.y;
            dst->z = ((struct ObjNet *) sDynListCurObj)->worldPos.z;
            break;
        case OBJ_TYPE_PARTICLES:
            dst->x = ((struct ObjParticle *) sDynListCurObj)->pos.x;
            dst->y = ((struct ObjParticle *) sDynListCurObj)->pos.y;
            dst->z = ((struct ObjParticle *) sDynListCurObj)->pos.z;
            break;
        case OBJ_TYPE_CAMERAS:
            dst->x = ((struct ObjCamera *) sDynListCurObj)->worldPos.x;
            dst->y = ((struct ObjCamera *) sDynListCurObj)->worldPos.y;
            dst->z = ((struct ObjCamera *) sDynListCurObj)->worldPos.z;
            break;
        case OBJ_TYPE_BONES:
            dst->x = ((struct ObjBone *) sDynListCurObj)->worldPos.x;
            dst->y = ((struct ObjBone *) sDynListCurObj)->worldPos.y;
            dst->z = ((struct ObjBone *) sDynListCurObj)->worldPos.z;
            break;
        case OBJ_TYPE_SHAPES:
            dst->x = dst->y = dst->z = 0.0f;
            break;
        case OBJ_TYPE_LABELS:
            dst->x = dst->y = dst->z = 0.0f;
            break;
        case OBJ_TYPE_GADGETS:
            dst->x = ((struct ObjGadget *) sDynListCurObj)->worldPos.x;
            dst->y = ((struct ObjGadget *) sDynListCurObj)->worldPos.y;
            dst->z = ((struct ObjGadget *) sDynListCurObj)->worldPos.z;
            break;
        case OBJ_TYPE_PLANES:
            dst->x = ((struct ObjPlane *) sDynListCurObj)->boundingBox.minX;
            dst->y = ((struct ObjPlane *) sDynListCurObj)->boundingBox.minY;
            dst->z = ((struct ObjPlane *) sDynListCurObj)->boundingBox.minZ;

            dst->x += ((struct ObjPlane *) sDynListCurObj)->boundingBox.maxX;
            dst->y += ((struct ObjPlane *) sDynListCurObj)->boundingBox.maxY;
            dst->z += ((struct ObjPlane *) sDynListCurObj)->boundingBox.maxZ;

            dst->x *= 0.5;
            dst->y *= 0.5;
            dst->z *= 0.5;
            break;
        case OBJ_TYPE_ZONES:
            dst->x = ((struct ObjZone *) sDynListCurObj)->boundingBox.minX;
            dst->y = ((struct ObjZone *) sDynListCurObj)->boundingBox.minY;
            dst->z = ((struct ObjZone *) sDynListCurObj)->boundingBox.minZ;

            dst->x += ((struct ObjZone *) sDynListCurObj)->boundingBox.maxX;
            dst->y += ((struct ObjZone *) sDynListCurObj)->boundingBox.maxY;
            dst->z += ((struct ObjZone *) sDynListCurObj)->boundingBox.maxZ;

            dst->x *= 0.5;
            dst->y *= 0.5;
            dst->z *= 0.5;
            break;
        case OBJ_TYPE_LIGHTS:
            dst->x = ((struct ObjLight *) sDynListCurObj)->position.x;
            dst->y = ((struct ObjLight *) sDynListCurObj)->position.y;
            dst->z = ((struct ObjLight *) sDynListCurObj)->position.z;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dGetWorldPos()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_make_vertex(struct GdVec3f *pos) {
    d_makeobj(D_VERTEX, AsDynName(NULL));
    d_set_init_pos(pos->x, pos->y, pos->z);
}

void d_set_scale(f32 x, f32 y, f32 z) {
    struct GdObj *initDynobj;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    initDynobj = sDynListCurObj;
    d_stash_dynobj();
    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            ((struct ObjJoint *) initDynobj)->scale.x = x;
            ((struct ObjJoint *) initDynobj)->scale.y = y;
            ((struct ObjJoint *) initDynobj)->scale.z = z;
            break;
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) initDynobj)->scale.x = x;
            ((struct ObjNet *) initDynobj)->scale.y = y;
            ((struct ObjNet *) initDynobj)->scale.z = z;
            break;
        case OBJ_TYPE_VIEWS:
            ((struct ObjView *) initDynobj)->lowerRight.x = x;
            ((struct ObjView *) initDynobj)->lowerRight.y = y;
            ((struct ObjView *) initDynobj)->lowerRight.z = z;
            break;
        case OBJ_TYPE_PARTICLES:
            break;
        case OBJ_TYPE_GADGETS:
            if (((struct ObjGadget *) initDynobj)->shapePtr != NULL) {
                scale_verts_in_shape(((struct ObjGadget *) initDynobj)->shapePtr, x, y, z);
            }
            ((struct ObjGadget *) initDynobj)->size.x = x;
            ((struct ObjGadget *) initDynobj)->size.y = y;
            ((struct ObjGadget *) initDynobj)->size.z = z;
            break;
        case OBJ_TYPE_LIGHTS:
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetScale()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
    d_unstash_dynobj();
}

void d_set_rotation(f32 x, f32 y, f32 z) {
    struct GdObj *dynobj;
    UNUSED u8 filler[4];

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    dynobj = sDynListCurObj;
    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            ((struct ObjJoint *) dynobj)->unk6C.x = x;
            ((struct ObjJoint *) dynobj)->unk6C.y = y;
            ((struct ObjJoint *) dynobj)->unk6C.z = z;
            break;
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) dynobj)->unk68.x = x;
            ((struct ObjNet *) dynobj)->unk68.y = y;
            ((struct ObjNet *) dynobj)->unk68.z = z;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetRotation()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_center_of_gravity(f32 x, f32 y, f32 z) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) sDynListCurObj)->centerOfGravity.x = x;
            ((struct ObjNet *) sDynListCurObj)->centerOfGravity.y = y;
            ((struct ObjNet *) sDynListCurObj)->centerOfGravity.z = z;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dCofG()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_shape_offset(f32 x, f32 y, f32 z) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            ((struct ObjJoint *) sDynListCurObj)->shapeOffset.x = x;
            ((struct ObjJoint *) sDynListCurObj)->shapeOffset.y = y;
            ((struct ObjJoint *) sDynListCurObj)->shapeOffset.z = z;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dShapeOffset()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_add_valptr(DynObjName name, u32 vflags, enum ValPtrType type, size_t offset) {
    struct GdObj *dynobj;
    struct ObjValPtr *valptr;
    struct DynObjInfo *info;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    dynobj = sDynListCurObj;

    if (vflags == 0x40000) {

        info = get_dynobj_info(name);
        if (info == NULL) {
            fatal_printf("dAddValPtr(\"%s\"): Undefined object", DynNameAsStr(name));
        }

        valptr = make_valptr(info->obj, vflags, type, offset);
    } else {

        valptr = make_valptr(name, vflags, type, offset);
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_GADGETS:
            if (((struct ObjGadget *) dynobj)->valueGrp == NULL) {
                ((struct ObjGadget *) dynobj)->valueGrp = make_group(0);
            }
            addto_group(((struct ObjGadget *) dynobj)->valueGrp, &valptr->header);
            break;
        case OBJ_TYPE_LABELS:
            ((struct ObjLabel *) dynobj)->valptr = valptr;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dAddValPtr()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_add_valproc(valptrproc_t proc) {
    struct GdObj *dynobj;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    dynobj = sDynListCurObj;
    switch (sDynListCurObj->type) {
        case OBJ_TYPE_LABELS:
            ((struct ObjLabel *) dynobj)->valfn = proc;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dAddValProc()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_link_with_ptr(void *ptr) {
    struct GdObj *dynobj;
    struct ObjValPtr *valptr;
    struct ListNode *link;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    dynobj = sDynListCurObj;
    imin("dLinkWithPtr");
    switch (sDynListCurObj->type) {
        case OBJ_TYPE_CAMERAS:
            ((struct ObjCamera *) dynobj)->unk30 = ptr;
            break;
        case OBJ_TYPE_GROUPS:
            link = make_link_to_obj(NULL, ptr);
            ((struct ObjGroup *) dynobj)->firstMember = link;
            break;
        case OBJ_TYPE_BONES:
            add_joint2bone((struct ObjBone *) dynobj, ptr);
            break;
        case OBJ_TYPE_VIEWS:
            ((struct ObjView *) dynobj)->components = ptr;
            ((struct ObjView *) dynobj)->unk1C =
                setup_view_buffers(((struct ObjView *) dynobj)->namePtr, ((struct ObjView *) dynobj),
                                   (s32)((struct ObjView *) dynobj)->upperLeft.x,
                                   (s32)((struct ObjView *) dynobj)->upperLeft.y,
                                   (s32)((struct ObjView *) dynobj)->lowerRight.x,
                                   (s32)((struct ObjView *) dynobj)->lowerRight.y);
            reset_nets_and_gadgets(((struct ObjView *) dynobj)->components);
            break;
        case OBJ_TYPE_FACES:
            if (((struct ObjFace *) dynobj)->vtxCount >= 4) {
                fatal_printf("too many points");
            }

            ((struct ObjFace *) dynobj)->vertices[((struct ObjFace *) dynobj)->vtxCount] = ptr;
            ((struct ObjFace *) dynobj)->vtxCount++;

            if (((struct ObjFace *) dynobj)->vtxCount >= 3) {
                calc_face_normal((struct ObjFace *) dynobj);
            }

            break;
        case OBJ_TYPE_ANIMATORS:
            if (((struct ObjAnimator *) dynobj)->animatedPartsGrp == NULL) {
                ((struct ObjAnimator *) dynobj)->animatedPartsGrp = make_group(0);
            }

            addto_group(((struct ObjAnimator *) dynobj)->animatedPartsGrp, ptr);
            break;
        case OBJ_TYPE_LABELS:
            valptr = make_valptr(ptr, OBJ_TYPE_ALL, 0, 0);
            ((struct ObjLabel *) dynobj)->valptr = valptr;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dLinkWithPtr()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
    imout();
}

void d_link_with(DynObjName name) {
    struct DynObjInfo *info;
    struct DynObjInfo *origInfo = sDynListCurInfo;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    if (name == NULL) {
        return;
    }

    info = get_dynobj_info(name);
    if (info == NULL) {
        fatal_printf("dLinkWith(\"%s\"): Undefined object", DynNameAsStr(name));
    }

    d_link_with_ptr(info->obj);
    set_cur_dynobj(origInfo->obj);
    sDynListCurInfo = origInfo;
}

void d_set_flags(s32 flags) {
    struct GdObj *dynobj;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    dynobj = sDynListCurObj;
    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            ((struct ObjJoint *) dynobj)->flags |= flags;
            break;
        case OBJ_TYPE_BONES:
            ((struct ObjBone *) dynobj)->unk104 |= flags;
            break;
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) dynobj)->flags |= flags;
            break;
        case OBJ_TYPE_CAMERAS:
            ((struct ObjCamera *) dynobj)->flags |= flags;
            break;
        case OBJ_TYPE_VIEWS:
            ((struct ObjView *) dynobj)->flags |= flags;
            break;
        case OBJ_TYPE_SHAPES:
            ((struct ObjShape *) dynobj)->flag |= flags;
            break;
        case OBJ_TYPE_PARTICLES:
            ((struct ObjParticle *) dynobj)->flags |= flags;
            break;
        case OBJ_TYPE_LIGHTS:
            ((struct ObjLight *) dynobj)->flags |= flags;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetFlags()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_clear_flags(s32 flags) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            ((struct ObjJoint *) sDynListCurObj)->flags &= ~flags;
            break;
        case OBJ_TYPE_BONES:
            ((struct ObjBone *) sDynListCurObj)->unk104 &= ~flags;
            break;
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) sDynListCurObj)->flags &= ~flags;
            break;
        case OBJ_TYPE_CAMERAS:
            ((struct ObjCamera *) sDynListCurObj)->flags &= ~flags;
            break;
        case OBJ_TYPE_PARTICLES:
            ((struct ObjParticle *) sDynListCurObj)->flags &= ~flags;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dClrFlags()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_parm_f(enum DParmF param, f32 val) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_SHAPES:
            switch (param) {
                case PARM_F_ALPHA:
                    ((struct ObjShape *) sDynListCurObj)->alpha = val;
                    break;
                default:
                    fatal_printf("%s: Object '%s'(%x) does not support this function.",
                                 "dSetParmf() - unsupported parm.", sDynListCurInfo->name,
                                 sDynListCurObj->type);
            }
            break;
        case OBJ_TYPE_GADGETS:
            switch (param) {
                case PARM_F_RANGE_MIN:
                    ((struct ObjGadget *) sDynListCurObj)->rangeMin = val;
                    break;
                case PARM_F_RANGE_MAX:
                    ((struct ObjGadget *) sDynListCurObj)->rangeMax = val;
                    break;
                case PARM_F_VARVAL:
                    ((struct ObjGadget *) sDynListCurObj)->varval.f = val;
                    break;
                default:
                    fatal_printf("%s: Object '%s'(%x) does not support this function.",
                                 "dSetParmf() - unsupported parm.", sDynListCurInfo->name,
                                 sDynListCurObj->type);
            }
            break;
        case OBJ_TYPE_VERTICES:
            switch (param) {
                case PARM_F_ALPHA:
                    ((struct ObjVertex *) sDynListCurObj)->alpha = val;
                    break;
                default:
                    fatal_printf("%s: Object '%s'(%x) does not support this function.",
                                 "dSetParmf() - unsupported parm.", sDynListCurInfo->name,
                                 sDynListCurObj->type);
            }
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetParmf()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_parm_ptr(enum DParmPtr param, void *ptr) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_LABELS:
            switch (param) {
                case PARM_PTR_CHAR:
                    ((struct ObjLabel *) sDynListCurObj)->fmtstr = ptr;
                    break;
                default:
                    fatal_printf("Bad parm");
            }
            break;
        case OBJ_TYPE_VIEWS:
            switch (param) {
                case PARM_PTR_CHAR:
                    ((struct ObjView *) sDynListCurObj)->namePtr = ptr;
                    break;
                default:
                    fatal_printf("Bad parm");
            }
            break;
        case OBJ_TYPE_FACES:
            switch (param) {
                case PARM_PTR_OBJ_VTX:

                    if (((struct ObjFace *) sDynListCurObj)->vtxCount >= 4) {
                        fatal_printf("dsetparmp() too many points");
                    }

                    ((struct ObjFace *) sDynListCurObj)->vertices[((struct ObjFace *) sDynListCurObj)->vtxCount++] = ptr;
                    break;
                default:
                    fatal_printf("Bad parm");
            }
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetParmp()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_obj_draw_flag(enum ObjDrawingFlags flag) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    sDynListCurObj->drawFlags |= flag;
}

void d_set_type(s32 type) {
    struct GdObj *dynobj = sDynListCurObj;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) dynobj)->netType = type;
            break;
        case OBJ_TYPE_GADGETS:
            ((struct ObjGadget *) dynobj)->type = type;
            break;
        case OBJ_TYPE_GROUPS:
            ((struct ObjGroup *) dynobj)->debugPrint = type;
            break;
        case OBJ_TYPE_JOINTS:
            ((struct ObjJoint *) dynobj)->type = type;
            break;
        case OBJ_TYPE_PARTICLES:
            ((struct ObjParticle *) dynobj)->unk60 = type;
            break;
        case OBJ_TYPE_MATERIALS:
            ((struct ObjMaterial *) dynobj)->type = type;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetType()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_id(s32 id) {
    struct GdObj *dynobj = sDynListCurObj;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_MATERIALS:
            ((struct ObjMaterial *) dynobj)->id = id;
            break;
        case OBJ_TYPE_JOINTS:
            ((struct ObjJoint *) dynobj)->id = id;
            break;
        case OBJ_TYPE_VERTICES:
            ((struct ObjVertex *) dynobj)->id = id;
            break;
        case OBJ_TYPE_LIGHTS:
            ((struct ObjLight *) dynobj)->id = id;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetID()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_colour_num(s32 colornum) {
    struct GdColour *rgbcolor;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            ((struct ObjJoint *) sDynListCurObj)->colourNum = colornum;
            break;
        case OBJ_TYPE_PARTICLES:
            ((struct ObjParticle *) sDynListCurObj)->colourNum = colornum;
            break;
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) sDynListCurObj)->colourNum = colornum;
            break;
        case OBJ_TYPE_GADGETS:
            ((struct ObjGadget *) sDynListCurObj)->colourNum = colornum;
            break;
        case OBJ_TYPE_FACES:
            rgbcolor = gd_get_colour(colornum);
            if (rgbcolor != NULL) {
                ((struct ObjFace *) sDynListCurObj)->colour.r = rgbcolor->r;
                ((struct ObjFace *) sDynListCurObj)->colour.g = rgbcolor->g;
                ((struct ObjFace *) sDynListCurObj)->colour.b = rgbcolor->b;
                ((struct ObjFace *) sDynListCurObj)->colourNum = colornum;
            } else {
                fatal_printf("dSetColNum: Unkown colour number");
            }
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dColourNum()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_material(UNUSED void *a0, s32 mtlId) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_FACES:
            ((struct ObjFace *) sDynListCurObj)->mtlId = mtlId;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetMaterial()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_friction(f32 x, f32 y, f32 z) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            ((struct ObjJoint *) sDynListCurObj)->friction.x = x;
            ((struct ObjJoint *) sDynListCurObj)->friction.y = y;
            ((struct ObjJoint *) sDynListCurObj)->friction.z = z;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dFriction()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_spring(f32 spring) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_BONES:
            ((struct ObjBone *) sDynListCurObj)->spring = spring;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetSpring()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_ambient(f32 r, f32 g, f32 b) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_MATERIALS:
            ((struct ObjMaterial *) sDynListCurObj)->Ka.r = r;
            ((struct ObjMaterial *) sDynListCurObj)->Ka.g = g;
            ((struct ObjMaterial *) sDynListCurObj)->Ka.b = b;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetAmbient()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_diffuse(f32 r, f32 g, f32 b) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_MATERIALS:
            ((struct ObjMaterial *) sDynListCurObj)->Kd.r = r;
            ((struct ObjMaterial *) sDynListCurObj)->Kd.g = g;
            ((struct ObjMaterial *) sDynListCurObj)->Kd.b = b;
            break;
        case OBJ_TYPE_LIGHTS:
            ((struct ObjLight *) sDynListCurObj)->diffuse.r = r;
            ((struct ObjLight *) sDynListCurObj)->diffuse.g = g;
            ((struct ObjLight *) sDynListCurObj)->diffuse.b = b;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetDiffuse()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_control_type(s32 ctrltype) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_NETS:
            ((struct ObjNet *) sDynListCurObj)->ctrlType = ctrltype;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dControlType()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

struct GdBoundingBox *d_get_bounding_box(void) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_NETS:
            return &((struct ObjNet *) sDynListCurObj)->boundingBox;
            break;
        case OBJ_TYPE_PLANES:
            return &((struct ObjPlane *) sDynListCurObj)->boundingBox;
            break;
        case OBJ_TYPE_ZONES:
            return &((struct ObjZone *) sDynListCurObj)->boundingBox;
            break;
        default:
            return &sNullBoundingBox;
    }
}

void d_get_matrix(Mat4f *dst) {
    struct GdObj *dynobj;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    dynobj = sDynListCurObj;
    switch (sDynListCurObj->type) {
        case OBJ_TYPE_NETS:
            gd_copy_mat4f(&((struct ObjNet *) dynobj)->mat128, dst);
            break;
            break;
        case OBJ_TYPE_JOINTS:
            gd_copy_mat4f(&((struct ObjJoint *) dynobj)->matE8, dst);
            break;
        case OBJ_TYPE_CAMERAS:
            gd_copy_mat4f(&((struct ObjCamera *) dynobj)->unkE8, dst);
            break;
        case OBJ_TYPE_PARTICLES:
            gd_set_identity_mat4(dst);
            break;
        case OBJ_TYPE_SHAPES:
            gd_set_identity_mat4(dst);
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dGetMatrix()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_matrix(Mat4f *src) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_NETS:
            gd_copy_mat4f(src, &((struct ObjNet *) sDynListCurObj)->mat128);

            gd_copy_mat4f(src, &((struct ObjNet *) sDynListCurObj)->mat128);
            break;
        case OBJ_TYPE_JOINTS:
            gd_copy_mat4f(src, &((struct ObjJoint *) sDynListCurObj)->matE8);
            break;
        case OBJ_TYPE_CAMERAS:
            gd_copy_mat4f(src, &((struct ObjCamera *) sDynListCurObj)->unk64);
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetMatrix()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

void d_set_rot_mtx(Mat4f *src) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            gd_copy_mat4f(src, &((struct ObjJoint *) sDynListCurObj)->mat128);
            break;
        case OBJ_TYPE_NETS:
            gd_copy_mat4f(src, &((struct ObjNet *) sDynListCurObj)->mat168);
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetRMatrix()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

Mat4f *d_get_rot_mtx_ptr(void) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            return &((struct ObjJoint *) sDynListCurObj)->mat128;
        case OBJ_TYPE_NETS:
            return &((struct ObjNet *) sDynListCurObj)->mat168;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dGetRMatrixPtr()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }

}

void d_set_i_matrix(Mat4f *src) {
    struct GdObj *dynobj;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    dynobj = sDynListCurObj;
    switch (sDynListCurObj->type) {
        case OBJ_TYPE_NETS:
            gd_copy_mat4f(src, &((struct ObjNet *) dynobj)->matE8);
            break;
        case OBJ_TYPE_JOINTS:
            gd_copy_mat4f(src, &((struct ObjJoint *) dynobj)->mat168);
            break;
        case OBJ_TYPE_LIGHTS:
            ((struct ObjLight *) dynobj)->position.x = (*src)[3][0];
            ((struct ObjLight *) dynobj)->position.y = (*src)[3][1];
            ((struct ObjLight *) dynobj)->position.z = (*src)[3][2];
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetIMatrix()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}

Mat4f *d_get_matrix_ptr(void) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_NETS:
            return &((struct ObjNet *) sDynListCurObj)->mat128;
            break;
        case OBJ_TYPE_CAMERAS:
            return &((struct ObjCamera *) sDynListCurObj)->unk64;
            break;
        case OBJ_TYPE_BONES:
            return &((struct ObjBone *) sDynListCurObj)->mat70;
            break;
        case OBJ_TYPE_JOINTS:
            return &((struct ObjJoint *) sDynListCurObj)->matE8;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dGetMatrixPtr()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }

}

Mat4f *d_get_i_mtx_ptr(void) {
    struct GdObj *dynobj;

    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    dynobj = sDynListCurObj;
    switch (sDynListCurObj->type) {
        case OBJ_TYPE_NETS:
            return &((struct ObjNet *) dynobj)->matE8;
            break;
        case OBJ_TYPE_JOINTS:
            return &((struct ObjJoint *) dynobj)->mat168;
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dGetIMatrixPtr()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }

}

f32 d_calc_world_dist_btwn(struct GdObj *obj1, struct GdObj *obj2) {
    struct GdVec3f obj1pos;
    struct GdVec3f obj2pos;
    struct GdVec3f posdiff;

    set_cur_dynobj(obj1);
    d_get_world_pos(&obj1pos);
    set_cur_dynobj(obj2);
    d_get_world_pos(&obj2pos);

    posdiff.x = obj2pos.x - obj1pos.x;
    posdiff.y = obj2pos.y - obj1pos.y;
    posdiff.z = obj2pos.z - obj1pos.z;

    return gd_vec3f_magnitude(&posdiff);
}

void d_set_skin_weight(s32 vtxId, f32 percentWeight) {
    if (sDynListCurObj == NULL) {
        fatal_printf("proc_dynlist(): No current object");
    }

    switch (sDynListCurObj->type) {
        case OBJ_TYPE_JOINTS:
            set_skin_weight((struct ObjJoint *) sDynListCurObj, vtxId, NULL,
                            percentWeight / 100.0);
            break;
        default:
            fatal_printf("%s: Object '%s'(%x) does not support this function.", "dSetSkinWeight()",
                         sDynListCurInfo->name, sDynListCurObj->type);
    }
}
