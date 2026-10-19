#include "common.h"
#include "m2c.h"

typedef struct ALLink_s {
    struct ALLink_s *next;
    struct ALLink_s *prev;
} ALLink;

typedef struct {
    ALLink node;
    u32 startAddr;
    u32 lastFrame;
    char *ptr;
} AMDMABuffer;

typedef struct {
    AMDMABuffer *firstUsed;
    AMDMABuffer *firstFree;
} AMDMAState;

typedef struct {
    u16 type;
    u8 pri;
    u8 status;
    OSMesgQueue *retQueue;
} OSIoMesgHdr;

typedef struct {
    OSIoMesgHdr hdr;
    void *dramAddr;
    u32 devAddr;
    u32 size;
    void *piHandle;
} OSIoMesg;

extern AMDMAState D_800CEAD4;
extern void *D_800CEB20;
extern void *D_800CEB1C;
extern u32 D_801260AC;
extern s32 D_800CEB18;
extern u32 D_800B4E40;
extern s32 D_800B4E44;
extern OSIoMesg *D_800CEB10;
extern OSMesgQueue D_800CEAF0;

u32 osVirtualToPhysical(void *);
void alUnlink(ALLink *);
void alLink(ALLink *, ALLink *);
s32 osEPiStartDma(void *, OSIoMesg *, s32);

s32 func_80067D64(s32 addr, s32 len, void *state) {
    void *foundBuffer;
    s32 delta, addrEnd, buffEnd;
    AMDMABuffer *dmaPtr, *lastDmaPtr;
    void *handle;
    OSIoMesg *m;

    if ((addr & 0xFF000000) == 0xFF000000) {
        handle = D_800CEB20;
        addr &= 0xFFFFFF;
        addr += 0x140000;
    } else if (D_801260AC & 1) {
        return osVirtualToPhysical((void *)addr);
    } else {
        handle = D_800CEB1C;
    }

    lastDmaPtr = 0;
    dmaPtr = D_800CEAD4.firstUsed;
    addrEnd = addr + len;

    while (dmaPtr) {
        buffEnd = dmaPtr->startAddr + D_800CEB18;
        if (dmaPtr->startAddr > addr) {
            break;
        } else if (addrEnd <= buffEnd) {
            dmaPtr->lastFrame = D_800B4E40;
            foundBuffer = dmaPtr->ptr + addr - dmaPtr->startAddr;
            return osVirtualToPhysical(foundBuffer);
        }
        lastDmaPtr = dmaPtr;
        dmaPtr = (AMDMABuffer *)dmaPtr->node.next;
    }

    dmaPtr = D_800CEAD4.firstFree;
    if (!dmaPtr) {
        return osVirtualToPhysical(D_800CEAD4.firstUsed);
    }
    D_800CEAD4.firstFree = (AMDMABuffer *)dmaPtr->node.next;
    alUnlink((ALLink *)dmaPtr);

    if (lastDmaPtr) {
        alLink((ALLink *)dmaPtr, (ALLink *)lastDmaPtr);
    } else if (D_800CEAD4.firstUsed) {
        lastDmaPtr = D_800CEAD4.firstUsed;
        D_800CEAD4.firstUsed = dmaPtr;
        dmaPtr->node.next = (ALLink *)lastDmaPtr;
        dmaPtr->node.prev = 0;
        lastDmaPtr->node.prev = (ALLink *)dmaPtr;
    } else {
        D_800CEAD4.firstUsed = dmaPtr;
        dmaPtr->node.next = 0;
        dmaPtr->node.prev = 0;
    }

    foundBuffer = dmaPtr->ptr;
    delta = addr & 1;
    addr -= delta;
    dmaPtr->startAddr = addr;
    dmaPtr->lastFrame = D_800B4E40;

    m = &D_800CEB10[D_800B4E44++];
    m->hdr.pri = 0;
    m->hdr.retQueue = &D_800CEAF0;
    m->dramAddr = foundBuffer;
    m->devAddr = addr;
    m->size = D_800CEB18;
    osEPiStartDma(handle, m, 0);

    return osVirtualToPhysical(foundBuffer) + delta;
}
