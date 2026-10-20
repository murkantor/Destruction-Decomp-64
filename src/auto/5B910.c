#include "common.h"
#include "m2c.h"

M2C_UNK ModelInst_BuildMatrix();                            /* extern */

void ModelInst_UpdateMatrix(void *arg0) {
    if (M2C_FIELD(arg0, u8 *, 8) & 1) {
        ModelInst_BuildMatrix();
        M2C_FIELD(arg0, u8 *, 8) = (u8) (M2C_FIELD(arg0, u8 *, 8) & 0xFE);
    }
}
