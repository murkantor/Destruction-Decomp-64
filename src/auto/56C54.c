#include "common.h"
#include "m2c.h"

extern u8 D_800BCA90[];
extern char D_800BBB84[];
extern void DebugMenu_AddSubmenu(void *, void *, void *);
extern void DebugMenu_Init(void *);

void DebugMenu_BuildVideo(void *menu) {
    DebugMenu_AddSubmenu(menu, D_800BBB84, D_800BCA90);
    DebugMenu_Init(D_800BCA90);
}
