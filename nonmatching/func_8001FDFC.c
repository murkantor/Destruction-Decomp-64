extern s32 D_800958A8[];
extern s32 D_80096038[];
extern s32 D_80091AB4[];
extern s8 D_800D31CC;
extern u8 D_80124360;
extern s32 D_80095A70;

s32 Res_Get(s32);
void func_80063F78(void);
void func_80063F88(void);

void func_8001FDFC(void) {
    u8 used[12];
    s32 i;
    s32 j;
    s32 n;
    s32 count;
    u32 idx;
    u8 *p;

    i = 11;
    p = &used[11];
    for (; i >= 0; i--) {
        *p-- = 0;
    }
    D_80124360 = 0;
    n = D_80096038[D_800958A8[3]];
    if (D_800958A8[0] == 3) {
        if (D_800958A8[1] == 1) {
            n = 8;
        } else if (D_800958A8[1] == 0) {
            n = 8;
        } else if (D_800958A8[1] == 2) {
            n = (D_800958A8[-1] == 2) ? 10 : 4;
        } else if (D_800958A8[1] == 3) {
            n = 8;
        }
    }
    for (i = 0; i < D_800D31CC; i++) {
        if (D_800958A8[0] == 3 && D_800958A8[1] == 2) {
            func_80063F78();
        }
        Res_Get(D_80091AB4[D_800958A8[6 + i]]);
        idx = D_800958A8[6 + i];
        (idx < 12 ? used : used - 12)[idx] = 1;
        if (D_800958A8[0] == 3 && D_800958A8[1] == 2 && D_800958A8[-1] == 2) {
            for (j = 1; j < n / 2; j++) {
                func_80063F78();
                Res_Get(D_80091AB4[D_800958A8[6 + i]]);
            }
        }
    }
    count = D_800D31CC;
    if (D_800958A8[0] == 3 && D_800958A8[1] == 2) {
        func_80063F88();
        count = n;
    }
    if (D_800958A8[0] != 2 && count < n) {
        for (i = 0; count < n; i++) {
            if (used[i] == 0) {
                Res_Get(D_80091AB4[i]);
                count++;
            }
        }
    }
    D_80095A70 = count;
}
