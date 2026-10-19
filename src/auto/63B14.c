#include "common.h"
#include "m2c.h"

extern M2C_UNK gControllersLatched;

M2C_UNK *func_80062F14(void) {
    return &gControllersLatched;
}
