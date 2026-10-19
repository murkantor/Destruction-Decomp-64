#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x10];
    void *x10;
    void *x14;
    u8 x18[1];
} UiElement;
UiElement *Ui_FindElement(void *ui, s32 kind, s32 id);
void func_8005F9AC(void *, s32);
void func_80060428(void *, s32);
void func_80061580(void *, s32);

void func_8005E88C(void *ui, s32 kind, s32 id, s32 value) {
    switch (kind) {
        case 1:
            func_80060428(Ui_FindElement(ui, 1, id)->x14, value);
            break;
        case 2:
            func_80061580(Ui_FindElement(ui, 2, id)->x18, value);
            break;
        case 0:
            func_8005F9AC(Ui_FindElement(ui, 0, id)->x10, value);
            break;
    }
}
