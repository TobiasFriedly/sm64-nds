#ifndef GEO_COMMANDS_H
#define GEO_COMMANDS_H

#include "command_macros_base.h"

#include "game/shadow.h"
#include "game/object_helpers.h"
#include "game/behavior_actions.h"
#include "game/segment2.h"
#include "game/mario_misc.h"
#include "game/mario_actions_cutscene.h"

#define BACKGROUND_OCEAN_SKY        0
#define BACKGROUND_FLAMING_SKY      1
#define BACKGROUND_UNDERWATER_CITY  2
#define BACKGROUND_BELOW_CLOUDS     3
#define BACKGROUND_SNOW_MOUNTAINS   4
#define BACKGROUND_DESERT           5
#define BACKGROUND_HAUNTED          6
#define BACKGROUND_GREEN_SKY        7
#define BACKGROUND_ABOVE_CLOUDS     8
#define BACKGROUND_PURPLE_SKY       9

#define GEO_BRANCH_AND_LINK(scriptTarget) \
    CMD_BBH(0x00, 0x00, 0x0000), \
    CMD_PTR(scriptTarget)

#define GEO_END() \
    CMD_BBH(0x01, 0x00, 0x0000)

#define GEO_BRANCH(type, scriptTarget) \
    CMD_BBH(0x02, type, 0x0000), \
    CMD_PTR(scriptTarget)

#define GEO_RETURN() \
    CMD_BBH(0x03, 0x00, 0x0000)

#define GEO_OPEN_NODE() \
    CMD_BBH(0x04, 0x00, 0x0000)

#define GEO_CLOSE_NODE() \
    CMD_BBH(0x05, 0x00, 0x0000)

#define GEO_ASSIGN_AS_VIEW(index) \
    CMD_BBH(0x06, 0x00, index)

#define GEO_UPDATE_NODE_FLAGS(operation, flagBits) \
    CMD_BBH(0x07, operation, flagBits)

#define GEO_NODE_SCREEN_AREA(numEntries, x, y, width, height) \
    CMD_BBH(0x08, 0x00, numEntries), \
    CMD_HH(x, y), \
    CMD_HH(width, height)

#define GEO_NODE_ORTHO(scale) \
    CMD_BBH(0x09, 0x00, scale)

#define GEO_CAMERA_FRUSTUM(fov, near, far) \
    CMD_BBH(0x0A, 0x00, fov), \
    CMD_HH(near, far)
#define GEO_CAMERA_FRUSTUM_WITH_FUNC(fov, near, far, func) \
    CMD_BBH(0x0A, 0x01, fov), \
    CMD_HH(near, far), \
    CMD_PTR(func)

#define GEO_NODE_START() \
    CMD_BBH(0x0B, 0x00, 0x0000)

#define GEO_ZBUFFER(enable) \
    CMD_BBH(0x0C, enable, 0x0000)

#define GEO_RENDER_RANGE(minDistance, maxDistance) \
    CMD_BBH(0x0D, 0x00, 0x0000), \
    CMD_HH(minDistance, maxDistance)

#define GEO_SWITCH_CASE(count, function) \
    CMD_BBH(0x0E, 0x00, count), \
    CMD_PTR(function)

#define GEO_CAMERA(type, x1, y1, z1, x2, y2, z2, function) \
    CMD_BBH(0x0F, 0x00, type), \
    CMD_HHHHHH(x1, y1, z1, x2, y2, z2), \
    CMD_PTR(function)

#define GEO_TRANSLATE_ROTATE(layer, tx, ty, tz, rx, ry, rz) \
    CMD_BBH(0x10, (0x00 | layer), 0x0000), \
    CMD_HHHHHH(tx, ty, tz, rx, ry, rz)
#define GEO_TRANSLATE_ROTATE_WITH_DL(layer, tx, ty, tz, rx, ry, rz, displayList) \
    CMD_BBH(0x10, (0x00 | layer | 0x80), 0x0000), \
    CMD_HHHHHH(tx, ty, tz, rx, ry, rz), \
    CMD_PTR(displayList)

#define GEO_TRANSLATE(layer, tx, ty, tz) \
    CMD_BBH(0x10, (0x10 | layer), tx), \
    CMD_HH(ty, tz)
#define GEO_TRANSLATE_WITH_DL(layer, tx, ty, tz, displayList) \
    CMD_BBH(0x10, (0x10 | layer | 0x80), tx), \
    CMD_HH(ty, tz), \
    CMD_PTR(displayList)

