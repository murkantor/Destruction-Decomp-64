#include "common.h"
#include "m2c.h"

typedef struct { f32 x, y, z; } Vec3f;
typedef struct { u8 pad[0x10]; s8 x10; } Car;
typedef struct {
    u8 pad[0xC];
    s32 xC;
    s32 x10;
    u8 pad14[4];
    u64 x18;
    u8 x20;
    u8 x21;
    u8 x22;
    u8 x23;
    u8 x24;
    u8 x25;
    u8 x26;
    u8 pad27;
    Car *x28;
    u8 pad2C[0x3C];
} Player;
typedef struct {
    u8 (*x0)(Player *);
    u8 (*x4)(Player *);
    u8 (*x8)(Player *);
    u8 (*xC)(Player *);
    u8 (*x10)(Player *);
    u8 (*x14)(Player *);
    u8 (*x18)(Player *);
    u8 (*x1C)(Player *);
    void (*x20)(void);
} RuleHandlers;
typedef struct { u8 pad[0x10FC]; s16 x10FC; u8 x10FE; } Game;
extern Game *gGame;
extern u8 D_8011EEFC;
extern s16 D_8011EEEA;
extern s32 D_800958A4;
extern RuleHandlers *D_80097330;
extern void *D_8011EEF8[];
extern u64 gGameTime;
extern Vec3f D_800BBF60;
extern Vec3f D_800BBF6C;
void func_80024840(Car *);
u64 Time_Get(u64 *);
void Sound_PlayAt(void *, Vec3f, Vec3f, s32);
void Players_SyncAll(Player *);
void Players_SyncTeammate(Player *);

void Rules_UpdatePlayer(Player *arg0) {
    u8 r = 0;
    Vec3f a = D_800BBF60;
    Vec3f b = D_800BBF6C;
    Player *p = arg0;
    RuleHandlers *h;

    if (D_8011EEFC == 0 && D_8011EEEA == gGame->x10FC && p->x28->x10 <= D_800958A4 - 1) {
    func_80024840(p->x28);
    h = &D_80097330[gGame->x10FC];
    if (gGame->x10FE && h->x20) {
        h->x20();
    }
    gGame->x10FE = 0;
    if (h->x1C) {
        r = h->x1C(p);
    }
    if (Time_Get(&gGameTime) >= p->x18) {
        if (p->x21) {
            if (h->x10 && (r = h->x10(p))) {
                Sound_PlayAt(D_8011EEF8[0], a, b, 6);
            }
        } else if (p->x22) {
            if (h->x14 && (r = h->x14(p))) {
                Sound_PlayAt(D_8011EEF8[0], a, b, 5);
            }
        } else if (p->x23) {
            if (h->x0 && (r = h->x0(p))) {
                Sound_PlayAt(D_8011EEF8[0], a, b, 7);
            }
        } else if (p->x24) {
            if (h->x4 && (r = h->x4(p))) {
                Sound_PlayAt(D_8011EEF8[0], a, b, 7);
            }
        } else if (p->x25) {
            if (h->x8 && (r = h->x8(p))) {
                Sound_PlayAt(D_8011EEF8[0], a, b, 7);
            }
        } else if (p->x26) {
            if (h->xC && (r = h->xC(p))) {
                Sound_PlayAt(D_8011EEF8[0], a, b, 7);
            }
        } else if (p->x20) {
            if (h->x18 && (r = h->x18(p))) {
                Sound_PlayAt(D_8011EEF8[0], a, b, 8);
            }
        }
    }
    if (r) {
        if (p->xC == 2) {
            Players_SyncAll(p);
        }
        if (p->xC == 1) {
            Players_SyncTeammate(p);
        }
    }
    }
}
