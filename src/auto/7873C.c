#include "common.h"
#include "m2c.h"

typedef struct { u8 dummy; u8 txsize; u8 rxsize; u8 cmd; u8 typeh; u8 typel; u8 status; u8 dummy1; } __OSContRequesFormat;
typedef struct { u32 ramarray[15]; u32 pifstatus; } OSPifRam;
extern OSPifRam __osPfsPifRam;
extern u8 __osContLastCmd;
extern u8 __osMaxControllers;

void __osPfsRequestData(u8 cmd) {
    u8 *ptr;
    __OSContRequesFormat requestformat;
    s32 i;

    __osContLastCmd = cmd;
    ptr = (u8 *)&__osPfsPifRam;
    __osPfsPifRam.pifstatus = 1;
    requestformat.dummy = 0xFF;
    requestformat.txsize = 1;
    requestformat.rxsize = 3;
    requestformat.cmd = cmd;
    requestformat.typeh = 0xFF;
    requestformat.typel = 0xFF;
    requestformat.status = 0xFF;
    requestformat.dummy1 = 0xFF;
    for (i = 0; i < __osMaxControllers; i++) {
        *(__OSContRequesFormat *)ptr = requestformat;
        ptr += sizeof(requestformat);
    }
    *ptr = 0xFE;
}
