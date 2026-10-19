#include "common.h"
#include "m2c.h"

M2C_UNK Mem_Free(void *);                      /* extern */

void func_8005DC68(void *arg0) {
    void *var_a0;
    void *var_s0;

    var_s0 = M2C_FIELD(arg0, void **, 0x8B54);
    var_a0 = var_s0;
    if (var_s0 != NULL) {
        do {
            var_s0 = M2C_FIELD(var_s0, void **, 4);
            Mem_Free(var_a0);
            var_a0 = var_s0;
        } while (var_s0 != NULL);
    }
    M2C_FIELD(arg0, void **, 0x8B54) = NULL;
}

M2C_UNK Mem_Free(void *);                      /* extern */

void func_8005DCC4(void *arg0) {
    void *var_a0;
    void *var_s0;

    var_s0 = M2C_FIELD(arg0, void **, 0x8B6C);
    var_a0 = var_s0;
    if (var_s0 != NULL) {
        do {
            var_s0 = M2C_FIELD(var_s0, void **, 4);
            Mem_Free(var_a0);
            var_a0 = var_s0;
        } while (var_s0 != NULL);
    }
    M2C_FIELD(arg0, void **, 0x8B6C) = NULL;
}
