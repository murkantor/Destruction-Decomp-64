#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0x1C]; u8 paused; u8 pad1D[0x28 - 0x1D]; } Clock;
typedef struct {
    u8 pad0[0x1058];
    Clock clock;
    Clock timers[3];
    s32 nClocks;
    s16 x10FC;
} Game;
typedef struct { u8 pad[0x68]; } Slot68;
typedef struct { u8 pad[0xC]; s16 mode; u8 pad0E[0x334 - 0xE]; u8 x334[1]; } World;
extern Game *gGame[];
extern s32 gDoPhysics;
extern s8 D_800D31CC;
extern Slot68 D_800958D0[];
void Clock_Update(Clock *);
s32 Clock_GetFrames(Clock *);
void func_80064304(void *);
f32 func_80062F40(void);
void World_UpdateEffects(void *, f32);
void func_8002EB00(void *, f32);
void Announcer_UpdateRace(void);
void Anim_UpdateTimers(void);
void func_8002104C(Slot68 *);

void World_Update(World *w) {
    s32 i;
    s32 n;
    s32 step;
    f32 dt;
    s8 j;

    Clock_Update(&gGame[0]->clock);
    for (i = 0; i < gGame[0]->nClocks; i++) {
        Clock_Update(&gGame[0]->timers[i]);
    }
    func_80064304(w->x334);
    if (w->mode == 1) {
    } else if (w->mode >= 2) {
        if (w->mode == 2) {
        if (!gGame[0]->clock.paused) {
            n = Clock_GetFrames(&gGame[0]->timers[1]);
            step = 3;
            if (n > 20) {
                n = 2;
            }
            if (n > 6) {
                n = 6;
            }
            if (gDoPhysics) {
                while (n > 0) {
                    if (step < n) {
                        dt = func_80062F40() * step;
                    } else {
                        dt = func_80062F40() * n;
                    }
                    World_UpdateEffects(w->x334, dt);
                    func_8002EB00(w->x334, dt);
                    n -= step;
                }
            }
            if (gGame[0]->x10FC == 0x1C) {
                Announcer_UpdateRace();
            }
            Anim_UpdateTimers();
        }
        for (j = 0; j < D_800D31CC; j++) {
            func_8002104C(&D_800958D0[j]);
        }
        }
    }
}
