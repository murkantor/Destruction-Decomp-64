#include "common.h"
#include "m2c.h"

M2C_UNK Mem_Free(void *);                      /* extern */

void func_80013174(void **arg0) {
    Mem_Free(M2C_FIELD(*arg0, void **, 0xC));
    Mem_Free(M2C_FIELD(*arg0, void **, 0x14));
    Mem_Free(*arg0);
    *arg0 = NULL;
}
