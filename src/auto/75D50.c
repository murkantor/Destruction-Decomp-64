#include "common.h"
#include "m2c.h"

M2C_UNK osCreateMesgQueue(M2C_UNK *, M2C_UNK *, M2C_UNK); /* extern */
M2C_UNK osSendMesg(M2C_UNK *, M2C_UNK, M2C_UNK); /* extern */
extern s32 D_800B6D30;
extern M2C_UNK D_800D1070;
extern M2C_UNK D_801255B0;

void func_80075150(void) {
    D_800B6D30 = 1;
    osCreateMesgQueue(&D_801255B0, &D_800D1070, 1);
    osSendMesg(&D_801255B0, 0, 0);
}
