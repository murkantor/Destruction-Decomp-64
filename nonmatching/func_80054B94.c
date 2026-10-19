typedef struct { u8 pad[0x74]; u32 x74; } Buf;
typedef struct { u8 pad[0x8C]; Buf *x8C; u64 *x90; } Obj;
typedef struct { void *next; OSMesgQueue *mq; } OSScClient;
extern u8 D_80130250[];
extern u32 D_80125C90;
extern s32 D_800B04B0;
void osCreateMesgQueue(OSMesgQueue *, OSMesg *, s32);
s32 osRecvMesg(OSMesgQueue *, OSMesg *, s32);
void osScAddClient(void *, OSScClient *, OSMesgQueue *);
void osScRemoveClient(void *, OSScClient *);
void Pad_Read(void);
void func_80054508(Obj *);
void func_80056098(u64 *);
u8 func_800540C0(void);

void func_80054B94(Obj *obj) {
    OSScClient client;
    OSMesgQueue q;
    OSMesg msgs[2];
    u8 flag = 1;
    u64 *p;
    s32 i;

    osCreateMesgQueue(&q, msgs, 2);
    osScAddClient(D_80130250, &client, &q);
    do {
        Pad_Read();
    } while (D_80125C90 & 0x1000);
    do {
        osRecvMesg(&q, NULL, 1);
        if (flag) {
            p = obj->x90;
            for (i = 0; i < (s32)(obj->x8C->x74 >> 3); i++) {
                p[i] = 0;
            }
            func_80054508(obj);
            func_80056098(obj->x90);
        }
        flag = func_800540C0();
        i = D_80125C90;
        D_800B04B0++;
    } while (!(((u32)i >> 12) & 1));
    osScRemoveClient(D_80130250, &client);
}
