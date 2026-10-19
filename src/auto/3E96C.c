#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Menu;
extern void *gUi[];
extern u8 D_800964E1;
extern u8 D_80095898;
extern char gSaveSlotText[][0x32];
extern char D_800BA238[];
extern char D_800BA250[];
extern char D_800BA270[];
void Ui_AddElement(void *, s32, s32);
s32 sprintf(char *, const char *, ...);
void func_80063380(s32);
void func_80063564(s32, s32);

s32 func_8003DD6C(Menu *m) {
    if (D_800964E1) {
        Ui_AddElement(gUi[0], 2, 0x130);
        Ui_AddElement(gUi[0], 0, 0x17A);
        sprintf(gSaveSlotText[0], D_800BA238);
        sprintf(gSaveSlotText[1], D_800BA250);
        sprintf(gSaveSlotText[2], D_800BA270);
        m->x10 = 0;
    } else if (D_80095898) {
        func_80063380(0);
        func_80063564(2, 1);
        D_80095898 = 0;
        m->x10 = 0;
    } else {
        func_80063380(2);
        func_80063564(0x19, 1);
        m->x10 = 0;
    }
    D_800964E1 = 0;
    return 1;
}
