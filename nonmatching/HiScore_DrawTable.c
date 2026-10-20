typedef struct {
    char name[0x14];
    char a[10];
    char b[10];
    char c[10];
    s8 flag;
} HiRec;
typedef struct { char col[3][9]; } HiRow;

extern s32 D_800958A4;
extern s32 D_800958B4;
extern s32 D_80096038[];
extern HiRec D_801238F0[];
extern char gSaveSlotText[][50];
extern HiRow D_80130138[];
extern s32 D_800971DC[][3];
extern f32 D_80097278[];
extern void *gUi;

char *strcpy(char *, const char *);
f32 Font_RightAlignX(char *, s32, f32);
void Ui_SetElementPos(void *, s32, s32, f32, f32);

#define NUM_SHOWN (D_80096038[D_800958B4] < 7 ? D_80096038[D_800958B4] : 6)

void HiScore_DrawTable(f32 *xs, s32 ncols, s32 rank) {
    s32 skip;
    s32 i;
    s32 row;
    s32 e;
    s32 c;
    s32 idx;

    skip = D_800958A4;
    rank--;
    for (i = 0; i < rank; i++) {
        skip -= (D_801238F0[i].flag != -1);
    }
    for (row = 0, e = rank; e < (D_80096038[D_800958B4] < 7 ? D_80096038[D_800958B4] - skip + rank : 6 - skip + rank); row++, e++) {
        strcpy(gSaveSlotText[row], D_801238F0[e].name);
        strcpy(D_80130138[row].col[0], D_801238F0[e].a);
        strcpy(D_80130138[row].col[1], D_801238F0[e].b);
        strcpy(D_80130138[row].col[2], D_801238F0[e].c);
        for (c = 0; c < ncols; c++) {
            Ui_SetElementPos(gUi, 0, D_800971DC[row][c], Font_RightAlignX(D_80130138[row].col[c], 0x111F6, xs[c]), D_80097278[row]);
        }
    }
    row = (D_80096038[D_800958B4] < 7 ? D_80096038[D_800958B4] - skip : 6 - skip);
    for (e = rank; e < 12; e++) {
        if (D_801238F0[e].flag == -1) {
            continue;
        }
        idx = (e < row + rank) ? row + rank : e;
        strcpy(gSaveSlotText[row], D_801238F0[idx].name);
        strcpy(D_80130138[row].col[0], D_801238F0[idx].a);
        strcpy(D_80130138[row].col[1], D_801238F0[idx].b);
        strcpy(D_80130138[row].col[2], D_801238F0[idx].c);
        for (c = 0; c < ncols; c++) {
            Ui_SetElementPos(gUi, 0, D_800971DC[row][c], Font_RightAlignX(D_80130138[row].col[c], 0x111F6, xs[c]), D_80097278[row]);
        }
        row++;
    }
}
