typedef struct { u8 pad[0xB4]; u8 xB4; } Model;
typedef struct { u8 pad[0xC]; Model *xC; } CarRes;
typedef struct { s32 xA74; u8 pad[0x10]; } Part;
typedef struct Car {
    u8 pad0[0xE]; u8 xE; u8 padF[0x10 - 0xF]; s32 x10; s32 x14; s32 x18; u8 pad1C[0x20 - 0x1C]; s32 x20;
    u8 pad24[0x30 - 0x24]; s32 x30; u8 pad34[0x3C - 0x34]; CarRes *x3C; u8 pad40[0x50 - 0x40]; u64 x50;
    u8 pad58[0x94 - 0x58]; f32 x94; u8 pad98[0xA74 - 0x98]; Part parts[7]; u8 padB00[0xBE0 - 0xB00]; u8 xBE0;
} Car;
typedef struct { u8 pad[0x10]; s32 x10; u8 x14[4]; } Race;
typedef struct { u8 pad[0x10A8]; u64 x10A8; u8 pad10B0[0x10FC - 0x10B0]; s16 x10FC; u8 pad10FE[0x1100 - 0x10FE]; Race *x1100; } Game;
typedef struct { s32 v[3]; } List3;
typedef struct { s32 v[4]; } List4;
typedef struct { s32 v[5]; } List5;
extern Game *gGame;
extern void *gUi;
extern u32 D_800958B0;
extern u8 D_800958A1;
extern List3 D_800B9100;
extern List4 D_800B910C;
extern List5 D_800B911C;
extern const f64 D_800B9130;
extern const f64 D_800B9150;
extern const f64 D_800B9158;
extern const f64 D_800B9160;
extern const f64 D_800B9168;
extern const f64 D_800B9170;
extern const f64 D_800B9178;
extern const f64 D_800B9180;
extern const f64 D_800B9188;
extern const f64 D_800B9190;
extern const f64 D_800B9198;
extern const f64 D_800B91A0;
u64 Time_Get(void *);
f32 Time_ToSeconds(void *);
void Msg_Queue(s32, s32, s32, s32);
void func_8000F1C4(void *, Car *, s32, s32);
f32 func_8001FB40(Car *);
void Standings_Resort(void *, Car *);
void func_8002A5CC(Car *, s32);
void Commentary_PickRandom(s32 *, s32, s32, s32);
void Announcer_Request(s32, s32, s32);

void func_80029A18(Car *atk, Car *vic, s32 part, s32 dmg) {
    Race *race = gGame->x1100;
    f32 m;
    f64 t;
    f32 f;
    s32 p;
    s32 old;
    s32 new;
    u64 now;
    f64 base;

    if (atk->x30 == 0 && atk->x3C->xC->xB4 == 7) {
        return;
    }
    if (vic->x30 == 0 && vic->x3C->xC->xB4 == 7) {
        return;
    }
    if (atk->x30 == 1 && vic->x30 == 0) {
        m = 1.0f;
        switch (D_800958B0) {
        case 0:
            m = 1.15f;
            break;
        case 1:
            m = 1.25f;
            break;
        case 2:
            m = 1.35f;
            break;
        case 3:
            m = 1.5f;
            break;
        }
        dmg = dmg * m;
    }
    atk->x14 = dmg;
    if (vic->x10 > 0) {
        if (vic->xBE0 == 0) {
            t = dmg * D_800B9130;
            vic->x10 = vic->x10 - t;
            vic->parts[part].xA74 = vic->parts[part].xA74 - t;
        }
        if (D_800958A1) {
            switch (part) {
            case 1:
                p = 4;
                break;
            case 2:
                p = 5;
                break;
            case 3:
                p = 6;
                break;
            case 4:
                p = 1;
                break;
            case 5:
                p = 2;
                break;
            case 6:
                p = 3;
                break;
            default:
                p = part;
                break;
            }
        } else {
            p = part;
        }
        func_8000F1C4(gUi, vic, dmg, p);
        if (vic->x10 < 0) {
            vic->x10 = 0;
        }
        if (vic->parts[part].xA74 < 0) {
            vic->parts[part].xA74 = 0;
        }
        old = atk->x18 / 30;
        if (func_8001FB40(atk) > D_800B9150) {
            t = dmg * D_800B9158;
            if (t > D_800B9160) {
                atk->x18 += 100;
            } else {
                f = t;
                if (f >= D_800B9168 && f <= D_800B9170) {
                    f = 1.0f;
                }
                atk->x18 += (s32)f;
            }
        }
        new = atk->x18 / 30;
        if (race->x10 == 0 && old < new && atk->x30 == 0 && atk->x10 > 0) {
            now = Time_Get(&gGame->x10A8);
            if (now >= atk->x50) {
                base = now;
            } else {
                base = atk->x50;
            }
            atk->x50 = base + (new - old) * 10 * D_800B9178;
            if ((f64)atk->x50 > (f64)now + D_800B9180) {
                atk->x50 = (f64)now + D_800B9188;
            }
            Msg_Queue(0xF0, 0, 1, 0);
        }
        if (gGame->x10FC == 0x1C) {
            if (vic->x30 == 0 && dmg >= 30) {
                List3 l1 = D_800B9100;
                Commentary_PickRandom(l1.v, 3, 0, 1);
            } else if (atk->x30 == 0) {
                if (dmg >= 45) {
                    Announcer_Request(0xCD, 0, 1);
                } else if (dmg >= 35) {
                    s32 l2[2];
                    l2[0] = 0xB4;
                    l2[1] = 0xB5;
                    Commentary_PickRandom(l2, 2, 0, 1);
                } else if (dmg >= 25) {
                    Announcer_Request(0xB2, 0, 1);
                } else if (dmg >= 15) {
                    List4 l3 = D_800B910C;
                    Commentary_PickRandom(l3.v, 4, 0, 1);
                } else if (dmg >= 10) {
                    Announcer_Request(0xB3, 0, 1);
                }
            }
        }
        if (old != new) {
            Standings_Resort(race->x14, atk);
        }
        func_8002A5CC(vic, part);
        return;
    }
    if (gGame->x10FC != 0x1C) {
        return;
    }
    if (!(vic->x94 < Time_ToSeconds(&gGame->x10A8))) {
        return;
    }
    if (vic->xE) {
        return;
    }
    if (race->x10 == 1) {
        atk->x20 += 20;
    } else {
        old = atk->x18 / 30;
        atk->x18 += 20;
        new = atk->x18 / 30;
        if (old < new && atk->x30 == 0 && atk->x10 > 0) {
            now = Time_Get(&gGame->x10A8);
            if (now >= atk->x50) {
                base = now;
            } else {
                base = atk->x50;
            }
            atk->x50 = base + (new - old) * 10 * D_800B9190;
            if ((f64)atk->x50 > (f64)now + D_800B9198) {
                atk->x50 = (f64)now + D_800B91A0;
            }
            Msg_Queue(0xF0, 0, 1, 0);
        }
    }
    Standings_Resort(race->x14, atk);
    vic->xE = 1;
    if (vic->x30 == 1 && atk->x30 == 0) {
        List5 l4 = D_800B911C;
        Commentary_PickRandom(l4.v, (race->x10 == 1) | 4, 0x12C, 1);
    }
    if (vic->x30 == 0) {
        if (atk->xBE0 == 0) {
            atk->x10 -= dmg * 2;
        }
        if (atk->x10 < 0) {
            atk->x10 = 0;
        }
    }
}
