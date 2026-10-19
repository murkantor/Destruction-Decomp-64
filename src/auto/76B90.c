#include "common.h"
#include "m2c.h"

typedef struct {
    u32 errStatus;
    void *dramAddr;
    void *C2Addr;
    u32 sectorSize;
    u32 C1ErrNum;
    u32 C1ErrSector[4];
} __OSBlockInfo;

typedef struct {
    u32 cmdType;
    u16 transferMode;
    u16 blockNum;
    s32 sectorNum;
    u32 devAddr;
    u32 bmCtlShadow;
    u32 seqCtlShadow;
    __OSBlockInfo block[2];
} __OSTranxInfo;

typedef struct {
    u8 pad[0x14];
    __OSTranxInfo transferInfo;
} OSPiHandle;

extern OSPiHandle *D_80124460;
extern u32 D_800B56A0;
extern void __osLeoResume(void);
extern void __osLeoAbnormalResume(void);
extern s32 osEPiRawStartDma(OSPiHandle *, s32, u32, void *, u32);

#define IO_READ(addr) (*(volatile u32 *)(0xA0000000 | (addr)))
#define IO_WRITE(addr, data) (*(volatile u32 *)(0xA0000000 | (addr)) = (u32)(data))
#define PI_STATUS_REG 0x04600010
#define LEO_STATUS 0x05000508
#define LEO_BM_CTL 0x05000510
#define LEO_BM_STATUS 0x05000510
#define WAIT_ON_IOBUSY(stat) \
    stat = IO_READ(PI_STATUS_REG); \
    while (stat & 3) \
        stat = IO_READ(PI_STATUS_REG);

s32 func_80075F90(void)
{
    volatile u32 pi_stat;
    u32 stat;
    u32 bm_stat;
    __OSTranxInfo *info;
    __OSBlockInfo *blockInfo;

    info = &D_80124460->transferInfo;
    blockInfo = &info->block[info->blockNum];
    pi_stat = IO_READ(PI_STATUS_REG);
    if (pi_stat & 1) {
        D_800B56A0 = D_800B56A0 & ~0x800;
        blockInfo->errStatus = 29;
        __osLeoResume();
        return 1;
    }
    WAIT_ON_IOBUSY(pi_stat);
    stat = IO_READ(LEO_STATUS);
    if (stat & 0x2000000) {
        WAIT_ON_IOBUSY(pi_stat);
        IO_WRITE(LEO_BM_CTL, info->bmCtlShadow | 0x1000000);
        blockInfo->errStatus = 0;
        return 0;
    }
    if (info->cmdType == 2) {
        return 1;
    }
    if (stat & 0x8000000) {
        WAIT_ON_IOBUSY(pi_stat);
        stat = IO_READ(LEO_STATUS);
        blockInfo->errStatus = 22;
        __osLeoResume();
        IO_WRITE(PI_STATUS_REG, 2);
        D_800B56A0 |= 0x100401;
        return 1;
    }
    if (info->cmdType == 1) {
        if ((stat & 0x40000000) == 0) {
            if (info->sectorNum + 1 != info->transferMode * 85) {
                blockInfo->errStatus = 24;
                __osLeoAbnormalResume();
                return 1;
            }
            IO_WRITE(PI_STATUS_REG, 2);
            D_800B56A0 |= 0x100401;
            blockInfo->errStatus = 0;
            __osLeoResume();
            return 1;
        }
        blockInfo->dramAddr = (void *)((u32)blockInfo->dramAddr + blockInfo->sectorSize);
        info->sectorNum++;
        osEPiRawStartDma(D_80124460, 1, 0x05000400, blockInfo->dramAddr, blockInfo->sectorSize);
        return 1;
    }
    if (info->cmdType == 0) {
        if (info->transferMode == 3) {
            if ((s32)blockInfo->C1ErrNum + 17 < info->sectorNum) {
                blockInfo->errStatus = 0;
                __osLeoAbnormalResume();
                return 1;
            }
            if ((stat & 0x40000000) == 0) {
                blockInfo->errStatus = 23;
                __osLeoAbnormalResume();
                return 1;
            }
        } else {
            blockInfo->dramAddr = (void *)((u32)blockInfo->dramAddr + blockInfo->sectorSize);
        }
        bm_stat = IO_READ(LEO_BM_STATUS);
        if ((bm_stat & 0x200000 && bm_stat & 0x400000) || bm_stat & 0x2000000) {
            if (blockInfo->C1ErrNum > 3) {
                if (info->transferMode != 3 || info->sectorNum > 0x52) {
                    blockInfo->errStatus = 23;
                    __osLeoAbnormalResume();
                    return 1;
                }
            } else {
                int errNum = blockInfo->C1ErrNum;
                blockInfo->C1ErrSector[errNum] = info->sectorNum + 1;
            }
            blockInfo->C1ErrNum++;
        }
        if (stat & 0x10000000) {
            if (info->sectorNum + 1 != 88) {
                blockInfo->errStatus = 24;
                __osLeoAbnormalResume();
            }
            if (info->transferMode == 2 && info->blockNum == 0) {
                info->blockNum = 1;
                info->sectorNum = -1;
                info->block[1].dramAddr = (void *)((u32)info->block[1].dramAddr - info->block[1].sectorSize);
                blockInfo->errStatus = 22;
            } else {
                IO_WRITE(PI_STATUS_REG, 2);
                D_800B56A0 |= 0x100401;
                info->cmdType = 2;
                blockInfo->errStatus = 0;
            }
            osEPiRawStartDma(D_80124460, 0, 0x05000000, blockInfo->C2Addr, blockInfo->sectorSize * 4);
            return 1;
        }
        if (info->sectorNum == -1 && info->transferMode == 2 && info->blockNum == 1) {
            __OSBlockInfo *bptr = &info->block[0];
            if (bptr->C1ErrNum == 0) {
                if (((u32 *)bptr->C2Addr)[0] | ((u32 *)bptr->C2Addr)[1] | ((u32 *)bptr->C2Addr)[2] | ((u32 *)bptr->C2Addr)[3]) {
                    bptr->errStatus = 23;
                    __osLeoAbnormalResume();
                    return 1;
                }
            }
            bptr->errStatus = 0;
            __osLeoResume();
        }
        info->sectorNum++;
        if (stat & 0x40000000) {
            if (info->sectorNum > 0x54) {
                blockInfo->errStatus = 24;
                __osLeoAbnormalResume();
                return 1;
            }
            osEPiRawStartDma(D_80124460, 0, 0x05000400, blockInfo->dramAddr, blockInfo->sectorSize);
            blockInfo->errStatus = 0;
            return 1;
        }
        if (info->sectorNum <= 0x54) {
            blockInfo->errStatus = 24;
            __osLeoAbnormalResume();
            return 1;
        }
        return 1;
    }
    blockInfo->errStatus = 4;
    __osLeoAbnormalResume();
    return 1;
}
