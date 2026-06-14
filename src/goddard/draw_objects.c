#include <PR/ultratypes.h>
#include <stdio.h>

#if defined(VERSION_JP) || defined(VERSION_US)
#include "prevent_bss_reordering.h"
#endif

#include "debug_utils.h"
#include "dynlist_proc.h"
#include "gd_macros.h"
#include "gd_main.h"
#include "gd_math.h"
#include "gd_types.h"
#include "macros.h"
#include "objects.h"
#include "old_menu.h"
#include "renderer.h"
#include "shape_helper.h"
#include "draw_objects.h"

void func_80179B64(struct ObjGroup *);
void update_shaders(struct ObjShape *, struct GdVec3f *);
void draw_shape_faces(struct ObjShape *);
void register_light(struct ObjLight *);

enum SceneType {
    RENDER_SCENE = 26,
    FIND_PICKS = 27
};

struct BetaVtx {
     u8 filler[68];
     f32 s;
     f32 t;
};

static struct GdColour sClrWhite = { 1.0, 1.0, 1.0 };
static struct GdColour sClrRed = { 1.0, 0.0, 0.0 };
static struct GdColour sClrGreen = { 0.0, 1.0, 0.0 };
static struct GdColour sClrBlue = { 0.0, 0.0, 1.0 };
static struct GdColour sClrErrDarkBlue = { 0.0, 0.0, 6.0 };
static struct GdColour sClrPink = { 1.0, 0.0, 1.0 };
static struct GdColour sClrBlack = { 0.0, 0.0, 0.0 };
static struct GdColour sClrGrey = { 0.6, 0.6, 0.6 };
static struct GdColour sClrDarkGrey = { 0.4, 0.4, 0.4 };
static struct GdColour sClrYellow = { 1.0, 1.0, 0.0 };
static struct GdColour sLightColours[1] = { { 1.0, 1.0, 0.0 } };
static struct GdColour *sSelectedColour = &sClrRed;
struct ObjCamera *gViewUpdateCamera = NULL;
UNUSED static void *sUnref801A80FC = NULL;
static s32 sUnreadShapeFlag = 0;
struct GdColour *sColourPalette[5] = {
    &sClrWhite, &sClrYellow, &sClrRed, &sClrBlack, &sClrBlack
};
struct GdColour *sWhiteBlack[2] = {

    &sClrWhite,
    &sClrBlack,
};
UNUSED static Mat4f sUnref801A8120 = {
    { 1.0, 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0, 0.0 }, { 0.0, 0.0, 1.0, 0.0 }, { 0.0, 0.0, 0.0, 1.0 }
};
UNUSED static Mat4f sUnrefIden801A8160 = {
    { 1.0, 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0, 0.0 }, { 0.0, 0.0, 1.0, 0.0 }, { 0.0, 0.0, 0.0, 1.0 }
};
static s32 sLightDlCounter = 1;
UNUSED static s32 sUnref801A81A4[4] = { 0 };

u8 gUnref_801B9B30[0x88];
struct ObjGroup *gGdLightGroup;

UNUSED static u8 sUnref_801B9BBC[0x40];
static enum SceneType sSceneProcessType;
static s32 sUseSelectedColor;
static s16 sPickBuffer[100];
static s32 sPickDataTemp;
static f32 sPickObjDistance;
static struct GdObj *sPickedObject;

static struct {
    u8 filler1[4];
    struct ObjView *view;
    s32 unreadCounter;
    s32 mtlDlNum;
    s32 shapesDrawn;
    s32 unused;
} sUpdateViewState;
static struct ObjLight *sPhongLight;
static struct GdVec3f sPhongLightPosition;

static struct GdVec3f sLightPositionOffset;
static struct GdVec3f sLightPositionCache[8];
static s32 sNumActiveLights;
static struct GdVec3f sGrabCords;

void setup_lights(void) {
    set_light_num(NUMLIGHTS_2);
    gd_setproperty(GD_PROP_AMB_COLOUR, 0.5f, 0.5f, 0.5f);
    gd_setproperty(GD_PROP_CULLING, 1.0f, 0.0f, 0.0f);
    return;

    gd_setproperty(GD_PROP_STUB17, 2.0f, 0.0f, 0.0f);
    gd_setproperty(GD_PROP_ZBUF_FN, 24.0f, 0.0f, 0.0f);
    gd_setproperty(GD_PROP_CULLING, 1.0f, 0.0f, 0.0f);
    return;
}

void Unknown801781DC(struct ObjZone *zone) {
    struct GdVec3f lightPos;
    struct ObjUnk200000 *unk;
    f32 sp34;
    f32 sp30;
    f32 sp2C;
    struct ObjLight *light;
    register struct ListNode *link = zone->unk30->firstMember;
    struct GdObj *obj;

    while (link != NULL) {
        obj = link->obj;
        light = (struct ObjLight *) gGdLightGroup->firstMember->obj;
        lightPos.x = light->position.x;
        lightPos.y = light->position.y;
        lightPos.z = light->position.z;
        unk = (struct ObjUnk200000 *) obj;
        sp34 = gd_dot_vec3f(&unk->unk34->normal, &unk->unk30->pos);
        sp30 = gd_dot_vec3f(&unk->unk34->normal, &lightPos);
        lightPos.x -= unk->unk34->normal.x * (sp30 - sp34);
        lightPos.y -= unk->unk34->normal.y * (sp30 - sp34);
        lightPos.z -= unk->unk34->normal.z * (sp30 - sp34);
        unk->unk30->pos.x = lightPos.x;
        unk->unk30->pos.y = lightPos.y;
        unk->unk30->pos.z = lightPos.z;
        sp2C = ABS((sp30 - sp34));
        if (sp2C > 600.0f) {
            sp2C = 600.0f;
        }
        sp2C = 1.0 - sp2C / 600.0;
        unk->unk30->normal.x = sp2C * light->colour.r;
        unk->unk30->normal.y = sp2C * light->colour.g;
        unk->unk30->normal.z = sp2C * light->colour.b;
        link = link->next;
    }
}

