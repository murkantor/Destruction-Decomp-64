#include "common.h"
#include "m2c.h"

void Menu_ShowRecordRows(s32, s32);
void Timer_Start(void *, s64);

s32 BombResults_Prev(u8 *obj) {
    s32 n = *(s32 *)(obj + 0x10);

    if (n == 0) {
        return 0;
    }
    *(s32 *)(obj + 0x10) = n - 1;
    Menu_ShowRecordRows(n, n - 1);
    Timer_Start(obj, 300000000LL);
    return 1;
}


s32 BombResults_Next(u8 *obj) {
    s32 n = *(s32 *)(obj + 0x10);

    if (n == 4) {
        return 0;
    }
    *(s32 *)(obj + 0x10) = n + 1;
    Menu_ShowRecordRows(n, n + 1);
    Timer_Start(obj, 300000000LL);
    return 1;
}
