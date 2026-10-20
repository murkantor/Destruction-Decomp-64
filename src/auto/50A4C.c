#include "common.h"
#include "m2c.h"

s32 *Mem_Alloc(M2C_UNK);                            /* extern */
M2C_UNK func_8002D0E0(s32);                         /* extern */
M2C_UNK Save_CopyToGlobal(s32 *);                       /* extern */
M2C_UNK func_80050684(s32 *);                       /* extern */
M2C_UNK Mem_Free(s32 *);                       /* extern */
M2C_UNK Cont_ReadAll();                            /* extern */
M2C_UNK Pad_StartRead();                            /* extern */
s32 Pak_SetGameInfo(u16, s32, M2C_UNK *, M2C_UNK *, M2C_UNK *); /* extern */
s32 Pak_Init(M2C_UNK *);                       /* extern */
s32 Pak_DeleteFile(M2C_UNK);                         /* extern */
s32 Pak_ReadFile(M2C_UNK, s32, M2C_UNK, M2C_UNK, s32 *); /* extern */
extern s32 D_80097A10;
extern s32 D_80097A14;
extern u8 D_80097D4C;
extern s32 D_80097D50;
extern u8 D_80097D58;
extern u16 D_80097D66;
extern s32 D_80097D68;
extern M2C_UNK D_80097D6C;
extern M2C_UNK D_80097D7C;
extern M2C_UNK gSiMesgQueue;

void func_8004FE4C(s32 *arg0) {
    s32 *temp_s1;
    s32 temp_v0;
    s32 temp_v0_2;

    temp_s1 = Mem_Alloc(0x320);
    if (D_80097D58 == 0) {
        Cont_ReadAll();
    }
    temp_v0 = Pak_ReadFile(0, D_80097D50, 0, 0x320, temp_s1);
    *arg0 = temp_v0;
    if (temp_v0 != 0) {
        if (D_80097D58 == 0) {
            Pad_StartRead();
        }
    } else {
        if (*temp_s1 != D_80097A10) {
            *arg0 = Pak_DeleteFile(0);
            D_80097D50 = -1;
            if (D_80097D58 == 0) {
                Pad_StartRead();
            }
            Cont_ReadAll();
            D_80097D58 = 1;
            if (D_80097D4C != 0) {
                D_80097D4C = 0;
                *arg0 = Pak_SetGameInfo(D_80097D66, D_80097D68, &D_80097D6C, &D_80097D7C, &gSiMesgQueue);
            } else {
                temp_v0_2 = Pak_Init(&gSiMesgQueue);
                *arg0 = temp_v0_2;
                if (temp_v0_2 == 0) {
                    func_80050684(arg0);
                }
            }
            Pad_StartRead();
            D_80097D58 = 0;
        } else {
            if (D_80097D58 == 0) {
                Pad_StartRead();
            }
            Save_CopyToGlobal(temp_s1);
            func_8002D0E0(D_80097A14);
        }
        Mem_Free(temp_s1);
    }
}