void draw_shape(struct ObjShape *shape, s32 flag, f32 c, f32 d, f32 e,
                f32 f, f32 g, f32 h,
                f32 i, f32 j, f32 k,
                f32 l, f32 m, f32 n,
                s32 colorIdx, Mat4f *rotMtx) {
    UNUSED u8 filler[8];
    struct GdVec3f sp1C;

    restart_timer("drawshape");
    sUpdateViewState.shapesDrawn++;

    if (shape == NULL) {
        return;
    }

    sp1C.x = sp1C.y = sp1C.z = 0.0f;
    if (flag & 2) {
        gd_dl_load_trans_matrix(f, g, h);
        sp1C.x += f;
        sp1C.y += g;
        sp1C.z += h;
    }

    if ((flag & 0x10) && rotMtx != NULL) {
        gd_dl_load_matrix(rotMtx);
        sp1C.x += (*rotMtx)[3][0];
        sp1C.y += (*rotMtx)[3][1];
        sp1C.z += (*rotMtx)[3][2];
    }

    if (flag & 8) {
        if (m != 0.0f) {
            func_8019F2C4(m, 121);
        }
        if (l != 0.0f) {
            func_8019F2C4(l, 120);
        }
        if (n != 0.0f) {
            func_8019F2C4(n, 122);
        }
    }

    if (colorIdx != 0) {
        sUseSelectedColor = TRUE;
        sSelectedColour = gd_get_colour(colorIdx);
        if (sSelectedColour != NULL) {
            gd_dl_material_lighting(-1, sSelectedColour, GD_MTL_LIGHTS);
        } else {
            fatal_print("Draw_shape(): Bad colour");
        }
    } else {
        sUseSelectedColor = FALSE;
        sSelectedColour = NULL;
    }

    if (sNumActiveLights != 0 && shape->mtlGroup != NULL) {
        if (rotMtx != NULL) {
            sp1C.x = (*rotMtx)[3][0];
            sp1C.y = (*rotMtx)[3][1];
            sp1C.z = (*rotMtx)[3][2];
        } else {
            sp1C.x = sp1C.y = sp1C.z = 0.0f;
        }
        update_shaders(shape, &sp1C);
    }

    if (flag & 4) {
        gd_dl_mul_trans_matrix(i, j, k);
    }

    if (flag & 1) {
        gd_dl_scale(c, d, e);
    }

    draw_shape_faces(shape);
    sUseSelectedColor = FALSE;
    split_timer("drawshape");
}

void draw_shape_2d(struct ObjShape *shape, s32 flag, UNUSED f32 c, UNUSED f32 d, UNUSED f32 e, f32 f,
                   f32 g, f32 h, UNUSED f32 i, UNUSED f32 j, UNUSED f32 k, UNUSED f32 l, UNUSED f32 m,
                   UNUSED f32 n, UNUSED s32 color, UNUSED s32 p) {
    UNUSED u8 filler[8];
    struct GdVec3f sp1C;

    restart_timer("drawshape2d");
    sUpdateViewState.shapesDrawn++;

    if (shape == NULL) {
        return;
    }

    if (flag & 2) {
        sp1C.x = f;
        sp1C.y = g;
        sp1C.z = h;
        if (gViewUpdateCamera != NULL) {
            gd_rotate_and_translate_vec3f(&sp1C, &gViewUpdateCamera->unkE8);
        }
        gd_dl_load_trans_matrix(sp1C.x, sp1C.y, sp1C.z);
    }
    draw_shape_faces(shape);
    split_timer("drawshape2d");
}

void draw_light(struct ObjLight *light) {
    struct GdVec3f sp94;
    Mat4f sp54;
    UNUSED Mat4f *uMatPtr;
    UNUSED f32 uMultiplier;
    struct ObjShape *shape;

    if (sSceneProcessType == FIND_PICKS) {
        return;
    }

    sLightColours[0].r = light->colour.r;
    sLightColours[0].g = light->colour.g;
    sLightColours[0].b = light->colour.b;

    if (light->flags & LIGHT_UNK02) {
        gd_set_identity_mat4(&sp54);
        sp94.x = -light->unk80.x;
        sp94.y = -light->unk80.y;
        sp94.z = -light->unk80.z;
        gd_create_origin_lookat(&sp54, &sp94, 0.0f);
        uMultiplier = light->unk38 / 45.0;
        shape = gSpotShape;
        uMatPtr = &sp54;
    } else {
        uMultiplier = 1.0f;
        shape = light->unk9C;
        uMatPtr = NULL;
        if (++sLightDlCounter >= 17) {
            sLightDlCounter = 1;
        }
        shape->unk50 = sLightDlCounter;
    }

    draw_shape_2d(shape, 2, 1.0f, 1.0f, 1.0f, light->position.x, light->position.y, light->position.z,
                  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1, 0);
}

void draw_material(struct ObjMaterial *mtl) {
    s32 mtlType = mtl->type;

    if (mtlType == GD_MTL_SHINE_DL) {
        if (sPhongLight != NULL && sPhongLight->unk30 > 0.0f) {
            if (gViewUpdateCamera != NULL) {
                gd_dl_hilite(mtl->gddlNumber, gViewUpdateCamera, &sPhongLight->position,
                              &sLightPositionOffset, &sPhongLightPosition, &sPhongLight->colour);
            } else {
                fatal_printf("draw_material() no active camera for phong");
            }
        } else {
            mtlType = GD_MTL_BREAK;
        }
    }
    if (sUseSelectedColor == FALSE) {
        gd_dl_material_lighting(mtl->gddlNumber, &mtl->Kd, mtlType);
    } else {
        gd_dl_material_lighting(mtl->gddlNumber, sSelectedColour, GD_MTL_LIGHTS);
    }
}

