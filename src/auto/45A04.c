#include "common.h"
#include "m2c.h"

extern u8 *gGame[];
extern void *gUi;
extern u8 D_800D2E58;
extern s8 D_800D31CC;
extern s32 D_8009735C;
extern s32 D_8011EEF0;
extern u8 D_800958A1;
extern u32 D_800958B4;
extern s32 D_800958A8;
extern s32 D_800958A4;
void func_800638C8(void *);
void func_80063890(void *);
void func_800638C0(void *);
void func_800627F0(s32, s32, s32);
void func_8005F884(void *);
void func_800100BC(s32, s32);

void func_80044E04(void) {
    s32 i;

    func_800638C8(gGame[0] + 0x10A8);
    func_80063890(gGame[0] + 0x10A8);
    func_800638C0(gGame[0] + 0x10A8);
    D_8009735C = 0;
    if (D_800D2E58 != D_800D31CC) {
        if ((D_800D2E58 = D_800D31CC) == 0) {
            D_800D2E58 = 1;
        }
        func_800627F0(D_8011EEF0, D_800D2E58, D_800958A1);
    }
    func_8005F884(gUi);
    if (D_800958B4 >= 4 && D_800958A8 != 2) {
        for (i = 0; i < D_800958A4; i++) {
            func_800100BC(i + 0x200, 0x111D6);
        }
    }
}
