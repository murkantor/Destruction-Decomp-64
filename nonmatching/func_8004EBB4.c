extern u8 D_80097335;
extern s32 D_800958A8;
void func_80063564(s32, s32);

s32 func_8004EBB4(u8 *obj) {
    s32 id;

    if (D_80097335) {
        *(s32 *)(obj + 0x10) = 0;
        id = 0x26;
    } else {
        *(s32 *)(obj + 0x10) = D_800958A8;
        id = 0x13;
    }
    func_80063564(id, 1);
    return 1;
}
