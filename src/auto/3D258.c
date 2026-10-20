#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x10]; s32 x10; } Obj;
extern u8 D_80123C24[];
extern void *Ui_LookupTextId(void *);
extern void Ui_SetElementColor(void *, s32);
extern void Timer_Start(void *, s64);
s32 func_8003C658(Obj *o) {
    if (o->x10 == 0) {
        o->x10 = 1;
        Ui_SetElementColor(Ui_LookupTextId(D_80123C24), 0x111F8);
        Ui_SetElementColor(Ui_LookupTextId(D_80123C24 + 0x32), 0x111F4);
    } else {
        o->x10 = 0;
        Ui_SetElementColor(Ui_LookupTextId(D_80123C24 + 0x32), 0x111F8);
        Ui_SetElementColor(Ui_LookupTextId(D_80123C24), 0x111F4);
    }
    Timer_Start(o, 300000000);
    return 1;
}
