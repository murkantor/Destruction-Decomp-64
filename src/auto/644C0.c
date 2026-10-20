#include "common.h"
#include "m2c.h"

void Clock_Resume(void *arg0) {
    M2C_FIELD(arg0, s8 *, 0x1C) = 0;
}

void Clock_Pause(void *arg0) {
    M2C_FIELD(arg0, s8 *, 0x1C) = 1;
}
