#include "common.h"
#include "m2c.h"

typedef struct {
    s16 id;
    void (*x4)();
    void (*x8)();
    u8 padC[8];
} Handler;
extern Handler D_80095630[];
extern s32 D_80095810;

void func_80023D60(void *arg, s32 item) {
    s16 *id = (s16 *)item;
    char unused[1];
    s32 i;
    

    for (i = 0; i < D_80095810; i++) {
        if (D_80095630[i].id == *id) {
            if (D_80095630[i].x4 != NULL) {
                D_80095630[i].x4(arg, item);
            }
            return;
        }
    }
}
