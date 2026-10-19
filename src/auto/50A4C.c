#include "common.h"
#include "m2c.h"

s32 *Mem_Alloc(M2C_UNK);                            /* extern */
M2C_UNK func_8002D0E0(s32);                         /* extern */
M2C_UNK func_800500CC(s32 *);                       /* extern */
M2C_UNK func_80050684(s32 *);                       /* extern */
M2C_UNK func_80055AB4(s32 *);                       /* extern */
M2C_UNK func_80058A74();                            /* extern */
M2C_UNK func_80058B58();                            /* extern */
s32 func_80059650(u16, s32, M2C_UNK *, M2C_UNK *, M2C_UNK *); /* extern */
s32 func_8005969C(M2C_UNK *);                       /* extern */
s32 func_800598E0(M2C_UNK);                         /* extern */
s32 func_800599B4(M2C_UNK, s32, M2C_UNK, M2C_UNK, s32 *); /* extern */
extern s32 D_80097A10;
extern s32 D_80097A14;
extern u8 D_80097D4C;
extern s32 D_80097D50;
extern u8 D_80097D58;
extern u16 D_80097D66;
extern s32 D_80097D68;
extern M2C_UNK D_80097D6C;
extern M2C_UNK D_80097D7C;
extern M2C_UNK D_800D2E40;

void func_8004FE4C(s32 *arg0) {
    s32 *temp_s1;
    s32 temp_v0;
    s32 temp_v0_2;

    temp_s1 = Mem_Alloc(0x320);
    if (D_80097D58 == 0) {
        func_80058A74();
    }
    temp_v0 = func_800599B4(0, D_80097D50, 0, 0x320, temp_s1);
    *arg0 = temp_v0;
    if (temp_v0 != 0) {
        if (D_80097D58 == 0) {
            func_80058B58();
        }
    } else {
        if (*temp_s1 != D_80097A10) {
            *arg0 = func_800598E0(0);
            D_80097D50 = -1;
            if (D_80097D58 == 0) {
                func_80058B58();
            }
            func_80058A74();
            D_80097D58 = 1;
            if (D_80097D4C != 0) {
                D_80097D4C = 0;
                *arg0 = func_80059650(D_80097D66, D_80097D68, &D_80097D6C, &D_80097D7C, &D_800D2E40);
            } else {
                temp_v0_2 = func_8005969C(&D_800D2E40);
                *arg0 = temp_v0_2;
                if (temp_v0_2 == 0) {
                    func_80050684(arg0);
                }
            }
            func_80058B58();
            D_80097D58 = 0;
        } else {
            if (D_80097D58 == 0) {
                func_80058B58();
            }
            func_800500CC(temp_s1);
            func_8002D0E0(D_80097A14);
        }
        func_80055AB4(temp_s1);
    }
}
