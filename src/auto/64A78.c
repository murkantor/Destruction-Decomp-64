#include "common.h"
#include "m2c.h"

typedef struct {
    s32 id;
    u8 pad[0x1C];
} Entry20;
extern s32 D_800B0330;
extern Entry20 D_80097D90[];

Entry20 *Res_Find(s32 id) {
    Entry20 *found = NULL;
    s32 i;

    for (i = 0; i < D_800B0330; i++) {
        if (D_80097D90[i].id == id) {
            found = &D_80097D90[i];
            break;
        }
    }
    return found;
}
