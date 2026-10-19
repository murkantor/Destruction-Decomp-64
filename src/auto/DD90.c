#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad0[0x38];
    s32 x38;
} Rec;
extern Rec *D_80096080;
extern Rec *D_80096084;
extern char D_8007F840[];
extern char D_8007F846[];
extern char D_800B8230[];
extern char D_800B8238[];
extern char D_800B8184[];
int sprintf(char *, const char *, ...);

s32 func_8000D190(void) {
    s32 a = D_80096084->x38;
    s32 b = D_80096080->x38;

    if (a < 10) {
        sprintf(D_8007F840, D_800B8230, a);
    } else if (a < 1000) {
        sprintf(D_8007F840, D_800B8238, a);
    } else {
        sprintf(D_8007F840, D_800B8184, a);
    }
    if (b < 10) {
        sprintf(D_8007F846, D_800B8230, b);
    } else if (a < 1000) {
        sprintf(D_8007F846, D_800B8238, b);
    } else {
        sprintf(D_8007F846, D_800B8184, b);
    }
    return 0;
}
