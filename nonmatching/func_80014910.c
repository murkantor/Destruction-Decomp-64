extern f64 D_800B85F0;
extern f64 D_800B85F8;

void func_80014910(u8 *obj) {
    u8 *p = *(u8 **)(obj + 0x38);

    *(f32 *)(p + 0x10) += D_800B85F0;
    if (*(f32 *)(p + 0x10) > D_800B85F8) {
        *(f32 *)(p + 0x10) = 0.0f;
    }
}
