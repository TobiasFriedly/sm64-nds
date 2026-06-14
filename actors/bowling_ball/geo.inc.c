
const GeoLayout bowling_ball_geo[] = {
    GEO_SHADOW(SHADOW_CIRCLE_4_VERTS, 0xC8, 280),
    GEO_OPEN_NODE(),
        GEO_SCALE(0x00, 170393),
        GEO_OPEN_NODE(),
            GEO_DISPLAY_LIST(LAYER_ALPHA, bobomb_seg8_dl_08022D08),
        GEO_CLOSE_NODE(),
    GEO_CLOSE_NODE(),
    GEO_END(),
};

const GeoLayout bowling_ball_track_geo[] = {
    GEO_NODE_START(),
    GEO_OPEN_NODE(),
        GEO_SCALE(0x00, 170393),
        GEO_OPEN_NODE(),
            GEO_DISPLAY_LIST(LAYER_ALPHA, bobomb_seg8_dl_08022D08),
        GEO_CLOSE_NODE(),
    GEO_CLOSE_NODE(),
    GEO_END(),
};
