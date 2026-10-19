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
extern OSPiHandle CartRomHandle;
extern OSPiHandle *__osPiTable;
s32 osPiRawReadIo(u32, u32 *);
void bzero(void *, s32);
u32 __osDisableInt(void);
void __osRestoreInt(u32);

OSPiHandle *osCartRomInit(void) {
    u32 domain = 0;
    u32 saveMask;

    if (CartRomHandle.baseAddress == 0xB0000000) {
        return &CartRomHandle;
    }
    CartRomHandle.type = 0;
    CartRomHandle.baseAddress = 0xB0000000;
    osPiRawReadIo(0, &domain);
    CartRomHandle.latency = (u8)domain;
    CartRomHandle.pulse = (u8)(domain >> 8);
    CartRomHandle.pageSize = (u8)(domain >> 0x10) & 0xF;
    CartRomHandle.relDuration = (u8)(domain >> 0x14) & 0xF;
    CartRomHandle.domain = 0;
    CartRomHandle.speed = 0;
    bzero(&CartRomHandle.transferInfo, sizeof(CartRomHandle.transferInfo));
    saveMask = __osDisableInt();
    CartRomHandle.next = __osPiTable;
    __osPiTable = &CartRomHandle;
    __osRestoreInt(saveMask);
    return &CartRomHandle;
}
