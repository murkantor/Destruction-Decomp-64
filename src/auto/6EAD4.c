#include "common.h"
#include "m2c.h"

typedef struct {
    u32 ramarray[15];
    u32 pifstatus;
} OSPifRam;
typedef struct {
    u8 dummy;
    u8 txsize;
    u8 rxsize;
    u8 cmd;
    u16 button;
    s8 stick_x;
    s8 stick_y;
} __OSContReadFormat;
extern OSPifRam __osContPifRam;
extern u8 __osMaxControllers;

void __osPackReadData(void) {
    u8 *ptr;
    __OSContReadFormat readformat;
    int i;

    ptr = (u8 *)&__osContPifRam.ramarray;
    for (i = 0; i < 15; i++) {
        __osContPifRam.ramarray[i] = 0;
    }
    __osContPifRam.pifstatus = 1;
    readformat.dummy = 0xFF;
    readformat.txsize = 1;
    readformat.rxsize = 4;
    readformat.cmd = 1;
    readformat.button = 0xFFFF;
    readformat.stick_x = -1;
    readformat.stick_y = -1;
    for (i = 0; i < __osMaxControllers; i++) {
        *(__OSContReadFormat *)ptr = readformat;
        ptr += sizeof(__OSContReadFormat);
    }
    *ptr = 0xFE;
}
