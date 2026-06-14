
static const Vtx pokey_seg5_vertex_05011710[] = {
    {{{    26,     26,      0}, 0, {   990,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   -25,     26,      0}, 0, {     0,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   -25,    -25,      0}, 0, {     0,    990}, {0xff, 0xff, 0xff, 0xff}}},
    {{{    26,    -25,      0}, 0, {   990,    990}, {0xff, 0xff, 0xff, 0xff}}},
};

ALIGNED8 static const Texture pokey_seg5_texture_05011750[] = {
#include "actors/pokey/pokey_face.rgba16.inc.c"
};

ALIGNED8 static const Texture pokey_seg5_texture_05011F50[] = {
#include "actors/pokey/pokey_face_blink.rgba16.inc.c"
};

const Gfx pokey_seg5_dl_05012750[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPEndDisplayList(),
};

const Gfx pokey_seg5_dl_05012798[] = {
    gsSPVertex(pokey_seg5_vertex_05011710, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};

const Gfx pokey_seg5_dl_050127D8[] = {
    gsSPDisplayList(pokey_seg5_dl_05012750),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, pokey_seg5_texture_05011750),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(pokey_seg5_dl_05012798),
    gsSPEndDisplayList(),
};

const Gfx pokey_seg5_dl_05012808[] = {
    gsSPDisplayList(pokey_seg5_dl_05012750),
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, pokey_seg5_texture_05011F50),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPDisplayList(pokey_seg5_dl_05012798),
    gsSPEndDisplayList(),
};

static const Vtx pokey_seg5_vertex_05012838[] = {
    {{{    26,     26,      0}, 0, {   990,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   -25,     26,      0}, 0, {     0,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{   -25,    -25,      0}, 0, {     0,    990}, {0xff, 0xff, 0xff, 0xff}}},
    {{{    26,    -25,      0}, 0, {   990,    990}, {0xff, 0xff, 0xff, 0xff}}},
};

ALIGNED8 static const Texture pokey_seg5_texture_05012878[] = {
#include "actors/pokey/pokey_body.rgba16.inc.c"
};

const Gfx pokey_seg5_dl_05013078[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, pokey_seg5_texture_05012878),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(pokey_seg5_vertex_05012838, 4, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSPEndDisplayList(),
};

const Gfx pokey_seg5_dl_050130B0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(pokey_seg5_dl_05013078),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
