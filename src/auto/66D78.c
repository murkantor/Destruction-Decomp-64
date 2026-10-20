#include "common.h"
#include "m2c.h"

M2C_UNK Bank_Relocate();                            /* extern */

void Sound_RelocateBank(void *arg0, s32 arg1) {
    Bank_Relocate(arg0, arg1);
}
