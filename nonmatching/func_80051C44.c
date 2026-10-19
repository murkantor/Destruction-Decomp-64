extern s32 D_800BCA1C;
extern s32 D_800BCA20;
void func_80052334(s32);

s32 func_80051C44(void) {
    if (D_800BCA1C == 0) {
        return 0;
    }
    func_80052334(D_800BCA20);
    func_80052334(D_800BCA1C);
    D_800BCA1C = 0;
    D_800BCA20 = 0;
    return 0;
}
