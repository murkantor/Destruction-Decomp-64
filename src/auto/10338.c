#include "common.h"
#include "m2c.h"

typedef struct {
    s32 unk0;
} Race;
typedef struct {
    u8 pad[0x1100];
    Race *race;
} Game;
extern Game *gGame;
extern char D_800B81A0[];
extern char D_8007F7F0[][5];
s32 sprintf(char *, const char *, ...);

s32 func_8000F738(s32 arg0, s8 idx) {
    s32 n = gGame->race->unk0 - 1;

    if (n < 0) {
        n = 0;
    }
    sprintf(D_8007F7F0[idx], D_800B81A0, n);
    return 0;
}
