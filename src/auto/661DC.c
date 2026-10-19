#include "common.h"
#include "m2c.h"

void *func_800655DC(void *arg0, u8 *arg1) {
    M2C_FIELD(arg0, u8 *, 0xB4) = (u8) *arg1;
    return arg1 + 1;
}
