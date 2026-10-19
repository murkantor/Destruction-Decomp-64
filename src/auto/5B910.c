#include "common.h"
#include "m2c.h"

M2C_UNK func_8005ABC4();                            /* extern */

void func_8005AD10(void *arg0) {
    if (M2C_FIELD(arg0, u8 *, 8) & 1) {
        func_8005ABC4();
        M2C_FIELD(arg0, u8 *, 8) = (u8) (M2C_FIELD(arg0, u8 *, 8) & 0xFE);
    }
}
