extern s32 D_800958A4;

s32 func_800267B4(u8 *obj, s32 value) {
    s32 found = 0;
    s32 n = (D_800958A4 == 2) ? 3 : 2;
    s32 i;

    for (i = 0; i < n; i++) {
        if (((s32 *)(obj + 0x3C))[i] == value) {
            found = 1;
            break;
        }
    }
    return found;
}
