extern u8 D_800B0BC4[];

void func_80059540(u8 *src, u8 *dst, s32 n) {
    s32 i;
    u8 ch;
    s32 j;

    for (i = 0; i < n; i++) {
        ch = *src;
        for (j = 0; j < 0x68; j += 2) {
            if (ch == D_800B0BC4[j]) {
                *dst = D_800B0BC4[j + 1];
                break;
            }
        }
        src++;
        dst++;
    }
}
