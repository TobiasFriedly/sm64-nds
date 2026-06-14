#include <ultra64.h>

#include "sm64.h"
#include "engine/graph_node.h"
#include "math_util.h"
#include "surface_collision.h"

#include "trig_tables.inc.c"

#ifdef TARGET_NDS
#define NDS_ITCM_CODE __attribute__((section(".itcm")))
#else
#define NDS_ITCM_CODE
#endif

Vec4s *gSplineKeyframe;
float gSplineKeyframeFraction;
int gSplineState;

#pragma GCC diagnostic push

#ifdef __GNUC__
#if defined(__clang__)
  #pragma GCC diagnostic ignored "-Wreturn-stack-address"
#else
  #pragma GCC diagnostic ignored "-Wreturn-local-addr"
#endif
#endif

NDS_ITCM_CODE void *vec3f_copy(Vec3f dest, Vec3f src) {
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
    return &dest;
}

NDS_ITCM_CODE void *vec3f_set(Vec3f dest, f32 x, f32 y, f32 z) {
    dest[0] = x;
    dest[1] = y;
    dest[2] = z;
    return &dest;
}

NDS_ITCM_CODE void *vec3f_add(Vec3f dest, Vec3f a) {
    dest[0] += a[0];
    dest[1] += a[1];
    dest[2] += a[2];
    return &dest;
}

NDS_ITCM_CODE void *vec3f_sum(Vec3f dest, Vec3f a, Vec3f b) {
    dest[0] = a[0] + b[0];
    dest[1] = a[1] + b[1];
    dest[2] = a[2] + b[2];
    return &dest;
}

NDS_ITCM_CODE void *vec3s_copy(Vec3s dest, Vec3s src) {
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
    return &dest;
}

NDS_ITCM_CODE void *vec3s_set(Vec3s dest, s16 x, s16 y, s16 z) {
    dest[0] = x;
    dest[1] = y;
    dest[2] = z;
    return &dest;
}

NDS_ITCM_CODE void *vec3s_add(Vec3s dest, Vec3s a) {
    dest[0] += a[0];
    dest[1] += a[1];
    dest[2] += a[2];
    return &dest;
}

NDS_ITCM_CODE void *vec3s_sum(Vec3s dest, Vec3s a, Vec3s b) {
    dest[0] = a[0] + b[0];
    dest[1] = a[1] + b[1];
    dest[2] = a[2] + b[2];
    return &dest;
}

NDS_ITCM_CODE void *vec3s_sub(Vec3s dest, Vec3s a) {
    dest[0] -= a[0];
    dest[1] -= a[1];
    dest[2] -= a[2];
    return &dest;
}

NDS_ITCM_CODE void *vec3s_to_vec3f(Vec3f dest, Vec3s a) {
    dest[0] = a[0];
    dest[1] = a[1];
    dest[2] = a[2];
    return &dest;
}

NDS_ITCM_CODE void *vec3f_to_vec3s(Vec3s dest, Vec3f a) {

    dest[0] = a[0] + ((a[0] > 0) ? 0.5f : -0.5f);
    dest[1] = a[1] + ((a[1] > 0) ? 0.5f : -0.5f);
    dest[2] = a[2] + ((a[2] > 0) ? 0.5f : -0.5f);
    return &dest;
}

NDS_ITCM_CODE void *find_vector_perpendicular_to_plane(Vec3f dest, Vec3f a, Vec3f b, Vec3f c) {
    dest[0] = (b[1] - a[1]) * (c[2] - b[2]) - (c[1] - b[1]) * (b[2] - a[2]);
    dest[1] = (b[2] - a[2]) * (c[0] - b[0]) - (c[2] - b[2]) * (b[0] - a[0]);
    dest[2] = (b[0] - a[0]) * (c[1] - b[1]) - (c[0] - b[0]) * (b[1] - a[1]);
    return &dest;
}

NDS_ITCM_CODE void *vec3f_cross(Vec3f dest, Vec3f a, Vec3f b) {
    dest[0] = a[1] * b[2] - b[1] * a[2];
    dest[1] = a[2] * b[0] - b[2] * a[0];
    dest[2] = a[0] * b[1] - b[0] * a[1];
    return &dest;
}

NDS_ITCM_CODE void *vec3f_normalize(Vec3f dest) {

    f32 invsqrt = 1.0f / sqrtf(dest[0] * dest[0] + dest[1] * dest[1] + dest[2] * dest[2]);

    dest[0] *= invsqrt;
    dest[1] *= invsqrt;
    dest[2] *= invsqrt;
    return &dest;
}

