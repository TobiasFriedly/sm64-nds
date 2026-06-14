#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include <math.h>

#include "engine/math_util.h"
#include "engine/surface_collision.h"
#include "geo_misc.h"
#include "level_table.h"
#include "memory.h"
#include "object_list_processor.h"
#include "rendering_graph_node.h"
#include "segment2.h"
#include "shadow.h"
#include "sm64.h"

struct Shadow {

    f32 parentX;
    f32 parentY;
    f32 parentZ;

    f32 floorHeight;

    f32 shadowScale;

    f32 floorNormalX;
    f32 floorNormalY;
    f32 floorNormalZ;

    f32 floorOriginOffset;

    f32 floorDownwardAngle;

    f32 floorTilt;

    u8 solidity;
};

#define SHADOW_SOLIDITY_NO_SHADOW 0

#define SHADOW_SOILDITY_ALREADY_SET 1

#define SHADOW_SOLIDITY_NOT_YET_SET 2

#define SHADOW_SHAPE_CIRCLE 10

#define SHADOW_SHAPE_SQUARE 20

#define SHADOW_WITH_9_VERTS 0

#define SHADOW_WITH_4_VERTS 1

typedef struct {

    f32 halfWidth;

    f32 halfLength;

    s8 scaleWithDistance;
} shadowRectangle;

shadowRectangle rectangles[2] = {

    { 360.0f, 230.0f, TRUE },

    { 200.0f, 180.0f, TRUE }
};

s8 gShadowAboveWaterOrLava;
s8 gMarioOnIceOrCarpet;
s8 sMarioOnFlyingCarpet;
s16 sSurfaceTypeBelowShadow;

void rotate_rectangle(f32 *newZ, f32 *newX, f32 oldZ, f32 oldX) {
    struct Object *obj = (struct Object *) gCurGraphNodeObject;
    *newZ = oldZ * coss(obj->oFaceAngleYaw) - oldX * sins(obj->oFaceAngleYaw);
    *newX = oldZ * sins(obj->oFaceAngleYaw) + oldX * coss(obj->oFaceAngleYaw);
}

f32 atan2_deg(f32 a, f32 b) {
    return ((f32) atan2s(a, b) / 65535.0 * 360.0);
}

f32 scale_shadow_with_distance(f32 initial, f32 distFromFloor) {
    f32 newScale;

    if (distFromFloor <= 0.0) {
        newScale = initial;
    } else if (distFromFloor >= 600.0) {
        newScale = initial * 0.5;
    } else {
        newScale = initial * (1.0 - (0.5 * distFromFloor / 600.0));
    }

    return newScale;
}

f32 disable_shadow_with_distance(f32 shadowScale, f32 distFromFloor) {
    if (distFromFloor >= 600.0) {
        return 0.0f;
    } else {
        return shadowScale;
    }
}

u8 dim_shadow_with_distance(u8 solidity, f32 distFromFloor) {
    f32 ret;

    if (solidity < 121) {
        return solidity;
    } else if (distFromFloor <= 0.0) {
        return solidity;
    } else if (distFromFloor >= 600.0) {
        return 120;
    } else {
        ret = ((120 - solidity) * distFromFloor) / 600.0 + (f32) solidity;
        return ret;
    }
}

f32 get_water_level_below_shadow(struct Shadow *s) {
    f32 waterLevel = find_water_level(s->parentX, s->parentZ);
    if (waterLevel < FLOOR_LOWER_LIMIT_SHADOW) {
        return 0;
    } else if (s->parentY >= waterLevel && s->floorHeight <= waterLevel) {
        gShadowAboveWaterOrLava = TRUE;
        return waterLevel;
    }

#ifdef AVOID_UB
    return waterLevel;
#endif
}

