#include "common.h"
#include "m2c.h"

typedef struct {
    u8 init;
    s32 a;
    s32 b;
} Lazy;
extern Lazy D_800CEAD0;
extern s32 D_800CEB0C;
void __amDMA(void);

void *__amDmaNew(Lazy **out) {
    if (D_800CEAD0.init == 0) {
        D_800CEAD0.a = 0;
        D_800CEAD0.init = 1;
        D_800CEAD0.b = D_800CEB0C;
    }
    *out = &D_800CEAD0;
    return __amDMA;
}