#pragma GCC diagnostic pop

NDS_ITCM_CODE void mtxf_copy(Mat4 dest, Mat4 src) {
    register s32 i;
    register u32 *d = (u32 *) dest;
    register u32 *s = (u32 *) src;

    for (i = 0; i < 16; i++) {
        *d++ = *s++;
    }
}

NDS_ITCM_CODE void mtxf_identity(Mat4 mtx) {
    register s32 i;
    register f32 *dest;

    for (dest = (f32 *) mtx + 1, i = 0; i < 14; dest++, i++) *dest = 0;

    for (dest = (f32 *) mtx, i = 0; i < 4; dest += 5, i++) *dest = 1;
}

NDS_ITCM_CODE void mtxf_translate(Mat4 dest, Vec3f b) {
    mtxf_identity(dest);
    dest[3][0] = b[0];
    dest[3][1] = b[1];
    dest[3][2] = b[2];
}

NDS_ITCM_CODE void mtxf_lookat(Mat4 mtx, Vec3f from, Vec3f to, s16 roll) {
    register f32 invLength;
    f32 dx;
    f32 dz;
    f32 xColY;
    f32 yColY;
    f32 zColY;
    f32 xColZ;
    f32 yColZ;
    f32 zColZ;
    f32 xColX;
    f32 yColX;
    f32 zColX;

    dx = to[0] - from[0];
    dz = to[2] - from[2];

    invLength = -1.0 / sqrtf(dx * dx + dz * dz);
    dx *= invLength;
    dz *= invLength;

    yColY = coss(roll);
    xColY = sins(roll) * dz;
    zColY = -sins(roll) * dx;

    xColZ = to[0] - from[0];
    yColZ = to[1] - from[1];
    zColZ = to[2] - from[2];

    invLength = -1.0 / sqrtf(xColZ * xColZ + yColZ * yColZ + zColZ * zColZ);
    xColZ *= invLength;
    yColZ *= invLength;
    zColZ *= invLength;

    xColX = yColY * zColZ - zColY * yColZ;
    yColX = zColY * xColZ - xColY * zColZ;
    zColX = xColY * yColZ - yColY * xColZ;

    invLength = 1.0 / sqrtf(xColX * xColX + yColX * yColX + zColX * zColX);

    xColX *= invLength;
    yColX *= invLength;
    zColX *= invLength;

    xColY = yColZ * zColX - zColZ * yColX;
    yColY = zColZ * xColX - xColZ * zColX;
    zColY = xColZ * yColX - yColZ * xColX;

    invLength = 1.0 / sqrtf(xColY * xColY + yColY * yColY + zColY * zColY);
    xColY *= invLength;
    yColY *= invLength;
    zColY *= invLength;

    mtx[0][0] = xColX;
    mtx[1][0] = yColX;
    mtx[2][0] = zColX;
    mtx[3][0] = -(from[0] * xColX + from[1] * yColX + from[2] * zColX);

    mtx[0][1] = xColY;
    mtx[1][1] = yColY;
    mtx[2][1] = zColY;
    mtx[3][1] = -(from[0] * xColY + from[1] * yColY + from[2] * zColY);

    mtx[0][2] = xColZ;
    mtx[1][2] = yColZ;
    mtx[2][2] = zColZ;
    mtx[3][2] = -(from[0] * xColZ + from[1] * yColZ + from[2] * zColZ);

    mtx[0][3] = 0;
    mtx[1][3] = 0;
    mtx[2][3] = 0;
    mtx[3][3] = 1;
}

NDS_ITCM_CODE void mtxf_rotate_zxy_and_translate(Mat4 dest, Vec3f translate, Vec3s rotate) {
    register f32 sx = sins(rotate[0]);
    register f32 cx = coss(rotate[0]);

    register f32 sy = sins(rotate[1]);
    register f32 cy = coss(rotate[1]);

    register f32 sz = sins(rotate[2]);
    register f32 cz = coss(rotate[2]);

    dest[0][0] = cy * cz + sx * sy * sz;
    dest[1][0] = -cy * sz + sx * sy * cz;
    dest[2][0] = cx * sy;
    dest[3][0] = translate[0];

    dest[0][1] = cx * sz;
    dest[1][1] = cx * cz;
    dest[2][1] = -sx;
    dest[3][1] = translate[1];

    dest[0][2] = -sy * cz + sx * cy * sz;
    dest[1][2] = sy * sz + sx * cy * cz;
    dest[2][2] = cx * cy;
    dest[3][2] = translate[2];

    dest[0][3] = dest[1][3] = dest[2][3] = 0.0f;
    dest[3][3] = 1.0f;
}

