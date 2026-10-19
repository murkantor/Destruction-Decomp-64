#include "common.h"
#include "m2c.h"

typedef struct { u32 ramarray[15]; u32 pifstatus; } OSPifRam;
typedef struct { u8 dummy; u8 txsize; u8 rxsize; u8 cmd; u8 typeh; u8 typel; u8 status; u8 dummy1; } __OSContRequestFormat;
extern OSPifRam __osContPifRam;
extern u8 __osMaxControllers;

void __osPackRequestData(u8 cmd) {
    u8 *ptr;
    __OSContRequestFormat requestformat;
    int i;

    for (i = 14; i >= 0; i--) {
        __osContPifRam.ramarray[i] = 0;
    }
    __osContPifRam.pifstatus = 1;
    ptr = (u8 *)__osContPifRam.ramarray;
    requestformat.dummy = 0xFF;
    requestformat.txsize = 1;
    requestformat.rxsize = 3;
    requestformat.cmd = cmd;
    requestformat.typeh = 0xFF;
    requestformat.typel = 0xFF;
    requestformat.status = 0xFF;
    requestformat.dummy1 = 0xFF;
    for (i = 0; i < __osMaxControllers; i++) {
        *(__OSContRequestFormat *)ptr = requestformat;
        ptr += sizeof(__OSContRequestFormat);
    }
    *ptr = 0xFE;
}
