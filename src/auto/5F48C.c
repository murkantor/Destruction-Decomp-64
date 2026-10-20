#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x10];
    void *x10;
    void *x14;
    u8 x18[1];
} UiElement;
UiElement *Ui_FindElement(void *ui, s32 kind, s32 id);
void Font_SetAlphaValue(void *, s32);
void Sprite_SetAlphaValue(void *, s32);
void Bg_SetAlphaValue(void *, s32);

void Ui_SetElementAlphaValue(void *ui, s32 kind, s32 id, s32 value) {
    switch (kind) {
        case 1:
            Sprite_SetAlphaValue(Ui_FindElement(ui, 1, id)->x14, value);
            break;
        case 2:
            Bg_SetAlphaValue(Ui_FindElement(ui, 2, id)->x18, value);
            break;
        case 0:
            Font_SetAlphaValue(Ui_FindElement(ui, 0, id)->x10, value);
            break;
    }
}