s8 init_shadow(struct Shadow *s, f32 xPos, f32 yPos, f32 zPos, s16 shadowScale, u8 overwriteSolidity) {
    f32 waterLevel;
    f32 floorSteepness;
    struct FloorGeometry *floorGeometry;

    s->parentX = xPos;
    s->parentY = yPos;
    s->parentZ = zPos;

    s->floorHeight = find_floor_height_and_data(s->parentX, s->parentY, s->parentZ, &floorGeometry);

    if (gEnvironmentRegions != NULL) {
        waterLevel = get_water_level_below_shadow(s);
    }
    if (gShadowAboveWaterOrLava) {

        s->floorHeight = waterLevel;

        s->floorNormalX = 0;
        s->floorNormalY = 1.0;
        s->floorNormalZ = 0;
        s->floorOriginOffset = -waterLevel;
    } else {

        if (s->floorHeight < FLOOR_LOWER_LIMIT_SHADOW || floorGeometry->normalY <= 0.0) {
            return 1;
        }

        s->floorNormalX = floorGeometry->normalX;
        s->floorNormalY = floorGeometry->normalY;
        s->floorNormalZ = floorGeometry->normalZ;
        s->floorOriginOffset = floorGeometry->originOffset;
    }

    if (overwriteSolidity) {
        s->solidity = dim_shadow_with_distance(overwriteSolidity, yPos - s->floorHeight);
    }

    s->shadowScale = scale_shadow_with_distance(shadowScale, yPos - s->floorHeight);

    s->floorDownwardAngle = atan2_deg(s->floorNormalZ, s->floorNormalX);

    floorSteepness = sqrtf(s->floorNormalX * s->floorNormalX + s->floorNormalZ * s->floorNormalZ);

    if (floorSteepness == 0.0) {
        s->floorTilt = 0;
    } else {
        s->floorTilt = 90.0 - atan2_deg(floorSteepness, s->floorNormalY);
    }
    return 0;
}

void get_texture_coords_9_vertices(s8 vertexNum, s16 *textureX, s16 *textureY) {
    *textureX = vertexNum % 3 * 15 - 15;
    *textureY = vertexNum / 3 * 15 - 15;
}

void get_texture_coords_4_vertices(s8 vertexNum, s16 *textureX, s16 *textureY) {
    *textureX = (vertexNum % 2) * 2 * 15 - 15;
    *textureY = (vertexNum / 2) * 2 * 15 - 15;
}

void make_shadow_vertex_at_xyz(Vtx *vertices, s8 index, f32 relX, f32 relY, f32 relZ, u8 alpha,
                               s8 shadowVertexType) {
    s16 vtxX = round_float(relX);
    s16 vtxY = round_float(relY);
    s16 vtxZ = round_float(relZ);
    s16 textureX, textureY;

    switch (shadowVertexType) {
        case SHADOW_WITH_9_VERTS:
            get_texture_coords_9_vertices(index, &textureX, &textureY);
            break;
        case SHADOW_WITH_4_VERTS:
            get_texture_coords_4_vertices(index, &textureX, &textureY);
            break;
    }

    if (sMarioOnFlyingCarpet) {
        vtxX += 5;
        vtxY += 5;
        vtxZ += 5;
    }
    make_vertex(
        vertices, index, vtxX, vtxY, vtxZ, textureX << 5, textureY << 5, 255, 255, 255, alpha
    );
}

f32 extrapolate_vertex_y_position(struct Shadow s, f32 vtxX, f32 vtxZ) {
    return -(s.floorNormalX * vtxX + s.floorNormalZ * vtxZ + s.floorOriginOffset) / s.floorNormalY;
}

void get_vertex_coords(s8 index, s8 shadowVertexType, s8 *xCoord, s8 *zCoord) {
    *xCoord = index % (3 - shadowVertexType) - 1;
    *zCoord = index / (3 - shadowVertexType) - 1;

    if (shadowVertexType == SHADOW_WITH_4_VERTS) {
        if (*xCoord == 0) {
            *xCoord = 1;
        }
        if (*zCoord == 0) {
            *zCoord = 1;
        }
    }
}

void calculate_vertex_xyz(s8 index, struct Shadow s, f32 *xPosVtx, f32 *yPosVtx, f32 *zPosVtx,
                          s8 shadowVertexType) {
    f32 tiltedScale = cosf(s.floorTilt * M_PI / 180.0) * s.shadowScale;
    f32 downwardAngle = s.floorDownwardAngle * M_PI / 180.0;
    f32 halfScale;
    f32 halfTiltedScale;
    s8 xCoordUnit;
    s8 zCoordUnit;
    struct FloorGeometry *dummy;

    get_vertex_coords(index, shadowVertexType, &xCoordUnit, &zCoordUnit);

    halfScale = (xCoordUnit * s.shadowScale) / 2.0;
    halfTiltedScale = (zCoordUnit * tiltedScale) / 2.0;

    *xPosVtx = (halfTiltedScale * sinf(downwardAngle)) + (halfScale * cosf(downwardAngle)) + s.parentX;
    *zPosVtx = (halfTiltedScale * cosf(downwardAngle)) - (halfScale * sinf(downwardAngle)) + s.parentZ;

    if (gShadowAboveWaterOrLava) {
        *yPosVtx = s.floorHeight;
    } else {
        switch (shadowVertexType) {

            case SHADOW_WITH_9_VERTS:

                *yPosVtx = find_floor_height_and_data(*xPosVtx, s.parentY, *zPosVtx, &dummy);
                break;
            case SHADOW_WITH_4_VERTS:

                *yPosVtx = extrapolate_vertex_y_position(s, *xPosVtx, *zPosVtx);
                break;
        }
    }
}

