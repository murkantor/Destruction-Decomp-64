#include "common.h"
#include "m2c.h"

M2C_UNK func_8006B050(M2C_UNK *, M2C_UNK *, M2C_UNK); /* extern */
extern M2C_UNK D_800BC190;
extern M2C_UNK D_800BC194;

s32 func_8006B788(void *arg0, void **arg1) {
    s32 var_s1;
    void *var_a0;

    var_s1 = 0x7FFFFFFF;
    if (M2C_FIELD(arg0, void **, 0) == NULL) {
        func_8006B050(&D_800BC190, &D_800BC194, 0x133);
    }
    *arg1 = NULL;
    var_a0 = M2C_FIELD(arg0, void **, 0);
    if (var_a0 != NULL) {
        do {
            if ((M2C_FIELD(var_a0, s32 *, 0x10) - M2C_FIELD(arg0, s32 *, 0x20)) < var_s1) {
                *arg1 = var_a0;
                var_s1 = M2C_FIELD(var_a0, s32 *, 0x10) - M2C_FIELD(arg0, s32 *, 0x20);
            }
            var_a0 = M2C_FIELD(var_a0, void **, 0);
        } while (var_a0 != NULL);
    }
    return M2C_FIELD(*arg1, s32 *, 0x10);
}

void func_8006B830(s32 *arg0) {
    *arg0 = 0;
}
