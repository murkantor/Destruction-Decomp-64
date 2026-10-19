#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x52C];
    s32 unk52C;
    u8 unk530;
} Obj8011EEF8;
typedef struct {
    u8 pad[0x4C4];
    void *bufs[4];
    s32 sizes[4];
    u8 pad4E4[0x20];
    void *buf504;
    s32 size508;
} Owner;
extern Obj8011EEF8 *D_8011EEF8;
extern s8 D_800D31CC;
void Mem_Free(void *);
s32 func_80065DE0(s32);
void func_80065D60(s32, s32);

s32 func_8005C6A0(Owner *o) {
    s32 i;

    if (D_8011EEF8->unk530 == 0) {
        if (func_80065DE0(3) != 0) {
            func_80065D60(3, 30);
            while (func_80065DE0(3) != 0) {
            }
        }
        D_8011EEF8->unk52C = 0;
    }
    for (i = 0; i < D_800D31CC; i++) {
        if (o->bufs[i] != NULL) {
            Mem_Free(o->bufs[i]);
            o->bufs[i] = NULL;
            o->sizes[i] = 0;
        }
    }
    if (o->buf504 != NULL) {
        Mem_Free(o->buf504);
        o->buf504 = NULL;
        o->size508 = 0;
    }
    return 0;
}