s16 floor_local_tilt(struct Shadow s, f32 vtxX, f32 vtxY, f32 vtxZ) {
    f32 relX = vtxX - s.parentX;
    f32 relY = vtxY - s.floorHeight;
    f32 relZ = vtxZ - s.parentZ;

    f32 ret = (relX * s.floorNormalX) + (relY * s.floorNormalY) + (relZ * s.floorNormalZ);
    return ret;
}

void make_shadow_vertex(Vtx *vertices, s8 index, struct Shadow s, s8 shadowVertexType) {
    f32 xPosVtx, yPosVtx, zPosVtx;
    f32 relX, relY, relZ;

    u8 solidity = s.solidity;
    if (gShadowAboveWaterOrLava) {
        solidity = 200;
    }

    calculate_vertex_xyz(index, s, &xPosVtx, &yPosVtx, &zPosVtx, shadowVertexType);

    if (shadowVertexType == SHADOW_WITH_9_VERTS && !gShadowAboveWaterOrLava
        && floor_local_tilt(s, xPosVtx, yPosVtx, zPosVtx) != 0) {
        yPosVtx = extrapolate_vertex_y_position(s, xPosVtx, zPosVtx);
        solidity = 0;
    }
    relX = xPosVtx - s.parentX;
    relY = yPosVtx - s.parentY;
    relZ = zPosVtx - s.parentZ;

    make_shadow_vertex_at_xyz(vertices, index, relX, relY, relZ, solidity, shadowVertexType);
}

void add_shadow_to_display_list(Gfx *displayListHead, Vtx *verts, s8 shadowVertexType, s8 shadowShape) {
    switch (shadowShape) {
        case SHADOW_SHAPE_CIRCLE:
            gSPDisplayList(displayListHead++, dl_shadow_circle);
            break;
        case SHADOW_SHAPE_SQUARE:
            gSPDisplayList(displayListHead++, dl_shadow_square) break;
    }
    switch (shadowVertexType) {
        case SHADOW_WITH_9_VERTS:
            gSPVertex(displayListHead++, verts, 9, 0);
            gSPDisplayList(displayListHead++, dl_shadow_9_verts);
            break;
        case SHADOW_WITH_4_VERTS:
            gSPVertex(displayListHead++, verts, 4, 0);
            gSPDisplayList(displayListHead++, dl_shadow_4_verts);
            break;
    }
    gSPDisplayList(displayListHead++, dl_shadow_end);
    gSPEndDisplayList(displayListHead);
}

void linearly_interpolate_solidity_positive(struct Shadow *s, u8 finalSolidity, s16 curr, s16 start,
                                            s16 end) {
    if (curr >= 0 && curr < start) {
        s->solidity = 0;
    } else if (end < curr) {
        s->solidity = finalSolidity;
    } else {
        s->solidity = (f32) finalSolidity * (curr - start) / (end - start);
    }
}

void linearly_interpolate_solidity_negative(struct Shadow *s, u8 initialSolidity, s16 curr, s16 start,
                                            s16 end) {

    if (curr >= start && end >= curr) {
        s->solidity = ((f32) initialSolidity * (1.0 - (f32)(curr - start) / (end - start)));
    } else {
        s->solidity = 0;
    }
}

s8 correct_shadow_solidity_for_animations(s32 isLuigi, u8 initialSolidity, struct Shadow *shadow) {
    struct Object *player;
    s8 ret;
    s16 animFrame;

    switch (isLuigi) {
        case 0:
            player = gMarioObject;
            break;
        case 1:

            player = gLuigiObject;
            break;
    }

    animFrame = player->header.gfx.animInfo.animFrame;
    switch (player->header.gfx.animInfo.animID) {
        case MARIO_ANIM_IDLE_ON_LEDGE:
            ret = SHADOW_SOLIDITY_NO_SHADOW;
            break;
        case MARIO_ANIM_FAST_LEDGE_GRAB:
            linearly_interpolate_solidity_positive(shadow, initialSolidity, animFrame, 5, 14);
            ret = SHADOW_SOILDITY_ALREADY_SET;
            break;
        case MARIO_ANIM_SLOW_LEDGE_GRAB:
            linearly_interpolate_solidity_positive(shadow, initialSolidity, animFrame, 21, 33);
            ret = SHADOW_SOILDITY_ALREADY_SET;
            break;
        case MARIO_ANIM_CLIMB_DOWN_LEDGE:
            linearly_interpolate_solidity_negative(shadow, initialSolidity, animFrame, 0, 5);
            ret = SHADOW_SOILDITY_ALREADY_SET;
            break;
        default:
            ret = SHADOW_SOLIDITY_NOT_YET_SET;
            break;
    }
    return ret;
}

