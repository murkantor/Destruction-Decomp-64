#include "common.h"
#include "m2c.h"

M2C_UNK Cont_ReadAll();                            /* extern */
M2C_UNK Pad_StartRead();                            /* extern */
s32 Pak_FindFile(M2C_UNK, M2C_UNK *);              /* extern */
extern M2C_UNK D_80097D50;
extern u8 D_80097D58;

void func_80050684(s32 *arg0) {
    if (D_80097D58 == 0) {
        Cont_ReadAll();
    }
    *arg0 = Pak_FindFile(0, &D_80097D50);
    if (D_80097D58 == 0) {
        Pad_StartRead();
    }
}
