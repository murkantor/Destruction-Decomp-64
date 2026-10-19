#include "common.h"
#include "m2c.h"

extern u8 D_80097D4C;
extern u8 D_80097D58;
extern u16 D_80097D66;
extern s32 D_80097D68;
extern u8 D_80097D6C[];
extern u8 D_80097D7C[];
extern OSMesgQueue gSiMesgQueue;
void func_80058A74(void);
void func_80058B58(void);
void func_80050684(s32 *);
s32 Pak_SetGameInfo(s32, s32, void *, void *, OSMesgQueue *);
s32 Pak_Init(OSMesgQueue *);

u8 Pak_Open(s32 *out) {
    func_80058A74();
    D_80097D58 = 1;
    if (D_80097D4C) {
        D_80097D4C = 0;
        *out = Pak_SetGameInfo(D_80097D66, D_80097D68, D_80097D6C, D_80097D7C, &gSiMesgQueue);
        func_80058B58();
        D_80097D58 = 0;
        return *out != 1;
    }
    *out = Pak_Init(&gSiMesgQueue);
    if (*out != 0) {
        func_80058B58();
        D_80097D58 = 0;
        return *out != 1;
    }
    func_80050684(out);
    func_80058B58();
    D_80097D58 = 0;
    return 1;
}
