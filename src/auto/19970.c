#include "common.h"
#include "m2c.h"

M2C_UNK memset(M2C_UNK *, M2C_UNK, M2C_UNK); /* extern */
extern M2C_UNK D_80126140;
extern M2C_UNK D_801313A0;

void func_80018D70(void) {
    memset(&D_801313A0, 0, 0x800);
    memset(&D_80126140, 0, 0x40);
}
