typedef struct { f32 m[7]; f32 x1C; f32 x20; } Mat9;
typedef struct { Mat9 v[4]; } Mat9x4;
typedef struct { f32 *v[4]; } Ptr4;
extern Mat9x4 D_800BA5D4;
extern Ptr4 D_800BA664;
extern volatile s32 D_80097350;
extern s32 D_800958A4[];
extern s32 D_800958DC;
extern Mat9 D_801242C0[];
void func_8006300C(s32, f32, f32, f32, f32, f32, f32);

void Menu_SetupTrackSelect(void);
void func_800430E8(void);


void Menu_InitPlayerViews(void) {
    Mat9x4 mats;
    Ptr4 ptrs;
    f32 *sel;
    s32 i;

    mats = D_800BA5D4;
    ptrs = D_800BA664;
    sel = ptrs.v[D_800958A4[0] - 1];
    D_80097350 = 0x18;
    for (i = 0; i < *(volatile s32 *)D_800958A4; i++) {
        D_801242C0[i] = mats.v[i];
        D_801242C0[i].x1C = sel[i];
    }
    { f32 z = 0.0f; f32 m = -1.0f; func_8006300C(D_800958DC, z, z, z, z, z, m); }
    if (D_800958A4[0] == 1) {
        Menu_SetupTrackSelect();
    } else {
        func_800430E8();
    }
}
/* agent54: volatile D_80097350 + loop bound fixes the store order (from agent49 v17/D). Left:
 * the original passes args 5 and 6 with sw a1 (a GPR copy of the mfc1'd 0.0f) and loads
 * D_800958DC after the mfc1. Tried literals, inline helpers (with const or variable params give
 * move a1,zero / sw zero), f32 array, s32 bit-pattern params, z set before the loop. */
