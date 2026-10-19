#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x2C]; void *x2C; s32 x30; u8 pad34[8]; void *x3C; } Obj;
typedef struct { u8 pad[0x10FC]; s16 x10FC; } Game;
extern Game *gGame;
void func_80004E00(Obj *);
void func_80024B88(void *);
void func_80024760(void *, void *);

void func_80020FB0(Obj *o) {
    s16 s = gGame->x10FC;

    if (s == 0x19) {
        func_80004E00(o);
        return;
    }
    switch (o->x30) {
        case 1:
            if (s == 0x19 || s == 0x1C || s == 0x1D) {
                func_80004E00(o);
            }
            break;
        case 0:
            if (s != 0x1D) {
                func_80024B88(o->x3C);
                func_80024760(o->x2C, o->x3C);
            }
            break;
    }
}
