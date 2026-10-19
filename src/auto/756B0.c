#include "common.h"
#include "m2c.h"

M2C_UNK osCreateMesgQueue(M2C_UNK *, M2C_UNK *, M2C_UNK); /* extern */
M2C_UNK osSendMesg(M2C_UNK *, M2C_UNK, M2C_UNK); /* extern */
extern s32 D_800B6C30;
extern M2C_UNK D_800D1060;
extern M2C_UNK D_80131F10;

void func_80074AB0(void) {
    D_800B6C30 = 1;
    osCreateMesgQueue(&D_80131F10, &D_800D1060, 1);
    osSendMesg(&D_80131F10, 0, 0);
}
