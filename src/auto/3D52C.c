#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x10];
    s32 x10;
} Item;
extern char D_80123C56[];
extern char D_80123C88[];
void *Ui_LookupTextId(char *);
void Ui_SetElementColor(void *, s32);
void Timer_Start(void *, s64);

s32 PakMenu_NoSaveToggle(Item *item) {
    char *p;
    char *q;

    if (item->x10 == 0) {
        item->x10 = 1;
        p = D_80123C56;
        Ui_SetElementColor(Ui_LookupTextId(p), 0x111F8);
        q = p + 0x32;
    } else {
        item->x10 = 0;
        p = D_80123C88;
        Ui_SetElementColor(Ui_LookupTextId(p), 0x111F8);
        q = p - 0x32;
    }
    Ui_SetElementColor(Ui_LookupTextId(q), 0x111F4);
    Timer_Start(item, 300000000LL);
    return 1;
}
