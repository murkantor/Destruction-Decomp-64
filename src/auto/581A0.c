#include "common.h"
#include "m2c.h"

M2C_UNK Font_Init(M2C_UNK *, M2C_UNK, M2C_UNK, M2C_UNK *, M2C_UNK *, M2C_UNK *, M2C_UNK *); /* extern */
extern M2C_UNK D_800B04C0;
extern M2C_UNK D_800B0520;
extern M2C_UNK D_800B0580;
extern M2C_UNK D_800B0B80;
extern M2C_UNK D_800BD1F0;

void Font_InitDebug(void) {
    Font_Init(&D_800BD1F0, 8, 8, &D_800B04C0, &D_800B0520, &D_800B0580, &D_800B0B80);
}

void func_800575F4(void) {

}
