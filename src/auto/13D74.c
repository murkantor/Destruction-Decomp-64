#include "common.h"
#include "m2c.h"

M2C_UNK func_80055AB4(void *);                      /* extern */

void func_80013174(void **arg0) {
    func_80055AB4(M2C_FIELD(*arg0, void **, 0xC));
    func_80055AB4(M2C_FIELD(*arg0, void **, 0x14));
    func_80055AB4(*arg0);
    *arg0 = NULL;
}
