
static const Vtx burn_smoke_seg4_vertex_040217C0[] = {
    {{{   -50,    -50,      0}, 0, {     0,    992}, {0x14, 0x0a, 0x0a, 0xff}}},
    {{{    50,    -50,      0}, 0, {   992,    992}, {0x14, 0x0a, 0x0a, 0xff}}},
    {{{    50,     50,      0}, 0, {   992,      0}, {0x14, 0x0a, 0x0a, 0xff}}},
    {{{   -50,     50,      0}, 0, {     0,      0}, {0x14, 0x0a, 0x0a, 0xff}}},
};

ALIGNED8 static const Texture burn_smoke_seg4_texture_04021800[] = {
#include "actors/burn_smoke/burn_smoke.ia16.inc.c"
};

const Gfx burn_smoke_seg4_dl_04022000[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPEndDisplayList(),
};

const Gfx burn_smoke_seg4_dl_04022028[] = {
    gsSPVertex(burn_smoke_seg4_vertex_040217C0, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

const Gfx burn_smoke_seg4_dl_04022048[] = {
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

const Gfx burn_smoke_seg4_dl_04022070[] = {
    gsSPDisplayList(burn_smoke_seg4_dl_04022000),
    gsDPLoadTextureBlock(burn_smoke_seg4_texture_04021800, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_CLAMP, G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPDisplayList(burn_smoke_seg4_dl_04022028),
    gsSPDisplayList(burn_smoke_seg4_dl_04022048),
    gsSPEndDisplayList(),
};
