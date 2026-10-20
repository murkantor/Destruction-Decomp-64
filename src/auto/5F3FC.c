#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x10];
    void *x10;
    void *x14;
    u8 x18[1];
} UiElement;
UiElement *Ui_FindElement(void *ui, s32 kind, s32 id);
void Font_DisableAlpha(void *);
void Sprite_DisableAlpha(void *);
void Bg_DisableAlpha(void *);

void Ui_DisableElementAlpha(void *ui, s32 kind, s32 id) {
    switch (kind) {
        case 1:
            Sprite_DisableAlpha(Ui_FindElement(ui, 1, id)->x14);
            break;
        case 2:
            Bg_DisableAlpha(Ui_FindElement(ui, 2, id)->x18);
            break;
        case 0:
            Font_DisableAlpha(Ui_FindElement(ui, 0, id)->x10);
            break;
    }
}
