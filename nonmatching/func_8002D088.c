typedef struct {
    u8 pad[0x10];
    s32 unk10;
    u8 pad14[0x20];
    void *unk34;
} Race;
typedef struct {
    u8 pad[0xC];
    s16 mode;
    u8 padE[0x104A];
    u64 time;
    u8 pad1060[0x9C];
    s16 unk10FC;
    u8 pad10FE[0x2];
    Race *race;
} Game;
extern Game *gGame;

void func_80025450(void);

void func_8002D088(u8 *obj) {
    *(s16 *)(obj + 0xC) = gGame->mode;
    switch (gGame->mode) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            func_80025450();
            break;
    }
}
