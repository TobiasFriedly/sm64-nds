
void create_transform_from_normals(Mat4 transform, f32 xNorm, f32 yNorm, f32 zNorm) {
    Vec3f normal;
    Vec3f pos;

    pos[0] = o->oPosX;
    pos[1] = o->oPosY;
    pos[2] = o->oPosZ;

    normal[0] = xNorm;
    normal[1] = yNorm;
    normal[2] = zNorm;

    mtxf_align_terrain_normal(transform, normal, pos, 0);
}

void bhv_platform_normals_init(void) {
    Mat4 *transform = &o->transform;

    o->oTiltingPyramidNormalX = 0.0f;
    o->oTiltingPyramidNormalY = 1.0f;
    o->oTiltingPyramidNormalZ = 0.0f;

    create_transform_from_normals(*transform, 0.0f, 1.0f, 0.0f);
}

f32 approach_by_increment(f32 goal, f32 src, f32 inc) {
    f32 newVal;

    if (src <= goal) {
        if (goal - src < inc) {
            newVal = goal;
        } else {
            newVal = src + inc;
        }
    } else if (goal - src > -inc) {
        newVal = goal;
    } else {
        newVal = src - inc;
    }

    return newVal;
}

void bhv_tilting_inverted_pyramid_loop(void) {
    f32 dx;
    f32 dy;
    f32 dz;
    f32 d;

    Vec3f dist;
    Vec3f posBeforeRotation;
    Vec3f posAfterRotation;

    f32 mx;
    f32 my;
    f32 mz;

    s32 marioOnPlatform = FALSE;
    UNUSED u8 filler1[4];
    Mat4 *transform = &o->transform;
    UNUSED u8 filler2[28];

    if (gMarioObject->platform == o) {
        get_mario_pos(&mx, &my, &mz);

        dist[0] = gMarioObject->oPosX - o->oPosX;
        dist[1] = gMarioObject->oPosY - o->oPosY;
        dist[2] = gMarioObject->oPosZ - o->oPosZ;
        linear_mtxf_mul_vec3f(*transform, posBeforeRotation, dist);

        dx = gMarioObject->oPosX - o->oPosX;
        dy = 500.0f;
        dz = gMarioObject->oPosZ - o->oPosZ;
        d = sqrtf(dx * dx + dy * dy + dz * dz);

        if (d != 0.0f) {

            d = 1.0 / d;
            dx *= d;
            dy *= d;
            dz *= d;
        } else {
            dx = 0.0f;
            dy = 1.0f;
            dz = 0.0f;
        }

        if (o->oTiltingPyramidMarioOnPlatform == TRUE) {
            marioOnPlatform++;
        }

        o->oTiltingPyramidMarioOnPlatform = TRUE;
    } else {
        dx = 0.0f;
        dy = 1.0f;
        dz = 0.0f;
        o->oTiltingPyramidMarioOnPlatform = FALSE;
    }

    o->oTiltingPyramidNormalX = approach_by_increment(dx, o->oTiltingPyramidNormalX, 0.01f);
    o->oTiltingPyramidNormalY = approach_by_increment(dy, o->oTiltingPyramidNormalY, 0.01f);
    o->oTiltingPyramidNormalZ = approach_by_increment(dz, o->oTiltingPyramidNormalZ, 0.01f);
    create_transform_from_normals(*transform, o->oTiltingPyramidNormalX, o->oTiltingPyramidNormalY, o->oTiltingPyramidNormalZ);

    if (marioOnPlatform) {
        linear_mtxf_mul_vec3f(*transform, posAfterRotation, dist);
        mx += posAfterRotation[0] - posBeforeRotation[0];
        my += posAfterRotation[1] - posBeforeRotation[1];
        mz += posAfterRotation[2] - posBeforeRotation[2];
        set_mario_pos(mx, my, mz);
    }

    o->header.gfx.throwMatrix = transform;
}
