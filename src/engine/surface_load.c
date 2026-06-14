#include <PR/ultratypes.h>

#ifdef TARGET_NDS
extern void nds_debug_printf(const char *fmt, ...);
#define DBG(...)
#else
#define DBG(...)
#endif

#include "sm64.h"
#include "game/ingame_menu.h"
#include "graph_node.h"
#include "behavior_script.h"
#include "behavior_data.h"
#include "game/memory.h"
#include "game/object_helpers.h"
#include "game/macro_special_objects.h"
#include "surface_collision.h"
#include "game/mario.h"
#include "game/object_list_processor.h"
#include "surface_load.h"
#ifdef TARGET_NDS
#include <stdlib.h>
#endif

s32 unused8038BE90;

SpatialPartitionCell gStaticSurfacePartition[NUM_CELLS][NUM_CELLS];
SpatialPartitionCell gDynamicSurfacePartition[NUM_CELLS][NUM_CELLS];

struct SurfaceNode *sSurfaceNodePool;
struct Surface *sSurfacePool;

#ifdef TARGET_NDS

#define NDS_SURFACE_POOL_SIZE 2300
#define NDS_SURFACE_NODE_POOL_SIZE 7000
static struct Surface *sNdsSurfacePoolHeap = NULL;
static struct SurfaceNode *sNdsSurfaceNodePoolHeap = NULL;
#endif

s16 sSurfacePoolSize;
static s16 sSurfaceNodePoolSize;

static s16 sTerrainNumVertices;
static u8 sSurfaceLoadWarnFlags;

#define SURF_WARN_POOL      (1 << 0)
#define SURF_WARN_NODE_POOL (1 << 1)
#define SURF_WARN_BAD_INDEX (1 << 2)

u8 unused8038EEA8[0x30];

static struct SurfaceNode *alloc_surface_node(void) {
    if (sSurfaceNodePool == NULL || gSurfaceNodesAllocated >= sSurfaceNodePoolSize) {
        if (!(sSurfaceLoadWarnFlags & SURF_WARN_NODE_POOL)) {
            DBG("surf node pool full n=%d cap=%d\n", gSurfaceNodesAllocated, sSurfaceNodePoolSize);
            sSurfaceLoadWarnFlags |= SURF_WARN_NODE_POOL;
        }
        return NULL;
    }

    struct SurfaceNode *node = &sSurfaceNodePool[gSurfaceNodesAllocated];
    gSurfaceNodesAllocated++;

    node->next = NULL;

    return node;
}

static struct Surface *alloc_surface(void) {
    if (sSurfacePool == NULL || gSurfacesAllocated >= sSurfacePoolSize) {
        if (!(sSurfaceLoadWarnFlags & SURF_WARN_POOL)) {
            DBG("surf pool full s=%d cap=%d\n", gSurfacesAllocated, sSurfacePoolSize);
            sSurfaceLoadWarnFlags |= SURF_WARN_POOL;
        }
        return NULL;
    }

    struct Surface *surface = &sSurfacePool[gSurfacesAllocated];
    gSurfacesAllocated++;

    surface->type = 0;
    surface->force = 0;
    surface->flags = 0;
    surface->room = 0;
    surface->object = NULL;

    return surface;
}

static void clear_spatial_partition(SpatialPartitionCell *cells) {
    register s32 i = NUM_CELLS * NUM_CELLS;

    while (i--) {
        (*cells)[SPATIAL_PARTITION_FLOORS].next = NULL;
        (*cells)[SPATIAL_PARTITION_CEILS].next = NULL;
        (*cells)[SPATIAL_PARTITION_WALLS].next = NULL;

        cells++;
    }
}

static void clear_static_surfaces(void) {
    clear_spatial_partition(&gStaticSurfacePartition[0][0]);
}

