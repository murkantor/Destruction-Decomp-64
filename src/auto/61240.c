#include "common.h"
#include "m2c.h"

M2C_UNK func_80055AB4(void *);                      /* extern */

void func_80060640(void *arg0) {
    func_80055AB4(M2C_FIELD(arg0, void **, 0x1C));
    func_80055AB4(M2C_FIELD(arg0, void **, 0x18));
    func_80055AB4(M2C_FIELD(arg0, void **, 0x14));
    func_80055AB4(M2C_FIELD(arg0, void **, 0x20));
    func_80055AB4(arg0);
}
