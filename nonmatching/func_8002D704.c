u8 *func_80062F14(void);

s32 func_8002D704(void) {
    s32 found = 0;
    u8 *p = func_80062F14();
    s32 i;

    for (i = 0; i < 4; i++) {
        if (*(s32 *)(p + 0xC) & 0x10) {
            found = 1;
        }
        p += 0xB0;
    }
    return found;
}
