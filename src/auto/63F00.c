#include "common.h"
#include "m2c.h"

M2C_UNK func_8002D188();                            /* extern */
M2C_UNK func_80062D9C();                            /* extern */
M2C_UNK func_800635B0(void *, M2C_UNK);             /* extern */
u64 Time_Get(M2C_UNK *);                       /* extern */
M2C_UNK Res_ComputeSizes();                            /* extern */
s16 func_80064D70();                                /* extern */
M2C_UNK func_80079BF4(u32);                         /* extern */
extern M2C_UNK gGameTime;

s32 func_80063300(void *arg0) {
    M2C_FIELD(arg0, s16 *, 0) = 0x3E8;
    M2C_FIELD(arg0, s16 *, 2) = func_80064D70();
    M2C_FIELD(arg0, void **, 4) = arg0;
    M2C_FIELD(arg0, s8 *, 0x4BD3C) = 1;
    M2C_FIELD(arg0, s8 *, 0x38) = 0;
    func_80062D9C();
    func_800635B0(arg0 + 0x10, 0);
    Res_ComputeSizes();
    func_80079BF4((u32) Time_Get(&gGameTime));
    func_8002D188();
    return 0;
}
