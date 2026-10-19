#include "common.h"
#include "m2c.h"

s32 alSavePull(void *, s32 *, s32, s32, s32);
s32 alSaveParam(void *, s32, void *);
void alFilterNew(void *, void *, void *, s32);
typedef struct {
    u8 filter[0x14];
    s32 dramout;
    s32 first;
} ALSave;

void alSaveNew(ALSave *f) {
    alFilterNew(f, alSavePull, alSaveParam, 3);
    f->dramout = 0;
    f->first = 1;
}
