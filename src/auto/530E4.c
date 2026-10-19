#include "common.h"
#include "m2c.h"

extern s32 D_800BCA30, D_800BCA34, D_800BCA38, D_800BCA3C, D_800BCA40, D_800BCA44, D_800BCA48, D_800BCA4C, D_800BCA50, D_800BCA54;
extern s32 D_800B048C;
void *Mem_AllocAlt(u32 size);
void Mem_Free(void *ptr);
s32 inflate(void);


s32 Decompress_Stream(s32 a0, s32 a1, s32 a2, s32 a3) {
    s32 r;
    D_800BCA44 = 0;
    D_800BCA48 = 0;
    D_800BCA40 = (s32)Mem_AllocAlt(0x4000);
    if (D_800BCA40 == 0) {
        return -1;
    }
    D_800BCA38 = 0;
    D_800BCA3C = a1;
    D_800BCA34 = a0;
    D_800BCA50 = 0;
    D_800BCA54 = a3;
    D_800BCA4C = a2;
    D_800BCA30 = (s32)Mem_AllocAlt(0x8000);
    if (D_800BCA30 == 0) {
        Mem_Free((void *)D_800BCA40);
        return -1;
    }
    D_800B048C = 0;
    r = inflate();
    Mem_Free((void *)D_800BCA40);
    Mem_Free((void *)D_800BCA30);
    if (D_800BCA50 != D_800BCA54) return -1;
    if (D_800BCA38 != D_800BCA3C) return -1;
    if (D_800BCA44 != D_800BCA48) return -1;
    if (r != 0 || D_800B048C != 0) return -1;
    return 0;
}
