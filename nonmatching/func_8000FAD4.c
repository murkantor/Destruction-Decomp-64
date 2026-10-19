typedef struct { u8 data[8]; } HeapBlock;
typedef struct {
    s32 x0;
    s32 x4;
    u8 pad8[0xC];
    s32 x14;
    s32 x18;
    f32 x1C;
    f32 x20;
    u8 pad24[0x14];
    u8 x38;
    u8 pad39[0xF];
} UiDesc48;
typedef struct { u8 pad[0xC]; u8 xC; u8 padD[0x34 - 0xD]; s32 x34; u8 pad38[0x10]; void *x48; } CarInfo;
typedef struct { u8 pad[0x74]; CarInfo *x74[12]; u8 padA4[0xA4 - 0x74 - 0x30]; s32 xA4; } Race;
typedef struct { u8 pad[0x1100]; Race *x1100; } Game;
typedef struct { s32 a4; s32 a8; s32 ac; } Mode;
typedef struct { s32 *x0; } Img;
typedef struct { u8 pad[0x10]; Img *x10; u8 pad14[0x88 - 0x14]; s32 x88; s32 x8C; } UiElem;
typedef struct { s32 a; s32 b; u8 pad8[0x20]; f32 pos[4][2]; u8 pad48[0]; } PosEnt;

extern Game *gGame;
extern s8 D_800D31CC;
extern Mode D_800958A4;
extern s32 D_800958C0[];
extern s32 D_8007F288[];
extern s32 D_8007F298[];
extern s32 D_800958B4;
extern PosEnt D_8007F2C4[];
extern u8 D_800958A1;
extern u8 gMainHeap[];

u32 Res_GetSize(s32);
void Res_Load(s32, void *);
void Ui_AddSprite(void *, UiDesc48 *);
void func_8005D8B8(void *, s32, s32);
UiElem *Ui_FindElement(void *, s32, s32);
void Ui_AddSpriteElement(void *, UiDesc48 *);
void Heap_AllocAlt(void *, HeapBlock *, s32, s32, s32);
void *HeapBlock_Ptr(HeapBlock *);
void Heap_Free(void *, HeapBlock *);

s32 func_8000FAD4(void *ui) {
    UiDesc48 descs[15];
    HeapBlock blk;
    u32 count;
    u32 size;
    UiDesc48 *d;
    u8 found;
    s8 k;
    s8 i;
    s32 j;
    u32 m;
    s32 id;
    s32 *data;
    CarInfo *car;

    size = Res_GetSize(0x1129C);
    Res_Load(0x1129C, descs);
    count = size / sizeof(UiDesc48);
    id = -1;
    found = 0;
    k = 0;
    data = NULL;
    for (i = 0; i < gGame->x1100->xA4; i++) {
        car = gGame->x1100->x74[i];
        if (car->xC == 0) {
            continue;
        }
        for (j = 0; j < D_800D31CC; j++) {
            if (D_800958A4.a8 == 3 && D_800958A4.ac == 2) {
                if (D_800958A4.a4 == 2) {
                    if (i == 0 && j == 0) {
                        found = 1;
                    } else if (i == 3 && j == 1) {
                        found = 1;
                    }
                } else if (i == j) {
                    found = 1;
                }
            } else if (car->x34 == D_800958C0[j]) {
                found = 1;
            }
            if (found) {
                id = D_8007F288[j];
                break;
            }
        }
        if (!found) {
            id = D_8007F298[k];
            k++;
        } else {
            found = 0;
        }
        d = descs;
        for (m = 0; m < count; m++, d++) {
            if (d->x4 == id) {
                break;
            }
        }
        if (D_800958A4.a8 == 3 && D_800958A4.ac == 2 && D_800D31CC == 2 && (i == 1 || i == 2)) {
            d->x14 = 0x11310;
            d->x18 = 0x11311;
        }
        Ui_AddSprite(ui, d);
        if (d->x38) {
            func_8005D8B8(ui, 1, id);
        }
        gGame->x1100->x74[i]->x48 = Ui_FindElement(ui, 1, id);
    }
    Res_Load(0x1129D, descs);
    descs[0].x14 = D_8007F2C4[D_800958B4].a;
    descs[0].x18 = D_8007F2C4[D_800958B4].b;
    descs[0].x1C = D_8007F2C4[D_800958B4].pos[D_800D31CC - 1][0];
    descs[0].x20 = D_8007F2C4[D_800958B4].pos[D_800D31CC - 1][1];
    d = descs;
    if (D_800958A1) {
        Heap_AllocAlt(gMainHeap, &blk, Res_GetSize(descs[0].x14), 7, 0);
        data = HeapBlock_Ptr(&blk);
        Res_Load(descs[0].x14, data);
    }
    Ui_AddSpriteElement(ui, d);
    if (descs[0].x38) {
        func_8005D8B8(ui, 2, 14);
    }
    if (D_800958A1) {
        UiElem *e = Ui_FindElement(ui, 2, 14);
        s32 r;
        for (r = 0; r < e->x8C; r++) {
            s32 *dst = e->x10->x0 + r * e->x88;
            s32 *src = data + e->x88 - 1;
            s32 c;
            for (c = 0; c < e->x88; c++) {
                *dst++ = *src--;
            }
            data += e->x88;
        }
        Heap_Free(gMainHeap, &blk);
    }
    return 0;
}
