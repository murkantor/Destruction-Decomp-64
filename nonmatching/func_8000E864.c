typedef struct { u8 pad[0x10]; s16 **x10; u8 pad14[0x14]; s16 *x28; } Elem;
typedef struct { u8 pad[0x1058]; u64 x1058; } Game;
extern Game *gGame;
extern void *gUi;
extern s32 D_800D1AE8[];
extern s16 *D_800D1AEC;
extern s32 D_800D1AF0;
extern u64 D_800D1AF8;
extern u64 D_800D1B00;
s32 Res_GetSize(s32);
void *Mem_AllocAligned(s32, s32, s32);
Elem *Ui_FindElement(void *, s32, s32);
u64 Time_Get(u64 *);

void func_8000E864(void) {
    s32 size = Res_GetSize(0x11302);
    s16 *buf;
    Elem *e;
    s32 i;

    D_800D1AE8[0] = size / 2;
    buf = Mem_AllocAligned(size, 7, 0);
    e = Ui_FindElement(gUi, 2, 0x1F);
    D_800D1AEC = *e->x10;
    *e->x10 = buf;
    e->x28 = buf;
    D_800D1AF0 = 0;
    D_800D1B00 = Time_Get(&gGame->x1058);
    D_800D1AF8 = 100000;
    for (i = 0; i < D_800D1AE8[0]; i++) {
        *buf++ = 1;
    }
}