static void add_surface_to_cell(s16 dynamic, s16 cellX, s16 cellZ, struct Surface *surface) {
    struct SurfaceNode *newNode = alloc_surface_node();
    struct SurfaceNode *list;
    s16 surfacePriority;
    s16 priority;
    s16 sortDir;
    s16 listIndex;

    if (newNode == NULL) {
        return;
    }

    if (surface->normal.y > 0.01) {
        listIndex = SPATIAL_PARTITION_FLOORS;
        sortDir = 1;
    } else if (surface->normal.y < -0.01) {
        listIndex = SPATIAL_PARTITION_CEILS;
        sortDir = -1;
    } else {
        listIndex = SPATIAL_PARTITION_WALLS;
        sortDir = 0;

        if (surface->normal.x < -0.707 || surface->normal.x > 0.707) {
            surface->flags |= SURFACE_FLAG_X_PROJECTION;
        }
    }

    surfacePriority = surface->vertex1[1] * sortDir;

    newNode->surface = surface;

    if (dynamic) {
        list = &gDynamicSurfacePartition[cellZ][cellX][listIndex];
    } else {
        list = &gStaticSurfacePartition[cellZ][cellX][listIndex];
    }

    while (list->next != NULL) {
        priority = list->next->surface->vertex1[1] * sortDir;

        if (surfacePriority > priority) {
            break;
        }

        list = list->next;
    }

    newNode->next = list->next;
    list->next = newNode;
}

static s16 min_3(TerrainData a0, TerrainData a1, TerrainData a2) {
    if (a1 < a0) {
        a0 = a1;
    }

    if (a2 < a0) {
        a0 = a2;
    }

    return a0;
}

static s16 max_3(TerrainData a0, TerrainData a1, TerrainData a2) {
    if (a1 > a0) {
        a0 = a1;
    }

    if (a2 > a0) {
        a0 = a2;
    }

    return a0;
}

static s16 lower_cell_index(TerrainData coord) {
    s16 index;

    coord += LEVEL_BOUNDARY_MAX;
    if (coord < 0) {
        coord = 0;
    }

    index = coord / CELL_SIZE;

    if (coord % CELL_SIZE < 50) {
        index--;
    }

    if (index < 0) {
        index = 0;
    }

    return index;
}

static s16 upper_cell_index(TerrainData coord) {
    s16 index;

    coord += LEVEL_BOUNDARY_MAX;
    if (coord < 0) {
        coord = 0;
    }

    index = coord / CELL_SIZE;

    if (coord % CELL_SIZE > CELL_SIZE - 50) {
        index++;
    }

    if (index > NUM_CELLS_INDEX) {
        index = NUM_CELLS_INDEX;
    }

    return index;
}

static void add_surface(struct Surface *surface, s32 dynamic) {

    UNUSED s32 unused1, unused2;
    s16 minX, minZ, maxX, maxZ;

    s16 minCellX, minCellZ, maxCellX, maxCellZ;

    s16 cellZ, cellX;

    UNUSED s32 unused3 = 0;

    minX = min_3(surface->vertex1[0], surface->vertex2[0], surface->vertex3[0]);
    minZ = min_3(surface->vertex1[2], surface->vertex2[2], surface->vertex3[2]);
    maxX = max_3(surface->vertex1[0], surface->vertex2[0], surface->vertex3[0]);
    maxZ = max_3(surface->vertex1[2], surface->vertex2[2], surface->vertex3[2]);

    minCellX = lower_cell_index(minX);
    maxCellX = upper_cell_index(maxX);
    minCellZ = lower_cell_index(minZ);
    maxCellZ = upper_cell_index(maxZ);

    for (cellZ = minCellZ; cellZ <= maxCellZ; cellZ++) {
        for (cellX = minCellX; cellX <= maxCellX; cellX++) {
            add_surface_to_cell(dynamic, cellX, cellZ, surface);
        }
    }
}

UNUSED static void stub_surface_load_1(void) {
}

