#include "common.h"
#include "m2c.h"

M2C_UNK func_80024BA0(s32, s32, s32);               /* extern */
M2C_UNK Track_LoadResources(s32, s32);                    /* extern */
s32 Mem_AllocAligned(s32, M2C_UNK, M2C_UNK);           /* extern */
M2C_UNK Res_Load(s32, s32);                    /* extern */
s32 Res_GetSize();                                /* extern */
s32 func_80063D8C(s32);                             /* extern */
s32 func_80063DAC(s32);                             /* extern */

s32 func_80024FD4(s32 arg0) {
    s32 temp_s0;
    s32 temp_s2;
    s32 temp_v0;

    temp_v0 = Mem_AllocAligned(Res_GetSize(arg0), 0x3F, 0);
    Res_Load(arg0, temp_v0);
    temp_s2 = func_80063D8C(arg0);
    temp_s0 = (func_80063DAC(arg0) - temp_s2) + temp_v0;
    func_80024BA0(temp_s0, temp_s2, temp_v0);
    Track_LoadResources(temp_s0, temp_v0);
    return 0;
}

M2C_UNK Track_RelocateDisplayList(M2C_UNK *, void *, s32);      /* extern */
s32 Mem_AllocAligned(s32, M2C_UNK, M2C_UNK);           /* extern */
M2C_UNK Res_Load(s32, s32);                    /* extern */
s32 Res_GetSize();                                /* extern */
s32 func_80063D8C(s32);                             /* extern */
s32 func_80063DAC(s32);                             /* extern */
s32 func_80063E0C(s32, s32, s32);                   /* extern */
extern M2C_UNK D_80125460;

s32 func_80025064(s32 arg0) {
    s32 temp_s1;
    s32 temp_v0;
    void *temp_s0;

    temp_v0 = Mem_AllocAligned(Res_GetSize(arg0), 0x3F, 0);
    Res_Load(arg0, temp_v0);
    temp_s1 = func_80063D8C(arg0);
    temp_s0 = (func_80063DAC(arg0) - temp_s1) + temp_v0;
    M2C_FIELD(temp_s0, s32 *, 4) = func_80063E0C(M2C_FIELD(temp_s0, s32 *, 4), temp_s1, temp_v0);
    M2C_FIELD(temp_s0, s32 *, 0xC) = func_80063E0C(M2C_FIELD(temp_s0, s32 *, 0xC), temp_s1, temp_v0);
    Track_RelocateDisplayList(&D_80125460, temp_s0, temp_v0);
    return 0;
}
