typedef struct { s32 a; s32 b; } Pair;
typedef struct { s32 x0; s32 x4; Pair x8; Pair x10; } Slot;
typedef struct {
    u8 name0[9];
    u8 name1[9];
    u8 name2[9];
    u8 pad1B[5];
    s32 x20;
    s32 x24;
    Pair x28;
    Pair x30;
} HiEntry;
typedef struct { s32 x0; u8 name[0xC]; s32 x10; } HiSmall;
typedef struct { s32 x0; s32 x4; s32 x8; } Entry4A574;

extern HiEntry D_80097A28[];
extern Slot D_80130518[];
extern u8 D_80097D5C[];
extern s32 D_80125C5C;
extern s32 D_800958B4;
extern HiSmall D_80097CC8[];
extern s32 D_80097D48;
extern Entry4A574 D_80097D18[];

void HiScore_ApplyNewEntries(void) {
    s32 i;
    s32 j;
    s32 k;

    for (i = 0; i < 12; i++) {
        if (D_80130518[i].x0 != 0) {
            D_80097A28[i].x20 = D_80130518[i].x0;
            for (j = 0; j < 8; j++) {
                D_80097A28[i].name0[j] = D_80097D5C[j];
            }
        }
        if (D_80130518[i].x8.a != 0 || D_80130518[i].x8.b != 0) {
            D_80097A28[i].x28 = D_80130518[i].x8;
            for (j = 0; j < 8; j++) {
                D_80097A28[i].name1[j] = D_80097D5C[j];
            }
        }
        if (D_80130518[i].x10.a != 0 || D_80130518[i].x10.b != 0) {
            D_80097A28[i].x30 = D_80130518[i].x10;
            for (j = 0; j < 8; j++) {
                D_80097A28[i].name2[j] = D_80097D5C[j];
            }
        }
    }
    if (D_80125C5C != -1) {
        D_80097CC8[0].x10 = D_80125C5C;
        D_80097CC8[0].x0 = D_800958B4;
        for (j = 0; j < 8; j++) {
            D_80097CC8[0].name[j] = D_80097D5C[j];
        }
    }
    if (D_80097D18[D_80097D48].x0 != -1) {
        D_80097CC8[1].x10 = D_80097D18[D_80097D48].x0;
        for (j = 0; j < 8; j++) {
            D_80097CC8[1].name[j] = D_80097D5C[j];
        }
    }
    if (D_80097D18[D_80097D48].x4 != -1) {
        D_80097CC8[2].x10 = D_80097D18[D_80097D48].x4;
        for (j = 0; j < 8; j++) {
            D_80097CC8[2].name[j] = D_80097D5C[j];
        }
    }
    if (D_80097D18[D_80097D48].x8 != -1) {
        D_80097CC8[3].x10 = D_80097D18[D_80097D48].x8;
        for (j = 0; j < 8; j++) {
            D_80097CC8[3].name[j] = D_80097D5C[j];
        }
    }
}
