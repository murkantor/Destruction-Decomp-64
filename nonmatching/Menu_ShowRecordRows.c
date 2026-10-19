typedef struct { s32 v[4][2]; } PosTbl;
typedef struct { f32 v[3]; } ColTbl;
typedef struct { s32 v[4]; } RowTbl;
typedef struct {
    char name[16];
    s32 x10;
    s32 x14;
    s32 x18;
    s32 x1C;
} Record;

extern PosTbl D_800BB280;
extern ColTbl D_800BB2A0;
extern RowTbl D_800BB2AC;
extern char D_800BAF68[];
extern char gSaveSlotText[][50];
extern char D_80130138[][27];
extern Record D_80131D88[];
extern s32 D_800972F4[];
extern s32 D_800971DC[][3];
extern void *gUi[];

char *strcpy(char *, const char *);
s32 sprintf(char *, const char *, ...);
f32 func_8003ECC0(char *, s32, f32);
void Ui_AddElement(void *, s32, s32);
void Ui_RemoveElement(void *, s32);
void Ui_SetElementPos(void *, s32, s32, f32, f32);

void Menu_ShowRecordRows(s32 prev, s32 first) {
    PosTbl pos = D_800BB280;
    ColTbl xs = D_800BB2A0;
    RowTbl ys = D_800BB2AC;
    s32 i;
    s32 j;
    s32 id;
    s32 k;

    for (i = 0; i < 4; i++) {
        strcpy(gSaveSlotText[i], D_80131D88[D_800972F4[first + i]].name);
        sprintf(D_80130138[i], D_800BAF68, D_80131D88[D_800972F4[first + i]].x10);
        sprintf(D_80130138[i] + 9, D_800BAF68, D_80131D88[D_800972F4[first + i]].x14);
        sprintf(D_80130138[i] + 18, D_800BAF68, D_80131D88[D_800972F4[first + i]].x18);
        for (j = 0; j < 3; j++) {
            Ui_SetElementPos(gUi[0], 0, D_800971DC[i][j], func_8003ECC0(D_80130138[i] + j * 9, 0x111F6, xs.v[j]), ys.v[i]);
        }
    }
    if (prev == -1) {
        i = 0; while (1) { Ui_AddElement(gUi[0], 2, first + i + 0x139); Ui_SetElementPos(gUi[0], 2, first + i + 0x139, pos.v[i][0], pos.v[i][1]); if (++i >= 4) return; }
    } else {
        for (i = 0; i < 4; i++) {
            k = i + 0x139;
            Ui_RemoveElement(gUi[0], prev + k);
            id = first + k;
            Ui_AddElement(gUi[0], 2, id);
            Ui_SetElementPos(gUi[0], 2, id, pos.v[i][0], pos.v[i][1]);
        }
    }
}