NDS_ITCM_CODE void mtxf_rotate_xyz_and_translate(Mat4 dest, Vec3f b, Vec3s c) {
    register f32 sx = sins(c[0]);
    register f32 cx = coss(c[0]);

    register f32 sy = sins(c[1]);
    register f32 cy = coss(c[1]);

    register f32 sz = sins(c[2]);
    register f32 cz = coss(c[2]);

    dest[0][0] = cy * cz;
    dest[0][1] = cy * sz;
    dest[0][2] = -sy;
    dest[0][3] = 0;

    dest[1][0] = sx * sy * cz - cx * sz;
    dest[1][1] = sx * sy * sz + cx * cz;
    dest[1][2] = sx * cy;
    dest[1][3] = 0;

    dest[2][0] = cx * sy * cz + sx * sz;
    dest[2][1] = cx * sy * sz - sx * cz;
    dest[2][2] = cx * cy;
    dest[2][3] = 0;

    dest[3][0] = b[0];
    dest[3][1] = b[1];
    dest[3][2] = b[2];
    dest[3][3] = 1;
}

NDS_ITCM_CODE void mtxf_billboard(Mat4 dest, Mat4 mtx, Vec3f position, s16 angle) {
    dest[0][0] = coss(angle);
    dest[0][1] = sins(angle);
    dest[0][2] = 0;
    dest[0][3] = 0;

    dest[1][0] = -dest[0][1];
    dest[1][1] = dest[0][0];
    dest[1][2] = 0;
    dest[1][3] = 0;

    dest[2][0] = 0;
    dest[2][1] = 0;
    dest[2][2] = 1;
    dest[2][3] = 0;

    dest[3][0] =
        mtx[0][0] * position[0] + mtx[1][0] * position[1] + mtx[2][0] * position[2] + mtx[3][0];
    dest[3][1] =
        mtx[0][1] * position[0] + mtx[1][1] * position[1] + mtx[2][1] * position[2] + mtx[3][1];
    dest[3][2] =
        mtx[0][2] * position[0] + mtx[1][2] * position[1] + mtx[2][2] * position[2] + mtx[3][2];
    dest[3][3] = 1;
}

NDS_ITCM_CODE void mtxf_align_terrain_normal(Mat4 dest, Vec3f upDir, Vec3f pos, s16 yaw) {
    Vec3f lateralDir;
    Vec3f leftDir;
    Vec3f forwardDir;

    vec3f_set(lateralDir, sins(yaw), 0, coss(yaw));
    vec3f_normalize(upDir);

    vec3f_cross(leftDir, upDir, lateralDir);
    vec3f_normalize(leftDir);

    vec3f_cross(forwardDir, leftDir, upDir);
    vec3f_normalize(forwardDir);

    dest[0][0] = leftDir[0];
    dest[0][1] = leftDir[1];
    dest[0][2] = leftDir[2];
    dest[3][0] = pos[0];

    dest[1][0] = upDir[0];
    dest[1][1] = upDir[1];
    dest[1][2] = upDir[2];
    dest[3][1] = pos[1];

    dest[2][0] = forwardDir[0];
    dest[2][1] = forwardDir[1];
    dest[2][2] = forwardDir[2];
    dest[3][2] = pos[2];

    dest[0][3] = 0.0f;
    dest[1][3] = 0.0f;
    dest[2][3] = 0.0f;
    dest[3][3] = 1.0f;
}

