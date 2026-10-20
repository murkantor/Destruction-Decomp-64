#include "common.h"
#include "m2c.h"

s32 osViGetCurrentFramebuffer();                                /* extern */
s32 osViGetNextFramebuffer();                                /* extern */

s32 __scTaskReady(s32 arg0) {
    s32 temp_s0;

    if (arg0 != 0) {
        temp_s0 = osViGetCurrentFramebuffer();
        return arg0 & -(temp_s0 == osViGetNextFramebuffer());
    }
    return 0;
}
