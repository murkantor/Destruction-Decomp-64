#include "common.h"
#include "m2c.h"

M2C_UNK osCreateMesgQueue(M2C_UNK *, M2C_UNK *, M2C_UNK); /* extern */
M2C_UNK osCreatePiManager(M2C_UNK, M2C_UNK *, M2C_UNK *, M2C_UNK); /* extern */
extern M2C_UNK D_800BD220;
extern M2C_UNK D_800BD260;
extern M2C_UNK D_80125C20;
extern M2C_UNK D_801310F0;

void func_80058800(void) {
    osCreateMesgQueue(&D_80125C20, &D_801310F0, 0x32);
    osCreatePiManager(0x96, &D_800BD260, &D_800BD220, 0x10);
}
