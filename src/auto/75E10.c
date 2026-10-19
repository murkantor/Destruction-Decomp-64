#include "common.h"
#include "m2c.h"

M2C_UNK osSendMesg(M2C_UNK *, M2C_UNK, M2C_UNK); /* extern */
extern M2C_UNK D_801255B0;

void func_80075210(void) {
    osSendMesg(&D_801255B0, 0, 0);
}
