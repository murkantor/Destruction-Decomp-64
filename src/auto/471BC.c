#include "common.h"
#include "m2c.h"

typedef struct { char s[7]; } Str7;
extern void *gUi[];
extern Str7 gSaveSlotText;
void func_8005DC68(void *);
void Ui_AddElement(void *, s32, s32);

void func_800465BC(void) {
    func_8005DC68(gUi[0]);
    Ui_AddElement(gUi[0], 0, 0x17A);
    gSaveSlotText = *(Str7 *)"REPLAY";
}
