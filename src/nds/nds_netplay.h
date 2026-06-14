#ifndef NDS_NETPLAY_H
#define NDS_NETPLAY_H

#include <ultra64.h>

void nds_netplay_init(void);

void nds_netplay_update(void);

void bhv_net_player_loop(void);

void nds_make_120_star_file(s32 fileIndex);

void nds_netplay_host_picked(s32 fileNum);

s32 nds_netplay_block_local_pick(void);

void nds_netplay_on_star_collected(s32 courseNum, s32 starIndex, s32 levelNum);

extern volatile u8 gNdsNetLoopback;
extern volatile u8 gNdsNetPlayPuppetActive;
extern volatile u32 gNdsNetPlayStatesSent;
extern volatile u32 gNdsNetPlayStatesApplied;

#endif