static struct Surface *read_surface_data(TerrainData *vertexData, TerrainData **vertexIndices) {
    struct Surface *surface;
    register s32 x1, y1, z1;
    register s32 x2, y2, z2;
    register s32 x3, y3, z3;
    s32 maxY, minY;
    f32 nx, ny, nz;
    f32 mag;
    TerrainData offset1, offset2, offset3;

    if (vertexData == NULL || sTerrainNumVertices <= 0) {
        return NULL;
    }

    if ((*vertexIndices)[0] < 0 || (*vertexIndices)[1] < 0 || (*vertexIndices)[2] < 0
        || (*vertexIndices)[0] >= sTerrainNumVertices || (*vertexIndices)[1] >= sTerrainNumVertices
        || (*vertexIndices)[2] >= sTerrainNumVertices) {
        if (!(sSurfaceLoadWarnFlags & SURF_WARN_BAD_INDEX)) {
            DBG("bad tri index %d,%d,%d verts=%d\n",
                (int)(*vertexIndices)[0], (int)(*vertexIndices)[1], (int)(*vertexIndices)[2],
                sTerrainNumVertices);
            sSurfaceLoadWarnFlags |= SURF_WARN_BAD_INDEX;
        }
        return NULL;
    }

    offset1 = 3 * (*vertexIndices)[0];
    offset2 = 3 * (*vertexIndices)[1];
    offset3 = 3 * (*vertexIndices)[2];

    x1 = *(vertexData + offset1 + 0);
    y1 = *(vertexData + offset1 + 1);
    z1 = *(vertexData + offset1 + 2);

    x2 = *(vertexData + offset2 + 0);
    y2 = *(vertexData + offset2 + 1);
    z2 = *(vertexData + offset2 + 2);

    x3 = *(vertexData + offset3 + 0);
    y3 = *(vertexData + offset3 + 1);
    z3 = *(vertexData + offset3 + 2);

    nx = (y2 - y1) * (z3 - z2) - (z2 - z1) * (y3 - y2);
    ny = (z2 - z1) * (x3 - x2) - (x2 - x1) * (z3 - z2);
    nz = (x2 - x1) * (y3 - y2) - (y2 - y1) * (x3 - x2);
    mag = sqrtf(nx * nx + ny * ny + nz * nz);

    minY = y1;
    if (y2 < minY) {
        minY = y2;
    }
    if (y3 < minY) {
        minY = y3;
    }

    maxY = y1;
    if (y2 > maxY) {
        maxY = y2;
    }
    if (y3 > maxY) {
        maxY = y3;
    }

    if (mag < 0.0001) {
        return NULL;
    }
    mag = (f32)(1.0 / mag);
    nx *= mag;
    ny *= mag;
    nz *= mag;

    surface = alloc_surface();
    if (surface == NULL) {
        return NULL;
    }

    surface->vertex1[0] = x1;
    surface->vertex2[0] = x2;
    surface->vertex3[0] = x3;

    surface->vertex1[1] = y1;
    surface->vertex2[1] = y2;
    surface->vertex3[1] = y3;

    surface->vertex1[2] = z1;
    surface->vertex2[2] = z2;
    surface->vertex3[2] = z3;

    surface->normal.x = nx;
    surface->normal.y = ny;
    surface->normal.z = nz;

    surface->originOffset = -(nx * x1 + ny * y1 + nz * z1);

    surface->lowerY = minY - 5;
    surface->upperY = maxY + 5;

    return surface;
}

static s32 surface_has_force(TerrainData surfaceType) {
    s32 hasForce = FALSE;

    switch (surfaceType) {
        case SURFACE_0004:
        case SURFACE_FLOWING_WATER:
        case SURFACE_DEEP_MOVING_QUICKSAND:
        case SURFACE_SHALLOW_MOVING_QUICKSAND:
        case SURFACE_MOVING_QUICKSAND:
        case SURFACE_HORIZONTAL_WIND:
        case SURFACE_INSTANT_MOVING_QUICKSAND:
            hasForce = TRUE;
            break;

        default:
            break;
    }

    return hasForce;
}

