#include "common.h"
#include "m2c.h"

f32 func_8005D208(s32, void *);

s32 func_8005CF28(s32 arg0, u8 *obj) {
    f32 d = func_8005D208(arg0, obj + 0x18);
    f32 r = *(f32 *)(obj + 0x30);

    if (d <= r * r) {
        return 1;
    }
    return 0;
}
