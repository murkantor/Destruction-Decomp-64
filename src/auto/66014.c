#include "common.h"
#include "m2c.h"

void *func_80065414(void *arg0, u8 *arg1) {
    M2C_FIELD(arg0, s8 *, 0xB5) = (s8) ((u8) *arg1 >> 1);
    return arg1 + 1;
}
