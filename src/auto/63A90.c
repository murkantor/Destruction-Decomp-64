#include "common.h"
#include "m2c.h"

typedef struct { s32 data[0x2C]; } Controller;
extern Controller gControllers[4];
extern Controller gControllersLatched[4];
void Fps_Update(void);
void Pad_Read(void);

void Pad_LatchAll(void) {
    s32 i;

    Fps_Update();
    Pad_Read();
    for (i = 0; i < 4; i++) {
        gControllersLatched[i] = gControllers[i];
    }
}
