#include "common.h"
#include "m2c.h"

extern s32 D_800BCA1C[];
extern s32 D_800BCA20;
void huft_free(s32);

s32 inflate_free(void) {
    if (D_800BCA1C[0] == 0) {
        return 0;
    }
    huft_free(D_800BCA20);
    huft_free(D_800BCA1C[0]);
    D_800BCA1C[0] = 0;
    D_800BCA20 = 0;
    return 0;
}
