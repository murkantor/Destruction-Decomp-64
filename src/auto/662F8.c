#include "common.h"
#include "m2c.h"

void *func_800656F8(void *arg0, u8 *arg1) {
    M2C_FIELD(arg0, u8 *, 0xCC) = (u8) *arg1;
    return arg1 + 1;
}
