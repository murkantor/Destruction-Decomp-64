#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x10];
    s32 x10;
} Item;
extern char D_80123C56[];
extern char D_80123C88[];
void *func_8001007C(char *);
void func_800100BC(void *, s32);
void Timer_Start(void *, s64);

s32 func_8003C92C(Item *item) {
    char *p;
    char *q;

    if (item->x10 == 0) {
        item->x10 = 1;
        p = D_80123C56;
        func_800100BC(func_8001007C(p), 0x111F8);
        q = p + 0x32;
    } else {
        item->x10 = 0;
        p = D_80123C88;
        func_800100BC(func_8001007C(p), 0x111F8);
        q = p - 0x32;
    }
    func_800100BC(func_8001007C(q), 0x111F4);
    Timer_Start(item, 300000000LL);
    return 1;
}
