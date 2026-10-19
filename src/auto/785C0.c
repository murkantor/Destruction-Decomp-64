#include "common.h"
#include "m2c.h"

typedef struct {
    u16 type;
    u8 status;
    u8 errnum;
} OSContStatus;

extern u8 __osMaxControllers;
extern u8 __osPfsPifRam[];
void __osSiGetAccess(void);
void __osSiRelAccess(void);
void __osPfsRequestData(u8 cmd);
s32 __osSiRawStartDma(s32 direction, void *dramAddr);
s32 osRecvMesg(OSMesgQueue *mq, OSMesg *msg, s32 flag);
void __osPfsGetInitData(u8 *pattern, OSContStatus *data);

s32 osPfsIsPlug(OSMesgQueue *queue, u8 *pattern) {
    s32 ret = 0;
    OSMesg dummy;
    u8 bitpattern;
    OSContStatus data[4];
    s32 channel;
    u8 bits = 0;
    s32 crc_error_cnt = 3;

    __osSiGetAccess();
    do {
        __osPfsRequestData(0);
        ret = __osSiRawStartDma(1, __osPfsPifRam);
        osRecvMesg(queue, &dummy, 1);
        ret = __osSiRawStartDma(0, __osPfsPifRam);
        osRecvMesg(queue, &dummy, 1);
        __osPfsGetInitData(&bitpattern, data);
        for (channel = 0; channel < __osMaxControllers; channel++) {
            if ((data[channel].status & 4) == 0) {
                crc_error_cnt--;
                break;
            }
        }
        if (channel == __osMaxControllers) {
            crc_error_cnt = 0;
        }
    } while (crc_error_cnt > 0);
    for (channel = 0; channel < __osMaxControllers; channel++) {
        if (data[channel].errnum == 0 && (data[channel].status & 1) != 0) {
            bits |= (1 << channel);
        }
    }
    __osSiRelAccess();
    *pattern = bits;
    return ret;
}
