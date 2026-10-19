void func_80064E08(u8 *, u8 *);

u8 *func_800651B8(u8 *obj, u8 *src) {
    s32 v = *src++;

    if (v & 0x80) {
        v = (v & 0x7F) << 8;
        v |= *src++;
        func_80064E08(obj, *(u8 **)(*(u8 **)(obj + 0x70) + 0x10) + v * 7);
    }
    return src;
}
