#include "common.h"
#include "m2c.h"

M2C_UNK memset();            /* extern */

void func_80079A40(M2C_UNK arg0, M2C_UNK arg1, M2C_UNK arg2) {
    memset(arg0, arg2, arg1);
}
