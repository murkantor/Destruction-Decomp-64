#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    s32 x4;
    void *x8;
    s32 xC;
    s32 x10;
    f32 x14;
} Item;

typedef struct {
    u8 x0[0xC];
    u8 xC;
    u8 xD;
} ItemB;

typedef struct {
    s32 cur;
    s32 count;
    Item items[1];
} Menu;

extern s32 D_800B04B4;
extern s16 D_80125C98;
extern s16 D_80125C9A;
extern s32 D_80125C90;
extern s32 D_80125C8C;
Menu *DebugMenu_GetCurrent(void);
void Pad_Read(void);
void DebugMenu_Push(void *);
void DebugMenu_Pop(void);

#define ABS(x) ((x) < 0 ? -(x) : (x))

s32 DebugMenu_HandleInput(void) {
    Menu *m = DebugMenu_GetCurrent();
    s32 changed = 0;
    Item *it = &m->items[m->cur];
    s32 sx;
    s32 sy;

    Pad_Read();
    sx = D_80125C98;
    sy = D_80125C9A;
    if (D_800B04B4 != 0) {
        if (ABS(sx) < 40 && ABS(sy) < 40) {
            D_800B04B4 = 0;
        } else {
            D_800B04B4--;
        }
    } else {
        if (ABS(sy) > 40 && ABS(sx) < 40) {
            D_800B04B4 = 30;
            if (sy < 0.0) {
                m->cur = (m->cur + 1) % m->count;
                changed = 1;
            }
            if (sy > 0.0) {
                m->cur = (m->cur + m->count - 1) % m->count;
                changed = 1;
            }
        }
        if (ABS(sx) > 40) {
            D_800B04B4 = 10;
            if (it->x0 == 1) {
                if (sx > 0.0) {
                    u8 *p = it->x8;
                    if (*p != 0xFF) {
                        (*p)++;
                    }
                    if (((ItemB *)it)->xC != ((ItemB *)it)->xD) {
                        u8 *q = it->x8;
                        u8 v = *q;
                        if (((ItemB *)it)->xD < v) {
                            v = ((ItemB *)it)->xD;
                        }
                        *q = v;
                    }
                }
                if (sx < 0.0) {
                    u8 *p = it->x8;
                    if (*p != 0) {
                        (*p)--;
                    }
                    {
                        u8 min = ((ItemB *)it)->xC;
                        if (min != ((ItemB *)it)->xD) {
                            u8 *q = it->x8;
                            *q = (*q < min) ? min : *q;
                        }
                    }
                }
            }
            if (it->x0 == 0) {
                if (sx > 0.0) {
                    (*(s32 *)it->x8)++;
                    if (it->xC != it->x10) {
                        s32 *q = it->x8;
                        s32 v = *q;
                        if (it->x10 < v) {
                            v = it->x10;
                        }
                        *q = v;
                    }
                }
                if (sx < 0.0) {
                    (*(s32 *)it->x8)--;
                    if (it->xC != it->x10) {
                        s32 *q = it->x8;
                        s32 v = *q;
                        if (v < it->xC) {
                            v = it->xC;
                        }
                        *q = v;
                    }
                }
            }
            if (it->x0 == 2) {
                if (sx > 0.0) {
                    *(f32 *)it->x8 += it->x14;
                }
                if (sx < 0.0) {
                    *(f32 *)it->x8 -= it->x14;
                }
            }
            changed = 1;
        }
    }
    if ((D_80125C90 & 0x8000) && it->x0 == 3) {
        DebugMenu_Push(it->x8);
        changed = 1;
    }
    if (D_80125C90 & 0x4000) {
        DebugMenu_Pop();
        changed = 1;
    }
    if (D_80125C8C & 0x2000) {
        D_800B04B4 = 0;
    }
    return changed;
}
