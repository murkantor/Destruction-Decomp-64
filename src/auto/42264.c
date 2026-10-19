#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    u8 pad4[0x10 - 0x4];
} Entry;

extern u32 D_800958A8;
extern s32 D_800958AC;
extern s32 D_800958B0;
extern s32 D_80097A14;
extern Entry D_80095BE8[];
extern Entry *D_80095CA8[];
extern s32 D_80095CB8[];
typedef struct {
    s32 n;
    s32 items[12];
} IdList;

extern IdList D_80125BE0;

void Menu_BuildIdList(void) {
    s32 idx;
    Entry *list;
    s32 n;
    s32 i;
    s32 j;
    s32 lo;
    s32 w;
    s32 hi;
    s32 v;

    if (D_800958A8 == 2) {
        idx = D_80097A14;
    } else {
        idx = D_800958B0;
    }
    list = D_80095CA8[idx];
    n = D_80095CB8[idx];
    switch (D_800958A8) {
    case 0:
    case 1:
        D_80125BE0.n = 12;
        for (j = 0; j < n; j++) { D_80125BE0.items[j] = list++->x0; }
        for (j = n; j < 12; j++) { D_80125BE0.items[j] = D_80095BE8[j].x0; }
        break;
    case 2: {
        s32 k = 0;
        D_80125BE0.n = 8;
        for (i = 0; i < n; i++, list++) {
            v = list->x0;
            if ((u32)v >= 4) {
                D_80125BE0.items[k++] = v;
            }
        }
        for (j = n; j < 12; j++) {
            v = D_80095BE8[j].x0;
            if ((u32)v >= 4) {
                D_80125BE0.items[k++] = v;
            }
        }
        break;
    }
    case 3: {
        s32 k = 0;
        if (D_800958AC == 3) {
            lo = 4;
            w = 12;
            D_80125BE0.n = 8;
        } else {
            lo = 0;
            w = 4;
            D_80125BE0.n = 4;
        }
        hi = lo + w;
        for (i = 0; i < n; i++, list++) {
            v = list->x0;
            if (v >= lo) {
                if (v < hi) {
                    D_80125BE0.items[k++] = v;
                }
            }
        }
        for (i = n; i < 12; i++) {
            v = D_80095BE8[i].x0;
            if (v >= lo) {
                if (v < hi) {
                    D_80125BE0.items[k++] = v;
                }
            }
        }
        break;
    }
    }
}
