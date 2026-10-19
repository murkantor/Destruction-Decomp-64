#include "common.h"
#include "m2c.h"

M2C_UNK func_8002D088(void *);                      /* extern */
M2C_UNK func_800635B0(void *, M2C_UNK *);           /* extern */
M2C_UNK func_800638C0(s32);                         /* extern */
M2C_UNK func_800641F0(void *);                      /* extern */
s16 func_80064D70();                                /* extern */
extern M2C_UNK D_800D31D0;
extern s32 gGame;

s32 func_80063488(void *arg0, s16 arg1) {
    M2C_FIELD(arg0, s16 *, 0) = 0x3E9;
    M2C_FIELD(arg0, s16 *, 2) = func_80064D70();
    M2C_FIELD(arg0, void **, 4) = arg0;
    M2C_FIELD(arg0, s16 *, 0xC) = arg1;
    M2C_FIELD(arg0, s32 *, 0x10F8) = 0;
    func_800635B0(arg0 + 0x1058, &D_800D31D0);
    func_800638C0(gGame + 0x1058);
    func_8002D088(arg0);
    func_800641F0(arg0 + 0x334);
    return 0;
}
