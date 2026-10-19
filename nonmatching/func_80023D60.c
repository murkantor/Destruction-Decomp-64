typedef struct {
    s16 id;
    void (*x4)(void *);
    void (*x8)(void *);
    u8 padC[8];
} Handler;
extern Handler D_80095630[];
extern s32 D_80095810;

void func_80023D60(void *arg, s16 *id) {
    char unused[1];
    s32 i;

    for (i = 0; i < D_80095810; i++) {
        if (D_80095630[i].id == *id) {
            if (D_80095630[i].x4 != NULL) {
                D_80095630[i].x4(arg);
            }
            return;
        }
    }
}
