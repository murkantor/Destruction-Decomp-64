#include "common.h"
#include "m2c.h"

typedef struct { char s[7]; } Str7;
extern void *gUi[];
extern Str7 gSaveSlotText;
void Ui_ClearDrawList(void *);
void Ui_AddElement(void *, s32, s32);

void Replay_Init(void) {
    Ui_ClearDrawList(gUi[0]);
    Ui_AddElement(gUi[0], 0, 0x17A);
    gSaveSlotText = *(Str7 *)"REPLAY";
}
