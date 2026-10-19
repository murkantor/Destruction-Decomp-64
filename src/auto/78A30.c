#include "common.h"
#include "m2c.h"

typedef struct OSPiHandle_s { struct OSPiHandle_s *next; u8 type; u8 latency; u8 pageSize; u8 relDuration; u8 pulse; u8 domain; u32 baseAddress; } OSPiHandle;
extern OSPiHandle *D_800B5740[];
#define IO_READ(a) (*(vu32 *)((a) | 0xA0000000))
#define IO_WRITE(a, d) (*(vu32 *)((a) | 0xA0000000) = (d))
#define PI_STATUS_REG 0x04600010
#define PI_BSD_DOM1_LAT_REG 0x04600014
#define PI_BSD_DOM1_PWD_REG 0x04600018
#define PI_BSD_DOM1_PGS_REG 0x0460001C
#define PI_BSD_DOM1_RLS_REG 0x04600020
#define PI_BSD_DOM2_LAT_REG 0x04600024
#define PI_BSD_DOM2_LWD_REG 0x04600028
#define PI_BSD_DOM2_PGS_REG 0x0460002C
#define PI_BSD_DOM2_RLS_REG 0x04600030

s32 osEPiRawWriteIo(OSPiHandle *pihandle, u32 devAddr, u32 data) {
    register u32 stat;
    register u32 domain;

    while (stat = IO_READ(PI_STATUS_REG), stat & 3)
        ;
    domain = pihandle->domain;
    if (D_800B5740[domain] != pihandle) {
        OSPiHandle *cHandle = D_800B5740[domain];
        if (domain == 0) {
            if (cHandle->latency != pihandle->latency) IO_WRITE(PI_BSD_DOM1_LAT_REG, pihandle->latency);
            if (cHandle->pageSize != pihandle->pageSize) IO_WRITE(PI_BSD_DOM1_PGS_REG, pihandle->pageSize);
            if (cHandle->relDuration != pihandle->relDuration) IO_WRITE(PI_BSD_DOM1_RLS_REG, pihandle->relDuration);
            if (cHandle->pulse != pihandle->pulse) IO_WRITE(PI_BSD_DOM1_PWD_REG, pihandle->pulse);
        } else {
            if (cHandle->latency != pihandle->latency) IO_WRITE(PI_BSD_DOM2_LAT_REG, pihandle->latency);
            if (cHandle->pageSize != pihandle->pageSize) IO_WRITE(PI_BSD_DOM2_PGS_REG, pihandle->pageSize);
            if (cHandle->relDuration != pihandle->relDuration) IO_WRITE(PI_BSD_DOM2_RLS_REG, pihandle->relDuration);
            if (cHandle->pulse != pihandle->pulse) IO_WRITE(PI_BSD_DOM2_LWD_REG, pihandle->pulse);
        }
        D_800B5740[domain] = pihandle;
    }
    IO_WRITE(pihandle->baseAddress | devAddr, data);
    return 0;
}
