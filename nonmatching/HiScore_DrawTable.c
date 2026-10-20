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

void HiScore_DrawTable(f32 *xs, s32 ncols, s32 rank0) {
    s32 skip;
    s32 rank;
    s32 i;
    s32 row;
    s32 e;
    s32 c;
    s32 idx;
    s32 lim;

    skip = D_800958A4;
    rank = rank0 - 1;
    for (e = 0; e < rank; e++) {
        skip -= (D_801238F0[e].flag != -1);
    }
    row = 0;
    i = rank;
    goto test;
    do {
        strcpy(gSaveSlotText[row], D_801238F0[i].name);
        strcpy(D_80130138[row].col[0], D_801238F0[i].a);
        strcpy(D_80130138[row].col[1], D_801238F0[i].b);
        strcpy(D_80130138[row].col[2], D_801238F0[i].c);
        for (c = 0; c < ncols; c++) {
            Ui_SetElementPos(gUi, 0, D_800971DC[row][c], Font_RightAlignX(D_80130138[row].col[c], 0x111F6, xs[c]), D_80097278[row]);
        }
        row++;
        i++;
    test:;
    } while (i < (D_80096038[D_800958B4] < 7 ? D_80096038[D_800958B4] - skip + rank : 6 - skip + rank));
    row = (D_80096038[D_800958B4] < 7 ? D_80096038[D_800958B4] - skip : 6 - skip);
    lim = row + rank;
    for (e = rank; e < 12; e++) {
        if (D_801238F0[e].flag == -1) {
            continue;
        }
        idx = (e < lim) ? lim : e;
        strcpy(gSaveSlotText[row], D_801238F0[idx].name);
        strcpy(D_80130138[row].col[0], D_801238F0[idx].a);
        strcpy(D_80130138[row].col[1], D_801238F0[idx].b);
        strcpy(D_80130138[row].col[2], D_801238F0[idx].c);
        for (c = 0; c < ncols; c++) {
            Ui_SetElementPos(gUi, 0, D_800971DC[row][c], Font_RightAlignX(D_80130138[row].col[c], 0x111F6, xs[c]), D_80097278[row]);
        }
        row++;
        lim++;
    }
}
/* agent61: 30 diff lines (was 52). Changes: local rank = rank0 - 1 declared after skip (fixes the 44/52 stack slots),
 * loop 1 reuses e (s5) and loop 2 has its own counter i (s6), loop 3 keeps lim = row + rank as its own induction
 * variable (lim++ with row++; loop 3 now matches), and loop 2 is "goto test; do {...} while (cond)", which gives
 * the right registers (rank spilled, i in s6). Left: the original has the full loop 2 test before the loop, and at
 * the bottom only "n<7 ? arm1 + j CMP : j ARM2". A plain for() (scratch hs5, 74 lines) gives that layout except that
 * the bottom arm1 gets its own slt/bnez instead of "j CMP", and rank then takes s7 while i is spilled. */
