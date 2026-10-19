#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x18]; s16 x18; s16 x1A; } UiElem0;
typedef struct { u8 pad[0x18]; f32 x18; f32 x1C; } UiElem1;
typedef struct { u8 pad[0x80]; s32 x80; s32 x84; } UiElem2;
void *Ui_FindElement(void *ui, s32 kind, s32 id);

void Ui_SetElementPos(void *ui, s32 kind, s32 id, f32 x, f32 y) {
    UiElem0 *e0;
    UiElem1 *e1;
    UiElem2 *e2;

    switch (kind) {
        case 1:
            e1 = Ui_FindElement(ui, 1, id);
            e1->x18 = x;
            e1->x1C = y;
            break;
        case 2:
            e2 = Ui_FindElement(ui, 2, id);
            e2->x80 = x;
            e2->x84 = y;
            break;
        case 0:
            e0 = Ui_FindElement(ui, 0, id);
            e0->x18 = x;
            e0->x1A = y;
            break;
    }
}
