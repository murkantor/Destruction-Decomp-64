#include "common.h"
#include "m2c.h"

typedef struct { s32 id; u8 pad[0x40]; } UiElemA;
typedef struct { s32 id; u8 pad[0x8C]; } UiElemB;
typedef struct { s32 id; u8 pad[0x20]; } UiElemC;
typedef struct {
    u8 pad0[0xC];
    UiElemA a[(0x670 - 0xC) / 0x44];
    u8 padA[(0x670 - 0xC) % 0x44];
    UiElemB b[(0x6BB0 - 0x670) / 0x90];
    u8 padB[(0x6BB0 - 0x670) % 0x90];
    UiElemC c[(0x7588 - 0x6BB0) / 0x24];
    u8 padC[0x8B5E - 0x6BB0 - ((0x7588 - 0x6BB0) / 0x24) * 0x24];
    s16 countA;
    s16 countB;
    s16 pad8B62;
    s16 countC;
} Ui;

void *Ui_FindElement(Ui *ui, s32 type, s32 id) {
    void *r = NULL;
    s32 i;

    switch (type) {
        case 1:
            for (i = 0; i < ui->countA; i++) {
                if (id == ui->a[i].id) {
                    r = &ui->a[i];
                    break;
                }
            }
            break;
        case 2:
            for (i = 0; i < ui->countB; i++) {
                if (id == ui->b[i].id) {
                    r = &ui->b[i];
                    break;
                }
            }
            break;
        case 0:
            for (i = 0; i < ui->countC; i++) {
                if (id == ui->c[i].id) {
                    r = &ui->c[i];
                    break;
                }
            }
            break;
    }
    return r;
}
