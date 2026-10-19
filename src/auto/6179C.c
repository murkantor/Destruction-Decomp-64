#include "common.h"
#include "m2c.h"

typedef struct {
    s16 objX;
    u16 scaleW;
    u16 imageW;
    u16 paddingX;
    s16 objY;
    u16 scaleH;
    u16 imageH;
    u16 paddingY;
    u16 imageStride;
    u16 imageAdrs;
    u8 imageFmt;
    u8 imageSiz;
    u8 imagePal;
    u8 imageFlags;
} uObjSprite;
typedef struct {
    u8 pad[4];
    s32 fmt;
    s32 siz;
    s32 width;
    s32 height;
} TexInfo;
typedef struct {
    u8 pad[0x14];
    uObjSprite *sprite;
} SpriteObj;

void Sprite_InitObjSprite(SpriteObj *o, TexInfo *t) {
    uObjSprite *s = o->sprite;
    s32 siz, w;
    s->objX = 0; s->objY = 0; s->scaleW = 0x400; s->scaleH = 0x400; s->paddingX = 0; s->paddingY = 0;
    siz = t->siz; w = t->width;
    s->imageAdrs = 0;
    s->imageStride = w >> (4 - siz);
    s->imageFmt = t->fmt; s->imageSiz = t->siz; s->imageW = t->width << 5; s->imageH = t->height << 5; s->imagePal = 0; s->imageFlags = 0;
}
