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
    { s32 *p = &D_800958DC; f32 z = 0.0f; f32 m = -1.0f; func_8006300C(*p, z, z, z, z, z, m); }
    if (D_800958A4[0] == 1) {
        Menu_SetupTrackSelect();
    } else {
        CarSelect_ShowMulti();
    }
}
/* agent69: call fixed. Passing D_800958DC through a pointer local (s32 *p = &D_800958DC; func(*p, ...))
 * keeps the arg as (mem (reg p)) at expand (rtx_cost <= 2, so no precompute into a pseudo); a0 is then
 * loaded directly, sched1 puts the a1 = z copy before the two stack stores, and local-alloc's
 * optimize_reg_copy_1 rewrites the stack stores and a2/a3 to use a1: sw a1 / move a2,a1 / move a3,a1
 * exactly as the original. Only diff left: move t2,zero (i = 0) sits after lw v1 instead of in the blez slot.
 * Why: reorg cannot pull an insn back across a volatile mem ref (resource_conflicts_p), so i = 0 must be
 * after the volatile loop-bound load or in the fallthrough. i has 2 sets so it is never "birthing" in
 * sched1 (priority stays 1) and gets hoisted to T-9, where it is also the filler that keeps the ptrs copy
 * above the sel index load (30 is blocked one cycle by the volatile store's memory unit). The if/do form
 * puts i = 0 in the fallthrough (correct slot) but loses that filler (ptrs copy goes below the index load)
 * and i drops to 7 refs (priority below base/offset pseudos: t5 instead of t2). Tried: int/s32/u32/register,
 * scoped for, sel/store/init orders, i reused for the sel index (gets i=0 in the slot but i takes the first
 * value in t2), idx/n locals before/after the volatile store, guard forms (> 0, 0 <, i < with double init),
 * goto loop, pointer-walk sel, hard-reg i: none better than this. */
