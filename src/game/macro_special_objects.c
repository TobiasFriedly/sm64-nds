#include <PR/ultratypes.h>

#include "sm64.h"
#include "object_helpers.h"
#include "macro_special_objects.h"
#include "object_list_processor.h"
#include "behavior_data.h"

#include "macro_presets.inc.c"
#include "special_presets.inc.c"

s16 convert_rotation(s16 inRotation) {
    u16 rotation = ((u16)(inRotation & 0xFF));
    rotation <<= 8;

    if (rotation == 0x3F00) {
        rotation = 0x4000;
    }

    if (rotation == 0x7F00) {
        rotation = 0x8000;
    }

    if (rotation == 0xBF00) {
        rotation = 0xC000;
    }

    if (rotation == 0xFF00) {
        rotation = 0x0000;
    }

    return (s16) rotation;
}

void spawn_macro_abs_yrot_2params(s32 model, const BehaviorScript *behavior, s16 x, s16 y, s16 z, s16 ry, s16 params) {
    if (behavior != NULL) {
        struct Object *newObj = spawn_object_abs_with_rot(
            &gMacroObjectDefaultParent, 0, model, behavior, x, y, z, 0, convert_rotation(ry), 0);
        newObj->oBhvParams = ((u32) params) << 16;
    }
}

void spawn_macro_abs_yrot_param1(s32 model, const BehaviorScript *behavior, s16 x, s16 y, s16 z, s16 ry, s16 param) {
    if (behavior != NULL) {
        struct Object *newObj = spawn_object_abs_with_rot(
            &gMacroObjectDefaultParent, 0, model, behavior, x, y, z, 0, convert_rotation(ry), 0);
        newObj->oBhvParams = ((u32) param) << 24;
    }
}

void spawn_macro_abs_special(s32 model, const BehaviorScript *behavior, s16 x, s16 y, s16 z, s16 unkA, s16 unkB,
                             s16 unkC) {
    struct Object *newObj =
        spawn_object_abs_with_rot(&gMacroObjectDefaultParent, 0, model, behavior, x, y, z, 0, 0, 0);

    newObj->oMacroUnk108 = (f32) unkA;
    newObj->oMacroUnk10C = (f32) unkB;
    newObj->oMacroUnk110 = (f32) unkC;
}

#define MACRO_OBJ_Y_ROT 0
#define MACRO_OBJ_X 1
#define MACRO_OBJ_Y 2
#define MACRO_OBJ_Z 3
#define MACRO_OBJ_PARAMS 4

UNUSED static void spawn_macro_coin_unknown(const BehaviorScript *behavior, s16 objInfo[]) {
    struct Object *coin;
    s16 model = bhvYellowCoin == behavior ? MODEL_YELLOW_COIN : MODEL_NONE;

    coin = spawn_object_abs_with_rot(&gMacroObjectDefaultParent, 0, model, behavior,
                                     objInfo[MACRO_OBJ_X], objInfo[MACRO_OBJ_Y], objInfo[MACRO_OBJ_Z],
                                     0, convert_rotation(objInfo[MACRO_OBJ_Y_ROT]), 0);
    coin->oUnusedBhvParams = objInfo[MACRO_OBJ_PARAMS];
    coin->oBhvParams = (objInfo[MACRO_OBJ_PARAMS] & 0xFF) >> 16;
}

struct LoadedPreset {
     const BehaviorScript *behavior;
     s16 param;
     s16 model;
};

