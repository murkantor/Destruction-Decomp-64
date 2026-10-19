typedef struct {
    u8 pad[0x10];
    s32 value;
} MenuItem;
extern s32 D_80125BE4[];
extern s32 D_80097350;
void Timer_Start(void *, s64);
void func_80040FD8(s32, s32);

s32 func_80040A28(MenuItem *item) {
    s32 i;

    Timer_Start(item, 300000000LL);
    i = item->value;
    if (i == 0) {
        return 0;
    }
    func_80040FD8(D_80125BE4[i], D_80125BE4[--i]);
    item->value = i;
    return 1;
}
