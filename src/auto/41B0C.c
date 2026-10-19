#include "common.h"
#include "m2c.h"

typedef struct { u8 *x0; u8 pad[0x18]; } UiSlot;
typedef struct { u8 pad0[0x7588]; UiSlot slots[199]; u8 pad8B4C[0x10]; s16 count; } Ui40F0C;
extern Ui40F0C *gUi[];
extern u8 *D_800958B4[];
extern u8 *D_80125C14[];

void Ui_RemapTextures(void) {
    s32 i;
    char unused[1];
    UiSlot *s;
    for (i = 0; i < gUi[0]->count; i++) {
        if (D_800958B4[0] + 0x112D0 == gUi[0]->slots[i].x0) {
            s = &gUi[0]->slots[i];
            s->x0 = D_80125C14[0] + 0x112D0;
        } else if (D_800958B4[0] + 0x112DD == gUi[0]->slots[i].x0) {
            s = &gUi[0]->slots[i];
            s->x0 = D_80125C14[0] + 0x112DD;
        }
    }
}