NDS_ITCM_CODE void mtxf_align_terrain_triangle(Mat4 mtx, Vec3f pos, s16 yaw, f32 radius) {
    struct Surface *sp74;
    Vec3f point0;
    Vec3f point1;
    Vec3f point2;
    Vec3f forward;
    Vec3f xColumn;
    Vec3f yColumn;
    Vec3f zColumn;
    f32 avgY;
    f32 minY = -radius * 3;

    point0[0] = pos[0] + radius * sins(yaw + 0x2AAA);
    point0[2] = pos[2] + radius * coss(yaw + 0x2AAA);
    point1[0] = pos[0] + radius * sins(yaw + 0x8000);
    point1[2] = pos[2] + radius * coss(yaw + 0x8000);
    point2[0] = pos[0] + radius * sins(yaw + 0xD555);
    point2[2] = pos[2] + radius * coss(yaw + 0xD555);

    point0[1] = find_floor(point0[0], pos[1] + 150, point0[2], &sp74);
    point1[1] = find_floor(point1[0], pos[1] + 150, point1[2], &sp74);
    point2[1] = find_floor(point2[0], pos[1] + 150, point2[2], &sp74);

    if (point0[1] - pos[1] < minY) {
        point0[1] = pos[1];
    }

    if (point1[1] - pos[1] < minY) {
        point1[1] = pos[1];
    }

    if (point2[1] - pos[1] < minY) {
        point2[1] = pos[1];
    }

    avgY = (point0[1] + point1[1] + point2[1]) / 3;

    vec3f_set(forward, sins(yaw), 0, coss(yaw));
    find_vector_perpendicular_to_plane(yColumn, point0, point1, point2);
    vec3f_normalize(yColumn);
    vec3f_cross(xColumn, yColumn, forward);
    vec3f_normalize(xColumn);
    vec3f_cross(zColumn, xColumn, yColumn);
    vec3f_normalize(zColumn);

    mtx[0][0] = xColumn[0];
    mtx[0][1] = xColumn[1];
    mtx[0][2] = xColumn[2];
    mtx[3][0] = pos[0];

    mtx[1][0] = yColumn[0];
    mtx[1][1] = yColumn[1];
    mtx[1][2] = yColumn[2];
    mtx[3][1] = (avgY < pos[1]) ? pos[1] : avgY;

    mtx[2][0] = zColumn[0];
    mtx[2][1] = zColumn[1];
    mtx[2][2] = zColumn[2];
    mtx[3][2] = pos[2];

    mtx[0][3] = 0;
    mtx[1][3] = 0;
    mtx[2][3] = 0;
    mtx[3][3] = 1;
}

NDS_ITCM_CODE void mtxf_mul(Mat4 dest, Mat4 a, Mat4 b) {
    Mat4 temp;
    register f32 entry0;
    register f32 entry1;
    register f32 entry2;

    entry0 = a[0][0];
    entry1 = a[0][1];
    entry2 = a[0][2];
    temp[0][0] = entry0 * b[0][0] + entry1 * b[1][0] + entry2 * b[2][0];
    temp[0][1] = entry0 * b[0][1] + entry1 * b[1][1] + entry2 * b[2][1];
    temp[0][2] = entry0 * b[0][2] + entry1 * b[1][2] + entry2 * b[2][2];

    entry0 = a[1][0];
    entry1 = a[1][1];
    entry2 = a[1][2];
    temp[1][0] = entry0 * b[0][0] + entry1 * b[1][0] + entry2 * b[2][0];
    temp[1][1] = entry0 * b[0][1] + entry1 * b[1][1] + entry2 * b[2][1];
    temp[1][2] = entry0 * b[0][2] + entry1 * b[1][2] + entry2 * b[2][2];

    entry0 = a[2][0];
    entry1 = a[2][1];
    entry2 = a[2][2];
    temp[2][0] = entry0 * b[0][0] + entry1 * b[1][0] + entry2 * b[2][0];
    temp[2][1] = entry0 * b[0][1] + entry1 * b[1][1] + entry2 * b[2][1];
    temp[2][2] = entry0 * b[0][2] + entry1 * b[1][2] + entry2 * b[2][2];

    entry0 = a[3][0];
    entry1 = a[3][1];
    entry2 = a[3][2];
    temp[3][0] = entry0 * b[0][0] + entry1 * b[1][0] + entry2 * b[2][0] + b[3][0];
    temp[3][1] = entry0 * b[0][1] + entry1 * b[1][1] + entry2 * b[2][1] + b[3][1];
    temp[3][2] = entry0 * b[0][2] + entry1 * b[1][2] + entry2 * b[2][2] + b[3][2];

    temp[0][3] = temp[1][3] = temp[2][3] = 0;
    temp[3][3] = 1;

    mtxf_copy(dest, temp);
}

NDS_ITCM_CODE void mtxf_mul_to_fixed(Mtx *dest, Mat4 floatDest, Mat4 a, Mat4 b) {
    s32 af[4][3];
    s32 bf[4][3];
    s32 fx[4][4];
    s32 i, j;
    s32 *m1, *m2;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            af[i][j] = (s32) (a[i][j] * 65536.0f);
            bf[i][j] = (s32) (b[i][j] * 65536.0f);
        }
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            s64 acc = (s64) af[i][0] * bf[0][j]
                    + (s64) af[i][1] * bf[1][j]
                    + (s64) af[i][2] * bf[2][j];
            if (i == 3) {
                acc += (s64) bf[3][j] << 16;
            }
            fx[i][j] = (s32) ((acc + 0x8000) >> 16);
        }
        fx[i][3] = 0;
    }
    fx[3][3] = 1 << 16;

    m1 = &dest->m[0][0];
    m2 = &dest->m[2][0];
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 2; j++) {
            s32 t1 = fx[i][2 * j];
            s32 t2 = fx[i][2 * j + 1];
            *m1++ = (t1 & 0xffff0000) | ((t2 >> 16) & 0xffff);
            *m2++ = ((t1 << 16) & 0xffff0000) | (t2 & 0xffff);
        }
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            floatDest[i][j] = fx[i][j] * (1.0f / 65536.0f);
        }
    }
}

