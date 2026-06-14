#ifndef GD_DYNLIST_MACROS_H
#define GD_DYNLIST_MACROS_H

#define BeginList() \
    { 53716, {0}, {0}, {0.0, 0.0, 0.0} }

#define EndList() \
    { 58, {0}, {0}, {0.0, 0.0, 0.0} }

#define UseIntegerNames(enable) \
    { 0, {0}, {(void *)(enable)}, {0.0, 0.0, 0.0} }

#define SetInitialPosition(x, y, z) \
    { 1, {0}, {0}, {(x), (y), (z)} }

#define SetRelativePosition(x, y, z) \
    { 2, {0}, {0}, {(x), (y), (z)} }

#define SetWorldPosition(x, y, z) \
    { 3, {0}, {0}, {(x), (y), (z)} }

#define SetNormal(x, y, z) \
    { 4, {0}, {0}, {(x), (y), (z)} }

#define SetScale(x, y, z) \
    { 5, {0}, {0}, {(x), (y), (z)} }

#define SetRotation(x, y, z) \
    { 6, {0}, {0}, {(x), (y), (z)} }

#define SetDrawFlag(flags) \
    { 7, {0}, {(void *)(flags)}, {0.0, 0.0, 0.0} }

#define SetFlag(flags) \
    { 8, {0}, {(void *)(flags)}, {0.0, 0.0, 0.0} }

#define ClearFlag(flags) \
    { 9, {0}, {(void *)(flags)}, {0.0, 0.0, 0.0} }

#define SetFriction(x, y, z) \
    { 10, {0}, {0}, {(x), (y), (z)} }

#define SetSpring(spring) \
    { 11, {0}, {0}, {(spring), 0.0, 0.0} }

#define CallList(list) \
    { 12, {(void *)(list)}, {0}, {0.0, 0.0, 0.0} }

#define SetColourNum(colourNum) \
    { 13, {0}, {(void *)(colourNum)}, {0.0, 0.0, 0.0} }

#define MakeDynObj(type, name) \
    { 15, {(void *)(name)}, {(void *)(type)}, {0.0, 0.0, 0.0} }

#define StartGroup(grpName) \
    { 16, {(void *)(grpName)}, {0}, {0.0, 0.0, 0.0} }

#define EndGroup(grpName) \
    { 17, {(void *)(grpName)}, {0}, {0.0, 0.0, 0.0} }

#define AddToGroup(grpName) \
    { 18, {(void *)(grpName)}, {0}, {0.0, 0.0, 0.0} }

#define SetType(type) \
    { 19, {0}, {(void *)(type)}, {0.0, 0.0, 0.0} }

#define SetMaterialGroup(mtlGrpName) \
    { 20, {(void *)(mtlGrpName)}, {0}, {0.0, 0.0, 0.0} }

#define SetNodeGroup(grpName) \
    { 21, {(void *)(grpName)}, {0}, {0.0, 0.0, 0.0} }

#define SetSkinShape(shapeName) \
    { 22, {(void *)(shapeName)}, {0}, {0.0, 0.0, 0.0} }

#define SetPlaneGroup(planeGrpName) \
    { 23, {(void *)(planeGrpName)}, {0}, {0.0, 0.0, 0.0} }

#define SetShapePtrPtr(shapePtr) \
    { 24, {(void *)(shapePtr)}, {0}, {0.0, 0.0, 0.0} }

#define SetShapePtr(shapeName) \
    { 25, {(void *)(shapeName)}, {0}, {0.0, 0.0, 0.0} }

#define SetShapeOffset(x, y, z) \
    { 26, {0}, {0}, {(x), (y), (z)} }

#define SetCenterOfGravity(x, y, z) \
    { 27, {0}, {0}, {(x), (y), (z)} }

#define LinkWith(w1) \
    { 28, {(void *)(w1)}, {0}, {0.0, 0.0, 0.0} }

#define LinkWithPtr(w1) \
    { 29, {(void *)(w1)}, {0}, {0.0, 0.0, 0.0} }

#define UseObj(name) \
    { 30, {(void *)(name)}, {0}, {0.0, 0.0, 0.0} }

#define SetControlType(w2) \
    { 31, {0}, {(void *)(w2)}, {0.0, 0.0, 0.0} }

#define SetSkinWeight(vtxNum, weight) \
    { 32, {0}, {(void *)(vtxNum)}, {(weight), 0.0, 0.0} }

#define SetAmbient(r, g, b) \
    { 33, {0}, {0}, {(r), (g), (b)} }

#define SetDiffuse(r, g, b) \
    { 34, {0}, {0}, {(r), (g), (b)} }

#define SetId(id) \
    { 35, {0}, {(void *)(id)}, {0.0, 0.0, 0.0} }

#define SetMaterial(id) \
    { 36, {0}, {(void *)(id)}, {0.0, 0.0, 0.0} }

#define MapMaterials(name) \
    { 37, {(void *)(name)}, {0}, {0.0, 0.0, 0.0} }

#define MapVertices(name) \
    { 38, {(void *)(name)}, {0}, {0.0, 0.0, 0.0} }

#define Attach(name) \
    { 39, {(void *)(name)}, {0}, {0.0, 0.0, 0.0} }

#define AttachTo(flags, name) \
    { 40, {(void *)(name)}, {(void *)(flags)}, {0.0, 0.0, 0.0} }

#define SetAttachOffset(x, y, z) \
    { 41, {0}, {0}, {(x), (y), (z)} }

#define SetNameSuffix(suffix) \
    { 43, {(void *)(suffix)}, {0}, {0.0, 0.0, 0.0} }

#define SetParamF(param, value) \
    { 44, {0}, {(void *)(param)}, {(value), 0.0, 0.0} }

#define SetParamPtr(param, value) \
    { 45, {(void *)(value)}, {(void *)(param)}, {0.0, 0.0, 0.0} }

#define MakeNetWithSubGroup(name) \
    { 46, {(void *)(name)}, {0}, {0.0, 0.0, 0.0} }

#define MakeAttachedJoint(name) \
    { 47, {(void *)(name)}, {0}, {0.0, 0.0, 0.0} }

#define EndNetWithSubGroup(name) \
    { 48, {(void *)(name)}, {0}, {0.0, 0.0, 0.0} }

#define MakeVertex(x, y, z) \
    { 49, {0}, {0}, {(x), (y), (z)} }

#define MakeValPtr(id, flags, type, offset) \
    { 50, {(void *)(id)}, {(void *)(type)}, {(offset), (flags), 0.0} }

#define UseTexture(texture) \
    { 52, {0}, {(void *)(texture)}, {0.0, 0.0, 0.0} }

#define SetTextureST(s, t) \
    { 53, {0}, {0}, {(s), (t), 0.0} }

#define MakeNetFromShape(shape) \
    { 54, {(void *)(shape)}, {0}, {0.0, 0.0, 0.0} }

#define MakeNetFromShapePtrPtr(w1) \
    { 55, {(void *)(w1)}, {0}, {0.0, 0.0, 0.0} }

#endif
