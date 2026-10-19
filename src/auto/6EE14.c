#include "common.h"
#include "m2c.h"

typedef struct {
    s32 status;
    OSMesgQueue *queue;
    s32 channel;
    u8 id[32];
    u8 label[32];
    s32 version;
    s32 dir_size;
    s32 inode_table;
    s32 minode_table;
    s32 dir_table;
    s32 inode_start_page;
    u8 banks;
    u8 activebank;
} OSPfs;

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

extern OSPifRam D_800CFD00[];

s32 __osPfsSelectBank(OSPfs *pfs, u8 bank);
s32 __osContRamRead(OSMesgQueue *mq, s32 channel, u16 address, u8 *buffer);
u8 __osContAddressCrc(u16 addr);

static inline void _MakeMotorData(s32 channel, OSPifRam *mdata)
{
    u8 *ptr = (u8 *)mdata->ramarray;
    __OSContRamReadFormat ramreadformat;
    s32 i;

    ramreadformat.dummy = 0xFF;
    ramreadformat.txsize = 0x23;
    ramreadformat.rxsize = 1;
    ramreadformat.cmd = 3;
    ramreadformat.addrh = 0x600 >> 3;
    ramreadformat.addrl = (u8)(__osContAddressCrc(0x600) | (0x600 << 5));
    if (channel != 0) {
        for (i = 0; i < channel; i++) {
            *ptr++ = 0;
        }
    }
    *(__OSContRamReadFormat *)ptr = ramreadformat;
    ptr += sizeof(__OSContRamReadFormat);
    ptr[0] = 0xFE;
}

s32 osMotorInit(OSMesgQueue *mq, OSPfs *pfs, s32 channel)
{
    s32 ret;
    u8 temp[32];

    pfs->queue = mq;
    pfs->channel = channel;
    pfs->activebank = 0xFF;
    pfs->status = 0;

    ret = __osPfsSelectBank(pfs, 0xFE);
    if (ret == 2)
        ret = __osPfsSelectBank(pfs, 0x80);
    if (ret != 0) return ret;
    ret = __osContRamRead(mq, channel, 0x400, temp);
    if (ret == 2) ret = 4;
    if (ret != 0) return ret;
    if (temp[31] == 0xFE) return 11;
    ret = __osPfsSelectBank(pfs, 0x80);
    if (ret == 2) ret = 4;
    if (ret != 0) return ret;
    ret = __osContRamRead(mq, channel, 0x400, temp);
    if (ret == 2) ret = 4;
    if (ret != 0) return ret;
    if (temp[31] != 0x80) return 11;

    if (!(pfs->status & 8)) {
        _MakeMotorData(channel, &D_800CFD00[channel]);
    }
    pfs->status = 8;
    return 0;
}
