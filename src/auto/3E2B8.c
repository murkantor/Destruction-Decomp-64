#include "common.h"
#include "m2c.h"

u8 func_80050ACC(void);
void func_80063380(s32);
void func_80063564(s32, s32);

s32 func_8003D6B8(void) {
    s32 id;

    if (func_80050ACC()) {
        func_80063380(2);
        id = 0x1A;
    } else {
        id = 0x10;
    }
    func_80063564(id, 1);
    return 1;
}
