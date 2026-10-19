#include "common.h"
#include "m2c.h"

void func_8004E144(s32, s32);
void Timer_Start(void *, s64);

s32 func_8004E4D8(u8 *obj) {
    s32 n = *(s32 *)(obj + 0x10);

    if (n == 0) {
        return 0;
    }
    *(s32 *)(obj + 0x10) = n - 1;
    func_8004E144(n, n - 1);
    Timer_Start(obj, 300000000LL);
    return 1;
}


s32 func_8004E52C(u8 *obj) {
    s32 n = *(s32 *)(obj + 0x10);

    if (n == 4) {
        return 0;
    }
    *(s32 *)(obj + 0x10) = n + 1;
    func_8004E144(n, n + 1);
    Timer_Start(obj, 300000000LL);
    return 1;
}
