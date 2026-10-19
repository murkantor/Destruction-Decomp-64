#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x10];
    void *x10;
    void *x14;
    u8 x18[1];
} UiElement;
UiElement *Ui_FindElement(void *ui, s32 kind, s32 id);
void func_8005F994(void *);
void func_80060410(void *);
void func_80061568(void *);

void func_8005E7FC(void *ui, s32 kind, s32 id) {
    switch (kind) {
        case 1:
            func_80060410(Ui_FindElement(ui, 1, id)->x14);
            break;
        case 2:
            func_80061568(Ui_FindElement(ui, 2, id)->x18);
            break;
        case 0:
            func_8005F994(Ui_FindElement(ui, 0, id)->x10);
            break;
    }
}
