#include "common.h"
#include "m2c.h"

typedef struct {
    u8 pad[0x10];
    s32 x10;
    u8 pad14[0x50 - 0x14];
    u64 x50;
    u8 pad58[4];
    s32 x5C;
} Car;
typedef struct {
    s32 x0;
    u8 pad4[4];
    s32 x8;
    u8 padC[4];
    s32 x10;
    void *x14;
} Race;
typedef struct { u8 pad[0x10A8]; u64 x10A8; u8 pad10B0[0x1100 - 0x10B0]; Race *x1100; } Game;

extern Game *gGame;
extern s32 D_800958A8[];
extern s32 D_800958AC;
extern Car *D_800958F0;
extern s8 D_80125C1C;
extern s8 D_800D2E11;
extern u8 D_80124597;
extern u8 D_80124598[];
extern u8 D_8012459A[];
extern u8 D_8012459C[];
extern u8 D_8012459D[];
extern u8 D_8012459E[];
extern u8 D_8012459F[];
extern u8 D_801245A0[];
extern s32 rand(void);
extern void Announcer_ReplaceQueued(s32 *, s32, s32);
extern u64 Time_Get(u64 *);
extern s8 ItemList_IndexOf(void *, Car *);
extern void Announcer_QueueRandom(s32 *list, s32 n, s32 arg, s32 force, s32 unique);
extern void Announcer_Request(s32 id, s32 arg1, s32 check);
extern void Commentary_PickRandom(s32 *, s32, s32, s32);
extern f32 Car_GetSpeed(Car *);
#ifdef TARGET_PC
s32 Port_FieldAnnounceCarsLeft(s32 left);
/* the race record's car count, after its list */
typedef struct { u8 pad[0x74 + 4 * RACE_CARS]; s32 ncars; } RaceCars;
#endif

typedef struct { s32 v[11]; } L11;
typedef struct { s32 v[5]; } L5;
typedef struct { s32 v[4]; } L4;

extern L11 D_800B8840; /* 0x98, 0x8E..0x97 */
extern L5 D_800B886C;  /* 0xB9, 0xBA, 0xBC, 0xBB, 0xC1 */
extern L4 D_800B8880;  /* 0x66, 0xA1, 0xA2, 0xA3 */
extern L5 D_800B8890;  /* 0x88..0x8C */
extern f64 D_800B88A8; /* 5000000000.0 */
extern f64 D_800B88B0; /* 10000000000.0 */

void Announcer_UpdateRace(void) {
    L11 tbl;
    L5 lines;
    L4 lines4;
    L5 lines5;
    s32 l[2];
    Race *race = gGame->x1100;
    Car *car;
    u64 elapsed;
    s32 pos;
    s32 n;
    s32 r;

    if (D_800958A8[0] == 3 && (D_800958AC == 1 || D_800958AC == 2)) {
        return;
    }
    if (race->x8 == 0) {
        return;
    }
    n = race->x0 - 1;
#ifdef TARGET_PC
    /* the doubled field: no call-out while more cars are left than the original
       game's field has (port/src/field.c), and the lines (tbl) go to 10 */
    if (n < D_80125C1C && n > 0 && Port_FieldAnnounceCarsLeft(n) && n < 11) {
#else
    if (n < D_80125C1C && n > 0) {
#endif
        tbl = D_800B8840;
        D_80125C1C = n;
        if (n == 1) {
            n = rand() % 2;
        }
        Announcer_ReplaceQueued(tbl.v, 0xB, tbl.v[n]);
    }
    if (D_800958A8[0] == 3) {
        return;
    }
    car = D_800958F0;
    elapsed = car->x50 - Time_Get(&gGame->x10A8);
    pos = ItemList_IndexOf(&race->x14, car);
    if (pos == 1) {
        if (D_800D2E11 < 60) {
            D_800D2E11++;
        } else if (D_800D2E11 == 60 && D_800958A8[0] != 2) {
            lines = D_800B886C;
            D_800D2E11++;
            if (race->x10 == 1) {
                Announcer_QueueRandom(&lines.v[3], 2, 0, 1, 1);
            } else {
                Announcer_QueueRandom(lines.v, 4, 0, 1, 1);
            }
        }
    } else {
        D_800D2E11 = 0;
    }
    if ((u32)(car->x10 - 6) < 5 && !D_8012459A[1] && race->x10 == 1) {
        D_8012459A[1] = 1;
        Announcer_Request(0xA0, 0, 0);
    } else if ((u32)(car->x10 - 1) < 5 && !D_80124598[1]) {
        lines4 = D_800B8880;
        D_80124598[1] = 1;
        D_8012459A[1] = 1;
        Commentary_PickRandom(lines4.v, race->x10 == 0 ? 1 : 4, 0, 1);
    }
    if (race->x10 != 0) {
        return;
    }
    if (!D_8012459A[0] && Car_GetSpeed(car) >= 150.0f) {
        r = rand() % 100;
        D_8012459A[0] = 1;
        if (r < 26) {
            Announcer_Request(0xA8, 0, 0);
        }
    }
    if (D_800958A8[0] == 2) {
        return;
    }
    if (!D_80124597 && (f64)elapsed <= D_800B88A8) {
        r = rand() % 100;
        D_80124597 = 1;
        D_80124598[0] = 1;
        if (r < 21) {
            Announcer_Request(0x8D, 0, 0);
        }
    }
    if (!D_80124598[0] && (f64)elapsed <= D_800B88B0) {
        lines5 = D_800B8890;
        D_80124598[0] = 1;
        Commentary_PickRandom(lines5.v, 5, 0, 1);
    }
#ifdef TARGET_PC
    {
        /* the doubled field: last place (12th of the N64's 12) and the last three are
           those of the whole field */
        s32 last = ((RaceCars *)race)->ncars;

        if (last <= 12) {
            /* as on the N64 */
        } else if (pos == last) {
            pos = 12;
        } else if (pos >= last - 2) {
            pos = 10;
        } else if (pos >= 10) {
            pos = 9;
        }
    }
#endif
    if (car->x5C == 1 && pos == 12 && !D_8012459C[0]) {
        D_8012459C[0] = 1;
        Announcer_Request(0xBD, 0, 0);
    } else if (car->x5C == 2 && pos == 12 && !D_8012459D[0]) {
        D_8012459D[0] = 1;
        Announcer_Request(0xBE, 0, 0);
    } else if (car->x5C == 3 && pos == 12 && !D_8012459E[0]) {
        D_8012459E[0] = 1;
        Announcer_Request(0xBF, 0, 0);
    } else if (car->x5C >= 4 && pos == 12 && !D_8012459F[0]) {
        D_8012459F[0] = 1;
        Announcer_Request(0xC0, 0, 0);
    } else if (car->x5C >= 5 && pos >= 10 && !D_801245A0[0]) {
        l[0] = 0xC4;
        l[1] = 0xC5;
        D_801245A0[0] = 1;
        Commentary_PickRandom(l, 2, 0, 1);
    }
}
