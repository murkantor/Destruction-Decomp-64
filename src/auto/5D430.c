#include "common.h"
#include "m2c.h"

typedef struct {
    s32 x0;
    s32 x4;
    void *x8;
    s32 xC;
    void *x10;
    s32 x14;
    void *x18;
    void *x1C;
    void *x20;
    void *x24;
    s32 x28;
    s32 x2C;
    s32 x30;
    s32 x34;
    s32 x38;
    s32 x3C;
} AudioCfg;
extern u8 D_8FC5B0[], D_9EF540[], D_BD4BA0[], D_8FE290[], D_9F5680[], D_BDAC70[];
extern u8 D_80108D60[];
extern u8 D_80130250[], D_800D3200[], D_80133D08[], D_801340E8[];
void Rom_Copy(void *, void *, s32);
void Sound_RelocateBank(void *, void *);
void func_80065708(AudioCfg *);

void func_8005C830(void) {
    AudioCfg cfg;

    Rom_Copy(D_8FC5B0, D_80108D60, 0x1D48);
    Rom_Copy(D_9EF540, D_80108D60 + 0x1D48, 0x61A8);
    Rom_Copy(D_BD4BA0, D_80108D60 + 0x7EF0, 0x61A8);
    Sound_RelocateBank(D_80108D60, D_8FE290);
    Sound_RelocateBank(D_80108D60 + 0x1D48, D_9F5680);
    Sound_RelocateBank(D_80108D60 + 0x7EF0, D_BDAC70);
    cfg.x0 = 0;
    cfg.x4 = 0x18;
    cfg.x8 = D_80130250;
    cfg.xC = 0xC;
    cfg.x10 = D_800D3200;
    cfg.x14 = 0x35B60;
    cfg.x18 = D_80108D60;
    cfg.x1C = D_8FE290;
    cfg.x20 = D_80133D08;
    cfg.x24 = D_801340E8;
    cfg.x2C = 0x5622;
    cfg.x28 = 0x100;
    cfg.x30 = 0x1800;
    cfg.x34 = 1;
    cfg.x38 = 0x24;
    cfg.x3C = 0x800;
    func_80065708(&cfg);
}
