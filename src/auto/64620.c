#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0xC];
    void *(*load)(void *);
    u8 pad10[0x4];
    void **x14;
    u8 pad18[0x4];
    s32 x1C;
} Res;

extern Res *Res_Find(void *);
extern void List_Push(void ***, void *);
extern u8 D_800B4D70;

void *Res_Get(void *id)
{
    Res *r;
    void *p;
    p = NULL;
    r = Res_Find(id);
    if (r != NULL) {
        if (D_800B4D70 == 0 && r->x1C > 0) {
            id = *r->x14;
        } else {
            id = r->load(id);
            List_Push(&r->x14, id);
        }
        r->x1C++;
        p = id;
    }
    else return NULL;
    return p;
}
