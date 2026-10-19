#include "common.h"
#include "m2c.h"

M2C_UNK Mem_Free(void *);                      /* extern */

s32 func_80016090(void *arg0) {
    Mem_Free(M2C_FIELD(arg0, void **, 0x20));
    Mem_Free(arg0);
    return 0;
}
