#include "common.h"
#include "m2c.h"

typedef struct {
    s32 id;
    u8 pad[0x1C];
} Entry20;
extern s32 gNumResTypes;
extern Entry20 gResTypes[];

Entry20 *Res_Find(s32 id) {
    Entry20 *found = NULL;
    s32 i;

    for (i = 0; i < gNumResTypes; i++) {
        if (gResTypes[i].id == id) {
            found = &gResTypes[i];
            break;
        }
    }
    return found;
}
