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
    u8 pad[2];
    u32 baseAddress;
} OSPiHandle;

extern OSPiHandle *D_800B5740[];
extern char D_800BC6B8[];
extern char D_800BC6BC[];
void __assert(const char *, const char *, int);

#define IO_READ(addr) (*(vu32 *)((addr) | 0xA0000000))
#define IO_WRITE(addr, data) (*(vu32 *)((addr) | 0xA0000000) = (u32)(data))

s32 osEPiRawReadIo(OSPiHandle *pihandle, u32 devAddr, u32 *data) {
    register u32 stat;
    register u32 domain;

    if (data == NULL) {
        __assert(D_800BC6B8, D_800BC6BC, 0x2B);
    }
    while ((stat = IO_READ(0x04600010)) & 3) {
    }
    domain = pihandle->domain;
    if (D_800B5740[domain] != pihandle) {
        OSPiHandle *cHandle = D_800B5740[domain];
        if (domain == 0) {
            if (cHandle->latency != pihandle->latency) IO_WRITE(0x04600014, pihandle->latency);
            if (cHandle->pageSize != pihandle->pageSize) IO_WRITE(0x0460001C, pihandle->pageSize);
            if (cHandle->relDuration != pihandle->relDuration) IO_WRITE(0x04600020, pihandle->relDuration);
            if (cHandle->pulse != pihandle->pulse) IO_WRITE(0x04600018, pihandle->pulse);
        } else {
            if (cHandle->latency != pihandle->latency) IO_WRITE(0x04600024, pihandle->latency);
            if (cHandle->pageSize != pihandle->pageSize) IO_WRITE(0x0460002C, pihandle->pageSize);
            if (cHandle->relDuration != pihandle->relDuration) IO_WRITE(0x04600030, pihandle->relDuration);
            if (cHandle->pulse != pihandle->pulse) IO_WRITE(0x04600028, pihandle->pulse);
        }
        D_800B5740[domain] = pihandle;
    }
    *data = IO_READ(pihandle->baseAddress | devAddr);
    return 0;
}
