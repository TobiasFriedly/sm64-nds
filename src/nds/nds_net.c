#include <stdio.h>
#include <string.h>

#include "nds_include.h"
#include "nds_net.h"
#include <dswifi9.h>

#define NET_MAGIC0     0x4D
#define NET_MAGIC1     0x36
#define NET_VERSION    1
#define NET_CHANNEL    13
#define NET_HELLO_DIV  15

enum NetMsgType {
    NET_MSG_HELLO = 1,
    NET_MSG_STATE = 2,
    NET_MSG_FILE = 3,
    NET_MSG_FILE_ACK = 4,
    NET_MSG_STAR = 5,
};

struct NetFrame {
    u8 magic0, magic1;
    u8 version;
    u8 type;
    u32 nonce;
    u8 seq;
    u8 pad[3];
    u8 payload[28];

};

struct RawFrame {
    u16 fc;
    u16 duration;
    u8 dst[6];
    u8 src[6];
    u8 bssid[6];
    u16 seqctl;
    struct NetFrame net;
};

#define RX_RING_LEN 16
static struct NetFrame sRxRing[RX_RING_LEN];
static volatile u8 sRxHead, sRxTail;

volatile u8 gNdsNetState = NDS_NET_OFF;
volatile u8 gNdsNetIsHost;
volatile u32 gNdsNetOwnNonce;
volatile u32 gNdsNetPeerNonce;
volatile u32 gNdsNetRxFrames;
volatile u32 gNdsNetRxOwn;
volatile u32 gNdsNetTxFrames;
volatile u32 gNdsNetTxRejected;
volatile u32 gNdsNetLastPeerMs;
volatile u8 gNdsNetFail;
volatile u8 gNdsNetRequest;
volatile u8 gNdsBootChoice;

static u8 sTxSeq;
static u8 sLastPeerSeq;
static u32 sFrameCounter;

static u8 sPeerStateBuf[28];
static u8 sPeerStateLen;
static u32 sPeerStateFrame;
static u8 sPeerStateValid;

#define FILE_Q_LEN 8
static u8 sFileQ[FILE_Q_LEN][28];
static u8 sFileQLen[FILE_Q_LEN];
static u8 sFileQHead, sFileQTail;

volatile u8 gNdsNetFileAckNum;
volatile u8 gNdsNetFileAckGen;
volatile u8 gNdsNetFileAckMask;

#define STAR_Q_LEN 8
static u8 sStarQ[STAR_Q_LEN][3];
static u8 sStarQHead, sStarQTail;
static u8 sStarLast[3];

void nds_debug_printf(const char *fmt, ...);

static void rx_handler(int packetID, int readlength) {

    u16 buf[64];
    int len = readlength;
    int off;
    if (len > (int) sizeof(buf)) {
        len = sizeof(buf);
    }
    Wifi_RxRawReadPacket(packetID, len, buf);

    for (off = 0; off + (int) sizeof(struct NetFrame) <= len; off += 2) {
        const u8 *p = (const u8 *) buf + off;
        if (p[0] == NET_MAGIC0 && p[1] == NET_MAGIC1 && p[2] == NET_VERSION) {
            const struct NetFrame *f = (const struct NetFrame *) p;
            u8 next;
            if (f->nonce == gNdsNetOwnNonce) {
                gNdsNetRxOwn++;
                return;
            }
            next = (sRxHead + 1) % RX_RING_LEN;
            if (next != sRxTail) {
                memcpy(&sRxRing[sRxHead], f, sizeof(struct NetFrame));
                sRxHead = next;
            }
            return;
        }
    }
}

static void send_frame(u8 type, const void *payload, u32 payloadLen) {
    static struct RawFrame f;
    memset(&f, 0, sizeof(f));
    f.fc = 0x0208;
    memset(f.dst, 0xFF, 6);
    Wifi_GetData(WIFIGETDATA_MACADDRESS, 6, f.src);
    memset(f.bssid, 0xFF, 6);
    f.net.magic0 = NET_MAGIC0;
    f.net.magic1 = NET_MAGIC1;
    f.net.version = NET_VERSION;
    f.net.type = type;
    f.net.nonce = gNdsNetOwnNonce;
    f.net.seq = ++sTxSeq;
    if (payload != NULL && payloadLen > 0) {
        if (payloadLen > sizeof(f.net.payload)) {
            payloadLen = sizeof(f.net.payload);
        }
        memcpy(f.net.payload, payload, payloadLen);
    }
    if (Wifi_RawTxFrame(sizeof(f), 0x0014 , (u16 *) &f) != 0) {
        gNdsNetTxRejected++;
    }
    gNdsNetTxFrames++;
}

