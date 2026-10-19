#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0xC];
    s16 mode;
} Game;
extern Game *gGame;
extern u8 *D_80126138;

f32 func_80012508(void) {
    return (gGame->mode == 2) ? *(f32 *)(D_80126138 + 0xC) : 1.0f;
}
