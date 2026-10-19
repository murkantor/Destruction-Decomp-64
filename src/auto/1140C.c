#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
extern u8 D_8012455C;
extern u8 D_1304D8[];
extern Gfx D_8008F8C0[];
void func_8001723C(void *, s32, s32);
void func_80017404(void *, Gfx *, s32);
void func_80016A04(void *, s32, s32);
void func_80017030(void *, Gfx *, s32);
void func_80017610(void *, s32, s32);
void func_80017908(void *, Gfx *, s32);

Gfx *func_8001080C(u8 *obj, Gfx *gfx, s32 arg) {
    { Gfx *_g = gfx++; _g->w0 = 0xDA380000; _g->w1 = (u32)D_1304D8; }
    { Gfx *_g = gfx++; _g->w0 = 0xDE000000; _g->w1 = (u32)D_8008F8C0; }
    func_8001723C(obj + 0x148, arg, D_8012455C);
    func_80017404(obj + 0x148, gfx++, D_8012455C);
    func_80016A04(obj + 0xB0, arg, D_8012455C);
    func_80017030(obj + 0xB0, gfx++, D_8012455C);
    { Gfx *_g = gfx++; _g->w0 = 0xE3001201; _g->w1 = 0; }
    func_80017610(obj + 0xC, arg, D_8012455C);
    func_80017908(obj + 0xC, gfx++, D_8012455C);
    { Gfx *_g = gfx++; _g->w0 = 0xD8380002; _g->w1 = 0x40; }
    return gfx;
}
