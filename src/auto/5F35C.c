#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x10];
    void *x10;
    void *x14;
    u8 x18[1];
} UiElement;
UiElement *Ui_FindElement(void *ui, s32 kind, s32 id);
void Font_EnableAlpha(void *, s32);
void Sprite_EnableAlpha(void *, s32);
void Bg_EnableAlpha(void *, s32);

void Ui_EnableElementAlpha(void *ui, s32 kind, s32 id, s32 value) {
    switch (kind) {
        case 1:
            Sprite_EnableAlpha(Ui_FindElement(ui, 1, id)->x14, value);
            break;
        case 2:
            Bg_EnableAlpha(Ui_FindElement(ui, 2, id)->x18, value);
            break;
        case 0:
            Font_EnableAlpha(Ui_FindElement(ui, 0, id)->x10, value);
            break;
    }
}