static void wifi_timer_50ms(void) {

    int oldIME = enterCriticalSection();
    Wifi_Update();
    leaveCriticalSection(oldIME);
}

static void wifi_sync_handler(u32 value, void *userdata) {
    (void) value;
    (void) userdata;
    Wifi_Sync();
}

static void arm9_sync_to_arm7(void) {
    fifoSendValue32(FIFO_DSWIFI, WIFI_SYNC);
}

s32 nds_net_init(void) {
    u32 token;
    int i;

    fifoSetValue32Handler(FIFO_DSWIFI, wifi_sync_handler, NULL);
    token = Wifi_Init(WIFIINIT_OPTION_USECUSTOMALLOC);
    if (token == 0) {
        gNdsNetFail = 1;
        return -1;
    }
    Wifi_SetSyncHandler(arm9_sync_to_arm7);

    irqSet(IRQ_TIMER3, wifi_timer_50ms);
    irqEnable(IRQ_TIMER3);
    TIMER3_DATA = TIMER_FREQ_256(20);
    TIMER3_CR = TIMER_ENABLE | TIMER_IRQ_REQ | TIMER_DIV_256;

    fifoSendAddress(FIFO_DSWIFI, (void *) token);

    for (i = 0; i < 180 && Wifi_CheckInit() == 0; i++) {
        swiWaitForVBlank();
    }
    if (Wifi_CheckInit() == 0) {
        TIMER3_CR = 0;
        gNdsNetFail = 2;
        return -1;
    }

    fifoSendValue32(FIFO_USER_03, 1);

    Wifi_SetPromiscuousMode(1);
    Wifi_EnableWifi();
    Wifi_SetChannel(NET_CHANNEL);
    Wifi_RawSetPacketHandler(rx_handler);

    {
        u8 mac[6] = { 0 };
        Wifi_GetData(WIFIGETDATA_MACADDRESS, 6, mac);
        gNdsNetOwnNonce = ((u32) mac[2] << 24) | ((u32) mac[3] << 16)
                          | ((u32) mac[4] << 8) | mac[5];
    }
    if (gNdsNetOwnNonce == 0) {
        gNdsNetOwnNonce = 0xC0FFEE01;
    }

    gNdsNetState = NDS_NET_OFF;
    nds_debug_printf("net: up, nonce %08lx\n", (unsigned long) gNdsNetOwnNonce);
    return 0;
}

void nds_net_start_search(void) {
    if (gNdsNetState == NDS_NET_OFF && gNdsNetOwnNonce != 0) {
        gNdsNetState = NDS_NET_SEARCHING;
        nds_debug_printf("net: searching...\n");
    }
}

void nds_net_stop(void) {
    gNdsNetState = NDS_NET_OFF;
    gNdsNetPeerNonce = 0;
}

static void handle_frame(const struct NetFrame *f) {
    gNdsNetRxFrames++;
    gNdsNetLastPeerMs = sFrameCounter;

    switch (f->type) {
        case NET_MSG_HELLO:
            if (gNdsNetState == NDS_NET_SEARCHING) {
                gNdsNetPeerNonce = f->nonce;
                gNdsNetIsHost = (gNdsNetOwnNonce < f->nonce);
                gNdsNetState = NDS_NET_PAIRED;
                nds_debug_printf("net: paired! peer %08lx %s\n",
                                 (unsigned long) f->nonce,
                                 gNdsNetIsHost ? "(host)" : "(guest)");
            }
            break;
        case NET_MSG_FILE: {
            u8 next = (sFileQHead + 1) % FILE_Q_LEN;
            if (next != sFileQTail) {
                memcpy(sFileQ[sFileQHead], f->payload, sizeof(f->payload));
                sFileQLen[sFileQHead] = sizeof(f->payload);
                sFileQHead = next;
            }
            break;
        }
        case NET_MSG_FILE_ACK:
            gNdsNetFileAckNum = f->payload[0];
            gNdsNetFileAckGen = f->payload[1];
            gNdsNetFileAckMask = f->payload[2];
            break;
        case NET_MSG_STAR:
            if (f->payload[0] != sStarLast[0] || f->payload[1] != sStarLast[1]
                || f->payload[2] != sStarLast[2]) {
                u8 next = (sStarQHead + 1) % STAR_Q_LEN;
                sStarLast[0] = f->payload[0];
                sStarLast[1] = f->payload[1];
                sStarLast[2] = f->payload[2];
                if (next != sStarQTail) {
                    sStarQ[sStarQHead][0] = f->payload[0];
                    sStarQ[sStarQHead][1] = f->payload[1];
                    sStarQ[sStarQHead][2] = f->payload[2];
                    sStarQHead = next;
                }
            }
            break;
        case NET_MSG_STATE:
            if (f->seq != sLastPeerSeq) {
                sLastPeerSeq = f->seq;
                memcpy(sPeerStateBuf, f->payload, sizeof(sPeerStateBuf));
                sPeerStateLen = sizeof(sPeerStateBuf);
                sPeerStateFrame = sFrameCounter;
                sPeerStateValid = 1;
            }

            if (gNdsNetState == NDS_NET_SEARCHING) {
                gNdsNetPeerNonce = f->nonce;
                gNdsNetIsHost = (gNdsNetOwnNonce < f->nonce);
                gNdsNetState = NDS_NET_PAIRED;
            }
            break;
        default:
            break;
    }
}

