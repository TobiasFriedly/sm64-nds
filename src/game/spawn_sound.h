#ifndef SPAWN_SOUND_H
#define SPAWN_SOUND_H

#include <PR/ultratypes.h>

struct SoundState {
    s16 playSound;

    s8 animFrame1;

    s8 animFrame2;
    s32 soundMagic;
};

void cur_obj_play_sound_1(s32 soundMagic);
void cur_obj_play_sound_2(s32 soundMagic);
void create_sound_spawner(s32 soundMagic);
void exec_anim_sound_state(struct SoundState *soundStates);

#endif
