#include "common.h"
#include "m2c.h"

typedef struct {
    u32 ramarray[15];
    u32 pifstatus;
} OSPifRam;

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

extern OSPifRam __osPfsPifRam;
extern u8 __osContLastCmd;
extern s32 D_800B6DA0;

void __osSiGetAccess(void);
void __osSiRelAccess(void);
s32 __osSiRawStartDma(s32 direction, void *dramAddr);
s32 osRecvMesg(OSMesgQueue *mq, OSMesg *msg, s32 flag);
u8 __osContAddressCrc(u16 addr);
u8 __osContDataCrc(u8 *data);
s32 __osPfsGetStatus(OSMesgQueue *queue, s32 channel);
void bcopy(const void *src, void *dst, s32 len);

s32 __osContRamRead(OSMesgQueue *mq, s32 channel, u16 address, u8 *buffer) {
    s32 ret = 0;
    s32 i;
    u8 *ptr;
    s32 retry;

    __osSiGetAccess();
    ptr = (u8 *)&__osPfsPifRam;
    retry = 2;
    if (__osContLastCmd != 2 || D_800B6DA0 != channel) {
        __osContLastCmd = 2;
        D_800B6DA0 = channel;
        for (i = 0; i < channel; i++) {
            *ptr++ = 0;
        }
        __osPfsPifRam.pifstatus = 1;
        ((__OSContRamReadFormat *)ptr)->dummy = 0xFF;
        ((__OSContRamReadFormat *)ptr)->txsize = 3;
        ((__OSContRamReadFormat *)ptr)->rxsize = 0x21;
        ((__OSContRamReadFormat *)ptr)->cmd = 2;
        ((__OSContRamReadFormat *)ptr)->datacrc = 0xFF;
        ptr[sizeof(__OSContRamReadFormat)] = 0xFE;
    } else {
        ptr += channel;
    }
    ((__OSContRamReadFormat *)ptr)->addrh = address >> 3;
    ((__OSContRamReadFormat *)ptr)->addrl = (u8)(__osContAddressCrc(address) | (address << 5));
    ret = __osSiRawStartDma(1, &__osPfsPifRam);
    osRecvMesg(mq, NULL, 1);
    do {
        ret = __osSiRawStartDma(0, &__osPfsPifRam);
        osRecvMesg(mq, NULL, 1);
        ret = (((__OSContRamReadFormat *)ptr)->rxsize & 0xC0) >> 4;
        if (ret == 0) {
            if (__osContDataCrc(((__OSContRamReadFormat *)ptr)->data) != ((__OSContRamReadFormat *)ptr)->datacrc) {
                if ((ret = __osPfsGetStatus(mq, channel)) != 0) {
                    break;
                }
                ret = 4;
            } else {
                bcopy(((__OSContRamReadFormat *)ptr)->data, buffer, 32);
            }
        } else {
            ret = 1;
        }
        if (ret != 4) {
            break;
        }
    } while (retry-- >= 0);
    __osSiRelAccess();
    return ret;
}
