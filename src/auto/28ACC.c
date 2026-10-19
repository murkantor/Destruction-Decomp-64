#include "common.h"
#include "m2c.h"

M2C_UNK func_80055AB4(void *);                      /* extern */

void func_80027ECC(void **arg0) {
    void *temp_s0;
    void *var_a0;

    var_a0 = *arg0;
    if (var_a0 != NULL) {
        do {
            temp_s0 = M2C_FIELD(var_a0, void **, 4);
            func_80055AB4(var_a0);
            var_a0 = temp_s0;
        } while (var_a0 != NULL);
    }
}
