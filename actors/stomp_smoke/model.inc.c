
static const Vtx stomp_smoke_seg4_vertex_040220C8[] = {
    {{{   -32,      0,     32}, 0, {     0,      0}, {0xff, 0xff, 0xff, 0xc8}}},
    {{{    32,      0,     32}, 0, {  1984,      0}, {0xff, 0xff, 0xff, 0xc8}}},
    {{{    32,      0,    -32}, 0, {  1984,   1984}, {0xff, 0xff, 0xff, 0xc8}}},
    {{{   -32,      0,    -32}, 0, {     0,   1984}, {0xff, 0xff, 0xff, 0xc8}}},
};

static const Vtx stomp_smoke_seg4_vertex_04022108[] = {
    {{{   -32,      0,     32}, 0, {     0,      0}, {0xff, 0x00, 0x00, 0xc8}}},
    {{{    32,      0,     32}, 0, {  1984,      0}, {0xff, 0x00, 0x00, 0xc8}}},
    {{{    32,      0,    -32}, 0, {  1984,   1984}, {0xff, 0x00, 0x00, 0xc8}}},
    {{{   -32,      0,    -32}, 0, {     0,   1984}, {0xff, 0x00, 0x00, 0xc8}}},
};

ALIGNED8 static const Texture stomp_smoke_seg4_texture_04022148[] = {
#include "actors/stomp_smoke/stomp_smoke_0.ia16.inc.c"
};

ALIGNED8 static const Texture stomp_smoke_seg4_texture_04022948[] = {
#include "actors/stomp_smoke/stomp_smoke_1.ia16.inc.c"
};

ALIGNED8 static const Texture stomp_smoke_seg4_texture_04023148[] = {
#include "actors/stomp_smoke/stomp_smoke_2.ia16.inc.c"
};

ALIGNED8 static const Texture stomp_smoke_seg4_texture_04023948[] = {
#include "actors/stomp_smoke/stomp_smoke_3.ia16.inc.c"
};

ALIGNED8 static const Texture stomp_smoke_seg4_texture_04024148[] = {
#include "actors/stomp_smoke/stomp_smoke_4.ia16.inc.c"
};

ALIGNED8 static const Texture stomp_smoke_seg4_texture_04024948[] = {
#include "actors/stomp_smoke/stomp_smoke_5.ia16.inc.c"
};

const Gfx stomp_smoke_seg4_dl_04025148[] = {
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA),
    gsSPTexture(0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPEndDisplayList(),
};

const Gfx stomp_smoke_seg4_dl_04025190[] = {
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsDPPipeSync(),
    gsSPTexture(0x0001, 0x0001, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

const Gfx stomp_smoke_seg4_dl_040251C8[] = {
    gsSPDisplayList(stomp_smoke_seg4_dl_04025148),
    gsSPVertex(stomp_smoke_seg4_vertex_040220C8, 4, 0),
    gsSPBranchList(stomp_smoke_seg4_dl_04025190),
};

const Gfx stomp_smoke_seg4_dl_040251E0[] = {
    gsSPDisplayList(stomp_smoke_seg4_dl_04025148),
    gsSPVertex(stomp_smoke_seg4_vertex_04022108, 4, 0),
    gsSPBranchList(stomp_smoke_seg4_dl_04025190),
};

const Gfx stomp_smoke_seg4_dl_040251F8[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, stomp_smoke_seg4_texture_04022148),
    gsSPBranchList(stomp_smoke_seg4_dl_040251C8),
};

const Gfx stomp_smoke_seg4_dl_04025210[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, stomp_smoke_seg4_texture_04022948),
    gsSPBranchList(stomp_smoke_seg4_dl_040251C8),
};

const Gfx stomp_smoke_seg4_dl_04025228[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, stomp_smoke_seg4_texture_04023148),
    gsSPBranchList(stomp_smoke_seg4_dl_040251C8),
};

const Gfx stomp_smoke_seg4_dl_04025240[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, stomp_smoke_seg4_texture_04023948),
    gsSPBranchList(stomp_smoke_seg4_dl_040251C8),
};

const Gfx stomp_smoke_seg4_dl_04025258[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, stomp_smoke_seg4_texture_04024148),
    gsSPBranchList(stomp_smoke_seg4_dl_040251C8),
};

const Gfx stomp_smoke_seg4_dl_04025270[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, stomp_smoke_seg4_texture_04024948),
    gsSPBranchList(stomp_smoke_seg4_dl_040251C8),
};

const Gfx stomp_smoke_seg4_dl_04025288[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, stomp_smoke_seg4_texture_04022148),
    gsSPBranchList(stomp_smoke_seg4_dl_040251E0),
};

const Gfx stomp_smoke_seg4_dl_040252A0[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, stomp_smoke_seg4_texture_04022948),
    gsSPBranchList(stomp_smoke_seg4_dl_040251E0),
};

const Gfx stomp_smoke_seg4_dl_040252B8[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, stomp_smoke_seg4_texture_04023148),
    gsSPBranchList(stomp_smoke_seg4_dl_040251E0),
};

const Gfx stomp_smoke_seg4_dl_040252D0[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, stomp_smoke_seg4_texture_04023948),
    gsSPBranchList(stomp_smoke_seg4_dl_040251E0),
};

const Gfx stomp_smoke_seg4_dl_040252E8[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, stomp_smoke_seg4_texture_04024148),
    gsSPBranchList(stomp_smoke_seg4_dl_040251E0),
};

const Gfx stomp_smoke_seg4_dl_04025300[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, stomp_smoke_seg4_texture_04024948),
    gsSPBranchList(stomp_smoke_seg4_dl_040251E0),
};