static s32 surf_has_no_cam_collision(TerrainData surfaceType) {
    s32 flags = 0;

    switch (surfaceType) {
        case SURFACE_NO_CAM_COLLISION:
        case SURFACE_NO_CAM_COLLISION_77:
        case SURFACE_NO_CAM_COL_VERY_SLIPPERY:
        case SURFACE_SWITCH:
            flags = SURFACE_FLAG_NO_CAM_COLLISION;
            break;

        default:
            break;
    }

    return flags;
}

static void load_static_surfaces(TerrainData **data, TerrainData *vertexData, TerrainData surfaceType, RoomData **surfaceRooms) {
    s32 i;
    s32 numSurfaces;
    struct Surface *surface;
    RoomData room = 0;
    s16 hasForce = surface_has_force(surfaceType);
    s16 flags = surf_has_no_cam_collision(surfaceType);

    numSurfaces = *(*data);
    (*data)++;

    for (i = 0; i < numSurfaces; i++) {
        if (*surfaceRooms != NULL) {
            room = *(*surfaceRooms);
            (*surfaceRooms)++;
        }

        surface = read_surface_data(vertexData, data);
        if (surface != NULL) {
            surface->room = room;
            surface->type = surfaceType;
            surface->flags = (s8) flags;

            if (hasForce) {
                surface->force = *(*data + 3);
            } else {
                surface->force = 0;
            }

            add_surface(surface, FALSE);
        }

        *data += 3;
        if (hasForce) {
            (*data)++;
        }
    }
}

static TerrainData *read_vertex_data(TerrainData **data) {
    s32 numVertices;
    UNUSED u8 filler[16];
    TerrainData *vertexData;

    numVertices = *(*data);
    (*data)++;

    vertexData = *data;
    *data += 3 * numVertices;
    sTerrainNumVertices = numVertices;

    return vertexData;
}

static void load_environmental_regions(TerrainData **data) {
    s32 numRegions;
    s32 i;

    gEnvironmentRegions = *data;
    numRegions = *(*data)++;

    if (numRegions > 20) {
        CN_DEBUG_PRINTF(("Error Water Over\n"));
    }

    for (i = 0; i < numRegions; i++) {
        UNUSED TerrainData val, loX, loZ, hiX, hiZ;
        TerrainData height;

        val = *(*data)++;

        loX = *(*data)++;
        hiX = *(*data)++;
        loZ = *(*data)++;
        hiZ = *(*data)++;

        height = *(*data)++;

        gEnvironmentLevels[i] = height;
    }
}

