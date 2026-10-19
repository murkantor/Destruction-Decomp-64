#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; u8 pad4[0xC]; } Ent10;
typedef struct { s32 x0; s32 x4; s32 x8; s32 xC; } Race;
typedef struct { u32 cup; u8 flags[16]; } Progress;
typedef struct { s32 v[4]; } S4;
extern s32 D_80095CD4[];
extern Ent10 D_80126188[];
extern s32 D_800958C0;
extern s32 D_800958B0[];
extern Race *D_80095CA8[];
extern s32 D_80095CB8[];
extern s32 D_801252F4;
extern u8 D_800972F0;
extern Progress D_80097A14;
extern u8 D_80097336;
extern u8 D_80097337[];
extern s32 D_80097338;
extern S4 D_800BB228;
extern s32 D_800958B4;
s32 func_8001B298(s32);
void func_8002D0E0(u32);
void func_80063564(s32, s32);
void func_80063380(s32);
void func_800087C8(void);
void Msg_Queue(s32, s32, s32, s32);

s32 Race_ProcessResult(void) {
    s32 i;
    s32 idx;
    s32 rank;
    u8 ok;

    {
        s32 k;
        for (k = 0; k < 12; k++) {
            D_80095CD4[k] += D_80126188[k].x0;
        }
    }
    idx = func_8001B298(D_800958C0);
    if (D_80095CD4[idx] < D_80095CA8[D_800958B0[0]][D_801252F4].x8) {
        goto fail;
    }
    D_801252F4++;
    if (D_80095CB8[D_800958B0[0]] - 1 < D_801252F4) {
        ok = 0;
        rank = 0;
        for (i = 0; i < 12; i++) {
            if (i != idx && D_80095CD4[i] > D_80095CD4[idx]) {
                rank++;
            }
        }
        if (rank < D_80095CA8[D_800958B0[0]][D_801252F4 - 1].xC) {
            ok = 1;
        }
        D_800972F0 = 0;
        if (!ok) {
            goto fail;
        }
        if (D_800958B0[0] == D_80097A14.cup) {
            D_80097A14.cup = D_800958B0[0] + 1;
            D_80097336 = 1;
        }
        if (D_80097A14.cup >= 4) {
            D_80097A14.cup = 3;
            D_800972F0 = 1;
            D_80097336 = 0;
        }
        if (rank == 0) {
            S4 t = D_800BB228;
            if (!D_80097A14.flags[t.v[D_800958B0[0]]]) {
                D_80097337[0] = D_80097A14.flags[t.v[D_800958B0[0]]] = 1;
                D_80097338 = t.v[D_800958B0[0]];
            }
        }
        func_8002D0E0(D_80097A14.cup);
        func_80063564(0x29, 1);
        return 1;
    }
    func_80063380(2);
    func_80063564(0x1A, 1);
    D_800958B4 = D_80095CA8[D_800958B0[0]][D_801252F4].x0;
    func_800087C8();
    Msg_Queue(0xE9, 100, 1, 0);
    return 1;
fail:
    func_80063564(0x28, 1);
    func_800087C8();
    Msg_Queue(0xCB, 100, 1, 0);
    return 1;
}