#define FIXED_MUL16(x, y) ((s32) ((((s64) (x) * (y)) + 0x8000) >> 16))

static inline s32 mtx_get_fixed(const Mtx *m, s32 r, s32 j) {
    const s32 *p = &m->m[0][0];
    s32 c = j >> 1;
    s32 iw = p[2 * r + c];
    s32 fw = p[8 + 2 * r + c];
    s16 ih;
    u16 fh;
    if (j & 1) {
        ih = (s16) (iw & 0xffff);
        fh = (u16) (fw & 0xffff);
    } else {
        ih = (s16) (iw >> 16);
        fh = (u16) (fw >> 16);
    }
    return ((s32) ih << 16) | fh;
}

NDS_ITCM_CODE void mtxf_mul_fixed_to_fixed(Mtx *dest, Mat4 floatDest, s32 aFixed[4][4], const Mtx *bPacked) {
    s32 b[4][3];
    s32 fx[4][4];
    s32 i, j;
    s32 *m1, *m2;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            b[i][j] = mtx_get_fixed(bPacked, i, j);
        }
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            s64 acc = (s64) aFixed[i][0] * b[0][j]
                    + (s64) aFixed[i][1] * b[1][j]
                    + (s64) aFixed[i][2] * b[2][j];
            if (i == 3) {
                acc += (s64) b[3][j] << 16;
            }
            fx[i][j] = (s32) ((acc + 0x8000) >> 16);
        }
        fx[i][3] = 0;
    }
    fx[3][3] = 1 << 16;

    m1 = &dest->m[0][0];
    m2 = &dest->m[2][0];
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 2; j++) {
            s32 t1 = fx[i][2 * j];
            s32 t2 = fx[i][2 * j + 1];
            *m1++ = (t1 & 0xffff0000) | ((t2 >> 16) & 0xffff);
            *m2++ = ((t1 << 16) & 0xffff0000) | (t2 & 0xffff);
        }
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            floatDest[i][j] = fx[i][j] * (1.0f / 65536.0f);
        }
    }
}

NDS_ITCM_CODE void mtxf_rotate_zxy_and_translate_fixed(s32 dest[4][4], Vec3f translate, Vec3s rotate) {
    s32 sx = (s32) (sins(rotate[0]) * 65536.0f);
    s32 cx = (s32) (coss(rotate[0]) * 65536.0f);
    s32 sy = (s32) (sins(rotate[1]) * 65536.0f);
    s32 cy = (s32) (coss(rotate[1]) * 65536.0f);
    s32 sz = (s32) (sins(rotate[2]) * 65536.0f);
    s32 cz = (s32) (coss(rotate[2]) * 65536.0f);

    dest[0][0] = FIXED_MUL16(cy, cz) + FIXED_MUL16(FIXED_MUL16(sx, sy), sz);
    dest[1][0] = -FIXED_MUL16(cy, sz) + FIXED_MUL16(FIXED_MUL16(sx, sy), cz);
    dest[2][0] = FIXED_MUL16(cx, sy);
    dest[3][0] = (s32) (translate[0] * 65536.0f);

    dest[0][1] = FIXED_MUL16(cx, sz);
    dest[1][1] = FIXED_MUL16(cx, cz);
    dest[2][1] = -sx;
    dest[3][1] = (s32) (translate[1] * 65536.0f);

    dest[0][2] = -FIXED_MUL16(sy, cz) + FIXED_MUL16(FIXED_MUL16(sx, cy), sz);
    dest[1][2] = FIXED_MUL16(sy, sz) + FIXED_MUL16(FIXED_MUL16(sx, cy), cz);
    dest[2][2] = FIXED_MUL16(cx, cy);
    dest[3][2] = (s32) (translate[2] * 65536.0f);

    dest[0][3] = dest[1][3] = dest[2][3] = 0;
    dest[3][3] = 1 << 16;
}

