#include "common.h"
#include "m2c.h"

void func_8001934C(void *arg0) {
    void **temp_v1;

    M2C_FIELD(M2C_FIELD(arg0, void **, 0), void ***, 4) = (void **) M2C_FIELD(arg0, void ***, 4);
    temp_v1 = M2C_FIELD(arg0, void ***, 4);
    if (temp_v1 != NULL) {
        *temp_v1 = M2C_FIELD(arg0, void **, 0);
    }
}

void func_80019374(void *arg0, s32 arg1, s32 arg2, s32 arg3) {
    M2C_FIELD(arg0, s32 *, 4) = arg1;
    M2C_FIELD(arg0, s32 *, 0) = arg2;
    M2C_FIELD(arg0, s32 *, 8) = arg3;
}
