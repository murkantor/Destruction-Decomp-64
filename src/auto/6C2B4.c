#include "common.h"
#include "m2c.h"

M2C_UNK alLink(M2C_UNK, s32);                /* extern */
M2C_UNK alUnlink(M2C_UNK);                     /* extern */

void _freePVoice(s32 arg0, M2C_UNK arg1) {
    alUnlink(arg1);
    alLink(arg1, arg0 + 0x14);
}
