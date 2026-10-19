#include "common.h"
#include "m2c.h"

void func_80060430(void *arg0, s8 arg1) {
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x14), s8 *, 0x17) = arg1;
}
