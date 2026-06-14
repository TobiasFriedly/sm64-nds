#include "../nds_include.h"

#include "nds_audio.h"

#undef SOUND_FREQ
#define SOUND_FREQ(n) (-(BUS_CLOCK >> 1) / (n))

static u16 high_freqs;

static u8 chan_seq[16];
static u16 chan_inited;

static u16 calculate_freq(f32 frequency) {

    u32 freq = frequency * 32000;
    if (freq > 0xFFFF) freq = 0xFFFF;
    return SOUND_FREQ((u16)freq);
}

static u32 calculate_vol_pan(const struct Note *note) {

    u32 vol = note->targetVolLeft + note->targetVolRight;
    u32 pan = (vol << 13) / note->targetVolLeft;
    vol >>= 8;
    pan >>= 8;
    if (vol > 127) vol = 127;
    if (pan > 127) pan = 127;
    return SOUND_VOL(vol) | SOUND_PAN(pan);
}

void play_notes(struct Note *notes) {

    if (notes == NULL) return;

    for (int i = 0; i < 16; i++) {
        struct Note *note = &notes[i];

        if (note->enabled && note->ndsSourceFull != 0) {
            const bool wantHalf = (note->frequency >= 2.0f) && (note->ndsSourceHalf != 0);
            const bool needsInit = !(chan_inited & BIT(i)) || chan_seq[i] != note->ndsSeq;

            if (needsInit || ((SCHANNEL_CR(i) & SCHANNEL_ENABLE) && wantHalf != (bool)(high_freqs & BIT(i)))) {
                const u32 loop = (note->ndsLoop ? SOUND_REPEAT : SOUND_ONE_SHOT);

                SCHANNEL_CR(i) &= ~SCHANNEL_ENABLE;

                if (wantHalf) {

                    SCHANNEL_SOURCE(i) = note->ndsSourceHalf;
                    SCHANNEL_REPEAT_POINT(i) = note->ndsRepeatHalf;
                    SCHANNEL_LENGTH(i) = note->ndsLengthHalf;
                    SCHANNEL_TIMER(i) = calculate_freq(note->frequency / 2);
                    high_freqs |= BIT(i);
                } else {

                    SCHANNEL_SOURCE(i) = note->ndsSourceFull;
                    SCHANNEL_REPEAT_POINT(i) = note->ndsRepeatFull;
                    SCHANNEL_LENGTH(i) = note->ndsLengthFull;
                    SCHANNEL_TIMER(i) = calculate_freq(note->frequency);
                    high_freqs &= ~BIT(i);
                }

                SCHANNEL_CR(i) = SCHANNEL_ENABLE | SOUND_FORMAT_ADPCM | calculate_vol_pan(note) | loop;

                chan_seq[i] = note->ndsSeq;
                chan_inited |= BIT(i);
            } else if (SCHANNEL_CR(i) & SCHANNEL_ENABLE) {

                SCHANNEL_TIMER(i) = calculate_freq(note->frequency / ((high_freqs & BIT(i)) ? 2 : 1));
                SCHANNEL_CR(i) = (SCHANNEL_CR(i) & ~(SOUND_VOL(127) | SOUND_PAN(127))) | calculate_vol_pan(note);
            }
        } else {

            SCHANNEL_CR(i) &= ~SCHANNEL_ENABLE;
            chan_inited &= ~BIT(i);
        }
    }
}
