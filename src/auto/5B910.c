#include "common.h"
#include "m2c.h"

M2C_UNK Particle_BuildMatrix();                            /* extern */

void func_8005AD10(void *arg0) {
    if (M2C_FIELD(arg0, u8 *, 8) & 1) {
        Particle_BuildMatrix();
        M2C_FIELD(arg0, u8 *, 8) = (u8) (M2C_FIELD(arg0, u8 *, 8) & 0xFE);
    }
}
