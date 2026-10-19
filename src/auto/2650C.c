#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    s32 x4;
    u64 x8;
    u64 x10;
    s8 x18;
    u8 x19;
    u8 x1A;
    s32 x1C;
} Timer;
typedef struct { u8 pad[0x38]; Timer *x38; } Obj;
void *Mem_Alloc(s32);

static inline void Timer_Init(Timer *t, u64 start, f64 dur) {
    t->x8 = start;
    t->x0 = 0;
    t->x4 = 0;
    t->x18 = -1;
    t->x19 = 0;
    t->x1A = 0;
    t->x1C = 0;
    t->x10 = dur;
}

void Timer_Create(Obj *o) {
    Timer *t = Mem_Alloc(sizeof(Timer));

    Timer_Init(t, 0, 20000000000.0);
    o->x38 = t;
}
