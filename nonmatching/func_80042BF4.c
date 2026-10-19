typedef struct { f32 m[7]; f32 x1C; f32 x20; } Mat9;
typedef struct { Mat9 v[4]; } Mat9x4;
typedef struct { f32 *v[4]; } Ptr4;
extern Mat9x4 D_800BA5D4;
extern Ptr4 D_800BA664;
extern s32 D_80097350;
extern s32 D_800958A4[];
extern s32 D_800958DC;
extern Mat9 D_801242C0[];
void func_8006300C(s32, f32, f32, f32, f32, f32, f32);
void func_80042F2C(void);
void func_800430E8(void);

static inline void setcam(f32 z, f32 m) {
    func_8006300C(D_800958DC, z, z, z, z, z, m);
}

void func_80042BF4(void) {
    Mat9x4 mats;
    Ptr4 ptrs;
    f32 *sel;
    s32 i;

    mats = D_800BA5D4;
    ptrs = D_800BA664;
    sel = ptrs.v[D_800958A4[0] - 1];
    D_80097350 = 0x18;
    for (i = 0; i < D_800958A4[0]; i++) {
        D_801242C0[i] = mats.v[i];
        D_801242C0[i].x1C = sel[i];
    }
    setcam(0.0f, -1.0f);
    if (D_800958A4[0] == 1) {
        func_80042F2C();
    } else {
        func_800430E8();
    }
}
