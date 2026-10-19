#include "common.h"
#include "m2c.h"

typedef struct {
    s8 x0;
    u8 pad1[0x32];
} Slot;

extern void *gUi[];
extern s32 D_800958A4;
extern s32 D_800958B4;
extern s32 D_80096038[];
extern s32 D_800972C0[];
extern Slot D_80123922[];
void Ui_SetElementPos(void *, s32, s32, f32, f32);

void Menu_LayoutResultRows(f32 y, f32 *xs, s32 num) {
    s32 i;
    s32 j;
    s32 cnt;
    s32 k;
    s32 n;

    cnt = D_800958A4;
    n = num - 1;
    for (i = 0; i < n; i++) {
        cnt -= (D_80123922[i].x0 != -1);
    }
    for (i = 0; i < 12; i++) {
        Ui_SetElementPos(gUi[0], 2, D_800972C0[i], 25.0f, -20.0f);
    }
    for (i = 0; i < D_800958A4; i++) {
        Ui_SetElementPos(gUi[0], 2, i + 0xC1, 25.0f, -20.0f);
    }
    for (j = 0; (D_80096038[D_800958B4] < 7) ? (j < D_80096038[D_800958B4] - cnt) : (j < 6 - cnt); j++) {
        i = j + n;
        Ui_SetElementPos(gUi[0], 2, D_800972C0[i], y, xs[j]);
        if (D_80123922[i].x0 != -1) {
            Ui_SetElementPos(gUi[0], 2, D_80123922[i].x0 + 0xC1, y - 7.0f, xs[j] - 3.0f);
        }
    }
    i = D_80096038[D_800958B4];
    if (i < 7) {
        j = i - cnt;
    } else {
        j = 6 - cnt;
    }
    cnt = j + n;
    for (i = n; i < 12; i++) {
        if (D_80123922[i].x0 != -1) {
            k = (i < cnt) ? cnt : i;
            Ui_SetElementPos(gUi[0], 2, D_800972C0[k], y, xs[j]);
            if (D_80123922[k].x0 != -1) {
                Ui_SetElementPos(gUi[0], 2, D_80123922[k].x0 + 0xC1, y - 7.0f, xs[j] - 3.0f);
            }
            j++;
            cnt++;
        }
    }
}
