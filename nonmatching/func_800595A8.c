extern u8 D_800B0BC4[];

void func_800595A8(u8 *src, u8 *dst, s32 n) {
    s32 i;
    s32 j;
    u8 ch;

    for (i = 0; i < n; i++) {
        ch = *src;
        for (j = 0; j < 0x34; j++) {
            if (ch == D_800B0BC4[j * 2 + 1]) {
                if (n == 4 && ch >= 0x34) {
                    *dst = ' ';
                } else {
                    *dst = D_800B0BC4[j * 2];
                }
                break;
            }
        }
        if (j == 0x34) {
            if (n == 16) {
                *dst = '-';
            } else {
                *dst = ' ';
            }
        }
        src++;
        dst++;
    }
}