void nds_net_update(void) {

    if (gNdsNetRequest == 1) {
        gNdsNetRequest = 2;
        if (nds_net_init() == 0) {
            nds_net_start_search();
        }
    }
    if (gNdsNetOwnNonce == 0) {
        return;
    }
    sFrameCounter++;

    while (sRxTail != sRxHead) {
        struct NetFrame f;
        memcpy(&f, &sRxRing[sRxTail], sizeof(f));
        sRxTail = (sRxTail + 1) % RX_RING_LEN;
        handle_frame(&f);
    }

    if (gNdsNetState == NDS_NET_SEARCHING) {
        if (sFrameCounter % NET_HELLO_DIV == 0) {
            send_frame(NET_MSG_HELLO, NULL, 0);
        }
    } else if (gNdsNetState == NDS_NET_PAIRED) {
        if (sFrameCounter % 60 == 0) {
            send_frame(NET_MSG_HELLO, NULL, 0);
        }
    }
}

void nds_net_send_state(const void *buf, u32 len) {
    if (gNdsNetState == NDS_NET_PAIRED) {
        send_frame(NET_MSG_STATE, buf, len);
    }
}

s32 nds_net_get_peer_state(void *out, u32 len) {
    if (!sPeerStateValid) {
        return -1;
    }
    if (len > sizeof(sPeerStateBuf)) {
        len = sizeof(sPeerStateBuf);
    }
    memcpy(out, sPeerStateBuf, len);
    sPeerStateValid = 0;
    return (s32) (sFrameCounter - sPeerStateFrame);
}

void nds_net_send_file_chunk(const void *buf, u32 len) {
    if (gNdsNetState == NDS_NET_PAIRED) {
        send_frame(NET_MSG_FILE, buf, len);
    }
}

void nds_net_send_star(u8 course, u8 star, u8 level) {
    u8 p[3];
    p[0] = course;
    p[1] = star;
    p[2] = level;
    if (gNdsNetState == NDS_NET_PAIRED) {
        send_frame(NET_MSG_STAR, p, 3);
    }
}

s32 nds_net_get_star(u8 *out) {
    if (sStarQTail == sStarQHead) {
        return 0;
    }
    out[0] = sStarQ[sStarQTail][0];
    out[1] = sStarQ[sStarQTail][1];
    out[2] = sStarQ[sStarQTail][2];
    sStarQTail = (sStarQTail + 1) % STAR_Q_LEN;
    return 1;
}

void nds_net_send_file_ack(u8 fileNum, u8 gen, u8 mask) {
    u8 p[3];
    p[0] = fileNum;
    p[1] = gen;
    p[2] = mask;
    if (gNdsNetState == NDS_NET_PAIRED) {
        send_frame(NET_MSG_FILE_ACK, p, 3);
    }
}

s32 nds_net_get_file_chunk(void *out, u32 maxLen) {
    u32 len;
    if (sFileQTail == sFileQHead) {
        return -1;
    }
    len = sFileQLen[sFileQTail];
    if (len > maxLen) {
        len = maxLen;
    }
    memcpy(out, sFileQ[sFileQTail], len);
    sFileQTail = (sFileQTail + 1) % FILE_Q_LEN;
    return (s32) len;
}
