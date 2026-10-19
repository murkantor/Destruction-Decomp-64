#include "common.h"
#include "m2c.h"

s32 __osSpDeviceBusy();                                /* extern */
M2C_UNK __osSpSetStatus(M2C_UNK);                     /* extern */

void osSpTaskStartGo(void) {
    do {

    } while (__osSpDeviceBusy() != 0);
    __osSpSetStatus(0x125);
}
