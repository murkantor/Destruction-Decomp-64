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
    u32 speed;
    u8 transferInfo[0x60];
} OSPiHandle;

extern OSPiHandle DriveRomHandle;
extern OSPiHandle *__osPiTable;
extern OSPiHandle *D_80124460;
void bzero(void *, s32);
u32 __osDisableInt(void);
void __osRestoreInt(u32);

OSPiHandle *osDriveRomInit(void) {
    u32 saveMask;

    DriveRomHandle.type = 2;
    DriveRomHandle.baseAddress = 0xA5000000;
    DriveRomHandle.latency = 3;
    DriveRomHandle.pulse = 6;
    DriveRomHandle.pageSize = 6;
    DriveRomHandle.relDuration = 2;
    DriveRomHandle.domain = 1;
    DriveRomHandle.speed = 0;
    bzero(&DriveRomHandle.transferInfo, sizeof(DriveRomHandle.transferInfo));
    saveMask = __osDisableInt();
    DriveRomHandle.next = __osPiTable;
    __osPiTable = &DriveRomHandle;
    D_80124460 = &DriveRomHandle;
    __osRestoreInt(saveMask);
    return &DriveRomHandle;
}
