#include "common.h"
#include "m2c.h"

M2C_UNK EngineSfx_UpdateAll();                            /* extern */
M2C_UNK Sound_ExpireScrapes(s32);                         /* extern */
M2C_UNK Sound_UpdateCarSurfaces(s32);                         /* extern */
M2C_UNK Announcer_Update();                            /* extern */
extern void *gGame;

void Sound_UpdateRace(s32 arg0) {
    if (M2C_FIELD(gGame, s16 *, 0xC) == 2) {
        EngineSfx_UpdateAll();
        Sound_UpdateCarSurfaces(arg0);
        Sound_ExpireScrapes(arg0);
    }
    Announcer_Update();
}
