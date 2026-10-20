#include "common.h"
#include "m2c.h"

typedef struct { s32 x0; s32 x4; s32 x8; s32 xC; u8 x10; u8 pad11[3]; s32 x14; s32 x18; f32 x1C[7]; u8 x38; u8 pad39[3]; u8 x3C; u8 pad3D[3]; s32 x40; s32 x44; } UiElemDesc; /* the real layout (Ui_AddSpriteElement reads x10, x38 and x3C as bytes): the PC port converts these static descriptors at boot by their declared widths */
typedef struct { s32 x0; s32 x4; s32 pad[9]; } UnkEntry;
extern UiElemDesc D_800BAA80;
extern UiElemDesc D_800BAAC8;
extern UnkEntry D_80096EDC[];
extern void *gUi[];
void Ui_AddSpriteElement(void *, UiElemDesc *);

void CourseIntro_AddTrackSprites(s32 idx) {
    UiElemDesc a = D_800BAA80;
    UiElemDesc b = D_800BAAC8;
    a.x14 = D_80096EDC[idx].x0;
    a.x18 = D_80096EDC[idx].x4;
    Ui_AddSpriteElement(gUi[0], &b);
    Ui_AddSpriteElement(gUi[0], &a);
}
