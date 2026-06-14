
const GeoLayout springboard_top_geo[] = {
    GEO_CULLING_RADIUS(500),
    GEO_OPEN_NODE(),
        GEO_DISPLAY_LIST(LAYER_OPAQUE, springboard_checkerboard_seg5_dl_050016B8),
    GEO_CLOSE_NODE(),
    GEO_END(),
};

const GeoLayout springboard_spring_geo[] = {
    GEO_CULLING_RADIUS(500),
    GEO_OPEN_NODE(),
        GEO_DISPLAY_LIST(LAYER_OPAQUE, springboard_spring_seg5_dl_05001800),
    GEO_CLOSE_NODE(),
    GEO_END(),
};

const GeoLayout springboard_bottom_geo[] = {
    GEO_CULLING_RADIUS(500),
    GEO_OPEN_NODE(),
        GEO_DISPLAY_LIST(LAYER_OPAQUE, springboard_checkerboard_seg5_dl_05001900),
    GEO_CLOSE_NODE(),
    GEO_END(),
};
