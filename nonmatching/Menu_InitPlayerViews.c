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
void CarSelect_ShowMulti(void);


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
        CarSelect_ShowMulti();
    }
}
/* agent54: volatile D_80097350 + loop bound fixes the store order (from agent49 v17/D). Left:
 * the original passes args 5 and 6 with sw a1 (a GPR copy of the mfc1'd 0.0f) and loads
 * D_800958DC after the mfc1. Tried literals, inline helpers (with const or variable params give
 * move a1,zero / sw zero), f32 array, s32 bit-pattern params, z set before the loop. */
/* agent58: unchanged. expand_call stores stack args before loading reg args, so sw a1 for args 5/6
 * means the original's stack-arg source was already a1 at cse time. Tried a Vec3f-by-value prototype,
 * inline wrappers taking (x), (a, x, m), (x, m) with const and variable args, z/m order: no gain. */
/* agent59: unchanged. The trick that fixed Car_UpdatePhysics (an inline wrapper
 * that passes one f32 param twice) gives move a1,zero / sw zero here, and a
 * union bit-cast with an s32 asm-label prototype folds to constants too. The
 * stack args need a GPR-class pseudo P = mfc1 Z that is also a1's source. */
/* agent63: unchanged (12 diff lines). Findings: (1) i=0: combine re-emits it at the copied loop test, then sched1
 * (bottom-up, priority 1) hoists it to the top of the block; the original has it as the first insn of the
 * fallthrough (reorg's eager fill puts it in the blez slot), i.e. after the test. "if (n > 0) { i = 0; do ... }"
 * gives that (move t5,zero in the slot) but changes the struct-copy schedule and the t-register numbering (49 diffs).
 * (2) Hard-register locals: f32 z0 = 0.0f; register f32 z asm("$5"); z = z0; func(D, z, z, z, z, z, m) gives
 * "sw a1,16/20(sp)" for the stack args, but then a2/a3 get mfc1 from z0 (CSE prefers the pseudo); brute force of
 * z/z0 per argument (mpb/) and z0 in $f0 did not get both. Wrapping the D_80097350 store in do{}while(0), moving
 * the sel load, a V2/V3 struct-by-value or union prototype: worse. */