void create_mtl_gddl_if_empty(struct ObjMaterial *mtl) {
    if (mtl->gddlNumber == 0) {
        mtl->gddlNumber = create_mtl_gddl(mtl->type);
    }
}

void check_face_bad_vtx(struct ObjFace *face) {
    s32 i;
    struct ObjVertex *vtx;

    for (i = 0; i < face->vtxCount; i++) {
        vtx = face->vertices[i];

        if ((uintptr_t) vtx == 39) {
            gd_printf("bad1\n");
            return;
        }
        if ((uintptr_t) vtx->gbiVerts == 0x3F800000) {
            fatal_printf("bad2 %x,%d,%d,%d\n", (u32) (uintptr_t) vtx, vtx->scaleFactor, vtx->id, vtx->header.type);
        }
    }
}

struct GdColour *gd_get_colour(s32 idx) {
    switch (idx) {
        case COLOUR_BLACK:
            return &sClrBlack;
            break;
        case COLOUR_WHITE:
            return &sClrWhite;
            break;
        case COLOUR_RED:
            return &sClrRed;
            break;
        case COLOUR_GREEN:
            return &sClrGreen;
            break;
        case COLOUR_BLUE:
            return &sClrBlue;
            break;
        case COLOUR_GRAY:
            return &sClrGrey;
            break;
        case COLOUR_DARK_GRAY:
            return &sClrDarkGrey;
            break;
        case COLOUR_DARK_BLUE:
            return &sClrErrDarkBlue;
            break;
        case COLOUR_BLACK2:
            return &sClrBlack;
            break;
        case COLOUR_YELLOW:
            return &sClrYellow;
            break;
        case COLOUR_PINK:
            return &sClrPink;
            break;
        case -1:
            return &sLightColours[0];
            break;
        default:
            return NULL;
    }
}

void Unknown80178ECC(f32 v0X, f32 v0Y, f32 v0Z, f32 v1X, f32 v1Y, f32 v1Z) {
    f32 difY = v1Y - v0Y;
    f32 difX = v1X - v0X;
    f32 difZ = v1Z - v0Z;

    gd_dl_make_triangle(v0X, v0Y, v0Z, v1X, v1Y, v1Z, v0X + difY * 0.1, v0Y + difX * 0.1, v0Z + difZ * 0.1);
}

void draw_face(struct ObjFace *face) {
    struct ObjVertex *vtx;
    f32 z;
    f32 y;
    f32 x;
    UNUSED u8 filler[12];
    s32 i;
    s32 hasTextCoords;
    Vtx *gbiVtx;

    imin("draw_face");
    hasTextCoords = FALSE;
    if (sUseSelectedColor == FALSE && face->mtlId >= 0) {
        if (face->mtl != NULL) {
            if ((i = face->mtl->gddlNumber) != 0) {
                if (i != sUpdateViewState.mtlDlNum) {
                    gd_dl_flush_vertices();
                    branch_to_gddl(i);
                    sUpdateViewState.mtlDlNum = i;
                }
            }
        }

        if (FALSE) {
        }
    }

    check_tri_display(face->vtxCount);

    if (!gGdUseVtxNormal) {
        set_Vtx_norm_buf_1(&face->normal);
    }

    for (i = 0; i < face->vtxCount; i++) {
        vtx = face->vertices[i];
        x = vtx->pos.x;
        y = vtx->pos.y;
        z = vtx->pos.z;
        if (gGdUseVtxNormal) {
            set_Vtx_norm_buf_2(&vtx->normal);
        }

        if (hasTextCoords) {
            set_vtx_tc_buf(((struct BetaVtx *) vtx)->s, ((struct BetaVtx *) vtx)->t);
        }

        gbiVtx = gd_dl_make_vertex(x, y, z, vtx->alpha);

        if (gbiVtx != NULL) {
            vtx->gbiVerts = make_vtx_link(vtx->gbiVerts, gbiVtx);
        }
    }
    func_8019FEF0();
    imout();
}

void draw_rect_fill(s32 color, f32 ulx, f32 uly, f32 lrx, f32 lry) {
    gd_dl_set_fill(gd_get_colour(color));
    gd_draw_rect(ulx, uly, lrx, lry);
}

void draw_rect_stroke(s32 color, f32 ulx, f32 uly, f32 lrx, f32 lry) {
    gd_dl_set_fill(gd_get_colour(color));
    gd_draw_border_rect(ulx, uly, lrx, lry);
}

void Unknown801792F0(struct GdObj *obj) {
    char objId[32];
    struct GdVec3f objPos;

    format_object_id(objId, obj);
    set_cur_dynobj(obj);
    d_get_world_pos(&objPos);
    func_801A4438(objPos.x, objPos.y, objPos.z);
    stub_draw_label_text(objId);
}

