#include "common.h"
#include "m2c.h"

typedef struct {
    u16 type;
    u8 pri;
    u8 status;
    OSMesgQueue *retQueue;
} OSIoMesgHdr;

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

typedef struct OSPiHandle_s {
    struct OSPiHandle_s *next;
    u8 type;
    u8 latency;
    u8 pageSize;
    u8 relDuration;
    u8 pulse;
    u8 domain;
    u32 baseAddress;
    u32 speed;
    __OSTranxInfo transferInfo;
} OSPiHandle;

typedef struct {
    OSIoMesgHdr hdr;
    void *dramAddr;
    u32 devAddr;
    u32 size;
    OSPiHandle *piHandle;
} OSIoMesg;

typedef struct {
    s32 active;
    OSThread *thread;
    OSMesgQueue *cmdQueue;
    OSMesgQueue *evtQueue;
    OSMesgQueue *acsQueue;
    s32 (*dma)(s32, u32, void *, u32);
    s32 (*edma)(OSPiHandle *, s32, u32, void *, u32);
} OSDevMgr;

s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);
s32 osSendMesg(OSMesgQueue *, OSMesg, s32);
void __osResetGlobalIntMask(u32);
void __osSetGlobalIntMask(u32);
s32 osEPiRawWriteIo(OSPiHandle *, u32, u32);
s32 osEPiRawReadIo(OSPiHandle *, u32, u32 *);
void osYieldThread(void);

void __osDevMgrMain(void *args) {
    OSIoMesg *mb;
    OSMesg em;
    OSMesg dummy;
    s32 ret;
    OSDevMgr *dm;
    s32 messageSend;

    messageSend = 0;
    mb = NULL;
    ret = 0;
    dm = (OSDevMgr *)args;
    while (TRUE) {
        osRecvMesg(dm->cmdQueue, (OSMesg *)&mb, OS_MESG_BLOCK);
        if (mb->piHandle != NULL && mb->piHandle->type == 2 &&
            (mb->piHandle->transferInfo.cmdType == OS_READ || mb->piHandle->transferInfo.cmdType == OS_WRITE)) {
            __OSBlockInfo *blockInfo;
            __OSTranxInfo *info;
            info = &mb->piHandle->transferInfo;
            blockInfo = &info->block[info->blockNum];
            info->sectorNum = -1;
            if (info->transferMode != 3) {
                blockInfo->dramAddr = (void *)((u32)blockInfo->dramAddr - blockInfo->sectorSize);
            }
            if (info->transferMode == 2 && mb->piHandle->transferInfo.cmdType == OS_READ) {
                messageSend = 1;
            } else {
                messageSend = 0;
            }
            osRecvMesg(dm->acsQueue, &dummy, OS_MESG_BLOCK);
            __osResetGlobalIntMask(0x00100401);
            osEPiRawWriteIo(mb->piHandle, 0x05000510, info->bmCtlShadow | 0x80000000);
        readblock1:
            osRecvMesg(dm->evtQueue, &em, OS_MESG_BLOCK);
            info = &mb->piHandle->transferInfo;
            blockInfo = &info->block[info->blockNum];
            if (blockInfo->errStatus == 29) {
                u32 stat;
                osEPiRawWriteIo(mb->piHandle, 0x05000510, info->bmCtlShadow | 0x10000000);
                osEPiRawWriteIo(mb->piHandle, 0x05000510, info->bmCtlShadow);
                osEPiRawReadIo(mb->piHandle, 0x05000508, &stat);
                if (stat & 0x02000000) {
                    osEPiRawWriteIo(mb->piHandle, 0x05000510, info->bmCtlShadow | 0x01000000);
                }
                blockInfo->errStatus = 4;
                *(vu32 *)0xA4600010 = 2;
                __osSetGlobalIntMask(0x00100C01);
            }
            osSendMesg(mb->hdr.retQueue, mb, OS_MESG_NOBLOCK);
            if (messageSend == 1 && mb->piHandle->transferInfo.block[0].errStatus == 0) {
                messageSend = 0;
                goto readblock1;
            }
            osSendMesg(dm->acsQueue, NULL, OS_MESG_NOBLOCK);
            if (mb->piHandle->transferInfo.blockNum == 1) {
                osYieldThread();
            }
        } else {
            switch (mb->hdr.type) {
            case 11:
                osRecvMesg(dm->acsQueue, &dummy, OS_MESG_BLOCK);
                ret = dm->dma(OS_READ, mb->devAddr, mb->dramAddr, mb->size);
                break;
            case 12:
                osRecvMesg(dm->acsQueue, &dummy, OS_MESG_BLOCK);
                ret = dm->dma(OS_WRITE, mb->devAddr, mb->dramAddr, mb->size);
                break;
            case 15:
                osRecvMesg(dm->acsQueue, &dummy, OS_MESG_BLOCK);
                ret = dm->edma(mb->piHandle, OS_READ, mb->devAddr, mb->dramAddr, mb->size);
                break;
            case 16:
                osRecvMesg(dm->acsQueue, &dummy, OS_MESG_BLOCK);
                ret = dm->edma(mb->piHandle, OS_WRITE, mb->devAddr, mb->dramAddr, mb->size);
                break;
            case 10:
                osSendMesg(mb->hdr.retQueue, mb, OS_MESG_NOBLOCK);
                ret = -1;
                break;
            default:
                ret = -1;
                break;
            }
            if (ret == 0) {
                osRecvMesg(dm->evtQueue, &em, OS_MESG_BLOCK);
                osSendMesg(mb->hdr.retQueue, mb, OS_MESG_NOBLOCK);
                osSendMesg(dm->acsQueue, NULL, OS_MESG_NOBLOCK);
            }
        }
    }
}
