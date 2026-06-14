#include <PR/ultratypes.h>
#include <stdio.h>

#include "debug_utils.h"
#include "dynlist_proc.h"
#include "gd_types.h"
#include "macros.h"
#include "objects.h"
#include "old_menu.h"
#include "renderer.h"

static char sDefSettingsMenuStr[0x100];
static struct GdVec3f sStaticVec;
UNUSED static struct GdVec3f unusedVec;
static struct ObjGadget *sCurGadgetPtr;

static void reset_gadget_default(struct ObjGadget *);

void get_objvalue(union ObjVarVal *dst, enum ValPtrType type, void *base, size_t offset) {
    union ObjVarVal *objAddr = (void *) ((u8 *) base + offset);

    switch (type) {
        case OBJ_VALUE_INT:
            dst->i = objAddr->i;
            break;
        case OBJ_VALUE_FLOAT:
            dst->f = objAddr->f;
            break;
        default:
            fatal_printf("%s: Undefined ValueType", "get_objvalue");
    }
}

void Unknown8018B7A8(void *a0) {
    struct GdVec3f sp1C;

    set_cur_dynobj(a0);
    d_get_init_pos(&sp1C);

    sp1C.x += sStaticVec.x;
    sp1C.y += sStaticVec.y;
    sp1C.z += sStaticVec.z;
    d_set_world_pos(sp1C.x, sp1C.y, sp1C.z);
}

static void menu_cb_default_settings(intptr_t itemId) {
    struct ObjGroup *group = (struct ObjGroup *)itemId;
    apply_to_obj_types_in_group(OBJ_TYPE_GADGETS, (applyproc_t) reset_gadget_default, group);
    apply_to_obj_types_in_group(OBJ_TYPE_VIEWS, (applyproc_t) stub_renderer_6, gGdViewsGroup);
}

static void add_item_to_default_settings_menu(struct ObjGroup *group) {
    char buf[0x100];

    if (group->debugPrint == 1) {

        sprintf(buf, "| %s %%x%d", group->name, (u32) (intptr_t) group);
        gd_strcat(sDefSettingsMenuStr, buf);
    }
}

long create_gui_menu(struct ObjGroup *grp) {
    long dynamicsMenuId;
    long defaultSettingMenuId;
    long contTypeMenuId;

    gd_strcpy(sDefSettingsMenuStr, "Default Settings %t %F");
    apply_to_obj_types_in_group(OBJ_TYPE_GROUPS, (applyproc_t) add_item_to_default_settings_menu, grp);
    defaultSettingMenuId = defpup(sDefSettingsMenuStr, &menu_cb_default_settings);

    contTypeMenuId = defpup(
        "Control Type %t %F"
        "| U-64 Analogue Joystick %x1 "
        "| Keyboard %x2 "
        "| Mouse %x3",
        &menu_cb_control_type);

    dynamicsMenuId = defpup(
        "Dynamics %t "
        "|\t\t\tReset Positions %f "
        "|\t\t\tSet Defaults %m "
        "|\t\t\tSet Controller %m "
        "|\t\t\tRe-Calibrate Controller %f "
        "|\t\t\tQuit %f",
        &menu_cb_reset_positions, defaultSettingMenuId, contTypeMenuId, &menu_cb_recalibrate_controller, &gd_exit);

    return dynamicsMenuId;
}

struct ObjLabel *make_label(struct ObjValPtr *ptr, char *str, s32 a2, f32 x, f32 y, f32 z) {
    struct ObjLabel *label = (struct ObjLabel *) make_object(OBJ_TYPE_LABELS);
    label->valfn = NULL;
    label->valptr = ptr;
    label->fmtstr = str;
    label->unk24 = a2;
    label->unk30 = 4;
    label->position.x = x;
    label->position.y = y;
    label->position.z = z;

    return label;
}

struct ObjGadget *make_gadget(UNUSED s32 a0, s32 a1) {
    struct ObjGadget *gdgt = (struct ObjGadget *) make_object(OBJ_TYPE_GADGETS);
    gdgt->valueGrp = NULL;
    gdgt->rangeMax = 1.0f;
    gdgt->rangeMin = 0.0f;
    gdgt->unk20 = a1;
    gdgt->colourNum = 0;
    gdgt->sliderPos = 1.0f;
    gdgt->size.x = 100.0f;
    gdgt->size.y = 10.0f;
    gdgt->size.z = 10.0f;