NDS_ITCM_CODE void mtxf_rotate_xyz_and_translate_fixed(s32 dest[4][4], Vec3f translate, Vec3s rotate) {
    s32 sx = (s32) (sins(rotate[0]) * 65536.0f);
    s32 cx = (s32) (coss(rotate[0]) * 65536.0f);
    s32 sy = (s32) (sins(rotate[1]) * 65536.0f);
    s32 cy = (s32) (coss(rotate[1]) * 65536.0f);
    s32 sz = (s32) (sins(rotate[2]) * 65536.0f);
    s32 cz = (s32) (coss(rotate[2]) * 65536.0f);

    dest[0][0] = FIXED_MUL16(cy, cz);
    dest[0][1] = FIXED_MUL16(cy, sz);
    dest[0][2] = -sy;
    dest[0][3] = 0;

    dest[1][0] = FIXED_MUL16(FIXED_MUL16(sx, sy), cz) - FIXED_MUL16(cx, sz);
    dest[1][1] = FIXED_MUL16(FIXED_MUL16(sx, sy), sz) + FIXED_MUL16(cx, cz);
    dest[1][2] = FIXED_MUL16(sx, cy);
    dest[1][3] = 0;

    dest[2][0] = FIXED_MUL16(FIXED_MUL16(cx, sy), cz) + FIXED_MUL16(sx, sz);
    dest[2][1] = FIXED_MUL16(FIXED_MUL16(cx, sy), sz) - FIXED_MUL16(sx, cz);
    dest[2][2] = FIXED_MUL16(cx, cy);
    dest[2][3] = 0;

    dest[3][0] = (s32) (translate[0] * 65536.0f);
    dest[3][1] = (s32) (translate[1] * 65536.0f);
    dest[3][2] = (s32) (translate[2] * 65536.0f);
    dest[3][3] = 1 << 16;
}

NDS_ITCM_CODE void mtxf_pack(Mtx *dest, s32 src[4][4]) {
    s32 *m1 = &dest->m[0][0];
    s32 *m2 = &dest->m[2][0];
    s32 i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 2; j++) {
            s32 t1 = src[i][2 * j];
            s32 t2 = src[i][2 * j + 1];
            *m1++ = (t1 & 0xffff0000) | ((t2 >> 16) & 0xffff);
            *m2++ = ((t1 << 16) & 0xffff0000) | (t2 & 0xffff);
        }
    }
}

NDS_ITCM_CODE void mtxf_mul_fixed_raw(s32 destFixed[4][4], Mat4 floatDest, s32 aFixed[4][4], const Mtx *bPacked) {
    s32 b[4][3];
    s32 i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            b[i][j] = mtx_get_fixed(bPacked, i, j);
        }
    }

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            s64 acc = (s64) aFixed[i][0] * b[0][j]
                    + (s64) aFixed[i][1] * b[1][j]
                    + (s64) aFixed[i][2] * b[2][j];
            if (i == 3) {
                acc += (s64) b[3][j] << 16;
            }
            destFixed[i][j] = (s32) ((acc + 0x8000) >> 16);
        }
        destFixed[i][3] = 0;
    }
    destFixed[3][3] = 1 << 16;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            floatDest[i][j] = destFixed[i][j] * (1.0f / 65536.0f);
        }
    }
}

NDS_ITCM_CODE void mtxf_scale_fixed(s32 mtx[4][4], Mat4 floatDest, Vec3f s) {
    s32 sx = (s32) (s[0] * 65536.0f);
    s32 sy = (s32) (s[1] * 65536.0f);
    s32 sz = (s32) (s[2] * 65536.0f);
    s32 i;
    for (i = 0; i < 4; i++) {
        mtx[0][i] = FIXED_MUL16(mtx[0][i], sx);
        mtx[1][i] = FIXED_MUL16(mtx[1][i], sy);
        mtx[2][i] = FIXED_MUL16(mtx[2][i], sz);
        floatDest[0][i] = mtx[0][i] * (1.0f / 65536.0f);
        floatDest[1][i] = mtx[1][i] * (1.0f / 65536.0f);
        floatDest[2][i] = mtx[2][i] * (1.0f / 65536.0f);
        floatDest[3][i] = mtx[3][i] * (1.0f / 65536.0f);
    }
}

NDS_ITCM_CODE void mtxf_scale_vec3f(Mat4 dest, Mat4 mtx, Vec3f s) {
    register s32 i;

    for (i = 0; i < 4; i++) {
        dest[0][i] = mtx[0][i] * s[0];
        dest[1][i] = mtx[1][i] * s[1];
        dest[2][i] = mtx[2][i] * s[2];
        dest[3][i] = mtx[3][i];
    }
}

