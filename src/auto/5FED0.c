#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s32 x8; s32 xC; s32 x10; s32 x14; s32 x18; } Entry;
typedef struct { s32 id; u8 x4[0x18]; } UiElem;
typedef struct {
    u8 pad[0x7588];
    UiElem elems[(0x8B5C - 0x7588) / 0x1C];
    u8 padE[(0x8B5C - 0x7588) % 0x1C];
    u16 count;
} Ui;
s32 Res_Get(s32);
void Tex_Init(void *, s32, s32, s32, s32, s32, s32);

void Ui_LoadElements(Ui *ui, Entry *e, u32 size) {
    u32 i;
    u32 n = size / sizeof(Entry);
    UiElem *el;

    for (i = 0; i < n; i++) {
        el = &ui->elems[i];
        switch (e->x4) {
            case 1:
                Tex_Init(el->x4, Res_Get(e->x8), e->x14, e->x18, e->xC, e->x10, 0xFC1034);
                break;
            case 2:
                Tex_Init(el->x4, Res_Get(e->x8), e->x14, e->x18, e->xC, e->x10, 0xFFF4);
                break;
        }
        el->id = e->x8;
        e++;
        ui->count++;
    }
}