    return gdgt;
}

void set_objvalue(union ObjVarVal *src, enum ValPtrType type, void *base, size_t offset) {
    union ObjVarVal *dst = (void *) ((u8 *) base + offset);
    switch (type) {
        case OBJ_VALUE_INT:
            dst->i = src->i;
            break;
        case OBJ_VALUE_FLOAT:
            dst->f = src->f;
            break;
        default:
            fatal_printf("%s: Undefined ValueType", "set_objvalue");
    }
}

void set_static_gdgt_value(struct ObjValPtr *vp) {
    switch (vp->datatype) {
        case OBJ_VALUE_FLOAT:
            set_objvalue(&sCurGadgetPtr->varval, OBJ_VALUE_FLOAT, vp->obj, vp->offset);
            break;
        case OBJ_VALUE_INT:
            set_objvalue(&sCurGadgetPtr->varval, OBJ_VALUE_INT, vp->obj, vp->offset);
            break;
    }
}

static void reset_gadget_default(struct ObjGadget *gdgt) {
    UNUSED u8 filler[4];

    sCurGadgetPtr = gdgt;
    apply_to_obj_types_in_group(OBJ_TYPE_VALPTRS, (applyproc_t) set_static_gdgt_value, gdgt->valueGrp);
}

void adjust_gadget(struct ObjGadget *gdgt, s32 a1, s32 a2) {
    UNUSED u8 filler[8];
    f32 range;
    struct ObjValPtr *vp;

    if (gdgt->type == 1) {
        gdgt->sliderPos += a2 * (-sCurrentMoveCamera->unk40.z * 1.0E-5);
    } else if (gdgt->type == 2) {
        gdgt->sliderPos += a1 * (-sCurrentMoveCamera->unk40.z * 1.0E-5);
    }

    if (gdgt->sliderPos < 0.0f) {
        gdgt->sliderPos = 0.0f;
    } else if (gdgt->sliderPos > 1.0f) {
        gdgt->sliderPos = 1.0f;
    }

    range = gdgt->rangeMax - gdgt->rangeMin;

    if (gdgt->valueGrp != NULL) {
        vp = (struct ObjValPtr *) gdgt->valueGrp->firstMember->obj;

        switch (vp->datatype) {
            case OBJ_VALUE_FLOAT:
                gdgt->varval.f = gdgt->sliderPos * range + gdgt->rangeMin;
                break;
            case OBJ_VALUE_INT:
                gdgt->varval.i = ((s32)(gdgt->sliderPos * range)) + gdgt->rangeMin;
                break;
            default:
                fatal_printf("%s: Undefined ValueType", "adjust_gadget");
        }
    }

    reset_gadget_default(gdgt);
}

void reset_gadget(struct ObjGadget *gdgt) {
    UNUSED u8 filler[8];
    f32 range;
    struct ObjValPtr *vp;

    if (gdgt->rangeMax - gdgt->rangeMin == 0.0f) {
        fatal_printf("gadget has zero range (%f -> %f)\n", gdgt->rangeMin, gdgt->rangeMax);
    }

    range = (f32)(1.0 / (gdgt->rangeMax - gdgt->rangeMin));

    if (gdgt->valueGrp != NULL) {
        vp = (struct ObjValPtr *) gdgt->valueGrp->firstMember->obj;

        switch (vp->datatype) {
            case OBJ_VALUE_FLOAT:
                get_objvalue(&gdgt->varval, OBJ_VALUE_FLOAT, vp->obj, vp->offset);
                gdgt->sliderPos = (gdgt->varval.f - gdgt->rangeMin) * range;
                break;
            case OBJ_VALUE_INT:
                get_objvalue(&gdgt->varval, OBJ_VALUE_INT, vp->obj, vp->offset);
                gdgt->sliderPos = (gdgt->varval.i - gdgt->rangeMin) * range;
                break;
            default:
                fatal_printf("%s: Undefined ValueType", "reset_gadget");
        }
    }
}

void reset_gadgets_in_grp(struct ObjGroup *grp) {
    apply_to_obj_types_in_group(OBJ_TYPE_GADGETS, (applyproc_t) reset_gadget, grp);
}
