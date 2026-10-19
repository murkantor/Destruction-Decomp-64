#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x24];
    s32 key;
} ObjK;

ObjK *Obj_MaxByKey24(ObjK *a, ObjK *b) {
    if (b == NULL) return a;
    if (a->key > b->key) return a;
    return b;
}
