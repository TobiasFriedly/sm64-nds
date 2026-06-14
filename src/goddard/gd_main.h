#ifndef GD_MAIN_H
#define GD_MAIN_H

#include <PR/ultratypes.h>

#ifdef __GNUC__
#define printf(...)                                       \
    _Pragma ("GCC diagnostic push")                       \
    _Pragma ("GCC diagnostic ignored \"-Wunused-value\"") \
    (__VA_ARGS__);                                        \
    _Pragma ("GCC diagnostic pop")
#else
#define printf
#endif

struct GdControl {
     s32 unk00;
     u8  filler1[4];
     s32 dleft;
     s32 dright;
     s32 dup;
     s32 ddown;
     s32 cleft;
     s32 cright;
     s32 cup;
     s32 cdown;
     void * unk28;
     void * unk2C;
     void * unk30;
     s32 btnA;
     s32 btnB;
     u8  filler2[8];
     s32 trgL;
     s32 trgR;
     s32 unk4C;
     s32 unk50;
     s32 newStartPress;
     u8  filler3[36];
     f32 stickXf;
     f32 stickYf;
     u8  filler4[4];
     f32 unk88;
     u8  filler5[20];
     f32 unkA0;
     u8  filler6[8];
     f32 unkAC;
     u8  filler7[8];
     s32 dragStartX;
     s32 dragStartY;
     s32 stickDeltaX;
     s32 stickDeltaY;
     s32 stickX;
     s32 stickY;
     s32 csrX;
     s32 csrY;

         u8 dragging : 1;
         u8 unkD8b40 : 1;
         u8 unkD8b20 : 1;
         u8 startedDragging : 1;
         u8 unkD8b08 : 1;
         u8 unkD8b04 : 1;
         u8 AbtnPressWait : 1;
     u32 dragStartFrame;
     u8  filler8[8];
     u32 currFrame;
     u8  filler9[4];
     struct GdControl *prevFrame;
};

extern s32 gGdMoveScene;
extern f32 D_801A8058;
extern s32 gGdUseVtxNormal;

extern struct GdControl gGdCtrl;
extern struct GdControl gGdCtrlPrev;

#endif
