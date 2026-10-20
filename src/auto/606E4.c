#include "common.h"
#include "m2c.h"

typedef struct { u32 w0; u32 w1; } Gfx;
typedef struct { s32 a; s32 b; } HeapBlock;
typedef struct { u8 pad[0x1C]; } ViewCam;

typedef struct {
    u8 pad0[0xC];
    s32 xC;
    u8 x10[0x90 - 0x10];
    u8 x90[0x150 - 0x90];
    HeapBlock x150;
    HeapBlock x158[2];
    HeapBlock x168[2];
    HeapBlock x178[2];
    u8 pad188[0x198 - 0x188];
    ViewCam x198[4];
} Renderer;

typedef struct {
    u8 pad0[0xC];
    u8 xC[1];
} Car;

typedef struct {
    u8 pad0[0xC];
    s16 xC;
    u8 padE[0xB40 - 0xE];
    Car *xB40[8];
    u8 padB60[0x1058 - 0xB60];
    u64 x1058;
    u8 pad1060[0x10FC - 0x1060];
    s16 x10FC;
} Game;

extern Game *gGame;
extern void *gUi;
extern u8 D_800D31F8;
extern f32 D_80125BD4;
extern s32 D_80124350;
extern u8 D_8012455C;
extern u8 D_800D2E58;
extern s32 D_800958A8;
extern u8 D_8007BCC0[];
extern u8 D_800B7560[];

void *HeapBlock_Ptr(HeapBlock *);
void func_800563A0(void *, s32, void *, void *, void *);
void func_80056578(void *, s32);
Gfx *func_8005793C(Gfx *);
u32 func_8005537C(HeapBlock *);
void osWritebackDCache(void *, s32);
void func_80056588(void *, void *, Gfx *, void *, void *);
s32 func_800572C8(void *);
f32 Time_ToSeconds(void *);
void Game_GetMode(void);
Gfx *func_8005B5E8(Gfx *, ViewCam *);
Gfx *Camera_BuildProjection(Gfx *, void *);
Gfx *Render_DrawSkyAndWorld(Renderer *, Gfx *, void *);
Gfx *func_8006449C(Gfx *, void *, Car *);
u8 func_800232DC(Car *);
Gfx *Weather_Draw(void *, Gfx *);
Gfx *Gfx_DrawLensFlare(Gfx *, void *, ViewCam *, s32);
Gfx *func_8005D580(void *, Gfx *);
void func_80012540(void);

void func_8005FAE4(Renderer *r) {
    Game *g = gGame;
    void *buf;
    Gfx *gfx;
    f32 t;
    s32 modes[2];
    Car *car;
    void *cam;

    if (D_800D31F8 != 0) {
        r->xC = (r->xC + 1) % 2;
        func_800563A0(r->x90, 3, r->x10, HeapBlock_Ptr(&r->x158[r->xC]), HeapBlock_Ptr(&r->x150));
        func_80056578(r->x90, 0);
        gfx = func_8005793C(HeapBlock_Ptr(&r->x178[r->xC]));
        osWritebackDCache(HeapBlock_Ptr(&r->x178[r->xC]), func_8005537C(&r->x178[r->xC]));
        osWritebackDCache(HeapBlock_Ptr(&r->x168[r->xC]), func_8005537C(&r->x168[r->xC]));
        func_80056588(r->x90, HeapBlock_Ptr(&r->x178[r->xC]), gfx, D_8007BCC0, D_800B7560);
        func_800572C8(r->x90);
        return;
    }
    t = Time_ToSeconds(&g->x1058);
    if (g->xC == 2 && gGame->x10FC == 0x1C && D_80125BD4 + 1.0f <= t) {
        modes[0] = g->xC;
        modes[1] = 6;
    } else {
        modes[0] = 3;
        modes[1] = 7;
    }
    r->xC = (r->xC + 1) % 2;
    func_800563A0(r->x90, modes[D_80124350], r->x10, HeapBlock_Ptr(&r->x158[r->xC]), HeapBlock_Ptr(&r->x150));
    func_80056578(r->x90, 0);
    gfx = HeapBlock_Ptr(&r->x178[r->xC]);
    Game_GetMode();
    for (D_8012455C = 0; D_8012455C < D_800D2E58; D_8012455C++) {
        gfx = func_8005B5E8(gfx, &r->x198[D_8012455C]);
        car = gGame->xB40[D_8012455C];
        cam = car->xC;
        gfx = Camera_BuildProjection(gfx, cam);
        gfx = Render_DrawSkyAndWorld(r, gfx, cam);
        gfx = func_8006449C(gfx, cam, car);
        if (D_800958A8 != 3 && func_800232DC(car) == 0) {
            gfx = Weather_Draw(cam, gfx);
        }
        gfx = Gfx_DrawLensFlare(gfx, cam, &r->x198[D_8012455C], D_8012455C);
    }
    gfx = func_8005D580(gUi, gfx);
    gfx = func_8005793C(gfx);
    osWritebackDCache(HeapBlock_Ptr(&r->x178[r->xC]), func_8005537C(&r->x178[r->xC]));
    osWritebackDCache(HeapBlock_Ptr(&r->x168[r->xC]), func_8005537C(&r->x168[r->xC]));
    func_80056588(r->x90, HeapBlock_Ptr(&r->x178[r->xC]), gfx, D_8007BCC0, D_800B7560);
    if (func_800572C8(r->x90) < 0) {
        D_800D31F8 = 1;
    }
    func_80012540();
}
