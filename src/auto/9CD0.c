#include "common.h"
#include "m2c.h"

typedef struct { u8 pad0[0x10FC]; s16 x10FC; } Game;
extern Game *gGame;
extern s32 D_80124364;
extern s32 D_8013137C;
extern s32 D_80125C18;
extern u64 gGameTime;
extern u64 D_801244D8;
extern u8 D_80124368[];
u64 Time_Get(u64 *);

void Announcer_Request(s32 id, s32 arg1, u8 check) {
    if (D_80124364 <= 0 && D_8013137C < 0) {
        if (Time_Get(&gGameTime) >= D_801244D8) {
            if (gGame->x10FC != 0x19) {
                if (!check || !D_80124368[id]) {
                    D_8013137C = id;
                    D_80125C18 = arg1;
                }
            }
        }
    }
}
