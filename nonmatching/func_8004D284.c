extern u32 D_800958B4;
void func_80063564(s32, s32);

s32 func_8004D284(void) {
    s32 id;

    if (D_800958B4 < 4) {
        id = 0x22;
    } else {
        id = 0x23;
    }
    func_80063564(id, 1);
    return 1;
}
