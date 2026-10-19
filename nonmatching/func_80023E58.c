typedef struct {
    s16 id;
    u8 x2;
    u8 pad3[9];
    void (*xC)(void);
    u8 pad10[4];
} Handler;
typedef struct { u8 pad[0x1074]; u8 x1074; } Game;
extern Game *gGame;
extern Handler D_80095630[];
extern s32 D_80095810;

void func_80023E58(s16 *id) {
    char unused[1];
    s32 i;

    for (i = 0; i < D_80095810; i++) {
        if (D_80095630[i].id == *id) {
            if (D_80095630[i].xC != NULL) {
                if (!gGame->x1074 || D_80095630[i].x2) {
                    D_80095630[i].xC();
                }
            }
            return;
        }
    }
}
