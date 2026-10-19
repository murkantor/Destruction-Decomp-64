#include "common.h"
#include "m2c.h"

typedef struct {
    u8 dummy;
    u8 txsize;
    u8 rxsize;
    u8 cmd;
    u8 addrh;
    u8 addrl;
    u8 data[32];
    u8 datacrc;
} __OSContRamReadFormat;

extern u8 __osPfsPifRam[];
extern s32 D_801312DC;
extern u8 __osContLastCmd;
extern s32 D_800B6DA0;
void __osSiGetAccess(void);
void __osSiRelAccess(void);
s32 __osSiRawStartDma(s32, void *);
u8 __osContAddressCrc(u16);
u8 __osContDataCrc(u8 *);
void bcopy(const void *, void *, s32);
s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);
s32 __osPfsGetStatus(OSMesgQueue *, s32);

s32 __osContRamWrite(OSMesgQueue *mq, s32 channel, u16 address, u8 *buffer, s32 force) {
    s32 ret;
    s32 i;
    u8 *ptr = __osPfsPifRam;
    s32 retry = 2;
    u8 crc;


    if (force != 1 && address < 7 && address != 0) {
        return 0;
    }
    __osSiGetAccess();
    if (__osContLastCmd != 3 || D_800B6DA0 != channel) {
        __osContLastCmd = 3;
        D_800B6DA0 = channel;
        for (i = 0; i < channel; i++) {
            *ptr++ = 0;
        }
        D_801312DC = 1;
        ((__OSContRamReadFormat *)ptr)->dummy = 0xFF;
        ((__OSContRamReadFormat *)ptr)->txsize = 0x23;
        ((__OSContRamReadFormat *)ptr)->rxsize = 1;
        ((__OSContRamReadFormat *)ptr)->cmd = 3;
        ((__OSContRamReadFormat *)ptr)->datacrc = 0xFF;
        ptr[sizeof(__OSContRamReadFormat)] = 0xFE;
    } else {
        ptr += channel;
    }
    ((__OSContRamReadFormat *)ptr)->addrh = address >> 3;
    ((__OSContRamReadFormat *)ptr)->addrl = __osContAddressCrc(address) | (address << 5);
    bcopy(buffer, ((__OSContRamReadFormat *)ptr)->data, 32);
    __osSiRawStartDma(1, __osPfsPifRam);
    crc = __osContDataCrc(buffer);
    osRecvMesg(mq, NULL, 1);
    do {
        __osSiRawStartDma(0, __osPfsPifRam);
        osRecvMesg(mq, NULL, 1);
        ret = (((__OSContRamReadFormat *)ptr)->rxsize & 0xC0) >> 4;
        if (ret == 0) {
            if (crc != ((__OSContRamReadFormat *)ptr)->datacrc) {
                if ((ret = __osPfsGetStatus(mq, channel)) != 0) {
                    break;
                }
                ret = 4;
            }
        } else {
            ret = 1;
        }
    } while (ret == 4 && retry-- >= 0);
    __osSiRelAccess();
    return ret;
}
