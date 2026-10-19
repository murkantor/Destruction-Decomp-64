#include "common.h"
#include "m2c.h"

typedef struct { u16 type; u8 status; u8 errno; } OSContStatus;
typedef struct { u32 ramarray[15]; u32 pifstatus; } OSPifRam;
extern OSPifRam __osPfsPifRam;
extern u8 __osPfsInodeCacheBank;
void __osPfsRequestOneChannel(s32);
s32 __osSiRawStartDma(s32, void *);
s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);
void __osPfsGetOneChannelData(s32, OSContStatus *);

s32 __osPfsGetStatus(OSMesgQueue *queue, s32 channel) {
    s32 ret = 0;
    OSMesg dummy;
    OSContStatus data;

    __osPfsInodeCacheBank = 250;
    __osPfsRequestOneChannel(channel);
    ret = __osSiRawStartDma(1, &__osPfsPifRam);
    osRecvMesg(queue, &dummy, 1);
    ret = __osSiRawStartDma(0, &__osPfsPifRam);
    osRecvMesg(queue, &dummy, 1);
    __osPfsGetOneChannelData(channel, &data);
    if ((data.status & 1) && (data.status & 2)) {
        return 2;
    } else if (data.errno != 0 || !(data.status & 1)) {
        return 1;
    } else if (data.status & 4) {
        return 4;
    }
    return ret;
}
