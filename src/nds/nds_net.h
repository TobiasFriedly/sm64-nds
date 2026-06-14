#ifndef NDS_NET_H
#define NDS_NET_H

#include <ultra64.h>

enum NdsNetState {
    NDS_NET_OFF = 0,
    NDS_NET_SEARCHING,
    NDS_NET_PAIRED,
};

extern volatile u8 gNdsNetState;
extern volatile u8 gNdsNetIsHost;
extern volatile u32 gNdsNetOwnNonce;
extern volatile u32 gNdsNetPeerNonce;
extern volatile u32 gNdsNetRxFrames;
extern volatile u32 gNdsNetTxFrames;
extern volatile u32 gNdsNetTxRejected;
extern volatile u32 gNdsNetLastPeerMs;

extern volatile u8 gNdsNetRequest;

extern volatile u8 gNdsBootChoice;

s32 nds_net_init(void);

void nds_net_start_search(void);
void nds_net_stop(void);

void nds_net_update(void);

void nds_net_send_state(const void *buf, u32 len);
s32 nds_net_get_peer_state(void *out, u32 len);

void nds_net_send_file_chunk(const void *buf, u32 len);
s32 nds_net_get_file_chunk(void *out, u32 maxLen);

void nds_net_send_file_ack(u8 fileNum, u8 gen, u8 mask);
extern volatile u8 gNdsNetFileAckNum;
extern volatile u8 gNdsNetFileAckGen;
extern volatile u8 gNdsNetFileAckMask;

void nds_net_send_star(u8 course, u8 star, u8 level);
s32 nds_net_get_star(u8 *out);

#endif
