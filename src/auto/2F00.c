#include "common.h"
#include "m2c.h"

void *Mem_Alloc(M2C_UNK);                           /* extern */
void *Mem_AllocAlignedAlt(s32, M2C_UNK, M2C_UNK);         /* extern */
M2C_UNK Mem_Free(void *);                      /* extern */
s32 Res_Get(s32);                             /* extern */
M2C_UNK Res_Load(s32, void *);                 /* extern */
s32 Res_GetSize();                                /* extern */
extern void *gGame;

s32 func_80002300(s32 arg0) {
    s32 temp_a0;
    void *temp_s0;
    void *temp_v0;
    void *temp_v0_2;

    temp_v0 = Mem_AllocAlignedAlt(Res_GetSize(), 7, 0);
    Res_Load(arg0, temp_v0);
    temp_s0 = Mem_Alloc(0x10);
    temp_v0_2 = Mem_Alloc(0x20);
    M2C_FIELD(temp_v0_2, s16 *, 0) = 8;
    M2C_FIELD(temp_v0_2, void **, 0x1C) = temp_s0;
    M2C_FIELD(temp_v0_2, s32 *, 0xC) = (s32) M2C_FIELD(temp_v0, s32 *, 4);
    M2C_FIELD(temp_v0_2, s32 *, 0x14) = (s32) M2C_FIELD(temp_v0, s32 *, 8);
    M2C_FIELD(M2C_FIELD(temp_v0_2, void **, 0x1C), f32 *, 0) = (f32) M2C_FIELD(temp_v0, f32 *, 0xC);
    M2C_FIELD(M2C_FIELD(temp_v0_2, void **, 0x1C), f32 *, 4) = (f32) M2C_FIELD(temp_v0, f32 *, 0x10);
    M2C_FIELD(M2C_FIELD(temp_v0_2, void **, 0x1C), f32 *, 8) = (f32) M2C_FIELD(temp_v0, f32 *, 0x14);
    M2C_FIELD(M2C_FIELD(temp_v0_2, void **, 0x1C), f32 *, 0xC) = (f32) M2C_FIELD(temp_v0, f32 *, 0x18);
    M2C_FIELD(M2C_FIELD(gGame, void **, 0x1100), void **, 0x2C) = temp_v0_2;
    temp_a0 = M2C_FIELD(temp_v0, s32 *, 0x1C);
    if (temp_a0 == 0) {
        M2C_FIELD(temp_v0_2, s32 *, 0x10) = 0;
    } else {
        M2C_FIELD(temp_v0_2, s32 *, 0x10) = Res_Get(temp_a0);
    }
    M2C_FIELD(temp_v0_2, s32 *, 0x18) = Res_Get(M2C_FIELD(temp_v0, s32 *, 0x20));
    M2C_FIELD(M2C_FIELD(gGame, void **, 0x1100), void **, 0x2C) = temp_v0_2;
    Mem_Free(temp_v0);
    return 0;
}
