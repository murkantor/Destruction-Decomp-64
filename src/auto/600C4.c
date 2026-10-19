#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x18]; f32 x; f32 y; } ElemF;
typedef struct { u8 pad[0x18]; s16 x; s16 y; } ElemS;
typedef struct { u8 pad[0x80]; s32 x; s32 y; } ElemI;
void *Ui_FindElement(void *, s32, s32);
Vec3f Ui_GetElementPos(void *, s32, s32);

void Ui_SetElementX(void *ui, s32 kind, s32 id, f32 x) {
    f32 y = Ui_GetElementPos(ui, kind, id).y;

    switch (kind) {
        case 1: {
            ElemF *e = Ui_FindElement(ui, 1, id);
            e->x = x;
            e->y = y;
            break;
        }
        case 2: {
            ElemI *e = Ui_FindElement(ui, 2, id);
            e->x = x;
            e->y = y;
            break;
        }
        case 0: {
            ElemS *e = Ui_FindElement(ui, 0, id);
            e->x = x;
            e->y = y;
            break;
        }
    }
}
