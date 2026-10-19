#include "common.h"
#include "m2c.h"

typedef struct { u8 txsize; u8 rxsize; u8 cmd; u8 typeh; u8 typel; u8 status; } __OSContRequesFormatShort;
typedef struct { u32 ramarray[15]; u32 pifstatus; } OSPifRam;
extern OSPifRam __osPfsPifRam;
extern u8 __osContLastCmd;

void __osPfsRequestOneChannel(s32 channel) {
    u8 *ptr;
    __OSContRequesFormatShort requestformat;
    s32 i;

    __osPfsPifRam.pifstatus = 1;
    ptr = (u8 *)&__osPfsPifRam;
    requestformat.txsize = 1;
    requestformat.rxsize = 3;
    __osContLastCmd = 0;
    requestformat.cmd = 0;
    requestformat.typeh = 0xFF;
    requestformat.typel = 0xFF;
    requestformat.status = 0xFF;
    for (i = 0; i < channel; i++) {
        *ptr++ = 0;
    }
    *(__OSContRequesFormatShort *)ptr = requestformat;
    ptr += sizeof(requestformat);
    *ptr = 0xFE;
}
