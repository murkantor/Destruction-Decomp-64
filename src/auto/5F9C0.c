#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[4];
    s32 x4;
    u8 pad8[0x20];
} Ent;
typedef struct {
    s32 x0;
    u8 pad4[0x20];
} Slot;
typedef struct {
    u8 pad0[0x8938];
    Slot slots[15];
    u8 pad8B54[0xE];
    s16 x8B62;
} Mgr;
void Ui_AddResource(Mgr *, Ent *);

void Ui_AddMissingResources(Mgr *m, Ent *arg1, u32 size) {
    Ent *e = arg1;
    u32 i;
    s32 j;
    u32 n = size / 40;

    for (i = 0; i < n; i++, e++) {
        for (j = 0; j < m->x8B62; j++) {
            if (e->x4 == m->slots[j].x0) {
                break;
            }
        }
        if (j == m->x8B62) {
            Ui_AddResource(m, e);
        }
    }
}
