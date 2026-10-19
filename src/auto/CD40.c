#include "common.h"
#include "m2c.h"

typedef struct { s32 v[4]; } Ids4;
typedef struct {
    s32 cur;
    s32 start;
    u8 counting;
    u8 inited;
    u8 padA[6];
    u64 t0;
    u8 x18;
    u8 x19;
    u8 pad1A[2];
    f32 x1C;
    f32 x20;
    f32 x24;
    f32 x28;
    u8 pad2C[4];
    u64 x30;
    u64 x38;
    s32 x40;
    s32 x44;
    u64 x48;
} Score;
typedef struct { u8 pad0[0x18]; s32 x18; s32 x1C; s32 x20; } PlayerInfo;
typedef struct { PlayerInfo *x0; u8 pad4[0x64]; } PlayerSlot;
typedef struct { u8 pad0[0x10]; s32 x10; } World;
typedef struct { u8 pad0[0x10A8]; u8 x10A8[8]; } GameT;
typedef struct { u8 pad0[0x1100]; World *x1100; } GameH;

extern GameH *gGame;
extern void *gUi[];
extern Ids4 D_800B81A4;
extern Ids4 D_800B81B4;
extern Score D_8011EF08[];
extern PlayerSlot D_800958F0[];
extern char D_801255D0[][5];
extern char D_80123BA8[][5];
extern char D_800B81C4[];
extern f64 D_800B81C8;
extern f64 D_800B81D0;
extern f64 D_800B81D8;
extern f64 D_800B81E0;
extern f64 D_800B81E8;
extern u64 D_8007EF48;

int sprintf(char *, const char *, ...);
void Ui_GetElementPos(f32 *, void *, s32, s32);
void Ui_SetElementPos(void *, s32, s32, f32, f32);
u64 Time_Get(void *);

s32 Hud_UpdateScoreCounter(void *arg0, s8 p)
{
    f32 pos[3];
    Ids4 idsA = D_800B81A4;
    Ids4 idsB = D_800B81B4;
    World *w = gGame->x1100;
    s32 target;
    u64 now;
    u64 zero = 0;

    if (!D_8011EF08[p].inited) {
        D_8011EF08[p].cur = D_800958F0[p].x0->x18;
        D_8011EF08[p].counting = 0;
        D_8011EF08[p].inited = 1;
        sprintf(D_801255D0[p], D_800B81C4, D_8011EF08[p].cur);
        D_8011EF08[p].x30 = zero;
        D_8011EF08[p].x38 = zero;
        D_8011EF08[p].x48 = zero;
        D_8011EF08[p].x18 = 0;
        D_8011EF08[p].x19 = 0;
        D_8011EF08[p].x40 = 0;
        Ui_GetElementPos(pos, gUi[0], 2, idsB.v[p]);
        D_8011EF08[p].x24 = pos[0];
        D_8011EF08[p].x28 = pos[1];
        Ui_SetElementPos(gUi[0], 2, idsB.v[p], 350.0f, pos[1]);
        Ui_GetElementPos(pos, gUi[0], 0, idsA.v[p]);
        D_8011EF08[p].x1C = pos[0];
        D_8011EF08[p].x20 = pos[1];
        sprintf(D_80123BA8[p], D_800B81C4, D_8011EF08[p].x40);
        Ui_SetElementPos(gUi[0], 0, idsA.v[p], 350.0f, pos[1]);
    }

    if (w->x10 == 1)
        target = D_800958F0[p].x0->x18 + D_800958F0[p].x0->x20;
    else
        target = D_800958F0[p].x0->x18 + D_800958F0[p].x0->x1C;

    now = Time_Get(((GameT *)gGame)->x10A8);
    if (D_8011EF08[p].cur < target) {
        if (!D_8011EF08[p].counting) {
            D_8011EF08[p].t0 = now;
            D_8011EF08[p].counting = 1;
            D_8011EF08[p].start = D_8011EF08[p].cur;
            D_8011EF08[p].x30 = now + D_800B81C8;
            D_8011EF08[p].x38 = now + D_800B81D0;
            D_8011EF08[p].x48 = now + D_800B81D8;
            D_8011EF08[p].x18 = 1;
            D_8011EF08[p].x19 = 0;
            D_8011EF08[p].x40 = target;
            D_8011EF08[p].x44 = target - D_8011EF08[p].start;
            if (D_8011EF08[p].x44 > 100)
                D_8011EF08[p].x44 = 100;
        }
        {
            s32 d = (now - D_8011EF08[p].t0) / D_8007EF48 + 1;
            D_8011EF08[p].cur = D_8011EF08[p].start + d;
        }
        if (target < D_8011EF08[p].cur)
            D_8011EF08[p].cur = target;
        sprintf(D_801255D0[p], D_800B81C4, D_8011EF08[p].cur);
        if (D_8011EF08[p].cur == target)
            D_8011EF08[p].counting = 0;
    }

    if (!D_8011EF08[p].x18)
        return 0;

    if (!D_8011EF08[p].counting && now >= D_8011EF08[p].x30) {
        Ui_SetElementPos(gUi[0], 0, idsA.v[p], 350.0f, D_8011EF08[p].x20);
        Ui_SetElementPos(gUi[0], 2, idsB.v[p], 350.0f, D_8011EF08[p].x28);
        D_8011EF08[p].x18 = 0;
        D_8011EF08[p].x40 = 0;
    }
    if (D_8011EF08[p].x18 && now >= D_8011EF08[p].x38) {
        if (D_8011EF08[p].x19) {
            Ui_SetElementPos(gUi[0], 0, idsA.v[p], 350.0f, D_8011EF08[p].x20);
            Ui_SetElementPos(gUi[0], 2, idsB.v[p], 350.0f, D_8011EF08[p].x28);
            D_8011EF08[p].x19 = 0;
        } else {
            Ui_SetElementPos(gUi[0], 0, idsA.v[p], D_8011EF08[p].x1C, D_8011EF08[p].x20);
            Ui_SetElementPos(gUi[0], 2, idsB.v[p], D_8011EF08[p].x24, D_8011EF08[p].x28);
            D_8011EF08[p].x19 = 1;
        }
        D_8011EF08[p].x38 = now + D_800B81E0;
    }
    if (D_8011EF08[p].x40 != target) {
        if (now < D_8011EF08[p].x48) {
            D_8011EF08[p].x44 += target - D_8011EF08[p].x40;
        } else {
            D_8011EF08[p].x44 = target - D_8011EF08[p].x40;
            D_8011EF08[p].x48 = now + D_800B81E8;
        }
        D_8011EF08[p].x40 = target;
    }
    sprintf(D_80123BA8[p], D_800B81C4, D_8011EF08[p].x44);
    return 0;
}
