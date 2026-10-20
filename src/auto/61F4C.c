#include "common.h"
#include "m2c.h"

typedef struct {
    u16 imageX;
    u16 imageW;
    s16 frameX;
    u16 frameW;
    u16 imageY;
    u16 imageH;
    s16 frameY;
    u16 frameH;
    u64 *imagePtr;
    u16 imageLoad;
    u8 imageFmt;
    u8 imageSiz;
    u16 imagePal;
    u16 imageFlip;
    u16 scaleW;
    u16 scaleH;
    s32 imageYorig;
    u8 padding[4];
} uObjBg;
typedef struct {
    u32 type;
    u64 *image;
    u16 phead;
    u16 pnum;
    u16 zero;
    s16 sid;
    u32 flag;
    u32 mask;
} uObjTxtrTLUT;
typedef struct {
    uObjBg bg;
    uObjTxtrTLUT tlut;
    s32 flags;
    s32 x44;
    s32 x48;
    u8 pad4C[4];
    s32 x50;
    s32 x54;
} Background;
typedef struct {
    u64 *image;
    s32 fmt;
    s32 siz;
    s32 width;
    s32 height;
    s32 load;
} BgImage;
typedef struct {
    u64 *data;
    s32 start;
    s32 count;
} Palette;
void guS2DInitBg(uObjBg *);

void Bg_Init(Background *b, BgImage *img, Palette *pal) {
    uObjTxtrTLUT *t;

    b->flags = 0;
    b->x44 = 0;
    b->x48 = 0;
    b->x50 = 0;
    b->x54 = 0;
    b->bg.imageX = 0;
    b->bg.imageY = 0;
    b->bg.imageW = img->width * 4;
    b->bg.imageH = img->height * 4;
    b->bg.frameX = 0;
    b->bg.frameY = 0;
    b->bg.frameW = img->width * 4;
    b->bg.frameH = img->height * 4;
    b->bg.imagePtr = img->image;
    b->bg.imageLoad = img->load;
    b->bg.imageFmt = img->fmt;
    b->bg.imageSiz = img->siz;
    t = &b->tlut;
    b->bg.imagePal = 0;
    b->bg.imageFlip = 0;
    if (b->bg.imageSiz == 3) {
        b->bg.scaleW = 0x400;
        b->bg.scaleH = 0x400;
        b->bg.imageYorig = 0;
    } else {
        guS2DInitBg(&b->bg);
    }
    if (pal != NULL) {
        t->type = 0x30;
        t->image = pal->data;
        t->phead = pal->start + 0x100;
        t->pnum = pal->count - 1;
        t->zero = 0;
        t->sid = 0;
        t->flag = -1;
        t->mask = 0;
        b->flags |= 2;
    } else {
        b->flags &= 0xFFFD;
    }
}