void draw_label(struct ObjLabel *label) {
    struct GdVec3f position;
    char strbuf[0x100];
    UNUSED u8 filler[16];
    struct ObjValPtr *valptr;
    union ObjVarVal varval;
    valptrproc_t valfn = label->valfn;

    if ((valptr = label->valptr) != NULL) {
        if (valptr->flag == 0x40000) {

            set_cur_dynobj(valptr->obj);
            d_get_world_pos(&position);
        } else {

            position.x = position.y = position.z = 0.0f;
        }

        switch (valptr->datatype) {
            case OBJ_VALUE_FLOAT:
                get_objvalue(&varval, OBJ_VALUE_FLOAT, valptr->obj, valptr->offset);
                if (valfn != NULL) {
                    valfn(&varval, varval);
                }
                sprintf(strbuf, label->fmtstr, varval.f);
                break;
            case OBJ_VALUE_INT:
                get_objvalue(&varval, OBJ_VALUE_INT, valptr->obj, valptr->offset);
                if (valfn != NULL) {
                    valfn(&varval, varval);
                }
                sprintf(strbuf, label->fmtstr, varval.i);
                break;
            default:
                if (label->fmtstr != NULL) {
                    gd_strcpy(strbuf, label->fmtstr);
                } else {
                    gd_strcpy(strbuf, "NONAME");
                }
                break;
        }
    } else {
        position.x = position.y = position.z = 0.0f;
        if (label->fmtstr != NULL) {
            gd_strcpy(strbuf, label->fmtstr);
        } else {
            gd_strcpy(strbuf, "NONAME");
        }
    }
    position.x += label->position.x;
    position.y += label->position.y;
    position.z += label->position.z;
    func_801A4438(position.x, position.y, position.z);
    stub_renderer_10(label->unk30);
    stub_draw_label_text(strbuf);
}

void draw_net(struct ObjNet *self) {
    struct ObjNet *net = self;
    s32 netColor;
    UNUSED u8 filler[80];

    if (sSceneProcessType == FIND_PICKS) {
        return;
    }

    if (net->header.drawFlags & OBJ_HIGHLIGHTED) {
        netColor = COLOUR_YELLOW;
    } else {
        netColor = net->colourNum;
    }

    if (net->shapePtr != NULL) {
        draw_shape(net->shapePtr, 0x10, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 0.0f, 0.0f, 0.0f, netColor, &net->mat168);
    }

    if (net->unk1C8 != NULL) {
        draw_group(net->unk1C8);
    }
}

void draw_gadget(struct ObjGadget *gdgt) {
    s32 colour = 0;

    if (gdgt->colourNum != 0) {
        colour = gdgt->colourNum;
    }

    draw_rect_fill(colour,
        gdgt->worldPos.x,
        gdgt->worldPos.y,
        gdgt->worldPos.x + gdgt->sliderPos * gdgt->size.x,
        gdgt->worldPos.y + gdgt->size.y);

    if (gdgt->header.drawFlags & OBJ_HIGHLIGHTED) {
        draw_rect_stroke(COLOUR_YELLOW,
            gdgt->worldPos.x,
            gdgt->worldPos.y,
            gdgt->worldPos.x + gdgt->sliderPos * gdgt->size.x,
            gdgt->worldPos.y + gdgt->size.y);
    }
    gdgt->header.drawFlags &= ~OBJ_HIGHLIGHTED;
}

void draw_camera(struct ObjCamera *cam) {
    struct GdVec3f sp44;
    UNUSED f32 sp40 = 0.0f;

    sp44.x = 0.0f;
    sp44.y = 0.0f;
    sp44.z = 0.0f;
    if (cam->unk30 != NULL) {
        set_cur_dynobj(cam->unk30);
        d_get_world_pos(&sp44);
        sp44.x += cam->lookAt.x;
        sp44.y += cam->lookAt.y;
        sp44.z += cam->lookAt.z;
        ;
    } else {
        sp44.x = cam->lookAt.x;
        sp44.y = cam->lookAt.y;
        sp44.z = cam->lookAt.z;
    }

    if (0) {

        gd_printf("%f,%f,%f\n", cam->worldPos.x, cam->worldPos.y, cam->worldPos.z);
    }

    if (ABS(cam->worldPos.x - sp44.x) + ABS(cam->worldPos.z - sp44.z) == 0.0f) {
        gd_printf("Draw_Camera(): Zero view distance\n");
        return;
    }
    gd_dl_lookat(cam, cam->worldPos.x, cam->worldPos.y, cam->worldPos.z, sp44.x, sp44.y, sp44.z, cam->unkA4);
}

void Unknown80179ACC(struct GdObj *obj) {
    switch (obj->type) {
        case OBJ_TYPE_NETS:
            if (((struct ObjNet *) obj)->unk1C8 != NULL) {
                func_80179B64(((struct ObjNet *) obj)->unk1C8);
            }
            break;
        default:
            break;
    }
    obj->drawFlags &= ~OBJ_DRAW_UNK01;
}

void func_80179B64(struct ObjGroup *group) {
    apply_to_obj_types_in_group(OBJ_TYPE_LABELS | OBJ_TYPE_GADGETS | OBJ_TYPE_CAMERAS | OBJ_TYPE_NETS
                                    | OBJ_TYPE_JOINTS | OBJ_TYPE_BONES,
                                (applyproc_t) Unknown80179ACC, group);
}

void world_pos_to_screen_coords(struct GdVec3f *pos, struct ObjCamera *cam, struct ObjView *view) {
    gd_rotate_and_translate_vec3f(pos, &cam->unkE8);
    if (pos->z > -256.0f) {
        return;
    }

    pos->x *= 256.0 / -pos->z;
    pos->y *= 256.0 / pos->z;
    pos->x += view->lowerRight.x / 2.0f;
    pos->y += view->lowerRight.y / 2.0f;
}

