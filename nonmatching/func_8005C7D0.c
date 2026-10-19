typedef struct {
    u8 pad[0xDC4];
    u8 *saved[60];
    u8 padEB4[0x198];
    s16 numSaved;
} Game;
extern Game *gGame;
typedef struct {
    u8 pad[0xC];
    u8 *items[60];
} ItemList;

s32 func_8005C7D0(ItemList *list) {
    s16 i;

    for (i = gGame->numSaved; i < 60; i++) {
        gGame->saved[i] = list->items[i];
    }
    gGame->numSaved = 60;
    return 0;
}
