#include "common.h"
#include "m2c.h"

void *Track_DrawDisplayList(void *arg0, void *arg1) {
    M2C_FIELD(arg1, s32 *, 0) = 0xDE000000;
    M2C_FIELD(arg1, s32 *, 4) = (s32) M2C_FIELD(M2C_FIELD(arg0, void **, 0x110), s32 *, 4);
    return arg1 + 8;
}
