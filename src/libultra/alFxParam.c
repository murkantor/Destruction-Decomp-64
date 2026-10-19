#include "common.h"
#include "m2c.h"

s32 alFxParam(s32 *arg0, s32 arg1, s32 arg2) {
    if (arg1 == 1) {
        *arg0 = arg2;
    }
    return 0;
}
