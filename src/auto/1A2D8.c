#include "common.h"
#include "m2c.h"

typedef struct {
    u8 x0;
    u8 pad1[3];
    s32 x4;
} Entry;
typedef struct {
    u8 pad0[4];
    Entry *x4;
    s32 x8;
} Table;
typedef struct {
    u8 pad0[0x10C];
    s32 x10C;
    Table *x110;
    void *x114;
} Obj;
void Grid_Free(s32 *);
void Res_ReleaseInstance(s32);
void Mem_Free(void *);

void Obj_FreeResources(Obj *obj) {
    s32 i;
    Table *t;

    if (obj->x10C != 0) {
        Grid_Free(&obj->x10C);
    }
    t = obj->x110;
    if (t != NULL) {
        for (i = 0; i < t->x8; i++) {
            Entry *e = &t->x4[i];
            if (e->x0 == 0xFD) {
                Res_ReleaseInstance(e->x4);
            }
            t = obj->x110;
        }
        obj->x110 = NULL;
    }
    if (obj->x114 != NULL) {
        Mem_Free(obj->x114);
        obj->x114 = NULL;
    }
}
