#include "common.h"
#include "m2c.h"

M2C_UNK func_80019790(u8, void *, s32);             /* extern */
s32 Res_Get(s32);                             /* extern */
s32 func_80063E28(s32);                             /* extern */
extern s8 D_80083AE8;
extern M2C_UNK D_80083AEC;
extern u8 D_800958A1;
extern s32 D_800958A8;
extern s32 D_800958B4;
extern void *gGame;
extern s8 D_801242B8;
extern s32 D_801244D0;
extern void *D_801252F0;
extern s32 D_80125B68;
extern s32 D_80126138;
extern s32 D_80130248;
extern u8 D_80130638;

void func_800123C0(void) {
    s32 temp_v0;
    void *temp_a1;

    if (M2C_FIELD(gGame, s16 *, 0xC) == 2) {
        temp_a1 = M2C_FIELD(M2C_FIELD(gGame, void **, 0x1100), void **, 0x70);
        temp_v0 = M2C_FIELD(temp_a1, s32 *, 0xC);
        D_80130638 = D_800958A1;
        D_80130248 = D_800958B4;
        D_801252F0 = temp_a1;
        D_80125B68 = temp_v0;
        D_80126138 = temp_v0 + 4;
        if (D_800958A8 != 3) {
            func_80019790(D_800958A1, temp_a1, D_800958A8);
        }
        D_801244D0 = func_80063E28(Res_Get(*((__typeof__(D_80083AEC) *)((s8 *)&D_80083AEC + (D_80130248 * 4)))));
        D_801242B8 = 0;
        D_80083AE8 = 1;
    }
}