void alloc_surface_pools(void) {
#ifdef TARGET_NDS
    if (sNdsSurfacePoolHeap == NULL || sNdsSurfaceNodePoolHeap == NULL) {
        sNdsSurfaceNodePoolHeap = malloc((u32) NDS_SURFACE_NODE_POOL_SIZE * sizeof(struct SurfaceNode));
        sNdsSurfacePoolHeap = malloc((u32) NDS_SURFACE_POOL_SIZE * sizeof(struct Surface));
        if (sNdsSurfaceNodePoolHeap == NULL || sNdsSurfacePoolHeap == NULL) {
            if (sNdsSurfaceNodePoolHeap != NULL) {
                free(sNdsSurfaceNodePoolHeap);
                sNdsSurfaceNodePoolHeap = NULL;
            }
            if (sNdsSurfacePoolHeap != NULL) {
                free(sNdsSurfacePoolHeap);
                sNdsSurfacePoolHeap = NULL;
            }
        }
    }

    if (sNdsSurfacePoolHeap != NULL && sNdsSurfaceNodePoolHeap != NULL) {
        sSurfacePool = sNdsSurfacePoolHeap;
        sSurfaceNodePool = sNdsSurfaceNodePoolHeap;
        sSurfacePoolSize = NDS_SURFACE_POOL_SIZE;
        sSurfaceNodePoolSize = NDS_SURFACE_NODE_POOL_SIZE;
        DBG("surf_pools: heap nodes=%p surfs=%p\n", (void *)sSurfaceNodePool, (void *)sSurfacePool);
        DBG("surf_caps: nodes=%d surfs=%d avail=%u\n",
            sSurfaceNodePoolSize, sSurfacePoolSize, (unsigned)main_pool_available());
        sSurfaceLoadWarnFlags = 0;
        gCCMEnteredSlide = 0;
        reset_red_coins_collected();
        return;
    }
#endif

    s32 surfaces = 2300;
    s32 nodes = 7000;
    const s32 minSurfaces = 64;
    const u32 reserve = 0x1000;
    const u32 allocOverhead = 32;
    u32 avail = main_pool_available();
    u32 usable = (avail > reserve) ? (avail - reserve) : avail;

    while (surfaces >= minSurfaces) {
        u32 needed;

        nodes = (surfaces * 7000 + 2299) / 2300;
        needed = (u32) surfaces * sizeof(struct Surface) + (u32) nodes * sizeof(struct SurfaceNode) + allocOverhead;
        if (needed <= usable) {
            break;
        }
        surfaces -= 16;
    }

    if (surfaces < minSurfaces) {
        surfaces = minSurfaces;
    }

    sSurfacePool = NULL;
    sSurfaceNodePool = NULL;
    while (surfaces >= minSurfaces && (sSurfacePool == NULL || sSurfaceNodePool == NULL)) {
        nodes = (surfaces * 7000 + 2299) / 2300;
        sSurfaceNodePool = main_pool_alloc((u32) nodes * sizeof(struct SurfaceNode), MEMORY_POOL_LEFT);
        if (sSurfaceNodePool != NULL) {
            sSurfacePool = main_pool_alloc((u32) surfaces * sizeof(struct Surface), MEMORY_POOL_LEFT);
            if (sSurfacePool == NULL) {
                main_pool_free(sSurfaceNodePool);
                sSurfaceNodePool = NULL;
            }
        }
        if (sSurfacePool == NULL || sSurfaceNodePool == NULL) {
            surfaces -= 16;
        }
    }

    if (sSurfacePool == NULL || sSurfaceNodePool == NULL) {
        sSurfacePoolSize = 0;
        sSurfaceNodePoolSize = 0;
    } else {
        sSurfacePoolSize = surfaces;
        sSurfaceNodePoolSize = nodes;
    }

    DBG("surf_pools: nodes=%p surfs=%p\n", (void *)sSurfaceNodePool, (void *)sSurfacePool);
    DBG("surf_caps: nodes=%d surfs=%d avail=%u\n",
        sSurfaceNodePoolSize, sSurfacePoolSize, (unsigned)main_pool_available());

    sSurfaceLoadWarnFlags = 0;

    gCCMEnteredSlide = 0;
    reset_red_coins_collected();
}

#ifdef NO_SEGMENTED_MEMORY

u32 get_area_terrain_size(TerrainData *data) {
    TerrainData *startPos = data;
    s32 end = FALSE;
    TerrainData terrainLoadType;
    s32 numVertices;
    s32 numRegions;
    s32 numSurfaces;
    TerrainData hasForce;

    while (!end) {
        terrainLoadType = *data++;

        switch (terrainLoadType) {
            case TERRAIN_LOAD_VERTICES:
                numVertices = *data++;
                data += 3 * numVertices;
                break;

            case TERRAIN_LOAD_OBJECTS:
                data += get_special_objects_size(data);
                break;

            case TERRAIN_LOAD_ENVIRONMENT:
                numRegions = *data++;
                data += 6 * numRegions;
                break;

            case TERRAIN_LOAD_CONTINUE:
                continue;

            case TERRAIN_LOAD_END:
                end = TRUE;
                break;

            default:
                numSurfaces = *data++;
                hasForce = surface_has_force(terrainLoadType);
                data += (3 + hasForce) * numSurfaces;
                break;
        }
    }

    return data - startPos;
}
#endif