#define GEO_ROTATE(layer, rx, ry, rz) \
    CMD_BBH(0x10, (0x20 | layer), rx), \
    CMD_HH(ry, rz)
#define GEO_ROTATE_WITH_DL(layer, rx, ry, rz, displayList) \
    CMD_BBH(0x10, (0x20 | layer | 0x80), rx), \
    CMD_HH(ry, rz), \
    CMD_PTR(displayList)

#define GEO_ROTATE_Y(layer, ry) \
    CMD_BBH(0x10, (0x30 | layer), ry)
#define GEO_ROTATE_Y_WITH_DL(layer, ry, displayList) \
    CMD_BBH(0x10, (0x30 | layer | 0x80), ry), \
    CMD_PTR(displayList)

#define GEO_TRANSLATE_NODE(layer, ux, uy, uz) \
    CMD_BBH(0x11, layer, ux), \
    CMD_HH(uy, uz)
#define GEO_TRANSLATE_NODE_WITH_DL(layer, ux, uy, uz, displayList) \
    CMD_BBH(0x11, (layer | 0x80), ux), \
    CMD_HH(uy, uz), \
    CMD_PTR(displayList)

#define GEO_ROTATION_NODE(layer, ux, uy, uz) \
    CMD_BBH(0x12, layer, ux), \
    CMD_HH(uy, uz)
#define GEO_ROTATION_NODE_WITH_DL(layer, ux, uy, uz, displayList) \
    CMD_BBH(0x12, (layer | 0x80), ux), \
    CMD_HH(uy, uz), \
    CMD_PTR(displayList)

#define GEO_ANIMATED_PART(layer, x, y, z, displayList) \
    CMD_BBH(0x13, layer, x), \
    CMD_HH(y, z), \
    CMD_PTR(displayList)

#define GEO_BILLBOARD_WITH_PARAMS(layer, tx, ty, tz) \
    CMD_BBH(0x14, layer, tx), \
    CMD_HH(ty, tz)
#define GEO_BILLBOARD_WITH_PARAMS_AND_DL(layer, tx, ty, tz, displayList) \
    CMD_BBH(0x14, (layer | 0x80), tx), \
    CMD_HH(ty, tz), \
    CMD_PTR(displayList)
#define GEO_BILLBOARD() \
     GEO_BILLBOARD_WITH_PARAMS(0, 0, 0, 0)

#define GEO_DISPLAY_LIST(layer, displayList) \
    CMD_BBH(0x15, layer, 0x0000), \
    CMD_PTR(displayList)

#define GEO_SHADOW(type, solidity, scale) \
    CMD_BBH(0x16, 0x00, type), \
    CMD_HH(solidity, scale)

#define GEO_RENDER_OBJ() \
    CMD_BBH(0x17, 0x00, 0x0000)

#define GEO_ASM(param, function) \
    CMD_BBH(0x18, 0x00, param), \
    CMD_PTR(function)

#define GEO_BACKGROUND(background, function) \
    CMD_BBH(0x19, 0x00, background), \
    CMD_PTR(function)
#define GEO_BACKGROUND_COLOR(background) \
     GEO_BACKGROUND(background, NULL)

#define GEO_NOP_1A() \
    CMD_BBH(0x1A, 0x00, 0x0000), \
    CMD_HH(0x0000, 0x0000)

#define GEO_COPY_VIEW(index) \
    CMD_BBH(0x1B, 0x00, index)

#define GEO_HELD_OBJECT(param, ux, uy, uz, nodeFunc) \
    CMD_BBH(0x1C, param, ux), \
    CMD_HH(uy, uz), \
    CMD_PTR(nodeFunc)

#define GEO_SCALE(layer, scale) \
    CMD_BBH(0x1D, layer, 0x0000), \
    CMD_W(scale)
#define GEO_SCALE_WITH_DL(layer, scale, displayList) \
    CMD_BBH(0x1D, (layer | 0x80), 0x0000), \
    CMD_W(scale), \
    CMD_PTR(displayList)

#define GEO_NOP_1E() \
    CMD_BBH(0x1E, 0x00, 0x0000), \
    CMD_HH(0x0000, 0x0000)

#define GEO_NOP_1F() \
    CMD_BBH(0x1F, 0x00, 0x0000), \
    CMD_HH(0x0000, 0x0000), \
    CMD_HH(0x0000, 0x0000), \
    CMD_HH(0x0000, 0x0000)

#define GEO_CULLING_RADIUS(cullingRadius) \
    CMD_BBH(0x20, 0x00, cullingRadius)

#endif
