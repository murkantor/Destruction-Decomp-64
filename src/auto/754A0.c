#include "common.h"
#include "m2c.h"

M2C_UNK __osSpSetStatus(M2C_UNK);                     /* extern */

void osSpTaskYield(void) {
    __osSpSetStatus(0x400);
}