void load_area_terrain(s16 index, TerrainData *data, RoomData *surfaceRooms, s16 *macroObjects) {
    TerrainData terrainLoadType;
    TerrainData *vertexData = NULL;
    UNUSED u8 filler[4];
    UNUSED int loopIter = 0;

    gEnvironmentRegions = NULL;
    unused8038BE90 = 0;
    gSurfaceNodesAllocated = 0;
    gSurfacesAllocated = 0;
    sTerrainNumVertices = 0;
    sSurfaceLoadWarnFlags = 0;

    DBG("lat: clear\n");
    clear_static_surfaces();

    DBG("lat: loop start data=%p\n", (void *)data);
    while (TRUE) {
        terrainLoadType = *data;
        data++;
        DBG("lat[%d] t=0x%x\n", loopIter, (unsigned)(u16)terrainLoadType);

        if (TERRAIN_LOAD_IS_SURFACE_TYPE_LOW(terrainLoadType)) {
            DBG("lat[%d] surf_lo cnt=%d\n", loopIter, (int)*data);
            load_static_surfaces(&data, vertexData, terrainLoadType, &surfaceRooms);
            DBG("lat[%d] surf_lo done s=%d n=%d\n", loopIter, gSurfacesAllocated, gSurfaceNodesAllocated);
        } else if (terrainLoadType == TERRAIN_LOAD_VERTICES) {
            DBG("lat[%d] verts cnt=%d\n", loopIter, (int)*data);
            vertexData = read_vertex_data(&data);
            DBG("lat[%d] verts done p=%p\n", loopIter, (void *)vertexData);
        } else if (terrainLoadType == TERRAIN_LOAD_OBJECTS) {
            DBG("lat[%d] objs cnt=%d\n", loopIter, (int)*data);
            spawn_special_objects(index, &data);
            DBG("lat[%d] objs done\n", loopIter);
        } else if (terrainLoadType == TERRAIN_LOAD_ENVIRONMENT) {
            DBG("lat[%d] env cnt=%d\n", loopIter, (int)*data);
            load_environmental_regions(&data);
            DBG("lat[%d] env done\n", loopIter);
        } else if (terrainLoadType == TERRAIN_LOAD_CONTINUE) {
            loopIter++;
            continue;
        } else if (terrainLoadType == TERRAIN_LOAD_END) {
            DBG("lat: end\n");
            break;
        } else if (TERRAIN_LOAD_IS_SURFACE_TYPE_HIGH(terrainLoadType)) {
            DBG("lat[%d] surf_hi cnt=%d\n", loopIter, (int)*data);
            load_static_surfaces(&data, vertexData, terrainLoadType, &surfaceRooms);
            DBG("lat[%d] surf_hi done s=%d n=%d\n", loopIter, gSurfacesAllocated, gSurfaceNodesAllocated);
        } else {
            DBG("lat[%d] ERROR!\n", loopIter);
            CN_DEBUG_PRINTF((" BGCode Error \n"));
        }
        loopIter++;
    }

    DBG("lat: macros p=%p\n", (void *)macroObjects);
    if (macroObjects != NULL && *macroObjects != -1) {

        if (0 <= *macroObjects && *macroObjects < 30) {
            spawn_macro_objects_hardcoded(index, macroObjects);
        }

        else {
            spawn_macro_objects(index, macroObjects);
        }
    }
    DBG("lat: done\n");

    gNumStaticSurfaceNodes = gSurfaceNodesAllocated;
    gNumStaticSurfaces = gSurfacesAllocated;
}

void clear_dynamic_surfaces(void) {
    if (!(gTimeStopState & TIME_STOP_ACTIVE)) {
        gSurfacesAllocated = gNumStaticSurfaces;
        gSurfaceNodesAllocated = gNumStaticSurfaceNodes;

        clear_spatial_partition(&gDynamicSurfacePartition[0][0]);
    }
}

