/*
 * Game state the renderer (port/fast3d) asks about.
 */
#include "common.h"

typedef struct {
    u8 pad0[0xC];
    s16 xC; /* 2: the 3D world is drawn (Render_DrawSkyAndWorld, 129B0.c) */
} Game;

extern Game *gGame;

/* The game is drawing the race's 3D world (its sky dome, then the track), not a
   menu screen */
s32 Port_GameDrawsWorld(void) {
    return gGame != NULL && gGame->xC == 2;
}