void correct_lava_shadow_height(struct Shadow *s) {
    if (gCurrLevelNum == LEVEL_BITFS && sSurfaceTypeBelowShadow == SURFACE_BURNING) {
        if (s->floorHeight < -3000.0) {
            s->floorHeight = -3062.0;
            gShadowAboveWaterOrLava = TRUE;
        } else if (s->floorHeight > 3400.0) {
            s->floorHeight = 3492.0;
            gShadowAboveWaterOrLava = TRUE;
        }
    } else if (gCurrLevelNum == LEVEL_LLL && gCurrAreaIndex == 1
               && sSurfaceTypeBelowShadow == SURFACE_BURNING) {
        s->floorHeight = 5.0;
        gShadowAboveWaterOrLava = TRUE;
    }
}

Gfx *create_shadow_player(f32 xPos, f32 yPos, f32 zPos, s16 shadowScale, u8 solidity, s32 isLuigi) {
    Vtx *verts;
    Gfx *displayList;
    struct Shadow shadow;
    s8 ret;
    s32 i;

    if (gCurrLevelNum == LEVEL_RR && sSurfaceTypeBelowShadow != SURFACE_DEATH_PLANE) {
        switch (gFlyingCarpetState) {
            case FLYING_CARPET_MOVING_WITHOUT_MARIO:
                gMarioOnIceOrCarpet = 1;
                sMarioOnFlyingCarpet = 1;
                break;
            case FLYING_CARPET_MOVING_WITH_MARIO:
                gMarioOnIceOrCarpet = 1;
                break;
        }
    }

    switch (correct_shadow_solidity_for_animations(isLuigi, solidity, &shadow)) {
        case SHADOW_SOLIDITY_NO_SHADOW:
            return NULL;
            break;
        case SHADOW_SOILDITY_ALREADY_SET:
            ret = init_shadow(&shadow, xPos, yPos, zPos, shadowScale,  0);
            break;
        case SHADOW_SOLIDITY_NOT_YET_SET:
            ret = init_shadow(&shadow, xPos, yPos, zPos, shadowScale, solidity);
            break;
    }
    if (ret != 0) {
        return NULL;
    }

    verts = alloc_display_list(9 * sizeof(Vtx));
    displayList = alloc_display_list(5 * sizeof(Gfx));
    if (verts == NULL || displayList == NULL) {
        return NULL;
    }

    correct_lava_shadow_height(&shadow);

    for (i = 0; i < 9; i++) {
        make_shadow_vertex(verts, i, shadow, SHADOW_WITH_9_VERTS);
    }
    add_shadow_to_display_list(displayList, verts, SHADOW_WITH_9_VERTS, SHADOW_SHAPE_CIRCLE);
    return displayList;
}

Gfx *create_shadow_circle_9_verts(f32 xPos, f32 yPos, f32 zPos, s16 shadowScale, u8 solidity) {
    Vtx *verts;
    Gfx *displayList;
    struct Shadow shadow;
    s32 i;

    if (init_shadow(&shadow, xPos, yPos, zPos, shadowScale, solidity) != 0) {
        return NULL;
    }

    verts = alloc_display_list(9 * sizeof(Vtx));
    displayList = alloc_display_list(5 * sizeof(Gfx));

    if (verts == NULL || displayList == NULL) {
        return 0;
    }
    for (i = 0; i < 9; i++) {
        make_shadow_vertex(verts, i, shadow, SHADOW_WITH_9_VERTS);
    }
    add_shadow_to_display_list(displayList, verts, SHADOW_WITH_9_VERTS, SHADOW_SHAPE_CIRCLE);
    return displayList;
}

