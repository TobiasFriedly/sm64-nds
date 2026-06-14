#include <PR/ultratypes.h>

#include "audio/external.h"
#include "behavior_data.h"
#include "engine/behavior_script.h"
#include "engine/graph_node.h"
#include "object_helpers.h"
#include "object_list_processor.h"
#include "sm64.h"
#include "spawn_sound.h"
#include "rumble_init.h"

void exec_anim_sound_state(struct SoundState *soundStates) {
    s32 stateIdx = gCurrentObject->oSoundStateID;

    switch (soundStates[stateIdx].playSound) {

        case FALSE:
            break;
        case TRUE: {
            s32 animFrame;

            if ((animFrame = soundStates[stateIdx].animFrame1) >= 0) {
                if (cur_obj_check_anim_frame(animFrame)) {
                    cur_obj_play_sound_2(soundStates[stateIdx].soundMagic);
                }
            }

            if ((animFrame = soundStates[stateIdx].animFrame2) >= 0) {
                if (cur_obj_check_anim_frame(animFrame)) {
                    cur_obj_play_sound_2(soundStates[stateIdx].soundMagic);
                }
            }
        } break;
    }
}

void create_sound_spawner(s32 soundMagic) {
    struct Object *obj = spawn_object(gCurrentObject, 0, bhvSoundSpawner);

    obj->oSoundEffectUnkF4 = soundMagic;
}

void cur_obj_play_sound_1(s32 soundMagic) {
    if (gCurrentObject->header.gfx.node.flags & GRAPH_RENDER_ACTIVE) {
        play_sound(soundMagic, gCurrentObject->header.gfx.cameraToObject);
    }
}

void cur_obj_play_sound_2(s32 soundMagic) {
    if (gCurrentObject->header.gfx.node.flags & GRAPH_RENDER_ACTIVE) {
        play_sound(soundMagic, gCurrentObject->header.gfx.cameraToObject);
#if ENABLE_RUMBLE
        if (soundMagic == SOUND_OBJ_BOWSER_WALK) {
            queue_rumble_data(3, 60);
        }
        if (soundMagic == SOUND_OBJ_POUNDING_LOUD) {
            queue_rumble_data(3, 60);
        }
        if (soundMagic == SOUND_OBJ_WHOMP) {
            queue_rumble_data(5, 80);
        }
#endif
    }
}

s32 calc_dist_to_volume_range_1(f32 distance) {
    s32 volume;

    if (distance < 500.0f) {
        volume = 127;
    } else if (1500.0f < distance) {
        volume = 0;
    } else {
        volume = (((distance - 500.0f) / 1000.0f) * 64.0f) + 60.0f;
    }

    return volume;
}

s32 calc_dist_to_volume_range_2(f32 distance) {
    s32 volume;

    if (distance < 1300.0f) {
        volume = 127;
    } else if (2300.0f < distance) {
        volume = 0;
    } else {
        volume = (((distance - 1000.0f) / 1000.0f) * 64.0f) + 60.0f;
    }

    return volume;
}
