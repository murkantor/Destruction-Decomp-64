extern u64 D_800C0700;
extern s32 D_800C0708;
extern s32 D_800C070C;
void osGetTime(void);

void func_80062E4C(void) {
    osGetTime();
    D_800C0700 = 0;
    D_800C0708 = 0;
    D_800C070C = 0;
}