Gfx *create_shadow_circle_4_verts(f32 xPos, f32 yPos, f32 zPos, s16 shadowScale, u8 solidity) {
    Vtx *verts;
    Gfx *displayList;
    struct Shadow shadow;
    s32 i;

    if (init_shadow(&shadow, xPos, yPos, zPos, shadowScale, solidity) != 0) {
        return NULL;
    }

    verts = alloc_display_list(4 * sizeof(Vtx));
    displayList = alloc_display_list(5 * sizeof(Gfx));

    if (verts == NULL || displayList == NULL) {
        return 0;
    }

    for (i = 0; i < 4; i++) {
        make_shadow_vertex(verts, i, shadow, SHADOW_WITH_4_VERTS);
    }
    add_shadow_to_display_list(displayList, verts, SHADOW_WITH_4_VERTS, SHADOW_SHAPE_CIRCLE);
    return displayList;
}

Gfx *create_shadow_circle_assuming_flat_ground(f32 xPos, f32 yPos, f32 zPos, s16 shadowScale,
                                               u8 solidity) {
    Vtx *verts;
    Gfx *displayList;
    struct FloorGeometry *dummy;
    f32 distBelowFloor;
    f32 floorHeight = find_floor_height_and_data(xPos, yPos, zPos, &dummy);
    f32 radius = shadowScale / 2;

    if (floorHeight < FLOOR_LOWER_LIMIT_SHADOW) {
        return NULL;
    } else {
        distBelowFloor = floorHeight - yPos;
    }

    verts = alloc_display_list(4 * sizeof(Vtx));
    displayList = alloc_display_list(5 * sizeof(Gfx));

    if (verts == NULL || displayList == NULL) {
        return 0;
    }

    make_shadow_vertex_at_xyz(verts, 0, -radius, distBelowFloor, -radius, solidity, 1);
    make_shadow_vertex_at_xyz(verts, 1, radius, distBelowFloor, -radius, solidity, 1);
    make_shadow_vertex_at_xyz(verts, 2, -radius, distBelowFloor, radius, solidity, 1);
    make_shadow_vertex_at_xyz(verts, 3, radius, distBelowFloor, radius, solidity, 1);

    add_shadow_to_display_list(displayList, verts, SHADOW_WITH_4_VERTS, SHADOW_SHAPE_CIRCLE);
    return displayList;
}

Gfx *create_shadow_rectangle(f32 halfWidth, f32 halfLength, f32 relY, u8 solidity) {
    Vtx *verts = alloc_display_list(4 * sizeof(Vtx));
    Gfx *displayList = alloc_display_list(5 * sizeof(Gfx));
    f32 frontLeftX, frontLeftZ, frontRightX, frontRightZ, backLeftX, backLeftZ, backRightX, backRightZ;

    if (verts == NULL || displayList == NULL) {
        return NULL;
    }

    rotate_rectangle(&frontLeftZ, &frontLeftX, -halfLength, -halfWidth);
    rotate_rectangle(&frontRightZ, &frontRightX, -halfLength, halfWidth);
    rotate_rectangle(&backLeftZ, &backLeftX, halfLength, -halfWidth);
    rotate_rectangle(&backRightZ, &backRightX, halfLength, halfWidth);

    make_shadow_vertex_at_xyz(verts, 0, frontLeftX, relY, frontLeftZ, solidity, 1);
    make_shadow_vertex_at_xyz(verts, 1, frontRightX, relY, frontRightZ, solidity, 1);
    make_shadow_vertex_at_xyz(verts, 2, backLeftX, relY, backLeftZ, solidity, 1);
    make_shadow_vertex_at_xyz(verts, 3, backRightX, relY, backRightZ, solidity, 1);

    add_shadow_to_display_list(displayList, verts, SHADOW_WITH_4_VERTS, SHADOW_SHAPE_SQUARE);
    return displayList;
}

s32 get_shadow_height_solidity(f32 xPos, f32 yPos, f32 zPos, f32 *shadowHeight, u8 *solidity) {
    struct FloorGeometry *dummy;
    f32 waterLevel;
    *shadowHeight = find_floor_height_and_data(xPos, yPos, zPos, &dummy);

    if (*shadowHeight < FLOOR_LOWER_LIMIT_SHADOW) {
        return 1;
    } else {
        waterLevel = find_water_level(xPos, zPos);

        if (waterLevel < FLOOR_LOWER_LIMIT_SHADOW) {

        } else if (yPos >= waterLevel && waterLevel >= *shadowHeight) {
            gShadowAboveWaterOrLava = TRUE;
            *shadowHeight = waterLevel;
            *solidity = 200;
        }
    }
    return 0;
}

