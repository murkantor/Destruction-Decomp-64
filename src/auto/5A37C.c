#include "common.h"
#include "m2c.h"

typedef struct { u8 data[0x68]; } OSPfs;
extern s32 gPakReady[];
extern OSPfs gPakFs[];
s32 osPfsChecker(OSPfs *);

/* The dead "if (channel != 0) { t...; }" keeps a conditional branch (and its
 * label) between "channel = v0" and "v0 = channel" through combine; flow
 * deletes the dead body, jump2's no-op-move pass sees the label and keeps
 * both moves, then deletes the branch to the next insn. Probably a debug
 * print compiled out in the original. */
s32 Pak_Check(s32 channel) {
    s32 t = channel;
    if (gPakReady[channel] != 0) {
        channel = osPfsChecker(&gPakFs[channel]);
    } else {
        return -1;
    }
    if (channel != 0) {
        t++;
        t *= 3;
    }
    return channel;
}
