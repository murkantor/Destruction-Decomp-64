#include "common.h"
#include "m2c.h"

M2C_UNK func_80058044();                            /* extern */
extern s32 D_800BD20C;
extern s32 D_800BD210;
extern s32 D_800BD214;
extern s32 D_800BD218;

void func_800581F4(void) {
    D_800BD20C = 0;
    D_800BD210 = 0;
    D_800BD214 = 0;
    D_800BD218 = 0;
    func_80058044();
}
