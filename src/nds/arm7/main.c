#include <string.h>

#include "../nds_include.h"
#include <dswifi7.h>

#include "nds_audio.h"
#include "../nds_arm7_copy.h"

struct Note *gNotes;
static bool running;
static vu32 wifi_ready;

static void send_input(void) {
    inputGetAndSend();
}

#define W_BEACONCOUNT2 (*((vu16 *) 0x04808134))

static void wifi_keepalive_rearm(void) {
    static u32 count;
    if (++count >= 60) {
        count = 0;
        W_BEACONCOUNT2 = 0xFFFF;
    }
}

static void wifi_ready_handler(u32 value, void *userdata) {

    (void) value;
    (void) userdata;
    wifi_ready = 1;
}

static void copy_service(int num_bytes, void *userdata) {
    struct NdsCopyCmd cmd;
    (void) userdata;
    if (num_bytes != sizeof(cmd)) {

        fifoGetDatamsg(NDS_COPY_FIFO_CHANNEL, sizeof(cmd), (u8 *) &cmd);
        return;
    }
    fifoGetDatamsg(NDS_COPY_FIFO_CHANNEL, sizeof(cmd), (u8 *) &cmd);
    if (cmd.len > NDS_COPY_MAX_CHUNK) {
        cmd.len = NDS_COPY_MAX_CHUNK;
    }
    memcpy((void *) cmd.dst, (const void *) cmd.src, cmd.len);
    fifoSendValue32(NDS_COPY_FIFO_CHANNEL, cmd.dst);
}

static void update_audio(void) {

    if (REG_IF & IRQ_IPC_SYNC) {
        REG_IF = IRQ_IPC_SYNC;

        play_notes(gNotes);

        IPC_SendSync(0);
    }
}

static void power_down(void) {
    running = false;
}

int main(void) {
    irqInit();
    fifoInit();
    touchInit();

    readUserSettings();
    installSystemFIFO();
    installWifiFIFO();
    setPowerButtonCB(power_down);

    SetYtrigger(80);
    irqSet(IRQ_VCOUNT, send_input);

    irqEnable(IRQ_VCOUNT | IRQ_VBLANK);

    fifoSetDatamsgHandler(NDS_COPY_FIFO_CHANNEL, copy_service, NULL);

    fifoSetValue32Handler(FIFO_USER_03, (FifoValue32HandlerFunc) wifi_ready_handler, NULL);

    while (!fifoCheckValue32(FIFO_USER_01)) {
        swiWaitForVBlank();
    }
    gNotes = (struct Note*)fifoGetValue32(FIFO_USER_01);

    REG_IPC_SYNC |= IPC_SYNC_IRQ_ENABLE;
    REG_IF = IRQ_IPC_SYNC;

    enableSound();
    IPC_SendSync(0);
    timerStart(0, ClockDivider_64, TIMER_FREQ_64(240), update_audio);
    running = true;

    while (running) {
        swiWaitForVBlank();
        if (wifi_ready) {
            wifi_keepalive_rearm();
        }
    }

    return 0;
}
