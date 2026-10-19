extern char gSaveSlotText[][50];
extern char D_80123C56[];
extern char D_80123CBA[];
extern u32 D_800958B0;
extern s32 D_80095CB8[];
extern s32 D_801252F4;
extern s32 D_80097360;
extern s32 D_80097350;
void Ui_ResetScreen(s32, s32);
s32 sprintf(char *, const char *, ...);

void Menu_SetupRetryPlaceText(void) {
    char *text;
    char *t2;
    s32 place;

    Ui_ResetScreen(0x112F2, 0x112F3);
    text = gSaveSlotText[0];
    sprintf(text, "TRY AGAIN");
    sprintf(text + 50, "QUIT");
    sprintf(text + 100, "YOU HAD TO PLACE");
    place = D_80097360;
    if (D_80095CB8[D_800958B0] - 1 < D_801252F4) {
        sprintf(text + 200, "OR BETTER IN CIRCUIT STANDINGS");
        switch (D_800958B0) {
            case 0:
                place = 3;
                break;
            case 1:
                place = 2;
                break;
            case 2:
                place = 1;
                break;
            case 3:
                place = 1;
                break;
        }
    } else {
        sprintf(text + 200, "OR BETTER IN RACE STANDINGS");
    }
    t2 = D_80123CBA;
    sprintf(t2, "TO ADVANCE");
    switch (place) {
        case 1:
            sprintf(D_80123C56, "1ST");
            break;
        case 2:
            sprintf(D_80123C56, "2ND");
            break;
        case 3:
            sprintf(D_80123C56, "3RD");
            break;
        case 4:
            sprintf(D_80123C56, "4TH");
            break;
        case 5:
            sprintf(D_80123C56, "5TH");
            break;
        case 6:
            sprintf(D_80123C56, "6TH");
            break;
        case 7:
            sprintf(D_80123C56, "7TH");
            break;
        case 8:
        default:
            sprintf(D_80123C56, "8TH");
            break;
    }
    D_80097350 = 0;
}
/* agent45: only diff is original keeps &D_80123CBA (=gSaveSlotText[5]) in s1 and does move a0,s1 for "TO ADVANCE". Tried reusing text, text+=250, gSaveSlotText[5], assignment in arg. */
