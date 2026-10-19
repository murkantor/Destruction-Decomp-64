#include "common.h"
#include "m2c.h"

typedef struct {
    s16 x0;
    u8 pad2[2];
    s32 x4;
    u8 pad8[8];
    u8 x10[0x28];
    void *x38;
} Obj;
extern s32 D_8011F048[];
void func_80025FB0(void *);
void func_80025EB4(void *);
void func_800260D0(void *);
void func_80025984(void *);
void func_8002629C(void *);
void func_80026968(void *);
void func_80026170(void *);
void func_80026034(void *);
void func_8005AAC0(void *);
void func_80063AAC(s32);
void Mem_Free(void *);

void Obj_Free(Obj *o) {
    s32 i;
    s32 *p;

    switch (o->x0) {
        case 32: i = 0; func_80026034(o->x38); break;
        case 6: i = 1; func_80025FB0(o->x38); break;
        case 19: i = 2; func_80025EB4(o->x38); break;
        case 27: i = 3; func_800260D0(o->x38); break;
        case 28: i = 4; func_800260D0(o->x38); break;
        case 14: i = 5; func_80025984(o->x38); break;
        case 15: i = 6; func_8002629C(o->x38); break;
        case 16: i = 7; func_80026968(o->x38); break;
        case 17: i = 8; func_8002629C(o->x38); break;
        case 18: i = 9; func_80026968(o->x38); break;
        case 29: i = 10; func_800260D0(o->x38); break;
        case 30: i = 11; func_80026170(o->x38); break;
        case 33: i = 12; func_80026034(o->x38); break;
        case 23: i = 13; func_80026034(o->x38); break;
        default: return;
    }
    D_8011F048[i]--;
    func_8005AAC0(o->x10);
    if (D_8011F048[i] <= 0) {
        D_8011F048[i] = 0;
        func_80063AAC(o->x4);
    }
    Mem_Free(o);
}
