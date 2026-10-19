extern u8 D_80096ED0[12];
extern s32 D_800958A4;
extern s32 D_800958A8;
extern s32 D_800958B4;
extern s32 D_800958C0;
s32 rand(void);

void func_8004396C(void) {
    s32 n;
    s32 sel = -1;
    s32 i;
    s32 k;
    n = rand() % 8;

    if (n == 0) {
        n = 1;
    }
    k = n;
    do {
        for (i = 0; i < 12; i++) {
            if (D_80096ED0[i] == 0) {
                if (--k == 0) {
                    sel = i;
                    D_80096ED0[i] = 1;
                }
            }
        }
    } while (sel == -1);
    if (sel >= 12) {
        sel = 11;
    }
    D_800958A4 = 1;
    D_800958A8 = 1;
    D_800958B4 = sel;
    D_800958C0 = rand() % 12;
}