void check_grabbable_click(struct GdObj *input) {
    struct GdVec3f objPos;
    UNUSED u8 filler[12];
    struct GdObj *obj;
    Mat4f *mtx;

    if (gViewUpdateCamera == NULL) {
        return;
    }
    obj = input;
    if (!(obj->drawFlags & OBJ_IS_GRABBABLE)) {
        return;
    }

    set_cur_dynobj(obj);
    mtx = d_get_rot_mtx_ptr();
    objPos.x = (*mtx)[3][0];
    objPos.y = (*mtx)[3][1];
    objPos.z = (*mtx)[3][2];
    world_pos_to_screen_coords(&objPos, gViewUpdateCamera, sUpdateViewState.view);
    if (ABS(gGdCtrl.csrX - objPos.x) < 20.0f) {
        if (ABS(gGdCtrl.csrY - objPos.y) < 20.0f) {

            store_in_pickbuf(2);
            store_in_pickbuf(obj->type);
            store_in_pickbuf(obj->index);
            sGrabCords.x = objPos.x;
            sGrabCords.y = objPos.y;
        }
    }
}

void drawscene(enum SceneType process, struct ObjGroup *interactables, struct ObjGroup *lightgrp) {
    UNUSED u8 filler[16];

    restart_timer("drawscene");
    imin("draw_scene()");
    sUnreadShapeFlag = 0;
    sUpdateViewState.unreadCounter = 0;
    restart_timer("draw1");
    set_gd_mtx_parameters(G_MTX_PROJECTION | G_MTX_MUL | G_MTX_PUSH);
    if (sUpdateViewState.view->projectionType == 1) {
        gd_create_perspective_matrix(sUpdateViewState.view->clipping.z,
                      sUpdateViewState.view->lowerRight.x / sUpdateViewState.view->lowerRight.y,
                      sUpdateViewState.view->clipping.x, sUpdateViewState.view->clipping.y);
    } else {
        gd_create_ortho_matrix(
            -sUpdateViewState.view->lowerRight.x / 2.0, sUpdateViewState.view->lowerRight.x / 2.0,
            -sUpdateViewState.view->lowerRight.y / 2.0, sUpdateViewState.view->lowerRight.y / 2.0,
            sUpdateViewState.view->clipping.x, sUpdateViewState.view->clipping.y);
    }

    if (lightgrp != NULL) {
        set_gd_mtx_parameters(G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_PUSH);
        apply_to_obj_types_in_group(OBJ_TYPE_LIGHTS | OBJ_TYPE_PARTICLES,
                                    (applyproc_t) apply_obj_draw_fn, lightgrp);
        set_gd_mtx_parameters(G_MTX_PROJECTION | G_MTX_MUL | G_MTX_PUSH);
    }

    if (gViewUpdateCamera != NULL) {
        draw_camera(gViewUpdateCamera);
    } else {
        gd_dl_mul_trans_matrix(0.0f, 0.0f, -1000.0f);
    }

    setup_lights();
    set_gd_mtx_parameters(G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_PUSH);
    gd_dl_push_matrix();
    sSceneProcessType = process;

    if ((sNumActiveLights = sUpdateViewState.view->flags & VIEW_LIGHT)) {
        sUpdateViewState.view->flags &= ~VIEW_LIGHT;
    }

    sNumActiveLights = 1;
    apply_to_obj_types_in_group(OBJ_TYPE_LIGHTS, (applyproc_t) register_light, gGdLightGroup);
    split_timer("draw1");
    restart_timer("drawobj");
    imin("process_group");
    if (sSceneProcessType == FIND_PICKS) {
        apply_to_obj_types_in_group(OBJ_TYPE_ALL, (applyproc_t) check_grabbable_click, interactables);
    } else {
        apply_to_obj_types_in_group(OBJ_TYPE_LIGHTS | OBJ_TYPE_GADGETS | OBJ_TYPE_NETS
                                        | OBJ_TYPE_PARTICLES,
                                    (applyproc_t) apply_obj_draw_fn, interactables);
    }
    imout();
    split_timer("drawobj");
    gd_setproperty(GD_PROP_LIGHTING, 0.0f, 0.0f, 0.0f);
    apply_to_obj_types_in_group(OBJ_TYPE_LABELS, (applyproc_t) apply_obj_draw_fn, interactables);
    gd_setproperty(GD_PROP_LIGHTING, 1.0f, 0.0f, 0.0f);
    gd_dl_pop_matrix();
    imout();
    split_timer("drawscene");
    return;
}

void draw_nothing(UNUSED struct GdObj *nop) {
}

void draw_shape_faces(struct ObjShape *shape) {
    sUpdateViewState.mtlDlNum = 0;
    sUpdateViewState.unreadCounter = 0;
    gddl_is_loading_stub_dl(FALSE);
    sUnreadShapeFlag = (s32) shape->flag & 1;
    set_render_alpha(shape->alpha);
    if (shape->dlNums[gGdFrameBufNum] != 0) {
        draw_indexed_dl(shape->dlNums[gGdFrameBufNum], shape->unk50);
    } else if (shape->faceGroup != NULL) {
        func_801A0038();
        draw_group(shape->faceGroup);
        gd_dl_flush_vertices();
    }
}

void draw_particle(struct GdObj *obj) {
    struct ObjParticle *ptc = (struct ObjParticle *) obj;
    UNUSED u8 filler1[16];
    struct GdColour *white;
    struct GdColour *black;
    f32 brightness;
    UNUSED u8 filler2[16];

    if (ptc->timeout > 0) {
        white = sColourPalette[0];
        black = sWhiteBlack[1];
        brightness = ptc->timeout / 10.0;
        sLightColours[0].r = (white->r - black->r) * brightness + black->r;
        sLightColours[0].g = (white->g - black->g) * brightness + black->g;
        sLightColours[0].b = (white->b - black->b) * brightness + black->b;
        ;
    } else {
        sLightColours[0].r = 0.0f;
        sLightColours[0].g = 0.0f;
        sLightColours[0].b = 0.0f;
    }

    if (ptc->timeout > 0) {
        ptc->shapePtr->unk50 = ptc->timeout;
        draw_shape_2d(ptc->shapePtr, 2, 1.0f, 1.0f, 1.0f, ptc->pos.x, ptc->pos.y, ptc->pos.z, 0.0f,
                      0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1, 0);
    }
    if (ptc->unk60 == 3) {
        if (ptc->subParticlesGrp != NULL) {
            draw_group(ptc->subParticlesGrp);
        }
    }
}

