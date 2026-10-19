#include "common.h"
#include "m2c.h"

typedef struct { u8 pad[0xC]; s16 mode; } Game;
extern Game *gGame[];
void func_80025450(void);
void func_8002D088(u8 *obj) {
    *(s16 *)(obj + 0xC) = gGame[0]->mode;
    switch (gGame[0]->mode) { case 0: break; case 1: break; case 2: func_80025450(); break; case -1: break; }
}
