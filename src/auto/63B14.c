#include "common.h"
#include "m2c.h"

extern M2C_UNK gControllersLatched;

M2C_UNK *Pad_GetLatched(void) {
    return &gControllersLatched;
}
