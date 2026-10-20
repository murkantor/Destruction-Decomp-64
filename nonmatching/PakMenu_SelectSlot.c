typedef struct {
    s32 id;
    void *x4;
    u8 pad8[0x1C - 0x8];
} ResEnt;

typedef struct {
    u8 pad0[0x7588];
    ResEnt res[199];
    u8 pad[0x8B5C - 0x7588 - 199 * 0x1C];
    s16 count;
} Ui;

typedef struct {
    s32 count;
    s32 ids[1];
} SlotList;

extern Ui *gUi[];
extern s32 D_800958A8;
extern s32 D_800958AC;
extern s32 D_80096EA0;
extern char gSaveSlotText[];
extern char D_80123BF2[];
extern char D_80123C24[];
extern u8 D_80095CC8[];
extern SlotList D_80125BE0;

void Res_Load(s32, void *);
void Ui_RemoveElement(void *, s32);
void func_8005D8B8(void *, s32, s32);
void TrackMenu_FormatRecords(s32);
s32 Font_GetStringWidth(char *, s32);
void Ui_SetElementPos(void *, s32, s32, f32, f32);

void PakMenu_SelectSlot(s32 prev, s32 slot) {
    s32 ids[3] = { 0xB4, 0x3F, 0xB5 };
    ResEnt *ent;
    s32 i;
    s32 mode;
    f32 x;

ent = NULL; i = 0;    if (prev != -1) {
        for (; i < gUi[0]->count; i++) {
            if (prev + 0x112D0 == gUi[0]->res[i].id) {
                ent = &gUi[0]->res[i];
                break;
            }
        }
        Res_Load(slot + 0x112D0, ent->x4);
        ent->id = slot + 0x112D0;
        for (i = 0; i < gUi[0]->count; i++) {
            if (prev + 0x112DD == gUi[0]->res[i].id) {
                ent = &gUi[0]->res[i];
                break;
            }
        }
        Res_Load(slot + 0x112DD, ent->x4);
        ent->id = slot + 0x112DD;
    }
    mode = slot >= 4;
    if (D_800958A8 == 3 && D_800958AC == 2) {
        mode = 2;
    }
    if (D_80096EA0 != mode) {
        Ui_RemoveElement(gUi[0], ids[D_80096EA0]);
        func_8005D8B8(gUi[0], 2, ids[mode]);
        D_80096EA0 = mode;
    }
    TrackMenu_FormatRecords(slot);
    x = 250.0 - (f32)Font_GetStringWidth(gSaveSlotText, 0x111FC) * 0.5;
    switch (mode) {
        case 0:
            Ui_SetElementPos(gUi[0], 0, 0x17A, x, 164.0f);
            break;
        case 1:
            Ui_SetElementPos(gUi[0], 0, 0x17A, x, 134.0f);
            break;
        case 2:
            Ui_SetElementPos(gUi[0], 0, 0x17A, x, 134.0f);
            Ui_SetElementPos(gUi[0], 0, 0x17B, 250.0 - (f32)Font_GetStringWidth(D_80123BF2, 0x111FC) * 0.5, 164.0f);
            Ui_SetElementPos(gUi[0], 0, 0x17C, 250.0 - (f32)Font_GetStringWidth(D_80123C24, 0x111FC) * 0.5, 196.0f);
            break;
    }
    if (D_80095CC8[slot]) {
        Ui_SetElementPos(gUi[0], 2, 5, -65.0f, 103.0f);
    } else {
        Ui_SetElementPos(gUi[0], 2, 5, 68.0f, 103.0f);
    }
    for (i = 0; i < D_80125BE0.count; i++) {
        if (D_80125BE0.ids[i] == slot) {
            break;
        }
    }
    if (i == 0) {
        Ui_SetElementPos(gUi[0], 2, 0x9F, -20.0f, 73.0f);
    } else if (i == D_80125BE0.count - 1) {
        Ui_SetElementPos(gUi[0], 2, 0xA0, -20.0f, 73.0f);
    } else {
        Ui_SetElementPos(gUi[0], 2, 0x9F, 197.0f, 73.0f);
        Ui_SetElementPos(gUi[0], 2, 0xA0, 289.0f, 73.0f);
    }
}
