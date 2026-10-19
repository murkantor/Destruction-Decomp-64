u8 *func_80063ED0(s32);

s32 func_80063E28(s32 id) {
    s32 end = *(s32 *)(func_80063ED0(id) + 0x8);
    s32 size = end - *(s32 *)(func_80063ED0(id) + 0x4);

    if (id == 0) {
        return 0;
    }
    return size + id;
}
