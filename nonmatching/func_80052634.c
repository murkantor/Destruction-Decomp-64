extern s32 D_800BCA30;
extern s32 D_800BCA34;
extern s32 D_800BCA38;
extern s32 D_800BCA3C;
extern s32 D_800BCA40;
extern s32 D_800BCA44;
extern s32 D_800BCA48;
extern s32 D_800BCA4C;
extern s32 D_800BCA50;
extern s32 D_800BCA54;
extern s32 D_800B048C;
s32 Mem_AllocAlt(s32);
void Mem_Free(s32);
s32 func_80051AC4(void);

s32 func_80052634(s32 src, s32 srcLen, s32 dst, s32 dstLen) {
    s32 r;

    D_800BCA40 = src;
    D_800BCA44 = 0;
    D_800BCA48 = srcLen;
    D_800BCA38 = 0;
    D_800BCA3C = 0;
    D_800BCA34 = 0;
    D_800BCA50 = 0;
    D_800BCA54 = dstLen;
    D_800BCA4C = dst;
    D_800BCA30 = Mem_AllocAlt(0x8000);
    if (D_800BCA30 == 0) {
        return -1;
    }
    D_800B048C = 0;
    r = func_80051AC4();
    Mem_Free(D_800BCA30);
    if (D_800BCA50 != D_800BCA54) {
        return -1;
    }
    if (D_800BCA44 != D_800BCA48) {
        return -1;
    }
    if (r != 0) {
        return -1;
    }
    if (D_800B048C != 0) {
        r = -1;
    }
    return r;
}
