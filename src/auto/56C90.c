#include "common.h"
#include "m2c.h"

void func_80056090(void) {

}

M2C_UNK osViSwapBuffer();                            /* extern */
M2C_UNK osViBlack(M2C_UNK);                     /* extern */

void Vi_SwapBuffer(void *arg0) {
    osViSwapBuffer(arg0);
    osViBlack(0);
}

s32 osViGetCurrentFramebuffer();                                /* extern */
s32 osViGetNextFramebuffer();                                /* extern */

void Vi_WaitSwap(void) {
    s32 temp_s0;

    do {
        temp_s0 = osViGetCurrentFramebuffer();
    } while (temp_s0 != osViGetNextFramebuffer());
}