NDS_ITCM_CODE void mtxf_mul_vec3s(Mat4 mtx, Vec3s b) {
    register f32 x = b[0];
    register f32 y = b[1];
    register f32 z = b[2];

    b[0] = x * mtx[0][0] + y * mtx[1][0] + z * mtx[2][0] + mtx[3][0];
    b[1] = x * mtx[0][1] + y * mtx[1][1] + z * mtx[2][1] + mtx[3][1];
    b[2] = x * mtx[0][2] + y * mtx[1][2] + z * mtx[2][2] + mtx[3][2];
}

NDS_ITCM_CODE void mtxf_to_mtx(Mtx *dest, Mat4 src) {
#ifdef AVOID_UB

    guMtxF2L(src, dest);
#else
    s32 asFixedPoint;
    register s32 i;
    register s16 *a3 = (s16 *) dest;
    register s16 *t0 = (s16 *) dest + 16;
    register f32 *t1 = (f32 *) src;

    for (i = 0; i < 16; i++) {
        asFixedPoint = *t1++ * (1 << 16);
        *a3++ = GET_HIGH_S16_OF_32(asFixedPoint);
        *t0++ = GET_LOW_S16_OF_32(asFixedPoint);
    }
#endif
}

NDS_ITCM_CODE void mtxf_rotate_xy(Mtx *mtx, s16 angle) {
    Mat4 temp;

    mtxf_identity(temp);
    temp[0][0] = coss(angle);
    temp[0][1] = sins(angle);
    temp[1][0] = -temp[0][1];
    temp[1][1] = temp[0][0];
    mtxf_to_mtx(mtx, temp);
}

NDS_ITCM_CODE void get_pos_from_transform_mtx(Vec3f dest, Mat4 objMtx, Mat4 camMtx) {
    f32 camX = camMtx[3][0] * camMtx[0][0] + camMtx[3][1] * camMtx[0][1] + camMtx[3][2] * camMtx[0][2];
    f32 camY = camMtx[3][0] * camMtx[1][0] + camMtx[3][1] * camMtx[1][1] + camMtx[3][2] * camMtx[1][2];
    f32 camZ = camMtx[3][0] * camMtx[2][0] + camMtx[3][1] * camMtx[2][1] + camMtx[3][2] * camMtx[2][2];

    dest[0] =
        objMtx[3][0] * camMtx[0][0] + objMtx[3][1] * camMtx[0][1] + objMtx[3][2] * camMtx[0][2] - camX;
    dest[1] =
        objMtx[3][0] * camMtx[1][0] + objMtx[3][1] * camMtx[1][1] + objMtx[3][2] * camMtx[1][2] - camY;
    dest[2] =
        objMtx[3][0] * camMtx[2][0] + objMtx[3][1] * camMtx[2][1] + objMtx[3][2] * camMtx[2][2] - camZ;
}

NDS_ITCM_CODE void vec3f_get_dist_and_angle(Vec3f from, Vec3f to, f32 *dist, s16 *pitch, s16 *yaw) {
    register f32 x = to[0] - from[0];
    register f32 y = to[1] - from[1];
    register f32 z = to[2] - from[2];

    *dist = sqrtf(x * x + y * y + z * z);
    *pitch = atan2s(sqrtf(x * x + z * z), y);
    *yaw = atan2s(z, x);
}

NDS_ITCM_CODE void vec3f_set_dist_and_angle(Vec3f from, Vec3f to, f32 dist, s16 pitch, s16 yaw) {
    to[0] = from[0] + dist * coss(pitch) * sins(yaw);
    to[1] = from[1] + dist * sins(pitch);
    to[2] = from[2] + dist * coss(pitch) * coss(yaw);
}

s32 approach_s32(s32 current, s32 target, s32 inc, s32 dec) {

    if (current < target) {
        current += inc;
        if (current > target) {
            current = target;
        }
    } else {
        current -= dec;
        if (current < target) {
            current = target;
        }
    }
    return current;
}

f32 approach_f32(f32 current, f32 target, f32 inc, f32 dec) {
    if (current < target) {
        current += inc;
        if (current > target) {
            current = target;
        }
    } else {
        current -= dec;
        if (current < target) {
            current = target;
        }
    }
    return current;
}

static u16 atan2_lookup(f32 y, f32 x) {
    u16 ret;

    if (x == 0) {
        ret = gArctanTable[0];
    } else {
        ret = gArctanTable[(s32)(y / x * 1024 + 0.5f)];
    }
    return ret;
}

