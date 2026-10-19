#include "common.h"
#include "m2c.h"

extern void *gUi;
extern u8 D_8007EF33;
extern s32 D_800D1AEC;
u8 *Ui_FindElement(void *, s32, s32);
void Mem_Free(void *);

void func_8000E688(void) {
    u8 *e = Ui_FindElement(gUi, 2, 0x1E);
    void **slot;

    Mem_Free(**(void ***)(e + 0x10));
    slot = *(void ***)(e + 0x10);
    D_8007EF33 = 0;
    *slot = NULL;
    D_800D1AEC = 0;
}