UNUSED static void unused_80383604(void) {
}

void transform_object_vertices(TerrainData **data, TerrainData *vertexData) {
    register TerrainData *vertices;
    register f32 vx, vy, vz;
    register s32 numVertices;

    Mat4 *objectTransform;
    Mat4 m;

    objectTransform = &gCurrentObject->transform;

    numVertices = *(*data);
    (*data)++;

    vertices = *data;

    if (gCurrentObject->header.gfx.throwMatrix == NULL) {
        gCurrentObject->header.gfx.throwMatrix = objectTransform;
        obj_build_transform_from_pos_and_angle(gCurrentObject, O_POS_INDEX, O_FACE_ANGLE_INDEX);
    }

    obj_apply_scale_to_matrix(gCurrentObject, m, *objectTransform);

    while (numVertices--) {
        vx = *(vertices++);
        vy = *(vertices++);
        vz = *(vertices++);

        *vertexData++ = (TerrainData)(vx * m[0][0] + vy * m[1][0] + vz * m[2][0] + m[3][0]);
        *vertexData++ = (TerrainData)(vx * m[0][1] + vy * m[1][1] + vz * m[2][1] + m[3][1]);
        *vertexData++ = (TerrainData)(vx * m[0][2] + vy * m[1][2] + vz * m[2][2] + m[3][2]);
    }

    *data = vertices;
}

void load_object_surfaces(TerrainData **data, TerrainData *vertexData) {
    s32 surfaceType;
    s32 i;
    s32 numSurfaces;
    TerrainData hasForce;
    TerrainData flags;
    s16 room;

    surfaceType = *(*data);
    (*data)++;

    numSurfaces = *(*data);
    (*data)++;

    hasForce = surface_has_force(surfaceType);

    flags = surf_has_no_cam_collision(surfaceType);
    flags |= SURFACE_FLAG_DYNAMIC;

    if (gCurrentObject->behavior == segmented_to_virtual(bhvDDDWarp)) {
        room = 5;
    } else {
        room = 0;
    }

    for (i = 0; i < numSurfaces; i++) {
        struct Surface *surface = read_surface_data(vertexData, data);

        if (surface != NULL) {
            surface->object = gCurrentObject;
            surface->type = surfaceType;

            if (hasForce) {
                surface->force = *(*data + 3);
            } else {
                surface->force = 0;
            }

            surface->flags |= flags;
            surface->room = (s8) room;
            add_surface(surface, TRUE);
        }

        if (hasForce) {
            *data += 4;
        } else {
            *data += 3;
        }
    }
}

void load_object_collision_model(void) {
    UNUSED u8 filler[4];
    TerrainData vertexData[600];

    TerrainData *collisionData = gCurrentObject->collisionData;
    f32 marioDist = gCurrentObject->oDistanceToMario;
    f32 tangibleDist = gCurrentObject->oCollisionDistance;

    if (gCurrentObject->oDistanceToMario == 19000.0f) {
        marioDist = dist_between_objects(gCurrentObject, gMarioObject);
    }

    if (gCurrentObject->oCollisionDistance > 4000.0f) {
        gCurrentObject->oDrawingDistance = gCurrentObject->oCollisionDistance;
    }

    if (!(gTimeStopState & TIME_STOP_ACTIVE) && marioDist < tangibleDist
        && !(gCurrentObject->activeFlags & ACTIVE_FLAG_IN_DIFFERENT_ROOM)) {
        collisionData++;
        transform_object_vertices(&collisionData, vertexData);

        while (*collisionData != TERRAIN_LOAD_CONTINUE) {
            load_object_surfaces(&collisionData, vertexData);
        }
    }

    if (marioDist < gCurrentObject->oDrawingDistance) {
        gCurrentObject->header.gfx.node.flags |= GRAPH_RENDER_ACTIVE;
    } else {
        gCurrentObject->header.gfx.node.flags &= ~GRAPH_RENDER_ACTIVE;
    }
}