void draw_bone(struct GdObj *obj) {
    struct ObjBone *bone = (struct ObjBone *) obj;
    UNUSED u8 filler1[4];
    s32 colour;
    UNUSED u8 filler2[4];
    struct GdVec3f scale;

    return;

    scale.x = 1.0f;
    scale.y = 1.0f;
    scale.z = bone->unkF8 / 50.0f;

    if (bone->header.drawFlags & OBJ_HIGHLIGHTED) {
        colour = COLOUR_YELLOW;
    } else {
        colour = bone->colourNum;
    }
    bone->header.drawFlags &= ~OBJ_HIGHLIGHTED;

    if (sSceneProcessType != FIND_PICKS) {
        draw_shape(bone->shapePtr, 0x1B, scale.x, scale.y, scale.z, bone->worldPos.x, bone->worldPos.y,
                   bone->worldPos.z, 0.0f, 0.0f, 0.0f, bone->unk28.x, bone->unk28.y, bone->unk28.z, colour,
                   &bone->mat70);
    }
}

void draw_joint(struct GdObj *obj) {
    struct ObjJoint *joint = (struct ObjJoint *) obj;
    UNUSED u8 filler1[4];
    UNUSED f32 sp7C = 70.0f;
    UNUSED u8 filler2[4];
    UNUSED s32 sp74 = 1;
    s32 colour;
    UNUSED u8 filler3[8];
    struct ObjShape *boneShape;
    UNUSED u8 filler4[28];

    if ((boneShape = joint->shapePtr) == NULL) {
        return;
    }

    if (joint->header.drawFlags & OBJ_HIGHLIGHTED) {
        colour = COLOUR_YELLOW;
    } else {
        colour = joint->colourNum;
    }

    draw_shape(boneShape, 0x10, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
               colour, &joint->mat128);
}

void draw_group(struct ObjGroup *grp) {
    if (grp == NULL) {
        fatal_print("Draw_Group: Bad group definition!");
    }

    apply_to_obj_types_in_group(OBJ_TYPE_ALL, (applyproc_t) apply_obj_draw_fn, grp);
}

void draw_plane(struct GdObj *obj) {
    struct ObjPlane *plane = (struct ObjPlane *) obj;

    if (obj->drawFlags & OBJ_HIGHLIGHTED) {
        obj->drawFlags &= ~OBJ_HIGHLIGHTED;
        ;
    } else {
        sUseSelectedColor = FALSE;
    }
    draw_face(plane->unk40);
}

void apply_obj_draw_fn(struct GdObj *obj) {
    if (obj == NULL) {
        fatal_print("Bad object!");
    }
    if (obj->drawFlags & OBJ_INVISIBLE) {
        return;
    }

    obj->objDrawFn(obj);
}

void register_light(struct ObjLight *light) {
    set_light_id(light->id);
    gd_setproperty(GD_PROP_LIGHTING, 2.0f, 0.0f, 0.0f);
    if (light->flags & LIGHT_NEW_UNCOUNTED) {
        sNumActiveLights++;
    }
    light->flags &= ~LIGHT_NEW_UNCOUNTED;
}

void Proc8017A980(struct ObjLight *light) {
    f32 sp24;
    f32 sp20;
    f32 sp1C;

    light->colour.r = light->diffuse.r * light->unk30;
    light->colour.g = light->diffuse.g * light->unk30;
    light->colour.b = light->diffuse.b * light->unk30;
    sLightPositionCache[light->id].x = light->position.x - sLightPositionOffset.x;
    sLightPositionCache[light->id].y = light->position.y - sLightPositionOffset.y;
    sLightPositionCache[light->id].z = light->position.z - sLightPositionOffset.z;
    gd_normalize_vec3f(&sLightPositionCache[light->id]);
    if (light->flags & LIGHT_UNK20) {
        sPhongLightPosition.x = sLightPositionCache[light->id].x;
        sPhongLightPosition.y = sLightPositionCache[light->id].y;
        sPhongLightPosition.z = sLightPositionCache[light->id].z;
        sPhongLight = light;
    }
    sp24 = light->unk30;
    if (light->flags & LIGHT_UNK02) {
        sp20 = -gd_dot_vec3f(&sLightPositionCache[light->id], &light->unk80);
        sp1C = 1.0 - light->unk38 / 90.0;
        if (sp20 > sp1C) {
            sp20 = (sp20 - sp1C) * (1.0 / (1.0 - sp1C));
            if (sp20 > 1.0) {
                sp20 = 1.0;
            } else if (sp20 < 0.0f) {
                sp20 = 0.0f;
            }
        } else {
            sp20 = 0.0f;
        }
        sp24 *= sp20;
    }
    set_light_id(light->id);
    gd_setproperty(GD_PROP_DIFUSE_COLOUR, light->diffuse.r * sp24, light->diffuse.g * sp24,
                   light->diffuse.b * sp24);
    gd_setproperty(GD_PROP_LIGHT_DIR, sLightPositionCache[light->id].x,
                   sLightPositionCache[light->id].y, sLightPositionCache[light->id].z);
    gd_setproperty(GD_PROP_LIGHTING, 2.0f, 0.0f, 0.0f);
}

