#include "common.h"
#include "m2c.h"

typedef struct {
    u32 ramarray[15];
    u32 pifstatus;
} OSPifRam;
extern OSPifRam __osContPifRam;

void __osPackResetData(void) {
    u8 *ptr;
    s32 i;

    __osContPifRam.pifstatus = 1;
    ptr = (u8 *)&__osContPifRam.ramarray;
    for (i = 0; i < 4; i++) {
        *ptr++ = 0xFD;
    }
    *ptr = 0xFE;
}
