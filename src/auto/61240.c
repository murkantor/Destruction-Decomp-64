#include "common.h"
#include "m2c.h"

M2C_UNK Mem_Free(void *);                      /* extern */

void Sprite_Destroy(void *arg0) {
    Mem_Free(M2C_FIELD(arg0, void **, 0x1C));
    Mem_Free(M2C_FIELD(arg0, void **, 0x18));
    Mem_Free(M2C_FIELD(arg0, void **, 0x14));
    Mem_Free(M2C_FIELD(arg0, void **, 0x20));
    Mem_Free(arg0);
}
