#ifndef NDS_MENU_H
#define NDS_MENU_H

#include <ultra64.h>

extern volatile u8 gNdsMenuOpen;
extern volatile u8 gNdsLocalAppearanceModel;
extern volatile u8 gNdsLocalAppearanceColor;

void nds_menu_feed_keys(u32 keysDown);

void nds_menu_render(void);

s16  nds_skin_model(u8 idx);
u32  nds_skin_color_tint(u8 idx);
u8   nds_skin_count(void);
u8   nds_color_count(void);

#endif