s16 atan2s(f32 y, f32 x) {
    u16 ret;

    if (x >= 0) {
        if (y >= 0) {
            if (y >= x) {
                ret = atan2_lookup(x, y);
            } else {
                ret = 0x4000 - atan2_lookup(y, x);
            }
        } else {
            y = -y;
            if (y < x) {
                ret = 0x4000 + atan2_lookup(y, x);
            } else {
                ret = 0x8000 - atan2_lookup(x, y);
            }
        }
    } else {
        x = -x;
        if (y < 0) {
            y = -y;
            if (y >= x) {
                ret = 0x8000 + atan2_lookup(x, y);
            } else {
                ret = 0xC000 - atan2_lookup(y, x);
            }
        } else {
            if (y < x) {
                ret = 0xC000 + atan2_lookup(y, x);
            } else {
                ret = -atan2_lookup(x, y);
            }
        }
    }
    return ret;
}

f32 atan2f(f32 y, f32 x) {
    return (f32) atan2s(y, x) * M_PI / 0x8000;
}

#define CURVE_BEGIN_1 1
#define CURVE_BEGIN_2 2
#define CURVE_MIDDLE 3
#define CURVE_END_1 4
#define CURVE_END_2 5

NDS_ITCM_CODE void spline_get_weights(Vec4f result, f32 t, UNUSED s32 c) {
    f32 tinv = 1 - t;
    f32 tinv2 = tinv * tinv;
    f32 tinv3 = tinv2 * tinv;
    f32 t2 = t * t;
    f32 t3 = t2 * t;

    switch (gSplineState) {
        case CURVE_BEGIN_1:
            result[0] = tinv3;
            result[1] = t3 * 1.75f - t2 * 4.5f + t * 3.0f;
            result[2] = -t3 * (11 / 12.0f) + t2 * 1.5f;
            result[3] = t3 * (1 / 6.0f);
            break;
        case CURVE_BEGIN_2:
            result[0] = tinv3 * 0.25f;
            result[1] = t3 * (7 / 12.0f) - t2 * 1.25f + t * 0.25f + (7 / 12.0f);
            result[2] = -t3 * 0.5f + t2 * 0.5f + t * 0.5f + (1 / 6.0f);
            result[3] = t3 * (1 / 6.0f);
            break;
        case CURVE_MIDDLE:
            result[0] = tinv3 * (1 / 6.0f);
            result[1] = t3 * 0.5f - t2 + (4 / 6.0f);
            result[2] = -t3 * 0.5f + t2 * 0.5f + t * 0.5f + (1 / 6.0f);
            result[3] = t3 * (1 / 6.0f);
            break;
        case CURVE_END_1:
            result[0] = tinv3 * (1 / 6.0f);
            result[1] = -tinv3 * 0.5f + tinv2 * 0.5f + tinv * 0.5f + (1 / 6.0f);
            result[2] = tinv3 * (7 / 12.0f) - tinv2 * 1.25f + tinv * 0.25f + (7 / 12.0f);
            result[3] = t3 * 0.25f;
            break;
        case CURVE_END_2:
            result[0] = tinv3 * (1 / 6.0f);
            result[1] = -tinv3 * (11 / 12.0f) + tinv2 * 1.5f;
            result[2] = tinv3 * 1.75f - tinv2 * 4.5f + tinv * 3.0f;
            result[3] = t3;
            break;
    }
}

NDS_ITCM_CODE void anim_spline_init(Vec4s *keyFrames) {
    gSplineKeyframe = keyFrames;
    gSplineKeyframeFraction = 0;
    gSplineState = 1;
}

s32 anim_spline_poll(Vec3f result) {
    Vec4f weights;
    s32 i;
    s32 hasEnded = FALSE;

    vec3f_copy(result, gVec3fZero);
    spline_get_weights(weights, gSplineKeyframeFraction, gSplineState);
    for (i = 0; i < 4; i++) {
        result[0] += weights[i] * gSplineKeyframe[i][1];
        result[1] += weights[i] * gSplineKeyframe[i][2];
        result[2] += weights[i] * gSplineKeyframe[i][3];
    }

    if ((gSplineKeyframeFraction += gSplineKeyframe[0][0] / 1000.0f) >= 1) {
        gSplineKeyframe++;
        gSplineKeyframeFraction--;
        switch (gSplineState) {
            case CURVE_END_2:
                hasEnded = TRUE;
                break;
            case CURVE_MIDDLE:
                if (gSplineKeyframe[2][0] == 0) {
                    gSplineState = CURVE_END_1;
                }
                break;
            default:
                gSplineState++;
                break;
        }
    }

    return hasEnded;
}
