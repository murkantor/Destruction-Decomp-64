#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x44]; s32 x44; u8 pad48[0x68]; } Controller;
extern Controller gControllers[];
extern s32 D_800958A4;
void Cont_ReadAll(void);
void func_80058B58(void);

s32 Cont_AllPaksOk(void) {
    s32 r;
    s32 i;

    Cont_ReadAll();
    r = 1;
    for (i = 0; i < D_800958A4; i++) {
        if (gControllers[i].x44 != 0) {
            /* double store keeps jump.c from turning this into an and-mask */
            r = 1;
            r = 0;
        }
    }
    func_80058B58();
    return r;
}
