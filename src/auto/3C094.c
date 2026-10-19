#include "common.h"
#include "m2c.h"

extern void *gUi[];
extern u8 *gGame;
void func_8005DC68(void *);
void Ui_AddElement(void *, s32, s32);
void func_8000E508(void);
void func_800638C8(void *);
void func_80063890(void *);
void func_800638C0(void *);

void func_8003B494(void) {
    func_8005DC68(gUi[0]);
    Ui_AddElement(gUi[0], 2, 30);
    func_8000E508();
    func_800638C8(gGame + 0x10A8);
    func_80063890(gGame + 0x10A8);
    func_800638C0(gGame + 0x10A8);
}
