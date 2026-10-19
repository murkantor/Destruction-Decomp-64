#include "common.h"
#include "m2c.h"

typedef struct { u32 inst1, inst2, inst3, inst4; } __osExceptionVector;
extern __osExceptionVector func_80070420[];
extern u64 D_800B5690;
extern s32 D_800B5698;
extern u32 D_80131290;
extern s32 osResetType;
extern s32 osTvType;
extern u8 osAppNMIBuffer[];
extern u32 __osGetSR(void);
extern void __osSetSR(u32);
extern void __osSetFpcCsr(u32);
extern s32 __osSiRawReadIo(u32, u32 *);
extern s32 __osSiRawWriteIo(u32, u32);
extern void osWritebackDCache(void *, s32);
extern void osInvalICache(void *, s32);
extern void osMapTLBRdb(void);
extern s32 osPiRawReadIo(u32, u32 *);
extern void bzero(void *, s32);

void osInitialize(void) {
    u32 pifdata;
    u32 clock = 0;

    D_80131290 = 1;
    __osSetSR(__osGetSR() | 0x20000000);
    __osSetFpcCsr(0x01000800);
    while (__osSiRawReadIo(0x1FC007FC, &pifdata)) {
    }
    while (__osSiRawWriteIo(0x1FC007FC, pifdata | 8)) {
    }
    *(__osExceptionVector *)0x80000000 = *func_80070420;
    *(__osExceptionVector *)0x80000080 = *func_80070420;
    *(__osExceptionVector *)0x80000100 = *func_80070420;
    *(__osExceptionVector *)0x80000180 = *func_80070420;
    osWritebackDCache((void *)0x80000000, 0x190);
    osInvalICache((void *)0x80000000, 0x190);
    osMapTLBRdb();
    osPiRawReadIo(4, &clock);
    clock &= ~0xf;
    if (clock != 0) {
        D_800B5690 = clock;
    }
    D_800B5690 = D_800B5690 * 3 / 4;
    if (osResetType == 0) {
        bzero(osAppNMIBuffer, 0x40);
    }
    if (osTvType == 0) {
        D_800B5698 = 0x2F5B2D2;
    } else if (osTvType == 2) {
        D_800B5698 = 0x2E6025C;
    } else {
        D_800B5698 = 0x2E6D354;
    }
}
