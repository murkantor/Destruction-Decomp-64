#include "common.h"
#include "m2c.h"

typedef struct {
    s32 count;
    u16 *buf;
    s32 x8;
    u8 padC[4];
    u64 x10;
    u64 x18;
} State;

typedef struct {
    u8 pad0[0x10];
    u16 **x10;
    u8 pad14[0x14];
    u16 *x28;
} Elem;

typedef struct {
    u8 pad[0x1058];
    u64 x1058;
} Game;

extern State D_800D1AE8;
extern void *gUi;
extern Game *gGame[];
extern f64 D_800B8288;

s32 Res_GetSize(s32);
void *Mem_AllocAligned(s32, s32, s32);
Elem *Ui_FindElement(void *, s32, s32);
u64 Time_Get(u64 *);

void Ui_RevealInit(void) {
    s32 size;
    u16 *buf;
    Elem *e;
    s32 i;

    size = Res_GetSize(0x11300);
    D_800D1AE8.count = size / 2;
    buf = Mem_AllocAligned(size, 7, 0);
    e = Ui_FindElement(gUi, 2, 0x1E);
    D_800D1AE8.buf = *e->x10;
    *e->x10 = buf;
    e->x28 = buf;
    D_800D1AE8.x8 = 0;
    D_800D1AE8.x18 = Time_Get(&gGame[0]->x1058) + D_800B8288;
    D_800D1AE8.x10 = 75000000;
    for (i = 0; i < D_800D1AE8.count; i++) {
        *buf++ = 1;
    }
}
