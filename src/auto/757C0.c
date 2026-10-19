#include "common.h"
#include "m2c.h"

typedef struct OSPiHandle_s {
    struct OSPiHandle_s *next;
    u8 type;
    u8 latency;
    u8 pageSize;
    u8 relDuration;
    u8 pulse;
    u8 domain;
    u32 baseAddress;
} OSPiHandle;

extern OSPiHandle *D_800B5740[];
u32 osVirtualToPhysical(void *);

#define IO_READ(a) (*(vu32 *)(a))
#define IO_WRITE(a, v) (*(vu32 *)(a) = (v))

s32 osEPiRawStartDma(OSPiHandle *pihandle, s32 direction, u32 devAddr, void *dramAddr, u32 size) {
    register u32 stat;
    register u32 domain;

    stat = IO_READ(0xA4600010);
    while (stat & 3) {
        stat = IO_READ(0xA4600010);
    }
    domain = pihandle->domain;
    if (D_800B5740[domain] != pihandle) {
        OSPiHandle *cHandle = D_800B5740[domain];
        if (domain == 0) {
            if (cHandle->latency != pihandle->latency) IO_WRITE(0xA4600014, pihandle->latency);
            if (cHandle->pageSize != pihandle->pageSize) IO_WRITE(0xA460001C, pihandle->pageSize);
            if (cHandle->relDuration != pihandle->relDuration) IO_WRITE(0xA4600020, pihandle->relDuration);
            if (cHandle->pulse != pihandle->pulse) IO_WRITE(0xA4600018, pihandle->pulse);
        } else {
            if (cHandle->latency != pihandle->latency) IO_WRITE(0xA4600024, pihandle->latency);
            if (cHandle->pageSize != pihandle->pageSize) IO_WRITE(0xA460002C, pihandle->pageSize);
            if (cHandle->relDuration != pihandle->relDuration) IO_WRITE(0xA4600030, pihandle->relDuration);
            if (cHandle->pulse != pihandle->pulse) IO_WRITE(0xA4600028, pihandle->pulse);
        }
        D_800B5740[domain] = pihandle;
    }
    IO_WRITE(0xA4600000, osVirtualToPhysical(dramAddr));
    IO_WRITE(0xA4600004, (pihandle->baseAddress | devAddr) & 0x1FFFFFFF);
    switch (direction) {
        case 0:
            IO_WRITE(0xA460000C, size - 1);
            break;
        case 1:
            IO_WRITE(0xA4600008, size - 1);
            break;
        default:
            return -1;
    }
    return 0;
}
