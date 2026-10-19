#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x2C];
    f32 x2C;
    f32 x30;
    f32 x34;
    f32 x38;
    f32 x3C;
    f32 x40;
    f32 x44;
    u8 pad2[0x4C - 0x48];
} Checkpoint;

typedef struct {
    s32 count;
    Checkpoint *cps;
} Track;

typedef struct {
    u8 pad[0x1C];
    f32 x1C;
    u8 pad2[0xB4 - 0x20];
    u8 xB4;
} Player;

typedef struct {
    u8 pad[0x290];
    f32 x290;
    f32 x294;
    f32 x298;
} Car;

typedef struct {
    s32 active;
    s32 idx;
    s32 step;
    f32 timer;
} CpState;
extern CpState D_80130200;
extern Checkpoint *D_8009562C;
extern Track *D_80123320;
extern f32 D_80091C80;


void Camera_UpdateCheckpointCam(Player *pl, Car *car, f32 dt) {
    u8 hit = 0;
    f32 *pos = &car->x290;
    f32 dx, dy, dz, d;
    s32 fwd, back, n, i, found;
    Checkpoint *cp;

    if (D_80130200.active == 0) {
        if (D_80130200.timer > 1.0) {
            hit = 1;
        }
    } else if (D_8009562C->x2C < D_80130200.timer) {
        dx = D_8009562C->x3C - car->x290;
        dy = D_8009562C->x40 - car->x294;
        dz = D_8009562C->x44 - car->x298;
        d = dx * dx + dy * dy + dz * dz;
        if ((0.0 < D_8009562C->x30 && D_8009562C->x30 < D_80130200.timer) || D_8009562C->x38 * D_8009562C->x38 < d) {
            if (D_8009562C->x34 * D_8009562C->x34 < d) {
                hit = 1;
            }
        }
    }
    if (hit) {
        back = D_80130200.idx;
        fwd = 0;
        if (back < 0) {
            n = D_80123320->count;
            back = -D_80130200.step;
        } else {
            fwd = back + D_80130200.step;
            back = back - D_80130200.step;
            n = D_80123320->count - 1;
        }
        found = -1;
        for (i = 0; i < n; ) {
            if (fwd >= D_80123320->count) {
                fwd -= D_80123320->count;
            } else if (fwd < 0) {
                fwd += D_80123320->count;
            }
            cp = &D_80123320->cps[fwd];
            i++;
            dx = cp->x3C - pos[0];
            dy = cp->x40 - pos[1];
            dz = cp->x44 - pos[2];
            d = dx * dx + dy * dy + dz * dz;
            if (d < cp->x38 * cp->x38) {
                found = fwd;
                break;
            }
            if (i >= n) {
                break;
            }
            if (back >= D_80123320->count) {
                back -= D_80123320->count;
            } else if (back < 0) {
                back += D_80123320->count;
            }
            cp = &D_80123320->cps[back];
            i++;
            dx = cp->x3C - pos[0];
            dy = cp->x40 - pos[1];
            dz = cp->x44 - pos[2];
            d = dx * dx + dy * dy + dz * dz;
            if (d < cp->x38 * cp->x38) {
                found = back;
                D_80130200.step = -D_80130200.step;
                break;
            }
            fwd += D_80130200.step;
            back -= D_80130200.step;
        }
        if (found >= 0) {
            D_80130200.active = 1;
            D_80130200.idx = found;
            D_80130200.timer = 0.0f;
            D_8009562C = &D_80123320->cps[found];
            pl->xB4 = 9;
        } else if (D_80130200.active != 0) {
            D_80130200.idx = -1;
            D_80130200.active = 0;
            D_80130200.timer = 0.0f;
            pl->xB4 = 2;
            pl->x1C = D_80091C80;
        }
    }
    D_80130200.timer += dt;
}
