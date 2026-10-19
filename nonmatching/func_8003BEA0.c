typedef struct {
    s32 x0;
    u8 pad4[6];
    u8 ext[4];
    u8 name[16];
    u8 pad1E[2];
} SaveSlot;

extern SaveSlot gSaveSlots[16];
extern char gSaveSlotText[][16][0x32];
extern char D_80123BD0[][0x32];
extern char D_80123EE0[][0x32];
extern char D_80124200[];
extern u8 D_80095898;
extern void *gUi;
extern s32 D_80095908;
extern s32 D_80097D80;
extern s32 D_80097350;
extern char D_800B9DF8[];
extern char D_800B9E00[];

u8 Pak_Open(s32 *);
void Pak_RefreshSaveSlots(void);
void Ui_ResetScreen(s32, s32);
void Ui_AddElement(void *, s32, s32);
void func_80063380(s32);
void func_80063564(s32, s32);
void Text_MapChars2(void *, void *, s32);
void *func_8001007C(char *);
void func_800100BC(void *, s32);
s32 sprintf(char *, const char *, ...);

void func_8003BEA0(void) {
    u8 name[16];
    u8 ext[4];
    char sep[3] = " - ";
    s32 status;
    s32 count = 0;
    s32 i;
    s8 j;
    s32 k;
    u8 *e;
    u8 flag;
    u8 found;

    if (Pak_Open(&status) && status != 0 && status != 5) {
        func_80063380(0);
        func_80063564(2, 1);
        return;
    }
    Pak_RefreshSaveSlots();
    D_80095898 = 0;
    Ui_ResetScreen(0x112C4, 0x112C5);
    Ui_AddElement(gUi, 2, 1);
    for (i = 0; i < 16; i++) {
        Text_MapChars2(gSaveSlots[i].name, name, 16);
        flag = 0;
        for (k = 0; k < 16; k++) {
            if (name[k] == ' ') {
                if (k == 0) {
                    flag = 1;
                }
            }
            if (name[k] == '\\') {
                name[k] = ' ';
            }
            if (flag) {
                name[k] = '-';
            }
        }
        for (j = 0; j < 16 && name[j] == ' '; j++) {
        }
        if (j >= 16) {
            sprintf(gSaveSlotText[0][i], "EMPTY");
            sprintf(gSaveSlotText[1][i], " ");
            continue;
        }
        e = ext;
        Text_MapChars2(gSaveSlots[i].ext, e, 4);
        flag = 0;
        for (k = 0; k < 4; k++) {
            if (e[k] == '\\') {
                e[k] = ' ';
            }
            if (flag) {
                e[k] = '-';
            }
        }
        sprintf(gSaveSlotText[0][i], name);
        D_80123BD0[i][0] = 0;
        if (!(ext[0] == ' ' && ext[1] == ext[0] && ext[2] == ext[1] && ext[3] == ext[2])) {
            sprintf(D_80123BD0[i], sep);
            sprintf(&D_80123BD0[i][3], ext);
            sprintf(&D_80123BD0[i][7], D_800B9DF8);
        }
        k = gSaveSlots[i].x0 / 256;
        sprintf(D_80123EE0[i], D_800B9E00, k);
        count++;
    }
    for (i = 0; i < 33; i++) {
        Ui_AddElement(gUi, 0, i + 0x17A);
    }
    found = 0;
    for (i = 0; i < 16; i++) {
        if (!found && gSaveSlots[i].x0 != 0) {
            func_800100BC(func_8001007C(gSaveSlotText[0][i]), 0x111FE);
            func_800100BC(func_8001007C(gSaveSlotText[1][i]), 0x111FE);
            D_80095908 = i;
            found = 1;
        } else {
            func_800100BC(func_8001007C(gSaveSlotText[0][i]), 0x11200);
            func_800100BC(func_8001007C(gSaveSlotText[1][i]), 0x11200);
        }
    }
    sprintf(D_80124200, D_800B9E00, D_80097D80 / 256);
    D_80097350 = count;
}