void update_shaders(struct ObjShape *shape, struct GdVec3f *offset) {
    restart_timer("updateshaders");
    stash_current_gddl();
    sLightPositionOffset.x = offset->x;
    sLightPositionOffset.y = offset->y;
    sLightPositionOffset.z = offset->z;
    sPhongLight = NULL;
    if (gGdLightGroup != NULL) {
        apply_to_obj_types_in_group(OBJ_TYPE_LIGHTS, (applyproc_t) Proc8017A980, gGdLightGroup);
    }
    if (shape->mtlGroup != NULL) {
        apply_to_obj_types_in_group(OBJ_TYPE_MATERIALS, (applyproc_t) apply_obj_draw_fn,
                                    shape->mtlGroup);
    }
    pop_gddl_stash();
    split_timer("updateshaders");
}

void create_shape_mtl_gddls(struct ObjShape *shape) {
    if (shape->mtlGroup != NULL) {
        apply_to_obj_types_in_group(OBJ_TYPE_MATERIALS, (applyproc_t) create_mtl_gddl_if_empty,
                                    shape->mtlGroup);
    }
}

void unref_8017AEDC(struct ObjGroup *grp) {
    register struct ListNode *link = grp->firstMember;

    while (link != NULL) {
        struct GdObj *obj = link->obj;

        stub_objects_1(grp, obj);
        link = link->next;
    }
}

#ifdef AVOID_UB
void
#else
s32
#endif
create_shape_gddl(struct ObjShape *s) {
    struct ObjShape *shape = s;
    s32 shapedl;
    UNUSED s32 enddl;

    create_shape_mtl_gddls(shape);
    shapedl = gd_startdisplist(7);
    if (shapedl == 0) {
#ifdef AVOID_UB
        return;
#else
        return -1;
#endif
    }

    setup_lights();
    sUseSelectedColor = FALSE;
    if (shape->unk3C == 0) {
        draw_shape_faces(shape);
    }
    enddl = gd_enddlsplist_parent();
    shape->dlNums[0] = shapedl;
    shape->dlNums[1] = shapedl;

    if (shape->name[0] != '\0') {
        printf("Generated '%s' (%d) display list ok.(%d)\n", shape->name, shapedl, enddl);
    } else {
        printf("Generated 'UNKNOWN' (%d) display list ok.(%d)\n", shapedl, enddl);
    }
}

void create_gddl_for_shapes(struct ObjGroup *grp) {
    UNUSED s32 shapedls =
        apply_to_obj_types_in_group(OBJ_TYPE_SHAPES, (applyproc_t) create_shape_gddl, grp);
    printf("made %d display lists\n", shapedls);
}

void map_face_materials(struct ObjGroup *faces, struct ObjGroup *mtls) {
    struct ObjFace *face;
    register struct ListNode *linkFaces;
    struct GdObj *temp;
    register struct ListNode *linkMtls;
    struct ObjMaterial *mtl;

    linkFaces = faces->firstMember;
    while (linkFaces != NULL) {
        temp = linkFaces->obj;
        face = (struct ObjFace *) temp;
        linkMtls = mtls->firstMember;
        while (linkMtls != NULL) {
            mtl = (struct ObjMaterial *) linkMtls->obj;
            if (mtl->id == face->mtlId) {
                break;
            }
            linkMtls = linkMtls->next;
        }

        if (linkMtls != NULL) {
            face->mtl = mtl;
        }

        linkFaces = linkFaces->next;
    }
}

static void calc_vtx_normal(struct ObjVertex *vtx, struct ObjGroup *facegrp) {
    s32 i;
    s32 faceCount;
    register struct ListNode *node;
    struct ObjFace *curFace;

    vtx->normal.x = vtx->normal.y = vtx->normal.z = 0.0f;

    faceCount = 0;
    node = facegrp->firstMember;
    while (node != NULL) {
        curFace = (struct ObjFace *) node->obj;
        for (i = 0; i < curFace->vtxCount; i++) {
            if (curFace->vertices[i] == vtx) {
                vtx->normal.x += curFace->normal.x;
                vtx->normal.y += curFace->normal.y;
                vtx->normal.z += curFace->normal.z;
                faceCount++;
            }
        }
        node = node->next;
    }
    if (faceCount != 0) {
        vtx->normal.x /= faceCount;
        vtx->normal.y /= faceCount;
        vtx->normal.z /= faceCount;
    }
}

static void find_thisface_verts(struct ObjFace *face, struct ObjGroup *vertexGrp) {
    s32 i;
    u32 currIndex;
    struct ListNode *node;

    for (i = 0; i < face->vtxCount; i++) {

        node = vertexGrp->firstMember;
        currIndex = 0;
        while (node != NULL) {
            if (node->obj->type == OBJ_TYPE_VERTICES || node->obj->type == OBJ_TYPE_PARTICLES) {
                if (currIndex++ == (u32) (uintptr_t) face->vertices[i]) {
                    break;
                }
            }
            node = node->next;
        }
        if (node == NULL) {
            fatal_printf("find_thisface_verts(): Vertex not found");
        }

        face->vertices[i] = (struct ObjVertex *) node->obj;
    }
    calc_face_normal(face);
}

void map_vertices(struct ObjGroup *facegrp, struct ObjGroup *vtxgrp) {
    register struct ListNode *faceNode;
    struct ObjFace *curFace;
    register struct ListNode *vtxNode;
    struct ObjVertex *vtx;

    imin("map_vertices");

    faceNode = facegrp->firstMember;
    while (faceNode != NULL) {
        curFace = (struct ObjFace *) faceNode->obj;
        find_thisface_verts(curFace, vtxgrp);
        faceNode = faceNode->next;
    }

    vtxNode = vtxgrp->firstMember;
    while (vtxNode != NULL) {
        vtx = (struct ObjVertex *) vtxNode->obj;
        calc_vtx_normal(vtx, facegrp);
        vtxNode = vtxNode->next;
    }

    imout();
}

