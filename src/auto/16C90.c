#include "common.h"
#include "m2c.h"

M2C_UNK func_80055AB4(void *);                      /* extern */

s32 func_80016090(void *arg0) {
    func_80055AB4(M2C_FIELD(arg0, void **, 0x20));
    func_80055AB4(arg0);
    return 0;
}