Gfx *create_shadow_square(f32 xPos, f32 yPos, f32 zPos, s16 shadowScale, u8 solidity, s8 shadowType) {
    f32 shadowHeight;
    f32 distFromShadow;
    f32 shadowRadius;

    if (get_shadow_height_solidity(xPos, yPos, zPos, &shadowHeight, &solidity) != 0) {
        return NULL;
    }

    distFromShadow = yPos - shadowHeight;
    switch (shadowType) {
        case SHADOW_SQUARE_PERMANENT:
            shadowRadius = shadowScale / 2;
            break;
        case SHADOW_SQUARE_SCALABLE:
            shadowRadius = scale_shadow_with_distance(shadowScale, distFromShadow) / 2.0;
            break;
        case SHADOW_SQUARE_TOGGLABLE:
            shadowRadius = disable_shadow_with_distance(shadowScale, distFromShadow) / 2.0;
            break;
        default:
            return NULL;
    }

    return create_shadow_rectangle(shadowRadius, shadowRadius, -distFromShadow, solidity);
}

Gfx *create_shadow_hardcoded_rectangle(f32 xPos, f32 yPos, f32 zPos, UNUSED s16 shadowScale,
                                       u8 solidity, s8 shadowType) {
    f32 shadowHeight;
    f32 distFromShadow;
    f32 halfWidth;
    f32 halfLength;
    s8 idx = shadowType - SHADOW_RECTANGLE_HARDCODED_OFFSET;

    if (get_shadow_height_solidity(xPos, yPos, zPos, &shadowHeight, &solidity) != 0) {
        return NULL;
    }

    distFromShadow = yPos - shadowHeight;

    if (rectangles[idx].scaleWithDistance == TRUE) {
        halfWidth = scale_shadow_with_distance(rectangles[idx].halfWidth, distFromShadow);
        halfLength = scale_shadow_with_distance(rectangles[idx].halfLength, distFromShadow);
    } else {

        halfWidth = rectangles[idx].halfWidth;
        halfLength = rectangles[idx].halfLength;
    }
    return create_shadow_rectangle(halfWidth, halfLength, -distFromShadow, solidity);
}

Gfx *create_shadow_below_xyz(f32 xPos, f32 yPos, f32 zPos, s16 shadowScale, u8 shadowSolidity,
                             s8 shadowType) {
    Gfx *displayList = NULL;
    struct Surface *pfloor;
    find_floor(xPos, yPos, zPos, &pfloor);

    gShadowAboveWaterOrLava = FALSE;
    gMarioOnIceOrCarpet = 0;
    sMarioOnFlyingCarpet = 0;
    if (pfloor != NULL) {
        if (pfloor->type == SURFACE_ICE) {
            gMarioOnIceOrCarpet = 1;
        }
        sSurfaceTypeBelowShadow = pfloor->type;
    }
    switch (shadowType) {
        case SHADOW_CIRCLE_9_VERTS:
            displayList = create_shadow_circle_9_verts(xPos, yPos, zPos, shadowScale, shadowSolidity);
            break;
        case SHADOW_CIRCLE_4_VERTS:
            displayList = create_shadow_circle_4_verts(xPos, yPos, zPos, shadowScale, shadowSolidity);
            break;
        case SHADOW_CIRCLE_4_VERTS_FLAT_UNUSED:
            displayList = create_shadow_circle_assuming_flat_ground(xPos, yPos, zPos, shadowScale,
                                                                    shadowSolidity);
            break;
        case SHADOW_SQUARE_PERMANENT:
            displayList =
                create_shadow_square(xPos, yPos, zPos, shadowScale, shadowSolidity, shadowType);
            break;
        case SHADOW_SQUARE_SCALABLE:
            displayList =
                create_shadow_square(xPos, yPos, zPos, shadowScale, shadowSolidity, shadowType);
            break;
        case SHADOW_SQUARE_TOGGLABLE:
            displayList =
                create_shadow_square(xPos, yPos, zPos, shadowScale, shadowSolidity, shadowType);
            break;
        case SHADOW_CIRCLE_PLAYER:
            displayList = create_shadow_player(xPos, yPos, zPos, shadowScale, shadowSolidity,
                                                FALSE);
            break;
        default:
            displayList = create_shadow_hardcoded_rectangle(xPos, yPos, zPos, shadowScale,
                                                            shadowSolidity, shadowType);
            break;
    }
    return displayList;
}
