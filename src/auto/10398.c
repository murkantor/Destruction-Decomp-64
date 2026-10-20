#include "common.h"
#include "m2c.h"

M2C_UNK func_8005DBDC();                            /* extern */
extern s8 D_8007F6C1;

s32 func_8000F798(s32 arg0) {
    D_8007F6C1 = 0x2F;
    func_8005DBDC(arg0);
    return 0;
}