void unpick_obj(struct GdObj *obj) {
    struct GdObj *why = obj;
    if (why->drawFlags & OBJ_IS_GRABBABLE) {
        why->drawFlags &= ~(OBJ_PICKED | OBJ_HIGHLIGHTED);
    }
}

void find_closest_pickable_obj(struct GdObj *input) {
    struct GdObj *obj = input;
    UNUSED u8 filler[12];
    f32 distance;

    if (obj->drawFlags & OBJ_IS_GRABBABLE) {
        if (obj->index == sPickDataTemp) {
            if (gViewUpdateCamera != NULL) {
                distance = d_calc_world_dist_btwn(&gViewUpdateCamera->header, obj);
            } else {
                distance = 0.0f;
            }

            if (distance < sPickObjDistance) {
                sPickObjDistance = distance;
                sPickedObject = obj;
            }
        }
    }
}

void set_view_update_camera(struct ObjCamera *cam) {
    if (gViewUpdateCamera != NULL) {
        return;
    }

    gViewUpdateCamera = cam;
}

void update_view(struct ObjView *view) {
    s32 i;
    s32 pickOffset;
    s32 pickDataSize;
    s32 j;
    s32 pickDataIdx;
    s32 pickedObjType;
    char objTypeAbbr[0x100];

    sUpdateViewState.shapesDrawn = 0;
    sUpdateViewState.unused = 0;

    if (!(view->flags & VIEW_UPDATE)) {
        view->flags &= ~VIEW_WAS_UPDATED;
        return;
    }

    imin("UpdateView()");
    if (view->proc != NULL) {
        view->proc(view);
    }

    if (!(view->flags & VIEW_WAS_UPDATED)) {
        view->flags |= VIEW_WAS_UPDATED;
    }

    gViewUpdateCamera = NULL;
    if (view->components != NULL) {
        apply_to_obj_types_in_group(OBJ_TYPE_CAMERAS, (applyproc_t) set_view_update_camera,
                                    view->components);
        view->activeCam = gViewUpdateCamera;

        if (view->activeCam != NULL) {
            gViewUpdateCamera->unk18C = view;
        }
    }

    if (view->flags & VIEW_MOVEMENT) {
        split_timer("dlgen");
        restart_timer("dynamics");
        proc_view_movement(view);
        split_timer("dynamics");
        restart_timer("dlgen");
        gViewUpdateCamera = view->activeCam;
    }

    if (!(view->flags & VIEW_DRAW)) {
        imout();
        return;
    }

    sUpdateViewState.view = view;
    set_active_view(view);
    view->gdDlNum = gd_startdisplist(8);
    start_view_dl(sUpdateViewState.view);
    gd_shading(9);

    if (view->flags & (VIEW_UNK_2000 | VIEW_UNK_4000)) {
        gd_set_one_cycle();
    }

    if (view->components != NULL) {
        if (gGdCtrl.dragging) {
            if (gd_getproperty(3, 0) != FALSE && gGdCtrl.startedDragging != FALSE) {
                init_pick_buf(sPickBuffer, ARRAY_COUNT(sPickBuffer));
                drawscene(FIND_PICKS, sUpdateViewState.view->components, NULL);
                pickOffset = get_cur_pickbuf_offset(sPickBuffer);
                sPickDataTemp = 0;
                sPickedObject = NULL;
                sPickObjDistance = 10000000.0f;

                if (pickOffset < 0) {
                    fatal_printf("UpdateView(): Pick buffer too small");
                } else if (pickOffset > 0) {
                    pickDataIdx = 0;
                    for (i = 0; i < pickOffset; i++) {
                        pickDataSize = sPickBuffer[pickDataIdx++];
                        if (pickDataSize != 0) {
                            switch ((pickedObjType = sPickBuffer[pickDataIdx++])) {
                                case OBJ_TYPE_JOINTS:
                                    gd_strcpy(objTypeAbbr, "J");
                                    break;
                                case OBJ_TYPE_NETS:
                                    gd_strcpy(objTypeAbbr, "N");
                                    break;
                                case OBJ_TYPE_PARTICLES:
                                    gd_strcpy(objTypeAbbr, "P");
                                    break;
                                default:
                                    gd_strcpy(objTypeAbbr, "?");
                                    break;
                            }
                        }

                        if (pickDataSize >= 2) {
                            for (j = 0; j < pickDataSize - 1; j++) {
                                sPickDataTemp = sPickBuffer[pickDataIdx++];
                                apply_to_obj_types_in_group(pickedObjType,
                                                            (applyproc_t) find_closest_pickable_obj,
                                                            sUpdateViewState.view->components);
                            }
                        }
                    }
                }

                if (sPickedObject != NULL) {
                    sPickedObject->drawFlags |= OBJ_PICKED;
                    sPickedObject->drawFlags |= OBJ_HIGHLIGHTED;
                    sUpdateViewState.view->pickedObj = sPickedObject;
                    gGdCtrl.dragStartX = gGdCtrl.csrX = sGrabCords.x;
                    gGdCtrl.dragStartY = gGdCtrl.csrY = sGrabCords.y;
                }
            }

            find_and_drag_picked_object(sUpdateViewState.view->components);
        } else {
            if (sUpdateViewState.view->pickedObj != NULL) {
                sUpdateViewState.view->pickedObj->drawFlags &= ~OBJ_PICKED;
                sUpdateViewState.view->pickedObj->drawFlags &= ~OBJ_HIGHLIGHTED;
                sUpdateViewState.view->pickedObj = NULL;
            }
        }

        drawscene(RENDER_SCENE, sUpdateViewState.view->components, sUpdateViewState.view->lights);
    }

    border_active_view();
    gd_enddlsplist_parent();
    imout();
    return;
}

void stub_draw_objects_1(void) {
}