void spawn_macro_objects(s16 areaIndex, s16 *macroObjList) {
    UNUSED u8 filler[4];
    s32 presetID;

    s16 macroObject[5];
    struct Object *newObj;
    struct LoadedPreset preset;

    gMacroObjectDefaultParent.header.gfx.areaIndex = areaIndex;
    gMacroObjectDefaultParent.header.gfx.activeAreaIndex = areaIndex;

    while (TRUE) {
        if (*macroObjList == -1) {
            break;
        }

        presetID = (*macroObjList & 0x1FF) - 31;

        if (presetID < 0) {
            break;
        }

        macroObject[MACRO_OBJ_Y_ROT] = ((*macroObjList++ >> 9) & 0x7F) << 1;
        macroObject[MACRO_OBJ_X] = *macroObjList++;
        macroObject[MACRO_OBJ_Y] = *macroObjList++;
        macroObject[MACRO_OBJ_Z] = *macroObjList++;
        macroObject[MACRO_OBJ_PARAMS] = *macroObjList++;

        preset.model = sMacroObjectPresets[presetID].model;
        preset.behavior = sMacroObjectPresets[presetID].behavior;
        preset.param = sMacroObjectPresets[presetID].param;

        if (preset.param != 0) {
            macroObject[MACRO_OBJ_PARAMS] =
                (macroObject[MACRO_OBJ_PARAMS] & 0xFF00) + (preset.param & 0x00FF);
        }

        if (((macroObject[MACRO_OBJ_PARAMS] >> 8) & RESPAWN_INFO_DONT_RESPAWN)
            != RESPAWN_INFO_DONT_RESPAWN) {

            newObj = spawn_object_abs_with_rot(
                         &gMacroObjectDefaultParent,
                         0,
                         preset.model,
                         preset.behavior,
                         macroObject[MACRO_OBJ_X],
                         macroObject[MACRO_OBJ_Y],
                         macroObject[MACRO_OBJ_Z],
                         0,
                         convert_rotation(macroObject[MACRO_OBJ_Y_ROT]),
                         0
                     );

            newObj->oUnusedBhvParams = macroObject[MACRO_OBJ_PARAMS];
            newObj->oBhvParams = ((macroObject[MACRO_OBJ_PARAMS] & 0x00FF) << 16)
                                 + (macroObject[MACRO_OBJ_PARAMS] & 0xFF00);
            newObj->oBhvParams2ndByte = macroObject[MACRO_OBJ_PARAMS] & 0x00FF;
            newObj->respawnInfoType = RESPAWN_INFO_TYPE_16;
            newObj->respawnInfo = macroObjList - 1;
            newObj->parentObj = newObj;
        }
    }
}

void spawn_macro_objects_hardcoded(s16 areaIndex, s16 *macroObjList) {
    UNUSED u8 filler1[8];

    s16 macroObjX;
    s16 macroObjY;
    s16 macroObjZ;
    s16 macroObjPreset;
    s16 macroObjRY;

    UNUSED u8 filler2[10];

    gMacroObjectDefaultParent.header.gfx.areaIndex = areaIndex;
    gMacroObjectDefaultParent.header.gfx.activeAreaIndex = areaIndex;

    while (TRUE) {
        macroObjPreset = *macroObjList++;

        if (macroObjPreset < 0) {
            break;
        }

        macroObjX = *macroObjList++;
        macroObjY = *macroObjList++;
        macroObjZ = *macroObjList++;
        macroObjRY = *macroObjList++;

        switch (macroObjPreset) {
            case 0:
                spawn_macro_abs_yrot_2params(MODEL_NONE, bhvBooStaircase, macroObjX, macroObjY,
                                             macroObjZ, macroObjRY, 0);
                break;
            case 1:
                spawn_macro_abs_yrot_2params(MODEL_BBH_TILTING_FLOOR_PLATFORM,
                                             bhvBBHTiltingTrapPlatform, macroObjX, macroObjY, macroObjZ,
                                             macroObjRY, 0);
                break;
            case 2:
                spawn_macro_abs_yrot_2params(MODEL_BBH_TUMBLING_PLATFORM, bhvBBHTumblingBridge,
                                             macroObjX, macroObjY, macroObjZ, macroObjRY, 0);
                break;
            case 3:
                spawn_macro_abs_yrot_2params(MODEL_BBH_MOVING_BOOKSHELF, bhvHauntedBookshelf, macroObjX,
                                             macroObjY, macroObjZ, macroObjRY, 0);
                break;
            case 4:
                spawn_macro_abs_yrot_2params(MODEL_BBH_MESH_ELEVATOR, bhvMeshElevator, macroObjX,
                                             macroObjY, macroObjZ, macroObjRY, 0);
                break;
            case 20:
                spawn_macro_abs_yrot_2params(MODEL_YELLOW_COIN, bhvYellowCoin, macroObjX, macroObjY,
                                             macroObjZ, macroObjRY, 0);
                break;
            case 21:
                spawn_macro_abs_yrot_2params(MODEL_YELLOW_COIN, bhvYellowCoin, macroObjX, macroObjY,
                                             macroObjZ, macroObjRY, 0);
                break;
            default:
                break;
        }
    }
}

