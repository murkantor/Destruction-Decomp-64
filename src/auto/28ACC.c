#include "common.h"
#include "m2c.h"

M2C_UNK Mem_Free(void *);                      /* extern */

void List_FreeAll(void **arg0) {
    void *temp_s0;
    void *var_a0;

    var_a0 = *arg0;
    if (var_a0 != NULL) {
        do {
            temp_s0 = M2C_FIELD(var_a0, void **, 4);
            Mem_Free(var_a0);
            var_a0 = temp_s0;
        } while (var_a0 != NULL);
    }
}
