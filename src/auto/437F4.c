#include "common.h"
#include "m2c.h"

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

void CarSelect_Show(void);
void CarSelect_ShowMulti(void);


void Menu_InitPlayerViews(void) {
    Mat9x4 mats;
    Ptr4 ptrs;
    f32 *sel;
    s32 i;
    volatile s32 pad;

    mats = D_800BA5D4;
    do { ptrs = D_800BA664; } while (0);
    sel = ptrs.v[D_800958A4[0] - 1];
    D_80097350 = 0x18;
    if (*(volatile s32 *)D_800958A4 > 0) {
    do { i = 0; } while (0);
    do {
        D_801242C0[i] = mats.v[i];
        D_801242C0[i].x1C = sel[i];
        i++;
    } while (i < *(volatile s32 *)D_800958A4); }
    { s32 *p = &D_800958DC; f32 z = 0.0f; f32 m = -1.0f; func_8006300C(*p, z, z, z, z, z, m); }
    if (D_800958A4[0] == 1) {
        CarSelect_Show();
    } else {
        CarSelect_ShowMulti();
    }
}
/* agent71: MATCH. Three independent fixes on top of agent69's draft (see the notes there for the call trick):
 * 1. `do { ptrs = D_800BA664; } while (0);` - the LOOP_BEG/LOOP_END notes are a full sched1 barrier, so the
 *    16-byte ptrs copy stays above the `lw v1, D_800958A4` index load. Without it, sched1 (a reverse list
 *    scheduler) blocks the index load one cycle at T-9 (memory-unit hazard after the volatile sw at T-7), the
 *    ptrs copy (priority 1, no dependents) takes T-9 and ends up below the load, which then changes the block
 *    move's scratch registers (v1 live). An empty `do { } while (0);` macro statement between the two also works.
 * 2. `if (n > 0) { do { i = 0; } while (0); do { ... } while (++i < n); }` - i = 0 must be the first insn of the
 *    fallthrough block so reorg's eager fill puts it in the blez delay slot (nothing before the volatile lw a0
 *    can be pulled back, and a `for` loop's i = 0 is moved by combine to just before the branch and then hoisted
 *    by sched1). The do-while(0) around i = 0 counts that ref with loop weight 2, giving i 8 refs; with 7 refs
 *    its allocation priority (2*7/24) falls below the D_801242C0 base pointer's (2*7/19) and it gets t5.
 * 3. `volatile s32 pad;` - the original frame is 0xD0 (vars=168) although mats+ptrs are only 160 bytes; a
 *    for-form guard leaves a dead compare pseudo (combine emits a USE for it) that reload spills to an 8-byte
 *    slot. Here the guard is a plain `n > 0` branch, so an unused volatile local supplies the slot instead
 *    (`char unused[1];` did not add a slot in this function; `s32 pad[2];` also works).
 * The volatile cast `*(volatile s32 *)D_800958A4` keeps the loop bound address in a register (a1/t6) exactly
 * like the original; a struct-pointer local (`Glob *g = &D_800958A4`) instead gets loaded at function entry. */