void spawn_special_objects(s16 areaIndex, TerrainData **specialObjList) {
    s32 numOfSpecialObjects;
    s32 i;
    s32 offset;
    s16 x;
    s16 y;
    s16 z;
    s16 extraParams[4];
    u8 model;
    u8 type;
    u8 presetID;
    u8 defaultParam;
    const BehaviorScript *behavior;

    numOfSpecialObjects = **specialObjList;
    (*specialObjList)++;

    gMacroObjectDefaultParent.header.gfx.areaIndex = areaIndex;
    gMacroObjectDefaultParent.header.gfx.activeAreaIndex = areaIndex;

    for (i = 0; i < numOfSpecialObjects; i++) {
        presetID = (u8) **specialObjList;
        (*specialObjList)++;
        x = **specialObjList;
        (*specialObjList)++;
        y = **specialObjList;
        (*specialObjList)++;
        z = **specialObjList;
        (*specialObjList)++;

        offset = 0;
        while (TRUE) {
            if (sSpecialObjectPresets[offset].presetID == presetID) {
                break;
            }

            if (sSpecialObjectPresets[offset].presetID == special_null_end) {
            }

            offset++;
        }

        model = sSpecialObjectPresets[offset].model;
        behavior = sSpecialObjectPresets[offset].behavior;
        type = sSpecialObjectPresets[offset].type;
        defaultParam = sSpecialObjectPresets[offset].defParam;

        switch (type) {
            case SPTYPE_NO_YROT_OR_PARAMS:
                spawn_macro_abs_yrot_2params(model, behavior, x, y, z, 0, 0);
                break;
            case SPTYPE_YROT_NO_PARAMS:
                extraParams[0] = **specialObjList;
                (*specialObjList)++;
                spawn_macro_abs_yrot_2params(model, behavior, x, y, z, extraParams[0], 0);
                break;
            case SPTYPE_PARAMS_AND_YROT:
                extraParams[0] = **specialObjList;
                (*specialObjList)++;
                extraParams[1] = **specialObjList;
                (*specialObjList)++;
                spawn_macro_abs_yrot_2params(model, behavior, x, y, z, extraParams[0], extraParams[1]);
                break;
            case SPTYPE_UNKNOWN:
                extraParams[0] =
                    **specialObjList;
                (*specialObjList)++;
                extraParams[1] =
                    **specialObjList;
                (*specialObjList)++;
                extraParams[2] =
                    **specialObjList;
                (*specialObjList)++;
                spawn_macro_abs_special(model, behavior, x, y, z, extraParams[0], extraParams[1],
                                        extraParams[2]);
                break;
            case SPTYPE_DEF_PARAM_AND_YROT:
                extraParams[0] = **specialObjList;
                (*specialObjList)++;
                spawn_macro_abs_yrot_param1(model, behavior, x, y, z, extraParams[0], defaultParam);
                break;
            default:
                break;
        }
    }
}

#ifdef NO_SEGMENTED_MEMORY
u32 get_special_objects_size(s16 *data) {
    s16 *startPos = data;
    s32 numOfSpecialObjects;
    s32 i;
    u8 presetID;
    s32 offset;

    numOfSpecialObjects = *data++;

    for (i = 0; i < numOfSpecialObjects; i++) {
        presetID = (u8) *data++;
        data += 3;
        offset = 0;

        while (TRUE) {
            if (sSpecialObjectPresets[offset].presetID == presetID) {
                break;
            }
            offset++;
        }

        switch (sSpecialObjectPresets[offset].type) {
            case SPTYPE_NO_YROT_OR_PARAMS:
                break;
            case SPTYPE_YROT_NO_PARAMS:
                data++;
                break;
            case SPTYPE_PARAMS_AND_YROT:
                data += 2;
                break;
            case SPTYPE_UNKNOWN:
                data += 3;
                break;
            case SPTYPE_DEF_PARAM_AND_YROT:
                data++;
                break;
            default:
                break;
        }
    }

    return data - startPos;
}
#endif
