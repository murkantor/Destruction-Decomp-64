#include "common.h"
#include "m2c.h"

void Skid_Start(void *arg0) {
    if (M2C_FIELD(arg0, u8 *, 0) == 0) {
        M2C_FIELD(arg0, u8 *, 0) = 1U;
        M2C_FIELD(arg0, s32 *, 4) = 0;
    }
}

void Skid_Stop(s8 *arg0) {
    *arg0 = 0;
}
