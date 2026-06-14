
struct WaterDropletParams sWaterSplashDropletParams = {
     WATER_DROPLET_FLAG_RAND_ANGLE,
     MODEL_WHITE_PARTICLE_SMALL,
     bhvWaterDroplet,
     0, 0,
     5.0f, 3.0f,
     30.0f, 20.0f,
     0.5f, 1.0f
};

struct WaterDropletParams gShallowWaterSplashDropletParams = {
     WATER_DROPLET_FLAG_RAND_ANGLE | WATER_DROPLET_FLAG_SET_Y_TO_WATER_LEVEL,
     MODEL_WHITE_PARTICLE_SMALL,
     bhvWaterDroplet,
     0, 0,
     2.0f, 3.0f,
     20.0f, 20.0f,
     0.5f, 1.0f
};

struct WaterDropletParams sWaterDropletFishParams = {
     WATER_DROPLET_FLAG_RAND_ANGLE | WATER_DROPLET_FLAG_SET_Y_TO_WATER_LEVEL,
     MODEL_FISH,
     bhvWaterDroplet,
     0, 0,
     2.0f, 3.0f,
     20.0f, 20.0f,
     1.0f, 0.0f
};

struct WaterDropletParams gShallowWaterWaveDropletParams = {
     WATER_DROPLET_FLAG_RAND_ANGLE_INCR_PLUS_8000 | WATER_DROPLET_FLAG_RAND_ANGLE | WATER_DROPLET_FLAG_SET_Y_TO_WATER_LEVEL,
     MODEL_WHITE_PARTICLE_SMALL,
     bhvWaterDroplet,
     0x6000,
     0,
     2.0f, 8.0f,
     10.0f, 10.0f,
     0.5f, 1.0f
};

void bhv_water_splash_spawn_droplets(void) {
    s32 i;
    if (o->oTimer == 0) {
        o->oPosY = find_water_level(o->oPosX, o->oPosZ);
    }

    if (o->oPosY > FLOOR_LOWER_LIMIT_MISC) {
        for (i = 0; i < 3; i++) {
            spawn_water_droplet(o, &sWaterSplashDropletParams);
        }
    }
}

void bhv_water_droplet_loop(void) {
    UNUSED u8 filler[4];
    f32 waterLevel = find_water_level(o->oPosX, o->oPosZ);

    if (o->oTimer == 0) {
        if (cur_obj_has_model(MODEL_FISH)) {
            o->header.gfx.node.flags &= ~GRAPH_RENDER_BILLBOARD;
        } else {
            o->header.gfx.node.flags |= GRAPH_RENDER_BILLBOARD;
        }
        o->oFaceAngleYaw = random_u16();
    }

    o->oVelY -= 4.0f;
    o->oPosY += o->oVelY;

    if (o->oVelY < 0.0f) {
        if (waterLevel > o->oPosY) {

            try_to_spawn_object(0, 1.0f, o, MODEL_SMALL_WATER_SPLASH, bhvWaterDropletSplash);
            obj_mark_for_deletion(o);
        } else if (o->oTimer > 20) {
            obj_mark_for_deletion(o);
        }
    }
    if (waterLevel < FLOOR_LOWER_LIMIT_MISC) {
        obj_mark_for_deletion(o);
    }
}

void bhv_idle_water_wave_loop(void) {
    obj_copy_pos(o, gMarioObject);
    o->oPosY = gMarioStates[0].waterLevel + 5;
    if (!(gMarioObject->oMarioParticleFlags & ACTIVE_PARTICLE_IDLE_WATER_WAVE)) {
        gMarioObject->oActiveParticleFlags &= (u16) ~ACTIVE_PARTICLE_IDLE_WATER_WAVE;
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }
}

void bhv_water_droplet_splash_init(void) {
    cur_obj_scale(random_float() + 1.5);
}

void bhv_bubble_splash_init(void) {
    f32 waterLevel = find_water_level(o->oPosX, o->oPosZ);
    obj_scale_xyz(o, 0.5f, 1.0f, 0.5f);
    o->oPosY = waterLevel + 5.0f;
}

void bhv_shallow_water_splash_init(void) {

    if ((random_u16() & 0xFF) <= 0) {
        struct Object *fishObj = spawn_water_droplet(o, &sWaterDropletFishParams);
        obj_init_animation_with_sound(fishObj, blue_fish_seg3_anims_0301C2B0, 0);
    }
}

void bhv_wave_trail_shrink(void) {
    f32 waterLevel = find_water_level(o->oPosX, o->oPosZ);

    if ((o->oTimer == 0) && (gGlobalTimer & 1)) {
        obj_mark_for_deletion(o);
    }
    o->oPosY = waterLevel + 5.0f;

    if (o->oTimer == 0) {
        o->oWaveTrailSize = o->header.gfx.scale[0];
    }

    if (o->oAnimState > 3) {
        o->oWaveTrailSize = o->oWaveTrailSize - 0.1;
        if (o->oWaveTrailSize < 0.0f) {
            o->oWaveTrailSize = 0.0f;
        }
        o->header.gfx.scale[0] = o->oWaveTrailSize;
        o->header.gfx.scale[2] = o->oWaveTrailSize;
    }
}
