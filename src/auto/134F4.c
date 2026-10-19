#include "common.h"
#include "m2c.h"

M2C_UNK func_80011CCC(s32, s32, s32);               /* extern */

void func_800128F4(void *arg0) {
    func_80011CCC(M2C_FIELD(arg0, s32 *, 0x290), M2C_FIELD(arg0, s32 *, 0x294), M2C_FIELD(arg0, s32 *, 0x298));
}

extern void *D_8011EEE4;

s16 func_8001291C(void) {
    return M2C_FIELD(D_8011EEE4, s16 *, 0xC);
}

s32 Mem_Alloc(s32);                                 /* extern */
extern s32 D_800D1C18;
extern s32 D_80124470;

void func_8001292C(s32 arg0) {
    D_80124470 = 0;
    D_800D1C18 = Mem_Alloc(arg0 * 4);
}

M2C_UNK Mem_Free(s32);                         /* extern */

void func_80012958(void) {
    Mem_Free(D_800D1C18);
    D_800D1C18 = 0;
}
