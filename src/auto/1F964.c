#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0xC]; s16 xC; } Game;
typedef struct {
    u8 pad[0xC];
    u8 xC;
    u8 padD[0x2C - 0xD];
    void *x2C;
    u8 pad30[4];
    u32 x34;
    u8 pad38[0x98 - 0x38];
    u8 x98[0x28];
    u8 xC0[0x28];
    u8 xE8[0x28];
    u8 x110[0x28];
    u8 x138[0x28];
    u8 pad160[0xB3C - 0x160];
    u8 xB3C[8];
    u8 xB44[8];
    u8 xB4C[8];
    u8 xB54[8];
    u8 xB5C[8];
    u8 xB64[8];
    s32 xB6C;
    s32 xB70;
    s32 xB74;
    s32 xB78;
    s32 xB7C;
    s32 xB80;
} Car;

extern Game *gGame;
extern u8 gMainHeap[];
extern s32 D_800958A8;
extern s32 D_800958AC;
extern s8 D_800D31CC;
extern u32 D_800958C0;
extern u32 D_800958C4;
extern u32 D_800958C8;
extern s32 D_800958B4;
extern s32 D_800919F4[];
extern s32 D_80091A54[];
extern u8 D_80124360;
void Obj_FreeSubBuffers(s32 *);
void Skid_Free(s32 *);
void func_80013174(s32 *);
void ModelInst_Free(void *);
void Res_Release(s32);
void Debris_FreeCar(Car *);
void Heap_Free(void *, void *);
void Mem_Free(void *);

void Car_Destroy(Car *o) {
    s8 idx;

    if (D_800958A8 == 3 && D_800958AC == 2) {
        if (D_800D31CC == 2) {
            switch (o->x34) {
                case 0:
                    o->x34 = D_800958C0;
                    break;
                case 1:
                    o->x34 = D_800958C0;
                    break;
                case 2:
                    o->x34 = D_800958C4;
                    break;
                case 3:
                    o->x34 = D_800958C4;
                    break;
            }
        } else {
            switch (o->x34) {
                case 0:
                    o->x34 = D_800958C0;
                    break;
                case 1:
                    o->x34 = D_800958C0;
                    break;
                case 2:
                    o->x34 = D_800958C8;
                    break;
                case 3:
                    o->x34 = D_800958C8;
                    break;
            }
        }
    }
    if (o->xC) {
#ifdef TARGET_PC
        /* the low byte of the s32 at 0x34 (Car_Create reads it whole) */
        idx = (u8)*(s32 *)((u8 *)o + 0x34);
#else
        idx = ((u8 *)o)[0x37];
#endif
        if (gGame->xC == 2) {
            if (o->xB6C) {
                Obj_FreeSubBuffers(&o->xB6C);
            }
            if (o->xB70) {
                Skid_Free(&o->xB70);
                Skid_Free(&o->xB74);
                Skid_Free(&o->xB78);
                Skid_Free(&o->xB7C);
            }
            if (o->xB80) {
                func_80013174(&o->xB80);
            }
        }
        ModelInst_Free(o->x98);
        ModelInst_Free(o->xC0);
        ModelInst_Free(o->xE8);
        ModelInst_Free(o->x110);
        ModelInst_Free(o->x138);
        Res_Release(D_800919F4[idx]);
        if (D_80124360) {
            D_80124360 = 0;
            Res_Release(0x4650);
        }
        Debris_FreeCar(o);
        Res_Release(D_80091A54[idx]);
        Heap_Free(gMainHeap, o->xB3C);
        Heap_Free(gMainHeap, o->xB54);
        if (D_800958B4 == 2 || D_800958B4 == 11) {
            Heap_Free(gMainHeap, o->xB44);
            Heap_Free(gMainHeap, o->xB5C);
            Heap_Free(gMainHeap, o->xB4C);
            Heap_Free(gMainHeap, o->xB64);
        }
    }
    Mem_Free(o->x2C);
    o->x2C = 0;
    Mem_Free(o);
}
