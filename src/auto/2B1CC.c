#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
} Cfg;
typedef struct {
    u8 pad0[0x10A8];
    u8 x10A8[0x58];
    Cfg *x1100;
} Game;
typedef struct {
    s32 xA74;
    s32 xA78;
    u8 pad[0xC];
} Slot;
typedef struct {
    u8 pad0[0x10];
    s32 x10;
    u8 pad14[0x80];
    f32 x94;
    u8 pad98[0xA74 - 0x98];
    Slot slots[1];
} Obj;
extern Game *gGame[];
extern f64 D_800B91A8;
f32 Time_ToSeconds(void *);
void func_80009B00(Obj *);

void func_8002A5CC(Obj *obj, s32 idx) {
    Cfg *cfg;

    cfg = gGame[0]->x1100;
    if (obj->x94 > 0.0) {
        return;
    }
    if (obj->x10 <= 0 && cfg->x0 > 0) {
        obj->x10 = 0;
        obj->x94 = Time_ToSeconds(gGame[0]->x10A8) + D_800B91A8;
    } else if (obj->slots[idx].xA74 <= 0 && obj->slots[idx].xA78 == 0) {
        obj->slots[idx].xA78 = 1;
        func_80009B00(obj);
    }
}
