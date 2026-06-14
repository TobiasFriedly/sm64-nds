#ifndef GD_BAD_DECLARATIONS_H
#define GD_BAD_DECLARATIONS_H

#include "gd_types.h"

#ifndef AVOID_UB

#define GD_USE_BAD_DECLARATIONS

extern struct ObjFace *make_face_with_colour();

extern struct ObjLabel *make_label();

#endif

#endif
